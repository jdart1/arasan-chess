// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "boardio.h"
#include "notation.h"
#include "legal.h"
#include "hash.h"
#include "globals.h"
#include "chessio.h"
#include "util.h"
#include "search.h"
#include "tune.h"
#include "tuner.h"
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <vector>
#include <unordered_map>
#include <thread>
#include <atomic>
extern "C" {
#include <math.h>
#include <ctype.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
};

// Parameter tuning code for Arasan.
// Implements the "Texel" tuning method.
//
// Note: this file requires a C++11 compatible compiler/libraries.

using namespace tuner;

static bool regularize = false;

enum class OptimMethod { AdaGrad, ADAM, Adaptive };

static OptimMethod method = OptimMethod::Adaptive;

static int iterations = 2;

static int cores = 1;

static string out_file_name="params.cpp";

static string x0_file_name="x0";

static string pos_file_name = "games.fen";

static ifstream pos_file;

static bool verbose = false;
static bool validate = false;
static bool recalc = false;

static const int MAX_PV_LENGTH = 256;
static const int NUM_RESULT = 8;
static const int MAX_CORES = 64;
static const int THREAD_STACK_SIZE = 12*1024*1024;
static const int LEARNING_SEARCH_DEPTH = 1;
static int LEARNING_SEARCH_WINDOW = 3*PAWN_VALUE;
// L2-regularization factor
static const double REGULARIZATION = 6E-5;
static const int PV_RECALC_INTERVAL = 16;
static const int MIN_PLY = 16;
static const double ADAGRAD_FUDGE_FACTOR = 1.0e-9;
// step size relative to parameter range:
static const double ADAGRAD_STEP_SIZE = 0.01;
static const double PARAM1 = 0.75;
// step size relative to parameter range:
static const double ADAM_ALPHA = 0.01;
static const double ADAM_BETA1 = 0.9;
static const double ADAM_BETA2 = 0.999;
static const double ADAM_EPSILON = 1.0e-8;
// for ordinal regression
static const double THETA1 = -0.75;
static const double THETA2 = 0.75;

static const int ADAPTIVE_STEP_BASE = 0.04;
static const int ADAPTIVE_STEP_FACTOR1 = 0.2;
static const int ADAPTIVE_STEP_FACTOR2 = 0.8;

static const char *CASTLE_STATUS_KEY = "c1";
static const char *RESULT_KEY = "c2";

enum class Objective {
   Msq, Ordinal
};

static Objective obj = Objective::Msq;

static vector<GameInfo *> tmpdata;

static atomic<int> phase2_game_index;

static string cmdline;

LockDefine(file_lock);
LockDefine(data_lock);
LockDefine(hash_lock);

#ifdef _POSIX_VERSION
static pthread_attr_t stackSizeAttrib;
#endif

struct PosInfo
{
   PosInfo(const string &pos, int ws, int bs, double res)
      :position(pos),wstatus(ws),bstatus(bs),result(res)
      {
      }

   PosInfo(const string &pos, double res)
      :position(pos),wstatus(5),bstatus(5),result(res)
      {
      }

   string position;
   int wstatus,bstatus;
   double result;
};

static vector<PosInfo *> positions;

struct PositionDupEntry
{
   PositionDupEntry() : val(0)
      {
      }

   int val;
};

static unordered_map<hash_t,PositionDupEntry> *hash_table;

enum Phase {Phase1, Phase2};

struct ThreadData {
    SearchController *searcher;
    int index;
    Phase phase;
    void *data;
    double penalty;
    THREAD thread_id;
};

struct Parse1Data
{
   // accumulated stats for this phase:
   double target;

   void clear() {
      target = 0.0;
   }

};

struct Parse2Data
{
   unsigned int id;

   // holds accumulated derivatives for the scoring parameters:
   vector <double> grads;
   double target;
};

static std::thread threads[MAX_CORES];
static ThreadData threadDatas[MAX_CORES];
static Parse1Data data1[MAX_CORES];
static Parse2Data data2[MAX_CORES];

static void usage()
{
   cerr << "Usage: tuner <options> <training file>" << endl;
   cerr << "Options:" << endl;
   cerr << " -c <cores>" << endl;
   cerr << " -d just write out current parameters values to params.cpp" << endl;
   cerr << " -f <ouput .cpp file>" << endl;
   cerr << " -i <input parameter file>" << endl;
   cerr << " -r apply regularization" << endl;
   cerr << " -n <iterations>" << endl;
   cerr << " -o adagrad|adam|adaptive select optimization method" << endl;
   cerr << " -O ordinal|msq select objective type" << endl;
   cerr << " -R periodically recalulate PVs" << endl;
   cerr << " -V validate gradient" << endl;
   cerr << " -x <ouput parameter file>" << endl;
}

static double texelSigmoid(double val) {
   // predicted score in range [0,1] based on eval.
   return 1.0/(1.0+exp(-PARAM1*val));
}

double result_val(const string &res)
{
   double result;
   if (res == "0-1")
      result = 0.0;
   else if (res == "1-0")
      result = 1.0;
   else if (res == "1/2-1/2")
      result = 0.5;
   else {
      cerr << "Missing or unrecognized result: " << res << endl;
      return 0.5;
   }
   return result;
}

// value is eval in pawn units; res is result string for game
static double computeErrorTexel(double value,double result,const ColorType side)
{
   value /= PAWN_VALUE;

   if (side == Black) value = -value;

   const double predict = texelSigmoid(value);

   double err = 0.0;

   switch(obj) {
   case Objective::Msq:
      err = (predict-result)*(predict-result);
      break;
   case Objective::Ordinal:
      if (result == 0) {
         err = texelSigmoid(value-THETA1);
      }
      else if (result == 0.5) {
         err = texelSigmoid(THETA1-value) + texelSigmoid(value-THETA2);
      }
      else {
         err = texelSigmoid(THETA2-value);
      }
      break;
   }
   return err;
}

static double computeTexelDeriv(double value, double result, const ColorType side)
{
   value /= PAWN_VALUE;

   if (side == Black) value = -value;

   double deriv = 0.0;

   switch(obj) {
   case Objective::Msq: {
      double p = exp(PARAM1*value);
      deriv = -2*PARAM1*p*((result-1)*p + result)/(PAWN_VALUE*pow(p+1,3.0));
      break;
   }
   case Objective::Ordinal: {
      auto g = [] (double x) { return PARAM1/(PAWN_VALUE*(1.0+exp(-PARAM1*x))); };
      if (result == 0) {
         deriv = g(value-THETA1);
      }
      else if (result == 0.5) {
         deriv = g(value-THETA2) - g(THETA1-value);
      }
      else {
         deriv = -g(THETA2-value);
      }
      break;
   }
   }
   return deriv;
}

static double norm_val(const Tune::TuneParam &p)
{
   double mid = (p.range())/2.0;
   return (double(p.current)-mid)/p.range();
}

static double calc_penalty()
{
   // apply L2-regularization to the tuning parameters
   double l2 = 0.0;
   if (regularize) {
      for (int i = 0; i < tune_params.numTuningParams(); i++) {
         Tune::TuneParam p;
         tune_params.getParam(i,p);
         // apply penalty only for parameters being tuned
         if (p.tunable) {
            if (p.range()==0) {
               cerr << "warning: param " << p.name << " has zero range" << endl;
               continue;
            }
            // normalize the values since their ranges differ
            l2 += REGULARIZATION*norm_val(p)*norm_val(p);
         }
      }
   }
   return l2;
}

// search a position and return the terminal position at the end of
// the PV. Return 1 if position is valid for tuning.
static int make_pv(ThreadData &td,const Board &board, Board &pvBoard,int &score)
{
   Statistics stats;
   (void) td.searcher->findBestMove(board,
                                    FixedDepth,
                                    999999,
                                    0,
                                    LEARNING_SEARCH_DEPTH,
                                    false,
                                    false,
                                    stats,
                                    Silent);
   int len = 0;
   score = stats.value;
   // skip positions with very large scores (including mate scores)
   if (Util::Abs(score/PAWN_VALUE)<30) {
      pvBoard = board;
      for (; !IsNull(stats.best_line[len]) && len < MAX_PV_LENGTH; len++) {
         pvBoard.doMove(stats.best_line[len]);
      }
      // skip KPK positions (evaluated by bitbases)
      if ((pvBoard.getMaterial(White).kingOnly() &&
           pvBoard.getMaterial(Black).infobits() == Material::KP) ||
          (pvBoard.getMaterial(Black).kingOnly() &&
           pvBoard.getMaterial(White).infobits() == Material::KP)) {
         return 0;
      }
      return 1;
   }
   else
      return 0;
}


static void parse1(ThreadData &td, Parse1Data &pdata, int id)
{
   pdata.clear();
   // iterate for each position in file
   while (!pos_file.eof() && pos_file.good()) {
      try {
         Board board, pvBoard;
         double result = 0.0;
         EPDRecord rec;
         Lock(file_lock);
         if (!ChessIO::readEPDRecord(pos_file,board,rec)) {
            // EOF
            Unlock(file_lock);
            break;
         }
         if (rec.hasError()) {
            cerr << "error in EPD record: " << rec.getError() << endl;
         }
         Unlock(file_lock);
         string val;
         if (rec.getVal(CASTLE_STATUS_KEY,val)) {
            // trim quotes
            val.erase(std::remove( val.begin(), val.end(), '\"' ),val.end());
            stringstream vstream(val);
            int wstatus = 5, bstatus = 5;
            vstream >> wstatus;
            vstream >> bstatus;
            if (!vstream.bad() && !vstream.fail()) {
               board.setCastleStatus((CastleType)wstatus,White);
               board.setCastleStatus((CastleType)bstatus,Black);
            } else {
               cerr << "error parsing castling status";
               continue;
            }
         }
         if (rec.getVal(RESULT_KEY,val)) {
            // trim quotes
            val.erase(std::remove( val.begin(), val.end(), '\"' ),val.end());
            stringstream rstream(val);
            rstream >> result;
            if (rstream.bad() || rstream.fail()) {
               cerr << "error parsing result";
               continue;
            }
         }
         // check for illegal positions
         Bitboard atcks = board.calcAttacks(board.kingSquare(board.oppositeSide()),board.sideToMove());
         // If king can be captured, position is illegal
         if (!atcks.isClear()) continue;
         int score;
         if (make_pv(td,board,pvBoard,score)) {
            double func_value = computeErrorTexel(score, result, board.sideToMove());
            pdata.target += func_value;
            stringstream fen;
            fen << pvBoard;
            Lock(data_lock);
            positions.push_back(new PosInfo(fen.str(),pvBoard.castleStatus(White),pvBoard.castleStatus(Black),result));
            Unlock(data_lock);
         }
      } catch(std::bad_alloc) {
         cerr << "out of memory" << endl;
         exit(-1);
      }
   }
}

// returns index into PST corresponding to square "i"
static int map_to_pst(int i, ColorType side)
{
   int r = Rank(i,side);
   int f = File(i);
   if (f > 4) f = 9-f;
   return 4*(r-1) + f - 1;
}

// map square to outpost index, returns -1 if no mapping
static int map_to_outpost(int i, ColorType side)
{
   int r = Rank(i,side);
   int f = File(i);
   if (r < 5) return -1;
   r -= 5;
   if (f > 4) f = 9-f;
   return r*4 + f - 1;
}

static inline int FileOpen(const Board &board, int file) {
   return !TEST_MASK((board.pawn_bits[White] | board.pawn_bits[Black]), Attacks::file_mask[file - 1]);
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

static void adjustMaterialScore(const Board &board, ColorType side,
                                vector<double> &grads, double inc) {
    const Material &ourmat = board.getMaterial(side);
    const Material &oppmat = board.getMaterial(OppositeColor(side));
    const int pieceDiff = ourmat.pieceValue() - oppmat.pieceValue();
    const int mdiff = ourmat.value() - oppmat.value();
    const int pawnDiff = ourmat.pawnCount() - oppmat.pawnCount();
    if (ourmat.pieceBits() == Material::KB && oppmat.pieceBits() == Material::KB) {
       return;
    }

    if (ourmat.materialLevel() <= 9 && pieceDiff > 0) {
       const uint32_t pieces = ourmat.pieceBits();
       if (pieces == Material::KN || pieces == Material::KB) {
          // Knight or Bishop vs pawns
          if (ourmat.pawnCount() == 0) {
             // no mating material. We can't be better but if
             // opponent has 1-2 pawns we are not so bad
             int index = Util::Min(2,oppmat.pawnCount());
             grads[Tune::KN_VS_PAWN_ADJUST0+index] += inc;
          }
       }
       return;
    }
    int majorDiff = ourmat.majorCount() - oppmat.majorCount();
    switch(majorDiff) {
    case 0: {
       if (ourmat.minorCount() == oppmat.minorCount()+1) {
          // we have extra minor (but not only a minor)
          if (oppmat.pieceBits() == Material::KR) {
             // KR + minor vs KR - draw w. no pawns so lower score
             if (ourmat.hasPawns()) {
                grads[Tune::KRMINOR_VS_R] += inc;
             } else {
                grads[Tune::KRMINOR_VS_R_NO_PAWNS] += inc;
             }
             // do not apply trade down or pawn bonus
             return;
          }
          else if ((ourmat.pieceBits() == Material::KQN ||
                   ourmat.pieceBits() == Material::KQB) &&
                   oppmat.pieceBits() == Material::KQ) {
              // Q + minor vs Q is a draw, generally
             if (ourmat.hasPawns()) {
                grads[Tune::KQMINOR_VS_Q] += inc;
             } else {
                grads[Tune::KQMINOR_VS_Q_NO_PAWNS] += inc;
             }
             // do not apply trade down or pawn bonus
             return;
          } else if (oppmat.pieceValue() > ROOK_VALUE) {
             // Knight or Bishop traded for pawns. Bonus for piece
             grads[Tune::MINOR_FOR_PAWNS] += inc;
          }
        }
        else if  (ourmat.queenCount() == oppmat.queenCount()+1 &&
             ourmat.rookCount() == oppmat.rookCount() - 2) {
            // Queen vs. Rooks
            // Queen is better with minors on board (per Kaufman)
           grads[Tune::QR_ADJUST0+Util::Min(3,ourmat.minorCount())] += inc;
        }
        break;
    }
    case 1: {
        if (ourmat.rookCount() == oppmat.rookCount()+1) {
            if (ourmat.minorCount() == oppmat.minorCount()) {
                // Rook vs. pawns. Usually the Rook is better.
               grads[Tune::ROOK_VS_PAWNS] += inc;
            }
            else if (ourmat.minorCount() == oppmat.minorCount() - 1) {
                // Rook vs. minor
                // not as bad w. fewer pieces
               ASSERT(ourmat.majorCount()>0);
               grads[Tune::RB_ADJUST1+Util::Min(3,ourmat.majorCount()-1)] += inc;
            }
            else if (ourmat.minorCount() == oppmat.minorCount() - 2) {
                // bad trade - Rook for two minors, but not as bad w. fewer pieces
               ASSERT(ourmat.majorCount()>0);
               grads[Tune::RBN_ADJUST1+Util::Min(3,ourmat.majorCount()-1)] += inc;
            }
        }
        // Q vs RB or RN is already dealt with by piece values
        break;
    }
    default:
        break;
    }
    int index = Scoring::tradeDownIndex(ourmat,oppmat);
    if (index != -1) {
       grads[Tune::TRADE_DOWN+index] += inc*mdiff/4096;
    }
    if (ourmat.materialLevel() < 16) {
       if (pawnDiff > 0 && pieceDiff >= 0) {
          // better to have more pawns in endgame (if we have not
          // traded pieces for pawns).
          grads[Tune::ENDGAME_PAWN_ADVANTAGE] +=
             inc*(4-ourmat.materialLevel()/4)*Util::Min(2,pawnDiff)/4;
       }
       if (pieceDiff > 0) {
          // bonus for last few pawns - to discourage trade
          const int ourp = ourmat.pawnCount();
          int factor1 = (ourp >= 3) + (ourp >= 2);
          int factor2 = (ourp >=1);
          grads[Tune::PAWN_ENDGAME1] += inc*factor1*(4-ourmat.materialLevel()/4)/4;
          grads[Tune::PAWN_ENDGAME2] += inc*factor2*(4-ourmat.materialLevel()/4)/4;
       }
    }

}

template<ColorType bishopColor>
static void bishopAndPawns(const Board &board,ColorType side,
                           const Scoring::PawnHashEntry::PawnData &ourPawnData,
                           const Scoring::PawnHashEntry::PawnData &oppPawnData,
                    vector<double> &grads, double inc)
{
   int mLevel = board.getMaterial(OppositeColor(side)).materialLevel();
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
      grads[Tune::BAD_BISHOP_MID] += tune_params.scale(inc*(ourPawns - oppPawns),Tune::BAD_BISHOP_MID,mLevel);
      grads[Tune::BAD_BISHOP_END] += tune_params.scale(inc*(ourPawns - oppPawns),Tune::BAD_BISHOP_END,mLevel);
   }
   const int totalOppPawns = oppPawnData.b_square_pawns + oppPawnData.w_square_pawns;
   if (totalOppPawns) {
      // penalize pawns on same color square as opposing single Bishop
#ifdef EVAL_DEBUG
      int tmp = opp_scores.end;
#endif
      grads[Tune::BISHOP_PAWN_PLACEMENT_END] -=
         tune_params.scale(inc*((bishopColor == White ? oppPawnData.w_square_pawns : oppPawnData.b_square_pawns))/ totalOppPawns, Tune::BISHOP_PAWN_PLACEMENT_END,board.getMaterial(side).materialLevel());
   }
}


// Updates a vector where each entry corresponds to a tunable
// parameter. The update is based on a particular board position and
// side and consists for each parameter the contribution of
// the parameter to the overall score, i.e. its first partial derivative.
//
// Currently not all parameters can be related to the overall score
// easily. For those parameters the vector entry is always zero.
static void update_deriv_vector(Scoring &s, const Board &board, ColorType side,
                        vector<double> &grads, double inc)
{

   const ColorType oside = OppositeColor(side);
   const Square kp = board.kingSquare(side);
   const Square okp = board.kingSquare(oside);
   int pin_count = 0;
   int attackWeight = 0;
   int attackCount = 0;
   int majorAttackCount = 0;
   const int mLevel = board.getMaterial(oside).materialLevel();
   const int ourMatLevel = board.getMaterial(side).materialLevel();
   const bool deep_endgame = ourMatLevel <= Scoring::Params::MIDGAME_THRESHOLD;
   const bool early_endgame = board.getMaterial(side).materialLevel() <= Scoring::Params::ENDGAME_THRESHOLD;
   const Bitboard opponent_pawn_attacks(board.allPawnAttacks(oside));
   const Scoring::PawnHashEntry &pawn_entr = s.pawnEntry(board,!validate);
   const Scoring::PawnHashEntry::PawnData ourPawnData = pawn_entr.pawnData(side);
   const Scoring::PawnHashEntry::PawnData oppPawnData = pawn_entr.pawnData(oside);

   const Material &ourmat = board.getMaterial(side);
   const Material &oppmat = board.getMaterial(oside);
   const Bitboard &nearKing(Scoring::kingProximity[oside][okp]);
   Scoring::KingPawnHashEntry oppKpe,ourKpe;
   if (side == White) {
      s.calcCover<White>(board,ourKpe);
      s.calcCover<Black>(board,oppKpe);
   }
   else {
      s.calcCover<Black>(board,ourKpe);
      s.calcCover<White>(board,oppKpe);
   }
   const int oppCover = oppKpe.cover;
   Bitboard minorAttacks, rookAttacks;
   vector<int> attackTypes(8,0);
   if (ourmat.infobits() != oppmat.infobits() &&
       (ourmat.hasPawns() || oppmat.hasPawns())) {
      adjustMaterialScore(board,side,grads,inc);
   }
   if (side == White) {
      if (board[chess::D2] == WhitePawn && board[chess::D3] > WhitePawn && board[chess::D3] < BlackPawn) {
         grads[Tune::CENTER_PAWN_BLOCK] += tune_params.scale(inc,Tune::CENTER_PAWN_BLOCK,mLevel);
      }

      if (board[chess::E2] == WhitePawn && board[chess::E3] > WhitePawn && board[chess::E3] < BlackPawn) {
         grads[Tune::CENTER_PAWN_BLOCK] += tune_params.scale(inc,Tune::CENTER_PAWN_BLOCK,mLevel);
      }
   }
   else {
      if (board[chess::D7] == BlackPawn && board[chess::D6] > BlackPawn) {
         grads[Tune::CENTER_PAWN_BLOCK] += tune_params.scale(inc,Tune::CENTER_PAWN_BLOCK,mLevel);
      }

      if (board[chess::E7] == BlackPawn && board[chess::E6] > BlackPawn) {
         grads[Tune::CENTER_PAWN_BLOCK] += tune_params.scale(inc,Tune::CENTER_PAWN_BLOCK,mLevel);
      }
   }

   grads[Tune::CASTLING0+(int)board.castleStatus(side)] +=
      tune_params.scale(inc,Tune::CASTLING0+(int)board.castleStatus(side),mLevel);
   Bitboard knight_bits(board.knight_bits[side]);
   Square sq = InvalidSquare;
   while (knight_bits.iterate(sq)) {
      const Bitboard &knattacks = Attacks::knight_attacks[sq];
      minorAttacks |= knattacks;
      const int mobl = Bitboard(knattacks &~board.allOccupied &~opponent_pawn_attacks).bitCount();
      grads[Tune::KNIGHT_MOBILITY+mobl] += tune_params.scale(inc,Tune::KNIGHT_MOBILITY+mobl,mLevel);
      int i;
      i = map_to_pst(sq,side);
      grads[Tune::KNIGHT_PST_MIDGAME+i] += tune_params.scale(inc,Tune::KNIGHT_PST_MIDGAME+i,mLevel);
      grads[Tune::KNIGHT_PST_ENDGAME+i] += tune_params.scale(inc,Tune::KNIGHT_PST_ENDGAME+i,mLevel);
      i = map_to_outpost(sq,side);
      ASSERT(i<16);
      if (s.outpost(board,sq,side)) {
         int defenders = s.outpost_defenders(board,sq,side);
         ASSERT(defenders <=2);
         grads[Tune::KNIGHT_OUTPOST+defenders*16+i] += inc;
      }
      Bitboard kattacks(knattacks & nearKing);
      if (kattacks) {
         attackCount++;
         attackWeight += tune_params[Tune::MINOR_ATTACK_FACTOR].current;
         attackTypes[0]++;
         if (kattacks & (kattacks-1)) {
            attackWeight += tune_params[Tune::MINOR_ATTACK_BOOST].current;
            attackTypes[1]++;
         }
      }
   }
   Bitboard bishop_bits(board.bishop_bits[side]);
   const int bishopCount = board.getMaterial(side).bishopCount();
   if (bishopCount == 1) {
      if (TEST_MASK(Board::white_squares, bishop_bits)) {
         bishopAndPawns<White>(board,side,ourPawnData,oppPawnData,grads,inc);
      }
      else {
         bishopAndPawns<Black>(board,side,ourPawnData,oppPawnData,grads,inc);
      }
   }
   else if (bishopCount > 1) {
      grads[Tune::BISHOP_PAIR_MID] += tune_params.scale(inc,Tune::BISHOP_PAIR_MID,mLevel);
      grads[Tune::BISHOP_PAIR_END] += tune_params.scale(inc,Tune::BISHOP_PAIR_END,mLevel);
   }

   while (bishop_bits.iterate(sq)) {
      if (board.pinOnDiag(sq, okp, oside)) {
         pin_count++;
      }
      Bitboard battacks(board.bishopAttacks(sq));
      minorAttacks |= battacks;
      int mobl = Bitboard(board.bishopAttacks(sq) &~board.allOccupied &~opponent_pawn_attacks).bitCount();
      grads[Tune::BISHOP_MOBILITY+mobl] += tune_params.scale(inc,Tune::BISHOP_MOBILITY+mobl,mLevel);
      int i = map_to_pst(sq,side);
      grads[Tune::BISHOP_PST_MIDGAME+i] += tune_params.scale(inc,Tune::BISHOP_PST_MIDGAME+i,mLevel);
      grads[Tune::BISHOP_PST_ENDGAME+i] += tune_params.scale(inc,Tune::BISHOP_PST_ENDGAME+i,mLevel);
      i = map_to_outpost(sq,side);
      ASSERT(i<16);
      if (s.outpost(board,sq,side)) {
         int defenders = s.outpost_defenders(board,sq,side);
         ASSERT(defenders <=2);
         grads[Tune::BISHOP_OUTPOST+defenders*16+i] += inc;
      }
      if (!deep_endgame) {
         Bitboard kattacks(battacks & nearKing);
         if (kattacks) {
            attackCount++;
            attackWeight += tune_params[Tune::MINOR_ATTACK_FACTOR].current;
            attackTypes[0]++;
            if (kattacks & (kattacks - 1)) {
               attackWeight += tune_params[Tune::MINOR_ATTACK_BOOST].current;
               attackTypes[1]++;
            }
         }
         else if (battacks & board.queen_bits[side]) {
            // possible stacked attackers
            kattacks = board.bishopAttacks(sq, side) & nearKing;
            if (kattacks) {
               attackCount++;
               attackWeight += tune_params[Tune::MINOR_ATTACK_FACTOR].current;
               attackTypes[0]++;
               if (kattacks & (kattacks - 1)) {
                  attackWeight += tune_params[Tune::MINOR_ATTACK_BOOST].current;
                  attackTypes[1]++;
               }
            }
         }
      }
   }
   Bitboard rook_bits(board.rook_bits[side]);
   while (rook_bits.iterate(sq)) {
      if (board.pinOnRankOrFile(sq, okp, oside)) {
         pin_count++;
      }
      rookAttacks |= board.rookAttacks(sq);
      Bitboard rattacks2(board.rookAttacks(sq, side));
      int mobl = Bitboard(rattacks2 &~board.allOccupied & ~opponent_pawn_attacks).bitCount();
      grads[Tune::ROOK_MOBILITY_MIDGAME+mobl] += tune_params.scale(inc,Tune::ROOK_MOBILITY_MIDGAME+mobl,mLevel);
      grads[Tune::ROOK_MOBILITY_ENDGAME+mobl] += tune_params.scale(inc,Tune::ROOK_MOBILITY_ENDGAME+mobl,mLevel);
      int i = map_to_pst(sq,side);
      grads[Tune::ROOK_PST_MIDGAME+i] += tune_params.scale(inc,Tune::ROOK_PST_MIDGAME+i,mLevel);
      grads[Tune::ROOK_PST_ENDGAME+i] += tune_params.scale(inc,Tune::ROOK_PST_ENDGAME+i,mLevel);
      if (Rank(sq,side) == 7 && (Rank(board.kingSquare(oside),side) == 8 || (board.pawn_bits[oside] & Attacks::rank7mask[side]))) {
         grads[Tune::ROOK_ON_7TH_MID] += tune_params.scale(inc,Tune::ROOK_ON_7TH_MID,mLevel);
         grads[Tune::ROOK_ON_7TH_END] += tune_params.scale(inc,Tune::ROOK_ON_7TH_END,mLevel);
         const Bitboard rattacks(board.rookAttacks(sq));
         if (Attacks::rank_mask_right[sq] & rattacks & board.rook_bits[side]) {
            // 2 connected rooks on 7th
            grads[Tune::TWO_ROOKS_ON_7TH_MID] += tune_params.scale(inc,Tune::TWO_ROOKS_ON_7TH_MID,mLevel);
            grads[Tune::TWO_ROOKS_ON_7TH_END] += tune_params.scale(inc,Tune::TWO_ROOKS_ON_7TH_END,mLevel);
         }
      }
      if (FileOpen(board, File(sq))) {
         grads[Tune::ROOK_ON_OPEN_FILE_MID] += tune_params.scale(inc,Tune::ROOK_ON_OPEN_FILE_MID,mLevel);
         grads[Tune::ROOK_ON_OPEN_FILE_END] += tune_params.scale(inc,Tune::ROOK_ON_OPEN_FILE_END,mLevel);
      }
      if (TEST_MASK(board.pawn_bits[side], Attacks::rank_mask[Rank(sq, White) - 1])) {
         Bitboard atcks(board.rankAttacks(sq) & board.pawn_bits[side]);
         Square pawnsq;
         int i = 0;
         while(atcks.iterate(pawnsq)) {
            if (!TEST_MASK(Attacks::pawn_attacks[pawnsq][side], board.pawn_bits[side])) // not protected by pawn
            ++i;
         }
         grads[Tune::SIDE_PROTECTED_PAWN] += tune_params.scale(inc*i,Tune::SIDE_PROTECTED_PAWN,mLevel);
      }
      if (!deep_endgame) {
         Bitboard rattacks2(board.rookAttacks(sq, side));
         Bitboard attacks(rattacks2 & nearKing);
         if (attacks) {
            attackCount++;
            majorAttackCount++;
            attackWeight += tune_params[Tune::ROOK_ATTACK_FACTOR].current;
            attackTypes[Tune::ROOK_ATTACK_FACTOR-
                        Tune::MINOR_ATTACK_FACTOR]++;
            Bitboard attacks2(attacks & Scoring::kingNearProximity[okp]);
            if (attacks2) {
               attacks2 &= (attacks2 - 1);
               if (attacks2) {
                  // rook attacks at least 2 squares near king
                  attackWeight += tune_params[Tune::ROOK_ATTACK_BOOST].current;
                  attackTypes[Tune::ROOK_ATTACK_BOOST-
                              Tune::MINOR_ATTACK_FACTOR]++;
                  attacks2 &= (attacks2 - 1);
                  if (attacks2) {
                     attackWeight += tune_params[Tune::ROOK_ATTACK_BOOST2].current;
                     attackTypes[Tune::ROOK_ATTACK_BOOST2-
                                 Tune::MINOR_ATTACK_FACTOR]++;
                  }
               }
            }
         }
      }
   }
   Bitboard queen_bits(board.queen_bits[side]);
   while (queen_bits.iterate(sq)) {
      if (board.pinOnDiag(sq, okp, oside)) {
         pin_count++;
      }
      if (board.pinOnRankOrFile(sq, okp, oside)) {
         pin_count++;
      }
      int mobl = Bitboard(board.queenAttacks(sq) &~board.allOccupied &~opponent_pawn_attacks).bitCount();
      grads[Tune::QUEEN_MOBILITY_MIDGAME+mobl] += tune_params.scale(inc,Tune::QUEEN_MOBILITY_MIDGAME+mobl,mLevel);
      grads[Tune::QUEEN_MOBILITY_ENDGAME+mobl] += tune_params.scale(inc,Tune::QUEEN_MOBILITY_ENDGAME+mobl,mLevel);
      int i = map_to_pst(sq,side);
      grads[Tune::QUEEN_PST_MIDGAME+i] += tune_params.scale(inc,Tune::QUEEN_PST_MIDGAME+i,mLevel);
      grads[Tune::QUEEN_PST_ENDGAME+i] += tune_params.scale(inc,Tune::QUEEN_PST_ENDGAME+i,mLevel);
      if (!deep_endgame) {
         Bitboard kattacks;
         Bitboard battacks(board.bishopAttacks(sq));
         kattacks = battacks & nearKing;
         if (!kattacks) {
            kattacks = board.bishopAttacks(sq, side) & nearKing;
         }
         Bitboard rattacks = board.rookAttacks(sq);
         if (rattacks & nearKing) {
            kattacks |= (rattacks & nearKing);
         }
         else {
            kattacks |= (board.rookAttacks(sq, side) & nearKing);
         }

         if (kattacks) {
            attackCount++;
            majorAttackCount++;
            attackWeight += tune_params[Tune::QUEEN_ATTACK_FACTOR].current;
#ifdef EVAL_DEBUG
            int tmp = attackWeight;
#endif
            attackTypes[Tune::QUEEN_ATTACK_FACTOR-
                        Tune::MINOR_ATTACK_FACTOR]++;
            // bonus if Queen attacks multiple squares near King:
            Bitboard nearAttacks(kattacks & Scoring::kingNearProximity[okp]);
            if (nearAttacks) {
               nearAttacks &= (nearAttacks - 1);      // clear 1st bit
               if (nearAttacks) {
                  attackWeight += tune_params[Tune::QUEEN_ATTACK_BOOST].current;
                  attackTypes[Tune::QUEEN_ATTACK_BOOST-
                              Tune::MINOR_ATTACK_FACTOR]++;
                  nearAttacks &= (nearAttacks - 1);   // clear 1st bit
                  if (nearAttacks) {
                     attackWeight += tune_params[Tune::QUEEN_ATTACK_BOOST2].current;
                     attackTypes[Tune::QUEEN_ATTACK_BOOST2-
                                 Tune::MINOR_ATTACK_FACTOR]++;
                  }
               }
            }
#ifdef EVAL_DEBUG
            if (attackWeight - tmp) cout << "queen attack boost= " << attackWeight - tmp << endl;
#endif
         }
      }
   }
   Square ksq = board.kingSquare(side);
   int ksq_map = map_to_pst(ksq,side);
   grads[Tune::KING_PST_MIDGAME+ksq_map] += tune_params.scale(inc,Tune::KING_PST_MIDGAME+ksq_map,mLevel);

   Bitboard all_pawns(board.pawn_bits[White] | board.pawn_bits[Black]);
   if (ourMatLevel <= Scoring::Params::ENDGAME_THRESHOLD &&
       all_pawns &&
       (!board.getMaterial(oside).kingOnly() ||
        (board.getMaterial(side).infobits() != Material::KP &&
         board.getMaterial(side).infobits() != Material::KBN &&
         board.getMaterial(side).infobits() != Material::KR &&
         board.getMaterial(side).infobits() != Material::KQ))) {
      const int pieces = board.getMaterial(side).pieceCount();
      double inc_adjust = inc;
      int k_pos = tune_params[Tune::KING_PST_ENDGAME + ksq_map].current;
      if (pieces < 3) {
         inc_adjust = inc_adjust*tune_params[Tune::KING_POSITION_LOW_MATERIAL0+pieces].current/128.0;
      }
      grads[Tune::KING_PST_ENDGAME+ksq_map] += tune_params.scale(inc_adjust,Tune::KING_PST_ENDGAME+ksq_map,mLevel);
      if (!TEST_MASK(Attacks::abcd_mask, all_pawns)) {
         if (File(kp) > chess::DFILE) {
            grads[Tune::PAWN_SIDE_BONUS] += tune_params.scale(inc_adjust,Tune::PAWN_SIDE_BONUS,mLevel);
            k_pos += tune_params[Tune::PAWN_SIDE_BONUS].current;
         }
         else {
            grads[Tune::PAWN_SIDE_BONUS] -= tune_params.scale(inc_adjust,Tune::PAWN_SIDE_BONUS,mLevel);
            k_pos -= tune_params[Tune::PAWN_SIDE_BONUS].current;
         }
      }
      else if (!TEST_MASK(Attacks::efgh_mask, all_pawns)) {
         if (File(kp) <= chess::DFILE) {
            grads[Tune::PAWN_SIDE_BONUS] += tune_params.scale(inc_adjust,Tune::PAWN_SIDE_BONUS,mLevel);
            k_pos += tune_params[Tune::PAWN_SIDE_BONUS].current;
         }
         else {
            grads[Tune::PAWN_SIDE_BONUS] -= tune_params.scale(inc_adjust,Tune::PAWN_SIDE_BONUS,mLevel);
            k_pos -= tune_params[Tune::PAWN_SIDE_BONUS].current;
         }
      }
      inc_adjust /= 4.0;
      Bitboard it(board.pawn_bits[side]);
      Square sq;
      while(it.iterate(sq)) {
         grads[Tune::KING_OWN_PAWN_DISTANCE] +=
           tune_params.scale(inc_adjust*(4-Scoring::distance(kp,sq)),Tune::KING_OWN_PAWN_DISTANCE,mLevel);
         k_pos += (4-Scoring::distance(kp,sq))*tune_params[Tune::KING_OWN_PAWN_DISTANCE].current/4;
         int file = File(sq);
         int rank = Rank (sq,side);
         if (ourPawnData.passers.isSet(sq) && rank >= 6 &&
             (File(kp) == file - 1 || File(kp) == file + 1) &&
             Rank(kp, side) >= rank) {
            if (rank == 6) {
               k_pos += tune_params[Tune::SUPPORTED_PASSER6].current/4;
               grads[Tune::SUPPORTED_PASSER6] +=
                  tune_params.scale(inc_adjust,Tune::SUPPORTED_PASSER6,mLevel);
            }
            else {
               k_pos += tune_params[Tune::SUPPORTED_PASSER7].current/4;
               grads[Tune::SUPPORTED_PASSER7] +=
                  tune_params.scale(inc_adjust,Tune::SUPPORTED_PASSER7,mLevel);
            }
         }
      }
      it = board.pawn_bits[oside];
      while (it.iterate(sq)) {
         grads[Tune::KING_OPP_PAWN_DISTANCE] +=
             tune_params.scale(inc_adjust*(4-Scoring::distance(kp,sq)),Tune::KING_OPP_PAWN_DISTANCE,mLevel);
         k_pos += (4-Scoring::distance(kp,sq))*tune_params[Tune::KING_OPP_PAWN_DISTANCE].current/4;
         int rank = Rank(sq,oside);
         if (oppPawnData.passers.isSet(sq) && Rank(kp,oside)>=rank) {
            Square queenSq = MakeSquare(File(sq),8,oside);
            grads[Tune::KING_OPP_PASSER_DISTANCE+rank-2] +=
               tune_params.scale(inc_adjust*((4-Scoring::distance(kp,sq))+(4-Scoring::distance(kp,queenSq)))/2,Tune::KING_OPP_PASSER_DISTANCE+rank-2,mLevel);
            k_pos += ((4-Scoring::distance(kp,sq))+(4-Scoring::distance(kp,queenSq)))*tune_params[Tune::KING_OPP_PASSER_DISTANCE+rank-2].current/(4*2);
         }
      }
      if (pieces < 3) {
         // compute partial derivative of KING_POSITION_LOW_MATERIAL
         grads[Tune::KING_POSITION_LOW_MATERIAL0+pieces] +=
            tune_params.scale(k_pos*inc/128.0,Tune::KING_POSITION_LOW_MATERIAL0+pieces,mLevel);
      }
   }

   if (pin_count && !deep_endgame) {
      grads[Tune::PIN_MULTIPLIER_MID] += tune_params.scale(inc,Tune::PIN_MULTIPLIER_MID,board.getMaterial(side).materialLevel())*pin_count;
   }
   if (pin_count) {
      grads[Tune::PIN_MULTIPLIER_END] += tune_params.scale(inc,Tune::PIN_MULTIPLIER_END,mLevel)*pin_count;
   }

   int mobl = Bitboard(Attacks::king_attacks[board.kingSquare(side)] & ~board.allOccupied & ~board.allAttacks(oside)).bitCount();
   grads[Tune::KING_MOBILITY_ENDGAME+Util::Min(4,mobl)] += tune_params.scale(inc,Tune::KING_MOBILITY_ENDGAME+Util::Min(4,mobl),mLevel);
   const Scoring::PawnDetail *pds = pawn_entr.pawnData(side).details;
   int pawns = board.pawn_bits[side].bitCount();
   for (int i = 0; i < pawns; i++) {
      ASSERT(OnBoard(pds[i].sq));
      grads[Tune::SPACE] += inc*pds[i].space_weight;
      if (pds[i].flags & Scoring::PawnDetail::PASSED) {
         grads[Tune::PASSED_PAWN_MID2+Rank(pds[i].sq,side)-2] +=
            tune_params.scale(inc,Tune::PASSED_PAWN_MID2+Rank(pds[i].sq,side)-2,mLevel);
         grads[Tune::PASSED_PAWN_END2+Rank(pds[i].sq,side)-2] +=
            tune_params.scale(inc,Tune::PASSED_PAWN_END2+Rank(pds[i].sq,side)-2,mLevel);
         sq = pds[i].sq;
         const Bitboard &mask = (side == White) ? Attacks::file_mask_up[sq] :
            Attacks::file_mask_down[sq];

         Square blocker;
         if (side == White)
            blocker = (mask & board.allOccupied).firstOne();
         else
            blocker = (mask & board.allOccupied).lastOne();
         if (blocker != InvalidSquare) {
            const int index = pp_block_index(sq,blocker,side);
            if (board.occupied[side].isSet(blocker)) {
               grads[Tune::PP_OWN_PIECE_BLOCK_MID+index] +=
                  tune_params.scale(inc,Tune::PP_OWN_PIECE_BLOCK_MID+index,mLevel);
               grads[Tune::PP_OWN_PIECE_BLOCK_END+index] +=
                  tune_params.scale(inc,Tune::PP_OWN_PIECE_BLOCK_END+index,mLevel);
            } else {
               grads[Tune::PP_OPP_PIECE_BLOCK_MID+index] +=
                  tune_params.scale(inc,Tune::PP_OPP_PIECE_BLOCK_MID+index,mLevel);
               grads[Tune::PP_OPP_PIECE_BLOCK_END+index] +=
                  tune_params.scale(inc,Tune::PP_OPP_PIECE_BLOCK_END+index,mLevel);
            }
         }
         Bitboard atcks(board.fileAttacks(sq));
#ifdef PAWN_DEBUG
         int mid_tmp = scores.mid;
         int end_tmp = scores.end;
#endif
         const int file = File(sq);
         const int rank = Rank(sq,side);
         if (TEST_MASK(board.rook_bits[side], Attacks::file_mask[file - 1])) {
            atcks &= board.rook_bits[side];
            if (atcks & mask) {
               grads[Tune::ROOK_BEHIND_PP_MID] +=
                  tune_params.scale(inc,Tune::ROOK_BEHIND_PP_MID,mLevel);
               grads[Tune::ROOK_BEHIND_PP_END] +=
                  tune_params.scale(inc,Tune::ROOK_BEHIND_PP_END,mLevel);
            }

            // Rook adjacent to pawn on 7th is good too
            if (rank == 7 && file < 8 && TEST_MASK(board.rook_bits[side], Attacks::file_mask[file])) {
               Bitboard atcks(board.fileAttacks(sq + 1) & board.rook_bits[side]);
               if (!atcks.isClear() || board.rook_bits[side].isSet(sq + 1)) {
                  grads[Tune::ROOK_BEHIND_PP_MID] +=
                     tune_params.scale(inc,Tune::ROOK_BEHIND_PP_MID,mLevel);
                  grads[Tune::ROOK_BEHIND_PP_END] +=
                     tune_params.scale(inc,Tune::ROOK_BEHIND_PP_END,mLevel);
               }
            }

            if (rank == 7 && file > 1 && TEST_MASK(board.rook_bits[side], Attacks::file_mask[file - 2])) {
               Bitboard atcks(board.fileAttacks(sq - 1) & board.rook_bits[side]);
               if (!atcks.isClear() || board.rook_bits[side].isSet(sq - 1)) {
                  grads[Tune::ROOK_BEHIND_PP_MID] +=
                     tune_params.scale(inc,Tune::ROOK_BEHIND_PP_MID,mLevel);
                  grads[Tune::ROOK_BEHIND_PP_END] +=
                     tune_params.scale(inc,Tune::ROOK_BEHIND_PP_END,mLevel);
               }
            }
         }
      }
      if (pds[i].flags & Scoring::PawnDetail::POTENTIAL_PASSER) {
         ASSERT(Rank(pds[i].sq,side)<7);
         grads[Tune::POTENTIAL_PASSER_MID2+Rank(pds[i].sq,side)-2] +=
            tune_params.scale(inc,Tune::POTENTIAL_PASSER_MID2+Rank(pds[i].sq,side)-2,mLevel);
         grads[Tune::POTENTIAL_PASSER_END2+Rank(pds[i].sq,side)-2] +=
            tune_params.scale(inc,Tune::POTENTIAL_PASSER_END2+Rank(pds[i].sq,side)-2,mLevel);
      }
      if (pds[i].flags & Scoring::PawnDetail::DOUBLED) {
         int f = File(pds[i].sq);
         if (f > 4) f = 9-f;
         grads[Tune::DOUBLED_PAWNS_MID1+f-1] +=
            tune_params.scale(inc,Tune::DOUBLED_PAWNS_MID1+f-1,mLevel);
         grads[Tune::DOUBLED_PAWNS_END1+f-1] +=
            tune_params.scale(inc,Tune::DOUBLED_PAWNS_END1+f-1,mLevel);
      }
      if (pds[i].flags & Scoring::PawnDetail::TRIPLED) {
         int f = File(pds[i].sq);
         if (f > 4) f = 9-f;
         grads[Tune::TRIPLED_PAWNS_MID1+f-1] +=
            tune_params.scale(inc,Tune::TRIPLED_PAWNS_MID1+f-1,mLevel);
         grads[Tune::TRIPLED_PAWNS_END1+f-1] +=
            tune_params.scale(inc,Tune::TRIPLED_PAWNS_END1+f-1,mLevel);
      }
      if (pds[i].flags & Scoring::PawnDetail::ISOLATED) {
         int f = File(pds[i].sq);
         if (f > 4) f = 9-f;
         grads[Tune::ISOLATED_PAWN_MID1+f-1] +=
            tune_params.scale(inc,Tune::ISOLATED_PAWN_MID1+f-1,mLevel);
         grads[Tune::ISOLATED_PAWN_END1+f-1] +=
            tune_params.scale(inc,Tune::ISOLATED_PAWN_END1+f-1,mLevel);
      }
      if (pds[i].flags & Scoring::PawnDetail::WEAK) {
         grads[Tune::WEAK_PAWN_MID] +=
            tune_params.scale(inc,Tune::WEAK_PAWN_MID,mLevel);
         grads[Tune::WEAK_PAWN_END] +=
            tune_params.scale(inc,Tune::WEAK_PAWN_END,mLevel);
      }
      if (pds[i].flags & Scoring::PawnDetail::CONNECTED_PASSER) {
         grads[Tune::CONNECTED_PASSER_MID2+Rank(pds[i].sq,side)-2] +=
            tune_params.scale(inc,Tune::CONNECTED_PASSER_MID2+Rank(pds[i].sq,side)-2,mLevel);
         grads[Tune::CONNECTED_PASSER_END2+Rank(pds[i].sq,side)-2] +=
            tune_params.scale(inc,Tune::CONNECTED_PASSER_END2+Rank(pds[i].sq,side)-2,mLevel);
      }
      if (pds[i].flags & Scoring::PawnDetail::ADJACENT_PASSER) {
         grads[Tune::ADJACENT_PASSER_MID2+Rank(pds[i].sq,side)-2] +=
            tune_params.scale(inc,Tune::ADJACENT_PASSER_MID2+Rank(pds[i].sq,side)-2,mLevel);
         grads[Tune::ADJACENT_PASSER_END2+Rank(pds[i].sq,side)-2] +=
            tune_params.scale(inc,Tune::ADJACENT_PASSER_END2+Rank(pds[i].sq,side)-2,mLevel);
      }
   }
   if (pawn_entr.pawnData(side).outside && !pawn_entr.pawnData(oside).outside) {
      grads[Tune::OUTSIDE_PASSER_MID] +=
         tune_params.scale(inc,Tune::OUTSIDE_PASSER_MID,mLevel);
      grads[Tune::OUTSIDE_PASSER_END] +=
         tune_params.scale(inc,Tune::OUTSIDE_PASSER_END,mLevel);
   }
   Bitboard centerCalc(Attacks::center & (board.pawn_bits[side] | board.allPawnAttacks(side)));
   grads[Tune::PAWN_CENTER_SCORE_MID] +=
      tune_params.scale(inc*centerCalc.bitCount(),Tune::PAWN_CENTER_SCORE_MID,mLevel);

   if (board.rook_bits[oside] | board.queen_bits[oside]) {
      grads[Tune::WEAK_ON_OPEN_FILE_MID] +=
         tune_params.scale(inc*pawn_entr.pawnData(side).weakopen,Tune::WEAK_ON_OPEN_FILE_MID,mLevel);
      grads[Tune::WEAK_ON_OPEN_FILE_END] +=
            tune_params.scale(inc*pawn_entr.pawnData(side).weakopen,Tune::WEAK_ON_OPEN_FILE_END,mLevel);
   }
   const Bitboard & opa = opponent_pawn_attacks;
   // bonus for pawn threats against pieces
   int pawnThreats = Bitboard(oppPawnData.opponent_pawn_attacks & board.occupied[side] & ~board.pawn_bits[side]).bitCountOpt();
   if (pawnThreats) {
      grads[Tune::PAWN_THREAT_ON_PIECE_MID] +=
         tune_params.scale(inc*pawnThreats,Tune::PAWN_THREAT_ON_PIECE_MID,mLevel);
      grads[Tune::PAWN_THREAT_ON_PIECE_END] +=
         tune_params.scale(inc*pawnThreats,Tune::PAWN_THREAT_ON_PIECE_END,mLevel);
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
         grads[Tune::PIECE_THREAT_MM_MID] +=
            tune_params.scale(inc*mattacks,Tune::PIECE_THREAT_MM_MID,mLevel);
         grads[Tune::PIECE_THREAT_MR_MID] +=
            tune_params.scale(inc*rattacks,Tune::PIECE_THREAT_MR_MID,mLevel);
         grads[Tune::PIECE_THREAT_MQ_MID] +=
            tune_params.scale(inc*qattacks,Tune::PIECE_THREAT_MQ_MID,mLevel);
         grads[Tune::MINOR_PAWN_THREAT_MID] +=
            tune_params.scale(inc*Bitboard(unsafePawns & minorAttacks).bitCountOpt(),Tune::MINOR_PAWN_THREAT_MID,mLevel);
      }
      if (early_endgame) {
         grads[Tune::PIECE_THREAT_MM_END] +=
            tune_params.scale(inc*mattacks,Tune::PIECE_THREAT_MM_END,mLevel);
         grads[Tune::PIECE_THREAT_MR_END] +=
            tune_params.scale(inc*rattacks,Tune::PIECE_THREAT_MR_END,mLevel);
         grads[Tune::PIECE_THREAT_MQ_END] +=
            tune_params.scale(inc*qattacks,Tune::PIECE_THREAT_MQ_END,mLevel);
         grads[Tune::MINOR_PAWN_THREAT_END] +=
            tune_params.scale(inc*Bitboard(unsafePawns & minorAttacks).bitCountOpt(),Tune::MINOR_PAWN_THREAT_END,mLevel);
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
         grads[Tune::PIECE_THREAT_RM_MID] +=
            tune_params.scale(inc*mattacks,Tune::PIECE_THREAT_RM_MID,mLevel);
         grads[Tune::PIECE_THREAT_RR_MID] +=
            tune_params.scale(inc*rattacks,Tune::PIECE_THREAT_RR_MID,mLevel);
         grads[Tune::PIECE_THREAT_RQ_MID] +=
            tune_params.scale(inc*qattacks,Tune::PIECE_THREAT_RQ_MID,mLevel);
         grads[Tune::ROOK_PAWN_THREAT_MID] +=
            tune_params.scale(inc*Bitboard(unsafePawns & rookAttacks).bitCountOpt(),Tune::ROOK_PAWN_THREAT_MID,mLevel);
      }
      if (early_endgame) {
         grads[Tune::PIECE_THREAT_RM_END] +=
            tune_params.scale(inc*mattacks,Tune::PIECE_THREAT_RM_END,mLevel);
         grads[Tune::PIECE_THREAT_RR_END] +=
            tune_params.scale(inc*rattacks,Tune::PIECE_THREAT_RR_END,mLevel);
         grads[Tune::PIECE_THREAT_RQ_END] +=
            tune_params.scale(inc*qattacks,Tune::PIECE_THREAT_RQ_END,mLevel);
         grads[Tune::ROOK_PAWN_THREAT_END] +=
            tune_params.scale(inc*Bitboard(unsafePawns & rookAttacks).bitCountOpt(),Tune::ROOK_PAWN_THREAT_END,mLevel);
      }
   }
   if (early_endgame) {
      // attacks on undefended pawns or pieces
      Bitboard kattacks(Attacks::king_attacks[kp] & board.occupied[oside] & ~opa);
      if (kattacks) {
         grads[Tune::ENDGAME_KING_THREAT] +=
            tune_params.scale(inc*kattacks.bitCountOpt(),
                              Tune::ENDGAME_KING_THREAT,mLevel);
      }
   }
   if (!deep_endgame) {
      int proximity = Bitboard(Scoring::kingPawnProximity[oside][okp] & board.pawn_bits[side]).bitCount();
      int pawnAttacks = Bitboard(oppPawnData.opponent_pawn_attacks & nearKing).bitCount();
      attackWeight += tune_params[Tune::PAWN_ATTACK_FACTOR1].current*proximity + tune_params[Tune::PAWN_ATTACK_FACTOR2].current*pawnAttacks;
      if (attackCount >= 2 && majorAttackCount) {
         attackWeight += Scoring::Params::KING_ATTACK_COUNT_BOOST[Util::Min(2,attackCount-2)];
      }
      int cover_boost_index = -1;
      if (oppCover <= -PAWN_VALUE/5) {
         cover_boost_index = Util::Min(4,int(-oppCover/(PAWN_VALUE/5))-1);
         attackWeight += tune_params[Tune::KING_ATTACK_COVER_BOOST+cover_boost_index].current;
      }
      // king safety tuning
      const int scale_index =
         Util::Min(Scoring::Params::KING_ATTACK_SCALE_SIZE-1,attackWeight/Scoring::Params::KING_ATTACK_FACTOR_RESOLUTION);
      // approximate the gradient of the scale
      double scale_grad = 0.0;
      if (scale_index > 8) {
         int low = Util::Max(0,scale_index);
         int high = Util::Min(scale_index + 1,Scoring::Params::KING_ATTACK_SCALE_SIZE-1);
         if (low == high) --low;
         scale_grad = double(tune_params[Tune::KING_ATTACK_SCALE+high].current-tune_params[Tune::KING_ATTACK_SCALE+low].current)/double(high-low);
      }
      grads[Tune::PAWN_ATTACK_FACTOR1] +=
         tune_params.scale(inc*scale_grad*proximity/Scoring::Params::KING_ATTACK_FACTOR_RESOLUTION,Tune::PAWN_ATTACK_FACTOR1,ourMatLevel);
      grads[Tune::PAWN_ATTACK_FACTOR2] +=
         tune_params.scale(inc*scale_grad*pawnAttacks/Scoring::Params::KING_ATTACK_FACTOR_RESOLUTION,Tune::PAWN_ATTACK_FACTOR2,ourMatLevel);
      // compute partial derivatives for attack factors
      for (int i = Tune::MINOR_ATTACK_FACTOR;
           i <= Tune::QUEEN_ATTACK_BOOST2;
           i++) {
         if (tune_params[i].tunable) {
            grads[i] +=
               tune_params.scale((inc*scale_grad*attackTypes[i-Tune::MINOR_ATTACK_FACTOR])/Scoring::Params::KING_ATTACK_FACTOR_RESOLUTION,i,ourMatLevel);
         }
      }
      if (attackCount >= 2 && majorAttackCount) {
         int index = Tune::KING_ATTACK_COUNT_BOOST+Util::Min(2,attackCount-2);
         grads[index] +=
            tune_params.scale(inc*scale_grad/Scoring::Params::KING_ATTACK_FACTOR_RESOLUTION,index,ourMatLevel);
      }
      if (oppCover < 0) {
         grads[Tune::KING_ATTACK_COVER_BOOST+cover_boost_index] +=
            tune_params.scale(inc*scale_grad/Scoring::Params::KING_ATTACK_FACTOR_RESOLUTION,Tune::KING_ATTACK_COVER_BOOST+cover_boost_index,ourMatLevel);
      }
      if (tune_params[Tune::KING_ATTACK_SCALE+scale_index].tunable) {
         grads[Tune::KING_ATTACK_SCALE+scale_index] +=
            tune_params.scale(inc,Tune::KING_ATTACK_SCALE+scale_index,ourMatLevel);
      }
   }
   if (mLevel >= Scoring::Params::MIDGAME_THRESHOLD) {
      for (int i=0; i<5; i++) {
         for (int j = 0; j < 4; j++) {
            // Note: we do not adjust the gradient to account for any
            // increase to the attackWeight (from the KING_COVER_BOOST
            // params) from changes in the king cover score. In most
            // cases small changes in the cover do not change the boost.
            if (ourKpe.counts[i][j]) {
               grads[Tune::KING_COVER1+i] +=
                  tune_params.scale(inc*ourKpe.counts[i][j]*tune_params[Tune::KING_COVER_FILE_FACTOR0+j].current/64.0,Tune::KING_COVER1+i,mLevel);
               grads[Tune::KING_COVER_FILE_FACTOR0+j] +=
                  tune_params.scale(inc*ourKpe.counts[i][j]*tune_params[Tune::KING_COVER1+i].current/64.0,Tune::KING_COVER_FILE_FACTOR0+j,mLevel);
            }
         }
      }
   }
}

void validateGradient(Scoring &s, const Board &board, double eval, double result) {
   vector<double> derivs(tune_params.numTuningParams(),0.0);
   double inc = 1.0;
   if (board.sideToMove() == Black) eval = -eval;
   // compute the partial derivative vector for this position
   update_deriv_vector(s, board, White, derivs, inc);
   update_deriv_vector(s, board, Black, derivs, -inc);
   for (int i = 0; i < tune_params.numTuningParams(); i++) {
      if (derivs[i] != 0.0 && tune_params[i].tunable) {
         Tune::TuneParam p = tune_params[i];
         score_t val = p.current;
         const score_t range = p.range();
         score_t delta = std::max<score_t>(1,range/40);

         // increase by delta
         score_t newval = val + delta;
         tune_params.updateParamValue(i,newval);
         tune_params.applyParams();
         score_t newEval = s.evalu8(board,false);
         if (board.sideToMove() == Black) newEval = -newEval;
         // compare predicted new value from gradient with
         // actual value
         double predictedEval = eval + derivs[i]*delta;
         if (fabs(predictedEval - newEval)>5.0) {
            cerr << board << endl;
            cerr << "name=" << p.name << " mLevels=" << board.getMaterial(White).materialLevel() << " " << board.getMaterial(White).materialLevel() << " delta=" << delta << " val=" << val << " newval=" << newval << " deriv=" << derivs[i] << " old score=" << eval << " predicted score=" << predictedEval << " actual score=" << newEval << endl;
            // The following code is useful when running under
            // gdb - it recomputes the before and after eval.
            tune_params.updateParamValue(i,val);
            tune_params.applyParams();
            s.evalu8(board,false);
            tune_params.updateParamValue(i,newval);
            tune_params.applyParams();
            s.evalu8(board,false);
         }
         // Test derivative of sigmoid computation too
         double dT = computeTexelDeriv(eval,result,White);
         double baseError = computeErrorTexel(eval,result,White);
         double newError = computeErrorTexel(newEval,result,White);
         double predictedError = baseError + dT*(newEval-eval);
         double estDeriv = (newError-baseError)/(newEval-eval);
         if (fabs(predictedError-newError) > 1e-4) {
            cerr << "warning: param " << p.name << " eval=" << eval << " base=" << baseError << " new=" << newError << " result=" << result << " dT=" << dT << " predicted=" << predictedError << " estDriv= " << estDeriv << endl;
         }
         // restore old value
         tune_params.updateParamValue(i,val);
         tune_params.applyParams();
      }
   }
}

// For a single position, compute its objective function and add to
// the sum total objective, and then, for each parameter, compute the
// gradient value and add it to the sum of gradients that we are
// accumulating.
static void calc_derivative(Scoring &s, Parse2Data &data, const Board &board, double result) {

#ifdef _TRACE
   cout << "game position " << board << endl;
#endif
   double record_value = s.evalu8(board,!validate);

   double func_value = computeErrorTexel(record_value,result,board.sideToMove());
   // compute the change in loss function per delta in eval
   double dT = computeTexelDeriv(record_value,result,board.sideToMove());
   // multiply the derivative by the x (feature) value, scaled if necessary
   // by game phase, and add to the gradient sum.
   update_deriv_vector(s, board, White, data.grads, dT);
   update_deriv_vector(s, board, Black, data.grads, -dT);
   data.target += func_value;
   if (validate) {
      validateGradient(s, board, record_value, result);
   }
   return;
}

static void parse2(ThreadData &td, Parse2Data &data)
{
   data.target = 0.0;
   data.grads.clear();
   data.grads.resize(tune_params.numTuningParams(),0.0);

   // This is large so allocate on heap:
   Scoring *s = new Scoring();
   const size_t max = positions.size();
   for (;;) {
      // obtain the next available posiion from the vector
      size_t next = (size_t)phase2_game_index.fetch_add(1);
      if (next >= max) break;
      if (verbose) cout << "game " << next << " thread " << td.index << endl;
      PosInfo *p = positions[next];
      Board board;
      stringstream pos(p->position);
      pos >> board;
      board.setCastleStatus((CastleType)p->wstatus,White);
      board.setCastleStatus((CastleType)p->bstatus,Black);
      calc_derivative(*s, data, board, p->result);
   }
   delete s;
//   if (verbose) cout << "thread " << td.index << " complete.";
}

static void adjust_params(Parse2Data &data0, vector<double> &historical_gradient,
                          vector<double> &m /* for ADAM */,
                          vector<double> &v /* for ADAM */,
                          vector<double> &prev_gradient /* for adaptive */,
                          vector<double> &step_sizes /* for adaptive */,
                          int iterations)
{
   for (int i = 0; i < tune_params.numTuningParams(); i++) {
      double dv = data0.grads[i];
      Tune::TuneParam p;
      tune_params.getParam(i,p);
      int val = p.current;
      if (regularize) {
         // add the derivative of the regularization term. Note:
         // non-tunable parameters will have derivative zero and
         // we don't regularize them.
         if (p.tunable) {
            dv += 2*REGULARIZATION*norm_val(p);
         }
      }
      if (dv != 0.0 && p.tunable) {
         score_t istep = 1;
         if (method == OptimMethod::AdaGrad) {
            historical_gradient[i] += dv*dv;
            double adjusted_grad  = dv/(ADAGRAD_FUDGE_FACTOR+sqrt(historical_gradient[i]));
            double istep = ADAGRAD_STEP_SIZE*p.range()*adjusted_grad;
            val = std::max<score_t>(p.min_value,std::min<score_t>(p.max_value,val - istep));
         } else if (method == OptimMethod::ADAM) {
            m[i] = ADAM_BETA1*m[i] + (1.0-ADAM_BETA1)*dv;
            v[i] = ADAM_BETA2*v[i] + (1.0-ADAM_BETA2)*dv*dv;
            double m_hat = m[i]/(1.0-pow(ADAM_BETA1,iterations));
            double v_hat = v[i]/(1.0-pow(ADAM_BETA2,iterations));
            double step_size = ADAM_ALPHA*p.range();
            istep = step_size*m_hat/(sqrt(v_hat)+ADAM_EPSILON);
//            cout << "ADAM step[" << i << "]" << ADAM_ALPHA*m_hat/(sqrt(v_hat)+ADAM_EPSILON) << " " << istep << endl;
            val = std::max<score_t>(p.min_value,std::min<score_t>(p.max_value,val - istep));
         } else {
            // Simple adaptive rate method with momentum (from Geoffrey
            // Hinton, U. Toronto).
            if (iterations == 1) {
               step_sizes[i] = std::max<double>(1.0,ADAPTIVE_STEP_BASE*p.range());
            }
            else if (std::signbit(prev_gradient[i]*dv)) {
               // gradient changed signs
               step_sizes[i] = step_sizes[i]*ADAPTIVE_STEP_FACTOR2;
            } else {
               step_sizes[i] = step_sizes[i] + ADAPTIVE_STEP_FACTOR1;
            }
            istep = step_sizes[i];
            if ( dv > 0.0) {
               val = std::max<score_t>(p.min_value,val - istep);
            }
            else if (dv < 0.0) {
               val = std::min<score_t>(p.max_value,val + istep);
            }
            prev_gradient[i] = dv;
         }
         tune_params.updateParamValue(i,val);
      }
   }
}

static void threadp(ThreadData *td)
{
   // set stack size for Linux/Mac
#ifdef _POSIX_VERSION
   size_t stackSize;
   if (pthread_attr_getstacksize(&stackSizeAttrib, &stackSize)) {
        perror("pthread_attr_getstacksize");
        return;
   }
   if (stackSize < THREAD_STACK_SIZE) {
      if (pthread_attr_setstacksize (&stackSizeAttrib, THREAD_STACK_SIZE)) {
         perror("error setting thread stack size");
         return;
      }
   }
#endif

   // allocate controller in the thread
   try {
      td->searcher = new SearchController();
   } catch(std::bad_alloc) {
      cerr << "out of memory, thread " << td->index << endl;
      return;
   }

   td->searcher->clearHashTables();
   // perform work based on phase
   if (td->phase == Phase1) {
      if (verbose) cout << "starting phase 1, thread " << td->index << endl;
      parse1(*td,data1[td->index],td->index);
   } else {
      if (verbose) cout << "starting phase 2, thread " << td->index << endl;
      parse2(*td,data2[td->index]);
   }
   delete td->searcher;
}

static void initThreads()
{
   // prepare threads
#ifdef _POSIX_VERSION
   if (pthread_attr_init (&stackSizeAttrib)) {
      perror("pthread_attr_init");
      return;
   }
#endif
   for (int i = 0; i < cores; i++) {
      threadDatas[i].index = i;
      threadDatas[i].searcher = NULL;
      threadDatas[i].phase = Phase1;
   }
}

static void launch_threads()
{
   if (verbose) cout << "launch_threads" << endl;
   for (int i = 0; i < cores; i++) {
      threads[i] = std::thread(threadp,&threadDatas[i]);
      if (verbose) cout << "thread " << i << " created." << endl;
   }
   // wait for all searchers done
   for (int i = 0; i < cores; i++) {
      threads[i].join();
   }
   if (verbose) cout << "all searchers done" << endl;
}

static void learn_parse(Phase p, int cores)
{
   hash_table->clear();
   if (verbose) cout << "hash table cleared" << endl;
   for (int i = 0; i < cores; i++) {
      threadDatas[i].phase = p;
   }
   launch_threads();
   if (p == Phase1) cout << positions.size() << " positions read." << endl;
}

static void output_solution(const string &cmd)
{
   tune_params.applyParams();
   ofstream param_out(out_file_name.c_str(),ios::out | ios::trunc);

   time_t curr_time;
   char buffer[256];
   time(&curr_time);
   struct tm *timeinfo = localtime(&curr_time);
   strftime(buffer,256,"%d-%b-%Y %I:%M:%S",timeinfo);
   string timestr(buffer);
   string comment("Generated " + timestr + " by \"" + cmd + "\"");
   Scoring::Params::write(param_out,comment);
   param_out << endl;
   if (param_out.bad() || param_out.fail()) {
      cerr << "error writing .cpp output file" << endl;
   }
   ofstream x0_out(x0_file_name.c_str(),ios::out | ios::trunc);
   tune_params.writeX0(x0_out);
   if (x0_out.bad() || x0_out.fail()) {
      cerr << "error writing parameters output file" << endl;
   }
}


static void learn()
{
   try {
      hash_table = new unordered_map<hash_t,PositionDupEntry>();
   } catch(std::bad_alloc) {
      cerr << "hash table alloc: out of memory!" << endl;
      exit(-1);
   }

   LockInit(data_lock);
   LockInit(file_lock);
   LockInit(hash_lock);
#ifdef _MSC_VER
   double best = 1.0e10;
#else
   double best = numeric_limits<double>::max();
#endif
   vector<double> historical_grad(tune_params.numTuningParams(),0.0);
   vector<double> m(tune_params.numTuningParams(),0.0);
   vector<double> v(tune_params.numTuningParams(),0.0);
   vector<double> prev_gradient(tune_params.numTuningParams(),0.0);
   vector<double> step_sizes(tune_params.numTuningParams(),0.0);
   for (int iter = 1; iter <= iterations; iter++) {
      cout << "iteration " << iter << endl;
      tune_params.applyParams();
      if (iter == 1 ||
          (recalc && ((iter-1) % PV_RECALC_INTERVAL) == 0)) {
         if (verbose) cout << "(re)calculating PVs" << endl;
         // clean up data from previous pass
         while (!positions.empty()) {
            delete positions.back();
            positions.pop_back();
         }
         learn_parse(Phase1, cores);
         // sum results over workers into 1st data element
         for (int i = 1; i < cores; i++) {
            data1[0].target += data1[i].target;
         }
         data1[0].target /= positions.size();
         if (verbose) {
            cout << "target=" << data1[0].target << " penalty=" << calc_penalty() << endl;
         }
         data1[0].target += calc_penalty();
         if (verbose) {
            cout << "pass 1 objective = " << data1[0].target << endl;
         }
         // rewind position file
         pos_file.clear();
         pos_file.seekg(0,ios::beg);
      }
      phase2_game_index = 0;
      learn_parse(Phase2, cores);
      // sum results over workers into 1st data element
      for (int i = 1; i < cores; i++) {
         data2[0].target += data2[i].target;
         for (int j = 0; j < tune_params.numTuningParams(); j++) {
            data2[0].grads[j] += data2[i].grads[j];
         }
      }
      data2[0].target /= positions.size();
      cout << "pass 2 target=" << data2[0].target << " penalty=" << calc_penalty
() << " objective=" << data2[0].target + calc_penalty() << endl;
      data2[0].target += calc_penalty();
      if (data2[0].target < best) {
         best = data2[0].target;
         cout << "new best objective: " << best << endl;
         output_solution(cmdline);
      }
      adjust_params(data2[0],historical_grad,m,v,prev_gradient,step_sizes,iter);
   }

   delete hash_table;
   LockFree(data_lock);
   LockFree(file_lock);
   LockFree(hash_lock);
}

int CDECL main(int argc, char **argv)
{
    Bitboard::init();
    initOptions(argv[0]);
    Attacks::init();
    Scoring::init();
    if (!initGlobals(argv[0], false)) {
        cleanupGlobals();
        exit(-1);
    }
    atexit(cleanupGlobals);
    delayedInit();

    string input_file;

    int arg = 0;
    string x0_input_file_name;

    int write_sol = 0;

    cmdline = "tuner";

    for (arg = 1; arg < argc; ++arg) {
       cmdline += " " + string(argv[arg]);
    }

    for (arg = 1; arg < argc && argv[arg][0] == '-'; ++arg) {
       if (strcmp(argv[arg],"-d")==0) {
          cerr << "writing initial solution" << endl;
          ++write_sol;
       }
       else if (strcmp(argv[arg],"-f")==0) {
          ++arg;
          out_file_name = argv[arg];
       }
       else if (strcmp(argv[arg],"-i")==0) {
          ++arg;
          x0_input_file_name = argv[arg];
       }
       else if (strcmp(argv[arg],"-r")==0) {
          regularize = true;
       }
       else if (strcmp(argv[arg],"-x")==0) {
          ++arg;
          x0_file_name = argv[arg];
       }
       else if (strcmp(argv[arg],"-n")==0) {
          ++arg;
          iterations = atoi(argv[arg]);
       }
       else if (strcmp(argv[arg],"-c")==0) {
          ++arg;
          cores = atoi(argv[arg]);
       }
       else if (strcmp(argv[arg],"-v")==0) {
          verbose = true;
       }
       else if (strcmp(argv[arg],"-V")==0) {
          validate = true;
       }
       else if (strcmp(argv[arg],"-o")==0) {
          ++arg;
          if (arg >= argc) {
             usage();
             exit(-1);
          }
          if (strcmp(argv[arg],"adagrad") == 0)
             method = OptimMethod::AdaGrad;
          else if (strcmp(argv[arg],"adam") == 0)
             method = OptimMethod::ADAM;
          else if (strcmp(argv[arg],"adaptive") == 0)
             method = OptimMethod::Adaptive;
          else {
             cerr << "invalid optimization type: specify one of: ";
             cerr << "adagrad, adam or adaptive." << endl;
             exit(-1);
          }
       }
       else if (strcmp(argv[arg],"-O")==0) {
          ++arg;
          if (arg >= argc) {
             usage();
             exit(-1);
          }
          if (strcmp(argv[arg],"ordinal") == 0)
             obj = Objective::Ordinal;
          else if (strcmp(argv[arg],"msq") == 0)
             obj = Objective::Msq;
          else {
             cerr << "invalid objective type: specify one of: ";
             cerr << "Ordinal or Msq" << endl;
             exit(-1);
          }
       }
       else if (strcmp(argv[arg],"-R")==0) {
          recalc = true;
       } else {
          cerr << "invalid option: " << argv[arg] << endl;
          usage();
          exit(-1);
       }
    }

    if (write_sol) {
      output_solution(cmdline);
      exit(0);
    }

    if (validate && cores > 1) {
       cerr << "error: validation (-V) does not work with multiple threads" << endl;
       exit(-1);
    }

    if (arg >= argc) {
       cerr << "no file name specified" << endl;
       usage();
       exit(-1);
    }

    if (x0_input_file_name.length()) {
       ifstream x0_input_file(x0_input_file_name.c_str());
       if (x0_input_file.fail()) {
          cerr << "error opening x0 input file: " << x0_input_file_name << endl;
          exit(-1);
       } else {
          tune_params.readX0(x0_input_file);
          tune_params.applyParams();
       }
    }

    LEARNING_SEARCH_WINDOW = 7*PAWN_VALUE;

    options.search.hash_table_size = 0;
    options.search.easy_threshold = LEARNING_SEARCH_WINDOW;
    options.learning.position_learning = 0;
    options.book.book_enabled = options.log_enabled = 0;
    options.learning.position_learning = false;
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS)
    options.search.use_tablebases = false;
#endif
    pos_file_name = argv[arg];

    if (verbose) cout << "game file: " << pos_file_name << endl;

    pos_file.open(pos_file_name.c_str());

    if (pos_file.fail()) {
       cerr << "failed to open file " << pos_file_name << endl;
       exit(-1);
    }

    cout << "parameter count: " << tune_params.numTuningParams() << " (";
    int tunable = 0;
    for (int i = 0; i < tune_params.numTuningParams(); i++) {
       Tune::TuneParam p;
       tune_params.getParam(i,p);
       if (p.tunable) ++tunable;
    }
    cout << tunable << " tunable)" << endl;

    initThreads();

    learn();

    return 0;
}
