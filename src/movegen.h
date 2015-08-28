// Copyright 1992-2008, 2011, 2012, 2015 by Jon Dart. All Rights Reserved.
//
#ifndef _MOVE_GENERATOR_H
#define _MOVE_GENERATOR_H

#include "board.h"
#include "constant.h"
#include "see.h"
#include "refut.h"
#include <vector>
using namespace std;

class SearchContext;
struct NodeInfo;
struct SplitPoint;

class MoveGenerator
{
   public:

      enum Phase
      {
         START_PHASE, HASH_MOVE_PHASE, WINNING_CAPTURE_PHASE,
         KILLER1_PHASE, KILLER2_PHASE, REFUTATION_PHASE, HISTORY_PHASE,
         LOSERS_PHASE, LAST_PHASE
      };

      MoveGenerator( const Board &,
         SearchContext *sc = NULL,
         unsigned ply = 0,
         Move pvMove = NullMove,
         Move prevMove = NullMove,                     
         int trace = 0);

      // Generate the next move, in sorted order, NullMove if none left
      virtual Move nextMove() {
         if (index >= batch_count) {
            if ((batch_count = getBatch(batch,index)) == 0)
               return NullMove;
         }
         if (phase == WINNING_CAPTURE_PHASE) {
             // We previously only checked MVV_LVA, now also check see() if
             // necessary to see if the move is really winning
             while (index < batch_count) {
                 Move &move = batch[index++];
                 if (MovesEqual(move,hashMove)) {
                     // already did this one
                     continue;
                 }
                 if (MVV_LVA(move)<=0) {
                    if (seeSign(board,move,0)) {
                         SetPhase(move,WINNING_CAPTURE_PHASE);
                         return move;
                     } else {
                         SetPhase(move,LOSERS_PHASE);
                         losers[losers_count++] = move;
                     }
                 } else {
                     SetPhase(move,WINNING_CAPTURE_PHASE);
                     return move;
                 }
             }
             // no winning captures, do next phase
             return nextMove();
         }
         return batch[index++];
      }

      virtual ~MoveGenerator() {
      }

      // Generate the next check evasion, NullMove if none left
      virtual Move nextEvasion();

      virtual Move nextMove(SplitPoint *);
      virtual Move nextEvasion(SplitPoint *);

      virtual int generateAllMoves(NodeInfo *, SplitPoint *);

      // Generate only non-capturing moves.
      int generateNonCaptures(Move *moves);

      // fill array "moves" with moves, starting at "index", returns
      // # of moves (0 if no more). This tries to generate moves in stages,
      // so it should be called repeatedly until the return value is zero.
      //
      // The moves returned are generally "pseudo-legal", i.e. they may
      // involve moves into check.  However, if the side to move is in
      // check, then all moves returned are strictly legal.
      //
      int generateAllMoves(Move *moves,int repeatable);

      // Only generates captures, promotions, and check evasions.
      // Only include captures on squares in the set "targets".
      //
      int generateCaptures( Move *moves, const Bitboard &targets );

      // Only generates captures, promotions, and check evasions.
      //
      int generateCaptures( Move *moves ) {
          return generateCaptures(moves, board.occupied[board.oppositeSide()]);
      }

      // Return a list of the moves that may be used to escape
      // check.  Unlike the regular move generation routine, all
      // moves are checked for legality.
      //
      int generateEvasions(Move * moves);

      // Generate non-capturing checking moves
      int generateChecks(Move * moves, const Bitboard &discoveredCheckCandidates);

      unsigned movesGenerated() const
      {
         return moves_generated;
      }

      Phase getPhase() const
      {
         return (Phase)phase;
      }

      int more() const
      {
         return phase<LAST_PHASE || index < batch_count;
      }

      static void sortMoves(Move moves[], int scores[], int n);

      inline void SetPhase(Move &move,Phase phase)
      {
          ((union MoveUnion*)&(move))->contents.phase = (byte)phase;
      }


      inline Phase GetPhase(const Move &move)
      {
          return (Phase)((union MoveUnion*)&(move))->contents.phase;
      }

      int initialSortCaptures(Move *moves, int captures);

      static const int EASY_PLIES;

   protected:

      int getBatch(Move *&batch,int &index);

      int generateEvasionsCaptures(Move * moves);
      int generateEvasionsNonCaptures(Move * moves);
      int generateEvasions(Move * moves,
         const Bitboard &mask);

      const Board &board;
      SearchContext *context;
      int ply;
      int moves_generated;
      int losers_count,index,batch_count,forced;
      Phase phase;
      Move hashMove;
      Move prevMove;
      Bitboard king_attacks;                      // for evasions
      int num_attacks;                            // for evasions
      Square source;                              // for evasions;
      Move *batch;
      Move losers[100];
      Move moves[Constants::MaxMoves];
      Move killer1,killer2,killer3,killer4;
      int master;

      inline void setMove( Square source, Square dest,
         PieceType promotion,
         Move *moves, unsigned &NumMoves);

};

struct MoveEntry
{
   Move move;
   int score;
};

class RootMoveGenerator : public MoveGenerator
{
   friend class RootSearch;
   friend class Search;

   public:
      RootMoveGenerator(const Board &board,
         SearchContext *context = NULL,
         Move pvMove = NullMove,
         int trace = 0);

      // Generate the next move, in sorted order, NullMove if none left
      virtual Move nextMove() {
         ASSERT(index<=batch_count);
         if (index < batch_count)
            return moveList[index++].move;
         else
            return NullMove;
      }

      // Generate the next check evasion, NullMove if none left
      virtual Move nextEvasion() {
         return nextMove();
      }

      using MoveGenerator::nextMove;
      using MoveGenerator::nextEvasion;
      virtual int generateAllMoves(NodeInfo *, SplitPoint *);

      void reorder(Move pvMove, int depth, bool initial = false);

      void reorderByScore();

      void suboptimal(int strength, Move &m, int &val);

      void exclude(Move *exclude, int num_exclude);

      void exclude(Move);

      int moveCount() const {
         return batch_count;
      }

      Move first() const {
         return moveList[0].move;
      }

      // enumerate the nodes for a "depth" ply search (for testing).
      static uint64 perft(Board &, int depth);

   protected:

      vector <MoveEntry> &getMoveList() {
          return moveList;
      }

      void setScore(Move m, int score) {         
          for (vector<MoveEntry>::iterator it = moveList.begin();it != moveList.end();it++) {
              if (MovesEqual((*it).move,m)) {
                  (*it).score = score;
                  break;
              }
          }
      }

   private:
      vector<MoveEntry> moveList;
};

inline MoveGenerator::Phase operator++(MoveGenerator::Phase &phase)
{
   return phase = (MoveGenerator::Phase)((int)phase + 1);
}


inline MoveGenerator::Phase operator++(MoveGenerator::Phase &phase, int)
{
   return phase = (MoveGenerator::Phase)((int)phase + 1);
}


inline void SetPhase(Move &move,MoveGenerator::Phase phase)
{
   ((union MoveUnion*)&(move))->contents.phase = (byte)phase;
}


inline MoveGenerator::Phase GetPhase(const Move &move)
{
   return (MoveGenerator::Phase)((union MoveUnion*)&(move))->contents.phase;
}
#endif
