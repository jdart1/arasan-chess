// Copyright 1992-2021, 2023-2024 by Jon Dart. All Rights Reserved.

#ifndef _SCORING_H
#define _SCORING_H

#include "board.h"
#include "hash.h"
#include "attacks.h"
#include "params.h"

struct NodeInfo;

#include <iostream>

class Scoring
{
    // This class does static evaluation of chess positions.

    public:

    Scoring() = default;

    ~Scoring() = default;

    // evaluate using the nnue. If set the node pointer is used to
    // enable incremental evaluation
    score_t evalu8NNUE(const Board &board, NodeInfo *node = nullptr);

    // checks for draw by repetition (returning repetition count) +
    // other draw situations. It is intended to be called from the
    // interior of the search. It does not strictly detect legal
    // draws: use isLegalDraw for that.
    static bool isDraw( const Board &board, int &rep_count, int ply);

    // Check for legal draws and certain other drawish situations
    static bool theoreticalDraw(const Board &board);

    // Turn a score into a formatted string (mate scores are
    // shown like +Mate6).
    static void printScore( score_t score, std::ostream & );

    // Output scores in the format required by the UCI protocol.
    static void printScoreUCI( score_t score, std::ostream & );

    static bool mateScore(score_t score) {
      return score != Constants::INVALID_SCORE &&
        (score>=Constants::TABLEBASE_WIN || score<=-Constants::TABLEBASE_WIN);
    }

    // Try to return a score based on bitbases, INVALID_SCORE if not found
    static score_t tryBitbase(const Board &board);

    static int distance(Square sq1, Square sq);

    template<ColorType side>
      static int KBPDraw(const Board &board);

 private:

    template <ColorType side>
        static bool theoreticalDraw(const Board &board);

    static void initBitboards();

};

#endif

