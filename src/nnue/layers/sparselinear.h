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

        // perform SIMD-optimized sparse dot product
        simd::sparseDotProduct<InputType, WeightType, BiasType, OutputType, inputSize, outputSize,
                               inputDequantifyShift, outputDequantifyShift>(
            input, _optimizedWeights[bucket], this->_biases[bucket], nnzContext.nzIndices.data(),
            nnzContext.nzCount, output);

#ifdef NNUE_TRACE
        this->trace(output);
#endif
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
        simd::calcNnzData<inputSize, typename NnzData::IndexType>(input, nzInputs.nzIndices.data(), nzInputs.nzCount);
    }


};

#endif
