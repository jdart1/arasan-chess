// Copyright 2021-2022, 2024-2025 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_SPARSE_LINEAR_H
#define _NNUE_SPARSE_LINEAR_H

#include "nndefs.h"
#include "linear.h"
#include "util.h"
#include <array>
#include <cstdint>

// This class defines a linear transformation layer of the NNUE, optimized for sparse 8-bit inputs
//
// If inputDequantifyShift is nonzero, is applied to the product part of the output as a right shift.
// If outputDequantifyShift is nonzero, is applied to the complete output (bias + product) as a right shift
//.
// Assuming a sparse input vector, the following optimizations are applied as in Stockfish, Obsidian, etc.:
// 1. Analyzing the input vector to determine which inputs are non-zero. The code will process only these.
// 2. Re-arrangement of the weights to enable use of combined multipy-add SIMD instructions on groups of
// values.
//
template <typename InputType, typename WeightType, typename BiasType, typename OutputType,
          size_t inputSize, size_t outputSize, size_t buckets, unsigned inputDequantifyShift,
          unsigned outputDequantifyShift,
          bool transpose = false,
          size_t alignment = DEFAULT_ALIGN>
class SparseLinear :  public LinearLayer<InputType, WeightType, BiasType, OutputType, inputSize, outputSize,
                                         buckets, inputDequantifyShift, outputDequantifyShift, transpose, alignment> {

#ifndef SIMD
    static_assert(0,"SparseLinear class requires SIMD suppport");
#endif

protected:

    struct NnzData {
        alignas(alignment) PackedIndexType nzIndices[inputSize];
        size_t nzCount;
        NnzData() : nzCount(0) {
        }
    };

    static constexpr unsigned ChunkSize = 4;
    static constexpr unsigned OutputSimdWidth = sizeof(simd::vec_t) / sizeof(OutputType);
    static constexpr unsigned NumAccums = outputSize / OutputSimdWidth;

public:
    SparseLinear() {
    }

    virtual ~SparseLinear() = default;

    static_assert( sizeof(InputType) == 1 );
    static_assert( (sizeof(InputType) * inputSize) >= simd::simdWidth / 8 );

    // propagate data through the layer
    virtual inline void forward(size_t bucket, const InputType *input,
                                OutputType *output) const noexcept {

        // compute data on non-zero indices
        NnzData nnzContext;
        calcNnzData(input, nnzContext);

        // perform SIMD-optimized dot product
        dotProductOptimized(bucket, input, output, nnzContext);

#ifdef NNUE_TRACE
        this->trace(output);
#endif
    }

    inline void dotProductOptimized(size_t bucket, const InputType *input, OutputType *output,
                                    const NnzData &nnzContext) const noexcept {
        using namespace simd;

        vec_t accum[NumAccums];
        // fill the output values with zeros
        for (size_t i = 0; i < NumAccums; ++i) {
#ifdef NEON
            accum[i] = zeros32;
#else
            vec_set_zero(accum[i]);
#endif
        }

        const auto getWeights = [this, bucket](unsigned i) {
            return reinterpret_cast<const vec_t*>(&_optimizedWeights[bucket][i * outputSize * ChunkSize]);
        };

        const auto input32 = reinterpret_cast<const int32_t*>(input);
        auto nnzPtr = nnzContext.nzIndices;
        // TBD: unroll loop
        for (;
             nnzPtr < nnzContext.nzIndices + nnzContext.nzCount;
             ++nnzPtr) {
            const unsigned i = static_cast<unsigned>(*nnzPtr);
            const vec_t in = vec_set_32<vec_t>(input32[i]);
            const auto col = getWeights(i);
            for (unsigned k = 0; k < NumAccums; ++k) {
                dpbusd_epi32(accum[k], in, col[k]);
            }
        }

        // add biases and copy to output, applying scaling if needed
        vec_t *out = reinterpret_cast<vec_t *>(output);
        for (size_t outIdx = 0; outIdx < NumAccums; ++outIdx) {
            if constexpr (inputDequantifyShift) {
#ifdef NEON
                accum[outIdx] = vec_shr32<inputDequantifyShift>(accum[outIdx]);
#else
                accum[outIdx] = vec_shr32(accum[outIdx], inputDequantifyShift);
#endif
            }
            // add biases
#ifdef NEON
            auto bias = vec_load32(reinterpret_cast<const vec32_t *>(this->_biases[bucket]) + outIdx);
#else
            vec_t bias = vec_load<vec_t>(reinterpret_cast<const vec_t *>(this->_biases[bucket]) + outIdx);
#endif
            accum[outIdx] = vec_add32(accum[outIdx],bias);
            if constexpr (outputDequantifyShift) {
#ifdef NEON
                accum[outIdx] = vec_shr32<outputDequantifyShift>(accum[outIdx]);
#else
                accum[outIdx] = vec_shr32(accum[outIdx], outputDequantifyShift);
#endif
            }
#ifdef NEON
            vec_store32(reinterpret_cast<vec32_t*>(&out[outIdx]), accum[outIdx]);
#else
            vec_store<vec_t>(reinterpret_cast<vec_t*>(&out[outIdx]), accum[outIdx]);
#endif
        }
    }

    virtual void postProcess() {
        // Convert the base class weight layout into the SIMD-optimized version this class uses.
        // Weights are grouped into units of 4 (= ratio of output size to input size), to
        // facilitate use of DPBUSD type vector instructions.
        //
        // Weights should have layout, within each bucket:
        //
        // weight for input index0 and output outIdx + 0
        // weight for input index1 and output outIdx + 0
        // weight for input index2 and output outIdx + 0
        // weight for input index3 and output outIdx + 0
        // weight for input index0 and output outIdx + 1
        // weight for input index1 and output outIdx + 1
        // weight for input index2 and output outIdx + 1
        // weight for input index3 and output outIdx + 1
        // .. etc.
        for (size_t b = 0; b < buckets; ++b) {
            for (size_t i = 0; i < inputSize * outputSize; ++i) {
                _optimizedWeights[b][transformedWeightIndex(i)] = this->_weights[b][i / inputSize][i % inputSize];
            }
        }

    }

  protected:
    // optimized weight layout. We maintain this in addition to the standard layout in the
    // subclass. This wastes some space, but is useful for testing.
    alignas(alignment) WeightType _optimizedWeights[buckets][inputSize * outputSize];

    size_t transformedWeightIndex(size_t i) const noexcept {
         return (i / ChunkSize) % (inputSize / ChunkSize) * outputSize * ChunkSize
             + i / inputSize * ChunkSize + i % ChunkSize;
    }

    void calcNnzData(const InputType *input, NnzData &nzInputs) const noexcept {
        simd::calcNnzData<inputSize>(input, nzInputs.nzIndices, nzInputs.nzCount);
    }


};

#endif
