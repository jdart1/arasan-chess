// Copyright 2022, 2024-2025 by Jon Dart. All Rights Reserved
#ifndef _NNUE_PAIRWISE_MULT_H
#define _NNUE_PAIRWISE_MULT_H

#include "typed.h"

// Combination of the accumulator halves into a combined output, by:
// 1. clamping to a range
// 2. pairwise multiplication
// 3. scaling the product
template <typename InputType, typename OutputType, size_t size, unsigned clampMax,
          unsigned shift, size_t alignment = DEFAULT_ALIGN>
class PairwiseMult : public TypedLayer<InputType, OutputType, size, size, alignment> {
  public:
    PairwiseMult() = default;

    virtual ~PairwiseMult() = default;

    virtual void doForward(const InputType *input, OutputType *output) const noexcept {
#if defined(SIMD)
        if constexpr (sizeof(OutputType) == 1) {
            simd::pairwiseMult<InputType, OutputType, size, clampMax, shift>(input, output);
        } else
#endif
            pairwseMultGeneric(input, output);
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

    void pairwiseMultGeneric(const InputType *input, OutputType *output) const noexcept {
        InputType maxVal = static_cast<InputType>(clampMax);
        for (size_t i = 0; i < size/2; ++i) {
            int32_t x0 = static_cast<int32_t>(std::clamp<InputType>(input[i],0,maxVal));
            int32_t x1 = static_cast<int32_t>(std::clamp<InputType>(input[i + (size / 2)],0,maxVal));
            output[i] = static_cast<OutputType>(
                std::clamp<int32_t>(x0 * x1, std::numeric_limits<int16_t>::min(),
                                    std::numeric_limits<int16_t>::max()) >>
                shift);
        }
    }
};

#endif
