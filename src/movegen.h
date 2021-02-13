// Copyright 1992-2008, 2011, 2012, 2015-2020 by Jon Dart. All Rights Reserved.
//
#ifndef _MOVE_GENERATOR_H
#define _MOVE_GENERATOR_H

#include "board.h"
#include "constant.h"
#include "params.h"
#include "see.h"
#include <algorithm>
#include <set>
#include <vector>
using namespace std;

class SearchContext;
struct NodeInfo;

namespace mg {
    // Only generates captures, promotions, and check evasions.
    // Only include captures on squares in the set "targets".
    // If allPromotions = true, generate all promotiions, otherwise
    // just Q + N.
    extern unsigned generateCaptures(const Board &board, Move *moves, bool allPromotions,
                                     const Bitboard &targets);

    // Only generates captures, promotions, and check evasions.
    //
    inline unsigned generateCaptures(const Board &board, Move *moveList, bool allPromotions) {
        return generateCaptures(board, moveList, allPromotions, board.occupied[board.oppositeSide()]);
    }

    // Generate only non-capturing moves.
    unsigned generateNonCaptures(const Board &board, Move *moves);

    // Generate non-capturing checking moves
    extern unsigned generateChecks(const Board &board, Move * moves, const Bitboard &discoveredCheckCandidates);

    struct EvasionInfo
    {
        Bitboard king_attacks;
        int num_attacks;
        Square source;

        void init(const Board &board) {
            king_attacks = board.calcAttacks(board.kingSquare(board.sideToMove()), board.oppositeSide());
#ifdef _DEBUG
            if (king_attacks.isClear()) {
                ASSERT(0);
            }
#endif
            num_attacks = king_attacks.bitCountOpt();
            if (num_attacks == 1) {
                source = king_attacks.firstOne();
            }
        }
        
        EvasionInfo() : num_attacks(0), source(InvalidSquare) {
        }

        EvasionInfo(const Board &board) {
            init(board);
        }
        
    };

    extern unsigned generateEvasionsCaptures(const Board &board, const EvasionInfo &info, Move * moves);
    extern unsigned generateEvasionsNonCaptures(const Board &board, const EvasionInfo &info, Move * moves);
    extern unsigned generateEvasions(const Board &board, const EvasionInfo &info, Move * moves);
    extern unsigned generateEvasions(const Board &board, const EvasionInfo &info, Move * moves, const Bitboard &mask);

    extern void initialSortCaptures(Move *moves, unsigned captures);

    extern void sortMoves(Move moves[], int scores[], unsigned n);
}

class MoveGenerator
{
   public:

      enum Phase
      {
         START_PHASE, HASH_MOVE_PHASE, WINNING_CAPTURE_PHASE,
         KILLER1_PHASE, KILLER2_PHASE, COUNTER_MOVE_PHASE, HISTORY_PHASE,
         LOSERS_PHASE, LAST_PHASE
      };

      MoveGenerator( const Board &,
         SearchContext *sc = nullptr,
         NodeInfo *node = nullptr,
         unsigned ply = 0,
         Move pvMove = NullMove,
         int trace = 0);

      // Generate the next move, in sorted order, NullMove if none left
      // "ord" is updated with the index of the move.
      virtual Move nextMove(int &ord) {
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
                 if (seeSign(board,move,0)) {
                     SetPhase(move,WINNING_CAPTURE_PHASE);
                     ord = order++;
                     ASSERT(ord<Constants::MaxMoves);
                     return move;
                 } else {
                     SetPhase(move,LOSERS_PHASE);
                     ASSERT(losers_count < Constants::MaxCaptures);
                     losers[losers_count++] = move;
                 }
             }
             // no winning captures, do next phase
             return nextMove(ord);
         }
         ord = order++;
         ASSERT(ord<Constants::MaxMoves);
         return batch[index++];
      }

      virtual ~MoveGenerator() {
      }

      // Generate the next check evasion, NullMove if none left
      virtual Move nextEvasion(int &order);

      // fill array "moves" with moves, starting at "index", returns
      // # of moves (0 if no more). This tries to generate moves in stages,
      // so it should be called repeatedly until the return value is zero.
      //
      // The moves returned are generally "pseudo-legal", i.e. they may
      // involve moves into check.  However, if the side to move is in
      // check, then all moves returned are strictly legal.
      //
      unsigned generateAllMoves(Move *moves,int repeatable);

      // Return a list of the moves that may be used to escape
      // check.  Unlike the regular move generation routine, all
      // moves are checked for legality.
      //
      unsigned generateEvasions(Move * moves);

      unsigned movesGenerated() const
      {
         return moves_generated;
      }

      Phase getPhase() const
      {
         return (Phase)phase;
      }

      virtual int more() const
      {
         return phase<LAST_PHASE || index < batch_count;
      }

      inline void SetPhase(Move &move, Phase p)
      {
          ((union MoveUnion*)&(move))->contents.phase = (uint8_t)p;
      }

      inline Phase GetPhase(const Move &move) const
      {
          return (Phase)((union MoveUnion*)&(move))->contents.phase;
      }

      static const int EASY_PLIES;

   protected:

      int getBatch(Move *&batch,int &index);

      const Board &board;
      SearchContext *context;
      NodeInfo *node;
      int ply;
      int moves_generated;
      int losers_count,index,order,batch_count,forced;
      Phase phase;
      Move hashMove;
      mg::EvasionInfo info;
      Move *batch;
      Move losers[Constants::MaxCaptures];
      Move moves[Constants::MaxMoves];
      Move killer1,killer2;
      int master;

};

class RootMoveGenerator : public MoveGenerator
{
   friend class Search;
   friend class SearchController;

   public:
      RootMoveGenerator(const Board &board,
         SearchContext *context = nullptr,
         Move pvMove = NullMove,
         int trace = 0);

      RootMoveGenerator(const RootMoveGenerator &mg,
                        SearchContext *s);

      // Generate the next move, in sorted order, NullMove if none left
      virtual Move nextMove(int &ord) {
         ASSERT(index<=batch_count);
         if (index < batch_count) {
            ord = order++;
            ASSERT(ord<Constants::MaxMoves);
            return moveList[index++].move;
         }
         else {
            return NullMove;
         }
      }

      // Generate the next check evasion, NullMove if none left
      virtual Move nextEvasion(int &ord) {
         return nextMove(ord);
      }

      using MoveGenerator::nextMove;
      using MoveGenerator::nextEvasion;

      virtual int more() const
      {
         return index < batch_count;
      }

      void reorder(Move pvMove, score_t pvScore, int depth, bool initial = false);

      void reorderByScore();

      void reset() {
        index = order = 0;  // reset so we will fetch moves again
        phase = START_PHASE;
      }

      void suboptimal(int strength, Move &m, int &val);

      void exclude(const MoveSet &excluded);

      // restrict moves we will search
      void filter(const MoveSet &include);

      void exclude(Move);

      int moveCount() const {
         return batch_count-excluded;
      }

      Move first() const {
         return moveList[0].move;
      }

      // enumerate the nodes for a "depth" ply search (for testing).
      static uint64_t perft(Board &, int depth);

      score_t getScore(Move m) const noexcept {
         for (auto &it : moveList) {
            if (MovesEqual(it.move,m)) {
               return it.score;
            }
         }
         return Constants::INVALID_SCORE;
      }

      // Rank the root moves using tablebases. Returns 1 if
      // successful, 0 if not
      int rank_root_moves();

   protected:

      const vector <RootMove> &getMoveList() const {
          return moveList;
      }

      void setScore(int move_index, score_t score) noexcept {
          moveList[move_index].score = score;
      }

      void resetScores() noexcept {
          for (auto &m : moveList) m.score = -Constants::MATE;
      }

   private:
      vector<RootMove> moveList;
      int excluded;

};

class QSearchMoveGenerator 
{
public:
    // Move generation is restricted to captures onto "tgts".
    QSearchMoveGenerator(const Board &b, Move hash, const Bitboard &tgts) :
        board(b), index(0), moveCount(0), hashMove(hash), targets(tgts), phase(0)
    {
    }
    
    QSearchMoveGenerator(const Board &b, Move hash) :
        QSearchMoveGenerator(b, hash, b.occupied[b.oppositeSide()]) 
    {
    }

    virtual ~QSearchMoveGenerator() {
    }
    
    Move nextMove() {
        if (phase == 0) {
            ++phase;
            if (!IsNull(hashMove) &&
                (IsPromotion(hashMove) || targets.isSet(DestSquare(hashMove)))) {
                return hashMove;
            }
        }
        if (phase == 1) {
            ++phase;
            moveCount = mg::generateCaptures(board, moves, false, targets);
            ASSERT(moveCount <= Constants::MaxCaptures);
            mg::initialSortCaptures(moves, moveCount);
        }
        while (index < moveCount) {
            const Move &move = moves[index++];
            if (!MovesEqual(move, hashMove)) {
                return move;
            }
        }
        return NullMove;
    }

private:
    const Board &board;
    Move moves[Constants::MaxCaptures];
    unsigned index, moveCount;
    Move hashMove;
    Bitboard targets;
    int phase;
};

class QSearchCheckGenerator 
{
public:
    QSearchCheckGenerator(const Board &board, const Bitboard &pins):
        index(0) {
        moveCount = mg::generateChecks(board, moves, pins);
        ASSERT(moveCount <= Constants::MaxChecks);
    }
    
    virtual ~QSearchCheckGenerator() {
    }
    
    Move nextCheck() {
        if (index < moveCount)
            return moves[index++];
        else
            return NullMove;
    }

private:
    Move moves[Constants::MaxChecks];
    unsigned index, moveCount;
};

class ProbCutMoveGenerator 
{
public:
    // Move generation is restricted to captures onto "tgts".
    ProbCutMoveGenerator(const Board &b, Move hash, const Bitboard &tgts) :
        board(b), index(0), moveCount(0), hashMove(hash), targets(tgts), phase(0)
    {
    }
    
    virtual ~ProbCutMoveGenerator() {
    }
    
    Move nextMove() {
        if (phase == 0) {
            ++phase;
            if (!IsNull(hashMove) &&
                (IsPromotion(hashMove) || targets.isSet(DestSquare(hashMove)))) {
                return hashMove;
            }
        }
        if (phase == 1) {
            ++phase;
            if (board.checkStatus() == InCheck) {
                mg::EvasionInfo info(board);
                moveCount = mg::generateEvasionsCaptures(board, info, moves);
            } else {
                moveCount = mg::generateCaptures(board, moves, false, targets);
            }
            ASSERT(moveCount <= Constants::MaxCaptures);
            mg::initialSortCaptures(moves, moveCount);
        }
        while (index < moveCount) {
            const Move &move = moves[index++];
            if (!MovesEqual(move, hashMove)) {
                return move;
            }
        }
        return NullMove;
    }

private:
    const Board &board;
    Move moves[Constants::MaxCaptures];
    unsigned index, moveCount;
    Move hashMove;
    Bitboard targets;
    int phase;
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
   ((union MoveUnion*)&(move))->contents.phase = (uint8_t)phase;
}


inline MoveGenerator::Phase GetPhase(const Move &move)
{
   return (MoveGenerator::Phase)((union MoveUnion*)&(move))->contents.phase;
}
#endif
