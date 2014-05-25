// Copyright 1994-2014 by Jon Dart.  All Rights Reserved.

#include "scoring.h"
#include "util.h"
#include "bhash.h"
#include "bitprobe.h"
#include "hash.h"
#include "globals.h"
#include "movegen.h"
extern "C"
{
#include <stdlib.h>
#include <math.h>
};
#include <iomanip>

//#define PAWN_DEBUG
//#define EVAL_DEBUG

static CACHE_ALIGN Bitboard kingProximity[2][64];
static CACHE_ALIGN Bitboard kingNearProximity[64];
static CACHE_ALIGN Bitboard kingPawnProximity[2][64];

#define PARAM(x) params[x].current

enum {
   WEAK_MIDGAME1,
   WEAK_ENDGAME1,
   WEAK2_MIDGAME1,
   WEAK2_ENDGAME1,
   WEAK_MIDGAME2,
   WEAK_ENDGAME2,
   WEAK2_MIDGAME2,
   WEAK2_ENDGAME2,
   WEAK_LIMIT,
   WEAK_ON_OPEN_FILE_MIDGAME,
   WEAK_ON_OPEN_FILE_ENDGAME,
   ROOK_ATTACKS_WEAK_PAWN_MIDGAME,
   ROOK_ATTACKS_WEAK_PAWN_ENDGAME,
   ROOK_OPEN_FILE_MIDGAME,
   ROOK_OPEN_FILE_ENDGAME
};


Scoring::TuneParam Scoring::params[Scoring::NUM_PARAMS] = {
   Scoring::TuneParam(0,"weak_midgame1",-6,-20,0),
   Scoring::TuneParam(0,"weak_endgame1",-6,-20,0),
   Scoring::TuneParam(0,"weak2_midgame1",-6,-20,0),
   Scoring::TuneParam(0,"weak2_endgame1",-6,-20,0),
   Scoring::TuneParam(0,"weak_midgame2",-6,-20,0),
   Scoring::TuneParam(0,"weak_endgame2",-6,-20,0),
   Scoring::TuneParam(0,"weak2_midgame2",-6,-20,0),
   Scoring::TuneParam(0,"weak2_endgame2",-6,-20,0),
   Scoring::TuneParam(0,"weak_limit",0,0,1),
   Scoring::TuneParam(0,"weak_on_open_file_midgame",-20,-25,0),
   Scoring::TuneParam(0,"weak_on_open_file_endgame",-20,-25,0),
   Scoring::TuneParam(0,"rook_attacks_weak_pawn_midgame",20,0,25),
   Scoring::TuneParam(0,"rook_attacks_weak_pawn_endgame",20,0,25),
   Scoring::TuneParam(0,"rook_attacks_weak_pawn_midgame",10,0,25),
   Scoring::TuneParam(0,"rook_attacks_weak_pawn_endgame",10,0,25)
};

static const int CENTER_PAWN_BLOCK = -12;

// king cover scores, by rank of Pawn - rank of King
static const int KING_COVER[5] = { 22, 31, 12, 3, 2 };
static const int KING_FILE_OPEN = -11;

static const int KING_OFF_BACK_RANK[9] = { 0, 0, 0, 6, 36, 36, 36, 36, 36 };
static const int PIN_MULTIPLIER[2] = { 20, 30 };
// tuned, 13-May-2014
static const int KING_ATTACK_PARAM1 = 16;
static const int KING_ATTACK_PARAM2 = 4;
static const int KING_ATTACK_PARAM3 = 13;
static const CACHE_ALIGN int KING_ATTACK_SCALE[64] = {
0, 7, 13, 19, 25, 31, 37, 42, 49, 54, 60, 66, 71, 77, 83, 89, 95, 101,
106, 112, 118, 123, 130, 135, 140, 147, 152, 158, 164, 169, 176, 181,
187, 193, 198, 204, 210, 216, 221, 227, 233, 239, 245, 250, 257, 262,
267, 274, 279, 285, 291, 296, 302, 308, 314, 320, 325, 331, 337, 343,
348, 354, 360, 365};
   
#define BOOST
static const int KING_ATTACK_BOOST_THRESHOLD = 48;
static const int KING_ATTACK_BOOST_DIVISOR = 50;

const CACHE_ALIGN int Scoring::Scores:: MATERIAL_SCALE[32] =
{
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 12, 24, 36, 48, 60, 72, 84, 96,
   108, 116, 120, 128, 128, 128, 128, 128, 128, 128, 128
};

static const int MIDGAME_MATERIAL_THRESHOLD = 12;
static const int ENDGAME_MATERIAL_THRESHOLD = 23;

static const CACHE_ALIGN int KnightScores[64][2] = { 
   { -22,-23},{-14,-19},{-11,-16},{-10,-15},{-10,-15},{-11,-16},{-14,-19},{-22,-23},
   {-15,-13},{-6,-9},{-4,-5},{-3,-4},{-3,-4},{-4,-5},{-6,-9},{-15,-13},
   {-12,-9},{-4,-5},{-1,-2},{0,-1},{0,-1},{-1,-2},{-4,-5},{-12,-9},
   {-11,-8},{-3,-4},{0,-1},{3,0},{3,0},{0,-1},{-3,-4},{-11,-8},
   {-11,-8},{-3,-3},{0,0},{3,1},{3,1},{0,0},{-3,-3},{-11,-8},
   {-12,-9},{-4,-4},{-1,0},{0,0},{0,0},{-1,0},{-4,-4},{-12,-9},
   {-15,-13},{-6,-7},{-4,-4},{-3,-3},{-3,-3},{-4,-4},{-6,-7},{-15,-13},
   {-18,-17},{-9,-13},{-7,-9} ,{-6,-8} ,{-6,-8} ,{-7,-9} ,{-9,-13},{-18,-17} };

static const CACHE_ALIGN int BishopScores[64] =
{
   -18, -12, -12, -15, -15, -12, -12, -18,
    -10, 8, 0, 6, 6, 0, 8, -10,
    -10, 0, 6, 8, 8, 6, 0, -10,
     0, 0, 6, 10, 10, 6, 0, 0,
     0, 6, 6, 6, 6, 6, 6, 0,
     10, 10, 10, 10, 10, 10, 10, 10,
    -10, -10, -10, -10, -10, -10, -10, 10,
    -10, -10, -10, -10, -10, -10, -10, 10
};

// scores for White pieces
static const int KnightOutpostScores[64] =
{
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 4, 7, 7, 7, 7, 4, 0,
   0, 5, 10, 15, 15, 10, 5, 0,
   0, 5, 10, 15, 15, 10, 5, 0,
   0, 0, 7, 7, 7, 7, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0
};

// scores for White pieces
static const int BishopOutpostScores[64] =
{
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 3, 6, 6, 6, 6, 3, 0,
   0, 4, 8, 12, 12, 8, 4, 0,
   0, 4, 8, 12, 12, 8, 4, 0,
   0, 0, 6, 6, 6, 6, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0
};

static const int KingEndgameScores[64] =
{
   -28, -23, -18, -13, -13, -18, -23, -28,
   -22, -17, -12, -7, -7, -12, -17, -22,
   -16, -11, -6, -1, -1, -6, -11, -16,
   -10, -5, 0, 5, 5, 0, -5, -10,
    -4, 1, 6, 11, 11, 6, 1, -4,
     2, 7, 12, 17, 17, 12, 7, 2,
     8, 13, 18, 23, 23, 18, 13, 8,
     8, 13, 18, 23, 23, 18, 13, 8
};

static const int KBNKScores[64] =
{
   -40, -30, -15, -5, 5, 15, 30, 40,
   -30, -15, -5, 0, -1, 5, 15, 30,
   -15, -5, 0, 0, 0, -1, 5, 15,
    -5, -5, 0, -3, -3, 0, -1, 5,
     5, -1, 0, -3, -3, 0, 0, -5,
    15, 5, -1, 0, 0, 0, -5, -15,
    30, 15, 5, -1, 0, -5, -15, -30,
    40, 30, 15, 5, -5, -15, -30, -40
};

static const int KRScores[64] =
{
   -16, -16, -16, -16, 16, -16, -16, -16,
   -16,  0, 0, 0, 0, 0, 0, -16,
   -16,  0, 8, 8, 8,  8, 0, -16,
   -16,  0, 8, 16, 16, 8, 0, -16,
   -16,  0, 8, 16, 16, 8, 0, -16,
   -16,  0,  8, 8, 8, 8, 0, -16,
   -16,  0, 0, 0, 0, 0, 0, -16,
   -16, -16, -16, -16, -16, -16, -16, -16
};

struct BishopTrapPattern
{
   Bitboard bishopMask, pawnMask;
} BISHOP_TRAP_PATTERN[2][4];

// bishop trapped by pawn(s)
static const int BISHOP_TRAPPED = -147;

static const int BISHOP_PAIR[2] = { 42, 55 };
static const int BAD_BISHOP[2] = { -4, -6 
}
   ;

   
static const int BISHOP_PAWN_PLACEMENT[2] = { -10, -16 };

// material terms and trade bonus/penalties
static const int RB_ADJUST[9] = {
   0,
  (int)(0.25*PAWN_VALUE),
  (int)(0.075*PAWN_VALUE),
  (int)(-0.075*PAWN_VALUE),
  (int)(-0.25*PAWN_VALUE),
  (int)(-0.25*PAWN_VALUE),
  (int)(-0.25*PAWN_VALUE),
  (int)(-0.25*PAWN_VALUE),
  (int)(-0.25*PAWN_VALUE)};

static const int RBN_ADJUST[9] = {
   0,
  (int)(0.75*PAWN_VALUE - 0.25*PAWN_VALUE),
  (int)(0.75*PAWN_VALUE - 0.075*PAWN_VALUE),
  (int)(0.75*PAWN_VALUE + 0.075*PAWN_VALUE),
  (int)(0.75*PAWN_VALUE + 0.25*PAWN_VALUE),
  (int)(0.75*PAWN_VALUE + 0.25*PAWN_VALUE),
  (int)(0.75*PAWN_VALUE + 0.25*PAWN_VALUE),
  (int)(0.75*PAWN_VALUE + 0.25*PAWN_VALUE),
  (int)(0.75*PAWN_VALUE + 0.25*PAWN_VALUE)};

static const int QR_ADJUST[9] = {
  (int)(-0.5*PAWN_VALUE),
  (int)(0*PAWN_VALUE),
  (int)(0.5*PAWN_VALUE),
  (int)(0.5*PAWN_VALUE),
  (int)(0.5*PAWN_VALUE), 
  (int)(0.5*PAWN_VALUE), 
  (int)(0.5*PAWN_VALUE), 
  (int)(0.5*PAWN_VALUE), 
  (int)(0.5*PAWN_VALUE)};

static const int ENDGAME_PAWN_BONUS = 12;

static const int CASTLING_SCORES[6] = { 0, -7, -10, 28, 20, -28 };

// scores by game phase and rank
static const int PASSED_PAWN[2][8] = {
   { 0, 0, 6, 11, 18, 27, 56, 111 },
   { 0, 0, 9, 16, 28, 42, 84, 141 }
};
static const int POTENTIAL_PASSER[2][8] = {
   { 0, 0, 2, 4, 6, 9, 20, 0 },
   { 0, 0, 3, 6, 10, 15, 30, 0 }
};
static const int PASSED_PAWN_BLOCK[2][8] = {
   { 19, 19, 22, 25, 29, 34, 50, 81 },
   { 14, 14, 16, 18, 21, 25, 37, 60 }
};

static const int OUTSIDE_PP[2] = { 12, 25 };

static const int Midgame = 0;
static const int Endgame = 1;

static const int CONNECTED_PASSERS[2][8] = {
   { 0, 0, 0, 0, 0, 0, 19, 13 },
   { 0, 0, 0, 0, 0, 0, 38, 26 }
};

// by file:
static const int DOUBLED_PAWNS[2][8] =
{
   { -6, -8, -10, -10, -10, -10, -8, -6 },
   { -13, -17, -21, -21, -21, -21, -17, -13 }
};
static const int ISOLATED_PAWN[2] = { -8, -12 };
static const int SPACE = 2;
static const int PAWN_CENTER_SCORE = 3;

static const int ROOK_ON_7TH_RANK[2] = { 26, 26 };
static const int TWO_ROOKS_ON_7TH_RANK[2] = { 57, 66 };

// rook on open file or file with weak pawn:
static const int ROOK_OPEN_FILE = 20;
static const int QUEEN_OUT = -6;
static const int ROOK_BEHIND_PP[2] = { 5, 10 };
static const int PASSER_OWN_PIECE_BLOCK[2] = { -2, -5 };
static const int SIDE_PROTECTED_PAWN = -10;

static const int KNIGHT_MOBILITY[9] = { -18, -7, -2, 0, 2, 5, 7, 10, 12 };
static const int ROOK_MOBILITY[2][16] =
{
   { -22, -12, -8, -3, 0, 3, 7, 10, 12, 14, 17, 19, 21, 23, 24 },
   { -30, -17, -11, -5, 0, 5, 9, 14, 17, 20, 23, 26, 29, 31, 32, 34 }
};
static const int BISHOP_MOBILITY[16] = { -20, -11, -7, -3, 0, 3, 6, 9, 9, 9, 9, 9, 9, 9, 9, 9 };
static const int QUEEN_MOBILITY = 2;

// endgame terms
static const int PAWN_SIDE_BONUS = 28;
static const int BITBASE_WIN = 500;
static const int KING_AHEAD_OF_PAWN = 10;
static const int DISTANCE_FROM_PAWN = -4;
static const int SUPPORTED_PASSER6 = 38;
static const int SUPPORTED_PASSER7 = 76;

static Bitboard backwardW[64], backwardB[64];
CACHE_ALIGN Bitboard passedW[64], passedB[64];              // not static because needed by search module
static Bitboard outpostW[64], outpostB[64];
static Bitboard connected_passers[64][2];
static Bitboard rook_pawn_mask(Attacks::file_mask[0] | Attacks::file_mask[7]);
static Bitboard abcd_mask, efgh_mask;
static Bitboard left_side_mask[8], right_side_mask[8];
static Bitboard isolated_file_mask[8];
static Bitboard center;
static byte is_outside[256][256];

int distances[64][64];

class EndgamePattern {
public:
    EndgamePattern(const int32 mat1, const int32 mat2) {
        key = mat1 | ((int64)mat2 << 32);
    }
    int operator == (const EndgamePattern &other) const {
        return other.key == key;
    }
    int operator != (const EndgamePattern &other) const {
        return other.key != key;
    }
    int operator > (const EndgamePattern &other) const {
        return key > other.key;
    }
    int operator < (const EndgamePattern &other) const {
        return key < other.key;
    }
private:
    int64 key;
};

static const int DRAW_PATTERN_COUNT = 5;

static const EndgamePattern DRAW_PATTERN[] = {
                   EndgamePattern(Material::KRN, Material::KR),
                   EndgamePattern(Material::KRB, Material::KR),
                   EndgamePattern(Material::KBB, Material::KB),
                   EndgamePattern(Material::KBN, Material::KR),
                   EndgamePattern(Material::KQ, Material::KRB)
};


// Don't call it "distance" - conflicts with MS template library
static inline int distance1(int x, int y) {
   return distances[x][y];
}

static inline int OnFile(const Bitboard &b, int file) {
   return TEST_MASK(b, Attacks::file_mask[file - 1]);
}

static inline int FileOpen(const Board &board, int file, ColorType side) {
   return !TEST_MASK(board.pawn_bits[side], Attacks::file_mask[file - 1]);
}

static inline int FileOpen(const Board &board, int file) {
   return !TEST_MASK((board.pawn_bits[White] | board.pawn_bits[Black]), Attacks::file_mask[file - 1]);
}

// Return all squares attacked by a pawn of color "side" at square "sq".
static FORCEINLINE Bitboard pawn_attacks(const Board &board, Square sq, ColorType side) {
   return Bitboard(Attacks::pawn_attacks[sq][side] & board.pawn_bits[side]);
}

static void initBitboards() {
   int i, r;

   for(i = 0; i < 64; i++) {
      int file = File(i);
      int fmin = Util::Max(1, file - 2);
      int fmax = Util::Min(8, file + 2);
      for(int f = fmin; f <= fmax; f++) {
         int r;
         Square kp = i;
         if (File(i) == 8) kp -= 1;
         if (File(i) == 1) kp += 1;
         kingProximity[White][i] = Attacks::king_attacks[kp];
         kingProximity[White][i].set(i);
         kingProximity[White][i].set(kp);
         kingNearProximity[i] = kingProximity[White][i];
         r = Rank(i, White);
         if (r <= 6) {
            kingProximity[White][i].set(MakeSquare(File(kp) - 1, r + 2, White));
            kingProximity[White][i].set(MakeSquare(File(kp), r + 2, White));
            kingProximity[White][i].set(MakeSquare(File(kp) + 1, r + 2, White));
         }

         kingPawnProximity[White][i] = kingProximity[White][i];

         Square sq;
         Bitboard tmp(kingProximity[White][i]);
         while(tmp.iterate(sq)) {
            kingPawnProximity[White][i].set(sq);
            kingPawnProximity[White][i] |= Attacks::pawn_attacks[sq][Black];
         }

         kingProximity[Black][i] = Attacks::king_attacks[kp];
         kingProximity[Black][i].set(i);
         kingProximity[Black][i].set(kp);
         r = Rank(i, Black);
         if (r <= 6) {
            kingProximity[Black][i].set(MakeSquare(File(kp) - 1, r + 2, Black));
            kingProximity[Black][i].set(MakeSquare(File(kp), r + 2, Black));
            kingProximity[Black][i].set(MakeSquare(File(kp) + 1, r + 2, Black));
         }

         kingPawnProximity[Black][i] = kingProximity[Black][i];
         tmp = kingPawnProximity[Black][i];
         while(tmp.iterate(sq)) {
            kingPawnProximity[Black][i].set(sq);
            kingPawnProximity[Black][i] |= Attacks::pawn_attacks[sq][White];
         }
      }

      for(int j = 0; j < 64; j++) {
         int file_distance, rank_distance;
         Square kp = i;
         Square oppkp = j;
         file_distance = Util::Abs(File(kp) - File(oppkp));
         rank_distance = Util::Abs(Rank(kp, Black) - Rank(oppkp, Black));
         if (file_distance > rank_distance)
            distances[i][j] = file_distance;
         else
            distances[i][j] = rank_distance;
      }

      int rank = Rank(i, White);
      for(r = rank - 1; r > 1; r--) {
         Square sq2 = MakeSquare(file, r, White);
         if (file != 8) {
            sq2 = MakeSquare(file + 1, r, White);
            backwardW[i].set((int) sq2);
         }

         if (file != 1) {
            sq2 = MakeSquare(file - 1, r, White);
            backwardW[i].set((int) sq2);
         }
      }

      for(r = rank + 1; r < 8; r++) {
         Square sq2 = MakeSquare(file, r, White);
         passedW[i].set(sq2);
         if (file != 8) {
            sq2 = MakeSquare(file + 1, r, White);
            passedW[i].set(sq2);
            outpostW[i].set(sq2);
         }

         if (file != 1) {
            sq2 = MakeSquare(file - 1, r, White);
            passedW[i].set(sq2);
            outpostW[i].set(sq2);
         }
      }

      rank = Rank(i, Black);
      for(r = rank - 1; r > 1; r--) {
         Square sq2 = MakeSquare(file, r, Black);
         if (file != 8) {
            sq2 = MakeSquare(file + 1, r, Black);
            backwardB[i].set(sq2);
         }

         if (file != 1) {
            sq2 = MakeSquare(file - 1, r, Black);
            backwardB[i].set(sq2);
         }
      }

      if (file != 8) {
         connected_passers[i][White].set(i + 1);
         connected_passers[i][Black].set(i + 1);
      }

      connected_passers[i][Black] |= Attacks::pawn_attacks[i][Black];
      connected_passers[i][White] |= Attacks::pawn_attacks[i][White];
      for(r = rank + 1; r < 8; r++) {
         Square sq2 = MakeSquare(file, r, Black);
         passedB[i].set((int) sq2);
         if (file != 8) {
            sq2 = MakeSquare(file + 1, r, Black);
            passedB[i].set(sq2);
            outpostB[i].set(sq2);
         }

         if (file != 1) {
            sq2 = MakeSquare(file - 1, r, Black);
            passedB[i].set(sq2);
            outpostB[i].set(sq2);
         }
      }

      if (file < EFILE)
         abcd_mask.set(i);
      else
         efgh_mask.set(i);
   }

   int x;
   for(x = 1; x < 8; x++) {
      for(int y = x - 1; y >= 0; y--) left_side_mask[x] |= Attacks::file_mask[y];
   }

   for(x = 6; x >= 0; x--) {
      for(int y = x + 1; y < 8; y++) right_side_mask[x] |= Attacks::file_mask[y];
   }

   center.set(D4);
   center.set(D5);
   center.set(E4);
   center.set(E5);

   int first_bit[256];
   int last_bit[256];
   byte near_left[8];
   byte near_right[8];
   byte right_mask[8];
   byte left_mask[8];
   last_bit[0] = 0;
   for(int i = 1; i < 256; i++) {
      Bitboard b(i);
      first_bit[i] = b.firstOne();
      for(int j = 7; j >= 0; j--) {
         if (i & (1 << j)) {
            last_bit[i] = j;
            break;
         }
      }
   }

   for(int i = 0; i < 8; i++) {
      near_left[i] = near_right[i] = 0;
      right_mask[i] = left_mask[i] = 0;
      if (i > 0) near_left[i] |= 1 << (i - 1);
      if (i > 1) near_left[i] |= 1 << (i - 2);
      if (i < 7) near_right[i] |= 1 << (i + 1);
      if (i < 6) near_right[i] |= 1 << (i + 2);
      for(int j = i + 1; j < 8; j++) {
         right_mask[i] |= 1 << j;
      }

      for(int j = i - 1; j >= 0; j--) {
         left_mask[i] |= 1 << j;
      }

      if (i != 7) {
         isolated_file_mask[i] |= Attacks::file_mask[i + 1];
      }

      if (i != 0) {
         isolated_file_mask[i] |= Attacks::file_mask[i - 1];
      }
   }

   // outside passed pawn table (like Crafty). first index is
   // passed pawn mask, 2nd is all pawn mask
   memset(is_outside, '\0', 256 * 256);
   for(int i = 1; i < 256; i++) {
      int first_passer, last_passer;

      first_passer = first_bit[(int) i];
      last_passer = last_bit[(int) i];
      for(int j = 1; j < 256; j++) {
         if (first_passer <= 3 && (j & 0xf0) && !(j & left_mask[first_passer]) && !(j & near_right[first_passer])) {
            is_outside[i][j] = 1;
         }
         else if (last_passer >= 4 && (j & 0x0f) && !(j & right_mask[last_passer]) && !(j & near_left[last_passer])) {
            is_outside[i][j] = 1;
         }
      }
   }

   BISHOP_TRAP_PATTERN[White][0].bishopMask.set(A7);
   BISHOP_TRAP_PATTERN[White][0].bishopMask.set(B8);
   BISHOP_TRAP_PATTERN[White][0].pawnMask.set(B6);
   BISHOP_TRAP_PATTERN[White][0].pawnMask.set(C7);

   BISHOP_TRAP_PATTERN[White][1].bishopMask.set(H7);
   BISHOP_TRAP_PATTERN[White][1].bishopMask.set(G8);
   BISHOP_TRAP_PATTERN[White][1].pawnMask.set(G6);
   BISHOP_TRAP_PATTERN[White][1].pawnMask.set(F7);

   BISHOP_TRAP_PATTERN[White][2].bishopMask.set(H6);
   BISHOP_TRAP_PATTERN[White][2].bishopMask.set(G7);
   BISHOP_TRAP_PATTERN[White][2].bishopMask.set(H8);
   BISHOP_TRAP_PATTERN[White][2].bishopMask.set(F8);
   BISHOP_TRAP_PATTERN[White][2].pawnMask.set(G5);
   BISHOP_TRAP_PATTERN[White][2].pawnMask.set(F6);
   BISHOP_TRAP_PATTERN[White][2].pawnMask.set(E7);

   BISHOP_TRAP_PATTERN[White][3].bishopMask.set(A6);
   BISHOP_TRAP_PATTERN[White][3].bishopMask.set(B7);
   BISHOP_TRAP_PATTERN[White][3].bishopMask.set(A8);
   BISHOP_TRAP_PATTERN[White][3].bishopMask.set(C8);
   BISHOP_TRAP_PATTERN[White][3].pawnMask.set(B5);
   BISHOP_TRAP_PATTERN[White][3].pawnMask.set(C6);
   BISHOP_TRAP_PATTERN[White][3].pawnMask.set(D7);

   BISHOP_TRAP_PATTERN[Black][0].bishopMask.set(A2);
   BISHOP_TRAP_PATTERN[Black][0].bishopMask.set(B1);
   BISHOP_TRAP_PATTERN[Black][0].pawnMask.set(B3);
   BISHOP_TRAP_PATTERN[Black][0].pawnMask.set(C2);

   BISHOP_TRAP_PATTERN[Black][1].bishopMask.set(H2);
   BISHOP_TRAP_PATTERN[Black][1].bishopMask.set(G1);
   BISHOP_TRAP_PATTERN[Black][1].pawnMask.set(G3);
   BISHOP_TRAP_PATTERN[Black][1].pawnMask.set(F2);

   BISHOP_TRAP_PATTERN[Black][2].bishopMask.set(H3);
   BISHOP_TRAP_PATTERN[Black][2].bishopMask.set(G2);
   BISHOP_TRAP_PATTERN[Black][2].bishopMask.set(H1);
   BISHOP_TRAP_PATTERN[Black][2].bishopMask.set(F1);
   BISHOP_TRAP_PATTERN[Black][2].pawnMask.set(G4);
   BISHOP_TRAP_PATTERN[Black][2].pawnMask.set(F3);
   BISHOP_TRAP_PATTERN[Black][2].pawnMask.set(E2);

   BISHOP_TRAP_PATTERN[Black][3].bishopMask.set(A3);
   BISHOP_TRAP_PATTERN[Black][3].bishopMask.set(B2);
   BISHOP_TRAP_PATTERN[Black][3].bishopMask.set(A1);
   BISHOP_TRAP_PATTERN[Black][3].bishopMask.set(C1);
   BISHOP_TRAP_PATTERN[Black][3].pawnMask.set(B4);
   BISHOP_TRAP_PATTERN[Black][3].pawnMask.set(C3);
   BISHOP_TRAP_PATTERN[Black][3].pawnMask.set(D2);
}

void Scoring::initParams() 
{
   
}

        
void Scoring::init() {
   initBitboards();

}

void Scoring::cleanup() {
}

Scoring::Scoring(Hash *ht) :
   hashTable(ht) {
   clearHashTables();
}

Scoring::~Scoring() {
}

int Scoring::adjustMaterialScore(const Board &board, ColorType side)
{
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    int score = 0;
#ifdef EVAL_DEBUG
    int tmp = score;
#endif
    const int opponentPieceValue = (oppmat.value()-oppmat.pawnCount()*PAWN_VALUE);
    const int pieceDiff = ourmat.value()-ourmat.pawnCount()*PAWN_VALUE - opponentPieceValue;
    static const int NEAR_DRAW_CONFIGURATION[3] =
       {-75, -40, -15 };
    // If we have a material advantage but a configuration that would be a likely draw
    // w/o pawns, give no bonus for the extra material but discourage trade or loss
    // of remaining pawns
    if (ourmat.materialLevel() <= 9 && pieceDiff > 0) {
        const uint32 pieces = ourmat.pieceBits();
        if (pieces == Material::KN || pieces == Material::KB ||
            pieces == Material::KNN) {
            score += NEAR_DRAW_CONFIGURATION[Util::Min(2,ourmat.pawnCount())];
            return score;
        } else {
            EndgamePattern pattern(ourmat.pieceBits(),oppmat.pieceBits());
            for (int i = 0; i < DRAW_PATTERN_COUNT; i++) {
                if (DRAW_PATTERN[i] == pattern) {
                    score += NEAR_DRAW_CONFIGURATION[Util::Min(2,ourmat.pawnCount())];
                    return score;
                }
            }
        }
    }
    int majorDiff = ourmat.majorCount() - oppmat.majorCount();
    switch(majorDiff) {
    case 0: {
        if (ourmat.minorCount() == oppmat.minorCount() + 1) {
            // Knight or Bishop vs. pawns
            // Bonus for piece. Note: we do not have KN or KB alone - those are special
            // cases handled above.
            score += PAWN_VALUE/4;
        }
        else if  (ourmat.queenCount() == oppmat.queenCount()+1 &&
             ourmat.rookCount() == oppmat.rookCount() - 2) {
            // Queen vs. Rooks
            // Queen is better with minors on board (per Kaufman)
            score += QR_ADJUST[ourmat.minorCount()];
        }
        break;
    } 
    case 1: {
        if (ourmat.rookCount() == oppmat.rookCount()+1) {
            if (ourmat.minorCount() == oppmat.minorCount()) {
                // Rook vs. pawns. Usually the Rook is better.
                int pawnDiff = oppmat.pawnCount() - ourmat.pawnCount();
                score += PAWN_VALUE/3 - 4*Util::Max(0,pawnDiff-5);
            }
            else if (ourmat.minorCount() == oppmat.minorCount() - 1) {
                // Rook vs. minor
                // not as bad w. fewer pieces
                score += RB_ADJUST[ourmat.majorCount()];
            }
            else if (ourmat.minorCount() == oppmat.minorCount() - 2) {
                // bad trade - Rook for two minors, but not as bad w. fewer pieces
                score -= RBN_ADJUST[oppmat.majorCount()];
            }
        }
        // Q vs RB or RN is already dealt with by piece values
        break;
    }
    default:
        break;
    }
    // Encourage trading pieces (but not pawns) when we are ahead in material.
#ifdef EVAL_DEBUG
    tmp = score;
#endif
    static const int PAWN_TRADE_SCORE[3] =
       {-45, -25, -10 };
    const int mdiff = ourmat.value() - oppmat.value();
    if (mdiff >= 3*PAWN_VALUE) {
       // Encourage trading pieces when we are ahead in material.
       if (oppmat.materialLevel() < 16) {
          score += mdiff*(16-oppmat.materialLevel())/96;
       }
       // Discourage trading pawns when our own material is low (because
       // harder to win).
       if (ourmat.materialLevel() < 16 && ourmat.pawnCount() < 3) {
          score += PAWN_TRADE_SCORE[ourmat.pawnCount()];
       }
    }
    // Also give bonus for having more pawns in endgame (assuming
    // we have not traded pieces for pawns).
    if (mdiff>=0 && pieceDiff>=0 && ourmat.materialLevel() <= ENDGAME_MATERIAL_THRESHOLD) {
       const int pawnDiff = ourmat.pawnCount() - oppmat.pawnCount();
       if (pawnDiff > 0) {
          score += pawnDiff*ENDGAME_PAWN_BONUS*(128-Scores::MATERIAL_SCALE[ourmat.materialLevel()])/128;
       }
    }
#ifdef EVAL_DEBUG
    if (score-tmp) {
       cout << "trade down=" << score-tmp << endl;
    }
#endif
    return score;
}


int Scoring::adjustMaterialScoreNoPawns( const Board &board, ColorType side )
{
    // pawnless endgames. Generally drawish in many cases.
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    int score = 0;
    if (ourmat.infobits() == Material::KQ) {
        if (oppmat.infobits() == Material::KRR) {
            score -= (QUEEN_VALUE-2*ROOK_VALUE);  // even
        }
        else if (oppmat.infobits() == Material::KRB) {
                                                  // even
            score -= QUEEN_VALUE-(ROOK_VALUE+BISHOP_VALUE);
        }
                                                  // close to even
        else if (oppmat.infobits() == Material::KRN) {
            score -= QUEEN_VALUE-(ROOK_VALUE+BISHOP_VALUE)+PAWN_VALUE/4;
        }
        else if (oppmat.infobits() == Material::KB ||
                 oppmat.infobits() == Material::KN ||
                 oppmat.infobits() == Material::KR) {
            // won for the stronger side
            score += 2*PAWN_VALUE;
        }
    }
    else if (ourmat.infobits() == Material::KR) {
        if (oppmat.infobits() == Material::KBN) { // close to even
            score += (KNIGHT_VALUE+BISHOP_VALUE-ROOK_VALUE)-PAWN_VALUE/4;
        }
                                                  // even
        else if (oppmat.infobits() == Material::KB) {
            score -= (ROOK_VALUE-BISHOP_VALUE);
        }
                                                  // close to even
        else if (oppmat.infobits() == Material::KN) {
            score -= (ROOK_VALUE-KNIGHT_VALUE)-PAWN_VALUE/4;
        }
        else if (oppmat.infobits() == Material::KRB) {
           // even
           score += BISHOP_VALUE;
        }
        else if (oppmat.infobits() == Material::KRN) { 
           // even
           score += KNIGHT_VALUE;
        }
    }
    else if (ourmat.infobits() == Material::KRR) {
        if (oppmat.infobits() == Material::KRB) { // even
            score -= ROOK_VALUE-BISHOP_VALUE;
        }
                                                  // close to even
        else if (oppmat.infobits() == Material::KRN) {
            score -= ROOK_VALUE-KNIGHT_VALUE-PAWN_VALUE/4;
        }
                                                  // close to even
        else if (oppmat.infobits() == Material::KRBN) {
            score += (KNIGHT_VALUE+BISHOP_VALUE-ROOK_VALUE)-PAWN_VALUE/4;
        }
    }
    return score;
}

template<ColorType side>
int Scoring::calcCover(const Board &board, int file, int rank) {
   Square sq, pawn;
   int cover = -KING_COVER[1];
   if (side == White) {
      sq = MakeSquare(file, Util::Max(1, rank - 1), White);
      pawn = Bitboard(Attacks::file_mask_up[sq] & board.pawn_bits[White]).firstOne();
   }
   else {
      sq = MakeSquare(file, Util::Max(1, rank - 1), Black);
      pawn = Bitboard(Attacks::file_mask_down[sq] & board.pawn_bits[Black]).lastOne();
   }
   if (pawn == InvalidSquare) {
      if (FileOpen(board, file)) cover += KING_FILE_OPEN;
   }
   else {
      cover += KING_COVER[Util::Min(4, Rank<side> (pawn) - rank)];
   }
   return cover;
}

// Calculate a king cover score
template<ColorType side>
int Scoring::calcCover(const Board &board, Square kp) {
   int cover = 0;
   int kpfile = File(kp);
   if (side == White) {
      int rank = Rank(kp, White);
      if (kpfile > 5) {
         for(int i = 6; i <= 8; i++) {
            cover += calcCover<White> (board, i, rank);
         }
      }
      else if (kpfile < 4) {
         for(int i = 1; i <= 3; i++) {
            cover += calcCover<White> (board, i, rank);
         }
      }
      else {
         for(int i = kpfile - 1; i <= kpfile + 1; i++) {
            cover += calcCover<White> (board, i, rank);
         }
      }
   }
   else {
      int rank = Rank(kp, Black);
      if (kpfile > 5) {
         for(int i = 6; i <= 8; i++) {
            cover += calcCover<Black> (board, i, rank);
         }
      }
      else if (kpfile < 4) {
         for(int i = 1; i <= 3; i++) {
            cover += calcCover<Black> (board, i, rank);
         }
      }
      else {
         for(int i = kpfile - 1; i <= kpfile + 1; i++) {
            cover += calcCover<Black> (board, i, rank);
         }
      }
   }

   cover = Util::Min(0, cover);
   return cover;
}

template<ColorType side>
void Scoring::calcCover(const Board &board, KingCoverHashEntry &coverEntry) {
   Square kp = board.kingSquare(side);

   // discourage shuttling the king between G1/H1 by
   // treating these the same
   if (side == White) {
      if (kp == H1) kp = G1;
      if (kp == A1) kp = B1;
   }
   else {
      if (kp == H8) kp = G8;
      if (kp == A8) kp = B8;
   }

   int cover = calcCover<side> (board, kp);
   switch(board.castleStatus(side))
   {
   case CanCastleEitherSide:
      {
         int k_cover = calcCover<side> (board, side == White ? G1 : G8);
         int q_cover = calcCover<side> (board, side == White ? B1 : B8);
         coverEntry.cover = (cover * 2) / 3 + Util::Min(k_cover, q_cover) / 3;
         break;
      }

   case CanCastleKSide:
      {
         int k_cover = calcCover<side> (board, side == White ? G1 : G8);
         coverEntry.cover = (cover * 2) / 3 + k_cover / 3;
         break;
      }

   case CanCastleQSide:
      {
         int q_cover = calcCover<side> (board, side == White ? B1 : B8);
         coverEntry.cover = (cover * 2) / 3 + q_cover / 3;
         break;
      }

   default:
      coverEntry.cover = cover;
      break;
   }
}

template<ColorType side>
int Scoring::kingCover(const Board &board) {
   const int oppMaterialLevel = board.getMaterial(OppositeColor(side)).materialLevel();
   if (oppMaterialLevel >= MIDGAME_MATERIAL_THRESHOLD) {
      hash_t kcHash = BoardHash::kingCoverHash(board, side);
      KingCoverHashEntry *kingCoverEntry = &kingCoverHashTable[side][kcHash % KING_COVER_HASH_SIZE];
      if (kingCoverEntry->hc != (uint32) (kcHash >> 32)) {
         calcCover<side> (board, *kingCoverEntry);
         kingCoverEntry->hc = (uint32) (kcHash >> 32);
      }
#ifdef EVAL_DEBUG
      cout << "unscaled cover score " << ColorImage(side) << " = " << kingCoverEntry->cover << endl;
#endif
      return kingCoverEntry->cover;
   }
   else {
      return 0;
   }
}

template<ColorType side>
int Scoring::outpost(const Board &board,
               Square sq,
               Square scoreSq,
               const int scores[64],
               const PawnHashEntry::PawnData &oppPawnData) {
   int outpost = 0;
   if (side == White) {
      if (!TEST_MASK(outpostW[sq], board.pawn_bits[Black])) {
         outpost = scores[scoreSq];
      }
   }
   else {
      if (!TEST_MASK(outpostB[sq], board.pawn_bits[White])) {
         outpost = scores[scoreSq];
      }
   }

   if (outpost) {
      if (oppPawnData.opponent_pawn_attacks.isSet(sq)) {
         return outpost;
      }
      else {
         // not defended by pawn
         return 2*outpost/3;
      }
   }
   else {
      return 0;
   }
}

template<ColorType side>
void Scoring::pieceScore(const Board &board,
               const PawnHashEntry::PawnData &ourPawnData,
               const PawnHashEntry::PawnData &oppPawnData,
               int cover,
               Scores &scores,
               Scores &opp_scores,
               bool endgame) {
   Bitboard b(board.occupied[side] &~board.pawn_bits[side]);
   b.clear(board.kingSquare(side));

   static const int ATTACK_FACTOR[6] = { 0, 1, 2, 2, 3, 4 };
   int pin_count = 0;
   Square kp = board.kingSquare(side);
   ColorType oside = OppositeColor(side);
   Square okp = board.kingSquare(oside);
   const Bitboard &nearKing = kingProximity[oside][okp];
   Bitboard allAttacks;
   int attackWeight = 0;
   int attackCount = 0;
   
   Square sq;
   while(b.iterate(sq))
   {
#ifdef EVAL_DEBUG
      Scores tmp = scores;
#endif
      Square scoreSq = (side == White) ? sq : 63 - sq;
      switch(TypeOfPiece(board[sq]))
      {
      case Knight:
         {
            scores.mid += KnightScores[scoreSq][Midgame];
            scores.end += KnightScores[scoreSq][Endgame];

            const Bitboard &knattacks = Attacks::knight_attacks[sq];
            const int mobl = KNIGHT_MOBILITY[Bitboard(knattacks &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount()];
#ifdef EVAL_DEBUG
            cout << "knight moblility =" << mobl << endl;
#endif
            scores.any += mobl;
            if (KnightOutpostScores[scoreSq]) {
               int outpost_score = outpost<side> (board, sq, scoreSq, KnightOutpostScores, oppPawnData);
#ifdef EVAL_DEBUG
               cout << "knight outpost =" << outpost_score << endl;
#endif
               scores.any += outpost_score;
            }
            allAttacks |= knattacks;

            if (knattacks & nearKing) {
               attackWeight += ATTACK_FACTOR[Knight];
               attackCount++;
            }
            break;
         }

      case Bishop:
         {
            scores.mid += BishopScores[scoreSq];

            //scores.end += BishopScores[scoreSq]/2;
            const Bitboard battacks(board.bishopAttacks(sq));
            allAttacks |= battacks;
            if (!endgame) {
               if (battacks & nearKing) {
                  attackWeight += ATTACK_FACTOR[Bishop];
                  attackCount++;
               }
               else if (battacks & board.queen_bits[side]) {
                  // possible stacked attackers
                  if (board.bishopAttacks(sq, side) & nearKing) {
                     attackWeight += ATTACK_FACTOR[Bishop];
                     attackCount++;
                  }
               }
            }

            if (BishopOutpostScores[scoreSq]) {
               scores.any += outpost<side> (board, sq, scoreSq, BishopOutpostScores, oppPawnData);
            }

            if (board.pinOnDiag(sq, okp, oside)) {
               pin_count++;
            }

            const int mobl = BISHOP_MOBILITY[Bitboard(battacks &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount()];
#ifdef EVAL_DEBUG
            cout << "bishop mobility (" << SquareImage(sq) << "): " << mobl << endl;
#endif
            scores.any += mobl;
            break;
         }

      case Rook:
         {
            const int r = Rank(sq, side);
            if (r == 7) {
               scores.mid += ROOK_ON_7TH_RANK[Midgame];
               scores.end += ROOK_ON_7TH_RANK[Endgame];

               Bitboard right(Attacks::rank_mask_right[r] & board.occupied[side]);
               if (right && board[right.firstOne()] == MakePiece(Rook, side)) {

                  // 2 connected rooks on 7th
                  scores.mid += TWO_ROOKS_ON_7TH_RANK[Midgame];
                  scores.end += TWO_ROOKS_ON_7TH_RANK[Endgame];
               }
            }

            const int file = File(sq);
            const Bitboard rattacks(board.rookAttacks(sq));
            allAttacks |= rattacks;
            if (FileOpen(board, file)) {
               scores.mid += PARAM(ROOK_OPEN_FILE_MIDGAME);
               scores.end += PARAM(ROOK_OPEN_FILE_ENDGAME);
            }

            Bitboard rattacks2(board.rookAttacks(sq, side));
            if (rattacks2 & oppPawnData.weak_pawns) {
               scores.mid += PARAM(ROOK_ATTACKS_WEAK_PAWN_MIDGAME);
               scores.end += PARAM(ROOK_ATTACKS_WEAK_PAWN_ENDGAME);
            }

            const int mobl = Bitboard(rattacks2 &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount();
            scores.mid += ROOK_MOBILITY[Midgame][mobl];
            scores.end += ROOK_MOBILITY[Endgame][mobl];
#ifdef EVAL_DEBUG
            cout << "rook mobility: " << mobl << endl;
#endif
            if (!endgame) {
               Bitboard attacks(rattacks2 &nearKing);
               if (attacks) {
                  attackWeight += ATTACK_FACTOR[Rook];

                  Bitboard attacks2(attacks &kingNearProximity[okp]);
                  if (attacks2) {
                     attacks2 &= (attacks2 - 1);
                     if (attacks2) {

                        // rook attacks at least 2 squares near king
                        attackWeight++;
#ifdef EVAL_DEBUG
                        cout << "rook attack boost= 1" << endl;
#endif
                     }
                  }
               }
            }

            if (board.pinOnRankOrFile(sq, okp, oside)) {
               pin_count++;
            }
            if (KnightOutpostScores[scoreSq]) {
               scores.any += outpost<side> (board, sq, scoreSq, KnightOutpostScores, oppPawnData)*3/2;
            }

            break;
         }

      case Queen:
         {
            int qmobl = 0;

            Bitboard battacks(board.bishopAttacks(sq));
            allAttacks |= battacks;
            Bitboard qmobility(battacks);
            Bitboard kattacks;
            
            if (!endgame) {
               kattacks = battacks & nearKing;
               if (!kattacks) {
                  kattacks = board.bishopAttacks(sq, side) & nearKing;
               }
            }

            if (board.pinOnDiag(sq, okp, oside)) {
               pin_count++;
            }

            Bitboard rattacks = board.rookAttacks(sq);
            qmobility |= rattacks;
            allAttacks |= qmobility;
            if (!endgame) {
               int rank = Rank(sq, side);
               if (rank > 3) {
                  Bitboard back
                           ((board.knight_bits[side] | board.bishop_bits[side]) & Attacks::rank_mask[side == White ? 0 : 7]);
                  int queenOut = (QUEEN_OUT - (rank - 4) / 2) * (int) back.bitCount();
#ifdef EVAL_DEBUG
                  if (queenOut) {
                     cout << "premature Queen develop (" << ColorImage(side) << "): " << queenOut << endl;
                  }
#endif
                  scores.mid += queenOut;
               }

               if (rattacks & nearKing) {
                  kattacks |= (rattacks & nearKing);
               }
               else {
                  kattacks |= (board.rookAttacks(sq, side) & nearKing);
               }

               if (kattacks) {
                  attackWeight += ATTACK_FACTOR[Queen];
#ifdef EVAL_DEBUG
                  int tmp = attackWeight;
#endif
                  // bonus if Queen attacks multiple squares near King:
                  Bitboard nearAttacks(kattacks &kingNearProximity[okp]);
                  if (nearAttacks) {
                     nearAttacks &= (nearAttacks - 1);      // clear 1st bit
                     if (nearAttacks) {
                        attackWeight++;
                        nearAttacks &= (nearAttacks - 1);   // clear 1st bit
                        if (nearAttacks) {
                           attackWeight++;
                        }
                     }
                  }

#ifdef EVAL_DEBUG
                  if (attackWeight - tmp) cout << "queen attack boost= " << attackWeight - tmp << endl;
#endif
               }
            }

            qmobl += Bitboard(qmobility &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount();
            scores.any += QUEEN_MOBILITY * (Util::Min(14, qmobl) - 7);
#ifdef EVAL_DEBUG
            cout << "queen mobility=" << QUEEN_MOBILITY * (Util::Min(14, qmobl) - 7) << endl;
#endif
            if (board.pinOnRankOrFile(sq, okp, oside)) {
               pin_count++;
            }
         }
         break;

      default:
         break;
      }

#ifdef EVAL_DEBUG
      cout <<
         ColorImage(side) << ' ' << PieceImage(TypeOfPiece(board[sq])) <<
         " on " << SquareImage(sq) << ": " <<
         " mid=" << scores.mid -tmp.mid <<
         " endgame=" << scores.end - tmp.end <<
         " any=" << scores.any - tmp.any << endl;
#endif
   }

   const int bishopCount = board.getMaterial(side).bishopCount();
   if (bishopCount == 1) {
      const int opp_pawns = oppPawnData.b_square_pawns + oppPawnData.w_square_pawns;
      const int whitePawns = ourPawnData.w_square_pawns;
      const int blackPawns = ourPawnData.b_square_pawns;
      const int totalPawns = whitePawns + blackPawns;
      if (TEST_MASK(Board::white_squares, board.bishop_bits[side])) {
         if ((whitePawns + blackPawns > 4) && (whitePawns > blackPawns + 2))
         {
#ifdef EVAL_DEBUG
            Scores tmp = scores;
#endif
            scores.mid += BAD_BISHOP[Midgame] * (whitePawns - blackPawns);
            scores.end += BAD_BISHOP[Endgame] * (whitePawns - blackPawns);
#ifdef EVAL_DEBUG
            cout << "bad bishop (" << ColorImage(side) << "): (" <<
               scores.mid - tmp.mid << "," << scores.end - tmp.end <<
               ")" << endl;
#endif
         }

         if (opp_pawns) {
            // penalize pawns on same color square as opposing single Bishop
            opp_scores.mid += oppPawnData.w_square_pawns*BISHOP_PAWN_PLACEMENT[Midgame]/opp_pawns;
            opp_scores.end += oppPawnData.w_square_pawns*BISHOP_PAWN_PLACEMENT[Endgame]/opp_pawns;
         }
      }
      else {
         if ((whitePawns + blackPawns > 4) && (blackPawns > whitePawns + 2))
         {
#ifdef EVAL_DEBUG
            Scores tmp = scores;
#endif
            scores.mid += BAD_BISHOP[Midgame] * (whitePawns - blackPawns);
            scores.end += BAD_BISHOP[Endgame] * (whitePawns - blackPawns);
#ifdef EVAL_DEBUG
            cout << "bad bishop (" << ColorImage(side) << "): (" <<
               scores.mid - tmp.mid << "," << scores.end - tmp.end <<
               ")" << endl;
#endif
         }
         if (opp_pawns) {
           // penalize pawns on same color square as opposing single Bishop
           opp_scores.mid += oppPawnData.b_square_pawns*BISHOP_PAWN_PLACEMENT[Midgame]/opp_pawns;
           opp_scores.end += oppPawnData.b_square_pawns*BISHOP_PAWN_PLACEMENT[Endgame]/opp_pawns;
         }
      }
   }
   else if (bishopCount >= 2) {
      // Bishop pair bonus, higher bonus in endgame
      scores.mid += BISHOP_PAIR[Midgame];
      scores.end += BISHOP_PAIR[Endgame];
#ifdef EVAL_DEBUG
      cout << "bishop pair (" << ColorImage(side) << ")" << endl;
#endif
   }

   allAttacks |= oppPawnData.opponent_pawn_attacks;
   allAttacks |= Attacks::king_attacks[kp];
   const int squaresAttacked =  Bitboard(allAttacks & kingNearProximity[okp]).bitCount();
   if (!endgame) {

      // add in pawn attacks
      int proximity = Bitboard(kingPawnProximity[oside][okp] & board.pawn_bits[side]).bitCount();
      attackWeight += proximity;
#ifdef EVAL_DEBUG
      cout << ColorImage(side) << " piece attacks on opposing king:" << endl;
      cout << " cover= " << cover << endl;
      cout << " pawn proximity=" << proximity << endl;
      cout << " attackCount=" << attackCount << endl;
      cout << " attackWeight=" << attackWeight << endl;
      cout << " squaresAttacked=" << squaresAttacked << endl;
      cout << " pin_count=" << pin_count << endl;
#endif

      int scale =
         (KING_ATTACK_PARAM1*attackWeight + KING_ATTACK_PARAM2*attackWeight*attackCount + KING_ATTACK_PARAM3*squaresAttacked)/16;
      int attack = KING_ATTACK_SCALE[Util::Min(scale, 63)];
      if (pin_count) attack += PIN_MULTIPLIER[Midgame] * pin_count;

      int kattack = attack;
#ifdef BOOST
#ifdef EVAL_DEBUG
      int kattack_tmp = kattack;
#endif
      if (kattack) {
         if (cover < -KING_ATTACK_BOOST_THRESHOLD) {
            kattack += Util::Min(kattack / 2, (-(cover + KING_ATTACK_BOOST_THRESHOLD) * kattack) / KING_ATTACK_BOOST_DIVISOR);
#ifdef EVAL_DEBUG
            cout << "boost factor= " << (float) kattack / (float) kattack_tmp << endl;
#endif
         }
      }
#endif
#ifdef EVAL_DEBUG
      Scores s;
      s.mid = kattack;
      cout << " king attack score (" << ColorImage(side) << ") : " << kattack << " (pre-scaling), " << s.blend(board.getMaterial(oside).materialLevel()) << " (scaled)" << endl;
#endif

      // decrement the opposing side's scores because we want to
      // scale king attacks by this side's material level.
      opp_scores.mid -= kattack;
   }

   if (pin_count) scores.end += PIN_MULTIPLIER[Endgame] * pin_count;
}

int Scoring::calcPawnData(const Board &board,
                          ColorType side,
                          PawnHashEntry::PawnData &entr) {

   // This function computes a pawn structure score that depends
   // only on the location of pawns (of both colors). It also fills
   // in the pawn hash entry.
   //
   memset(&entr, '\0', sizeof(PawnHashEntry::PawnData));

   int score = 0;
   int incr = (side == White) ? 8 : -8;
   const ColorType oside = OppositeColor(side);
   entr.opponent_pawn_attacks = board.allPawnAttacks(oside);

   Piece enemy_pawn = MakePiece(Pawn, oside);
   Piece our_pawn = MakePiece(Pawn, side);
   Bitboard bi(board.pawn_bits[side]);
   Bitboard all_pawns(board.pawn_bits[White] | board.pawn_bits[Black]);
   Bitboard potentialPlus, potentialMinus;
   Square sq;
   while(bi.iterate(sq))
   {
#ifdef PAWN_DEBUG
      int tmp = score;
      int mid_tmp = entr.midgame_score;
      int end_tmp = entr.endgame_score;
      cout << ColorImage(side) << " pawn on " << FileImage(sq) << RankImage(sq);
#endif

      int file = File(sq);
      int rank = Rank(sq, side);
      if (SquareColor(sq) == White)
         entr.w_square_pawns++;
      else
         entr.b_square_pawns++;
      entr.pawn_file_mask |= (1 << (file - 1));

      int backward = 0;
      int passed = 0;
      int weak = 0;
      int doubled = 0;
      int isolated;
      Bitboard doubles;

      // Note: "backward" here means that the pawn in its current location
      // cannot be protected by a pawn of the same color. I.e. it has
      // no friendly pawn on an adjacent file and earlier rank. This is
      // not though the customary definition of backwardness.
      if (side == White) {
         backward = !TEST_MASK(board.pawn_bits[side], backwardW[sq]);
         passed = !TEST_MASK(board.pawn_bits[oside], passedW[sq]);
         doubles = (board.pawn_bits[side] & Attacks::file_mask_up[sq]);
      }
      else {
         backward = !TEST_MASK(board.pawn_bits[side], backwardB[sq]);
         passed = !TEST_MASK(board.pawn_bits[oside], passedB[sq]);
         doubles = (board.pawn_bits[side] & Attacks::file_mask_down[sq]);
      }

      isolated = !TEST_MASK(isolated_file_mask[file - 1], board.pawn_bits[side]);
      if (doubles) {
#ifdef PAWN_DEBUG
         cout << " doubled";
#endif
         if (passed) {
#ifdef PAWN_DEBUG
            cout << " passed";
#endif

            // Doubled but potentially passed pawn.
            // Don't score as passed but give "potential passer" bonus
            entr.midgame_score += POTENTIAL_PASSER[Midgame][rank];
            entr.endgame_score += POTENTIAL_PASSER[Endgame][rank];
            passed = 0;
         }

         entr.midgame_score += DOUBLED_PAWNS[Midgame][file - 1];
         entr.endgame_score += DOUBLED_PAWNS[Endgame][file - 1];
         if (doubles.bitCountOpt() > 1) {
            // tripled pawns
#ifdef PAWN_DEBUG
            cout << " tripled";
#endif
            entr.midgame_score += DOUBLED_PAWNS[Midgame][file - 1] / 2;
            entr.endgame_score += DOUBLED_PAWNS[Endgame][file - 1] / 2;
         }

         doubled++;
      }

      if (isolated && !passed) {
         entr.midgame_score += ISOLATED_PAWN[Midgame];
         entr.endgame_score += ISOLATED_PAWN[Endgame];
#ifdef PAWN_DEBUG
         cout << " isolated";
#endif
      }
      else if (rank < 6 && backward) {
          // Pawn is not, and cannot be, protected by a pawn of
          // the same color. See if it is also blocked from advancing
          // by adjacent pawns.
          int i = 0;
          int limit = PARAM(WEAK_LIMIT) ? 3 : (rank == 2 ? 3 : 2);
          for(Square sq2 = sq + incr;
              ++i < limit && !weak && OnBoard(sq2) && TypeOfPiece(board[sq2]) != Pawn;
              sq2 += incr) {
              int defenders = 0;
              int attackers = 0;
              // count own pawns that are defending, or could defend, the pawn
              // on sq2
              if (file != 1) {
                  Piece attacker = board[(side == White) ? sq2 + 7 : sq2 - 9];
                  if (attacker == enemy_pawn) attackers++;
                  for (int r = Rank(sq2,side)-1; r >=2 ; r--) {
                      Piece defender = board[MakeSquare(file-1,r,side)];
                      if (defender == enemy_pawn) break;
                      if (defender == our_pawn) {
                          defenders++;
                          break;
                      }
                  }
              }
              if (file != 8) {
                  Piece attacker = board[(side == White) ? sq2 + 9 : sq2 - 7];
                  if (attacker == enemy_pawn) attackers++;
                  for (int r = Rank(sq2,side)-1; r >=2 ; r--) {
                      Piece defender = board[MakeSquare(file+1,r,side)];
                      if (defender == enemy_pawn) break;
                      if (defender == our_pawn) {
                          defenders++;
                          break;
                      }
                  }
              }
              int patcks = (attackers - defenders);
              if (patcks > 0) {
#ifdef PAWN_DEBUG
                  cout << " weak";
#endif
                  if (i == 1) {
                     entr.midgame_score += PARAM(WEAK_MIDGAME1);
                     entr.endgame_score += PARAM(WEAK_ENDGAME1);
                  }
                  else
                  {
                     entr.midgame_score += PARAM(WEAK_MIDGAME2);
                     entr.endgame_score += PARAM(WEAK_ENDGAME2);
                  }
                  weak++;
              }
              if (patcks > 1) {
                  // "extra weak" pawn
                 if (i == 1) {
                    entr.midgame_score += PARAM(WEAK2_MIDGAME1);
                    entr.endgame_score += PARAM(WEAK2_ENDGAME1);
#ifdef PAWN_DEBUG
                    cout << " extra weak";
#endif
                 }
                 else {
                    entr.midgame_score += PARAM(WEAK2_MIDGAME2);
                    entr.endgame_score += PARAM(WEAK2_ENDGAME2);
                 }
                 
              }
          }
      }

      if (!passed && (weak || isolated)) {
          entr.weak_pawns.set(sq);
          if (!OnFile(board.pawn_bits[oside], file)) {
              entr.weakopen++;
#ifdef PAWN_DEBUG
              cout << " weak/open";
#endif
          }
      }

      if (passed) {
         // note: doubled passed pawns were scored earlier, are not
         // considered passed.
#ifdef PAWN_DEBUG
         cout << " passed";
#endif
         entr.midgame_score += PASSED_PAWN[Midgame][rank];
         entr.endgame_score += PASSED_PAWN[Endgame][rank];
         entr.passers.set(sq);
      }
      else {
         int potential = 0;

         // not passed, check for potential passer
         Bitboard allPawns(board.pawn_bits[White] | board.pawn_bits[Black]);
         int diff;
         Square dup = InvalidSquare;
         if (side == White) {
            Bitboard ahead(board.pawn_bits[Black] & passedW[sq]);
            if (!(ahead & Attacks::file_mask[file - 1])) {
               int blocker1 = file == 1 ? 0 : !Bitboard(ahead & Attacks::file_mask[file - 2]).isClear();
               int blocker2 = file == 8 ? 0 : !Bitboard(ahead & Attacks::file_mask[file]).isClear();
               if ((blocker1 ^ blocker2) == 1) {

                  // We are blocked by one or more pawns on an adjacent file
                  diff = blocker1 ? -1 : 1;
                  ASSERT(diff != 0);
                  potential = 1;

                  int bPawns = 0, wPawns = 0;
                  for(int file = File(sq) + diff; file >= 1 && file <= 8 && potential; file += diff) {
                     Bitboard pawns(allPawns &Attacks::file_mask[file - 1]);
                     if (pawns.isClear()) break;             // no pawns on file
                     Square pawn;
                     int wFilePawns = 0, bFilePawns = 0;
                     while((pawn = pawns.lastOne()) != InvalidSquare) {
                        if (PieceColor(board[pawn]) == Black) {
                           if (wFilePawns) break;
                           bFilePawns++;
                        }
                        else {

                           // check for another potential passer that
                           // shares the same blocker(s) as this one
                           int aDup = diff == 1 ? potentialMinus.isSet(pawn) : potentialPlus.isSet(pawn);
                           if (aDup) {
                              dup = pawn;
                           }

                           wFilePawns++;
                        }

                        pawns.clear(pawn);
                     }

                     wPawns += wFilePawns;
                     bPawns += bFilePawns;
                     if (bPawns > wPawns + 1) break;
                  }

                  potential = wPawns >= bPawns;
               }
            }
         }
         else {
            Bitboard ahead(board.pawn_bits[White] & passedB[sq]);
            if (!(ahead & Attacks::file_mask[file - 1])) {
               int blocker1 = file == 1 ? 0 : !Bitboard(ahead & Attacks::file_mask[file - 2]).isClear();
               int blocker2 = file == 8 ? 0 : !Bitboard(ahead & Attacks::file_mask[file]).isClear();
               if ((blocker1 ^ blocker2) == 1) {

                  // We are blocked by one or more pawns on an adjacent file
                  diff = blocker1 ? -1 : 1;
                  ASSERT(diff != 0);
                  potential = 1;

                  int bPawns = 0, wPawns = 0;
                  for(int file = File(sq) + diff; file >= 1 && file <= 8 && potential; file += diff) {
                     Bitboard pawns(allPawns &Attacks::file_mask[file - 1]);
                     if (pawns.isClear()) break;             // no pawns on file
                     Square pawn;
                     int wFilePawns = 0, bFilePawns = 0;
                     while((pawn = pawns.firstOne()) != InvalidSquare) {
                        if (PieceColor(board[pawn]) == White) {
                           if (bFilePawns) break;
                           wFilePawns++;
                        }
                        else {
                           bFilePawns++;

                           // check for another potential passer that
                           // shares the same blocker(s) as this one
                           int aDup = diff == 1 ? potentialMinus.isSet(pawn) : potentialPlus.isSet(pawn);
                           if (aDup) {
                              dup = pawn;
                           }
                        }

                        pawns.clear(pawn);
                     }

                     wPawns += wFilePawns;
                     bPawns += bFilePawns;
                     if (wPawns > bPawns + 1) break;
                  }

                  potential = bPawns >= wPawns;
               }
            }
         }

         if (potential) {
#ifdef PAWN_DEBUG
            cout << " potential passer";
#endif
            if (diff > 0) {
               potentialPlus.set(sq);
            }
            else {
               potentialMinus.set(sq);
            }

            if (dup != InvalidSquare) {
#ifdef PAWN_DEBUG
               cout << " (dup)";
#endif

               int rankdup = Rank(dup, side);

               // Two potential passers share the same blocker(s).
               // Score according to the most advanced one.
               if (rank > rankdup) {
                  entr.midgame_score += (POTENTIAL_PASSER[Midgame][rank] - POTENTIAL_PASSER[Midgame][rankdup]);
                  entr.endgame_score += (POTENTIAL_PASSER[Endgame][rank] - POTENTIAL_PASSER[Endgame][rankdup]);
               }
            }
            else {
               entr.midgame_score += POTENTIAL_PASSER[Midgame][rank];
               entr.endgame_score += POTENTIAL_PASSER[Endgame][rank];
            }
         }
      }

      int duo = 0;
      if (rank > 3 && file != 8 && board[sq + 1] == our_pawn)
      {
#ifdef PAWN_DEBUG
         cout << " duo";
#endif
         duo++;
      }

      if (!passed && rank >= 4) {
         int space = SPACE * (rank - 3);
         if (duo) space *= 2;
         score += space;
#ifdef PAWN_DEBUG
         cout << " space=" << space;
#endif
      }
#ifdef PAWN_DEBUG
      cout << " total = (" <<
         entr.midgame_score - mid_tmp + score - tmp << ", " <<
         entr.endgame_score - end_tmp + score - tmp << ")" <<
         endl;
#endif
   }

   Bitboard passers2(entr.passers);
   while(passers2.iterate(sq)) {
      if (TEST_MASK(connected_passers[sq][side], entr.passers)) {
         entr.midgame_score += CONNECTED_PASSERS[Midgame][Rank(sq, side)];
         entr.endgame_score += CONNECTED_PASSERS[Endgame][Rank(sq, side)];
#ifdef PAWN_DEBUG
         cout << "connected passer score, " << SquareImage(sq) << " (";
         cout << ColorImage(side);
         cout << ") : (" << CONNECTED_PASSERS[Midgame][Rank(sq, side)] << ", " << CONNECTED_PASSERS[Endgame][Rank(sq,
                                                                                                                  side)];
         cout << ")" << endl;
#endif
      }
   }

   Bitboard centerCalc(center & board.pawn_bits[side]);
#ifdef PAWN_DEBUG
   int tmp = entr.midgame_score;
#endif
   entr.midgame_score += PAWN_CENTER_SCORE * centerCalc.bitCount();

   Bitboard centerCalc2(center & board.allPawnAttacks(side) &~centerCalc);
   entr.midgame_score += PAWN_CENTER_SCORE * centerCalc2.bitCount();
#ifdef PAWN_DEBUG
   cout << "pawn center score (" << ColorImage(side) << ") :" << entr.midgame_score - tmp << endl;
#endif
   entr.midgame_score += score;
   entr.endgame_score += score;
#ifdef PAWN_DEBUG
   cout <<
      "pawn score (" <<
      ColorImage(side) <<
      ") = (" <<
      entr.midgame_score <<
      ", " <<
      entr.endgame_score <<
      ')' <<
      endl;
#endif
   return score;
}



void Scoring::evalOutsidePassers(const Board &board, 
                                 PawnHashEntry &pawnEntry) {
   byte all_pawns = pawnEntry.wPawnData.pawn_file_mask | pawnEntry.bPawnData.pawn_file_mask;
   pawnEntry.wPawnData.outside = is_outside[pawnEntry.wPawnData.passer_file_mask][all_pawns];
   pawnEntry.bPawnData.outside = is_outside[pawnEntry.bPawnData.passer_file_mask][all_pawns];
   return;
}

void Scoring::calcPawnEntry(const Board &board, PawnHashEntry &pawnEntry) {

   // pawn position not found, calculate the data we need
   calcPawnData(board, White, pawnEntry.wPawnData);
   calcPawnData(board, Black, pawnEntry.bPawnData);
   evalOutsidePassers(board, pawnEntry);
   pawnEntry.hc = board.pawnHash();
}

int Scoring::evalu8(const Board &board) {
    const ColorType side = board.sideToMove();
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    const int mdiff =  (int)(ourmat.value() - oppmat.value());
    const int our_pieces = ourmat.pieceBits();
    const int opp_pieces = oppmat.pieceBits();
    // check for bishop endgame - drawish
    int adjust = 0;
    if (our_pieces == Material::KB && opp_pieces == Material::KB) {
        // Bishop endgame: drawish
        if (Util::Abs(ourmat.pawnCount() - oppmat.pawnCount()) < 4)
            adjust -= mdiff/4;
    }
    if (ourmat.pieceBits() != oppmat.pieceBits()) {
        if (ourmat.noPawns() && oppmat.noPawns()) {
            adjust += adjustMaterialScoreNoPawns(board,side) -
                adjustMaterialScoreNoPawns(board,OppositeColor(side));
        }
        else {
            adjust += adjustMaterialScore(board,side) -
                adjustMaterialScore(board,OppositeColor(side));
        }
    }
#ifdef EVAL_DEBUG
    if (adjust) cout << "material score adjustment = " << adjust << endl;
#endif
    const int matScore = mdiff + adjust;
#ifdef EVAL_DEBUG
    cout << "adjusted material score = " << matScore << endl;
#endif

   const hash_t pawnHash = board.pawnHashCodeW ^ board.pawnHashCodeB;

   PawnHashEntry &pawnEntry = pawnHashTable[pawnHash % PAWN_HASH_SIZE];

   if (pawnEntry.hc != pawnHash) {
      // Not found in table, need to calculate
      calcPawnEntry(board, pawnEntry);
   }

   Scores wScores, bScores;

   // compute positional scores
   positionalScore<White> (board, pawnEntry, wScores, bScores);
   positionalScore<Black> (board, pawnEntry, bScores, wScores);

   const int w_materialLevel = board.getMaterial(White).materialLevel();
   const int b_materialLevel = board.getMaterial(Black).materialLevel();

   // Endgame scoring
   if (w_materialLevel <= ENDGAME_MATERIAL_THRESHOLD || b_materialLevel <= ENDGAME_MATERIAL_THRESHOLD) {
      hash_t hc = BoardHash::kingPawnHash(board);
      EndgameHashEntry *endgameEntry = &endgameHashTable[hc % ENDGAME_HASH_SIZE];
      if (endgameEntry->hc != hc) {
         calcEndgame(board, pawnEntry, endgameEntry);
         endgameEntry->hc = hc;
      }

      if (w_materialLevel <= ENDGAME_MATERIAL_THRESHOLD)
      {
#ifdef EVAL_DEBUG
         int tmp = wScores.end;
#endif
         scoreEndgame(board, endgameEntry, pawnEntry.wPawnData, White, wScores);
#ifdef EVAL_DEBUG
         cout << "endgame score (White)=" << wScores.end - tmp << endl;
#endif
      }

      if (b_materialLevel <= ENDGAME_MATERIAL_THRESHOLD)
      {
#ifdef EVAL_DEBUG
         int tmp = bScores.end;
#endif
         scoreEndgame(board, endgameEntry, pawnEntry.bPawnData, Black, bScores);
#ifdef EVAL_DEBUG
         cout << "endgame score (Black)=" << bScores.end - tmp << endl;
#endif
      }
   }

#ifdef EVAL_DEBUG
   cout << "White scores: " << endl;
   cout << " general: " << wScores.any << endl;
   cout << " midgame: " << wScores.mid << endl;
   cout << " endgame: " << wScores.end << endl;
   cout << " blend: " << wScores.blend(b_materialLevel) << endl;
   cout << "Black scores: " << endl;
   cout << " general: " << bScores.any << endl;
   cout << " midgame: " << bScores.mid << endl;
   cout << " endgame: " << bScores.end << endl;
   cout << " blend: " << bScores.blend(w_materialLevel) << endl;
#endif

   // scale scores by game phase
   int score = wScores.blend(b_materialLevel) - bScores.blend(w_materialLevel);

   if (options.search.strength < 100) {
      // "flatten" positional score values
      score = score * Util::Max(10, options.search.strength) / 100;
      if (options.search.strength <= 75) {
         // somewhat randomize the scores
         int max = int(PAWN_VALUE * (100 - 1.33F * options.search.strength) / 100) + 
             Util::Max(0,(50-options.search.strength)*PAWN_VALUE/50) +
             Util::Max(0,(25-options.search.strength)*PAWN_VALUE/25);
         if ((rand() % 100) < (50 - options.search.strength)) {
            max *= 2;
         }
         if (max) score += -max / 2 + (rand() % max);
      }
   }

   // add material score, which is from the perspective of the side to move
   if (side == White)
      score += matScore;
   else
      score -= matScore;

   if (board.sideToMove() == Black) {
      score = -score;
   }

   // Because positional scoring is inexact anyway, round the scores
   // so we will not change the selected move over a trivial difference.
   score = (score / 4) * 4;

   return score;
}

void Scoring::pawnScore(const Board &board, ColorType side, const PawnHashEntry::PawnData &pawnData, Scores &scores) {
   scores.mid += pawnData.midgame_score;
   scores.end += pawnData.endgame_score;

   if (board.rook_bits[OppositeColor(side)] | board.queen_bits[OppositeColor(side)]) {
      scores.mid += (int) pawnData.weakopen * PARAM(WEAK_ON_OPEN_FILE_MIDGAME);
      scores.end += (int) pawnData.weakopen * PARAM(WEAK_ON_OPEN_FILE_ENDGAME);
   }

   // interaction of pawns and pieces
   if (side == White) {
      if (board[D2] == WhitePawn && board[D3] > WhitePawn && board[D3] < BlackPawn) {
         scores.mid += CENTER_PAWN_BLOCK;
      }

      if (board[E2] == WhitePawn && board[E3] > WhitePawn && board[E3] < BlackPawn) {
         scores.mid += CENTER_PAWN_BLOCK;
      }
   }
   else {
      if (board[D7] == BlackPawn && board[D6] > BlackPawn) {
         scores.mid += CENTER_PAWN_BLOCK;
      }

      if (board[E7] == BlackPawn && board[E6] > BlackPawn) {
         scores.mid += CENTER_PAWN_BLOCK;
      }
   }

   ColorType oside = OppositeColor(side);
   Square sq;
   Bitboard passers2(pawnData.passers);
   while(passers2.iterate(sq)) {
      ASSERT(OnBoard(sq));

      int rank = Rank(sq, side);
      int file = File(sq);
      Square sq2;
      if (side == White)
         sq2 = sq + 8;
      else
         sq2 = sq - 8;
      if (!IsEmptyPiece(board[sq2]) && PieceColor(board[sq2]) == oside) {
         scores.mid -= PASSED_PAWN_BLOCK[Midgame][rank];
         scores.end -= PASSED_PAWN_BLOCK[Endgame][rank];
#ifdef PAWN_DEBUG
         cout <<
            ColorImage(side) <<
            " passed pawn on " <<
            SquareImage(sq) <<
            " blocked, score= (" <<
            -PASSED_PAWN_BLOCK[Midgame][rank] <<
            ", " <<
            -PASSED_PAWN_BLOCK[Endgame][rank] <<
            ")" <<
            endl;
#endif
      }

      Bitboard atcks(board.fileAttacks(sq));
      if (atcks) {
         if (side == White) {
            if (atcks & Attacks::file_mask_up[sq] & board.occupied[White]) {
               scores.mid += (Rank(sq, White) - 1) * PASSER_OWN_PIECE_BLOCK[Midgame];
               scores.end += (Rank(sq, White) - 1) * PASSER_OWN_PIECE_BLOCK[Endgame];
            }
         }
         else {
            if (atcks & Attacks::file_mask_down[sq] & board.occupied[Black]) {
               scores.mid += (Rank(sq, Black) - 1) * PASSER_OWN_PIECE_BLOCK[Midgame];
               scores.end += (Rank(sq, Black) - 1) * PASSER_OWN_PIECE_BLOCK[Endgame];
            }
         }
      }

      if (TEST_MASK(board.rook_bits[side], Attacks::file_mask[file - 1])) {
         atcks &= board.rook_bits[side];
         if (atcks) {
            if (side == White) {
               if (atcks & Attacks::file_mask_down[sq]) {
                  scores.mid += ROOK_BEHIND_PP[Midgame];
                  scores.end += ROOK_BEHIND_PP[Endgame];
               }
            }
            else {
               if (atcks & Attacks::file_mask_up[sq]) {
                  scores.mid += ROOK_BEHIND_PP[Midgame];
                  scores.end += ROOK_BEHIND_PP[Endgame];
               }
            }
         }

         // Rook adjacent to pawn on 7th is good too
         if (rank == 7 && file < 8 && TEST_MASK(board.rook_bits[side], Attacks::file_mask[file])) {
            Bitboard atcks(board.fileAttacks(sq + 1) & board.rook_bits[side]);
            if (!atcks.isClear() || board.rook_bits[side].isSet(sq + 1)) {
               scores.mid += ROOK_BEHIND_PP[Midgame];
               scores.end += ROOK_BEHIND_PP[Endgame];
            }
         }

         if (rank == 7 && file > 1 && TEST_MASK(board.rook_bits[side], Attacks::file_mask[file - 2])) {
            Bitboard atcks(board.fileAttacks(sq - 1) & board.rook_bits[side]);
            if (!atcks.isClear() || board.rook_bits[side].isSet(sq - 1)) {
               scores.mid += ROOK_BEHIND_PP[Midgame];
               scores.end += ROOK_BEHIND_PP[Endgame];
            }
         }
      }
   }

#ifdef PAWN_DEBUG
   cout << ColorImage(side) << " pawn score: " << endl;
   cout << " general = " << scores.any << endl;
   cout << " midgame = " << scores.mid << endl;
   cout << " endgame = " << scores.end << endl;
   cout << " blend = " << scores.blend(board.getMaterial(OppositeColor(side)).materialLevel()) << endl;
#endif
}

void Scoring::scoreEndgame
            (
               const Board &board,
               EndgameHashEntry *endgameEntry,
               const PawnHashEntry::PawnData &pawnData,
               ColorType side,
               Scores &scores
            ) {
   const ColorType oside = OppositeColor(side);

   const Material &ourMaterial = board.getMaterial(side);
   const Material &oppMaterial = board.getMaterial(oside);

   if (oppMaterial.kingOnly()) {
      if (ourMaterial.infobits() == Material::KBN) {
         // KBNK endgame, special case.
         Square oppkp = board.kingSquare(oside);
         Bitboard bishops(board.bishop_bits[side]);
         Square sq = bishops.firstOne();

         // try to force king to a corner where mate is possible.
         if (SquareColor(sq) == Black) {
            scores.end += KBNKScores[Flip[oppkp]];
         }
         else {
            scores.end += KBNKScores[oppkp];
         }

         // keep the kings close
         scores.end += 32 - 8*distance1(board.kingSquare(White), board.kingSquare(Black));
         return;
      } else if (ourMaterial.infobits() == Material::KR) {
         int kingDistance = distance1(board.kingSquare(side),board.kingSquare(oside));
         // keep the kings close
         scores.end += 32 - 8*kingDistance;
         Square oppkp = board.kingSquare(oside);
         // drive opposing king to the edge
         scores.end -= KRScores[oppkp];
         Square rookSq = board.rook_bits[side].firstOne();
         int krank = Rank(oppkp,White);
         int rrank = Rank(rookSq,White);
         // Place the Rook so as to restrict the opposing King
         if (krank >= 4) {
             if (rrank == krank - 1) scores.end += 10+(4-krank);
         } else {
             if (rrank == krank + 1) scores.end += 10+(krank-4);
         }
         int kfile = File(oppkp);
         int rfile = File(rookSq);
         if (kfile >= 4) {
             if (rfile == kfile-1) scores.end += 10+(4-kfile);
         } else {
             if (rfile == kfile+1) scores.end += 10+(kfile-4);
         }
         return;
      } else if (ourMaterial.infobits() == Material::KQ) {
         int kingDistance = distance1(board.kingSquare(side),board.kingSquare(oside));
         // keep the kings close
         scores.end += 32 - 8*kingDistance;
         Square oppkp = board.kingSquare(oside);
         Square ourkp = board.kingSquare(side);
         int krank = Rank(oppkp,White);
         int kfile = File(oppkp);
         if (InCorner(oppkp) && kingDistance == 2) {
             scores.end += 10;
         } else if (OnEdge(oppkp)) {
             // position King appropriately
             if (kfile == AFILE) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp + 1)) scores.end += 10;
             } else if (kfile == HFILE) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp - 1)) scores.end += 10;
             } else if (krank == 1) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp+8)) scores.end += 10;
             } else if (krank == 8) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp-8)) scores.end += 10;
             }
         }
         // drive opposing king to the edge
         scores.end -= KRScores[oppkp];
         return;
      }
   }

   if (side == White) {
      scores.end += endgameEntry->wScore +
         (int) endgameEntry->white_endgame_pawn_proximity +
         (int) endgameEntry->white_king_position;
   }
   else {
      scores.end += endgameEntry->bScore +
         (int) endgameEntry->black_endgame_pawn_proximity +
         (int) endgameEntry->black_king_position;
   }

   // King/Pawn interactions
   int uncatchables = (side == White) ? (int) endgameEntry->w_uncatchable : (int) endgameEntry->b_uncatchable;
   Bitboard passers2(pawnData.passers);
   Square passer;
   while(passers2.iterate(passer)) {

      // Encourage escorting a passer with the King, ideally with King in
      // front
      if
      (
         Util::Abs(File(board.kingSquare(side)) - File(passer)) <= 1
      && Rank(board.kingSquare(side), side) >= Rank(passer, side)
      ) scores.end += KING_AHEAD_OF_PAWN;
   }

   if (uncatchables) {
      if ((ourMaterial.infobits() == Material::KP) && oppMaterial.kingOnly()) {
         scores.end += BITBASE_WIN;
      }
   }

   Bitboard rooks(board.rook_bits[side]);
   Square rooksq;
   while(rooks.iterate(rooksq)) {
      if (TEST_MASK(board.pawn_bits[side], Attacks::rank_mask[Rank(rooksq, White) - 1])) {
         Bitboard atcks(board.rankAttacks(rooksq) & board.pawn_bits[side]);
         Square pawnsq;
         int i = 0;
         while(atcks.iterate(pawnsq)) {
            if (!TEST_MASK(Attacks::pawn_attacks[pawnsq][side], board.pawn_bits[side])) // not protected by pawn
               scores.end += SIDE_PROTECTED_PAWN * (i + 1);
            ++i;
         }
      }
   }
}

Scoring::PawnHashEntry & Scoring::pawnEntry (const Board &board) {
   hash_t pawnHash = board.pawnHashCodeW ^ board.pawnHashCodeB;

   PawnHashEntry &pawnEntry = pawnHashTable[pawnHash % PAWN_HASH_SIZE];
   if (pawnEntry.hc != pawnHash) {
      calcPawnEntry(board, pawnEntry);
   }
   return pawnEntry;
}

template<ColorType side>
void Scoring::positionalScore(const Board &board, const PawnHashEntry &pawnEntry, Scores &scores, Scores &oppScores) {
   const ColorType oside = OppositeColor(side);

#ifdef EVAL_DEBUG
   cout << "scores for " << ColorImage(side) << ":" << endl;
#endif
   pawnScore(board, side, pawnEntry.pawnData(side), scores);

   // outside passed pawn scoring, based on cached pawn data
   if (pawnEntry.pawnData(side).outside && !pawnEntry.pawnData(oside).outside) {
      scores.end += OUTSIDE_PP[Endgame];
      scores.mid += OUTSIDE_PP[Midgame];
   }

   // Penalize loss of castling.
   int castling = CASTLING_SCORES[(int) board.castleStatus(side)];
   scores.mid += castling;
#ifdef EVAL_DEBUG
   cout << "castling score: " << castling << endl;
#endif
   for(int i = 0; i < 4; i++) {
      if
      (
         BISHOP_TRAP_PATTERN[side][i].bishopMask & board.bishop_bits[side]
      && BISHOP_TRAP_PATTERN[side][i].pawnMask & board.pawn_bits[oside]
      )
      {
#ifdef EVAL_DEBUG
         cout << "bishop trapped" << endl;
#endif
         scores.any += BISHOP_TRAPPED;
      }
   }

   // calculate penalties for damaged king cover
   int ourCover, oppCover;
   if (side == White) {
      ourCover = kingCover<White> (board);
      oppCover = kingCover<Black> (board);
   }
   else {
      ourCover = kingCover<Black> (board);
      oppCover = kingCover<White> (board);
   }

   scores.mid += ourCover - KING_OFF_BACK_RANK[Rank(board.kingSquare(side), side)];
   pieceScore<side> (board, pawnEntry.pawnData(side), 
                     pawnEntry.pawnData(oside), oppCover, scores, oppScores, 
                     board.getMaterial(side).materialLevel() < MIDGAME_MATERIAL_THRESHOLD);
}

template<ColorType side>
static int KBPDraw(const Board &board) {
   Square qsq = InvalidSquare;
   int pfile;
   if ((board.pawn_bits[side] & Attacks::file_mask[0]) == board.pawn_bits[side]) {
      qsq = MakeSquare(AFILE, 8, side);
      pfile = AFILE;
   }
   else if ((board.pawn_bits[side] & Attacks::file_mask[7]) == board.pawn_bits[side]) {
      qsq = MakeSquare(HFILE, 8, side);
      pfile = HFILE;
   }

   if (qsq != InvalidSquare) {

      // pawn(s) only on Rook file
      if (!TEST_MASK(board.bishop_bits[side], SquareColor(qsq) == White ? Board::white_squares : Board::black_squares)) {

         // wrong color Bishop
         Square okp = board.kingSquare(OppositeColor(side));
         Square kp = board.kingSquare(side);
         int pawn = (side == White) ? board.pawn_bits[side].lastOne() : board.pawn_bits[side].firstOne();
         ASSERT(pawn != InvalidSquare);

         // This does not cover all drawing cases but many of them:
         if (Attacks::king_attacks[qsq].isSet(okp)
             || (Util::Abs(File(okp) - pfile) <= 1 && 
                 Rank(okp, side) > Rank(pawn, side) &&
                 Rank(okp, side) > Rank(kp,side))) {
            return 1;   // draw
         }
      }
   }

   return 0;
}

int Scoring::theoreticalDraw(const Board &board) {
   const Material &mat1 = board.getMaterial(White);
   const Material &mat2 = board.getMaterial(Black);
   if (mat1.value() > KING_VALUE + (KNIGHT_VALUE * 2) || mat2.value() > KING_VALUE + (KNIGHT_VALUE * 2)) return 0;

   // Check for K + P vs rook pawn
   if (mat1.infobits() == Material::KP && mat2.kingOnly()) {
      if (TEST_MASK(rook_pawn_mask, board.pawn_bits[White])) {
         Square kp = board.kingSquare(White);
         Square kp2 = board.kingSquare(Black);
         Square psq = (board.pawn_bits[White].firstOne());
         return lookupBitbase(kp, psq, kp2, White, board.sideToMove()) == 0;
      }
   }
   else if (mat2.infobits() == Material::KP && mat1.kingOnly()) {
      if (TEST_MASK(rook_pawn_mask, board.pawn_bits[Black])) {
         Square kp = board.kingSquare(Black);
         Square kp2 = board.kingSquare(White);
         Square psq = (board.pawn_bits[Black].firstOne());
         return lookupBitbase(kp, psq, kp2, Black, board.sideToMove()) == 0;
      }
   }

   // Check for wrong bishop + rook pawn(s) vs king.  Not very common but
   // we don't want to get it wrong.
   else if (mat1.infobits() == Material::KBP && mat2.kingOnly()) {
      return KBPDraw<White> (board);
   }
   else if (mat2.infobits() == Material::KBP && mat1.kingOnly()) {
      return KBPDraw<Black> (board);
   }

   // check for KNN vs K
   if
   (
      ((unsigned) mat1.infobits() == Material::KNN && mat2.kingOnly())
   || ((unsigned) mat2.infobits() == Material::KNN && mat1.kingOnly())
   ) return 1;

   return 0;
}

int Scoring::repetitionDraw(const Board &board) {
   return board.repCount() >= 2;
}

int Scoring::isLegalDraw(const Board &board) {
   return repetitionDraw(board) || board.materialDraw();
}

int Scoring::isDraw(const Board &board) {
   return(repetitionDraw(board) || board.materialDraw() || theoreticalDraw(board));
}

int Scoring::isDraw(const Board &board, int &rep_count, int ply) {

   // First check for draw by repetition
   int entries = board.state.moveCount - 2;
   rep_count = 0;

   // follow rule Crafty uses: 2 repeats if ply<=2, 1 otherwise:
   if (entries >= 0) {
      const int target = (ply <= 2) ? 2 : 1;
      for(hash_t * rep_list = board.repListHead - 3; entries >= 0; rep_list -= 2, entries -= 2) {
         if (*rep_list == board.hashCode()) {
            rep_count++;
            if (rep_count >= target) {
               return 1;
            }
         }
      }
   }

   // check the 50 move rule
   if (board.state.moveCount >= 100) {
      if (board.checkStatus() == InCheck) {

         // must verify side to move is not checkmated
         MoveGenerator mg(board);
         Move moves[Constants::MaxMoves];
         return(mg.generateAllMoves(moves, 0) > 0);
      }
      else {
         return 1;
      }
   }

   if
   (
      board.getMaterial(White).value() <= KING_VALUE + (KNIGHT_VALUE * 2)
   && board.getMaterial(Black).value() <= KING_VALUE + (KNIGHT_VALUE * 2)
   ) {

      // check for insufficient material and other drawing situations
      return Scoring::isDraw(board);
   }
   else
      return 0;
}

void Scoring::printScore(int score, ostream &str) {
   if (score <= -Constants::MATE_RANGE) {
      str << "-Mate" << (Constants::MATE + score + 1) / 2;
   }
   else if (score >= Constants::MATE_RANGE) {
      str << "+Mate" << (Constants::MATE - score + 1) / 2;
   }
   else {
      if (score >= 0) str << '+';
      str << fixed << setprecision(2) << (score * 1.0) / 100.0;
   }
}

void Scoring::printScoreUCI(int score, ostream &str) {
   if (score <= -Constants::MATE_RANGE) {
      str << "mate " << -(Constants::MATE + score + 1) / 2;
   }
   else if (score >= Constants::MATE_RANGE) {
      str << "mate " << (Constants::MATE - score + 1) / 2;
   }
   else
      str << "cp " << score;
}

int Scoring::tryBitbase(const Board &board) {
   const Material &wMat = board.getMaterial(White);
   const Material &bMat = board.getMaterial(Black);
   int score = 0;
   if ((unsigned) wMat.infobits() == Material::KP && bMat.kingOnly()) {
      Square passer = board.pawn_bits[White].firstOne();
      if (lookupBitbase(board.kingSquare(White), passer, board.kingSquare(Black), White, board.sideToMove())) {
         score += BITBASE_WIN;
      }
      else {
         return 0;
      }
   }
   else if ((unsigned) bMat.infobits() == Material::KP && wMat.kingOnly()) {
      Square passer = board.pawn_bits[Black].firstOne();
      if (lookupBitbase(board.kingSquare(Black), passer, board.kingSquare(White), Black, board.sideToMove())) {
         score -= BITBASE_WIN;
      }
      else {
         return 0;
      }
   }

   return INVALID_SCORE;
}

void Scoring::calcEndgame(const Board &board, 
                          const PawnHashEntry &pawnEntry,
                          EndgameHashEntry *endgameEntry) {

   // calculate & cache endgame score based on interaction of king & pawns
   const PawnHashEntry::PawnData & wPawnData = pawnEntry.wPawnData;
   const PawnHashEntry::PawnData & bPawnData = pawnEntry.bPawnData;
   endgameEntry->wScore = endgameEntry->bScore = 0;
   endgameEntry->white_endgame_pawn_proximity = endgameEntry->black_endgame_pawn_proximity = (byte) 0;
   endgameEntry->w_uncatchable = endgameEntry->b_uncatchable = (byte) 0;

   Bitboard all_pawns(board.pawn_bits[White] | board.pawn_bits[Black]);
   if (!all_pawns) return;

   int k_pos = 0;
   Square kp = board.kingSquare(White);

   // Evaluate king position. Big bonus for centralizing king or (if
   // pawns are all on one side) being near pawns.
   // Similar to how Crafty does it.
   k_pos = KingEndgameScores[kp];
   if (!TEST_MASK(abcd_mask, all_pawns)) {
      if (File(kp) > DFILE)
         k_pos += PAWN_SIDE_BONUS;
      else
         k_pos -= PAWN_SIDE_BONUS;
   }
   else if (!TEST_MASK(efgh_mask, all_pawns)) {
      if (File(kp) <= DFILE)
         k_pos += PAWN_SIDE_BONUS;
      else
         k_pos -= PAWN_SIDE_BONUS;
   }

   endgameEntry->white_king_position = k_pos;
   kp = board.kingSquare(Black);
   k_pos = KingEndgameScores[63 - kp];
   if (!TEST_MASK(abcd_mask, all_pawns)) {
      if (File(kp) > DFILE)
         k_pos += PAWN_SIDE_BONUS;
      else
         k_pos -= PAWN_SIDE_BONUS;
   }
   else if (!TEST_MASK(efgh_mask, all_pawns)) {
      if (File(kp) <= DFILE)
         k_pos += PAWN_SIDE_BONUS;
      else
         k_pos -= PAWN_SIDE_BONUS;
   }

   endgameEntry->black_king_position = k_pos;

   // bonus for king near pawns
   Bitboard it(all_pawns);
   Square sq;
   while(it.iterate(sq)) {
      endgameEntry->white_endgame_pawn_proximity += (4 - distance1(board.kingSquare(White), sq));
      endgameEntry->black_endgame_pawn_proximity += (4 - distance1(board.kingSquare(Black), sq));
   }

#ifdef EVAL_DEBUG
   cout <<
      "king/pawn proximity (endgame) = " <<
      endgameEntry->white_endgame_pawn_proximity -
      endgameEntry->black_endgame_pawn_proximity <<
      endl;
#endif

   Bitboard passers2(wPawnData.passers);
   while(passers2.iterate(sq)) {
      if (lookupBitbase(board.kingSquare(White), sq, board.kingSquare(Black), White, board.sideToMove()))
      {
#ifdef PAWN_DEBUG
         cout << "White pawn on " << SquareImage(sq) << " is uncatchable" << endl;
#endif
         endgameEntry->w_uncatchable++;
      }

      int rank = Rank(sq, White);
      int file = File(sq);
      if
      (
         rank == 6
      && (File(board.kingSquare(White)) == file - 1 || File(board.kingSquare(White)) == file + 1)
      && Rank(board.kingSquare(White), White) >= rank
      ) endgameEntry->wScore += SUPPORTED_PASSER6;
      else if
         (
            rank == 7
         && (File(board.kingSquare(White)) == file - 1 || File(board.kingSquare(White)) == file + 1)
         && Rank(board.kingSquare(White), White) >= rank
         )
         endgameEntry->wScore += SUPPORTED_PASSER7;
   }

   passers2 = bPawnData.passers;
   while(passers2.iterate(sq)) {
      if (lookupBitbase(board.kingSquare(Black), sq, board.kingSquare(White), Black, board.sideToMove()))
      {
#ifdef PAWN_DEBUG
         cout << "Black pawn on " << SquareImage(sq) << " is uncatchable" << endl;
#endif
         endgameEntry->b_uncatchable++;
      }

      int rank = Rank(sq, Black);
      int file = File(sq);
      if (rank == 6 &&
         (File(board.kingSquare(Black)) == file - 1 || File(board.kingSquare(Black)) == file + 1) &&
         Rank(board.kingSquare(Black), Black) >= rank)
         endgameEntry->bScore += SUPPORTED_PASSER6;
      else if (rank == 7 && 
               (File(board.kingSquare(Black)) == file - 1 || File(board.kingSquare(Black)) == file + 1) &&
               Rank(board.kingSquare(Black), Black) >= rank)
         endgameEntry->bScore += SUPPORTED_PASSER7;
   }
}

void Scoring::clearHashTables() {
   memset(pawnHashTable, 0xff, PAWN_HASH_SIZE * sizeof(PawnHashEntry));
   memset(endgameHashTable, '\0', ENDGAME_HASH_SIZE * sizeof(EndgameHashEntry));
   memset(kingCoverHashTable, '\0', 2 * KING_COVER_HASH_SIZE * sizeof(KingCoverHashEntry));
}
