// Copyright 2026 by Jon Dart. All Rights Reserved.
#ifndef _PPFEATURE_H
#define _PPFEATURE_H

#include <algorithm>
#include <cassert>
#include <cstddef>

#include "accum.h"
#include "bitboard.h"
#include "chess.h"
#include "featureutil.h"
#include "nndefs.h"
#include "util.h"

// Pawn pair feature set, originally invented by Jonathan Hallstrom for Pawnocchio,
// and now in other engines including Stockfish.
//
// There is one input per pair of distinct pawns, of either color, that are on the same
// or adjacent files. This allows encoding of important information such as isolated
// or doubled pawns. Each pawn has an id: its square relative to the perspective and
// mirrored by king file (see FeatureUtils), plus a pawn position index. There are 48
// possible pawn locations (since pawns cannot be on the first or last rank), so
// the side to move pawns have position indices 0..47, and the opponent's pawns have
// indices 48..96.
//
// The pair index is triangular in the two ids: hi*(hi-1)/2 + lo, where hi is the larger
// id. 96 ids for each pawn thus give 4,560 possible pair indices: this is the feature
// input size.
//
// A pair that is not a feature (pawns on files more than 1 apart) gives LAST_INDEX as
// a sentinel.
//
// The class also holds the weights for the features. They are 8-bit and are widened to 16
// bits when added to the accumulator. Biases are maintained in the PSQ feature.
template <typename WeightType, typename OutputType, typename AccumulatorType, size_t outputSize,
          size_t alignment = DEFAULT_ALIGN>
class PawnPairFeature {

public:
    PawnPairFeature() = default;

    virtual ~PawnPairFeature() = default;

    // number of possible pawn ids, and of pair features
    static constexpr unsigned PAWN_IDS = 96;
    static constexpr unsigned DIMENSIONS = PAWN_IDS * (PAWN_IDS - 1) / 2;

    // id of a pawn on sq with color pawnColor, from the point of view of side whose
    // king is on ksq
    static unsigned getPawnId(ColorType side, Square ksq, Square sq, ColorType pawnColor) {
        return getPawnIdFromMask(side, FeatureUtils::squareFlipMask(side, ksq), sq, pawnColor);
    }

    // as getPawnId, but with the square orientation mask (see FeatureUtils::squareFlipMask)
    // precomputed
    static unsigned getPawnIdFromMask(ColorType side, unsigned flipMask, Square sq,
                                      ColorType pawnColor) {
        const Square oriented = sq ^ flipMask;
        assert(oriented >= 8 && oriented < 56);
        return static_cast<unsigned>(oriented - 8) + (pawnColor != side ? 48 : 0);
    }

    // index for a pair of pawn ids
    static IndexType getPairIndex(unsigned id1, unsigned id2) {
        assert(id1 != id2 && id1 < PAWN_IDS && id2 < PAWN_IDS);
        const unsigned hi = std::max(id1, id2);
        const unsigned lo = std::min(id1, id2);
        return hi * (hi - 1) / 2 + lo;
    }

    // true if pawns on these squares can form a pair: they are distinct and on the
    // same or adjacent files
    static bool isPair(Square sq1, Square sq2) {
        const int fileDiff = (sq1 & 7) - (sq2 & 7);
        return sq1 != sq2 && fileDiff >= -1 && fileDiff <= 1;
    }

    // Index of the feature for the pawns on sq1 (color1) and sq2 (color2), from the
    // point of view of side whose king is on ksq. Returns LAST_INDEX if the pawns do
    // not form a pair.
    static IndexType getIndex(ColorType side, Square ksq, Square sq1, ColorType color1,
                              Square sq2, ColorType color2) {
        if (!isPair(sq1, sq2)) {
            return LAST_INDEX;
        }
        const IndexType idx = getPairIndex(getPawnId(side, ksq, sq1, color1),
                                           getPawnId(side, ksq, sq2, color2));
        assert(idx < DIMENSIONS);
        return idx;
    }

    // Get all active features for a position, for the point of view of side whose king
    // is on ksq. pawns[c] is the bitboard of pawns of color c. The list is terminated
    // with LAST_INDEX. Returns the number of features.
    static size_t getActivePairs(ColorType side, Square ksq, const Bitboard pawns[2],
                                 PPIndexArray &out);

    // Get the changes in active features between two positions with the given pawns.
    // Features that are in the old position but not the new one are put in "removed",
    // those in the new position but not the old one in "added". The king square is
    // used only for its file, so the old and new king squares must be on the same side
    // of the board (a-d or e-h): otherwise all features change and the caller should
    // use getActivePairs instead.
    static void getPairDeltas(ColorType side, Square ksq, const Bitboard oldPawns[2],
                              const Bitboard newPawns[2], PPIndexArray &added,
                              size_t &addedCount, PPIndexArray &removed, size_t &removedCount);

    // Update half of the accumulator in place: add the weights of the "added" features and
    // subtract those of the "removed" features. To add all features of a position to an
    // accumulator half, pass the list from getActivePairs as "added".
    void updateAccum(AccumulatorType &accum, AccumulatorHalf half, const PPIndexArray &added,
                     size_t addedCount, const PPIndexArray &removed,
                     size_t removedCount) const noexcept {
#ifdef SIMD
        simd::update<OutputType, WeightType, DIMENSIONS, outputSize>(
            accum.getOutput(half), accum.data(half), _weights, added.data(), addedCount,
            removed.data(), removedCount);
#else
        for (size_t i = 0; i < addedCount; ++i) {
            accum.add_half(half, _weights[added[i]]);
        }
        for (size_t i = 0; i < removedCount; ++i) {
            accum.sub_half(half, _weights[removed[i]]);
        }
#endif
    }

    // read weights from a stream
    virtual std::istream &read(std::istream &s) {
        for (size_t i = 0; i < DIMENSIONS && s.good(); ++i) {
            for (size_t j = 0; j < outputSize && s.good(); ++j) {
                _weights[i][j] = read_little_endian<WeightType>(s);
            }
        }
        return s;
    }

    virtual const WeightType *getCol(size_t row) const noexcept {
        return _weights[row];
    }

    virtual void setCol(size_t row, const WeightType *col) {
        for (size_t i = 0; i < outputSize; ++i) {
            _weights[row][i] = col[i];
        }
    }

private:
    alignas(alignment) WeightType _weights[DIMENSIONS][outputSize];
};

#endif
