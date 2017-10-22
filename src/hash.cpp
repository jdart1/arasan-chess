// Copyright 1999-2005, 2011, 2012, 2014 Jon Dart. All Rights Reserved.

#include "hash.h"
#include "debug.h"
#include "constant.h"
#include "globals.h"
#include "legal.h"
#include "learn.h"
#include "scoring.h"
extern "C"
{
#if !defined(_MAC) && !defined(__clang__) && !defined(__FreeBSD__)
#include <malloc.h>
#endif
#include <memory.h>
#include <stddef.h>
};

Hash::Hash() {
   hashTable = NULL;
   hashSize = 0;
   hashMask = 0x0ULL;
   hashFree = 0;
   hash_init_done = 0;
}

void Hash::initHash(size_t bytes)
{
   if (!hash_init_done) {
      hashSize = (int)(bytes/sizeof(HashEntry));
      if (!hashSize) {
         hashMask = 0;
         hash_init_done++;
         return;
      }
      int hashPower;
      for (hashPower = 1; hashPower < 32; hashPower++) {
        if (((size_t)1 << hashPower) > hashSize) {
            hashPower--;
            break;
        }
      }
      hashSize = (size_t)1 << hashPower;
      hashMask = (uint64_t)(hashSize-1);
      size_t hashSizePlus = hashSize + MaxRehash;
      ALIGNED_MALLOC(hashTable,
         HashEntry,
         sizeof(HashEntry)*hashSizePlus,128);
      if (hashTable == NULL) {
          cerr << "hash table allocation failed!" << endl;
          hashSize = 0;
      }
      clearHash();
      hash_init_done++;
   }
}

void Hash::resizeHash(size_t bytes)
{
   freeHash();
   initHash(bytes);
}


void Hash::freeHash()
{
   ALIGNED_FREE(hashTable);
   hash_init_done = 0;
}


void Hash::clearHash()
{
   if (hashSize == 0) return;
   size_t hashSizePlus = hashSize + MaxRehash;
   hashFree = hashSize;
   memset(hashTable,'\0',hashSizePlus*sizeof(HashEntry));
   if (options.learning.position_learning) {
      loadLearnInfo();
    }
}


void Hash::loadLearnInfo()
{
   if (hashSize && options.learning.position_learning) {
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
               rec.score,
                      Scoring::INVALID_SCORE, // TBD
                      HashEntry::LEARNED_MASK |
                      (IsForced(best) ? HashEntry::FORCED_MASK : 0),
               best);
         }
      }
   }
}


