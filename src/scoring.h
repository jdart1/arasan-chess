// Copyright 1992-2015 by Jon Dart. All Rights Reserved.

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
		
  static const int PARAM_ARRAY_SIZE = 61;

  enum {Midgame = 0, Endgame = 1};

  // indexes into the parameter array, below
    enum {
      CASTLING0,
      CASTLING1,
      CASTLING2,
      CASTLING3,
      CASTLING4,
      CASTLING5, 
      KING_COVER0,
      KING_COVER1,
      KING_COVER2,
      KING_COVER3,
      KING_COVER4,
      KING_FILE_OPEN,
      KING_DISTANCE_BASIS,
      KING_DISTANCE_MULT,
      PIN_MULTIPLIER_MID,
      PIN_MULTIPLIER_END,
      KING_ATTACK_PARAM1,
      KING_ATTACK_PARAM2,
      KING_ATTACK_PARAM3,
      KING_ATTACK_BOOST_THRESHOLD,
      KING_ATTACK_BOOST_DIVISOR,
      BISHOP_TRAPPED,
      BISHOP_PAIR_MID,
      BISHOP_PAIR_END,
      BISHOP_PAWN_PLACEMENT_END,
      BAD_BISHOP_MID,
      BAD_BISHOP_END,
      OUTPOST_NOT_DEFENDED,
      CENTER_PAWN_BLOCK,
      OUTSIDE_PASSER_MID,
      OUTSIDE_PASSER_END,
      WEAK_PAWN_MID,
      WEAK_PAWN_END,
      WEAK_ON_OPEN_FILE_MID,
      WEAK_ON_OPEN_FILE_END,
      SPACE,
      PAWN_CENTER_SCORE_MID,
      ROOK_ON_7TH_MID,
      ROOK_ON_7TH_END,
      TWO_ROOKS_ON_7TH_MID,
      TWO_ROOKS_ON_7TH_END,
      ROOK_ON_OPEN_FILE_MID,
      ROOK_ON_OPEN_FILE_END,
      ROOK_ATTACKS_WEAK_PAWN_MID,
      ROOK_ATTACKS_WEAK_PAWN_END,
      ROOK_BEHIND_PP_MID,
      ROOK_BEHIND_PP_END,
      QUEEN_OUT,
      PASSER_OWN_PIECE_BLOCK_MID,
      PASSER_OWN_PIECE_BLOCK_END,
      PP_BLOCK_BASE_MID,
      PP_BLOCK_BASE_END,
      PP_BLOCK_MULT_MID,
      PP_BLOCK_MULT_END,
      ENDGAME_PAWN_BONUS,
      KING_NEAR_PASSER,
      OPP_KING_NEAR_PASSER,
      PAWN_SIDE_BONUS,
      SUPPORTED_PASSER6,
      SUPPORTED_PASSER7,
      SIDE_PROTECTED_PAWN
    };

#ifdef TUNE
#include "varparams.h"
#else
#include "params.h"
#endif
    enum { INVALID_SCORE = -Constants::MATE-1 };

    static void init();

    static void cleanup();

    Scoring();

    ~Scoring();
        
    // evaluate "board" from the perspective of the side to move.
    int evalu8( const Board &board );

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

    // return a material score
    int materialScore( const Board &board ) const;

 private:

    static const int PAWN_HASH_SIZE = 16384;
    static const int KING_COVER_HASH_SIZE = 8192;
    static const int ENDGAME_HASH_SIZE = 32768;

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
        int16 white_king_position, black_king_position;
        int16 white_endgame_pawn_proximity;
        int16 black_endgame_pawn_proximity;
        uint16 w_uncatchable, b_uncatchable;
        int wScore,bScore;
    } endgameHashTable[ENDGAME_HASH_SIZE];

    // The scores for opening, middlegame and endgame
    struct Scores {
      Scores()
      :mid(0), end(0), any(0)
      {
      }
      Scores(const Scores &s)
      :mid(s.mid),end(s.end),any(s.any) {
      }
      int mid, end, any;
      int blend(int materialLevel ) {
        return any + mid*params.MATERIAL_SCALE[materialLevel]/128 +
          end*(128-params.MATERIAL_SCALE[materialLevel])/128;
      }

      Scores & operator += (const Scores &s) {
          mid += s.mid;
          end += s.end;
          any += s.any;
          return *this;
      }
      const Scores operator + (const Scores &s) const {
          Scores result = *this;
          result += s;
          return result;
      }
      int operator == (const Scores &s) const {
        return s.mid == mid && s.end == end && s.any == any;
      }
      int operator != (const Scores &s) const {
        return s.mid != mid || s.end != end || s.any != any;
      }
    };

    template <ColorType side>
     void  positionalScore( const Board &board,
                            const PawnHashEntry &pawnEntry,
                            Scores &scores,
                            Scores &oppScores);

    int adjustMaterialScore(const Board &board, ColorType side) const;

    int adjustMaterialScoreNoPawns(const Board &board, ColorType side) const;

    template <ColorType side>
    void pieceScore(const Board &board,
                   const PawnHashEntry::PawnData &ourPawnData,
		   const PawnHashEntry::PawnData &oppPawnData,
                    int cover, Scores &, Scores &opp_scores,
                    bool early_endgame,
                    bool deep_endgame);

    // compute king cover for King on square 'kp' of color 'side'
    template <ColorType side>
    static int calcCover(const Board &board, Square kp);

    template <ColorType side>
    static int calcCover(const Board &board, int file, int rank);

    template <ColorType side>
    void calcCover(const Board &board, KingCoverHashEntry &cover);

    template <ColorType side>
        int kingCover(const Board &board);

    PawnHashEntry &pawnEntry(const Board &board);

    int calcPawnData(const Board &board, ColorType side,
			   PawnHashEntry::PawnData &entr);

    void evalOutsidePassers(const Board &board,
			    PawnHashEntry &pawnEntry);

    void calcPawnEntry(const Board &board, PawnHashEntry &pawnEntry);

    void pawnScore(const Board &board, ColorType side,
		  const PawnHashEntry::PawnData &oppPawnData, Scores &);

    void calcEndgame(const Board &board, const PawnHashEntry &pawnEntry,
		     EndgameHashEntry *endgameEntry);

    void scoreEndgame(const Board &, EndgameHashEntry *endgameEntry,
		     const PawnHashEntry::PawnData &pawnData, ColorType side,
		     Scores &);

    int kingDistanceScore(const Board &) const;

    template <ColorType side>
    int outpost(const Board &board, Square sq, Square scoreSq, 
                const int scores[64],
                const PawnHashEntry::PawnData &oppPawnData);


};

#endif



