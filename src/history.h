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
      return (unsigned)history[MakePiece(PieceMoved(m),side)][DestSquare(m)].h;
    }

    void updateHistory(const Board &,
       NodeInfo *parentNode, Move best, int depth, ColorType side);

    void updateHistoryMove(const Board &,
       Move best, int depth, ColorType side);

 private:
    struct HistoryEntry {
      int32_t h;
    } history[16][64];

    static unsigned depthFactor(int depth);
};

#endif

