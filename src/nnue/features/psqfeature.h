// Copyright 2020-2026 by Jon Dart. All Rights Reserved.
#ifndef _PSQFEATURE_H
#define _PSQFEATURE_H

#include "nndefs.h"
#include "nnparams.h"
#include "accum.h"
#include "chess.h"
#include "featureutil.h"
#include "util.h"
#include "weightcodec.h"

// Implements a set of Piece-Square features, as in Arasan v26.1 and earler.
// King position is mirrored so that the King is always on files e..h.
template <typename InputType, typename WeightType, typename BiasType, typename OutputType,
          typename AccumulatorType, size_t inputSize,
          size_t outputSize, const unsigned kingBucketsMap[64], size_t alignment = DEFAULT_ALIGN>
class PSQFeature
{
public:

    PSQFeature() = default;

    virtual ~PSQFeature() = default;

    // king bucket for side and king position
    inline static unsigned getKingBucket(ColorType kside, Square kp) {
        return kingBucketsMap[FeatureUtils::relativeSquare(kside, kp)];
    }

    inline static IndexType getIndex(ColorType kside, Square kp /* kside King */, Piece p, Square sq) {
        assert(p != EmptyPiece);
        IndexType idx = static_cast<IndexType>(getKingBucket(kside, kp) * 12 * 64 +
                                               pieceTypeMap[kside != ColorOfPiece(p)][p] * 64 +
                                               FeatureUtils::mirroredSquare(kside, kp, sq));
        assert(idx < inputSize);
        return idx;
    }

#ifdef SIMD
    // Rows to add and subtract for an accumulator update (see simd::updateRows)
    auto rowUpdate(const PSQIndexArray &added, size_t addedCount, const PSQIndexArray &removed,
                   size_t removedCount) const noexcept {
        return simd::makeRowUpdate(_weights, added.data(), addedCount, removed.data(),
                                   removedCount);
    }
#else
    // Add and subtract rows of weights to half of the accumulator
    void updateAccum(const PSQIndexArray &added, size_t addedCount, const PSQIndexArray &removed,
                     size_t removedCount, AccumulatorHalf half,
                     AccumulatorType &output) const noexcept {
        for (size_t i = 0; i < addedCount; i++) {
            output.add_half(half, this->_weights[added[i]]);
        }
        for (size_t i = 0; i < removedCount; i++) {
            output.sub_half(half, this->_weights[removed[i]]);
        }
    }
#endif

    // read weights from a stream
    virtual std::istream &read(std::istream &s) {
#ifdef NNUE_TRACE
       int min_weight = 1<<30, max_weight = -(1<<30), min_bias = 1<<30, max_bias = -(1<<30);
#endif
        WeightCodec::read<WeightType>(s, inputSize, outputSize,
                                      [this](size_t i) { return _weights[i]; });
#ifdef NNUE_TRACE
        for (size_t i = 0; i < inputSize && s.good(); ++i) {
            for (size_t j = 0; j < outputSize; ++j) {
                if (_weights[i][j] < min_weight) min_weight = _weights[i][j];
                if (_weights[i][j] > max_weight) max_weight = _weights[i][j];
            }
        }
#endif
        for (size_t i = 0; i < outputSize && s.good(); ++i) {
            _biases[i] = read_little_endian<BiasType>(s);
#ifdef NNUE_TRACE
            if (_biases[i] < min_bias) min_bias = _biases[i];
            if (_biases[i] > max_bias) max_bias = _biases[i];
#endif
        }
#ifdef NNUE_TRACE
        if (!s.fail()) {
            std::cout << "min feature weight = " << min_weight << " max feature weight = " << max_weight << std::endl;
            std::cout << "min feature bias = " << min_bias << " max feature bias = " << max_bias << std::endl;
        }
#endif
        return s;
    }

    virtual const WeightType *getCol(size_t row) const noexcept {
        return _weights[row];
    }

    virtual void setCol(size_t row, const WeightType *col) {
        for (size_t i = 0; i < outputSize; ++i) {
           _weights[row][i] = col[i];
        }
    }

    virtual void setBiases(const BiasType *b) {
        std::memcpy(_biases,reinterpret_cast<const void*>(b),sizeof(BiasType)*outputSize);
    }

    const BiasType *getBiases() const noexcept {
        return _biases;
    }

    static inline bool needsRefresh(ColorType perspective, Square oldKing, Square newKing) {
        return ((File(oldKing) >= chess::EFILE) != (File(newKing) >= chess::EFILE)) ||
            kingBucketsMap[FeatureUtils::relativeSquare(perspective, oldKing)] !=
            kingBucketsMap[FeatureUtils::relativeSquare(perspective, newKing)];
    }

private:
    static constexpr unsigned pieceTypeMap[2][16] = {
                                                     {0, 0, 1, 2, 3, 4, 5, 0, 0, 0, 1, 2, 3, 4, 5, 0},
                                                     {0, 6, 7, 8, 9, 10, 11, 0, 0, 6, 7, 8, 9, 10, 11, 0}};

    alignas(alignment) BiasType _biases[outputSize];
    alignas(alignment) WeightType _weights[inputSize][outputSize];

};
#endif
