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
        constexpr InputType maxVal = static_cast<InputType>(clampMax);
        constexpr InputType minVal = 0;
        // process halves, stm first
        for (int half = 0; half < 2; ++half) {
            constexpr size_t QTRSZ = size / 4; // 1/2 of 1/2 of the whole accum
            const InputType *inp = input + (size/2)*half;
            OutputType *outp = output + QTRSZ*half;
            for (size_t i = 0; i < QTRSZ; ++i) {
                uint32_t x0 = static_cast<uint32_t>(std::clamp(inp[i], minVal, maxVal));
                uint32_t x1 = static_cast<uint32_t>(std::clamp(inp[i + QTRSZ], minVal, maxVal));
                // Pairwise multiply then shift to rescale output.
                *outp++ = static_cast<OutputType>( (x0 * x1) >> scalingShift );
            }
        }
    }
};

#endif
