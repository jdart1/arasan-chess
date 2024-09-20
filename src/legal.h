// Copyright 1998, 2008, 2018, 2024 by Jon Dart.  All Rights Reserved.

#ifndef _LEGAL_MOVE
#define _LEGAL_MOVE

#include "board.h"

// checks for validity (not the same as strict legality - 
// may pass moves into check, assuming search will find these).
bool validMove(const Board &board, Move move);

bool legalMove(const Board &board, Move m);

#endif
