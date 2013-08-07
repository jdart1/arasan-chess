// Copyright 1994, 1996, 2005, 2008 by Jon Dart

#include "bitboard.h"

static int done_init = 0;

const CACHE_ALIGN uint64 Bitboard::mask[64] =
{
   1,
   ((uint64)1) << 1,
   ((uint64)1) << 2,
   ((uint64)1) << 3,
   ((uint64)1) << 4,
   ((uint64)1) << 5,
   ((uint64)1) << 6,
   ((uint64)1) << 7,
   ((uint64)1) << 8,
   ((uint64)1) << 9,
   ((uint64)1) << 10,
   ((uint64)1) << 11,
   ((uint64)1) << 12,
   ((uint64)1) << 13,
   ((uint64)1) << 14,
   ((uint64)1) << 15,
   ((uint64)1) << 16,
   ((uint64)1) << 17,
   ((uint64)1) << 18,
   ((uint64)1) << 19,
   ((uint64)1) << 20,
   ((uint64)1) << 21,
   ((uint64)1) << 22,
   ((uint64)1) << 23,
   ((uint64)1) << 24,
   ((uint64)1) << 25,
   ((uint64)1) << 26,
   ((uint64)1) << 27,
   ((uint64)1) << 28,
   ((uint64)1) << 29,
   ((uint64)1) << 30,
   ((uint64)1) << 31,
   ((uint64)1) << 32,
   ((uint64)1) << 33,
   ((uint64)1) << 34,
   ((uint64)1) << 35,
   ((uint64)1) << 36,
   ((uint64)1) << 37,
   ((uint64)1) << 38,
   ((uint64)1) << 39,
   ((uint64)1) << 40,
   ((uint64)1) << 41,
   ((uint64)1) << 42,
   ((uint64)1) << 43,
   ((uint64)1) << 44,
   ((uint64)1) << 45,
   ((uint64)1) << 46,
   ((uint64)1) << 47,
   ((uint64)1) << 48,
   ((uint64)1) << 49,
   ((uint64)1) << 50,
   ((uint64)1) << 51,
   ((uint64)1) << 52,
   ((uint64)1) << 53,
   ((uint64)1) << 54,
   ((uint64)1) << 55,
   ((uint64)1) << 56,
   ((uint64)1) << 57,
   ((uint64)1) << 58,
   ((uint64)1) << 59,
   ((uint64)1) << 60,
   ((uint64)1) << 61,
   ((uint64)1) << 62,
   ((uint64)1) << 63
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
     int64 bits = (int64)(((uint64)1)<<i);
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
   for (int i = 0; i < 256; i++) {
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


