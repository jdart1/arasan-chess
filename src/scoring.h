// Copyright 1992-2021, 2023-2024 by Jon Dart. All Rights Reserved.

#ifndef _SCORING_H
#define _SCORING_H

#include "board.h"
#include "hash.h"
#include "attacks.h"
#include "params.h"

struct NodeInfo;

#include <iostream>

class Scoring
{
    // This class does static evaluation of chess positions.

    public:

    enum {Midgame = 0, Endgame = 1};

    static void init();

    static void cleanup();

    Scoring();

    ~Scoring();

    // evaluate "board" from the perspective of the side to move.
    score_t evalu8( const Board &board, bool useCache = true );

    // evaluate using the nnue. If set the node pointer is used to
    // enable incremental evaluation
    score_t evalu8NNUE(const Board &board, NodeInfo *node = nullptr);

    // checks for draw by repetition (returning repetition count) +
    // other draw situations. It is intended to be called from the
    // interior of the search. It does not strictly detect legal
    // draws: use isLegalDraw for that.
    static bool isDraw( const Board &board, int &rep_count, int ply);

    // Check for legal draws and certain other drawish situations
    static bool theoreticalDraw(const Board &board);

    // Turn a score into a formatted string (mate scores are
    // shown like +Mate6).
    static void printScore( score_t score, std::ostream & );

    // Output scores in the format required by the UCI protocol.
    static void printScoreUCI( score_t score, std::ostream & );

    static bool mateScore(score_t score) {
      return score != Constants::INVALID_SCORE &&
        (score>=Constants::TABLEBASE_WIN || score<=-Constants::TABLEBASE_WIN);
    }

    // Try to return a score based on bitbases, INVALID_SCORE if not found
    static score_t tryBitbase(const Board &board);

    void clearHashTables();

    score_t outpost(const Board &board, Square sq, ColorType side) const;

    int outpost_defenders(const Board &board,
                          Square sq, ColorType side) const {
      return (Attacks::pawn_attacks[sq][side] & board.pawn_bits[side]).bitCountOpt();
    }

    static const int PAWN_HASH_SIZE = 16384;
    static const int KING_PAWN_HASH_SIZE = 8132;

    static CACHE_ALIGN Bitboard kingProximity[2][64];
    static CACHE_ALIGN Bitboard kingNearProximity[64];
    static CACHE_ALIGN Bitboard kingPawnProximity[2][4][64];

    struct PawnHashEntry {

       hash_t hc;

       struct PawnData {
           Bitboard passers;
           Bitboard opponent_pawn_attacks;
           Bitboard weak_pawns;
	   uint8_t weakopen;
           uint8_t pawn_file_mask;
           uint8_t passer_file_mask;
           uint8_t pad;
           int32_t endgame_score, midgame_score;
  	   int w_square_pawns,b_square_pawns;
           int outside;
       } wPawnData, bPawnData;

       const PawnData &pawnData(ColorType side) const {
	 return (side==White) ? wPawnData : bPawnData;
       }
    } pawnHashTable[PAWN_HASH_SIZE];

    struct KingPawnHashEntry {
       hash_t hc;
       int32_t cover, storm, pawn_attacks;
       int32_t king_endgame_position;
    };

    KingPawnHashEntry kingPawnHashTable[2][KING_PAWN_HASH_SIZE];

    PawnHashEntry &pawnEntry(const Board &board, bool useCache);

    template <ColorType side>
      KingPawnHashEntry &getKPEntry(const Board &board,
                        const PawnHashEntry::PawnData &ourPawnData,
                        const PawnHashEntry::PawnData &oppPawnData,
                        bool useCache);

    static int distance(Square sq1, Square sq);

    // Public, for use by the tuner.
    void calcCover(const Board &board, ColorType side, KingPawnHashEntry &cover);

    void calcStorm(const Board &board, ColorType side, KingPawnHashEntry &cover, const Bitboard &oppPawnAttacks);

    // The scores for opening, middlegame and endgame
    struct Scores {
      Scores()
      :mid(0), end(0), any(0)
      {
      }
      score_t blend(int materialLevel) {
        return any + mid*Params::MATERIAL_SCALE[materialLevel]/128 +
          end*(128-Params::MATERIAL_SCALE[materialLevel])/128;
      }

      int operator == (const Scores &s) const {
        return s.mid == mid && s.end == end && s.any == any;
      }
      int operator != (const Scores &s) const {
        return s.mid != mid || s.end != end || s.any != any;
      }
      score_t mid, end, any;
    };

    template <ColorType side>
      int specialCaseEndgame(const Board &,
                             const Material &ourMaterial,
                             const Material &oppMaterial,
                             Scores &);

    struct AttackInfo
    {
        Bitboard allAttacks[2];
        Bitboard pawnAttacks[2];
        Bitboard knightAttacks[2];
        Bitboard bishopAttacks[2];
        Bitboard minorAttacks[2];
        Bitboard rookAttacks[2];
        Bitboard queenAttacks[2];
        Bitboard majorAttacks[2];
        Bitboard attackedBy2[2];
    };

    template<ColorType side>
      static int KBPDraw(const Board &board);

 private:

    template <ColorType side>
     void  positionalScore( const Board &board,
                            const PawnHashEntry &pawnData,
                            const KingPawnHashEntry &ourKPEntry,
                            const KingPawnHashEntry &oppKPEntry,
                            AttackInfo &ai,
                            Scores &scores,
                            Scores &oppScores);

    template <ColorType side>
        static bool theoreticalDraw(const Board &board);

    void adjustMaterialScore(const Board &board, ColorType side, Scores &scores) const;

    void adjustMaterialScoreNoPawns(const Board &board, ColorType side, Scores &scores) const;

    template <ColorType bishopColor>
      void scoreBishopAndPawns(const PawnHashEntry::PawnData &ourPawnData,const PawnHashEntry::PawnData &oppPawnData,Scores &scores,Scores &opp_scores);

    template <ColorType side>
    void threatScore(const Board &board,
                     const AttackInfo &ai,
                     Scores &scores);

    static score_t calcCover(const Board &board, ColorType side, int file, int rank);

    // Compute king cover for King on square 'kp' of color 'side'
    static score_t calcCover(const Board &board, ColorType side, Square kp);

    void calcPawnData(const Board &board, ColorType side,
			   PawnHashEntry::PawnData &entr);

    void calcKingEndgamePosition(const Board &board,ColorType side,
                                 const PawnHashEntry::PawnData &oppPawnData,
                                 KingPawnHashEntry &entry);

    void evalOutsidePassers(PawnHashEntry &pawnEntry);

    void calcPawnEntry(const Board &board, PawnHashEntry &pawnEntry);

    void pawnScore(const Board &board, ColorType side,
                   const PawnHashEntry::PawnData &pawnData,
                   const AttackInfo &ai,
                   Scores &);

    template <ColorType side>
      void scoreEndgame(const Board &,score_t k_pos,Scores &);

    score_t kingDistanceScore(const Board &) const;

    static void initBitboards();

    template<ColorType side>
      static void initProximity(Square i);

};

#endif

