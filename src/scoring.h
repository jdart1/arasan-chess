// Copyright 1992-2021, 2023-2025 by Jon Dart. All Rights Reserved.

#ifndef _SCORING_H
#define _SCORING_H

#include "attacks.h"
#include "board.h"
#include "hash.h"
#include "evaluate.h"

struct NodeInfo;

#include <iostream>

class Scoring {
    // This class does static evaluation of chess positions.

  public:
    // Note: constructor does not initialize cache, allowing for
    // delayed initialization in a thread
    Scoring() = default;

    ~Scoring() = default;

    // Clear internal state (currently, finny tables)
    void clear() {
        evaluator.clearCache();
    }

    // evaluate using the nnue. If set the node pointer is used to
    // enable incremental evaluation
    score_t evalu8NNUE(const Board &board, NodeInfo *node = nullptr);

    // checks for draw by repetition (returning repetition count) +
    // other draw situations. It is intended to be called from the
    // interior of the search. It does not strictly detect legal
    // draws: use isLegalDraw for that.
    static bool isDraw(const Board &board, int &rep_count, int ply);

    // Check for legal draws and certain other drawish situations
    static bool theoreticalDraw(const Board &board);

    // Turn a score into a formatted string (mate scores are
    // shown like +Mate6).
    static void printScore(score_t score, std::ostream &);

    // Output scores in the format required by the UCI protocol.
    static void printScoreUCI(score_t score, std::ostream &);

    static bool mateScore(score_t score) {
        return score != Constants::INVALID_SCORE &&
               (score >= Constants::TABLEBASE_WIN || score <= -Constants::TABLEBASE_WIN);
    }

    // Try to return a score based on bitbases, INVALID_SCORE if not found
    static score_t tryBitbase(const Board &board);

    template <ColorType side> static int KBPDraw(const Board &board);

    static constexpr score_t PAWN_VALUE = (score_t)128; // midgame pawn value

    static constexpr score_t SEE_PIECE_VALUES[7] = {0,
                                                    PAWN_VALUE,
                                                    score_t(4.3 * PAWN_VALUE),
                                                    score_t(4.3 * PAWN_VALUE),
                                                    score_t(6.0 * PAWN_VALUE),
                                                    score_t(12.0 * PAWN_VALUE),
                                                    score_t(32 * PAWN_VALUE)};

    static FORCEINLINE score_t Gain(Move move) {
        return (TypeOfMove(move) == Promotion) ?
                SEE_PIECE_VALUES[Capture(move)] + SEE_PIECE_VALUES[PromoteTo(move)] - SEE_PIECE_VALUES[Pawn] :
                SEE_PIECE_VALUES[Capture(move)];
    }

    static FORCEINLINE score_t MVV_LVA(Move move) {
        return 8*Gain(move) - SEE_PIECE_VALUES[PieceMoved(move)];
    }

    static FORCEINLINE score_t maxValue(Move move) {
        return (TypeOfMove(move) == Promotion) ?
            SEE_PIECE_VALUES[Capture(move)] + SEE_PIECE_VALUES[PromoteTo(move)] - SEE_PIECE_VALUES[Pawn] :
            SEE_PIECE_VALUES[Capture(move)];
    }
  private:
    template <ColorType side> static bool theoreticalDraw(const Board &board);

    static void initBitboards();

    nnue::Evaluator evaluator;

};

#endif
