// Copyright 1994-2017 by Jon Dart.  All Rights Reserved.

#include "scoring.h"
#include "bhash.h"
#include "bitprobe.h"
#include "hash.h"
#include "globals.h"
#include "material.h"
#include "movegen.h"
#ifdef TUNE
#include "tune.h"
#endif
extern "C"
{
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
};
#include <cstddef>
#include <algorithm>
#include <climits>
#include <iomanip>

//#define PAWN_DEBUG
//#define EVAL_DEBUG
//#define ATTACK_DEBUG

const int Params:: MATERIAL_SCALE[32] =
{
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 12, 24, 36, 48, 60, 72, 84, 96,
   108, 116, 120, 128, 128, 128, 128, 128, 128, 128, 128
};

const int Params::MIDGAME_THRESHOLD = 12;
const int Params::ENDGAME_THRESHOLD = 23;

#define PARAM(x) Params::x
#define APARAM(x,index) Params::x[index]

CACHE_ALIGN Bitboard Scoring::kingProximity[2][64];
CACHE_ALIGN Bitboard Scoring::kingNearProximity[64];
CACHE_ALIGN Bitboard Scoring::kingPawnProximity[2][64];

static score_t VAL(double x) { return score_t(Params::PAWN_VALUE*x); }

// Note: the following tables are not part of Params structure (yet)
static const score_t KBNKScores[64] =
{
    VAL(-0.4), VAL(-0.3), VAL(-0.15), VAL(-0.05), VAL(0.05), VAL(0.150), VAL(0.3), VAL(0.4),
    VAL(-0.3), VAL(-0.15), VAL(-0.05), VAL(0), VAL(-0.01), VAL(0.05), VAL(0.15), VAL(0.3),
    VAL(-0.15), VAL(-0.05), VAL(0), VAL(0), VAL(0), VAL(-0.01), VAL(0.05), VAL(0.15),
    VAL(-0.05), VAL(-0.05), VAL(0), VAL(-0.03), VAL(-0.03), VAL(0), VAL(-0.01), VAL(0.05),
    VAL(0.05), VAL(-0.01), VAL(0), VAL(-0.03), VAL(-0.03), VAL(0), VAL(0), VAL(-0.05),
    VAL(0.15), VAL(0.05), VAL(-0.01), VAL(0), VAL(0), VAL(0), VAL(-0.05), VAL(-0.15),
    VAL(0.3), VAL(0.15), VAL(0.05), VAL(-0.01), VAL(0), VAL(-0.05), VAL(-0.15), VAL(-0.3),
    VAL(0.4), VAL(0.3), VAL(0.15), VAL(0.05), VAL(-0.05), VAL(-0.15), VAL(-0.3), VAL(-0.4)
};

static const score_t KRScores[64] =
{
    VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16),
    VAL(-0.16), VAL(0), VAL(0), VAL(0), VAL(0), VAL(0), VAL(0), VAL(-0.16),
    VAL(-0.16), VAL(0), VAL(0.08), VAL(0.08), VAL(0.08), VAL(0.08), VAL(0), VAL(-0.16),
    VAL(-0.16), VAL(0), VAL(0.08), VAL(0.16), VAL(0.16), VAL(0.08), VAL(0), VAL(-0.16),
    VAL(-0.16), VAL(0), VAL(0.08), VAL(0.08), VAL(0.08), VAL(0.08), VAL(0), VAL(-0.16),
    VAL(-0.16), VAL(0), VAL(0.08), VAL(0.08), VAL(0.08), VAL(0.08), VAL(0), VAL(-0.16),
    VAL(-0.16), VAL(0), VAL(0.08), VAL(0.08), VAL(0.08), VAL(0.08), VAL(0), VAL(-0.16),
    VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16), VAL(-0.16)
};

struct BishopTrapPattern
{
   Bitboard bishopMask, pawnMask;
} BISHOP_TRAP_PATTERN[2][4];

static Bitboard backwardW[64], backwardB[64];
CACHE_ALIGN Bitboard passedW[64], passedB[64];              // not static because needed by search module
static CACHE_ALIGN Bitboard outpostW[64], outpostB[64];
static const Bitboard rook_pawn_mask(Attacks::file_mask[0] | Attacks::file_mask[7]);
static Bitboard left_side_mask[8], right_side_mask[8];
static Bitboard isolated_file_mask[8];
static byte is_outside[256][256];

int Scoring::distance(Square sq1, Square sq2)
{
   int rankdist = std::abs(sq1/8 - sq2/8);
   int filedist = std::abs((sq1 % 8)-(sq2 % 8));
   return std::max<int>(rankdist,filedist);
}

static inline int OnFile(const Bitboard &b, int file) {
   return TEST_MASK(b, Attacks::file_mask[file - 1]);
}

static inline int FileOpen(const Board &board, int file) {
   return !TEST_MASK((board.pawn_bits[White] | board.pawn_bits[Black]), Attacks::file_mask[file - 1]);
}

template<ColorType side> void Scoring::initProximity(Square i) {
   kingNearProximity[i] = Attacks::king_attacks[i];
   kingNearProximity[i].set(i);
   Square kp = i;
   if (File(i) == 8) kp -= 1;
   if (File(i) == 1) kp += 1;
   kingProximity[side][i] = Attacks::king_attacks[kp];
   kingProximity[side][i].set(kp);
   const int r = Rank(i, side);
   if (r <= 6) {
      kingProximity[side][i].set(MakeSquare(File(kp) - 1, r + 2, side));
      kingProximity[side][i].set(MakeSquare(File(kp), r + 2, side));
      kingProximity[side][i].set(MakeSquare(File(kp) + 1, r + 2, side));
   }
   kingPawnProximity[side][i] = kingProximity[side][i];

   Square sq;
   Bitboard tmp(kingProximity[side][i]);
   while(tmp.iterate(sq)) {
      kingPawnProximity[side][i].set(sq);
      kingPawnProximity[side][i] |= Attacks::pawn_attacks[sq][OppositeColor(side)];
   }
}

void Scoring::initBitboards() {
   int i, r;

   for(i = 0; i < 64; i++) {
      initProximity<White>(i);
      initProximity<Black>(i);
      int rank = Rank(i, White);
      int file = File(i);
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
#ifdef TUNE
   tune_params.applyParams();
#endif
}

void Scoring::cleanup() {
}

Scoring::Scoring() {
   clearHashTables();
}

Scoring::~Scoring() {
}

int Scoring::tradeDownIndex(const Material &ourmat, const Material &oppmat)
{
   int index = -1;
   const score_t mdiff = ourmat.value() - oppmat.value();
   if (oppmat.materialLevel() < 16 && mdiff >= 3*Params::PAWN_VALUE) {
      // Encourage trading pieces when we are ahead in material.
      index = oppmat.materialLevel()/2;
   }
   return index;
}

score_t Scoring::adjustMaterialScore(const Board &board, ColorType side) const
{
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    score_t score = 0;
#ifdef EVAL_DEBUG
    score_t tmp = score;
#endif
    const score_t mdiff = ourmat.value() - oppmat.value();
    const int pawnDiff = ourmat.pawnCount() - oppmat.pawnCount();
    if (ourmat.pieceBits() == Material::KB && oppmat.pieceBits() == Material::KB) {
        // Bishop endgame: drawish
       if (pawnDiff > 0 && std::abs(ourmat.pawnCount() - oppmat.pawnCount()) < 4) {
#ifdef EVAL_DEBUG
          cout << "bishop endgame adjust: " << -mdiff/4 << endl;
#endif
          score -= mdiff/4;
       }
       return score;
    }
    const score_t pieceDiff = ourmat.pieceValue() - oppmat.pieceValue();
    const uint32_t pieces = ourmat.pieceBits();
    if (pieceDiff > 0 && (pieces == Material::KN || pieces == Material::KB)) {
        // Knight or Bishop vs pawns
        if (ourmat.pawnCount() == 0) {
            // no mating material. We can't be better but if
            // opponent has 1-2 pawns we are not so bad
            score += APARAM(KN_VS_PAWN_ADJUST,std::min<int>(2,oppmat.pawnCount()));
        } else if (oppmat.pawnCount() == 0) {
            if (pieces == Material::KN && ourmat.pawnCount() == 1) {
                // KNP vs K is a draw, generally
                score -= Params::KNIGHT_VALUE;
            }
        }
        else if (pieces == Material::KB && ourmat.pawnCount() == 1) {
            int wrongBishop = (side == White) ? KBPDraw<White>(board) : KBPDraw<Black>(board);
            if (wrongBishop) {
                // We have the configuration of "wrong color bishop"
                // and rook pawn. Opponent may or may not have
                // pawns. We likely can't do better than draw, so
                // reduce score.
                score -= Params::BISHOP_VALUE;
            }
        }
#ifdef EVAL_DEBUG
        cout << "minor piece adjustment (" << ColorImage(side) << ") = " << score << endl;
#endif
        return score;
    }
    int majorDiff = ourmat.majorCount() - oppmat.majorCount();
    switch(majorDiff) {
    case 0: {
       if (ourmat.minorCount() == oppmat.minorCount()+1) {
          // we have extra minor (but not only a minor)
          if (oppmat.pieceBits() == Material::KR) {
             // KR + minor vs KR - draw w. no pawns so lower score
             if (ourmat.hasPawns()) {
                score += PARAM(KRMINOR_VS_R);
             } else {
                score += PARAM(KRMINOR_VS_R_NO_PAWNS);
             }
             // do not apply trade down or pawn bonus
             return score;
          }
          else if ((ourmat.pieceBits() == Material::KQN ||
                   ourmat.pieceBits() == Material::KQB) &&
                   oppmat.pieceBits() == Material::KQ) {
              // Q + minor vs Q is a draw, generally
             if (ourmat.hasPawns()) {
                score += PARAM(KQMINOR_VS_Q);
             } else {
                score += PARAM(KQMINOR_VS_Q_NO_PAWNS);
             }
             // do not apply trade down or pawn bonus
             return score;
          } else if (oppmat.pieceValue() > Params::ROOK_VALUE) {
             // Knight or Bishop traded for pawns. Bonus for piece.
             score += PARAM(MINOR_FOR_PAWNS);
          }
        }
        else if  (ourmat.queenCount() == oppmat.queenCount()+1 &&
             ourmat.rookCount() == oppmat.rookCount() - 2) {
            // Queen vs. Rooks
            // Queen is better with minors on board (per Kaufman)
           score += APARAM(QR_ADJUST,std::min<int>(4,ourmat.minorCount()));
        }
        break;
    }
    case 1: {
        if (ourmat.rookCount() == oppmat.rookCount()+1) {
            if (ourmat.minorCount() == oppmat.minorCount() - 1) {
                // Rook vs. minor
                // not as bad w. fewer pieces
               score += APARAM(RB_ADJUST,std::min<int>(5,ourmat.materialLevel()/2-2));
            }
            else if (ourmat.minorCount() == oppmat.minorCount() - 2) {
                // bad trade - Rook for two minors, but not as bad w. fewer pieces
               score += APARAM(RBN_ADJUST,std::min<int>(5,ourmat.materialLevel()/2-2));
            }
        }
        // Q vs RB or RN is already dealt with by piece values
        break;
    }
    default:
        break;
    }
#ifdef EVAL_DEBUG
    if (score-tmp)
       cout << "material imbalance (" << ColorImage(side) << ") = " << score-tmp << endl;
#endif
    // Encourage trading pieces (but not pawns) when we are ahead in material.
    int index = tradeDownIndex(ourmat,oppmat);
    if (index >=0) {
       ASSERT(index<8);
       score += mdiff*APARAM(TRADE_DOWN,index)/4096;
    }
    if (ourmat.materialLevel() < 16) {
#ifdef EVAL_DEBUG
       tmp = score;
#endif
       score_t adj = 0;
       if (pawnDiff > 0 && pieceDiff >= 0) {
          // better to have more pawns in endgame (if we have not
          // traded pieces for pawns).
          adj += PARAM(ENDGAME_PAWN_ADVANTAGE)*std::min<int>(2,pawnDiff);
       }
       if (pieceDiff > 0) {
          // bonus for last few pawns - to discourage trade
          const int ourp = ourmat.pawnCount();
          if (ourp >= 3) adj += PARAM(PAWN_ENDGAME1);
          if (ourp >= 2) adj += PARAM(PAWN_ENDGAME1);
          if (ourp >= 1) adj += PARAM(PAWN_ENDGAME2);
       }
       score += (4-ourmat.materialLevel()/4)*adj/4;
#ifdef EVAL_DEBUG
       if (score-tmp) {
          cout << "pawn adjust (" << ColorImage(side) << ") = " << score-tmp << endl;
       }
#endif
    }
    return score;
}


score_t Scoring::adjustMaterialScoreNoPawns( const Board &board, ColorType side ) const
{
    // pawnless endgames. Generally drawish in many cases.
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    const score_t mdiff = ourmat.value()-oppmat.value();
    score_t score = 0;
    if (ourmat.infobits() == Material::KQ) {
        if (oppmat.infobits() == Material::KRR) {
            score -= (Params::QUEEN_VALUE-2*Params::ROOK_VALUE);  // even
        }
        else if (oppmat.infobits() == Material::KRB) {
           score -= Params::QUEEN_VALUE-(Params::ROOK_VALUE+Params::BISHOP_VALUE); // even
        }
        else if (oppmat.infobits() == Material::KQB) {
            score += Params::BISHOP_VALUE;  // even
        }
        else if (oppmat.infobits() == Material::KQN) {
            score += Params::KNIGHT_VALUE;  // even
        }
        else if (oppmat.infobits() == Material::KRN) { // close to even
            score -= Params::QUEEN_VALUE-(Params::ROOK_VALUE+Params::BISHOP_VALUE)-Params::PAWN_VALUE/4;
        }
        else if (oppmat.infobits() == Material::KB ||
                 oppmat.infobits() == Material::KN ||
                 oppmat.infobits() == Material::KR) {
            // won for the stronger side
            score += 2*Params::PAWN_VALUE;
        }
    }
    else if (ourmat.infobits() == Material::KR) {
        if (oppmat.infobits() == Material::KBN) { // close to even
            score += (Params::KNIGHT_VALUE+Params::BISHOP_VALUE-Params::ROOK_VALUE)-Params::PAWN_VALUE/4;
        }
                                                  // even
        else if (oppmat.infobits() == Material::KB) {
            score -= (Params::ROOK_VALUE-Params::BISHOP_VALUE);
        }
        else if (oppmat.infobits() == Material::KN) {
            score -= (Params::ROOK_VALUE-Params::KNIGHT_VALUE)-Params::PAWN_VALUE/4;
        }
        else if (oppmat.infobits() == Material::KRB) {
           // even
           score += Params::BISHOP_VALUE;
        }
        else if (oppmat.infobits() == Material::KRN) {
           // even
           score += Params::KNIGHT_VALUE;
        }
    }
    else if (ourmat.infobits() == Material::KRR) {
        if (oppmat.infobits() == Material::KRB) {
           // even
           score -= Params::ROOK_VALUE-Params::BISHOP_VALUE;
        }
        else if (oppmat.infobits() == Material::KRN) {
           score -= Params::ROOK_VALUE-Params::KNIGHT_VALUE-Params::PAWN_VALUE/4;
        }
        else if (oppmat.infobits() == Material::KRBN) {
           score += (Params::KNIGHT_VALUE+Params::BISHOP_VALUE-Params::ROOK_VALUE)-Params::PAWN_VALUE/4;
        }
    }
    else if (ourmat.infobits() == Material::KBB) {
       if (oppmat.infobits() == Material::KB) {
          // about 85% draw
          score -= (mdiff-score_t(0.35*Params::PAWN_VALUE));
       }
       else if (oppmat.infobits() == Material::KN) {
          // about 50% draw, 50% won for Bishops
          score -= score_t(1.5*Params::PAWN_VALUE);
       }
       else if (oppmat.infobits() == Material::KR) {
          // draw
          score -= mdiff;
       }
    } else if (ourmat.infobits() == Material::KBN) {
       if (oppmat.infobits() == Material::KN ||
           oppmat.infobits() == Material::KB) {
          // about 75% draw, a little less if opponent has Bishop
          score -= (Params::BISHOP_VALUE - score_t(0.6*Params::PAWN_VALUE) - (oppmat.hasBishop() ? score_t(0.15*Params::PAWN_VALUE) : 0));
       } else if (oppmat.infobits() == Material::KR) {
          score -= mdiff;
       }
    } else if (ourmat.infobits() == Material::KNN &&
               (oppmat.infobits() == Material::KN ||
                oppmat.infobits() == Material::KB ||
                oppmat.infobits() == Material::KR)) {
       // draw
       score -= mdiff;
    }
    return score;
}

template<ColorType side>
#ifdef TUNE
score_t Scoring::calcCover(const Board &board, int file, int rank, int (&counts)[6][4])
{
#else
score_t Scoring::calcCover(const Board &board, int file, int rank) {
#endif
   Square sq, pawn;
   score_t cover = PARAM(KING_COVER_BASE);
   if (rank > 2) return cover;
   const int f = file > 4 ? 8 - file : file - 1;
   Bitboard pawns;
   sq = MakeSquare(file, 1, side);
   if (side == White) {
      pawns = Attacks::file_mask_up[sq] & board.pawn_bits[White];
      if (!pawns) {
         if (FileOpen(board, file)) {
            cover += PARAM(KING_COVER)[5][f];
#ifdef TUNE
            counts[5][f]++;
#endif
         }
         else {
            cover += PARAM(KING_COVER)[4][f];
#ifdef TUNE
            counts[4][f]++;
#endif
         }
      }
      else {
         pawn = pawns.firstOne();
         const int rank = Rank<side>(pawn);
         ASSERT(rank >= 2);
         const int rank_dist = std::min<int>(3,rank - 2);
         cover += PARAM(KING_COVER)[rank_dist][f];
#ifdef TUNE
         counts[rank_dist][f]++;
#endif
      }
   }
   else {
      pawns = Attacks::file_mask_down[sq] & board.pawn_bits[Black];
      if (!pawns) {
         if (FileOpen(board, file)) {
            cover += PARAM(KING_COVER)[5][f];
#ifdef TUNE
            counts[5][f]++;
#endif
         }
         else {
            cover += PARAM(KING_COVER)[4][f];
#ifdef TUNE
            counts[4][f]++;
#endif
         }
      }
      else {
         pawn = pawns.lastOne();
         const int rank = Rank<side>(pawn);
         ASSERT(rank >= 2);
         const int rank_dist = std::min<int>(3,rank - 2);
         cover += PARAM(KING_COVER)[rank_dist][f];
#ifdef TUNE
         counts[rank_dist][f]++;
#endif
      }
   }
   return cover;
}

// Calculate a king cover score
template<ColorType side>
#ifdef TUNE
score_t Scoring::calcCover(const Board &board, Square kp, int (&counts)[6][4]) {
#else
score_t Scoring::calcCover(const Board &board, Square kp) {
#endif
   score_t cover = 0;
   int kpfile = File(kp);
   int rank = Rank(kp, side);
   if (kpfile > 5) {
      for(int i = 6; i <= 8; i++) {
#ifdef TUNE
         cover += calcCover<side> (board, i, rank, counts);
#else
         cover += calcCover<side> (board, i, rank);
#endif
      }
   }
   else if (kpfile < 4) {
      for(int i = 1; i <= 3; i++) {
#ifdef TUNE
         cover += calcCover<side> (board, i, rank, counts);
#else
         cover += calcCover<side> (board, i, rank);
#endif
      }
   }
   else {
      for(int i = kpfile - 1; i <= kpfile + 1; i++) {
#ifdef TUNE
         cover += calcCover<side> (board, i, rank, counts);
#else
         cover += calcCover<side> (board, i, rank);
#endif
      }
   }
   cover = std::min<score_t>(0, cover);
   return cover;
}

template<ColorType side>
void Scoring::calcCover(const Board &board, KingPawnHashEntry &coverEntry) {
   Square kp = board.kingSquare(side);

#ifdef TUNE
   for (int i = 0; i < 6; i++)
      for (int j = 0; j < 4; j++)
         coverEntry.counts[i][j] = 0.0F;
#endif

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

#ifdef TUNE
   int king_cover[6][4], kside_cover[6][4], qside_cover[6][4];
   for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 4; j++) {
         king_cover[i][j] = kside_cover[i][j] = qside_cover[i][j] = 0;
      }
   }
   score_t cover = calcCover<side> (board, kp, king_cover);
#else
   score_t cover = calcCover<side> (board, kp);
#endif
   switch(board.castleStatus(side))
   {
   case CanCastleEitherSide:
      {
#ifdef TUNE
         score_t k_cover = calcCover<side> (board, side == White ? chess::G1 : chess::G8, kside_cover);
         score_t q_cover = calcCover<side> (board, side == White ? chess::B1 : chess::B8, qside_cover);
#else
         score_t k_cover = calcCover<side> (board, side == White ? chess::G1 : chess::G8);
         score_t q_cover = calcCover<side> (board, side == White ? chess::B1 : chess::B8);
#endif
         coverEntry.cover = (cover * 2) / 3 + std::min<score_t>(k_cover, q_cover) / 3;
#ifdef TUNE
         for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 4; j++) {
               coverEntry.counts[i][j] = 2*float(king_cover[i][j])/3 +
               (k_cover < q_cover ?
                float(kside_cover[i][j])/3 :
                float(qside_cover[i][j])/3);
            }
         }
#endif
         break;
      }

   case CanCastleKSide:
      {
#ifdef TUNE
         score_t k_cover = calcCover<side> (board, side == White ? chess::G1 : chess::G8, kside_cover);
#else
         score_t k_cover = calcCover<side> (board, side == White ? chess::G1 : chess::G8);
#endif
         coverEntry.cover = (cover * 2) / 3 + k_cover / 3;
#ifdef TUNE
         for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 4; j++) {
               coverEntry.counts[i][j] = 2*float(king_cover[i][j])/3 +
               float(kside_cover[i][j])/3;
            }

         }
#endif
         break;
      }

   case CanCastleQSide:
      {
#ifdef TUNE
         score_t q_cover = calcCover<side> (board, side == White ? chess::B1 : chess::B8, qside_cover);
#else
         score_t q_cover = calcCover<side> (board, side == White ? chess::B1 : chess::B8);
#endif
         coverEntry.cover = (cover * 2) / 3 + q_cover / 3;
#ifdef TUNE
         for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 4; j++) {
               coverEntry.counts[i][j] = 2*float(king_cover[i][j])/3 +
               float(qside_cover[i][j])/3;
            }
         }
#endif
         break;
      }

   default:
      coverEntry.cover = cover;
#ifdef TUNE
      for (int i = 0; i < 6; i++) {
         for (int j = 0; j < 4; j++) {
            coverEntry.counts[i][j] = float(king_cover[i][j]);
         }
      }
#endif
      break;
   }
}

score_t Scoring::outpost(const Board &board,
                     Square sq, ColorType side) const
{
   return Rank(sq,side) > 4 && ((side == White) ?
                                 !TEST_MASK(outpostW[sq], board.pawn_bits[Black]) :
                                 !TEST_MASK(outpostB[sq], board.pawn_bits[White]));

}

template <ColorType bishopColor>
void Scoring::scoreBishopAndPawns(const Board &board,ColorType ourSide,const PawnHashEntry::PawnData &ourPawnData,const PawnHashEntry::PawnData &oppPawnData,Scores &scores,Scores &opp_scores)
{
   int whitePawns = ourPawnData.w_square_pawns;
   int blackPawns = ourPawnData.b_square_pawns;
   int ourPawns, oppPawns;
   if (bishopColor == White) {
      ourPawns = whitePawns;
      oppPawns = blackPawns;
   } else {
      ourPawns = blackPawns;
      oppPawns = whitePawns;
   }
   if ((whitePawns + blackPawns > 4) && (ourPawns > oppPawns + 2))
   {
#ifdef EVAL_DEBUG
      Scores tmp = scores;
#endif
      scores.mid += PARAM(BAD_BISHOP_MID) * (ourPawns - oppPawns);
      scores.end += PARAM(BAD_BISHOP_END) * (ourPawns - oppPawns);
#ifdef EVAL_DEBUG
      cout << "bad bishop (" << ColorImage(ourSide) << "): (" <<
         scores.mid - tmp.mid << "," << scores.end - tmp.end <<
         ")" << endl;
#endif
   }
   const int totalOppPawns = oppPawnData.b_square_pawns + oppPawnData.w_square_pawns;
   if (totalOppPawns) {
      // penalize pawns on same color square as opposing single Bishop
#ifdef EVAL_DEBUG
      score_t tmp = opp_scores.end;
#endif
      opp_scores.end += ((bishopColor == White ? oppPawnData.w_square_pawns : oppPawnData.b_square_pawns) * PARAM(BISHOP_PAWN_PLACEMENT_END))/ totalOppPawns;
#ifdef EVAL_DEBUG
      cout << "Bishop pawn placement: (" << ColorImage(OppositeColor(ourSide)) << "): " << opp_scores.end - tmp << endl;
#endif
   }
}


template<ColorType side>
void Scoring::pieceScore(const Board &board,
               const PawnHashEntry::PawnData &ourPawnData,
               const PawnHashEntry::PawnData &oppPawnData,
               score_t oppCover,
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
   const Bitboard &nearKing(kingProximity[oside][okp]);
   Bitboard allAttacks, minorAttacks, rookAttacks;
   score_t attackWeight = 0;
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
            const score_t mobl = PARAM(KNIGHT_MOBILITY)[Bitboard(knattacks &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount()];
#ifdef EVAL_DEBUG
            cout << "knight moblility =" << mobl << endl;
#endif
            scores.any += mobl;
            if (outpost(board,sq,side)) {
               int defenders = outpost_defenders(board,sq,side) != 0;
               score_t outpost_score_mid = Params::KNIGHT_OUTPOST[defenders][scoreSq][0];
               scores.mid += outpost_score_mid;
               score_t outpost_score_end = Params::KNIGHT_OUTPOST[defenders][scoreSq][1];
               scores.end += outpost_score_end;
#ifdef EVAL_DEBUG
               cout << "knight outpost (defenders=" <<
                   defenders << "): score (" << outpost_score_mid << ',' <<
                   outpost_score_end << ')' << endl;
#endif
            }
            allAttacks |= knattacks;
            minorAttacks |= knattacks;

            if (!deep_endgame) {
               Bitboard kattacks(knattacks & nearKing);
               if (kattacks) {
                  attackWeight += PARAM(MINOR_ATTACK_FACTOR);
                  if (kattacks & (kattacks-1)) {
                     attackWeight += PARAM(MINOR_ATTACK_BOOST);
                  }
                  attackCount++;
               }
            }
            break;
         }

      case Bishop:
         {
            scores.mid += PARAM(BISHOP_PST)[Midgame][scoreSq];
            scores.end += PARAM(BISHOP_PST)[Endgame][scoreSq];

            const Bitboard battacks(board.bishopAttacks(sq));
            allAttacks |= battacks;
            minorAttacks |= battacks;
            if (!deep_endgame) {
               Bitboard kattacks(battacks & nearKing);
               if (kattacks) {
                  attackWeight += PARAM(MINOR_ATTACK_FACTOR);
                  if (kattacks & (kattacks - 1)) {
                     attackWeight += PARAM(MINOR_ATTACK_BOOST);
                  }
                  attackCount++;
               }
               else if (battacks & board.queen_bits[side]) {
                  // possible stacked attackers
                  kattacks = board.bishopAttacks(sq, side) & nearKing;
                  if (kattacks) {
                     attackWeight += PARAM(MINOR_ATTACK_FACTOR);
                     if (kattacks & (kattacks - 1)) {
                        attackWeight += PARAM(MINOR_ATTACK_BOOST);
                     }
                     attackCount++;
                  }
               }
            }
            if (outpost(board,sq,side)) {
               int defenders = outpost_defenders(board,sq,side) != 0;
               score_t outpost_score_mid = Params::BISHOP_OUTPOST[defenders][scoreSq][0];
               scores.mid += outpost_score_mid;
               score_t outpost_score_end = Params::BISHOP_OUTPOST[defenders][scoreSq][1];
               scores.end += outpost_score_end;
#ifdef EVAL_DEBUG
               cout << "bishop outpost (defenders=" <<
                   defenders << "): score (" << outpost_score_mid << ',' <<
                   outpost_score_end << ')' << endl;
#endif
            }

            if (board.pinOnDiag(sq, okp, oside)) {
               pin_count++;
            }

            const score_t mobl = PARAM(BISHOP_MOBILITY)[Bitboard(battacks &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount()];
#ifdef EVAL_DEBUG
            cout << "bishop mobility (" << SquareImage(sq) << "): " << mobl << endl;
#endif
            scores.any += mobl;
            break;
         }

      case Rook:
         {
            scores.mid += PARAM(ROOK_PST)[Midgame][scoreSq];
            scores.end += PARAM(ROOK_PST)[Endgame][scoreSq];
            const Bitboard rattacks(board.rookAttacks(sq));
            const int r = Rank(sq, side);
            if (r == 7 && (Rank(okp,side) == 8 || (board.pawn_bits[oside] & Attacks::rank7mask[side]))) {
#ifdef EVAL_DEBUG
               Scores tmp(scores);
#endif
               scores.mid += PARAM(ROOK_ON_7TH_MID);
               scores.end += PARAM(ROOK_ON_7TH_END);

               if (Attacks::rank_mask_right[sq] & rattacks & board.rook_bits[side]) {
                  // 2 connected rooks on 7th
                  scores.mid += PARAM(TWO_ROOKS_ON_7TH_MID);
                  scores.end += PARAM(TWO_ROOKS_ON_7TH_END);
               }
#ifdef EVAL_DEBUG
               cout << "rook on 7th: (" << scores.mid-tmp.mid << ", " << scores.end-tmp.end << ")" << endl;
#endif
            }

            const int file = File(sq);
            allAttacks |= rattacks;
            rookAttacks |= rattacks;
            if (FileOpen(board, file)) {
               scores.mid += PARAM(ROOK_ON_OPEN_FILE_MID);
               scores.end += PARAM(ROOK_ON_OPEN_FILE_END);
            }

            Bitboard rattacks2(board.rookAttacks(sq, side));
            const int mobl = Bitboard(rattacks2 &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount();
            scores.mid += PARAM(ROOK_MOBILITY)[Midgame][mobl];
            scores.end += PARAM(ROOK_MOBILITY)[Endgame][mobl];
#ifdef EVAL_DEBUG
            cout << "rook mobility: (" << PARAM(ROOK_MOBILITY)[Midgame][mobl] <<
               ", " << PARAM(ROOK_MOBILITY)[Endgame][mobl] << ")" << endl;
#endif
            if (!deep_endgame) {
               Bitboard attacks(rattacks2 & nearKing);
               if (attacks) {
                  attackWeight += PARAM(ROOK_ATTACK_FACTOR);
                  attackCount++;
                  majorAttackCount++;
                  Bitboard attacks2(attacks & kingNearProximity[okp]);
                  if (attacks2) {
                     attacks2 &= (attacks2 - 1);
                     if (attacks2) {
                        attackWeight += PARAM(ROOK_ATTACK_BOOST);
                        attacks2 &= (attacks2 - 1);
                        if (attacks2) {
                           attackWeight += PARAM(ROOK_ATTACK_BOOST2);
                        }
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
            scores.mid += PARAM(QUEEN_PST)[Midgame][scoreSq];
            scores.end += PARAM(QUEEN_PST)[Endgame][scoreSq];
            int qmobl = 0;
            Bitboard battacks(board.bishopAttacks(sq));
            allAttacks |= battacks;
            Bitboard qmobility(battacks);
            Bitboard kattacks;

            if (board.pinOnDiag(sq, okp, oside)) {
               pin_count++;
            }

            Bitboard rattacks(board.rookAttacks(sq));
            qmobility |= rattacks;
            allAttacks |= qmobility;
            if (!deep_endgame) {
               int rank = Rank(sq, side);
               if (rank > 3) {
                  Bitboard back
                           ((board.knight_bits[side] | board.bishop_bits[side]) & Attacks::rank_mask[side == White ? 0 : 7]);
                  score_t queenOut = (PARAM(QUEEN_OUT) - (rank - 4) / 2) * (int) back.bitCount();
#ifdef EVAL_DEBUG
                  if (queenOut) {
                     cout << "premature Queen develop (" << ColorImage(side) << "): " << queenOut << endl;
                  }
#endif
                  scores.mid += queenOut;
               }

               kattacks = battacks & nearKing;
               if (!kattacks) {
                  kattacks = board.bishopAttacks(sq, side) & nearKing;
               }
               if (rattacks & nearKing) {
                  kattacks |= (rattacks & nearKing);
               }
               else {
                  kattacks |= (board.rookAttacks(sq, side) & nearKing);
               }

               if (kattacks) {
                  attackWeight += PARAM(QUEEN_ATTACK_FACTOR);
                  attackCount++;
                  majorAttackCount++;
#ifdef EVAL_DEBUG
                  int tmp = attackWeight;
#endif
                  // bonus if Queen attacks multiple squares near King:
                  Bitboard nearAttacks(kattacks & kingNearProximity[okp]);
                  if (nearAttacks) {
                     nearAttacks &= (nearAttacks - 1);      // clear 1st bit
                     if (nearAttacks) {
                        attackWeight += PARAM(QUEEN_ATTACK_BOOST);
                        nearAttacks &= (nearAttacks - 1);   // clear 1st bit
                        if (nearAttacks) {
                           attackWeight += PARAM(QUEEN_ATTACK_BOOST2);
                        }
                     }
                  }
#ifdef EVAL_DEBUG
                  if (attackWeight - tmp) cout << "queen attack boost= " << attackWeight - tmp << endl;
#endif
               }
            }

            qmobl += Bitboard(qmobility &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount();
            qmobl = std::min<int>(23,qmobl);
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
      if (TEST_MASK(Board::white_squares, board.bishop_bits[side])) {
         scoreBishopAndPawns<White>(board,side,ourPawnData,oppPawnData,scores,opp_scores);
      }
      else {
         scoreBishopAndPawns<Black>(board,side,ourPawnData,oppPawnData,scores,opp_scores);
      }
   }
   else if (bishopCount >= 2) {
#ifdef EVAL_DEBUG
      Scores tmp(scores);
#endif
      // Bishop pair bonus, higher bonus in endgame
      scores.mid += PARAM(BISHOP_PAIR_MID);
      scores.end += PARAM(BISHOP_PAIR_END);
#ifdef EVAL_DEBUG
      cout << "bishop pair (" << ColorImage(side) << "): (" <<
         scores.mid - tmp.mid << ", " << scores.end - tmp.end << ")" << endl;
#endif
   }
   scores.mid += PARAM(KING_PST)[Midgame][(side == White) ? kp : 63 - kp];

   allAttacks |= oppPawnData.opponent_pawn_attacks;
   allAttacks |= Attacks::king_attacks[kp];
   if (early_endgame) {
      int mobl = Bitboard(Attacks::king_attacks[okp] & ~board.allOccupied &
                  ~allAttacks).bitCount();
      opp_scores.end += PARAM(KING_MOBILITY_ENDGAME)[std::min<int>(4,mobl)];
#ifdef EVAL_DEBUG
      cout << ColorImage(oside) << " king mobility: " << PARAM(KING_MOBILITY_ENDGAME)[mobl] << endl;
#endif
   }
   if (!deep_endgame) {
      // add in pawn attacks
      int proximity = Bitboard(kingPawnProximity[oside][okp] & board.pawn_bits[side]).bitCount();
      int pawnAttacks = Bitboard(oppPawnData.opponent_pawn_attacks & nearKing).bitCount();
      attackWeight += PARAM(PAWN_ATTACK_FACTOR1)*proximity +
         PARAM(PAWN_ATTACK_FACTOR2)*pawnAttacks;
      attackWeight += PARAM(KING_ATTACK_COVER_BOOST_BASE) - oppCover*PARAM(KING_ATTACK_COVER_BOOST_SLOPE)/Params::PAWN_VALUE;

      const score_t index = std::max<score_t>(0,attackWeight/Params::KING_ATTACK_FACTOR_RESOLUTION);
#ifdef ATTACK_DEBUG
      cout << ColorImage(side) << " piece attacks on opposing king:" << endl;
      cout << " cover= " << oppCover << endl;
      cout << " pawn proximity=" << proximity << endl;
      cout << " attackCount=" << attackCount << endl;
      cout << " attackWeight=" << attackWeight << endl;
      cout << " index=" << index << endl;
      cout << " pin_count=" << pin_count << endl;
#endif
#ifdef TUNE
      score_t kattack = kingAttackSigmoid(index);
#else
      score_t kattack = PARAM(KING_ATTACK_SCALE)[std::min<int>(Params::KING_ATTACK_SCALE_SIZE-1,index)];
#endif
#ifdef ATTACK_DEBUG
      cout << "scaled king attack score=  " << kattack << endl;
#endif
      if (pin_count) kattack += PARAM(PIN_MULTIPLIER_MID) * pin_count;
#ifdef ATTACK_DEBUG
      Scores s;
      s.mid = kattack;
      cout << " king attack score (" << ColorImage(side) << ") : " << kattack << " (pre-scaling), " << s.blend(board.getMaterial(side).materialLevel()) << " (scaled)" << endl;
#endif
      // decrement the opposing side's scores because we want to
      // scale king attacks by this side's material level.
      opp_scores.mid -= kattack;
   }

   if (pin_count) scores.end += PARAM(PIN_MULTIPLIER_END) * pin_count;
   const Bitboard & opa = ourPawnData.opponent_pawn_attacks;
   // bonus for pawn threats against pieces
   int pawnThreats = Bitboard(oppPawnData.opponent_pawn_attacks & board.occupied[side] & ~board.pawn_bits[side]).bitCountOpt();
   if (pawnThreats) {
      scores.mid += PARAM(PAWN_THREAT_ON_PIECE_MID)*pawnThreats;
      scores.end += PARAM(PAWN_THREAT_ON_PIECE_END)*pawnThreats;
   }
   //Bitboard unsafePawns(board.pawn_bits[oside] & ~opa);
   const Bitboard unsafePawns = oppPawnData.weak_pawns;
   // penalize threats by pieces against pieces or pawns
   if (minorAttacks) {
      int qattacks = Bitboard(board.queen_bits[oside] & minorAttacks).bitCountOpt();
      int rattacks = Bitboard(board.rook_bits[oside] & minorAttacks).bitCountOpt();
      int mattacks = Bitboard((board.bishop_bits[oside] |
                               board.knight_bits[oside]) &
                              ~opa &
                              minorAttacks).bitCountOpt();
      if (!deep_endgame) {
         scores.mid += PARAM(PIECE_THREAT_MM_MID)*mattacks;
         scores.mid += PARAM(PIECE_THREAT_MR_MID)*rattacks;
         scores.mid += PARAM(PIECE_THREAT_MQ_MID)*qattacks;
         scores.mid += Bitboard(unsafePawns & minorAttacks).bitCountOpt()*PARAM(MINOR_PAWN_THREAT_MID);
      }
      if (early_endgame) {
         scores.end += PARAM(PIECE_THREAT_MM_END)*mattacks;
         scores.end += PARAM(PIECE_THREAT_MR_END)*rattacks;
         scores.end += PARAM(PIECE_THREAT_MQ_END)*qattacks;
         scores.end += Bitboard(unsafePawns & minorAttacks).bitCountOpt()*PARAM(MINOR_PAWN_THREAT_END);
      }
   }
   if (rookAttacks) {
      int qattacks = Bitboard(board.queen_bits[oside] & rookAttacks).bitCountOpt();
      int rattacks = Bitboard(board.rook_bits[oside] & minorAttacks & ~opa).bitCountOpt();
      int mattacks = Bitboard((board.bishop_bits[oside] |
                               board.knight_bits[oside]) &
                              ~opa &
                              minorAttacks).bitCountOpt();
      if (!deep_endgame) {
         scores.mid += PARAM(PIECE_THREAT_RM_MID)*mattacks;
         scores.mid += PARAM(PIECE_THREAT_RR_MID)*rattacks;
         scores.mid += PARAM(PIECE_THREAT_RQ_MID)*qattacks;
         scores.mid += Bitboard(unsafePawns & rookAttacks).bitCountOpt()*PARAM(ROOK_PAWN_THREAT_MID);
      }
      if (early_endgame) {
         scores.end += PARAM(PIECE_THREAT_RM_END)*mattacks;
         scores.end += PARAM(PIECE_THREAT_RR_END)*rattacks;
         scores.end += PARAM(PIECE_THREAT_RQ_END)*qattacks;
         scores.end += Bitboard(unsafePawns & rookAttacks).bitCountOpt()*PARAM(ROOK_PAWN_THREAT_END);
      }
   }
   if (early_endgame) {
      // attacks on undefended pawns or pieces
      Bitboard kattacks(Attacks::king_attacks[kp] & board.occupied[oside] & ~opa);
      if (kattacks) {
         scores.end += kattacks.bitCountOpt()*PARAM(ENDGAME_KING_THREAT);
      }
   }
}

void Scoring::calcPawnData(const Board &board,
                           ColorType side,
                           PawnHashEntry::PawnData &entr) {

   // This function computes a pawn structure score that depends
   // only on the location of pawns (of both colors). It also fills
   // in the pawn hash entry.
   //
   memset(&entr, '\0', sizeof(PawnHashEntry::PawnData));

   int incr = (side == White) ? 8 : -8;
   const ColorType oside = OppositeColor(side);
   entr.opponent_pawn_attacks = board.allPawnAttacks(oside);

   Piece enemy_pawn = MakePiece(Pawn, oside);
   Piece our_pawn = MakePiece(Pawn, side);
   Bitboard bi(board.pawn_bits[side]);
   Bitboard potentialPlus, potentialMinus;
   Square sq;
   int count = 0;
   while(bi.iterate(sq))
   {
      PawnDetails &details = entr.details;
      details[count].sq = sq;
      details[count].flags = 0;
      details[count].space_weight = 0;
      ASSERT(count<8);
      PawnDetail &td = details[count++];
#ifdef PAWN_DEBUG
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
            /*
            td.flags |= PawnDetail::POTENTIAL_PASSER;
            // Doubled but potentially passed pawn.
            // Don't score as passed but give "potential passer" bonus
            entr.midgame_score += PARAM(POTENTIAL_PASSER)[Midgame][rank];
            entr.endgame_score += PARAM(POTENTIAL_PASSER)[Endgame][rank];
            */
            passed = 0;
         }

         if (doubles.bitCountOpt() > 1) {
            // tripled pawns
#ifdef PAWN_DEBUG
            cout << " tripled";
#endif
#ifdef TUNE
            td.flags |= PawnDetail::TRIPLED;
#endif
            entr.midgame_score += PARAM(TRIPLED_PAWNS)[Midgame][file - 1];
            entr.endgame_score += PARAM(TRIPLED_PAWNS)[Endgame][file - 1];
         }
         else {
#ifdef TUNE
            td.flags |= PawnDetail::DOUBLED;
#endif
            entr.midgame_score += PARAM(DOUBLED_PAWNS)[Midgame][file - 1];
            entr.endgame_score += PARAM(DOUBLED_PAWNS)[Endgame][file - 1];
         }
         doubled++;
      }

      if (isolated && !passed) {
         entr.midgame_score += PARAM(ISOLATED_PAWN)[Midgame][file-1];
         entr.endgame_score += PARAM(ISOLATED_PAWN)[Endgame][file-1];
#ifdef PAWN_DEBUG
         cout << " isolated";
#endif
#ifdef TUNE
         td.flags |= PawnDetail::ISOLATED;
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
#ifdef TUNE
                  td.flags |= PawnDetail::WEAK;
#endif
                  entr.midgame_score += PARAM(WEAK_PAWN_MID);
                  entr.endgame_score += PARAM(WEAK_PAWN_END);
                  weak++;
              }
          }
      }

      if (weak || isolated) {
          entr.weak_pawns.set(sq);
          if (!passed && !OnFile(board.pawn_bits[oside], file)) {
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
#ifdef TUNE
         td.flags |= PawnDetail::PASSED;
#endif
         entr.midgame_score += PARAM(PASSED_PAWN)[Midgame][rank]*PARAM(PASSED_PAWN_FILE_ADJUST)[File(sq)-1]/64;
         entr.endgame_score += PARAM(PASSED_PAWN)[Endgame][rank]*PARAM(PASSED_PAWN_FILE_ADJUST)[File(sq)-1]/64;
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
#ifdef TUNE
                  td.flags |= PawnDetail::POTENTIAL_PASSER;
#endif
                  int i = 0;
#ifdef _DEBUG
                  bool found = false;
#endif
                  for (; i < count; i++) {
                     if (details[i].sq == dup) {
#ifdef _DEBUG
                        found = true;
#endif
                        break;
                     }
                  }
                  ASSERT(found);
                  entr.midgame_score += PARAM(POTENTIAL_PASSER)[Midgame][rank];
                  entr.endgame_score += PARAM(POTENTIAL_PASSER)[Endgame][rank];
                  if (details[i].flags & PawnDetail::POTENTIAL_PASSER) {
                      details[i].flags &= ~PawnDetail::POTENTIAL_PASSER;
                      entr.midgame_score -= PARAM(POTENTIAL_PASSER)[Midgame][rankdup];
                      entr.endgame_score -= PARAM(POTENTIAL_PASSER)[Endgame][rankdup];
                  }
               }
            }
            else {
               td.flags |= PawnDetail::POTENTIAL_PASSER;
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
         int space = (rank - 3);
         if (duo) space *= 2;
#ifdef PAWN_DEBUG
         cout << " space=" << space*PARAM(SPACE);
#endif
#ifdef TUNE
         td.space_weight += space;
#endif
         entr.midgame_score += space*PARAM(SPACE);
         entr.endgame_score += space*PARAM(SPACE);
      }
#ifdef PAWN_DEBUG
      cout << " total = (" <<
         entr.midgame_score - mid_tmp << ", " <<
         entr.endgame_score - end_tmp << ")" <<
         endl;
#endif
   }

   if (entr.passers) {
      Bitboard passers(entr.passers);
      Scores cp_score;
      while(passers.iterate(sq)) {
         if (File(sq) != 8 && entr.passers.isSet(sq+1)) {
            cp_score.mid += PARAM(CONNECTED_PASSER)[Midgame][Rank(sq, side)];
            cp_score.end += PARAM(CONNECTED_PASSER)[Endgame][Rank(sq, side)];
#ifdef TUNE
            for (int i = 0; i < count; i++) {
               if (entr.details[i].sq == sq) {
                  entr.details[i].flags |= PawnDetail::CONNECTED_PASSER;
                  break;
               }
            }
#endif
         }
         else if (TEST_MASK(Attacks::pawn_attacks[sq][side],entr.passers)) {
            cp_score.mid += PARAM(ADJACENT_PASSER)[Midgame][Rank(sq, side)];
            cp_score.end += PARAM(ADJACENT_PASSER)[Endgame][Rank(sq, side)];
#ifdef TUNE
            for (int i = 0; i < count; i++) {
               if (entr.details[i].sq == sq) {
                  entr.details[i].flags |= PawnDetail::ADJACENT_PASSER;
                  break;
               }
            }
#endif
         }
      }
      entr.midgame_score += cp_score.mid;
      entr.endgame_score += cp_score.end;
#ifdef EVAL_DEBUG
      if (cp_score.mid || cp_score.end) {
         cout << "connected passer score, square " << SquareImage(sq);
         cout << " = (" << cp_score.mid << ", " << cp_score.end << ")" << endl;
      }
#endif
   }

   Bitboard centerCalc(Attacks::center & (board.pawn_bits[side] | board.allPawnAttacks(side)));
#ifdef PAWN_DEBUG
   int tmp = entr.midgame_score;
#endif
   entr.midgame_score += PARAM(PAWN_CENTER_SCORE_MID) * centerCalc.bitCount();
#ifdef TUNE
   entr.center_pawn_factor = centerCalc.bitCount();
#endif
#ifdef PAWN_DEBUG
   if (entr.midgame_score - tmp) {
      cout << "pawn center score (" << ColorImage(side) << ") :" << entr.midgame_score - tmp << endl;
   }
#endif
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

score_t Scoring::materialScore(const Board &board) const {
    const ColorType side = board.sideToMove();
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    const int mdiff =  (int)(ourmat.value() - oppmat.value());
#ifdef EVAL_DEBUG
    cout << "mdiff=" << mdiff << endl;

#endif
    score_t adjust = 0;
    if (ourmat.infobits() != oppmat.infobits()) {
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
    const score_t matScore = mdiff + adjust;
#ifdef EVAL_DEBUG
    cout << "adjusted material score = " << matScore << endl;
#endif
    return matScore;
}


score_t Scoring::evalu8(const Board &board, bool useCache) {

   score_t score;
    
   if ((score = tryBitbase(board)) != INVALID_SCORE) {
       return score;
   }

   const score_t matScore = materialScore(board);

   const hash_t pawnHash = board.pawnHashCodeW ^ board.pawnHashCodeB;

   PawnHashEntry &pawnEntry = pawnHashTable[pawnHash % PAWN_HASH_SIZE];

   if (!useCache || pawnEntry.hc != pawnHash) {
      // Not found in table, need to calculate
      calcPawnEntry(board, pawnEntry);
   }

   Scores wScores, bScores;

   KingPawnHashEntry &whiteKPEntry = getKPEntry<White>(board,pawnEntry.pawnData(White),pawnEntry.pawnData(Black),useCache);
   KingPawnHashEntry &blackKPEntry = getKPEntry<Black>(board,pawnEntry.pawnData(Black),pawnEntry.pawnData(White),useCache);
   score_t whiteCover = whiteKPEntry.cover == Scoring::INVALID_SCORE ? 0 : whiteKPEntry.cover;
   score_t blackCover = blackKPEntry.cover == Scoring::INVALID_SCORE ? 0 : blackKPEntry.cover;

   const int w_materialLevel = board.getMaterial(White).materialLevel();
   const int b_materialLevel = board.getMaterial(Black).materialLevel();

   bool posEval = true;

   // Endgame scoring
   if (b_materialLevel <= PARAM(ENDGAME_THRESHOLD))
   {
#ifdef EVAL_DEBUG
      int tmp = wScores.end;
#endif
      if (specialCaseEndgame<White>(board,board.getMaterial(White),
                                    board.getMaterial(Black),wScores)) {
          posEval = false;
      }
      else {
          ASSERT(whiteKPEntry.king_endgame_position != Scoring::INVALID_SCORE);
          scoreEndgame<White>(board, whiteKPEntry.king_endgame_position,
                              wScores);
      }
#ifdef EVAL_DEBUG
      cout << "endgame score (White)=" << wScores.end - tmp << endl;
#endif

   }

   if (w_materialLevel <= PARAM(ENDGAME_THRESHOLD))
   {
#ifdef EVAL_DEBUG
      int tmp = bScores.end;
#endif
      if (specialCaseEndgame<Black>(board,board.getMaterial(Black),
                                    board.getMaterial(White),bScores)) {
          posEval = false;
      }
      else {
          ASSERT(blackKPEntry.king_endgame_position != Scoring::INVALID_SCORE);
          scoreEndgame<Black>(board,
                              blackKPEntry.king_endgame_position, bScores);
#ifdef EVAL_DEBUG
          cout << "endgame score (Black)=" << bScores.end - tmp << endl;
#endif
      }
   }

   if (posEval) {
       // compute positional scores
       positionalScore<White>(board, pawnEntry, whiteCover, blackCover, wScores, bScores);
       positionalScore<Black>(board, pawnEntry, blackCover, whiteCover, bScores, wScores);
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
   score = wScores.blend(b_materialLevel) - bScores.blend(w_materialLevel);

   if (options.search.strength < 100) {
      // "flatten" positional score values
      score = score * std::max<int>(100,options.search.strength*options.search.strength) / 10000;
   }

   // add material score, which is from the perspective of the side to move
   if (board.sideToMove() == White)
      score += matScore;
   else
      score -= matScore;

   if (board.sideToMove() == Black) {
      score = -score;
   }

#ifdef _DEBUG
#ifdef TUNE
   if (fabs(score) >= Constants::MATE) {
#else
   if (std::abs(score) >= Constants::MATE) {
#endif
      cout << board << endl;
      ASSERT(0);
   }
#endif

   return score;
}

static int pp_block_index(Square passer, Square blocker, ColorType side)
{
   int dist = Rank(blocker,side)-Rank(passer,side)-1;
   ASSERT(dist>=0);
   static const int offsets[6] = {0,6,11,15,18,20};
   int index = offsets[Rank(passer,side)-2]+dist;
   ASSERT(index>=0 && index<21);
   return index;
}

static Square nextBlocker(const Board &board, ColorType side, Square sq)
{
   const Bitboard &mask = (side == White) ? Attacks::file_mask_up[sq] :
      Attacks::file_mask_down[sq];
   Bitboard occ(mask & board.allOccupied);
   if (!occ) return InvalidSquare;
   if (side == White)
      return occ.firstOne();
   else
      return occ.lastOne();
}

void Scoring::pawnScore(const Board &board, ColorType side, const PawnHashEntry::PawnData &pawnData, Scores &scores) {
#ifdef PAWN_DEBUG
   Scores tmp(scores);
#endif
   scores.mid += pawnData.midgame_score;
   scores.end += pawnData.endgame_score;

   if (board.rook_bits[OppositeColor(side)] | board.queen_bits[OppositeColor(side)]) {
#ifdef PAWN_DEBUG
      int mid_tmp = scores.mid;
      int end_tmp = scores.end;
#endif
      scores.mid += (int) pawnData.weakopen * PARAM(WEAK_ON_OPEN_FILE_MID);
      scores.end += (int) pawnData.weakopen * PARAM(WEAK_ON_OPEN_FILE_END);
#ifdef PAWN_DEBUG
      if (mid_tmp != scores.mid ||
          end_tmp != scores.end) {
         cout << "weak pawns on open file (" << ColorImage(side) << "): (";
         cout << scores.mid - mid_tmp << ", ";
         cout << scores.end - end_tmp << ")" << endl;
      }
#endif
   }

   // interaction of pawns and pieces
#ifdef PAWN_DEBUG
   int center_tmp = scores.mid;
#endif
   if (side == White) {
      if (board[chess::D2] == WhitePawn && board[chess::D3] > WhitePawn && board[chess::D3] < BlackPawn) {
         scores.mid += PARAM(CENTER_PAWN_BLOCK);
      }

      if (board[chess::E2] == WhitePawn && board[chess::E3] > WhitePawn && board[chess::E3] < BlackPawn) {
         scores.mid += PARAM(CENTER_PAWN_BLOCK);
      }
#ifdef PAWN_DEBUG
      if (scores.mid - center_tmp) {
         cout << "center pawn block (White): " << scores.mid - center_tmp << endl;
      }
#endif
   }
   else {
      if (board[chess::D7] == BlackPawn && board[chess::D6] > BlackPawn) {
         scores.mid += PARAM(CENTER_PAWN_BLOCK);
      }

      if (board[chess::E7] == BlackPawn && board[chess::E6] > BlackPawn) {
         scores.mid += PARAM(CENTER_PAWN_BLOCK);
      }
#ifdef PAWN_DEBUG
      if (scores.mid - center_tmp) {
         cout << "center pawn block (Black): " << scores.mid - center_tmp << endl;
      }
#endif
   }

   Square sq;
   Bitboard passers2(pawnData.passers);
   while(passers2.iterate(sq)) {
      ASSERT(OnBoard(sq));

      const int file = File(sq);
      const int rank = Rank(sq,side);
      Square blocker;
      blocker = nextBlocker(board,side,sq);
      if (blocker != InvalidSquare) {
         const int index = pp_block_index(sq,blocker,side);
         if (board.occupied[side].isSet(blocker)) {
            score_t mid_penalty = Params::PP_OWN_PIECE_BLOCK[Midgame][index];
            score_t end_penalty = Params::PP_OWN_PIECE_BLOCK[Endgame][index];
            scores.mid += mid_penalty;
            scores.end += end_penalty;
#ifdef PAWN_DEBUG
            cout <<
               ColorImage(side) <<
               " passed pawn on " <<
               SquareImage(sq);
            cout << " blocked by piece on " <<
               SquareImage(blocker) << ", score= (" << mid_penalty << ", " <<
               end_penalty << ")" << endl;
#endif
         }
         else {
            score_t mid_penalty = Params::PP_OPP_PIECE_BLOCK[Midgame][index];
            score_t end_penalty = Params::PP_OPP_PIECE_BLOCK[Endgame][index];
            scores.mid += mid_penalty;
            scores.end += end_penalty;
#ifdef PAWN_DEBUG
            cout <<
               ColorImage(side) <<
               " passed pawn on " <<
               SquareImage(sq);
            cout << " blocked by piece on " <<
               SquareImage(blocker) << ", score= (" << mid_penalty << ", " <<
               end_penalty << ")" << endl;
#endif
         }
      }
#ifdef PAWN_DEBUG
      score_t mid_tmp = scores.mid;
      score_t end_tmp = scores.end;
#endif
      if (board.rook_bits[side] & Attacks::file_mask[file-1]) {
          Bitboard atcks = (side == White) ? board.fileAttacksDown(sq) :
              board.fileAttacksUp(sq);
          if (board.rook_bits[side] & atcks) {
#ifdef EVAL_DEBUG
            cout << "rook behind PP: " << SquareImage(sq) << endl;
#endif
            scores.mid += PARAM(ROOK_BEHIND_PP_MID);
            scores.end += PARAM(ROOK_BEHIND_PP_END);
         }
      }
#ifdef PAWN_DEBUG
      if ((mid_tmp != scores.mid) ||
          (end_tmp != scores.end)) {
         cout << "rook/passed pawn placement (" << ColorImage(side) << ") (";
         cout << scores.mid - mid_tmp << ", " <<
            scores.end - end_tmp << ")" << endl;
      }
#endif
      Square queenSq = MakeSquare(file,8,side);
      if (rank >= 6) {
          // evaluate control of Queening square
          Bitboard ahead = (side == White) ? Attacks::file_mask_up[file-1] :
              Attacks::file_mask_down[file-1];
          Bitboard atcks(board.calcAttacks(queenSq,side));
          // don't count pawn attacks because connected passer
          // score gives bonus for that. Also no bonus for
          // pieces that block queening path.
          if (atcks & ~board.pawn_bits[side] & ~ahead) {
              scores.mid += PARAM(QUEENING_SQUARE_CONTROL_MID);
              scores.end += PARAM(QUEENING_SQUARE_CONTROL_END);
          }
          Bitboard oppAtcks(board.calcAttacks(queenSq,OppositeColor(side)));
          if (oppAtcks) {
              scores.mid += PARAM(QUEENING_SQUARE_OPP_CONTROL_MID);
              scores.end += PARAM(QUEENING_SQUARE_OPP_CONTROL_END);
          }
      }
      if (board.getMaterial(side).materialLevel()<=9 &&
          board.bishop_bits[side]) {
          Bitboard mask = SquareColor(queenSq) == White ? Board::white_squares : Board::black_squares;
          if (!(mask & board.bishop_bits[side])) {
              scores.end += PARAM(WRONG_COLOR_BISHOP);
          }
      }
   }

#ifdef PAWN_DEBUG
   cout << ColorImage(side) << " pawn score: " << endl;
   cout << " general = " << scores.any-tmp.any << endl;
   cout << " midgame = " << scores.mid-tmp.mid << endl;
   cout << " endgame = " << scores.end-tmp.end << endl;
   Scores diff;
   diff.mid = scores.mid-tmp.mid;
   diff.end = scores.end-tmp.end;
   diff.any = scores.any-tmp.any;

   cout << " blend = " << diff.blend(board.getMaterial(OppositeColor(side)).materialLevel()) << endl;
#endif
}

score_t Scoring::kingDistanceScore(const Board &board) const
{
   return PARAM(KING_DISTANCE_BASIS) - PARAM(KING_DISTANCE_MULT)*distance(board.kingSquare(White), board.kingSquare(Black));
}

// returns 1 if we evaluated a special-case endgame
template <ColorType side>
int Scoring::specialCaseEndgame(const Board &board,
                                const Material &ourMaterial,
                                const Material &oppMaterial,
                                Scores &scores)
{
   if (oppMaterial.kingOnly()) {
      // code for some special-case endgames, pawn or piece vs bare King
      const ColorType oside = OppositeColor(side);
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
         return 1;
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
             if (rrank == krank - 1) scores.end += Params::PAWN_VALUE*(8+(4-krank))/128;
         } else {
             if (rrank == krank + 1) scores.end += Params::PAWN_VALUE*(8+(krank-4))/128;
         }
         int kfile = File(oppkp);
         int rfile = File(rookSq);
         if (kfile >= 4) {
             if (rfile == kfile-1) scores.end += Params::PAWN_VALUE*(8+(4-kfile))/128;
         } else {
             if (rfile == kfile+1) scores.end += Params::PAWN_VALUE*(8+(kfile-4))/128;
         }
         return 1;
      } else if (ourMaterial.infobits() == Material::KQ) {
         // keep the kings close
         scores.end += kingDistanceScore(board);
         Square oppkp = board.kingSquare(oside);
         Square ourkp = board.kingSquare(side);
         int krank = Rank(oppkp,White);
         int kfile = File(oppkp);
         if (InCorner(oppkp)) {
            const int kingDistance = distance(board.kingSquare(White), board.kingSquare(Black));
            if (kingDistance == 2) {
               scores.end += Params::PAWN_VALUE/10;
            }
         } else if (OnEdge(oppkp)) {
             // position King appropriately
             if (kfile == chess::AFILE) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp + 1)) scores.end += Params::PAWN_VALUE/10;
             } else if (kfile == chess::HFILE) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp - 1)) scores.end += Params::PAWN_VALUE/10;
             } else if (krank == 1) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp+8)) scores.end += Params::PAWN_VALUE/10;
             } else if (krank == 8) {
                 if (Attacks::king_attacks[ourkp].isSet(oppkp-8)) scores.end += Params::PAWN_VALUE/10;
             }
         }
         // drive opposing king to the edge
         scores.end -= KRScores[oppkp];
         return 1;
      }
   }
   return 0;
}

void Scoring::calcKingEndgamePosition(const Board &board, ColorType side,                                            const PawnHashEntry::PawnData &ourPawnData,
                                      const PawnHashEntry::PawnData &oppPawnData,
                                      KingPawnHashEntry &entry)
{
   const ColorType oside = OppositeColor(side);

   const Square kp = board.kingSquare(side);

   entry.king_endgame_position = (score_t)0;

   Bitboard all_pawns(board.pawn_bits[White] | board.pawn_bits[Black]);
   if (!all_pawns) return;

   // Evaluate king position. Big bonus for centralizing king or
   // moving it forward in the endgame.
   Square scoreSq = (side == White) ? kp : (63-kp);
   score_t k_pos = PARAM(KING_PST)[Endgame][scoreSq];
   score_t k_pos_adj = 0;

   // If all pawns are on same side, bonus for King being on the
   // same side (idea from Crafty):
   if (!TEST_MASK(Attacks::abcd_mask, all_pawns)) {
      if (File(kp) > chess::DFILE)
         k_pos += PARAM(PAWN_SIDE_BONUS);
      else
         k_pos -= PARAM(PAWN_SIDE_BONUS);
   }
   else if (!TEST_MASK(Attacks::efgh_mask, all_pawns)) {
      if (File(kp) <= chess::DFILE)
         k_pos += PARAM(PAWN_SIDE_BONUS);
      else
         k_pos -= PARAM(PAWN_SIDE_BONUS);
   }

   // Evaluate proximity of the King to pawns
   Bitboard it(board.pawn_bits[side]);
   Square sq;
   while(it.iterate(sq)) {
      k_pos_adj += (4-distance(kp,sq))*PARAM(KING_OWN_PAWN_DISTANCE);
   }
   it = board.pawn_bits[oside];
   while (it.iterate(sq)) {
      int rank = Rank(sq,oside);
      k_pos_adj += (4-distance(kp,sq))*PARAM(KING_OPP_PAWN_DISTANCE);
      if (oppPawnData.passers.isSet(sq) && Rank(kp,oside)>=rank) {
         // Extra bonus for being near and ahead of passers, esp.
         // advanced ones.
         // Reward king distance to the queening square and to the
         // pawn.
         Square queenSq = MakeSquare(File(sq),8,oside);
         k_pos_adj += (4-(distance(kp,sq))+(4-distance(kp,queenSq)))*PARAM(KING_OPP_PASSER_DISTANCE)[rank-2]/2;
      }
   }

   k_pos += k_pos_adj/4;
   entry.king_endgame_position = k_pos;
}


template<ColorType side>
void Scoring::scoreEndgame(const Board &board,score_t k_pos,Scores &scores) {

   const Material &ourMaterial = board.getMaterial(side);

   // King position is even more important with reduced
   // material. Apply special scaling for late endgame here.
   const int pieces = ourMaterial.pieceCount();
   if (pieces < 3) {
#ifdef EVAL_DEBUG
      int tmp = k_pos;
#endif
      k_pos = (k_pos*PARAM(KING_POSITION_LOW_MATERIAL)[pieces])/128;
#ifdef EVAL_DEBUG
      if (tmp - k_pos) {
         cout << "king pos after reduced material bonus (" << ColorImage(side) << "): " <<
            k_pos-tmp << endl;
      }
#endif
   }
   scores.end += k_pos;

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
            ++i;
         }
         scores.end += PARAM(SIDE_PROTECTED_PAWN)*i;
      }
   }
#ifdef EVAL_DEBUG
   if (scores.end - tmp) {
         cout << "side-protected pawns (" << ColorImage(side) << "): " <<
            scores.end-tmp << endl;
   }
#endif
}

Scoring::PawnHashEntry & Scoring::pawnEntry (const Board &board, bool useCache) {
   hash_t pawnHash = board.pawnHashCodeW ^ board.pawnHashCodeB;
   PawnHashEntry &pawnEntry = pawnHashTable[pawnHash % PAWN_HASH_SIZE];
   if (!useCache || pawnEntry.hc != pawnHash) {
      calcPawnEntry(board, pawnEntry);
   }
   return pawnEntry;
}

template <ColorType side>
Scoring::KingPawnHashEntry &Scoring::getKPEntry(const Board &board,
                                                const PawnHashEntry::PawnData &ourPawnData,
                                                const PawnHashEntry::PawnData &oppPawnData,
bool useCache)
{
   hash_t kphash = BoardHash::kingPawnHash(board,side);
   KingPawnHashEntry &entry = kingPawnHashTable[side][kphash % KING_PAWN_HASH_SIZE];
   int mLevel = board.getMaterial(OppositeColor(side)).materialLevel();
   bool needCover = mLevel >= PARAM(MIDGAME_THRESHOLD);
   bool needEndgame = mLevel <= PARAM(ENDGAME_THRESHOLD);
   if (!useCache || (entry.hc != kphash)) {
      if (needCover) {
         calcCover<side>(board,entry);
      } else {
         entry.cover = Scoring::INVALID_SCORE;
      }
      if (needEndgame) {
         calcKingEndgamePosition(board,side,ourPawnData,oppPawnData,entry);
      } else {
         entry.king_endgame_position = Scoring::INVALID_SCORE;
      }
      entry.hc = kphash;
   }
   else {
      if (needCover && entry.cover == Scoring::INVALID_SCORE) {
         calcCover<side>(board,entry);
      }
      if (needEndgame && entry.king_endgame_position == Scoring::INVALID_SCORE) {
         calcKingEndgamePosition(board,side,ourPawnData,oppPawnData,entry);
      }
#ifdef _DEBUG
      // cached entry better = computed entry
      KingPawnHashEntry entry2;
      entry2.cover = entry2.king_endgame_position = 0;
      calcCover<side>(board,entry2);
      calcKingEndgamePosition(board,side,ourPawnData,oppPawnData,entry2);
      if (needCover && entry.cover != entry2.cover) {
         cout << board << endl;
         cout << "mLevel=" << mLevel << endl;
         cout << "cover1=" << entry.cover << " cover2=" << entry2.cover << endl;
         ASSERT(entry.cover == entry2.cover);
      }

      if (needEndgame && entry.king_endgame_position != entry2.king_endgame_position) {
         cout << board << endl;
         ASSERT(entry.king_endgame_position == entry2.king_endgame_position);
      }
#endif
   }
   return entry;
}


template<ColorType side>
void Scoring::positionalScore(const Board &board, const PawnHashEntry &pawnEntry, score_t ownCover, score_t oppCover, Scores &scores, Scores &oppScores) {
   const ColorType oside = OppositeColor(side);

#ifdef EVAL_DEBUG
   cout << "scores for " << ColorImage(side) << ":" << endl;
#endif
   pawnScore(board, side, pawnEntry.pawnData(side), scores);

   // outside passed pawn scoring, based on cached pawn data
   if (pawnEntry.pawnData(side).outside && !pawnEntry.pawnData(oside).outside) {
      scores.mid += PARAM(OUTSIDE_PASSER_MID);
      scores.end += PARAM(OUTSIDE_PASSER_END);
#ifdef PAWN_DEBUG
      cout << "outside passer (" << ColorImage(side) << "): (" <<
         PARAM(OUTSIDE_PASSER_MID) << ", " <<
         PARAM(OUTSIDE_PASSER_END) << ")" << endl;
#endif
   }

   // Penalize loss of castling.
   score_t castling = APARAM(CASTLING,(int)board.castleStatus(side));
   scores.mid += castling;
#ifdef EVAL_DEBUG
   cout << "castling score: " << castling << endl;
#endif
   for(int i = 0; i < 4; i++) {
      if (BISHOP_TRAP_PATTERN[side][i].bishopMask & board.bishop_bits[side]
          && BISHOP_TRAP_PATTERN[side][i].pawnMask & board.pawn_bits[oside]) {
#ifdef EVAL_DEBUG
         cout << "bishop trapped" << endl;
#endif
         scores.any += PARAM(BISHOP_TRAPPED);
      }
   }

   // add penalty for damaged king cover
   scores.mid += ownCover;

   pieceScore<side> (board, pawnEntry.pawnData(side),
                     pawnEntry.pawnData(oside), oppCover, scores, oppScores,
                     board.getMaterial(side).materialLevel() <= PARAM(ENDGAME_THRESHOLD),
                     board.getMaterial(side).materialLevel() <= PARAM(MIDGAME_THRESHOLD));
}

template<ColorType side>
int Scoring::KBPDraw(const Board &board) {
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
             || (std::abs(File(okp) - pfile) <= 1 &&
                 Rank(okp, side) > Rank(pawn, side) &&
                 Rank(okp, side) > Rank(kp,side))) {
            return 1;   // draw
         }
      }
   }

   return 0;
}

int Scoring::theoreticalDraw(const Board &board) {
    if (board.getMaterial(White).value() >
        board.getMaterial(Black).value())
        return theoreticalDraw<White>(board);
    else if (board.getMaterial(White).value() <
             board.getMaterial(Black).value())
        return theoreticalDraw<Black>(board);
    else
        return 0;
}

// check for theoretical draws ("side" has the greater material)
template <ColorType side>
int Scoring::theoreticalDraw(const Board &board) {
    const Material &mat1 = board.getMaterial(side);
    const Material &mat2 = board.getMaterial(OppositeColor(side));
    if (mat1.value() > Params::KING_VALUE + (Params::KNIGHT_VALUE * 2)) return 0;

    // Check for K + P vs rook pawn
    if (mat1.infobits() == Material::KP && mat2.kingOnly()) {
        if (TEST_MASK(rook_pawn_mask, board.pawn_bits[side])) {
            Square kp = board.kingSquare(side);
            Square kp2 = board.kingSquare(OppositeColor(side));
            Square psq = (board.pawn_bits[side].firstOne());
            return lookupBitbase(kp, psq, kp2, side, board.sideToMove()) == 0;
        }
        else {
            return 0;
        }
    }
    // Check for wrong bishop + rook pawn(s) vs king.  Not very common but
    // we don't want to get it wrong.
    else if (mat1.pieceBits() == Material::KB && mat2.kingOnly()) {
        return KBPDraw<side> (board);
    }
    // check for KNN vs K
    else if (mat1.infobits() == Material::KNN && mat2.kingOnly()) {
        return 1;
    }
    else {
        return 0;
    }
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
      board.getMaterial(White).value() <= Params::KING_VALUE + (Params::KNIGHT_VALUE * 2)
   && board.getMaterial(Black).value() <= Params::KING_VALUE + (Params::KNIGHT_VALUE * 2)
   ) {

      // check for insufficient material and other drawing situations
      return Scoring::isDraw(board);
   }
   else
      return 0;
}

void Scoring::printScore(score_t score, ostream &str) {
   if (score <= -Constants::MATE_RANGE) {
      str << "-Mate" << int(Constants::MATE + score + 1) / 2;
   }
   else if (score >= Constants::MATE_RANGE) {
      str << "+Mate" << int(Constants::MATE - score + 1) / 2;
   }
   else if (score == Constants::TABLEBASE_WIN) {
      str << "+TbWin";
   }
   else if (score == -Constants::TABLEBASE_WIN) {
      str << "-TbLoss";
   }
   else {
      if (score >= 0) str << '+';
      std::ios_base::fmtflags original_flags = str.flags();
      str << fixed << setprecision(2) << (score * 1.0) / double(Params::PAWN_VALUE);
      str.flags(original_flags);
   }
}

void Scoring::printScoreUCI(score_t score, ostream &str) {
   if (score <= -Constants::MATE_RANGE) {
      str << "mate " << -int(Constants::MATE + score + 1) / 2;
   }
   else if (score >= Constants::MATE_RANGE) {
      str << "mate " << int(Constants::MATE - score + 1) / 2;
   }
   else
      str << "cp " << int(score*100)/Params::PAWN_VALUE;
}

score_t Scoring::tryBitbase(const Board &board) {
   const Material &wMat = board.getMaterial(White);
   const Material &bMat = board.getMaterial(Black);
   if ((unsigned) wMat.infobits() == Material::KP && bMat.kingOnly()) {
      Square passer = board.pawn_bits[White].firstOne();
      if (lookupBitbase(board.kingSquare(White), passer, board.kingSquare(Black), White, board.sideToMove())) {
         return board.sideToMove() == White ? Constants::BITBASE_WIN :
              -Constants::BITBASE_WIN;
      }
      else {
         return 0;
      }
   }
   else if ((unsigned) bMat.infobits() == Material::KP && wMat.kingOnly()) {
      Square passer = board.pawn_bits[Black].firstOne();
      if (lookupBitbase(board.kingSquare(Black), passer, board.kingSquare(White), Black, board.sideToMove())) {
         return board.sideToMove() == Black ? Constants::BITBASE_WIN :
              -Constants::BITBASE_WIN;
      }
      else {
         return 0;
      }
   }

   return INVALID_SCORE;
}

void Scoring::clearHashTables() {
   for (int i = 0; i < PAWN_HASH_SIZE; i++) {
      pawnHashTable[i].hc = (hash_t)0xababababababababULL;
   }
   for (int i = 0; i < KING_PAWN_HASH_SIZE; i++) {
      kingPawnHashTable[White][i].hc = (hash_t)0;
      kingPawnHashTable[Black][i].hc = (hash_t)0;
   }
}

#ifdef TUNE
#include "tune.h"

score_t Scoring::kingAttackSigmoid(score_t weight) const
{
    return PARAM(KING_ATTACK_SCALE_BIAS) +
        PARAM(KING_ATTACK_SCALE_MAX)/(1+exp(-PARAM(KING_ATTACK_SCALE_FACTOR)*(weight-PARAM(KING_ATTACK_SCALE_INFLECT))/1000.0));
}

static void print_array(ostream & o,score_t arr[], int size, int add_semi = 1)
{
   o << "{";
   score_t *p = arr;
   for (int i = 0; i < size; i++) {
      if (i) o << ", ";
      o << std::round(*p++);
   }
   o << "}";
   if (add_semi) o << ";" << endl;
}

static void print_array(ostream & o,score_t mid[], score_t end[], int size)
{
   o << "{";
   print_array(o,mid,size,0);
   o << ", ";
   print_array(o,end,size,0);
   o << "};" << endl;
}

void Params::write(ostream &o, const string &comment)
{
   o << "// Copyright 2015-2017 by Jon Dart. All Rights Reserved." << endl;
   o << "// This is a generated file. Do not edit." << endl;
   o << "// " << comment << endl;
   o << "//" << endl;
   o << endl << "#include \"params.h\"" << endl;
   o << endl;
   o << "const int Params::RB_ADJUST[6] = ";
   print_array(o,Params::RB_ADJUST,6);
   o << "const int Params::RBN_ADJUST[6] = ";
   print_array(o,Params::RBN_ADJUST,6);
   o << "const int Params::QR_ADJUST[5] = ";
   print_array(o,Params::QR_ADJUST,5);
   o << "const int Params::KN_VS_PAWN_ADJUST[3] = ";
   print_array(o,Params::KN_VS_PAWN_ADJUST,3);
   o << "const int Params::CASTLING[6] = ";
   print_array(o,Params::CASTLING,6);
   o << "const int Params::KING_COVER[6][4] = {";
   for (int i = 0; i < 6; i++) {
      print_array(o,Params::KING_COVER[i],4,0);
      if (i<5) o << "," << endl;
   }
   o << "};" << endl;
   int start = Tune::KING_COVER_BASE;
   for (int i = start; i < start+tune_params.paramArraySize(); i++) {
      o << "const int Params::";
      Tune::TuneParam param;
      tune_params.getParam(i,param);
      const string str(param.name);

      for (string::const_iterator it = str.begin();
           it != str.end();
           it++) {
         o << (char)toupper((int)*it);
      }
      o << " = " << std::round(param.current) << ";" << endl;
   }
   o << "const int Params::KING_OPP_PASSER_DISTANCE[6] = ";
   print_array(o,Params::KING_OPP_PASSER_DISTANCE,6);
   o << "const int Params::KING_POSITION_LOW_MATERIAL[3] =";
   print_array(o,Params::KING_POSITION_LOW_MATERIAL,3);
   o << "const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = ";
   print_array(o,Params::KING_ATTACK_SCALE,Params::KING_ATTACK_SCALE_SIZE);
   o << "const int Params::TRADE_DOWN[8] = ";
   print_array(o,Params::TRADE_DOWN,8);
   o << "const int Params::PASSED_PAWN[2][8] = ";
   print_array(o,Params::PASSED_PAWN[0], Params::PASSED_PAWN[1], 8);
   score_t file_adjust[8];
   for (int i = 0; i < 8; i++) {
      int j = i < 4 ? i : 7-i;
      file_adjust[i] = tune_params[Tune::PASSED_PAWN_FILE_ADJUST1+j].current;
   }
   o << "const int Params::PASSED_PAWN_FILE_ADJUST[8] = ";
   print_array(o,file_adjust,8);
   o << "const int Params::POTENTIAL_PASSER[2][8] = ";
   print_array(o,Params::POTENTIAL_PASSER[0], Params::POTENTIAL_PASSER[1], 8);
   o << "const int Params::CONNECTED_PASSER[2][8] = ";
   print_array(o,Params::CONNECTED_PASSER[0], Params::CONNECTED_PASSER[1], 8);
   o << "const int Params::ADJACENT_PASSER[2][8] = ";
   print_array(o,Params::ADJACENT_PASSER[0], Params::ADJACENT_PASSER[1], 8);
   o << "const int Params::PP_OWN_PIECE_BLOCK[2][21] = ";
   print_array(o,Params::PP_OWN_PIECE_BLOCK[0], Params::PP_OWN_PIECE_BLOCK[1], 21);
   o << "const int Params::PP_OPP_PIECE_BLOCK[2][21] = ";
   print_array(o,Params::PP_OPP_PIECE_BLOCK[0], Params::PP_OPP_PIECE_BLOCK[1], 21);
   o << "const int Params::DOUBLED_PAWNS[2][8] = ";
   print_array(o,Params::DOUBLED_PAWNS[0], Params::DOUBLED_PAWNS[1], 8);
   o << "const int Params::TRIPLED_PAWNS[2][8] = ";
   print_array(o,Params::TRIPLED_PAWNS[0], Params::TRIPLED_PAWNS[1], 8);
   o << "const int Params::ISOLATED_PAWN[2][8] = ";
   print_array(o,Params:: ISOLATED_PAWN[0], Params::ISOLATED_PAWN[1], 8);
   o << endl;
   o << "const int Params::KNIGHT_PST[2][64] = ";
   print_array(o,Params::KNIGHT_PST[0],Params::KNIGHT_PST[1],64);
   o << "const int Params::BISHOP_PST[2][64] = ";
   print_array(o,Params::BISHOP_PST[0],Params::BISHOP_PST[1],64);
   o << "const int Params::ROOK_PST[2][64] = ";
   print_array(o,Params::ROOK_PST[0],Params::ROOK_PST[1],64);
   o << "const int Params::QUEEN_PST[2][64] = ";
   print_array(o,Params::QUEEN_PST[0],Params::QUEEN_PST[1],64);
   o << "const int Params::KING_PST[2][64] = ";
   print_array(o,Params::KING_PST[0],Params::KING_PST[1],64);
   o << endl;
   o << "const int Params::KNIGHT_MOBILITY[9] = ";
   print_array(o,Params::KNIGHT_MOBILITY,9);
   o << "const int Params::BISHOP_MOBILITY[15] = ";
   print_array(o,Params::BISHOP_MOBILITY,15);
   o << "const int Params::ROOK_MOBILITY[2][15] = ";
   print_array(o,Params::ROOK_MOBILITY[0],Params::ROOK_MOBILITY[1],15);
   o << "const int Params::QUEEN_MOBILITY[2][24] = ";
   print_array(o,Params::QUEEN_MOBILITY[0],Params::QUEEN_MOBILITY[1],24);
   o << "const int Params::KING_MOBILITY_ENDGAME[5] = ";
   print_array(o,Params::KING_MOBILITY_ENDGAME,5);
   o << endl;
   o << "const int Params::KNIGHT_OUTPOST[2][64][2] = {";
   for (int p = 0; p < 2; p++) {
       o << "{";
       for (int sq = 0; sq < 64; sq++) {
           print_array(o,Params::KNIGHT_OUTPOST[p][sq],2,0);
           if (sq < 63) o << ",";
       }
       o << "}";
       if (p < 1) o << ",";
   }
   o << "};" << endl;
   o << "const int Params::BISHOP_OUTPOST[2][64][2] = {";
   for (int p = 0; p < 2; p++) {
       o << "{";
       for (int sq = 0; sq < 64; sq++) {
           print_array(o,Params::BISHOP_OUTPOST[p][sq],2,0);
           if (sq < 63) o << ",";
       }
       o << "}";
       if (p < 1) o << ",";
   }
   o << "};" << endl;
   o << endl;
}
#endif
