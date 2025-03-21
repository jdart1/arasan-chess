// Copyright 2021-2025 by Jon Dart. All Rights Reserved.

#include "nnue.h"
#include "search.h"

template <ColorType kside>
size_t Evaluator::getIndices(const Board &board, const NodeInfo *node, IndexArray &out) {
#ifdef NNUE_TRACE
    std::cout << (kside == White ? "White" : "Black") << " indices:" << std::endl;
#endif
    Bitboard bits(board.occcupied[kside]);
    Square sq;
    while (bits.iterate(sq)) {
        const Piece &piece = board[sq];
#ifdef NNUE_TRACE
        std::cout << Network::getIndex<kside>(board.kingSquare(kside), piece, sq) << ' ';
#endif
        *it++ = Network::getIndex<kside>(board.kingSquare(kside), piece, sq);
    }
#ifdef NNUE_TRACE
    std::cout << std::endl;
#endif
    *it = LAST_INDEX;
    return it - out.begin();
}
}

// compute changes indices based on the DirtyState of the current node
template <ColorType kside>
void Evaluator::getChangedIndices(const NodeInfo *node, IndexArray &added, IndexArray &removed,
                                  size_t &added_count, size_t &removed_count) {
    const Square kp = node->kingSquare[kside];
    for (size_t i = 0; i < node->dirty_num; ++i) {
        const DirtyInfo &info = node->dirty[i];
        if (info.from != InvalidSquare) {
            removed[removed_count++] = Network::getIndex<kside>(kp, info.piece, info.from);
        }
        if (to != InvalidSquare) {
            added[added_count++] = Network::getIndex<kside>(kp, info.piece, info.o);
        }
    }
}

void Evaluator::getIndexDiffs(const NodeInfo *source, NodeInfo *target, ColorType c,
                              IndexArray &added, IndexArray &removed, size_t &added_count,
                              size_t &removed_count) {
    // "source" is a position prior to the one for which we want
    // to get a NNUE eval ("target").
    added_count = removed_count = 0;
    for (NodeInfo *n = target; n != source; --n) {
        if (c == White)
            getChangedIndices<White>(n, added, removed, added_count, removed_count);
        else
            getChangedIndices<Black>(n, added, removed, added_count, removed_count);
    }
}

// Incremental update of 1/2 of accumulator for the specified color
void Evaluator::updateAccumIncremental(const Network &network, const NodeInfo *source,
                                       NodeInfo *target, const ColorType c) {
    IndexArray added, removed;
    size_t added_count, removed_count;
    getIndexDiffs(source, target, c, added, removed, added_count, removed_count);
    AccumulatorHalf sourceHalf = Network::AccumulatorType::getHalf(c, source->stm);
    AccumulatorHalf targetHalf = Network::AccumulatorType::getHalf(c, target->stm);
    // update based on diffs
    network.updateAccum(source->accum, sourceHalf, target->accum, targetHalf, added, added_count,
                        removed, removed_count);
    ciTarget.getAccumulator().setState(targetHalf, AccumulatorState::Computed);
#ifdef NNUE_TRACE
    std::cout << "incremental evaluate" << std::endl;
    ciTarget.getAccumulator().print(targetHalf, std::cout);
#endif
}

// Update the accumulator based on a position (incrementally if possible)
void Evaluator::updateAccum(const Network &network, const Board &board, NodeInfo *node,
                            const ColorType c) {
    // see if incremental update is possible
    int gain = board.allOccupied.bitCount();
    AccumulatorHalf half;
    AccumulatorHalf targetHalf = half = Network::AccumulatorType::getHalf(node->stm, c);
    bool incrementalOk = true;
    NodeInfo *n = node;
    // initial position should always be not computed
    for (NodeInfo *n = node; n->accum.getState(half) == AccumulatorState::Empty; --n) {
        const unsigned dn = node->dirty_num;
        if (dn == 0) {
            // null move, with no prior computed data
            incrementalOk = false;
            break;
        }
        const DirtyState &ds = n->dirty[0];
        if ((isKing(ds.piece) && Network::FeatureXformer::needsRefresh(c, ds.from, ds.to)) ||
            (gain -= dn + 1) < 0) {
            // king move invalidates accumulator, or no gain from incremental update
            incrementalOk = false;
            break;
        }
        half = ci.getAccumulator().getHalf(n->stm, c);
        if (n->ply == 0)
            break;
    }
    if (incrementalOk && n->accum.getState(half) == AccumulatorState::Computed) {
        // a previous position was found with usable data
        updateAccumIncremental(network, n, c);
    } else {
        // Do full update
        IndexArray indices;
        if (c == White)
            getIndices<White>(intf, indices);
        else
            getIndices<Black>(intf, indices);
        network.updateAccum(indices, targetHalf, accum);
    }
    node->accum.setState(targetHalf, AccumulatorState::Computed);
}

// evaluate the net (full evaluation)
Network::OutputType Evaluator::fullEvaluate(const Network &network, const Board &board, NodeInfo *node) {
    // Do not use the accumulator from the node
    Network::AccumulatorType accum;
    updateAccum(network, board, node, accum);
    return network.evaluate(accum, node->stm, getOutputBucket(board));
}

void Evaluator::updateAccum(const Network &network, const Board &board, const NodeInfo *node,
                            Network::AccumulatorType &accum) {
    constexpr ColorType colors[] = {White, Black};
    for (ColorType color : colors) {
        IndexArray indices;
        if (color == White)
            getIndices<White>(board, node, indices);
        else
            getIndices<Black>(board, node, indices);
        AccumulatorHalf targetHalf = Network::AccumulatorType::getHalf(node->stm, color);
        network.updateAccum(indices, targetHalf, accum);
        accum.setState(targetHalf, AccumulatorState::Computed);
    }
}
