// Copyright 2006-2008, 2016-2019, 2021, 2023-2024, 2026 by Jon Dart. All Rights Reserved.

#ifndef _SEARCHC_H
#define _SEARCHC_H

#include "board.h"
#include "constant.h"
#include "chess.h"

#include <cassert>
#include <cstdlib>
#include <array>
#include <limits>

struct NodeInfo;
class Board;

// Correction history tables, shared across threads
struct CorrectionHistory {
    static constexpr int CORR_HIST_SIZE = 16384;
    static constexpr int CORR_HIST_MAX = 1024;
    static constexpr int CORR_HIST_SCALE = 512;
    static constexpr int PIECE_TO_COUNT = 6 * 64; // 6 piece types (Pawn..Queen) x 64 squares

    // indexed by [side to move][hash % CORR_HIST_SIZE]
    using CorrHistArray = std::array<std::array<int, CORR_HIST_SIZE>, 2>;

    // indexed by [side to move][pieceToIndex1][pieceToIndex2]
    using ContCorrHistArray = std::array<std::array<std::array<int, PIECE_TO_COUNT>, PIECE_TO_COUNT>, 2>;

    CorrectionHistory();
    ~CorrectionHistory();

    void clear();

    // Get eval correction based on correction histories
    score_t getEvalCorrection(const Board &board, const NodeInfo *node) const;

    // Update correction histories based on search result vs static eval
    void updateCorrectionHistory(const Board &board, const NodeInfo *node,
                                 score_t bestScore, score_t rawEval);

private:
    CorrHistArray *pawnCorrHistory;
    CorrHistArray *nonPawnCorrHistoryW;
    CorrHistArray *nonPawnCorrHistoryB;
    CorrHistArray *minorPieceCorrHistoryW;
    CorrHistArray *minorPieceCorrHistoryB;
    ContCorrHistArray *contCorrHistory;

    static int pieceToIndex(Move m) {
        return (PieceMoved(m) - 1) * 64 + DestSquare(m);
    }
};

// Context for a search (per-thread data). Includes killer and history information.
//
class SearchContext {
public:
    static constexpr int HISTORY_MAX = std::numeric_limits<int>::max();

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

    void clearKillers(unsigned ply) {
        killers1[ply] = killers2[ply] = NullMove;
    }
    

    void getKillers(unsigned ply,Move &k1,Move &k2) const noexcept
    {
        k1 = killers1[ply]; k2 = killers2[ply];
    }

    int historyScore(Move m, const NodeInfo *, ColorType side) const noexcept;

    void updateStats(const Board &, const NodeInfo *node);

    inline int captureHistoryScore(const Board &b, Move m) const noexcept {
        return (*captureHistory)[b[StartSquare(m)]][DestSquare(m)][Capture(m)];
    }

    int captureHistoryScoreForOrdering(const Board &b, Move m) const noexcept;

    // update a single move's history
    void updateQuietMove(const Board &board, const NodeInfo *node, Move m, bool positive, bool continuationOnly);

    void updateNonQuietMove(const Board &board, const NodeInfo *node, Move m, bool positive);

    inline Move getCounterMove(const Board &board, Move prev) const {
        ColorType oside = board.oppositeSide();
        return IsNull(prev) ? NullMove : (*counterMoves)[MakePiece(PieceMoved(prev),oside)][DestSquare(prev)];
    }

    inline void setCounterMove(const Board &board, Move prev, Move counter) {
        if (!IsNull(prev)) {
            ColorType oside = board.oppositeSide();
            (*counterMoves)[MakePiece(PieceMoved(prev), oside)][DestSquare(prev)] = counter;
        }
    }

    // get counter move history
    int getCmHistory(NodeInfo *node, Move move) const noexcept;

    // get follow up move history
    int getFuHistory(NodeInfo *node, Move move) const noexcept;

private:

    template<class T>
    using PieceToArray = std::array<std::array<T, 64>, 16>;

    template<class T>
    using PieceTypeToArray = std::array<std::array<T, 64>, 8>;

    template<class T>
    using PieceTypeToMatrix = PieceTypeToArray< PieceTypeToArray<T> >;

    template<class T>
    using ButterflyArray = std::array<std::array<std::array<T, 64>, 64>, 2>;

    // indexed by piece moved, square (to), type of piece captured
    using CaptureHistoryArray = std::array<std::array<std::array<int, 8>, 64>, 16>;

    Move killers1[Constants::MaxPly+2];
    Move killers2[Constants::MaxPly+2];

    ButterflyArray<int> *history;

    PieceToArray<Move> *counterMoves;

    PieceTypeToMatrix<int> *counterMoveHistory, *fuMoveHistory;

    CaptureHistoryArray *captureHistory;
};

#endif
