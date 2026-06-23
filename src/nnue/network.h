// Copyright 2021-2026 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_NETWORK_H
#define _NNUE_NETWORK_H

#include "accum.h"
#include "nnparams.h"
#include "features/arasanv3.h"
#include "layers/pairwisemult.h"
#include "layers/sparselinear.h"
#include "layers/linear.h"
#include "layers/crelu.h"
#include "util.h"

#include <algorithm>

// Three-layer NNUE with a sparse int8 head (Stockfish/Hobbes-style):
//
//   feature transformer -> FT activation -> L1 (sparse int8) -> L2 -> output
//
//   FT act: per-perspective CReLU[0,QA] + pairwise-mul -> u8, width L1_IN
//   L1:     sparse affine, u8 x int8 weights (dpbusd), L1_IN -> L1_SIZE, int32
//           (L1 output is CReLU-clamped to L1_OUT_CLAMP)
//   L2:     affine, L1_SIZE -> L2_SIZE, int32, no shift, then CReLU clamp to L2_OUT_CLAMP
//   output: affine, L2_SIZE -> 1, int32 weights with int64 accumulation
//   score:  output * OUTPUT_SCALE / OUTPUT_DIVISOR
//
// The L1, L2 and output layers are all bucketed by output bucket.
class Network {

    friend class Evaluator;

  public:

    static constexpr size_t FeatureXformerOutputSize = NetworkParams::HIDDEN_WIDTH;

    static constexpr size_t FeatureXformerRows = 12 * NetworkParams::KING_BUCKETS * 64;

    using OutputType = int32_t;
    using XformerBiasType = int16_t;
    using FeatureXformer = ArasanV3Feature<uint16_t, int16_t, XformerBiasType, int16_t, FeatureXformerRows,
        NetworkParams::HIDDEN_WIDTH, NetworkParams::KING_BUCKETS_MAP>;
    using AccumulatorType = FeatureXformer::AccumulatorType;
    using AccumulatorOutputType = int16_t;

    // Feature-transformer activation: per-perspective CReLU[0,QA] + pairwise-mul,
    // producing a u8 vector (each HIDDEN_WIDTH perspective -> HIDDEN_WIDTH/2 u8).
    using FTActivation =
        PairwiseMult<int16_t, uint8_t, NetworkParams::HIDDEN_WIDTH,
                     NetworkParams::NETWORK_QA, NetworkParams::PAIRWISE_SHIFT>;

    // L1: sparse affine, u8 input x int8 weights (dpbusd), int32 output, bucketed.
    // No dequantizing shift; biases are int32 at scale PAIRWISE_QUANT*NETWORK_QB.
    // transpose=false: bullet emits these weights with .transpose() applied, so
    // they arrive in bucket-major (for bucket: for output: for input) order,
    // which is what the LinearLayer non-transpose reader expects.
    using L1Layer =
        SparseLinear<uint8_t, int8_t, int32_t, int32_t, NetworkParams::L1_IN,
                     NetworkParams::L1_SIZE, NetworkParams::OUTPUT_BUCKETS, 0, 0, false>;

    // L2: int32 affine, L1_SIZE -> L2_SIZE, no dequantizing shift (Raphael scheme).
    using L2Layer =
        LinearLayer<int32_t, int32_t, int32_t, int32_t, NetworkParams::L1_SIZE,
                    NetworkParams::L2_SIZE, NetworkParams::OUTPUT_BUCKETS, 0, 0, false>;

    // Output layer: int32 weights, L2_SIZE -> 1. Weights and biases are int32
    // (as serialized by the trainer), but accumulation is int64, since the
    // QC^2-scaled product sum exceeds the 32-bit range. The SIMD path
    // (dotProductnx1_i64) accumulates products in int32 lanes and reduces into
    // an int64 result; see linear.h.
    using OutputLayerType =
        LinearLayer<int32_t, int32_t, int32_t, int64_t, NetworkParams::L2_SIZE, 1,
                    NetworkParams::OUTPUT_BUCKETS, 0, 0, false>;

    // CReLU activations between the layers (not bucketed). The clamp bounds carry
    // the running "1.0" integer scale forward.
    using L1Activation =
        CReLU<int32_t, int32_t, NetworkParams::L1_SIZE, NetworkParams::L1_OUT_CLAMP, 0>;
    using L2Activation =
        CReLU<int32_t, int32_t, NetworkParams::L2_SIZE, NetworkParams::L2_OUT_CLAMP, 0>;

    static constexpr size_t BUFFER_SIZE = 4096;

    Network() : transformer(new FeatureXformer()) {
        l1 = new L1Layer();
        l2 = new L2Layer();
        outputLayer = new OutputLayerType();
    }

    virtual ~Network() {
        delete transformer;
        delete l1;
        delete l2;
        delete outputLayer;
    }

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
        std::cout << "accumulator:" << std::endl;
        std::cout << accum << std::endl;
#endif
        // FT activation: per-perspective CReLU[0,QA] + pairwise-mul -> u8.
        // Lower accumulator half is the side to move; each HIDDEN_WIDTH-wide
        // perspective is reduced to HIDDEN_WIDTH/2 u8 values.
        alignas(nnue::DEFAULT_ALIGN) uint8_t l1In[NetworkParams::L1_IN];
        ftActivation.forward(accum.getOutput(), l1In);
        ftActivation.forward(accum.getOutput() + NetworkParams::HIDDEN_WIDTH,
                             l1In + NetworkParams::HIDDEN_WIDTH / 2);

        // L1: sparse u8 x int8 affine (dpbusd), int32 output
        alignas(nnue::DEFAULT_ALIGN) int32_t l1Out[NetworkParams::L1_SIZE];
        l1->forward(bucket, l1In, l1Out);

        // L1 activation: CReLU clamp to [0, L1_OUT_CLAMP]
        alignas(nnue::DEFAULT_ALIGN) int32_t l1Act[NetworkParams::L1_SIZE];
        l1Activation.forward(l1Out, l1Act);

        // L2: int32 affine (no shift), then CReLU clamp to [0, L2_OUT_CLAMP]
        alignas(nnue::DEFAULT_ALIGN) int32_t l2Out[NetworkParams::L2_SIZE];
        l2->forward(bucket, l1Act, l2Out);
        alignas(nnue::DEFAULT_ALIGN) int32_t l2Act[NetworkParams::L2_SIZE];
        l2Activation.forward(l2Out, l2Act);

        // Output layer: int32 weights, int64 accumulation -> single value
        alignas(nnue::DEFAULT_ALIGN) int64_t out[1];
        outputLayer->forward(bucket, l2Act, out);

        const int64_t nnOut = out[0];
        const int32_t score = static_cast<int32_t>(nnOut * NetworkParams::OUTPUT_SCALE /
                                                   NetworkParams::OUTPUT_DIVISOR);
#ifdef NNUE_TRACE
        std::cout << "NN output (raw) = " << nnOut << ", after scaling: " << score << std::endl;
#endif
        return score;
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
        return transformer;
    }

  protected:
    FeatureXformer *transformer;
    L1Layer *l1;
    L2Layer *l2;
    OutputLayerType *outputLayer;
    FTActivation ftActivation;
    L1Activation l1Activation;
    L2Activation l2Activation;
    uint32_t version;
};

inline std::istream &operator>>(std::istream &s, Network &network) {
    // Arasan network files start with a 4-byte version
    std::byte header[4];
    s.read(reinterpret_cast<char*>(header), 4);
    if (s.fail()) return s;
    for (int i = 0; i < NetworkParams::NN_HEADER_SIZE; i++) {
        if (header[i] != NetworkParams::NN_HEADER[i]) {
            std::cerr << "Error: incorrect network version" << std::endl;
            s.setstate(std::ios::badbit);
            return s;
        }
    }
    // Read layers in serialization order: transformer, L1, L2, output.
    (void)network.transformer->read(s);
    if (s.fail()) return s;
    (void)network.l1->read(s);
    if (s.fail()) return s;
    (void)network.l2->read(s);
    if (s.fail()) return s;
    (void)network.outputLayer->read(s);
    return s;
}

#endif
