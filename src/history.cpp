// Copyright 2006-2008, 2011, 2016 by Jon Dart. All Rights Reserved.

#include "history.h"
#include "search.h"

struct History::HistoryEntry History::history[16][64];

void History::clearHistory() {
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 64; j++) {
      history[i][j].order = 0;
      history[i][j].successCount = history[i][j].failureCount = history[i][j].total = 1;
    }
  }
}

void History::updateHistory(const Board &board, NodeInfo *parentNode, Move best, int depth, ColorType side)
{
    // sanity checks
    ASSERT(!IsNull(best));
    ASSERT(OnBoard(StartSquare(best)) && OnBoard(DestSquare(best)));
    const Piece pieceMoved = MakePiece(PieceMoved(best),side);
    HistoryEntry &h = history[pieceMoved][DestSquare(best)];
    for (int i=0; i<parentNode->num_try; i++) {
        ASSERT(i<Constants::MaxMoves);
        // safe to access this here because it is after slave thread completion:
        const Move &m = (const Move &)parentNode->done[i];
        if (CaptureOrPromotion(m)) continue;
        if (MovesEqual(m,best)) {
	    h.successCount++;
	    h.total++;
	    if (h.total > Constants::HISTORY_MAX) {
               for (int i = 0; i < 16; i++) {
                  for (int j = 0; j < 64; j++) {
                     history[i][j].successCount /= 2;
                     history[i][j].failureCount /= 2;
		     history[i][j].total /= 2;
                  }
               }
	    }
            h.order += (4*depth*depth)/(DEPTH_INCREMENT*DEPTH_INCREMENT);
            if (h.order > Constants::HISTORY_MAX) {
               for (int i = 0; i < 16; i++) {
                  for (int j = 0; j < 64; j++) {
                     history[i][j].order /= 2;
                  }
               }
            }
	}
	else {
 	   h.failureCount++;
	   h.total++;
	   if (h.total > Constants::HISTORY_MAX) {
              for (int i = 0; i < 16; i++) {
                 for (int j = 0; j < 64; j++) {
                    history[i][j].successCount /= 2;
                    history[i][j].failureCount /= 2;
	            history[i][j].total /= 2;
                 }
              }
	   }
       }
    }
}

void History::updateHistoryMove(const Board &,
                                Move best, int depth, ColorType side) 
{
   const Piece pieceMoved = MakePiece(PieceMoved(best),side);
   HistoryEntry &h = history[pieceMoved][DestSquare(best)];
   h.order += (4*depth*depth)/(DEPTH_INCREMENT*DEPTH_INCREMENT);
   if (h.order > Constants::HISTORY_MAX) {
      for (int i = 0; i < 16; i++) {
         for (int j = 0; j < 64; j++) {
            history[i][j].order /= 2;
         }
      }
   }
}


