// Copyright 1996, 1997, 2000 by Jon Dart.  All Rights Reserved.

#include "attacke.h"
#include "attacks.h"
#include "util.h"
#include "constant.h"
#include "debug.h"

//#define ATTACK_TRACE

static void addXRay(const Board &board, Bitboard & attacks,Square attack_square,
                    Square square,ColorType side)
{
   int dir = Attacks::directions[attack_square][square];
   if (dir == 0)
      return;
   switch (dir)
   {
   case 1:
      // Find attacks on the rank
      if (!Bitboard::And(board.rooksqueens[side],Attacks::rank_mask_left[attack_square]).is_clear())
      {
         attacks.Or(Bitboard::And(board.rankAttacksLeft(attack_square),
            board.rooksqueens[side]));
      }
      break;
   case -1:
      if (!Bitboard::And(board.rooksqueens[side],Attacks::rank_mask_right[attack_square]).is_clear())
      {
         attacks.Or(Bitboard::And(board.rankAttacksRight(attack_square),
            board.rooksqueens[side]));
      }
      break;
   case 8:
      if (!Bitboard::And(board.rooksqueens[side],Attacks::file_mask_up[attack_square]).is_clear())
      {
         attacks.Or(Bitboard::And(board.fileAttacksUp(attack_square),board.rooksqueens[side]));
      }
      break;
   case -8:
     //      if (!Bitboard::And(board.rooksqueens[side],Attacks::file_mask_down[attack_square]).is_clear())
      {
         attacks.Or(Bitboard::And(board.fileAttacksDown(attack_square),board.rooksqueens[side]));
      }
      break;
   case 7:
      {
         Bitboard b(board.diagAttacksA1Upper(attack_square));
         b.And(board.bishopsqueens[side]);
         attacks.Or(b);
      }
      break;
   case -7:
      {
        Bitboard b(board.diagAttacksA1Lower(attack_square));
        b.And(board.bishopsqueens[side]);
        attacks.Or(b);
      }
      break;
   case 9:
      {
         Bitboard b(board.diagAttacksA8Upper(attack_square));
         b.And(board.bishopsqueens[side]);
         attacks.Or(b);
      }
      break;
   case -9:
      {
         Bitboard b(board.diagAttacksA8Lower(attack_square));
         b.And(board.bishopsqueens[side]);
         attacks.Or(b);
      }
      break;
   default:  
      break;
   }
}

int attack_estimate( const Board &board, const Move &emove )
{
   ASSERT(!IsNull(emove));
#ifdef ATTACK_TRACE
   cout << "attack_estimate " << MoveImage(emove) << endl;
#endif
   ColorType my_side = PieceColor(board[StartSquare(emove)]);
   ColorType side = my_side;
   ColorType oside = OppositeColor(side);
   Square square = (TypeOfMove(emove) == EnPassant) ?
                   board.enPassantSq() : DestSquare(emove);
   Square attack_square = StartSquare(emove);
   Piece attacker = board[attack_square];
   ASSERT(!IsEmptyPiece(attacker));
   Piece on_square = board[square];
   //ASSERT(!IsEmptyPiece(on_square));

   Bitboard opp_attacks(board.calcAttacks(square,oside));
   if (opp_attacks.is_clear()) // piece is undefended
   {
      if (TypeOfMove(emove) == Promotion)
         return PieceValue(on_square) + PieceValues[PromoteTo(emove)] - PAWN_VALUE;
      else 
         return PieceValue(on_square);
   }
   else if (PieceType(attacker) != Pawn && !Bitboard::And(opp_attacks,board.pawn_bits[oside]).is_clear()) {
      // defended by a pawn, return pessimistic estimate
      return (int)PieceValue(on_square)-(int)PieceValue(attacker);
   }
   int score_count = 0;
   int score_list[20];
   int swap_score = 0;
   int gain;

   Bitboard my_attacks(board.calcAttacks(square,side));
   my_attacks.clear(attack_square); // remove attacking piece
   if (TypeOfPiece(attacker) != Knight)
      addXRay(board,my_attacks,attack_square,square,side);
   int count = 0;
   for (;;)
   {
      attacker = board[attack_square];
#ifdef ATTACK_TRACE
      cout << " " << PieceImage(TypeOfPiece(attacker))
           << " on " <<
           FileImage(attack_square) <<
           RankImage(attack_square) <<
           " takes " << PieceImage(TypeOfPiece(on_square))
           << endl;
#endif
      gain = PieceValue(on_square);
      if (TypeOfPiece(attacker) == Pawn &&
          Rank(square,side) == 8)
      {
         if (count == 0) // initial capture is a promotion too
         {
            gain += (PieceValues[PromoteTo(emove)] - PAWN_VALUE);
            on_square = MakePiece(PromoteTo(emove),side);
         }
         else
         {
            gain += QUEEN_VALUE-PAWN_VALUE;
            on_square = MakePiece(Queen,side);
         }
      }
      else
         on_square = attacker;
      if (side == my_side)
         swap_score += gain;
      else
         swap_score -= gain;

      ASSERT(score_count < 20);
      score_list[score_count++] = swap_score;
      side = OppositeColor(side);
      if (side == my_side)
      {
         if (my_attacks.is_clear())
            break;
         attack_square = board.minAttacker(my_attacks,side).first_one();
         ASSERT(!IsInvalid(attack_square));
         my_attacks.clear(attack_square);
         addXRay(board,my_attacks,attack_square,square,side);
      }
      else
      {
         if (opp_attacks.is_clear())
            break;
         attack_square = board.minAttacker(opp_attacks,side).first_one();
         ASSERT(!IsInvalid(attack_square));
         opp_attacks.clear(attack_square);
         addXRay(board,opp_attacks,attack_square,square,side);
      }
      ++count;
   }
   if (score_count > 1 && score_count % 2 == 0)
   {
      // last capture is by other side, can't avoid it
      score_list[score_count-2] = score_list[score_count-1];
      --score_count; 
   }
   int i = score_count;
   while (i >= 3)
   {
      int max_score = Util::Max(score_list[i-1],score_list[i-2]);
      if (max_score < score_list[i-3])
         score_list[i-3] = max_score;
      i -= 2;
   }
#ifdef ATTACK_TRACE
   cout << "returning " << score_list[0] << endl;
#endif
   return score_list[0];
}

