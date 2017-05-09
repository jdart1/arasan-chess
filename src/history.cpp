// Copyright 2006-2008, 2011, 2016-2017 by Jon Dart. All Rights Reserved.

#include "history.h"
#include "search.h"
#include "debug.h"

static const int BONUS_MAX = 18*18;

int History::depthFactor(int depth) {
    return (depth/DEPTH_INCREMENT)*(depth/DEPTH_INCREMENT);
}

void History::clear() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 64; j++) {
       history[i][j].val = 0;
    }
  }
}

void History::updateHistory(const Board &board, NodeInfo *parentNode, Move best, int depth, ColorType side)
{
   // sanity checks
   ASSERT(!IsNull(best));
   ASSERT(OnBoard(StartSquare(best)) && OnBoard(DestSquare(best)));
   ASSERT(parentNode->num_try);
   const int bonus = depthFactor(depth);
   if (bonus >= BONUS_MAX) return;
   for (int i=0; i<parentNode->num_try; i++) {
      ASSERT(i<Constants::MaxMoves);
      // safe to access this here because it is after slave thread
      // completion:
      const Move &m = (const Move &)parentNode->done[i];
      const Piece pieceMoved = MakePiece(PieceMoved(m),side);
      HistoryEntry &h = history[pieceMoved][DestSquare(m)];
      if (MovesEqual(best,m)) {
         h.val = h.val*bonus/BONUS_MAX;
         h.val += bonus*32;
         h.val = std::min<int>(HISTORY_MAX-1,h.val);
      }
      else {
         h.val = h.val*bonus/BONUS_MAX;
         h.val -= bonus*32;
         h.val = std::max<int>(1-HISTORY_MAX,h.val);
      }
   }
}

void History::updateHistoryMove(const Board &,
                                Move best, int depth, ColorType side) 
{
   const int bonus = depthFactor(depth);
   if (bonus >= BONUS_MAX) return;
   const Piece pieceMoved = MakePiece(PieceMoved(best),side);
   HistoryEntry &h = history[pieceMoved][DestSquare(best)];
   h.val = h.val*bonus/BONUS_MAX;
   h.val += bonus*32;
   h.val = std::min<int>(HISTORY_MAX-1,h.val);
}


