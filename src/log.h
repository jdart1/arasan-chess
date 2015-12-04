// Copyright 1994, 1995, 2000, 2009, 2013-2015 by Jon Dart.  All Rights Reserved.

#ifndef _LOG_H
#define _LOG_H

#include "board.h"
#include "search.h"
#include <fstream>
#include <string>
#include <vector>

using namespace std;

class Log;

class LogEntry
{
     // Maintains info on a move made so far in the game.            

     public:
     
     // create a log entry. 
     // "move_image" is the string representation of the move.
     LogEntry(const BoardState &state,
               const Move &move,
               const string &move_image,
               bool fromBook,
               int score,
               int depth);
             
     // create a null log entry.  Used only to initialize storage.
     LogEntry() {
     }
             
     virtual ~LogEntry();
     
     const Move &move() const
     {
         return my_move;
     }
     
     const char *image() const
     {
         return my_image.c_str();
     }
     
     const BoardState &state() const
     {
        return my_state;
     }
     
     const string & result() const
     {
       return my_result;
     }
     
     void setResult( const char *result)
     {
       my_result = result;
     }
     
     int score() const
     {
         return my_score;
     }
     
     int depth() const
     {
         return my_depth;
     }

     bool fromBook() const {
         return my_fromBook;
     }
    
     int operator == ( const LogEntry &l ) const;
     int operator != ( const LogEntry &l ) const;

     bool operator < (const LogEntry &le) const
     {
        return my_state.hashCode < le.my_state.hashCode;
     }
     

     private:
             
     BoardState my_state;
     Move my_move;
     string my_image;
     bool my_fromBook;
     int my_score;
     int my_depth;
     string my_result;
};

class Log : public vector<LogEntry>
{
     // Maintains a log of moves made in the game so far.  Unlike the
     // Move_Array (see movearr.h), moves are not added and removed
     // during the search process.  The log maintains a size, which
     // is the total number of moves ever added, and a current position,
     // which is normally == to its size, but may be less if the
     // user has "taken back" moves.

     public:         
        
     enum GameResult {WhiteWin, BlackWin, DrawResult, Incomplete};

     Log();
     // create a log.
             
     virtual ~Log();         

     // Add a move to the log.  If "toFile" is true, also record it
     // in the log file.  This is called before the move has been
     // made.
     void add_move( Board &board,
                    const Move &emove,
                    const string &move_image,
                    const Statistics *stats,
                    int toFile);
             
     // remove the most recently added move to the log.
     void remove_move()
     {
        ASSERT(my_current>0);
        pop_back();
        --my_current;
     }
             
     // Return the number of the last move made.  "Backing up"
     // through the moves changes current w/o changing num_moves.        
     unsigned current() const
     {
       return my_current;
     }
     
     // Return the total number of moves made.
     unsigned num_moves() const
     {
       return (unsigned)size();
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
     const Move &move( int n ) const
     {
        return (*this)[n].move();
     }
        
     // remove everything from the log
     void clear();

     // resize down to newSize
     void truncate(int newSize) {
       erase(begin()+newSize,end());
     }
             
     void write_header();

     void write(const char *);

     void write(const string &s) {
       write(s.c_str());
     }

     void write_eol();
     
     void setResult(const char *result);
     
     GameResult getResult() const;
             
     void getResultAsString(string & result) const {
       result = empty() ? "*" : back().result();
     }

     void setEnabled(int enable) {
        enabled = enable;
     }

private:

     void flush();
     unsigned my_current;
     ofstream log_file;     
     char buf[256];
     int enabled;
};

#endif

