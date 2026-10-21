// Copyright 2026 by Jon Dart. All Rights Reserved.
#ifndef _FEATURETRANS_H
#define _FEATURETRANS_H

#include "accum.h"
#include "chess.h"
#include "nndefs.h"
#include "ppfeature.h"
#include "psqfeature.h"
#include "threatfeature.h"

// Wrapper class that contains the feature sets (PSQ, threats, pawn pairs;
// the latter two are stubs for now). Index calculation is feature-specific
// and is accessed via the feature classes, for example PSQ::getIndex.
template <typename InputType, typename WeightType, typename BiasType, typename OutputType, size_t inputSize,
          size_t outputSize, const unsigned kingBucketsMap[64], size_t alignment = DEFAULT_ALIGN>
class FeatureTransformer
{
public:

    using AccumulatorType = Accumulator<OutputType, outputSize>;

    using PSQ = PSQFeature<InputType, OutputType, WeightType, BiasType, AccumulatorType, inputSize,
                           outputSize, kingBucketsMap, alignment>;

    FeatureTransformer() = default;

    virtual ~FeatureTransformer() = default;

    // Full update: propagate data through the layer, updating the specified half of the
    // accumulator (side to move goes in lower half).
    void updateAccum(const IndexArray &indices, AccumulatorHalf half,
                     AccumulatorType &output) const noexcept {
        psq.updateAccum(indices, half, output);
    }

    // Perform an incremental update
    void updateAccum(const AccumulatorType &source, AccumulatorHalf sourceHalf,
                     AccumulatorType &target, AccumulatorHalf targetHalf,
                     const IndexArray &added, size_t added_count, const IndexArray &removed,
                     size_t removed_count) const noexcept {
        psq.updateAccum(source, sourceHalf, target, targetHalf, added, added_count, removed,
                        removed_count);
    }

    // Perform an incremental update in place
    void updateAccum(const IndexArray &added, const IndexArray &removed,
                     size_t added_count, size_t removed_count,
                     AccumulatorHalf half, AccumulatorType &output) const noexcept {
        psq.updateAccum(added, removed, added_count, removed_count, half, output);
    }

    // read weights from a stream
    virtual std::istream &read(std::istream &s) {
        return psq.read(s);
    }

    const BiasType *getBiases() const noexcept {
        return psq.getBiases();
    }

private:
    PSQ psq;
    ThreatFeature<alignment> threats;
    PawnPairFeature<alignment> pawnPairs;
};
#endif
