// Copyright 2006-2008, 2011, 2017-2021, 2023-2024, 2026 by Jon Dart. All Rights Reserved.

#include "searchc.h"
#include "search.h"
#include "tunable.h"

#include <algorithm>
#include <cassert>
#include <cmath>

// main (butterfly) quiet-move history: quadratic in depth, capped.
// BASE/SLOPE/MAX derived from Alexandria's history_bonus/history_malus
// (https://github.com/PGG106/Alexandria/blob/master/src/tune.h), scaled
// by HISTORY_DIVISOR / Alexandria's HH_MAX (4096/8192 = 0.5) since we
// keep a smaller divisor than Alexandria's. SLOPE2 is an added quadratic
// kicker (Alexandria's formula is linear).
TUNABLE(HISTORY_BONUS_BASE,82,-512,512);
TUNABLE(HISTORY_BONUS_SLOPE,179,1,750);
TUNABLE(HISTORY_BONUS_SLOPE2,5,0,15);
TUNABLE(HISTORY_BONUS_MAX,1388,1,4096);
TUNABLE(HISTORY_MALUS_BASE,63,-512,512);
TUNABLE(HISTORY_MALUS_SLOPE,219,1,750);
TUNABLE(HISTORY_MALUS_SLOPE2,5,0,15);
TUNABLE(HISTORY_MALUS_MAX,420,1,4096);
// capture history: quadratic in depth, capped. BASE/SLOPE/MAX derived from
// Alexandria's capthistory_bonus/capthistory_malus, scaled by
// CAPTURE_HISTORY_DIVISOR / Alexandria's CAPTHIST_MAX (4096/16384 = 0.25).
TUNABLE(CAPTURE_HISTORY_BONUS_BASE,-11,-256,256);
TUNABLE(CAPTURE_HISTORY_BONUS_SLOPE,84,1,375);
TUNABLE(CAPTURE_HISTORY_BONUS_SLOPE2,3,0,15);
TUNABLE(CAPTURE_HISTORY_BONUS_MAX,647,1,4096);
TUNABLE(CAPTURE_HISTORY_MALUS_BASE,1,-256,256);
TUNABLE(CAPTURE_HISTORY_MALUS_SLOPE,84,1,375);
TUNABLE(CAPTURE_HISTORY_MALUS_SLOPE2,4,0,15);
TUNABLE(CAPTURE_HISTORY_MALUS_MAX,256,1,4096);
// counter-move/follow-up ("continuation") history: linear in depth, capped;
// shared between counterMoveHistory and fuMoveHistory. BASE/SLOPE/MAX
// derived from Alexandria's conthistory_bonus/conthistory_malus, scaled by
// HISTORY_DIVISOR / Alexandria's CH_MAX (4096/16384 = 0.25) since this
// table shares HISTORY_DIVISOR with the main history table.
TUNABLE(CONT_HISTORY_BONUS_BASE,-48,-256,256);
TUNABLE(CONT_HISTORY_BONUS_SLOPE,56,1,375);
TUNABLE(CONT_HISTORY_BONUS_MAX,868,1,2048);
TUNABLE(CONT_HISTORY_MALUS_BASE,13,-256,256);
TUNABLE(CONT_HISTORY_MALUS_SLOPE,87,1,375);
TUNABLE(CONT_HISTORY_MALUS_MAX,179,1,4096);
TUNABLE(CAPTURE_HISTORY_ORDERING_DIVISOR,224,20,512);
TUNABLE(CORR_HIST_EVAL_DIVISOR,663,128,1024);
TUNABLE(CORR_PAWN_WEIGHT,28,0,128);
TUNABLE(CORR_NON_PAWN_WEIGHT,21,0,128);
TUNABLE(CORR_MINOR_PIECE_WEIGHT,13,0,128);
TUNABLE(CORR_CONT_WEIGHT,60,0,128);
TUNABLE(CORR_HIST_MAX_BONUS,156,100,400);

// not tunable
static constexpr int HISTORY_DIVISOR = 4096;
static constexpr int CAPTURE_HISTORY_DIVISOR = 4096;
static constexpr int CORR_HIST_DIVISOR = 1024;

// Whether a history update is rewarding the move that caused a cutoff
// (Bonus) or penalizing a move that was tried and did not (Malus). Bonus
// and malus use independently tuned magnitudes rather than assuming
// malus == -bonus.
enum class HistoryUpdateType { Bonus, Malus };

template <HistoryUpdateType T>
static inline void updateHistoryValue(int &val, int magnitude, int divisor) {
    assert(magnitude >= 0);
    val -= val * magnitude / divisor;
    if constexpr (T == HistoryUpdateType::Bonus)
        val += magnitude;
    else
        val -= magnitude;
}

// Dispatch to the Bonus or Malus update based on a runtime flag, using
// the (already depth-scaled) magnitude appropriate to each.
static inline void applyHistoryUpdate(int &val, bool positive, int bonusAmount,
                                      int malusAmount, int divisor) {
    if (positive)
        updateHistoryValue<HistoryUpdateType::Bonus>(val, bonusAmount, divisor);
    else
        updateHistoryValue<HistoryUpdateType::Malus>(val, malusAmount, divisor);
}

static inline void updateCorrHist(int &val, int bonus, int divisor) {
    val = val + bonus - (val * std::abs(bonus)) / divisor;
}

// Quiet-move (butterfly) history bonus/malus: quadratic in depth, capped
// (no longer cut off entirely at some maximum depth).
static int historyBonus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    return std::min<int>(HISTORY_BONUS_MAX,
                          HISTORY_BONUS_BASE + HISTORY_BONUS_SLOPE * d + HISTORY_BONUS_SLOPE2 * d * d);
}

static int historyMalus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    return std::min<int>(HISTORY_MALUS_MAX,
                          HISTORY_MALUS_BASE + HISTORY_MALUS_SLOPE * d + HISTORY_MALUS_SLOPE2 * d * d);
}

// Capture history bonus/malus: quadratic in depth, capped.
static int captureHistoryBonus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    return std::min<int>(CAPTURE_HISTORY_BONUS_MAX,
                          CAPTURE_HISTORY_BONUS_BASE + CAPTURE_HISTORY_BONUS_SLOPE * d +
                          CAPTURE_HISTORY_BONUS_SLOPE2 * d * d);
}

static int captureHistoryMalus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    return std::min<int>(CAPTURE_HISTORY_MALUS_MAX,
                          CAPTURE_HISTORY_MALUS_BASE + CAPTURE_HISTORY_MALUS_SLOPE * d +
                          CAPTURE_HISTORY_MALUS_SLOPE2 * d * d);
}

// Counter-move/follow-up ("continuation") history bonus/malus: linear in
// depth (not quadratic), capped. Shared between counterMoveHistory and
// fuMoveHistory.
static int contHistoryBonus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    return std::min<int>(CONT_HISTORY_BONUS_MAX, CONT_HISTORY_BONUS_BASE + CONT_HISTORY_BONUS_SLOPE * d);
}

static int contHistoryMalus(int depth) {
    const int d = std::max<int>(1, depth / DEPTH_INCREMENT);
    return std::min<int>(CONT_HISTORY_MALUS_MAX, CONT_HISTORY_MALUS_BASE + CONT_HISTORY_MALUS_SLOPE * d);
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
    if (!continuationOnly) {
        const int b = historyBonus(node->depth);
        const int malus = historyMalus(node->depth);
        applyHistoryUpdate((*history)[board.sideToMove()][StartSquare(m)][DestSquare(m)],
                            positive, b, malus, HISTORY_DIVISOR);
        if (positive && PieceMoved(m) != Pawn) {
            // penalize the reverse (to->from) entry for this move
            applyHistoryUpdate((*history)[board.sideToMove()][DestSquare(m)][StartSquare(m)],
                                false, b, malus, HISTORY_DIVISOR);
        }
    }
    if (node->ply > 0) {
        const int cb = contHistoryBonus(node->depth);
        const int cm = contHistoryMalus(node->depth);
        Move lastMove = (node - 1)->last_move;
        if (!IsNull(lastMove)) {
            applyHistoryUpdate((*counterMoveHistory)[PieceMoved(lastMove)][DestSquare(
                                   lastMove)][PieceMoved(m)][DestSquare(m)],
                                positive, cb, cm, HISTORY_DIVISOR);
        }
        if (node->ply > 1) {
            lastMove = (node - 2)->last_move;
            if (!IsNull(lastMove)) {
                applyHistoryUpdate((*fuMoveHistory)[PieceMoved(lastMove)][DestSquare(
                                       lastMove)][PieceMoved(m)][DestSquare(m)],
                                    positive, cb, cm, HISTORY_DIVISOR);
            }
        }
    }
}

void SearchContext::updateNonQuietMove(const Board &board, const NodeInfo *node, Move m, bool positive) {
    applyHistoryUpdate((*captureHistory)[board[StartSquare(m)]][DestSquare(m)][Capture(m)],
                        positive, captureHistoryBonus(node->depth), captureHistoryMalus(node->depth),
                        CAPTURE_HISTORY_DIVISOR);
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
