// Copyright 2021 by Jon Dart. All Rights Reserved.
#include "nnueintf.h"
#include "search.h"

ChessInterface::Iterator::value_type ChessInterface::Iterator::empty_val(nnue::InvalidSquare,
                                                                         nnue::EmptyPiece);;

nnue::Network::AccumulatorType &ChessInterface::getAccumulator() const
    noexcept {
    return node()->accum;
}

unsigned ChessInterface::getDirtyNum() const { return node()->dirty_num; }

void ChessInterface::getDirtyState(size_t index, nnue::Square &from,
                                   nnue::Square &to, nnue::Piece &p) const {
    const DirtyState &state = node()->dirty[index];
    from = state.from;
    to = state.to;
    p = state.piece;
}

void ChessInterface::setDirtyNum(unsigned num) { node()->dirty_num = num; }

void ChessInterface::setDirtyState(int index, nnue::Square &from,
                                   nnue::Square &to, nnue::Piece &p) {
    DirtyState &state = node()->dirty[index];
    from = state.from;
    to = state.to;
    p = state.piece;
}

bool ChessInterface::previous() {
    if (!node() || node()->ply == 0) {
        // no previous node
        return false;
    } else {
        --nodeIndex;
        stm = static_cast<nnue::Color>(OppositeColor(static_cast<ColorType>(stm)));
        return true;
    }
}

NodeInfo *ChessInterface::node() const noexcept {
    return pos->node + nodeIndex;
}

bool ChessInterface::hasPrevious() const noexcept {
    return node()->ply != 0;
}
