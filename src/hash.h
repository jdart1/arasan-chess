// Copyright 1992, 1999, 2011-2013 by Jon Dart.  All Rights Reserved.

#ifndef _HASH_H
#define _HASH_H

#include "chess.h"
#include "board.h"
#ifdef _DEBUG
#include "legal.h"
#endif

extern const hash_t rep_codes[3];

class HashEntry;

class PositionInfo
{
   public:

      // Contents of the flag field
      enum {
         TYPE_MASK = 0x3,
         TB_MASK = 0x04,
         LEARNED_MASK = 0x08,
         FULL_MASK = 0x20,
         FORCED_MASK = 0x40,
         FORCED2_MASK = 0x80
      };

      // Only the first 3 values are actually stored - Invalid indicates
      // a hash hit with inadequate depth; NoHit indicates failure to find
      // a hash match.
      enum ValueType { Valid, UpperBound, LowerBound, Invalid, NoHit };

      PositionInfo() {
      }

      PositionInfo(int value, int depth,
         ValueType type, int age, int flags = 0,
         Move bestMove = NullMove) {
         contents.value = value;
         contents.depth = depth;
         contents.age = age;
         contents.flags = type | flags;
         contents.start = StartSquare(bestMove);
         contents.dest = DestSquare(bestMove);
         contents.promotion = PromoteTo(bestMove);
      }

      int depth() const {
         return contents.depth;
      }

      int value() const {
         return (int)contents.value;
      }

      void setValue(int value) {
         contents.value = (int16)value;
      }

      ValueType type() const
      {
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

      int isFull() const {
         return (int)((contents.flags & FULL_MASK) != 0);
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

      void setFull(int b) {
         if (b) contents.flags |= FULL_MASK;
         else contents.flags &= ~FULL_MASK;
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

   protected:
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
         uint64 val;
      }
#ifndef _MSC_VER
      __attribute__((packed))
#endif
      ;

};

class HashEntry : public PositionInfo
{

   // this class represents one entry in the hash table.
   public:

      HashEntry( hash_t hash, const int depth,
         int age,
         PositionInfo::ValueType type,
         int value,
         byte flags,
         Move best_move)
         : PositionInfo(value, depth, type, age, flags,
                        best_move) {
         hc = val ^ hash;
      }

      hash_t hashCode() const {
         return hc;
      }

      hash_t getEffectiveHash() const {
         return val ^ hc;
      }

      void setEffectiveHash(hash_t hash) {
         hc = hash ^ val;
      }

   protected:
      hash_t hc;
};

unsigned long hash_code(const HashEntry &p);

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

    PositionInfo::ValueType searchHash(const Board& b,hash_t hashCode,
                                              int alpha, int beta, int ply,
                                              int depth, int age,
                                              PositionInfo &pi
                                              ) {
        int i;
        if (!hashSize) return PositionInfo::NoHit;
        int probe = (int)(hashCode % hashSize);
        HashEntry *p = &hashTable[probe];
        HashEntry *hit = NULL;
        for (i = MaxRehash; i != 0; --i) {
            // Copy hashtable entry before hash test below (avoids
            // race where entry is validated, then changed).
            HashEntry entry(*p);
            if (entry.getEffectiveHash() == hashCode) {
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
                pi = entry;
                if (entry.depth() >= depth) {
                    // usable depth
                    goto hash_hit;
                }
                else {
                    break;
                }
            }
            p++;
        }
        // If we got a hash hit but insufficient depth return here:
        if (hit) return PositionInfo::Invalid;
        return PositionInfo::NoHit;
    hash_hit:
        return pi.type();
    }

    void storeHash(hash_t hashCode, const int depth,
                          int age,
                          PositionInfo::ValueType type,
                          int value,
                          byte flags,
                          Move best_move) {

        if (!hashSize) return;
        int probe = (int)(hashCode % hashSize);
        HashEntry *p = &hashTable[probe];

        HashEntry *best = NULL;
        ASSERT(Util::Abs(value) <= Constants::MATE);
        // Of the positions that hash to the same locations
        // as this one, find the best one to replace.
        int maxScore = -Constants::MaxPly*DEPTH_INCREMENT;
        for (int i = MaxRehash; i != 0; --i) {
            HashEntry &q = *p;

            if (q.hashCode() == (hash_t)0) {
                // empty hash entry, available for use
                best = &q;
                break;
            }
            else if (q.getEffectiveHash() == hashCode) {
                // same position, always replace
                best = &q;
                break;
            }
            else if (!(q.flags() & 
                       (PositionInfo::LEARNED_MASK | PositionInfo::TB_MASK)) && 
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
            if (best->hashCode() == (hash_t)0x0ULL) {
               hashFree--;
            }
            HashEntry newPos(hashCode, depth, age, type, value, flags, best_move);
            *best = newPos;
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

    // Evaluation cache, used in qsearch
    struct CACHE_ALIGN EvalCacheEntry {
         hash_t score_key, move_key;
         int score;
         Move best;
    };

    void initEvalCache(size_t bytes);

    void freeEvalCache();

    void clearEvalCache();

    inline Move getBestMove(const Board &board) {
        const EvalCacheEntry &entry = evalCache[board.hashCode() & evalCacheMask];
        hash_t key = entry.move_key;
        Move best = entry.best;
        key ^= (hash_t)best;
        if (board.hashCode() == key) {
            return best;
        }
        else
            return NullMove;
    }

    inline void cacheBestMove(const Board &board, Move best) {
        EvalCacheEntry &entry = evalCache[board.hashCode() & evalCacheMask];
        entry.move_key = board.hashCode() ^ (hash_t)best;
        entry.best = best;
    }

 private:
    int replaceScore(const HashEntry &pos, int age) {
        return (Util::Abs(pos.age()-age)<<12) - pos.depth();
    }

    HashEntry *hashTable;
    size_t hashSize, hashFree;
    static const int MaxRehash = 4;
    int refCount;
    int hash_init_done;
    EvalCacheEntry *evalCache;
    int evalCacheSize;
    uint64 evalCacheMask;

};

#endif
