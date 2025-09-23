// Copyright 1994, 1996, 2005, 2008, 2013, 2016, 2019, 2021, 2023, 2025 by Jon Dart

#include "bitutil.h"

static int done_init = 0;

int BitUtils::MagicTable32[32];
#if defined(_64BIT)
int BitUtils::MagicTable64[64];
#endif
int BitUtils::msbTable[256];

void BitUtils::init()
{
   int i;
#if defined(_64BIT)
   for (i=0;i<64;i++) {
     int64_t bits = (int64_t)(((uint64_t)1)<<i);
     int index = (int)((GETBIT64(bits)*MAGIC64)>>58);
     MagicTable64[index] = i;
   }
#else
   for (i=0;i<32;i++) {
     unsigned bits = (((unsigned)1)<<i);
     int index = (int)((GETBIT32(bits)*MAGIC32)>>27);
     MagicTable32[index] = i;
   }
#endif
   for (i = 0; i < 256; i++) {
     int msb = 7;
     msbTable[i] = 0;
     for (int m = 128; m > 0; m >>= 1, msb--) {
       if (i & m) {
         msbTable[i] = msb;
         break;
       }
     }
   }
   done_init++;
}

void BitUtils::cleanup()
{
}
