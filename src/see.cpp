// Copyright 1996, 1997, 2000, 2008 by Jon Dart.  All Rights Reserved.

#include "see.h"
#include "see.h"
#include "util.h"
#include "constant.h"
#include "debug.h"

//#define ATTACK_TRACE

int see( const Board &board, Move move )
{
   ASSERT(!IsNull(move));
#ifdef ATTACK_TRACE
   cout << "see ";
   MoveImage(move,cout);
   cout << endl;
#endif
   ColorType my_side = PieceColor(board[StartSquare(move)]);
   ColorType side = my_side;
   ColorType oside = OppositeColor(side);
   Square square = (TypeOfMove(move) == EnPassant) ?
                   board.enPassantSq() : DestSquare(move);
   Square attack_square = StartSquare(move);
   Piece attacker = board[attack_square];
   Piece on_square = board[square];
   Bitboard opp_attacks(board.calcAttacks(square,oside));
   if (opp_attacks.is_clear()) // piece is undefended
   {
      if (TypeOfMove(move) == Promotion) {
#ifdef ATTACK_TRACE
	cout << "returning " << PieceValue(on_square) + PieceValues[PromoteTo(move)] - PAWN_VALUE << endl;
#endif
         return PieceValue(on_square) + PieceValues[PromoteTo(move)] - PAWN_VALUE;
      }
      else {
#ifdef ATTACK_TRACE
	cout << "returning " << PieceValue(on_square) << endl;
#endif
         return PieceValue(on_square);
      }
   }
   else if (PieceType(attacker) != Pawn && TEST_MASK(opp_attacks,board.pawn_bits[oside])) {
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
      my_attacks |= board.getXRay(attack_square,square,side);
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
            gain += (PieceValues[PromoteTo(move)] - PAWN_VALUE);
            on_square = MakePiece(PromoteTo(move),side);
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
         attack_square = board.minAttacker(my_attacks,side);
         ASSERT(!IsInvalid(attack_square));
         my_attacks.clear(attack_square);
	 my_attacks |= board.getXRay(attack_square,square,side);
      }
      else
      {
         if (opp_attacks.is_clear())
            break;
         attack_square = board.minAttacker(opp_attacks,side);
         ASSERT(!IsInvalid(attack_square));
         opp_attacks.clear(attack_square);
	 opp_attacks |= board.getXRay(attack_square,square,side);
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

