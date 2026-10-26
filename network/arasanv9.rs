// Arasan network trainer, version 9: version 8 architecture plus pawn pair inputs.
//
// NOTE: this script is written for the "bullet_trainer" model API (ModelDefinition,
// ModelInputs, ...) used by bullet's examples/advanced/main.rs, which it follows closely. It
// has not been compiled or run: the pair input code and the saved format need to be checked
// against the engine (see the index check described below) before a long training run.
//
// Inputs: two sparse inputs per perspective,
//   - "psqt": the king-bucketed, horizontally mirrored piece-square inputs (768 x 9)
//   - "pp":   pawn pair inputs (4,560). One input for each pair of distinct pawns of either
//             color that are on the same or adjacent files (any ranks). A pawn's id is its
//             square, relative to the perspective and mirrored if the perspective's king is on
//             files e-h, minus 8 (so 0..47), plus 48 for the opponent's pawns. The pair index is
//             hi*(hi-1)/2 + lo for the two ids. This must match the engine's
//             PawnPairFeature (src/nnue/features/ppfeature.h).
// The pair weights are separate from the piece-square weights so they can be clipped to the
// int8 range the engine stores them in, while the piece-square weights are int16.
//
// Architecture (as version 8):
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
// Saved file order (what the engine reads): header, piece-square weights (int16, one row of L1
// values per input), feature transformer biases (int16), pair weights (int8, one row of L1
// values per pair), then the head layers.
//
// To check the pair indices against the engine, print the active pair indices for some
// positions from both programs and compare them:
//   trainer: cargo run --release --example arasanv9 -- --dump-pairs [FEN ...]  (or FENs on stdin)
//   engine:  network/engine_pair_indices.py ENGINE FENFILE   (ENGINE built with NNUE_TRACE)
// Both print, for each FEN, the FEN and the sorted "white ..." and "black ..." index lists, so
// the outputs can be compared with diff. A set of test FENs is in pair_index_test_fens.txt.

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

const NET_NAME: &str = "arasan9";
const NETWORK_VERSION: u8 = 9;
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

// pair weights are stored as int8 at scale QA, so they must be within this range
const PP_RANGE: f32 = i8::MAX as f32 / (QA as f32);

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

type InputTy = (((((SparseInput, SparseInput), SparseInput), SparseInput), SparseInput), DenseInput<f32>);

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

fn make_inputs_mapper(
    params: (&ModelInputs<InputTy>, &PawnPairInputs, ChessBucketsMirrored, impl OutputBuckets<ChessBoard>),
    wdl: impl WdlScheduler,
) -> ModelInputsMapper<ChessBoard> {
    let pp = params.1.clone();

    ModelInputsMapper::build(
        params.0,
        move |pos, step, (((((stm_pp, ntm_pp), stm_psqt), ntm_psqt), bucket), target)| {
            let mut cnt = 0;
            params.2.map_features(pos, |stm, ntm| {
                stm_psqt[cnt] = stm.try_into().unwrap();
                ntm_psqt[cnt] = ntm.try_into().unwrap();
                cnt += 1;
            });

            if cnt < params.2.max_active() {
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

            bucket[0] = i32::from(params.3.bucket(pos));

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

    let pp = PawnPairInputs::new();
    let psqt = ChessBucketsMirrored::new(BUCKET_LAYOUT);
    let output_buckets = MaterialCount::<NUM_OUTPUT_BUCKETS>;

    let inputs = ModelInputs::default()
        .add_sparse("stm/pp", (pp.num_inputs(), 1), pp.max_active())
        .add_sparse("ntm/pp", (pp.num_inputs(), 1), pp.max_active())
        .add_sparse("stm/psqt", (psqt.num_inputs(), 1), psqt.max_active())
        .add_sparse("ntm/psqt", (psqt.num_inputs(), 1), psqt.max_active())
        .add_sparse("buckets", (NUM_OUTPUT_BUCKETS, 1), 1)
        .add_dense("targets", (1, 1));

    let defn = ModelDefinition::build(
        &inputs,
        |builder, (((((stm_pp, ntm_pp), stm_psqt), ntm_psqt), output_buckets), target)| {
            // pair weights and the feature transformer bias (the pair affine's bias is the
            // only bias of the feature transformer)
            let l0_pp = builder.new_affine("l0/pp/", pp.num_inputs(), L1);

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
            let ft = |pp, psqt, start, end| {
                (l0_pp.slice(start, end).forward(pp) + l0_psqt.slice_rows(start, end).matmul(psqt)).crelu()
            };
            let stm_hidden = ft(stm_pp, stm_psqt, 0, L1 / 2) * ft(stm_pp, stm_psqt, L1 / 2, L1);
            let ntm_hidden = ft(ntm_pp, ntm_psqt, 0, L1 / 2) * ft(ntm_pp, ntm_psqt, L1 / 2, L1);
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

    let train_params = (&inputs, &pp, psqt, output_buckets);
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
