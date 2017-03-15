// Copyright 2006-2008, 2011, 2016-2017 by Jon Dart. All Rights Reserved.

#include "history.h"
#include "search.h"
#include <limits>

static const unsigned BONUS_MAX=324;

static const int32_t HISTORY_MAX=std::numeric_limits<int32_t>::max()-BONUS_MAX-1;

unsigned History::depthFactor(int depth) {
   unsigned d = unsigned(depth)/DEPTH_INCREMENT;
   return d*d + 1;
}

void History::clear() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 64; j++) {
       history[i][j].h = (uint32_t)0;
    }
  }
}

void History::updateHistory(const Board &board, NodeInfo *parentNode, Move best, int depth, ColorType side)
{
   // sanity checks
   ASSERT(!IsNull(best));
   ASSERT(OnBoard(StartSquare(best)) && OnBoard(DestSquare(best)));
   ASSERT(parentNode->num_try);
   const unsigned bonus = depthFactor(depth);
   if (bonus>=BONUS_MAX) return;
   for (int i=0; i<parentNode->num_try; i++) {
      ASSERT(i<Constants::MaxMoves);
      // safe to access this here because it is after slave thread
      // completion:
      const Move &m = (const Move &)parentNode->done[i];
      const Piece pieceMoved = MakePiece(PieceMoved(m),side);
      HistoryEntry &h = history[pieceMoved][DestSquare(m)];
      h.h = h.h*bonus/BONUS_MAX;
      if (MovesEqual(best,m)) {
         h.h += 32*bonus;
         ASSERT(h.h<HISTORY_MAX);
      }
      else {
         h.h -= 32*bonus;
         ASSERT(h.h>-HISTORY_MAX);
      }
   }
}

void History::updateHistoryMove(const Board &,
                                Move best, int depth, ColorType side) 
{
   const Piece pieceMoved = MakePiece(PieceMoved(best),side);
   HistoryEntry &h = history[pieceMoved][DestSquare(best)];
   const unsigned bonus = depthFactor(depth);
   if (bonus>=BONUS_MAX) return;
   h.h = h.h*bonus/BONUS_MAX;
   h.h += 32*bonus;
   ASSERT(h.h<HISTORY_MAX);
}


