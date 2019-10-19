// Copyright 1997-2012, 2019 by Jon Dart.  All Rights Reserved
#ifndef _CALCTIME_H_
#define _CALCTIME_H_

#include "types.h"

namespace timeMgmt {

// Calculate the recommended time to search.
// incr = increment in milliseconds
// time_left = side to move time left in millseconds
// ponder = true if pondering enabled
// trace = 1 if tracing on, 0 if off
// Returns: search time in milliseconds 
extern int calcTimeLimit(int moves, int incr, 
      int time_left, bool ponder, int trace);

// UCI version
extern int calcTimeLimitUCI(int movestogo, int incr, 
      int time_left, bool ponder, int trace);

// Determine if we are allowed to use any time beyond the normal
// time target
extern int calcExtraTime(int time_left, int time_target, int inc);

};


#endif
