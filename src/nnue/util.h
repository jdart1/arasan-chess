// Copyright 2021, 2022 by Jon Dart. All Rights Reserved

#ifndef _UTIL_H
#define _UTIL_H

// Arasan uses stdendian, which defines the various swap functions as macros
#ifdef _STDENDIAN_H_
#if _BYTE_ORDER == _BIG_ENDIAN
static uint16_t to_little_endian16(uint16_t x) { return bswap16(x); }
static uint32_t to_little_endian32(uint32_t x) { return bswap32(x); }
static uint64_t to_little_endian64(uint64_t x) { return bswap64(x); }
#else
static uint16_t to_little_endian16(uint16_t x) { return (x); }
static uint32_t to_little_endian32(uint32_t x) { return (x); }
static uint64_t to_little_endian64(uint64_t x) { return (x); }
#endif
#else
// Somewhat less general endian support
#if defined (_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__) || (defined(__APPLE__) && defined(__MACH__))
// assume little-endian
static uint16_t to_little_endian16(uint16_t x) { return (x); }
static uint32_t to_little_endian32(uint32_t x) { return (x); }
static uint64_t to_little_endian64(uint64_t x) { return (x); }
#elif __BYTE_ORDER == __BIG_ENDIAN
static uint16_t to_little_endian16(uint16_t x) { return __builtin_bswap16(x); }
static uint32_t to_little_endian32(uint32_t x) { return __builtin_bswap32(x); }
static uint64_t to_little_endian64(uint64_t x) { return __builtin_bswap64(x); }
#else
static uint16_t to_little_endian16(uint16_t x) { return (x); }
static uint32_t to_little_endian32(uint32_t x) { return (x); }
static uint64_t to_little_endian64(uint64_t x) { return (x); }
#endif
#endif

template <typename T> T read_little_endian(std::istream &s) {
    char buf[sizeof(T)];
    s.read(buf, sizeof(T));
    T input = *(reinterpret_cast<T *>(buf));
    switch (sizeof(T)) {
    case 1:
        return static_cast<T>(input);
    case 2:
        return static_cast<T>(to_little_endian16(input));
    case 4:
        return static_cast<T>(to_little_endian32(input));
    case 8:
        return static_cast<T>(to_little_endian64(input));
    default:
        throw std::invalid_argument("unsupported size");
    }
}

#endif
