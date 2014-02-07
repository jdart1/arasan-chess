// Copyright 1992, 1999, 2011-2014 by Jon Dart.  All Rights Reserved.

#ifndef _HASH_H
#define _HASH_H

#include "chess.h"
#include "board.h"
#ifdef _DEBUG
#include "legal.h"
#endif

extern const hash_t rep_codes[3];

class HashEntry {

   public:

      // Contents of the flag field
      enum {
         TYPE_MASK = 0x7,
         TB_MASK = 0x08,
         LEARNED_MASK = 0x10,
         FORCED_MASK = 0x20,
         FORCED2_MASK = 0x40
      };

      static const hash_t HASH_MASK = 0xffffffffffff0000ULL;
      static const hash_t STATIC_VALUE_MASK = 0x000000000000ffffULL;

      static const int QSEARCH_CHECK_DEPTH = -1;
      static const int QSEARCH_NO_CHECK_DEPTH = -2;

      // Only the first 4 values are actually stored - Invalid indicates
      // a hash hit with inadequate depth; NoHit indicates failure to find
      // a hash match.
      enum ValueType { Valid, UpperBound, LowerBound, Eval, Invalid, NoHit };

      HashEntry() {
      }

      HashEntry(hash_t hash, int value, int staticValue, int depth,
         ValueType type, int age, int flags = 0,
         Move bestMove = NullMove) {
         contents.value = value;
         ASSERT(depth+2 >= 0 && depth+2 < 256);
         contents.depth = (byte)(depth+2);
         contents.age = age;
         contents.flags = type | flags;
         contents.start = StartSquare(bestMove);
         contents.dest = DestSquare(bestMove);
         contents.promotion = PromoteTo(bestMove);
         // Low order bits of val1 encode static value.
         // Make non-negative:
         val1 = (unsigned)(staticValue+Constants::MATE+1);
         setEffectiveHash(hash);
      }

      int empty() const {
         return val1 == 0x0ULL;
      }

      int depth() const {
          return (int)contents.depth - 2;
      }

      int value() const {
         return (int)contents.value;
      }

      int staticValue() const {
          return (int)(val1 & STATIC_VALUE_MASK) - Constants::MATE - 1;
      }

      void setValue(int value) {
         contents.value = (int16)value;
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

      int forced2() const {
         return (int)((contents.flags & FORCED2_MASK) != 0);
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
            if (forced2()) SetForced2(m);
            return m;
         }
      }

      bool avoidNull(int null_depth, int beta) const {
          return type() == UpperBound && depth() >= null_depth &&
              value() < beta;
      }

      int operator == (const hash_t hash) const {
          return getEffectiveHash() == (hash & HASH_MASK);
      }

      int operator != (const hash_t hash) const {
          return getEffectiveHash() != (hash & HASH_MASK);
      }

   protected:

      hash_t getEffectiveHash() const {
         return (val1 ^ val2) & HASH_MASK;
      }

      void setEffectiveHash(hash_t hash) {
         val1 &= ~HASH_MASK;
         val1 |= (hash ^ val2) & HASH_MASK;
	 ASSERT(*this == hash);
      }

      uint64 val1;
      union
      {
#ifdef _MSC_VER
#pragma pack(push,1)
#endif
         struct
         {
            int16 value;
            byte depth;
            byte age;
            byte flags;
            signed char start, dest, promotion;
         } contents;
#ifdef _MSC_VER
#pragma pack(pop)
#endif
         uint64 val2;
      }
#ifndef _MSC_VER
      __attribute__((packed))
#endif
      ;
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
        int i;
        if (!hashSize) return HashEntry::NoHit;
        int probe = (int)(hashCode & hashMask);
        HashEntry *p = &hashTable[probe];
        HashEntry *hit = NULL;
        for (i = MaxRehash; i != 0; --i) {
            // Copy hashtable entry before hash test below (avoids
            // race where entry is validated, then changed).
            HashEntry entry(*p);
            if (entry == hashCode) {
                /*
                // we got a hit on this entry in the current search,
                // so update the age to discourage replacement:
                if (entry.age() && (entry.age() != age)) {
                   entry.setAge(age);
                   entry.setEffectiveHash(hashCode);
                   *p = entry;
                }
                */
                hit = &entry;
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
                          int value,
                          int staticValue,
                          byte flags,
                          Move best_move) {

        if (!hashSize) return;
        int probe = (int)(hashCode & hashMask);
        HashEntry *p = &hashTable[probe];

        HashEntry *best = NULL;
        ASSERT(Util::Abs(value) <= Constants::MATE);
        // Of the positions that hash to the same locations
        // as this one, find the best one to replace.
        int maxScore = -Constants::MaxPly*DEPTH_INCREMENT;
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
                int score = replaceScore(q,age);
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
    int replaceScore(const HashEntry &pos, int age) {
        return (Util::Abs(pos.age()-age)<<12) - pos.depth();
    }

    HashEntry *hashTable;
    size_t hashSize, hashFree;
    hash_t hashMask;
    static const int MaxRehash = 4;
    int refCount;
    int hash_init_done;
};

#endif
