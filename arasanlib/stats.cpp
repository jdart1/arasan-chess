// Copyright 1994-2008, 2012, 2013, 2017 by Jon Dart. All Rights Reserved.

#include "stats.h"
#include "scoring.h"
#include <algorithm>
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
   tb_value = Scoring::INVALID_SCORE;
   fromBook = false;
   complete = 0;
   elapsed_time = 0;
   int i;
   best_line_image.clear();
   for (i = 0; i < Constants::MaxPly; i++) {
        best_line[i] = NullMove;
   }
   multipv_count = 0;
   multipv_limit = 1;
   failhigh = faillow = 0;
   depth = 0;
   num_nodes = (uint64_t)0;
   display_value = Scoring::INVALID_SCORE;
#ifdef SEARCH_STATS
   num_qnodes = reg_nodes = moves_searched = static_null_pruning =
       razored = (uint64_t)0;
   hash_hits = hash_searches = futility_pruning = null_cuts = lmp = (uint64_t)0;
   history_pruning = lmp = see_pruning = (uint64_t)0;
   check_extensions = capture_extensions =
     pawn_extensions = evasion_extensions = singular_extensions = 0L;
#endif
   end_of_game = 0;
   mvleft = mvtot = 0;
   tb_probes = tb_hits = (uint64_t)0;
#ifdef MOVE_ORDER_STATS
   move_order_count = 0;
   for (i = 0; i < 4; i++) move_order[i]=0;
#endif
   last_split_time = getCurrentTime();
#ifdef SMP_STATS
   splits = samples = threads = 0L;
   last_split_sample = 0ULL;
#endif
}

void Statistics::sortMultiPVs() {
   // Ensure Multi PVs are in descending order by score (may not
   // happen automatically, esp. when finding mate scores).
    std::sort(multi_pvs.begin(), multi_pvs.begin()+multipv_limit,
              [] (const MultiPVEntry &a, const MultiPVEntry &b) {
                  return a.score > b.score; });
}

void Statistics::printNPS(ostream &s) {
   double nps = ((float)num_nodes)/((float)elapsed_time);
   std::ios_base::fmtflags original_flags = s.flags();
   s.setf(ios::fixed);
   s << setprecision(2);
   if (nps >= 1000.0) {
     s << nps/1000.0 << "M";
   }
   else {
      s << nps << "k";
   }
   s.flags(original_flags);
}

