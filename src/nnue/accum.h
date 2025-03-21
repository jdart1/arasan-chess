// Copyright 2021-2024 by Jon Dart. All Rigths Reserved.
#ifndef _NNUE_ACCUM_H
#define _NNUE_ACCUM_H

#include "chess.h"
#include "nndefs.h"

#include <cstring>

enum class AccumulatorHalf { Lower, Upper };

enum class AccumulatorState { Empty, Computed };

// Holds calculations that reprepsent the output of the first layer, pre-scaling
template <typename OutputType, size_t size, size_t alignment = DEFAULT_ALIGN>
class Accumulator {
  public:
    static constexpr AccumulatorHalf halves[2] = {AccumulatorHalf::Lower, AccumulatorHalf::Upper};

    typedef const OutputType (*OutputPtr)[size];

    Accumulator() { _states[0] = _states[1] = AccumulatorState::Empty; }

    virtual ~Accumulator() = default;

    static inline AccumulatorHalf getHalf(ColorType sideToMove, ColorType c) {
        return halves[(c != sideToMove)];
    }

    template <typename BiasType>
    void init_half(AccumulatorHalf half, const BiasType *data) {
        const OutputType *in = data;
        OutputType *out = _accum[halfToIndex(half)];
#ifdef SIMD
        if constexpr (size*16 % simd::simdWidth == 0 && sizeof(BiasType) == sizeof(OutputType) && sizeof(OutputType)==2) {
            simd::vec_copy<size,OutputType>(in,out);
        }
        else
#endif
        for (size_t i = 0; i < size; ++i) {
            *out++ = static_cast<OutputType>(*in++);
        }
    }

    void copy_half(AccumulatorHalf half,
                   const Accumulator<OutputType, size, alignment> &source,
                   AccumulatorHalf sourceHalf) {
        const OutputType *in = source._accum[halfToIndex(sourceHalf)];
        OutputType *out = _accum[halfToIndex(half)];
        std::memcpy(out, in, sizeof(OutputType) * size);
    }

    // Update half of the accumulator (does not use SIMD)
    template <typename WeightType>
    void add_half(AccumulatorHalf half, const WeightType *data) {
        const WeightType *in = data;
        OutputType *out = _accum[halfToIndex(half)];
        for (size_t i = 0; i < size; ++i) {
            *out++ += static_cast<OutputType>(*in++);
        }
    }

    // Update half of the accumulator (does not use SIMD)
    template <typename WeightType>
    void sub_half(AccumulatorHalf half, const WeightType *data) {
        const WeightType *in = data;
        OutputType *out = _accum[halfToIndex(half)];
        for (size_t i = 0; i < size; ++i) {
            *out++ -= static_cast<OutputType>(*in++);
        }
    }

    OutputPtr getOutput() const noexcept { return _accum; }

    const OutputType *getOutput(AccumulatorHalf half) const noexcept {
        return _accum[halfToIndex(half)];
    }

    OutputType *data(AccumulatorHalf half) { return _accum[halfToIndex(half)]; }

    AccumulatorState getState(AccumulatorHalf half) const noexcept {
        return _states[halfToIndex(half)];
    }

    void setState(AccumulatorHalf half, AccumulatorState state) {
        _states[halfToIndex(half)] = state;
    }

    void setState(AccumulatorState state) { _states[0] = _states[1] = state; }

    void setEmpty() {
        setState(AccumulatorHalf::Lower, AccumulatorState::Empty);
        setState(AccumulatorHalf::Upper, AccumulatorState::Empty);
    }

    size_t getSize() const noexcept { return size; }

    void print(AccumulatorHalf half, std::ostream &out) const noexcept {
        int p = halfToIndex(half);
        out << "perspective " << p << std::endl;
        for (unsigned i = 0; i < size; ++i) {
            out << static_cast<int>(_accum[p][i]) << ' ';
            if ((i + 1) % 64 == 0)
                out << std::endl;
        }
    }

#ifdef NNUE_TRACE
    template <typename _OutputType, size_t _size,
              size_t _alignment>
    friend std::ostream &
    operator<<(std::ostream &o,
               const Accumulator<_OutputType, _size, _alignment> &accum);
#endif

    template <typename _OutputType,size_t _size,
              size_t _alignment>
    friend bool
    operator==(const Accumulator<_OutputType, _size, _alignment> &accum1,
               const Accumulator<_OutputType, _size, _alignment> &accum2);

    template <typename _OutputType, size_t _size,
              size_t _alignment>
    friend bool
    operator!=(const Accumulator<_OutputType, _size, _alignment> &accum1,
               const Accumulator<_OutputType, _size, _alignment> &accum2);

  private:
    static inline unsigned halfToIndex(AccumulatorHalf half) {
        return (half == AccumulatorHalf::Lower ? 0 : 1);
    }

    alignas(alignment) OutputType _accum[2][size];
    AccumulatorState _states[2];
};

#ifdef NNUE_TRACE
template <typename OutputType, size_t size,
          size_t alignment = DEFAULT_ALIGN>
inline std::ostream &operator<<(std::ostream &o,
                                const Accumulator<OutputType, size, alignment> &accum) {
    accum.print(AccumulatorHalf::Lower,o);
    accum.print(AccumulatorHalf::Upper,o);
    return o;
}
#endif

template <typename _OutputType, size_t _size,
          size_t _alignment>
inline bool
operator==(const Accumulator<_OutputType, _size, _alignment> &accum1,
           const Accumulator<_OutputType, _size, _alignment> &accum2) {
    const auto p = accum1._accum;
    const auto q = accum2._accum;
    for (size_t h = 0; h < 2; ++h) {
        for (size_t i = 0; i < _size; ++i) {
            if (p[h][i] != q[h][i]) {
                return false;
            }
        }
    }
    return true;
}

template <typename _OutputType, size_t _size,
          size_t _alignment>
inline bool
operator!=(const Accumulator<_OutputType, _size, _alignment> &accum1,
           const Accumulator<_OutputType, _size, _alignment> &accum2) {
    return !(accum1 == accum2);
}

#endif
