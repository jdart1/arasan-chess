// Copyright 2021-2022, 2024-2025 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_LINEAR_H
#define _NNUE_LINEAR_H

#include "nndefs.h"
#include "typed.h"
#include "util.h"
#ifdef NNUE_TRACE
#include <array>
#endif
#include <iostream>

// This class defines a linear transformation layer of the NNUE. Integer values are assumed.
// If inputDequantifyShift is nonzero, is applied to the product part of the output as a right shift.
// If outputDequantifyShift is nonzero, is applied to the complete output (bias + product) as a right shift.
//
template <typename InputType, typename WeightType, typename BiasType, typename OutputType,
          size_t inputSize, size_t outputSize, size_t buckets, unsigned inputDequantifyShift,
          unsigned outputDequantifyShift,
          bool transpose = false,
          size_t alignment = DEFAULT_ALIGN>
class LinearLayer : public TypedLayer<InputType, OutputType, inputSize, outputSize, alignment> {

    static constexpr size_t roundedInputSize = std::max<size_t>(32, inputSize);

  public:
    LinearLayer() {
        zero();
        postProcess();
    }

    virtual ~LinearLayer() = default;

    // post-processing after initialization: no-op for this class
    virtual void postProcess() {
    }

    // propagate data through the layer
    virtual inline void forward(size_t bucket, const InputType *input,
                                OutputType *output) const noexcept {
#if defined(SIMD)
        if constexpr (inputSize >= 32 && sizeof(BiasType) == 4 &&
                      (sizeof(WeightType) == 1 || sizeof(WeightType) == 4)) {
            simd::dotProductnxn<InputType, WeightType, BiasType, OutputType, inputSize,
                                roundedInputSize, outputSize, inputDequantifyShift,
                                outputDequantifyShift>(input, _weights[bucket], _biases[bucket],
                                                       output);
        } else
#endif
            dotProductGeneric(bucket, input, output);
#ifdef NNUE_TRACE
        trace(output);
#endif
    }

    inline void dotProductGeneric(size_t bucket, const InputType *input,
                                  OutputType *output) const noexcept {
        // generic implementation
        for (size_t i = 0; i < outputSize; i++) {
            OutputType out = 0;
            for (size_t j = 0; j < inputSize; j++) {
                auto x = static_cast<OutputType>(input[j]);
                auto y = static_cast<OutputType>(this->_weights[bucket][i][j]);
                out += x*y;
            }
            if constexpr (inputDequantifyShift > 0) {
                out >>= inputDequantifyShift;
            }
            out += static_cast<OutputType>(this->_biases[bucket][i]);
            if constexpr (outputDequantifyShift > 0) {
                out >>= outputDequantifyShift;
            }
            output[i] = out;
        }
    }

    virtual void zero() {
        for (size_t b = 0; b < buckets; ++b) {
            for (size_t i = 0; i < outputSize; ++i) {
                _biases[b][i] = 0;
            }
            for (size_t i = 0; i < outputSize; ++i) {
                for (size_t j = 0; j < roundedInputSize; ++j) {
                    _weights[b][i][j] = 0;
                }
            }
        }
    }

    virtual std::istream &read(std::istream &s) {
#ifdef NNUE_TRACE
        std::array<int, buckets> min_weights;
        std::array<int, buckets> max_weights;
        std::array<int, buckets> min_biases;
        std::array<int, buckets> max_biases;
        min_weights.fill(1 << 30);
        min_biases.fill(1 << 30);
        max_weights.fill(-(1 << 30));
        max_biases.fill(-(1 << 30));
#endif
        if constexpr (transpose) {
            // bullet format. Weights are in a matrix ordered with 1st
            // dimension weights, 2nd dimension buckets. We want 1st
            // dimension buckets, 2nd dimension weights for computational
            // efficiency. So do that transformation here.
            for (size_t i = 0; i < inputSize && s.good(); ++i) {
                for (size_t b = 0; b < buckets; ++b) {
                    for (size_t j = 0; j < outputSize && s.good(); ++j) {
                        _weights[b][j][i] = read_little_endian<WeightType>(s);
#ifdef NNUE_TRACE
                        if (_weights[b][j][i] < min_weights[b])
                            min_weights[b] = _weights[b][j][i];
                        if (_weights[b][j][i] > max_weights[b])
                            max_weights[b] = _weights[b][j][i];
#endif
                    }
                }
            }
            // similarly, biases are stored as outputSize x buckets
            for (size_t i = 0; i < outputSize && s.good(); ++i) {
                for (size_t b = 0; b < buckets; ++b) {
                    _biases[b][i] = read_little_endian<BiasType>(s);
#ifdef NNUE_TRACE
                    if (_biases[b][i] < min_biases[b])
                        min_biases[b] = _biases[b][i];
                    if (_biases[b][i] > max_biases[b])
                        max_biases[b] = _biases[b][i];
#endif
                }
            }
        } else {
            // This is the "new" bullet format, ordered by buckets
            for (size_t b = 0; b < buckets; ++b) {
                for (size_t i = 0; i < outputSize && s.good(); ++i) {
                    for (size_t j = 0; j < inputSize && s.good(); ++j) {
                        _weights[b][i][j] = read_little_endian<WeightType>(s);
#ifdef NNUE_TRACE
                        if (_weights[b][i][j] < min_weights[b])
                            min_weights[b] = _weights[b][i][j];
                        if (_weights[b][i][j] > max_weights[b])
                            max_weights[b] = _weights[b][i][j];
#endif
                    }
                }
            }
            for (size_t b = 0; b < buckets; ++b) {
                for (size_t i = 0; i < outputSize && s.good(); ++i) {
                    _biases[b][i] = read_little_endian<BiasType>(s);
#ifdef NNUE_TRACE
                    if (_biases[b][i] < min_biases[b])
                        min_biases[b] = _biases[b][i];
                    if (_biases[b][i] > max_biases[b])
                        max_biases[b] = _biases[b][i];
#endif
                }
            }
        }
#ifdef NNUE_TRACE
        if (!s.fail()) {
            std::cout << "linear layer stats by bucket" << std::endl;
            for (size_t b = 0; b < buckets; ++b) {
                std::cout << b << ": "
                          << "min weight = " << min_weights[b] << " max weight = " << max_weights[b]
                          << " min bias = " << min_biases[b] << " max bias = " << max_biases[b]
                          << std::endl;
            }
        }
#endif
        postProcess();
        return s;
    }

    virtual std::ostream &write(std::ostream &s,
                                const WeightType (&weights)[buckets][outputSize][roundedInputSize],
                                const BiasType (&biases)[buckets][outputSize]) {
        // Weights first, then biases
        // serialized in column order
        for (size_t b = 0; b < buckets; ++b) {
            for (size_t i = 0; i < outputSize; ++i) {
                for (size_t j = 0; j < roundedInputSize; ++j) {
                    s << weights[b][i][j];
                }
            }
        }
        for (size_t b = 0; b < buckets; ++b) {
            for (size_t i = 0; i < outputSize; ++i) {
                s << biases[b][i];
            }
        }
        return s;
    }

    virtual const BiasType *getBiases(size_t bucket) const noexcept { return _biases[bucket]; }

    virtual const WeightType *getCol(size_t bucket, size_t col) const noexcept {
        return _weights[bucket][col];
    }

    virtual void setCol(size_t bucket, size_t index, const WeightType *col) {
        for (size_t i = 0; i < inputSize; ++i)
            _weights[bucket][index][i] = col[i];
    }

    void setBiases(size_t bucket, const BiasType *b) {
        std::memcpy(_biases[bucket], b, outputSize * sizeof(BiasType));
    }

    unsigned numBuckets() const noexcept {
        return buckets;
    }

  protected:
    alignas(alignment) BiasType _biases[buckets][outputSize];
    alignas(alignment) WeightType _weights[buckets][outputSize][roundedInputSize];

#ifdef NNUE_TRACE
    void trace(OutputType *output) const noexcept {
        std::cout << "---- linear " << inputSize << 'x' << outputSize << " ----" << std::endl;
        for (unsigned i = 0; i < outputSize; ++i) {
            std::cout << static_cast<int>(output[i]) << ' ';
        }
        std::cout << std::endl;
    }
#endif

};

#endif
