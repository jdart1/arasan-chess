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

    using CmhArray = std::array<std::array<int, 64>, 6>;

    using CmhMatrix = std::array< std::array< CmhArray, 64>, 6 >;

    Move getRefutation(Move prev) const {
        return refutations[refutationKey(prev)];
    }

    void setRefutation(Move prev, Move ref) {
        refutations[refutationKey(prev)] = ref;
    }

private:
    struct CACHE_ALIGN HistoryEntry {
        int32_t val;
    } history[16][64];

    static const int REFUTATION_TABLE_SIZE = 16*64;

    static int refutationKey(Move ref) {
        int key = ((int)PieceMoved(ref) << 6) | (int)DestSquare(ref);
        ASSERT(key<REFUTATION_TABLE_SIZE);
        return key;
    }

    array<Move,REFUTATION_TABLE_SIZE> refutations;

    CmhMatrix *counterMoveHistory;

    void addBonus(int &val,int depth,int bonus);

    void addPenalty(int &val,int depth,int bonus);
};

#endif
