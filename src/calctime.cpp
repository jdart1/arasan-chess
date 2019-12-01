// Copyright 1997, 1998, 1999, 2012-2013, 2017-2019 by Jon Dart. All Rights Reserved.
#include "calctime.h"
#include "globals.h"

#include <algorithm>

static const int DEFAULT_MOVES_TO_TC = 28;
static const float PONDER_FACTOR = 1.4F;
static const int EXTRA_TIME_FACTOR_INC = 8;
static const int EXTRA_TIME_FACTOR_NO_INC = 12;
static const double EXTRA_TIME_MULT = 2.5;
static const int MOVES_TO_GO_THRESHOLD = 6;

int timeMgmt::calcTimeLimit(int moves, int incr, int time_left, bool ponder)
{
    int moves_in_game = gameMoves->num_moves()/2;  // full moves, not half-moves
    int moves_left;
    if (moves == 0) {
        moves_left = 0;
    } else {
        moves_left = moves-(moves_in_game % moves);
    }
    return calcTimeLimitUCI(moves_left,incr,time_left,ponder);
}


// UCI version
int timeMgmt::calcTimeLimitUCI(int movestogo, int incr,
                               int time_left, bool ponder)
{
    if (movestogo == 0) movestogo = DEFAULT_MOVES_TO_TC;
    double factor = 1.0;
    if (movestogo < MOVES_TO_GO_THRESHOLD) {
        // reduce time if near time control limit
        factor = 1.0 - 0.05*(MOVES_TO_GO_THRESHOLD-movestogo);
    }
    int time_target = static_cast<int>(std::max<double>(0,factor*(std::max<int>(0,movestogo-1)*incr + time_left)/movestogo - options.search.move_overhead));

    if (ponder && movestogo > 4) {
        time_target = (int)(PONDER_FACTOR*time_target);
    }

    // ensure we don't allocate more time than is left
    time_target = std::min<int>(time_left - options.search.move_overhead,time_target);

    // enforce minimum search time
    return std::max<int>(time_target,options.search.minimum_search_time);
}

int timeMgmt::calcExtraTime(int time_left, int time_target, int inc) {
    if (inc == 0 && time_left < time_target*EXTRA_TIME_FACTOR_NO_INC) {
        return std::max<int>(0,int(-EXTRA_TIME_MULT*time_target + 2*(EXTRA_TIME_MULT/EXTRA_TIME_FACTOR_NO_INC)*time_left));
    } else if (inc > 0 && time_left < time_target*EXTRA_TIME_FACTOR_INC) {
        return std::max<int>(0,int(-EXTRA_TIME_MULT*time_target + 2*(EXTRA_TIME_MULT/EXTRA_TIME_FACTOR_INC)*time_left));
    } else {
        return int(EXTRA_TIME_MULT*time_target);
    }
}
