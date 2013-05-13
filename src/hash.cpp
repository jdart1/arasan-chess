// Copyright 1999-2005, 2011, 2012 Jon Dart. All Rights Reserved.

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

HashEntry *Hash::hashTable = NULL;
size_t Hash::hashSize = 0;
size_t Hash::hashFree = 0;
int Hash::refCount = 0;
int Hash::hash_init_done = 0;


void Hash::initHash(size_t bytes)
{
   if (!hash_init_done) {
      hashSize = (int)(3*(bytes/sizeof(HashEntry))/4);
      size_t hashSizePlus = hashSize + MaxRehash;
      ALIGNED_MALLOC(hashTable,
         HashEntry,
         sizeof(HashEntry)*hashSizePlus,128);
      Scoring::initEvalCache(bytes/4);
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
      Scoring::freeEvalCache();
      hash_init_done = 0;
   }
}


void Hash::clearHash()
{
   size_t hashSizePlus = hashSize + MaxRehash;
   hashFree = hashSize;
   memset(hashTable,'\0',sizeof(HashEntry)*hashSizePlus);
   Scoring::clearEvalCache();
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
               PositionInfo::Valid,
               rec.score,
                      PositionInfo::FULL_MASK | PositionInfo::LEARNED_MASK |
                      (rec.in_check ? PositionInfo::CHECK_MASK : 0) |
                      (IsForced(best) ? PositionInfo::FORCED_MASK : 0) |
                      (IsForced2(best) ? PositionInfo::FORCED2_MASK : 0),
               best);
         }
      }
   }
}


