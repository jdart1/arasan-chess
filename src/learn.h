// Copyright 1994-2002, 2004, 2008-2009, 2014, 2021, 2024 by Jon Dart.  All Rights Reserved.

#ifndef _LEARN_H
#define _LEARN_H

// Support for learning

#include "movearr.h"
#include <istream>

// If conditions are met, update the learning file. Call after a move has been added to the move
// array.
extern void learn(const Board &board, const MoveArray &moves, bool doTrace);

struct LearnRecord {
    hash_t hashcode;
    int in_check;
    score_t score;
    int depth;
    Square start, dest;
    PieceType promotion;
};

// Retrieve position learning info
extern int getLearnRecord(std::istream &learnFile, LearnRecord &);

#endif
