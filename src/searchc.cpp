// Copyright 2006-2008, 2011, 2017-2018 by Jon Dart. All Rights Reserved.

#include "searchc.h"
#include "search.h"

SearchContext::SearchContext() {
   history = new PieceToArray<int>();
   counterMoves = new PieceToArray<Move>();
   counterMoveHistory = new PieceTypeToMatrix<int>();
   clear();
}

SearchContext::~SearchContext()
{
   delete history;
   delete counterMoves;
   delete counterMoveHistory;
}

void SearchContext::clear() {
    clearKiller();
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 64; j++) {
            (*history)[i][j] = 0;
        }
    }
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 64; j++) {
            (*counterMoves)[i][j] = NullMove;
        }
    }
    // clear counter move history
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 64; j++)
            for (int k = 0; k < 8; k++)
                for (int l = 0; l < 64; l++)
                    (*counterMoveHistory)[i][j][k][l] = 0;
}

void SearchContext::clearKiller() {
   for (int i = 0; i < Constants::MaxPly; i++) {
      killers1[i] = killers2[i] = NullMove;
   }
}

static const int MAX_HISTORY_DEPTH = 15;
static const int HISTORY_DIVISOR = 64;

int SearchContext::bonus(int depth) const noexcept
{
    const int d = std::min(MAX_HISTORY_DEPTH,depth/DEPTH_INCREMENT);
    return d*d;
}

void SearchContext::update(int &val,int depth,int bonus)
{
    val = val*depth/HISTORY_DIVISOR + bonus;
}

void SearchContext::updateStats(const Board &board, NodeInfo *node)
{
    // sanity checks
    Move best = node->best;
    ASSERT(!IsNull(best));
    ASSERT(OnBoard(StartSquare(best)) && OnBoard(DestSquare(best)));
    const int b = bonus(node->depth);
    for (int i=0; i<node->num_try; i++) {
        ASSERT(i<Constants::MaxMoves);
        const Move m = node->done[i];
        if (!CaptureOrPromotion(m)) {
            auto updateHist = [&](int &val) {
                if (MovesEqual(best,m)) {
                    update(val,node->depth,b);
                }
                else {
                    update(val,node->depth,-b);
                }
            };

            updateHist((*history)[MakePiece(PieceMoved(m),board.sideToMove())][DestSquare(m)]);
            if (node->ply > 0) {
                Move lastMove = (node-1)->last_move;
                if (!IsNull(lastMove)) {
                    updateHist((*counterMoveHistory)[PieceMoved(lastMove)][DestSquare(lastMove)][PieceMoved(m)][DestSquare(m)]);
                }

            }
        }
    }
}

