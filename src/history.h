// Copyright 2006-2008, 2011, 2016 by Jon Dart. All Rights Reserved.
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
      unsigned succ = (unsigned)history[MakePiece(PieceMoved(m),side)][DestSquare(m)].success;
      unsigned fail = (unsigned)history[MakePiece(PieceMoved(m),side)][DestSquare(m)].failure;
      return (int)(succ+fail > 0 ? (64*succ)/(succ+fail) : 0);
    }

    void updateHistory(const Board &,
       NodeInfo *parentNode, Move best, int depth, ColorType side);

    void updateHistoryMove(const Board &,
       Move best, int depth, ColorType side);

 private:
    struct HistoryEntry {
      uint32_t success, failure;
    } history[16][64];

    static int depthFactor(int depth);
};

#endif

