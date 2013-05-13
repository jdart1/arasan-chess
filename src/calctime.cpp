// Copyright 1997, 1998, 1999, 2012-2013 by Jon Dart. All Rights Reserved.
#include "calctime.h"
#include "globals.h"
#include "util.h"

static const int DEFAULT_MOVES_TO_TC = 40;
static const float PONDER_FACTOR = 1.3F;
static const int GAME_TIME_RESERVE = 75; // try to keep this amt of time in reserve

int calcTimeLimit(int moves, float minutes, int incr,
int time_left, int opp_time, bool ponderHit, int trace)
{
   int moves_in_game = gameMoves->num_moves()/2;  // full moves, not half-moves
   int moves_left;
   if (moves == 0) {
      moves_left = 0;
   } else {
      moves_left = moves-(moves_in_game % moves);
   }
   return calcTimeLimit(moves_left,incr,time_left,opp_time,ponderHit,trace);
}


// UCI version
int calcTimeLimit(int movestogo, int incr,
int time_left, int opp_time, bool ponderHit, int trace)
{
   if (trace) cout << "# movestogo=" << movestogo << " time_left=" << time_left << endl;
   if (movestogo == 0) movestogo = DEFAULT_MOVES_TO_TC;
   time_left = Util::Max(time_left-GAME_TIME_RESERVE,0);
   int time_target = (time_left/movestogo);
   if (ponderHit && movestogo > 4) {
       time_target = (int)(PONDER_FACTOR*time_target);
   }

/*
   if (movestogo > 4 && time_target < time_left/2 && time_left > 5000 && 
       opp_time > (time_left*5)/4) {
       // opponent is moving much slower, adjust our time
       time_target = int(time_target*1.1F);
       if (opp_time > time_left*3/2) {
           time_target = int(time_target*1.1F);
       }
   }
*/
   time_target += 9*incr/10;

   // ensure we never allocate more time than is left
   if (time_target >= time_left + incr) {
      if (trace) cout << "# warning : time_target >= time_left" << endl; 
      time_target = 8*(time_left+incr)/10;
   }

   // set a minimum (10 msec).
   if (time_target < 10) {
      time_target = 10;
   }
   return time_target;
}
