// Copyright 2021-2025 by Jon Dart. All Rights Reserved.
#ifndef NNUE_SIMD_H
#define NNUE_SIMD_H

#include "simddefs.h"
#include "../bitutil.h"

namespace simd {

template <typename T, unsigned simdWidth> inline static constexpr size_t chunks(unsigned len) {
    return (len * 8 * sizeof(T)) / simdWidth;
}

template <size_t size, typename DataType> FORCEINLINE void vec_copy(const DataType *input, DataType *output) {
    constexpr size_t bits = size * 8 * sizeof(DataType);
    if constexpr (bits < 128) {
        // fall back to memcpy
        std::memcpy(reinterpret_cast<char*>(output), reinterpret_cast<const char*>(input), sizeof(DataType)*size);
        return;
    }
    else if constexpr (bits >= simdWidth && bits % simdWidth == 0) {
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

template <size_t inputSize, size_t roundedInputSize, size_t outputSize, unsigned inputDequantifyShift,
          unsigned outputDequantifyShift>
inline void i8dotProductnxn(const uint8_t *input,
                            const int8_t weights[outputSize][roundedInputSize],
                            const int32_t *biases, int32_t *output) {
#ifdef AVX512
    if constexpr (inputSize >= 64 && inputSize % 64 == 0) {
        for (unsigned i = 0; i < outputSize; i++) {
            vec_t prod = zero512;
            const vec_t *w = reinterpret_cast<const vec_t *>(weights[i]);
            // 64 = 512/8
            const vec_t *inp = reinterpret_cast<const vec_t *>(input);
            for (unsigned j = 0; j < inputSize/64; ++j) {
                madd_dpbusd_epi32(prod, inp[j], w[j]);
            }
            output[i] = ((hsum32(prod) >> inputDequantifyShift) + biases[i]) >> outputDequantifyShift;
        }
        return;
    }
#endif
#ifdef AVX2
    if constexpr (inputSize >= 32 && inputSize % 32 == 0) {
        for (unsigned i = 0; i < outputSize; i++) {
            __m256i prod = _mm256_setzero_si256();
            const __m256i *w = reinterpret_cast<const __m256i *>(weights[i]);
            const __m256i *inp = reinterpret_cast<const __m256i *>(input);
            for (unsigned j = 0; j < inputSize/32; ++j) {
                dpbusd_epi32(prod, inp[j], w[j]);
            }
            output[i] = ((hsum32(prod) >> inputDequantifyShift) + biases[i]) >> outputDequantifyShift;
        }
    }
#elif defined(SSSE3)
    const vec_t *inp = reinterpret_cast<const vec_t *>(input);
    const vec_t ones = _mm_set1_epi16(1);
    for (unsigned i = 0; i < outputSize; i++) {
        const vec_t *row = reinterpret_cast<const vec_t *>(weights + i);
        vec_t total = _mm_setzero_si128();
        for (unsigned j = 0; j < chunks<uint8_t, simdWidth>(inputSize) / 2; ++j) {
            vec_t p0 = _mm_madd_epi16(_mm_maddubs_epi16(inp[2 * j + 0], row[2 * j + 0]), ones);
            vec_t p1 = _mm_madd_epi16(_mm_maddubs_epi16(inp[2 * j + 1], row[2 * j + 1]), ones);
            vec_t sum = _mm_add_epi32(p0, p1);
            sum = _mm_add_epi32(sum, _mm_shuffle_epi32(sum, 0xb));
            total = _mm_add_epi32(total, sum);
        }
#ifdef SSE41
        auto prodsum = _mm_cvtsi128_si32(total) + _mm_extract_epi32(total, 1);
#else
        total = _mm_add_epi32(total, _mm_shuffle_epi32(total, 1));
        auto prodsum = _mm_cvtsi128_si32(total);
#endif
        output[i] =  ((prodsum >> inputDequantifyShift) + biases[i]) >> outputDequantifyShift;
    }
#elif defined(SSE2)
    const vec_t zeros = _mm_setzero_si128();
    const vec_t *inp = reinterpret_cast<const vec_t *>(input);
    for (unsigned i = 0; i < outputSize; i++) {
        __m128i sum_lo = _mm_cvtsi32_si128(biases[i]);
        __m128i sum_hi = zeros;
        const auto row = reinterpret_cast<const vec_t *>(&weights[i]);
        for (unsigned j = 0; j < chunks<uint8_t, simdWidth>(inputSize); ++j) {
            __m128i row_j = _mm_load_si128(&row[j]);
            __m128i input_j = _mm_load_si128(&inp[j]);
            __m128i row_signs = _mm_cmpgt_epi8(zeros, row_j);
            __m128i extended_row_lo = _mm_unpacklo_epi8(row_j, row_signs);
            __m128i extended_row_hi = _mm_unpackhi_epi8(row_j, row_signs);
            __m128i extended_input_lo = _mm_unpacklo_epi8(input_j, zeros);
            __m128i extended_input_hi = _mm_unpackhi_epi8(input_j, zeros);
            __m128i product_lo = _mm_madd_epi16(extended_row_lo, extended_input_lo);
            __m128i product_hi = _mm_madd_epi16(extended_row_hi, extended_input_hi);
            sum_lo = _mm_add_epi32(sum_lo, product_lo);
            sum_hi = _mm_add_epi32(sum_hi, product_hi);
        }
        __m128i sum = _mm_add_epi32(sum_lo, sum_hi);
        __m128i sum_high_64 = _mm_shuffle_epi32(sum, _MM_SHUFFLE(1, 0, 3, 2));
        sum = _mm_add_epi32(sum, sum_high_64);
        __m128i sum_second_32 = _mm_shufflelo_epi16(sum, _MM_SHUFFLE(1, 0, 3, 2));
        int32_t final_sum = _mm_cvtsi128_si32(_mm_add_epi32(sum, sum_second_32));
        output[i] =  ((final_sum >> inputDequantifyShift) + biases[i]) >> outputDequantifyShift;
    }
#elif defined(NEON)
    const int8x8_t *inp = reinterpret_cast<const int8x8_t *>(input);
    for (unsigned i = 0; i < outputSize; ++i) {
        const int8x8_t *row = reinterpret_cast<const int8x8_t *>(weights[i]);
        int32x4_t accum = vmovq_n_s32(0);
        for (unsigned j = 0; j < chunks<uint8_t, simdWidth / 2>(inputSize); j += 2) {
            // parallel multiply 64-bit chunks into product register
            vec_t prod = vmull_s8(inp[j], row[j]);
            // multiply and add next 64 bits
            prod = vmlal_s8(prod, inp[j + 1], row[j + 1]);
            // sum the products
            accum = vpadalq_s16(accum, prod);
        }
        output[i] = ((hsum32(accum) >> inputDequantifyShift) + biases[i]) >> outputDequantifyShift;
    }
#endif
}

#ifndef NEON
// specialization for a particular x86 architecture, using vec_type registers
template <typename vec_type, unsigned simdWidth, size_t inputSize, size_t roundedInputSize, size_t outputSize,
          unsigned inputDequantifyShift, unsigned outputDequantifyShift>
static inline void i32dotProductnxn(const int32_t *input,
                                    const int32_t weights[outputSize][roundedInputSize],
                                    const int32_t *biases, int32_t *output) {
    using InputType = uint32_t;
    constexpr unsigned inputChunks = chunks<InputType, simdWidth>(inputSize);
    static_assert(inputChunks > 0);
    const vec_type *inp = reinterpret_cast<const vec_type *>(input);
    for (size_t i = 0; i < outputSize; ++i) {
        // Weights are column order
        vec_type sum;
        vec_set_zero(sum);
        const vec_type *col = reinterpret_cast<const vec_type *>(weights[i]);
        for (size_t j = 0; j < inputChunks; ++j) {
            auto x = vec_load<vec_type>(inp + j);
            auto w = vec_load<vec_type>(col + j);
            sum = vec_add32(sum,vec_mullo32<vec_type>(x,w));
        }
        int32_t out = hsum32(sum);
        if constexpr (inputDequantifyShift) {
            out >>= inputDequantifyShift;
        }
        out += biases[i];
        if constexpr (outputDequantifyShift) {
            out >>= outputDequantifyShift;
        }
        output[i] = out;
    }
}
#endif

// dot product, input of at least size 4 (to allow 128-bit vector operations)
template <typename InputType, typename WeightType, typename BiasType, typename OutputType,
          size_t inputSize, size_t roundedInputSize, size_t outputSize, unsigned inputDequantifyShift,
          unsigned outputDequantifyShift>
inline void dotProductnxn(const InputType *input,
                          const WeightType weights[outputSize][roundedInputSize],
                          const BiasType *biases, OutputType *output) {
    static_assert(inputSize * sizeof(InputType) >= 128);
    if constexpr (sizeof(InputType) == 1) {
        i8dotProductnxn<inputSize, roundedInputSize, outputSize, inputDequantifyShift,
                        outputDequantifyShift>(input, weights, biases, output);
    } else if constexpr (sizeof(InputType) == 4) {
#ifdef NEON
        constexpr unsigned inputChunks = chunks<InputType, simdWidth>(inputSize);
        static_assert(inputChunks > 0);
        const vec32_t *inp = reinterpret_cast<const vec32_t *>(input);
        for (size_t i = 0; i < outputSize; ++i) {
            // Weights are column order
            vec32_t sum = vec_set_32(0);
            const vec32_t *col = reinterpret_cast<const vec32_t *>(weights[i]);
            for (size_t j = 0; j < inputChunks; ++j) {
                auto x = vec_load32(inp + j);
                auto w = vec_load32(col + j);
                sum = vec_add32(sum,vec_mullo32(x,w));
            }
            int32_t out = hsum32(sum);
            if constexpr (inputDequantifyShift) {
                out >>= inputDequantifyShift;
            }
            out += biases[i];
            if constexpr (outputDequantifyShift) {
                out >>= outputDequantifyShift;
            }
            output[i] = out;
        }
        return;
#endif
#ifdef AVX512
        if (chunks<int32_t, 512>(inputSize) >= 1) {
            // wide enough for avx512
            i32dotProductnxn<__m512i, 512, inputSize, roundedInputSize, outputSize,
                             inputDequantifyShift, outputDequantifyShift>(input, weights, biases,
                                                                          output);
            return;
        }
#endif
#ifdef AVX2
        if (chunks<int32_t, 256>(inputSize) >= 1) {
            // AVX512 falls back to here if the input is not wide enough
            i32dotProductnxn<__m256i, 256, inputSize, roundedInputSize, outputSize, inputDequantifyShift,
                             outputDequantifyShift>(input, weights, biases, output);
            return;
        }
#endif
#if defined(SSE2) || defined(SSE3)
        i32dotProductnxn<__m128i, 128, inputSize, roundedInputSize, outputSize,
                         inputDequantifyShift, outputDequantifyShift>(input, weights, biases,
                                                                      output);
#endif
    }
}

#ifndef NEON
template <typename vec_type, unsigned vec_width, typename InType, typename OutType, size_t size, unsigned clampMax, unsigned rshift>
static inline void x86CReLU(const InType *in, OutType *out) {
    const vec_type *inp = reinterpret_cast<const vec_type *>(in);
    vec_type *outp = reinterpret_cast<vec_type *>(out);
    vec_type maxValues = vec_set_32<vec_type>(clampMax);
    for (size_t i = 0; i < chunks<InType, vec_width>(size); ++i) {
        // load
        auto x = vec_load<vec_type>(inp + i);
        // shift and clamp
        if constexpr (rshift) {
            x = vec_rshift32(x, rshift);
        }
        x = vec_clamp32<vec_type>(x, maxValues);
        // store
        vec_store<vec_type>(outp + i, x);
    }
}

template <typename vec_type, unsigned vec_width, typename InType, typename OutType, size_t size, unsigned clampMax, unsigned rshift>
static inline void x86SqrCReLU(const InType *in, OutType *out) {
    const vec_type *inp = reinterpret_cast<const vec_type *>(in);
    vec_type *outp = reinterpret_cast<vec_type *>(out);
    vec_type maxValues = vec_set_32<vec_type>(clampMax);
    for (size_t i = 0; i < chunks<InType, vec_width>(size); ++i) {
        // load
        auto x = vec_load<vec_type>(inp + i);
        x = vec_clamp32<vec_type>(x, maxValues);
        x = vec_mullo32<vec_type>(x, x);
        if constexpr (rshift) {
            x = vec_rshift32(x, rshift);
        }
        // store
        vec_store<vec_type>(outp + i, x);
    }
}
#endif

template <typename InType, typename OutType, size_t size, unsigned clampMax, unsigned rshift>
static inline void cReLU(const InType *in, OutType *out) {
    // 32-bit only, currently
#ifdef NEON
    vec32_t maxValues = vec_set_32(clampMax);
    const vec32_t *inp = reinterpret_cast<const vec32_t*>(in);
    vec32_t *outp = reinterpret_cast<vec32_t*>(out);
    for (size_t i = 0; i < chunks<InType, simdWidth>(size); ++i) {
        // load
        auto x = vec_load32(inp + i);
        // shift and clamp
        if constexpr (rshift != 0) {
            x = vec_rshift32<rshift>(x);
        }
        x = vec_clamp32(x, maxValues);
        // store
        vec_store32(outp + i, x);
    }
#else
    // no AVX512 yet
#if defined(AVX2)
    if constexpr (size * 8 >= 256) {
        return x86CReLU<__m256i, 256, InType, OutType, size, clampMax, rshift>(in, out);
    } else
#endif
#if defined(AVX2) || defined(SSE2) || defined(SSSE3)
        return x86CReLU<__m128i, 128, InType, OutType, size, clampMax, rshift>(in, out);
#endif
#endif // ifdef NEON
}

template <typename InType, typename OutType, size_t size, unsigned clampMax, unsigned rshift>
static inline void sqrCReLU(const InType *in, OutType *out) {
    // 32-bit only, currently
    static_assert(sizeof(InType) == 4 && sizeof(OutType) == 4);
#ifdef NEON
    vec32_t maxValues = vec_set_32(clampMax);
    const vec32_t *inp = reinterpret_cast<const vec32_t*>(in);
    vec32_t *outp = reinterpret_cast<vec32_t*>(out);
    for (size_t i = 0; i < chunks<InType, simdWidth>(size); ++i) {
        // load
        auto x = vec_load32(inp + i);
        x = vec_clamp32(x, maxValues);
        x = vec_mullo32(x, x);
        if constexpr (rshift != 0) {
            x = vec_rshift32<rshift>(x);
        }
        // store
        vec_store32(outp + i, x);
    }
#else
    // no AVX512 yet
#if defined(AVX2)
    if constexpr (size * 8 >= 256) {
        return x86SqrCReLU<__m256i, 256, InType, OutType, size, clampMax, rshift>(in, out);
    } else
#endif
#if defined(AVX2) || defined(SSE2) || defined(SSSE3)
        return x86SqrCReLU<__m128i, 128, InType, OutType, size, clampMax, rshift>(in, out);
#endif
#endif // ifdef NEON
}

// performs activation by pairwise multiplication, as in the SFv4 net, transforming the outputs of
// the 16-bit feature layer into a uint8_t vector
template <typename InType, typename OutType, size_t size, unsigned clampMax, unsigned scalingShift, unsigned outputQuant>
static inline void pairwiseMult(const InType *input, OutType *output) {
    // currently assumes fixed size types
    // Note: inputs are clamped to 0..255 range. However, when multiplying, 255 * 255 = 65025, which
    // produces 16-bit signed integer overflow. Multiplication must be signed, because the x86 architecture
    // does not have unsigned SIMD integer multiply except for 32-bit inputs. Therefore we use the approach
    // found in several engines including Obsidian, Plentychess, etc:
    // 1. Shift one of the operands left by a scaling factor
    // 2. Multiply by the other operand, retaining the high 16 bits
    // 3. Pack the high bits into an 8-bit result vector. Scaling factor is chosen to put the
    // outputs into the desired range.
    static_assert(sizeof(InType) == 2 && sizeof(OutType) == 1);
    static_assert((size * 8) >= simdWidth && (size * 8) % simdWidth == 0);

#ifdef NEON
    vec16_t *outp = reinterpret_cast<vec16_t *>(output);
    const vec16_t packedMax = vec_set_16(clampMax);
    const vec16_t *inp0 = reinterpret_cast<const vec16_t *>(input);
    const vec16_t *inp1 = reinterpret_cast<const vec16_t *>(input + (size / 2));
    for (size_t i = 0; i < chunks<InType, simdWidth>(size / 2); i += 2) {
        // load + do min/max
        const vec_t sum0a = vec_clamp16(inp0[i], packedMax);
        const vec_t sum0b = vec_clamp16(inp0[i + 1], packedMax);
        const vec_t sum1a = vec_clamp16(inp1[i], packedMax);
        const vec_t sum1b = vec_clamp16(inp1[i + 1], packedMax);
        // multiply and apply shift
        auto prod0 = vec_mulhi16(vec_shl16(sum0a, scalingShift), sum1a);
        auto prod1 = vec_mulhi16(vec_shl16(sum0b, scalingShift), sum1b);
        outp[i/2] = vcombine_u8(vqmovun_s16(prod0), vqmovun_s16(prod1));
    }
#else
    const vec_t limit = vec_set_16<vec_t>(clampMax);
    const vec_t *inp0 = reinterpret_cast<const vec_t *>(input);
    const vec_t *inp1 = reinterpret_cast<const vec_t *>(input + (size / 2));
    vec_t *outp = reinterpret_cast<vec_t *>(output);

    const size_t iterations = chunks<InType, simdWidth>(size/2);
    assert( iterations > 1 && (iterations % 2) == 0);
    for (size_t i = 0; i < iterations; i += 2) {
        const vec_t sum0a = vec_clamp16(inp0[i], limit);
        const vec_t sum0b = vec_clamp16(inp0[i + 1], limit);
        const vec_t sum1a = vec_clamp16(inp1[i], limit);
        const vec_t sum1b = vec_clamp16(inp1[i + 1], limit);
        // multiply and apply shift
        auto prod0 = vec_mulhi16(vec_shl16(sum0a, scalingShift), sum1a);
        auto prod1 = vec_mulhi16(vec_shl16(sum0b, scalingShift), sum1b);
        // AVX2, AVX512 pack instructions interleave their inputs, so must
        // permute the output to place it in the proper order.
#ifdef AVX512
        vec_t compacted = _mm512_packus_epi16(prod0, prod1);
        outp[i/2] = _mm512_permutexvar_epi64(_mm512_setr_epi64(0, 2, 4, 6, 1, 3, 5, 7), compacted);
#elif defined(AVX2)
        vec_t compacted = _mm256_packus_epi16(prod0, prod1);
        outp[i/2] =  _mm256_permute4x64_epi64(compacted, 0b11011000);
#elif defined(SSE2)
        outp[i/2] = _mm_packus_epi16(prod0, prod1);
#endif
    }
#endif
}

// Lookup table for non-zero indices in a byte
static const class BitmaskToIndices {

public:

    BitmaskToIndices() {
        for (unsigned i = 0; i < 256; ++i) {
            uint64_t j = i, k = 0;
            data[i].dataAsInt64 = 0;
            while (j) {
                // note: bit indices start at 1
                data[i].dataAsArray[k++] = BitUtils::firstOne(j) + 1;
                j &= j - 1;
            }
        }
    }

    const auto &lookup(uint8_t x) const noexcept {
        return data[x].dataAsArray;
    }

private:
    union DataUnion {
        uint8_t dataAsArray[8];
        uint64_t dataAsInt64;
    };

    alignas(64) DataUnion data[256];

} bitmaskToIndices;

#ifdef NEON
template<int lane, typename IndexType>
static inline void check_lane(uint64x2_t comp_u64, unsigned i, unsigned& lastGroupIdx, IndexType* nzIndices, size_t& nzCount) {
    if (vgetq_lane_u32(vreinterpretq_u32_u64(comp_u64), lane) != 0) {
        unsigned groupIdx = (i / 4) + lane;
        if (lastGroupIdx != groupIdx) {
            nzIndices[nzCount++] = groupIdx;
            lastGroupIdx = groupIdx;
        }
    }
}
#endif

 // compute a vector of group indices (groups of 4 consecutive elements) for which
// at least one element in the group is non-zero. This is optimized for DPBUSD
// operations which work on 4-element groups.
template <typename IndexType>
void calcNnzData(const uint8_t *input, size_t inputSize, IndexType *nzIndices, size_t &nzCount) {
    // bytes that will fit in a vector register of simdWidth bits:
    static unsigned inputChunkSize = simdWidth / (8 * sizeof(int8_t));
    nzCount = 0;
    unsigned lastGroupIdx = UINT_MAX;
#if defined(NEON)
    // We do not emulate the x86 approach, because NEON has no direct equivalent of
    // instructions such as _mm512_cmpgt_epu8_mask that pack the results of a comparision
    // operation into a mask vector.
    for (unsigned i = 0; i < inputSize; i += inputChunkSize) {
        auto x = vec_load8(reinterpret_cast<const vec8_t*>(input + i));
        uint8x16_t input_u8 = vreinterpretq_u8_s8(x);
        uint8x16_t comp = vcgtq_u8(input_u8, vdupq_n_u8(0));
        uint64x2_t comp_u64 = vreinterpretq_u64_u8(comp);

        // check groups of 4 bytes
        check_lane<0>(comp_u64, i, lastGroupIdx, nzIndices, nzCount);
        check_lane<1>(comp_u64, i, lastGroupIdx, nzIndices, nzCount);
        check_lane<2>(comp_u64, i, lastGroupIdx, nzIndices, nzCount);
        check_lane<3>(comp_u64, i, lastGroupIdx, nzIndices, nzCount);
    }
#else
    for (unsigned i = 0; i < inputSize; i += inputChunkSize) {
        // obtain bitmask of the non-zero values in the current input chunk of simdWidth bits
        MaskType_t mask = nnzMask(vec_load(reinterpret_cast<const vec_t*>(input + i)));
        // process the output one byte at a time
        for (unsigned j = 0; j < sizeof(MaskType_t) / sizeof(int8_t); ++j) {
            const uint8_t byte = static_cast<uint8_t>((mask >> (j * 8)) & 0xFF); // next byte to process
            if (byte == 0) continue; // skip if no bits set
            auto indexLookup = bitmaskToIndices.lookup(byte); // list of indices

            // Process individual elements but group them into sets of 4.
            // Track which groups of 4 have at least one non-zero element.
            for (unsigned k = 0; k < 8 && indexLookup[k]; ++k) {
                // -1 to correct for addition of 1 in the lookup table
                unsigned elementIdx = i + j * 8 + indexLookup[k] - 1;
                unsigned groupIdx = elementIdx / 4; // group index for DPBUSD

                // Only add if this is a new group (avoid duplicates)
                if (groupIdx != lastGroupIdx) {
                    nzIndices[nzCount++] = groupIdx;
                    lastGroupIdx = groupIdx;
                }
            }
        }
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
    // horizontal sum output register
    output[0] = hsum32(sum);
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
    output[0] = hsum32(sum);
}

} // namespace simd

#endif
