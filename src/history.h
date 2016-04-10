// Copyright 2006-2008, 2011, 2016 by Jon Dart. All Rights Reserved.
#ifndef _HISTORY_H
#define _HISTORY_H

#include "constant.h"
#include "chess.h"

struct NodeInfo;
class Board;

class History {

public:
    static void clearHistory();

    static int scoreForOrdering (Move m, ColorType side) {
      unsigned succ = (unsigned)history[MakePiece(PieceMoved(m),side)][DestSquare(m)].success;
      unsigned fail = (unsigned)history[MakePiece(PieceMoved(m),side)][DestSquare(m)].failure;
      return (int)(succ+fail > 0 ? (64*succ)/(succ+fail) : 0);
    }

    static void updateHistory(const Board &,
       NodeInfo *parentNode, Move best, int depth, ColorType side);

    static void updateHistoryMove(const Board &,
       Move best, int depth, ColorType side);

 private:
    static struct CACHE_ALIGN HistoryEntry {
      uint32_t success, failure;
    } history[16][64];

    static int depthFactor(int depth);
};

#endif

