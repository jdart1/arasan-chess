// Copyright 2006-2008, 2011, 2016 by Jon Dart. All Rights Reserved.

#include "history.h"
#include "search.h"
#include <limits>

struct History::HistoryEntry History::history[16][64];

#define HISTORY_MAX std::numeric_limits<uint32_t>::max()-(Constants::MaxPly*Constants::MaxPly)-1

int History::depthFactor(int depth) {
   return (depth/DEPTH_INCREMENT)*(depth/DEPTH_INCREMENT);
}

void History::clearHistory() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 64; j++) {
       history[i][j].success = history[i][j].failure = (uint32_t)0;
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
   for (int i=0; i<parentNode->num_try; i++) {
      ASSERT(i<Constants::MaxMoves);
      // safe to access this here because it is after slave thread
      // completion:
      const Move &m = (const Move &)parentNode->done[i];
      const Piece pieceMoved = MakePiece(PieceMoved(m),side);
      HistoryEntry &h = history[pieceMoved][DestSquare(m)];
      if (MovesEqual(best,m)) {
         h.success += bonus;
         if (h.success > HISTORY_MAX) {
            h.success /= 2;
            h.failure /= 2;
         }
      }
      else {
         h.failure += bonus;
         if (h.failure > HISTORY_MAX) {
            h.success /= 2;
            h.failure /= 2;
         }
      }
   }
}

void History::updateHistoryMove(const Board &,
                                Move best, int depth, ColorType side) 
{
   const Piece pieceMoved = MakePiece(PieceMoved(best),side);
   HistoryEntry &h = history[pieceMoved][DestSquare(best)];
   h.success += depthFactor(depth);
   if (h.success > HISTORY_MAX) {
      h.success /= 2;
      h.failure /= 2;
   }
}


