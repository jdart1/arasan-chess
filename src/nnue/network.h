// Copyright 2021-2024 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_NETWORK_H
#define _NNUE_NETWORK_H

#include "accum.h"
#include "nnparams.h"
#include "features/arasanv3.h"
// for testing include >1 layer type
#include "layers/creluandlinear.h"
#include "layers/sqrcreluandlinear.h"
#include "util.h"

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
    using OutputLayer =
        SqrCReLUAndLinear<AccumulatorType, int16_t, int16_t, int16_t, OutputType,
                          NetworkParams::HIDDEN_WIDTH * 2, NetworkParams::NETWORK_QA,
                          NetworkParams::NETWORK_QA, NetworkParams::OUTPUT_BUCKETS, true, true>;

    static constexpr size_t BUFFER_SIZE = 4096;

    Network() : transformer(new FeatureXformer()) {
        outputLayer = new OutputLayer();
    }

    virtual ~Network() {
        delete transformer;
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

    // evaluate the net (layers past the first one)
    int32_t evaluate(const AccumulatorType &accum, [[maybe_unused]] ColorType sideToMove, unsigned bucket) const {
        alignas(nnue::DEFAULT_ALIGN) std::byte buffer[BUFFER_SIZE];
        // propagate data through the remaining layers
#ifdef NNUE_TRACE
        std::cout << "output bucket=" << bucket << std::endl;
        std::cout << "accumulator:" << std::endl;
        std::cout << accum << std::endl;
#endif
        // evaluate the output layer, in the correct bucket
        outputLayer->postProcessAccum(accum, bucket, reinterpret_cast<OutputType *>(buffer));
        int32_t nnOut = reinterpret_cast<int32_t *>(buffer)[0];
#ifdef NNUE_TRACE
        std::cout << "NN output, after scaling: "
                  << (nnOut * NetworkParams::OUTPUT_SCALE) / (NetworkParams::NETWORK_QA * NetworkParams::NETWORK_QB) << std::endl;
#endif
        return (nnOut * NetworkParams::OUTPUT_SCALE) / (NetworkParams::NETWORK_QA * NetworkParams::NETWORK_QB);
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
    OutputLayer *outputLayer;
    uint32_t version;
};

inline std::istream &operator>>(std::istream &s, Network &network) {
    // Arasan network files start with a 4-byte version
    network.version = read_little_endian<uint32_t>(s);
    if (s.fail()) return s;
    if (network.version != NetworkParams::NN_VERSION) {
        std::cerr << "Error: incorrect network version" << std::endl;
        s.setstate(std::ios::badbit);
        return s;
    }
    // read feature layer
    (void)network.transformer->read(s);
    if (s.fail()) return s;
    // read bucketed output layer
    (void)network.outputLayer->read(s);
    return s;
}

#endif
