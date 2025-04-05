// Copyright 1994-2021, 2023-2024 by Jon Dart.  All Rights Reserved.

#include "scoring.h"
#include "bhash.h"
#include "bitprobe.h"
#include "globals.h"
#include "hash.h"
#include "material.h"
#include "movegen.h"
#include "nnue.h"
#include "search.h" // for NodeInfo
#include <algorithm>
#include <cassert>
#include <climits>
#include <cstddef>
#include <iomanip>

//#define PAWN_DEBUG
//#define EVAL_DEBUG
//#define ATTACK_DEBUG

const score_t Params::SEE_PIECE_VALUES[7] = {0,
                                             Params::PAWN_VALUE,
                                             score_t(4.3 * Params::PAWN_VALUE),
                                             score_t(4.3 * Params::PAWN_VALUE),
                                             score_t(6.0 * Params::PAWN_VALUE),
                                             score_t(12.0 * Params::PAWN_VALUE),
                                             score_t(32 * Params::PAWN_VALUE)};

int Scoring::distance(Square sq1, Square sq2) {
    int rankdist = std::abs(sq1 / 8 - sq2 / 8);
    int filedist = std::abs((sq1 % 8) - (sq2 % 8));
    return std::max<int>(rankdist, filedist);
}

template <ColorType side> int Scoring::KBPDraw(const Board &board) {
    Square qsq = InvalidSquare;
    int pfile;
    if ((board.pawn_bits[side] & Attacks::file_mask[0]) == board.pawn_bits[side]) {
        qsq = MakeSquare(chess::AFILE, 8, side);
        pfile = chess::AFILE;
    } else if ((board.pawn_bits[side] & Attacks::file_mask[7]) == board.pawn_bits[side]) {
        qsq = MakeSquare(chess::HFILE, 8, side);
        pfile = chess::HFILE;
    }

    if (qsq != InvalidSquare) {

        // pawn(s) only on Rook file
        if (!TEST_MASK(board.bishop_bits[side],
                       SquareColor(qsq) == White ? Board::white_squares : Board::black_squares)) {

            // wrong color Bishop
            Square okp = board.kingSquare(OppositeColor(side));
            Square kp = board.kingSquare(side);
            int pawn = (side == White) ? board.pawn_bits[side].lastOne()
                                       : board.pawn_bits[side].firstOne();
            assert(pawn != InvalidSquare);

            // This does not cover all drawing cases but many of them:
            if (Attacks::king_attacks[qsq].isSet(okp) ||
                (std::abs(File(okp) - pfile) <= 1 && Rank(okp, side) > Rank(pawn, side) &&
                 Rank(okp, side) > Rank(kp, side))) {
                return 1; // draw
            }
        }
    }

    return 0;
}

bool Scoring::theoreticalDraw(const Board &board) {
    if (board.getMaterial(White).totalMaterialLevel() >
        board.getMaterial(Black).totalMaterialLevel())
        return theoreticalDraw<White>(board);
    else if (board.getMaterial(White).totalMaterialLevel() <
             board.getMaterial(Black).totalMaterialLevel())
        return theoreticalDraw<Black>(board);
    else
        return false;
}

// check for theoretical draws ("side" has the greater material)
template <ColorType side> bool Scoring::theoreticalDraw(const Board &board) {
    const Material &mat1 = board.getMaterial(side);
    const Material &mat2 = board.getMaterial(OppositeColor(side));
    if (mat1.materialLevel() > 6)
        return false;

    // Check for K + P vs rook pawn
    if (mat1.infobits() == Material::KP && mat2.kingOnly()) {
        Bitboard rook_pawn_mask(Attacks::file_mask[0] | Attacks::file_mask[7]);
        if (TEST_MASK(rook_pawn_mask, board.pawn_bits[side])) {
            Square kp = board.kingSquare(side);
            Square kp2 = board.kingSquare(OppositeColor(side));
            Square psq = (board.pawn_bits[side].firstOne());
            assert(OnBoard(kp));
            assert(OnBoard(kp2));
            assert(OnBoard(psq));
            return lookupBitbase(kp, psq, kp2, side, board.sideToMove()) == 0;
        } else {
            return false;
        }
    }
    // Check for wrong bishop + rook pawn(s) vs king.  Not very common but
    // we don't want to get it wrong.
    else if (mat1.pieceBits() == Material::KB && mat2.kingOnly()) {
        return KBPDraw<side>(board);
    }
    // check for KNN vs K
    else if (mat1.infobits() == Material::KNN && mat2.kingOnly()) {
        return true;
    } else {
        return false;
    }
}

bool Scoring::isDraw(const Board &board, int &rep_count, int ply) {

    // First check for draw by repetition
    rep_count = 0;

    // follow rule Crafty uses: 2 repeats if ply<=2, 1 otherwise:
    const int target = (ply <= 2) ? 2 : 1;
    if ((rep_count = board.repCount(target)) >= target) {
        return true;
    } else if (board.fiftyMoveDraw()) {
        return true;
    } else if (board.getMaterial(White).materialLevel() <= 6 &&
               board.getMaterial(Black).materialLevel() <= 6) {
        // check for insufficient material and other drawing situations
        return board.materialDraw() || theoreticalDraw(board);
    } else {
        return false;
    }
}

void Scoring::printScore(score_t score, std::ostream &str) {
    if (score <= -Constants::MATE_RANGE) {
        str << "-Mate" << int(Constants::MATE + score + 1) / 2;
    } else if (score >= Constants::MATE_RANGE) {
        str << "+Mate" << int(Constants::MATE - score + 1) / 2;
    } else if (score == Constants::TABLEBASE_WIN) {
        str << "+TbWin";
    } else if (score == -Constants::TABLEBASE_WIN) {
        str << "-TbLoss";
    } else {
        if (score >= 0)
            str << '+';
        std::ios_base::fmtflags original_flags = str.flags();
        str << std::fixed << std::setprecision(2) << (score * 1.0) / double(Params::PAWN_VALUE);
        str.flags(original_flags);
    }
}

void Scoring::printScoreUCI(score_t score, std::ostream &str) {
    if (score <= -Constants::MATE_RANGE) {
        str << "mate " << -int(Constants::MATE + score + 1) / 2;
    } else if (score >= Constants::MATE_RANGE) {
        str << "mate " << int(Constants::MATE - score + 1) / 2;
    } else
        str << "cp " << int(score * 100) / Params::PAWN_VALUE;
}

score_t Scoring::tryBitbase(const Board &board) {
    const Material &wMat = board.getMaterial(White);
    const Material &bMat = board.getMaterial(Black);
    if ((unsigned)wMat.infobits() == Material::KP && bMat.kingOnly()) {
        Square passer = board.pawn_bits[White].firstOne();
        assert(!IsInvalid(passer));
        if (lookupBitbase(board.kingSquare(White), passer, board.kingSquare(Black), White,
                          board.sideToMove())) {
            return board.sideToMove() == White ? Constants::BITBASE_WIN : -Constants::BITBASE_WIN;
        } else {
            return 0;
        }
    } else if ((unsigned)bMat.infobits() == Material::KP && wMat.kingOnly()) {
        Square passer = board.pawn_bits[Black].firstOne();
        if (lookupBitbase(board.kingSquare(Black), passer, board.kingSquare(White), Black,
                          board.sideToMove())) {
            return board.sideToMove() == Black ? Constants::BITBASE_WIN : -Constants::BITBASE_WIN;
        } else {
            return 0;
        }
    }

    return Constants::INVALID_SCORE;
}

score_t Scoring::evalu8NNUE(const Board &board, NodeInfo *node) {
    score_t nnval;
    const unsigned bucket = nnue::Evaluator::getOutputBucket(board);
    if (node) {
        nnue::Evaluator::updateAccum(globals::network, board, node, White);
        nnue::Evaluator::updateAccum(globals::network, board, node, Black);
#ifdef _DEBUG
        assert(node->accum.getState(nnue::AccumulatorHalf::Lower) ==
               nnue::AccumulatorState::Computed);
        assert(node->accum.getState(nnue::AccumulatorHalf::Upper) ==
               nnue::AccumulatorState::Computed);
        score_t score1 = static_cast<score_t>(
            globals::network.evaluate(node->accum, board.sideToMove(), bucket));
        // non-incremental eval:
        score_t score2 =
            static_cast<score_t>(nnue::Evaluator::fullEvaluate(globals::network, board));
        if (score1 != score2) {
            std::cout << board << std::endl;
            NodeInfo *n = node;
            while (n->ply) {
                --n;
                std::cout << "king positions " << n->kingSquare[White] << ' '
                          << n->kingSquare[Black] << std::endl;
                std::cout << n->ply << " ";
                MoveImage(n->last_move, std::cout);
                std::cout << std::endl;
            }
            assert(0);
        }
        nnval = score1;
#else
        nnval = static_cast<score_t>(
            globals::network.evaluate(node->accum, board.sideToMove(), bucket));
#endif
    } else {
        nnval = static_cast<score_t>(nnue::Evaluator::fullEvaluate(globals::network, board));
    }
    int ply = node ? node->ply : 0;
    return std::clamp(nnval, -Constants::MATE + (ply - 1), Constants::MATE - (ply - 1));
}
