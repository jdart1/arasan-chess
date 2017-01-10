// Copyright 2016 by Jon Dart. All Rights Reserved.
#include "syzygy.h"
#include "constant.h"
#include "debug.h"
#include "bitboard.h"

extern unsigned TB_LARGEST;

const score_t SyzygyTb::CURSED_SCORE = (score_t)1;

static const score_t valueMap[5] = {-Constants::TABLEBASE_WIN, -SyzygyTb::CURSED_SCORE, 0, SyzygyTb::CURSED_SCORE, Constants::TABLEBASE_WIN};

static const score_t valueMapNo50[5] = {-Constants::TABLEBASE_WIN, -Constants::TABLEBASE_WIN, 0, Constants::TABLEBASE_WIN, Constants::TABLEBASE_WIN};

static PieceType getPromotion(unsigned res)
{
      switch (TB_GET_PROMOTES(res)) {
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

static const Move getMove(const Board &b, unsigned res) {
    const unsigned ep = TB_GET_EP(res);
    const PieceType promoteTo = getPromotion(res);
    // Note: castling not possible
    return CreateMove(TB_GET_FROM(res),
                      TB_GET_TO(res),
                      TypeOfPiece(b[TB_GET_FROM(res)]),
                      ep ? Pawn : TypeOfPiece(b[TB_GET_TO(res)]),
                      promoteTo,
                      ep ? EnPassant : (promoteTo != Empty ? Promotion : Normal));
}

int SyzygyTb::initTB(const string &path)
{
   bool ok = syzygy_tb_init(path.c_str());
   if (!ok)
      return 0;
   else
      return TB_LARGEST;
}

int SyzygyTb::probe_root(const Board &b, score_t &score, set<Move> &rootMoves)
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
                                   (int)b.castleStatus(White)<3 ||
                                   (int)b.castleStatus(Black)<3,
                                   b.enPassantSq()==InvalidSquare ? 0 : b.enPassantSq(),
                                   b.sideToMove() == White,
                                   results);

   if (result == TB_RESULT_FAILED) {
      return 0;
   }

   const unsigned wdl = TB_GET_WDL(result);
   ASSERT(wdl<5);
   score = valueMap[wdl];
   if (b.anyRep()) {
       // In case of repetition, fall back to making the single
       // suggested tb move that minimizes DTZ.
       // Otherwise the engine may repeat the position again.
       rootMoves.insert(getMove(b,result));
       return 1;
   }
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
   return 1;
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
                                   (int)b.castleStatus(White)<3 ||
                                   (int)b.castleStatus(Black)<3,
                                   b.enPassantSq()==InvalidSquare ? 0 : b.enPassantSq(),
                                   b.sideToMove() == White);

   if (result == TB_RESULT_FAILED) {
      return 0;
   }

   unsigned wdl = TB_GET_WDL(result);
   ASSERT(wdl<5);
   if (use50MoveRule)
      score = valueMap[wdl];
   else
      score = valueMapNo50[wdl];
   return 1;
}

