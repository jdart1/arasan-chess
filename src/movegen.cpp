// Copyright 1994-2021 by Jon Dart. All Rights Reserved.
//
#include "movegen.h"
#include "attacks.h"
#include "globals.h"
#include "search.h"
#include "legal.h"
#include "syzygy.h"
#include "trace.h"

#include <cstddef>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>

//#define _TRACE

extern const int Direction[2];

const int MoveGenerator::EASY_PLIES = 3;

static FORCEINLINE void swap( Move moves[], int scores[], int i, int j)
{
   std::swap(moves[i],moves[j]);
   std::swap(scores[i],scores[j]);
}

RootMoveGenerator::RootMoveGenerator(const Board &board,
                                     SearchContext *s,
                                     Move pvMove,
                                     int trace)
    : MoveGenerator(board,s,nullptr,0,pvMove,trace),
     excluded(0)
{
   batch = moves;
   batch_count = MoveGenerator::generateAllMoves(batch,0);
   for (int i=0; i < batch_count; i++) {
      RootMove me(batch[i],-Constants::MATE,0,0);
      moveList.push_back(me);
   }
   int j = batch_count;
   if (board.checkStatus() != InCheck) {
      // exclude illegal moves
      const BoardState state(board.state);
      Board tmp(board);
      j = 0;
      for (int i=0; i<batch_count; i++) {
         Move move = moveList[i].move;
         tmp.doMove(move);
         if (tmp.anyAttacks(tmp.kingSquare(tmp.oppositeSide()),tmp.sideToMove())) {
            // move was illegal, own king is in check
            tmp.undoMove(move,state);
            continue;
         }
         if (j != i) {
            // compress the move list so illegal moves are excluded
            moveList[j] = moveList[i];
         }
         ++j;
         tmp.undoMove(move,state);
      }
      moveList.erase(moveList.begin()+j,moveList.end());
   }
   reorder(pvMove,0,true);
#ifdef _TRACE
   if (master) {
      cout << "root moves:" << endl;
      for (auto it : moveList) {
         MoveImage(it.move,cout); cout << endl;
      }
   }
#endif
   batch_count = j;
   phase = LAST_PHASE;
}

RootMoveGenerator::RootMoveGenerator(const RootMoveGenerator &mg,
                                     SearchContext *s)
    : MoveGenerator(mg.board,s,nullptr,0,mg.hashMove,0)
{
   batch = moves;
   batch_count = mg.batch_count;

   for (const RootMove &rm : mg.moveList) {
      moveList.push_back(rm);
   }
   phase = LAST_PHASE;
   excluded = mg.excluded;
}

void RootMoveGenerator::reorder(Move pvMove, score_t pvScore, int depth, bool initial)
{
   reset();
   if (initial || depth <= EASY_PLIES) {
      // If this is the first sort, use SEE to assign scores to moves.
      // Else, if in the "easy move" part of the search use the scores
      // from the previous iteration to sort (do not reset them).
      for (RootMove &m : moveList) {
           ClearUsed(m.move);
           if (initial) {
              // sort winning captures first
              if (MovesEqual(m.move,pvMove)) {
                 SetPhase(m.move,HASH_MOVE_PHASE);
                 m.score = int(Params::PAWN_VALUE*100);
              } else if (CaptureOrPromotion(m.move)) {
                 int est;
                 if ((est = (int)see(board,m.move)) >= 0) {
                    SetPhase(m.move,WINNING_CAPTURE_PHASE);
                 } else {
                    SetPhase(m.move,LOSERS_PHASE);
                 }
                 m.score = est;
              }
              else {
                 SetPhase(m.move,HISTORY_PHASE);
                 m.score = 0;
              }
           }
      }
   }
   else {
      for (auto &m : moveList) {
         ClearUsed(m.move);
         // ensure the PV move is ordered first
         if (MovesEqual(m.move,pvMove)) {
             m.score = pvScore;
         } else {
             m.score = -Constants::MATE;
         }
      }
   }
   // Sort by score - top scorer (PV) will always be first
   if (moveList.size() > 1) {
       std::stable_sort(moveList.begin(),moveList.end(),
                        [](const RootMove &a, const RootMove &b)
                        {
                            return (a.score > b.score);
                        }
           );
   }
}

void RootMoveGenerator::reorderByScore() {
   reset();
   if (moveList.size() <= 1) {
      return;
   }
   for (auto it : moveList) {
      ClearUsed(it.move);
   }
   std::sort(moveList.begin(),moveList.end(),
             [](const RootMove &a, const RootMove &b)
             {
                return a.score > b.score;
             }
      );
}

void RootMoveGenerator::exclude(const MoveSet &excluded)
{
   for (int i = 0; i < batch_count; i++) {
      ClearUsed(moveList[i].move);
      if (excluded.find(moveList[i].move) != excluded.end()) {
         SetUsed(moveList[i].move);
      }
   }
}

void RootMoveGenerator::filter(const MoveSet &include)
{
   excluded = 0;
   score_t top_rank = batch_count == 0 ? 0 : moveList[0].tbRank;
   for (unsigned i = 0; i < unsigned(batch_count); i++) {
      Move m = moveList[i].move;
      int rank = moveList[i].tbRank;
       bool exclude;
      if (include.size()) {
          // Exclude moves not in include set
          exclude = include.find(m) == include.end();
      } else {
          // Exclude inferior ranked tb moves
          exclude = rank < top_rank;
      }
      if (exclude) {
          // Exclude move. Note: use the Excluded flag, since the Used flag
          // is reset each iteration
          SetExcluded(moveList[i].move);
          ++excluded;
      }
   }
}

void RootMoveGenerator::exclude(Move exclude) {
   for (int i = 0; i < batch_count; i++) {
      if (MovesEqual(moveList[i].move,exclude)) {
         SetUsed(moveList[i].move);
         break;
      }
   }
}

Move MoveGenerator::nextEvasion(int &ord) {
   if (batch_count==0) {
     if (phase == START_PHASE) {
        ++phase;
        if (!IsNull(hashMove)) {
           ord = order++;
           assert(ord<Constants::MaxMoves);
           SetPhase(hashMove,HASH_MOVE_PHASE);
           return hashMove;
        }
        ++phase;
     }
     info.init(board);
     batch_count = mg::generateEvasions(board, info, moves);
     if (batch_count == 0) {
        return NullMove;
     }
     else if (batch_count > 1) {
       int scores[Constants::MaxCaptures];
       assert(batch_count < Constants::MaxCaptures);
       int poscaps = 0, negcaps = 0;
       for (int i = 0; i < batch_count; i++) {
          if (MovesEqual(moves[i],hashMove)) {
             // make this the 1st move
             scores[i] = Constants::MATE;
             SetPhase(moves[i],HASH_MOVE_PHASE);
             if (i) swap(moves,scores,0,i);
             ++poscaps;
             // bump index so we will skip this move
             ++index;
             continue;
          }
          if (CaptureOrPromotion(moves[i])) {
             score_t gain = Params::Gain(moves[i]);
             score_t pieceVal = Params::SEE_PIECE_VALUES[PieceMoved(moves[i])];
             scores[i] = int(Params::MVV_LVA(moves[i]));
             if (gain-pieceVal > 0 || (scores[i] = (int)see(board,moves[i])) >= 0) {
                ++poscaps;
                SetPhase(moves[i],WINNING_CAPTURE_PHASE);
                if (i > poscaps) {
                   // move positive captures to front of list
                   swap(moves,scores,i,poscaps);
                }
             } else {
                SetPhase(moves[i],LOSERS_PHASE);
                ++negcaps;
             }
          } else {
             SetPhase(moves[i],HISTORY_PHASE);
             scores[i] = 0;
          }
       }
       if (poscaps > 1) {
          mg::sortMoves(moves,scores,negcaps ? batch_count : poscaps);
       } else if (negcaps) {
          mg::sortMoves(moves,scores,batch_count);
       }
     }
     else if (MovesEqual(moves[0],hashMove)) {
        // sole evasion move is the hash move
        index++;
     }
   }
   if (index < batch_count) {
      ord = order++;
      assert(ord<Constants::MaxMoves);
      return moves[index++];
   }
   else
      return NullMove;
}


int MoveGenerator::getBatch(Move *&batch,int &index)
{
   int numMoves  = 0;
   batch = moves;
   while(numMoves == 0 && phase<LOSERS_PHASE) {
      phase++;
#ifdef _TRACE
      if (master) {
         indent(ply); cout << "phase = " << phase << endl;
      }
#endif
      switch(phase) {
         case HASH_MOVE_PHASE:
         {
            if (!IsNull(hashMove)) {
               *moves = hashMove;
               SetPhase(*moves,phase);
#ifdef _TRACE
               if (master) {
                  indent(ply); cout << "hash move = ";
                  MoveImage(hashMove,cout);
                  cout << endl;
               }
#endif
               index = 0;
               numMoves = 1;
            }
            break;
         }
         case WINNING_CAPTURE_PHASE:
         {
            numMoves = mg::generateCaptures(board,moves,ply==0);
            mg::initialSortCaptures(moves,numMoves);
            index = 0;
            break;
         }
         case KILLER1_PHASE:
         {
            if (!context) continue;
            context->getKillers(ply,killer1,killer2);
            if (!IsNull(killer1) && !MovesEqual(hashMove,killer1)) {
               if (validMove(board,killer1)) {
                  SetPhase(killer1,KILLER1_PHASE);
                  moves[numMoves++] = killer1;
                  index = 0;
               }
            }
         }
         break;
         case KILLER2_PHASE:
         {
            if (!context) continue;
            if (!IsNull(killer2) && !MovesEqual(hashMove,killer2)) {
               if (validMove(board,killer2)) {
                  SetPhase(killer2,KILLER2_PHASE);
                  moves[numMoves++] = killer2;
                  index = 0;
               }
            }
         }
         break;
         case COUNTER_MOVE_PHASE:
         break;
         case HISTORY_PHASE:
         {
            numMoves = mg::generateNonCaptures(board,moves);
            if (numMoves) {
               Move counter(context && node && ply > 0 ? context->getCounterMove(board,(node-1)->last_move) :
                            NullMove);
               int scores[Constants::MaxMoves];
               for (int i = 0; i < numMoves; i++) {
                  scores[i] = 0;
                  if (MovesEqual(hashMove,moves[i])) {
                     SetUsed(moves[i]);
                     continue;
                  }
                  else if (MovesEqual(killer1,moves[i]) ||
                  MovesEqual(killer2,moves[i])) {
                     SetUsed(moves[i]);
                     continue;
                  }
                  SetPhase(moves[i],HISTORY_PHASE);
                  if (MovesEqual(counter,moves[i])) {
                     // score counter move much higher
                     scores[i] = SearchContext::HISTORY_MAX;
                     // and put in separate phase
                     SetPhase(moves[i],COUNTER_MOVE_PHASE);
                  }
                  else if (context && node) {
                     scores[i] = context->scoreForOrdering(moves[i],node,board.sideToMove());
                  }
               }
               if (numMoves > 1) {
                  mg::sortMoves(moves,scores,numMoves);
               }
            }
            index = 0;
            break;
         }
         case LOSERS_PHASE:
         {
            if (losers_count) {
               batch = losers;
            }
            numMoves = losers_count;
            index = 0;
            break;
         }
         default:
            break;
      }                                           // end switch
   }                                              // end for
   return numMoves;
}


MoveGenerator::MoveGenerator( const Board &ABoard,
                              SearchContext *s, NodeInfo *n, unsigned curr_ply, Move pvMove,
                              int trace)
    : board(ABoard),
      context(s),
      node(n),
      ply(curr_ply),
      moves_generated(0),
      losers_count(0),
      index(0),
      order(0),
      batch_count(0),
      forced(0),
      phase(START_PHASE),
      hashMove(pvMove),
      master(trace)
{
}

unsigned MoveGenerator::generateAllMoves(Move *moves,int repeatable)
{
   unsigned numMoves  = 0;
   if (board.checkStatus() == InCheck) {
       info.init(board);
       numMoves = mg::generateEvasions(board, info, moves);
   }
   else {
      numMoves += mg::generateCaptures(board,moves+numMoves,ply==0);
      numMoves += mg::generateNonCaptures(board,moves+numMoves);
   }
   if (repeatable) {
      int scores[Constants::MaxMoves];
      for (unsigned i = 0; i < numMoves; i++) {
         scores[i] = (int)StartSquare(moves[i]) + (int)(DestSquare(moves[i]) << 7);
         switch (PromoteTo(moves[i])) {
            case Queen:
               scores[i] &= 0xB000;
               break;
            case Rook:
               scores[i] &= 0x8000;
               break;
            case Bishop:
               scores[i] &= 0x4000;
               break;
            default:
               break;
         }
      }
      mg::sortMoves(moves, scores, numMoves);
   }
   return numMoves;
}

uint64_t RootMoveGenerator::perft(Board &b, int depth) {
   if (depth == 0) return 1;

   uint64_t nodes = 0ULL;
   Move m;
   RootMoveGenerator mg(b);
   BoardState state = b.state;
   int order;
   while ((m = mg.nextMove(order)) != NullMove) {
      if (depth > 1) {
         b.doMove(m);
         nodes += perft(b,depth-1);
         b.undoMove(m,state);
      } else {
         // skip do/undo
         nodes++;
      }
   }
   return nodes;
}

int RootMoveGenerator::rank_root_moves()
{
#ifdef SYZYGY_TBS
    const Material &wMat = board.getMaterial(White);
    const Material &bMat = board.getMaterial(Black);
    int tb_pieces = wMat.men() + bMat.men();
    int tb_hit = 0;
    if (tb_pieces <= globals::EGTBMenCount && !board.castlingPossible()) {
        tb_hit = SyzygyTb::rank_root_moves(board,
                                           board.anyRep(),
                                           globals::options.search.syzygy_50_move_rule,
                                           moveList);
    }
    if (tb_hit) {
        // Sort moves on descending rank returned by Fathom
        std::stable_sort(moveList.begin(),moveList.end(),
                         [](const RootMove &a, const RootMove &b) { return a.tbRank > b.tbRank; });
    }
    return tb_hit;
#else
    return 0;
#endif
}

unsigned mg::generateNonCaptures(const Board &board, Move *moves)
{
   unsigned numMoves = 0;
   // castling moves
   const ColorType side = board.sideToMove();
   CastleType CS = board.castleStatus(side);
   if ((CS == CanCastleEitherSide) ||
   (CS == CanCastleKSide)) {
      const Square kp = board.kingSquare(side);
#ifdef _DEBUG
      if (side == White) {
         assert(kp == chess::E1);
         assert(board[kp+3] == WhiteRook);
      }
      else {
         assert(kp == chess::E8);
         assert(board[kp+3] == BlackRook);
      }
#endif
      if (board[kp + 1] == EmptyPiece &&
         board[kp + 2] == EmptyPiece &&
         board.checkStatus() == NotInCheck &&
         !board.anyAttacks(kp + 1,OppositeColor(side)) &&
         !board.anyAttacks(kp + 2,OppositeColor(side)))
         // can castle
         moves[numMoves++] = CreateMove(kp, kp+2, King, Empty,
            Empty, KCastle);
   }
   if ((CS == CanCastleEitherSide) ||
   (CS == CanCastleQSide)) {
      const Square kp = board.kingSquare(side);
      if (board[kp - 1] == EmptyPiece &&
         board[kp - 2] == EmptyPiece &&
         board[kp - 3] == EmptyPiece &&
         board.checkStatus() == NotInCheck  &&
         !board.anyAttacks(kp - 1,OppositeColor(side)) &&
         !board.anyAttacks(kp - 2,OppositeColor(side)))
         // can castle
         moves[numMoves++] = CreateMove(kp, kp-2, King, Empty,
            Empty, QCastle);
   }
   // non-pawn moves:
   Square start,dest;
   Bitboard dests;
   Bitboard knights(board.knight_bits[side]);
   while (knights.iterate(start)) {
      dests = Attacks::knight_attacks[start] & ~board.allOccupied;
      while (dests.iterate(dest)) {
         moves[numMoves++] =
             CreateMove(start,dest,Knight);
      }
   }
   start = board.kingSquare(side);
   dests = Attacks::king_attacks[start] & ~board.allOccupied &
               ~Attacks::king_attacks[board.kingSquare(board.oppositeSide())];
   while (dests.iterate(dest)) {
      moves[numMoves++] =
        CreateMove(start,dest,King);
   }
   Bitboard bishops(board.bishop_bits[side] | board.queen_bits[side]);
   while (bishops.iterate(start)) {
      Bitboard dests(board.bishopAttacks(start) & ~board.allOccupied);
      while (dests.iterate(dest)) {
         moves[numMoves++] =
            CreateMove(start,dest,TypeOfPiece(board[start]));
      }
   }
   Bitboard rooks(board.rook_bits[side] | board.queen_bits[side]);
   while (rooks.iterate(start)) {
      Bitboard dests(board.rookAttacks(start) & ~board.allOccupied);
      while (dests.iterate(dest)) {
          moves[numMoves++] =
            CreateMove(start,dest,TypeOfPiece(board[start]));
      }
   }
   // pawn moves
   if (board.sideToMove() == White) {
      Bitboard pawns(board.pawn_bits[White]);
      pawns.shl8();
        // exclude promotions
      pawns &= ~(board.allOccupied | Attacks::rank_mask[7]);
      Square sq;
      while (pawns.iterate(sq)) {
         moves[numMoves++] = CreateMove(sq-8,sq,Pawn);
         if (Rank<White>(sq)==3 && board[sq+8] == EmptyPiece)
            moves[numMoves++] = CreateMove(sq-8,sq+8,Pawn);
      }
   }
   else {
      Bitboard pawns(board.pawn_bits[Black]);
      pawns.shr8();
        // exclude promotions
      pawns &= ~(board.allOccupied | Attacks::rank_mask[0]);
      Square sq;
      while (pawns.iterate(sq)) {
         moves[numMoves++] = CreateMove(sq+8,sq,Pawn);
         if (Rank<Black>(sq)==3 && board[sq-8] == EmptyPiece)
            moves[numMoves++] = CreateMove(sq+8,sq-8,Pawn);
      }
   }
   return numMoves;
}


unsigned mg::generateCaptures(const Board &board, Move * moves, bool allPromotions, const Bitboard &targets)
{
   unsigned numMoves = 0;

   if (board.sideToMove() == White) {
      Bitboard pawns(board.pawn_bits[White]);
      Bitboard pawns1 = pawns;
      pawns1.shl(7);
      pawns1 &= ~0x8080808080808080ULL;
      pawns1 &= board.occupied[Black];
      Square dest;
      while (pawns1.iterate(dest)) {
         Square start = dest-7;
         if (Rank<White>(start) == 7) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Queen,Promotion);
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Knight,Promotion);
            if (allPromotions) {
               moves[numMoves++] =
                  CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Rook,Promotion);
               moves[numMoves++] =
                  CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Bishop,Promotion);
            }
         }
         else if (targets.isSet(dest)) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]));
         }
      }
      pawns1 = pawns;
      pawns1.shl(9);
      pawns1 &= ~0x0101010101010101ULL;
      pawns1 &= board.occupied[Black];
      while (pawns1.iterate(dest)) {
         Square start = dest-9;
         if (Rank<White>(start) == 7) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Queen,Promotion);
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Knight,Promotion);
            if (allPromotions) {
               moves[numMoves++] =
                  CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Rook,Promotion);
               moves[numMoves++] =
                  CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Bishop,Promotion);
            }
         }
         else if (targets.isSet(dest)) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]));
         }
      }
      pawns1 = pawns;
      pawns1 &= Attacks::rank_mask[6];
      pawns1.shl8();
      pawns1 &= ~board.allOccupied;
      while (pawns1.iterate(dest)) {
         Square start = dest - 8;
         moves[numMoves++] =
            CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Queen,Promotion);
         moves[numMoves++] =
            CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Knight,Promotion);
         if (allPromotions) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Rook,Promotion);
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Bishop,Promotion);
         }
      }
      Square epsq = board.enPassantSq();
      if (!IsInvalid(epsq) && targets.isSet(epsq)) {
         assert(TypeOfPiece(board[epsq])==Pawn);
         if (File(epsq) != 8 && board[epsq + 1] == WhitePawn) {
            dest = epsq + 8;
            if (board[dest] == EmptyPiece)
               moves[numMoves++] =
                  CreateMove(epsq+1,dest,Pawn,Pawn,Empty,
                  EnPassant);
         }
         if (File(epsq) != 1 && board[epsq - 1] == WhitePawn) {
            dest = epsq + 8;
            if (board[dest] == EmptyPiece)
               moves[numMoves++] =
                  CreateMove(epsq-1,dest,Pawn,Pawn,Empty,
                  EnPassant);
         }
      }
   }
   else {
      Bitboard pawns(board.pawn_bits[Black]);
      Bitboard pawns1 = pawns;
      pawns1.shr(7);
      pawns1 &= ~0x0101010101010101ULL;
      pawns1 &= board.occupied[White];
      Square dest;
      while (pawns1.iterate(dest)) {
         Square start = dest+7;
         if (Rank<Black>(start) == 7) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Queen,Promotion);
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Knight,Promotion);
            if (allPromotions) {
               moves[numMoves++] =
                  CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Rook,Promotion);
               moves[numMoves++] =
                  CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Bishop,Promotion);
            }
         }
         else if (targets.isSet(dest)) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]));
         }
      }
      pawns1 = pawns;
      pawns1.shr(9);
      pawns1 &= ~0x8080808080808080ULL;
      pawns1 &= board.occupied[White];
      while (pawns1.iterate(dest)) {
         Square start = dest+9;
         if (Rank<Black>(start) == 7) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Queen,Promotion);
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Knight,Promotion);
            if (allPromotions) {
               moves[numMoves++] =
                  CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Rook,Promotion);
               moves[numMoves++] =
                  CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Bishop,Promotion);
            }
         }
         else if (targets.isSet(dest)) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]));
         }
      }
      pawns1 = pawns;
      pawns1 &= Attacks::rank_mask[1];
      pawns1.shr8();
      pawns1 &= ~board.allOccupied;
      while (pawns1.iterate(dest)) {
         Square start = dest + 8;
         moves[numMoves++] =
            CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Queen,Promotion);
         moves[numMoves++] =
            CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Knight,Promotion);
         if (allPromotions) {
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Rook,Promotion);
            moves[numMoves++] =
               CreateMove(start,dest,Pawn,TypeOfPiece(board[dest]),Bishop,Promotion);
         }
      }
      Square epsq = board.enPassantSq();
      if (!IsInvalid(epsq) && targets.isSet(epsq)) {
         assert(TypeOfPiece(board[epsq])==Pawn);
         if (File(epsq) != 8 && board[epsq + 1] == BlackPawn) {
            dest = epsq - 8;
            if (board[dest] == EmptyPiece)
               moves[numMoves++] =
                  CreateMove(epsq+1,dest,Pawn,Pawn,Empty,
                  EnPassant);
         }
         if (File(epsq) != 1 && board[epsq - 1] == BlackPawn) {
            dest = epsq - 8;
            if (board[dest] == EmptyPiece)
               moves[numMoves++] =
                  CreateMove(epsq-1,dest,Pawn,Pawn,Empty,
                  EnPassant);
         }
      }
   }
   const ColorType side = board.sideToMove();
   Bitboard knights(board.knight_bits[side]);
   Square start, dest;
   while (knights.iterate(start)) {
      Bitboard dests(Attacks::knight_attacks[start] & targets);
      while (dests.iterate(dest)) {
         moves[numMoves++] =
            CreateMove(start,dest,Knight,TypeOfPiece(board[dest]));
     }
   }
   Bitboard bishops(board.bishop_bits[side] | board.queen_bits[side]);
   while (bishops.iterate(start)) {
      Bitboard dests(board.bishopAttacks(start) & targets);
      Square dest;
      while (dests.iterate(dest)) {
         moves[numMoves++] =
           CreateMove(start,dest,TypeOfPiece(board[start]),TypeOfPiece(board[dest]));
      }
   }
   Bitboard rooks(board.rook_bits[side] | board.queen_bits[side]);
   while (rooks.iterate(start)) {
      Bitboard dests(board.rookAttacks(start) & targets);
      Square dest;
      while (dests.iterate(dest)) {
          moves[numMoves++] =
             CreateMove(start,dest,
                        TypeOfPiece(board[start]),TypeOfPiece(board[dest]));
      }
   }
   start = board.kingSquare(side);
   Bitboard dests(Attacks::king_attacks[start] & targets & ~Attacks::king_attacks[board.kingSquare(board.oppositeSide())]);
   while (dests.iterate(dest)) {
      moves[numMoves++] =
         CreateMove(start,dest,King,TypeOfPiece(board[dest]));
   }
   return numMoves;
}


unsigned mg::generateChecks(const Board &board, Move * moves, const Bitboard &discoveredCheckCandidates) {
   // Note: doesn't at present generate castling moves that check
   assert(board.checkStatus() == NotInCheck);
   const Square kp = board.kingSquare(board.oppositeSide());
   Square loc;
   unsigned numMoves = 0;

   // Discovered checks first.
   Bitboard disc(discoveredCheckCandidates);
   while (disc.iterate(loc)) {
      switch(TypeOfPiece(board[loc])) {
         case Pawn: {
            const int dir = Attacks::directions[loc][kp];
            assert(dir);
            if (std::abs(dir) != 8) {
               // Pawn does not move in direction of pin
               const int step = (board.sideToMove() == White) ? 8 : -8;
               if (board[loc+step] == EmptyPiece && Rank(loc,board.sideToMove()) < 7) {
                  moves[numMoves++] = CreateMove(loc,loc+step,Pawn);
                  if (Rank(loc,board.sideToMove()) == 2 && board[loc+2*step] == EmptyPiece) {
                     moves[numMoves++] = CreateMove(loc,loc+2*step,Pawn);
                  }
               }
            }
            break;
         }
         case Knight:
         {
            Bitboard dests(Attacks::knight_attacks[loc] & ~board.allOccupied);
            Square sq;
            while (dests.iterate(sq)) {
               moves[numMoves++] =
                  CreateMove(loc,sq,Knight);
            }
            break;
         }
         case Bishop: {
            Bitboard dests(board.bishopAttacks(loc) & ~board.allOccupied);
            Square sq;
             while (dests.iterate(sq)) {
                 moves[numMoves++] =
                     CreateMove(loc,sq,Bishop);
             }
             break;
         }
         case Rook: {
            Bitboard dests(board.rookAttacks(loc) & ~board.allOccupied);
            Square sq;
             while (dests.iterate(sq)) {
                 moves[numMoves++] =
                      CreateMove(loc,sq,Rook);
             }
             break;
         }
         case Queen: {
             // Queen can't actually be pinned w/o currently giving
             // check
             break;
         }
      default:
          break;
      }
   }

   // Now non-discovered checks
   Bitboard pieces(board.occupied[board.sideToMove()]);
   pieces &= ~board.pawn_bits[board.sideToMove()];
   pieces &= ~disc;
   pieces &= ~board.pawn_bits[board.sideToMove()];
   while (pieces.iterate(loc)) {
      switch(TypeOfPiece(board[loc])) {
         case Knight:
         {
            Bitboard dests(Attacks::knight_attacks[loc]);
            dests &= ~board.allOccupied;
            dests &= Attacks::knight_attacks[kp];
            Square sq;
            while (dests.iterate(sq)) {
               moves[numMoves++] =
                  CreateMove(loc,sq,Knight);
            }
            break;
         }
         case King:
            break;
         case Bishop:
         {
            Bitboard dests(board.bishopAttacks(loc) & ~board.allOccupied);
            Bitboard dests2(board.bishopAttacks(kp) & ~board.allOccupied);
            dests &= dests2;
            Square dest;
            while (dests.iterate(dest)) {
                moves[numMoves++] =
                   CreateMove(loc,dest,Bishop);
            }
            break;
         }
         case Rook:
         {
            Bitboard dests(board.rookAttacks(loc) & ~board.allOccupied);
            Bitboard dests2(board.rookAttacks(kp) & ~board.allOccupied);
            dests &= dests2;
            Square dest;
            while (dests.iterate(dest)) {
                moves[numMoves++] =
                   CreateMove(loc,dest,Rook);
            }
            break;
         }
         case Queen:
         {
            Bitboard dests(board.rookAttacks(loc) | board.bishopAttacks(loc));
            dests &= ~board.allOccupied;
            Bitboard dests2(board.bishopAttacks(kp) & ~board.allOccupied);
            Bitboard dests3(board.rookAttacks(kp) & ~board.allOccupied);
            dests &= (dests2 | dests3);
            Square dest;
            while (dests.iterate(dest)) {
                  moves[numMoves++] =
                     CreateMove(loc,dest,Queen);
            }
            break;
         }
         default:
           break;
      }
   }
   // pawn moves
   if (board.sideToMove() == White) {
      Square sq;
      Bitboard pawns(board.pawn_bits[White]);
      pawns.shl8();
      pawns &= ~(board.allOccupied | Attacks::rank_mask[7]);
      Bitboard pawns1(pawns & Attacks::pawn_attacks[kp][White]);
      while (pawns1.iterate(sq)) {
         moves[numMoves++] = CreateMove(sq-8,sq,Pawn);
      }
      pawns &= Attacks::rank_mask[2];
      pawns.shl8();
      pawns &= ~board.allOccupied;
      pawns &= Attacks::pawn_attacks[kp][White];
      while (pawns.iterate(sq)) {
         moves[numMoves++] = CreateMove(sq-16,sq,Pawn);
      }
   }
   else {
      Square sq;
      Bitboard pawns(board.pawn_bits[Black]);
      pawns.shr8();
      pawns &= ~(board.allOccupied | Attacks::rank_mask[0]);
      Bitboard pawns1(pawns & Attacks::pawn_attacks[kp][Black]);
      while (pawns1.iterate(sq)) {
         moves[numMoves++] = CreateMove(sq+8,sq,Pawn);
      }
      pawns &= Attacks::rank_mask[2];
      pawns.shl8();
      pawns &= ~board.allOccupied;
      pawns &= Attacks::pawn_attacks[kp][Black];
      while (pawns.iterate(sq)) {
         moves[numMoves++] = CreateMove(sq+16,sq,Pawn);
      }
   }
   return numMoves;
}

void mg::sortMoves(Move moves[], int scores[], unsigned n) {
    if (n == 2) {
        if (scores[1] > scores[0]) {
           swap(moves,scores,0,1);
        }
    } else if (n>1) {
       // insertion sort
       for (unsigned i = 1; i < n; i++) {
          int key = scores[i];
          Move m = moves[i];
          int j = int(i)-1;
          for (; j >= 0 && scores[j] < key; j--) {
              moves[j+1] = moves[j];
              scores[j+1] = scores[j];
          }
          moves[j+1] = m;
          scores[j+1] = key;
       }
    }
}

void mg::initialSortCaptures (Move *moves,unsigned captures) {
   if (captures > 1) {
      int scores[Constants::MaxCaptures];
      assert(captures < Constants::MaxCaptures);
      for (unsigned i = 0; i < captures; i++) {
          scores[i] = int(Params::MVV_LVA(moves[i]));
      }
      sortMoves(moves,scores,captures);
   }
}

unsigned mg::generateEvasionsNonCaptures(const Board &board, const mg::EvasionInfo &info, Move * moves)
{
   unsigned num_moves = 0;
   const Square kp = board.kingSquare(board.sideToMove());
   if (info.num_attacks == 1) {
      // try to interpose a piece
      const Square &source = info.source;
      if (Sliding(board[source])) {
         Bitboard btwn_squares;
         board.between(source,kp,btwn_squares);
         if (!btwn_squares.isClear()) {
            // blocking pawn moves
            if (board.sideToMove() == White) {
               Bitboard pawns(board.pawn_bits[White]);
               pawns.shl8();
               pawns &= ~board.allOccupied;
               Bitboard pawns1(pawns);
               pawns &= btwn_squares;
               Square sq;
               while (pawns.iterate(sq)) {
                  if (!board.isPinned(board.sideToMove(), sq-8, sq)) {
                     if (Rank<White>(sq) == 8) {
                        // interposition is a promotion
                        moves[num_moves++] = CreateMove(
                           sq-8, sq, Pawn, Empty, Queen, Promotion);
                        moves[num_moves++] = CreateMove(
                           sq-8, sq, Pawn, Empty, Rook, Promotion);
                        moves[num_moves++] = CreateMove(
                           sq-8, sq, Pawn, Empty, Knight, Promotion);
                        moves[num_moves++] = CreateMove(
                           sq-8, sq, Pawn, Empty, Bishop, Promotion);
                     }
                     else {
                        moves[num_moves++] = CreateMove(sq-8, sq, Pawn, Empty);
                     }
                  }
               }
               pawns1 &= Attacks::rank_mask[2];
               if (!pawns1.isClear()) {
                  pawns1.shl8();
                  pawns1 &= ~board.allOccupied;
                  pawns1 &= btwn_squares;
                  while (pawns1.iterate(sq)) {
                     if (!board.isPinned(White, sq-16, sq))
                        moves[num_moves++] = CreateMove(sq-16,sq,Pawn,Empty);
                  }
               }
            }
            else {
               Bitboard pawns(board.pawn_bits[Black]);
               pawns.shr8();
               pawns &= ~board.allOccupied;
               Bitboard pawns1(pawns);
               pawns &= btwn_squares;
               Square sq;
               while (pawns.iterate(sq)) {
                  if (!board.isPinned(Black, sq+8, sq)) {
                     if (Rank<Black>(sq) == 8) {
                        // interposition is a promotion
                        moves[num_moves++] = CreateMove(
                           sq+8, sq, Pawn, Empty, Queen, Promotion);
                        moves[num_moves++] = CreateMove(
                           sq+8, sq, Pawn, Empty, Rook, Promotion);
                        moves[num_moves++] = CreateMove(
                           sq+8, sq, Pawn, Empty, Knight, Promotion);
                        moves[num_moves++] = CreateMove(
                           sq+8, sq, Pawn, Empty, Bishop, Promotion);
                     }
                     else {
                        moves[num_moves++] = CreateMove(sq+8, sq, Pawn, Empty);
                     }
                  }
               }
               pawns1 &= Attacks::rank_mask[5];
               if (!pawns1.isClear()) {
                  pawns1.shr8();
                  pawns1 &= ~board.allOccupied;
                  pawns1 &= btwn_squares;
                  while (pawns1.iterate(sq)) {
                     if (!board.isPinned(board.sideToMove(), sq+16, sq))
                        moves[num_moves++] = CreateMove(sq+16,sq,Pawn,Empty);
                  }
               }
            }
            // other blocking pieces
            Bitboard pieces(board.occupied[board.sideToMove()]);
            pieces &= ~board.pawn_bits[board.sideToMove()];
            Square loc;
            while (pieces.iterate(loc)) {
               switch (TypeOfPiece(board[loc])) {
                  case Empty:
                  case Pawn:
                     break;
                  case Knight:
                  {
                    Bitboard dests(Attacks::knight_attacks[loc] & btwn_squares);
                     Square sq;
                     while (dests.iterate(sq)) {
                        if (!board.isPinned(board.sideToMove(), loc, sq))
                           moves[num_moves++] =
                              CreateMove(loc,sq,Knight,TypeOfPiece(board[sq]));
                     }
                  }
                  break;
                  case Bishop:
                  {
                     Square dest;
                     Bitboard dests(board.bishopAttacks(loc) & btwn_squares);
                     while (dests.iterate(dest)) {
                        if (board.clear(loc,dest) &&
                           !board.isPinned(board.sideToMove(), loc, dest))
                           moves[num_moves++] =
                              CreateMove(loc,dest,Bishop,TypeOfPiece(board[dest]));
                     }
                  }
                  break;
                  case Rook:
                  {
                     Square dest;
                     Bitboard dests(board.rookAttacks(loc) & btwn_squares);
                     while (dests.iterate(dest)) {
                        if (board.clear(loc,dest) && !board.isPinned(board.sideToMove(), loc, dest)) {
                           moves[num_moves++] =
                              CreateMove(loc,dest,Rook,TypeOfPiece(board[dest]));
                        }
                     }
                  }
                  break;
                  case Queen:
                  {
                     Square dest;
                     Bitboard dests(board.rookAttacks(loc)|board.bishopAttacks(loc));
                     dests &= btwn_squares;
                     while (dests.iterate(dest)) {
                        if (board.clear(loc,dest) &&
                           !board.isPinned(board.sideToMove(), loc, dest))
                           moves[num_moves++] =
                              CreateMove(loc,dest,Queen,TypeOfPiece(board[dest]));
                     }
                  }
                  break;
                  case King:
                     break;
               }
            }
         }
      }
   }
   // generate evasive moves that do not capture
   num_moves += generateEvasions(board, info, moves+num_moves,~board.allOccupied);
   return num_moves;
}

unsigned mg::generateEvasionsCaptures(const Board &board, const mg::EvasionInfo &info, Move * moves)
{
   unsigned num_moves = 0;
   if (info.num_attacks == 1) {
      const Square &source = info.source;
      assert(source != InvalidSquare);
      Bitboard atcks(board.calcAttacks(source,board.sideToMove()));
      Square sq;
      const PieceType sourcePiece = TypeOfPiece(board[source]);
      while (atcks.iterate(sq)) {
         PieceType capturingPiece = TypeOfPiece(board[sq]);
         if (capturingPiece == King) {
            // We can capture with the king only if the piece
            // checking us is undefended.  But always allow a
            // capture *of* the king - for illegal move detection.
            if (TypeOfPiece(board[source]) == King ||
            !board.anyAttacks(source, board.oppositeSide())) {
               moves[num_moves++] = CreateMove(sq, source,
                  King, sourcePiece);
            }
         }
         else {
            if (!board.isPinned(board.sideToMove(), sq, source)) {
               if (capturingPiece == Pawn &&
                   Rank(source,board.sideToMove()) == 8) {
                  moves[num_moves++] = CreateMove(
                     sq, source, Pawn, sourcePiece, Queen, Promotion);
                  moves[num_moves++] = CreateMove(
                     sq, source, Pawn, sourcePiece, Rook, Promotion);
                  moves[num_moves++] = CreateMove(
                     sq, source, Pawn, sourcePiece, Knight, Promotion);
                  moves[num_moves++] = CreateMove(
                     sq, source, Pawn, sourcePiece, Bishop, Promotion);
               }
               else {
                  moves[num_moves++] = CreateMove(sq, source, capturingPiece, sourcePiece);
               }
            }
         }
      }
      // Attacks::calcAttacks does not return en passant captures, so try
      // this as a special case
      if (board.enPassantSq() == source) {
         Square dest = source + 8 * Direction[board.sideToMove()];
         Piece myPawn = MakePiece(Pawn,board.sideToMove());
         if (File(source) != 8 && board[source + 1] == myPawn) {
            Piece tmp = board[source];
            Piece &place = (Piece &)board[source];
            place = EmptyPiece;                   // imagine me gone
            if (!board.isPinned(board.sideToMove(), source + 1, dest))
               moves[num_moves++] = CreateMove(source + 1, dest, Pawn,
                  Pawn, Empty, EnPassant);
            place = tmp;
         }
         if (File(source) != 1 && board[source - 1] == myPawn) {
            Piece tmp = board[source];
            Piece &place = (Piece &)board[source];
            place = EmptyPiece;                   // imagine me gone
            if (!board.isPinned(board.sideToMove(), source - 1, dest))
               moves[num_moves++] = CreateMove(source - 1, dest, Pawn, Pawn,
                  Empty, EnPassant);
            place = tmp;
         }
      }
   }
   // try evasions that capture pieces beside the attacker
   num_moves += generateEvasions(board, info, moves+num_moves, board.occupied[board.oppositeSide()]);

   return num_moves;
}

unsigned mg::generateEvasions(const Board &board, const mg::EvasionInfo &info, Move * moves)
{
    unsigned n = generateEvasionsCaptures(board, info, moves);
    n += generateEvasionsNonCaptures(board, info, moves+n);
    return n;
}

unsigned mg::generateEvasions(const Board &board, const mg::EvasionInfo &info, Move * moves, const Bitboard &mask)
{
   unsigned num_moves = 0;
   Square kp = board.kingSquare(board.sideToMove());
   Bitboard b(Attacks::king_attacks[kp]);
   b &= ~board.allPawnAttacks(board.oppositeSide());
   b &= ~Attacks::king_attacks[board.kingSquare(board.oppositeSide())];
   b &= mask;
   Square sq;
   while (b.iterate(sq)) {
      if (info.num_attacks > 1 || sq != info.source) {
         // We need to do some extra checking, since the board
         // info on attacks reflects the state before the move,
         // and we need to be sure that the destination square
         // is not attacked after the move.
         int illegal = 0;
         Bitboard it(info.king_attacks);
         Square atck_sq;
         while (it.iterate(atck_sq)) {
            Piece attacker = board[atck_sq];
            if (Sliding(attacker)) {
               int dir = Attacks::directions[atck_sq][sq];
               // check for movement in the direction of the
               // attacker:
               if (dir != 0 && (kp + dir == sq)) {
                  illegal = 1;
                  break;
               }
            }
         }
         if (!illegal && !board.anyAttacks(sq, board.oppositeSide())) {
            moves[num_moves++] = CreateMove(kp, sq, King, TypeOfPiece(board[sq]));
         }
      }
   }
   return num_moves;
}


