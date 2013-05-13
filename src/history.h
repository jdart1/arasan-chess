// Copyright 2006-2008, 2011 by Jon Dart. All Rights Reserved.
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
       return history[MakePiece(PieceMoved(m),side)][DestSquare(m)].order;
    }

    static int scoreForPruning (Move m, ColorType side) {
      const HistoryEntry &h = history[MakePiece(PieceMoved(m),side)][DestSquare(m)];
      if (h.total == 0) return Constants::HISTORY_MAX;
      else return h.successCount*Constants::HISTORY_MAX/h.total;
    }

    static void updateHistory(const Board &,
       NodeInfo *parentNode, Move best, int depth, ColorType side);

 private:
    static struct CACHE_ALIGN HistoryEntry {
      int successCount, failureCount, order, total;
    } history[16][64];
};

#endif

