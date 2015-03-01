// Copyright 1994-2015 by Jon Dart.  All Rights Reserved.

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
#include <ctype.h>
};
#include <iomanip>

//#define PAWN_DEBUG
//#define EVAL_DEBUG


#ifdef TUNE
#include "vparams.cpp"
#else
#include "params.cpp"
#endif

#define PARAM(x) Params::x
#define APARAM(x,index) *(&Params::x+index)

static CACHE_ALIGN Bitboard kingProximity[2][64];
static CACHE_ALIGN Bitboard kingNearProximity[64];
static CACHE_ALIGN Bitboard kingPawnProximity[2][64];

// tuned, July 2014
static const int ATTACK_FACTOR[6] = { 0, 4, 8, 8, 12, 12 };
static const int ROOK_ATTACK_BOOST = 5;
static const int QUEEN_ATTACK_BOOST1 = 4;
static const int QUEEN_ATTACK_BOOST2 = 4;

#define BOOST

// Note: the following tables are not part of Params structure (yet)
static const int KBNKScores[64] =
{
   -400, -300, -150, -50, 50, 150, 300, 400,
   -300, -150, -50, 0, -10, 50, 150, 300,
   -150, -50, 0, 0, 0, -10, 50, 150,
    -50, -50, 0, -30, -30, 0, -10, 50,
     50, -10, 0, -30, -30, 0, 0, -50,
    150, 50, -10, 0, 0, 0, -50, -150,
    300, 150, 50, -10, 0, -50, -150, -300,
    400, 300, 150, 50, -50, -150, -300, -400
};

static const int KRScores[64] =
{
   -160, -160, -160, -160, 160, -160, -160, -160,
   -160, 0, 0, 0, 0, 0, 0, -160,
   -160, 0, 80, 80, 80,  80, 0, -160,
   -160, 0, 80, 160, 160, 80, 0, -160,
   -160, 0, 80, 160, 160, 80, 0, -160,
   -160, 0,  80, 80, 80, 80, 0, -160,
   -160, 0, 0, 0, 0, 0, 0, -160,
   -160, -160, -160, -160, -160, -160, -160, -160
};

struct BishopTrapPattern
{
   Bitboard bishopMask, pawnMask;
} BISHOP_TRAP_PATTERN[2][4];

static const int BITBASE_WIN = 5000;

static Bitboard backwardW[64], backwardB[64];
CACHE_ALIGN Bitboard passedW[64], passedB[64];              // not static because needed by search module
static Bitboard outpostW[64], outpostB[64];
static const Bitboard rook_pawn_mask(Attacks::file_mask[0] | Attacks::file_mask[7]);
static Bitboard abcd_mask, efgh_mask;
static Bitboard left_side_mask[8], right_side_mask[8];
static Bitboard isolated_file_mask[8];
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

static const int DRAW_PATTERN_COUNT = 11;

static const EndgamePattern DRAW_PATTERN[] = {
                   EndgamePattern(Material::KNN, Material::K),
                   EndgamePattern(Material::KNN, Material::KR),
                   EndgamePattern(Material::KNN, Material::KB),
                   EndgamePattern(Material::KNN, Material::KN),
                   EndgamePattern(Material::KRN, Material::KR),
                   EndgamePattern(Material::KRB, Material::KR),
                   EndgamePattern(Material::KBB, Material::KB),
                   EndgamePattern(Material::KBB, Material::KR),
                   EndgamePattern(Material::KBN, Material::KR),
                   EndgamePattern(Material::KBN, Material::KN),
                   EndgamePattern(Material::KQ, Material::KRB)
};


// Don't call it "distance" - conflicts with MS template library
static inline int distance1(int x, int y) {
   return distances[x][y];
}

static inline int OnFile(const Bitboard &b, int file) {
   return TEST_MASK(b, Attacks::file_mask[file - 1]);
}

static inline int FileOpen(const Board &board, int file) {
   return !TEST_MASK((board.pawn_bits[White] | board.pawn_bits[Black]), Attacks::file_mask[file - 1]);
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

      if (file < chess::EFILE)
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

   BISHOP_TRAP_PATTERN[White][0].bishopMask.set(chess::A7);
   BISHOP_TRAP_PATTERN[White][0].bishopMask.set(chess::B8);
   BISHOP_TRAP_PATTERN[White][0].pawnMask.set(chess::B6);
   BISHOP_TRAP_PATTERN[White][0].pawnMask.set(chess::C7);

   BISHOP_TRAP_PATTERN[White][1].bishopMask.set(chess::H7);
   BISHOP_TRAP_PATTERN[White][1].bishopMask.set(chess::G8);
   BISHOP_TRAP_PATTERN[White][1].pawnMask.set(chess::G6);
   BISHOP_TRAP_PATTERN[White][1].pawnMask.set(chess::F7);

   BISHOP_TRAP_PATTERN[White][2].bishopMask.set(chess::H6);
   BISHOP_TRAP_PATTERN[White][2].bishopMask.set(chess::G7);
   BISHOP_TRAP_PATTERN[White][2].bishopMask.set(chess::H8);
   BISHOP_TRAP_PATTERN[White][2].bishopMask.set(chess::F8);
   BISHOP_TRAP_PATTERN[White][2].pawnMask.set(chess::G5);
   BISHOP_TRAP_PATTERN[White][2].pawnMask.set(chess::F6);
   BISHOP_TRAP_PATTERN[White][2].pawnMask.set(chess::E7);

   BISHOP_TRAP_PATTERN[White][3].bishopMask.set(chess::A6);
   BISHOP_TRAP_PATTERN[White][3].bishopMask.set(chess::B7);
   BISHOP_TRAP_PATTERN[White][3].bishopMask.set(chess::A8);
   BISHOP_TRAP_PATTERN[White][3].bishopMask.set(chess::C8);
   BISHOP_TRAP_PATTERN[White][3].pawnMask.set(chess::B5);
   BISHOP_TRAP_PATTERN[White][3].pawnMask.set(chess::C6);
   BISHOP_TRAP_PATTERN[White][3].pawnMask.set(chess::D7);

   BISHOP_TRAP_PATTERN[Black][0].bishopMask.set(chess::A2);
   BISHOP_TRAP_PATTERN[Black][0].bishopMask.set(chess::B1);
   BISHOP_TRAP_PATTERN[Black][0].pawnMask.set(chess::B3);
   BISHOP_TRAP_PATTERN[Black][0].pawnMask.set(chess::C2);

   BISHOP_TRAP_PATTERN[Black][1].bishopMask.set(chess::H2);
   BISHOP_TRAP_PATTERN[Black][1].bishopMask.set(chess::G1);
   BISHOP_TRAP_PATTERN[Black][1].pawnMask.set(chess::G3);
   BISHOP_TRAP_PATTERN[Black][1].pawnMask.set(chess::F2);

   BISHOP_TRAP_PATTERN[Black][2].bishopMask.set(chess::H3);
   BISHOP_TRAP_PATTERN[Black][2].bishopMask.set(chess::G2);
   BISHOP_TRAP_PATTERN[Black][2].bishopMask.set(chess::H1);
   BISHOP_TRAP_PATTERN[Black][2].bishopMask.set(chess::F1);
   BISHOP_TRAP_PATTERN[Black][2].pawnMask.set(chess::G4);
   BISHOP_TRAP_PATTERN[Black][2].pawnMask.set(chess::F3);
   BISHOP_TRAP_PATTERN[Black][2].pawnMask.set(chess::E2);

   BISHOP_TRAP_PATTERN[Black][3].bishopMask.set(chess::A3);
   BISHOP_TRAP_PATTERN[Black][3].bishopMask.set(chess::B2);
   BISHOP_TRAP_PATTERN[Black][3].bishopMask.set(chess::A1);
   BISHOP_TRAP_PATTERN[Black][3].bishopMask.set(chess::C1);
   BISHOP_TRAP_PATTERN[Black][3].pawnMask.set(chess::B4);
   BISHOP_TRAP_PATTERN[Black][3].pawnMask.set(chess::C3);
   BISHOP_TRAP_PATTERN[Black][3].pawnMask.set(chess::D2);
}

void Scoring::init() {
   initBitboards();

}

void Scoring::cleanup() {
}

Scoring::Scoring() {
   clearHashTables();
}

Scoring::~Scoring() {
}

// adjust material score when both sides have pawn(s)
static int near_draw_adjust(const Material &ourmat,
                            const Material &oppmat,
                            int pawndiff) 
{
   int score = 0;
   
   if (pawndiff == 0) {
      // stronger side's piece advantage is worth less, esp. with few pawns
      score += -int(0.6*PAWN_VALUE)+Util::Min(4,ourmat.pawnCount())*int(0.1*PAWN_VALUE);
   }
   else if (pawndiff == 1) {
      score += -int(0.4*PAWN_VALUE)+Util::Min(4,ourmat.pawnCount())*int(0.05*PAWN_VALUE);
   }
   else if (pawndiff == -1 || pawndiff == -2) {
      // add bonus for our side (thus, penalizing the other side for
      // having only 1-2 pawns advantage). Less bonus if more
      // oppponent pawns.
      score += int(0.4*PAWN_VALUE)-(4-Util::Min(4,oppmat.pawnCount()))*int(0.1*PAWN_VALUE);
   }
   return score;
             
}

int Scoring::adjustMaterialScore(const Board &board, ColorType side) const
{
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    int score = 0;
#ifdef EVAL_DEBUG
    int tmp = score;
#endif
    const int opponentPieceValue = (oppmat.value()-oppmat.pawnCount()*PAWN_VALUE);
    const int pieceDiff = ourmat.value()-ourmat.pawnCount()*PAWN_VALUE - opponentPieceValue;
    const int pawnDiff = ourmat.pawnCount() - oppmat.pawnCount();
    // If we have a material advantage but few pawns and a
    // configuration that would be a likely draw w/o pawns, move
    // the score towards a draw, and discourage trade or loss
    // of remaining pawns.
    if (ourmat.materialLevel() <= 9 && pieceDiff > 0) {
       const uint32 pieces = ourmat.pieceBits();
       if (pieces == Material::KN || pieces == Material::KB) {
          // Knight or Bishop vs pawns
          if (ourmat.pawnCount() == 0) {
             // no mating material. We can't be better but if
             // opponent has 1-2 pawns we are not so bad
             score += APARAM(KN_VS_PAWN_ADJUST0,Util::Min(2,oppmat.pawnCount()));
          } else if (oppmat.pawnCount() == 0) {
             if (pieces == Material::KN && ourmat.pawnCount() == 1) {
                // KNP vs K is a draw, generally
                score -= KNIGHT_VALUE;
             }
          }
          else {
             // both sides have pawns
             score = near_draw_adjust(ourmat,oppmat,pawnDiff);
          }
          return score;
       } else {
          EndgamePattern pattern(ourmat.pieceBits(),oppmat.pieceBits());
          for (int i = 0; i < DRAW_PATTERN_COUNT; i++) {
             if (DRAW_PATTERN[i] == pattern) {
                return near_draw_adjust(ourmat,oppmat,pawnDiff);
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
           score += APARAM(QR_ADJUST0,Util::Min(3,ourmat.minorCount()));
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
               ASSERT(ourmat.majorCount());
               score += APARAM(RB_ADJUST1,Util::Min(3,ourmat.majorCount()-1));
            }
            else if (ourmat.minorCount() == oppmat.minorCount() - 2) {
                // bad trade - Rook for two minors, but not as bad w. fewer pieces
               score -= APARAM(RBN_ADJUST1,oppmat.majorCount()-1);
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
    const int mdiff = ourmat.value() - oppmat.value();
    if (mdiff >= 3*PAWN_VALUE) {
       // Encourage trading pieces when we are ahead in material.
       if (oppmat.materialLevel() < 16) {
          score += mdiff*PARAM(TRADE_DOWN)[oppmat.materialLevel()]/4096;
       }
       // Discourage trading pawns when our own material is low (because
       // harder to win).
       if (ourmat.materialLevel() < 16 && ourmat.pawnCount() < 3) {
          score += APARAM(PAWN_TRADE0,ourmat.pawnCount());
       }
    }
    // Also give bonus for having more pawns in endgame (assuming
    // we have not traded pieces for pawns).
    if (mdiff>=0 && pieceDiff>=0 && ourmat.materialLevel() <= PARAM(ENDGAME_THRESHOLD)) {
       const int pawnDiff = ourmat.pawnCount() - oppmat.pawnCount();
       if (pawnDiff > 0) {
          score += pawnDiff*PARAM(ENDGAME_PAWN_BONUS)*(128-PARAM(MATERIAL_SCALE)[ourmat.materialLevel()])/128;
       }
    }
#ifdef EVAL_DEBUG
    if (score-tmp) {
       cout << "trade down=" << score-tmp << endl;
    }
#endif
    return score;
}


int Scoring::adjustMaterialScoreNoPawns( const Board &board, ColorType side ) const
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
            score -= QUEEN_VALUE-(ROOK_VALUE+BISHOP_VALUE)-PAWN_VALUE/4;
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
    else if (ourmat.infobits() == Material::KBB) {
       const int mdiff = ourmat.value() - oppmat.value();
       if (oppmat.infobits() == Material::KB ||
           oppmat.infobits() == Material::KR) {
          // about 85% draw
          score -= (mdiff-int(0.35*PAWN_VALUE));
       }
       else if (oppmat.infobits() == Material::KN) {
          // about 50% draw, 50% won for Bishops
          score -= int(1.5*PAWN_VALUE);
       }
    } else if (ourmat.infobits() == Material::KBN &&
               (oppmat.infobits() == Material::KN ||
                oppmat.infobits() == Material::KB ||
                oppmat.infobits() == Material::KR)) {
       // about 75% draw, a little less if opponent has Bishop
       score -= (BISHOP_VALUE - int(0.6*PAWN_VALUE) - (oppmat.hasBishop() ? int(0.15*PAWN_VALUE) : 0));
    } else if (ourmat.infobits() == Material::KNN &&
               (oppmat.infobits() == Material::KN ||
                oppmat.infobits() == Material::KB ||
                oppmat.infobits() == Material::KR)) {
       // draw
       score -= ourmat.value()-oppmat.value();
    }
    return score;
}

template<ColorType side>
int Scoring::calcCover(const Board &board, int file, int rank) {
   Square sq, pawn;
   int cover = -PARAM(KING_COVER1);
   Bitboard pawns;
   if (side == White) {
      sq = MakeSquare(file, Util::Max(1, rank - 1), White);
      pawns = Attacks::file_mask_up[sq] & board.pawn_bits[White];
      if (!pawns) {
         if (FileOpen(board, file)) cover += PARAM(KING_FILE_OPEN);
      }
      else {
         pawn = pawns.firstOne();
         cover += APARAM(KING_COVER0,Util::Min(4, Rank<side> (pawn) - rank));
         // also count if pawn is on next rank
         if (Rank(pawn,side)!=8 && pawns.isSet(pawn+8)) {
            cover += APARAM(KING_COVER0,Util::Min(4, Rank<side> (pawn) + 1 - rank));
         }
      }
   }
   else {
      sq = MakeSquare(file, Util::Max(1, rank - 1), Black);
      pawns = Attacks::file_mask_down[sq] & board.pawn_bits[Black];
      if (!pawns) {
         if (FileOpen(board, file)) cover += PARAM(KING_FILE_OPEN);
      }
      else {
         pawn = pawns.lastOne();
         cover += APARAM(KING_COVER0,Util::Min(4, Rank<side> (pawn) - rank));
         // also count if pawn is on next rank
         if (Rank(pawn,side)!=8 && pawns.isSet(pawn-8)) {
            cover += APARAM(KING_COVER0,Util::Min(4, Rank<side> (pawn) + 1 - rank));
         }
      }
   }
   
   return Util::Min(0,cover);
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
      if (kp == chess::H1) kp = chess::G1;
      if (kp == chess::A1) kp = chess::B1;
   }
   else {
      if (kp == chess::H8) kp = chess::G8;
      if (kp == chess::A8) kp = chess::B8;
   }

   int cover = calcCover<side> (board, kp);
   switch(board.castleStatus(side))
   {
   case CanCastleEitherSide:
      {
         int k_cover = calcCover<side> (board, side == White ? chess::G1 : chess::G8);
         int q_cover = calcCover<side> (board, side == White ? chess::B1 : chess::B8);
         coverEntry.cover = (cover * 2) / 3 + Util::Min(k_cover, q_cover) / 3;
         break;
      }

   case CanCastleKSide:
      {
         int k_cover = calcCover<side> (board, side == White ? chess::G1 : chess::G8);
         coverEntry.cover = (cover * 2) / 3 + k_cover / 3;
         break;
      }

   case CanCastleQSide:
      {
         int q_cover = calcCover<side> (board, side == White ? chess::B1 : chess::B8);
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
   if (oppMaterialLevel >= PARAM(MIDGAME_THRESHOLD)) {
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
         return PARAM(OUTPOST_NOT_DEFENDED) * outpost / 64;
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
               bool early_endgame,
               bool deep_endgame) {
   Bitboard b(board.occupied[side] &~board.pawn_bits[side]);
   b.clear(board.kingSquare(side));

   int pin_count = 0;
   Square kp = board.kingSquare(side);
   ColorType oside = OppositeColor(side);
   Square okp = board.kingSquare(oside);
   const Bitboard &nearKing = kingProximity[oside][okp];
   Bitboard allAttacks;
   int attackWeight = 0;
   int attackCount = 0;
   int majorAttackCount = 0;
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
            scores.mid += PARAM(KNIGHT_PST)[Midgame][scoreSq];
            scores.end += PARAM(KNIGHT_PST)[Endgame][scoreSq];

            const Bitboard &knattacks = Attacks::knight_attacks[sq];
            const int mobl = PARAM(KNIGHT_MOBILITY)[Bitboard(knattacks &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount()];
#ifdef EVAL_DEBUG
            cout << "knight moblility =" << mobl << endl;
#endif
            scores.any += mobl;
            if (PARAM(KNIGHT_OUTPOST)[scoreSq]) {
               int outpost_score = outpost<side> (board, sq, scoreSq, PARAM(KNIGHT_OUTPOST), oppPawnData);
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
            scores.mid += PARAM(BISHOP_PST)[Midgame][scoreSq];
            scores.end += PARAM(BISHOP_PST)[Endgame][scoreSq];
            
            const Bitboard battacks(board.bishopAttacks(sq));
            allAttacks |= battacks;
            if (!deep_endgame) {
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

            if (PARAM(BISHOP_OUTPOST)[scoreSq]) {
               scores.any += outpost<side> (board, sq, scoreSq, PARAM(BISHOP_OUTPOST), oppPawnData);
            }

            if (board.pinOnDiag(sq, okp, oside)) {
               pin_count++;
            }

            const int mobl = PARAM(BISHOP_MOBILITY)[Bitboard(battacks &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount()];
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
               scores.mid += PARAM(ROOK_ON_7TH_MID);
               scores.end += PARAM(ROOK_ON_7TH_END);

               Bitboard right(Attacks::rank_mask_right[r] & board.occupied[side]);
               if (right && board[right.firstOne()] == MakePiece(Rook, side)) {

                  // 2 connected rooks on 7th
                  scores.mid += PARAM(TWO_ROOKS_ON_7TH_MID);
                  scores.end += PARAM(TWO_ROOKS_ON_7TH_END);
               }
            }

            const int file = File(sq);
            const Bitboard rattacks(board.rookAttacks(sq));
            allAttacks |= rattacks;
            if (FileOpen(board, file)) {
               scores.mid += PARAM(ROOK_ON_OPEN_FILE_MID);
               scores.end += PARAM(ROOK_ON_OPEN_FILE_END);
            }

            Bitboard rattacks2(board.rookAttacks(sq, side));
            if (rattacks2 & oppPawnData.weak_pawns) {
               scores.mid += PARAM(ROOK_ATTACKS_WEAK_PAWN_MID);
               scores.end += PARAM(ROOK_ATTACKS_WEAK_PAWN_END);
            }

            const int mobl = Bitboard(rattacks2 &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount();
            // TBD: diff for mid/endgame?
            scores.any += PARAM(ROOK_MOBILITY)[mobl];
#ifdef EVAL_DEBUG
            cout << "rook mobility: " << mobl << endl;
#endif
            if (!deep_endgame) {
               Bitboard attacks(rattacks2 &nearKing);
               if (attacks) {
                  attackWeight += ATTACK_FACTOR[Rook];
                  attackCount++;
                  majorAttackCount++;
                  Bitboard attacks2(attacks &kingNearProximity[okp]);
                  if (attacks2) {
                     attacks2 &= (attacks2 - 1);
                     if (attacks2) {

                        // rook attacks at least 2 squares near king
                        attackWeight += ROOK_ATTACK_BOOST;
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
            break;
         }

      case Queen:
         {
            int qmobl = 0;

            Bitboard battacks(board.bishopAttacks(sq));
            allAttacks |= battacks;
            Bitboard qmobility(battacks);
            Bitboard kattacks;
            
            if (!deep_endgame) {
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
            if (!deep_endgame) {
               int rank = Rank(sq, side);
               if (rank > 3) {
                  Bitboard back
                           ((board.knight_bits[side] | board.bishop_bits[side]) & Attacks::rank_mask[side == White ? 0 : 7]);
                  int queenOut = (PARAM(QUEEN_OUT) - (rank - 4) / 2) * (int) back.bitCount();
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
                  attackCount++;
                  majorAttackCount++;
#ifdef EVAL_DEBUG
                  int tmp = attackWeight;
#endif
                  // bonus if Queen attacks multiple squares near King:
                  Bitboard nearAttacks(kattacks &kingNearProximity[okp]);
                  if (nearAttacks) {
                     nearAttacks &= (nearAttacks - 1);      // clear 1st bit
                     if (nearAttacks) {
                        attackWeight += QUEEN_ATTACK_BOOST1;
                        nearAttacks &= (nearAttacks - 1);   // clear 1st bit
                        if (nearAttacks) {
                           attackWeight += QUEEN_ATTACK_BOOST2;
                        }
                     }
                  }

#ifdef EVAL_DEBUG
                  if (attackWeight - tmp) cout << "queen attack boost= " << attackWeight - tmp << endl;
#endif
               }
            }

            qmobl += Bitboard(qmobility &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount();
            scores.mid += PARAM(QUEEN_MOBILITY)[Midgame][qmobl];
            scores.end += PARAM(QUEEN_MOBILITY)[Endgame][qmobl];
#ifdef EVAL_DEBUG
            cout << "queen mobility (" <<
               PARAM(QUEEN_MOBILITY)[Midgame][qmobl] << ", " <<
               PARAM(QUEEN_MOBILITY)[Endgame][qmobl] << ")" << endl;
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
      if (TEST_MASK(Board::white_squares, board.bishop_bits[side])) {
         int whitePawns = ourPawnData.w_square_pawns;
         int blackPawns = ourPawnData.b_square_pawns;
         if ((whitePawns + blackPawns > 4) && (whitePawns > blackPawns + 2))
         {
#ifdef EVAL_DEBUG
            Scores tmp = scores;
#endif
            scores.mid += PARAM(BAD_BISHOP_MID) * (whitePawns - blackPawns);
            scores.end += PARAM(BAD_BISHOP_END) * (whitePawns - blackPawns);
#ifdef EVAL_DEBUG
            cout << "bad bishop (" << ColorImage(side) << "): (" <<
               scores.mid - tmp.mid << "," << scores.end - tmp.end <<
               ")" << endl;
#endif
         }

         if (opp_pawns) {
            // penalize pawns on same color square as opposing single Bishop
            opp_scores.end += (oppPawnData.w_square_pawns * PARAM(BISHOP_PAWN_PLACEMENT_END))/ opp_pawns;
         }
      }
      else {
         int whitePawns = ourPawnData.w_square_pawns;
         int blackPawns = ourPawnData.b_square_pawns;
         if ((whitePawns + blackPawns > 4) && (blackPawns > whitePawns + 2))
         {
#ifdef EVAL_DEBUG
            Scores tmp = scores;
#endif
            scores.mid += PARAM(BAD_BISHOP_MID) * (blackPawns - whitePawns);
            scores.end += PARAM(BAD_BISHOP_END) * (blackPawns - whitePawns);
#ifdef EVAL_DEBUG
            cout << "bad bishop (" << ColorImage(side) << "): (" <<
               scores.mid - tmp.mid << "," << scores.end - tmp.end <<
               ")" << endl;
#endif
         }

         if (opp_pawns) {
            // penalize pawns on same color square as opposing single Bishop
            opp_scores.end += (oppPawnData.b_square_pawns * PARAM(BISHOP_PAWN_PLACEMENT_END))/opp_pawns;
         }
      }
   }
   else if (bishopCount >= 2) {
      // Bishop pair bonus, higher bonus in endgame
      scores.mid += PARAM(BISHOP_PAIR_MID);
      scores.end += PARAM(BISHOP_PAIR_END);
#ifdef EVAL_DEBUG
      cout << "bishop pair (" << ColorImage(side) << ")" << endl;
#endif
   }

   allAttacks |= oppPawnData.opponent_pawn_attacks;
   allAttacks |= Attacks::king_attacks[kp];
   const int squaresAttacked =  Bitboard(allAttacks & kingNearProximity[okp]).bitCount();
   if (early_endgame) {
      int mobl = Bitboard(Attacks::king_attacks[okp] & ~board.allOccupied &
                  ~allAttacks).bitCount();
      opp_scores.end += PARAM(KING_MOBILITY_ENDGAME)[mobl];
#ifdef EVAL_DEBUG
      cout << ColorImage(oside) << " king mobility: " << PARAM(KING_MOBILITY_ENDGAME)[mobl] << endl;
#endif
   }
   if (!deep_endgame) {
      // add in pawn attacks
      int proximity = Bitboard(kingPawnProximity[oside][okp] & board.pawn_bits[side]).bitCount();
      attackWeight += proximity*ATTACK_FACTOR[Pawn];
#ifdef EVAL_DEBUG
      cout << ColorImage(side) << " piece attacks on opposing king:" << endl;
      cout << " cover= " << cover << endl;
      cout << " pawn proximity=" << proximity << endl;
      cout << " attackCount=" << attackCount << endl;
      cout << " attackWeight=" << attackWeight << endl;
      cout << " squaresAttacked=" << squaresAttacked << endl;
      cout << " pin_count=" << pin_count << endl;
#endif
      if (!majorAttackCount) {
         attackCount = Util::Max(0,attackCount-1);
      }
      attackCount = Util::Min(4,attackCount);
      int attack =
         (PARAM(KING_ATTACK_PARAM1)*attackWeight/4 + 
          PARAM(KING_ATTACK_PARAM2)*attackWeight*attackCount/4 + PARAM(KING_ATTACK_PARAM3)*squaresAttacked)/16;
//      int attack = 8*KING_ATTACK_SCALE[Util::Min(attack/8, 511)];
      if (pin_count) attack += PARAM(PIN_MULTIPLIER_MID) * pin_count;

      int kattack = attack;
#ifdef BOOST
#ifdef EVAL_DEBUG
      int kattack_tmp = kattack;
#endif
      if (kattack && cover < -PARAM(KING_ATTACK_BOOST_THRESHOLD)) {
         kattack += Util::Min(kattack / 2, (-(cover + PARAM(KING_ATTACK_BOOST_THRESHOLD)) * kattack) / PARAM(KING_ATTACK_BOOST_DIVISOR));
#ifdef EVAL_DEBUG
         cout << "boost factor= " << (float) kattack / (float) kattack_tmp << endl;
#endif
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

   if (pin_count) scores.end += PARAM(PIN_MULTIPLIER_END) * pin_count;
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
            entr.midgame_score += PARAM(POTENTIAL_PASSER)[Midgame][rank];
            entr.endgame_score += PARAM(POTENTIAL_PASSER)[Endgame][rank];
            passed = 0;
         }

         entr.midgame_score += PARAM(DOUBLED_PAWNS)[Midgame][file - 1];
         entr.endgame_score += PARAM(DOUBLED_PAWNS)[Endgame][file - 1];
         if (doubles.bitCountOpt() > 1) {
            // tripled pawns
#ifdef PAWN_DEBUG
            cout << " tripled";
#endif
            entr.midgame_score += PARAM(DOUBLED_PAWNS)[Midgame][file - 1] / 2;
            entr.endgame_score += PARAM(DOUBLED_PAWNS)[Endgame][file - 1] / 2;
         }
         doubled++;
      }

      if (isolated && !passed) {
         entr.midgame_score += PARAM(ISOLATED_PAWN)[Midgame][file-1];
         entr.endgame_score += PARAM(ISOLATED_PAWN)[Endgame][file-1];
#ifdef PAWN_DEBUG
         cout << " isolated";
#endif
      }
      else if (rank < 6 && backward) {
          // Pawn is not, and cannot be, protected by a pawn of
          // the same color. See if it is also blocked from advancing
          // by adjacent pawns.
          int i = 0;
          const int limit = (rank == 2 ? 3 : 2);
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
                  entr.midgame_score += PARAM(WEAK_PAWN_MID);
                  entr.endgame_score += PARAM(WEAK_PAWN_END);
                  weak++;
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
         entr.midgame_score += PARAM(PASSED_PAWN)[Midgame][rank];
         entr.endgame_score += PARAM(PASSED_PAWN)[Endgame][rank];
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
                  entr.midgame_score += (PARAM(POTENTIAL_PASSER)[Midgame][rank] - PARAM(POTENTIAL_PASSER)[Midgame][rankdup]);
                  entr.endgame_score += (PARAM(POTENTIAL_PASSER)[Endgame][rank] - PARAM(POTENTIAL_PASSER)[Endgame][rankdup]);
               }
            }
            else {
               entr.midgame_score += PARAM(POTENTIAL_PASSER)[Midgame][rank];
               entr.endgame_score += PARAM(POTENTIAL_PASSER)[Endgame][rank];
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
         int space = PARAM(SPACE) * (rank - 3);
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

   if (entr.passers) {
      
      Bitboard passers(entr.passers);
      Scores cp_score;
      while(passers.iterate(sq)) {
         if (File(sq) != 8 && entr.passers.isSet(sq+1)) {
            cp_score.mid += PARAM(CONNECTED_PASSERS)[Midgame][Rank(sq, side)];
            cp_score.end += PARAM(CONNECTED_PASSERS)[Endgame][Rank(sq, side)];
         }
         else if (TEST_MASK(Attacks::pawn_attacks[sq][side],entr.passers)) {
            cp_score.mid += PARAM(ADJACENT_PASSERS)[Midgame][Rank(sq, side)];
            cp_score.end += PARAM(ADJACENT_PASSERS)[Endgame][Rank(sq, side)];
         }
      }
      entr.midgame_score += cp_score.mid;
      entr.endgame_score += cp_score.end;
#ifdef EVAL_DEBUG
      cout << "connected passer score, square " << SquareImage(sq);
      cout << " = (" << cp_score.mid << ", " << cp_score.end << ")" << endl;
#endif   
   }
   
   Bitboard centerCalc(Attacks::center & (board.pawn_bits[side] | board.allPawnAttacks(side)));
#ifdef PAWN_DEBUG
   int tmp = entr.midgame_score;
#endif
   entr.midgame_score += PARAM(PAWN_CENTER_SCORE_MID) * centerCalc.bitCount();
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

int Scoring::materialScore(const Board &board) const {
    const ColorType side = board.sideToMove();
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    const int mdiff =  (int)(ourmat.value() - oppmat.value());
#ifdef EVAL_DEBUG
    cout << "mdiff=" << mdiff << endl;
    
#endif
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
    return matScore;
}

      
int Scoring::evalu8(const Board &board) {
   const int matScore = materialScore(board);

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
   if (w_materialLevel <= PARAM(ENDGAME_THRESHOLD) || b_materialLevel <= PARAM(ENDGAME_THRESHOLD)) {
      hash_t hc = BoardHash::kingPawnHash(board);
      EndgameHashEntry *endgameEntry = &endgameHashTable[hc % ENDGAME_HASH_SIZE];
      if (endgameEntry->hc != hc) {
         calcEndgame(board, pawnEntry, endgameEntry);
         endgameEntry->hc = hc;
      }

      if (w_materialLevel <= PARAM(ENDGAME_THRESHOLD))
      {
#ifdef EVAL_DEBUG
         int tmp = wScores.end;
#endif
         scoreEndgame(board, endgameEntry, pawnEntry.wPawnData, White, wScores);
#ifdef EVAL_DEBUG
         cout << "endgame score (White)=" << wScores.end - tmp << endl;
#endif
      }

      if (b_materialLevel <= PARAM(ENDGAME_THRESHOLD))
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
   if (board.sideToMove() == White)
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
      scores.mid += (int) pawnData.weakopen * PARAM(WEAK_ON_OPEN_FILE_MID);
      scores.end += (int) pawnData.weakopen * PARAM(WEAK_ON_OPEN_FILE_END);
   }

   // interaction of pawns and pieces
   if (side == White) {
      if (board[chess::D2] == WhitePawn && board[chess::D3] > WhitePawn && board[chess::D3] < BlackPawn) {
         scores.mid += PARAM(CENTER_PAWN_BLOCK);
      }

      if (board[chess::E2] == WhitePawn && board[chess::E3] > WhitePawn && board[chess::E3] < BlackPawn) {
         scores.mid += PARAM(CENTER_PAWN_BLOCK);
      }
   }
   else {
      if (board[chess::D7] == BlackPawn && board[chess::D6] > BlackPawn) {
         scores.mid += PARAM(CENTER_PAWN_BLOCK);
      }

      if (board[chess::E7] == BlackPawn && board[chess::E6] > BlackPawn) {
         scores.mid += PARAM(CENTER_PAWN_BLOCK);
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
      Square blocker;
      if (side == White) {
         Bitboard blockers(Attacks::file_mask_up[sq] & board.occupied[oside]);
         sq2 = sq + 8;
         blocker = blockers.firstOne();
      }
      else {
         Bitboard blockers(Attacks::file_mask_down[sq] & board.occupied[oside]);
         sq2 = sq - 8;
         blocker = blockers.lastOne();
      }
      if (blocker != InvalidSquare) {
         // Tuned, Jan. 2015
         int mid_penalty = PARAM(PP_BLOCK_BASE_MID) + PARAM(PP_BLOCK_MULT_MID) * PARAM(PASSED_PAWN)[Midgame][rank]/256;
         int end_penalty = PARAM(PP_BLOCK_BASE_END) + PARAM(PP_BLOCK_MULT_END) * PARAM(PASSED_PAWN)[Endgame][rank]/256;
         if (blocker != sq2) {
            mid_penalty /= (Rank(blocker,side)-rank);
         }
         scores.mid -= mid_penalty;
         scores.end -= end_penalty;
#ifdef PAWN_DEBUG
         cout <<
            ColorImage(side) <<
            " passed pawn on " <<
            SquareImage(sq);
         cout << " blocked by piece on " << 
            SquareImage(blocker) << ", score= (" << -mid_penalty << ", " << 
            -end_penalty << ")" << endl;
#endif
      }

      Bitboard atcks(board.fileAttacks(sq));
      const Bitboard &mask = (side == White) ? Attacks::file_mask_up[sq] :
            Attacks::file_mask_down[sq];
      if (atcks) {
         if (atcks & mask & board.occupied[side]) {
            scores.mid += (Rank(sq, side) - 1) * PARAM(PASSER_OWN_PIECE_BLOCK_MID);
            scores.end += (Rank(sq, side) - 1) * PARAM(PASSER_OWN_PIECE_BLOCK_END);
         }
      }

      if (TEST_MASK(board.rook_bits[side], Attacks::file_mask[file - 1])) {
         atcks &= board.rook_bits[side];
         if (atcks & mask) {
            scores.mid += PARAM(ROOK_BEHIND_PP_MID);
            scores.end += PARAM(ROOK_BEHIND_PP_END);
         }
         
         // Rook adjacent to pawn on 7th is good too
         if (rank == 7 && file < 8 && TEST_MASK(board.rook_bits[side], Attacks::file_mask[file])) {
            Bitboard atcks(board.fileAttacks(sq + 1) & board.rook_bits[side]);
            if (!atcks.isClear() || board.rook_bits[side].isSet(sq + 1)) {
               scores.mid += PARAM(ROOK_BEHIND_PP_MID);
               scores.end += PARAM(ROOK_BEHIND_PP_END);
            }
         }

         if (rank == 7 && file > 1 && TEST_MASK(board.rook_bits[side], Attacks::file_mask[file - 2])) {
            Bitboard atcks(board.fileAttacks(sq - 1) & board.rook_bits[side]);
            if (!atcks.isClear() || board.rook_bits[side].isSet(sq - 1)) {
               scores.mid += PARAM(ROOK_BEHIND_PP_MID);
               scores.end += PARAM(ROOK_BEHIND_PP_END);
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

int Scoring::kingDistanceScore(const Board &board) const
{
   return PARAM(KING_DISTANCE_BASIS) - PARAM(KING_DISTANCE_MULT)*distance1(board.kingSquare(White), board.kingSquare(Black));
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
         scores.end += kingDistanceScore(board);
         return;
      } else if (ourMaterial.infobits() == Material::KR) {
         // keep the kings close
         scores.end += kingDistanceScore(board);
         Square oppkp = board.kingSquare(oside);
         // drive opposing king to the edge
         scores.end -= KRScores[oppkp];
         Square rookSq = board.rook_bits[side].firstOne();
         int krank = Rank(oppkp,White);
         int rrank = Rank(rookSq,White);
         // Place the Rook so as to restrict the opposing King
         if (krank >= 4) {
            if (rrank == krank - 1) scores.end += 10*(10+(4-krank));
         } else {
            if (rrank == krank + 1) scores.end += 10*(10+(krank-4));
         }
         int kfile = File(oppkp);
         int rfile = File(rookSq);
         if (kfile >= 4) {
            if (rfile == kfile-1) scores.end += 10*(10+(4-kfile));
         } else {
            if (rfile == kfile+1) scores.end += 10*(10+(kfile-4));
         }
         return;
      } else if (ourMaterial.infobits() == Material::KQ) {
         // keep the kings close
         scores.end += kingDistanceScore(board);
         Square oppkp = board.kingSquare(oside);
         Square ourkp = board.kingSquare(side);
         int krank = Rank(oppkp,White);
         int kfile = File(oppkp);
         if (InCorner(oppkp)) {
            const int kingDistance = distance1(board.kingSquare(White), board.kingSquare(Black));
            if (kingDistance == 2) {
               scores.end += 100;
            }
         } else if (OnEdge(oppkp)) {
             // position King appropriately
             if (kfile == chess::AFILE) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp + 1)) scores.end += 100;
             } else if (kfile == chess::HFILE) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp - 1)) scores.end += 100;
             } else if (krank == 1) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp+8)) scores.end += 100;
             } else if (krank == 8) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp-8)) scores.end += 100;
             }
         }
         // drive opposing king to the edge
         scores.end -= KRScores[oppkp];
         return;
      }
   }

   int k_pos = 0;
   if (side == White) {
#ifdef EVAL_DEBUG
      cout << "cached endgame scores (" << ColorImage(side) << "):" << endl;
      cout << " score: " << (int)endgameEntry->wScore << endl;
      cout << " king/pawn proximity: " << (int)endgameEntry->white_endgame_pawn_proximity << endl;
      cout << " king position: " << (int)endgameEntry->white_king_position << endl;
#endif
      scores.end += endgameEntry->wScore;
      k_pos = 
         (int) endgameEntry->white_endgame_pawn_proximity +
         (int) endgameEntry->white_king_position;
   }
   else {
#ifdef EVAL_DEBUG
      cout << "cached endgame scores (" << ColorImage(side) << "):" << endl;
      cout << " score: " << (int)endgameEntry->bScore << endl;
      cout << " king/pawn proximity: " << (int)endgameEntry->black_endgame_pawn_proximity << endl;
      cout << " king position: " << (int)endgameEntry->black_king_position << endl;
#endif
      scores.end += endgameEntry->bScore;
      k_pos =
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
      Square ahead = (side == White ? passer + 8 : passer - 8);
#ifdef EVAL_DEBUG
      int tmp = k_pos;
#endif
      k_pos += (8-distance1(ahead,board.kingSquare(side)))*PARAM(KING_NEAR_PASSER)/16 + (8-distance1(ahead,board.kingSquare(oside)))*PARAM(OPP_KING_NEAR_PASSER)/16;
#ifdef EVAL_DEBUG
      if (tmp - k_pos) {
         cout << "king escorting passer (" << ColorImage(side) << "): " <<
            k_pos-tmp << endl;
      }
#endif
   }

   // King position is even more important with reduced
   // material. Apply scaling here.
   const int opp_pieces = board.getMaterial(oside).pieceCount();
   if (opp_pieces < 2) {
#ifdef EVAL_DEBUG
      int tmp = k_pos;
#endif
      k_pos = ((150-10*opp_pieces)*k_pos)/128;
#ifdef EVAL_DEBUG
      if (tmp - k_pos) {
         cout << "king pos after reduced material bonus (" << ColorImage(side) << "): " <<
            k_pos-tmp << endl;
      }
#endif
   }
   scores.end += k_pos;
   
   if (uncatchables) {
      if ((ourMaterial.infobits() == Material::KP) && oppMaterial.kingOnly()) {
         scores.end += BITBASE_WIN;
#ifdef EVAL_DEBUG
         cout << "uncatchable pawn bonus (" << ColorImage(side) << "): " <<
            BITBASE_WIN << endl;
#endif
      }
   }

   Bitboard rooks(board.rook_bits[side]);
   Square rooksq;
#ifdef EVAL_DEBUG
   int tmp = scores.end;
#endif
   while(rooks.iterate(rooksq)) {
      if (TEST_MASK(board.pawn_bits[side], Attacks::rank_mask[Rank(rooksq, White) - 1])) {
         Bitboard atcks(board.rankAttacks(rooksq) & board.pawn_bits[side]);
         Square pawnsq;
         int i = 0;
         while(atcks.iterate(pawnsq)) {
            if (!TEST_MASK(Attacks::pawn_attacks[pawnsq][side], board.pawn_bits[side])) // not protected by pawn
               scores.end += PARAM(SIDE_PROTECTED_PAWN) * (i + 1);
            ++i;
         }
      }
   }
#ifdef EVAL_DEBUG
   if (scores.end - tmp) {
         cout << "side-protected pawns (" << ColorImage(side) << "): " <<
            scores.end-tmp << endl;
   }
#endif
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
      scores.end += PARAM(OUTSIDE_PASSER_MID);
      scores.mid += PARAM(OUTSIDE_PASSER_END);
   }

   // Penalize loss of castling.
   int castling = APARAM(CASTLING0,(int)board.castleStatus(side));
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
         scores.any += PARAM(BISHOP_TRAPPED);
      }
   }

   // calculate penalties for damaged king cover
   int ourCover, oppCover;
   if (side == White) {
      ourCover = kingCover<White> (board);
      oppCover = kingCover<Black> (board);
      scores.mid += ourCover + PARAM(KING_PST)[Midgame][board.kingSquare(White)];
   }
   else {
      ourCover = kingCover<Black> (board);
      oppCover = kingCover<White> (board);
      scores.mid += ourCover + PARAM(KING_PST)[Midgame][63-board.kingSquare(Black)];
   }

   pieceScore<side> (board, pawnEntry.pawnData(side), 
                     pawnEntry.pawnData(oside), oppCover, scores, oppScores, 
                     board.getMaterial(side).materialLevel() <= PARAM(ENDGAME_THRESHOLD),
                     board.getMaterial(side).materialLevel() <= PARAM(MIDGAME_THRESHOLD));
}

template<ColorType side>
static int KBPDraw(const Board &board) {
   Square qsq = InvalidSquare;
   int pfile;
   if ((board.pawn_bits[side] & Attacks::file_mask[0]) == board.pawn_bits[side]) {
      qsq = MakeSquare(chess::AFILE, 8, side);
      pfile = chess::AFILE;
   }
   else if ((board.pawn_bits[side] & Attacks::file_mask[7]) == board.pawn_bits[side]) {
      qsq = MakeSquare(chess::HFILE, 8, side);
      pfile = chess::HFILE;
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
      str << fixed << setprecision(2) << (score * 1.0) / double(PAWN_VALUE);
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
      str << "cp " << (score*100)/PAWN_VALUE;
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
   k_pos = PARAM(KING_PST)[Endgame][kp];
#ifdef EVAL_DEBUG
   cout << "king position (White): " << k_pos << endl;
#endif
   if (!TEST_MASK(abcd_mask, all_pawns)) {
      if (File(kp) > chess::DFILE)
         k_pos += PARAM(PAWN_SIDE_BONUS);
      else
         k_pos -= PARAM(PAWN_SIDE_BONUS);
   }
   else if (!TEST_MASK(efgh_mask, all_pawns)) {
      if (File(kp) <= chess::DFILE)
         k_pos += PARAM(PAWN_SIDE_BONUS);
      else
         k_pos -= PARAM(PAWN_SIDE_BONUS);
   }
#ifdef EVAL_DEBUG
   cout << "king position (White) after side bonus: " << k_pos << endl;
#endif

   endgameEntry->white_king_position = k_pos;
   kp = board.kingSquare(Black);
   k_pos = PARAM(KING_PST)[Endgame][63-kp];
#ifdef EVAL_DEBUG
   cout << "king position (Black): " << k_pos << endl;
#endif
   if (!TEST_MASK(abcd_mask, all_pawns)) {
      if (File(kp) > chess::DFILE)
         k_pos += PARAM(PAWN_SIDE_BONUS);
      else
         k_pos -= PARAM(PAWN_SIDE_BONUS);
   }
   else if (!TEST_MASK(efgh_mask, all_pawns)) {
      if (File(kp) <= chess::DFILE)
         k_pos += PARAM(PAWN_SIDE_BONUS);
      else
         k_pos -= PARAM(PAWN_SIDE_BONUS);
   }
#ifdef EVAL_DEBUG
   cout << "king position (Black) after side bonus: " << k_pos << endl;
#endif

   endgameEntry->black_king_position = k_pos;

   // bonus for king near pawns
   Bitboard it(all_pawns);
   Square sq;
   while(it.iterate(sq)) {
      endgameEntry->white_endgame_pawn_proximity += 10*(4 - distance1(board.kingSquare(White), sq));
      endgameEntry->black_endgame_pawn_proximity += 10*(4 - distance1(board.kingSquare(Black), sq));
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
         ) endgameEntry->wScore += PARAM(SUPPORTED_PASSER6);
      else if
         (
            rank == 7
         && (File(board.kingSquare(White)) == file - 1 || File(board.kingSquare(White)) == file + 1)
         && Rank(board.kingSquare(White), White) >= rank
         )
         endgameEntry->wScore += PARAM(SUPPORTED_PASSER7);
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
         endgameEntry->bScore += PARAM(SUPPORTED_PASSER6);
      else if (rank == 7 && 
               (File(board.kingSquare(Black)) == file - 1 || File(board.kingSquare(Black)) == file + 1) &&
               Rank(board.kingSquare(Black), Black) >= rank)
         endgameEntry->bScore += PARAM(SUPPORTED_PASSER7);
   }
}

void Scoring::clearHashTables() {
   memset(pawnHashTable, 0xff, PAWN_HASH_SIZE * sizeof(PawnHashEntry));
   memset(endgameHashTable, '\0', ENDGAME_HASH_SIZE * sizeof(EndgameHashEntry));
   memset(kingCoverHashTable, '\0', 2 * KING_COVER_HASH_SIZE * sizeof(KingCoverHashEntry));
}

static void print_array(ostream & o,int arr[], int size, int add_semi = 1) 
{
   o << "{";
   int *p = arr;
   for (int i = 0; i < size; i++) {
      if (i) o << ", ";
      o << *p++;
   }
   o << "}";
   if (add_semi) o << ";" << endl;
}

static void print_array(ostream & o,int mid[], int end[], int size) 
{
   o << "{";
   print_array(o,mid,size,0);
   o << ", ";
   print_array(o,end,size,0);
   o << "};" << endl;
}

static void print_pst(ostream & o,int arr[], int size, int add_semi = 1) 
{
   o << "{";
   int *p = arr;
   for (int i = 0; i < size; i++) {
      if (i) o << ",";
      o << *p++;
      if (i+1 % 8 == 0) 
         o << endl;
      else
         o << ' ';
   }
   o << "}";
   if (add_semi) o << ";" << endl;
}

static void print_pst(ostream & o,int mid[], int end[], int size) 
{
   o << "{";
   print_pst(o,mid,size,0);
   o << ", ";
   print_pst(o,end,size,0);
   o << "};" << endl;
}

#ifdef TUNE
#include "tune.h"
void Scoring::Params::write(ostream &o) 
{
   o << "// Copyright 2015 by Jon Dart. All Rights Reserved." << endl;
   o << "// This is a generated file. Do not edit." << endl;
   o << "//" << endl;
   o << endl;
   for (int i = 0; i < Scoring::Params::PARAM_ARRAY_SIZE; i++) {
      o << "const int Scoring::Params::";
      const string str(tune::tune_params[i].name);
      
      for (string::const_iterator it = str.begin();
           it != str.end();
           it++) {
         o << (char)toupper((int)*it);
      }
      o << " = " << tune::tune_params[i].current << ";" << endl;
   }
   //o << "const int Scoring::Params::params[" << Scoring::Params::PARAM_ARRAY_SIZE << "] = ";
   //print_array(o,params,Params::PARAM_ARRAY_SIZE);
   o << "const int Scoring::Params::MATERIAL_SCALE[32] = ";
   print_array(o,Params::MATERIAL_SCALE,32);
   o << "const int Scoring::Params::MIDGAME_THRESHOLD = " << Params::MIDGAME_THRESHOLD << ";" << endl;
   o << "const int Scoring::Params::ENDGAME_THRESHOLD = " << Params::ENDGAME_THRESHOLD << ";" << endl;
   o << "const int Scoring::Params::KNIGHT_PST[2][64] = " << endl;
   print_pst(o,Params::KNIGHT_PST[0],KNIGHT_PST[1],64);
   o << "const int Scoring::Params::BISHOP_PST[2][64] = " << endl;
   print_pst(o,Params::BISHOP_PST[0],BISHOP_PST[1],64);
   o << "const int Scoring::Params::KNIGHT_OUTPOST[64] = " << endl;
   print_pst(o,Params::KNIGHT_OUTPOST,64);
   o << "const int Scoring::Params::BISHOP_OUTPOST[64] = " << endl;
   print_pst(o,Params::BISHOP_OUTPOST,64);
   o << "const int Scoring::Params::KING_PST[2][64] = ";
   print_pst(o,Params::KING_PST[0],KING_PST[1],64);
   o << "const int Scoring::Params::KNIGHT_MOBILITY[9] = ";
   print_array(o,Params:: KNIGHT_MOBILITY, 9);
   o << "const int Scoring::Params::BISHOP_MOBILITY[15] = ";
   print_array(o,Params:: BISHOP_MOBILITY, 15);
   o << "const int Scoring::Params::ROOK_MOBILITY[15] = ";
   print_array(o,Params:: ROOK_MOBILITY, 15);
   o << "const int Scoring::Params::QUEEN_MOBILITY[2][29] = ";
   print_array(o,Params:: QUEEN_MOBILITY[0], QUEEN_MOBILITY[1], 29);
   o << "const int Scoring::Params::TRADE_DOWN[16] = ";
   print_array(o,Params::TRADE_DOWN,16);
   o << "const int Scoring::Params::KING_MOBILITY_ENDGAME[9] = ";
   print_array(o,Params::KING_MOBILITY_ENDGAME, 9);
   o << "const int Scoring::Params::PASSED_PAWN[2][8] = ";
   print_array(o,Params::PASSED_PAWN[0], Params::PASSED_PAWN[1], 8);
   o << "const int Scoring::Params::POTENTIAL_PASSER[2][8] = ";
   print_array(o,Params::POTENTIAL_PASSER[0], Params::POTENTIAL_PASSER[1], 8);
   o << "const int Scoring::Params::CONNECTED_PASSERS[2][8] = ";
   print_array(o,Params::CONNECTED_PASSERS[0], Params::CONNECTED_PASSERS[1], 8);
   o << "const int Scoring::Params::ADJACENT_PASSERS[2][8] = ";
   print_array(o,Params::ADJACENT_PASSERS[0], Params::ADJACENT_PASSERS[1], 8);
   o << "const int Scoring::Params::DOUBLED_PAWNS[2][8] = ";
   print_array(o,Params:: DOUBLED_PAWNS[0], Params::DOUBLED_PAWNS[1], 8);
   o << "const int Scoring::Params::ISOLATED_PAWN[2][8] = ";
   print_array(o,Params:: ISOLATED_PAWN[0], Params::ISOLATED_PAWN[1], 8);

   o << endl;
}
#endif
