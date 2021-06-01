// Copyright 2021 by Jon Dart. All Rights Reserved.
#include "nnueintf.h"
#include "search.h"

ChessInterface::Iterator::value_type ChessInterface::Iterator::empty_val(nnue::InvalidSquare,
                                                                         nnue::EmptyPiece);;

NodeInfo *Position::prev() const noexcept {
  return node - 1;
}

nnue::Network::AccumulatorType &ChessInterface::getAccumulator() const
    noexcept {
    return pos->node->accum;
}

unsigned ChessInterface::getDirtyNum() const { return pos->node->dirty_num; }

void ChessInterface::getDirtyState(int index, nnue::Square &from,
                                   nnue::Square &to, nnue::Piece &p) const {
    const DirtyState &state = pos->node->dirty[index];
    from = state.from;
    to = state.to;
    p = state.piece;
}

void ChessInterface::setDirtyNum(unsigned num) { pos->node->dirty_num = num; }

void ChessInterface::setDirtyState(int index, nnue::Square &from,
                                   nnue::Square &to, nnue::Piece &p) {
    DirtyState &state = pos->node->dirty[index];
    from = state.from;
    to = state.to;
    p = state.piece;
}

void ChessInterface::previous() {
    if (pos->node->ply == 0) {
      // no previous node
      pos->node = nullptr;
    } else {
      pos->node = pos->node-1;
    }
    stm = static_cast<nnue::Color>(OppositeColor(static_cast<ColorType>(stm)));
}

bool ChessInterface::hasPrevious() const noexcept {
    return pos->node->ply != 0;
}
