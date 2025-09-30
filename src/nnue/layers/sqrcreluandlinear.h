// Copyright 2024-2025 by Jon Dart. All Rights Reserved
#ifndef _NNUE_SQRCRELUANDLINEAR_H
#define _NNUE_SQRCRELUANDLINEAR_H

#include "linear.h"

// This combines both a SqrCReLU operation and a linear layer with output size one, the (single) hidden layer
// in the Arasan V3 architecture. TBD: generalize to larger output sizes.
// The weights are multiplied by the input and the high 16 bits discarded. Then the operation
// is completed by multiplying by the input again (squaring)
template <typename AccumulatorType, typename InputType, typename WeightType, typename BiasType, typename OutputType, size_t inputSize,
          int clampMax, unsigned inputDequantifyShift, size_t buckets, bool transpose = false, size_t alignment = DEFAULT_ALIGN>
class SqrCReLUAndLinear
    : public LinearLayer<InputType, WeightType, BiasType, OutputType, inputSize, 1 /* output size */, buckets,
                         0, 0, transpose, alignment> {
  public:
    SqrCReLUAndLinear() = default;

    virtual ~SqrCReLUAndLinear() = default;

    virtual void forward(size_t bucket, const InputType *input, OutputType *output) const noexcept {
        int32_t sum;
#ifdef SIMD
        if constexpr (sizeof(InputType) == 2) {
            // SIMD optimized implementation
            simd::sqrCRelUAndLinear < InputType, OutputType, WeightType, inputSize, 1>(
                input, output, clampMax, this->_weights[bucket][0]);
            sum = *output;
        } else
#endif
        {
            sum = 0;
            for (size_t i = 0; i < inputSize; ++i) {
                InputType x = input[i];
                // CReLU
                x = std::clamp<InputType>(x, 0, clampMax);
                sum += std::clamp<OutputType>(this->_weights[bucket][0][i] * x, -32767,
                                              32768) * x;
            }
        }
        output[0] = (sum >> inputDequantifyShift) + this->_biases[bucket][0];
#ifdef NNUE_TRACE
        std::cout << "output bucket = " << bucket << std::endl;
        std::cout << "---- SqrCReLUAndLinear output " << std::endl;
        std::cout << " prescaled = " << sum << " unsquared = " << output[0] << std::endl;
        for (size_t i = 0; i < 1 /*outputSize */; ++i) {
            std::cout << static_cast<int>(output[i]) << ' ';
            if ((i + 1) % 64 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
#endif
    }
};

#endif
