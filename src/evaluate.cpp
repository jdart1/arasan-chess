// Copyright 2021-2025 by Jon Dart. All Rights Reserved.

#include "nnue.h"
#include "evaluate.h"
#include "search.h"

size_t nnue::Evaluator::getIndices(ColorType kside, const Board &board, IndexArray &out) {
#ifdef NNUE_TRACE
    std::cout << (kside == White ? "White" : "Black") << " indices:" << std::endl;
#endif
    Bitboard bits(board.allOccupied);
    Square sq;
    auto it = out.begin();
    while (bits.iterate(sq)) {
        const Piece &piece = board[sq];
#ifdef NNUE_TRACE
        std::cout << Network::getIndex(kside, board.kingSquare(kside), piece, sq) << ' ';
#endif
        *it++ = Network::getIndex(kside, board.kingSquare(kside), piece, sq);
    }
#ifdef NNUE_TRACE
    std::cout << std::endl;
#endif
    *it = LAST_INDEX;
    return it - out.begin();
}

// compute changes indices based on the DirtyState of the current node
template <ColorType kside>
void nnue::Evaluator::getChangedIndices(const NodeInfo *node, IndexArray &added, IndexArray &removed,
                                  size_t &added_count, size_t &removed_count) {
    const Square kp = node->kingSquare[kside];
    for (size_t i = 0; i < node->dirty_num; ++i) {
        const nnue::DirtyState &info = node->dirty[i];
        if (info.from != InvalidSquare) {
            removed[removed_count++] = Network::getIndex(kside, kp, info.piece, info.from);
        }
        if (info.to != InvalidSquare) {
            added[added_count++] = Network::getIndex(kside, kp, info.piece, info.to);
        }
    }
}

void nnue::Evaluator::getIndexDiffs(const NodeInfo *source, NodeInfo *target, ColorType c,
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
        assert(n->ply != 0);
    }
}

// Incremental update of 1/2 of accumulator for the specified color
void nnue::Evaluator::updateAccumIncremental(const Network &network, const NodeInfo *source,
                                             NodeInfo *target, const ColorType c) {
    IndexArray added, removed;
    size_t added_count, removed_count;
    getIndexDiffs(source, target, c, added, removed, added_count, removed_count);
    AccumulatorHalf sourceHalf = Network::AccumulatorType::getHalf(c, source->stm);
    AccumulatorHalf targetHalf = Network::AccumulatorType::getHalf(c, target->stm);
    // update based on diffs
    /*
    std::cout << "removed ";
    for (size_t i = 0; i < removed_count; ++i) {
        std::cout << removed[i] << ' ';
    }
    std::cout << std::endl;
    std::cout << "added ";
    for (size_t i = 0; i < added_count; ++i) {
        std::cout << added[i] << ' ';
    }
    std::cout << std::endl;
    */
    network.updateAccum(source->accum, sourceHalf, target->accum, targetHalf, added, added_count,
                        removed, removed_count);
    target->accum.setState(targetHalf, AccumulatorState::Computed);
#ifdef NNUE_TRACE
    std::cout << "incremental evaluate" << std::endl;
    target->accum.print(targetHalf, std::cout);
#endif
}

// Update the accumulator based on a position (incrementally if possible)
void nnue::Evaluator::updateAccum(const nnue::Network &network, const Board &board, NodeInfo *node,
                                  const ColorType c) {
    AccumulatorHalf half;
    AccumulatorHalf targetHalf = half = Network::AccumulatorType::getHalf(node->stm, c);
    if (node->accum.getState(half) == AccumulatorState::Computed) {
        // nothing to do: can happen if we revist a node because we widened the search window
        return;
    } else if (node->ply != 0 && IsNull((node - 1)->last_move) && (node - 1)->num_legal) {
        AccumulatorHalf sourceHalf = (half == AccumulatorHalf::Lower ?
                                      AccumulatorHalf::Upper :
                                      AccumulatorHalf::Lower);
        // Previous move was a null move. Since only the side to move has changed,
        // we can just swap the accumulator halves.
        if ((node - 1)->accum.getState(sourceHalf) == AccumulatorState::Computed) {
            node->accum.copy_half(targetHalf, (node - 1)->accum, sourceHalf);
            node->accum.setState(targetHalf, AccumulatorState::Computed);
            return;
        }
    }
    int gain = board.allOccupied.bitCount();
    bool incrementalOk = true;
    NodeInfo *n = node;
    // see if incremental update is possible
    while (n->accum.getState(half) == AccumulatorState::Empty) {
        const unsigned dn = n->dirty_num;
        if (dn == 0) {
            // null move, with no prior computed data
            incrementalOk = false;
            break;
        }
        const DirtyState &ds = n->dirty[0];
        if ((TypeOfPiece(ds.piece) == King && Network::FeatureXformer::needsRefresh(c, ds.from, ds.to)) ||
            (gain -= dn + 1) < 0) {
            // king move invalidates accumulator, or no gain from incremental update
            incrementalOk = false;
            break;
        }
        if (n->ply == 0) {
            incrementalOk = false;
            break;
        }
        --n;
        half = node->accum.getHalf(n->stm, c);
    }
    if (incrementalOk && n->accum.getState(half) == AccumulatorState::Computed) {
        // a previous position was found with usable data
        updateAccumIncremental(network, n, node, c);
    } else {
        // Do full update
        IndexArray indices;
        getIndices(c, board, indices);
        network.updateAccum(indices, targetHalf, node->accum);
    }
    node->accum.setState(targetHalf, AccumulatorState::Computed);
}

// evaluate the net (full evaluation)
nnue::Network::OutputType nnue::Evaluator::fullEvaluate(const nnue::Network &network, const Board &board,
                                                        NodeInfo *node) {
    if (node) {
        updateAccum(network, board, node->accum);
        return network.evaluate(node->accum, board.sideToMove(), getOutputBucket(board));
    }
    else {
        Network::AccumulatorType accum;
        updateAccum(network, board, accum);
        return network.evaluate(accum, board.sideToMove(), getOutputBucket(board));
    }
}

void nnue::Evaluator::updateAccum(const nnue::Network &network, const Board &board,
                                  nnue::Network::AccumulatorType &accum) {
    constexpr ColorType colors[] = {White, Black};
    for (ColorType color : colors) {
        IndexArray indices;
        getIndices(color, board, indices);
        AccumulatorHalf targetHalf = Network::AccumulatorType::getHalf(board.sideToMove(), color);
        network.updateAccum(indices, targetHalf, accum);
        accum.setState(targetHalf, AccumulatorState::Computed);
    }
}
