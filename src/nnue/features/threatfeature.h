// Copyright 2026 by Jon Dart. All Rights Reserved.
#ifndef _THREATFEATURE_H
#define _THREATFEATURE_H

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "chess.h"
#include "featureutil.h"
#include "nndefs.h"
#include "util.h"

// Threat Feature set, as in Stockfish and a number of other engines (Pawnocchio,
// Reckless and Integral, for example).
//
// One input per "piece A attacks piece B" (direct attacks only, no x-rays).
// 59,808 inputs. Allowed pairs:
// - Pawn attacker: victims knight, rook only (pawn-vs-pawn is left to the separate
//   PawnPair pawn feature set); 4 slots (2 colors x 2 types)
// - Knight, Queen attacker: victims P, N, B, R, Q; 10 slots
// - Bishop, Rook attacker: victims P, N, B, R; 8 slots
// - King: never an attacker or victim
// Victim color (relative to the perspective) is part of the index.
//
// Note that same-type mutual attacks are kept in one direction only ("semi-excluded")
// only the `from > to` direction (after orientation) is kept; the other maps to the sentinel.
// Note also the rule for excluded victims: a pawn for example attacking a Queen is excluded,
// because in that case the Queen will also attack the pawn.
//
// Index layout: the index space is divided into one block per attacker piece (own pieces
// first, then opponent's). A block has one slot per allowed victim (color, type); a slot
// has one entry per (from, to) attack pair the attacker can make on an empty board. So
//    index = blockStart[attacker] + slot(victim) * attacksPerSlot[attacker type]
//            + attackPairIndex[attacker type][from][to]
// The first two terms are in lut1, the last in lut2. Both tables are built at compile time.
// Squares are oriented: from the perspective of "side" (rank flipped for Black), and
// mirrored horizontally if the side's king is on files e-h. Piece colors are relative to
// side. An excluded (attacker, victim) pair gives LAST_INDEX as a sentinel.

namespace ThreatTables {

static constexpr unsigned DIMENSIONS = 59808;

// piece types 0..5 = pawn, knight, bishop, rook, queen, king
static constexpr int TYPES = 6;

// Lookup tables used to compute indices, built at compile time (see threatfeature.cpp)
struct Tables {
    // [attacker piece][victim piece][from < to], piece = Piece value with color
    // bit (8) relative to the perspective (0 = own)
    IndexType lut1[16][16][2];
    // [attacker type][from][to]: offset of from's attack squares + rank of to among them
    uint16_t lut2[TYPES][64][64];
    // total number of index slots, should be DIMENSIONS
    unsigned total;
};

extern const Tables TABLES;

} // namespace ThreatTables

template <size_t alignment = DEFAULT_ALIGN>
class ThreatFeature {

public:
    ThreatFeature() = default;

    virtual ~ThreatFeature() = default;

    static constexpr unsigned DIMENSIONS = ThreatTables::DIMENSIONS;

    // Index of the feature "attacker on from attacks attacked on to", from the point
    // of view of side, whose king is on ksq. Returns LAST_INDEX if there is no such
    // feature (a pair that is not included).
    static IndexType getIndex(ColorType side, Piece attacker, Square from, Square to,
                              Piece attacked, Square ksq) {
        assert(attacker != EmptyPiece && attacked != EmptyPiece);
        // flip piece colors for the Black perspective, so that own pieces always have
        // the White piece values
        const unsigned swap = side == Black ? 8 : 0;
        const unsigned a = static_cast<unsigned>(attacker) ^ swap;
        const unsigned v = static_cast<unsigned>(attacked) ^ swap;
        const Square f = FeatureUtils::mirroredSquare(side, ksq, from);
        const Square t = FeatureUtils::mirroredSquare(side, ksq, to);
        const IndexType base = ThreatTables::TABLES.lut1[a][v][f < t];
        if (base == LAST_INDEX) {
            return LAST_INDEX;
        }
        const IndexType idx = base + ThreatTables::TABLES.lut2[(a & 7) - 1][f][t];
        assert(idx < DIMENSIONS);
        return idx;
    }
};

#endif
