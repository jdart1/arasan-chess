// Main driver module for Arasan chess engine.
// Handles Winboard/xboard/UCI protocol.
// Copyright 1997-2017 by Jon Dart. All Rights Reserved.
//
#include "arasanx.hpp"
#include "board.h"
#include "movegen.h"
#include "search.h"
#include "movearr.h"
#include "notation.h"
#include "scoring.h"
#include "chessio.h"
#include "bitboard.h"
#include "attacks.h"
#include "globals.h"
#include "log.h"
#include "calctime.h"
#include "eco.h"
#include "learn.h"
#include "boardio.h"
#include "legal.h"
#include "bitprobe.h"
#ifdef UNIT_TESTS
#include "unit.h"
#endif
#ifdef TUNE
#include "tune.h"
#endif
#ifdef GAVIOTA_TBS
#include "gtb.h"
#endif
#ifdef NALIMOV_TBS
#include "nalimov.h"
#endif
#ifdef SYZYGY_TBS
#include "syzygy.h"
#endif

#include <cstddef>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>

extern "C"
{
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#ifdef _WIN32
#include <io.h>
#else
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#endif
};
using namespace std;

static int verbose = 0, post = 0;
static SearchController *searcher = NULL;

static Move last_move;
static string last_move_image;
static Move last_computer_move;
static Statistics last_computer_stats;
static string game_pathname;
static ofstream *game_file = NULL;
static int time_left = 0;
static int opp_time = 0;
static float minutes = 5.0;
static int incr = 0; // Winboard increment
static int winc = 0; // UCI increment
static int binc = 0; // UCI increment
static int computer = 0;
static bool computer_plays_white = false;
static string opponent_name;
static bool ics = false;
static int forceMode = 0, analyzeMode = 0;
static int moves = 40;                            /* moves in a time control bloc */
static Board *ponder_board = NULL, *main_board = NULL;
static bool ponder_move_ok = false;
static Move predicted_move;
static int pondering = 0;
static Move ponder_move, best_move;
static Statistics stats, last_stats, ponder_stats;
static int time_target = 0;
static int last_time_target = INFINITE_TIME;
static int computer_rating = 0;
static int opponent_rating = 0;
// stack of pending commands
static list<string> pending;
static bool doTrace = false;
static bool easy = false;
static int game_end = 0;
static int result_pending = 0;
static score_t last_score = Constants::MATE;
static ECO *ecoCoder = NULL;
static string hostname;
static int xboard42 = 0;
static SearchType srctype = TimeLimit;
static int time_limit;
static int ply_limit;
static string start_fen;

static int uci = 0;                               // non-zero for UCI mode
static int movestogo = 0;
static int ponderhit = 0;
static int testing = 0;
// set true if waiting for "ponderhit" or "stop"
static int uciWaitState = 0;
static string test_file;
static int cpusSet = 0; // true if cmd line specifies -c
static int memorySet = 0; // true if cmd line specifies -H
static struct UciStrengthOpts
{
   bool limitStrength;
   int eco;

   UciStrengthOpts()
      : limitStrength(false),
        eco(2600)
      {
      }
} uciStrengthOpts;

#ifdef UCI_LOG
extern fstream ucilog;
#endif

// profile
uint64_t total_nodes = 0L;
int total_correct = 0L;
int total_tests = 0L;
uint64_t total_time = 0L;
int early_exit_plies = Constants::MaxPly;
int early_exit = 0;
int solution_move_count = 0;
time_t solution_time = 0;
uint64_t solution_nodes = (uint64_t)0;
int last_iteration_depth = -1;
int iterations_correct = 0;
int moves_to_search = 1;
int flip = 0;
Move solution_moves[10];
bool avoid = false;
vector<int> solution_times;
static uint64_t nodes_to_find_total;
static int depth_to_find_total;
static uint64_t time_to_find_total;
static int max_depth;
static struct
{
   Move move;
   score_t value;
   time_t time;
   int depth;
   uint64_t num_nodes;
} search_progress[256];

static string cmd_buf;
static ThreadControl inputSem;

// forward declarations
static bool do_command(const string &, Board &board);
static void check_command(const string &, int &);

static void add_pending(const string &cmd) {
    if (doTrace) cout << "# adding to pending list " << cmd << ", list size=" << pending.size() << endl;
    Lock(input_lock);
    pending.push_back(cmd);
    Unlock(input_lock);
}

static void split_cmd(const string &cmd, string &cmd_word, string &cmd_args) {
   size_t space = cmd.find_first_of(' ');
   cmd_word = cmd.substr(0,space);
   cmd_args = cmd.substr(cmd_word.length());
   size_t start = cmd_args.find_first_not_of(' ');
   if (start != string::npos) {
      cmd_args.erase(0,start);
   }
}

static Move text_to_move(const Board &board, const string &input) {
   // Try SAN
    Move m = Notation::value(board,board.sideToMove(),Notation::InputFormat::SAN,input);
   if (!IsNull(m)) return m;
   if (input.length() >= 4 && isalpha(input[0]) && isdigit(input[1]) &&
       isalpha(input[2]) && isdigit(input[3])) {
      // This appears to be "old" coordinate style notation, used in
      // Winboard before 4.2
       return Notation::value(board,board.sideToMove(),Notation::InputFormat::WB,input);
   } else {
      return NullMove;
   }
}

static Move get_move(const string &cmd_word, const string &cmd_args) {
    string move;
    if (cmd_word == "usermove") {
        // new for Winboard 4.2
        move = cmd_args;
    } else {
        move = cmd_word;
    }
    // see if it could be a move
    auto it = move.begin();
    while (it != move.end() && !isalpha(*it)) it++;
    move.erase(move.begin(),it);
    if (doTrace) {
        cout << "# move text = " << move << endl;
    }
    return text_to_move(*main_board,move);
}

// forward declaration
static void processCmdInWaitState(const string &cmd);

static void dispatchCmd(const string &cmd) {
    if (searcher && searcher->isActive()) {
        // a search is in progress, process cmd and see if we should terminate
        int terminate;
        check_command(cmd,terminate);
        if (terminate) {
            searcher->terminateNow();
        }
    } else if (uciWaitState) {
        // We are in the wait state (meaning we had already stopped
        // searching), see if we should exit it now.
        processCmdInWaitState(cmd);
    } else if (uci && cmd == "stop" ) {
        // This is a special case. The GUI wants us to stop. It is possible
        // though for UCI to send this when we are not searching, or not
        // yet. But in any case we need to obey it and send a matching
        // "bestmove". We never want to push it on the pending stack, which
        // is not processed until search completion. So set the stopped flag
        // and the search will detect that it should terminate. (This fixes
        // issues with the "Process testsuite" command in Fritz).
        ASSERT(searcher);
        searcher->stop();
#ifdef UCI_LOG
        ucilog << "got stop" << endl << (flush);
#endif
    }
    else {
        // Do not execute the command within the polling thread.
        // Add it to the pending stack.
        add_pending(cmd);
    }
}

static void processCmdChars(char *buf,int len) {
    // try to parse the buffer into command lines
    for (int i = 0; i < len; i++) {
       char c = buf[i];
       if (c == '\r' || c == '\n') {
           // handle CR + LF if present
           if (i+1 < len && (buf[i+1] == '\r' || buf[i+1] == '\n')) i++;
           if (cmd_buf.length()) {
               dispatchCmd(cmd_buf);
               cmd_buf.clear();
           }
       } else {
           cmd_buf += c;
       }
    }
    Lock(input_lock);
    if (pending.size() && (!searcher || !searcher->isActive())) {
        inputSem.signal();
    }
    Unlock(input_lock);
    // It is possible an unterminated line still exists in the
    // buffer: if so, leave it until an end-of-line is seen.
    if (doTrace && cmd_buf.length()) {
        Lock(input_lock);
        cout << "# " << cmd_buf.length() << " chars left in buffer: " <<
            cmd_buf << endl;
        Unlock(input_lock);
    }
#ifdef UCI_LOG
    if (cmd_buf.length()) {
        ucilog << "unterminated cmd seen: " << cmd_buf << endl;
    }
#endif
}

static int polling_terminated = 0;
static THREAD pollingThreadHandle;

#ifdef _WIN32

static DWORD WINAPI inputPoll(void *x) {
   HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
   if (doTrace) cout << "# starting poll thread" << endl;
   char buf[1024];
   while (!polling_terminated) {
      BOOL bSuccess;
      DWORD dwRead;
      if (_isatty(_fileno(stdin))) {
         // we are reading direct from the console, enable echo & control-char
         // processing
         if (!SetConsoleMode(hStdin, ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT |
                         ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT)) {
            cerr << "SetConsoleMode failed" << endl;
         }
         bSuccess = ReadConsole(hStdin, buf, 1024, &dwRead, NULL);
      }
      else {
         bSuccess = ReadFile(hStdin, buf, 1024, &dwRead, NULL);
         if (! bSuccess || dwRead == 0) {
            if (doTrace) cout << "# read error from input pipe" << endl;
#ifdef UCI_LOG
            ucilog << "read error from input pipe" << endl << (flush);
#endif
            break;
	 }
      }
      processCmdChars(buf,(int)dwRead);
   }
   if (doTrace) cout << "input polling thread terminated" << endl;
#ifdef UCI_LOG
    ucilog << "input polling thread terminated" << endl << (flush);
#endif
   return 0;
}

#else

static void * CDECL inputPoll(void *x) {
   if (doTrace) cout << "# starting poll thread" << endl;
   static const int INPUT_BUF_SIZE = 1024;
   char buf[INPUT_BUF_SIZE];
   while (!polling_terminated) {
      fd_set readfds;
      struct timeval tv;
      int data;

      FD_ZERO(&readfds);
      FD_SET(STDIN_FILENO, &readfds);
      // set a timeout so we can interrupt the polling thread
      // with no input.
      tv.tv_sec=2;
      tv.tv_usec=0;
      int ret = select(16, &readfds, 0, 0, &tv);
      if (ret == 0) {
         // no data available
         continue;
      } else if (ret == -1) {
         perror("select");
         continue;
      }
      data=FD_ISSET(STDIN_FILENO, &readfds);
      if (data>0) {
         // we have something to read
         int bytes = read(STDIN_FILENO,buf,INPUT_BUF_SIZE);
         if (bytes == -1) {
            perror("input poll: read");
            continue;
         }
         if (bytes) {
            processCmdChars(buf,bytes);
         }
      }
   }
   if (doTrace) cout << "input polling thread terminated" << endl;
   return NULL;
}

#endif

static void parseLevel(const string &cmd) {
  float time1, time2, floatincr;
  stringstream ss(cmd);
  istream_iterator<string> it(ss);
  string movesStr, minutesStr, incrStr;
  movesStr = *it++;
  minutesStr = *it++;
  incrStr = *it;
  size_t colon;
  if ((colon=minutesStr.find(":")) != string::npos) {
      minutesStr.replace(colon,1," ");
      stringstream nums(minutesStr);
      istream_iterator<float> it2(nums);
      time1 = *it2++;
      time2 = *it2;
      minutes = time1 + time2/60;
  } else {
      stringstream nums(minutesStr);
      nums >> minutes;
  }
  stringstream movesStream(movesStr);
  movesStream >> moves;
  stringstream incStream(incrStr);
  incStream >> floatincr;
  // Winboard increment is in seconds, convert to our
  // internal value (milliseconds).
  incr = int(1000*floatincr);
}

static void process_st_command(const string &cmd_args)
{
   stringstream s(cmd_args);
   float time_limit_sec;
   // we allow fractional seconds although UI may not support it
   s >> time_limit_sec;
   if (s.bad() || time_limit_sec <= 0.0) {
      cout << "# illegal value for st command: " << cmd_args << endl;
      return;
   } else {
      srctype = FixedTime;
   }
   // convert to ms. and subtract a buffer to prevent losses on time
   time_limit = int(time_limit_sec * 1000 - std::min<int>(int(time_limit_sec*100),100));
}

static int getIncrUCI(const ColorType side) {
    return side == White ? winc : binc;
}

static bool accept_draw(Board &board) {
   if (doTrace)
      cout << "# in accept_draw" << endl;
   // Code to handle draw offers.
   int rating_diff = opponent_rating - computer_rating;
   // ignore draw if we have just started searching
   if (last_score == Constants::MATE) {
      return false;
   }
   // If it's a 0-increment game and the opponent has < 1 minute,
   // and we have more time, decline
   int inc = uci ? getIncrUCI(board.oppositeSide()) : incr;
   if (!computer && inc == 0 && opp_time < 6000 && time_left > opp_time) {
      return false;
   }
   // See if we do not have enough material to mate
   const ColorType side = computer_plays_white ? White : Black;
   const Material &ourmat = board.getMaterial(side);
   const Material &oppmat = board.getMaterial(OppositeColor(side));
   if (ourmat.noPawns() && ourmat.value() <= Params::KING_VALUE + Params::BISHOP_VALUE) {
      // We don't have mating material
      if (doTrace)
         cout << "# no mating material, accept draw" << endl;
      return true;
   }
   // accept a draw in pawnless endings with even material, unless
   // our score is way positive
   if (ourmat.noPawns() && oppmat.noPawns() &&
       ourmat.materialLevel() <= 5 &&
      (std::abs(ourmat.value() - oppmat.value()) < Params::PAWN_VALUE/2) &&
      last_score < Params::PAWN_VALUE) {
      if (doTrace)
         cout << "# pawnless ending, accept draw" << endl;
      return true;
   }
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS)
   const Material &wMat = board.getMaterial(White);
   const Material &bMat = board.getMaterial(Black);
   if(options.search.use_tablebases &&
      wMat.men() + bMat.men() <= EGTBMenCount) {
      if (doTrace)
         cout << "# checking tablebases .." << endl;
      // accept a draw when the tablebases say it's a draw
      score_t tbscore;
#ifdef GAVIOTA_TBS
      if (options.search.tablebase_type == Options::TbType::GaviotaTb) {
          if (GaviotaTb::probe_tb(board,tbscore,0,1) && tbscore == 0) {
              if (doTrace)
                  cout << "# tablebase score says draw" << endl;
              return true;
          }
      }
#endif
#ifdef NALIMOV_TBS
      if (options.search.tablebase_type == Options::TbType::NalimovTb) {
          if (NalimovTb::probe_tb(board,tbscore,0) && tbscore == 0) {
              if (doTrace)
                  cout << "# tablebase score says draw" << endl;
              return true;
          }
      }
#endif
#ifdef SYZYGY_TBS
      if (options.search.tablebase_type == Options::TbType::SyzygyTb) {
         if (SyzygyTb::probe_wdl(board,tbscore,true) && std::abs(tbscore) <= SyzygyTb::CURSED_SCORE) {
              if (doTrace)
                  cout << "# tablebase score says draw" << endl;
              return true;
          }
      }
#endif
   }
#endif
   if (opponent_rating == 0)
      return false;
   // accept a draw if our score is negative .. how much negative
   // depends on opponent rating.
   if (doTrace)
      cout << "# checking draw score .." << endl;
   ColorType tmp = board.sideToMove();
   board.setSideToMove(side);
   score_t draw_score = searcher->root()->drawScore(board);
   board.setSideToMove(tmp);
   const score_t threshold = Params::PAWN_VALUE/4;
   if (doTrace) {
      cout << "# rating_diff = " << rating_diff << endl;
      cout << "# draw_score = " << draw_score << endl;
      cout << "# last_score = " << last_score << endl;
      cout << "# threshold = " << threshold << endl;
   }
   return draw_score-last_score > threshold;
}


static void do_help() {
#ifndef _WIN32
   if (!isatty(1)) return;
#endif
   cout << "analyze:         enter Winboard analyze mode" << endl;
   cout << "black:           set computer to play Black" << endl;
   cout << "bk:              show book moves" << endl;
   cout << "computer:        used to indicate the opponent is a computer" << endl;
   cout << "draw:            offer a draw" << endl;
   cout << "easy:            disable pondering" << endl;
   cout << "edit:            enter Winboard edit mode" << endl;
   cout << "force:           disable computer moving" << endl;
   cout << "go:              start searching" << endl;
   cout << "hard:            enable pondering" << endl;
   cout << "hint:            compute a hint for the current position" << endl;
   cout << "ics <hostname>:  set the name of the ICS host" << endl;
   cout << "level <a b c>:   set the time control:" << endl;
   cout << "  a -> moves to time control" << endl;
   cout << "  b -> minutes per game" << endl;
   cout << "  c -> increment in seconds" << endl;
   cout << "name <string>:   set the name of the opponent" << endl;
   cout << "new:             start a new game" << endl;
   cout << "nopost:          disable output during search" << endl;
   cout << "otim <int>:      set opponent time remaining (in centiseconds)" << endl;
   cout << "post:            show output during search" << endl;
   cout << "quit:            terminate the program" << endl;
   cout << "remove:          back up a full move" << endl;
   cout << "resign:          resign the current game" << endl;
   cout << "result <string>: set the game result (0-1, 1/2-1/2 or 1-0)" << endl;
   cout << "sd <x>:          limit thinking to depth x" << endl;
   cout << "setboard <FEN>:  set board to a specified FEN string" << endl;
   cout << "st <x>:          limit thinking to x seconds" << endl;
   cout << "test <epd_file> <sec/move> <-x iter> <-N pvs> <-v>:  run test suite" << endl;
   cout << "time <int>:      set computer time remaining (in centiseconds)" << endl;
   cout << "undo:            back up a half move" << endl;
   cout << "white:           set computer to play White" << endl;
   cout << "test <file> <-t seconds> <-x # moves> <-v> <-o outfile>: "<< endl;
   cout << "   - run an EPD testsuite" << endl;
   cout << "eval <file>:     evaluate a FEN position." << endl;
   cout << "perft <depth>:   compute perft value for a given depth" << endl;
}


static void save_game() {
   if (uci) return;                               // not supported
   if (doTrace) cout << "# in save_game" << endl;
   if (doTrace) cout << "# game_moves=" << gameMoves->num_moves() << endl;
   if (gameMoves->num_moves() == 0 || !options.store_games) {
      if (doTrace) cout << "# out of save_game" << endl;
      return;
   }
   if (game_file) {
      vector<ChessIO::Header> headers;
      string opening_name, eco;
      if (ecoCoder) {
         if (doTrace) cout << "# calling classify" << endl;
         ecoCoder->classify(*gameMoves,eco,opening_name);
         headers.push_back(ChessIO::Header("ECO",eco));
      }
      static char crating[15];
      static char orating[15];
      sprintf(crating,"%d",computer_rating);
      sprintf(orating,"%d",opponent_rating);

      if (hostname.length() > 0) {
          headers.push_back(ChessIO::Header("Site",hostname));
      }

      if (computer_plays_white) {
         headers.push_back(ChessIO::Header("Black",
            opponent_name.length() > 0 ? opponent_name : "?"));
         if (computer_rating)
            headers.push_back(ChessIO::Header("WhiteElo",crating));
         if (opponent_rating)
            headers.push_back(ChessIO::Header("BlackElo",orating));
      }
      else {
         headers.push_back(ChessIO::Header("White",
            opponent_name.length() > 0 ? opponent_name : "?"));
         if (opponent_rating)
            headers.push_back(ChessIO::Header("WhiteElo",orating));
         if (computer_rating)
            headers.push_back(ChessIO::Header("BlackElo",crating));
      }
      if (start_fen.size()) {
         // we had a non-standard starting position for the game
          headers.push_back(ChessIO::Header("FEN",start_fen));
      }
      stringstream timec;
      timec << minutes*60;
      string timestr = timec.str();
      if (incr) {
         timec << '+' << fixed << setprecision(2) << incr/1000.0F;
         timestr = timec.str();
         size_t per = timestr.find('.');
         if (per != string::npos) {
             int eraseCount = 0;
             // remove trailing zeros and decimal pt if possible:
             for (auto it = timestr.end()-1; it != timestr.begin(); it--) {
                 if (*it == '0' || *it == '.') {
                     ++eraseCount;
                     if (*it == '.') break;
                 }
                 else {
                     break;
                 }
             }
             if (eraseCount) {
                 timestr.erase(timestr.size()-eraseCount,eraseCount);
             }
         }
      } else {
          timestr = timec.str();
      }
      headers.push_back(ChessIO::Header("TimeControl",timestr));
      string result;
      theLog->getResultAsString(result);
      ChessIO::store_pgn(*game_file, *gameMoves,
         computer_plays_white ? White : Black,
         result,
         headers);
   }
   if (doTrace) cout << "# out of save_game" << endl;
}


static int calc_extra_time(const ColorType side) {
   // Determine if we are allowed to use any time beyond
   // the normal time target.
   int inc = (uci ? getIncrUCI(side) : incr);
   if (srctype == FixedTime || srctype == FixedDepth)
      return 0;
   if ((inc == 0 && time_left > time_target*10) ||
      (inc > 0 && time_left > time_target*6))
      return int(time_target*2.5);
   else
      return 0;
}


// Produce a text string representing the move, in a form
// the GUI understands
static void move_image(const Board &board, Move m, ostream &buf, int uci) {
    Notation::image(board,m,uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::WB,buf);
}


static void print_nodes(uint64_t nodes, ostream &out) {
   if (nodes >= 1000000) {
      cout << (float)(nodes)/1000000.0 << "M";
   }
   else if (nodes >= 1000) {
      cout << (float)(nodes)/1000.0 << "K";
   }
   else
      cout << nodes;
}


static void uciOut(int depth, score_t score, time_t time,
uint64_t nodes, uint64_t tb_hits, const string &best_line_image, int multipv) {
   stringstream s;
   s << "info";
   s << " multipv " << (multipv == 0 ? 1 : multipv);
   s << " depth " << depth << " score ";
   Scoring::printScoreUCI(score,s);
   if (stats.failhigh) s << " lowerbound";
   if (stats.faillow) s << " upperbound";
   s << " time " << time << " nodes " << nodes;
   if (time>30) s << " nps " << (long)((1000L*nodes)/time);
   if (tb_hits) {
      s << " tbhits " << tb_hits;
   }
   s << " hashfull " << searcher->hashTable.pctFull();
   if (best_line_image.length()) {
      s << " pv ";
      s << best_line_image;
   }
   cout << s.str() << endl;
   if (doTrace) {
      theLog->write(s.str().c_str()); theLog->write_eol();
   }
#ifdef UCI_LOG
   ucilog << s.str() << endl;
#endif
}


static void uciOut(const Statistics &stats) {
   uciOut(stats.depth,stats.value,stats.elapsed_time,
      stats.num_nodes,stats.tb_hits,
      stats.best_line_image,0);
}


static void CDECL post_output(const Statistics &stats) {
   last_score = stats.value;
   score_t score = stats.display_value;
   if (score == Scoring::INVALID_SCORE) {
      return; // no valid score yet
   }
   if (verbose) {
       if (uci) {
           if (options.search.multipv > 1) {
               // output stats only when multipv array has been filled
               if (stats.multipv_count+1 == stats.multipv_limit) {
                   for (int i = 0; i < stats.multipv_limit; i++) {
                       uciOut(stats.multi_pvs[i].depth,
                              stats.multi_pvs[i].score,
                              stats.multi_pvs[i].time,
                              stats.multi_pvs[i].nodes,
                              stats.multi_pvs[i].tb_hits,
                              stats.multi_pvs[i].best_line_image,
                              i+1);
                   }
               }
           }
           else {
               uciOut(stats);
           }
       }
   }
   else if (post) {
      // "post" output for Winboard
#ifdef _WIN32
      printf("%2u%c %6d %6ld %8I64u %s\n",
#else
      printf("%2u%c %6d %6ld %8llu %s\n",
#endif
         stats.depth,' ',
         int((score*100)/Params::PAWN_VALUE), // score in centipawns
         (long)stats.elapsed_time/10, // time in centiseconds
         (unsigned long long)stats.num_nodes,
         stats.best_line_image.c_str());
         fflush(stdout);
   }
}


static int solution_match(Move result) {
   for (int i = 0; i < std::min<int>(10,solution_move_count); i++) {
      if (MovesEqual(solution_moves[i],result)) {
         return 1;
      }
   }
   return 0;
}


// this function is called with "post" results during a test
// suite run.
static void CDECL post_test(const Statistics &stats)
{
   Move best = stats.best_line[0];
   if (!IsNull(best) && max_depth < 256) {
      int ply = stats.depth-1;
         search_progress[max_depth].move = best;
         search_progress[max_depth].value = stats.value;
         search_progress[max_depth].time = stats.elapsed_time;
         search_progress[max_depth].depth = ply;
         search_progress[max_depth].num_nodes = stats.num_nodes;
         max_depth++;
   }
   int ok;
   if (avoid) {
      // note: doesn't handle multiple "am" moves
      ok = !MovesEqual(solution_moves[0],best);
   }
   else {
      ok = solution_match(best);
   }
   if (ok) {
      if ((int)stats.depth > last_iteration_depth) {
         // Wait 2 sec before counting iterations correct, unless
         // we found a mate
         if (stats.elapsed_time >= 200 ||
             stats.value > Constants::MATE_RANGE) {
            ++iterations_correct;
         }
         last_iteration_depth = stats.depth;
      }
      if (iterations_correct >= early_exit_plies)
         early_exit = 1;
      if (solution_time == -1) {
         solution_time = stats.elapsed_time;
         solution_nodes = stats.num_nodes;
      }
      return;
   }
   else {
      solution_time = -1;   // not solved yet, or has moved off
                            // solution
   }
   iterations_correct = 0;
}


static int CDECL terminate(const Statistics &stats)
{
   post_test(stats);
   return early_exit;
}


// handle editing the chessboard (Winboard mode)
static void edit_board(istream &fin, Board &board)
{
    for (int i = 0; i < 64; i++)
        board.setContents(EmptyPiece,i);
    static char buf[80];
    static const char pieces[]="PNBRQKpnbrqk";
    ColorType side = White;
    while (fin.getline(buf,80)) {
        if (strcmp(buf,"white") == 0)
            side = White;
        else if (strcmp(buf,"black")==0)
            side = Black;
        else if (*buf == '#') {
            for (int i = 0; i < 64; i++)
                board.setContents(EmptyPiece,i);
        }
        else if (strcmp(buf,"c")==0)
            side = OppositeColor(side);
        else if (strcmp(buf,".")==0)
            break;
        else {
            if (strchr(pieces,buf[0])) {
                char c = tolower(buf[0]);
               Piece p = EmptyPiece;
               switch (c) {
               case 'p':
                   p = MakePiece(Pawn,side);
                   break;
               case 'n':
                   p = MakePiece(Knight,side);
                   break;
               case 'b':
                   p = MakePiece(Bishop,side);
                   break;
               case 'r':
                   p = MakePiece(Rook,side);
                   break;
               case 'q':
                   p = MakePiece(Queen,side);
                   break;
               case 'k':
                   p = MakePiece(King,side);
                   break;
               default:
                   break;
                }

                if (p != EmptyPiece) {
                    Square sq = SquareValue(buf+1);
                    if (!IsInvalid(sq))
                        board.setContents(p,sq);
                }
            }
        }
    }
    board.setSecondaryVars();
    // edit doesn't set the castle status, so try to deduce it
    // from the piece positions
    if (board.kingSquare(White) == chess::E1) {
       if (board[chess::A1] == WhiteRook &&
           board[chess::H1] == WhiteRook)
            board.setCastleStatus(CanCastleEitherSide,White);
       else if (board[chess::A1] == WhiteRook)
            board.setCastleStatus(CanCastleQSide,White);
       else if (board[chess::H1] == WhiteRook)
            board.setCastleStatus(CanCastleKSide,White);
    }
    else
        board.setCastleStatus(CantCastleEitherSide,White);
    if (board.kingSquare(Black) == chess::E8) {
       if (board[chess::A8] == BlackRook &&
           board[chess::H8] == BlackRook)
            board.setCastleStatus(CanCastleEitherSide,Black);
       else if (board[chess::A8] == BlackRook)
            board.setCastleStatus(CanCastleQSide,Black);
       else if (board[chess::H8] == BlackRook)
            board.setCastleStatus(CanCastleKSide,Black);
    }
    else
        board.setCastleStatus(CantCastleEitherSide,Black);
}


static void ponder(Board &board, Move move, Move predicted_reply, int uci)
{
    ponder_move_ok = false;
    ponder_move = NullMove;
    ponder_stats.clear();
    if (doTrace) {
       cout << "# in ponder(), move = ";
       MoveImage(move,cout);
       cout << " predicted reply = ";
       MoveImage(predicted_reply,cout);
       cout << endl;
    }
    if (uci || (!IsNull(move) && !IsNull(predicted_reply))) {
        if (!uci) {
            predicted_move = predicted_reply;
            // We have already set up the ponder board with the board
            // position after our last move. Now make the move we predicted.
            //
            BoardState previous_state = ponder_board->state;
            ASSERT(legalMove(*ponder_board,predicted_reply));
            ponder_board->doMove(predicted_reply);
            //
            // We must also add this move to the global move list,
            // otherwise repetition detection will be broken. Note,
            // though, that in case of a ponder miss we must later
            // remove this move.
            //
            gameMoves->add_move(board,previous_state,predicted_reply,"",true);
            // Start a search for the reply
        }
#ifdef UCI_LOG
        ucilog << "starting ponder search" << (flush) << endl;
#endif
        pondering++;
        int time_target = INFINITE_TIME;
        // in reduced strength mode, limit the ponder search time
        // (do not ponder indefinitely)
        if (options.search.strength < 100) {
           time_target = last_time_target;
           if (doTrace) cout << "# limiting ponder time to " <<
                           time_target << endl;
        }
        if (doTrace) {
           cout << "# starting to ponder" << endl;
        }
        if (srctype == FixedDepth) {
           ponder_move = searcher->findBestMove(
              uci ? *main_board : *ponder_board,
              srctype,
              0,
              0,
              ply_limit, true, uci,
              ponder_stats,
              (doTrace) ? Trace : Silent);
        }
        else {
           ponder_move = searcher->findBestMove(
              uci ? *main_board : *ponder_board,
              FixedTime,
              time_target,
              0,            /* extra time allowed */
              Constants::MaxPly,           /* ply limit */
              true,         /* background */
              uci,
              ponder_stats,
              (doTrace) ? Trace : Silent);
        }
        pondering--;
        if (doTrace) {
           cout << "# done pondering" << endl;
        }
        last_computer_move = ponder_move;
        last_computer_stats = ponder_stats;
        // Clean up the global move array, if we got no ponder hit.
        if (!uci && gameMoves->num_moves() > 0 && gameMoves->last().wasPonder()) {
            gameMoves->remove_move();
        }
    }
    if (doTrace) {
        cout << "# ponder move = ";
        MoveImage(ponder_move,cout);
        cout << endl;
        cout << "# out of ponder()" << endl;
    }
#ifdef UCI_LOG
    ucilog << "out of ponder()" << endl << (flush);
#endif
}


// Search using the current board position.
//
static Move search(SearchController *searcher, Board &board, 
                   const vector<Move> &movesToSearch, Statistics &stats, bool infinite)
{
    last_stats.clear();
    last_score = Constants::MATE;
    ponder_move = NullMove;
    if (doTrace) cout << "# in search()" << endl;

    Move move = NullMove;
    stats.fromBook = false;
    // Note: not clear what the semantics should be when "searchmoves"
    // is specified and using "own book." Currently we force a search
    // in this case and ignore the book moves.
    if (!infinite && options.book.book_enabled && !testing && movesToSearch.empty()) {
        move = openingBook.pick(board);
        if (!IsNull(move)) stats.fromBook = true;
    }

    if (IsNull(move)) {
        // no book move
        stats.clear();
        TalkLevel level = Silent;
        if (verbose && !uci) level = Debug;
        else if (doTrace) level = Trace;
        else level = Silent;
        vector<Move> excludes;
        if (srctype == FixedDepth) {
            move = searcher->findBestMove(board,
                srctype,
                0,
                0,
                ply_limit, false, uci,
                stats,
                level,
                excludes,
                movesToSearch);
        }
        else {
            if (infinite) {
                time_target = INFINITE_TIME;
            } else {
                time_target =
                    (srctype == FixedTime) ? time_limit :
                    (uci ? calcTimeLimit(movestogo,
                                         getIncrUCI(board.sideToMove()),
                                         time_left, opp_time, false, doTrace)
                     : calcTimeLimit(moves, minutes, incr, time_left, opp_time, false, doTrace));
                last_time_target = time_target;
            }
            if (doTrace) {
                cout << "# entering search, time_target = " << time_target << endl;
                cout << "# xtra time = " << calc_extra_time(board.sideToMove()) << endl;
            }
            move = searcher->findBestMove(board,
                srctype,
                time_target,
                calc_extra_time(board.sideToMove()),
                Constants::MaxPly, false, uci,
                stats,
                level,
                excludes,
                movesToSearch);
        }
        if (doTrace) {
            cout << "# search done : move = ";
            MoveImage(move,cout);
            cout << endl;
        }
        last_stats = stats;
        if (!forceMode) {
            if (testing) {
                post_test(stats);
            }
            else {
                post_output(stats);
            }
        }
    }
    else {
        if (ics || uci) {
            vector< pair<Move,int> > choices;
            int moveCount = 0;
            if (options.book.book_enabled) {
                moveCount = openingBook.book_moves(board,choices);
            }
            stringstream s;
            if (uci)
                s << "info string book moves: (";
            else if (computer)
                s << "tellics kibitz book moves: (";
            else
                s << "tellics whisper book moves: (";
            for (int i=0;i<moveCount;i++) {
                Notation::image(board,choices[i].first,Notation::OutputFormat::SAN,s);
                s << ' ';
                s << (int)(100.0*choices[i].second/(float)book::MAX_WEIGHT + 0.5F);
                if (i < moveCount-1)
                    s << ", ";
            }
            s << "), choosing ";
            Notation::image(board,move,Notation::OutputFormat::SAN,s);
            cout << s.str() << endl;
        }
        stats.clear();
    }
    last_computer_move = move;
    last_computer_stats = stats;
    return move;
}

// return true if current board position is a draw by the
// rules of chess.
static int isDraw(const Board &board, Statistics &last_stats, string &reason) {
   if (last_stats.state == Stalemate) {
       if (doTrace) cout << "# stalemate" << endl;
       reason = "Stalemate";
       return 1;
   }
   else if (last_stats.value < Constants::MATE-1 &&
            board.state.moveCount >= 100) {
       // Note: do not count as draw if we have checkmated opponent!
       if (doTrace) cout << "# 50 move draw" << endl;
       reason = "50 move draw";
       return 1;
   }
   else if (Scoring::materialDraw(board)) {
       if (doTrace) cout << "# material draw" << endl;
       reason = "Insufficient material";
       return 1;
   }
   else if (Scoring::repetitionDraw(board)) {
       if (doTrace) cout << "# repetition draw" << endl;
       reason = "Repetition";
       return 1;
   }
   else if (stats.state == Draw) {
       return 1;
   }
   return 0;
}

static void send_move(Board &board, Move &move, Statistics
                      &stats) {
    // In case of multi-pv, make sure the high-scoring move is
    // sent as best move.
    if (stats.multipv_limit > 1) {
        move = stats.multi_pvs[0].best;
    }
    last_move = move;
    last_stats = stats;
    if (stats.state == Terminated) {
        // Winboard has already set the game result. We should
        // not send a move or try to set the result.
        return;
    }
    ColorType sideToMove = board.sideToMove();
    if (!uci) {
        string reason;
        if (isDraw(board,last_stats,reason)) {
            // A draw position exists before we even move (probably
            // because the opponent did not claim the draw).
            // Send the result command to claim the draw.
            if (doTrace) {
               cout << "# claiming draw before move";
               if (reason.length()) cout << " (" << reason << ")";
               cout << endl;
            }
            cout << "1/2-1/2 {" << reason << "}" << endl;
            // Wait for Winboard to send a "result" command before
            // actually concluding it's a draw.
            // Set flag to indicate we are waiting.
            result_pending++;
            return;
        }
    }
    if (!IsNull(move)) {
        stringstream img;
        Notation::image(board,last_move,Notation::OutputFormat::SAN,img);
        last_move_image = img.str();
        theLog->add_move(board,last_move,last_move_image,&last_stats,true);
        // Perform learning (if enabled):
        learn(board,board.repCount());
        stringstream movebuf;
        move_image(board,last_move,movebuf,uci);

        BoardState previous_state = board.state;
        board.doMove(last_move);
        gameMoves->add_move(board,previous_state,last_move,last_move_image,false);

        delete ponder_board;
        ponder_board = new Board(board);

        if (uci) {
#ifdef UCI_LOG
            ucilog << "bestmove " << movebuf.str();
#endif
            cout << "bestmove " << movebuf.str();
            if (!easy && !IsNull(stats.best_line[1])) {
                stringstream ponderbuf;
                move_image(board,stats.best_line[1],ponderbuf,uci);
                cout << " ponder " << ponderbuf.str();
#ifdef UCI_LOG
                ucilog << " ponder " << ponderbuf.str();
#endif
            }
            cout << endl << (flush);
#ifdef UCI_LOG
            ucilog << endl << (flush);
#endif
        }
        else { // Winboard
            string reason;
            if (isDraw(board,last_stats,reason)) {
                // It will be a draw after we move (by rule).
                // Following the current protocol standard, send
                // "offer draw" and then send the move (formerly
                // we would send the move then send the result,
                // which is incorrect).
                cout << "offer draw" << endl;
            }
            if (xboard42) {
                cout << "move " << movebuf.str() << endl;
            }
            else {
                cout << gameMoves->num_moves()/2 << ". ... ";
                cout << movebuf.str() << endl;
            }
            cout << (flush);
        }
    }
    else if (uci) {
#ifdef UCI_LOG
        ucilog << "bestmove 0000" << endl;
#endif
        // must always send a "bestmove" command even if no move is available, to
        // acknolwedge the previous "stop" command.
        cout << "bestmove 0000" << endl;
    } else {
        if (doTrace) cout << "# warning : move is null" << endl;
    }
    if (ics && time_target >= 3000 && stats.display_value != Scoring::INVALID_SCORE) {
        if (computer)
            cout << "tellics kibitz ";
        else
            cout << "tellics whisper ";
        std::ios_base::fmtflags original_flags = cout.flags();
        cout << "time=" << fixed << setprecision(2) <<
            (float)last_stats.elapsed_time/1000.0 << " sec. score=";
        Scoring::printScore(last_stats.display_value,cout);
        cout << " depth=" << last_stats.depth;
        if (last_stats.elapsed_time > 0) {
            cout << " nps=";
            last_stats.printNPS(cout);
        }
        if (last_stats.tb_hits) {
            cout << " egtb=" << last_stats.tb_hits << '/' << last_stats.tb_probes;
        }
#if defined(SMP_STATS)
        if (last_stats.samples && options.search.ncpus>1) {
            cout << " cpu=" << fixed << setprecision(2) <<
                (100.0*last_stats.threads)/((float)last_stats.samples) << '%';
        }
#endif
        if (last_stats.best_line_image.length() && !game_end) {
            cout << " pv: " << last_stats.best_line_image;
        }
        cout.flags(original_flags);
        cout << endl;
    }
    if (uci) return; // With UCI, GUI is in charge of game end detection
    // We already checked for draws, check now for other game end
    // conditions.
    if (last_stats.value >= Constants::MATE-1) {
        if (doTrace) cout << "# last_score = mate" << endl;
        if (sideToMove == White) {
            theLog->setResult("1-0");
            cout << "1-0 {White mates}" << endl;
        }
        else {
            theLog->setResult("0-1");
            cout << "0-1 {Black mates}" << endl;
        }
        game_end = 1;
    }
    else if (last_stats.state == Checkmate) {
        if (doTrace) cout << "# state = Checkmate" << endl;
        if (sideToMove == White) {
            theLog->setResult("0-1");
            cout << "0-1 {Black mates}" << endl;
        }
        else {
            theLog->setResult("1-0");
            cout << "1-0 {White mates}" << endl;
        }
        game_end = 1;
    }
    else if (last_stats.state == Resigns) {
        // Don't resign a zero-increment game if the opponent is short
        // on time
        if (!(incr == 0 && opp_time < 2000)) {
            // Winboard passes the resign command to ICS, but ignores it
            // itself.
            if (computer_plays_white) {
                theLog->setResult("0-1 {White resigns}");
                cout << "0-1 {White resigns}" << endl;
            }
            else {
                theLog->setResult("1-0 {Black resigns}");
                cout << "1-0 {Black resigns}" << endl;
            }
            game_end = 1;
        }
        else {   // reset flag - we're not resigning
            stats.end_of_game = 0;
            game_end = 0;
        }
    }
}

static void processCmdInWaitState(const string &cmd) {
    if (doTrace) {
        cout << "# got command in wait state: " << cmd << (flush) << endl;
    }
#ifdef UCI_LOG
    ucilog << "got command in wait state: " << cmd << (flush) << endl;
#endif
    // we expect a "stop" or "ponderhit"
    if (cmd == "ponderhit" || cmd == "stop") {
        send_move(*main_board,last_computer_move,last_computer_stats);
        uciWaitState = 0;
    } else if (cmd == "quit") {
#ifdef UCI_LOG
        ucilog << "received quit" << endl;
#endif
        uciWaitState = 0;
        polling_terminated++;
    } else if (cmd == "ucinewgame") {
        // Arena at least can send this w/o "stop"
#ifdef UCI_LOG
        ucilog << "received ucinewgame" << endl;
#endif
        uciWaitState = 0;
    }
}

// Find best move using the current board position.
//
static Move analyze(SearchController &searcher, Board &board, Statistics &stats)
{
    last_stats.clear();
    last_score = Constants::MATE;

    stats.clear();
    if (doTrace)
        cout << "# entering analysis search" << endl;
    Move move = searcher.findBestMove(board,
        FixedTime,
        INFINITE_TIME,
        0,
        Constants::MaxPly, false, uci,
        stats,
        Whisper);
    if (doTrace) {
        cout << "# search done : move = ";
        MoveImage(move,cout);
        cout << endl;
    }

    last_stats = stats;
    post_output(stats);

    return move;
}


static void doHint() {
    // try book move first
    vector < pair<Move,int> > moves;
    unsigned count = 0;
    if (options.book.book_enabled) {
        count = openingBook.book_moves(*main_board,moves);
    }
    if (count > 0) {
        if (count == 1)
            cout << "Book move: " ;
        else
            cout << "Book moves: ";
        for (unsigned i = 0; i<count; i++) {
            Notation::image(*main_board,moves[i].first,Notation::OutputFormat::SAN,cout);
            if (i<count-1) cout << ' ';
        }
        cout << endl;
        return;
    }
    else {
        // no book move, see if we have a ponder move
        const string &img = last_stats.best_line_image;
        if (img.length()) {
            string::const_iterator it = img.begin();
            while (it != img.end() && !isspace(*it)) it++;
            string last_move;
            if (it != img.end()) {
                it++;
                while (it != img.end() && !isspace(*it)) last_move += *it++;
            }
            if (last_move.length()) {
                cout << "Hint: " << last_move << endl;
                return;
            }
        }
    }
    // no ponder move or book move. If we are already pondering but
    // have no ponder move we could wait a while for a ponder result,
    // but we just return for now.
    if (pondering) return;
    if (doTrace) cout << "# computing hint" << endl;

    Statistics tmp;
    // do low-depth search for hint move
    Move move = searcher->findBestMove(*main_board,
        FixedDepth,
        0,
        0,
        4, false, uci,
        tmp,
        (doTrace) ? Trace : Silent);
    if (!IsNull(move)) {
        cout << "Hint: ";
        Notation::image(*main_board,move,Notation::OutputFormat::SAN,cout);
        cout << endl;
    }
}


//
// Do all pending commands in stack.
// Return 1 if "new" command was executed, 2 if "quit" was seen.
//
static int do_all_pending(Board &board)
{
    int retVal = 0;
    if (doTrace) cout << "# in do_all_pending" << endl;
    while (!pending.empty()) {
        Lock(input_lock);
        string cmd = pending.front();
        pending.pop_front();
        Unlock(input_lock);
        if (doTrace) {
            cout << "# pending command(a): " << cmd << endl;
        }
#ifdef UCI_LOG
        ucilog << "do_all_pending: " << cmd << (flush) << endl;
#endif
        if (cmd.substr(0,3) == "new")
            retVal = 1;
        else if (cmd.substr(0,4) == "quit") {
            retVal = 2;
            break;
        }
        do_command(cmd,board);
    }
    if (doTrace) cout << "# out of do_all_pending, list size=" << pending.size() << endl;
    return retVal;
}


//
// Check for user move, resign or result in pending stack.
// If found, return (1 if user move, 2 if game end command)
// and do not dequeue command. Otherwise return 0.
//
static int check_pending(Board &board) {
    if (doTrace) cout << "# in check_pending" << endl;
    int retVal = 0;
    Lock(input_lock);
    while (!pending.empty()) {
        const string cmd = pending.front();
        string cmd_word, cmd_args;
        split_cmd(cmd,cmd_word,cmd_args);
        if (cmd == "result" ||
            cmd == "new" ||
            cmd == "quit" ||
            cmd == "resign") {
                if (doTrace) cout << "# game end signal in pending stack" << endl;
                retVal = 1;                                // game end
                break;
        }
        else if (cmd_word == "usermove" || text_to_move(board,cmd) != NullMove) {
            if (doTrace) cout << "# move in pending stack" << endl;
            retVal = 2;
            break;
        }
        else {                                      // might as well execute this
#ifdef UCI_LOG
            ucilog << "calling do_command from check_pending" << (flush) << endl;
#endif
            if (doTrace) cout << "# calling do_command from check_pending" << (flush) << endl;
            // dequeue command
            if (doTrace) cout << "# removing " << cmd << endl;
            pending.pop_front();
            Unlock(input_lock);
            // execute command
            do_command(cmd,board);
            Lock(input_lock);
        }
    }
    Unlock(input_lock);
    return retVal;
}

static void analyze_output(const Statistics &stats) {
    // output search status
    cout << "stat01: " <<
        stats.elapsed_time << " " << stats.num_nodes << " " <<
        stats.depth << " " <<
        stats.mvleft << " " << stats.mvtot << endl;
}

static void analyze(Board &board)
{
    if (doTrace) cout << "# entering analysis mode" << endl;
    while (analyzeMode) {
        Board previous(board);
        analyze(*searcher,board,stats);
        if (doTrace) cout << "# analysis mode: out of search" << endl;
        // If we did "quit" while searching, exit analysis mode now
        // Process commands received while searching; exit loop
        // if "quit" seen.
        if (do_all_pending(board)==2) break;
        while (board == previous && analyzeMode) {
            // The user has given us no new position to search. We probably
            // got here because the search has terminated early, due to
            // forced move, forced mate, tablebase hit, or hitting the max
            // ply depth. Wait here for more input.
            if (doTrace) cout << "# analysis mode: wait for input" << endl;
            if (inputSem.wait()) {
#ifdef UCI_LOG
                ucilog << "wait interrupted" << endl << (flush);
#endif
                break;
            }
            while (!pending.empty()) {
                Lock(input_lock);
                string cmd (pending.front());
                pending.pop_front();
                Unlock(input_lock);
                string cmd_word, cmd_arg;
                split_cmd(cmd,cmd_word,cmd_arg);
#ifdef _TRACE
                cout << "# processing cmd in analysis mode: " << cmd << endl;
#endif
                if (cmd == "undo" || cmd == "setboard") {
                    do_command(cmd,board);
                }
                // Technically "quit" is not supposed to be the way
                // to exit analysis mode but we allow it.
                else if (cmd == "exit" || cmd == "quit") {
                    analyzeMode = 0;
                }
                else if (cmd == "bk") {
                    do_command(cmd,board);
                }
                else if (cmd == "hint") {
                    do_command(cmd,board);
                }
                else if (cmd_word == "usermove" || text_to_move(board,cmd) != NullMove) {
                    Move m = get_move(cmd_word, cmd_arg);
                    if (!IsNull(m)) {
                        board.doMove(m);
                    }
                }
                else if (cmd == ".") {
                    analyze_output(stats);
                }
            }
        }
    }
    if (doTrace) cout << "# exiting analysis mode" << endl;
}

static void undo( Board &board)
{
    if (theLog->current() < 1) return;             // ignore "undo"

    board.undoMove((*theLog)[theLog->current()-1].move(),
        (*theLog)[theLog->current()-1].state());
    theLog->back_up();
    gameMoves->remove_move();
    last_stats.clear();
    if (theLog->current()) {
        last_move = (*theLog)[theLog->current()-1].move();
        last_move_image = (*theLog)[theLog->current()-1].image();
    }
    else {
        last_move = NullMove;
        last_move_image.clear();
    }
    // In case we have backed up from the end of the game, reset
    // the "game end" flag.
    game_end = 0;
}


static void setCheckOption(const string &value, int &dest) {
   stringstream buf(value);
   int tmp;
    buf >> tmp;
    if (!buf.bad() && !buf.fail()) {
        dest = tmp != 0;
    }
}

// Execute a move made by the opponent or in "force" mode.
static void execute_move(Board &board,Move m)
{
    if (doTrace) {
        cout << "# execute_move: ";
        MoveImage(m,cout);
        cout << endl;
    }
    last_move = m;
    stringstream img;
    Notation::image(board,m,Notation::OutputFormat::SAN,img);
    last_move_image = img.str();
    theLog->add_move(board,m,last_move_image,NULL,true);
    BoardState previous_state = board.state;
    board.doMove(m);
    // If our last move added was the pondering move, replace it
    if (gameMoves->num_moves() > 0 && gameMoves->last().wasPonder()) {
        gameMoves->remove_move();
    }
    gameMoves->add_move(board,previous_state,m,last_move_image,false);
}

#ifdef TUNE
static void setTuningParam(const string &name, const string &value)
{
   // set named parameters that are in the tuning set
   int index = tune_params.findParamByName(name);
   if (index > 0) {
      stringstream buf(value);
      int tmp;
      buf >> tmp;
      if (!buf.bad() && !buf.fail()) {
         tune_params.updateParamValue(index,tmp);
         // apply params to Scoring module
         tune_params.applyParams();
      }
      else {
         if (uci) cout << "info ";
         else cout << "#";
         cout << "Warning: invalid value for option " <<
            name << ": " << value << endl;
         return;
      }
   }
   else {
      if (uci) cout << "info ";
      else cout << "#";
      cout << "Warning: invalid option name \"" <<
            name << "\"" << endl;
   }
}
#endif

static void processWinboardOptions(const string &args) {
    string name, value;
    size_t eq = args.find("=");
    if (eq == string::npos) {
        // no value
        name = args;
    } else {
        name = args.substr(0,eq);
        value = args.substr(eq+1);
    }
    // trim spaces
    name = name.erase(0 , name.find_first_not_of(' ') );
    name = name.erase( name.find_last_not_of(' ') + 1);
    value = value.erase(0, value.find_first_not_of(' '));
    value = value.erase(value.find_last_not_of(' ') + 1);
    // handle option settings
    if (doTrace) {
        cout << "# setting option " << name << "=" << value << endl;
    }
    if (name == "Book selectivity") {
        Options::setOption<int>(value,options.book.selectivity);
    } else if (name == "Can resign") {
        setCheckOption(value,options.search.can_resign);
    } else if (name == "Position learning") {
        setCheckOption(value,options.learning.position_learning);
    } else if (name == "Strength") {
        Options::setOption<int>(value,options.search.strength);
#ifdef NUMA
    } else if (name == "Set processor affinity") {
       setCheckOption(value,options.search.set_processor_affinity);
       int tmp = options.search.set_processor_affinity;
       if (tmp != options.search.set_processor_affinity) {
          if (options.search.set_processor_affinity) {
             searcher->rebind();
          } else {
             searcher->unbind();
          }
       }
    } else if (name == "Affinity offset") {
       int tmp = options.search.affinity_offset;
       Options::setOption<int>(value,options.search.affinity_offset);
       if (tmp != options.search.affinity_offset) {
          searcher->rebind();
       }
#endif
    }
#ifdef TUNE
    else {
       setTuningParam(name,value);
    }
#else
    else {
       cout << "# Warning: invalid option name \"" << name << "\"" << endl;
   }
#endif
   searcher->updateSearchOptions();
}

// Handle a command received while searching.  terminate is set
// true if the search should stop.  Some commands are executed,
// some are placed on the pending stack to be executed after
// the search completes.
//
static void check_command(const string &cmd, int &terminate)
{
    if (doTrace)
        cout << "# check_command: " << cmd << endl;
    terminate = 0;
    string cmd_word, cmd_args;
    // extract first word of command:
    split_cmd(cmd,cmd_word,cmd_args);
    if (uci) {
        if (doTrace) {
            theLog->write("check_command: ");
            theLog->write(cmd.c_str());
            theLog->write_eol();
        }
#ifdef UCI_LOG
        ucilog << "gui: in check_command: " << cmd << (flush) << endl;
#endif
        if (cmd == "quit") {
            add_pending(cmd);
            terminate++;
            return;
        }
        else if (cmd == "ponderhit") {
            // We predicted the opponent's move, so we need to
            // continue doing the ponder search but adjust the time
            // limit.
            ++ponderhit;
            ponder_move_ok = true;
            // continue the search in non-ponder mode
            if (srctype != FixedDepth) {
                // Compute how much longer we must search
                ColorType side = searcher->getComputerSide();
                time_target =
                    (srctype == FixedTime) ? time_limit :
                    calcTimeLimit(movestogo,
                                  side == White ? winc : binc,
                                  time_left, opp_time, !easy, doTrace);
                if (doTrace) {
                    stringstream s;
                    s << "time_left=" << time_left << " opp_time=" << opp_time << " time_target=" <<
                        time_target << '\0';
                    theLog->write(s.str().c_str()); theLog->write_eol();
                    cout << "# time_target = " << time_target << endl;
                }
                searcher->setTimeLimit(time_target,calc_extra_time(side));
            }
            searcher->setTalkLevel(Whisper);
            searcher->setBackground(false);
            pondering = false;
            // Since we have shifted to foreground mode, show the current
            // search statistics:
            post_output(ponder_stats);
        }
        else if ((cmd_word == "position" ||
                  cmd == "ucinewgame")) {
            terminate = 1; // stop this search in order to start a new one
            add_pending(cmd);
        } else {
            // Most other commands do not terminate the search. Execute them
            // now. (technically, according the UCI spec, setoption is not
            // allowed during search: but UIs such as ChessBase assume it is).
            Board &board = pondering ? *ponder_board : *main_board;
            if (!do_command(cmd,board)) {
                terminate = 1; // search is terminating
            }
        }
        return;
    }
    else if (analyzeMode) {
        if (cmd == "undo" || cmd == "setboard") {
            add_pending(cmd);
            terminate = true;
        }
        else if (cmd == "exit") {
            analyzeMode = 0;
            terminate = true;
        }
        else if (cmd == "bk") {
            do_command(cmd, *main_board);
        }
        else if (cmd == "hint") {
            //do_command(cmd);
        }
        else if (cmd == ".") {
            analyze_output(stats);
        }
        else if (cmd_word == "usermove" || text_to_move(*main_board,cmd) != NullMove) {
            add_pending(cmd);
            terminate = true;
        }
        // all other commands are ignored
        return;
    }
    else if (cmd == "quit" || cmd == "end" || cmd_word == "test") {
        add_pending(cmd);
        terminate = 1;
    }
    else if (cmd == "new") {
        add_pending(cmd);
        terminate = 1;
    }
    else if (cmd == "random" || cmd_word == "ics") {
        // ignore
    }
    else if (cmd == "computer") {
        computer = 1;
    }
    else if (cmd == "?") {
        // Winboard 3.6 or higher sends this to terminate a search
        // in progress
        if (doTrace) cout << "# terminating." << endl;
        terminate = true;
    }
    else if (cmd_word == "ping") {
        // new for Winboard 4.2
        // do not respond until the search completes.
        add_pending(cmd);
    }
    else if (cmd == "hint") {
        doHint();
    }
    else if (cmd == "depth") {
    }
    else if (cmd_word == "level") {
        parseLevel(cmd_args);
        srctype = TimeLimit;
    }
    else if (cmd_word == "st") {
       // Note: Winboard does not send this during a search but
       // it is possible other interaces might.
       process_st_command(cmd_args);
    }
    else if (cmd_word == "sd") {
        stringstream s(cmd_args);
        s >> ply_limit;
        srctype = FixedDepth;
    }
    else if (cmd_word == "time") {
        // my time left in centiseconds
        int t;
        stringstream s(cmd_args);
        s >> t;
        time_left = t*10; // convert to ms
    }
    else if (cmd_word == "otim") {
        // opponent's time left
        int t;
        stringstream s(cmd_args);
        s >> t;
        opp_time = t*10; // convert to ms
    }
    else if (cmd == "post") {
        post = 1;
    }
    else if (cmd == "nopost") {
        post = 0;
    }
    else if (cmd == "savegame") {
    }
    else if (cmd == "remove"  || cmd == "undo") {
        add_pending(cmd);
        terminate = true;
    }
    else if (cmd == "resign" || cmd_word == "result") {
        add_pending(cmd);
        game_end = 1;
        if (doTrace) {
            cout << "# received_result: " << cmd << endl;
        }
        terminate = 1;
        // set the state to Terminated - this is a signal that
        // regardless of the search result, we should not send
        // a move, because the server has terminated the game.
        if (pondering) {
            ponder_stats.state = Terminated;
        } else {
            stats.state = Terminated;
        }
    }
    else if (cmd == "draw") {
        // "draw" command. Requires winboard 3.6 or higher.
        if (accept_draw(*main_board)) {
            // Notify opponent. don't assume draw is concluded yet.
            cout << "offer draw" << endl;
        }
        else if (doTrace) {
            cout << "# draw declined" << endl;
        }
    }
    else if (cmd == "edit" || cmd_word == "setboard" || cmd == "analyze" ||
             cmd == "go" || cmd == "exit" || cmd == "white" ||
             cmd == "black" || cmd == "playother") {
        add_pending(cmd);
        terminate = true;
    }
    else if (cmd == "bk") {
        // not supported while searching
    }
    else if (cmd == "easy") {
        easy = true;
    }
    else if (cmd == "hard") {
        easy = false;
    }
    else if (cmd_word == "name") {
        // We've received the name of our opponent.
        opponent_name = cmd_args;
    }
    else if (cmd == "bogus" || cmd == "accepted") {
    }
    else if (cmd == "force") {
        forceMode = 1;
        terminate = true;
    }
    else if (cmd_word == "rating") {
        stringstream args(cmd_args);
        args >> computer_rating;
        args >> opponent_rating;
        if (searcher) searcher->setRatingDiff(opponent_rating-computer_rating);
    }
    else if (cmd_word == "option") {
        processWinboardOptions(cmd_args);
    }
    else if (cmd_word == "cores" || cmd_word == "memory" || cmd_word == "egtpath") {
        // defer until search completes
        add_pending(cmd);
    }
    else {
        Move rmove = get_move(cmd_word, cmd_args);
        if (!IsNull(rmove)) {
            last_move = rmove;
            if (doTrace) {
                cout << "# predicted move = ";
                MoveImage(predicted_move,cout);
                cout << " last move = ";
                MoveImage(last_move,cout);
                cout << endl;
            }
            if (forceMode || analyzeMode || !pondering) {
                add_pending(cmd);
                terminate = true;
            }
            else if (!IsNull(predicted_move) &&
                MovesEqual(predicted_move,last_move)) {
                    // ponder hit
                    if (doTrace) {
                        cout << "# ponder ok" << endl;
                    }
                    execute_move(*main_board,last_move);
                    // We predicted the opponent's move, so we need to
                    // continue doing the ponder search but adjust the time
                    // limit.
                    ponder_move_ok = true;
                    if (srctype != FixedDepth) {
                        // Compute how much longer we must search

                        ColorType side = searcher->getComputerSide();
                        time_target =
                            (srctype == FixedTime) ? time_limit :
                            (uci ? calcTimeLimit(movestogo,
                                                 getIncrUCI(side),
                                                 time_left, opp_time,
                                                 true, doTrace)
                             : calcTimeLimit(moves, minutes, incr, time_left, opp_time, true, doTrace));
                        if (doTrace) {
                            cout << "# time_target = " << time_target << endl;
                            cout << "# xtra time = " << calc_extra_time(side) << endl;
                        }
                        searcher->setTimeLimit(time_target,calc_extra_time(side));
                    }
                    searcher->setTalkLevel(Whisper);
                    searcher->setBackground(false);
                    post_output(ponder_stats);
                    terminate = false;
            }
            else {
                if (doTrace) cout << "# ponder not ok" << endl;
                // We can't use the results of pondering because we
                // did not predict the opponent's move.  Stop the
                // search and then execute the move.
                ponder_move_ok = false;
                add_pending(cmd);
                terminate = true;
            }
        }
        else if (doTrace) {
            cout << "# warning: move string not parsed" << endl;
        }
    }
}


// for support of the "test" command
static Move test_search(Board &board, int ply_limit,
int time_limit, Statistics &stats,
const vector<Move> &excludes) {
   Move move = NullMove;
   solution_time = -1;

   vector<Move> includes;
   move = searcher->findBestMove(board,
      srctype,
      time_limit, 0, ply_limit,
      0, 0, stats,
      verbose ? Debug : Silent,
      excludes, includes);

   if (excludes.size())
      cout << "result(" << excludes.size()+1 << "):";
   else
      cout << "result:";
   cout << '\t';
   Notation::image(board,move,Notation::OutputFormat::SAN,cout);
   cout << "\tscore: ";
   Scoring::printScore(stats.display_value,cout);
   cout <<  '\t';
   total_time += stats.elapsed_time;
   total_nodes += stats.num_nodes;
   gameMoves->removeAll();
   return move;
}


static void do_test(string test_file)
{
   delayedInit();
   int tmp = options.book.book_enabled;
   options.book.book_enabled = 0;
   total_nodes = (uint64_t)0;
   total_correct = total_tests = 0;
   total_time = (uint64_t)0;
   nodes_to_find_total = (uint64_t)0;
   depth_to_find_total = 0;
   time_to_find_total = (uint64_t)0;

   solution_times.clear();
   Board board;
   ifstream pos_file( test_file.c_str(), ios::in);
   if (!pos_file) {
      cout << "Failed to open EPD file." << endl;
         return;
   }
   string buf;
   while (!pos_file.eof()) {
      std::getline(pos_file,buf);
      if (!pos_file) {
         cout << "Error reading EPD file." << endl;
         return;
      }
      // Try to parse this line as an EPD command.
      stringstream stream(buf);
      string id, comment;
      EPDRecord epd_rec;
      if (!ChessIO::readEPDRecord(stream,board,epd_rec)) break;
      if (epd_rec.hasError()) {
         cerr << "error in EPD record ";
         if (id.length()>0) cerr << id;
         cerr << ": ";
         cerr << epd_rec.getError();
         cerr << endl;
      }
      else {
         solution_move_count = 0;
         int illegal=0;
         id = "";
         for (unsigned i = 0; i < epd_rec.getSize(); i++) {
            string key, val;
            epd_rec.getData(i,key,val);
            if (key == "bm" || key == "am") {
               Move m;
               stringstream s(val);
               while (!s.eof()) {
                  string moveStr;
                  // skips spaces
                  s >> moveStr;
                  if (s.bad() || s.fail() || !moveStr.length()) {
                      cerr << "error reading solution move " << val << endl;
                      break;
                  }
                  m = Notation::value(board,board.sideToMove(),Notation::InputFormat::SAN,moveStr);
                  if (IsNull(m)) {
                     ++illegal;
                  }
                  else if (solution_move_count < 10) {
                     solution_moves[solution_move_count++] = m;
                  }
                  avoid = (key == "am");
               }
            }
            else if (key == "id") {
               id = val;
            }
            else if (key == "c0") {
               comment = val;
            }
         }
         if (illegal) {
            cerr << "illegal or invalid solution move(s) for EPD record ";
            if (id.length()>0) cerr << id;
            cerr << endl;
            continue;
         }
         else if (!solution_move_count) {
            cerr << "no solution move(s) for EPD record ";
            if (id.length()>0) cerr << id;
            cerr << endl;
            continue;
         }
         last_iteration_depth = -1;
         iterations_correct = 0;
         early_exit = 0;
         solution_time = -1;
         max_depth = 0;
         testing = 1;
         cout << id << ' ';
         if (comment.length()) cout << comment << ' ';
         if (avoid) {
            cout << "am ";
            Notation::image(board,solution_moves[0],Notation::OutputFormat::SAN,cout);
            cout << endl;
         }
         else {
            cout << "bm";
            for (int i = 0; i < solution_move_count; i++) {
               cout << ' ';
               Notation::image(board,solution_moves[i],Notation::OutputFormat::SAN,cout);
            }
            cout << endl;
         }
         srctype = FixedTime;
         vector <Move> excludes;
         total_tests++;
         for (int index = 0; index < moves_to_search; index++) {
            searcher->clearHashTables();
            Move result = test_search(board,ply_limit,time_limit,stats,excludes);
            if (IsNull(result)) break;
            excludes.push_back(result);
            int correct = solution_time >=0;
            if (index == 0) {
                // only put solutions in summary at end if they are
                // made on the first search attempt.
                solution_times.push_back((int)solution_time);
                if (correct) total_correct++;
            }
            std::ios_base::fmtflags original_flags = cout.flags();
            cout << setprecision(4);
            if (correct) {
               cout << "\t++ solved in " << (float)solution_time/1000.0 <<
                  " sec. (";
               print_nodes(solution_nodes,cout);
            }
            else {
               cout << "\t** not solved in " <<
                  (float)stats.elapsed_time/1000.0 << " secs. (";
               print_nodes(stats.num_nodes,cout);
            }
            cout << " nodes)" << endl;
            cout.flags(original_flags);
            cout << stats.best_line_image << endl;
            if (index == 0 && correct && max_depth>0) {
               uint64_t nodes_to_find = (uint64_t)0;
               int depth_to_find = 0;
               time_t time_to_find = 0;
               for (int i=max_depth-1;i>=0;i--) {
                  if (avoid) {
                     if (MovesEqual(search_progress[i].move,result)) {
                        nodes_to_find = search_progress[i+1].num_nodes;
                        time_to_find = search_progress[i+1].time;
                        depth_to_find = search_progress[i+1].depth;
                        break;
                     }
                  }
                  else {
                     if (!solution_match(search_progress[i].move)) {
                        nodes_to_find = search_progress[i+1].num_nodes;
                        time_to_find = search_progress[i+1].time;
                        depth_to_find = search_progress[i+1].depth;
                        break;
                     }
                  }
               }
               nodes_to_find_total += nodes_to_find;
               depth_to_find_total += depth_to_find;
               time_to_find_total += time_to_find;
            }
         }
      }

      int c;
      while (!pos_file.eof()) {
         c = pos_file.get();
         if (!isspace(c) && c != '\n') {
            if (!pos_file.eof()) {
               pos_file.putback(c);
            }
            break;
         }
      }
   }
   pos_file.close();
   cout << endl << "solution times:" << endl;
   cout << "         ";
   unsigned i = 0;
   for (i = 0; i < 10; i++)
      cout << i << "      ";
   cout << endl;
   double score = 0.0;
   for (i = 0; i < solution_times.size(); i++) {
      char digits[15];
      if (i == 0) {
         sprintf(digits,"% 4d |       ",i);
         cout << endl << digits;
      }
      else if ((i+1) % 10 == 0) {
         sprintf(digits,"% 4d |",(i+1)/10);
         cout << endl << digits;
      }
      if (solution_times[i] == -1) {
         cout << "  ***  ";
      }
      else {
         sprintf(digits,"%6.2f ",solution_times[i]/1000.0);
         cout << digits;
         score += (float)time_limit/1000.0 - solution_times[i]/1000.0;
      }
   }
   cout << endl << endl << "correct : " << total_correct << '/' <<
   total_tests << endl;
   if (total_correct) {
      string avg = "";
      if (total_correct > 1) avg = "avg. ";
      cout << avg << "nodes to solution : ";
      uint64_t avg_nodes = nodes_to_find_total/total_correct;
      if (avg_nodes > 1000000L) {
         cout << (float)(avg_nodes)/1000000.0 << "M" << endl;
      }
      else {
         cout << (float)(avg_nodes)/1000.0 << "K" << endl;
      }
      cout << avg << "depth to solution : " << (float)(depth_to_find_total)/total_correct << endl;
      cout << avg << "time to solution  : " << (float)(time_to_find_total)/(1000.0*total_correct) << " sec." << endl;
   }
   options.book.book_enabled = tmp;
   testing = 0;
}

static uint64_t perft(Board &board, int depth) {
   if (depth == 0) return 1;

   uint64_t nodes = 0ULL;
   RootMoveGenerator mg(board);
   Move m;
   BoardState state = board.state;
   int order = 0;
   while ((m = mg.nextMove(order)) != NullMove) {
      if (depth > 1) {
         board.doMove(m);
         nodes += perft(board,depth-1);
         board.undoMove(m,state);
      } else {
         // skip do/undo
         nodes++;
      }
   }
   return nodes;
}

static void loadgame(Board &board,ifstream &file) {
    vector<ChessIO::Header> hdrs(20);
    long first;
    ChessIO::collect_headers(file,hdrs,first);
    ColorType side = White;
    for (;;) {
        static string num;
        ChessIO::Token tok = ChessIO::get_next_token(file);
        if (tok.type == ChessIO::Eof)
            break;
        else if (tok.type == ChessIO::Number) {
            num = tok.val;
        }
        else if (tok.type == ChessIO::GameMove) {
            // parse the move
            Move m = Notation::value(board,side,Notation::InputFormat::SAN,tok.val);
            if (IsNull(m) ||
                !legalMove(board,StartSquare(m),
                           DestSquare(m))) {
                cerr << "Illegal move" << endl;
                break;
            }
            else {
                BoardState previous_state = board.state;
                string image;
                // Don't use the current move string as the input
                // parser is forgiving and will accept incorrect
                // SAN. Convert it here to the correct form:
                Notation::image(board,m,Notation::OutputFormat::SAN,image);
                gameMoves->add_move(board,previous_state,m,image.c_str(),false);
                board.doMove(m);
            }
            side = OppositeColor(side);
        }
        else if (tok.type == ChessIO::Result) {
            break;
        }
    }
}


#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS) || defined(SYZYGY_TBS)
static bool validTbPath(const string &path) {
   // Shredder at least sets path to "<empty>" for tb types that are disabled
   return path != "" && path != "<empty>";
}
#endif

static bool uciOptionCompare(const string &a, const string &b) {
   if (a.length() != b.length()) {
      return false;
   } else {
      for (unsigned i = 0; i < a.length(); i++) {
         if (tolower(a[i]) != tolower(b[i])) return false;
      }
      return true;
   }
}

// Execute a command, return false if program should terminate.
static bool do_command(const string &cmd, Board &board) {
#ifdef UCI_LOG
    ucilog << "gui: " << cmd << (flush) << endl;
#endif
    if (doTrace) {
        cout << "# do_command: " << cmd << endl;
    }
    if (doTrace && uci) {
        theLog->write(cmd.c_str()); theLog->write_eol();
    }
    string cmd_word, cmd_args;
    split_cmd(cmd, cmd_word, cmd_args);
    if (cmd == "uci") {
        uci = 1;
        verbose = 1;                       // TBD: fixed for now
        // Learning is disabled because we don't have full game history w/ scores
        options.learning.position_learning = 0;
        cout << "id name " << "Arasan " << Arasan_Version;
        cout << endl;
        cout << "id author Jon Dart" << endl;
        cout << "option name Hash type spin default " <<
            options.search.hash_table_size/(1024L*1024L) << " min 4 max " <<
#ifdef _64BIT
            "64000" << endl;
#else
            "2000" << endl;
#endif
        cout << "option name Ponder type check default true" << endl;
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS) || defined(SYZYGY_TBS)
        cout << "option name Use tablebases type check default ";
        if (options.search.use_tablebases) cout << "true"; else cout << "false";
        cout << endl;
        cout << "option name Tablebases type combo var None";
#ifdef SYZYGY_TBS
        cout << " var " << Options::SYZYGY_TYPE;
#endif
#ifdef NALIMOV_TBS
        cout << " var " << Options::NALIMOV_TYPE;
#endif
#ifdef GAVIOTA_TBS
        cout << " var " << Options::GAVIOTA_TYPE;
#endif
        cout << " default " << Options::tbTypeToString(options.search.tablebase_type);
        cout << endl;
#ifdef GAVIOTA_TBS
        cout << "option name GaviotaTbPath type string default " <<
            options.search.gtb_path << endl;
        cout << "option name GaviotaTbCache type spin default " <<
            options.search.gtb_cache_size/(1024*1024) <<
            " min 1 max 32" << endl;
#endif
#ifdef NALIMOV_TBS
        cout << "option name NalimovPath type string default " <<
            options.search.nalimov_path << endl;
        cout << "option name NalimovCache type spin default " <<
            options.search.nalimov_cache_size/(1024*1024) <<
            " min 1 max 32" << endl;
#endif
#ifdef SYZYGY_TBS
        cout << "option name SyzygyTbPath type string default " <<
            options.search.syzygy_path << endl;
        cout << "option name SyzygyUse50MoveRule type check default true" << endl;
        cout << "option name SyzygyProbeDepth type spin default " <<
            options.search.syzygy_probe_depth <<
           " min 0 max 64" << endl;
#endif
#endif
        cout << "option name MultiPV type spin default 1 min 1 max " << Statistics::MAX_PV << endl;
        cout << "option name OwnBook type check default true" << endl;
        cout << "option name Book selectivity type spin default " <<
            options.book.selectivity << " min 0 max 100" << endl;
        cout << "option name Threads type spin default " <<
            options.search.ncpus << " min 1 max " <<
            Constants::MaxCPUs << endl;
        cout << "option name UCI_LimitStrength type check default false" << endl;
        cout << "option name UCI_Elo type spin default " <<
            1000+options.search.strength*16 << " min 1000 max 2600" << endl;
#ifdef NUMA
        cout << "option name Set processor affinity type check default " <<
           (options.search.set_processor_affinity ? "true" : "false") << endl;
        cout << "option name Affinity offset type spin default " <<
           options.search.affinity_offset << " min 0 max " <<
           Constants::MaxCPUs << endl;
#endif
        cout << "uciok" << endl;
        return true;
    }
    else if (cmd == "quit") {
        return false;
    }
    else if (uci && cmd_word == "setoption") {
        string name, value;
        size_t nam = cmd_args.find("name");
        if (nam != string::npos) {
            // search for "value"
            size_t val = cmd_args.find(" value",nam+4);
            if (val != string::npos) {
               name = cmd_args.substr(nam+4,val-nam-4);
               // trim spaces
               name = name.erase(0 , name.find_first_not_of(' ') );
               name = name.erase( name.find_last_not_of(' ') + 1);
               value = cmd_args.substr(val+6);
               value = value.erase(0, value.find_first_not_of(' '));
               value = value.erase(value.find_last_not_of(' ') + 1);
            }
        }
        if (uciOptionCompare(name,"Hash")) {
            if (!memorySet) {
                size_t old = options.search.hash_table_size;
                // size is in megabytes
                stringstream buf(value);
                int size;
                buf >> size;
                if (buf.bad()) {
                    cout << "info problem setting hash size to " << buf.str() << endl;
                }
                else {
                    options.search.hash_table_size = (size_t)size*1024L*1024L;
                    if (old != options.search.hash_table_size) {
                       searcher->resizeHash(options.search.hash_table_size);
                    }
                }
            }
        }
        else if (uciOptionCompare(name,"Ponder")) {
            easy = !(value == "true");
        }
#if defined(NALIMOV_TBS) || defined(GAVIOTA_TBS) || defined(SYZYGY_TBS)
        else if (uciOptionCompare(name,"Use tablebases")) {
            options.search.use_tablebases = (value == "true");
        }
        else if (uciOptionCompare(name,"Tablebases")) {
            Options::TbType previous = options.search.tablebase_type;
            options.search.tablebase_type = Options::stringToTbType(value);
            if (previous != options.search.tablebase_type) {
                // Type has changed
                unloadTb(previous);
            }
        }
#endif
#ifdef NALIMOV_TBS
        else if (uciOptionCompare(name,"NalimovPath") && validTbPath(value)) {
            unloadTb(options.search.tablebase_type);
            options.search.nalimov_path = value;
            options.search.use_tablebases = 1;
            options.search.tablebase_type = Options::TbType::NalimovTb;
        }
        else if (uciOptionCompare(name,"NalimovCache")) {
            // This is in MB
            int size = 0;
            if (Options::setOption<int>(value,size)) {
                options.search.nalimov_cache_size = std::min<int>(32,size)*1024L*1024L;
            }
        }
#endif
#ifdef GAVIOTA_TBS
        else if (name == "GaviotaTbPath" && validTbPath(value)) {
            unloadTb(options.search.tablebase_type);
            options.search.gtb_path = value;
            options.search.use_tablebases = 1;
            options.search.tablebase_type = Options::TbType::GaviotaTb;
        }
        else if (uciOptionCompare(name,"GaviotaTbCache")) {
            // This is in MB
            int size;
            if (Options::setOption<int>(value,size)) {
                options.search.gtb_cache_size = std::min<int>(32,size)*1024L*1024L;
            }
        }
#endif
#ifdef SYZYGY_TBS
        else if (uciOptionCompare(name,"SyzygyTbPath") && validTbPath(value)) {
           unloadTb(options.search.tablebase_type);
           options.search.syzygy_path = value;
           options.search.use_tablebases = 1;
           options.search.tablebase_type = Options::TbType::SyzygyTb;
        }
        else if (uciOptionCompare(name,"SyzygyUse50MoveRule")) {
           options.search.syzygy_50_move_rule = (value == "true");
        }
        else if (uciOptionCompare(name,"SyzygyProbeDepth")) {
           Options::setOption<int>(value,options.search.syzygy_probe_depth);
        }
#endif
        else if (uciOptionCompare(name,"OwnBook")) {
            options.book.book_enabled = (value == "true");
        }
        else if (uciOptionCompare(name,"Book selectivity")) {
            Options::setOption<int>(value,options.book.selectivity);
        }
        else if (uciOptionCompare(name,"MultiPV")) {
            Options::setOption<int>(value,options.search.multipv);
            options.search.multipv = std::min<int>(Statistics::MAX_PV,options.search.multipv);
            // GUIs (Shredder at least) send 0 to turn multi-pv off: but
            // our option counts the # of lines to show, so we set it to
            // 1 in this case.
            if (options.search.multipv == 0) options.search.multipv = 1;
            stats.multipv_count = 0;
            // migrate current stats to 1st Multi-PV table entry:
            stats.multi_pvs[0] =
              Statistics::MultiPVEntry(stats);
        }
        else if (uciOptionCompare(name,"Threads")) {
            int threads = options.search.ncpus;
            if (Options::setOption<int>(value,threads) && threads >0 && threads <= Constants::MaxCPUs) {
                options.search.ncpus = threads;
                searcher->setThreadCount(options.search.ncpus);
            }
        }
        else if (uciOptionCompare(name,"UCI_LimitStrength")) {
            uciStrengthOpts.limitStrength = (value == "true");
            if (uciStrengthOpts.limitStrength) {
               options.setRating(uciStrengthOpts.eco);
            } else {
               // reset to full strength
               options.setRating(2600);
            }
        } else if (uciOptionCompare(name,"UCI_Elo")) {
            int rating;
            if (Options::setOption<int>(value,rating)) {
               uciStrengthOpts.eco = rating;
               if (uciStrengthOpts.limitStrength) {
                  options.setRating(rating);
               }
            }
	}
#ifdef NUMA
        else if (uciOptionCompare(name,"Set processor affinity")) {
           int tmp = options.search.set_processor_affinity;
           options.search.set_processor_affinity = (value == "true");
           if (tmp != options.search.set_processor_affinity) {
              if (options.search.set_processor_affinity) {
                 searcher->rebind();
              } else {
                 searcher->unbind();
              }
           }
        }
        else if (uciOptionCompare(name,"Affinity offset")) {
           int tmp = options.search.affinity_offset;
           Options::setOption<int>(value,options.search.affinity_offset);
           if (tmp != options.search.affinity_offset) {
              searcher->rebind();
           }
        }
#endif
#ifdef TUNE
        else {
           setTuningParam(name,value);
        }
#else
        else {
           cout << "info error: invalid option name \"" << name << "\"" << endl;
        }
#endif
        searcher->updateSearchOptions();
    }
    else if (uci && cmd == "ucinewgame") {
        do_command("new",board);
        return true;
    }
    else if (uci && cmd == "isready") {
        delayedInit();
        cout << "readyok" << endl;
#ifdef UCI_LOG
        ucilog << "readyok" << endl;
#endif
    }
    else if (uci && cmd_word == "position") {
        ponder_move = NullMove;
        if (cmd_args.substr(0,8) == "startpos") {
            board.reset();
            gameMoves->removeAll();
        }
        else if (cmd_args.substr(0,3) == "fen") {
            string fen;
            int valid = 0;
            if (cmd_args.length() > 3) {
                fen = cmd_args.substr(3);
                valid = BoardIO::readFEN(board, fen);
            }
            if (!valid) {
                if (doTrace) cout << "# warning: invalid fen!" << endl;
#ifdef UCI_LOG
                ucilog << "warning: invalid FEN!" << endl;
#endif
            }
            // clear some global vars
            stats.clear();
            ponder_stats.clear();
            last_stats.clear();
            last_move = NullMove;
            last_move_image.clear();
            gameMoves->removeAll();
            predicted_move = NullMove;
            pondering = 0;
            ponder_move_ok = false;
        }
        size_t movepos = cmd_args.find("moves");
        if (movepos != string::npos) {
            stringstream s(cmd_args.substr(movepos+5));
            istream_iterator<string> it(s);
            istream_iterator<string> eos;
            while (it != eos) {
                string move(*it++);
                Move m = Notation::value(board,board.sideToMove(),Notation::InputFormat::UCI,move);
                if (!IsNull(m)) {
                   BoardState previous_state = board.state;
                   board.doMove(m);
                   gameMoves->add_move(board,previous_state,m,"",false);
                }
            }
        }
    }
    else if (cmd_word == "test") {
        string filename;
        ofstream *out_file = NULL;
        streambuf *sbuf = cout.rdbuf();
        stringstream s(cmd_args);
        istream_iterator<string> it(s);
        istream_iterator<string> eos;
        if (it != eos) {
            filename = *it++;
            if (it != eos) {
                stringstream num(*it++);
                num >> time_limit;
                time_limit *= 1000; // convert seconds to milliseconds
                early_exit_plies = Constants::MaxPly;
                moves_to_search = 1;
                verbose = 0;
                while (it != eos) {
                    if (*it == "-v") {
                        it++;
                        ++verbose;
                        continue;
                    }
                    else if (*it == "-x") {
                        if (++it == eos) {
                            cerr << "expected number after -x" << endl;
                        } else {
                            stringstream num(*it);
                            num >> early_exit_plies;
                            it++;
                        }
                    }
                    else if (*it == "-N") {
                        if (++it == eos) {
                            cerr << "Expected number after -N" << endl;
                        } else {
                            stringstream num(*it);
                            num >> moves_to_search;
                            it++;
                        }
                    }
                    else if (*it == "-o") {
                        if (++it == eos) {
                            cerr << "Expected filename after -o" << endl;
                        } else {
                           out_file = new ofstream((*it).c_str(), ios::out | ios::trunc);
                           // redirect stdout
                           cout.rdbuf(out_file->rdbuf());
                           break;
                        }
                    } else if ((*it)[0] == '-') {
                        cerr << "unexpected switch: " << *it << endl;
                        it++;
                    } else {
                        break;
                    }
                }
                do_command("new",board);
                PostFunction old_post = searcher->registerPostFunction(post_test);
                TerminateFunction old_terminate = searcher->registerTerminateFunction(terminate);
                Options tmp = options;
                options.book.book_enabled = 0;
                options.learning.position_learning = 0;
                do_test(filename);
                if (out_file) {
                    out_file->close();
                    delete out_file;
                    cout.rdbuf(sbuf);               // restore console output
                }
                options = tmp;
                searcher->registerPostFunction(old_post);
                searcher->registerTerminateFunction(old_terminate);
                cout << "test complete" << endl;
            }
            else
                cout << "invalid command" << endl;
        }
        else
            cout << "invalid command" << endl;
    }
    else if (cmd_word == "perft") {
       if (cmd_args.length()) {
          stringstream ss(cmd_args);
          int depth;
          if ((ss >> depth).fail()) {
             cerr << "usage: perft <depth>" << endl;
          } else {
             Board b;
             cout << "perft " << depth << " = " << perft(b,depth) << endl;
          }
       }
       else {
          cerr << "usage: perft <depth>" << endl;
       }
    }
    else if (cmd_word == "eval") {
        string filename;
        if (cmd_args.length()) {
            filename = cmd_args;
            ifstream pos_file( filename.c_str(), ios::in);
            pos_file >> board;
            if (!pos_file.good()) {
                cout << "File not found, or bad format." << endl;
            }
            else {
                delayedInit();
#if defined(NALIMOV_TBS) || defined(GAVIOTA_TBS) || defined(SYZYGY_TBS)
                score_t tbscore;
                if (options.search.use_tablebases) {
#ifdef GAVIOTA_TBS
                   if (options.search.tablebase_type == Options::TbType::GaviotaTb && GaviotaTb::probe_tb(board,tbscore,0,1)) {
                        cout << "score = ";
                        Scoring::printScore(tbscore,cout);
                        cout << " (from " << Options::tbTypeToString(options.search.tablebase_type) << " tablebases)" << endl;
                    }
#endif
#ifdef NALIMOV_TBS
                    if (options.search.tablebase_type ==
                        Options::TbType::NalimovTb && NalimovTb::probe_tb(board,tbscore,0)) {
                        cout << "score = ";
                        Scoring::printScore(tbscore,cout);
                        cout << " (from " << Options::tbTypeToString(options.search.tablebase_type) << " tablebases)" << endl;
                    }
#endif
#ifdef SYZYGY_TBS
                    if (options.search.tablebase_type == Options::TbType::SyzygyTb) {
                       set<Move> rootMoves;
                       if (SyzygyTb::probe_root(board,tbscore,rootMoves)) {
                          cout << "score = ";
                          Scoring::printScore(tbscore,cout);
                          cout << " (from " << Options::tbTypeToString(options.search.tablebase_type) << " tablebases)" << endl;
                       }
                    }
#endif
                }
#endif
                int score;
                if ((score = Scoring::tryBitbase(board))!= Scoring::INVALID_SCORE) {
                    cout << "bitbase score=";
                    Scoring::printScore(score,cout);
                    cout << endl;
                }
                Scoring::init();
                if (Scoring::isDraw(board))
                    cout << "position evaluates to draw (statically)" << endl;
                Scoring *s = new Scoring();
                s->init();
                cout << board << endl;
                Scoring::printScore(s->evalu8(board),cout);
                cout << endl;
                board.flip();
                cout << board << endl;
                Scoring::printScore(s->evalu8(board),cout);
                delete s;
                cout << endl;
            }
        }
    }
    else if (uci && cmd == "stop") {
        searcher->stop();
        return true;
    }
    else if (uci && cmd_word == "go") {
        string option;
        srctype = TimeLimit;
        int do_ponder = 0;
        movestogo = 0;
        bool infinite = false;
        stringstream ss(cmd_args);
        istream_iterator<string> it(ss);
        istream_iterator<string> eos;
        vector<Move> movesToSearch;
        while (it != eos) {
            option = *it++;
            if (option == "wtime") {
                srctype = TimeLimit;
                if (it == eos) break;
                int t = 0;
                if (Options::setOption<int>(*it++,t)) {
                   if (board.sideToMove() == White)
                       time_left = t;
                   else
                       opp_time = t;
                }
            }
            else if (option == "btime") {
                srctype = TimeLimit;
                if (it == eos) break;
                int t = 0;
                if (Options::setOption<int>(*it++,t)) {
                    if (board.sideToMove() == Black)
                        time_left = t;
                    else
                        opp_time = t;
                }
            }
            else if (option == "infinite") {
                srctype = FixedTime;
                time_limit = INFINITE_TIME;
                infinite = true;
            }
            else if (option == "winc") {
                if (it == eos) break;
                // incr is in milliseconds
                int tmp = 0;
                if (Options::setOption<int>(*it++,tmp)) {
                    winc = tmp;
                }
            }
            else if (option == "binc") {
                if (it == eos) break;
                // incr is in milliseconds
                int tmp = 0;
                if (Options::setOption<int>(*it++,tmp)) {
                    binc = tmp;
                }
            }
            else if (option == "movestogo") {
                if (it == eos) break;
                Options::setOption<int>(*it++,movestogo);
            }
            else if (option == "depth") {
                if (it == eos) break;
                if (Options::setOption<int>(*it++,ply_limit)) {
                    srctype = FixedDepth;
                }
            }
            else if (option == "movetime") {
                if (it == eos) break;
                stringstream s(*it++);
                int srctime;
                s >> srctime;
                if (s.bad() || srctime < 0.0) {
                    cerr << "movetime: invalid argument" << endl;
                } else {
                    srctype = FixedTime;
                    // set time limit (milliseconds)
                    time_limit = srctime;
                }
            }
            else if (option == "ponder") {
                ++do_ponder;
                ponderhit = 0;
            }
            else if (option == "searchmoves") {
                for (;it != eos;it++) {
                    Move m = Notation::value(board,board.sideToMove(),
                                             Notation::InputFormat::UCI,*it);
                    if (IsNull(m)) {
                        // illegal move, or end of move list
                        break;
                    } else {
                        movesToSearch.push_back(m);
                    }
                }
            }
        }
        forceMode = 0;
        if (do_ponder) {
            ponder(board,NullMove,NullMove,1);
            // We should not send the move unless we have received a
            // "ponderhit" command, in which case we were pondering the
            // right move. If pondering completes early, we must wait
            // for ponderhit before sending. But also send the move if
            // we were stopped - this is the "handshake" that tells the
            // UI we received the stop.
#ifdef UCI_LOG
            ucilog << "done pondering: stopped=" << (int)searcher->wasStopped() << " move=";
            Notation::image(board,ponder_move,Notation::OutputFormat::SAN,ucilog);
            ucilog << (flush) << endl;
#endif
            if (ponderhit || searcher->wasStopped()) {
                // ensure we send an "info" command - may not have been
                // sent if the previous move was forced or a tb hit.
                uciOut(ponder_stats);
                send_move(board,ponder_move,ponder_stats);
                ponder_move = NullMove;
                ponderhit = 0;
            }
            else {
                // We completed pondering early - the protocol requires
                // that we delay sending the move until "ponderhit" or
                // "stop" is received
#ifdef UCI_LOG
                ucilog << "entering wait state" << endl << (flush);
#endif
                uciWaitState = 1;
            }
        }
        else {
#ifdef UCI_LOG
            ucilog << "starting search, time=" << getCurrentTime() << (flush) << endl;
#endif
            best_move = search(searcher,board,movesToSearch,stats,infinite);

#ifdef UCI_LOG
            ucilog << "done searching, time=" << getCurrentTime() << ", stopped=" << (int)searcher->wasStopped() << (flush) << endl;
#endif
            if (infinite && !searcher->wasStopped()) {
                // ensure we send some info in analysis mode:
                post_output(stats);
                // We were told "go infinite" but completed searching early
                // (due to a mate or forced move or tb hit). The protocol
                // requires that we go into a wait state before sending the
                // move. We will exit when a "stop" command is received.
                uciWaitState = 1;
            }
            else {
                send_move(board,best_move,stats);
            }
        }
        // reset stopped flag after search
        searcher->setStop(false);
    }
    else if (uci && cmd == "ponderhit") {
        ++ponderhit;
        if (!IsNull(ponder_move)) {
            uciOut(ponder_stats);
            send_move(board,ponder_move,ponder_stats);
        }
    }
    else if (cmd == "help") {
        do_help();
    }
    else if (cmd == "end") {
        return false;
    }
    else if (cmd == "new") {
        if (!analyzeMode) save_game();
        board.reset();
        theLog->clear();
        if (!uci) theLog->write_header();
        computer_plays_white = false;
        // Note: "new" does not reset analyze mode
        forceMode = 0;
        game_end = result_pending = 0;
        testing = 0;
        computer = 0;
        opponent_name = "";
        stats.clear();
        ponder_stats.clear();
        last_stats.clear();
        last_move = NullMove;
        last_move_image.clear();
        gameMoves->removeAll();
        predicted_move = NullMove;
        pondering = 0;
        ponder_move_ok = false;
        start_fen.clear();
        searcher->registerPostFunction(post_output);
        delayedInit();
        searcher->clearHashTables();
#ifdef TUNE
        tune_params.applyParams();
#endif
        if (!analyzeMode && ics) {
           cout << "kib Hello from Arasan " << Arasan_Version << endl;
        }
        if (doTrace) cout << "# finished 'new' processing" << endl;
    }
    else if (cmd == "random" || cmd_word == "variant") {
        // ignore
    }
    else if (cmd_word == "protover") {
        // new in Winboard 4.2
        cout << "feature name=1 setboard=1 san=1 usermove=1 ping=1 ics=1 playother=0 sigint=0 colors=0 analyze=1 debug=1 memory=1 smp=1 variants=\"normal\"";
#if defined(GAVIOTA_TBS) && defined(NALIMOV_TBS) || defined(SYZYGY_TBS)
        string opts(" egt=\"");
        int i = 0;
#if defined(SYZYGY_TBS)
        opts += "syzygy";
        i++;
#endif
#if defined(NALIMOV_TBS)
        if (i) opts += ',';
        opts += "nalimov";
        i++;
#endif
#if defined(GAVIOTA_TBS)
        if (i) opts += ',';
        opts += "gaviota";
#endif
        cout << opts << '"';
#endif
        // custom option for book selectivity
        cout << " option=\"Book selectivity -spin " <<
            options.book.selectivity << " 1 100\"";
        cout << " option=\"Can resign -check " <<
            options.search.can_resign << "\"";
        cout << " option=\"Position learning -check " <<
            options.learning.position_learning << "\"";
        // strength option (new for 14.2)
        cout << " option=\"Strength -spin " << options.search.strength << " 0 100\"";
#ifdef NUMA
        cout << " option=\"Set processor affinity\" -check " <<
           options.search.set_processor_affinity << endl;
        // TBD: limit to actual detected # of CPUs
        cout << " option=\"Affinity offset\" -spin " <<
           options.search.affinity_offset << " 0 " << Constants::MaxCPUs << endl;
#endif
        cout << " myname=\"" << "Arasan " << Arasan_Version << "\"" << endl;
        // set done = 0 because it may take some time to initialize tablebases.
        cout << "feature done=0" << endl;
        delayedInit();
        cout << "feature done=1" << endl;
#ifdef UCI_LOG
        ucilog << "feature done=1" << endl;
#endif
        ++xboard42;
    }
    else if (cmd == "computer") {
        computer = 1;
    }
    else if (cmd_word == "ping") {
        // extract digits
        stringstream digits(cmd.substr(4));
        int pingValue;
        digits >> pingValue;
        cout << "pong " << pingValue << (flush) << endl;
    }
    else if (cmd_word == "ics") {
        hostname = cmd_args;
    }
    else if (cmd == "hint") {
        doHint();
    }
    else if (cmd == "bk") {
        // list book moves
	vector < pair<Move,int> > moves;
        int count = 0;
        delayedInit(); // to allow "bk" before "new"
        if (options.book.book_enabled) {
            count = openingBook.book_moves(*main_board,moves);
        }
        if (count == 0) {
            cout << '\t' << "No book moves for this position." << endl
                << endl;
        }
        else {
            cout << " book moves:" << endl;
            for (int i = 0; i<count; i++) {
                cout << '\t';
                Notation::image(*main_board,moves[i].first,Notation::OutputFormat::SAN,cout);
                cout << endl;
            }
            cout << endl;
        }
    }
    else if (cmd == "depth") {
    }
    else if (cmd_word == "level") {
        parseLevel(cmd_args);
        srctype = TimeLimit;
    }
    else if (cmd_word == "st") {
        process_st_command(cmd_args);
    }
    else if (cmd_word == "sd") {
        stringstream s(cmd_args);
        s >> ply_limit;
        srctype = FixedDepth;
    }
    else if (cmd_word == "time") {
        // my time left
        int t;
        stringstream s(cmd_args);
        s >> t;
        time_left = t*10; // convert from centiseconds to ms
    }
    else if (cmd_word == "otim") {
        // opponent's time left
        int t;
        stringstream s(cmd_args);
        s >> t;
        opp_time = t*10; // convert from centiseconds to ms
    }
    else if (cmd == "post") {
        post = 1;
    }
    else if (cmd == "nopost") {
        post = 0;
    }
    else if (cmd_word == "result") {
        // Game has ended
        theLog->setResult(cmd_args.c_str());
        save_game();
        game_end = 1;
        gameMoves->removeAll();
    }
    else if (cmd == "savegame") {
    }
    else if (cmd == "remove") {
        undo(board);
        undo(board);
    }
    else if (cmd == "undo") {
        undo(board);
    }
    else if (cmd == "resign") {
        // our opponent has resigned
        cout << "# setting log result" << endl;
        if (computer_plays_white)
            theLog->setResult("0-1");
        else
            theLog->setResult("1-0");
        cout << "# set log result" << endl;
    }
    else if (cmd == "draw") {
        // "draw" command. Requires winboard 3.6 or higher.
        if (accept_draw(board)) {
            // Notify opponent. don't assume draw is concluded yet.
            cout << "offer draw" << endl;
        }
        else if (doTrace) {
            cout << "# draw declined" << endl;
        }
    }
    else if (cmd_word == "setboard") {
        start_fen = cmd_args;
        stringstream s(start_fen,ios::in);
        ChessIO::load_fen(s,board);
    }
    else if (cmd_word == "loadgame") {
        string filename = cmd_args;
        ifstream file(filename.c_str(),ios::in);
        loadgame(board,file);
    }
    else if (cmd == "edit") {
        edit_board(cin,board);
    }
    else if (cmd == "analyze") {
        analyzeMode = 1;
        analyze(board);
    }
    else if (cmd == "exit") {
        if (analyzeMode) analyzeMode = 0;
    }
    else if (cmd == ".") {
        analyze_output(stats);
    }
    else if (cmd == "go") {
        forceMode = 0;
        // set the side flag here - do not rely on the deprecated
        // "white" and "black" commands.
        computer_plays_white = board.sideToMove() == White;
        vector<Move> movesToSearch;
        Move reply = search(searcher,board,movesToSearch,stats,false);
        if (!forceMode) send_move(board,reply,stats);
    }
    else if (cmd == "easy") {
        easy = true;
    }
    else if (cmd == "hard") {
        easy = false;
    }
    else if (cmd == "white" || cmd == "black") {
        computer_plays_white = (cmd == "white");
    }
    /**   else if (cmd == "playother") {
          computer_plays_white = !computer_plays_white;
          forceMode = 0;
          int tmp = opp_time;
          opp_time = time_left;
          time_left = tmp;
    }**/
    else if (cmd_word == "name") {
        // We've received the name of our opponent.
        opponent_name = cmd_args;
    }
    else if (cmd == "bogus" || cmd == "accepted") {
    }
    else if (cmd == "force") {
        forceMode = 1;
    }
    else if (cmd_word == "rating") {
        stringstream args(cmd_args);
        args >> computer_rating;
        args >> opponent_rating;
        if (searcher) searcher->setRatingDiff(opponent_rating-computer_rating);
    }
    else if (cmd == "computer") {
        computer = 1;
    }
    else if (cmd_word == "option") {
        // Winboard option command
        processWinboardOptions(cmd_args);
    }
    else if (cmd_word == "cores") {
        // Setting -c on the Arasan command line takes precedence over
        // what the GUI sets
        if (!cpusSet) {
           // set number of threads
           stringstream ss(cmd_args);
           if((ss >> options.search.ncpus).fail()) {
               cerr << "invalid value following 'cores'" << endl;
           } else {
              if (doTrace) {
                 cout << "# setting cores to " << options.search.ncpus << endl;
              }
              options.search.ncpus = std::min<int>(options.search.ncpus,Constants::MaxCPUs);
              searcher->updateSearchOptions();
              searcher->setThreadCount(options.search.ncpus);
           }
        }
    }
    else if (cmd_word == "memory") {
        // Setting -H on the Arasan command line takes precedence over
        // what the GUI sets
        if (!memorySet) {
           // set memory size in MB
           stringstream ss(cmd_args);
           int mbs;
           if((ss >> mbs).fail()) {
               cerr << "invalid value following 'memory'" << endl;
           } else {
               if (doTrace) {
                  cout << "# setting hash size to " << options.search.hash_table_size << " MB" << endl;
               }
               options.search.hash_table_size = (size_t)(mbs*1024L*1024L);
               searcher->updateSearchOptions();
               searcher->resizeHash(options.search.hash_table_size);
           }
        }
    }
    else if  (cmd_word == "egtpath") {
        size_t space = cmd_args.find_first_of(' ');
        string type = cmd_args.substr(0,space);
        transform(type.begin(), type.end(), type.begin(), ::tolower);
        if (type.length() > 0) {
           transform(type.begin(), type.begin()+1, type.begin(), ::toupper);
        }
        string path;
        if (space != string::npos) path = cmd_args.substr(space+1);
#ifdef _WIN32
        // path may be in Unix format. Convert.
        string::iterator it = path.begin();
        while (it != path.end()) {
             if (*it == '/') {
                 *it = '\\';
             }
             it++;
        }
#endif
#if defined(NALIMOV_TBS) || defined(GAVIOTA_TBS) || defined(SYZYGY_TBS)
        Options::TbType tb_type = Options::stringToTbType(type);
        // Unload existing tb set if in use and if type or path
        // has changed
        if ((options.tbPath(tb_type) != path) ||
            (options.search.tablebase_type != tb_type)) {
           if (doTrace) cout << "# unloading tablebases" << endl;
           unloadTb(options.search.tablebase_type);
        }
#endif
        // Set the tablebase options. But do not initialize the
        // tablebases here. Defer until delayedInit is called again.
        // One reason to do this is that we may receive multiple
        // egtpath commands from Winboard.
#ifdef NALIMOV_TBS
        if (tb_type == Options::TbType::NalimovTb) {
            options.search.use_tablebases = 1;
            options.search.nalimov_path = path;
            options.search.tablebase_type = tb_type;
            if (doTrace) {
                cout << "# setting Nalimov tb path to " << options.search.nalimov_path << endl;
            }
        }
#endif
#ifdef GAVIOTA_TBS
        if (tb_type == Options::TbType::GaviotaTb) {
            options.search.use_tablebases = 1;
            options.search.gtb_path = path;
            options.search.tablebase_type = tb_type;
            if (doTrace) {
                cout << "# setting Gaviota tb path to " << options.search.gtb_path << endl;
            }
        }
#endif
#ifdef SYZYGY_TBS
        if (tb_type == Options::TbType::SyzygyTb) {
            options.search.use_tablebases = 1;
            options.search.syzygy_path = path;
            options.search.tablebase_type = tb_type;
            if (doTrace) {
                cout << "# setting Syzygy tb path to " << options.search.syzygy_path << endl;
            }
        }
#endif
    }
    else {
        // see if it could be a move
        string movetext;
        if (cmd_word == "usermove") {
            // new for Winboard 4.2
            movetext = cmd_args;
        } else {
            movetext = cmd;
        }
        string::iterator it = movetext.begin();
        while (it != movetext.end() && !isalpha(*it)) it++;
        movetext.erase(movetext.begin(),it);
        if (doTrace) {
            cout << "# move text = " << movetext << endl;
        }
        Move move;
        if ((move = text_to_move(board,movetext)) != NullMove) {
            if (game_end) {
                if (forceMode)
                    game_end = 0;
                else {
                    if (doTrace) cout << "# ignoring move " << movetext << " received after game end" << endl;
                    return true;
                }
            }
            if (doTrace) {
                cout << "# got move: " << movetext << endl;
            }
            // make the move on the board
            execute_move(board,move);
            if (analyzeMode) {
               // re-enter analysis loop
               analyze(board);
            }
            Move reply;
            if (!forceMode && !analyzeMode) {
               // determine what to do with the pondering result, if
               // there is one.
               if (MovesEqual(predicted_move,move) && !IsNull(ponder_move)) {
                  // We completed pondering already and we got a reply to
                  // this move (e.g. might be a forced reply).
                  if (doTrace) cout << "# pondering complete already" << endl;
                  if (doTrace) {
                     cout << "# sending ponder move ";
                     MoveImage(ponder_move,cout);
                     cout << endl << (flush);
                  }
                  reply = ponder_move;
                  stats = ponder_stats;
                  post_output(stats);
                  game_end |= stats.end_of_game;
                  if (doTrace) cout << "# game_end = " << game_end << endl;
                  predicted_move = ponder_move = NullMove;
               }
               else {
                  predicted_move = ponder_move = NullMove;
                  vector<Move> movesToSearch;
                  reply = search(searcher,board,movesToSearch,stats,false);
                  // Note: we may know the game has ended here before
                  // we get confirmation from Winboard. So be sure
                  // we set the global game_end flag here so that we won't
                  // start pondering after the game is over.
                  game_end |= stats.end_of_game;
                  if (doTrace) {
                     cout << "# state = " << stats.state << endl;
                     cout << "# game_end = " << game_end  << endl;
                  }
               }
               // Check for game end conditions like resign, draw acceptance, etc:
               if (check_pending(board)==1) {
                  game_end = true;
               } else if (!forceMode) {
                  // call send_move even if game_end = true because we
                  // handle resignation, etc. there.
                  send_move(board,reply,stats);
               }
            }
            while (!forceMode && !analyzeMode && !game_end && !result_pending && !easy && time_target >= 100 /* 0.1 second */) {
               ponder_move_ok = false;
               int result;
               // check pending commands again before pondering in case
               // we have a resign or draw, or a move has come in (no
               // good pondering if the opponent has replied already).
               if ((result = check_pending(board)) != 0) {
                   return result == 1;
               }
               ponder(board,reply,stats.best_line[1],uci);
               // check again for game-ending commands before we process
               // ponder result
               if (check_pending(board)==1) {
                  return 1;                       // game end signal seen
               }
               // We are done pondering. If we got a ponder hit
               // (opponent made our predicted move), then we are ready
               // to move now.
               if (ponder_move_ok && !IsNull(ponder_move) && !game_end
                   && !forceMode && !analyzeMode) {
                  // we got a reply from pondering
                  if (doTrace) {
                     cout << "# sending ponder move" << endl;
                  }
                  stats = ponder_stats;
                  send_move(board,ponder_move,stats);
                  post_output(stats);
                  reply = ponder_move;
                  predicted_move = ponder_move = NullMove;
                  // Continue pondering now with the new predicted
                  // move from "stats".
               }
               else {
                  if (doTrace) cout << "# ponder failed, exiting ponder loop" << endl;
                  ponder_move = NullMove;
                  // Leave ponder loop. If we were interrupted by
                  // "usermove" we will ponder again immediately;
                  // otherwise we completed pondering early and
                  // will wait for the next opponent move.
                  break;
               }
            }
        }
    }
    return true;
}

 arasan:: arasan(){
     
 }
             
int CDECL arasan::arasanmain(int argc, char **argv) {
    signal(SIGINT,SIG_IGN);

    // Show a message on the console
    cout << "Arasan " Arasan_Version << ' ' << Arasan_Copyright << endl;
    // Must use unbuffered console
    setbuf(stdin,NULL);
    setbuf(stdout, NULL);
    std::cout.rdbuf()->pubsetbuf(NULL, 0);
    std::cin.rdbuf()->pubsetbuf(NULL, 0);

    Bitboard::init();
    initOptions(argv[0]);
    Attacks::init();
    Scoring::init();
    if (!initGlobals(argv[0], true)) {
        cleanupGlobals();
        exit(-1);
    }
    atexit(cleanupGlobals);

#ifdef _WIN32
    // setup polling thread for input from engine
    polling_terminated = 0;
    DWORD id;
    pollingThreadHandle = CreateThread(NULL,0,
        inputPoll,NULL,
        0,
        &id);
#else
    if (pthread_create(&pollingThreadHandle, NULL, inputPoll, NULL)) {
        perror("input thread creation failed");
    }
#endif

    ecoCoder = new ECO();
    Board board;
    main_board = &board;
    int arg = 1;

    if (argc > 1) {
        while (arg < argc && *(argv[arg]) == '-') {
            char c = *(argv[arg]+1);
            switch (c) {
            case 'c':
                ++arg;
                options.search.ncpus = std::min<int>(Constants::MaxCPUs,atol(argv[arg]));
                ++cpusSet;
                if (options.search.ncpus<=0) {
                    cerr << "-c parameter must be >=1" << endl;
                    exit(-1);
                }
                break;
            case 'i':
                if (strcmp(argv[arg]+1,"ics")==0)
                    ics = true;
                else {
                    cerr << "Warning: unknown option: " << argv[arg]+1 <<
                        endl;
                }
                break;
            case 'H':
                ++arg;
                Options::setMemoryOption(options.search.hash_table_size,
                    string(argv[arg]));
                ++memorySet;
                break;
            case 't':
                //++arg;
                doTrace = true;
                break;
            default:
                cerr << "Warning: unknown option: " << argv[arg]+1 <<
                    endl;
                break;
            }
            ++arg;
        }
    }
    if (arg < argc) {
        cout << "loading " << argv[arg] << endl;
        ifstream pos_file( argv[arg], ios::in);
        if (pos_file.good()) {
            pos_file >> board;
        }
        else {
            cout << "file not found: " << argv[arg] << endl;
            return -1;
        }
    }

    last_move_image.clear();

    if (options.store_games) {
        if (options.game_pathname == "") {
            game_pathname = derivePath("games.pgn");
        }
        else {
            game_pathname = options.game_pathname;
        }
        game_file = new ofstream(game_pathname.c_str(),ios::out | ios::app);
        if (!game_file->good()) {
            cerr << "Warning: cannot open game file. Games will not be saved." << endl;
            delete game_file;
            game_file = NULL;
        }
    }

#ifdef UNIT_TESTS
    int errs = doUnit();
    cout << "Unit tests ran: " << errs << " error(s)" << endl;
#endif

    searcher = new SearchController();

    searcher->registerPostFunction(post_output);

    while(!polling_terminated) {
        if (inputSem.wait()) {
#ifdef UCI_LOG
            ucilog << "wait interrupted" << endl << (flush);
#endif
            break;
        }
        while (!pending.empty() && !polling_terminated) {
            Lock(input_lock);
            string cmd (pending.front());
            pending.pop_front();
#ifdef UCI_LOG
            ucilog << "got cmd (main): " << cmd << endl;
#endif
            if (doTrace) {
                cout << "# got cmd (main): "  << cmd << endl;
                //cout << "# pending size = " << pending.size() << endl;
            }
            Unlock(input_lock);
#ifdef UCI_LOG
            ucilog << "calling do_command(main):" << cmd << (flush) << endl;
#endif
            if (doTrace) cout << "# calling do_command(main):" << cmd << (flush) << endl;
            if (!do_command(cmd,board)) {
                if (doTrace) cout << "#terminating .. " << endl;
                polling_terminated++;
            }
        }
    }
    // handle termination.
    save_game();
    if (doTrace) cout << "# terminating" << endl;
#ifdef UCI_LOG
    ucilog << "terminating" << endl << (flush);
#endif
    delete searcher;
    delete ecoCoder;
#ifdef _WIN32
    TerminateThread(pollingThreadHandle,0);
#else
    polling_terminated++;
    void *value_ptr;
    pthread_join(pollingThreadHandle,&value_ptr);
#endif
    delete game_file;
    return 0;
}
