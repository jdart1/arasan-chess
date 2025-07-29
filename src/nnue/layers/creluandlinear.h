// Copyright 2024-2025 by Jon Dart. All Rights Reserved
#ifndef _NNUE_CRELUANDLINEAR_H
#define _NNUE_CRELUANDLINEAR_H

#include "linear.h"

// This combines both a CReLU operation and a linear layer with output size one
template <typename AccumulatorType, typename InputType, typename WeightType, typename BiasType, typename OutputType, size_t inputSize,
          int clampMax, int NETWORK_QA, size_t buckets, bool transpose = false, size_t alignment = DEFAULT_ALIGN>
class CReLUAndLinear
    : public LinearLayer<InputType, WeightType, BiasType, OutputType, inputSize, 1, buckets, transpose, alignment> {
  public:
    CReLUAndLinear() = default;

    virtual ~CReLUAndLinear() = default;

    virtual void doForward(size_t bucket, const InputType *input, OutputType *output) const noexcept {
        int32_t sum;
#if defined(SIMD)
        if constexpr (sizeof(InputType) == 2) {
            simd::CRelUAndLinear < InputType, OutputType, WeightType, inputSize, 1 >
                                      (input, output, clampMax, this->_weights[bucket]);
            sum = *output;
        } else
#endif
        {
            // generic implementation
            sum = 0;
            for (size_t i = 0; i < inputSize; ++i) {
                int16_t x = input[i];
                // CReLU
                x = std::clamp<int16_t>(x, 0, clampMax);
                // multiply by weights and sum
                sum += (x * this->_weights[bucket][0][i]);
            }
        }
        output[0] = sum + this->_biases[bucket][0];
#ifdef NNUE_TRACE
        std::cout << "---- CReLUAndLinear output " << std::endl;
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
