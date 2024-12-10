// Copyright 1993, 2008 by Jon Dart.  All Rights Reserved.

#ifndef _CLOCK_H
#define _CLOCK_H

#include "chess.h"
#include <time.h>
#include <windows.h>

class ArasanGuiView;

class Clock
{
   public:
      enum Direction { Up, Down };

      Clock( CWnd *parent );

      virtual ~Clock();

      void set_handle( const HWND parent );

      void reset();

      void start( ColorType side );

      void pause();

      void resume();

      void stop();

      void update();

      void count_up();

      void count_down( time_t limit, const ColorType side );

      time_t get_limit( const ColorType side ) const
      {
         return limit[side];
      }

      void set_limit( time_t lim, const ColorType side ) {
         limit[side] = lim;
      }

      time_t elapsed_time( const ColorType side ) {
         return etime[side];
      }

      void add_increment(time_t incr, const ColorType side) {
         etime[side] -= incr;
      }

      time_t time_left( const ColorType side );

      // a not-running clock may be stopped or paused.
      BOOL is_running() const
      {
         return running;
      }

      // a stopped clock can only be reset.
      BOOL is_stopped() const
      {
         return stopped;
      }

      // True if one side has exceeded the time limit
      BOOL time_is_up() const
      {
         return time_up;
      }

      void show_time( ColorType side, int active );

      ColorType side_to_move() const;

   private:
      HWND pWin;
      ArasanGuiView *view;
      BOOL running, stopped, time_up;
      time_t etime[2];
      time_t last_start[2];
      time_t limit[2];
      Direction dir;
};
#endif
