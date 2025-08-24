// Copyright 2021, 2022, 2025 by Jon Dart. All Rights Reserved

#ifndef _UTIL_H
#define _UTIL_H

#include "../stdendian.h"
#include <cstdint>

static inline int16_t to_little_endian16(int16_t input) {
#if BYTE_ORDER == LITTLE_ENDIAN
    return input;
#else
    return bswap16(input);
#endif
}

static inline int32_t to_little_endian32(int32_t input) {
#if BYTE_ORDER == LITTLE_ENDIAN
    return input;
#else
    return bswap32(input);
#endif
}

static inline int64_t to_little_endian64(int64_t input) {
#if BYTE_ORDER == LITTLE_ENDIAN
    return input;
#else
    return bswap64(input);
#endif
}

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
