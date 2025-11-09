// Copyright 2021-2025 by Jon Dart. All Rights Reserved.
#ifndef NNUE_SIMDDEFS_H
#define NNUE_SIMDDEFS_H

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

// Define some constants and a default vector type for each architecture
#ifdef NEON
using vec_t = int16x8_t;
using vec8_t = int8x16_t;
using vec16_t = int16x8_t;
using vec32_t = int32x4_t;
static constexpr size_t VEC_ALIGN = 32;
static constexpr size_t simdWidth = 128;
static constexpr size_t simdRegCount = 16;
using MaskType_t = uint32_t;
#else
#if defined(AVX512)
using vec_t = __m512i;
static constexpr size_t VEC_ALIGN = 64;
static constexpr size_t simdWidth = 512;
static constexpr size_t simdRegCount = 32;
using MaskType_t = uint64_t;
#elif defined(AVX2)
using vec_t = __m256i;
static constexpr size_t VEC_ALIGN = 32;
static constexpr size_t simdWidth = 256;
static constexpr size_t simdRegCount = 16;
using MaskType_t = uint32_t;
#else
using vec_t = __m128i;
static constexpr size_t VEC_ALIGN = 32;
static constexpr size_t simdWidth = 128;
static constexpr size_t simdRegCount = 16;
using MaskType_t = uint32_t;
#endif
using vec8_t = vec_t;
using vec16_t = vec_t;
using vec32_t = vec_t;
#endif // x86

#if defined(NEON)
static inline vec_t vec_load(const vec_t *x) {
    return vld1q_s16(reinterpret_cast<const int16_t *>(x));
}
static inline vec8_t vec_load8(const vec8_t *x) {
    return vld1q_u8(reinterpret_cast<const uint8_t *>(x));
}
static inline vec_t vec_load32(const vec32_t *x) {
    return vld1q_s32(reinterpret_cast<const int32_t *>(x));
}
static inline void vec_store32(vec32_t *x, vec32_t y) { vst1q_s32(reinterpret_cast<int32_t*>(x), y); }
static inline vec16_t vec_set_16(int16_t x) { return vdupq_n_s16(x); }
static inline vec32_t vec_set_32(int32_t x) { return vdupq_n_s32(x); }
static const vec_t ones128 = vec_set_16(1);
static const vec_t zeros128 = vec_set_16(0);
static const vec_t zero = zeros128;
static const vec_t zeros32 = vec_set_32(0);
static inline vec_t vec_add32(vec_t x, vec_t y) { return vaddq_s32(x, y); }
static inline vec_t vec_add16(vec_t x, const vec_t *y) {
    return vaddq_s16(x, vec_load(y));
}
static inline vec_t vec_sub16(vec_t x, const vec_t *y) {
    return vsubq_s16(x, vec_load(y));
}
static inline vec32_t vec_sub32(vec32_t x, const vec32_t *y) {
    return vsubq_s32(x, vec_load32(y));
}
static inline vec_t vec_clamp16(vec_t x, vec_t maxValues) {
    return vminq_s16(vmaxq_s16(x, zero), maxValues);
}
static inline vec_t vec_clamp32(vec_t x, vec_t maxValues) {
    return vminq_s32(vmaxq_s32(x, zero), maxValues);
}
static inline vec_t vec_shl16(vec_t x, unsigned shift) {
    return vshlq_s16(x, vdupq_n_s16(shift));
}
template <unsigned shift>
static inline vec_t vec_shr16(vec_t x) {
    return vshrq_n_s16(x, shift);
}
template <unsigned shift>
static inline vec_t vec_shr32(vec_t x) {
    return vshrq_n_s32(x, shift);
}
static inline vec_t vec_mullo16(vec_t x, vec_t y) { return vmulq_s16(x, y); }
static inline vec_t vec_mulhi16(vec_t x, vec_t y) {
    auto lo = vmull_s16(vget_low_s16(x), vget_low_s16(y));
    auto hi = vmull_s16(vget_high_s16(x), vget_high_s16(y));
    return vcombine_s16(vmovn_s32(vshrq_n_s32(lo, 16)), vmovn_s32(vshrq_n_s32(hi, 16)));
}
static inline vec32_t vec_mullo32(vec32_t x, vec32_t y) { return vmulq_s32(x, y); }
static inline vec_t vec_madd16(vec_t x, vec_t y) {
    const int32x4_t low = vmull_s16(vget_low_s16(x), vget_low_s16(y));
    const int32x4_t high = vmull_high_s16(x, y);
    return vpaddq_s32(low, high);
}
static inline int32_t hsum32(int32x4_t reg) {
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
// must be templatized because shift must be a compile-time constant
template<unsigned shift>
static inline vec32_t vec_rshift32(vec32_t x) { return vshrq_n_s32(x, shift); }
// templatize these for compatibility with x86 code
template <typename T> static inline MaskType_t nnzMask(T x);
template <typename T> static inline MaskType_t nnzMask32(T x);
template <typename T> static inline T vec_set_32(int32_t);

// NEON specialization for nnzMask32, following Stockfish approach
// Efficiently creates a bitmask from a 32-bit vector comparison
template <> inline MaskType_t nnzMask32(vec32_t x) {
    static constexpr uint32_t Mask[4] = {1, 2, 4, 8};
    const uint32x4_t test = vtstq_u32(vreinterpretq_u32_s32(x), vreinterpretq_u32_s32(x));
    const uint32x4_t masked = vandq_u32(test, vld1q_u32(Mask));
#if defined(__aarch64__)
    return vaddvq_u32(masked);
#else
    // 32-bit ARM fallback
    uint32x2_t sum = vadd_u32(vget_low_u32(masked), vget_high_u32(masked));
    sum = vpadd_u32(sum, sum);
    return vget_lane_u32(sum, 0);
#endif
}

template <> vec32_t vec_set_32(int32_t x) {
    return vdupq_n_s32(x);
}

// 16-bit vector operations for calcNnzData (using uint16x8_t for indices)
using vec_u16_t = uint16x8_t;
static inline vec_u16_t vec_load_u16(const uint16_t *x) {
    return vld1q_u16(x);
}
static inline void vec_store_u16(uint16_t *x, vec_u16_t y) {
    vst1q_u16(x, y);
}
static inline vec_u16_t vec_set_u16(uint16_t x) {
    return vdupq_n_u16(x);
}
static inline vec_u16_t vec_add_u16(vec_u16_t x, vec_u16_t y) {
    return vaddq_u16(x, y);
}

static inline void dpbusd_epi32(vec32_t sum, uint8x16_t a, vec8_t b) {
#if defined(__aarch64__)
    return vdotq_s32(sum, vreinterpretq_s8_u8(a), b);
#else
    // emulate Intel's _mm_dpbusd_epi32 instruction with NEON
    uint8x8_t a_lo = vget_low_u8(a);
    int8x8_t b_lo = vget_low_s8(b);
    uint8x8_t a_hi = vget_high_u8(a);
    int8x8_t b_hi = vget_high_s8(b);

    // Create 16-bit vectors and multiply
    int16x8_t p_lo = vmull_s8(b_lo, vreinterpret_s8_u8(a_lo));
    int16x8_t p_hi = vmull_s8(b_hi, vreinterpret_s8_u8(a_hi));

    // Add pairs of 16-bit products
    int32x4_t dp = vpaddlq_s16(p_lo);
    dp = vpadalq_s16(dp, p_hi);

    sum = vaddq_s32(sum, dp);
#endif
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
template <typename T> static inline T vec_add16(T, T);
template <typename T> static inline T vec_add32(T, T);
template <typename T> static inline T vec_sub16(T, const T *);
template <typename T> static inline T vec_sub32(T, const T *);
template <typename T> static inline T vec_clamp16(T x, T maxValues);
template <typename T> static inline T vec_clamp32(T x, T maxValues);
template <typename T> static inline T vec_mullo16(T x, T y);
template <typename T> static inline T vec_mullo32(T x, T y);
template <typename T> static inline T vec_mulhi16(T x, T y);
template <typename T> static inline T vec_madd16(T x, T y);
template <typename T> static inline T vec_rshift32(T x, unsigned shift);
template <typename T> static inline T vec_shl16(T x, unsigned shift);
template <typename T> static inline T vec_shr16(T x, unsigned shift);
template <typename T> static inline T vec_shr32(T x, unsigned shift);
template <typename T> static inline void dpbusd_epi32(T &x, T y, T z);
template <typename T> static inline int32_t hsum32(T x);
template <typename T> static inline MaskType_t nnzMask(T x);
template <typename T> static inline MaskType_t nnzMask32(T x);
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

// Template specializations for each architecture. Multiple specializations
// may be defined.
#ifdef AVX512
static const vec_t ones512 = _mm512_set1_epi16(1);
static const vec_t zero512 = _mm512_setzero_si512();
template <> vec_t vec_set_32<__m512i>(int32_t x) { return _mm512_set1_epi32(x); }
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
template <> __m512i vec_mulhi16(__m512i x, __m512i y) { return _mm512_mulhi_epi16(x, y); }
template <> __m512i vec_madd16(__m512i x, __m512i y) { return _mm512_madd_epi16(x, y); }
template <> __m512i vec_rshift32(__m512i x, unsigned shift) { return _mm512_srai_epi32(x, shift); }
template <> __m512i vec_shl16(__m512i x, unsigned shift) { return _mm512_slli_epi16(x, shift); }
template <> __m512i vec_shr16(__m512i x, unsigned shift) { return _mm512_srai_epi16(x, shift); }
template <> __m512i vec_shr32(__m512i x, unsigned shift) { return _mm512_srai_epi32(x, shift); }
template <> int32_t hsum32(__m512i prod) { return _mm512_reduce_add_epi32(prod); }
template <> MaskType_t nnzMask(__m512i x) {
    // AVX512 returns a 64-bit mask for byte comparisons
    return _mm512_cmpgt_epu8_mask(x, _mm512_setzero_si512());
}
template <> MaskType_t nnzMask32(__m512i x) {
    return _mm512_cmp_epi32_mask(x, _mm512_setzero_si512(), _MM_CMPINT_NE);
}
template <>
[[maybe_unused]]
void dpbusd_epi32(__m512i &acc, __m512i a, __m512i b) {
#ifdef AVX512_VNNI
    acc = _mm512_dpbusd_epi32(acc, a, b);
#else
    __m512i x = _mm512_maddubs_epi16(a, b);
    x = _mm512_madd_epi16(x, ones512);
    acc = _mm512_add_epi32(acc, x);
#endif
}
#endif

#if defined(AVX2)
static const __m256i ones256 = _mm256_set1_epi16(1);
static const __m256i zero256 = _mm256_setzero_si256();
static inline __m256i vec_set_16(int x) { return _mm256_set1_epi16(x); }
template <> __m256i vec_set_32<__m256i>(int32_t x) { return _mm256_set1_epi32(x); }
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
template <> __m256i vec_mulhi16(__m256i x, __m256i y) { return _mm256_mulhi_epi16(x, y); }
template <> __m256i vec_madd16(__m256i x, __m256i y) { return _mm256_madd_epi16(x, y); }
template <> __m256i vec_rshift32(__m256i x, unsigned shift) { return _mm256_srai_epi32(x, shift); }
template <> __m256i vec_shl16(__m256i x, unsigned shift) { return _mm256_slli_epi16(x, shift); }
template <> __m256i vec_shr16(__m256i x, unsigned shift) { return _mm256_srai_epi16(x, shift); }
template <> __m256i vec_shr32(__m256i x, unsigned shift) { return _mm256_srai_epi32(x, shift); }
template <> int32_t hsum32(__m256i x) {
    __m128i sum128 = _mm_add_epi32(_mm256_castsi256_si128(x), _mm256_extracti128_si256(x, 1));
    return hsum32<__m128i>(sum128);
}
template <> MaskType_t nnzMask(__m256i x) {
    return _mm256_movemask_epi8(_mm256_cmpgt_epi8(x, _mm256_setzero_si256()));
}
template <> MaskType_t nnzMask32(__m256i x) {
    return _mm256_movemask_ps(_mm256_castsi256_ps(_mm256_cmpgt_epi32(x, _mm256_setzero_si256())));
}
template <>
[[maybe_unused]] void dpbusd_epi32(__m256i &acc, __m256i a, __m256i b) {
#ifdef VNNI
    acc = _mm256_dpbusd_epi32(acc, a, b);
#else
    __m256i x = _mm256_maddubs_epi16(a, b);
    x = _mm256_madd_epi16(x, ones256);
    acc = _mm256_add_epi32(acc, x);
#endif
}
#endif

#if defined(SSE2) || defined(SSSE3)
static const __m128i ones128 = _mm_set1_epi16(1);
static const __m128i zero128 = _mm_setzero_si128();
template <> __m128i vec_load(const __m128i *x) { return _mm_load_si128(x); }
template <> void vec_store(__m128i *x, __m128i y) { _mm_store_si128(x, y); }
template <> void vec_set_zero(__m128i &x) { x = zero128; }
template <> __m128i vec_add16<__m128i>(__m128i x, __m128i y) {return _mm_add_epi16(x, y); }
template <> __m128i vec_mullo16(__m128i x, __m128i y) { return _mm_mullo_epi16(x, y); }
template <> __m128i vec_mullo32(__m128i x, __m128i y) {
#ifdef SSE41
    return _mm_mullo_epi32(x, y);
#else
    // https://fgiesen.wordpress.com/2016/04/03/sse-mind-the-gap/
    // even and odd lane products
    __m128i evnp = _mm_mul_epu32(x, y);
    __m128i odda = _mm_srli_epi64(x, 32);
    __m128i oddb = _mm_srli_epi64(y, 32);
    __m128i oddp = _mm_mul_epu32(odda, oddb);

    // merge results
    __m128i evn_mask = _mm_setr_epi32(-1, 0, -1, 0);
    __m128i evn_result = _mm_and_si128(evnp, evn_mask);
    __m128i odd_result = _mm_slli_epi64(oddp, 32);

    return _mm_or_si128(evn_result, odd_result);
#endif
}
template <> __m128i vec_mulhi16(__m128i x, __m128i y) { return _mm_mulhi_epi16(x, y); }
template <> __m128i vec_madd16(__m128i x, __m128i y) { return _mm_madd_epi16(x, y); }
template <> __m128i vec_set_16(int16_t x) { return _mm_set1_epi16(x); }
template <> __m128i vec_set_32(int32_t x) { return _mm_set1_epi32(x); }
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
    // SSE2 implementation of clamp: max(x, 0) then min(result, maxValues)
    // Since SSE2 doesn't have _mm_min_epi32/_mm_max_epi32, we use comparison and blending

    // First clamp to minimum (zero): max(x, 0)
    __m128i mask_pos = _mm_cmpgt_epi32(x, zero128);
    __m128i clamped_min = _mm_and_si128(x, mask_pos);

    // Then clamp to maximum: min(clamped_min, maxValues)
    __m128i mask_max = _mm_cmpgt_epi32(maxValues, clamped_min);
    return _mm_or_si128(_mm_and_si128(clamped_min, mask_max),
                       _mm_andnot_si128(mask_max, maxValues));
}
// TBD: not currently used
// template <>
// __m128i vec_madd16(__m128i x, __m128i y) { return _mm256_madd_epi16(x, y); }
template <> __m128i vec_rshift32(__m128i x, unsigned shift) { return _mm_srai_epi32(x, shift); }
template <> __m128i vec_shl16(__m128i x, unsigned shift) { return _mm_slli_epi16(x, shift); }
template <> __m128i vec_shr16(__m128i x, unsigned shift) { return _mm_srai_epi16(x, shift); }
template <> __m128i vec_shr32(__m128i x, unsigned shift) { return _mm_srai_epi32(x, shift); }
template <> MaskType_t nnzMask(__m128i x) {
    // set dest vector to 0xff for non-zero bytes in x, 0 for zero bytes
    return _mm_movemask_epi8(_mm_cmpgt_epi8(x, zero128));
}
template <> MaskType_t nnzMask32(__m128i x) {
    return _mm_movemask_ps(_mm_castsi128_ps(_mm_cmpgt_epi32(x, zero128)));
}
template <>
void dpbusd_epi32(__m128i &x, __m128i y, __m128i z) {
#ifdef VNNI
    x = _mm_dpbusd_epi32(x, y, z);
#elif defined(SSE3)
    auto sum32 = _mm_madd_epi16(_mm_maddubs_epi16(y, z), _mm_set1_epi16(1));
    x = _mm_add_epi32(sum32, x);
#else
    // SSE2 implementation: emulate _mm_maddubs_epi16 manually
    // _mm_maddubs_epi16 multiplies unsigned bytes with signed bytes and adds adjacent pairs
    // Split into even and odd bytes
    __m128i y_even = _mm_and_si128(y, _mm_set1_epi16(0x00FF));
    __m128i y_odd = _mm_srli_epi16(y, 8);
    __m128i z_even = _mm_srai_epi16(_mm_slli_epi16(z, 8), 8); // sign-extend even bytes
    __m128i z_odd = _mm_srai_epi16(z, 8); // sign-extend odd bytes
    // Multiply and add adjacent pairs to get 16-bit results (emulating _mm_maddubs_epi16)
    __m128i prod = _mm_add_epi16(_mm_mullo_epi16(y_even, z_even), _mm_mullo_epi16(y_odd, z_odd));
    // Horizontal add pairs of 16-bit values to 32-bit
    __m128i sum32 = _mm_madd_epi16(prod, ones128);
    x = _mm_add_epi32(sum32, x);
#endif
}
#endif

#if !defined(_MSC_VER) || defined(__clang__)
#pragma GCC diagnostic pop
#endif

#ifdef NEON
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

#endif // x86

} // namespace

#endif
