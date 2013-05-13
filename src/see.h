// Copyright 1993 by Jon Dart.  All Rights Reserved.

#ifndef _SEE_H
#define _SEE_H

#include "board.h"

// This function performs a static exchange evalution ("SEE") to
// estimate the gain from making capture move "move" on board
// "board".
int see( const Board &board, Move move );

#endif

