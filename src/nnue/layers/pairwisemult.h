// Copyright 2022, 2024-2025 by Jon Dart. All Rights Reserved
#ifndef _NNUE_PAIRWISE_MULT_H
#define _NNUE_PAIRWISE_MULT_H

#include "typed.h"

// Combination of the accumulator halves into a combined output, by:
// 1. clamping to a range
// 2. pairwise multiplication
// 3. scaling the product
template <typename InputType, typename AccumulatorType, typename OutputType, size_t size,
          unsigned clampMax, unsigned scaleFactor, size_t alignment = DEFAULT_ALIGN>
class PairwiseMult
    : public TypedLayer<InputType, OutputType, size, size, alignment> {
  public:
    PairwiseMult() = default;

    virtual ~PairwiseMult() = default;

    virtual void doForward(const InputType *input, OutputType *output) const noexcept {
#if defined(SIMD)
        if constexpr (sizeof(OutputType) == 1) {
            simd::pairwiseMult<InputType, OutputType, size, clampMax, scaleFactor>(
                input, output);
        } else
#endif
        {
            for (size_t i = 0; i < size; ++i) {
                InputType sum0 = input[i];
                InputType sum1 = input[i + size / 2];
                sum0 = std::clamp<int>(sum0, 0, clampMax);
                sum1 = std::clamp<int>(sum1, 0, clampMax);
                output[i] = static_cast<OutputType>((sum0 * sum1) >> scaleFactor);
            }
        }
#ifdef NNUE_TRACE
            std::cout << "---- PairwiseMult output " << std::endl;
        for (size_t i = 0; i < size; ++i) {
            std::cout << static_cast<int>(output[i]) << ' ';
            if ((i + 1) % 64 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
#endif
    }
};

#endif
