// Copyright 2016, 2018-2019, 2021 by Jon Dart. All Rights Reserved.
#include "syzygy.h"
#include "constant.h"
#include "bitboard.h"

#include <algorithm>
#include <cassert>

#include "syzygy/src/tbprobe.h"

extern unsigned TB_LARGEST;

const score_t SyzygyTb::CURSED_SCORE = (score_t)1;

static const score_t valueMap[5] = {-Constants::TABLEBASE_WIN, -SyzygyTb::CURSED_SCORE, 0, SyzygyTb::CURSED_SCORE, Constants::TABLEBASE_WIN};

static const score_t valueMapNo50[5] = {-Constants::TABLEBASE_WIN, -Constants::TABLEBASE_WIN, 0, Constants::TABLEBASE_WIN, Constants::TABLEBASE_WIN};

static PieceType getPromotion(unsigned res)
{
      switch (res) {
      case TB_PROMOTES_QUEEN:
         return Queen;
      case TB_PROMOTES_ROOK:
         return Rook;
      case TB_PROMOTES_BISHOP:
         return Bishop;
      case TB_PROMOTES_KNIGHT:
         return Knight;
      default:
         return Empty;
      }
}

static Move getMove(const Board &b, unsigned res) {
    const unsigned ep = TB_GET_EP(res);
    const PieceType promoteTo = getPromotion(TB_GET_PROMOTES(res));
    // Note: castling not possible
    return CreateMove(TB_GET_FROM(res),
                      TB_GET_TO(res),
                      TypeOfPiece(b[TB_GET_FROM(res)]),
                      ep ? Pawn : TypeOfPiece(b[TB_GET_TO(res)]),
                      promoteTo,
                      ep ? EnPassant : (promoteTo != Empty ? Promotion : Normal));
}

int SyzygyTb::initTB(const std::string &path)
{
   bool ok = tb_init(path.c_str());
   if (!ok)
      return 0;
   else
      return TB_LARGEST;
}

int SyzygyTb::rank_root_moves(const Board &b, bool hasRepeated, bool useRule50, RootMoveList &rootMoves)
{

    // Fathom move list
    TbRootMoves tbMoves;

    Bitboard king_bits;
    king_bits.set(b.kingSquare(White));
    king_bits.set(b.kingSquare(Black));

    // Try to rank moves using DTZ tables.
    int hit = tb_probe_root_dtz((uint64_t)(b.occupied[White]),
                                (uint64_t)(b.occupied[Black]),
                                (uint64_t)king_bits,
                                (uint64_t)(b.queen_bits[Black] | b.queen_bits[White]),
                                (uint64_t)(b.rook_bits[Black] | b.rook_bits[White]),
                                (uint64_t)(b.bishop_bits[Black] | b.bishop_bits[White]),
                                (uint64_t)(b.knight_bits[Black] | b.knight_bits[White]),
                                (uint64_t)(b.pawn_bits[Black] | b.pawn_bits[White]),
                                b.state.moveCount,
                                b.castlingPossible(),
                                // Fathom expects 0 if no e.p.,
                                // e.p. target square for capture
                                // otherwise
                                b.enPassantSq()==InvalidSquare ? 0 :
                                (b.enPassantSq() +
                                 ((b.sideToMove() == White) ? 8 : -8)),
                                b.sideToMove() == White,
                                hasRepeated,
                                useRule50,
                                &tbMoves);

    if (!hit) {
        // Some or all DTZ tables are missing.
        // Try to rank moves using WDL tables as fallback.
        hit = tb_probe_root_wdl((uint64_t)(b.occupied[White]),
                                (uint64_t)(b.occupied[Black]),
                                (uint64_t)king_bits,
                                (uint64_t)(b.queen_bits[Black] | b.queen_bits[White]),
                                (uint64_t)(b.rook_bits[Black] | b.rook_bits[White]),
                                (uint64_t)(b.bishop_bits[Black] | b.bishop_bits[White]),
                                (uint64_t)(b.knight_bits[Black] | b.knight_bits[White]),
                                (uint64_t)(b.pawn_bits[Black] | b.pawn_bits[White]),
                                b.state.moveCount,
                                b.castlingPossible(),
                                // Fathom expects 0 if no e.p.,
                                // e.p. target square for capture
                                // otherwise
                                b.enPassantSq()==InvalidSquare ? 0 :
                                (b.enPassantSq() +
                                 ((b.sideToMove() == White) ? 8 : -8)),
                                b.sideToMove() == White,
                                useRule50,
                                &tbMoves);
    }

    if (hit) {
        rootMoves.clear();
        for (unsigned i = 0; i < tbMoves.size; i++) {
            const TbRootMove &tbMove = tbMoves.moves[i];
            // Convert from Fathom move type to Arasan type
            Move m = CreateMove(b,TB_MOVE_FROM(tbMove.move),
                                  TB_MOVE_TO(tbMove.move),
                                  getPromotion(TB_MOVE_PROMOTES(tbMove.move)));
            rootMoves.push_back(RootMove(m,
                                         0,
                                         tbMove.tbRank,
                                         tbMove.tbScore));

        }
        std::sort(rootMoves.begin(),rootMoves.end(),
             [](const RootMove &a, const RootMove &b)
             {
                 return a.tbRank == b.tbRank ? (a.tbScore > b.tbScore) :
                     (a.tbRank > b.tbRank);
             }
        );
        return 1;
    } else {
        for (auto &rootMove : rootMoves) {
            rootMove.tbRank = rootMove.tbScore = 0;
        }
        return 0;
    }
}

int SyzygyTb::probe_root(const Board &b, bool hasRepeated, score_t &score, MoveSet &rootMoves)
{
   score = 0;
   unsigned results[TB_MAX_MOVES];
   Bitboard king_bits;
   king_bits.set(b.kingSquare(White));
   king_bits.set(b.kingSquare(Black));
   unsigned result = tb_probe_root((uint64_t)(b.occupied[White]),
                                   (uint64_t)(b.occupied[Black]),
                                   (uint64_t)king_bits,
                                   (uint64_t)(b.queen_bits[Black] | b.queen_bits[White]),
                                   (uint64_t)(b.rook_bits[Black] | b.rook_bits[White]),
                                   (uint64_t)(b.bishop_bits[Black] | b.bishop_bits[White]),
                                   (uint64_t)(b.knight_bits[Black] | b.knight_bits[White]),
                                   (uint64_t)(b.pawn_bits[Black] | b.pawn_bits[White]),
                                   b.state.moveCount,
                                   b.castlingPossible(),
                                   // Fathom expects 0 if no e.p.,
                                   // e.p. target square for capture
                                   // otherwise
                                   b.enPassantSq()==InvalidSquare ? 0 :
                                     (b.enPassantSq() +
                                      ((b.sideToMove() == White) ? 8 : -8)),
                                   b.sideToMove() == White,
                                   results);

   if (result == TB_RESULT_FAILED) {
      return -1;
   }

   const unsigned wdl = TB_GET_WDL(result);
   assert(wdl<5);
   score = valueMap[wdl];
   if (hasRepeated) {
       // In case of repetition, fall back to making the single
       // suggested tb move that minimizes DTZ.
       // Otherwise the engine may repeat the position again.
       rootMoves.insert(getMove(b,result));
   } else {
      // In positions w/o repetition, return a move list containing
      // moves that preserved the WDL value. These will be searched.
      unsigned res;
      for (int i = 0; (res = results[i]) != TB_RESULT_FAILED; i++) {
         const unsigned moveWdl = TB_GET_WDL(res);
         if (moveWdl >= wdl) {
            // move is ok, i.e. preserves WDL value
            rootMoves.insert(getMove(b,res));
         }
      }
   }
   return TB_GET_DTZ(result);
}

int SyzygyTb::probe_wdl(const Board &b, score_t &score, bool use50MoveRule)
{
   score = 0;
   Bitboard king_bits;
   king_bits.set(b.kingSquare(White));
   king_bits.set(b.kingSquare(Black));
   unsigned result = tb_probe_wdl((uint64_t)(b.occupied[White]),
                                   (uint64_t)(b.occupied[Black]),
                                   (uint64_t)king_bits,
                                   (uint64_t)(b.queen_bits[Black] | b.queen_bits[White]),
                                   (uint64_t)(b.rook_bits[Black] | b.rook_bits[White]),
                                   (uint64_t)(b.bishop_bits[Black] | b.bishop_bits[White]),
                                   (uint64_t)(b.knight_bits[Black] | b.knight_bits[White]),
                                   (uint64_t)(b.pawn_bits[Black] | b.pawn_bits[White]),
                                   b.state.moveCount,
                                   b.castlingPossible(),
                                   // Fathom expects 0 if no e.p.,
                                   // e.p. target square for capture
                                   // otherwise
                                   b.enPassantSq()==InvalidSquare ? 0 :
                                     (b.enPassantSq() +
                                      ((b.sideToMove() == White) ? 8 : -8)),
                                   b.sideToMove() == White);

   if (result == TB_RESULT_FAILED) {
      return 0;
   }

   unsigned wdl = TB_GET_WDL(result);
   assert(wdl<5);
   if (use50MoveRule)
      score = valueMap[wdl];
   else
      score = valueMapNo50[wdl];
   return 1;
}

