// Copyright 2006-2008, 2016-2017 by Jon Dart. All Rights Reserved.

#ifndef _SEARCHC_H
#define _SEARCHC_H

#include "board.h"
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

    void setKiller(const Move & move,unsigned ply)
    {
        if (!MovesEqual(move,killers1[ply])) {
           killers2[ply] = killers1[ply];
        }
        killers1[ply] = move;
    }

    void getKillers(unsigned ply,Move &k1,Move &k2) const noexcept
    {
        k1 = killers1[ply]; k2 = killers2[ply];
    }

    int scoreForOrdering (Move m, Move prevMove, ColorType side) const noexcept {
        int score = (*history)[MakePiece(PieceMoved(m),side)][DestSquare(m)];
        if (!IsNull(prevMove))
           score += (*counterMoveHistory)[PieceMoved(prevMove)][DestSquare(prevMove)][PieceMoved(m)][DestSquare(m)];
        return score;
    }

    void updateStats(const Board &, NodeInfo *parentNode);

    static const int HISTORY_MAX = 1<<16;

    template<class T>
    using PieceToArray = std::array<std::array<T, 64>, 16>;

    template<class T>
    using PieceTypeToArray = std::array<std::array<T, 64>, 8>;

    template<class T>
    using PieceTypeToMatrix = PieceTypeToArray< PieceTypeToArray<T> >;

    // not used currently
    template<class T>
    using ButterflyArray = std::array<std::array<std::array<T, 64>, 64>, 2>;

    Move getCounterMove(const Board &board, Move prev) const {
        ColorType oside = board.oppositeSide();
        return (*counterMoves)[MakePiece(PieceMoved(prev),oside)][DestSquare(prev)];
    }

    void setCounterMove(const Board &board, Move prev, Move counter) {
        ColorType oside = board.oppositeSide();
        (*counterMoves)[MakePiece(PieceMoved(prev),oside)][DestSquare(prev)] = counter;
    }

private:

    Move killers1[Constants::MaxPly];
    Move killers2[Constants::MaxPly];

    PieceToArray<int> *history;

    PieceToArray<Move> *counterMoves;

    PieceTypeToMatrix<int> *counterMoveHistory;

    int bonus(int depth) const noexcept;

    void update(int &val,int depth,int bonus);
};

#endif
