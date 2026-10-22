// Copyright 2026 by Jon Dart. All Rights Reserved.
// Compile-time tables for the threat feature index computation (see threatfeature.h)

#include "nnue.h"

using namespace nnue::ThreatTables;
using nnue::IndexType;
using nnue::LAST_INDEX;

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

static constexpr uint64_t bitFor(int rank, int file) {
    return (rank >= 0 && rank < 8 && file >= 0 && file < 8) ?
        (uint64_t(1) << (rank * 8 + file)) : 0;
}

// Attacks on an empty board. "down" is only meaningful for pawns: false = attacks
// toward higher ranks (own pawn after orientation), true = toward lower ranks.
static constexpr uint64_t emptyBoardAttacks(int type, int sq, bool down) {
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

static constexpr int countBits(uint64_t b) {
    int n = 0;
    for (; b; b &= b - 1) ++n;
    return n;
}

static constexpr Tables makeTables() {
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

constexpr Tables nnue::ThreatTables::TABLES = makeTables();

static_assert(makeTables().total == DIMENSIONS, "threat feature dimensions mismatch");
