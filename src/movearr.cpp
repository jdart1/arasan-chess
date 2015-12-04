// Copyright 1997-2002, 2008, 2012 by Jon Dart. All Rights Reserved.
//
#include "movearr.h"
#include "types.h"
#include "util.h"
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
    push_back(entry);
}

void MoveArray::remove_move() {
    if (size() > 0) {
       pop_back();
    }
}

const Move &MoveArray::move( unsigned n ) const {
    ASSERT(n<num_moves());
    const MoveRecord &entry = (*this)[n];
    return entry.move();
}

unsigned MoveArray::num_moves(const ColorType side) const {
    if (side == White)
        return num_moves() ? num_moves()/2 + 1 : 0;
    else
        return num_moves() ? (num_moves()-1)/2 : 0;
}

