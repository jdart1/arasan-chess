// Copyright 1996, 1997, 2000, 2008, 2013 by Jon Dart.  All Rights Reserved.

#include "see.h"
#include "see.h"
#include "util.h"
#include "constant.h"
#include "debug.h"

#include <algorithm>

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
   Square square = DestSquare(move);
   Square attack_square = StartSquare(move);
   Piece attacker = board[attack_square];
   Piece on_square = (TypeOfMove(move) == EnPassant) ? 
       MakePiece(Pawn,oside) : board[square];
   Bitboard opp_attacks(board.calcAttacks(square,oside));
   if (opp_attacks.is_clear()) {
       // piece is undefended
#ifdef ATTACK_TRACE
       cout << "undefended, returning " << Gain(move) << endl;
#endif
       return Gain(move);
   }
   int score_list[20];
   int swap_score = 0;
   int gain;
   Bitboard attacks[2]; 
   Square last_attack_sq[2] = {InvalidSquare, InvalidSquare};
   attacks[side] = board.calcAttacks(square,side);
   attacks[oside] = opp_attacks;
   int count = 0;

   for (;;) {
      last_attack_sq[side] = attack_square; 
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
          Rank(square,side) == 8) {
          if (count == 0) {
             // initial capture is a promotion (could be under-promotion)
             gain += (PieceValues[PromoteTo(move)] - PAWN_VALUE);
             on_square = MakePiece(PromoteTo(move),side);
          }
          else {
             // assume Queen promotion
             gain += QUEEN_VALUE-PAWN_VALUE;
             on_square = MakePiece(Queen,side);
          }
      }
      else {
          on_square = attacker;
      }
      if (side == my_side)
          swap_score += gain;
      else
          swap_score -= gain;

      ASSERT(count < 20);
      score_list[count++] = swap_score;
      // remove piece we used from attacks
      attacks[side].clear(attack_square);
      // switch sides
      side = OppositeColor(side);
      if (last_attack_sq[side] != InvalidSquare &&
          TypeOfPiece(board[last_attack_sq[side]]) != Knight) {
          // add in x-ray attacks if any
          attacks[side] |= board.getXRay(last_attack_sq[side],square,side);
      }
      if (attacks[side]) {
          // get next opponent attacker
          attack_square = board.minAttacker(attacks[side],side);
      } else {
          // no more attackers (including x-rays)
          break;
      }
   }
   ASSERT(count >= 1);
   if (count % 2 == 0) {
       // opponent went last, cannot avoid this capture
      score_list[count-2] = score_list[count-1];
      --count;
   }
   // minimax over the score list 
   for (int i = count-1; i > 0; --i) {
       if (i % 2 == 0) {
           score_list[i-1] = max(score_list[i],score_list[i-1]);
       } else {
           score_list[i-1] = min(score_list[i],score_list[i-1]);
       }
   }
#ifdef ATTACK_TRACE
   cout << "returning " << score_list[0] << endl;
#endif
   return score_list[0];
}

