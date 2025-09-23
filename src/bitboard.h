// Copyright 1996-2008, 2010-2013, 2015-2020, 2022, 2025 by Jon Dart.  All Rights Reserved.
#ifndef _BITBOARD_H
#define _BITBOARD_H

#include "types.h"
#include "bitutil.h"

#include <cstring>
#include <iostream>

class Bitboard
{
    friend std::ostream & operator << (std::ostream &o, const Bitboard &b);


    public:

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
        BitUtils::setBit(data, n);
    }

    void FORCEINLINE clear(int n)
    {
        BitUtils::clearBit(data, n);
    }

    void setClear(const Bitboard &b)
    {
        data ^= b.data;
    }

    bool FORCEINLINE isSet(int n) const
    {
        return BitUtils::testBit(data, n);
    }

    bool FORCEINLINE isClear()const {
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
        BitUtils::shiftRight8_32bit(data);
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
        BitUtils::shiftLeft8_32bit(data);
#endif
    }

    bool operator == (const Bitboard &b) const
    {
       return b.data == data;
    }

    bool operator != (const Bitboard &b) const
    {
       return b.data != data;
    }

    // return the number of bits set
    FORCEINLINE unsigned int bitCount() const
    {
        return BitUtils::bitCount(data);
    }

    FORCEINLINE unsigned int singleBitSet() const {
        return BitUtils::singleBitSet(data);
    }

    FORCEINLINE Square firstOne() const {
        return BitUtils::firstOne(data);
    }

    FORCEINLINE Square lastOne() const {
        return BitUtils::lastOne(data);
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

    static void init() {
        BitUtils::init();
    }

    static void cleanup() {
        BitUtils::cleanup();
    }

  private:
    uint64_t data;

    struct ints {
#if _BYTE_ORDER == _BIG_ENDIAN
        uint32_t hival, loval;
#else
        uint32_t loval, hival;
#endif
    };
    struct shorts {
#if _BYTE_ORDER == _BIG_ENDIAN
        uint16_t hi2, hi1, lo2, lo1;
#else
        uint16_t lo1, lo2, hi1, hi2;
#endif
    };
    struct bytes {
#if _BYTE_ORDER == _BIG_ENDIAN
        uint8_t b8, b7, b6, b5, b4, b3, b2, b1;
#else
        uint8_t b1, b2, b3, b4, b5, b6, b7, b8;
#endif
    };
    union conv {
        uint64_t val1;
        ints val2;
        shorts val3;
        bytes val4;
    };
};

#endif
