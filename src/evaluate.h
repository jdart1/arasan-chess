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
    alignas(64) Occupancies occupancies;
    alignas(64) Network::AccumulatorType acc;
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

    static size_t getIndices(ColorType kside, const Board &board, IndexArray &out);

    static size_t getIndices(ColorType kside, const Occupancies &occupancies, IndexArray &out);

private:
    // full evaluation of accumulator, update into 3rd argument
    void updateAccum(const Network &network, const Board &board, Network::AccumulatorType &accum);

    // Full evaluation of 1/2 of the accumulator for a specified color (c)
    void updateAccum(const Network &network, const IndexArray &indices, ColorType c,
                     ColorType sideToMove, Network::AccumulatorType &accum) {
        AccumulatorHalf targetHalf = Network::AccumulatorType::getHalf(c, sideToMove);
        network.updateAccum(indices, targetHalf, accum);
        accum.setState(targetHalf, AccumulatorState::Computed);
    }

    // Incremental update of 1/2 of accumulator for the specified color
    void updateAccumIncremental(const Network &network, const NodeInfo *source, NodeInfo *target,
                                const ColorType c);

    // compute changes indices based on the DirtyState of the current node
    template <ColorType kside>
    void getChangedIndices(const NodeInfo *node, IndexArray &added, IndexArray &removed,
                           size_t &added_count, size_t &removed_count);

    void getIndexDiffs(const NodeInfo *source, NodeInfo *target, ColorType c, IndexArray &added,
                       IndexArray &removed, size_t &added_count, size_t &removed_count);

    // Update the corresponding cache entry and from that, the specified node's accumulator half
    void finnyUpdate(const Network &network, const Board &board,
                     NodeInfo *node, ColorType color, AccumulatorHalf targetHalf);

    nnue::FinnyTable finnyTable;
};

} // namespace nnue

#endif
