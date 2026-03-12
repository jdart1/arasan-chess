// Copyright 2006-2008, 2011, 2017-2021, 2023-2024, 2026 by Jon Dart. All Rights Reserved.

#include "searchc.h"
#include "search.h"
#include "tunable.h"

#include <algorithm>
#include <cassert>
#include <cmath>

TUNABLE(MAX_HISTORY_DEPTH,17,8,20);
TUNABLE(HISTORY_BASE,-10,-150,150);
TUNABLE(HISTORY_SLOPE,25,0,600);
TUNABLE(HISTORY_SLOPE2,5,0,15);
TUNABLE(MAX_CAPTURE_HISTORY_DEPTH,10,4,20);
TUNABLE(CAPTURE_HISTORY_BASE,-129,-150,150);
TUNABLE(CAPTURE_HISTORY_SLOPE,440,0,600);
TUNABLE(CAPTURE_HISTORY_SLOPE2,4,0,15);
TUNABLE(CAPTURE_HISTORY_ORDERING_DIVISOR,111,1,128);
TUNABLE(CORR_HIST_EVAL_DIVISOR,663,128,1024);
TUNABLE(CORR_PAWN_WEIGHT,28,0,128);
TUNABLE(CORR_NON_PAWN_WEIGHT,21,0,128);
TUNABLE(CORR_MINOR_PIECE_WEIGHT,13,0,128);
TUNABLE(CORR_CONT_WEIGHT,60,0,128);
TUNABLE(CORR_HIST_MAX_BONUS,156,100,400);

// not tunable
static constexpr int HISTORY_DIVISOR = 2048;
static constexpr int CAPTURE_HISTORY_DIVISOR = 2048;
static constexpr int CORR_HIST_DIVISOR = 1024;

static inline void update(int &val, int bonus, int divisor, bool is_best) {
    assert(bonus >= 0);
    val -= val * bonus / divisor;
    if (is_best)
        val += bonus;
    else
        val -= bonus;
}

static inline void updateCorrHist(int &val, int bonus, int divisor) {
    val = val + bonus - (val * std::abs(bonus)) / divisor;
}

static int bonus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    if (d > MAX_HISTORY_DEPTH)
        return 0;
    else
        return HISTORY_BASE + HISTORY_SLOPE*d + HISTORY_SLOPE2*d*d;
}

static int captureBonus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    if (d > MAX_CAPTURE_HISTORY_DEPTH)
        return 0;
    else
        return CAPTURE_HISTORY_BASE + CAPTURE_HISTORY_SLOPE*d + CAPTURE_HISTORY_SLOPE2*d*d;
}

CorrectionHistory::CorrectionHistory() {
    pawnCorrHistory = new CorrHistArray();
    nonPawnCorrHistoryW = new CorrHistArray();
    nonPawnCorrHistoryB = new CorrHistArray();
    minorPieceCorrHistoryW = new CorrHistArray();
    minorPieceCorrHistoryB = new CorrHistArray();
    contCorrHistory = new ContCorrHistArray();
    clear();
}

CorrectionHistory::~CorrectionHistory() {
    delete pawnCorrHistory;
    delete nonPawnCorrHistoryW;
    delete nonPawnCorrHistoryB;
    delete minorPieceCorrHistoryW;
    delete minorPieceCorrHistoryB;
    delete contCorrHistory;
}

void CorrectionHistory::clear() {
    for (int side = 0; side < 2; side++) {
        (*pawnCorrHistory)[side].fill(0);
        (*nonPawnCorrHistoryW)[side].fill(0);
        (*nonPawnCorrHistoryB)[side].fill(0);
        (*minorPieceCorrHistoryW)[side].fill(0);
        (*minorPieceCorrHistoryB)[side].fill(0);
        for (int i = 0; i < PIECE_TO_COUNT; i++) {
            (*contCorrHistory)[side][i].fill(0);
        }
    }
}

SearchContext::SearchContext() {
    history = new ButterflyArray<int>();
    counterMoves = new PieceToArray<Move>();
    counterMoveHistory = new PieceTypeToMatrix<int>();
    fuMoveHistory = new PieceTypeToMatrix<int>();
    captureHistory = new CaptureHistoryArray();
    clear();
}

SearchContext::~SearchContext() {
    delete history;
    delete counterMoves;
    delete counterMoveHistory;
    delete fuMoveHistory;
    delete captureHistory;
}

void SearchContext::clear() {
    clearKiller();
    for (int side = 0; side < 2; side++)
        for (int i = 0; i < 64; i++) {
            (*history)[side][i].fill(0);
        }
    for (int p = 0; p < 16; p++)
        for (int sq = 0; sq < 64; sq++) {
            (*captureHistory)[p][sq].fill(0);
        }
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 64; j++) {
            (*counterMoves)[i][j] = NullMove;
        }
    }
    // clear counter move history
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 64; j++)
            for (int k = 0; k < 8; k++) {
                (*counterMoveHistory)[i][j][k].fill(0);
                (*fuMoveHistory)[i][j][k].fill(0);
            }
}

void SearchContext::clearKiller() {
    for (int i = 0; i < Constants::MaxPly + 2; i++) {
        killers1[i] = killers2[i] = NullMove;
    }
}

int SearchContext::historyScore(Move m, const NodeInfo *node,
                                ColorType side) const noexcept {
    int score = (*history)[side][StartSquare(m)][DestSquare(m)];
    if (node->ply > 0 && !IsNull((node - 1)->last_move)) {
        Move prevMove = (node - 1)->last_move;
        score +=
            (*counterMoveHistory)[PieceMoved(prevMove)][DestSquare(prevMove)]
                                 [PieceMoved(m)][DestSquare(m)];
    }
    if (node->ply > 1 && !IsNull((node - 2)->last_move)) {
        Move prevMove = (node - 2)->last_move;
        score += (*fuMoveHistory)[PieceMoved(prevMove)][DestSquare(prevMove)]
                                 [PieceMoved(m)][DestSquare(m)];
    }
    return score;
}

void SearchContext::updateStats(const Board &board, const NodeInfo *node) {
    // sanity checks
    assert(!IsNull(node->best));
    assert(OnBoard(StartSquare(node->best)) && OnBoard(DestSquare(node->best)));
    assert(node->num_quiets < Constants::MaxMoves);
    if (CaptureOrPromotion(node->best)) {
        assert(node->num_captures<Constants::MaxCaptures);
        for (int i = 0; i < node->num_captures; i++) {
            const Move m = node->captures[i];
            updateNonQuietMove(board, node, m, MovesEqual(m, node->best));
        }
    } else {
        // Do not update on fail high of 1st quiet and low depth (idea from
        // Ethereal).
        if (node->num_quiets == 1 && node->depth <= 3 * DEPTH_INCREMENT)
            return;
        for (int i = 0; i < node->num_quiets; i++) {
            const Move m = node->quiets[i];
            updateQuietMove(board, node, m, MovesEqual(m,node->best), false);
        }
    }
}

void SearchContext::updateQuietMove(const Board &board, const NodeInfo *node, Move m,
                                    bool positive, bool continuationOnly) {
    const int b = bonus(node->depth);
    if (!continuationOnly) {
        update((*history)[board.sideToMove()][StartSquare(m)][DestSquare(m)], b,
           HISTORY_DIVISOR, positive);
        if (positive && PieceMoved(m) != Pawn) {
            update((*history)[board.sideToMove()][DestSquare(m)][StartSquare(m)], b,
                   HISTORY_DIVISOR, false);
        }
    }
    if (node->ply > 0) {
        Move lastMove = (node - 1)->last_move;
        if (!IsNull(lastMove)) {
            update((*counterMoveHistory)[PieceMoved(lastMove)][DestSquare(
                       lastMove)][PieceMoved(m)][DestSquare(m)],
                   b, HISTORY_DIVISOR, positive);
        }
        if (node->ply > 1) {
            lastMove = (node - 2)->last_move;
            if (!IsNull(lastMove)) {
                update((*fuMoveHistory)[PieceMoved(lastMove)][DestSquare(
                           lastMove)][PieceMoved(m)][DestSquare(m)],
                       b, HISTORY_DIVISOR, positive);
            }
        }
    }
}

void SearchContext::updateNonQuietMove(const Board &board, const NodeInfo *node, Move m, bool positive) {
    update((*captureHistory)[board[StartSquare(m)]][DestSquare(m)][Capture(m)],
            captureBonus(node->depth),
            CAPTURE_HISTORY_DIVISOR, positive);
}

int SearchContext::getCmHistory(NodeInfo *node, Move move) const noexcept {
    if (node->ply == 0 || IsNull((node - 1)->last_move)) {
        return 0;
    }
    Move prev((node - 1)->last_move);
    return (*counterMoveHistory)[PieceMoved(prev)][DestSquare(prev)]
                                [PieceMoved(move)][DestSquare(move)];
}

int SearchContext::getFuHistory(NodeInfo *node, Move move) const noexcept {
    if (node->ply < 2 || IsNull((node - 2)->last_move)) {
        return 0;
    }
    Move prev((node - 2)->last_move);
    return (*fuMoveHistory)[PieceMoved(prev)][DestSquare(prev)]
                           [PieceMoved(move)][DestSquare(move)];
}

int SearchContext::captureHistoryScoreForOrdering(const Board &b, Move m) const noexcept {
    return captureHistoryScore(b, m) / CAPTURE_HISTORY_ORDERING_DIVISOR;
}

score_t CorrectionHistory::getEvalCorrection(const Board &board,
                                              const NodeInfo *node) const {
    const int side = board.sideToMove();
    int corr = 0;
    corr += CORR_PAWN_WEIGHT *
            (*pawnCorrHistory)[side][board.pawnHash() % CORR_HIST_SIZE];
    corr += CORR_NON_PAWN_WEIGHT *
            (*nonPawnCorrHistoryW)[side][board.nonPawnHash(White) % CORR_HIST_SIZE];
    corr += CORR_NON_PAWN_WEIGHT *
            (*nonPawnCorrHistoryB)[side][board.nonPawnHash(Black) % CORR_HIST_SIZE];
    corr += CORR_MINOR_PIECE_WEIGHT *
            (*minorPieceCorrHistoryW)[side][board.minorPieceHash(White) % CORR_HIST_SIZE];
    corr += CORR_MINOR_PIECE_WEIGHT *
            (*minorPieceCorrHistoryB)[side][board.minorPieceHash(Black) % CORR_HIST_SIZE];
    if (node->ply >= 2 && !IsNull((node - 1)->last_move)) {
        if (!IsNull((node - 2)->last_move)) {
            corr += CORR_CONT_WEIGHT *
                (*contCorrHistory)[side][pieceToIndex((node - 1)->last_move)]
                [pieceToIndex((node - 2)->last_move)];
        }
        if (node->ply >= 4 && !IsNull((node - 4)->last_move)) {
            corr += CORR_CONT_WEIGHT *
                (*contCorrHistory)[side][pieceToIndex((node - 1)->last_move)]
                [pieceToIndex((node - 4)->last_move)];
        }

    }
    return static_cast<score_t>(corr / CORR_HIST_SCALE);
}

void CorrectionHistory::updateCorrectionHistory(const Board &board,
                                                const NodeInfo *node,
                                                score_t bestScore,
                                                score_t rawEval) {
    const int diff = bestScore - rawEval;
    if (diff == 0)
        return;
    const int bonus = std::clamp(diff * 128 * (node->depth / DEPTH_INCREMENT) / CORR_HIST_EVAL_DIVISOR,
                                 -static_cast<int>(CORR_HIST_MAX_BONUS),
                                 static_cast<int>(CORR_HIST_MAX_BONUS));
    if (bonus == 0)
        return;
    const int side = board.sideToMove();
    updateCorrHist((*pawnCorrHistory)[side][board.pawnHash() % CORR_HIST_SIZE], bonus, CORR_HIST_DIVISOR);
    updateCorrHist((*nonPawnCorrHistoryW)[side][board.nonPawnHash(White) % CORR_HIST_SIZE], bonus,
           CORR_HIST_DIVISOR);
    updateCorrHist((*nonPawnCorrHistoryB)[side][board.nonPawnHash(Black) % CORR_HIST_SIZE], bonus,
           CORR_HIST_DIVISOR);
    updateCorrHist((*minorPieceCorrHistoryW)[side][board.minorPieceHash(White) % CORR_HIST_SIZE], bonus,
           CORR_HIST_DIVISOR);
    updateCorrHist((*minorPieceCorrHistoryB)[side][board.minorPieceHash(Black) % CORR_HIST_SIZE], bonus,
           CORR_HIST_DIVISOR);
    if (node->ply >= 2 && !IsNull((node - 1)->last_move)) {
        if (!IsNull((node - 2)->last_move)) {
            updateCorrHist((*contCorrHistory)[side][pieceToIndex((node - 1)->last_move)]
                                     [pieceToIndex((node - 2)->last_move)],
                   bonus, CORR_HIST_DIVISOR);
        }
        if (node->ply >= 4 && !IsNull((node - 4)->last_move)) {
            updateCorrHist((*contCorrHistory)[side][pieceToIndex((node - 1)->last_move)]
                                     [pieceToIndex((node - 4)->last_move)],
                           bonus / 2 /*as in Stockfish*/, CORR_HIST_DIVISOR);
        }
    }
}
