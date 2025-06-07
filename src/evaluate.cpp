// Copyright 2021-2025 by Jon Dart. All Rights Reserved.

#include "nnue.h"
#include "evaluate.h"
#include "globals.h"
#include "search.h"

#include <algorithm>

#define USE_FINNY

void nnue::Evaluator::clearCache() {
    for (unsigned i = 0; i < 2; i++) { // sides
        for (unsigned j = 0; j < nnue::NetworkParams::KING_BUCKETS; j++) { // buckets
            for (unsigned k = 0; k < 2; k++) { // mirrored
                auto &f = finnyTable[i][j][k];
                std::memset(f.occupancies,'\0',sizeof(Bitboard) * N_PIECES * N_COLORS);
                f.acc.setState(nnue::AccumulatorState::Empty);
                f.acc.init_half(nnue::AccumulatorHalf::Lower, globals::network.getTransformer()->getBiases());
                f.acc.init_half(nnue::AccumulatorHalf::Upper, globals::network.getTransformer()->getBiases());
            }
        }
    }
}

size_t nnue::Evaluator::getIndices(ColorType kside, const Board &board, IndexArray &out) {
    Bitboard bits(board.allOccupied);
    Square sq;
    auto it = out.begin();
    while (bits.iterate(sq)) {
        const Piece &piece = board[sq];
        *it++ = Network::getIndex(kside, board.kingSquare(kside), piece, sq);
    }
    *it = LAST_INDEX;
#ifdef NNUE_TRACE
    std::cout << "full evaluation indices:" << std::endl;
    printIndices(out);
#endif
    return it - out.begin();
}

size_t nnue::Evaluator::getIndices(ColorType kside, const Occupancies &occupancies, IndexArray &out) {
    Square kp = occupancies[King-1][kside].firstOne();
    Square sq;
    auto it = out.begin();
    if (kp != InvalidSquare) {
        for (unsigned i = 0; i < N_PIECES; ++i) {
            for (unsigned j = 0; j < 2; ++j) {
                Bitboard bits(occupancies[i][j]);
                while (bits.iterate(sq)) {
                    // Note: piece types start at Pawn = 1
                    const Piece piece = MakePiece(static_cast<PieceType>(i + 1), static_cast<ColorType>(j));
                    *it++ = Network::getIndex(kside, kp, piece, sq);
                }
            }
        }
    }
    *it = LAST_INDEX;
    return it - out.begin();
}

// compute changes indices based on the DirtyState of the current node
template <ColorType kside>
void nnue::Evaluator::getChangedIndices(const NodeInfo *node, IndexArray &added, IndexArray &removed,
                                  size_t &added_count, size_t &removed_count) {
    const Square kp = node->kingSquare(kside);
#ifdef NNUE_TRACE
    std::cout << "---" << std::endl;
#endif
    for (size_t i = 0; i < node->dirty_num; ++i) {
        const nnue::DirtyState &info = node->dirty[i];
        if (info.from != InvalidSquare) {
#ifdef NNUE_TRACE
            std::cout << "removing" << SquareImage(info.from) << std::endl;
#endif
            removed[removed_count++] = Network::getIndex(kside, kp, info.piece, info.from);
        }
        if (info.to != InvalidSquare) {
#ifdef NNUE_TRACE
            std::cout << "adding" << SquareImage(info.to) << std::endl;
#endif
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
    added[added_count] = removed[removed_count] = LAST_INDEX;
#ifdef NNUE_TRACE
    std::cout << "index diffs: added" << std::endl;
    printIndices(added);
    std::cout << "index diffs: removed" << std::endl;
    printIndices(removed);
#endif
}

// Incremental update of 1/2 of accumulator for the specified color
void nnue::Evaluator::updateAccumIncremental(const Network &network, const NodeInfo *source,
                                             NodeInfo *target, const ColorType c) {
    IndexArray added, removed;
    size_t added_count, removed_count;
    getIndexDiffs(source, target, c, added, removed, added_count, removed_count);
    AccumulatorHalf sourceHalf = Network::AccumulatorType::getHalf(c, source->stm);
    AccumulatorHalf targetHalf = Network::AccumulatorType::getHalf(c, target->stm);
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
    AccumulatorHalf targetHalf = Network::AccumulatorType::getHalf(node->stm, c);
    if (node->accum.getState(targetHalf) == AccumulatorState::Computed) {
        // nothing to do: can happen if we revist a node because we widened the search window
        return;
    } else if (node->ply != 0 && IsNull((node - 1)->last_move) && (node - 1)->num_legal) {
        AccumulatorHalf sourceHalf = (targetHalf == AccumulatorHalf::Lower ?
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
    AccumulatorHalf half = targetHalf;
    // see if incremental update is possible
    while (n->accum.getState(half) == AccumulatorState::Empty) {
        const unsigned dn = n->dirty_num;
        const DirtyState &ds = n->dirty[0];
        if ((dn == 0) ||
            (n ->ply == 0) ||
            (TypeOfPiece(ds.piece) == King && Network::FeatureXformer::needsRefresh(c, ds.from, ds.to)) ||
            ((gain -= dn + 1) < 0)) {
            // previous move was null, or was a King move that invalides the accumulator,
            // or we can't get any gain from incremental update
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
#ifdef USE_FINNY
        // update from cache
        finnyUpdate(network, board, node, c, targetHalf);
#else
        IndexArray indices;
        getIndices(c, board, indices);
        network.updateAccum(indices, targetHalf, node->accum);
#endif
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
#ifdef NNUE_TRACE
        std::cout << "full evaluation (non-incremental):" << std::endl;
        std::cout << accum << std::endl;
#endif
        return network.evaluate(accum, board.sideToMove(), getOutputBucket(board));
    }
}

void nnue::Evaluator::updateAccum(const nnue::Network &network, const Board &board,
                                  nnue::Network::AccumulatorType &accum) {
    for (ColorType color : colors) {
        IndexArray indices;
        getIndices(color, board, indices);
        AccumulatorHalf targetHalf = Network::AccumulatorType::getHalf(board.sideToMove(), color);
        network.updateAccum(indices, targetHalf, accum);
        accum.setState(targetHalf, AccumulatorState::Computed);
    }
}

void nnue::Evaluator::finnyUpdate(const nnue::Network &network, const Board &board,
                                  NodeInfo *node, ColorType side, nnue::AccumulatorHalf targetHalf) {

    const Square kp = node->kingSquare(side);
    FinnyEntry& cacheEntry = finnyTable[side][nnue::Network::FeatureXformer::getKingBucket(side,kp)][File(kp) > 4];
    nnue::Network::AccumulatorType &accum = node->accum;

    // get the index diffs between the cache entry and our position
    nnue::IndexArray add, remove;
    size_t added_count = 0, removed_count = 0;
#ifdef NNUE_TRACE
    std::cout << "--- finnyUpdate: incremental eval --" << std::endl;
    std::cout << "base indices (from cache):" << std::endl;
    IndexArray baseIndices;
    getIndices(side, cacheEntry.occupancies, baseIndices);
    printIndices(baseIndices);
    IndexArray nodeIndices;
    std::cout << "node indices:" << std::endl;
    getIndices(side, board, nodeIndices);
    printIndices(nodeIndices);
#endif
    const Occupancies &boardOcc = *(reinterpret_cast<const Occupancies*>(&board.pawn_bits[0]));
    for (ColorType color : colors) {
        for (unsigned i = 0; i < N_PIECES; ++i) {
            if (boardOcc[i][color] != cacheEntry.occupancies[i][color]) {
                Bitboard added(boardOcc[i][color] & ~cacheEntry.occupancies[i][color]);
                Bitboard removed(~boardOcc[i][color] & cacheEntry.occupancies[i][color]);
                // Note: piece types start at Pawn = 1
                PieceType piece = static_cast<PieceType>(i + 1);
                Square sq;
                while (added.iterate(sq)) {
                    add[added_count++] = Network::getIndex(side, kp, MakePiece(piece, color), sq);
                }
                while (removed.iterate(sq)) {
                    remove[removed_count++] =
                        Network::getIndex(side, kp, MakePiece(piece, color), sq);
                }
            }
        }
    }
    add[added_count] = remove[removed_count] = LAST_INDEX;
#ifdef NNUE_TRACE
    std::cout << "adding:" << std::endl;
    nnue::printIndices(add);
    std::cout << "removing:" << std::endl;
    nnue::printIndices(remove);
#endif

    // update the cache entry's accumulator, based on the index diffs. We always use the lower half
    // of the cache's accumulator.
    network.updateAccum(cacheEntry.acc, AccumulatorHalf::Lower, cacheEntry.acc, AccumulatorHalf::Lower, add, added_count,
                        remove, removed_count);
    // update occupancy data in the cache to match the computed accumulator
    // cacheEntry.occupancies = node->occupancies;
    Bitboard::bulkCopy<N_COLORS * N_PIECES>(reinterpret_cast<const Bitboard *>(&boardOcc),
                                            reinterpret_cast<Bitboard *>(&cacheEntry.occupancies));
    // update node accumulator with newly computed data from the cache
    accum.copy_half(targetHalf, cacheEntry.acc, AccumulatorHalf::Lower);
    accum.setState(targetHalf, AccumulatorState::Computed);
}
