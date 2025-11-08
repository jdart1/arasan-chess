// Copyright 2021-2025 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_NETWORK_H
#define _NNUE_NETWORK_H

#include "accum.h"
#include "nnparams.h"
#include "features/arasanv3.h"
#include "layers/crelu.h"
#include "layers/linear.h"
#include "layers/pairwisemult.h"
#include "layers/sparselinear.h"
#include "layers/sqrcrelu.h"
#include "util.h"

#include <memory>

class Network {

    friend class Evaluator;

  public:

    static constexpr size_t FeatureXformerRows = 12 * NetworkParams::KING_BUCKETS * 64;

    using OutputType = int32_t;
    using FeatureXformer = ArasanV3Feature<uint16_t, int16_t, int16_t, int16_t, FeatureXformerRows,
        NetworkParams::HIDDEN_WIDTH_1, NetworkParams::KING_BUCKETS_MAP>;
    using AccumulatorType = FeatureXformer::AccumulatorType;
    using AccumulatorOutputType = int16_t;
    using L1InputType = uint8_t;
    using L1WeightType = int8_t;
    using L2InputType = int32_t;
    using L3InputType = int32_t;

    static constexpr auto Q_H = NetworkParams::Q_HIDDEN;
    static constexpr auto Q_H_BITS = NetworkParams::Q_HIDDEN_BITS;
    static constexpr auto Q_H2 = NetworkParams::Q_HIDDEN2;
    static constexpr auto Q_H2_BITS = NetworkParams::Q_HIDDEN2_BITS;

    // definitions of the network layers:
    // Pairwise multiplication of the accumulator outputs.
    // Input quantization of the accumulator is Q_FT: y = sum(w * Q_FT) + b * Q_FT
    // Pairwise multiplcation adds another Q_FT quantization:
    // y = sum(w * Q_FT * Q_FT) + b * Q_FT * Q_FT
    // But output of the pairwise layer is then dequantized by back to Q_H range via a shift:
    // y = sum(w * Q_H) + b * Q_H
    // Outputs must be in range 0..127 to work properly with the following SparseLinear layer.
    using FTActivation =
        PairwiseMult<AccumulatorOutputType, L1InputType, NetworkParams::HIDDEN_WIDTH_1 * 2,
                     NetworkParams::Q_FT, NetworkParams::FT_SCALING_SHIFT>;
    // L1 layer, 8 bit inputs/weights, 32 bit outputs. Weights quantized to Q_H, biases to Q_L1_IN * Q_H.
    // y = sum(Q_L1_IN * x * Q_H * w) + Q_L1_IN * Q_H * b1 = Q_L1_OUT * (sum(x*w) + b1)
    // Because after this layer, we are operating on 32-bit quantities, do not dequantize the output.
#ifdef SIMD
    using L1 = SparseLinear<L1InputType, L1WeightType, int32_t /* biases */, int32_t /* output */,
                            NetworkParams::HIDDEN_WIDTH_1, NetworkParams::HIDDEN_WIDTH_2,
                            NetworkParams::OUTPUT_BUCKETS, 0, 0, false>;
#else
    using L1 = LinearLayer<L1InputType, L1WeightType, int32_t /* biases */, int32_t /* output */,
                           NetworkParams::HIDDEN_WIDTH_1, NetworkParams::HIDDEN_WIDTH_2,
                           NetworkParams::OUTPUT_BUCKETS, 0, 0, false>;
#endif
    // SqrCReLU activation.
    // Input clamped to range 0 .. 8128 (Q_L1_OUT = Q_L1_IN * Q_H)
    // y = clamp(x, 0, Q_L1_OUT)^2 >> 14
    // Output quantization: (Q_L1_OUT)^2 >> 14 = (8128)^2 >> 14 = 4032 ≈ Q_H * Q_H
    using L1Activation = SqrCReLU<int32_t, int32_t, NetworkParams::HIDDEN_WIDTH_2, NetworkParams::Q_L1_OUT, 14>;
    // L2 layer, 16x32, 32-bit inputs and weights. Weights quantized to Q_H2, biases to Q_H2 * Q_H * Q_H
    // y = sum(x * Q_H2 * Q_H * Q_H * w2) + Q_H2 * Q_H * Q_H * b
    // dequantize output by Q_H_BITS
    using L2 =
         LinearLayer<L2InputType, int32_t, int32_t, L3InputType, NetworkParams::HIDDEN_WIDTH_2,
                     NetworkParams::HIDDEN_WIDTH_3, NetworkParams::OUTPUT_BUCKETS, 0, Q_H_BITS, false>;
    // CReLU activation, clamp to Q_H * Q_H2
    // does not change the scaling
    using L2Activation =
        CReLU<int32_t, int32_t, NetworkParams::HIDDEN_WIDTH_3, Q_H * Q_H2, 0>;
    // Output layer, 32 bits. Weights quantized to Q_H2, biases to Q_H * Q_H2 * Q_H2
    // y = sum(x * Q_H * Q_H2 * Q_H2 * w2) + Q_H * Q_H2 * Q_H2 * b
    using OutputLayer =
        LinearLayer<L3InputType, int32_t, int32_t, OutputType, NetworkParams::HIDDEN_WIDTH_3, 1,
                    NetworkParams::OUTPUT_BUCKETS, 0, false>;

    Network()
        : transformer(new FeatureXformer()), ftActivation(new FTActivation()), l1(new L1()),
          l1Activation(new L1Activation()), l2(new L2()), l2Activation(new L2Activation()),
          outputLayer(new OutputLayer()) {}

    virtual ~Network() = default;

    inline static unsigned getIndex(ColorType kside, Square kp, Piece p, Square sq) {
#ifdef NDEBUG
        return FeatureXformer::getIndex(kside, kp, p, sq);
#else
        auto idx = FeatureXformer::getIndex(kside, kp, p, sq);
        assert(idx < FeatureXformerRows);
        return idx;
#endif
    }

    // evaluate the net (layers past the feature transformer)
    int32_t evaluate(const AccumulatorType &accum, [[maybe_unused]] ColorType sideToMove, unsigned bucket) const {
#ifdef NNUE_TRACE
        std::cout << "output bucket=" << bucket << std::endl;
#endif
        alignas(DEFAULT_ALIGN) L1InputType buffer1[NetworkParams::HIDDEN_WIDTH_1];
        alignas(DEFAULT_ALIGN) L2InputType buffer2[NetworkParams::HIDDEN_WIDTH_2];
        alignas(DEFAULT_ALIGN) L2InputType buffer3[NetworkParams::HIDDEN_WIDTH_2];
        alignas(DEFAULT_ALIGN) L3InputType buffer4[NetworkParams::HIDDEN_WIDTH_3];
        alignas(DEFAULT_ALIGN) L3InputType buffer5[NetworkParams::HIDDEN_WIDTH_3];
        alignas(DEFAULT_ALIGN) OutputType output[1];

        // pairwise mult, reduces 2 x 16-bit vectors to 1 8-bit vector
        ftActivation->forward(accum.getOutput(), buffer1);
        // L1 affine layer
        l1->forward(bucket, buffer1, buffer2);
        // activation
        l1Activation->forward(buffer2, buffer3);
        // L2 affine layer
        l2->forward(bucket, buffer3, buffer4);
        // activation
        l2Activation->forward(buffer4, buffer5);
        // output to single int32_t
        outputLayer->forward(bucket, buffer5, output);

        int32_t nnOut = output[0];

        // max scaled output is NetworkParams::OUTPUT_SCALE * 64 = 256 centipawns
        int32_t scaled = static_cast<int32_t>((static_cast<int64_t>(nnOut) * NetworkParams::OUTPUT_SCALE) / (Q_H2 * Q_H2 * Q_H));
#ifdef NNUE_TRACE
    std::cout << "NN output, before scaling = " << nnOut << ", after scaling: " << scaled << std::endl;
#endif
        return scaled;
    }

    friend std::istream &operator>>(std::istream &i, Network &);

    // Perform an incremental update
    void updateAccum(const AccumulatorType &source, AccumulatorHalf sourceHalf,
                     AccumulatorType &target, AccumulatorHalf targetHalf, const IndexArray &added,
                     size_t added_count, const IndexArray &removed,
                     size_t removed_count) const noexcept {
        transformer->updateAccum(source, sourceHalf, target, targetHalf, added, added_count,
                                 removed, removed_count);
    }

    // Propagate data through the layer, updating the specified half of the
    // accumulator (side to move goes in lower half).
    void updateAccum(const IndexArray &indices, AccumulatorHalf half,
                     AccumulatorType &output) const noexcept {
        transformer->updateAccum(indices, half, output);
    }

    uint32_t getVersion() const noexcept {
        return version;
    }

    FeatureXformer *getTransformer() const noexcept {
        return transformer.get();
    }

  protected:
    std::unique_ptr<FeatureXformer> transformer;
    std::unique_ptr<FTActivation> ftActivation;
    std::unique_ptr<L1> l1;
    std::unique_ptr<L1Activation> l1Activation;
    std::unique_ptr<L2> l2;
    std::unique_ptr<L2Activation> l2Activation;
    std::unique_ptr<OutputLayer> outputLayer;
    uint32_t version;
};

inline std::istream &operator>>(std::istream &s, Network &network) {
    // Arasan network files start with a 4-byte version header

    char header[NetworkParams::NN_HEADER_SIZE];
    (void)s.read(header, NetworkParams::NN_HEADER_SIZE);
    if (!s.good()) return s;
    for (size_t i = 0; i < NetworkParams::NN_HEADER_SIZE; ++i) {
        if (header[i] != NetworkParams::NN_HEADER[i]) {
            std::cerr << "Error: incorrect network version" << std::endl;
            s.setstate(std::ios::badbit);
            return s;
        }
    }
    // read feature layer
    (void)network.transformer->read(s);
    if (!s.good()) return s;
    // read bucketed hidden layers
    (void)network.l1->read(s);
    if (!s.good()) return s;
    (void)network.l2->read(s);
    if (!s.good()) return s;
    (void)network.outputLayer->read(s);
    return s;
}

#endif
