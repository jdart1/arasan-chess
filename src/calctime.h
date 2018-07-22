// Copyright 1997-2012, 2018 by Jon Dart.  All Rights Reserved
#ifndef _CALCTIME_H_
#define _CALCTIME_H_

#include "types.h"

// Calculate the recommended time to search.
// minutes = minutes per game or time period
// incr = increment in milliseconds
// time_left = side to move time left in millseconds
// opp_time = opponent's time left in milliseconds
// ponderHit = true if we are moving after a ponder hit
// trace = 1 if tracing on, 0 if off
// Returns: search time in milliseconds 
extern int calcTimeLimit(int moves, float minutes, int incr, 
      int time_left, int opp_time, bool ponderHit, int trace);

// UCI version
extern int calcTimeLimit(int movestogo, int incr, 
      int time_left, int opp_time, bool ponderHit, int trace);

#endif
