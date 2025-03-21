// Copyright 2021, 2022, 2024 by Jon Dart. All Rights Reserved
#ifndef _NNUE_CRELU_H
#define _NNUE_CRELU_H

#include "typed.h"

// CReLU activation function with scaling of input
template <typename InputType, typename OutputType, size_t size, unsigned scaleFactor,
          size_t alignment = DEFAULT_ALIGN>
class CReLU
    : public TypedLayer<InputType, OutputType, size, size, alignment> {

public:
    // scaleFactor is right shift, clampMax is upper limit for output
    CReLU(int clampMax)
        : _clampMax(clampMax) {
    }

    virtual ~CReLU() = default;

    virtual void doForward(const InputType *input, OutputType *output) const
        noexcept {
#if defined(SIMD)
        simd::CRelU<InputType, OutputType, size, scaleFactor>(input, output, _clampMax);
#else
        for (size_t i = 0; i < size; i++) {
            output[i] = static_cast<OutputType>(
                                                std::clamp<InputType>(input[i] >> scaleFactor, 0, _clampMax));
        }
#endif
#ifdef NNUE_TRACE
        std::cout << "---- CRelU ----" << std::endl;
        for (size_t i = 0; i < size; ++i) {
            std::cout << int(output[i]) << ' ';
        }
        std::cout << std::endl;
#endif
    }

  protected:
    int _clampMax;
};

#endif
