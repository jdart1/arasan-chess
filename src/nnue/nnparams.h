// Copyright 2024-2025 by Jon Dart. All Rights Reserved
#ifndef _NETWORK_PARAMS
#define _NETWORK_PARAMS

struct NetworkParams {

    static constexpr unsigned KING_BUCKETS = 9;

    static constexpr unsigned OUTPUT_BUCKETS = 8;

    static constexpr unsigned BUCKET_DIVISOR = (32 + OUTPUT_BUCKETS - 1) / OUTPUT_BUCKETS;

    static constexpr unsigned HIDDEN_WIDTH_1 = 1536;
    static constexpr unsigned HIDDEN_WIDTH_2 = 16;
    static constexpr unsigned HIDDEN_WIDTH_3 = 32;

    // quantization factors, i.e. integer value that corresponds to 1.0 in the float domain
    static constexpr unsigned Q_FT = 255; // Feature transformer quantization
    static constexpr unsigned Q_HIDDEN = 64; // first hidden layer weight quantization
    static constexpr unsigned Q_HIDDEN_BITS = 6;
    static constexpr unsigned Q_HIDDEN2 = 128; // other hidden layers weight/bias quantization
    static constexpr unsigned Q_HIDDEN2_BITS = 7;
    static constexpr unsigned FT_SCALING_SHIFT = 9;
    // L1 input quantization: (Q_FT * Q_FT) >> FT_SCALING_SHIFT = (255 * 255) >> 9 = 127
    static constexpr unsigned Q_L1_IN = 127;
    // L1 output quantization: Q_L1_IN * Q_HIDDEN = 127 * 64 = 8128
    static constexpr unsigned Q_L1_OUT = Q_L1_IN * Q_HIDDEN;

    // dequantized network output value is multiplied by this to obtain position score
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

    static constexpr size_t NN_HEADER_SIZE = 4;

    // version of the network
    static constexpr unsigned NN_VERSION = 5;

    static constexpr char NN_HEADER[4] = {'A', 'R', 'A', static_cast<char>(NN_VERSION)};

};

#endif
