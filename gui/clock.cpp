// Copyright 1993-5, 2002, 2008 by Jon Dart.  All Rights Reserved.

#include "stdafx.h"
#include "clock.h"
#include "display.h"
#include "guiView.h"
#include "resource.h"

Clock::Clock( CWnd * parent )
: pWin(parent->m_hWnd),view((ArasanGuiView*)parent), running(TRUE), dir(Up)
{
   limit[White] = limit[Black] = 0;
   reset();
}


Clock::~Clock()
{
}


void Clock::reset()
{
   etime[White] = etime[Black] = 0;
   last_start[White] =
      last_start[Black] = -1;
   //show_time(White,0);
   //show_time(Black,0);
   running = TRUE;
   stopped = FALSE;
   time_up = FALSE;
}


void Clock::start( ColorType side )
{
   const ColorType oside = OppositeColor(side);
   time_t currentTime = GetTickCount()/10;
   // Make sure opposing side's clock is current:
   if (last_start[oside] != (time_t)-1) {
      etime[oside] +=  currentTime - last_start[oside];
      show_time(oside,0);
   }
   // Now start the clock for "side":
   if (last_start[side] == (time_t)-1)
      last_start[side] = currentTime;
   last_start[side_to_move()] = currentTime;
   show_time(side_to_move(),1);
   running = TRUE;
   stopped = FALSE;
}


void Clock::pause()
{
   etime[side_to_move()] += GetTickCount()/10 - last_start[side_to_move()];
   show_time(side_to_move(),0);
   running = FALSE;
}


void Clock::resume()
{
   running = TRUE;
   last_start[side_to_move()] = GetTickCount()/10;
}


void Clock::stop()
{
   running = FALSE;
   stopped = TRUE;
}


// This is only really meaningful when "counting down":
time_t Clock::time_left( const ColorType side )
{
   time_t elapsed = elapsed_time(side) + GetTickCount()/10 -
      last_start[side];
   elapsed = (limit[side] > elapsed) ?
      limit[side] - elapsed : 0;
   return elapsed;
}


void Clock::update()
{
   if (running) {
      time_t elapsed = elapsed_time(side_to_move()) + GetTickCount()/10 -
         last_start[side_to_move()];
      time_t displayTime = elapsed;
      if (dir == Down)
         displayTime = (limit[side_to_move()] > elapsed) ?
            limit[side_to_move()] - elapsed : 0;
      view->showTime(displayTime, side_to_move(),1);
      if (dir == Down && displayTime == 0 && !time_up) {
         time_up = TRUE;
         etime[side_to_move()] = elapsed;
         stop();                                  // prevent further updates
         AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_LOSSONTIME,0L);
      }
   }
}


void Clock::count_up()
{
   dir = Up;
}


void Clock::count_down(time_t lim, const ColorType side)
{
   dir = Down;
   limit[side] = lim;
   etime[side] = 0;
}


void Clock::show_time( ColorType side, int active )
{
   time_t elapsed = elapsed_time(side);
   if (dir == Down)
      elapsed = (limit[side] > elapsed) ?
         limit[side] - elapsed : 0;
   if (view->m_hWnd) {
      view->showTime(elapsed,side,active);
   }
}


ColorType Clock::side_to_move() const
{
   return (view == NULL) ? White :
   ((ArasanGuiDoc*)view->GetDocument())->getCurrentBoard().sideToMove();
}
