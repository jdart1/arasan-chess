// Copyright 1999-2005, 2011, 2012, 2013 Jon Dart. All Rights Reserved.

#include "hash.h"
#include "util.h"
#include "debug.h"
#include "constant.h"
#include "globals.h"
#include "legal.h"
#include "learn.h"
#include "scoring.h"
extern "C"
{
#ifndef _MAC
#include <malloc.h>
#endif
#include <memory.h>
#include <stddef.h>
};

Hash::Hash() {
   hashTable = NULL;
   hashSlots = 0;
   hashFree = 0;
   refCount = 0;
   hash_init_done = 0;
}

void Hash::initHash(size_t bytes)
{
   if (!hash_init_done) {
      hashSlots = (int)(bytes/sizeof(HashEntry));
      
      int hashPower;
      // Size hashtable to the nearest power of 2 that does
      // not exceed the specified memory usage in bytes
      for (hashPower = 1; hashPower < 64; hashPower++) {
         if (1 << hashPower > hashSlots) {
            hashPower--;
            break;
         }
      }
      hashSlots = 1 << hashPower;
      hashSlotsPlus = hashSlots+MaxRehash;
      hashSize = sizeof(HashEntry)*hashSlotsPlus;
      hashMask = (uint64)(hashSlots-1);
      ALIGNED_MALLOC(hashTable,
         HashEntry,
         hashSize,128);
      clearHash();
      hash_init_done++;
      ++refCount;
   }
}

void Hash::resizeHash(size_t bytes)
{
   freeHash();
   initHash(bytes);
}


void Hash::freeHash()
{
   if (--refCount == 0) {
      ALIGNED_FREE(hashTable);
      hash_init_done = 0;
   }
}


void Hash::clearHash()
{
   hashFree = hashSlots;
   memset(hashTable,'\0',hashSize);
   if (options.learning.position_learning) {
      loadLearnInfo();
    }
}


void Hash::loadLearnInfo()
{
   if (options.learning.position_learning) {
      ifstream plog;
      plog.open(learnFileName.c_str(),ios_base::in);
      while (plog.good() && !plog.eof()) {
         LearnRecord rec;
         if (getLearnRecord(plog,rec)) {
            Move best = NullMove;
            if (rec.start != InvalidSquare)
               best = CreateMove(rec.start,rec.dest,rec.promotion);
            storeHash(rec.hashcode,rec.depth*DEPTH_INCREMENT,
               0,                                 /* age */
               HashEntry::Valid,
                      rec.score, Scoring::INVALID_SCORE,
                      HashEntry::FULL_MASK | HashEntry::LEARNED_MASK |
                      (rec.in_check ? HashEntry::CHECK_MASK : 0) |
                      (IsForced(best) ? HashEntry::FORCED_MASK : 0) |
                      (IsForced2(best) ? HashEntry::FORCED2_MASK : 0),
               best);
         }
      }
   }
}


