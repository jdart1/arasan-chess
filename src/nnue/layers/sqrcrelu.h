// Copyright 2025 by Jon Dart. All Rights Reserved
#ifndef _NNUE_SQRCRELU_H
#define _NNUE_SQRCRELU_H

#include "typed.h"

// SqrCReLU activation function: clamp to range, then square the result
// This provides a nonlinear activation similar to CReLU but with squaring
template <typename InputType, typename OutputType, size_t size, unsigned clampMax,
          unsigned shift,
          size_t alignment = DEFAULT_ALIGN>
class SqrCReLU
    : public TypedLayer<InputType, OutputType, size, size, alignment> {

public:
    SqrCReLU() = default;

    virtual ~SqrCReLU() = default;

    virtual void forward(const InputType *input, OutputType *output) const
        noexcept {
#if defined(SIMD)
        if constexpr (sizeof(InputType) == sizeof(OutputType) && sizeof(InputType) == 4 &&
                      (size >= 128/sizeof(InputType)) && (size % 128 == 0)) {
            simd::sqrCReLU<InputType, OutputType, size, clampMax, shift>(input, output);
            return;
        }
        else
#endif
           sqrCReLUGeneric(input, output);
#ifdef NNUE_TRACE
        std::cout << "---- SqrCReLU ----" << std::endl;
        for (size_t i = 0; i < size; ++i) {
            std::cout << int(output[i]) << ' ';
        }
        std::cout << std::endl;
#endif
    }

    void sqrCReLUGeneric(const InputType *input, OutputType *output) const noexcept {
        for (size_t i = 0; i < size; i++) {
            // Clamp input to [0, clampMax]
            InputType x = std::clamp<InputType>(input[i], 0, clampMax);
            // Square the clamped value: x^2
            int32_t squared = static_cast<int32_t>(x) * static_cast<int32_t>(x);
            int32_t scaled = squared >> shift;
            // Clamp to output type range
            output[i] = static_cast<OutputType>(std::clamp<int32_t>(scaled, 
                                                                    std::numeric_limits<OutputType>::min(),
                                                                    std::numeric_limits<OutputType>::max()));
        }
    }

protected:
    // No additional members needed
};

#endif
