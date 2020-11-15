// Copyright 1996-2008, 2010-2013, 2015-2020 by Jon Dart.  All Rights Reserved.
#ifndef _BITBOARD_H
#define _BITBOARD_H

#include "types.h"
#include <iostream>
#ifdef __cpp_lib_bitops
#include <bit>
#endif
#if defined (USE_INTRINSICS) && defined(_WIN32) && !defined(__MINGW32__)
#include <intrin.h>
#if defined(USE_POPCNT) && (_MSC_VER >= 1500) && defined(_64BIT)
#include <nmmintrin.h>
#endif
#endif
#if defined(USE_ASM) && defined(__x86_64__)
#include <string.h>
// Inline ASM
static FORCEINLINE unsigned int _bitScanForwardAsm(const uint64_t bits)
{
   uint64_t ret;
   __asm__ (
            "bsfq %[bits], %[ret]"
            :[ret] "=r" (ret)
            :[bits] "mr" (bits)
	   );
   return (unsigned int)ret;
}

static FORCEINLINE unsigned int _bitScanReverseAsm(const uint64_t bits)
{
   uint64_t ret;
   __asm__ (
            "bsrq %[bits], %[ret]"
            :[ret] "=r" (ret)
            :[bits] "mr" (bits)
	   );
   return (unsigned int)ret;
}

#endif

class Bitboard
{
       friend ostream & operator << (ostream &o, const Bitboard &b);

#define GETBIT64(x) (uint64_t)(((int64_t)x)&-((int64_t)x))
#define GETBIT32(x) (uint32_t)(((int32_t)x)&-((int32_t)x))

    public:

       struct ints
       {
#if _BYTE_ORDER == _BIG_ENDIAN
          uint32_t hival,loval;
#else
          uint32_t loval,hival;
#endif
       };
       struct shorts
       {
#if _BYTE_ORDER == _BIG_ENDIAN
          uint16_t hi2,hi1,lo2,lo1;
#else
          uint16_t lo1,lo2,hi1,hi2;
#endif
       };
       struct bytes
       {
#if _BYTE_ORDER == _BIG_ENDIAN
          uint8_t b8,b7,b6,b5,b4,b3,b2,b1;
#else
          uint8_t b1,b2,b3,b4,b5,b6,b7,b8;
#endif
       };
       union conv
       {
          uint64_t val1;
          ints val2;
          shorts val3;
          bytes val4;
       };

    Bitboard()
    : data(0)
    {
    }

    Bitboard(uint64_t n)
    : data(n)
    {
    }

    Bitboard(uint32_t hi,uint32_t lo) {
      ((conv*)&data)->val2.hival = hi;
      ((conv*)&data)->val2.loval = lo;
    }

    Bitboard(const Bitboard &) = default;

    ~Bitboard() = default;

    operator uint64_t() const
    {
       return data;
    }

    uint8_t uint8_t_value() const
    {
       return ((conv*)&data)->val4.b1;
    }

    uint32_t FORCEINLINE hivalue() const
    {
      return ((conv*)(this))->val2.hival;
    }

    uint32_t FORCEINLINE lovalue() const
    {
      return ((conv*)(this))->val2.loval;
    }

    void clear()
    {
       data = 0;
    }

    void set(int n)
    {
#if defined(_WIN64) && defined(_MSC_VER) && defined(USE_INTRINSICS)
       _bittestandset64((LONG64*)&data,(DWORD)n);
#elif defined(_64BIT)
       data |= ((uint64_t)1)<<n;
#else
       data |= mask[n];
#endif
    }

    void FORCEINLINE clear(int n)
    {
#if defined(_WIN64) && defined(_MSC_VER) && defined(USE_INTRINSICS)
       _bittestandreset64((LONG64*)&data,(DWORD)n);
#elif defined(_64BIT)
       data &= ~(((uint64_t)1)<<n);
#else
       data &= ~mask[n];
#endif
    }

    void setClear(const Bitboard &b)
    {
        data ^= b.data;
    }

    int FORCEINLINE isSet(int n) const
    {
#if defined(_WIN64) & defined(USE_INTRINSICS)
        return _bittest64((int64_t*)&data,n);
#elif defined(_64BIT)
        return (data & (((uint64_t)1)<<n)) != (uint64_t)0;
#else
        return ((data & mask[n]) != (uint64_t)0);
#endif
    }

    int FORCEINLINE isClear()const {
        return (data == (uint64_t)0);
    }

    Bitboard &operator = (const Bitboard &b) = default;

    Bitboard operator & (const Bitboard &src) const {
      return Bitboard(data & src.data);
    }

    Bitboard operator & (uint64_t src) const {
      return Bitboard(data & src);
    }

    const Bitboard & operator &= (const Bitboard &src) {
      data &= src.data;
      return *this;
    }

    const Bitboard & operator &= (uint64_t src) {
      data &= src;
      return *this;
    }

    Bitboard operator | (const Bitboard &src) const {
      return Bitboard(data | src.data);
    }

    Bitboard operator | (uint64_t src) const {
      return Bitboard(data | src);
    }

    const Bitboard & operator |= (const Bitboard &src) {
      data |= src.data;
      return *this;
    }

    const Bitboard & operator |= (uint64_t src) {
      data |= src;
      return *this;
    }

    void shr(int n) {
        data = data >> n;
    }

    static inline Bitboard shr(const Bitboard &src, int n)
    {
        return Bitboard(src.data >> n);
    }

    void shr8()
    {
#if defined(_64BIT)
        data >>= 8;
#else
        uint8_t b = ((conv*)&data)->val4.b5;
        ((conv*)&data)->val2.hival >>= 8;
        ((conv*)&data)->val2.loval >>= 8;
        ((conv*)&data)->val4.b4 = b;
#endif
    }

    static Bitboard shl(const Bitboard &src, int n)
    {
        return Bitboard(src.data << n);
    }

    void shl(int n)
    {
        data = data << n;
    }

    void shl8()
    {
#if defined(_64BIT)
        data <<= 8;
#else
        uint8_t b = ((conv*)&data)->val4.b4;
        ((conv*)&data)->val2.loval <<= 8;
        ((conv*)&data)->val2.hival <<= 8;
        ((conv*)&data)->val4.b5 = b;
#endif
    }

    int operator == (const Bitboard &b) const
    {
       return b.data == data;
    }

    int operator != (const Bitboard &b) const
    {
       return b.data != data;
    }

    // return the number of bits set
    FORCEINLINE unsigned int bitCount() const
    {
#ifdef __cpp_lib_bitops
        return std::popcount<uint64_t>(data);
#endif
#if defined(__INTEL_COMPILER) && defined(USE_INTRINSICS)
#ifdef _64BIT
#ifdef USE_POPCNT
      // workaround ICC bug
      return data == 0ULL ? 0 : (unsigned int)_mm_popcnt_u64(data);
#else
      return _popcnt64(data);
#endif
#else
      return _popcnt32(lovalue()) + _popcnt32(hivalue());
#endif
#elif defined(_MSC_VER) && _MSC_VER >= 1500 && defined(USE_INTRINSICS)
#ifdef _64BIT
#ifdef USE_POPCNT
      return (int)_mm_popcnt_u64(data);
#else
      return (int)__popcnt64(data);
#endif
#else // 32-bit
#ifdef USE_POPCNT
      // With current MSVC this apparently only works with POPCNT
      // hardware support
      return __popcnt(lovalue()) + __popcnt(hivalue());
#else
      return genericPopcnt(data);
#endif
#endif
#elif defined(__GNUC__) && defined(_64BIT) && defined(__SSE4_2__)
      // GCC only uses POPCNT instruction if -msse4.2. Otherwise
      // it uses a relatively slow algorithm.
      return __builtin_popcountll(data);
#else
      return genericPopcnt(data);
#endif
    }

    // optimized bit count for low number of bits set
    FORCEINLINE unsigned int bitCountOpt() const {
#ifdef USE_POPCNT
        return bitCount();
#else
        int count;
        uint64_t tmp = data;
        for (count=0; tmp; count++)
           tmp &= tmp-1;
        return count;
#endif
    }

    FORCEINLINE unsigned int singleBitSet() const {
#ifdef __cpp_lib_bitops
        return (unsigned int)std::has_single_bit<uint64_t>(data);
#else
#ifdef USE_POPCNT
       return bitCount() == 1;
#else
       return data != (uint64_t)0 && ((data & (data-1)) == (uint64_t)0);
#endif
#endif
    }

    FORCEINLINE Square firstOne() const {
#ifdef __cpp_lib_bitops
      int zeros = std::countr_zero<uint64_t>(data);
      return zeros==64 ? InvalidSquare : zeros;
#else
#ifdef _64BIT
#if defined(USE_ASM) && defined(__x86_64__)
      if (data==0) return InvalidSquare;
      return _bitScanForwardAsm(data);
#elif defined(_WIN64) && defined(_MSC_VER) && defined(USE_INTRINSICS)
      DWORD index;
      if (_BitScanForward64(&index,data))
        return (Square)index;
      else
        return InvalidSquare;
#elif defined(USE_INTRINSICS) && defined(__INTEL_COMPILER)
      if (lovalue()) {
         return _bit_scan_forward(lovalue());
      else if (hivalue()) {
         return _bit_scan_forward(hivalue())+32;
      } else {
         return InvalidSquare;
      }
#elif defined(USE_INTRINSICS) && defined(__GNUC__)
      int tmp = __builtin_ffsll(data);
      if (tmp == 0) return InvalidSquare;
      else return tmp-1;
#else
      // generic 64-bit version, no intrinsics
      if (data == 0) return InvalidSquare;
      else return MagicTable64[(GETBIT64(data)*MAGIC64)>>58];
#endif
#else // 32-bit
#if defined(USE_INTRINSICS) && defined(_MSC_VER)
      DWORD index;
      if (_BitScanForward(&index,lovalue()))
        return (Square)index;
      else if (_BitScanForward(&index,hivalue()))
         return 32 + (Square)index;
      else
        return InvalidSquare;
#elif defined(USE_INTRINSICS) && defined(__INTEL_COMPILER)
      if (lovalue()) {
         return _bit_scan_forward(lovalue());
      else if (hivalue()) {
         return _bit_scan_forward(hivalue())+32;
      } else {
         return InvalidSquare;
      }
#elif defined(USE_INTRINSICS) && defined(__GNUC__)
      int tmp = __builtin_ffs(lovalue());
      if (tmp) return tmp-1;
      tmp = __builtin_ffs(hivalue());
      if (tmp) return tmp+31;
      return InvalidSquare;
#else
      // generic 32-bit code
      if (data == 0) return InvalidSquare;
      // use De Bruijn multiplication code from Lasse Hansen
      if (lovalue())
        return MagicTable32[(GETBIT32(lovalue())*MAGIC32)>>27];
      else
        return MagicTable32[(GETBIT32(hivalue())*MAGIC32)>>27]+32;
#endif
#endif
#endif
    }

    FORCEINLINE Square lastOne() const {
#ifdef __cpp_lib_bitops
      int zeros = countl_zero<uint64_t>(data);
      return zeros==64 ? InvalidSquare : 63-zeros;
#else
#ifdef _64BIT
#if defined(USE_ASM) && defined(__x86_64__)
      if (data==0) return InvalidSquare;
      return _bitScanReverseAsm(data);
#elif defined(_WIN64) && defined(_MSC_VER) && defined(USE_INTRINSICS)
      DWORD index;
      if (_BitScanReverse64(&index,data))
        return (Square)index;
      else
        return InvalidSquare;
#elif defined(USE_INTRINSICS) && defined(__INTEL_COMPILER)
      if (hivalue()) {
         return _bit_scan_reverse(hivalue())+32;
      else if (lovalue()) {
         return _bit_scan_reverse(lovalue());
      } else {
         return InvalidSquare;
      }
#elif defined(USE_INTRINSICS) && defined(__GNUC__)
      if (data==0) return InvalidSquare;
      else return 63-__builtin_clzll(data);
#else
      // generic 64-bit version, no intrinsics
      if (data == 0) return InvalidSquare;
      int bias = 0;
      uint64_t x = data;
      if (x > 0xFFFFFFFF) {
        x >>= 32;
        bias = 32;
      }
      if (x > 0xFFFF) {
        x >>= 16;
        bias += 16;
      }
      if (x > 0xFF) {
        x >>= 8;
        bias += 8;
      }
      return bias + msbTable[(int)x];
#endif
#else // 32-bit
#if defined(USE_INTRINSICS) && defined(_MSC_VER)
      DWORD index;
      if (_BitScanReverse(&index,hivalue()))
        return (Square)index+32;
      else if (_BitScanReverse(&index,lovalue()))
         return (Square)index;
      else
        return InvalidSquare;
#elif defined(USE_INTRINSICS) && defined(__INTEL_COMPILER)
      if (hivalue()) {
         return _bit_scan_reverse(hivalue()+32);
      else if (lovalue()) {
         return _bit_scan_reverse(lovalue());
      } else {
         return InvalidSquare;
      }
#elif defined(USE_INTRINSICS) && defined(__GNUC__)
      if (hivalue()) return 63-__builtin_clz(hivalue());
      else if (lovalue()) return 31-__builtin_clz(lovalue());
      else return InvalidSquare;
#else
      // generic 32-bit code
      if (data == 0) return InvalidSquare;
      int bias = 0;
      uint64_t x = data;
      if (x > 0xFFFFFFFF) {
        x >>= 32;
        bias = 32;
      }
      if (x > 0xFFFF) {
        x >>= 16;
        bias += 16;
      }
      if (x > 0xFF) {
        x >>= 8;
        bias += 8;
      }
      return bias + msbTable[(int)x];
#endif
#endif
#endif
    }


    FORCEINLINE int iterate(Square &sq) {
        if (!data) {
            return 0;
        }
        else {
            sq = firstOne();
            clear(sq);
            return 1;
        }
    }

    static void init();

    static void cleanup();

    static CACHE_ALIGN int MagicTable32[32];
#if defined(_64BIT)
    static CACHE_ALIGN int MagicTable64[64];
#endif
    static CACHE_ALIGN const uint64_t mask[64];

    uint64_t data;

    private:
#ifdef _64BIT
    static const uint64_t MAGIC64 = 0x07EDD5E59A4E28C2;
#endif
    static const unsigned MAGIC32 = 0xe89b2be;
    static const uint64_t m1  = 0x5555555555555555ULL; //binary: 0101...
    static const uint64_t m2  = 0x3333333333333333ULL; //binary: 00110011..
    static const uint64_t m4  = 0x0f0f0f0f0f0f0f0fULL; //binary:  4 zeros,  4 ones ...
    static int msbTable[256];

    unsigned genericPopcnt(uint64_t x) const {
      x = (x & 0x5555555555555555ULL) + ((x >>  1) & 0x5555555555555555ULL);
      x = (x & 0x3333333333333333ULL) + ((x >>  2) & 0x3333333333333333ULL);
      x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >>  4) & 0x0F0F0F0F0F0F0F0FULL);
      return (((uint32_t)(x >> 32)) * 0x01010101 >> 24) +
        (((uint32_t)(x      )) * 0x01010101 >> 24);
    }
};

inline int TEST_MASK(const Bitboard &b1,const Bitboard &b2) {
  return (uint64_t)(b1 & b2) != (uint64_t)0;
}


#endif
