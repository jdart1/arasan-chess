// Copyright 1992, 1999, 2011-2015, 2017 by Jon Dart.  All Rights Reserved.

#ifndef _HASH_H
#define _HASH_H

#include "chess.h"
#include "board.h"
#ifdef _DEBUG
#include "legal.h"
#endif
#include <cstddef>

extern const hash_t rep_codes[3];

class HashEntry {

	public:

      // Contents of the flag field
      enum {
         TYPE_MASK = 0x7,
         TB_MASK = 0x08,
         LEARNED_MASK = 0x10,
         FORCED_MASK = 0x20
      };

      static const int QSEARCH_CHECK_DEPTH = -1;
      static const int QSEARCH_NO_CHECK_DEPTH = -2;

      // Only the first 4 values are actually stored - Invalid indicates
      // a hash hit with inadequate depth; NoHit indicates failure to find
      // a hash match.
      enum ValueType { Valid, UpperBound, LowerBound, Eval, Invalid, NoHit };

      HashEntry() {
      }

      HashEntry(hash_t hash, score_t val, score_t staticValue, int depth,
         ValueType type, int age, int flags = 0,
                Move bestMove = NullMove) {
         ASSERT(depth+2 >= 0 && depth+2 < 256);
         contents.depth = (byte)(depth+2);
         contents.age = age;
         contents.flags = type | flags;
         contents.start = StartSquare(bestMove);
         contents.dest = DestSquare(bestMove);
         contents.promotion = PromoteTo(bestMove);
         values.value = stored_score_t(val);
         values.static_value = stored_score_t(staticValue);
         setEffectiveHash(hash);
      }

      int empty() const {
         return hc == 0x0ULL;
      }

      void clear() {
        hc = 0x0ULL;
      }

      int depth() const {
          return (int)contents.depth - 2;
      }

      score_t getValue() const {
         return score_t(values.value);
      }

      score_t staticValue() const {
         return (score_t)values.static_value;
      }

      void setValue(score_t val) {
         values.value = (stored_score_t)val;
      }

      ValueType type() const {
         return (ValueType)(contents.flags & TYPE_MASK);
      }

      int age() const {
         return (int)contents.age;
      }

      void setAge(int age) {
         contents.age = age;
      }

      int forced() const {
         return (int)((contents.flags & FORCED_MASK) != 0);
      }

      int learned() const {
         return (int)((contents.flags & LEARNED_MASK) != 0);
      }

      int tb() const {
         return (int)((contents.flags & TB_MASK) != 0);
      }

      byte flags() const {
          return contents.flags;
      }

      Move bestMove(const Board &b) const {
         if (contents.start == InvalidSquare)
            return NullMove;
         else {
            Move m = CreateMove(b,(Square)contents.start,(Square)contents.dest,
               (PieceType)contents.promotion);
            //if (!validMove(b,m)) return NullMove;
            if (forced()) SetForced(m);
            return m;
         }
      }

      bool avoidNull(int null_depth, score_t beta) const {
          return type() == UpperBound && depth() >= null_depth &&
              getValue() < beta;
      }

      int operator == (const hash_t hash) const {
         return getEffectiveHash() == hash;
      }

      int operator != (const hash_t hash) const {
         return getEffectiveHash() != hash;
      }

      hash_t getEffectiveHash() const {
         return hc ^ val2 ^ val3;
      }

      void setEffectiveHash(hash_t hash) {
         hc = hash ^ val2 ^ val3;
      }

   protected:

#ifdef __INTEL_COMPILER
#pragma pack(push,1)
#endif
      struct Contents
      BEGIN_PACKED_STRUCT
        int16_t pad;
        byte depth;
        byte age;
        byte flags;
        signed char start, dest, promotion;
      END_PACKED_STRUCT

      struct Values
      BEGIN_PACKED_STRUCT
        stored_score_t value;
        stored_score_t static_value;
      END_PACKED_STRUCT
#ifdef __INTEL_COMPILER
#pragma pack(pop)
#endif
       uint64_t hc;
      union
      {
        Contents contents;
        uint64_t val2;
      };

      union {
        Values values;
        uint64_t val3;
      };
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

    HashEntry::ValueType searchHash(const Board& b,hash_t hashCode,
                                              int ply,
                                              int depth, int age,
                                              HashEntry &he
                                              ) {
        if (!hashSize) return HashEntry::NoHit;
        int probe = (int)(hashCode & hashMask);
        HashEntry *p = &hashTable[probe];
        HashEntry *hit = NULL;
        for (int i = MaxRehash; i != 0; --i) {
            // Copy hashtable entry before hash test below (avoids
            // race where entry is validated, then changed).
            HashEntry entry(*p);
            if (entry == hashCode) {
                // we got a hit on this entry in the current search,
                // so update the age to discourage replacement:
                if (entry.age() && (entry.age() != age)) {
                   entry.setAge(age);
                   entry.setEffectiveHash(hashCode);
                   *p = entry;
                }
                hit = p;
                he = entry;
                if (entry.depth() >= depth) {
                    // usable depth
                    return he.type();
                }
                else {
                    break;
                }
            }
            p++;
        }
        if (hit) {
          // hash hit, but with insufficient depth:
          return HashEntry::Invalid;
        } else {
          return HashEntry::NoHit;
        }
    }

    void storeHash(hash_t hashCode, const int depth,
                          int age,
                          HashEntry::ValueType type,
                          score_t value,
                          score_t staticValue,
                          byte flags,
                          Move best_move) {

        if (!hashSize) return;
        int probe = (int)(hashCode & hashMask);
        HashEntry *p = &hashTable[probe];

        HashEntry *best = NULL;
        ASSERT(value >= -Constants::MATE && value <= Constants::MATE);
        // Of the positions that hash to the same locations
        // as this one, find the best one to replace.
        score_t maxScore = score_t(-Constants::MaxPly*DEPTH_INCREMENT);
        for (int i = MaxRehash; i != 0; --i) {
            HashEntry &q = *p;

            if (q.empty()) {
                // empty hash entry, available for use
                best = &q;
                break;
            }
            else if (q == hashCode) {
                // same position, always replace
                best = &q;
                break;
            }
            else if (!(q.flags() & 
                       (HashEntry::LEARNED_MASK | HashEntry::TB_MASK)) && 
                     (q.depth() <= depth || q.age() != age)) {
                // candidate for replacement
                score_t score = replaceScore(q,age);
                if (score > maxScore) {
                    maxScore = score;
                    best = &q;
                }
            }
            p++;
        }
        if (best != NULL) {
            if (best->empty()) {
               hashFree--;
            }
            *best = HashEntry(hashCode, value, 
                              staticValue, depth, type, age, flags, best_move);
       }
    }

    size_t getHashSize() const {
        return hashSize;
    }

    // Percent full (percentage x 10)
    int pctFull() const {
        if (hashSize == 0)
            return 0;
        else
            return (int)(1000.0*(hashSize-hashFree)/(double)hashSize);
    }

private:
    score_t replaceScore(const HashEntry &pos, int age) const {
        return score_t((std::abs(pos.age()-age)<<12) - pos.depth());
    }

    HashEntry *hashTable;
    size_t hashSize, hashFree;
    hash_t hashMask;
    static const int MaxRehash = 4;
    int hash_init_done;
};

#endif
