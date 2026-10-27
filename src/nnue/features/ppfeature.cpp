// Copyright 2026 by Jon Dart. All Rights Reserved.
// Pawn pair feature: list of active features and feature deltas (see ppfeature.h)

#include "nnue.h"

// TBD speedups for pair features:
//  - fuse the PSQ and pair accumulator updates in simd::update (one tile loop, two weight tables)
//  - add two weight rows per operation on AVX2 too (needs pair weights stored in a permuted
//    order, as in Stockfish)
//  - share the work of getPairDeltas between the two perspectives (same pawn changes, different
//    orientation)
//  - avoid re-adding pairs after a refresh caused only by a king bucket change (pair indices
//    depend only on the file side of the king)

using nnue::PPIndexArray;
using PP = nnue::Network::FeatureXformer::PawnPair;

// bitboard of the files of the pawn and the adjacent files, for each file
static constexpr uint64_t fileBand(unsigned file) {
    constexpr uint64_t A = 0x0101010101010101ULL;
    uint64_t band = A << file;
    if (file > 0) band |= A << (file - 1);
    if (file < 7) band |= A << (file + 1);
    return band;
}

// Add to "out" the index of every pair that includes at least one pawn in "changed",
// where pawns[c] are all pawns of color c. A pair is added only once, even if both of its
// pawns are in "changed".
static void collectPairs(ColorType side, Square ksq, const Bitboard pawns[2],
                         const Bitboard changed[2], PPIndexArray &out, size_t &count) {
    const unsigned flipMask = nnue::FeatureUtils::squareFlipMask(side, ksq);
    // changed pawns that were already processed
    Bitboard done[2];
    for (ColorType c : colors) {
        Bitboard bb(changed[c]);
        Square sq;
        while (bb.iterate(sq)) {
            const unsigned id = PP::getPawnIdFromMask(side, flipMask, sq, c);
            const Bitboard band(fileBand(sq & 7));
            for (ColorType d : colors) {
                Bitboard partners(pawns[d] & band & ~static_cast<uint64_t>(done[d]));
                if (d == c) {
                    partners.clear(sq);
                }
                Square sq2;
                while (partners.iterate(sq2)) {
                    assert(count < out.size());
                    out[count++] = PP::getPairIndex(id, PP::getPawnIdFromMask(side, flipMask, sq2, d));
                }
            }
            done[c].set(sq);
        }
    }
}

template <typename WeightType, typename OutputType, typename AccumulatorType, size_t outputSize,
          size_t alignment>
size_t nnue::PawnPairFeature<WeightType, OutputType, AccumulatorType, outputSize, alignment>::
    getActivePairs(ColorType side, Square ksq, const Bitboard pawns[2], PPIndexArray &out) {
    size_t count = 0;
    collectPairs(side, ksq, pawns, pawns, out, count);
    assert(count < out.size());
    out[count] = nnue::LAST_INDEX;
    return count;
}

template <typename WeightType, typename OutputType, typename AccumulatorType, size_t outputSize,
          size_t alignment>
void nnue::PawnPairFeature<WeightType, OutputType, AccumulatorType, outputSize, alignment>::
    getPairDeltas(ColorType side, Square ksq, const Bitboard oldPawns[2], const Bitboard newPawns[2],
                  PPIndexArray &added, size_t &addedCount, PPIndexArray &removed,
                  size_t &removedCount) {
    // pawns that are only in one of the positions
    Bitboard removedPawns[2], addedPawns[2];
    for (ColorType c : colors) {
        removedPawns[c] = oldPawns[c] & ~static_cast<uint64_t>(newPawns[c]);
        addedPawns[c] = newPawns[c] & ~static_cast<uint64_t>(oldPawns[c]);
    }
    // Pairs between unchanged pawns are in both positions, so need not be updated.
    // Removed pairs are the pairs in the old position that include a removed pawn, and
    // added pairs are the pairs in the new position that include an added pawn.
    removedCount = addedCount = 0;
    collectPairs(side, ksq, oldPawns, removedPawns, removed, removedCount);
    collectPairs(side, ksq, newPawns, addedPawns, added, addedCount);
}

// explicit instantiation for the type used by the network (see FeatureTransformer)
template class nnue::PawnPairFeature<int8_t, int16_t,
                                     nnue::Accumulator<int16_t, nnue::NetworkParams::FT_OUTPUT_SIZE>,
                                     nnue::NetworkParams::FT_OUTPUT_SIZE>;
