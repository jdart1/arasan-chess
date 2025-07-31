 use bullet_lib::{
    game::outputs::MaterialCount,
    nn::{optimiser, InitSettings, Shape},
    trainer::{
        save::SavedFormat,
        schedule::{lr, wdl, TrainingSchedule, TrainingSteps},
        settings::LocalSettings,
    },
    default::{
        inputs, loader,
    },
    value::ValueTrainerBuilder,
};

macro_rules! net_id {
    () => {
        "arasan4"
    };
}

macro_rules! net_id {
    () => {
        "arasan3"
    };
}

const NET_ID: &str = net_id!();

fn main() {
    const NUM_INPUT_BUCKETS: usize = 9;
    const NUM_OUTPUT_BUCKETS: usize = 8;
    const L1: usize = 1536;
    const INITIAL_LR: f32 = 0.001;
    let FINAL_LR = INITIAL_LR * 0.4f32.powi(4);
    const SUPERBATCHES: usize = 240;

    #[rustfmt::skip]
    let mut trainer = ValueTrainerBuilder::default()
        .dual_perspective()
        .optimiser(optimiser::AdamW)
        .inputs(inputs::ChessBucketsMirrored::new([
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
            // merge in the factoriser weights
            SavedFormat::id("l0w")
                .add_transform(|builder, _, mut weights| {
                    let factoriser = builder.get_weights("l0f").get_dense_vals().unwrap();
                    let expanded = factoriser.repeat(NUM_INPUT_BUCKETS);

                    for (i, &j) in weights.iter_mut().zip(expanded.iter()) {
                        *i += j;
                    }

                    weights
                })
                .quantise::<i16>(255),
            SavedFormat::id("l0b").quantise::<i16>(255),
            SavedFormat::id("l1w").quantise::<i16>(64),
            SavedFormat::id("l1b").quantise::<i16>(255 * 64),
        ])
        .loss_fn(|output, target| output.sigmoid().squared_error(target))
        .build(|builder, stm_inputs, ntm_inputs, output_buckets| {
            // input layer factoriser
            let l0f = builder.new_weights("l0f", Shape::new(L1, 768), InitSettings::Zeroed);
            let expanded_factoriser = l0f.repeat(NUM_INPUT_BUCKETS);

            // input layer weights
            let mut l0 = builder.new_affine("l0", 768 * NUM_INPUT_BUCKETS, L1);
            l0.weights = l0.weights + expanded_factoriser;

            // output layer weights
            let l1 = builder.new_affine("l1", 2 * L1, NUM_OUTPUT_BUCKETS);

            // inference
            let stm_hidden = l0.forward(stm_inputs).screlu();
            let ntm_hidden = l0.forward(ntm_inputs).screlu();
            let hidden_layer = stm_hidden.concat(ntm_hidden);
            l1.forward(hidden_layer).select(output_buckets)
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
        lr_scheduler: lr::CosineDecayLR { initial_lr: INITIAL_LR, final_lr: FINAL_LR, final_superbatch: SUPERBATCHES },
//        lr_scheduler: lr::StepLR { start: 0.0015, gamma: 0.45, step: 60 },
        save_rate: 60,
    };

    let stricter_clipping = optimiser::AdamWParams { max_weight: 0.99, min_weight: -0.99, ..Default::default() };
    trainer.optimiser.set_params_for_weight("l0w", stricter_clipping);
    trainer.optimiser.set_params_for_weight("l0f", stricter_clipping);

    let settings = LocalSettings { threads: 8, test_set: None, output_directory: "checkpoints", batch_queue_size: 512 };

    let data_loader = loader::DirectSequentialDataLoader::new(&[
        "/data2/bullet/feb2025/pos-shuffled.bullet",
        "/data2/bullet/mar2025/pos-shuffled.bullet",
        "/data2/bullet/apr2025/pos-shuffled.bullet",
        "/data2/bullet/jun2025/pos-shuffled.bullet",
        "/data2/lc0/feb2025-2/pos-shuffled.bullet",
        "/data2/lc0/jun2025/pos-shuffled.bullet",
        "/data2/bullet/oct2024/lc0/lc0-test80-oct1-10-shuffled.bullet",
        "/data2/bullet/oct2024/lc0/lc0-test80-oct10-20-shuffled.bullet",
        "/data2/bullet/oct2024/lc0/lc0-test80-oct20-31-shuffled.bullet",
        "/data2/bullet/oct2024/lc0/lc0-test80-oct31-nov3-shuffled.bullet",
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
