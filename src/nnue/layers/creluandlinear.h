// Copyright 2024-2025 by Jon Dart. All Rights Reserved
#ifndef _NNUE_CRELUANDLINEAR_H
#define _NNUE_CRELUANDLINEAR_H

#include "linear.h"

// This combines both a CReLU operation and a linear layer with output size one, the (single) hidden layer
// in the Arasan V3 architecture.
template <typename AccumulatorType, typename InputType, typename WeightType, typename BiasType, typename OutputType, size_t inputSize,
          int clampMax, int NETWORK_QA, size_t buckets, bool transpose = false, size_t alignment = DEFAULT_ALIGN>
class CReLUAndLinear
    : public LinearLayer<InputType, WeightType, BiasType, OutputType, inputSize, 1, buckets, transpose, alignment> {
  public:
    CReLUAndLinear() = default;

    virtual ~CReLUAndLinear() = default;

    virtual void doForward([[maybe_unused]] const InputType *input, [[maybe_unused]] OutputType *output) const noexcept {
        // no-op for this layer: use method below
        assert(0);
    }

    void postProcessAccum(const AccumulatorType &accum, unsigned bucket, OutputType *output) const {
        int32_t sum = 0;
#if defined(SIMD)
        if constexpr (sizeof(InputType) == 2) {
            simd::CRelUAndLinear < InputType, OutputType, WeightType, inputSize / 2, 1 >
                                      (accum.getOutput(AccumulatorHalf::Lower), output, clampMax,
                                       this->_weights[bucket][0]);
            sum += *output;
            simd::CRelUAndLinear < InputType, OutputType, WeightType, inputSize / 2, 1 >
                                      (accum.getOutput(AccumulatorHalf::Upper), output, clampMax,
                                       this->_weights[bucket][0] + inputSize / 2);
            sum += *output;
            output[0] = sum + this->_biases[bucket][0];
        } else
#endif
        {
            // generic implementation
            static AccumulatorHalf halves[] = {AccumulatorHalf::Lower, AccumulatorHalf::Upper};
            size_t offset = 0;
            for (auto h : halves) {
                for (size_t i = 0; i < accum.getSize(); ++i) {
                    int16_t x = accum.getOutput(h)[i];
                    // CReLU
                    x = std::clamp<int16_t>(x, 0, clampMax);
                    // multiply by weights and sum
                    sum += (x * this->_weights[bucket][0][i + offset]);
                }
                offset += accum.getSize();
            }
            output[0] = sum + this->_biases[bucket][0];
        }
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
