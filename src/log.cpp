// Copyright 1994, 1995, 2000, 2008-2014, 2017-2018 by Jon Dart.  All Rights Reserved.
#include "log.h"
#include "notation.h"
#include "globals.h"
#include "debug.h"
#include "scoring.h"
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif
#include <sstream>
#include <iomanip>
using namespace std;

#define LOG_FILE_NAME "arasan.log"

LogEntry::LogEntry(const BoardState &state, const Move &move,
                   const string &move_image, bool fromBook,
                   score_t score, int depth)
: my_state(state),
my_move(move),
my_image(move_image),
my_fromBook(fromBook),
my_score(score),
my_depth(depth),
my_result("")
{
}


LogEntry::~LogEntry()
{
}


static void time_image(time_t time,ostream &s)
{
   time /= 1000;                                   // time is in millseconds
   int hrs = (int)(time / 3600);
   int mins = (int)((time - (hrs*3600))/60);
   int secs = (int)(time - (hrs*3600) - (mins*60));
   s << fixed << setw(2) << setfill('0') << hrs << ':';
   s << fixed << setw(2) << setfill('0') << mins << ':';
   s << fixed << setw(2) << setfill('0') << secs;
}


Log::Log()
{
   my_current = 0;
   enabled = 0;
   string log_path;
   if (options.log_enabled) {
      if (options.log_pathname == "") {
         // find the last part of the program pathname
         log_path = derivePath(LOG_FILE_NAME);
      }
      else {
         log_path = options.log_pathname;
      }
      if (options.log_append) {
         log_file.open(log_path.c_str(),ios::out|ios::app);
      }
      else {
         log_file.open(log_path.c_str(),ios::out|ios::in|ios::trunc);
      }
      if (!log_file.is_open() || !log_file.good()) {
#ifdef _WIN32
         MessageBox(NULL,"Can't open log file.  Game moves will not be saved.","",MB_OK);
#else
         cerr << "Warning: Can't open log file : " << log_path << endl;
#endif
      }
      else
         ++enabled;
   }
}


Log::~Log()
{
   if (enabled && log_file.good()) {
      log_file.close();
   }
}


void Log::add_move( Board &board, const Move &emove,
                    const string &move_image, const Statistics *stats,
                    uint64_t elapsed_time,
                    int toFile)
{
   // adding a move clears the "result" field
   setResult("");

   BoardState state = board.state;

   LogEntry entry(
      state, emove, move_image,
      stats ? stats->fromBook : false,
      stats ? stats->display_value : 0,
      stats ? stats->depth : 0);

   // moves are always added at the "current" position.

   if (my_current >= entries.size()) {
       entries.push_back(entry);
   }
   else {
       entries[my_current] = entry;
   }
   my_current++;
   // Adding a move at a given point removes any moves
   // after it in the array.
   if (entries.size() > my_current) {
       entries.resize(my_current);
   }
   stringstream s;
   s << (num_moves()-1)/2 + 1 << ". " << move_image;
   for (int len = (int)s.tellp(); len<15; len++) s << ' ';
   if (stats) {
      if (stats->num_nodes == 0ULL) {
         s << " (book)";
      }
      else {
         s << '\t';
         time_image(elapsed_time,s);
         s << "     " << stats->depth << "\t" << stats->num_nodes << "\t";
         Scoring::printScore(stats->display_value,s);
         const string &pv = stats->best_line_image;
         if (pv.length()) {
            s << '\t';
            // output up to 40 chars of the PV. Do not output the first
            // move, only the predicted continuation after that.
            size_t first_space = pv.find(' ');
            if (first_space != string::npos) {
                string shortPV = pv.substr(first_space+1, first_space+41);
                size_t last_space = shortPV.find_last_of(' ');
                s << shortPV.substr(0,last_space);
            }
         }
      }
      if (stats->value == 9999) {
         // side to move has delivered checkmate
         s << " mate";
         if (board.sideToMove() == White)
            setResult("1-0");
         else
            setResult("0-1");
      }
      else if (stats->state == Stalemate) {
         s << " stalemate";
         setResult("1/2-1/2");
      }
      else if (stats->state == Resigns) {
         if (board.sideToMove() == White)
            setResult("0-1");
         else
            setResult("1-0");
         s << " resigns";
      }
      else if (stats->state == Draw) {
         s << " draw";
         setResult("1/2-1/2");
      }
   }
   if (!enabled || !toFile || !log_file.is_open())
      return;
   log_file << s.str() << endl << (flush);
}


const Move &Log::last_move() const {
   ASSERT(current());
   return move(current()-1);
}


void Log::setResult(const char *result) {
   if (!empty()) {
      entries.back().setResult(result);
   }
}


Log::GameResult Log::getResult() const {
   if (!empty()) {
      const string &result = entries.back().result();
      if (result.find("1/2-1/2") != string::npos)
        return DrawResult;
      else if (result.find("1-0") != string::npos)
        return WhiteWin;
      else if (result.find("0-1") != string::npos)
        return BlackWin;
      else
        return Incomplete;
   }
   else
      return Incomplete;
}


int Log::back_up() {
   if (my_current >0) {
      --my_current;
      return 1;
   }
   else
      return 0;
}


int Log::go_forward()
{
   if (my_current < entries.size()) {
      ++my_current;
      return 1;
   }
   else
      return 0;
}


void Log::reset()
{
   my_current = 0;
}


void Log::clear()
{
   entries.clear();
   my_current = 0;
}


void Log::write_header()
{
   if (enabled && log_file.is_open()) {
      static char header1[] = "Arasan version ";
      static char header2[] = "   move          time     depth\tnodes\tvalue\tpredicted";
      log_file << endl << header1 << Arasan_Version << " game log" << endl;
      log_file << header2 << endl << (flush);
   }
}


void Log::write(const char *s)
{
   if (enabled && log_file.is_open()) {
      log_file << s << (flush);
   }
}


void Log::write_eol()
{
   if (enabled && log_file.is_open())
      log_file << endl;
}

