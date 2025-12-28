// Copyright 2021-2022, 2024-2025 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_SPARSE_LINEAR_H
#define _NNUE_SPARSE_LINEAR_H

#include "nndefs.h"
#include "linear.h"
#include "util.h"
#include <array>
#include <cstdint>

// This class defines a linear transformation layer of the NNUE, optimized for sparse 8-bit inputs.
// If inputDequantifyShift is nonzero, is applied to the product part of the output as a right shift.
// If outputDequantifyShift is nonzero, is applied to the complete output (bias + product) as a right shift.
// Assuming a sparse input vector, the following optimizations are applied as in Stockfish, Obsidian, etc.:
// 1. Analyzing the input vector to determine groups of 4 input values for which any values are non-zero.
// The code will process only these.
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
        using IndexType = uint16_t;

        alignas(alignment) std::array<IndexType,inputSize> nzIndices;
        size_t nzCount;
        NnzData() : nzCount(0) {
        }
    };



public:
    SparseLinear() {
    }

    virtual ~SparseLinear() = default;

    static_assert( sizeof(InputType) == 1 );
    static_assert( (sizeof(InputType) * inputSize) >= simd::simdWidth / 8 );

    // propagate data through the layer
    virtual inline void forward(size_t bucket, const InputType *input,
                                OutputType *output) const noexcept {

        // compute array of index groups for which at least one input is non-zero
        NnzData nnzContext;
        calcNnzData(input, nnzContext);

        // perform SIMD-optimized dot product
        dotProductOptimized(bucket, input, output, nnzContext);

#ifdef NNUE_TRACE
        this->trace(output);
#endif
    }

    inline void dotProductOptimized(size_t bucket, const InputType *input,
                                    OutputType *output, const NnzData &nnzContext) const noexcept {
        using namespace simd;

        static constexpr size_t outputChunkSize = simdWidth / (8*sizeof(OutputType)); // 32 bit quantities in a vector register
        static_assert(outputSize >= outputChunkSize && (outputSize % outputChunkSize) == 0, "output size does not meet criteria for this class");
        // fill the output values with zeros
        vec_t accum[outputSize / outputChunkSize];
        for (size_t i = 0; i < outputSize / outputChunkSize; ++i) {
#ifdef NEON
            accum[i] = zeros32;
#else
            vec_set_zero(accum[i]);
#endif
        }
        // TBD: unroll loop
        for (size_t i = 0; i < nnzContext.nzCount; ++i) {
            // nzIndices contains group indices (groups of 4 elements)
            unsigned groupIdx = nnzContext.nzIndices[i];

            // Load 4 consecutive input bytes as 32-bit value and broadcast.
            // inps vector register will have layout:
            // (index0 index1 index2 index3 index 0 index 1 index 2 index 3 ...)
            // repeated for the width of a SIMD register.
            uint32_t inp4 = *reinterpret_cast<const uint32_t*>(&input[groupIdx * 4]);
#ifdef NEON
            vec_t inps = vec_set_32(inp4);
#else
            vec_t inps = vec_set_32<vec_t>(inp4);
#endif
            // iterate over the output dimensions, in chunks that correspond to a SIMD
            // register width
            for (size_t outIdx = 0; outIdx < outputSize; outIdx += outputChunkSize) {
                // Load weights for this group of outputs
                // we load weights in chunks, each of which is outputChunkSize bytes
#ifdef NEON
                auto weights = vec_load32(reinterpret_cast<const vec32_t*>(&_optimizedWeights[bucket][groupIdx][outIdx][0]));
#else
                vec_t weights = vec_load<vec_t>(reinterpret_cast<const vec_t*>(&_optimizedWeights[bucket][groupIdx][outIdx][0]));
#endif
                // multiply/sum into accumulator
                dpbusd_epi32(accum[outIdx/outputChunkSize], inps, weights);
            }
        }
        // add biases and copy to output, applying scaling if needed
        for (size_t outIdx = 0; outIdx < outputSize / outputChunkSize; ++outIdx) {
            if constexpr (inputDequantifyShift > 0) {
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
            if constexpr (outputDequantifyShift > 0) {
#ifdef NEON
                accum[outIdx] = vec_shr32<outputDequantifyShift>(accum[outIdx]);
#else
                accum[outIdx] = vec_shr32(accum[outIdx], outputDequantifyShift);
#endif
            }
#ifdef NEON
            vec_store32(reinterpret_cast<vec32_t*>(&output[outIdx * outputChunkSize]), accum[outIdx]);
#else
            vec_store<vec_t>(reinterpret_cast<vec_t*>(&output[outIdx * outputChunkSize]), accum[outIdx]);
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
        // _optimizedWeights[bucket][input_group][output][input_within_group]
        for (size_t b = 0; b < buckets; ++b) {
            for (size_t i = 0; i < inputSize; i += 4) {
                for (size_t j = 0; j < outputSize; ++j) {
                    for (size_t k = 0; k < 4; ++k) {
                        _optimizedWeights[b][i/4][j][k] = this->_weights[b][j][i + k];
                    }
                }
            }
        }
    }

  protected:
    // optimized weight layout. We maintain this in addition to the standard layout in the
    // subclass.
    alignas(alignment) WeightType _optimizedWeights[buckets][inputSize/4][outputSize][4];

    void calcNnzData(const InputType *input, NnzData &nzInputs) const noexcept {
        simd::calcNnzData<typename NnzData::IndexType>(input, inputSize, nzInputs.nzIndices.data(), nzInputs.nzCount);
    }


};

#endif
