// Copyright 2026 by Jon Dart. All Rights Reserved.
#ifndef _THREATFEATURE_H
#define _THREATFEATURE_H

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>

#include "accum.h"
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

// A threat feature independent of the perspective: the (colored) attacker piece and its square,
// and the (colored) victim piece and its square. The index of the feature for each perspective is
// computed from this, so a list of these can be shared by both perspectives.
struct ThreatDelta {
    uint8_t attacker, from, victim, to;
};

// The threat features that differ between two positions
struct ThreatChanges {
    std::array<ThreatDelta, MAX_THREAT_INDICES> added, removed;
    size_t addedCount = 0, removedCount = 0;
};

// The class also holds the weights for the features. They are
// 8-bit and are widened to 16 bits when added to the accumulator. Biases are maintained
// in the PSQ feature.
template <typename WeightType, typename OutputType, typename AccumulatorType, size_t outputSize,
          size_t alignment = DEFAULT_ALIGN>
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
        collect(side, ksq, occ, allPieces(occ), Bitboard(~uint64_t(0)), [&](IndexType idx) {
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

    // Get the threat features that differ between two positions, however many moves apart,
    // independent of the perspective. Features in the old position but not the new one are put
    // in "removed", those in the new position but not the old one in "added": the lists have no
    // feature in common, so they are the smallest possible update. The lists are not sorted.
    //
    // Only the attackers that could have changed are examined: pieces on a square whose
    // contents changed, and pieces whose attacks include such a square (sliders gain or
    // lose attacks through squares that are vacated or occupied). For an attacker that is the
    // same in both positions, only the targets that are attacked in just one position, or that
    // are on a changed square, give features that differ.
    static void getThreatChanges(const Occupancies &oldOcc, const Occupancies &newOcc,
                                 ThreatChanges &changes) {
        changes.addedCount = changes.removedCount = 0;
        uint64_t changedBits = 0;
        for (unsigned i = 0; i < N_PIECES; ++i) {
            for (ColorType c : colors) {
                changedBits |= oldOcc[i][c] ^ newOcc[i][c];
            }
        }
        if (changedBits == 0) {
            return;
        }
        const Bitboard changed(changedBits);
        const Bitboard oldAll(allPieces(oldOcc)), newAll(allPieces(newOcc));
        const Bitboard oldNonKings(oldAll & ~(oldOcc[King - 1][White] | oldOcc[King - 1][Black]));
        const Bitboard newNonKings(newAll & ~(newOcc[King - 1][White] | newOcc[King - 1][Black]));
        const Bitboard attackers(affectedAttackers(oldOcc, oldAll, changed) |
                                 affectedAttackers(newOcc, newAll, changed));
        for (ColorType c : colors) {
            for (int t = Pawn; t <= Queen; ++t) {
                const PieceType type = static_cast<PieceType>(t);
                const Piece attacker = MakePiece(type, c);
                const Bitboard inOld(oldOcc[t - 1][c] & attackers);
                const Bitboard inNew(newOcc[t - 1][c] & attackers);
                Bitboard squares(inOld | inNew);
                Square from;
                while (squares.iterate(from)) {
                    const Bitboard oldTargets =
                        inOld.isSet(from)
                            ? Bitboard(Attacks::pieceAttacks(type, c, from, oldAll) & oldNonKings)
                            : Bitboard();
                    const Bitboard newTargets =
                        inNew.isSet(from)
                            ? Bitboard(Attacks::pieceAttacks(type, c, from, newAll) & newNonKings)
                            : Bitboard();
                    // a target attacked in both positions, and not on a changed square, has the
                    // same victim in both: its feature is unchanged
                    const Bitboard same(oldTargets & newTargets & ~changed);
                    forEachTarget(oldOcc, Bitboard(oldTargets & ~same),
                                  [&](Piece victim, Square to) {
                                      assert(changes.removedCount < changes.removed.size());
                                      changes.removed[changes.removedCount++] = {
                                          static_cast<uint8_t>(attacker), static_cast<uint8_t>(from),
                                          static_cast<uint8_t>(victim), static_cast<uint8_t>(to)};
                                  });
                    forEachTarget(newOcc, Bitboard(newTargets & ~same),
                                  [&](Piece victim, Square to) {
                                      assert(changes.addedCount < changes.added.size());
                                      changes.added[changes.addedCount++] = {
                                          static_cast<uint8_t>(attacker), static_cast<uint8_t>(from),
                                          static_cast<uint8_t>(victim), static_cast<uint8_t>(to)};
                                  });
                }
            }
        }
    }

    // Get the indices of the features in a list from getThreatChanges, for the point of view of
    // side, whose king is on ksq (used only for its file). The index list is terminated with
    // LAST_INDEX. Returns the number of indices. Features that are not part of the feature set
    // are dropped.
    static size_t toIndices(ColorType side, Square ksq, const ThreatDelta *list, size_t count,
                            ThreatIndexArray &out) {
        // flip piece colors for the Black perspective, so that own pieces always have the White
        // piece values, and orient the squares (see getIndex)
        const unsigned swap = side == Black ? 8 : 0;
        const unsigned flip = FeatureUtils::squareFlipMask(side, ksq);
        size_t n = 0;
        for (size_t i = 0; i < count; ++i) {
            const unsigned a = list[i].attacker ^ swap;
            const unsigned v = list[i].victim ^ swap;
            const Square f = list[i].from ^ flip;
            const Square t = list[i].to ^ flip;
            const IndexType base = ThreatTables::TABLES.lut1[a][v][f < t];
            if (base != LAST_INDEX) {
                assert(n < out.size() - 1);
                out[n++] = base + ThreatTables::TABLES.lut2[(a & 7) - 1][f][t];
            }
        }
        out[n] = LAST_INDEX;
        return n;
    }

    // Get the changes in active features between two positions for the point of view of side:
    // see getThreatChanges and toIndices. The king square ksq is the king square of side in the
    // new position. It is used only for its file, so the old king square must be on the same
    // side of the board (a-d or e-h): otherwise all features change and the caller should use
    // getActiveThreats instead. Both index lists are terminated with LAST_INDEX.
    static void getThreatDeltas(ColorType side, Square ksq, const Occupancies &oldOcc,
                                const Occupancies &newOcc, ThreatIndexArray &added,
                                size_t &addedCount, ThreatIndexArray &removed,
                                size_t &removedCount) {
        ThreatChanges changes;
        getThreatChanges(oldOcc, newOcc, changes);
        addedCount = toIndices(side, ksq, changes.added.data(), changes.addedCount, added);
        removedCount = toIndices(side, ksq, changes.removed.data(), changes.removedCount, removed);
    }

#ifdef SIMD
    // Rows to add and subtract for an accumulator update (see simd::updateRows). The "added"
    // list can be the list from getActiveThreats, to add all features of a position.
    auto rowUpdate(const ThreatIndexArray &added, size_t addedCount,
                   const ThreatIndexArray &removed, size_t removedCount) const noexcept {
        return simd::makeRowUpdate(_weights, added.data(), addedCount, removed.data(),
                                   removedCount);
    }
#else
    // Update half of the accumulator in place: add the weights of the "added" features and
    // subtract those of the "removed" features.
    void updateAccum(AccumulatorType &accum, AccumulatorHalf half, const ThreatIndexArray &added,
                     size_t addedCount, const ThreatIndexArray &removed,
                     size_t removedCount) const noexcept {
        for (size_t i = 0; i < addedCount; ++i) {
            accum.add_half(half, _weights[added[i]]);
        }
        for (size_t i = 0; i < removedCount; ++i) {
            accum.sub_half(half, _weights[removed[i]]);
        }
    }
#endif

    // Prefetch the weight rows for a list of features. The rows are scattered over a large
    // table (tens of MB), so they are usually not in cache.
    void prefetch(const ThreatIndexArray &list, size_t count) const noexcept {
#ifdef SIMD
        for (size_t i = 0; i < count; ++i) {
            simd::prefetch(_weights[list[i]]);
        }
#else
        (void)list;
        (void)count;
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
    static Bitboard allPieces(const Occupancies &occ) {
        uint64_t all = 0;
        for (unsigned i = 0; i < N_PIECES; ++i) {
            all |= occ[i][White] | occ[i][Black];
        }
        return Bitboard(all);
    }

    // Squares of the non-king pieces that are on, or attack, a square in "changed". "all" is
    // the occupancy of the position. A piece attacks a square if and only if a piece of the
    // same type on that square would attack it, so the attackers of each changed square are
    // found by lookups from that square, masked by the pieces that could be attackers (this
    // is much cheaper than computing the attacks of every piece). The attacks are the same
    // for both colors except for pawns.
    static Bitboard affectedAttackers(const Occupancies &occ, const Bitboard &all,
                                      const Bitboard &changed) {
        const Bitboard knights(occ[Knight - 1][White] | occ[Knight - 1][Black]);
        const Bitboard diagonal(occ[Bishop - 1][White] | occ[Bishop - 1][Black] |
                                occ[Queen - 1][White] | occ[Queen - 1][Black]);
        const Bitboard straight(occ[Rook - 1][White] | occ[Rook - 1][Black] |
                                occ[Queen - 1][White] | occ[Queen - 1][Black]);
        // pieces on the changed squares, but not kings
        Bitboard result(all & changed &
                        ~(occ[King - 1][White] | occ[King - 1][Black]));
        Bitboard squares(changed);
        Square sq;
        while (squares.iterate(sq)) {
            // pawn_attacks[sq][c] holds the squares from which a pawn of color c attacks sq
            result |= (Attacks::knight_attacks[sq] & knights) |
                      (Attacks::pawn_attacks[sq][White] & occ[Pawn - 1][White]) |
                      (Attacks::pawn_attacks[sq][Black] & occ[Pawn - 1][Black]) |
                      (Attacks::bishopAttacks(sq, all) & diagonal) |
                      (Attacks::rookAttacks(sq, all) & straight);
        }
        return result;
    }

    // Call out(victim, to) for each piece (victim) on a square in "targets", which must not
    // include kings or empty squares of the position with occupancies "occ".
    template <typename Output>
    static void forEachTarget(const Occupancies &occ, Bitboard targets, Output &&out) {
        for (ColorType vc : colors) {
            for (int vt = Pawn; vt <= Queen && !targets.isClear(); ++vt) {
                Bitboard victims(targets & occ[vt - 1][vc]);
                if (victims.isClear()) {
                    continue;
                }
                targets &= ~victims;
                const Piece victim = MakePiece(static_cast<PieceType>(vt), vc);
                Square to;
                while (victims.iterate(to)) {
                    out(victim, to);
                }
            }
        }
    }

    // Call out(index) for each feature in the position whose attacker is on a square in
    // "attackers". Kings are never attackers or victims.
    template <typename Output>
    static void collect(ColorType side, Square ksq, const Occupancies &occ, const Bitboard &all,
                        const Bitboard &attackers, Output &&out) {
        // all pieces except kings: the possible victims
        const Bitboard nonKings(all &
                                ~(occ[King - 1][White] | occ[King - 1][Black]));
        for (ColorType c : colors) {
            for (int t = Pawn; t <= Queen; ++t) {
                const PieceType type = static_cast<PieceType>(t);
                const Piece attacker = MakePiece(type, c);
                Bitboard pieces(occ[t - 1][c] & attackers);
                Square from;
                while (pieces.iterate(from)) {
                    const Bitboard targets(Attacks::pieceAttacks(type, c, from, all) & nonKings);
                    forEachTarget(occ, targets, [&](Piece victim, Square to) {
                        const IndexType idx = getIndex(side, attacker, from, to, victim, ksq);
                        if (idx != LAST_INDEX) {
                            out(idx);
                        }
                    });
                }
            }
        }
    }

    alignas(alignment) WeightType _weights[DIMENSIONS][outputSize];
};

#endif
