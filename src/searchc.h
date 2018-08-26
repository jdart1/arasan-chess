// Copyright 2006-2008, 2016-2017 by Jon Dart. All Rights Reserved.

#ifndef _SEARCHC_H
#define _SEARCHC_H

#include "constant.h"
#include "chess.h"

#include <array>

struct NodeInfo;
class Board;

class SearchContext {
public:
    SearchContext();

    virtual ~SearchContext();

    void clear();

    void clearKiller();
    void setKiller(const Move & move,unsigned ply);
    void getKillers(unsigned ply,Move &k1,Move &k2) const;

    Move Killers1[Constants::MaxPly];
    Move Killers2[Constants::MaxPly];

    int scoreForOrdering (Move m, Move prevMove, ColorType side) const {
        int score = history[MakePiece(PieceMoved(m),side)][DestSquare(m)].val;
        if (!IsNull(prevMove))
           score += (*counterMoveHistory)[PieceMoved(prevMove)-1][DestSquare(prevMove)][PieceMoved(m)-1][DestSquare(m)];
        return score;
    }

    void updateStats(const Board &,
                     NodeInfo *parentNode, Move best, int depth, ColorType side);

    static const int HISTORY_MAX = 1<<16;

    template<class T>
    using PieceTypeToArray = std::array<std::array<T, 64>, 6>;

    template<class T>
    using PieceToArray = std::array<std::array<T, 64>, 16>;

    using CmhArray = PieceTypeToArray<int>;

    using CmhMatrix = PieceTypeToArray<CmhArray>;

    using CmArray = PieceToArray<Move>;

    Move getCounterMove(Move prev) const {
        return counterMoves[PieceMoved(prev)][DestSquare(prev)];
    }

    void setCounterMove(Move prev, Move counter) {
        counterMoves[PieceMoved(prev)][DestSquare(prev)] = counter;
    }

private:
    struct HistoryEntry {
        int32_t val;
    } history[16][64];

    CmArray counterMoves;

    CmhMatrix *counterMoveHistory;

    void addBonus(int &val,int depth,int bonus);

    void addPenalty(int &val,int depth,int bonus);
};

#endif
