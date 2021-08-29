// Copyright 1994, 1995, 2000, 2009, 2013-2014, 2017-2019, 2021 by Jon
// Dart.  All Rights Reserved.

#ifndef _LOG_H
#define _LOG_H

#include "board.h"
#include <fstream>
#include <string>
#include <vector>

struct Statistics;

class Log;

class LogEntry
{
   // Maintains info on a move made so far in the game.

public:

   // create a log entry.
   // "move_image" is the string representation of the move.
   LogEntry(const BoardState &state,
            const Move &move,
            const std::string &move_image,
            bool fromBook,
            score_t score,
            int depth);

    LogEntry() : my_move(NullMove),
                my_image(""),
                my_fromBook(false),
                my_score((score_t)0),
                my_depth(0),
                my_result("") {
   }

   virtual ~LogEntry();

   const Move &move() const {
      return my_move;
   }

   const char *image() const {
      return my_image.c_str();
   }

   const BoardState &state() const {
      return my_state;
   }

   const std::string & result() const {
      return my_result;
   }

   void setResult( const char *result) {
      my_result = result;
   }

   score_t score() const {
      return my_score;
   }

   int depth() const {
      return my_depth;
   }

   bool fromBook() const {
      return my_fromBook;
   }

   bool operator < (const LogEntry &le) const noexcept {
      return my_state.hashCode < le.my_state.hashCode;
   }

private:

   BoardState my_state;
   Move my_move;
   std::string my_image;
   bool my_fromBook;
   score_t my_score;
   int my_depth;
   std::string my_result;
};

class Log
{
   // Maintains a log of moves made in the game so far. The log maintains
   // a size, which is the total number of moves ever added, and a current
   // position, which is normally == to its size, but may be less if the
   // user has "taken back" moves.

public:

   enum GameResult {WhiteWin, BlackWin, DrawResult, Incomplete};

   Log();

   virtual ~Log();

   // Add a move to the log.  If "toFile" is true, also record it
   // in the log file.  This is called before the move has been
   // made.
   void add_move( Board &board,
                  const Move &emove,
                  const std::string &move_image,
                  const Statistics *stats,
                  uint64_t elapsed_time,
                  int toFile);

   // remove the most recently added move to the log.
   void remove_move() {
      if (my_current) {
         entries.pop_back();
         --my_current;
      }
   }

   // Return the number of the last move made.  "Backing up"
   // through the moves changes current w/o changing num_moves.
   unsigned current() const noexcept {
      return my_current;
   }

   // Return the total number of moves made.
   unsigned num_moves() const noexcept {
      return (unsigned)entries.size();
   }

   // Decrement the "current" move by one.
   int back_up();

   // Advance the "current" move pointer.
   int go_forward();

   // Reset the "current" position to the start of the game, w/o
   // altering the file or clearing the log.
   void reset();

   // return the last move in the log.  The log must be non-empty.
   const Move &last_move() const;

   // return the nth move in the log.  0 <= n <= num_moves - 1.
   const Move &move( int n ) const {
      return entries[n].move();
   }

   // remove everything from the log
   void clear();

   void write_header();

   void write(const char *);

   void write(const std::string &s) {
      write(s.c_str());
   }

   void write_eol();

   void setResult(const char *result);

   GameResult getResult() const;

   void getResultAsString(std::string & result) const noexcept {
      result = empty() ? "*" : entries.back().result();
   }

   void setEnabled(int enable) {
      enabled = enable;
   }

   bool empty() const {
      return entries.size() == 0;
   }

   const LogEntry &operator[]( size_t index ) const {
      return entries[index];
   }

private:

   std::vector<LogEntry> entries;

   unsigned my_current;
   std::ofstream log_file;
   int enabled;
};

#endif

