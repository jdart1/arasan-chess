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

// victim slot for [attacker type][victim type], or -1 if not an allowed pair.
// Pawn-vs-pawn threats are not included (covered by pawn pair features). A bishop
// or queen victim is excluded for a pawn attacker (it attacks the pawn back).
static constexpr int VICTIM_MAP[TYPES][TYPES] = {
    {-1, 0, -1, 1, -1, -1},
    { 0, 1,  2, 3,  4, -1},
    { 0, 1,  2, 3, -1, -1},
    { 0, 1,  2, 3, -1, -1},
    { 0, 1,  2, 3,  4, -1},
    {-1, -1, -1, -1, -1, -1}
};

constexpr uint64_t bitFor(int rank, int file) {
    return (rank >= 0 && rank < 8 && file >= 0 && file < 8) ?
        (uint64_t(1) << (rank * 8 + file)) : 0;
}

// Attacks on an empty board. "down" is only meaningful for pawns: false = attacks
// toward higher ranks (own pawn after orientation), true = toward lower ranks.
constexpr uint64_t emptyBoardAttacks(int type, int sq, bool down) {
    const int r = sq / 8;
    const int f = sq % 8;
    uint64_t result = 0;
    switch (type) {
    case 0: {
        const int dr = down ? -1 : 1;
        result = bitFor(r + dr, f - 1) | bitFor(r + dr, f + 1);
        break;
    }
    case 1: {
        constexpr int dr[8] = {1, 2, 2, 1, -1, -2, -2, -1};
        constexpr int df[8] = {2, 1, -1, -2, -2, -1, 1, 2};
        for (int i = 0; i < 8; ++i) result |= bitFor(r + dr[i], f + df[i]);
        break;
    }
    case 5: {
        for (int dr = -1; dr <= 1; ++dr)
            for (int df = -1; df <= 1; ++df)
                if (dr || df) result |= bitFor(r + dr, f + df);
        break;
    }
    default: {
        // sliders: type 2 = bishop (diagonals), 3 = rook (ranks/files), 4 = queen (both)
        for (int dr = -1; dr <= 1; ++dr) {
            for (int df = -1; df <= 1; ++df) {
                if (!dr && !df) continue;
                const bool diagonal = dr && df;
                if ((type == 2 && !diagonal) || (type == 3 && diagonal)) continue;
                for (int rr = r + dr, ff = f + df; rr >= 0 && rr < 8 && ff >= 0 && ff < 8;
                     rr += dr, ff += df)
                    result |= bitFor(rr, ff);
            }
        }
        break;
    }
    }
    return result;
}

constexpr int countBits(uint64_t b) {
    int n = 0;
    for (; b; b &= b - 1) ++n;
    return n;
}

struct Tables {
    // [attacker piece][victim piece][from < to], piece = Piece value with color
    // bit (8) relative to the perspective (0 = own)
    IndexType lut1[16][16][2];
    // [attacker type][from][to]: offset of from's attack squares + rank of to among them
    uint16_t lut2[TYPES][64][64];
    // total number of index slots, should be DIMENSIONS
    unsigned total;
};

constexpr Tables makeTables() {
    Tables t{};
    // victim slots per attacker type: 2 colors x number of allowed victim types
    int numTargets[TYPES] = {};
    for (int a = 0; a < TYPES; ++a) {
        for (int v = 0; v < TYPES; ++v) {
            if (VICTIM_MAP[a][v] >= 0) numTargets[a] += 2;
        }
    }
    // lut2, and number of (from, to) attack pairs per attacker type
    int attacksPerSlot[TYPES] = {};
    for (int type = 0; type < TYPES; ++type) {
        int base = 0;
        for (int from = 0; from < 64; ++from) {
            // a pawn can't be on the first or last rank
            const bool pawnEnd = type == 0 && (from < 8 || from >= 56);
            const uint64_t up = emptyBoardAttacks(type, from, false);
            // for pawns, attacks toward lower squares are used when to < from
            const uint64_t down = type == 0 ? emptyBoardAttacks(type, from, true) : up;
            int rankUp = 0, rankDown = 0;
            for (int to = 0; to < 64; ++to) {
                const int rank = (type == 0 && to < from) ? rankDown : rankUp;
                t.lut2[type][from][to] = static_cast<uint16_t>(base + rank);
                if (up & (uint64_t(1) << to)) ++rankUp;
                if (down & (uint64_t(1) << to)) ++rankDown;
            }
            if (!pawnEnd) base += countBits(up);
        }
        attacksPerSlot[type] = base;
    }
    // start of the block for each attacker piece: own pieces first, then opponent's
    IndexType blockStart[2][TYPES] = {};
    unsigned total = 0;
    for (int color = 0; color < 2; ++color) {
        for (int type = 0; type < TYPES; ++type) {
            blockStart[color][type] = total;
            total += static_cast<unsigned>(numTargets[type] * attacksPerSlot[type]);
        }
    }
    t.total = total;
    // lut1: default is the sentinel
    for (int a = 0; a < 16; ++a)
        for (int v = 0; v < 16; ++v)
            for (int lt = 0; lt < 2; ++lt) t.lut1[a][v][lt] = LAST_INDEX;
    for (int ac = 0; ac < 2; ++ac) {
        for (int at = 0; at < TYPES; ++at) {
            for (int vc = 0; vc < 2; ++vc) {
                for (int vt = 0; vt < TYPES; ++vt) {
                    const int map = VICTIM_MAP[at][vt];
                    if (map < 0) continue;
                    const bool enemy = ac != vc;
                    // mutual same-type attacks are kept in one direction only
                    const bool semiExcluded = at == vt && (enemy || at != 0);
                    const IndexType feature = blockStart[ac][at] +
                        static_cast<IndexType>((vc * (numTargets[at] / 2) + map) *
                                               attacksPerSlot[at]);
                    // Piece values: type 1..6, black pieces + 8
                    const int a = (ac << 3) | (at + 1);
                    const int v = (vc << 3) | (vt + 1);
                    // entry [0] is used when from > to, entry [1] when from < to
                    t.lut1[a][v][0] = feature;
                    t.lut1[a][v][1] = semiExcluded ? LAST_INDEX : feature;
                }
            }
        }
    }
    return t;
}

static constexpr Tables TABLES = makeTables();

static_assert(TABLES.total == DIMENSIONS, "threat feature dimensions mismatch");

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
