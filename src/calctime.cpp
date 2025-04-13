// Copyright 1997, 1998, 1999, 2012-2013, 2017-2021, 2024-2025 by Jon Dart. All Rights Reserved.
#include "calctime.h"
#include "globals.h"

#include <algorithm>

static const int DEFAULT_MOVES_TO_TC[2] = {28, 40};
static const float PONDER_FACTOR = 1.4F;
static const int EXTRA_TIME_FACTOR_INC = 8;
static const int EXTRA_TIME_FACTOR_NO_INC = 12;
static const double EXTRA_TIME_MULT = 2.5;
static const int MOVES_TO_GO_THRESHOLD = 6;

void timeMgmt::calcTimeLimit(int moves, int incr, int time_left, int opp_time, bool ponder,
                             timeMgmt::Times &times) {
    const int moves_in_game = globals::gameMoves->num_moves()/2;  // full moves, not half-moves
    const bool zero_inc = moves == 0 && incr == 0;
    int moves_left;
    if (moves == 0) {
        moves_left = 0;
    } else {
        moves_left = moves-(moves_in_game % moves);
    }
    if (moves_left == 0) moves_left = DEFAULT_MOVES_TO_TC[zero_inc];
    double factor = 1.0;
    if (moves_left < MOVES_TO_GO_THRESHOLD) {
        // reduce time if near time control limit
        factor = 1.0 - 0.05*(MOVES_TO_GO_THRESHOLD-moves_left);
    }
    int time_target = static_cast<int>(std::max<double>(0,factor*(std::max<int>(0,moves_left-1)*incr + time_left)/moves_left - globals::options.search.move_overhead));

    if (ponder && moves_left > 4) {
        time_target = static_cast<int>(PONDER_FACTOR*time_target);
    }

    // if a zero-increment game, adjust time lower when move count is large and when
    // opponent has more time
    if (zero_inc) {
        if (moves_in_game > 60) {
            int f = std::min<int>(7,(moves_in_game - 40)/20);
            time_target = time_target*(8-f)/8;
        }
        const int threshold = opp_time*2/3;
        if (time_left < threshold) {
            time_target = static_cast<int>(time_target*(1.0 - static_cast<double>(threshold - time_left)/opp_time));
        }
    }

    // ensure we don't allocate more time than is left, and enfore minimum search time
    time_target = std::min<int>(time_left - globals::options.search.move_overhead,time_target);
    time_target = std::max<int>(time_target, globals::options.search.minimum_search_time);

    times.time_target = time_target;

    int extra;
    if (incr == 0 && time_left < time_target*EXTRA_TIME_FACTOR_NO_INC) {
        extra = std::max<int>(0,int(-EXTRA_TIME_MULT*time_target + 2*(EXTRA_TIME_MULT/EXTRA_TIME_FACTOR_NO_INC)*time_left));
        // reduce extra time when little time left
        if (time_left < 500) {
            extra *= time_left/500;
        }
    } else if (incr > 0 && time_left < time_target*EXTRA_TIME_FACTOR_INC) {
        extra = std::max<int>(0,static_cast<int>(-EXTRA_TIME_MULT*time_target + 2*(EXTRA_TIME_MULT/EXTRA_TIME_FACTOR_INC)*time_left));
    } else {
        extra = static_cast<int>(EXTRA_TIME_MULT*time_target);
    }
    times.extra_time = extra;
}
