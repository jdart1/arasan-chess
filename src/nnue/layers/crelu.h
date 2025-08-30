// Copyright 2021, 2022, 2024 by Jon Dart. All Rights Reserved
#ifndef _NNUE_CRELU_H
#define _NNUE_CRELU_H

#include "typed.h"

// CReLU activation function with scaling of input
// scaleFactor is right shift, clampMax is upper limit for output
template <typename InputType, typename OutputType, size_t size, unsigned clampMax,
          unsigned scaleFactor,
          size_t alignment = DEFAULT_ALIGN>
class CReLU
    : public TypedLayer<InputType, OutputType, size, size, alignment> {

public:
    CReLU() = default;

    virtual ~CReLU() = default;

    virtual void doForward(const InputType *input, OutputType *output) const
        noexcept {
#if defined(SIMD)
        if constexpr (sizeof(InputType) == sizeof(OutputType) && sizeof(InputType) == 4 &&
                      (size >= 128/sizeof(InputType)) && (size % 128 == 0)) {
            simd::cReLU<InputType, OutputType, size, clampMax, scaleFactor>(input, output);
            return;
        }
        else
#endif
           cReLUGeneric(input, output);
#ifdef NNUE_TRACE
        std::cout << "---- CRelU ----" << std::endl;
        for (size_t i = 0; i < size; ++i) {
            std::cout << int(output[i]) << ' ';
        }
        std::cout << std::endl;
#endif
    }

    void cReLUGeneric(const InputType *input, OutputType *output) const noexcept {
        for (size_t i = 0; i < size; i++) {
            if constexpr (scaleFactor > 0)
                output[i] = static_cast<OutputType>(std::clamp<InputType>(input[i] >> scaleFactor, 0, clampMax));
            else
                output[i] = static_cast<OutputType>(std::clamp<InputType>(input[i], 0, clampMax));
        }
    }


  protected:
    int _clampMax;
};

#endif
