// Copyright 1994, 1996, 2005, 2008, 2013, 2016, 2019 by Jon Dart

#include "bitboard.h"

static int done_init = 0;

const CACHE_ALIGN uint64_t Bitboard::mask[64] =
{
   1,
   ((uint64_t)1) << 1,
   ((uint64_t)1) << 2,
   ((uint64_t)1) << 3,
   ((uint64_t)1) << 4,
   ((uint64_t)1) << 5,
   ((uint64_t)1) << 6,
   ((uint64_t)1) << 7,
   ((uint64_t)1) << 8,
   ((uint64_t)1) << 9,
   ((uint64_t)1) << 10,
   ((uint64_t)1) << 11,
   ((uint64_t)1) << 12,
   ((uint64_t)1) << 13,
   ((uint64_t)1) << 14,
   ((uint64_t)1) << 15,
   ((uint64_t)1) << 16,
   ((uint64_t)1) << 17,
   ((uint64_t)1) << 18,
   ((uint64_t)1) << 19,
   ((uint64_t)1) << 20,
   ((uint64_t)1) << 21,
   ((uint64_t)1) << 22,
   ((uint64_t)1) << 23,
   ((uint64_t)1) << 24,
   ((uint64_t)1) << 25,
   ((uint64_t)1) << 26,
   ((uint64_t)1) << 27,
   ((uint64_t)1) << 28,
   ((uint64_t)1) << 29,
   ((uint64_t)1) << 30,
   ((uint64_t)1) << 31,
   ((uint64_t)1) << 32,
   ((uint64_t)1) << 33,
   ((uint64_t)1) << 34,
   ((uint64_t)1) << 35,
   ((uint64_t)1) << 36,
   ((uint64_t)1) << 37,
   ((uint64_t)1) << 38,
   ((uint64_t)1) << 39,
   ((uint64_t)1) << 40,
   ((uint64_t)1) << 41,
   ((uint64_t)1) << 42,
   ((uint64_t)1) << 43,
   ((uint64_t)1) << 44,
   ((uint64_t)1) << 45,
   ((uint64_t)1) << 46,
   ((uint64_t)1) << 47,
   ((uint64_t)1) << 48,
   ((uint64_t)1) << 49,
   ((uint64_t)1) << 50,
   ((uint64_t)1) << 51,
   ((uint64_t)1) << 52,
   ((uint64_t)1) << 53,
   ((uint64_t)1) << 54,
   ((uint64_t)1) << 55,
   ((uint64_t)1) << 56,
   ((uint64_t)1) << 57,
   ((uint64_t)1) << 58,
   ((uint64_t)1) << 59,
   ((uint64_t)1) << 60,
   ((uint64_t)1) << 61,
   ((uint64_t)1) << 62,
   ((uint64_t)1) << 63
};

int Bitboard::MagicTable32[32];
#if defined(_64BIT)
int Bitboard::MagicTable64[64];
#endif
int Bitboard::msbTable[256];

void Bitboard::init()
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
     int mask = 128;
     int msb = 7;
     msbTable[i] = 0;
     while (mask > 0) {
       if (i & mask) {
         msbTable[i] = msb;
         break;
       }
       mask >>=1; msb--;
     }
   }
   done_init++;
}

void Bitboard::cleanup()
{
}
                                   
ostream & operator << (ostream &o, const Bitboard &b)
{
   o << (hex) << b.data << (dec);
   return o;
}


