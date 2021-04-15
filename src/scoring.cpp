// Copyright 1994-2020 by Jon Dart.  All Rights Reserved.

#include "scoring.h"
#include "bhash.h"
#include "bitprobe.h"
#include "hash.h"
#include "globals.h"
#include "material.h"
#include "movegen.h"
#ifdef TUNE
#include "tune.h"
#include <ctime>
#endif
#include <cstddef>
#include <algorithm>
#include <climits>
#include <iomanip>

//#define PAWN_DEBUG
//#define EVAL_DEBUG
//#define ATTACK_DEBUG

const int Params::MATERIAL_SCALE[32] =
{
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 12, 24, 36, 48, 60, 72, 84, 96,
   108, 116, 120, 128, 128, 128, 128, 128, 128, 128, 128
};

const int Params::MIDGAME_THRESHOLD = 12;
const int Params::ENDGAME_THRESHOLD = 23;

const score_t Params::SEE_PIECE_VALUES[7] = {0, Params::PAWN_VALUE, score_t(4.3*Params::PAWN_VALUE),
      score_t(4.3*Params::PAWN_VALUE), score_t(6.0*Params::PAWN_VALUE), score_t(12.0*Params::PAWN_VALUE), score_t(32*Params::PAWN_VALUE) };

#define PARAM(x) Params::x
#define APARAM(x,index) Params::x[index]

CACHE_ALIGN Bitboard Scoring::kingProximity[2][64];
CACHE_ALIGN Bitboard Scoring::kingNearProximity[64];
CACHE_ALIGN Bitboard Scoring::kingPawnProximity[2][4][64]; // side,
                                                           // zone,
                                                           // square

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
static uint8_t is_outside[256][256];

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
   const int kpfile = File(kp);
   const int kprank = Rank(i,side);
   kingProximity[side][i] = Attacks::king_attacks[kp];
   kingProximity[side][i].set(kp);
   if (kprank <= 6) {
       for (int f=kpfile-1; f <= kpfile+1; f++) {
           kingProximity[side][i].set(MakeSquare(f,kprank+2,side));
       }
   }
   int z = 0;
   for (int r = kprank+1; r <= std::min(8,kprank+4); r++, z++) {
       for (int f=kpfile-1; f <= kpfile+1; f++) {
           Square sq = MakeSquare(f,r,side);
           kingPawnProximity[side][z][i].set(sq);
       }
   }
}

void Scoring::initBitboards() {
   int i, r;

   for(i = 0; i < 64; i++) {
      initProximity<White>(i);
      initProximity<Black>(i);
      int rank = Rank(i, White);
      int file = File(i);
      Square sq;

      for(r = rank - 1; r > 1; r--) {
         if (file != 8) {
            backwardW[i].set(MakeSquare(file + 1, r, White));
         }
         if (file != 1) {
            backwardW[i].set(MakeSquare(file - 1, r, White));
         }
      }

      for(r = rank + 1; r < 8; r++) {
         passedW[i].set(MakeSquare(file, r, White));
         if (file != 8) {
            sq = MakeSquare(file + 1, r, White);
            passedW[i].set(sq);
            outpostW[i].set(sq);
         }
         if (file != 1) {
            sq = MakeSquare(file - 1, r, White);
            passedW[i].set(sq);
            outpostW[i].set(sq);
         }
      }

      rank = Rank(i, Black);
      for(r = rank - 1; r > 1; r--) {
         if (file != 8) {
            backwardB[i].set(MakeSquare(file + 1, r, Black));
         }
         if (file != 1) {
            backwardB[i].set(MakeSquare(file - 1, r, Black));
         }
      }

      for(r = rank + 1; r < 8; r++) {
         passedB[i].set(MakeSquare(file, r, Black));
         if (file != 8) {
            sq = MakeSquare(file + 1, r, Black);
            passedB[i].set(sq);
            outpostB[i].set(sq);
         }
         if (file != 1) {
            sq = MakeSquare(file - 1, r, Black);
            passedB[i].set(sq);
            outpostB[i].set(sq);
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
   uint8_t near_left[8];
   uint8_t near_right[8];
   uint8_t right_mask[8];
   uint8_t left_mask[8];
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

void Scoring::adjustMaterialScore(const Board &board, ColorType side, Scores &scores) const 
{
    const ColorType oside = OppositeColor(side);
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(oside);
    const int pieceDiff = ourmat.materialLevel() - oppmat.materialLevel();
    const uint32_t pieces = ourmat.pieceBits();
    if (pieceDiff > 0 && (pieces == Material::KN || pieces == Material::KB)) {
        // Knight or Bishop vs pawns
        if (ourmat.pawnCount() == 0) {
            // no mating material. We can't be better but if
            // opponent has 1-2 pawns we are not so bad
            scores.any += APARAM(KN_VS_PAWN_ADJUST,std::min<int>(2,oppmat.pawnCount()));
        } else if (oppmat.pawnCount() == 0) {
            if (pieces == Material::KN && ourmat.pawnCount() == 1) {
                // KNP vs K is a draw, generally
                scores.any -= Params::KNIGHT_VALUE_ENDGAME;
                return;
            }
        }
        else if (pieces == Material::KB && ourmat.pawnCount() == 1) {
            int wrongBishop = (side == White) ? KBPDraw<White>(board) : KBPDraw<Black>(board);
            if (wrongBishop) {
                // We have the configuration of "wrong color bishop"
                // and rook pawn. Opponent may or may not have
                // pawns. We likely can't do better than draw, so
                // reduce score.
                scores.any -= Params::BISHOP_VALUE_ENDGAME;
                return;
            }
        }
    }
    if (ourmat.pieceBits() == Material::KB && oppmat.pieceBits() == Material::KB &&
        !(Attacks::rank7mask[side] & board.pawn_bits[side]) &&
        !(Attacks::rank7mask[oside] & board.pawn_bits[oside]) ) {
        // Bishop endgame: drawish
        scores.any -= Params::PAWN_VALUE_ENDGAME*(ourmat.pawnCount() - oppmat.pawnCount())/2;
        return;
    }
    switch(ourmat.majorCount() - oppmat.majorCount()) {
    case 0: {
        if (ourmat.queenCount() == oppmat.queenCount() &&
            ourmat.minorCount() == oppmat.minorCount()+1) {
            // we have extra minor
            if (!ourmat.hasPawns()) {
                // If we have no pawns, penalize being ahead but
                // in a drawish material configuration.
                if (oppmat.pieceBits() == Material::KR) {
                    // KR + minor vs KR - draw w. no pawns so lower score
                    scores.any += PARAM(KRMINOR_VS_R_NO_PAWNS);
                    return;
                }
                else if ((ourmat.pieceBits() == Material::KQN ||
                          ourmat.pieceBits() == Material::KQB) &&
                         oppmat.pieceBits() == Material::KQ) {
                    scores.any += PARAM(KQMINOR_VS_Q_NO_PAWNS);
                    return;
                }
            } else if (oppmat.pawnCount() > ourmat.pawnCount()) {
                // Knight or Bishop traded for pawns. Bonus for piece.
                scores.mid += PARAM(MINOR_FOR_PAWNS_MIDGAME);
                scores.end += PARAM(MINOR_FOR_PAWNS_ENDGAME);
#ifdef EVAL_DEBUG
                cout << "minors vs pawns (" << ColorImage(side) << "): (" << 
                    PARAM(MINOR_FOR_PAWNS_MIDGAME) << "," << PARAM(MINOR_FOR_PAWNS_ENDGAME) << ")" << endl;
#endif            
            }
        }
        if  (ourmat.queenCount() == oppmat.queenCount()+1 &&
                  ourmat.rookCount() == oppmat.rookCount() - 2) {
            // Queen vs. Rooks
            // Queen is better with minors on board (per Kaufman)
            scores.mid += PARAM(QR_ADJUST_MIDGAME);
            scores.end += PARAM(QR_ADJUST_ENDGAME);
        }
        break;
    }
    case 1: {
        if (ourmat.rookCount() == oppmat.rookCount()+1) {
            if (ourmat.minorCount() == oppmat.minorCount() - 1) {
                // Rook vs. minor
                // not as bad w. fewer pieces
                scores.mid += PARAM(RB_ADJUST_MIDGAME);
                scores.end += PARAM(RB_ADJUST_ENDGAME);
            }
            else if (ourmat.minorCount() == oppmat.minorCount() - 2) {
                // bad trade - Rook for two minors, but not as bad w. fewer pieces
                scores.mid += PARAM(RBN_ADJUST_MIDGAME);
                scores.end += PARAM(RBN_ADJUST_ENDGAME);
            }
        }
        // Q vs RB or RN is already dealt with by piece values
        break;
    }
    case 2:
        if (ourmat.hasQueen() && ourmat.rookCount() == oppmat.rookCount() &&
            oppmat.minorCount()-ourmat.minorCount() == 3) {
            // Queen vs. 3 minors
            scores.mid += PARAM(Q_VS_3MINORS_MIDGAME);
            scores.end += PARAM(Q_VS_3MINORS_ENDGAME);
        }
        break;
    default:
        break;
    }
    if (ourmat.materialLevel() < 16) {
        const int pieceDiff = ourmat.materialLevel() - oppmat.materialLevel();
        if (pieceDiff > 0) {
            // encourage trading pieces but not pawns.
            score_t adj = 0;
            adj += PARAM(TRADE_DOWN1) + (pieceDiff >= 5)*PARAM(TRADE_DOWN2);
            adj += std::min(3,ourmat.pawnCount())*PARAM(TRADE_DOWN3);
            scores.any += adj*(4-ourmat.materialLevel()/4);
        }
    }
}


void Scoring::adjustMaterialScoreNoPawns( const Board &board, ColorType side, Scores &scores) const
{
    // pawnless endgames. Generally drawish in many cases.
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    score_t score = 0;
    if (ourmat.infobits() == Material::KQ) {
        if (oppmat.infobits() == Material::KRR) {
            score -= (Params::QUEEN_VALUE_ENDGAME-2*Params::ROOK_VALUE_ENDGAME);  // even
        }
        else if (oppmat.infobits() == Material::KRB) {
           score -= Params::QUEEN_VALUE_ENDGAME-(Params::ROOK_VALUE_ENDGAME+Params::BISHOP_VALUE_ENDGAME); // even
        }
        else if (oppmat.infobits() == Material::KQB) {
            score += Params::BISHOP_VALUE_ENDGAME;  // even
        }
        else if (oppmat.infobits() == Material::KQN) {
            score += Params::KNIGHT_VALUE_ENDGAME;  // even
        }
        else if (oppmat.infobits() == Material::KRN) { // close to even
            score -= Params::QUEEN_VALUE_ENDGAME-(Params::ROOK_VALUE_ENDGAME+Params::BISHOP_VALUE_ENDGAME)-Params::PAWN_VALUE_ENDGAME/4;
        }
        else if (oppmat.infobits() == Material::KB ||
                 oppmat.infobits() == Material::KN ||
                 oppmat.infobits() == Material::KR) {
            // won for the stronger side
            score += 2*Params::PAWN_VALUE_ENDGAME;
        } else if (oppmat.infobits() == Material::KBBN) {
            // draw
            score -= Params::QUEEN_VALUE_ENDGAME - (2*Params::BISHOP_VALUE_ENDGAME + Params::KNIGHT_VALUE_ENDGAME);
        } else if (oppmat.infobits() == Material::KBNN) {
            // draw
            score -= Params::QUEEN_VALUE_ENDGAME - (2*Params::KNIGHT_VALUE_ENDGAME + Params::BISHOP_VALUE_ENDGAME);
        }
    }
    else if (ourmat.infobits() == Material::KR) {
        if (oppmat.infobits() == Material::KB) {
            score -= (Params::ROOK_VALUE_ENDGAME-Params::BISHOP_VALUE_ENDGAME);
        }
        else if (oppmat.infobits() == Material::KN) {
            score -= (Params::ROOK_VALUE_ENDGAME-Params::KNIGHT_VALUE_ENDGAME)-Params::PAWN_VALUE_ENDGAME/4;
        }
    }
    else if (ourmat.infobits() == Material::KRR) {
        if (oppmat.infobits() == Material::KRB) {
           // even
           score -= Params::ROOK_VALUE_ENDGAME-Params::BISHOP_VALUE_ENDGAME;
        }
        else if (oppmat.infobits() == Material::KRN) {
           score -= Params::ROOK_VALUE_ENDGAME-Params::KNIGHT_VALUE_ENDGAME-Params::PAWN_VALUE_ENDGAME/4;
        }
        else if (oppmat.infobits() == Material::KRBN) {
           score += (Params::KNIGHT_VALUE_ENDGAME+Params::BISHOP_VALUE_ENDGAME-Params::ROOK_VALUE_ENDGAME)-Params::PAWN_VALUE_ENDGAME/4;
        }
    }
    else if (ourmat.infobits() == Material::KBB) {
       if (oppmat.infobits() == Material::KB) {
          // about 85% draw
          score -= (Params::BISHOP_VALUE_ENDGAME-score_t(0.35*Params::PAWN_VALUE_ENDGAME));
       }
       else if (oppmat.infobits() == Material::KN) {
          // about 50% draw, 50% won for Bishops
          score -= score_t(1.5*Params::PAWN_VALUE_ENDGAME);
       }
       else if (oppmat.infobits() == Material::KR) {
          // draw
          score -= (2*Params::BISHOP_VALUE_ENDGAME - Params::ROOK_VALUE_ENDGAME);
       }
    } else if (ourmat.infobits() == Material::KBN) {
       if (oppmat.infobits() == Material::KN ||
           oppmat.infobits() == Material::KB) {
          // about 75% draw, a little less if opponent has Bishop
          score -= (Params::BISHOP_VALUE_ENDGAME - score_t(0.6*Params::PAWN_VALUE_ENDGAME) - (oppmat.hasBishop() ? score_t(0.15*Params::PAWN_VALUE_ENDGAME) : 0));
       } else if (oppmat.infobits() == Material::KR) {
          score -= (Params::KNIGHT_VALUE_ENDGAME + Params::BISHOP_VALUE_ENDGAME - Params::ROOK_VALUE_ENDGAME);
       }
    } else if (ourmat.infobits() == Material::KNN &&
               (oppmat.infobits() == Material::KN ||
                oppmat.infobits() == Material::KB ||
                oppmat.infobits() == Material::KR)) {
        // draw
       score -= (2*Params::KNIGHT_VALUE_ENDGAME - oppmat.knightCount()*Params::KNIGHT_VALUE_ENDGAME -
                 oppmat.bishopCount()*Params::BISHOP_VALUE_ENDGAME - oppmat.rookCount()*Params::ROOK_VALUE_ENDGAME);
    } else if (ourmat.infobits() == Material::KQR &&
               (oppmat.infobits() == Material::KQB ||
                oppmat.infobits() == Material::KQN)) {
       // draw
       score -= (Params::QUEEN_VALUE_ENDGAME + Params::ROOK_VALUE_ENDGAME - oppmat.knightCount()*Params::KNIGHT_VALUE_ENDGAME -
                 oppmat.bishopCount()*Params::BISHOP_VALUE_ENDGAME);
    }
    scores.any += score;
}

#ifdef TUNE
score_t Scoring::calcCover(const Board &board, ColorType side, int file, int rank, int (&counts)[6][4]) {
#else
score_t Scoring::calcCover(const Board &board, ColorType side, int file, int rank) {
#endif
   Square sq, pawn;
   score_t cover = PARAM(KING_COVER_BASE);
   if (rank > 2) return cover;
   const int f = Params::foldFile(file);
   sq = MakeSquare(file, 1, side);
   if (side == White) {
      Bitboard pawns(Attacks::file_mask_up[sq] & board.pawn_bits[White]);
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
         const int rank = Rank(pawn,side);
         ASSERT(rank >= 2);
         const int rank_dist = std::min<int>(3,rank - 2);
         cover += PARAM(KING_COVER)[rank_dist][f];
#ifdef TUNE
         counts[rank_dist][f]++;
#endif
      }
   }
   else {
      Bitboard pawns(Attacks::file_mask_down[sq] & board.pawn_bits[Black]);
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
         const int rank = Rank(pawn,side);
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
#ifdef TUNE
score_t Scoring::calcCover(const Board &board, ColorType side, Square kp, int (&counts)[6][4]) {
#else
score_t Scoring::calcCover(const Board &board, ColorType side, Square kp) {
#endif
   score_t cover = 0;
   int kpfile = File(kp);
   int rank = Rank(kp, side);
   if (kpfile > 5) {
      for(int i = 6; i <= 8; i++) {
#ifdef TUNE
         cover += calcCover(board, side, i, rank, counts);
#else
         cover += calcCover(board, side, i, rank);
#endif
      }
   }
   else if (kpfile < 4) {
      for(int i = 1; i <= 3; i++) {
#ifdef TUNE
         cover += calcCover(board, side, i, rank, counts);
#else
         cover += calcCover(board, side, i, rank);
#endif
      }
   }
   else {
      for(int i = kpfile - 1; i <= kpfile + 1; i++) {
#ifdef TUNE
         cover += calcCover(board, side, i, rank, counts);
#else
         cover += calcCover(board, side, i, rank);
#endif
      }
   }
   cover = std::min<score_t>(0, cover);
   return cover;
}

void Scoring::calcCover(const Board &board, ColorType side, KingPawnHashEntry &coverEntry) {
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
   score_t cover = calcCover(board, side, kp, king_cover);
#else
   score_t cover = calcCover(board, side, kp);
#endif
   switch(board.castleStatus(side))
   {
   case CanCastleEitherSide:
      {
#ifdef TUNE
         score_t k_cover = calcCover (board, side, side == White ? chess::G1 : chess::G8, kside_cover);
         score_t q_cover = calcCover (board, side, side == White ? chess::B1 : chess::B8, qside_cover);
#else
         score_t k_cover = calcCover (board, side, side == White ? chess::G1 : chess::G8);
         score_t q_cover = calcCover (board, side, side == White ? chess::B1 : chess::B8);
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
         score_t k_cover = calcCover (board, side, side == White ? chess::G1 : chess::G8, kside_cover);
#else
         score_t k_cover = calcCover (board, side, side == White ? chess::G1 : chess::G8);
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
         score_t q_cover = calcCover (board, side, side == White ? chess::B1 : chess::B8, qside_cover);
#else
         score_t q_cover = calcCover (board, side, side == White ? chess::B1 : chess::B8);
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

static int is_blocked(const Board &board, Square sq, ColorType side)
{
    if (side == Black) {
        return (board[sq-8] == WhitePawn);
    } else {
        return (board[sq+8] == BlackPawn);
    }
}

void Scoring::calcStorm(const Board &board, ColorType side, KingPawnHashEntry &coverEntry,
    const Bitboard &oppPawnAttacks) {
    const ColorType oside = OppositeColor(side);
    Square ksq = board.kingSquare(side);
    int krank = Rank(ksq,side);
    int kfile = File(ksq);
    if (kfile == 1) kfile = 2;
    if (kfile == 8) kfile = 7;
    coverEntry.storm = 0;
    const int pawn_attack_count = Bitboard(oppPawnAttacks & kingPawnProximity[side][0][ksq]).bitCount();
#ifdef TUNE
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 5; k++)
                coverEntry.storm_counts[i][j][k] = 0;
    coverEntry.pawn_attack_count = pawn_attack_count;
#endif
    coverEntry.pawn_attacks = PARAM(PAWN_ATTACK_FACTOR)*pawn_attack_count;
    for (int file = std::max<int>(1,kfile-1); file <= std::min<int>(8,kfile+1); file++) {
        // find nearest enemy pawn at or above King
        Square sq = MakeSquare(file,krank,side);
        Square opp_pawn = side == White ?
            (board.pawn_bits[oside] & Attacks::file_mask_up[sq]).firstOne() :
            (board.pawn_bits[oside] & Attacks::file_mask_down[sq]).lastOne();
        if (opp_pawn != InvalidSquare) {
            const int opp_dist = Rank(opp_pawn,side) - krank;
            if (opp_dist < 5) {
                const int folded_file = file > 4 ? 8 - file : file-1;
                const int blocked = is_blocked(board,opp_pawn,oside);
                coverEntry.storm += PARAM(PAWN_STORM)[blocked][folded_file][opp_dist];
#ifdef PAWN_DEBUG
                cout << "storm: " << ColorImage(side) << " " << SquareImage(opp_pawn) << " blocked=" << blocked << endl;
#endif
#ifdef TUNE
                coverEntry.storm_counts[blocked][folded_file][opp_dist]++;
#endif
            }
        }
    }
#ifdef PAWN_DEBUG
    cout << ColorImage(side) << " storm = " << coverEntry.storm << " pawn_attacks=" << coverEntry.pawn_attacks << endl;
#endif
}

score_t Scoring::outpost(const Board &board,
                     Square sq, ColorType side) const
{
   return Rank(sq,side) > 4 && ((side == White) ?
                                Bitboard(outpostW[sq] & board.pawn_bits[Black]).isClear() :
                                Bitboard(outpostB[sq] & board.pawn_bits[White]).isClear());
}

template <ColorType bishopColor>
void Scoring::scoreBishopAndPawns(const PawnHashEntry::PawnData &ourPawnData,const PawnHashEntry::PawnData &oppPawnData,Scores &scores,Scores &opp_scores)
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
      cout << "bad bishop (" << ColorImage(bishopColor) << "): (" <<
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
      cout << "bishop pawn placement: (" << ColorImage(OppositeColor(bishopColor)) << "): " << opp_scores.end - tmp << endl;
#endif
   }
}


template<ColorType side>
void Scoring::positionalScore(const Board &board,
                              const PawnHashEntry &pawnEntry,
                              const KingPawnHashEntry &ourKPEntry,
                              const KingPawnHashEntry &oppKPEntry,
                              AttackInfo &ai,
                              Scores &scores,
                              Scores &opp_scores) {


   const ColorType oside = OppositeColor(side);
   const PawnHashEntry::PawnData &ourPawnData = pawnEntry.pawnData(side);
   const PawnHashEntry::PawnData &oppPawnData = pawnEntry.pawnData(oside);

#ifdef EVAL_DEBUG
   cout << "scores for " << ColorImage(side) << ":" << endl;
#endif
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
   scores.mid += (ourKPEntry.cover == Constants::INVALID_SCORE ? 0 : ourKPEntry.cover);

   Bitboard b(board.occupied[side] &~board.pawn_bits[side]);
   b.clear(board.kingSquare(side));

   const bool early_endgame = board.getMaterial(side).materialLevel() <= PARAM(ENDGAME_THRESHOLD);
   const bool deep_endgame = board.getMaterial(side).materialLevel() <= PARAM(MIDGAME_THRESHOLD);

   score_t oppCover = oppKPEntry.cover == Constants::INVALID_SCORE ? 0 : oppKPEntry.cover;

   int pin_count = 0;
   Square kp = board.kingSquare(side);
   Square okp = board.kingSquare(oside);
   const Bitboard &nearKing(kingProximity[oside][okp]);
   score_t attackWeight = 0;
   int simpleAttackWeight = 0;
   unsigned kingAttackCount = 0;
   Square sq;

   ai.pawnAttacks[side] = oppPawnData.opponent_pawn_attacks;
   ai.allAttacks[side] |= oppPawnData.opponent_pawn_attacks;

   while(b.iterate(sq))
   {
#ifdef EVAL_DEBUG
      Scores tmp = scores;
#endif
      const Square pstSq = (side == White) ? sq : 63 - sq;
      switch(TypeOfPiece(board[sq]))
      {
      case Knight:
         {
            scores.mid += PARAM(KNIGHT_PST)[Midgame][pstSq] + PARAM(KNIGHT_VALUE_MIDGAME);
            scores.end += PARAM(KNIGHT_PST)[Endgame][pstSq] + PARAM(KNIGHT_VALUE_ENDGAME);

            const Bitboard &knattacks = Attacks::knight_attacks[sq];
            const score_t mobl = PARAM(KNIGHT_MOBILITY)[Bitboard(knattacks &~board.allOccupied &~ourPawnData.opponent_pawn_attacks).bitCount()];
#ifdef EVAL_DEBUG
            cout << "knight moblility =" << mobl << endl;
#endif
            scores.any += mobl;
            if (outpost(board,sq,side)) {
               auto defenders = std::min<int>(1,outpost_defenders(board,sq,side));
               score_t outpost_score_mid = Params::KNIGHT_OUTPOST[Midgame][defenders];
               scores.mid += outpost_score_mid;
               score_t outpost_score_end = Params::KNIGHT_OUTPOST[Endgame][defenders];
               scores.end += outpost_score_end;
#ifdef EVAL_DEBUG
               cout << "knight outpost (defenders=" <<
                   defenders << "): score (" << outpost_score_mid << ',' <<
                   outpost_score_end << ')' << endl;
#endif
            }
            ai.allAttacks[side] |= knattacks;
            ai.knightAttacks[side] |= knattacks;
            ai.minorAttacks[side] |= knattacks;
            ai.attackedBy2[side] |= (knattacks & ai.allAttacks[side]);
            if (!deep_endgame) {
               Bitboard kattacks(knattacks & nearKing);
               if (kattacks) {
                  int count = kattacks.bitCountOpt();
                  attackWeight += PARAM(MINOR_ATTACK_FACTOR) +
                      PARAM(MINOR_ATTACK_BOOST)*(count-1);
                  simpleAttackWeight += 4 + 2*std::max<int>(0,count-1);
                  ++kingAttackCount;
               }
            }
            break;
         }

      case Bishop:
         {
            scores.mid += PARAM(BISHOP_PST)[Midgame][pstSq] + PARAM(BISHOP_VALUE_MIDGAME);
            scores.end += PARAM(BISHOP_PST)[Endgame][pstSq] + PARAM(BISHOP_VALUE_ENDGAME);

            const Bitboard battacks(board.bishopAttacks(sq));
            ai.allAttacks[side] |= battacks;
            ai.bishopAttacks[side] |= battacks;
            ai.minorAttacks[side] |= battacks;
            ai.attackedBy2[side] |= (battacks & ai.allAttacks[side]);
            if (!deep_endgame) {
               Bitboard kattacks(battacks & nearKing);
               if (!kattacks && (battacks & board.queen_bits[side])) {
                   kattacks = board.bishopAttacks(sq, side) & nearKing;
               }
               if (kattacks) {
                   int count = kattacks.bitCountOpt();
                   attackWeight += PARAM(MINOR_ATTACK_FACTOR) +
                       PARAM(MINOR_ATTACK_BOOST)*(count-1);
                   simpleAttackWeight += 4 + 2*std::max<int>(0,count-1);
                   ++kingAttackCount;
               }
            }
            if (outpost(board,sq,side)) {
               auto defenders = std::min<int>(1,outpost_defenders(board,sq,side));
               score_t outpost_score_mid = Params::BISHOP_OUTPOST[Midgame][defenders];
               scores.mid += outpost_score_mid;
               score_t outpost_score_end = Params::BISHOP_OUTPOST[Endgame][defenders];
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
            scores.mid += PARAM(ROOK_PST)[Midgame][pstSq] + PARAM(ROOK_VALUE_MIDGAME);
            scores.end += PARAM(ROOK_PST)[Endgame][pstSq] + PARAM(ROOK_VALUE_ENDGAME);
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
            ai.allAttacks[side] |= rattacks;
            ai.rookAttacks[side] |= rattacks;
            ai.attackedBy2[side] |= (rattacks & ai.allAttacks[side]);
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
               int kfile = File(kp);
               if (mobl <= 5 && Rank(kp,side)==1 && Rank(sq,side)==1 && (kfile < chess::EFILE) == (file < kfile)) {
                  int hmobl = (board.rankAttacks(sq) & ~board.allOccupied).bitCountOpt();
                  // penalty for Rook trapped by King, similar to Stockfish
                  if (hmobl <= 3) {
                      if (board.canCastle(side)) {
                          scores.mid += PARAM(TRAPPED_ROOK);
                      } else {
                          scores.mid += PARAM(TRAPPED_ROOK_NO_CASTLE);
                      }
                  }
               }
               Bitboard kattacks(rattacks2 & nearKing);
               if (kattacks) {
                   int boost = std::max<int>(0,Bitboard(kattacks & kingNearProximity[okp]).bitCountOpt()-1);
                   attackWeight += PARAM(ROOK_ATTACK_FACTOR) +
                       PARAM(ROOK_ATTACK_BOOST)*boost;
                   simpleAttackWeight += 5 + 3*boost;
                   ++kingAttackCount;
               }
            }
            if (board.pinOnRankOrFile(sq, okp, oside)) {
               pin_count++;
            }
            break;
         }

      case Queen:
         {
            scores.mid += PARAM(QUEEN_PST)[Midgame][pstSq] + PARAM(QUEEN_VALUE_MIDGAME);
            scores.end += PARAM(QUEEN_PST)[Endgame][pstSq] + PARAM(QUEEN_VALUE_ENDGAME);
            int qmobl = 0;
            Bitboard battacks(board.bishopAttacks(sq));
            Bitboard qmobility(battacks);
            Bitboard kattacks;

            if (board.pinOnDiag(sq, okp, oside)) {
               pin_count++;
            }

            Bitboard rattacks(board.rookAttacks(sq));
            qmobility |= rattacks;
            ai.allAttacks[side] |= qmobility;
            ai.attackedBy2[side] |= (qmobility & ai.allAttacks[side]);
            ai.queenAttacks[side] |= qmobility;
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
               if (!kattacks && (battacks & (board.bishop_bits[side] | board.queen_bits[side]))) {
                  kattacks = board.bishopAttacks(sq, side) & nearKing;
               }
               if (rattacks & nearKing) {
                  kattacks |= (rattacks & nearKing);
               }
               else if (rattacks & (board.queen_bits[side] | board.rook_bits[side])) {
                  kattacks |= (board.rookAttacks(sq, side) & nearKing);
               }
               if (kattacks) {
                   int boost = std::max<int>(0,Bitboard(kattacks & kingNearProximity[okp]).bitCountOpt()-1);
                   attackWeight += PARAM(QUEEN_ATTACK_FACTOR) +
                       PARAM(QUEEN_ATTACK_BOOST)*boost;
                   simpleAttackWeight += 6 + 4*boost;
                   ++kingAttackCount;
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
         scoreBishopAndPawns<White>(ourPawnData,oppPawnData,scores,opp_scores);
      }
      else {
         scoreBishopAndPawns<Black>(ourPawnData,oppPawnData,scores,opp_scores);
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
   scores.mid += PARAM(KING_PST)[Midgame][side == White ? kp : 63 - kp];

   ai.allAttacks[side] |= Attacks::king_attacks[kp];
   ai.attackedBy2[side] |= (Attacks::king_attacks[kp] & ai.allAttacks[side]);

   if (early_endgame) {
      int mobl = std::min<int>(4,Bitboard(Attacks::king_attacks[okp] & ~board.allOccupied & ~ai.allAttacks[side]).bitCount());
      opp_scores.end += PARAM(KING_MOBILITY_ENDGAME)[mobl];
#ifdef EVAL_DEBUG
      cout << ColorImage(oside) << " king mobility: " << PARAM(KING_MOBILITY_ENDGAME)[mobl] << endl;
#endif
   }
   if (!deep_endgame) {
      // add in pawn attack measure (from king/pawn hash table)
      attackWeight += oppKPEntry.pawn_attacks + oppKPEntry.storm;
      // add "boost" factor due to damaged king cover
      attackWeight += PARAM(KING_ATTACK_COVER_BOOST_BASE) - oppCover*PARAM(KING_ATTACK_COVER_BOOST_SLOPE)/Params::PAWN_VALUE;

      Bitboard kingAttackSquares(kingNearProximity[okp] & ai.allAttacks[side]);

      attackWeight += PARAM(KING_ATTACK_COUNT)*kingAttackCount +
          8*PARAM(KING_ATTACK_SQUARES)*kingAttackSquares.bitCount()/kingNearProximity[okp].bitCount();

      const score_t index = std::max<score_t>(0,attackWeight/Params::KING_ATTACK_FACTOR_RESOLUTION);

      simpleAttackWeight /= 2;
      if (simpleAttackWeight >= PARAM(OWN_PIECE_KING_PROXIMITY_MIN)) {
         // Opposing side is under attack, evaluate its own pieces'
         // proximity to their King
         int minorProx = Bitboard(nearKing & (board.knight_bits[oside] | board.bishop_bits[oside])).bitCountOpt();
         if (minorProx>2) minorProx -= (minorProx-2)/2;
         int rookProx = Bitboard(nearKing & board.rook_bits[oside]).bitCountOpt();
         Bitboard qbits(board.queen_bits[oside]);
         Square sq;
         int queenProx = 0;
         while (qbits.iterate(sq)) {
            queenProx += 4-distance(okp,sq);
         }
         opp_scores.mid += PARAM(OWN_PIECE_KING_PROXIMITY_MULT)[std::min<int>(15,simpleAttackWeight)]*
            (PARAM(OWN_MINOR_KING_PROXIMITY)*minorProx +
             PARAM(OWN_ROOK_KING_PROXIMITY)*rookProx +
             PARAM(OWN_QUEEN_KING_PROXIMITY)*queenProx)/64;
      }

#ifdef ATTACK_DEBUG
      cout << ColorImage(side) << " piece attacks on opposing king:" << endl;
      cout << " cover= " << oppCover << endl;
      cout << " king attack count=" << kingAttackCount << endl;
      cout << " king squares=" << kingAttackSquares.bitCount() << endl;
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
}

template <ColorType side>
void Scoring::threatScore(const Board &board,
                          const AttackInfo &ai,
                          Scores &scores)
{
    const bool earlyEndgame = board.getMaterial(side).materialLevel() <= PARAM(ENDGAME_THRESHOLD);

    const ColorType oside = OppositeColor(side);

    // Weak pieces/pawns are not defended at all or not defended by pawns
    const Bitboard weak((ai.allAttacks[side] & ~ai.allAttacks[oside]) | ~ai.pawnAttacks[oside]);

    Bitboard nonPawns(board.occupied[oside] & ~board.pawn_bits[oside]);

    Bitboard safe(~ai.allAttacks[oside] | ai.allAttacks[side]);

    Bitboard targets(board.occupied[oside] & weak);
    targets.clear(board.kingSquare(oside));

    Bitboard weakPawns(board.pawn_bits[oside] & weak);

    Bitboard weakMinors((board.knight_bits[oside] | board.bishop_bits[oside]) & weak);

    // Threats against pieces by safe pawns
    Bitboard pawnThreats(board.allPawnAttacks(side,board.pawn_bits[side] & safe) & nonPawns);
    Square sq;
    while (pawnThreats.iterate(sq)) {
        ASSERT(TypeOfPiece(board[sq])!=Empty);
        scores.mid += Params::THREAT_BY_PAWN[Midgame][TypeOfPiece(board[sq])-2];
        scores.end += Params::THREAT_BY_PAWN[Endgame][TypeOfPiece(board[sq])-2];
    }
    // Knight attacks on Bishops, higher-valued pieces and weak pieces
    Bitboard knightAttacks(ai.knightAttacks[side] & (weakMinors | board.rook_bits[oside] | board.queen_bits[oside] | weakPawns));
    while (knightAttacks.iterate(sq)) {
        ASSERT(TypeOfPiece(board[sq])!=Empty);
        scores.mid += Params::THREAT_BY_KNIGHT[Midgame][TypeOfPiece(board[sq])-1];
        scores.end += Params::THREAT_BY_KNIGHT[Endgame][TypeOfPiece(board[sq])-1];
    }
    // Bishop attacks on Knights, higher-valued pieces and weak pieces
    Bitboard bishopAttacks(ai.bishopAttacks[side] & (weakMinors | board.rook_bits[oside] | board.queen_bits[oside] | weakPawns));
    while (bishopAttacks.iterate(sq)) {
        ASSERT(TypeOfPiece(board[sq])!=Empty);
        scores.mid += Params::THREAT_BY_BISHOP[Midgame][TypeOfPiece(board[sq])-1];
        scores.end += Params::THREAT_BY_BISHOP[Endgame][TypeOfPiece(board[sq])-1];
    }
    // rook attacks on queens and weak pieces
    Bitboard rookAttacks(ai.rookAttacks[side] & (board.queen_bits[oside] | targets));
    while (rookAttacks.iterate(sq)) {
        ASSERT(TypeOfPiece(board[sq])!=Empty);
        scores.mid += Params::THREAT_BY_ROOK[Midgame][TypeOfPiece(board[sq])-1];
        scores.end += Params::THREAT_BY_ROOK[Endgame][TypeOfPiece(board[sq])-1];
    }
    // Pawn push threats as in Stockfish/Ethereal
    // compute pawn push destination square bitboard
    Bitboard pawns;
    if (side == White) {
        pawns = board.pawn_bits[White];
        pawns.shl8();
        // exclude promotions
        pawns &= ~(board.allOccupied | Attacks::rank_mask[7]);
        Square sq;
        Bitboard pawns2(pawns & Attacks::rank_mask[1]);
        while (pawns2.iterate(sq)) {
            if (board[sq+8] == EmptyPiece)
                pawns.set(sq+8);
        }
    }
    else {
        pawns = board.pawn_bits[Black];
        pawns.shr8();
        // exclude promotions
        pawns &= ~(board.allOccupied | Attacks::rank_mask[0]);
        Square sq;
        Bitboard pawns2(pawns & Attacks::rank_mask[6]);
        while (pawns2.iterate(sq)) {
            if (board[sq-8] == EmptyPiece)
                pawns.set(sq-8);
        }
    }
    int pawnPushThreats = Bitboard(board.allPawnAttacks(side,pawns) & safe & ~ai.pawnAttacks[oside] & nonPawns).bitCountOpt();
    scores.end += PARAM(PAWN_PUSH_THREAT_END)*pawnPushThreats;
    scores.mid += PARAM(PAWN_PUSH_THREAT_MID)*pawnPushThreats;
    if (earlyEndgame) {
        // King attacks on undefended or poorly defended pawns or pieces
        Bitboard kattacks(Attacks::king_attacks[board.kingSquare(side)] & board.occupied[oside] & weak);
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
#ifdef TUNE
   int count = 0;
#endif
   while(bi.iterate(sq))
   {
#ifdef TUNE
      PawnDetails &details = entr.details;
      details[count].sq = sq;
      details[count].flags = 0;
      details[count].space_weight = 0;
      ASSERT(count<8);
      PawnDetail &td = details[count++];
#endif
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

      entr.midgame_score += PARAM(PAWN_VALUE_MIDGAME);
      entr.endgame_score += PARAM(PAWN_VALUE_ENDGAME);

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
      const auto fileIndex = Params::foldFile(file);
      if (doubles) {
#ifdef PAWN_DEBUG
         cout << " doubled";
#endif
         if (passed) {
#ifdef PAWN_DEBUG
            cout << " passed";
#endif
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
            entr.midgame_score += PARAM(TRIPLED_PAWNS)[Midgame][fileIndex];
            entr.endgame_score += PARAM(TRIPLED_PAWNS)[Endgame][fileIndex];
         }
         else {
#ifdef TUNE
            td.flags |= PawnDetail::DOUBLED;
#endif
            entr.midgame_score += PARAM(DOUBLED_PAWNS)[Midgame][fileIndex];
            entr.endgame_score += PARAM(DOUBLED_PAWNS)[Endgame][fileIndex];
         }
         doubled++;
      }

      if (isolated && !passed) {
         entr.midgame_score += PARAM(ISOLATED_PAWN)[Midgame][fileIndex];
         entr.endgame_score += PARAM(ISOLATED_PAWN)[Endgame][fileIndex];
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
         int f = Params::foldFile(file);
         entr.midgame_score += PARAM(PASSED_PAWN)[Midgame][rank]*PARAM(PASSED_PAWN_FILE_ADJUST)[f]/64;
         entr.endgame_score += PARAM(PASSED_PAWN)[Endgame][rank]*PARAM(PASSED_PAWN_FILE_ADJUST)[f]/64;
         entr.passers.set(sq);
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
#ifdef PAWN_DEBUG
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

void Scoring::evalOutsidePassers(PawnHashEntry &pawnEntry) {
   uint8_t all_pawns = pawnEntry.wPawnData.pawn_file_mask | pawnEntry.bPawnData.pawn_file_mask;
   pawnEntry.wPawnData.outside = is_outside[pawnEntry.wPawnData.passer_file_mask][all_pawns];
   pawnEntry.bPawnData.outside = is_outside[pawnEntry.bPawnData.passer_file_mask][all_pawns];
   return;
}

void Scoring::calcPawnEntry(const Board &board, PawnHashEntry &pawnEntry) {

   // pawn position not found, calculate the data we need
   calcPawnData(board, White, pawnEntry.wPawnData);
   calcPawnData(board, Black, pawnEntry.bPawnData);
   evalOutsidePassers(pawnEntry);
   pawnEntry.hc = board.pawnHash();
}

score_t Scoring::evalu8(const Board &board, bool useCache) {

   score_t score;

   if ((score = tryBitbase(board)) != Constants::INVALID_SCORE) {
       return score;
   }

   Scores wScores, bScores;
   const Material &wMat = board.getMaterial(White);
   const Material &bMat = board.getMaterial(Black);

   const int w_materialLevel = wMat.materialLevel();
   const int b_materialLevel = bMat.materialLevel();

   if (wMat.infobits() != bMat.infobits()) {
       if (wMat.noPawns() && bMat.noPawns()) {
           adjustMaterialScoreNoPawns(board,White,wScores);
           adjustMaterialScoreNoPawns(board,Black,bScores);
       }
       else {
           adjustMaterialScore(board,White,wScores);
           adjustMaterialScore(board,Black,bScores);
       }
   }
#ifdef EVAL_DEBUG
   score_t adjusted = wScores.blend(b_materialLevel) - bScores.blend(w_materialLevel);                                      
   cout << "adjusted material score = " << (board.sideToMove() == White ? adjusted : -adjusted) << endl;
#endif
   const hash_t pawnHash = board.pawnHashCodeW ^ board.pawnHashCodeB;

   PawnHashEntry &pawnEntry = pawnHashTable[pawnHash % PAWN_HASH_SIZE];

   if (!useCache || pawnEntry.hc != pawnHash) {
      // Not found in table, need to calculate
      calcPawnEntry(board, pawnEntry);
   }

   KingPawnHashEntry &whiteKPEntry = getKPEntry<White>(board,pawnEntry.pawnData(White),
                                                       pawnEntry.pawnData(Black),useCache);
   KingPawnHashEntry &blackKPEntry = getKPEntry<Black>(board,pawnEntry.pawnData(Black),
                                                       pawnEntry.pawnData(White),useCache);

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
          ASSERT(whiteKPEntry.king_endgame_position != Constants::INVALID_SCORE);
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
          ASSERT(blackKPEntry.king_endgame_position != Constants::INVALID_SCORE);
          scoreEndgame<Black>(board,
                              blackKPEntry.king_endgame_position, bScores);
#ifdef EVAL_DEBUG
          cout << "endgame score (Black)=" << bScores.end - tmp << endl;
#endif
      }
   }

   if (posEval) {
       AttackInfo ai;
       // compute positional scores
       positionalScore<White>(board, pawnEntry, whiteKPEntry, blackKPEntry,
                              ai,
                              wScores, bScores);
       positionalScore<Black>(board, pawnEntry, blackKPEntry, whiteKPEntry,
                              ai,
                              bScores, wScores);
       // now that we have attack info, calculate threat scores
       threatScore<White>(board, ai, wScores);
       threatScore<Black>(board, ai, bScores);
       // And also pawn scores, which use the attack info
       pawnScore(board, White, pawnEntry.pawnData(White), ai, wScores);
       pawnScore(board, Black, pawnEntry.pawnData(Black), ai, bScores);
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

void Scoring::pawnScore(const Board &board, ColorType side, const PawnHashEntry::PawnData &pawnData,
                        const AttackInfo &ai, Scores &scores) {
#ifdef PAWN_DEBUG
   Scores tmp(scores);
#endif
   const ColorType oside = OppositeColor(side);

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
#ifdef EVAL_DEBUG
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
#ifdef EVAL_DEBUG
      if ((mid_tmp != scores.mid) ||
          (end_tmp != scores.end)) {
         cout << "rook/passed pawn placement (" << ColorImage(side) << ") (";
         cout << scores.mid - mid_tmp << ", " <<
            scores.end - end_tmp << ")" << endl;
      }
#endif
      Bitboard ahead = (side == White) ? Attacks::file_mask_up[sq] :
         Attacks::file_mask_down[sq];
#ifdef EVAL_DEBUG
      mid_tmp = scores.mid;
      end_tmp = scores.end;
#endif
      if (!(ai.allAttacks[oside] & ahead) && !(board.allOccupied & ahead)) {
          scores.mid += PARAM(QUEENING_PATH_CLEAR)[Midgame][rank-2];
          scores.end += PARAM(QUEENING_PATH_CLEAR)[Endgame][rank-2];
      }
#ifdef EVAL_DEBUG
      if ((mid_tmp != scores.mid) ||
          (end_tmp != scores.end)) {
         cout << "queening path clear (" << ColorImage(side) << ") (";
         cout << scores.mid - mid_tmp << ", " <<
            scores.end - end_tmp << ")" << endl;
      }
      mid_tmp = scores.mid;
      end_tmp = scores.end;
#endif
      if (rank >= 5) {
          // evaluate control of next square to advance to
          Square pathSq = MakeSquare(file, rank+1, side);
          if (board.occupied[side].isSet(pathSq)) {
              scores.mid += PARAM(PP_OWN_PIECE_BLOCK)[Midgame][rank-5];
              scores.end += PARAM(PP_OWN_PIECE_BLOCK)[Endgame][rank-5];
          } else if (board.occupied[oside].isSet(pathSq)) {
              scores.mid += PARAM(PP_OPP_PIECE_BLOCK)[Midgame][rank-5];
              scores.end += PARAM(PP_OPP_PIECE_BLOCK)[Endgame][rank-5];
          } else {
              if (ai.allAttacks[side].isSet(pathSq)) {
                  scores.mid += PARAM(QUEENING_PATH_CONTROL)[Midgame][rank-5];
                  scores.end += PARAM(QUEENING_PATH_CONTROL)[Endgame][rank-5];
              }
              if (ai.allAttacks[oside].isSet(pathSq)) {
                  scores.mid += PARAM(QUEENING_PATH_OPP_CONTROL)[Midgame][rank-5];
                  scores.end += PARAM(QUEENING_PATH_OPP_CONTROL)[Endgame][rank-5];
              }
          }
      }
#ifdef EVAL_DEBUG
      if ((mid_tmp != scores.mid) ||
          (end_tmp != scores.end)) {
         cout << "queening path control (" << ColorImage(side) << ") (";
         cout << scores.mid - mid_tmp << ", " <<
            scores.end - end_tmp << ")" << endl;
      }
      mid_tmp = scores.mid;
      end_tmp = scores.end;
#endif
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
         ASSERT(!IsInvalid(sq));

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

void Scoring::calcKingEndgamePosition(const Board &board, ColorType side,
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
   score_t k_pos = PARAM(KING_PST)[Endgame][(side == White) ? kp : 63 - kp];
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
      ASSERT(rank>=2 && rank<8);
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
   bool needCover = mLevel > PARAM(MIDGAME_THRESHOLD);
   bool needEndgame = mLevel <= PARAM(ENDGAME_THRESHOLD);
   if (!useCache || (entry.hc != kphash)) {
      if (needCover) {
         calcCover(board,side,entry);
         calcStorm(board,side,entry,ourPawnData.opponent_pawn_attacks);
      } else {
         entry.cover = entry.storm = Constants::INVALID_SCORE;
      }
      if (needEndgame) {
         calcKingEndgamePosition(board,side,oppPawnData,entry);
      } else {
         entry.king_endgame_position = Constants::INVALID_SCORE;
      }
      entry.hc = kphash;
   }
   else {
      if (needCover && entry.cover == Constants::INVALID_SCORE) {
         calcCover(board,side,entry);
         calcStorm(board,side,entry,ourPawnData.opponent_pawn_attacks);
      }
      if (needEndgame && entry.king_endgame_position == Constants::INVALID_SCORE) {
         calcKingEndgamePosition(board,side,oppPawnData,entry);
      }
#ifdef _DEBUG
      // cached entry better = computed entry
      KingPawnHashEntry entry2;
      entry2.cover = entry2.king_endgame_position = 0;
      calcCover(board,side,entry2);
      calcStorm(board,side,entry2,ourPawnData.opponent_pawn_attacks);
      calcKingEndgamePosition(board,side,oppPawnData,entry2);
      if (needCover && ((entry.cover != entry2.cover) || (entry.storm != entry2.storm))) {
         cout << board << endl;
         cout << "mLevel=" << mLevel << endl;
         cout << "cover1=" << entry.cover << " cover2=" << entry2.cover << endl;
         cout << "storm1=" << entry.storm << " storm2=" << entry2.storm << endl;
         ASSERT(entry.cover == entry2.cover);
         ASSERT(entry.storm == entry2.storm);
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

bool Scoring::theoreticalDraw(const Board &board) {
    if (board.getMaterial(White).totalMaterialLevel() >
        board.getMaterial(Black).totalMaterialLevel())
        return theoreticalDraw<White>(board);
    else if (board.getMaterial(White).totalMaterialLevel() <
             board.getMaterial(Black).totalMaterialLevel())
        return theoreticalDraw<Black>(board);
    else
        return false;
}

// check for theoretical draws ("side" has the greater material)
template <ColorType side>
bool Scoring::theoreticalDraw(const Board &board) {
    const Material &mat1 = board.getMaterial(side);
    const Material &mat2 = board.getMaterial(OppositeColor(side));
    if (mat1.materialLevel() > 6) return false;

    // Check for K + P vs rook pawn
    if (mat1.infobits() == Material::KP && mat2.kingOnly()) {
        if (TEST_MASK(rook_pawn_mask, board.pawn_bits[side])) {
            Square kp = board.kingSquare(side);
            Square kp2 = board.kingSquare(OppositeColor(side));
            Square psq = (board.pawn_bits[side].firstOne());
            ASSERT(OnBoard(kp));
            ASSERT(OnBoard(kp2));
            ASSERT(OnBoard(psq));
            return lookupBitbase(kp, psq, kp2, side, board.sideToMove()) == 0;
        }
        else {
            return false;
        }
    }
    // Check for wrong bishop + rook pawn(s) vs king.  Not very common but
    // we don't want to get it wrong.
    else if (mat1.pieceBits() == Material::KB && mat2.kingOnly()) {
        return KBPDraw<side> (board);
    }
    // check for KNN vs K
    else if (mat1.infobits() == Material::KNN && mat2.kingOnly()) {
        return true;
    }
    else {
        return false;
    }
}

bool Scoring::isDraw(const Board &board, int &rep_count, int ply) {

   // First check for draw by repetition
   rep_count = 0;

   // follow rule Crafty uses: 2 repeats if ply<=2, 1 otherwise:
   const int target = (ply <= 2) ? 2 : 1;
   if ((rep_count=board.repCount(target))>=target) {
      return true;
   }
   else if (board.fiftyMoveDraw()) {
      return true;
   }
   else if (board.getMaterial(White).materialLevel() <= 6 &&
            board.getMaterial(Black).materialLevel() <= 6) {
      // check for insufficient material and other drawing situations
      return board.materialDraw() || theoreticalDraw(board);
   } else {
      return false;
   }
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
      ASSERT(!IsInvalid(passer));
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

   return Constants::INVALID_SCORE;
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
   time_t rawtime;
   struct tm * tminfo;
   time (&rawtime);
   tminfo = localtime (&rawtime);
   o << "// Copyright 2015-" << tminfo->tm_year+1900 << " by Jon Dart. All Rights Reserved." << endl;
   o << "// This is a generated file. Do not edit." << endl;
   o << "// " << comment << endl;
   o << "//" << endl;
   o << endl << "#include \"params.h\"" << endl;
   o << endl;
   o << "const int Params::KN_VS_PAWN_ADJUST[3] = ";
   print_array(o,Params::KN_VS_PAWN_ADJUST,3);
   o << "const int Params::KING_COVER[6][4] = {";
   for (int i = 0; i < 6; i++) {
      print_array(o,Params::KING_COVER[i],4,0);
      if (i<5) o << "," << endl;
   }
   o << "};" << endl;
   for (int i = Tune::PAWN_VALUE_MIDGAME; i <= Tune::QUEEN_VALUE_ENDGAME; i++) {
      o << "const int Params::";
      for (auto it : tune_params[i].name) {
         o << (char)toupper((int)it);
      }
      o << " = " << std::round(tune_params[i].current) << ";" << endl;
   }
   int start = Tune::KING_COVER_BASE;
   for (int i = start; i < start+tune_params.paramArraySize(); i++) {
      o << "const int Params::";
      for (auto it : tune_params[i].name) {
         o << (char)toupper((int)it);
      }
      o << " = " << std::round(tune_params[i].current) << ";" << endl;
   }
   o << "const int Params::KING_OPP_PASSER_DISTANCE[6] = ";
   print_array(o,Params::KING_OPP_PASSER_DISTANCE,6);
   o << "const int Params::KING_POSITION_LOW_MATERIAL[3] =";
   print_array(o,Params::KING_POSITION_LOW_MATERIAL,3);
   o << "const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = ";
   print_array(o,Params::KING_ATTACK_SCALE,Params::KING_ATTACK_SCALE_SIZE);
   o << "const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = ";
   print_array(o,Params::OWN_PIECE_KING_PROXIMITY_MULT,16);
   o << "const int Params::PASSED_PAWN[2][8] = ";
   print_array(o,Params::PASSED_PAWN[0], Params::PASSED_PAWN[1], 8);
   o << "const int Params::PASSED_PAWN_FILE_ADJUST[4] = ";
   print_array(o,Params::PASSED_PAWN_FILE_ADJUST,4);
   o << "const int Params::CONNECTED_PASSER[2][8] = ";
   print_array(o,Params::CONNECTED_PASSER[0], Params::CONNECTED_PASSER[1], 8);
   o << "const int Params::ADJACENT_PASSER[2][8] = ";
   print_array(o,Params::ADJACENT_PASSER[0], Params::ADJACENT_PASSER[1], 8);
   o << "const int Params::QUEENING_PATH_CLEAR[2][6] = ";
   print_array(o,Params::QUEENING_PATH_CLEAR[0], Params::QUEENING_PATH_CLEAR[1], 6);
   o << "const int Params::PP_OWN_PIECE_BLOCK[2][3] = ";
   print_array(o,Params::PP_OWN_PIECE_BLOCK[0], Params::PP_OWN_PIECE_BLOCK[1], 3);
   o << "const int Params::PP_OPP_PIECE_BLOCK[2][3] = ";
   print_array(o,Params::PP_OPP_PIECE_BLOCK[0], Params::PP_OPP_PIECE_BLOCK[1], 3);
   o << "const int Params::QUEENING_PATH_CONTROL[2][3] = ";
   print_array(o,Params::QUEENING_PATH_CONTROL[0], Params::QUEENING_PATH_CONTROL[1], 3);
   o << "const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = ";
   print_array(o,Params::QUEENING_PATH_OPP_CONTROL[0], Params::QUEENING_PATH_OPP_CONTROL[1], 3);
   o << "const int Params::DOUBLED_PAWNS[2][8] = ";
   print_array(o,Params::DOUBLED_PAWNS[0], Params::DOUBLED_PAWNS[1], 8);
   o << "const int Params::TRIPLED_PAWNS[2][8] = ";
   print_array(o,Params::TRIPLED_PAWNS[0], Params::TRIPLED_PAWNS[1], 8);
   o << "const int Params::ISOLATED_PAWN[2][8] = ";
   print_array(o,Params:: ISOLATED_PAWN[0], Params::ISOLATED_PAWN[1], 8);
   o << endl;
   o << "const int Params::THREAT_BY_PAWN[2][5] = ";
   print_array(o,Params::THREAT_BY_PAWN[0],Params::THREAT_BY_PAWN[1],5);
   o << "const int Params::THREAT_BY_KNIGHT[2][5] = ";
   print_array(o,Params::THREAT_BY_KNIGHT[0],Params::THREAT_BY_KNIGHT[1],5);
   o << "const int Params::THREAT_BY_BISHOP[2][5] = ";
   print_array(o,Params::THREAT_BY_BISHOP[0],Params::THREAT_BY_BISHOP[1],5);
   o << "const int Params::THREAT_BY_ROOK[2][5] = ";
   print_array(o,Params::THREAT_BY_ROOK[0],Params::THREAT_BY_ROOK[1],5);
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
   o << "const int Params::KNIGHT_OUTPOST[2][2] = ";
   print_array(o,Params::KNIGHT_OUTPOST[0],Params::KNIGHT_OUTPOST[1],2);
   o << "const int Params::BISHOP_OUTPOST[2][2] = ";
   print_array(o,Params::BISHOP_OUTPOST[0],Params::BISHOP_OUTPOST[1],2);
   o << "const int Params::PAWN_STORM[2][4][5] = {";
   for (int b = 0; b < 2; b++) {
       o << '{';
       for (int f = 0; f < 4; f++) {
           print_array(o,Params::PAWN_STORM[b][f],5,0);
           if (f < 3) o << ',';
       }
       o << '}';
       if (b < 1) o << ',';
   }
   o << "};" << endl;
}

#endif
