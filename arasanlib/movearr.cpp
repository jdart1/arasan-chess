// Copyright 1997-2002, 2008, 2012, 2017 by Jon Dart. All Rights Reserved.
//
#include "movearr.h"
#include "types.h"
#include "debug.h"

MoveRecord::MoveRecord(const Board &board, const BoardState &previous_state,
                       const Move &move, const string &image, bool ponder)
  : my_move(move),my_hashcode(board.state.hashCode),my_state(previous_state),
    my_image(image), my_ponder(ponder)
{
}

void MoveArray::add_move( const Board &board, 
                          const BoardState &previous_state, const Move &move,
                          const string &image, bool ponder) {
    MoveRecord entry( board, previous_state, move, image, ponder );
    append(entry);
}

void MoveArray::remove_move() {
    if (entries.size() > 0) {
        entries.pop_back();
    }
}

unsigned MoveArray::num_moves(const ColorType side) const {
    if (side == White)
        return num_moves() ? num_moves()/2 + 1 : 0;
    else
        return num_moves() ? (num_moves()-1)/2 : 0;
}

