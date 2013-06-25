// Copyright 1992-2012 by Jon Dart. All Rights Reserved.

#ifndef _SCORING_H
#define _SCORING_H

#include "board.h"
#include "hash.h"

#include <iostream>
using namespace std;

class Scoring
{
    // This class does static evaluation of chess positions.

    public:
		
    enum { INVALID_SCORE = -Constants::MATE-1 };

    static void init();

    static void cleanup();

    Scoring(Hash *ht);

    ~Scoring();
        
    // evaluate "board" from the perspective of the side to move.
    int evalu8( const Board &board );

    // evaluate "board" from the perspective of the side to move.
    int evalu8( const Board &board, int alpha, int beta );

    // return a positional score
    int positionalScore( const Board &board, 
			int alpha, int beta);

    // return a material score
    int materialScore( const Board &board );
		
    // checks for legal draws plus certain other theoretically
    // draw positions
    static int isDraw(const Board &board);
        
    // checks for draw by repetition (returning repetition count) + 
    // other draw situations as in isDraw above.
    static int isDraw( const Board &board, int &rep_count, int ply);

    static int repetitionDraw( const Board &board );

    static int materialDraw( const Board &board ) {
        return board.materialDraw();
    }

    // Check only for legal draws
    static int isLegalDraw(const Board &board);

    static int theoreticalDraw(const Board &board);

    // Turn a score into a formatted string (mate scores are
    // shown like +Mate6).
    static void printScore( int score, ostream & );

    // Output scores in the format required by the UCI protocol.
    static void printScoreUCI( int score, ostream & );

    static bool mateScore(int score) {
      return score>=Constants::MATE_RANGE || score<=-Constants::MATE_RANGE;
    }

#ifdef EVAL_STATS
    static void clearStats();

    static void showStats(ostream &out);
#endif

    // Try to return a score based on bitbases, INVALID_SCORE if not found
    static int tryBitbase(const Board &board);

    void clearHashTables();

 private:

    static const int PAWN_HASH_SIZE = 16384;
    static const int KING_COVER_HASH_SIZE = 8192;
    static const int ENDGAME_HASH_SIZE = 32768;
    static const int EVAL_CACHE_SIZE = 131072;

    struct CACHE_ALIGN PawnHashEntry {

       hash_t hc;

       struct PawnData {
           Bitboard passers;
           Bitboard opponent_pawn_attacks;
           Bitboard weak_pawns;
	   byte weakopen;
           byte pawn_file_mask;
           byte passer_file_mask;
           byte pad;
           int32 endgame_score, midgame_score;
  	   int w_square_pawns,b_square_pawns;
           int outside;
       } wPawnData, bPawnData;

       const PawnData &pawnData(ColorType side) const {
	 return (side==White) ? wPawnData : bPawnData;
       }
    } pawnHashTable[PAWN_HASH_SIZE];

    struct KingCoverHashEntry {
       uint32 hc;
       int32 cover;
    } kingCoverHashTable[2][KING_COVER_HASH_SIZE];

    struct CACHE_ALIGN EndgameHashEntry {
        // defines one entry in the king/pawn hash table. 128 bytes.
        hash_t hc;                                    // hashcode
        signed char white_king_position, black_king_position;
        signed char white_endgame_pawn_proximity;
        signed char black_endgame_pawn_proximity;
        byte w_uncatchable, b_uncatchable;
        int wScore,bScore;
    } endgameHashTable[ENDGAME_HASH_SIZE];

    // pointer to hash table (contains eval cache)
    Hash *hashTable;

    // The scores for opening, middlegame and endgame
    struct Scores {
      Scores()
        :mid(0), end(0),any(0)
      {
      }
      int mid, end, any; 
      int blend(int materialLevel ) {
          return any + mid*MATERIAL_SCALE[materialLevel]/128 +
             end*(128-MATERIAL_SCALE[materialLevel])/128;
      }
      static const int MATERIAL_SCALE[32];
    };

    int adjustMaterialScore( const Board &board, ColorType side);
    int adjustMaterialScoreNoPawns( const Board &board, ColorType side);

    template <ColorType side>
    void pieceScore(const Board &board,
                   const PawnHashEntry::PawnData &ourPawnData,
		   const PawnHashEntry::PawnData &oppPawnData,
                    int cover, Scores &, Scores &opp_scores, bool endgame);

    // compute king cover for King on square 'kp' of color 'side'
    template <ColorType side>
    static int calcCover(const Board &board, Square kp);

    template <ColorType side>
    static int calcCover(const Board &board, int file, int rank);

    // return a pawn structure entry
    template <ColorType side>
    void calcCover(const Board &board, KingCoverHashEntry &cover);

    PawnHashEntry *pawnEntry(const Board &board);

    int calcPawnData(const Board &board, ColorType side,
			   PawnHashEntry::PawnData &entr);

    void evalOutsidePassers(const Board &board,
			   PawnHashEntry *pawnEntry);

    void calcPawnEntry(const Board &board, PawnHashEntry *pawnEntry);

    void pawnScore(const Board &board, ColorType side,
		  const PawnHashEntry::PawnData &oppPawnData, Scores &);

    void calcEndgame(const Board &board, PawnHashEntry *pawnEntry,
		     EndgameHashEntry *endgameEntry);

    void scoreEndgame(const Board &, EndgameHashEntry *endgameEntry,
		     const PawnHashEntry::PawnData &pawnData, ColorType side,
		     Scores &);

    int outpost(const Board &board, ColorType side, Square sq, Square scoreSq, 
                const int scores[64],
                const PawnHashEntry::PawnData &oppPawnData);


};

#endif



