// Arasan network trainer, version 10: version 9 architecture plus threat inputs.
//
// NOTE: this script is written for the "bullet_trainer" model API (ModelDefinition,
// ModelInputs, ...) used by bullet's examples/advanced/main.rs, which it follows closely.
//
// Inputs: two sparse inputs per perspective,
//   - "psqt": the king-bucketed, horizontally mirrored piece-square inputs (768 x 9)
//   - "pp":   pawn pair inputs (4,560). One input for each pair of distinct pawns of either
//             color that are on the same or adjacent files (any ranks). A pawn's id is its
//             square, relative to the perspective and mirrored if the perspective's king is on
//             files e-h, minus 8 (so 0..47), plus 48 for the opponent's pawns. The pair index is
//             hi*(hi-1)/2 + lo for the two ids. This must match the engine's
//             PawnPairFeature (src/nnue/features/ppfeature.h).
//   - "th":   threat inputs (59,808). One input for each (attacker piece, from, to, victim piece)
//             direct attack (no x-rays), kings are never attackers or victims. Must match the
//             engine's ThreatFeature (src/nnue/features/threatfeature.h/.cpp) exactly: see
//             ThreatInputs below. They use the same orientation as the pawn pairs and PSQ
//             (rank flip for the perspective, horizontal mirror by king file) but do not depend
//             on the king bucket. They have no bias of their own.
// The pair and threat weights are separate from the piece-square weights so they can be clipped
// to the int8 range the engine stores them in, while the piece-square weights are int16.
//
// Architecture:
//   feature transformer (psqt + pp -> L1, per perspective)       [psqt, bias int16; pp int8]
//     -> CReLU[0,1] + pairwise-mul (per perspective)             -> u8, width L1
//     -> L1 head:  L1 -> L2_NEURONS                              [int8 weights, int32 bias]
//        (sparse u8 x int8 affine via dpbusd in the engine;
//         engine CReLU-clamps the int32 output to L1_OUT_CLAMP = QP*QB)
//     -> L2 head:  L2_NEURONS -> L3_NEURONS                      [int32 weights/bias, no shift]
//        (engine CReLU-clamps to L2_OUT_CLAMP = QP*QB*QC)
//     -> output:   L3_NEURONS -> 1                               [int32 weights, int64 accum]
// The head uses no intermediate dequantizing shift; a single divide by
// OUTPUT_DIVISOR = QP*QB*QC*QC in the engine recovers the float score.
//
// Quantization scales (must match the engine's nnparams.h):
//   QA = 255 (feature transformer activation, integer scale of float 1.0; also the scale of the
//             piece-square and pair weights and of the feature transformer bias)
//   QP = 127 (pairwise-mul u8 activation scale = (QA*QA) >> (16-7), the engine
//             clamps to QA and right-shifts by 16-PAIRWISE_SHIFT, PAIRWISE_SHIFT=7)
//   QB = 64  (int8 L1 head weights)
//   QC = 64  (int32 L2/output head weights)
// Bias quantise factors are the product of input scale and weight scale at
// each layer (no intermediate shift):
//   l1b: QP*QB        = 127*64       = 8128
//   l2b: QP*QB*QC     = 8128*64      = 520192
//   l3b: QP*QB*QC*QC  = 520192*64    = 33292288   (== engine OUTPUT_DIVISOR)
//
use bullet_lib::{
    game::{
        formats::{
            bulletformat::ChessBoard,
            montyformat::chess::{Piece, Side},
        },
        inputs::{get_num_buckets, ChessBucketsMirrored, SparseInputType},
        outputs::{MaterialCount, OutputBuckets},
    },
    trainer::schedule::{
        lr::{self, LrScheduler},
        wdl,
    },
    value::{loader::DirectSequentialDataLoader, save::save_to_checkpoint},
    wdl::WdlScheduler,
};
use bullet_trainer::{
    model::{
        DenseInput, InitSettings, ModelDefinition, ModelEvaluator, ModelInputs, ModelInputsMapper,
        ModelWeights, SavedFormat, SparseInput,
    },
    optimiser::{
        adam::{AdamW, AdamWParams},
        Optimiser,
    },
    reader::ReadMapLoader,
    run::{train, DefaultDevice, HostPool, TrainingSchedule, TrainingSteps},
};

const NET_NAME: &str = "arasan10";
const NETWORK_VERSION: u8 = 10;
const MAP_THREADS: u8 = 8;
const SAVE_RATE: usize = 200;
const DATA_PATH: &str = "/data3/bullet/apr-may2026/dec25-may26-shuffled-interleaved.bullet";
// "/data3/bullet/aug2026/dec25-aug26-shuffled-interleaved.bullet"

const NUM_OUTPUT_BUCKETS: usize = 8;
const L1: usize = 1536; // feature transformer width (per perspective)
const L2_NEURONS: usize = 16; // sparse L1 head output width (engine L1_SIZE)
const L3_NEURONS: usize = 32; // second head layer width (engine L2_SIZE)

const QA: i16 = 255;
const QB: i16 = 64;
const QC: i32 = 64;
// Pairwise-mul u8 activation scale (see above). Used for the head bias scales.
const QP: i32 = (255 * 255) >> (16 - 7); // 127

// pair and threat weights are stored as int8 at scale QA, so they must be within this range
const PP_RANGE: f32 = i8::MAX as f32 / (QA as f32);
const TH_RANGE: f32 = PP_RANGE;

const SUPERBATCHES: usize = 600;
const BATCH_SIZE: usize = 16_384;
const BATCHES_PER_SUPERBATCH: usize = 6104;

// Three-stage schedule (LR and WDL), after bullet's examples/advanced/main.rs.
// The stage lengths must sum to SUPERBATCHES.
const STAGE0: usize = 100;
const STAGE1: usize = 400;
const STAGE2: usize = 100;
const WARMUP_SBS: usize = STAGE0 / 2;
const WARMUP_MAX_LR: f32 = 2e-3;
const WARMUP_MIN_LR: f32 = 1e-4;
const _: () = assert!(STAGE0 + STAGE1 + STAGE2 == SUPERBATCHES);

#[rustfmt::skip]
const BUCKET_LAYOUT: [usize; 32] = [
    0, 1, 2, 3,
    4, 4, 5, 5,
    6, 6, 7, 7,
    6, 6, 7, 7,
    8, 8, 8, 8,
    8, 8, 8, 8,
    8, 8, 8, 8,
    8, 8, 8, 8,
];
const INPUT_BUCKETS: usize = get_num_buckets(&BUCKET_LAYOUT);

type InputTy = (((((((SparseInput, SparseInput), SparseInput), SparseInput), SparseInput), SparseInput), SparseInput), DenseInput<f32>);

// Squares on the same or adjacent files as each pawn square (ranks 2-7), including the pawn's
// own square. Pawns cannot be on the first or last rank.
fn three_file_band_mask() -> [u64; 64] {
    const A: u64 = 0x0101_0101_0101_0101;
    let mut masks = [0; 64];
    for (sq, mask) in masks.iter_mut().enumerate().take(56).skip(8) {
        let f = sq & 7;
        let mut m: u64 = A << f;
        if f > 0 {
            m |= A << (f - 1);
        }
        if f < 7 {
            m |= A << (f + 1);
        }
        *mask = m;
    }
    masks
}

#[derive(Clone)]
struct PawnPairInputs {
    masks: [u64; 64],
}

impl PawnPairInputs {
    // 96 pawn ids: 48 squares (ranks 2-7) for each of our pawns and the opponent's
    const TOTAL_PAIRS: usize = 96 * 95 / 2;
    // at most 16 pawns in total
    const MAX_PAIRS: usize = 16 * 15 / 2;

    fn new() -> Self {
        Self { masks: three_file_band_mask() }
    }

    fn num_inputs(&self) -> usize {
        Self::TOTAL_PAIRS
    }

    fn max_active(&self) -> usize {
        Self::MAX_PAIRS
    }

    fn pawn_id(colour: usize, sq: usize) -> usize {
        colour * 48 + sq - 8
    }

    fn pair_index(id_a: usize, id_b: usize) -> usize {
        let lo = id_a.min(id_b);
        let hi = id_a.max(id_b);
        hi * (hi - 1) / 2 + lo
    }

    // pairs of pawns of the same color. "outer" pawns are removed as they are processed, so
    // each pair is counted once
    fn emit_same_colour(&self, bb: u64, colour: usize, f: &mut impl FnMut(usize)) {
        let mut outer = bb;
        while outer != 0 {
            let sq_a = outer.trailing_zeros() as usize;
            outer &= outer - 1;
            let id_a = Self::pawn_id(colour, sq_a);
            map_bb(outer & self.masks[sq_a], |sq_b| f(Self::pair_index(id_a, Self::pawn_id(colour, sq_b))));
        }
    }

    // bbs are from the perspective whose features are wanted: color 0 is that side
    fn collect_pairs(&self, bbs: [u64; 8], f: &mut impl FnMut(usize)) {
        let friendly = bbs[Side::WHITE] & bbs[Piece::PAWN];
        let enemy = bbs[Side::BLACK] & bbs[Piece::PAWN];

        self.emit_same_colour(friendly, 0, f);

        map_bb(friendly, |sq_a| {
            let id_a = Self::pawn_id(0, sq_a);
            map_bb(enemy & self.masks[sq_a], |sq_b| f(Self::pair_index(id_a, Self::pawn_id(1, sq_b))));
        });

        self.emit_same_colour(enemy, 1, f);
    }

    fn map_features(&self, pos: &ChessBoard, mut on_stm: impl FnMut(usize), mut on_ntm: impl FnMut(usize)) {
        let bbs = build_bbs(pos);
        self.collect_pairs(normalize_hm(bbs), &mut on_stm);
        self.collect_pairs(normalize_hm(flip_view(bbs)), &mut on_ntm);
    }
}

// Threat inputs. Index layout (identical to src/nnue/features/threatfeature.cpp): one block per
// attacker piece (own pieces first, then the opponent's); a block has one slot per allowed victim
// (color, type); a slot has one entry per (from, to) attack the attacker can make on an empty
// board:
//    index = block_start[attacker] + slot(victim) * attacks_per_slot[type] + lut2[type][from][to]
// where lut2 is the offset of `from`'s attack squares plus the rank of `to` among them.
// Same-type mutual attacks (enemy same-type pairs, and own same-type pairs other than pawns) are
// kept only in the from > to direction (after orientation). Pawn attackers: victims knight and
// rook only. Knight and queen: P N B R Q. Bishop and rook: P N B R.
#[derive(Clone)]
struct ThreatInputs {
    // [attacker color][attacker type][victim color][victim type] -> base index, or NONE
    // (indexed by `from < to` as last dimension)
    lut1: [[[[[u32; 2]; 6]; 2]; 6]; 2],
    // [attacker type][from][to]
    lut2: Vec<[[u16; 64]; 64]>,
    total: usize,
}

impl ThreatInputs {
    const NONE: u32 = u32::MAX;
    // pawn, knight, bishop, rook, queen, king; -1 = not allowed
    #[rustfmt::skip]
    const VICTIM_MAP: [[i32; 6]; 6] = [
        [-1, 0, -1, 1, -1, -1],
        [ 0, 1,  2, 3,  4, -1],
        [ 0, 1,  2, 3, -1, -1],
        [ 0, 1,  2, 3, -1, -1],
        [ 0, 1,  2, 3,  4, -1],
        [-1, -1, -1, -1, -1, -1],
    ];
    const TOTAL: usize = 59808;
    // at most 30 non-king pieces, each attacking at most 8 non-king pieces (engine
    // MAX_THREAT_INDICES is this plus one)
    const MAX_ACTIVE: usize = 30 * 8;

    fn bit_for(rank: i32, file: i32) -> u64 {
        if (0..8).contains(&rank) && (0..8).contains(&file) {
            1u64 << (rank * 8 + file)
        } else {
            0
        }
    }

    // attacks on an empty board. `down` matters only for pawns (false = toward higher ranks)
    fn empty_board_attacks(typ: usize, sq: usize, down: bool) -> u64 {
        let r = (sq / 8) as i32;
        let f = (sq % 8) as i32;
        let mut result = 0u64;
        match typ {
            0 => {
                let dr = if down { -1 } else { 1 };
                result = Self::bit_for(r + dr, f - 1) | Self::bit_for(r + dr, f + 1);
            }
            1 => {
                const DR: [i32; 8] = [1, 2, 2, 1, -1, -2, -2, -1];
                const DF: [i32; 8] = [2, 1, -1, -2, -2, -1, 1, 2];
                for i in 0..8 {
                    result |= Self::bit_for(r + DR[i], f + DF[i]);
                }
            }
            5 => {
                for dr in -1..=1 {
                    for df in -1..=1 {
                        if dr != 0 || df != 0 {
                            result |= Self::bit_for(r + dr, f + df);
                        }
                    }
                }
            }
            _ => {
                for dr in -1..=1 {
                    for df in -1..=1 {
                        if dr == 0 && df == 0 {
                            continue;
                        }
                        let diagonal = dr != 0 && df != 0;
                        if (typ == 2 && !diagonal) || (typ == 3 && diagonal) {
                            continue;
                        }
                        let (mut rr, mut ff) = (r + dr, f + df);
                        while (0..8).contains(&rr) && (0..8).contains(&ff) {
                            result |= Self::bit_for(rr, ff);
                            rr += dr;
                            ff += df;
                        }
                    }
                }
            }
        }
        result
    }

    // attacks of a piece on the (oriented) board with the given occupancy. Color 0 is the side
    // whose perspective is wanted, whose pawns move toward higher ranks.
    fn piece_attacks(typ: usize, colour: usize, sq: usize, occ: u64) -> u64 {
        match typ {
            0 => Self::empty_board_attacks(0, sq, colour == 1),
            1 | 5 => Self::empty_board_attacks(typ, sq, false),
            _ => {
                let r = (sq / 8) as i32;
                let f = (sq % 8) as i32;
                let mut result = 0u64;
                for dr in -1..=1 {
                    for df in -1..=1 {
                        if dr == 0 && df == 0 {
                            continue;
                        }
                        let diagonal = dr != 0 && df != 0;
                        if (typ == 2 && !diagonal) || (typ == 3 && diagonal) {
                            continue;
                        }
                        let (mut rr, mut ff) = (r + dr, f + df);
                        while (0..8).contains(&rr) && (0..8).contains(&ff) {
                            let b = 1u64 << (rr * 8 + ff);
                            result |= b;
                            if occ & b != 0 {
                                break;
                            }
                            rr += dr;
                            ff += df;
                        }
                    }
                }
                result
            }
        }
    }

    fn new() -> Self {
        let mut num_targets = [0usize; 6];
        for a in 0..6 {
            for v in 0..6 {
                if Self::VICTIM_MAP[a][v] >= 0 {
                    num_targets[a] += 2;
                }
            }
        }
        let mut lut2 = vec![[[0u16; 64]; 64]; 6];
        let mut attacks_per_slot = [0usize; 6];
        for typ in 0..6 {
            let mut base = 0usize;
            for from in 0..64 {
                // a pawn can't be on the first or last rank
                let pawn_end = typ == 0 && !(8..56).contains(&from);
                let up = Self::empty_board_attacks(typ, from, false);
                let down = if typ == 0 { Self::empty_board_attacks(typ, from, true) } else { up };
                let (mut rank_up, mut rank_down) = (0usize, 0usize);
                for to in 0..64 {
                    // for pawns, attacks toward lower squares are used when to < from
                    let rank = if typ == 0 && to < from { rank_down } else { rank_up };
                    lut2[typ][from][to] = (base + rank) as u16;
                    if up >> to & 1 != 0 {
                        rank_up += 1;
                    }
                    if down >> to & 1 != 0 {
                        rank_down += 1;
                    }
                }
                if !pawn_end {
                    base += up.count_ones() as usize;
                }
            }
            attacks_per_slot[typ] = base;
        }
        let mut block_start = [[0usize; 6]; 2];
        let mut total = 0usize;
        for colour in 0..2 {
            for typ in 0..6 {
                block_start[colour][typ] = total;
                total += num_targets[typ] * attacks_per_slot[typ];
            }
        }
        assert_eq!(total, Self::TOTAL, "threat feature dimensions mismatch");
        let mut lut1 = [[[[[Self::NONE; 2]; 6]; 2]; 6]; 2];
        for ac in 0..2 {
            for at in 0..6 {
                for vc in 0..2 {
                    for vt in 0..6 {
                        let map = Self::VICTIM_MAP[at][vt];
                        if map < 0 {
                            continue;
                        }
                        let enemy = ac != vc;
                        // mutual same-type attacks are kept in one direction only
                        let semi_excluded = at == vt && (enemy || at != 0);
                        let feature =
                            (block_start[ac][at] + (vc * (num_targets[at] / 2) + map as usize) * attacks_per_slot[at]) as u32;
                        // [0] is used when from > to, [1] when from < to
                        lut1[ac][at][vc][vt][0] = feature;
                        lut1[ac][at][vc][vt][1] = if semi_excluded { Self::NONE } else { feature };
                    }
                }
            }
        }
        Self { lut1, lut2, total }
    }

    fn num_inputs(&self) -> usize {
        self.total
    }

    fn max_active(&self) -> usize {
        Self::MAX_ACTIVE
    }

    // bbs are oriented from the perspective whose features are wanted (see normalize_hm and
    // flip_view): color 0 is that side. Attacks are computed on the oriented board, which gives
    // the same indices as orienting the squares after computing attacks on the real board.
    fn collect_threats(&self, bbs: [u64; 8], f: &mut impl FnMut(usize)) {
        let occ = bbs[Side::WHITE] | bbs[Side::BLACK];
        for ac in 0..2 {
            let side_bb = bbs[ac];
            for at in 0..5 {
                map_bb(side_bb & bbs[2 + at], |from| {
                    let targets = Self::piece_attacks(at, ac, from, occ);
                    for vc in 0..2 {
                        for vt in 0..5 {
                            map_bb(targets & bbs[vc] & bbs[2 + vt], |to| {
                                let base = self.lut1[ac][at][vc][vt][usize::from(from < to)];
                                if base != Self::NONE {
                                    f(base as usize + usize::from(self.lut2[at][from][to]));
                                }
                            });
                        }
                    }
                });
            }
        }
    }

    fn map_features(&self, pos: &ChessBoard, mut on_stm: impl FnMut(usize), mut on_ntm: impl FnMut(usize)) {
        let bbs = build_bbs(pos);
        self.collect_threats(normalize_hm(bbs), &mut on_stm);
        self.collect_threats(normalize_hm(flip_view(bbs)), &mut on_ntm);
    }
}

fn map_bb(mut bb: u64, mut f: impl FnMut(usize)) {
    while bb > 0 {
        let sq = bb.trailing_zeros() as usize;
        f(sq);
        bb &= bb - 1;
    }
}

// bitboards: [white, black, pawns, knights, bishops, rooks, queens, kings]. Positions in
// bullet's format are always from the point of view of the side to move, which is "white" here.
fn build_bbs(pos: &ChessBoard) -> [u64; 8] {
    let mut bbs = [0u64; 8];
    for (pc, sq) in pos.into_iter() {
        let bit = 1 << sq;
        bbs[usize::from(pc & 8 > 0)] |= bit;
        bbs[2 + usize::from(pc & 7)] |= bit;
    }
    bbs
}

// the position from the other side's point of view
fn flip_view(mut bbs: [u64; 8]) -> [u64; 8] {
    bbs.swap(Side::WHITE, Side::BLACK);
    for bb in bbs.iter_mut() {
        *bb = bb.swap_bytes();
    }
    bbs
}

// mirror horizontally if the king of the side whose features are wanted is on files e-h
fn normalize_hm(mut bbs: [u64; 8]) -> [u64; 8] {
    let ksq = (bbs[Side::WHITE] & bbs[Piece::KING]).trailing_zeros();
    if ksq % 8 > 3 {
        for bb in bbs.iter_mut() {
            *bb = bb.swap_bytes().reverse_bits();
        }
    }
    bbs
}

// Print the pair indices for each FEN (arguments, or lines on standard input if there are
// none), in the format of network/engine_pair_indices.py: the FEN, then the sorted indices for
// White and for Black. Positions are from the side to move's point of view, so "stm" is the
// side to move.
fn dump_pairs(args: &[String]) {
    let pp = PawnPairInputs::new();
    let fens: Vec<String> = if args.is_empty() {
        std::io::stdin().lines().map(|line| line.unwrap().trim().to_string()).filter(|line| !line.is_empty()).collect()
    } else {
        args.to_vec()
    };
    for fen in fens {
        let pos: ChessBoard = format!("{fen} | 0 | 0.0").parse().unwrap();
        let mut stm = Vec::new();
        let mut ntm = Vec::new();
        pp.map_features(&pos, |idx| stm.push(idx), |idx| ntm.push(idx));
        stm.sort();
        ntm.sort();
        let black_to_move = fen.split_whitespace().nth(1) == Some("b");
        let (white, black) = if black_to_move { (ntm, stm) } else { (stm, ntm) };
        let join = |list: &Vec<usize>| list.iter().map(|idx| idx.to_string()).collect::<Vec<_>>().join(" ");
        println!("{fen}");
        println!("white {}", join(&white));
        println!("black {}", join(&black));
    }
}

// Print the threat indices for each FEN (arguments, or lines on standard input if there are
// none; lines starting with '#' are skipped), in the format of network/engine_threat_indices.py:
// the FEN, then the sorted indices for White and for Black.
fn dump_threats(args: &[String]) {
    let th = ThreatInputs::new();
    let fens: Vec<String> = if args.is_empty() {
        std::io::stdin()
            .lines()
            .map(|line| line.unwrap().trim().to_string())
            .filter(|line| !line.is_empty() && !line.starts_with('#'))
            .collect()
    } else {
        args.to_vec()
    };
    for fen in fens {
        // bullet's parser wants the move counters
        let full = if fen.split_whitespace().count() < 6 { format!("{fen} 0 1") } else { fen.clone() };
        let pos: ChessBoard = format!("{full} | 0 | 0.0").parse().unwrap();
        let mut stm = Vec::new();
        let mut ntm = Vec::new();
        th.map_features(&pos, |idx| stm.push(idx), |idx| ntm.push(idx));
        stm.sort();
        ntm.sort();
        let black_to_move = fen.split_whitespace().nth(1) == Some("b");
        let (white, black) = if black_to_move { (ntm, stm) } else { (stm, ntm) };
        let join = |list: &Vec<usize>| list.iter().map(|idx| idx.to_string()).collect::<Vec<_>>().join(" ");
        println!("{fen}");
        println!("white {}", join(&white));
        println!("black {}", join(&black));
    }
}

fn make_inputs_mapper(
    params: (&ModelInputs<InputTy>, &PawnPairInputs, &ThreatInputs, ChessBucketsMirrored, impl OutputBuckets<ChessBoard>),
    wdl: impl WdlScheduler,
) -> ModelInputsMapper<ChessBoard> {
    let pp = params.1.clone();
    let th = params.2.clone();

    ModelInputsMapper::build(
        params.0,
        move |pos, step, (((((((stm_pp, ntm_pp), stm_th), ntm_th), stm_psqt), ntm_psqt), bucket), target)| {
            let mut cnt = 0;
            params.3.map_features(pos, |stm, ntm| {
                stm_psqt[cnt] = stm.try_into().unwrap();
                ntm_psqt[cnt] = ntm.try_into().unwrap();
                cnt += 1;
            });

            if cnt < params.3.max_active() {
                stm_psqt[cnt] = -1;
                ntm_psqt[cnt] = -1;
            }

            let mut stm_cnt = 0;
            let mut ntm_cnt = 0;
            pp.map_features(
                pos,
                |stm| {
                    stm_pp[stm_cnt] = stm.try_into().unwrap();
                    stm_cnt += 1;
                },
                |ntm| {
                    ntm_pp[ntm_cnt] = ntm.try_into().unwrap();
                    ntm_cnt += 1;
                },
            );

            assert_eq!(stm_cnt, ntm_cnt);

            if stm_cnt < pp.max_active() {
                stm_pp[stm_cnt] = -1;
                ntm_pp[stm_cnt] = -1;
            }

            // the number of active threats differs between the perspectives (same-type
            // attacks are kept in one direction only after orientation)
            let mut stm_th_cnt = 0;
            let mut ntm_th_cnt = 0;
            th.map_features(
                pos,
                |stm| {
                    stm_th[stm_th_cnt] = stm.try_into().unwrap();
                    stm_th_cnt += 1;
                },
                |ntm| {
                    ntm_th[ntm_th_cnt] = ntm.try_into().unwrap();
                    ntm_th_cnt += 1;
                },
            );

            if stm_th_cnt < th.max_active() {
                stm_th[stm_th_cnt] = -1;
            }
            if ntm_th_cnt < th.max_active() {
                ntm_th[ntm_th_cnt] = -1;
            }

            bucket[0] = i32::from(params.4.bucket(pos));

            let result = f32::from(pos.result) / 2.0;
            let score = 1.0 / (1.0 + (f32::from(-pos.score) / 400.0).exp());
            let lambda = wdl.blend(step.batch(), step.superbatch(), step.final_superbatch());
            assert!((0.0..=1.0).contains(&lambda), "WDL lambda must be in [0, 1]");
            target[0] = lambda * result + (1. - lambda) * score;
        },
    )
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.get(1).map(String::as_str) == Some("--dump-pairs") {
        dump_pairs(&args[2..]);
        return;
    }
    if args.get(1).map(String::as_str) == Some("--dump-threats") {
        dump_threats(&args[2..]);
        return;
    }

    let pp = PawnPairInputs::new();
    let th = ThreatInputs::new();
    let psqt = ChessBucketsMirrored::new(BUCKET_LAYOUT);
    let output_buckets = MaterialCount::<NUM_OUTPUT_BUCKETS>;

    let inputs = ModelInputs::default()
        .add_sparse("stm/pp", (pp.num_inputs(), 1), pp.max_active())
        .add_sparse("ntm/pp", (pp.num_inputs(), 1), pp.max_active())
        .add_sparse("stm/th", (th.num_inputs(), 1), th.max_active())
        .add_sparse("ntm/th", (th.num_inputs(), 1), th.max_active())
        .add_sparse("stm/psqt", (psqt.num_inputs(), 1), psqt.max_active())
        .add_sparse("ntm/psqt", (psqt.num_inputs(), 1), psqt.max_active())
        .add_sparse("buckets", (NUM_OUTPUT_BUCKETS, 1), 1)
        .add_dense("targets", (1, 1));

    let defn = ModelDefinition::build(
        &inputs,
        |builder, (((((((stm_pp, ntm_pp), stm_th), ntm_th), stm_psqt), ntm_psqt), output_buckets), target)| {
            // pair weights and the feature transformer bias (the pair affine's bias is the
            // only bias of the feature transformer)
            let l0_pp = builder.new_affine("l0/pp/", pp.num_inputs(), L1);

            // threat weights: no bias of their own (the FT bias is the pair affine's)
            let l0_th = builder.new_weights("l0/th", (L1, th.num_inputs()), InitSettings::Zeroed);

            // piece-square weights, with an input layer factoriser
            let l0f = builder.new_weights("l0/fac", (L1, 768), InitSettings::Zeroed);
            let psqt_init = InitSettings::Normal { mean: 0.0, stdev: (2f32 / 32.0).sqrt() };
            let mut l0_psqt = builder.new_weights("l0/psqt", (L1, psqt.num_inputs()), psqt_init);
            l0_psqt = l0_psqt + l0f.repeat(psqt.num_inputs() / 768);

            // bucketed head: each affine produces (neurons * buckets) outputs;
            // select() picks the active bucket's contiguous slice. After the
            // per-perspective pairwise-mul the L1 input width is L1 (each
            // perspective's L1 outputs are halved, two perspectives -> L1).
            let l1 = builder.new_affine("l1/", L1, NUM_OUTPUT_BUCKETS * L2_NEURONS);
            let l2 = builder.new_affine("l2/", L2_NEURONS, NUM_OUTPUT_BUCKETS * L3_NEURONS);
            let l3 = builder.new_affine("l3/", L3_NEURONS, NUM_OUTPUT_BUCKETS);

            // inference: feature transformer -> CReLU[0,1] + pairwise-mul (per
            // perspective), matching the engine's PairwiseMult<int16,u8> FT
            // activation. The pairwise product is the only nonlinearity feeding
            // the head; subsequent layers use plain clipped ReLU [0,1].
            let ft = |pp, thr, psqt, start, end| {
                (l0_pp.slice(start, end).forward(pp)
                    + l0_th.slice_rows(start, end).matmul(thr)
                    + l0_psqt.slice_rows(start, end).matmul(psqt))
                .crelu()
            };
            let stm_hidden = ft(stm_pp, stm_th, stm_psqt, 0, L1 / 2) * ft(stm_pp, stm_th, stm_psqt, L1 / 2, L1);
            let ntm_hidden = ft(ntm_pp, ntm_th, ntm_psqt, 0, L1 / 2) * ft(ntm_pp, ntm_th, ntm_psqt, L1 / 2, L1);
            let hidden_layer = stm_hidden.concat(ntm_hidden);

            let l1_out = l1.forward(hidden_layer).select(output_buckets).crelu();
            let l2_out = l2.forward(l1_out).select(output_buckets).crelu();
            let l3_out = l3.forward(l2_out).select(output_buckets);

            let loss = l3_out.sigmoid().squared_error(target);

            (Some(loss.reduce_sum_batch()), vec![("output".to_string(), l3_out)])
        },
    );

    let weights = ModelWeights::new(&defn, 12412421);
    let device = DefaultDevice::new(0).unwrap();
    let params = AdamWParams::default();

    let mut evaluator = ModelEvaluator::new(&defn, device.clone()).unwrap();
    let mut optimiser = Optimiser::<_, AdamW<_>>::new(defn, weights, device.clone(), params).unwrap();

    // piece-square weights (int16 in the engine)
    let psqt_clip = AdamWParams { max_weight: 0.99, min_weight: -0.99, ..Default::default() };
    optimiser.set_params_for_weight("l0/fac", psqt_clip);
    optimiser.set_params_for_weight("l0/psqt", psqt_clip);
    // pair weights must fit in an int8 at scale QA
    let pp_clip = AdamWParams { max_weight: PP_RANGE, min_weight: -PP_RANGE, ..Default::default() };
    optimiser.set_params_for_weight("l0/pp/w", pp_clip);
    // threat weights must fit in an int8 at scale QA too
    let th_clip = AdamWParams { max_weight: TH_RANGE, min_weight: -TH_RANGE, ..Default::default() };
    optimiser.set_params_for_weight("l0/th", th_clip);

    let saved_format = vec![
        // Arasan header
        SavedFormat::custom([b'A', b'R', b'A', NETWORK_VERSION]),
        // piece-square weights: merge in the factoriser weights
        SavedFormat::id("l0/psqt")
            .transform(|weights, values| {
                let fac = weights.get("l0/fac").values.f32().repeat(INPUT_BUCKETS);
                assert_eq!(values.len(), fac.len());
                values.iter().zip(fac).map(|(&a, b)| a + b).collect()
            })
            .round()
            .quantise::<i16>(QA),
        // feature transformer biases, then the pair weights
        SavedFormat::id("l0/pp/b").round().quantise::<i16>(QA),
        SavedFormat::id("l0/pp/w").round().quantise::<i8>(QA),
        // threat weights (int8, [inputs][L1] like the pair weights)
        SavedFormat::id("l0/th").round().quantise::<i8>(QA),
        // L1 head (sparse int8). .transpose() puts weights in bucket-major
        // (for bucket: for output: for input) order to match the engine's
        // SparseLinear/LinearLayer non-transpose reader. Biases are int32 at
        // scale QP*QB (the dpbusd product scale, no intermediate shift).
        SavedFormat::id("l1/w").transpose().round().quantise::<i8>(QB),
        SavedFormat::id("l1/b").round().quantise::<i32>(QP * i32::from(QB)),
        // L2 and output heads (int32, Raphael scheme). Weights quantise to QC;
        // there is no intermediate dequantizing shift, so each bias is at the
        // product scale of its layer: l2b at QP*QB*QC, l3b at QP*QB*QC*QC
        // (the latter equals the engine's OUTPUT_DIVISOR).
        SavedFormat::id("l2/w").transpose().round().quantise::<i32>(QC),
        SavedFormat::id("l2/b").round().quantise::<i32>(QP * i32::from(QB) * QC),
        SavedFormat::id("l3/w").transpose().round().quantise::<i32>(QC),
        SavedFormat::id("l3/b").round().quantise::<i32>(QP * i32::from(QB) * QC * QC),
    ];

    let reader = DirectSequentialDataLoader::new(&[DATA_PATH]);

    let wdl_scheduler = wdl::Sequence {
        first: wdl::ConstantWDL { value: 0.2 },
        second: wdl::Sequence {
            first: wdl::LinearWDL { start: 0.2, end: 0.5 },
            second: wdl::ConstantWDL { value: 1.0 },
            first_scheduler_final_superbatch: STAGE1,
        },
        first_scheduler_final_superbatch: STAGE0,
    };

    let lr_schedule = lr::Sequence {
        first: lr::Sequence {
            first: lr::LinearDecayLR { initial_lr: WARMUP_MIN_LR, final_lr: WARMUP_MAX_LR, final_superbatch: WARMUP_SBS },
            second: lr::LinearDecayLR { initial_lr: WARMUP_MAX_LR, final_lr: WARMUP_MIN_LR, final_superbatch: STAGE0 - WARMUP_SBS },
            first_scheduler_final_superbatch: WARMUP_SBS,
        },
        second: lr::Sequence {
            first: lr::LinearDecayLR { initial_lr: 1e-3, final_lr: 1e-6, final_superbatch: STAGE1 },
            second: lr::LinearDecayLR { initial_lr: 1e-5, final_lr: 1e-7, final_superbatch: STAGE2 },
            first_scheduler_final_superbatch: STAGE1,
        },
        first_scheduler_final_superbatch: STAGE0,
    };

    let train_params = (&inputs, &pp, &th, psqt, output_buckets);
    let mapper = make_inputs_mapper(train_params, wdl_scheduler);

    train(
        &mut optimiser,
        TrainingSchedule {
            steps: TrainingSteps {
                batch_size: BATCH_SIZE,
                batches_per_superbatch: BATCHES_PER_SUPERBATCH,
                start_superbatch: 1,
                end_superbatch: SUPERBATCHES,
            },
            lr_schedule: lr_schedule.boxed(),
            log_rate: 128,
        },
        ReadMapLoader::new(reader, mapper, MAP_THREADS),
        |_, _, _| {},
        |optimiser, step| {
            let superbatch = step.superbatch();
            if superbatch % SAVE_RATE == 0 || superbatch == step.final_superbatch() {
                let name = format!("{NET_NAME}-{superbatch}");
                save_to_checkpoint(optimiser, &saved_format, &format!("checkpoints/{name}"));
                println!("Saved [{name}]");
            }
        },
    )
    .unwrap();

    evaluator.load_device_weights(optimiser.weights()).unwrap();
    let evaluator_mapper = make_inputs_mapper(train_params, wdl::ConstantWDL { value: 0.0 });
    let pool = HostPool::new(device.clone());

    for fen in [
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    ] {
        let pos = format!("{fen} | 0 | 0.0").parse().unwrap();
        let inputs = evaluator_mapper.map(&pool, &[pos], Default::default(), 1).unwrap().to_device(&device).unwrap();
        let output = evaluator.evaluate(&inputs).unwrap().get("output").unwrap();
        let [value] = output.to_host().unwrap().f32()[..] else { panic!() };
        println!("FEN: {fen}");
        println!("EVAL: {}", 400.0 * value);
    }
}
