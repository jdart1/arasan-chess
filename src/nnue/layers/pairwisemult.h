// Copyright 2022, 2024-2025 by Jon Dart. All Rights Reserved
#ifndef _NNUE_PAIRWISE_MULT_H
#define _NNUE_PAIRWISE_MULT_H

#include "typed.h"

// Combination of the accumulator halves into a combined output, by:
// 1. clamping to a range
// 2. pairwise multiplication
// 3. scaling the product
// Output size is 1/2 of input size
template <typename InputType, typename OutputType, size_t size, unsigned clampMax,
          unsigned scalingShift, size_t alignment = DEFAULT_ALIGN>
class PairwiseMult : public TypedLayer<InputType, OutputType, size, size, alignment> {
  public:
    PairwiseMult() = default;

    virtual ~PairwiseMult() = default;

    virtual void forward(const InputType *input, OutputType *output) const noexcept {
#if defined(SIMD)
        if constexpr (sizeof(OutputType) == 1 && sizeof(InputType) == 2) {
            simd::pairwiseMult<InputType, OutputType, size, clampMax, scalingShift>(input, output);
        } else
#endif
            pairwiseMultGeneric(input, output);
#ifdef NNUE_TRACE
        std::cout << "---- PairwiseMult output " << std::endl;
        constexpr size_t limit = size * sizeof(OutputType) / sizeof(InputType);
        for (size_t i = 0; i < limit; ++i) {
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
            uint32_t x0 = static_cast<uint32_t>(std::clamp<InputType>(input[i],0,maxVal));
            uint32_t x1 = static_cast<uint32_t>(std::clamp<InputType>(input[i + (size / 2)],0,maxVal));
            // Pairwise multiply then shift to rescale output.
            uint32_t product = (x0 * x1) >> (16 - scalingShift);
            // if (i<20) std::cout << i << " " << "x0 = " << x0 << " x1 = " << x1 << " mult = " << x0*x1 << " shifted " << product << std::endl;
            // output[i] = static_cast<OutputType>(std::clamp<int32_t>(product, 0, static_cast<int32_t>(outputQuant)));
            output[i] = product;
        }
    }
};

#endif
