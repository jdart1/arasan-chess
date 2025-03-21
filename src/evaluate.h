// Copyright 2021-2025 by Jon Dart. All Rights Reserved.
#ifndef _EVALUATE_H
#define _EVALUATE_H

// NNUE evaluation, interface between Arasan data types and the NNUE code.

#include "board.h"

struct NodeInfo;

namespace nnue {

struct DirtyState {
    Square from, to;
    Piece piece;

    DirtyState()
        : from(InvalidSquare), to(InvalidSquare),
          piece(EmptyPiece) {}

    DirtyState(Square f, Square t, Piece p)
        : from(f), to(t), piece(p) {}
};

// Functions to perform incremental or full NNUE evaluation
class Evaluator {
    friend class NNUETest;

public:
    // Update the accumulator based on a position (incrementally if possible)
    static void updateAccum(const Network &network, const Board &board, NodeInfo *node,
                            const ColorType c);

    // full evaluation of accumulator, update into 3rd argument
    static void updateAccum(const Network &network, const Board &board, Network::AccumulatorType &accum);

    // evaluate the net (full evaluation)
    static Network::OutputType fullEvaluate(const Network &network,
                                            const Board &board,
                                            NodeInfo *node = nullptr);

    static unsigned getOutputBucket(const Board &board) {
        return NetworkParams::getOutputBucket(board.men());
    }

    // Full evaluation of 1/2 of the accumulator for a specified color (c)
    static void updateAccum(const Network &network, const IndexArray &indices, ColorType c,
                            ColorType sideToMove, Network::AccumulatorType &accum) {
        AccumulatorHalf targetHalf =
            Network::AccumulatorType::getHalf(c, sideToMove);
        network.updateAccum(indices, targetHalf, accum);
        accum.setState(targetHalf,AccumulatorState::Computed);
    }

    // Incremental update of 1/2 of accumulator for the specified color
    static void updateAccumIncremental(const Network &network,
                                       const NodeInfo *source,
                                       NodeInfo *target, const ColorType c);

    static size_t getIndices(ColorType kside, const Board &board, IndexArray &out);

    // compute changes indices based on the DirtyState of the current node
    template <ColorType kside>
    static void getChangedIndices(const NodeInfo *node, IndexArray &added,
                                  IndexArray &removed, size_t &added_count,
                                  size_t &removed_count);

    static void getIndexDiffs(const NodeInfo *source,
                              NodeInfo *target, ColorType c,
                              IndexArray &added, IndexArray &removed,
                              size_t &added_count, size_t &removed_count);

};

}

#endif
