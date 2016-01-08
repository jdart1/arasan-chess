// Copyright 2015 by Jon Dart. All Rights Reserved.
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
#include <cstdio>
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

// MMTO tuning code for Arasan, based on:
// Kunihuto Hoki & Tomoyuki Kaneko, "Large-Scale Optimization for Evaluation
// Functions with Minimax Search,"
// Journal of Articial Intelligence Research 49 (2014) 527-568.
// Partly based on Bonanza source code.
//
// Also implements the "Texel" tuning method via the -t switch.
//
// Note: this file requires a C++11 compatible compiler/libraries.

using namespace tuner;

static bool regularize = false;

static bool texel = false;

static bool use_adagrad = false;

static int iterations = 2;

static int cores = 1;

static string out_file_name="params.cpp";

static string x0_file_name="x0";

static string game_file_name = "games.pgn";

static ifstream game_file;

static bool verbose = false;
static bool validate = false;

static const int MAX_PV_LENGTH = 256;
static const int NUM_RESULT = 8;
static const int MAX_CORES = 64;
static const int THREAD_STACK_SIZE = 12*1024*1024;
static const int LEARNING_SEARCH_DEPTH = 1;
static int LEARNING_SEARCH_WINDOW = 3*PAWN_VALUE;
// Max score deviation from optimal move:
static const double MMTO_DELTA = LEARNING_SEARCH_WINDOW/7.0;
// L2-regularization factor
static const double REGULARIZATION = 1.2E-4;
static const int PV_RECALC_INTERVAL = 16; // for MMTO
static const int MIN_PLY = 16;
static const double ADAGRAD_FUDGE_FACTOR = 1.0e-9;
static const double ADAGRAD_STEP_SIZE = 4.0;
static const double PARAM1 = 0.75;

static vector<GameInfo *> tmpdata;

static atomic<int> phase2_game_index;

LockDefine(file_lock);
LockDefine(data_lock);
LockDefine(hash_lock);

#ifdef _POSIX_VERSION
static pthread_attr_t stackSizeAttrib;
#endif

struct PositionDupEntry
{
   PositionDupEntry() : val(0)
      {
      }

   int val;
};

static unordered_map<hash_t,PositionDupEntry> *hash_table;

enum Phase {Phase1, Phase2};

// game count, used in phase 1
unsigned long games = 0;

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
   uint64 result[NUM_RESULT];

   // accumulated stats for this phase:
   double target;
   double target_out_window;
   uint64 num_moves;
   uint64 result_norm;

   unsigned amove_legal[Constants::MaxMoves];

   void clear() {
      for (int i = 0; i < NUM_RESULT; i++) {
        result[i] = (uint64)0;
      }
      result_norm = 0x0ULL;
      target = target_out_window = 0.0;
      num_moves = (uint64)0;
   }

};

struct Parse2Data
{
   unsigned int id;

   // holds accumulated derivatives for the scoring parameters:
   vector <double> grads;
   uint64 num_moves_counted;
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
   cerr << " -a use AdaGrad" << endl;
   cerr << " -c <cores>" << endl;
   cerr << " -d just write out current parameters values to params.cpp" << endl;
   cerr << " -i <input parameter file> -o <output parameter file>" << endl;
   cerr << " -r apply regularization" << endl;
   cerr << " -t use Texel method instead of MMTO" << endl;
   cerr << " -x <output objective file>" << endl;
   cerr << " -n <iterations>" << endl;
   cerr << " -V validate gradient" << endl;
}

static double texelSigmoid(double val) {
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
static double computeErrorTexel(double value,const string &res,const ColorType side)
{
   value /= PAWN_VALUE;

   if (side == Black) value = -value;

   double predict = texelSigmoid(value);

   double result = result_val(res);

   return - result*log(predict) - (1.0-result)*log(1-predict);
}

static double computeTexelDeriv(double value,const string &res,const ColorType side)
{
   value /= PAWN_VALUE;

   if (side == Black) value = -value;

   double result = result_val(res);

   return (result-texelSigmoid(value));
}


static double func( double x ) {
   if ( x < -LEARNING_SEARCH_WINDOW ) {
      x = -LEARNING_SEARCH_WINDOW;
   }
   else if ( x >  LEARNING_SEARCH_WINDOW ) {
      x = LEARNING_SEARCH_WINDOW;
   }
   return 1.0 / ( 1.0 + exp(-x/MMTO_DELTA) );
}

// computes derivative of the sigmoid function "func", above
static double dfunc( double x )
{
   double dd, dn, dtemp, dret;
   if      ( x <= -LEARNING_SEARCH_WINDOW ) {
      dret = 0.0;
   }
   else if ( x >= LEARNING_SEARCH_WINDOW ) {
      dret = 0.0;
   }
   else {
      dn    = exp( - x / MMTO_DELTA );
      dtemp = dn + 1.0;
      dd    = MMTO_DELTA * dtemp * dtemp;
      dret  = dn / dd;
   }
   return dret;
}

// convert a move to an index based on the order the move generator
// provides. Return -1 if move is not in generated vector, hence is illegal.
#ifdef LEGALITY_CHECK
static int get_move_indx(const Board &board, Move move) {
    // check (pseudo) legality:
    MoveGenerator mg(board);
    int move_indx = -1;
    Move moves[Constants::MaxMoves];
    int n = mg.generateAllMoves(moves, 1 /* repeatable */);
    for (int i = 0; i < n; i++) {
      if (MovesEqual(moves[i],move)) {
          move_indx = i;
          break;
       }
    }
    return move_indx;
}
#endif

static double norm_val(const Tune::TuneParam &p)
{
   double mid = (p.max_value-p.min_value)/2.0;
   return (double(p.current)-mid)/double(p.max_value-p.min_value);
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
            // normalize the values since their ranges differ
            l2 += REGULARIZATION*norm_val(p)*norm_val(p);
         }
      }
   }
   return l2;
}

// search all moves from a position, creating a Pv for each legal move
static void make_pv(const Board &board, ThreadData &td,
                    vector<Pv> &pvs,Parse1Data &pdata, Move record_move,
                    const string &result)
{
   if (texel) {
      // We only care about the score for the PV and the PV moves.
      // We do not need scores for all the moves.
      PackedMove pv[MAX_PV_LENGTH];
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
      const int score = stats.value;
      // skip positions with very large scores (including mate scores)
      if (Util::Abs(score/PAWN_VALUE)<30) {
         for (; !IsNull(stats.best_line[len]) && len < MAX_PV_LENGTH; len++) {
#ifdef _TRACE
            MoveImage(stats.best_line[len],cout);
            cout << ' ';
#endif
            pv[len] = pack_move(stats.best_line[len]);
         }
#ifdef _TRACE
         cout << endl;
#endif
         pvs.push_back(Pv(score,pv,len));
         const double x = double(score)/double(PAWN_VALUE);
         double func_value = computeErrorTexel(x, result, board.sideToMove());
         pdata.target += func_value;
#ifdef _TRACE
         cout << "score=" << x << " sigmoid=" << texelSigmoid(x) << " error=" << func_value << " target sum=" << pdata.target << endl;
#endif
         pdata.num_moves++;
      }
      return;
   }


   RootMoveGenerator mg(board,NULL,record_move);
   Move m;
   bool first = true;
   int record_value = Scoring::INVALID_SCORE;
   int nth = 0;
   int nc = 0;

   int alpha = -Constants::MATE;
   int beta = Constants::MATE;
   Board board_copy(board);

   int order = 0;
   while (!IsNull(m = mg.nextMove(order))) {
      BoardState state(board_copy.state);
#ifdef _TRACE
      cout << board_copy << endl;
      cout << " searching ";
      MoveImage(m,cout);
      cout << endl;
#endif
      board_copy.doMove(m);
      if (!first) {
         alpha = Util::Max(-Constants::MATE,record_value - LEARNING_SEARCH_WINDOW/2);
         beta = Util::Min(Constants::MATE,record_value + LEARNING_SEARCH_WINDOW/2);
      }
      PackedMove pv[MAX_PV_LENGTH];
      Statistics stats;
      (void) td.searcher->findBestMove(board_copy,
                                       FixedDepth,
                                       999999,
                                       0,
                                       LEARNING_SEARCH_DEPTH,
                                       false,
                                       false,
                                       stats,
                                       Silent);

      board_copy.undoMove(m,state);
      const int score = -stats.value;
      pv[0] = pack_move(m);
      int len = 1;
#ifdef _TRACE
      MoveImage(m,cout); cout << ' ';
#endif
      for (; !IsNull(stats.best_line[len-1]) && len < MAX_PV_LENGTH; len++) {
#ifdef _TRACE
         MoveImage(stats.best_line[len-1],cout);
         cout << ' ';
#endif
         pv[len] = pack_move(stats.best_line[len-1]);
      }
#ifdef _TRACE
      cout << endl;
#endif
      pvs.push_back(Pv(score,pv,len));
      if (first) {
         first = false;
         if (score > alpha && score < beta) {
            nth++;
            record_value = score;
#ifdef _TRACE
            MoveImage(m,cout);
            cout << " (record) " << record_value << endl;
#endif
         } else {
            // means a mate score
#ifdef _TRACE
            cout << "record value out of bounds" << endl;
#endif
            break; // score for record move out of bounds
         }
      } else {
#ifdef _TRACE
         MoveImage(m,cout);
         cout << ' ' << score << endl;
#endif
         double func_value = func(score-record_value);
         pdata.target += func_value;
#ifdef _TRACE
         cout << "diff=" << score-record_value << " target increment = " << func_value << " target sum=" << pdata.target << endl;
#endif
         pdata.num_moves++;
         if (score > alpha && score < beta) {
            nc++;
         }
         else {
            pdata.target_out_window += func_value;
            if (score >= record_value) nth++;
         }
      }
      if (score > alpha) {
         alpha = score;
      }
   }
   if (nth - 1 >= 0) {
      pdata.result_norm++;
      if (nth-1 < NUM_RESULT) pdata.result[nth-1]++;
   }
}

static int read_next_game(ifstream &infile, const string &file_name,
                          vector<Move>&moves, unsigned long &games,
                          string &result)
{
   ArasanVector<ChessIO::Header> hdrs;
   if (!infile.eof() && infile.good()) {
      long first;
      hdrs.removeAll();
      int c;
      // skip to start of next header (handles cases where
      // comment follows end of previous game).
      while (infile.good() && (c = infile.get()) != EOF) {
         if (c=='[') {
            infile.putback(c);
            break;
         }
      }
      ChessIO::collect_headers(infile,hdrs,first);
      if (hdrs.length()) {
         ++games;
         int move_num = 0;
         Board board;

         for (;;) {
            string num;
            ChessIO::Token tok = ChessIO::get_next_token(infile);
            if (tok.type == ChessIO::Eof) {
               break;
            }
            else if (tok.type == ChessIO::Comment) {
               // ignore
            }
            else if (tok.type == ChessIO::Number) {
               num = tok.val;
               stringstream s(tok.val);
               s >> move_num;
            }
            else if (tok.type == ChessIO::GameMove) {
               // parse the move
               Move move = Notation::value(board,board.sideToMove(),Notation::SAN_IN,tok.val);
               if (IsNull(move)) {
                  cerr << "Illegal move: " << tok.val <<
                     " in game " << games << ", file " <<
                     file_name << endl;
                  return -1;
               }
               else {
#ifdef LEGALITY_CHECK
                  int move_indx = get_move_indx(board,move);
                  if (move_indx == -1) {
                     cerr << "Illegal move: " << tok.val <<
                        " in game " << games << ", file " <<
                        file_name << endl;
                     return -1;
                  } else {
                     moves.push_back(move);
                  }
#else
                  moves.push_back(move);
#endif
               }
               board.doMove(move);
            }
            else if (tok.type == ChessIO::Unknown) {
               cerr << "Unrecognized text: " << tok.val <<
                  " in game " << games << ", file " <<
                  file_name << endl;
            }
            else if (tok.type == ChessIO::Result) {
               result = tok.val;
            }
            else if (tok.type == ChessIO::OpenVar || tok.type == ChessIO::CloseVar) {
               cerr << "Warning: variations detected in game " << games << ", file " << file_name << endl;
               break;
            }
            else {
               // ignore
            }
         }
      }
   }
   return moves.size() > 0;
}

static void parse1(ThreadData &td, Parse1Data &pdata, int id)
{
   pdata.clear();
   int have_next_game = 1;
   vector<Move> moves;
   string result;
   while (have_next_game) {
      Board board;
      if (moves.size()) {
         if (verbose) cout << "game " << games << " thread " << id << endl;
         GameInfo *g;
         try {
            g = new GameInfo;
         } catch(std::bad_alloc) {
            cerr << "out of memory!" << endl;
            exit(-1);
         }
         g->result = result;
         for (vector<Move>::const_iterator it = moves.begin();
              it != moves.end() && !IsNull(*it);
              it++) {
            Move record_move = *it;
            PositionInfo pi(record_move);
            if (texel) {
               // allow duplicate position/move pairs
               make_pv(board, td, pi.pvs, pdata, record_move, result);
            }
            else {
               // see if the pair position hash/move has already been used
               BoardState s(board.state);
               board.doMove(record_move);
               Lock(hash_lock);
               PositionDupEntry &val = (*hash_table)[board.hashCode()];
               // Note: duplicated position/moves are recorded with an empty
               // pv, so we can trace through the game again, but
               // we won't otherwise use this position record.
               board.undoMove(record_move,s);
               if (val.val == 0) { // not already present
                  val.val++; // mark present
                  Unlock(hash_lock);
                  make_pv(board, td, pi.pvs, pdata, record_move, result);
               }
               else {
                  Unlock(hash_lock);
               }
            }
            g->pis.push_back(pi);
            board.doMove(record_move);
            
         }
         Lock(data_lock);
         tmpdata.push_back(g);
         Unlock(data_lock);
      }
      // iterate until valid game read or EOF
      for (;;) {
         moves.clear();
         Lock(file_lock);
         try {
            have_next_game = read_next_game(game_file, game_file_name, moves, games, result);
         } catch(std::bad_alloc) {
            cerr << "out of memory reading game!" << endl;
            Unlock(file_lock);
            exit(-1);
         }
         Unlock(file_lock);
         if (have_next_game != -1) break;
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
       const uint32 pieces = ourmat.pieceBits();
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
   const int mLevel = board.getMaterial(oside).materialLevel();
   const int ourMatLevel = board.getMaterial(side).materialLevel();
   const Bitboard opponent_pawn_attacks(board.allPawnAttacks(oside));
   const Scoring::PawnHashEntry &pawn_entr = s.pawnEntry(board,!validate);
   const Scoring::PawnHashEntry::PawnData ourPawnData = pawn_entr.pawnData(side);
   const Scoring::PawnHashEntry::PawnData oppPawnData = pawn_entr.pawnData(oside);
   
   const Material &ourmat = board.getMaterial(side);
   const Material &oppmat = board.getMaterial(oside);
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
   }
   Bitboard rook_bits(board.rook_bits[side]);
   while (rook_bits.iterate(sq)) {
      if (board.pinOnRankOrFile(sq, okp, oside)) {
         pin_count++;
      }
      Bitboard rattacks2(board.rookAttacks(sq, side));
      int mobl = Bitboard(rattacks2 &~board.allOccupied &~opponent_pawn_attacks).bitCount();
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
      if (rattacks2 & pawn_entr.pawnData(oside).weak_pawns) {
         grads[Tune::ROOK_ATTACKS_WEAK_PAWN_MID] += tune_params.scale(inc,Tune::ROOK_ATTACKS_WEAK_PAWN_MID,mLevel);
         grads[Tune::ROOK_ATTACKS_WEAK_PAWN_END] += tune_params.scale(inc,Tune::ROOK_ATTACKS_WEAK_PAWN_END,mLevel);
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
      inc_adjust /= 16.0;
      Bitboard it(board.pawn_bits[side]);
      Square sq;
      while(it.iterate(sq)) {
         grads[Tune::KING_OWN_PAWN_DISTANCE] +=
           tune_params.scale(inc_adjust*(4-Scoring::distance(kp,sq)),Tune::KING_OWN_PAWN_DISTANCE,mLevel);
         k_pos += (4-Scoring::distance(kp,sq))*tune_params[Tune::KING_OWN_PAWN_DISTANCE].current/16;
         int file = File(sq);
         int rank = Rank (sq,side);
         if (ourPawnData.passers.isSet(sq) && rank >= 6 &&
             (File(kp) == file - 1 || File(kp) == file + 1) &&
             Rank(kp, side) >= rank) {
            if (rank == 6) {
               k_pos += tune_params[Tune::SUPPORTED_PASSER6].current/16;
               grads[Tune::SUPPORTED_PASSER6] +=
                  tune_params.scale(inc_adjust,Tune::SUPPORTED_PASSER6,mLevel);
            }
            else {
               k_pos += tune_params[Tune::SUPPORTED_PASSER7].current/16;
               grads[Tune::SUPPORTED_PASSER7] +=
                  tune_params.scale(inc_adjust,Tune::SUPPORTED_PASSER7,mLevel);
            }
         }
      }
      it = board.pawn_bits[oside];
      while (it.iterate(sq)) {
         grads[Tune::KING_OPP_PAWN_DISTANCE] +=
             tune_params.scale(inc_adjust*(4-Scoring::distance(kp,sq)),Tune::KING_OPP_PAWN_DISTANCE,mLevel);
         k_pos += (4-Scoring::distance(kp,sq))*tune_params[Tune::KING_OPP_PAWN_DISTANCE].current/16;
         int rank = Rank(sq,oside);
         if (oppPawnData.passers.isSet(sq) && Rank(kp,oside)>=rank) {
            Square queenSq = MakeSquare(File(sq),8,oside);
            grads[Tune::KING_OPP_PASSER_DISTANCE+rank-2] +=
               tune_params.scale(inc_adjust*((4-Scoring::distance(kp,sq))+(4-Scoring::distance(kp,queenSq)))/2,Tune::KING_OPP_PASSER_DISTANCE+rank-2,mLevel);
            k_pos += ((4-Scoring::distance(kp,sq))+(4-Scoring::distance(kp,queenSq)))*tune_params[Tune::KING_OPP_PASSER_DISTANCE+rank-2].current/(16*2);
         }
      }
      if (pieces < 3) {
         // compute partial derivative of KING_POSITION_LOW_MATERIAL
         grads[Tune::KING_POSITION_LOW_MATERIAL0+pieces] +=
            tune_params.scale(k_pos*inc/128.0,Tune::KING_POSITION_LOW_MATERIAL0+pieces,mLevel);
      }
   }

   bool deep_endgame = board.getMaterial(side).materialLevel() <= Scoring::Params::MIDGAME_THRESHOLD;
   if (pin_count && !deep_endgame) {
      grads[Tune::PIN_MULTIPLIER_MID] += tune_params.scale(inc,Tune::PIN_MULTIPLIER_MID,board.getMaterial(side).materialLevel())*pin_count;
   }
   if (pin_count) {
      grads[Tune::PIN_MULTIPLIER_END] += tune_params.scale(inc,Tune::PIN_MULTIPLIER_END,mLevel)*pin_count;
   }

   int mobl = Bitboard(Attacks::king_attacks[board.kingSquare(side)] & ~board.allOccupied & ~board.allAttacks(oside)).bitCount();
   grads[Tune::KING_MOBILITY_ENDGAME+mobl] += tune_params.scale(inc,Tune::KING_MOBILITY_ENDGAME+mobl,mLevel);
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
}

void validateGradient(Scoring &s, const Board &board, ColorType side, double eval) {
   vector<double> derivs(tune_params.numTuningParams(),0.0);
   double inc = 1.0;
   // compute the partial derivative vector for this position
   update_deriv_vector(s, board, side, derivs, inc);
   update_deriv_vector(s, board, OppositeColor(side), derivs, -inc);
   for (int i = 0; i < tune_params.numTuningParams(); i++) {
      if (derivs[i] != 0.0) {
         Tune::TuneParam p = tune_params[i];
         int val = p.current;
         int range = p.max_value - p.min_value;
         int delta = Util::Max(1,range/20);
         // increase by delta
         int newval = val + delta;
         tune_params.updateParamValue(i,newval);
         tune_params.applyParams();
         int score = s.evalu8(board,false);
         if (board.sideToMove() != side) {
            score = -score;
         }
         // compare predicted new value from gradient with
         // actual value
         if (fabs(eval + derivs[i]*delta - score)>5.0) {
            cerr << board << endl;
            cerr << "name=" << p.name << " mLevels=" << board.getMaterial(White).materialLevel() << " " << board.getMaterial(White).materialLevel() << " delta=" << delta << " val=" << val << " newval=" << newval << " deriv=" << derivs[i] << " old score=" << eval << " predicted score=" << eval + derivs[i]*delta << " actual score=" << score << endl;
            // The following code is useful when running under
            // gdb - it recomputes the before and after eval.
            tune_params.updateParamValue(i,val);
            tune_params.applyParams();
            s.evalu8(board,false);
            tune_params.updateParamValue(i,newval);
            tune_params.applyParams();
            s.evalu8(board,false);
         }
         // restore old value
         tune_params.updateParamValue(i,val);
         tune_params.applyParams();
      }
   }
}

// Computes one summand of equation (8) in the paper
static void calc_derivative(Scoring &s, Parse2Data &data, const Board &board, const PositionInfo &pi, const string &result) {

   unsigned int nc = 0;
   double record_value;
   double sum_dT = 0.0;

#ifdef _TRACE
   cout << "game position " << board << endl;
#endif
   const ColorType turn0 = board.sideToMove();

   // First PV is based on the "record move" (move actually played).
   // move board position to end of PV
   const Pv &pv_first = pi.pvs[0];

   Board board_copy(board);
   int ply = 0;

   for (;ply<pv_first.len;++ply) {
      Move m = unpack_move(board_copy, pv_first.pv[ply]);
      if (IsNull(m)) break;
#ifdef _TRACE
      MoveImage(m,cout);
      cout << ' ' << (flush);
#endif
      board_copy.doMove( m );
   }
#ifdef _TRACE
   cout << endl;
#endif
   // skip KPK positions (evaluated by bitbases)
   if ((board_copy.getMaterial(White).kingOnly() &&
       board_copy.getMaterial(Black).infobits() == Material::KP) ||
       (board_copy.getMaterial(Black).kingOnly() &&
        board_copy.getMaterial(White).infobits() == Material::KP)) {
      return;
   }

   // save position at end of 1st pv
   Board record_board(board_copy);
   record_value = s.evalu8(board_copy,!validate);

   if (board.sideToMove() != board_copy.sideToMove()) {
      record_value = -record_value;
   }

#ifdef _TRACE
   MoveImage(unpack_move(board,pv_first.pv[0]),cout);
   cout << ' ' << record_value << endl;
#endif

   if (texel) {
      // only need the value at the end of the 1st pv
      double func_value = computeErrorTexel(record_value,result,board.sideToMove());
      // compute the change in loss function per delta in eval
      // (partial derivative)
      double dT = computeTexelDeriv(record_value,result,board.sideToMove());
      // multiply the derivative by the x (feature) value, scaled if necessary
      // by game phase, and add to the gradient sum.
      update_deriv_vector(s, board, White, data.grads, dT);
      update_deriv_vector(s, board, Black, data.grads, -dT);
      data.target += func_value;
      if (validate) {
         validateGradient(s, board_copy, board.sideToMove(), record_value);
      }
      return;
   }

   // iterate over the remaining legal moves
   if (pi.pvs.size() > 1) {
      for (vector<Pv>::const_iterator it = pi.pvs.begin()+1;
           it != pi.pvs.end();
           it++) {
         board_copy = board;
         const Pv & pv = *it;
         int ply = 0;
         for (;ply<pv.len;++ply) {
            Move m = unpack_move(board_copy, pv.pv[ply]);
            if (IsNull(m)) break;
#ifdef _TRACE
            MoveImage(m,cout);
            cout << ' ';
#endif
            board_copy.doMove( m );
         }
#ifdef _TRACE
         cout << endl;
#endif
         // skip KPK positions (evaluated by bitbases)
         if ((board_copy.getMaterial(White).kingOnly() &&
              board_copy.getMaterial(Black).infobits() == Material::KP) ||
             (board_copy.getMaterial(Black).kingOnly() &&
              board_copy.getMaterial(White).infobits() == Material::KP)) {
            continue;
         }
         double value = double(s.evalu8(board_copy,false));
         // make score be from the perspective of "board", the head of
         // the PV:
         if (board.sideToMove() != board_copy.sideToMove()) {
            value = -value;
         }
#ifdef _TRACE
         MoveImage(unpack_move(board,(*it).pv[0]),cout);
         cout << ' ' << value << endl;
#endif
         double func_value, dT;
         // accumulate differences with record value
         func_value = func( value - record_value );
#ifdef _TRACE
         cout << "diff=" << value-record_value << " target increment = " << func_value << " target sum=" << data.target << endl;
#endif
         // and compute dT (first factor of equation (8))
         dT = dfunc( value - record_value );
         if (turn0 == Black) {
            dT = -dT;
         }
         sum_dT += dT;
         // update derivatives with a new summand. This is based on
         // the position at the of the pv.
         update_deriv_vector(s, board_copy, Black, data.grads, dT);
         update_deriv_vector(s, board_copy, White, data.grads, -dT);
         data.target += func_value;
         if (validate) {
            validateGradient(s, board_copy, board.sideToMove(), value);
         }
         nc++;
      }
   }

   // perform diff with record move position (2nd term of 2nd factor
   // in equation (8))
   update_deriv_vector(s, record_board, Black, data.grads, -sum_dT);
   update_deriv_vector(s, record_board, White, data.grads, sum_dT);
#ifdef _TRACE
   cout << "accumulated target: " << data.target << endl;
#endif
}

static void parse2(ThreadData &td, Parse2Data &data)
{
   data.target = 0.0;
   data.grads.clear();
   data.grads.resize(tune_params.numTuningParams(),0.0);

   // This is large so allocate on heap:
   Scoring *s = new Scoring();
   const size_t max = tmpdata.size();
   for (;;) {
      // obtain the next available game from the vector
      size_t next = (size_t)phase2_game_index.fetch_add(1);
      if (next >= max) break;
      if (verbose) cout << "game " << next << " thread " << td.index << endl;
      GameInfo *g = tmpdata[next];
      // iterate over the game moves and the positions derived from them
      Board board;
      int ply = 0;
      for (vector<PositionInfo>::const_iterator it2 = g->pis.begin();
           it2 != g->pis.end();
           it2++, ply++) {
         PositionInfo pi = *it2;
         // skip calculation if no pvs (this indicates a position/
         // record move pair that is a duplicate).
         if (pi.pvs.size() > 0 && ply >= MIN_PLY) {
            calc_derivative(*s, data, board, pi, g->result);
         }
         board.doMove(pi.record_move);
      }
   }
   delete s;
//   if (verbose) cout << "thread " << td.index << " complete.";
}

static void adjust_params(Parse2Data &data0, vector<double> &historical_gradient, int iterations)
{
   for (int i = 0; i < tune_params.numTuningParams(); i++) {
      double dv = data0.grads[i];
      Tune::TuneParam p;
      tune_params.getParam(i,p);
      int v = p.current;
      if (regularize) {
         // add the derivative of the regularization term. Note:
         // non-tunable parameters will have derivative zero and
         // we don't regularize them.
         if (p.tunable) {
            dv += 2*REGULARIZATION*norm_val(p);
         }
      }
      if (dv != 0.0) {
         int istep = 1;
         if (use_adagrad) {
            historical_gradient[i] += dv*dv;
            double adjusted_grad  = dv/(ADAGRAD_FUDGE_FACTOR+sqrt(historical_gradient[i]));
            istep = Util::Round(ADAGRAD_STEP_SIZE*adjusted_grad);
            //cout << i << " step: " << istep << " variance " << dv << " adjusted grad " << adjusted_grad <<  endl;
            v = Util::Max(p.min_value,Util::Min(p.max_value,v + istep));
         } else {
            if ( dv > 0.0) {
               v = Util::Min(p.max_value,v + istep);
            }
            else if (dv < 0.0) {
               v = Util::Max(p.min_value,v - istep);
            }
         }
         tune_params.updateParamValue(i,v);
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
}

static void output_solution()
{
   tune_params.applyParams();
   ofstream param_out(out_file_name.c_str(),ios::out | ios::trunc);
   Scoring::Params::write(param_out);
   param_out << endl;
   ofstream x0_out(x0_file_name.c_str(),ios::out | ios::trunc);
   tune_params.writeX0(x0_out);
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
   uint64 num_moves = 0;
#ifdef _MSC_VER
   double best = 1.0e10;
#else
   double best = numeric_limits<double>::max();
#endif
   vector<double> historical_grad(tune_params.numTuningParams(),0.0);
   for (int iter = 1; iter <= iterations; iter++) {
      cout << "iteration " << iter << endl;
      tune_params.applyParams();
      int recalc;
      if (texel)
         recalc = iter == 1;
      else
         recalc = ((iter-1) % PV_RECALC_INTERVAL) == 0;
      if (recalc) {
         if (verbose) cout << "(re)calculating PVs" << endl;
         // clean up data from previous pass
         while (!tmpdata.empty()) {
            delete tmpdata.back();
            tmpdata.pop_back();
         }
         // reset global game count
         games = 0;
         learn_parse(Phase1, cores);
         // sum results over workers into 1st data element
         for (int i = 1; i < cores; i++) {
            data1[0].num_moves += data1[i].num_moves;
            data1[0].target += data1[i].target;
            data1[0].target_out_window += data1[i].target_out_window;
            data1[0].result_norm += data1[i].result_norm;
         }
         if (data1[0].result_norm == 0) data1[0].result_norm = 1;
         if (data1[0].num_moves == 0) data1[0].num_moves = 1;
         num_moves = data1[0].num_moves;
         data1[0].target /= num_moves;
         if (verbose) {
            cout << "target=" << data1[0].target << " penalty=" << calc_penalty() << endl;
         }
         data1[0].target += calc_penalty();
         if (verbose) {
            cout << "pass 1 objective = " << data1[0].target << endl;
         }
         // rewind game file
         game_file.clear();
         game_file.seekg(0,ios::beg);
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
      data2[0].target /= num_moves;
      cout << "pass 2 target=" << data2[0].target << " penalty=" << calc_penalty
() << " objective=" << data2[0].target + calc_penalty() << endl;
      data2[0].target += calc_penalty();
      if (data2[0].target < best) {
         best = data2[0].target;
         cout << "new best objective: " << best << endl;
         output_solution();
      }
      adjust_params(data2[0],historical_grad,iter);
   }

   delete hash_table;
   LockDestroy(data_lock);
   LockFree(data_lock);
   LockDestroy(file_lock);
   LockFree(file_lock);
   LockDestroy(hash_lock);
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

    for (arg = 1; arg < argc && argv[arg][0] == '-'; ++arg) {
       if (strcmp(argv[arg],"-a")==0) {
          use_adagrad = true;
       }
       else if (strcmp(argv[arg],"-d")==0) {
          cerr << "writing initial solution" << endl;
          ++write_sol;
       }
       else if (strcmp(argv[arg],"-o")==0) {
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
       else if (strcmp(argv[arg],"-t")==0) {
          texel = true;
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
       } else {
          cerr << "invalid option: " << argv[arg] << endl;
          usage();
          exit(-1);
       }
    }

    if (write_sol) {
      output_solution();
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

    if (texel) LEARNING_SEARCH_WINDOW = 7*PAWN_VALUE;

    options.search.hash_table_size = 0;
    options.search.easy_threshold = LEARNING_SEARCH_WINDOW;
    options.learning.position_learning = 0;
    options.book.book_enabled = options.log_enabled = 0;
    options.learning.position_learning = false;
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS)
    options.search.use_tablebases = false;
#endif
    game_file_name = argv[arg];

    if (verbose) cout << "game file: " << game_file_name << endl;

    game_file.open(game_file_name.c_str());

    if (game_file.fail()) {
       cerr << "failed to open file " << game_file_name << endl;
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
