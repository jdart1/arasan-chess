// Copyright 1998, 2008, 2018 by Jon Dart.  All Rights Reserved.

#ifndef LEGAL_MOVE
#define LEGAL_MOVE

#include "board.h"

// checks for validity (not the same as strict legality - 
// may pass moves into check, assuming search will find these).
extern int validMove(const Board &board, Move move);

extern int legalMove(const Board &board, Square start,
                      Square dest);

inline int legalMove(const Board &board, Move m) {
  return legalMove(board,StartSquare(m),DestSquare(m));
}

#endif
