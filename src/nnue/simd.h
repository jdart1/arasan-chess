// Copyright 2021-2025 by Jon Dart. All Rights Reserved.
#ifndef NNUE_SIMD_H
#define NNUE_SIMD_H

#if !defined(AVX512) && !defined(AVX2) && !defined(SSE2) && !defined(SSE3) && !defined(NEON)
#error must set at least one of: AVX512, AVX2, SSSE3, SSE2 or NEON
#endif

#if !defined(_MSC_VER) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

extern "C" {
#if defined(NEON)
#include <arm_neon.h>
#else
#include <immintrin.h>
#endif
}

#include "../types.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>

// fix for clang compile
#ifndef _MM_PERM_BADC
#define _MM_PERM_BADC 0x4E
#endif
#ifndef _MM_PERM_CDAB
#define _MM_PERM_CDAB 0xB1
#endif

namespace simd {

#if defined(NEON)
using vec_t = int16x8_t;
using vec16_t = int16x8_t;
using vec32_t = int32x4_t;
static inline vec_t vec_load(const vec_t *x) {
    return vld1q_s16(reinterpret_cast<const int16_t *>(x));
}
static inline vec_t vec_set_16(int16_t x) { return vdupq_n_s16(x); }
static const vec_t ones128 = vec_set_16(1);
static const vec_t zeros128 = vec_set_16(0);
static const vec_t zero = zeros128;
static inline vec_t vec_add32(vec_t x, vec_t y) { return vaddq_s32(x, y); }
static inline vec_t vec_clamp16(vec_t x, vec_t maxValues) {
    return vminq_s16(vmaxq_s16(x, zero), maxValues);
}
static inline vec_t vec_clamp32(vec_t x, vec_t maxValues) {
    return vminq_s32(vmaxq_s32(x, zero), maxValues);
}
static inline vec_t vec_mullo16(vec_t x, vec_t y) { return vmulq_s16(x, y); }
static inline vec_t vec_mull16(vec_t x, vec_t y) { return vmulq_s16(x, y); }
static inline vec_t vec_madd16(vec_t x, vec_t y) {
    const int32x4_t low = vmull_s16(vget_low_s16(x), vget_low_s16(y));
    const int32x4_t high = vmull_high_s16(x, y);
    return vpaddq_s32(low, high);
}
#else
// x86 functions are templatized
template <typename T> static inline T vec_load(const T *);
template <typename T> static inline void vec_store(T *x, T y);
template <typename T> static inline void vec_set_zero(T &x);
template <typename T> static inline T vec_set_32(int32_t);
template <typename T> static inline T vec_set_16(int16_t);
template <typename T> static inline T vec_add16(T, const T *);
template <typename T> static inline T vec_add32(T, const T *);
template <typename T> static inline T vec_add32(T, T);
template <typename T> static inline T vec_sub16(T, const T *);
template <typename T> static inline T vec_sub32(T, const T *);
template <typename T> static inline T vec_clamp16(T x, T maxValues);
template <typename T> static inline T vec_clamp32(T x, T maxValues);
template <typename T> static inline T vec_mullo16(T x, T y);
template <typename T> static inline T vec_mullo32(T x, T y);
template <typename T> static inline T vec_madd16(T x, T y);
template <typename T> static inline int32_t hsum32(T x);
#endif

#if defined(SSE2) || defined(SSE3) || defined(AVX2)
// define here because called by AVX2 code
template <> int32_t hsum32(__m128i x) {
    // https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-sse-vector-sum-or-other-reduction
    // TBD: use AVX version if possible? Right now we assume no AVX
    __m128i hi64 = _mm_shuffle_epi32(x, _MM_SHUFFLE(1, 0, 3, 2));
    __m128i sum64 = _mm_add_epi32(hi64, x);
    __m128i hi32 = _mm_shufflelo_epi16(sum64, _MM_SHUFFLE(1, 0, 3, 2)); // Swap the low two elements
    __m128i sum32 = _mm_add_epi32(sum64, hi32);
    return _mm_cvtsi128_si32(sum32); // SSE2 movd
}
#endif

// Define some constants and a default vector type for each architecture
#ifdef NEON
static constexpr size_t VEC_ALIGN = 32;
static constexpr size_t simdWidth = 128;
static constexpr size_t simdRegCount = 16;
#elif defined(AVX512)
using vec_t = __m512i;
static constexpr size_t VEC_ALIGN = 64;
static constexpr size_t simdWidth = 512;
static constexpr size_t simdRegCount = 32;
#elif defined(AVX2)
using vec_t = __m256i;
static constexpr size_t VEC_ALIGN = 32;
static constexpr size_t simdWidth = 256;
static constexpr size_t simdRegCount = 16;
#else
using vec_t = __m128i;
static constexpr size_t VEC_ALIGN = 32;
static constexpr size_t simdWidth = 128;
static constexpr size_t simdRegCount = 16;
#endif

// Template specializations for each architecture. Multiple specializations
// may be defined.
#ifdef AVX512
static const vec_t ones512 = _mm512_set1_epi16(1);
    static const vec_t zero512 = _mm512_setzero_si512();
template <> vec_t vec_set_32<__m512i>(int x) { return _mm512_set1_epi32(x); }
template <> vec_t vec_load(const __m512i *x) { return _mm512_load_si512(x); }
template <> void vec_store(__m512i *x, vec_t y) { _mm512_store_si512(x, y); }
template <> void vec_set_zero(__m512i &x) { x = zero512; }
template <> __m512i vec_set_16<__m512i>(int16_t x) { return _mm512_set1_epi16(x); }
template <> __m512i vec_add16<__m512i>(__m512i x, const __m512i *y) {
    return _mm512_add_epi16(x, vec_load(y));
}
template <> inline __m512i vec_add32<__m512i>(__m512i x, const __m512i *y) {
    return _mm512_add_epi32(x, vec_load(y));
}
template <> inline __m512i vec_add32<__m512i>(__m512i x, __m512i y) { return _mm512_add_epi32(x, y); }
template <> inline __m512i vec_sub16<__m512i>(__m512i x, const __m512i *y) {
    return _mm512_sub_epi16(x, vec_load(y));
}
template <> inline __m512i vec_sub32<__m512i>(__m512i x, const __m512i *y) {
    return _mm512_sub_epi32(x, vec_load(y));
}
template <> __m512i vec_clamp16(__m512i x, __m512i maxValues) {
    return _mm512_min_epi16(maxValues, _mm512_max_epi16(x, _mm512_setzero_si512()));
}
template <> __m512i vec_clamp32(__m512i x, __m512i maxValues) {
    return _mm512_min_epi32(maxValues, _mm512_max_epi32(x, _mm512_setzero_si512()));
}
template <> __m512i vec_mullo16(__m512i x, __m512i y) { return _mm512_mullo_epi16(x, y); }
template <> __m512i vec_mullo32(__m512i x, __m512i y) { return _mm512_mullo_epi32(x, y); }
template <> __m512i vec_madd16(__m512i x, __m512i y) { return _mm512_madd_epi16(x, y); }
template <> int32_t hsum32(__m512i prod) { return _mm512_reduce_add_epi32(prod); }
#endif

#if defined(AVX2)
static const __m256i ones256 = _mm256_set1_epi16(1);
static const __m256i zero256 = _mm256_setzero_si256();
static inline __m256i vec_set_16(int x) { return _mm256_set1_epi16(x); }
template <> __m256i vec_set_32<__m256i>(int x) { return _mm256_set1_epi32(x); }
template <> __m256i vec_load(const __m256i *x) { return _mm256_load_si256(x); }
template <> void vec_store(__m256i *x, __m256i y) { _mm256_store_si256(x, y); }
template <> void vec_set_zero(__m256i &x) { x = zero256; }
template <> __m256i vec_set_16<__m256i>(int16_t x) { return _mm256_set1_epi16(x); }
template <> __m256i vec_add16<__m256i>(__m256i x, const __m256i *y) {
    return _mm256_add_epi16(x, vec_load(y));
}
template <> __m256i vec_add32<__m256i>(__m256i x, const __m256i *y) {
    return _mm256_add_epi32(x, vec_load(y));
}
template <> __m256i vec_add32<__m256i>(__m256i x, __m256i y) { return _mm256_add_epi32(x, y); }
template <> __m256i vec_sub16<__m256i>(__m256i x, const __m256i *y) {
    return _mm256_sub_epi16(x, vec_load(y));
}
template <> inline __m256i vec_sub32<__m256i>(__m256i x, const __m256i *y) {
    return _mm256_sub_epi32(x, vec_load(y));
}
template <> __m256i vec_clamp16(__m256i x, __m256i maxValues) {
    return _mm256_min_epi16(maxValues, _mm256_max_epi16(x, _mm256_setzero_si256()));
}
template <> __m256i vec_clamp32(__m256i x, __m256i maxValues) {
    return _mm256_min_epi32(maxValues, _mm256_max_epi32(x, _mm256_setzero_si256()));
}
template <> __m256i vec_mullo16(__m256i x, __m256i y) { return _mm256_mullo_epi16(x, y); }
template <> __m256i vec_mullo32(__m256i x, __m256i y) { return _mm256_mullo_epi32(x, y); }
template <> __m256i vec_madd16(__m256i x, __m256i y) { return _mm256_madd_epi16(x, y); }
template <> int32_t hsum32(__m256i x) {
    __m128i sum128 = _mm_add_epi32(_mm256_castsi256_si128(x), _mm256_extracti128_si256(x, 1));
    return hsum32<__m128i>(sum128);
}
#endif

#if defined(SSE2) || defined(SSSE3)
static const __m128i ones128 = _mm_set1_epi16(1);
static const __m128i zero128 = _mm_setzero_si128();
template <> __m128i vec_load(const __m128i *x) { return _mm_load_si128(x); }
template <> void vec_store(__m128i *x, __m128i y) { _mm_store_si128(x, y); }
template <> void vec_set_zero(__m128i &x) { x = zero128; }
template <> __m128i vec_mullo16(__m128i x, __m128i y) { return _mm_mullo_epi16(x, y); }
template <> __m128i vec_madd16(__m128i x, __m128i y) { return _mm_madd_epi16(x, y); }
template <> __m128i vec_set_16(int16_t x) { return _mm_set1_epi16(x); }
template <> __m128i vec_add16(__m128i x, const __m128i *y) {
    return _mm_add_epi16(x, vec_load(y));
}
template <> __m128i vec_add32<__m128i>(__m128i x, const __m128i *y) {
    return _mm_add_epi32(x, vec_load(y));
}
template <> __m128i vec_add32<__m128i>(__m128i x, __m128i y) { return _mm_add_epi32(x, y); }
template <> __m128i vec_sub16<__m128i>(__m128i x, const __m128i *y) {
    return _mm_sub_epi16(x, vec_load(y));
}
template <> __m128i vec_sub32<__m128i>(__m128i x, const __m128i *y) {
    return _mm_sub_epi32(x, vec_load(y));
}
template <> __m128i vec_clamp16(__m128i x, __m128i maxValues) {
    return _mm_min_epi16(maxValues, _mm_max_epi16(x, zero128));
}
template <> __m128i vec_clamp32(__m128i x, __m128i maxValues) {
    return _mm_min_epi32(maxValues, _mm_max_epi32(x, zero128));
}
// TBD: not currently used
// template <>
// __m128i vec_madd16(__m128i x, __m128i y) { return _mm256_madd_epi16(x, y); }
#endif

#if !defined(_MSC_VER) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#ifdef NEON
static inline int32_t add4x32_neon(int32x4_t reg) {
#if defined(__aarch64__)
    return vaddvq_s32(reg);
#else
    using ints = int32_t[4];
    ints *inp = reinterpret_cast<ints *>(&reg);
    int32_t sum = 0;
    for (unsigned i = 0; i < 4; ++i) {
        sum += (*inp)[i];
    }
    return sum;
#endif
}

template <typename vec_type, typename AccumType, size_t bytes>
struct SimdOperationsNeon {
    static inline vec_type load(const AccumType *x) {
       if constexpr (bytes==2)
           return vld1q_s16(x);
       else
           return vld1q_s32(x);
    }
    static inline vec_type add(vec_type x, vec_type y) {
       if constexpr (bytes==2)
           return vaddq_s16(x,y);
       else
           return vaddq_s32(x,y);
    }
    static inline vec_type sub(vec_type x, vec_type y) {
       if constexpr (bytes==2)
           return vsubq_s16(x,y);
       else
           return vsubq_s32(x,y);
    }
    static inline void store(AccumType *x, const vec_type y) {
       if constexpr (bytes==2)
           vst1q_s16(x,y);
       else
           vst1q_s32(x,y);
    }
    static inline vec_type zero() {
       if constexpr (bytes==2)
           return vdupq_n_s16(0);
       else
           return vdupq_n_s32(0);
    }
};

#else // x86

template <size_t bytes> static inline vec_t vec_add(vec_t x, const vec_t *y) {
    if constexpr (bytes == 2)
        return vec_add16(x, y);
    else // (bytes==4)
        return vec_add32(x, y);
}

template <size_t bytes> static inline vec_t vec_sub(vec_t x, const vec_t *y) {
    if constexpr (bytes == 2)
        return vec_sub16(x, y);
    else // (bytes==4)
        return vec_sub32(x, y);
}

#ifdef AVX2
[[maybe_unused]] void inline m256_add_dpbusd_epi32(__m256i &acc, __m256i a, __m256i b) {
#ifdef VNNI
    acc = _mm256_dpbusd_epi32(acc, a, b);
#else
    __m256i x = _mm256_maddubs_epi16(a, b);
    x = _mm256_madd_epi16(x, ones256);
    acc = _mm256_add_epi32(acc, x);
#endif
}
#endif

#ifdef AVX512
[[maybe_unused]] void inline m512_add_dpbusd_epi32(__m512i &acc, __m512i a, __m512i b) {
#ifdef AVX512_VNNI
    acc = _mm512_dpbusd_epi32(acc, a, b);
#else
    __m512i x = _mm512_maddubs_epi16(a, b);
    x = _mm512_madd_epi16(x, ones512);
    acc = _mm512_add_epi32(acc, x);
#endif
}
#endif

#endif // x86

template <typename T, unsigned simdWidth> inline static constexpr size_t chunks(unsigned len) {
    return (len * 8 * sizeof(T)) / simdWidth;
}

template <size_t size, typename DataType> FORCEINLINE void vec_copy(const DataType *input, DataType *output) {
    constexpr size_t bits = size * 8 * sizeof(DataType);
    static_assert(bits >= 128 && (bits % 128) == 0);
    if constexpr (bits >= simdWidth && bits % simdWidth == 0) {
        const vec_t *inp = reinterpret_cast<const vec_t *>(input);
        vec_t *outp = reinterpret_cast<vec_t *>(output);
// old Apple clang does not support this
#if !defined(_MSC_VER) && !defined(__MINGW32__) && !(defined(_MAC) && defined(__x86_64__))
#pragma GCC unroll 8
#endif
        for (size_t i = 0; i < bits / simdWidth; ++i) {
            outp[i] = vec_load(inp + i);
        }
        return;
    }
    size_t width = simdWidth;
    size_t remaining = bits;
    unsigned incr = 0;
    while (remaining) {
        width = std::min<size_t>(width,remaining);
        switch(width) {
#ifdef AVX512
        case 512: {
            const __m512i *inp = reinterpret_cast<const __m512i *>(input);
            __m512i *outp = reinterpret_cast<__m512i *>(output);
            for (size_t i = 0; i < remaining / 512; ++i) {
                outp[i] = _mm512_load_si512(inp + i);
            }
            incr = sizeof(DataType) * (remaining / 512);
            remaining = remaining % 512;
            break;
        }
#endif
#ifdef AVX2
        case 256: {
            const __m256i *inp = reinterpret_cast<const __m256i *>(input);
            __m256i *outp = reinterpret_cast<__m256i *>(output);
            for (size_t i = 0; i < remaining / 256; ++i) {
                outp[i] = _mm256_load_si256(inp + i);
            }
            incr = sizeof(DataType) * (remaining / 256);
            remaining = remaining % 256;
            break;
        }
#endif
#if defined(SSE2) || defined(SSE3)
        case 128: {
            const __m128i *inp = reinterpret_cast<const __m128i *>(input);
            __m128i *outp = reinterpret_cast<__m128i *>(output);
            for (size_t i = 0; i < remaining / 128; ++i) {
                outp[i] = _mm_load_si128(inp + i);
            }
            remaining = remaining % 128;
            break;
        }
#endif
#ifdef NEON
        case 128: {
            const int16x8_t *inp = reinterpret_cast<const int16x8_t *>(input);
            int16x8_t *outp = reinterpret_cast<int16x8_t *>(output);
            for (size_t i = 0; i < remaining / 128; ++i) {
                outp[i] = vld1q_s64(reinterpret_cast<const int64_t *>(inp + i));
            }
            remaining = remaining % 128;
            break;
        }
#endif
        default:
            assert(0);
        } // switch
        input += incr;
        output += incr;
        width /= 2;
    }
}

#ifdef NEON

template <typename vec_type, typename AccumType, typename WeightType, size_t inputSize /* features */,
          size_t outputSize /* accumulator size */, size_t regCount, size_t regWidth,
          size_t iterations, typename operations>
inline void updateLoopNeon(const AccumType *source, AccumType *target,
                           const WeightType (&weights)[inputSize][outputSize],
                           const unsigned *added, size_t added_count, const unsigned *removed,
                           size_t removed_count, size_t &offset) {
    constexpr size_t indexMultiplier = simdWidth / (8 * sizeof(AccumType));
    alignas(VEC_ALIGN) vec_type regs[regCount];
    for (size_t iter = 0; iter < iterations; ++iter, offset += regCount) {
        // load source into registers
        for (size_t i = 0; i < regCount; ++i) {
            regs[i] = operations::load(source + indexMultiplier * (offset + i));
        }
        // perform updates in registers
        for (size_t i = 0; i < added_count; ++i) {
            const auto *w = weights[added[i]];
            for (size_t j = 0; j < regCount; ++j) {
                regs[j] =
                    operations::add(regs[j], operations::load(w + indexMultiplier * (offset + j)));
            }
        }
        for (size_t i = 0; i < removed_count; ++i) {
            const auto *w = weights[removed[i]];
            for (size_t j = 0; j < regCount; ++j) {
                regs[j] =
                    operations::sub(regs[j], operations::load(w + indexMultiplier * (offset + j)));
            }
        }
        // store results to memory
        for (size_t i = 0; i < regCount; ++i) {
            operations::store(target + indexMultiplier * (offset + i), regs[i]);
        }
    }
}

template <typename vec_type, typename AccumType, typename WeightType, typename BiasType,
          size_t inputSize /* features */, size_t outputSize /* accumulator size */,
          size_t regCount, size_t regWidth, size_t iterations, typename operations>
FORCEINLINE void
fullUpdateLoopNeon(AccumType *target, const WeightType (*weights)[inputSize][outputSize],
                   const BiasType (*biases)[outputSize], const unsigned *indices, size_t &offset) {
    static_assert(outputSize * sizeof(AccumType) * 8 >= simdWidth,
                  "insufficient accumulator width");
    static_assert(outputSize * sizeof(AccumType) * 8 % simdWidth == 0,
                  "accumulator size is not multiple of SIMD width");
    constexpr size_t indexMultipler = simdWidth / (8 * sizeof(AccumType));
    alignas(VEC_ALIGN) vec_type regs[regCount];
    for (size_t iter = 0; iter < iterations; ++iter, offset += regCount) {
        // load biases into registers
        if (biases) {
            for (size_t i = 0; i < regCount; ++i) {
                regs[i] = operations::load((*biases) + indexMultipler * (offset + i));
            }
        } else {
            for (size_t i = 0; i < regCount; ++i) {
                regs[i] = operations::zero();
            }
        }
        // perform updates in registers
        for (size_t i = 0; indices[i] != 1000000; ++i) {
            const auto w = (*weights)[indices[i]];
            for (size_t j = 0; j < regCount; ++j) {
                regs[j] =
                    operations::add(regs[j], operations::load(w + indexMultipler * (offset + j)));
            }
        }
        // store results to memory
        for (size_t i = 0; i < regCount; ++i) {
            operations::store(target + indexMultipler * (offset + i), regs[i]);
        }
    }
}

#else

template <typename vec_type, typename AccumType, typename WeightType,
          size_t inputSize /* features */, size_t outputSize /* accumulator size */,
          size_t regCount, size_t regWidth, size_t iterations, size_t bytes>
inline void updateLoop(const AccumType *source, AccumType *target,
                       const WeightType (&weights)[inputSize][outputSize], const unsigned *added,
                       size_t added_count, const unsigned *removed, size_t removed_count,
                       size_t &offset) {
    static_assert(outputSize * sizeof(AccumType) * 8 >= regWidth, "insufficient accumulator width");
    static_assert(outputSize * sizeof(AccumType) * 8 % regWidth == 0,
                  "accumulator size is not multiple of SIMD register width");
    static_assert(bytes == 2 || bytes == 4);
    vec_type *outp = reinterpret_cast<vec_type *>(target);
    const vec_type *inp = reinterpret_cast<const vec_type *>(source);
    alignas(VEC_ALIGN) vec_type regs[regCount];
    for (size_t iter = 0; iter < iterations; ++iter, offset += regCount) {
        // load source into registers
        for (size_t i = 0; i < regCount; ++i) {
            regs[i] = vec_load(inp + offset + i);
        }
        // perform updates in registers
        for (size_t i = 0; i < added_count; ++i) {
            const vec_type *w = reinterpret_cast<const vec_type *>(weights[added[i]]);
            for (size_t j = 0; j < regCount; ++j) {
                regs[j] = vec_add<bytes>(regs[j], w + offset + j);
            }
        }
        for (size_t i = 0; i < removed_count; ++i) {
            const vec_type *w = reinterpret_cast<const vec_type *>(weights[removed[i]]);
            for (size_t j = 0; j < regCount; ++j) {
                regs[j] = vec_sub<bytes>(regs[j], w + offset + j);
            }
        }
        // store results to memory
        for (size_t i = 0; i < regCount; ++i) {
            vec_store(outp + offset + i, regs[i]);
        }
    }
}

template <typename vec_type, typename AccumType, typename WeightType, typename BiasType,
          size_t inputSize /* features */, size_t outputSize /* accumulator size */,
          size_t regCount, size_t regWidth, size_t iterations, size_t bytes>
inline void fullUpdateLoop(AccumType *target, const WeightType (*weights)[inputSize][outputSize],
                           const BiasType (*biases)[outputSize], const unsigned *indices,
                           size_t &offset) {
    static_assert(outputSize * sizeof(AccumType) * 8 >= regWidth, "insufficient accumulator width");
    static_assert(outputSize * sizeof(AccumType) * 8 % regWidth == 0,
                 "accumulator size is not multiple of SIMD width");
    static_assert(bytes == 2 || bytes == 4);
    vec_type *outp = reinterpret_cast<vec_type *>(target);
    alignas(VEC_ALIGN) vec_type regs[regCount];
    for (size_t iter = 0; iter < iterations; ++iter, offset += regCount) {
        // load biases into registers
        if (biases) {
            const vec_type *biasp = reinterpret_cast<const vec_type *>(*biases);
            for (size_t i = 0; i < regCount; ++i) {
                regs[i] = vec_load(biasp + offset + i);
            }
        } else {
            for (size_t i = 0; i < regCount; ++i) {
                vec_set_zero(regs[i]);
            }
        }
        // perform updates in registers
        for (size_t i = 0; indices[i] != 1000000; ++i) {
            const vec_type *w = reinterpret_cast<const vec_type *>((*weights)[indices[i]]);
            for (size_t j = 0; j < regCount; ++j) {
                regs[j] = vec_add<bytes>(regs[j], w + offset + j);
            }
        }
        // store results to memory
        for (size_t i = 0; i < regCount; ++i) {
            vec_store(outp + offset + i, regs[i]);
        }
    }
}

#endif

template <typename AccumType, typename WeightType, typename BiasType,
          size_t inputSize /* features */, size_t outputSize /* accumulator size */>
void fullUpdate(AccumType *target, const WeightType (*weights)[inputSize][outputSize],
                const BiasType (*biases)[outputSize], const unsigned *indices) {
    static_assert(sizeof(AccumType) == 2 || sizeof(AccumType) == 4, "unsupported accumulator type");
    static_assert(sizeof(AccumType) == sizeof(WeightType),
                  "AccumType different from WeightType not currently supported");
    static_assert(sizeof(WeightType) == sizeof(BiasType),
                  "BiasType different from WeightType not currently supported");
    size_t offset = 0;
#ifdef NEON
    constexpr size_t registerWidths = chunks<AccumType, simdWidth>(outputSize);
    // It is faster here to not reserve all registers for storing intermediate results,
    // because NEON vector operations except load/store operate on registers only.
    constexpr size_t regCount = simdRegCount / 2;
    constexpr size_t iterations = registerWidths / regCount;
    constexpr size_t remaining = registerWidths % regCount;
    if constexpr (iterations > 0) {
        if constexpr (sizeof(AccumType) == 2)
            fullUpdateLoopNeon<vec16_t, AccumType, WeightType, BiasType, inputSize, outputSize,
                               regCount, simdWidth, iterations,
                               SimdOperationsNeon<vec16_t, AccumType, 2>>(target, weights, biases,
                                                                          indices, offset);
        else
            fullUpdateLoopNeon<vec32_t, AccumType, WeightType, BiasType, inputSize, outputSize,
                               regCount, simdWidth, iterations,
                               SimdOperationsNeon<vec32_t, AccumType, 4>>(target, weights, biases,
                                                                          indices, offset);
    }
    if constexpr (remaining > 0) {
        if constexpr (sizeof(AccumType) == 2)
            fullUpdateLoopNeon<vec16_t, AccumType, WeightType, BiasType, inputSize, outputSize,
                               remaining, simdWidth, 1, SimdOperationsNeon<vec16_t, AccumType, 2>>(
                target, weights, biases, indices, offset);
        else
            fullUpdateLoopNeon<vec32_t, AccumType, WeightType, BiasType, inputSize, outputSize,
                               remaining, simdWidth, 1, SimdOperationsNeon<vec32_t, AccumType, 4>>(
                target, weights, biases, indices, offset);
    }
#else
#if defined(AVX512)
    if constexpr (outputSize * sizeof(AccumType) * 8 < simdWidth) {
        // special case, fall back to AVX2 because accum is not wide enough for AVX512
        constexpr size_t registerWidths = chunks<AccumType,256>(outputSize);
        constexpr size_t iterations = registerWidths / 16;
        constexpr size_t remaining = registerWidths % 16;
        if constexpr (iterations > 0) {
             fullUpdateLoop<__m256i, AccumType, WeightType, BiasType, inputSize, outputSize, 16,
                            256, iterations, sizeof(AccumType)>(target, weights, biases, indices, offset);
        }
        if constexpr (remaining > 0) {
             fullUpdateLoop<__m256i, AccumType, WeightType, BiasType, inputSize, outputSize, remaining,
                            256, 1, sizeof(AccumType)>(target, weights, biases, indices, offset);
        }
    } else
#endif
    {
        // generic x86 code, for AVX512, AVX2 or SSE2
        static_assert(outputSize * sizeof(AccumType) * 8 >= simdWidth,
                      "insufficient accumulator width");
        static_assert(outputSize * sizeof(AccumType) * 8 % simdWidth == 0,
                      "accumulator size is not multiple of SIMD width");
        // how many simdWidth registers are needed to process accumulator
        constexpr size_t registerWidths = chunks<AccumType,simdWidth>(outputSize);
        constexpr size_t iterations = registerWidths / simdRegCount;
        constexpr size_t remaining = registerWidths % simdRegCount;
        if constexpr (iterations > 0) {
             fullUpdateLoop<vec_t, AccumType, WeightType, BiasType, inputSize, outputSize, simdRegCount,
                            simdWidth, iterations, sizeof(AccumType)>(target, weights, biases, indices, offset);
        }
        if constexpr (remaining > 0) {
                fullUpdateLoop<vec_t, AccumType, WeightType, BiasType, inputSize, outputSize, remaining,
                               simdWidth, 1, sizeof(AccumType)>(target, weights, biases, indices, offset);
        }
    }
#endif
}

template <typename vec_type, typename AccumType, typename WeightType,
          size_t inputSize /* features */, size_t outputSize /* accumulator size */,
          size_t regCount, size_t regWidth, size_t iterations, typename operations>
inline void updateLoop(const AccumType *source, AccumType *target,
                       const WeightType (&weights)[inputSize][outputSize], const unsigned *added,
                       size_t added_count, const unsigned *removed, size_t removed_count,
                       size_t &offset) {
    static_assert(outputSize * sizeof(AccumType) * 8 >= regWidth,
                  "insufficient accumulator width");
    static_assert(outputSize * sizeof(AccumType) * 8 % regWidth == 0,
                  "accumulator size is not multiple of SIMD register width");
    vec_type *outp = reinterpret_cast<vec_type *>(target);
    const vec_type *inp = reinterpret_cast<const vec_type *>(source);
    alignas(VEC_ALIGN) vec_type regs[regCount];
    for (size_t iter = 0; iter < iterations; ++iter, offset += regCount) {
        // load source into registers
        for (size_t i = 0; i < regCount; ++i) {
            regs[i] = operations::load(inp + offset + i);
        }
        // perform updates in registers
        for (size_t i = 0; i < added_count; ++i) {
            const vec_type *w = reinterpret_cast<const vec_type *>(weights[added[i]]);
            for (size_t j = 0; j < regCount; ++j) {
                regs[j] = operations::add(regs[j], w + offset + j);
            }
        }
        for (size_t i = 0; i < removed_count; ++i) {
            const vec_type *w = reinterpret_cast<const vec_type *>(weights[removed[i]]);
            for (size_t j = 0; j < regCount; ++j) {
                regs[j] = operations::sub(regs[j], w + offset + j);
            }
        }
        // store results to memory
        for (size_t i = 0; i < regCount; ++i) {
            operations::store(outp + offset + i, regs[i]);
        }
    }
}

// Incremental update of 1/2 of the accumulator
template <typename AccumType, typename WeightType, size_t inputSize /* features */,
          size_t outputSize /* accumulator size*/>
void update(const AccumType *source, AccumType *target,
            const WeightType (&weights)[inputSize][outputSize], const unsigned *added,
            size_t added_count, const unsigned *removed, size_t removed_count) {
    // optimized sparse accumulator update, as detailed at
    // https://github.com/official-stockfish/nnue-pytorch/blob/master/docs/nnue.md
    // outputSize is the size of an accumulator half
    static_assert(sizeof(AccumType) == 2 || sizeof(AccumType) == 4, "unsupported accumulator type");
    static_assert(sizeof(AccumType) == sizeof(WeightType),
                  "AccumType different from WeightType not currently supported");
    size_t offset = 0;
#ifdef NEON
    static_assert(outputSize * sizeof(AccumType) * 8 >= simdWidth,
                  "insufficient accumulator width");
    static_assert(outputSize * sizeof(AccumType) * 8 % simdWidth == 0,
                  "accumulator size is not multiple of SIMD width");
    constexpr size_t registerWidths = chunks<AccumType, simdWidth>(outputSize);
    // It is faster here to not reserve all registers for storing intermediate results,
    // because NEON vector operations except load/store operate on registers only.
    constexpr size_t regCount = simdRegCount / 2;
    constexpr size_t iterations = registerWidths / regCount;
    constexpr size_t remaining = registerWidths % regCount;
    if constexpr (iterations > 0) {
        if constexpr (sizeof(AccumType)==2)
            updateLoopNeon<vec16_t, AccumType, WeightType, inputSize, outputSize, regCount, simdWidth,
                       iterations, SimdOperationsNeon<vec16_t, AccumType, sizeof(AccumType)>>(
            source, target, weights, added, added_count, removed, removed_count, offset);
        else
            updateLoopNeon<vec32_t, AccumType, WeightType, inputSize, outputSize, regCount, simdWidth,
                       iterations, SimdOperationsNeon<vec32_t, AccumType, sizeof(AccumType)>>(
            source, target, weights, added, added_count, removed, removed_count, offset);
    }
    if constexpr (remaining > 0) {
        if constexpr (sizeof(AccumType) == 2)
            updateLoopNeon<vec16_t, AccumType, WeightType, inputSize, outputSize, remaining,
                           simdWidth, 1, SimdOperationsNeon<vec16_t, AccumType, sizeof(AccumType)>>(
                source, target, weights, added, added_count, removed, removed_count, offset);
        else
            updateLoopNeon<vec32_t, AccumType, WeightType, inputSize, outputSize, remaining,
                           simdWidth, 1, SimdOperationsNeon<vec32_t, AccumType, sizeof(AccumType)>>(
                source, target, weights, added, added_count, removed, removed_count, offset);
    }
#else
#if defined(AVX512)
    if constexpr (outputSize * sizeof(AccumType) * 8 < simdWidth) {
        static_assert(outputSize * sizeof(AccumType) * 8 % 256 == 0,
                      "expected size to be multiple of 256");
        // special case, fall back to AVX2 because accum is not wide enough for AVX512
        constexpr size_t registerWidths = chunks<AccumType, 256>(outputSize);
        constexpr size_t iterations = registerWidths / 16;
        constexpr size_t remaining = registerWidths % 16;
        if constexpr (iterations > 0) {
            updateLoop<__m256i, AccumType, WeightType, inputSize, outputSize, 16, 256, iterations,
                sizeof(AccumType)>(
                source, target, weights, added, added_count, removed, removed_count, offset);
        }
        if constexpr (remaining > 0) {
            updateLoop<__m256i, AccumType, WeightType, inputSize, outputSize, remaining, 256, 1,
                       sizeof(AccumType)>(
                source, target, weights, added, added_count, removed, removed_count, offset);
        }
    } else
#endif
    {
        static_assert(outputSize * sizeof(AccumType) * 8 >= simdWidth,
                      "insufficient accumulator width");
        static_assert(outputSize * sizeof(AccumType) * 8 % simdWidth == 0,
                      "accumulator size is not multiple of SIMD width");
        // how many simdWidth registers are needed to process accumulator
        constexpr size_t registerWidths = chunks<AccumType, simdWidth>(outputSize);
        constexpr size_t iterations = registerWidths / simdRegCount;
        constexpr size_t remaining = registerWidths % simdRegCount;
        if constexpr (iterations > 0) {
            updateLoop<vec_t, AccumType, WeightType, inputSize, outputSize, simdRegCount, simdWidth,
                       iterations, sizeof(AccumType)>(
                source, target, weights, added, added_count, removed, removed_count, offset);
        }
        if constexpr (remaining > 0) {
            updateLoop<vec_t, AccumType, WeightType, inputSize, outputSize, remaining, simdWidth, 1,
                       sizeof(AccumType)>(
                source, target, weights, added, added_count, removed, removed_count, offset);
        }
    }
#endif
}

template <size_t size, typename InType, typename OutType>
inline void clamp(const InType *in, OutType *out, [[maybe_unused]] InType clampMax) {
    // TBD: can use AVX512 here?
#if defined(NEON)
    int8x16_t *outp = reinterpret_cast<int8x16_t *>(out);
    const vec_t packedZeros = vdupq_n_s16(0);
    const vec_t packedMax = vdupq_n_s16(clampMax);
    size_t j = 0;
    for (size_t i = 0; i < chunks<OutType, simdWidth>(size); ++i, j += 2) {
        vec_t words0 = vld1q_s16(in + 8 * (j + 0));
        vec_t words1 = vld1q_s16(in + 8 * (j + 1));
        vec_t out0 = vminq_s16(vmaxq_s16(words0, packedZeros), packedMax);
        vec_t out1 = vminq_s16(vmaxq_s16(words1, packedZeros), packedMax);
        outp[i] = vcombine_s8(vmovn_s16(out0), vmovn_s16(out1));
    }
#elif defined(AVX2)
    assert(sizeof(InType) == 2);
    assert(sizeof(OutType) == 1);
    const __m256i *inp = reinterpret_cast<const __m256i *>(in);
    __m256i *outp = reinterpret_cast<__m256i *>(out);
    for (size_t i = 0; i < chunks<OutType, 256>(size); ++i) {
        // load 2x256 bit registers of input data
        __m256i words0 = _mm256_load_si256(reinterpret_cast<const __m256i *>(inp + 2 * i + 0));
        __m256i words1 = _mm256_load_si256(reinterpret_cast<const __m256i *>(inp + 2 * i + 1));
        // clamp and store into one 256-bit output chunk
        _mm256_store_si256(
            &outp[i],
            _mm256_permute4x64_epi64(_mm256_max_epi8(_mm256_packs_epi16(words0, words1), zero256),
                                     0b11011000));
    }
#elif defined(SSE2) || defined(SSSE3)
    const vec_t *inp = reinterpret_cast<const vec_t *>(in);
    vec_t *outp = reinterpret_cast<vec_t *>(out);
    __m128i packedZeros = _mm_setzero_si128();
    __m128i packedMax = _mm_set1_epi16(clampMax);
    for (size_t i = 0; i < chunks<OutType, simdWidth>(size); ++i) {
        __m128i out0, out1;
        __m128i words0 = _mm_load_si128(reinterpret_cast<const __m128i *>(inp + 2 * i + 0));
        __m128i words1 = _mm_load_si128(reinterpret_cast<const __m128i *>(inp + 2 * i + 1));
        out0 = _mm_min_epi16(_mm_max_epi16(words0, packedZeros), packedMax);
        out1 = _mm_min_epi16(_mm_max_epi16(words1, packedZeros), packedMax);
        outp[i] = _mm_packs_epi16(out0, out1);
    }
#endif
}

template <typename InType, typename OutType, size_t size, unsigned rshift>
inline void CReLU(const InType *in, OutType *out, [[maybe_unused]] InType clampMax) {
    static_assert(sizeof(InType) == 4 && sizeof(OutType) == 1,
                  "conditions not met for SIMD CRelU implementation");
#ifdef NEON
    int8x16_t *outp = reinterpret_cast<int8x16_t *>(out);
    const vec_t packedZeros = vdupq_n_s16(0);
    const vec_t packedMax = vdupq_n_s16(clampMax);
    size_t j = 0;
    static_assert(size * 8 >= simdWidth && size * 8 % simdWidth == 0,
                  "conditions not met for SIMD CRelU implementation");
    for (size_t i = 0; i < chunks<OutType, simdWidth>(size); ++i, j += 4) {
        int32x4_t r0 = vld1q_s32(in + 4 * (j + 0));
        int32x4_t r1 = vld1q_s32(in + 4 * (j + 1));
        int32x4_t r2 = vld1q_s32(in + 4 * (j + 2));
        int32x4_t r3 = vld1q_s32(in + 4 * (j + 3));
        // shift and narrow
        vec_t words0 = vcombine_s16(vshrn_n_s32(r0, rshift), vshrn_n_s32(r1, rshift));
        vec_t words1 = vcombine_s16(vshrn_n_s32(r2, rshift), vshrn_n_s32(r3, rshift));
        // do min/max
        vec_t out0 = vminq_s16(vmaxq_s16(words0, packedZeros), packedMax);
        vec_t out1 = vminq_s16(vmaxq_s16(words1, packedZeros), packedMax);
        // pack into output
        outp[i] = vcombine_s8(vmovn_s16(out0), vmovn_s16(out1));
    }
#else
#if defined(AVX2)
    if constexpr (size * 8 >= 256) {
        const __m256i *inp = reinterpret_cast<const __m256i *>(in);
        __m256i *outp = reinterpret_cast<__m256i *>(out);
        static_assert(size * 8 % 256 == 0,
                      "conditions not met for SIMD CRelU implementation");
        const __m256i control = _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0);
        for (size_t i = 0; i < chunks<OutType, 256>(size); ++i) {
            // load 2x256 bit registers of shifted input data (32 bit input, 16 bit output)
            const __m256i r1 =
                _mm256_srai_epi16(_mm256_packs_epi32(inp[4 * i + 0], inp[4 * i + 1]), rshift);
            const __m256i r2 =
                _mm256_srai_epi16(_mm256_packs_epi32(inp[4 * i + 2], inp[4 * i + 3]), rshift);
            // clamp and store into one 256-bit output chunk
            outp[i] = _mm256_permutevar8x32_epi32(
                _mm256_max_epi8(_mm256_packs_epi16(r1, r2), zero256), control);
        }
        return;
    } else
#endif
#if defined(AVX2) || defined(SSE2) || defined(SSSE3)
    {
        const __m128i *inp = reinterpret_cast<const __m128i *>(in);
        __m128i *outp = reinterpret_cast<__m128i *>(out);
        static_assert(size * 8 % 128 == 0,
                      "conditions not met for SIMD CRelU implementation");
        for (size_t i = 0; i < chunks<OutType, 128>(size); ++i) {
            // load 2x128 bit registers of shifted input data (32 bit input, 16 bit output) and
            // clamp
            __m128i r1 = _mm_srai_epi16(_mm_packs_epi32(inp[4 * i + 0], inp[4 * i + 1]), rshift);
            __m128i r2 = _mm_srai_epi16(_mm_packs_epi32(inp[4 * i + 2], inp[4 * i + 3]), rshift);
            // pack into 8-bit output and clamp
#ifdef SSE41
            outp[i] = _mm_max_epi8(_mm_packs_epi16(r1, r2), _mm_setzero_si128());
#else
            const __m128i k0x80s = _mm_set1_epi8(-128);
            outp[i] = _mm_subs_epi8(_mm_adds_epi8(_mm_packs_epi16(r1, r2), k0x80s), k0x80s);
#endif
        }
    }
#endif
#endif
}

// performs activation by pairwise multiplication, as in the SFv4 net, transforming the outputs of
// the feature layer into a uint8_t vector
template <typename InType, typename OutType, size_t size, unsigned clampMax, unsigned shift>
static inline void pairwiseMult(const InType *input, OutType *output) {
    // currently assume fixed size types
    static_assert(sizeof(InType) == 2 && sizeof(OutType) == 1);
    static_assert(size * 8 >= simdWidth && size * 8 % simdWidth == 0);

#ifdef NEON
    int8x16_t *outp = reinterpret_cast<int8x16_t *>(output);
    const int16x8_t packedZeros = vdupq_n_s16(0);
    const int16x8_t packedMax = vdupq_n_s16(clampMax);
    const int16x8_t *inp0 = reinterpret_cast<const int16x8_t *>(input);
    const int16x8_t *inp1 = reinterpret_cast<const int16x8_t *>(input + size);
    size_t j = 0;
    for (size_t i = 0; i < chunks<InType, simdWidth>(size / 2); ++i, j += 2) {
        // load + do min/max
        const int16x8_t sum0a = vminq_s16(vmaxq_s16(inp0[j + 0], packedZeros), packedMax);
        const int16x8_t sum0b = vminq_s16(vmaxq_s16(inp0[j + 1], packedZeros), packedMax);
        const int16x8_t sum1a = vminq_s16(vmaxq_s16(inp1[j + 0], packedZeros), packedMax);
        const int16x8_t sum1b = vminq_s16(vmaxq_s16(inp1[j + 1], packedZeros), packedMax);
        // multiply
        const vec_t prod0 = vmulq_s16(sum0a, sum1a);
        const vec_t prod1 = vmulq_s16(sum0b, sum1b);
        // shift + narrow, pack into output register
        outp[i] = vcombine_s8(vshrn_n_s16(prod0, shift), vshrn_n_s16(prod1, shift));
    }
#else
    const vec_t limit = vec_set_16<vec_t>(clampMax);
    const vec_t *inp0 = reinterpret_cast<const vec_t *>(input);
    const vec_t *inp1 = reinterpret_cast<const vec_t *>(input + size);
    vec_t *outp = reinterpret_cast<vec_t *>(output);

    for (size_t i = 0; i < chunks<InType, simdWidth>(size / 2); ++i) {
        const vec_t sum0a = vec_clamp16(inp0[i * 2], limit);
        const vec_t sum0b = vec_clamp16(inp0[i * 2 + 1], limit);
        const vec_t sum1a = vec_clamp16(inp0[i * 2], limit);
        const vec_t sum1b = vec_clamp16(inp1[i * 2 + 1], limit);
        const vec_t prod0 = vec_mullo16(sum0a, sum1a);
        const vec_t prod1 = vec_mullo16(sum0b, sum1b);
#ifdef AVX512
        vec_t compacted =
            _mm512_packs_epi16(_mm512_srli_epi16(prod0, 7), _mm512_srli_epi16(prod1, shift));
        outp[i] = _mm512_permutexvar_epi64(_mm512_setr_epi64(0, 2, 4, 6, 1, 3, 5, 7), compacted);
#elif defined(AVX2)
        vec_t compacted =
            _mm256_packs_epi16(_mm256_srli_epi16(prod0, shift), _mm256_srli_epi16(prod1, shift));
        outp[i] = _mm256_permute4x64_epi64(compacted, 0b11011000);
#elif defined(SSE2)
        outp[i] = _mm_packs_epi16(_mm_srli_epi16(prod0, shift), _mm_srli_epi16(prod1, shift));
#endif
    }
#endif
}

// Combination of piece-wise multitplication with CRelU activation and a linear layer with 1-dimensional output.
// Operations are re-arranged as suggested here: https://github.com/cosmobobak/viridithas/blob/master/nnue-speedups.md#lizard-simd-for-squared-clipped-relu, allowing efficient SIMD execution with 16-bit quantities (originally implemented in LizardChess).
template <typename InType, typename OutType, typename WeightType, size_t inputSize /* features */, size_t outputSize>
static inline void sqrCRelUAndLinear(const InType *input, OutType *output,
                                     const int clampMax, const WeightType *weights) {
    static_assert(sizeof(InType) == 2, "only 16bit is supported");
    static_assert(inputSize*8 >= simdWidth && (inputSize*8) % simdWidth == 0,"width is not multiple of simdWidth");
#ifdef NEON
    const vec_t maxValues = vec_set_16(clampMax);
    vec32_t sum = zero;
#else
    const vec_t maxValues = vec_set_16<vec_t>(clampMax);
    vec_t sum;
    vec_set_zero(sum);
#endif
    const vec_t *inp = reinterpret_cast<const vec_t*>(input);
    const vec_t *w = reinterpret_cast<const vec_t *>(weights);
    constexpr size_t iterations = chunks<InType, simdWidth>(inputSize);
    for (size_t i = 0; i < iterations; ++i) {
        vec_t x = vec_clamp16(vec_load(inp + i), maxValues);
        sum = vec_add32(sum, vec_madd16(vec_mullo16(x,vec_load(w+i)),x));
    }
    // horizontal add output register
#ifdef NEON
    output[0] = add4x32_neon(sum);
#else
    output[0] = hsum32(sum);
#endif
}

// Combination of CRelU activation and a linear layer with 1-dimensional output.
template <typename InType, typename OutType, typename WeightType, size_t inputSize /* features */, size_t outputSize>
static inline void CRelUAndLinear(const InType *input, OutType *output,
                                  const int clampMax, const WeightType *weights) {
    static_assert(sizeof(InType) == 2, "only 16bit is supported");
    vec_t sum;
#ifdef NEON
    const vec_t maxValues = vec_set_16(clampMax);
    sum = zero;
#else
    const vec_t maxValues = vec_set_16<vec_t>(clampMax);
    vec_set_zero(sum);
#endif
    const vec_t *inp = reinterpret_cast<const vec_t*>(input);
    const vec_t *w = reinterpret_cast<const vec_t *>(weights);
    constexpr size_t iterations = chunks<InType, simdWidth>(inputSize);
    for (size_t i = 0; i < iterations; ++i) {
        vec_t x = vec_clamp16(vec_load(inp + i), maxValues);
        // 16bit x 16bit multiplication with weights, add to 32 bit accumulator
        sum = vec_add32(sum, vec_madd16(x, vec_load(w+i)));
    }
    // horizontal add output register
#ifdef NEON
    output[0] = add4x32_neon(sum);
#else
    output[0] = hsum32(sum);
#endif
}

} // namespace simd

#endif
