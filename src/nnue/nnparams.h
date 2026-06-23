// Copyright 2024-2025 by Jon Dart. All Rights Reserved
#ifndef _NETWORK_PARAMS
#define _NETWORK_PARAMS

struct NetworkParams {

static constexpr unsigned KING_BUCKETS = 9;

static constexpr unsigned OUTPUT_BUCKETS = 8;

static constexpr unsigned BUCKET_DIVISOR = (32 + OUTPUT_BUCKETS - 1) / OUTPUT_BUCKETS;

static constexpr unsigned HIDDEN_WIDTH = 1792;

// 3-layer sparse int8 head (Stockfish/Hobbes-style), Raphael-style downstream:
//   feature transformer (HIDDEN_WIDTH per perspective, 2*HIDDEN_WIDTH total)
//     -> CReLU[0,NETWORK_QA] + pairwise-mul (per perspective) -> u8, width L1_IN
//     -> L1: sparse affine, u8 x int8 weights (dpbusd), int32, -> L1_SIZE
//     -> CReLU clamp to L1_OUT_CLAMP
//     -> L2: affine, int32, L1_SIZE -> L2_SIZE, no shift, CReLU clamp to L2_OUT_CLAMP
//     -> output: affine, int32 weights / int64 accumulate, L2_SIZE -> 1
//     -> score = output * OUTPUT_SCALE / OUTPUT_DIVISOR
static constexpr unsigned L1_SIZE = 16;
static constexpr unsigned L2_SIZE = 32;

// quantization for feature transformer, i.e. 0..1 in float domain is
// 0 .. NETWORK_QA in integer domain.
static constexpr int NETWORK_QA = 255;
static constexpr int NETWORK_QA_BITS = 8;
// weight quantization for the int8 sparse L1 layer
static constexpr int NETWORK_QB = 64;
// weight quantization for the int32 L2 and output (L3) layers (Raphael scheme)
static constexpr int NETWORK_QC = 64;
// network output value is multiplied by this to obtain position score
static constexpr int OUTPUT_SCALE = 400;

// Pairwise-mul activation (feature transformer -> L1 input): clamp the FT output
// to [0,NETWORK_QA], multiply paired neurons within a perspective, then right
// shift by (16 - PAIRWISE_SHIFT) to a u8 result. PAIRWISE_SHIFT=7 keeps
// NETWORK_QA << PAIRWISE_SHIFT within signed-int16 range (required by the SIMD
// mulhi path) and yields a u8 activation in [0, PAIRWISE_QUANT].
static constexpr unsigned PAIRWISE_SHIFT = 7;
static constexpr unsigned PAIRWISE_QUANT =
    (static_cast<unsigned>(NETWORK_QA) * NETWORK_QA) >> (16 - PAIRWISE_SHIFT); // 127
// width of the pairwise-mul output and the sparse L1 input: each perspective's
// HIDDEN_WIDTH outputs are halved by the pairing, two perspectives -> HIDDEN_WIDTH.
static constexpr unsigned L1_IN = HIDDEN_WIDTH;

// Integer quantization scales for the head. The sparse L1 layer emits an int32
// value whose "1.0" activation corresponds to PAIRWISE_QUANT * NETWORK_QB. The
// L2 and output layers use int32 weights quantized to NETWORK_QC with no
// intermediate dequantizing shift; the CReLU activations clamp to the running
// "1.0" scale, and a single divide by OUTPUT_DIVISOR dequantizes the final output.
static constexpr unsigned L1_OUT_CLAMP = PAIRWISE_QUANT * NETWORK_QB;
static constexpr unsigned L2_OUT_CLAMP = L1_OUT_CLAMP * NETWORK_QC;
static constexpr int OUTPUT_DIVISOR =
    static_cast<int>(L1_OUT_CLAMP) * NETWORK_QC * NETWORK_QC;

// clang-format off
static constexpr unsigned KING_BUCKETS_MAP[] = {
    0, 1, 2, 3, 3, 2, 1, 0,
    4, 4, 5, 5, 5, 5, 4, 4,
    6, 6, 7, 7, 7, 7, 6, 6,
    6, 6, 7, 7, 7, 7, 6, 6,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8};
// clang-format on

static inline unsigned getOutputBucket(unsigned pieceCount) {
    return (pieceCount - 2) / BUCKET_DIVISOR;
}

// version of the network (bumped to 8 for the sparse int8 head)
static constexpr std::byte NN_VERSION = std::byte(8);

static constexpr int NN_HEADER_SIZE = 4;

static constexpr std::byte NN_HEADER[NN_HEADER_SIZE] = {std::byte('A'), std::byte('R'), std::byte('A'), NN_VERSION};

};

#endif
