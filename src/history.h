// Copyright 2006-2008, 2011, 2016-2017 by Jon Dart. All Rights Reserved.
#ifndef _HISTORY_H
#define _HISTORY_H

#include "constant.h"
#include "chess.h"

struct NodeInfo;
class Board;

class History {

public:
    void clear();

    int scoreForOrdering (Move m, ColorType side) {
      return (int)history[MakePiece(PieceMoved(m),side)][DestSquare(m)].val;
    }

    void updateHistory(const Board &,
       NodeInfo *parentNode, Move best, int depth, ColorType side);

    static const int HISTORY_MAX = 1<<16;

 private:
    struct CACHE_ALIGN HistoryEntry {
      int32_t val;
    } history[16][64];

    void addBonus(int &val,int depth,int bonus);

    void addPenalty(int &val,int depth,int bonus);
};

#endif

