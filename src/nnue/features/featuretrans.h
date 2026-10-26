// Copyright 2026 by Jon Dart. All Rights Reserved.
#ifndef _FEATURETRANS_H
#define _FEATURETRANS_H

#include "accum.h"
#include "chess.h"
#include "nndefs.h"
#include "ppfeature.h"
#include "psqfeature.h"

// Wrapper class that contains the feature sets. Index calculation is
// feature-specific and is accessed via the feature classes, for
// example PSQ::getIndex.
template <typename InputType, typename WeightType, typename BiasType, typename OutputType, size_t inputSize,
          size_t outputSize, const unsigned kingBucketsMap[64], size_t alignment = DEFAULT_ALIGN>
class FeatureTransformer
{
public:

    using AccumulatorType = Accumulator<OutputType, outputSize>;

    using PSQ = PSQFeature<InputType, OutputType, WeightType, BiasType, AccumulatorType, inputSize,
                           outputSize, kingBucketsMap, alignment>;

    // pair feature weights are 8-bit
    using PawnPair = PawnPairFeature<int8_t, OutputType, AccumulatorType, outputSize, alignment>;

    FeatureTransformer() = default;

    virtual ~FeatureTransformer() = default;

    // Full update: propagate data through the layer, updating the specified half of the
    // accumulator (side to move goes in lower half).
    void updateAccum(const PSQIndexArray &indices, AccumulatorHalf half,
                     AccumulatorType &output) const noexcept {
        psq.updateAccum(indices, half, output);
    }

    // Perform an incremental update
    void updateAccum(const AccumulatorType &source, AccumulatorHalf sourceHalf,
                     AccumulatorType &target, AccumulatorHalf targetHalf,
                     const PSQIndexArray &added, size_t added_count, const PSQIndexArray &removed,
                     size_t removed_count) const noexcept {
        psq.updateAccum(source, sourceHalf, target, targetHalf, added, added_count, removed,
                        removed_count);
    }

    // Perform an incremental update in place
    void updateAccum(const PSQIndexArray &added, const PSQIndexArray &removed,
                     size_t added_count, size_t removed_count,
                     AccumulatorHalf half, AccumulatorType &output) const noexcept {
        psq.updateAccum(added, removed, added_count, removed_count, half, output);
    }

    // Update half of the accumulator with pawn pair features
    void updatePawnPairs(AccumulatorType &accum, AccumulatorHalf half, const PPIndexArray &added,
                         size_t addedCount, const PPIndexArray &removed,
                         size_t removedCount) const noexcept {
        pawnPairs.updateAccum(accum, half, added, addedCount, removed, removedCount);
    }

    // read weights from a stream
    virtual std::istream &read(std::istream &s) {
        // the pawn pair weights follow the piece-square weights and biases
        psq.read(s);
        return pawnPairs.read(s);
    }

    PawnPair &getPawnPairs() noexcept {
        return pawnPairs;
    }

    const BiasType *getBiases() const noexcept {
        return psq.getBiases();
    }

private:
    PSQ psq;
    PawnPair pawnPairs;
};
#endif
