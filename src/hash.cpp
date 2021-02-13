// Copyright 1999-2005, 2011, 2012, 2014-2017, 2020 Jon Dart. All Rights Reserved.

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
}

Hash::Hash() {
   hashTable = nullptr;
   hashSize = 0;
   hashMask = 0x0ULL;
   hashFree = 0;
   hash_init_done = 0;
}

void Hash::initHash(size_t bytes)
{
   if (!hash_init_done) {
      hashSize = bytes/sizeof(HashEntry);
      if (!hashSize) {
         hashMask = 0;
         hash_init_done++;
         return;
      }
      unsigned hashPower;
      for (hashPower = 1; hashPower < 64; hashPower++) {
          if ((1ULL << hashPower) > hashSize) {
              hashPower--;
              break;
          }
      }
      hashSize = 1ULL << hashPower;
      hashMask = (uint64_t)(hashSize-1);
      size_t hashSizePlus = hashSize + MaxRehash;
      ALIGNED_MALLOC(hashTable,
         HashEntry,
         sizeof(HashEntry)*hashSizePlus,128);
      if (hashTable == nullptr) {
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
   hashFree = hashSize;
   HashEntry empty;
   for (size_t i = 0; i < hashSize; i++) {
       hashTable[i] = empty;
   }
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
                      Constants::INVALID_SCORE, // TBD
                      HashEntry::LEARNED_MASK,
                      best);
         }
      }
   }
}


