// Copyright 1994-2008, 2012, 2013 by Jon Dart. All Rights Reserved.

#include "stats.h"
#include "scoring.h"
#include <iomanip>

Statistics::Statistics()
{
   clear();
}

Statistics::~Statistics()
{
}

void Statistics::clear()
{
   state = NormalState;
   value = 0;
   elapsed_time = 0;
   int i;
   best_line_image.clear();
   for (i = 0; i < Constants::MaxPly; i++) {
        best_line[i] = NullMove;
   }
   multipv_count = 0;
   failhigh = faillow = 0;
   depth = 0;
   num_nodes = (uint64)0;
   display_value = Scoring::INVALID_SCORE;
#ifdef SEARCH_STATS
   num_qnodes = (uint64)0;
   hash_hits = hash_searches = (uint64)0;
   futility_pruning = num_null_cuts = num_razored = 0L;
   check_extensions = recap_extensions =
     pawn_extensions = forced_extensions = 0L;
#endif
   end_of_game = 0;
   mvleft = mvtot = 0;
   tb_probes = tb_hits = (uint64)0;
#ifdef MOVE_ORDER_STATS
   move_order_count = 0;
   for (i = 0; i < 4; i++) move_order[i]=0;
#endif
#ifdef SMP_STATS
   splits = samples = threads = 0L;
   last_split_sample = 0ULL;
   last_split_time = getTimeMillisec();
#endif
}

void Statistics::printNPS(ostream &s) {
   double nps = ((float)num_nodes)/((float)elapsed_time);
   s.setf(ios::fixed);
   s << setprecision(2);
   if (nps >= 1000.0) {
     s << nps/1000.0 << "M";
   }
   else {
      s << nps << "k";
   }
}
