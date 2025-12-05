// Copyright 2024-2025 by Jon Dart. All Rights Reserved
#ifndef _NETWORK_PARAMS
#define _NETWORK_PARAMS

struct NetworkParams {

static constexpr unsigned KING_BUCKETS = 9;

static constexpr unsigned OUTPUT_BUCKETS = 8;

static constexpr unsigned BUCKET_DIVISOR = (32 + OUTPUT_BUCKETS - 1) / OUTPUT_BUCKETS;

static constexpr unsigned HIDDEN_WIDTH = 1792;

// quantization for feature transformer, i.e. 0..1 in float domain is
// 0 .. NETWORK_QA in integer domain.
static constexpr int NETWORK_QA = 255;
static constexpr int NETWORK_QA_BITS = 8;
// quantization for the L1 layer
static constexpr int NETWORK_QB = 64;
// network output value is multiplied by this to obtain position score
static constexpr int OUTPUT_SCALE = 400;

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

// version of the network
static constexpr std::byte NN_VERSION = std::byte(5);

static constexpr int NN_HEADER_SIZE = 4;

static constexpr std::byte NN_HEADER[NN_HEADER_SIZE] = {std::byte('A'), std::byte('R'), std::byte('A'), NN_VERSION};

};

#endif
