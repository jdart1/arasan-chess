// Copyright 1997-2012, 2019-2020, 2024 by Jon Dart.  All Rights Reserved
#ifndef _CALCTIME_H_
#define _CALCTIME_H_

#include "types.h"

namespace timeMgmt {

struct Times {
    int time_target, extra_time;
};

// Calculate the recommended time to search.  incr = increment in
// milliseconds time_left = side to move time left in millseconds,
// ponder = true if pondering enabled, ics = true if -ics specified.
// Returns: search time in milliseconds
extern void calcTimeLimit(int moves, int incr, int time_left, int opp_time, bool ponder,
                          Times &times);
} // namespace timeMgmt

#endif
