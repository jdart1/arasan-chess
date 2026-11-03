// Copyright 2026 by Jon Dart. All Rights Reserved.
#ifndef _FEATURETRANS_H
#define _FEATURETRANS_H

#include "accum.h"
#include "chess.h"
#include "nndefs.h"
#include "ppfeature.h"
#include "psqfeature.h"
#include "threatfeature.h"

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

    // threat feature weights are 8-bit
    using Threat = ThreatFeature<int8_t, OutputType, AccumulatorType, outputSize, alignment>;

    FeatureTransformer() = default;

    virtual ~FeatureTransformer() = default;

    // Full update: compute the specified half of the accumulator (side to move goes in lower
    // half) from the biases and the "added" features.
    void updateAccum(const FeatureChanges &changes, AccumulatorHalf half,
                     AccumulatorType &output) const noexcept {
#ifdef SIMD
        updateRows(reinterpret_cast<const OutputType *>(psq.getBiases()), output.data(half), changes);
#else
        output.init_half(half, psq.getBiases());
        updateRows(changes, half, output);
#endif
    }

    // Perform an incremental update, in one pass over the accumulator. The source and
    // target can be the same.
    void updateAccum(const AccumulatorType &source, AccumulatorHalf sourceHalf,
                     AccumulatorType &target, AccumulatorHalf targetHalf,
                     const FeatureChanges &changes) const noexcept {
#ifdef SIMD
        updateRows(source.getOutput(sourceHalf), target.data(targetHalf), changes);
#else
        if (&target != &source) {
            target.copy_half(targetHalf, source, sourceHalf);
        }
        updateRows(changes, targetHalf, target);
#endif
    }

    // read weights from a stream
    virtual std::istream &read(std::istream &s) {
        // the pawn pair weights follow the piece-square weights and biases
        psq.read(s);
        pawnPairs.read(s);
        // and the threat weights follow the pawn pair weights
        threats.read(s);
        return s;
    }

    // Prefetch the threat weight rows for the threat features in the changes
    void prefetchThreats(const FeatureChanges &c) const noexcept {
        threats.prefetch(c.threatAdded, c.threatAddedCount);
        threats.prefetch(c.threatRemoved, c.threatRemovedCount);
    }

    PawnPair &getPawnPairs() noexcept {
        return pawnPairs;
    }

    const BiasType *getBiases() const noexcept {
        return psq.getBiases();
    }

private:
#ifdef SIMD
    // apply the changes in one pass over the accumulator
    void updateRows(const OutputType *source, OutputType *target,
                    const FeatureChanges &c) const noexcept {
        simd::updateRows<OutputType, outputSize>(
            source, target,
            psq.rowUpdate(c.psqAdded, c.psqAddedCount, c.psqRemoved, c.psqRemovedCount),
            pawnPairs.rowUpdate(c.ppAdded, c.ppAddedCount, c.ppRemoved, c.ppRemovedCount),
            threats.rowUpdate(c.threatAdded, c.threatAddedCount, c.threatRemoved,
                              c.threatRemovedCount));
    }
#else
    // apply the changes to the accumulator
    void updateRows(const FeatureChanges &c, AccumulatorHalf half,
                    AccumulatorType &accum) const noexcept {
        psq.updateAccum(c.psqAdded, c.psqAddedCount, c.psqRemoved, c.psqRemovedCount, half, accum);
        pawnPairs.updateAccum(accum, half, c.ppAdded, c.ppAddedCount, c.ppRemoved,
                              c.ppRemovedCount);
        threats.updateAccum(accum, half, c.threatAdded, c.threatAddedCount, c.threatRemoved,
                            c.threatRemovedCount);
    }
#endif

    PSQ psq;
    PawnPair pawnPairs;
    Threat threats;
};
#endif
