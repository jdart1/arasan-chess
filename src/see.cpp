// Copyright 1996, 1997, 2000, 2008, 2013, 2017-2021 by Jon Dart.
// All Rights Reserved.

#include "see.h"
#include "params.h"
#include "constant.h"

#include <algorithm>
#include <array>
#include <cassert>

//#define ATTACK_TRACE

static Square minAttacker(const Board &board, Bitboard atcks, ColorType side) {
   if (side == White)
   {
      Bitboard retval(atcks & board.pawn_bits[White]);
      if (retval)
         return retval.firstOne();
      retval = (atcks & board.knight_bits[White]);
      if (retval)
         return retval.firstOne();
      retval = (atcks & board.bishop_bits[White]);
      if (retval)
         return retval.firstOne();
      retval = (atcks & board.rook_bits[White]);
      if (retval)
         return retval.firstOne();
      retval = (atcks & board.queen_bits[White]);
      if (retval)
         return retval.firstOne();
      if (atcks.isSet(board.kingSquare(White)))
        return board.kingSquare(White);
      else
        return InvalidSquare;
   }
   else
   {
      Bitboard retval(atcks & board.pawn_bits[Black]);
      if (retval)
         return retval.firstOne();
      retval = (atcks & board.knight_bits[Black]);
      if (retval)
         return retval.firstOne();
      retval = (atcks & board.bishop_bits[Black]);
      if (retval)
         return retval.firstOne();
      retval = (atcks & board.rook_bits[Black]);
      if (retval)
         return retval.firstOne();
      retval = (atcks & board.queen_bits[Black]);
      if (retval)
         return retval.firstOne();
      if (atcks.isSet(board.kingSquare(Black)))
        return board.kingSquare(Black);
      else
        return InvalidSquare;
   }    
}

score_t see( const Board &board, Move move ) {
   assert(!IsNull(move));
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
   Piece on_square = (TypeOfMove(move) == EnPassant) ? 
       MakePiece(Pawn,oside) : board[square];
   Bitboard opp_attacks(board.calcAttacks(square,oside));
   if (opp_attacks.isClear()) {
       // piece is undefended
#ifdef ATTACK_TRACE
       cout << "undefended, returning " << Params::Gain(move) << endl;
#endif
       return Params::Gain(move);
   }
   std::array<score_t,20> score_list;
   score_t swap_score = 0;
   score_t gain;
   Bitboard attacks[2]; 
   Square last_attack_sq[2] = {InvalidSquare, InvalidSquare};
   attacks[side] = board.calcAttacks(square,side);
   attacks[oside] = opp_attacks;
   int count = 0;

   for (;;) {
      last_attack_sq[side] = attack_square; 
      Piece attacker = board[attack_square];
#ifdef ATTACK_TRACE
      cout << " " << PieceImage(TypeOfPiece(attacker))
           << " on " <<
           FileImage(attack_square) <<
           RankImage(attack_square) <<
           " takes " << PieceImage(TypeOfPiece(on_square))
           << endl;
#endif
      gain = Params::SEE_PIECE_VALUES[TypeOfPiece(on_square)];
      if (TypeOfPiece(attacker) == Pawn && Rank(square,side) == 8) {
          if (count == 0) {
             // initial capture is a promotion (could be under-promotion)
             gain += (Params::SEE_PIECE_VALUES[PromoteTo(move)] - Params::SEE_PIECE_VALUES[Pawn]);
             on_square = MakePiece(PromoteTo(move),side);
          }
          else {
             // assume Queen promotion
             gain += Params::SEE_PIECE_VALUES[Queen] - Params::SEE_PIECE_VALUES[Pawn];
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

      assert(count < 20);
      score_list[count++] = swap_score;
      // remove piece we used from attacks
      attacks[side].clear(attack_square);
      // switch sides
      side = OppositeColor(side);
      const Square atk = last_attack_sq[side];
      if (atk != InvalidSquare &&
          TypeOfPiece(board[atk]) != Knight) {
          // add in x-ray attacks if any
          Square xray = board.getDirectionalAttack(atk,
                                                   -Attacks::directions[atk][square],
                                                   side);
          if (xray != InvalidSquare) {
             attacks[side].set(xray);
          }
      }
      if (attacks[side]) {
          // get next opponent attacker
          attack_square = minAttacker(board,attacks[side],side);
      } else {
          // no more attackers (including x-rays)
          break;
      }
   }
   assert(count >= 1);
   // minimax over the score list 
   for (int i = count-1; i > 0; --i) {
       if (i % 2 == 0) {
          score_list[i-1] = std::max<score_t>(score_list[i],score_list[i-1]);
       } else {
          score_list[i-1] = std::min<score_t>(score_list[i],score_list[i-1]);
       }
   }
#ifdef ATTACK_TRACE
   cout << "returning " << score_list[0] << endl;
#endif
   return score_list[0];
}

score_t seeSign( const Board &board, Move move, score_t threshold ) {
   assert(!IsNull(move));
#ifdef ATTACK_TRACE
   cout << "see ";
   MoveImage(move,cout);
   cout << endl;
#endif
   if (Params::Gain(move) < threshold) {
       // Even capturing the target piece "for free" would
       // not get us up to the threshold
#ifdef ATTACK_TRACE
       cout << "threshold test failed, return 0" << endl;
#endif
       return 0;
   } else if (!IsPromotion(move) &&
              (Params::Gain(move) - Params::SEE_PIECE_VALUES[PieceMoved(move)] >= threshold)) {
       // Even the loss of the capturing piece would still leave us >= threshold
#ifdef ATTACK_TRACE
       cout << "threshold test succeeded, return 1" << endl;
#endif
       return 1;
   }
   ColorType my_side = PieceColor(board[StartSquare(move)]);
   ColorType side = my_side;
   ColorType oside = OppositeColor(side);
   Square square = DestSquare(move);
   Square attack_square = StartSquare(move);
   Piece on_square = (TypeOfMove(move) == EnPassant) ? 
       MakePiece(Pawn,oside) : board[square];
   Bitboard opp_attacks(board.calcAttacks(square,oside));
   if (opp_attacks.isClear()) {
       // piece is undefended
#ifdef ATTACK_TRACE
       cout << "undefended, returning 1"<< endl;
#endif
       return 1;
   }
   score_t score_list[20];
   score_t swap_score = 0;
   score_t gain;
   Bitboard attacks[2]; 
   Square last_attack_sq[2] = {InvalidSquare, InvalidSquare};
   attacks[side] = board.calcAttacks(square,side);
   attacks[oside] = opp_attacks;
   int count = 0;

   for (;;) {
      last_attack_sq[side] = attack_square; 
      Piece attacker = board[attack_square];
#ifdef ATTACK_TRACE
      cout << " " << PieceImage(TypeOfPiece(attacker))
           << " on " <<
           FileImage(attack_square) <<
           RankImage(attack_square) <<
           " takes " << PieceImage(TypeOfPiece(on_square))
           << endl;
#endif
      gain = Params::SEE_PIECE_VALUES[TypeOfPiece(on_square)];
      if (TypeOfPiece(attacker) == Pawn && Rank(square,side) == 8) {
          if (count == 0) {
             // initial capture is a promotion (could be under-promotion)
             gain += (Params::SEE_PIECE_VALUES[PromoteTo(move)] - Params::SEE_PIECE_VALUES[Pawn]);
             on_square = MakePiece(PromoteTo(move),side);
          }
          else {
             // assume Queen promotion
             gain += Params::SEE_PIECE_VALUES[Queen] - Params::SEE_PIECE_VALUES[Pawn];
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

      assert(count < 20);
      score_list[count++] = swap_score;
      // remove piece we used from attacks
      attacks[side].clear(attack_square);
      // switch sides
      side = OppositeColor(side);
      if (count % 2 == 0) {
          // If it is our turn to move and we are above the threshold
          // then we can exit - if we capture it only improves the score.
          if (swap_score >= threshold) {
              assert(see(board,move) >= threshold);
              return 1;
          }
          // Futility: If capturing the opponent piece for free does
          // not bring us up to the threshold, exit. (Do not cut off
          // if we have a potential promotion).
          if ((Rank(square,side) != 8 ||
               !(attacks[side] & board.pawn_bits[side])) &&
              swap_score + Params::SEE_PIECE_VALUES[TypeOfPiece(on_square)] < threshold) {
              assert(see(board,move) < threshold);
              return 0;
          }
      } else {
          // See if opponent already has captured enough that SEE is
          // below threshold
          if (swap_score < threshold) {
              assert(see(board,move) < threshold);
              return 0;
          }
          // Futility: opponent capture cannot get us below threshold
          if ((Rank(square,side) != 8 ||
               !(attacks[side] & board.pawn_bits[side])) &&
              swap_score - Params::SEE_PIECE_VALUES[TypeOfPiece(on_square)] >= threshold) {
              assert(see(board,move) >= threshold);
              return 1;
          }
      }
      const Square atk = last_attack_sq[side];
      if (atk != InvalidSquare &&
          TypeOfPiece(board[atk]) != Knight) {
          // add in x-ray attacks if any
          Square xray = board.getDirectionalAttack(atk,
                                                   -Attacks::directions[atk][square],
                                                   side);
          if (xray != InvalidSquare) {
             attacks[side].set(xray);
          }
      }
      if (attacks[side]) {
          // get next opponent attacker
          attack_square = minAttacker(board,attacks[side],side);
      } else {
          // no more attackers (including x-rays)
          break;
      }
   }
   assert(count >= 1);
   // minimax over the score list 
   for (int i = count-1; i > 0; --i) {
       if (i % 2 == 0) {
          score_list[i-1] = std::max<score_t>(score_list[i],score_list[i-1]);
       } else {
          score_list[i-1] = std::min<score_t>(score_list[i],score_list[i-1]);
       }
   }
#ifdef ATTACK_TRACE
   cout << "returning " << (score_list[0]>=threshold) << endl;
#endif
   assert((score_list[0] >= threshold) == (see(board,move) >= threshold));
   return score_list[0] >= threshold;
}

