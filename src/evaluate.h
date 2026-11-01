// Copyright 2021-2025 by Jon Dart. All Rights Reserved.
#ifndef _EVALUATE_H
#define _EVALUATE_H

// NNUE evaluation, interface between Arasan data types and the NNUE code.

#include "board.h"
#include "nnue/nnue.h"

struct NodeInfo;

namespace nnue {

struct DirtyState {
    Square from, to;
    Piece piece;

    DirtyState() : from(InvalidSquare), to(InvalidSquare), piece(EmptyPiece) {}

    DirtyState(Square f, Square t, Piece p) : from(f), to(t), piece(p) {}
};

struct FinnyEntry {
    alignas(nnue::DEFAULT_ALIGN) Occupancies occupancies;
    alignas(nnue::DEFAULT_ALIGN) Network::AccumulatorType acc;
};

// indices: color, king bucket, mirrored (king file >=4)
using FinnyTable = FinnyEntry[2][NetworkParams::KING_BUCKETS][2];

// Functions to perform incremental or full NNUE evaluation
class Evaluator {

  public:
    Evaluator() = default;

    virtual ~Evaluator() = default;

    // Update the accumulator based on a position (incrementally if possible)
    void updateAccum(const Network &network, const Board &board, NodeInfo *node, const ColorType c);

    // evaluate the net (full evaluation)
    Network::OutputType fullEvaluate(const Network &network, const Board &board,
                                     NodeInfo *node = nullptr);

    void clearCache();

    unsigned getOutputBucket(const Board &board) {
        return NetworkParams::getOutputBucket(board.men());
    }

    static size_t getIndices(ColorType kside, const Board &board, PSQIndexArray &out);

    static size_t getIndices(ColorType kside, const Occupancies &occupancies, PSQIndexArray &out);

private:
    // full evaluation of accumulator, update into 3rd argument
    void updateAccum(const Network &network, const Board &board, Network::AccumulatorType &accum);

    // Get all features of the position for one perspective (as "added" features)
    static void getFeatures(const Board &board, ColorType c, FeatureChanges &features);

    // Incremental update of 1/2 of accumulator for the specified color
    void updateAccumIncremental(const Network &network, const NodeInfo *source, NodeInfo *target,
                                const ColorType c);

    // compute changes indices based on the DirtyState of the current node
    template <ColorType kside>
    void getChangedIndices(const NodeInfo *node, PSQIndexArray &added, PSQIndexArray &removed,
                           size_t &added_count, size_t &removed_count);

    void getIndexDiffs(const NodeInfo *source, NodeInfo *target, ColorType c, PSQIndexArray &added,
                       PSQIndexArray &removed, size_t &added_count, size_t &removed_count);

    // Update the corresponding cache entry and from that, the specified node's accumulator half
    void finnyUpdate(const Network &network, const Board &board,
                     NodeInfo *node, ColorType color, AccumulatorHalf targetHalf);

    nnue::FinnyTable finnyTable;
};

} // namespace nnue

#endif
