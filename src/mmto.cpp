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
#include "mmto.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <vector>
#if __cplusplus >= 201103L
#include <unordered_map>
#else
#include <map>
#endif
extern "C" {
#include <math.h>
#include <ctype.h>
#include <unistd.h>
};

// MMTO tuning code for Arasan, based on:
// Kunihuto Hoki & Tomoyuki Kaneko, "Large-Scale Optimization for Evaluation
// Functions with Minimax Search,"
// Journal of Articial Intelligence Research 49 (2014) 527-568.
// Partly based on Bonanza source code.

using namespace mmto;

#define REGULARIZE

static int iterations = 2;

static int cores = 1;

static bool terminated = false;

static string out_file_name="params.cpp";

static string x0_file_name="x0";

static string game_file_name = "games.pgn";

static ifstream game_file;

static bool verbose = false;

static const int MAX_PV_LENGTH = 256;
static const int NUM_RESULT = 8;
static const int MAX_CORES = 64;
static const int THREAD_STACK_SIZE = 8*1024*1024;
static const int LEARNING_SEARCH_DEPTH = 1;
static const int LEARNING_SEARCH_WINDOW = 3*PAWN_VALUE;
// L2-regularization factor
static const double REGULARIZATION = 1.0E-4;
static const int PV_RECALC_INTERVAL = 16;

static int first_index = 0;

static int last_index = Tune::NUM_MISC_PARAMS;

static vector<GameInfo *> tmpdata;

LockDefine(data_lock);
LockDefine(file_lock);

static pthread_attr_t stackSizeAttrib;

struct PositionDupEntry
{
   PositionDupEntry() : val(0)
      {
      }

   int val;
};

#if __cplusplus >= 201103L
static unordered_map<hash_t,PositionDupEntry> *hash_table;
#else
static map<hash_t,PositionDupEntry> *hash_table;
#endif

enum Phase {Phase1, Phase2};

// game count, used in phase 1
unsigned long games = 0;

struct ThreadData {
    SearchController *searcher;
    int index;
    Phase phase;
    void *data;
    double penalty;
    sem_t sem;
    sem_t done;
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

static ThreadData threadDatas[MAX_CORES];
static Parse1Data data1[MAX_CORES];
static Parse2Data data2[MAX_CORES];

static void usage()
{
   cerr << "Usage: mmto -c <cores>" << endl;
   cerr << "-d just write out current parameters values to params.cpp" << endl;
   cerr << "-i <input parameter file> -o <output parameter file>" << endl;
   cerr << "-x <output objective file>" << endl;
   cerr << "-f <first_parameter_name> -s <last_parameter_name>" << endl;
   cerr << "-n <iterations>" << endl;
}

static double func( double x ) {
   const double delta = (double)LEARNING_SEARCH_WINDOW / 7.0;
   if ( x < -LEARNING_SEARCH_WINDOW ) {
      x = -LEARNING_SEARCH_WINDOW;
   }
   else if ( x >  LEARNING_SEARCH_WINDOW ) {
      x = LEARNING_SEARCH_WINDOW;
   }
   return 1.0 / ( 1.0 + exp(-x/delta) );
}

// computes derivative of the sigmoid function "func", above
static double dfunc( double x )
{
   const double delta = (double)LEARNING_SEARCH_WINDOW / 7.0;
   double dd, dn, dtemp, dret;
   if      ( x <= -LEARNING_SEARCH_WINDOW ) {
      dret = 0.0;
   }
   else if ( x >= LEARNING_SEARCH_WINDOW ) {
      dret = 0.0;
   }
   else {
      dn    = exp( - x / delta );
      dtemp = dn + 1.0;
      dd    = delta * dtemp * dtemp;
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
#ifdef REGULARIZE
   for (int i = first_index; i < last_index; i++) {
      Tune::TuneParam p;
      tune_params.getParam(i,p);
      // apply penalty only for parameters being tuned
      if (p.tunable) {
         // normalize the values since their ranges differ
         l2 += REGULARIZATION*norm_val(p)*norm_val(p);
      }
   }
#endif
   return l2;
}

// search all moves from a position, creating a Pv for each legal move
static void make_pv(const Board &board, ThreadData &td,
                   vector<Pv> &pvs,Parse1Data &pdata, Move record_move )
{
   RootMoveGenerator mg(board,NULL,record_move);
   Move m;
   bool first = true;
   int record_value = Scoring::INVALID_SCORE;
   int nth = 0;
   int nc = 0;

   int alpha = -Constants::MATE;
   int beta = Constants::MATE;
   Board board_copy(board);

   while (!IsNull(m = mg.nextMove())) {
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
                          vector<Move>&moves, unsigned long &games)
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
               break;
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
         for (vector<Move>::const_iterator it = moves.begin();
              it != moves.end() && !IsNull(*it);
              it++) {
            Move record_move = *it;
            // see if the pair position hash/move has already been used
            BoardState s(board.state);
            board.doMove(record_move);
            PositionDupEntry &val = (*hash_table)[board.hashCode()];
            // Note: duplicated position/moves are recorded with an empty
            // pv, so we can trace through the game again, but
            // we won't otherwise use this position record.
            board.undoMove(record_move,s);
            PositionInfo pi(record_move);
            if (val.val == 0) { // not already present
               val.val++; // mark present
               make_pv(board, td, pi.pvs, pdata, record_move);
            }
            g->push_back(pi);
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
            have_next_game = read_next_game(game_file, game_file_name, moves, games);
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
   const int mLevel = board.getMaterial(side).materialLevel();
   const Bitboard opponent_pawn_attacks(board.allPawnAttacks(oside));
   const Scoring::PawnHashEntry &pawn_entr = s.pawnEntry(board);
   grads[Tune::CASTLING0+(int)board.castleStatus(side)] +=
      tune_params.scale(1,Tune::CASTLING0+(int)board.castleStatus(side),mLevel);
   Bitboard knight_bits(board.knight_bits[side]);
   Square sq;
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
   if (bishop_bits.bitCountOpt() > 1) {
      grads[Tune::BISHOP_PAIR_MID] += tune_params.scale(inc,Tune::BISHOP_PAIR_MID,mLevel);
      grads[Tune::BISHOP_PAIR_END] += tune_params.scale(inc,Tune::BISHOP_PAIR_END,mLevel);
   }

   while (bishop_bits.iterate(sq)) {
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
      int mobl = Bitboard(board.rookAttacks(sq) &~board.allOccupied &~opponent_pawn_attacks).bitCount();
      grads[Tune::ROOK_MOBILITY_MIDGAME+mobl] += tune_params.scale(inc,Tune::ROOK_MOBILITY_MIDGAME+mobl,mLevel);
      grads[Tune::ROOK_MOBILITY_ENDGAME+mobl] += tune_params.scale(inc,Tune::ROOK_MOBILITY_ENDGAME+mobl,mLevel);
      int i = map_to_pst(sq,side);
      grads[Tune::ROOK_PST_MIDGAME+i] += tune_params.scale(inc,Tune::ROOK_PST_MIDGAME+i,mLevel);
      grads[Tune::ROOK_PST_ENDGAME+i] += tune_params.scale(inc,Tune::ROOK_PST_ENDGAME+i,mLevel);
      if (Rank(sq,side) == 7 && (Rank(board.kingSquare(OppositeColor(side)),side) == 8 || (board.pawn_bits[board.oppositeSide()] & Attacks::rank7mask[side]))) {
         grads[Tune::ROOK_ON_7TH_MID] += tune_params.scale(inc,Tune::ROOK_ON_7TH_MID,mLevel);
         grads[Tune::ROOK_ON_7TH_END] += tune_params.scale(inc,Tune::ROOK_ON_7TH_END,mLevel);
         Bitboard right(Attacks::rank_mask_right[sq] & board.occupied[side]);
         if (right && board[right.firstOne()] == MakePiece(Rook, side)) {
            // 2 connected rooks on 7th
            grads[Tune::TWO_ROOKS_ON_7TH_MID] += tune_params.scale(inc,Tune::TWO_ROOKS_ON_7TH_MID,mLevel);
            grads[Tune::TWO_ROOKS_ON_7TH_END] += tune_params.scale(inc,Tune::TWO_ROOKS_ON_7TH_END,mLevel);
         }
      }
      if (FileOpen(board, File(sq))) {
         grads[Tune::ROOK_ON_OPEN_FILE_MID] += tune_params.scale(inc,Tune::ROOK_ON_OPEN_FILE_MID,mLevel);
         grads[Tune::ROOK_ON_OPEN_FILE_END] += tune_params.scale(inc,Tune::ROOK_ON_OPEN_FILE_END,mLevel);
      }
      Bitboard rattacks2(board.rookAttacks(sq, side));
      if (rattacks2 & pawn_entr.pawnData(oside).weak_pawns) {
         grads[Tune::ROOK_ATTACKS_WEAK_PAWN_MID] += tune_params.scale(inc,Tune::ROOK_ATTACKS_WEAK_PAWN_MID,mLevel);
         grads[Tune::ROOK_ATTACKS_WEAK_PAWN_END] += tune_params.scale(inc,Tune::ROOK_ATTACKS_WEAK_PAWN_END,mLevel);
      }
   }
   Bitboard queen_bits(board.queen_bits[side]);
   while (queen_bits.iterate(sq)) {
      int mobl = Bitboard(board.queenAttacks(sq) &~board.allOccupied &~opponent_pawn_attacks).bitCount();
      grads[Tune::QUEEN_MOBILITY_MIDGAME+mobl] += tune_params.scale(inc,Tune::QUEEN_MOBILITY_MIDGAME+mobl,mLevel);
      grads[Tune::QUEEN_MOBILITY_ENDGAME+mobl] += tune_params.scale(inc,Tune::QUEEN_MOBILITY_MIDGAME+mobl,mLevel);
      int i = map_to_pst(sq,side);
      grads[Tune::QUEEN_PST_MIDGAME+i] += tune_params.scale(inc,Tune::QUEEN_PST_MIDGAME+i,mLevel);
      grads[Tune::QUEEN_PST_ENDGAME+i] += tune_params.scale(inc,Tune::QUEEN_PST_ENDGAME+i,mLevel);
   }
   Square ksq = board.kingSquare(side);
   int i = map_to_pst(ksq,side);
   grads[Tune::KING_PST_MIDGAME+i] += tune_params.scale(inc,Tune::KING_PST_MIDGAME+i,mLevel);
   grads[Tune::KING_PST_ENDGAME+i] += tune_params.scale(inc,Tune::KING_PST_ENDGAME+i,mLevel);
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

         Square own_blocker, opp_blocker;
         if (side == White) {
            opp_blocker = (mask & board.occupied[oside]).firstOne();
            own_blocker = (mask & board.occupied[side]).firstOne();
         }
         else {
            opp_blocker = (mask & board.occupied[oside]).lastOne();
            own_blocker = (mask & board.occupied[side]).lastOne();
         }
         if (own_blocker != InvalidSquare) {
            int index = pp_block_index(sq,own_blocker,side);
            grads[Tune::PP_OWN_PIECE_BLOCK_MID+index] +=
               tune_params.scale(inc,Tune::PP_OWN_PIECE_BLOCK_MID+index,mLevel);
            grads[Tune::PP_OWN_PIECE_BLOCK_END+index] +=
               tune_params.scale(inc,Tune::PP_OWN_PIECE_BLOCK_END+index,mLevel);
         }
         if (opp_blocker != InvalidSquare) {
            int index = pp_block_index(sq,opp_blocker,side);
            grads[Tune::PP_OPP_PIECE_BLOCK_MID+index] +=
               tune_params.scale(inc,Tune::PP_OPP_PIECE_BLOCK_MID+index,mLevel);
            grads[Tune::PP_OPP_PIECE_BLOCK_END+index] +=
               tune_params.scale(inc,Tune::PP_OPP_PIECE_BLOCK_END+index,mLevel);
         }
      }
      if (pds[i].flags & Scoring::PawnDetail::POTENTIAL_PASSER) {
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
            tune_params.scale(inc,Tune::CONNECTED_PASSER_MID2+Rank(pds[i].sq,side),mLevel);
         grads[Tune::CONNECTED_PASSER_END2+Rank(pds[i].sq,side)-2] +=
            tune_params.scale(inc,Tune::CONNECTED_PASSER_END2+Rank(pds[i].sq,side),mLevel);
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

// Computes one summand of equation (8) in the paper
static void calc_derivative(Scoring &s, Parse2Data &data, const Board &board, const PositionInfo &pi ) {

   unsigned int nc = 0;
   int record_value;
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
   // save position at end of 1st pv
   Board record_board(board_copy);
   record_value = s.evalu8(board_copy);

   if (board.sideToMove() != board_copy.sideToMove()) {
      record_value = -record_value;
   }

#ifdef _TRACE
   MoveImage(unpack_move(board,pv_first.pv[0]),cout);
   cout << ' ' << record_value << endl;
#endif

   // now iterate over the remaining legal moves
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
         int value = s.evalu8(board_copy);

         if (board.sideToMove() != board_copy.sideToMove()) {
            value = -value;
         }
#ifdef _TRACE
         MoveImage(unpack_move(board,(*it).pv[0]),cout);
         cout << ' ' << value << endl;
#endif
         // accumulate differences with record value
         const double func_value = func( value - record_value );
         data.target += func_value;
#ifdef _TRACE
         cout << "diff=" << value-record_value << " target increment = " << func_value << " target sum=" << data.target << endl;
#endif
         // and compute dT (first factor of equation (8))
         double dT = dfunc( value - record_value );
         if (turn0 == Black) {
            dT = -dT;
         }
         sum_dT += dT;
         // update derivatives with a new summand. This is based on
         // the position at the of the pv.
         update_deriv_vector(s, board_copy, Black, data.grads, dT);
         update_deriv_vector(s, board_copy, White, data.grads, -dT);
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

   Scoring s;
   const int max = tmpdata.size();
   // each thread reads distinct games from the vector
   for (int i = td.index;
        i < max;
        i += cores) {
      GameInfo *g = tmpdata[i];
      // iterate over the game moves and the positions derived from them
      Board board;
      for (vector<PositionInfo>::const_iterator it2 = g->begin();
           it2 != g->end();
           it2++) {
         PositionInfo pi = *it2;
         // skip calculation if no pvs (this indicates a position/
         // record move pair that is a duplicate).
         if (pi.pvs.size() > 0) {
            calc_derivative(s, data, board, pi);
         }
         board.doMove(pi.record_move);
      }
   }
   if (verbose) cout << "thread " << td.index << " complete.";
}

static void adjust_params(Parse2Data &data0, int iterations)
{
   for (int i = 0; i < tune_params.numTuningParams(); i++) {
      double dv = data0.grads[i];
      Tune::TuneParam p;
      tune_params.getParam(i,p);
      int v = p.current;
#ifdef REGULARIZE
      // add the derivative of the regularization term. Note:
      // non-tunable parameters will have derivative zero and
      // we don't regularize them.
      if (p.tunable) {
         dv += 2*REGULARIZATION*norm_val(p);
      }
#endif
      // TBD size the step based on parameter range?
//      const int istep = Util::Max(1,(p.max_value-p.min_value)/(100+2*iterations));
      const int istep = 1;
      if ( dv > 0.0) {
         v = Util::Min(p.max_value,v + istep);
      }
      else if (dv < 0.0) {
         v = Util::Max(p.min_value,v - istep);
      }
      tune_params.updateParamValue(i,v);
   }
}

static void * CDECL threadp(void *x)
{
   ThreadData *td = (ThreadData*)x;

   // set stack size
   size_t stackSize;
   if (pthread_attr_getstacksize(&stackSizeAttrib, &stackSize)) {
        perror("pthread_attr_getstacksize");
        return 0;
   }
   if (stackSize < THREAD_STACK_SIZE) {
      if (pthread_attr_setstacksize (&stackSizeAttrib, THREAD_STACK_SIZE)) {
         perror("error setting thread stack size");
         return 0;
      }
   }

   // allocate controller in the thread
   try {
      td->searcher = new SearchController();
   } catch(std::bad_alloc) {
      cerr << "out of memory, thread " << td->index << endl;
      return 0;
   }

   while (!terminated) {
      // wait until signalled
      sem_wait(&td->sem);
      td->searcher->clearHashTables();
      // perform work based on phase
      if (td->phase == Phase1) {
         if (verbose) cout << "starting phase 1, thread " << td->index << endl;
         parse1(*td,data1[td->index],td->index);
      } else {
         if (verbose) cout << "starting phase 2, thread " << td->index << endl;
         parse2(*td,data2[td->index]);
      }
      // tell parent we are done
      sem_post(&td->done);
   }
   delete td->searcher;
   return 0;
}

static void initThreads()
{
   // prepare threads
   if (pthread_attr_init (&stackSizeAttrib)) {
      perror("pthread_attr_init");
      return;
   }
   for (int i = 0; i < cores; i++) {
      threadDatas[i].index = i;
      threadDatas[i].searcher = NULL;
      threadDatas[i].phase = Phase1;
      sem_init(&threadDatas[i].sem,0,0);
      sem_init(&threadDatas[i].done,0,0);
      if (pthread_create(&(threadDatas[i].thread_id), &stackSizeAttrib, threadp, (void*)&(threadDatas[i]))) {
         perror("thread creation failed");
      }
      if (verbose) cout << "thread " << i << " created." << endl;
   }
}

static void launch_threads()
{
   if (verbose) cout << "launch_threads" << endl;
   for (int i = 0; i < cores; i++) {
      // signal searchers to start
      sem_post(&threadDatas[i].sem);
   }
   // wait for all searchers done
   for (int i = 0; i < cores; i++) {
      sem_wait(&threadDatas[i].done);
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
#if __cplusplus >= 201103L
      hash_table = new unordered_map<hash_t,PositionDupEntry>();
#else
      hash_table = new map<hash_t,PositionDupEntry>();
#endif
   } catch(std::bad_alloc) {
      cerr << "hash table alloc: out of memory!" << endl;
      exit(-1);
   }

   LockInit(data_lock);
   LockInit(file_lock);
   uint64 num_moves = 0;
   double best = numeric_limits<double>::max();

   for (int iter = 1; iter <= iterations; iter++) {
      cout << "iteration " << iter << endl;
      tune_params.applyParams();
      if (((iter-1) % PV_RECALC_INTERVAL) == 0) {
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
         num_moves = (double)data1[0].num_moves;
         data1[0].target /= num_moves;
         if (verbose) {
            cout << "target=" << data1[0].target << " penalty=" << calc_penalty() << endl;
         }
         data1[0].target += calc_penalty();
         //double dtemp = data1[0].target_out_window / obj_norm;
         if (verbose) {
            cout << "pass 1 objective = " << data1[0].target << endl;
         }
         // rewind game file
         game_file.clear();
         game_file.seekg(0,ios::beg);
      }
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
      adjust_params(data2[0],iter);
   }

   delete hash_table;
   LockDestroy(data_lock);
   LockFree(data_lock);
   LockDestroy(file_lock);
   LockFree(file_lock);
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
    options.search.hash_table_size = 0;

//    if (EGTBMenCount) {
//        cerr << "Initialized tablebases" << endl;
//    }
    options.book.book_enabled = options.log_enabled = 0;
    options.learning.position_learning = false;
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS)
    options.search.use_tablebases = false;
#endif
//    options.search.easy_plies = 0;

    string input_file;

    int arg = 0;
    string first_param, last_param;
    string x0_input_file_name;

    int write_sol = 0;

    for (arg = 1; arg < argc && argv[arg][0] == '-'; ++arg) {
       if (strcmp(argv[arg],"-d")==0) {
          cout << "writing initial solution" << endl;
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
       else if (strcmp(argv[arg],"-f")==0) {
          ++arg;
          first_param = argv[arg];
       }
       else if (strcmp(argv[arg],"-s")==0) {
          ++arg;
          last_param = argv[arg];
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

    last_index = tune_params.numTuningParams();

    if (first_param.length()) {
       first_index = tune_params.findParamByName(first_param);
       if (first_index == -1) {
          cerr << "Error: Parameter named " << first_param << " not found." << endl;
          exit(-1);
       }
    }
    if (last_param.length()) {
       last_index = tune_params.findParamByName(last_param);
       if (last_index == -1) {
          cerr << "Error: Parameter named " << last_param << " not found." << endl;
          exit(-1);
       }
    }

    const int dim = last_index - first_index + 1;
    if (dim<=0) {
       cerr << "Error: 2nd named parameter is before 1st!" << endl;
       exit(-1);
    }
    if (arg >= argc) {
       cerr << "no file name specified" << endl;
       usage();
       exit(-1);
    }

    if (x0_input_file_name.length()) {
       ifstream x0_input_file(x0_input_file_name.c_str());
       if (x0_input_file.good()) {
          tune_params.readX0(x0_input_file);
          tune_params.applyParams();
       } else {
          cerr << "error opening x0 input file: " << x0_input_file_name << endl;
          exit(-1);
       }
    }

    game_file_name = argv[arg];

    if (verbose) cout << "game file: " << game_file_name << endl;

    game_file.open(game_file_name.c_str());

    if (game_file.bad()) {
       cerr << "failed to open file " << game_file_name << endl;
       exit(-1);
    }

    if (game_file.bad()) {
       cerr << "failed to open file " << game_file_name << endl;
       exit(-1);
    }

//    if (verbose) {
       cout << "parameter count: " << tune_params.numTuningParams() << " (";
       int tunable = 0;
       for (int i = 0; i < tune_params.numTuningParams(); i++) {
          Tune::TuneParam p;
          tune_params.getParam(i,p);
          if (p.tunable) ++tunable;
       }
       cout << tunable << " tunable)" << endl;
//    }

    initThreads();

    learn();

    return 0;
}
