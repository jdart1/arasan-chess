// Copyright 1996-2008, 2010-2013, 2015-2020, 2022, 2025 by Jon Dart.  All Rights Reserved.
#ifndef _BITUTIL_H
#define _BITUTIL_H

// Defines bit-level operations. Many of these can be implemented via the C++ standard,
// but that requires C++20, which we do not currently assume.

#include "types.h"

#include <cstring>
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

#define GETBIT64(x) (uint64_t)(((int64_t)x)&-((int64_t)x))
#define GETBIT32(x) (uint32_t)(((int32_t)x)&-((int32_t)x))

class BitUtils
{
public:
    static void init();
    static void cleanup();

    static FORCEINLINE void setBit(uint64_t &data, int n)
    {
#if defined(_WIN64) && defined(_MSC_VER) && defined(USE_INTRINSICS)
        _bittestandset64((LONG64*)&data,(DWORD)n);
#elif defined(_64BIT)
        data |= ((uint64_t)1)<<n;
#else
        data |= mask[n];
#endif
    }

    static FORCEINLINE void clearBit(uint64_t &data, int n)
    {
#if defined(_WIN64) && defined(_MSC_VER) && defined(USE_INTRINSICS)
        _bittestandreset64((LONG64*)&data,(DWORD)n);
#elif defined(_64BIT)
        data &= ~(((uint64_t)1)<<n);
#else
        data &= ~mask[n];
#endif
    }

    static FORCEINLINE bool testBit(uint64_t data, int n)
    {
#if defined(_WIN64) & defined(USE_INTRINSICS)
        return _bittest64((int64_t*)&data,n);
#elif defined(_64BIT)
        return (data & (((uint64_t)1)<<n)) != (uint64_t)0;
#else
        return ((data & mask[n]) != (uint64_t)0);
#endif
    }

    static FORCEINLINE unsigned int bitCount(uint64_t data)
    {
#ifdef __cpp_lib_bitops
        return std::popcount<uint64_t>(data);
#endif
#if defined(__INTEL_COMPILER) && defined(USE_INTRINSICS)
#ifdef _64BIT
#ifdef USE_POPCNT
        return data == 0ULL ? 0 : (unsigned int)_mm_popcnt_u64(data);
#else
        return _popcnt64(data);
#endif
#else
        return _popcnt32(lovalue(data)) + _popcnt32(hivalue(data));
#endif
#elif defined(_MSC_VER) && _MSC_VER >= 1500 && defined(USE_INTRINSICS)
#ifdef _64BIT
#ifdef USE_POPCNT
        return (int)_mm_popcnt_u64(data);
#else
        return (int)__popcnt64(data);
#endif
#else
#ifdef USE_POPCNT
        return __popcnt(lovalue(data)) + __popcnt(hivalue(data));
#else
        return genericPopcnt(data);
#endif
#endif
#elif defined(__GNUC__) && defined(_64BIT) && defined(__SSE4_2__)
        return __builtin_popcountll(data);
#else
        return genericPopcnt(data);
#endif
    }

    static FORCEINLINE unsigned int singleBitSet(uint64_t data)
    {
#ifdef __cpp_lib_bitops
        return (unsigned int)std::has_single_bit<uint64_t>(data);
#else
#ifdef USE_POPCNT
        return bitCount(data) == 1;
#else
        return data != (uint64_t)0 && ((data & (data-1)) == (uint64_t)0);
#endif
#endif
    }

    static FORCEINLINE int firstOne(uint64_t data)
    {
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
        if (lovalue(data)) {
            return _bit_scan_forward(lovalue(data));
        } else if (hivalue(data)) {
            return _bit_scan_forward(hivalue(data))+32;
        } else {
            return InvalidSquare;
        }
#elif defined(USE_INTRINSICS) && defined(__GNUC__)
        int tmp = __builtin_ffsll(data);
        if (tmp == 0) return InvalidSquare;
        else return tmp-1;
#else
        if (data == 0) return InvalidSquare;
        else return MagicTable64[(GETBIT64(data)*MAGIC64)>>58];
#endif
#else
#if defined(USE_INTRINSICS) && defined(_MSC_VER)
        DWORD index;
        if (_BitScanForward(&index,lovalue(data)))
            return (Square)index;
        else if (_BitScanForward(&index,hivalue(data)))
            return 32 + (Square)index;
        else
            return InvalidSquare;
#elif defined(USE_INTRINSICS) && defined(__INTEL_COMPILER)
        if (lovalue(data)) {
            return _bit_scan_forward(lovalue(data));
        } else if (hivalue(data)) {
            return _bit_scan_forward(hivalue(data))+32;
        } else {
            return InvalidSquare;
        }
#elif defined(USE_INTRINSICS) && defined(__GNUC__)
        int tmp = __builtin_ffs(lovalue(data));
        if (tmp) return tmp-1;
        tmp = __builtin_ffs(hivalue(data));
        if (tmp) return tmp+31;
        return InvalidSquare;
#else
        if (data == 0) return InvalidSquare;
        if (lovalue(data))
            return MagicTable32[(GETBIT32(lovalue(data))*MAGIC32)>>27];
        else
            return MagicTable32[(GETBIT32(hivalue(data))*MAGIC32)>>27]+32;
#endif
#endif
#endif
    }

    static FORCEINLINE int lastOne(uint64_t data)
    {
#ifdef __cpp_lib_bitops
        int zeros = std::countl_zero<uint64_t>(data);
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
        if (hivalue(data)) {
            return _bit_scan_reverse(hivalue(data))+32;
        } else if (lovalue(data)) {
            return _bit_scan_reverse(lovalue(data));
        } else {
            return InvalidSquare;
        }
#elif defined(USE_INTRINSICS) && defined(__GNUC__)
        if (data==0) return InvalidSquare;
        else return 63-__builtin_clzll(data);
#else
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
#else
#if defined(USE_INTRINSICS) && defined(_MSC_VER)
        DWORD index;
        if (_BitScanReverse(&index,hivalue(data)))
            return (Square)index+32;
        else if (_BitScanReverse(&index,lovalue(data)))
            return (Square)index;
        else
            return InvalidSquare;
#elif defined(USE_INTRINSICS) && defined(__INTEL_COMPILER)
        if (hivalue(data)) {
            return _bit_scan_reverse(hivalue(data)+32);
        } else if (lovalue(data)) {
            return _bit_scan_reverse(lovalue(data));
        } else {
            return InvalidSquare;
        }
#elif defined(USE_INTRINSICS) && defined(__GNUC__)
        if (hivalue(data)) return 63-__builtin_clz(hivalue(data));
        else if (lovalue(data)) return 31-__builtin_clz(lovalue(data));
        else return InvalidSquare;
#else
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

    static FORCEINLINE void shiftRight8_32bit(uint64_t &data)
    {
        uint32_t hival = (uint32_t)(data >> 32);
        uint32_t loval = (uint32_t)(data & 0xFFFFFFFF);
        uint8_t b = (uint8_t)((hival >> 0) & 0xFF);
        hival >>= 8;
        loval >>= 8;
        loval |= ((uint32_t)b) << 24;
        data = ((uint64_t)hival << 32) | loval;
    }

    static FORCEINLINE void shiftLeft8_32bit(uint64_t &data)
    {
        uint32_t hival = (uint32_t)(data >> 32);
        uint32_t loval = (uint32_t)(data & 0xFFFFFFFF);
        uint8_t b = (uint8_t)((loval >> 24) & 0xFF);
        loval <<= 8;
        hival <<= 8;
        hival |= (uint32_t)b;
        data = ((uint64_t)hival << 32) | loval;
    }

    static constexpr CACHE_ALIGN uint64_t mask[64] = {
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
            ((uint64_t)1) << 63};


private:
    static CACHE_ALIGN int MagicTable32[32];
#if defined(_64BIT)
    static CACHE_ALIGN int MagicTable64[64];
#endif
    static int msbTable[256];

#ifdef _64BIT
    static const uint64_t MAGIC64 = 0x07EDD5E59A4E28C2;
#endif
    static const unsigned MAGIC32 = 0xe89b2be;

    static FORCEINLINE uint32_t hivalue(uint64_t data)
    {
#if _BYTE_ORDER == _BIG_ENDIAN
        return (uint32_t)(data >> 32);
#else
        return (uint32_t)(data >> 32);
#endif
    }

    static FORCEINLINE uint32_t lovalue(uint64_t data)
    {
#if _BYTE_ORDER == _BIG_ENDIAN
        return (uint32_t)(data & 0xFFFFFFFF);
#else
        return (uint32_t)(data & 0xFFFFFFFF);
#endif
    }

    static unsigned genericPopcnt(uint64_t x)
    {
        x = (x & 0x5555555555555555ULL) + ((x >>  1) & 0x5555555555555555ULL);
        x = (x & 0x3333333333333333ULL) + ((x >>  2) & 0x3333333333333333ULL);
        x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x >>  4) & 0x0F0F0F0F0F0F0F0FULL);
        return (((uint32_t)(x >> 32)) * 0x01010101 >> 24) +
            (((uint32_t)(x      )) * 0x01010101 >> 24);
    }

};

#endif
