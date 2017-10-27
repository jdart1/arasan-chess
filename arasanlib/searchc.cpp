// Copyright 2006-2008, 2011, 2017 by Jon Dart. All Rights Reserved.

#include "searchc.h"
#include "search.h"

SearchContext::SearchContext() {
   counterMoveHistory = new CmhMatrix;
   clear();
}

SearchContext::~SearchContext()
{
   delete counterMoveHistory;
}

void SearchContext::clear() {
    clearKiller();
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 64; j++) {
            history[i][j].val = 0;
        }
    }
    for (int i = 0; i < REFUTATION_TABLE_SIZE; i++) refutations[i] = NullMove;
    // clear counter move history
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 64; j++)
            for (int k = 0; k < 6; k++)
                for (int l = 0; l < 64; l++)
                    (*counterMoveHistory)[i][j][k][l] = 0;
}

void SearchContext::clearKiller() {
   for (int i = 0; i < Constants::MaxPly; i++) {
      Killers1[i] = Killers2[i] = NullMove;
   }
}

void SearchContext::setKiller(const Move & move,unsigned ply) {
   if (!MovesEqual(move,Killers1[ply])) {
      Killers2[ply] = Killers1[ply];
   }
   Killers1[ply] = move;
}

void SearchContext::getKillers(unsigned ply,Move &k1,Move &k2) const {
   k1 = Killers1[ply]; k2 = Killers2[ply];
}

static const int MAX_HISTORY_DEPTH = 15;
static const int HISTORY_DIVISOR = 64;

void SearchContext::addBonus(int &val,int depth,int bonus)
{
    val = val*depth/HISTORY_DIVISOR;
    val += bonus;
    val = std::min<int>(HISTORY_MAX-1,val);
}

void SearchContext::addPenalty(int &val,int depth,int bonus)
{
     val = val*depth/HISTORY_DIVISOR;
     val -= bonus;
     val = std::max<int>(1-HISTORY_MAX,val);
}

void SearchContext::updateStats(const Board &board, NodeInfo *parentNode, Move best, int depth, ColorType side)
{
    // sanity checks
    ASSERT(!IsNull(best));
    ASSERT(OnBoard(StartSquare(best)) && OnBoard(DestSquare(best)));
    depth = std::min(MAX_HISTORY_DEPTH,depth/DEPTH_INCREMENT);
    const int bonus = depth*depth;
    if (parentNode && parentNode->num_try) {
        for (int i=0; i<parentNode->num_try; i++) {
            ASSERT(i<Constants::MaxMoves);
            // safe to access this here because it is after slave thread
            // completion:
            const Move m = parentNode->done[i];
            if (!CaptureOrPromotion(m)) {
                auto update = [&](int &val) {
                if (MovesEqual(best,m)) {
                    addBonus(val,depth,bonus);
                }
                else {
                    addPenalty(val,depth,bonus);
                }
                };

                update(history[MakePiece(PieceMoved(m),side)][DestSquare(m)].val);
                if (parentNode->ply > 0) {
                    Move lastMove = (parentNode-1)->last_move;
                    if (!IsNull(lastMove)) {
                       update((*counterMoveHistory)[PieceMoved(lastMove)-1][DestSquare(lastMove)][PieceMoved(m)-1][DestSquare(m)]);
                    }

                }
            }
        }
    } else {
        int &val = history[MakePiece(PieceMoved(best),side)][DestSquare(best)].val;
        addBonus(val,depth,bonus);
        if (parentNode && parentNode->ply > 0) {
           Move lastMove = (parentNode-1)->last_move;
		   if (!IsNull(lastMove)) {
			  val = (*counterMoveHistory)[PieceMoved(lastMove) - 1][DestSquare(lastMove)][PieceMoved(best) - 1][DestSquare(best)];
			  addBonus(val, depth, bonus);
		   }
        }
    }
}

