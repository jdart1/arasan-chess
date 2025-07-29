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

    virtual void doForward([[maybe_unused]] const InputType *input, [[maybe_unused]] OutputType *output) const noexcept {
        // no-op for this layer: use method below
        assert(0);
    }

    void postProcessAccum(const AccumulatorType &accum, OutputType *output) const {
#if defined(SIMD)
        if constexpr (sizeof(OutputType) == 1) {
            simd::pairwiseMult<InputType, OutputType, size / 2, clampMax, scaleFactor>(
                accum.getOutput(AccumulatorHalf::Lower), output);
            simd::pairwiseMult<InputType, OutputType, size / 2, clampMax, scaleFactor>(
                accum.getOutput(AccumulatorHalf::Upper), output + size / 2);
        } else
#endif
        {
            size_t offset = 0;
            static const AccumulatorHalf halves[2] = {AccumulatorHalf::Lower,
                                                      AccumulatorHalf::Upper};
            for (size_t p = 0; p < 2; ++p, offset += size / 2) {
                const InputType *input = accum.getOutput(halves[p]);
                for (size_t i = 0; i < size / 2; ++i) {
                    InputType sum0 = input[i];
                    InputType sum1 = input[i + size / 2];
                    sum0 = std::clamp<int>(sum0, 0, clampMax);
                    sum1 = std::clamp<int>(sum1, 0, clampMax);
                    output[offset + i] = static_cast<OutputType>((sum0 * sum1) >> scaleFactor);
                }
            }
        }
#ifdef NNUE_TRACE
            std::cout << "---- PairwiseMult output " << std::endl;
        for (size_t i = 0; i < size; ++i) {
            std::cout << int(output[i]) << ' ';
            if ((i + 1) % 64 == 0)
                std::cout << std::endl;
        }
        std::cout << std::endl;
#endif
    }
};

#endif
