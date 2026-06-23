use bullet_lib::{
    game::inputs::ChessBucketsMirrored,
    game::outputs::MaterialCount,
    nn::{optimiser, InitSettings, Shape},
    trainer::{
        save::SavedFormat,
        schedule::{lr, wdl, TrainingSchedule, TrainingSteps},
        settings::LocalSettings,
    },
    value::{ValueTrainerBuilder, loader::DirectSequentialDataLoader},

};

macro_rules! net_id {
    () => {
        "arasan8"
    };
}

const NET_ID: &str = net_id!();

fn main() {
    // Version 8: 3-layer sparse int8 head (Stockfish/Hobbes-style), with the
    // Raphael-style int32 downstream layers retained.
    //   feature transformer (768*buckets -> L1, per perspective)   [int16]
    //     -> CReLU[0,1] + pairwise-mul (per perspective)           -> u8, width L1
    //     -> L1 head:  L1 -> L2_NEURONS                            [int8 weights, int32 bias]
    //        (sparse u8 x int8 affine via dpbusd in the engine;
    //         engine CReLU-clamps the int32 output to L1_OUT_CLAMP = QP*QB)
    //     -> L2 head:  L2_NEURONS -> L3_NEURONS                    [int32 weights/bias, no shift]
    //        (engine CReLU-clamps to L2_OUT_CLAMP = QP*QB*QC)
    //     -> output:   L3_NEURONS -> 1                             [int32 weights, int64 accum]
    // The head uses no intermediate dequantizing shift; a single divide by
    // OUTPUT_DIVISOR = QP*QB*QC*QC in the engine recovers the float score.
    //
    // Quantization scales (must match the engine's nnparams.h):
    //   QA = 255 (feature transformer activation, integer scale of float 1.0)
    //   QP = 127 (pairwise-mul u8 activation scale = (QA*QA) >> (16-7), the engine
    //             clamps to QA and right-shifts by 16-PAIRWISE_SHIFT, PAIRWISE_SHIFT=7)
    //   QB = 64  (int8 L1 head weights)
    //   QC = 64  (int32 L2/output head weights)
    // Bias quantise factors are the product of input scale and weight scale at
    // each layer (no intermediate shift):
    //   l1b: QP*QB        = 127*64       = 8128
    //   l2b: QP*QB*QC     = 8128*64      = 520192
    //   l3b: QP*QB*QC*QC  = 520192*64    = 33292288   (== engine OUTPUT_DIVISOR)
    const NETWORK_VERSION: u8 = 8;
    const NUM_INPUT_BUCKETS: usize = 9;
    const NUM_OUTPUT_BUCKETS: usize = 8;
    const L1: usize = 1792;        // feature transformer width (per perspective)
    const L2_NEURONS: usize = 16;  // sparse L1 head output width (engine L1_SIZE)
    const L3_NEURONS: usize = 32;  // second head layer width (engine L2_SIZE)

    // Pairwise-mul u8 activation scale (see above). Used for the head bias scales.
    const QP: i32 = (255 * 255) >> (16 - 7); // 127
    const QB: i32 = 64;
    const QC: i32 = 64;

    const INITIAL_LR: f32 = 0.001;
    let final_lr = INITIAL_LR * 0.01;
    const SUPERBATCHES: usize = 800;

    #[rustfmt::skip]
    let mut trainer = ValueTrainerBuilder::default()
        .dual_perspective()
        .optimiser(optimiser::AdamW)
        .inputs(ChessBucketsMirrored::new([
            0, 1, 2, 3,
            4, 4, 5, 5,
            6, 6, 7, 7,
            6, 6, 7, 7,
            8, 8, 8, 8,
            8, 8, 8, 8,
            8, 8, 8, 8,
            8, 8, 8, 8]))
        .output_buckets(MaterialCount::<NUM_OUTPUT_BUCKETS>)
        .save_format(&[
            // Arasan header
            SavedFormat::custom([b'A', b'R', b'A', NETWORK_VERSION]),
            // feature transformer: merge in the factoriser weights
            SavedFormat::id("l0w")
            .transform(|store, weights| {
                    let factoriser = store.get("l0f").values.f32().repeat(NUM_INPUT_BUCKETS);
                    weights.into_iter().zip(factoriser).map(|(a, b)| a + b).collect()
                })
                .round()
                .quantise::<i16>(255),
            SavedFormat::id("l0b").quantise::<i16>(255),
            // L1 head (sparse int8). .transpose() puts weights in bucket-major
            // (for bucket: for output: for input) order to match the engine's
            // SparseLinear/LinearLayer non-transpose reader. Biases are int32 at
            // scale QP*QB (the dpbusd product scale, no intermediate shift).
            SavedFormat::id("l1w").transpose().round().quantise::<i8>(QB.try_into().unwrap()),
            SavedFormat::id("l1b").round().quantise::<i32>(QP * QB),
            // L2 and output heads (int32, Raphael scheme). Weights quantise to QC;
            // there is no intermediate dequantizing shift, so each bias is at the
            // product scale of its layer: l2b at QP*QB*QC, l3b at QP*QB*QC*QC
            // (the latter equals the engine's OUTPUT_DIVISOR).
            SavedFormat::id("l2w").transpose().round().quantise::<i32>(QC),
            SavedFormat::id("l2b").round().quantise::<i32>(QP * QB * QC),
            SavedFormat::id("l3w").transpose().round().quantise::<i32>(QC),
            SavedFormat::id("l3b").round().quantise::<i32>(QP * QB * QC * QC),
        ])
        .loss_fn(|output, target| output.sigmoid().squared_error(target))
        .build(|builder, stm_inputs, ntm_inputs, output_buckets| {
            // input layer factoriser
            let l0f = builder.new_weights("l0f", Shape::new(L1, 768), InitSettings::Zeroed);
            let expanded_factoriser = l0f.repeat(NUM_INPUT_BUCKETS);

            // feature transformer
            let mut l0 = builder.new_affine("l0", 768 * NUM_INPUT_BUCKETS, L1);
            l0.weights = l0.weights + expanded_factoriser;

            // bucketed head: each affine produces (neurons * buckets) outputs;
            // select() picks the active bucket's contiguous slice. After the
            // per-perspective pairwise-mul the L1 input width is L1 (each
            // perspective's L1 outputs are halved, two perspectives -> L1).
            let l1 = builder.new_affine("l1", L1, NUM_OUTPUT_BUCKETS * L2_NEURONS);
            let l2 = builder.new_affine("l2", L2_NEURONS, NUM_OUTPUT_BUCKETS * L3_NEURONS);
            let l3 = builder.new_affine("l3", L3_NEURONS, NUM_OUTPUT_BUCKETS);

            // inference: feature transformer -> CReLU[0,1] + pairwise-mul (per
            // perspective), matching the engine's PairwiseMult<int16,u8> FT
            // activation. The pairwise product is the only nonlinearity feeding
            // the head; subsequent layers use plain clipped ReLU [0,1].
            let stm_hidden = l0.forward(stm_inputs).crelu().pairwise_mul();
            let ntm_hidden = l0.forward(ntm_inputs).crelu().pairwise_mul();
            let hidden_layer = stm_hidden.concat(ntm_hidden);

            let l1_out = l1.forward(hidden_layer).select(output_buckets).crelu();
            let l2_out = l2.forward(l1_out).select(output_buckets).crelu();
            l3.forward(l2_out).select(output_buckets)
        });

    let schedule = TrainingSchedule {
        net_id: NET_ID.to_string(),
        eval_scale: 400.0,
        steps: TrainingSteps {
            batch_size: 16_384,
            batches_per_superbatch: 6104,
            start_superbatch: 1,
            end_superbatch: SUPERBATCHES,
        },
        wdl_scheduler: wdl::ConstantWDL { value: 0.0 },
        lr_scheduler: lr::Warmup{ inner: lr::CosineDecayLR { initial_lr: INITIAL_LR, final_lr: final_lr, final_superbatch: SUPERBATCHES },
                warmup_batches: 10},
        save_rate: 100,
    };

    let stricter_clipping = optimiser::AdamWParams { max_weight: 0.99, min_weight: -0.99, ..Default::default() };
    trainer.optimiser.set_params_for_weight("l0w", stricter_clipping);
    trainer.optimiser.set_params_for_weight("l0f", stricter_clipping);

    let settings = LocalSettings { threads: 8, test_set: None, output_directory: "checkpoints", batch_queue_size: 512 };

    let data_loader = DirectSequentialDataLoader::new(&[
        "/data3/bullet/apr-may2026/dec25-may26-shuffled-interleaved.bullet",
    ]);

    trainer.run(&schedule, &settings, &data_loader);

    for fen in [
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
        "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
        "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
    ] {
        let eval = trainer.eval(fen);
        println!("FEN: {fen}");
        println!("EVAL: {}", 400.0 * eval);
    }
}
