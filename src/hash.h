// Copyright 1992, 1999, 2011-2015, 2017-2019, 2021, 2023, 2025 by Jon Dart.  All Rights Reserved.

#ifndef _HASH_H
#define _HASH_H

#include "board.h"
#include "chess.h"
#include "legal.h"

#include <climits>
#include <cstddef>

extern const hash_t rep_codes[3];

class HashEntry {

  public:
    // Contents of the flag field
    enum { TYPE_MASK = 0x7, TB_MASK = 0x08, LEARNED_MASK = 0x10 };

    // Only the first 4 values are actually stored - Invalid indicates
    // a hash hit with inadequate depth; NoHit indicates failure to find
    // a hash match.
    enum ValueType { Valid, UpperBound, LowerBound, Eval, Invalid, NoHit };

    HashEntry() {
        contents.depth = 0;
        contents.age = 0;
        contents.flags = Eval;
        contents.start = contents.dest = InvalidSquare;
        contents.promotion = Empty;
        contents.start = InvalidSquare;
        contents.value = static_cast<stored_score_t>(Constants::INVALID_SCORE);
        setEffectiveHash(0, Constants::INVALID_SCORE);
    }

    HashEntry(hash_t hash, score_t value, score_t staticValue, int depth, ValueType type,
              unsigned age, uint8_t flags = 0, Move bestMove = NullMove) {
        assert(depth + 2 >= 0 && depth + 2 < 256);
        contents.depth = static_cast<uint8_t>(depth + 2);
        contents.age = static_cast<uint8_t>(age);
        contents.flags = static_cast<uint8_t>(type | flags);
        contents.start = static_cast<uint8_t>(StartSquare(bestMove));
        contents.dest = static_cast<uint8_t>(DestSquare(bestMove));
        contents.promotion = static_cast<uint8_t>(PromoteTo(bestMove));
        contents.value = static_cast<stored_score_t>(value);
        setEffectiveHash(hash, staticValue);
    }

    bool empty() const noexcept { return getEffectiveHash() == 0x0ULL; }

    void clear() { hc = 0x0ULL; }

    int depth() const noexcept { return static_cast<int>(contents.depth) - 2; }

    score_t getValue() const noexcept { return static_cast<score_t>(contents.value); }

    // Get value correcting mate scores for ply
    static score_t hashValueToScore(score_t hashValue, int ply) {
        if (hashValue != Constants::INVALID_SCORE) {
            if (hashValue >= Constants::MATE_RANGE) {
                hashValue -= ply - 1;
            } else if (hashValue <= -Constants::MATE_RANGE) {
                hashValue += ply - 1;
            }
        }
        return hashValue;
    }

    // Get value to store given score
    static score_t scoreToHashValue(score_t score, int ply) {
        if (score == Constants::INVALID_SCORE) {
            return score;
        } else if (score <= -Constants::MATE_RANGE) {
            assert(score - (ply - 1) >= -Constants::MATE);
            return score - (ply - 1);
        } else if (score >= Constants::MATE_RANGE) {
            assert(score + (ply - 1) <= Constants::MATE);
            return score + (ply - 1);
        } else {
            return score;
        }
    }

    score_t staticValue() const noexcept {
        // assumes 2's complement machine
        unsigned bits = static_cast<unsigned>(hc & STATIC_VALUE_MASK);
        if (bits & 0x8000)
            return static_cast<score_t>(-32768 + (bits & 0x7fff));
        else
            return static_cast<score_t>(bits & 0x7fff);
    }

    void setValue(score_t score) { contents.value = static_cast<stored_score_t>(score); }

    ValueType type() const noexcept { return static_cast<ValueType>(contents.flags & TYPE_MASK); }

    unsigned age() const noexcept { return static_cast<unsigned>(contents.age); }

    void setAge(unsigned age) { contents.age = static_cast<uint8_t>(age); }

    bool learned() const noexcept { return ((contents.flags & LEARNED_MASK) != 0); }

    bool tb() const noexcept { return ((contents.flags & TB_MASK) != 0); }

    uint8_t flags() const noexcept { return contents.flags; }

    Move bestMove(const Board &b) const noexcept {
        Move m =
            CreateMove(b, static_cast<Square>(contents.start), static_cast<Square>(contents.dest),
                       static_cast<PieceType>(contents.promotion));
        return validMove(b, m) ? m : NullMove;
    }

    bool avoidNull(int null_depth, score_t beta) const noexcept {
        return type() == UpperBound && depth() >= null_depth && getValue() < beta;
    }

    bool operator==(const hash_t hash) const noexcept {
        return getEffectiveHash() == (hash & HASH_MASK);
    }

    bool operator!=(const hash_t hash) const noexcept {
        return getEffectiveHash() != (hash & HASH_MASK);
    }

    hash_t getEffectiveHash() const noexcept { return (hc ^ val) & HASH_MASK; }

    void setEffectiveHash(hash_t hash, score_t static_score) {
        hc = ((hash ^ val) & HASH_MASK) | static_cast<uint16_t>(static_score);
    }

  protected:
    static constexpr hash_t HASH_MASK = 0xffffffffffff0000;
    static constexpr hash_t STATIC_VALUE_MASK = ~HASH_MASK;

#ifdef __INTEL_COMPILER
#pragma pack(push, 1)
#endif
    struct Contents BEGIN_PACKED_STRUCT stored_score_t value;
    uint8_t depth;
    uint8_t age;
    uint8_t flags;
    signed char start, dest, promotion;
    END_PACKED_STRUCT

#ifdef __INTEL_COMPILER
#pragma pack(pop)
#endif
    union {
        Contents contents;
        uint64_t val;
    };

    static_assert(sizeof(contents) == sizeof(val));

    // static value is packed in the low-order hash bits
    uint64_t hc;
};

class Hash {

    friend class Scoring;

  public:
    Hash();

    void initHash(size_t bytes);

    void resizeHash(size_t bytes);

    void freeHash();

    void clearHash();

    // put info from the external permanent hash table into the
    // in-memory hash table
    void loadLearnInfo();

    HashEntry::ValueType searchHash(hash_t hashCode, int depth, unsigned age, HashEntry &he) {
        if (!hashSize)
            return HashEntry::NoHit;
        int probe = static_cast<int>(hashCode & hashMask);

        HashEntry *p = &hashTable[probe];
        HashEntry *hit = nullptr;
        for (int i = MaxRehash; i != 0; --i, p++) {
            // Copy hashtable entry before hash test below (avoids
            // race where entry is validated, then changed).
            HashEntry entry(*p);
            if (entry == hashCode) {
                // we got a hit on this entry in the current search,
                // so update the age to discourage replacement:
                if (entry.age() && (entry.age() != age)) {
                    entry.setAge(age);
                    entry.setEffectiveHash(hashCode, entry.staticValue());
                    *p = entry;
                }
                hit = p;
                he = entry;
                if (entry.depth() >= depth) {
                    // usable depth
                    return he.type();
                } else {
                    break;
                }
            }
        }
        if (hit) {
            // hash hit, but with insufficient depth:
            return HashEntry::Invalid;
        } else {
            return HashEntry::NoHit;
        }
    }

    void storeHash(hash_t hashCode, const int depth, unsigned age, HashEntry::ValueType type,
                   score_t value, score_t staticValue, unsigned flags, Move best_move) {

        if (!hashSize)
            return;
        int probe = static_cast<int>(hashCode & hashMask);
        HashEntry *p = &hashTable[probe];

        HashEntry *best = nullptr;
        assert(value >= SHRT_MIN && value <= SHRT_MAX);
        // Of the positions that hash to the same locations
        // as this one, find the best one to replace.
        score_t maxScore = static_cast<score_t>(-Constants::MaxPly * DEPTH_INCREMENT);
        for (int i = MaxRehash; i != 0; --i) {
            HashEntry &q = *p;

            if (q.empty()) {
                // empty hash entry, available for use
                best = &q;
                break;
            } else if (q == hashCode) {
                // same position, always replace
                best = &q;
                break;
            } else if (!(q.flags() & (HashEntry::LEARNED_MASK | HashEntry::TB_MASK)) &&
                       (q.depth() <= depth || q.age() != age)) {
                // candidate for replacement
                score_t score = replaceScore(q, age);
                if (score > maxScore) {
                    maxScore = score;
                    best = &q;
                }
            }
            p++;
        }
        if (best != nullptr) {
            if (best->empty()) {
                hashFree--;
            }
            *best = HashEntry(hashCode, value, staticValue, depth, type, age,
                              static_cast<uint8_t>(flags), best_move);
        }
    }

    size_t getHashSize() const { return hashSize; }

    // Percent full (percentage x 10)
    int pctFull() const noexcept {
        if (hashSize == 0)
            return 0;
        else
            return static_cast<int>((hashSize - hashFree) * 1000.0 / hashSize);
    }

  private:
    score_t replaceScore(const HashEntry &pos, int age) const {
        return static_cast<score_t>((std::abs((int)pos.age() - (int)age) << 12) - pos.depth());
    }

    HashEntry *hashTable;
    size_t hashSize, hashFree;
    hash_t hashMask;
    static constexpr int MaxRehash = 4;
    int hash_init_done;
};

#endif
