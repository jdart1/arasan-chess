// Copyright 2026 by Jon Dart. All Rights Reserved.
#ifndef _THREATFEATURE_H
#define _THREATFEATURE_H

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "attacks.h"
#include "board.h"
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

    // Get all active features for a position, for the point of view of side. The list is
    // terminated with LAST_INDEX. Returns the number of features.
    static size_t getActiveThreats(ColorType side, Square ksq, const Occupancies &occ,
                                   ThreatIndexArray &out) {
        size_t count = 0;
        collect(side, ksq, occ, Bitboard(~uint64_t(0)), [&](IndexType idx) {
            assert(count < out.size() - 1);
            out[count++] = idx;
        });
        out[count] = LAST_INDEX;
        return count;
    }

    static size_t getActiveThreats(ColorType side, const Board &board, ThreatIndexArray &out) {
        // the Board piece bitboards have the layout of Occupancies
        return getActiveThreats(side, board.kingSquare(side),
                                *reinterpret_cast<const Occupancies *>(&board.pawn_bits[0]), out);
    }

    // Get the changes in active features between two positions, however many moves apart.
    // Features in the old position but not the new one are put in "removed", those in the
    // new position but not the old one in "added": the lists have no feature in common, so
    // they are the smallest possible update. Both lists are sorted and terminated with
    // LAST_INDEX. ksq is the king square of side in the new position. It is used only for
    // its file, so the old king square must be on the same side of the board (a-d or e-h):
    // otherwise all features change and the caller should use getActiveThreats instead.
    //
    // Only the attackers that could have changed are examined: pieces on a square whose
    // contents changed, and pieces whose attacks include such a square (sliders gain or
    // lose attacks through squares that are vacated or occupied).
    static void getThreatDeltas(ColorType side, Square ksq, const Occupancies &oldOcc,
                                const Occupancies &newOcc, ThreatIndexArray &added, size_t &addedCount,
                                ThreatIndexArray &removed, size_t &removedCount) {
        addedCount = removedCount = 0;
        uint64_t changed = 0;
        for (unsigned i = 0; i < N_PIECES; ++i) {
            for (ColorType c : colors) {
                changed |= static_cast<uint64_t>(oldOcc[i][c]) ^ static_cast<uint64_t>(newOcc[i][c]);
            }
        }
        if (changed == 0) {
            added[0] = removed[0] = LAST_INDEX;
            return;
        }
        const Bitboard changedSquares(changed);
        const Bitboard attackers(affectedAttackers(oldOcc, changedSquares) |
                                 affectedAttackers(newOcc, changedSquares));
        collect(side, ksq, oldOcc, attackers, [&](IndexType idx) {
            assert(removedCount < removed.size() - 1);
            removed[removedCount++] = idx;
        });
        collect(side, ksq, newOcc, attackers, [&](IndexType idx) {
            assert(addedCount < added.size() - 1);
            added[addedCount++] = idx;
        });
        // cancel features that are in both lists
        std::sort(removed.begin(), removed.begin() + removedCount);
        std::sort(added.begin(), added.begin() + addedCount);
        size_t i = 0, j = 0, r = 0, a = 0;
        while (i < removedCount && j < addedCount) {
            if (removed[i] == added[j]) {
                ++i;
                ++j;
            } else if (removed[i] < added[j]) {
                removed[r++] = removed[i++];
            } else {
                added[a++] = added[j++];
            }
        }
        while (i < removedCount) removed[r++] = removed[i++];
        while (j < addedCount) added[a++] = added[j++];
        removedCount = r;
        addedCount = a;
        removed[removedCount] = added[addedCount] = LAST_INDEX;
    }

private:
    static Bitboard allPieces(const Occupancies &occ) {
        uint64_t all = 0;
        for (unsigned i = 0; i < N_PIECES; ++i) {
            all |= static_cast<uint64_t>(occ[i][White]) | static_cast<uint64_t>(occ[i][Black]);
        }
        return Bitboard(all);
    }

    // Squares of the non-king pieces that are on, or attack, a square in "changed"
    static Bitboard affectedAttackers(const Occupancies &occ, const Bitboard &changed) {
        const Bitboard all(allPieces(occ));
        Bitboard result;
        for (ColorType c : colors) {
            for (int t = Pawn; t <= Queen; ++t) {
                Bitboard pieces(occ[t - 1][c]);
                Square sq;
                while (pieces.iterate(sq)) {
                    if (changed.isSet(sq) ||
                        !Bitboard(Attacks::pieceAttacks(static_cast<PieceType>(t), c, sq, all) & changed)
                             .isClear()) {
                        result.set(sq);
                    }
                }
            }
        }
        return result;
    }

    // Call out(index) for each feature in the position whose attacker is on a square in
    // "attackers". Kings are never attackers or victims.
    template <typename Output>
    static void collect(ColorType side, Square ksq, const Occupancies &occ,
                        const Bitboard &attackers, Output &&out) {
        const Bitboard all(allPieces(occ));
        for (ColorType c : colors) {
            for (int t = Pawn; t <= Queen; ++t) {
                const PieceType type = static_cast<PieceType>(t);
                const Piece attacker = MakePiece(type, c);
                Bitboard pieces(occ[t - 1][c] & attackers);
                Square from;
                while (pieces.iterate(from)) {
                    const Bitboard targets(Attacks::pieceAttacks(type, c, from, all));
                    for (ColorType vc : colors) {
                        for (int vt = Pawn; vt <= Queen; ++vt) {
                            Bitboard victims(targets & occ[vt - 1][vc]);
                            Square to;
                            while (victims.iterate(to)) {
                                const IndexType idx =
                                    getIndex(side, attacker, from, to,
                                             MakePiece(static_cast<PieceType>(vt), vc), ksq);
                                if (idx != LAST_INDEX) {
                                    out(idx);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};

#endif
