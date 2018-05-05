// Copyright 1994-2008, 2012, 2013, 2017-2018 by Jon Dart. All Rights Reserved.

#include "stats.h"
#include "notation.h"
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

// Copy constructor
// Needed because some members are atomic.
// Note: we only copy Statistics after search completion,
// so there is no multithreaded access then.
Statistics::Statistics(const Statistics &s) 
{
   if (this != &s) {
      state = s.state;
      value = s.value;
      display_value = s.display_value;
      tb_value = s.tb_value;
      fromBook = s.fromBook;
      complete = s.complete;
      multipv_count = s.multipv_count;
      multipv_limit = s.multipv_limit;
      failHigh = s.failHigh;
      failLow = s.failLow;
      int i = 0;
      do {
         best_line[i] = s.best_line[i];
         i++;
      } while (i<Constants::MaxPly && !IsNull(best_line[i-1]));
      best_line_image = s.best_line_image;
      depth = s.depth;
      completedDepth = s.completedDepth;
      mvtot = s.mvtot;
      mvleft = s.mvleft;
      tb_probes = s.tb_probes;
      tb_hits = s.tb_hits.load();
#ifdef SEARCH_STATS
      num_qnodes = s.num_qnodes;
      reg_nodes = s.reg_nodes;
      moves_searched = s.moves_searched;
      futility_pruning = s.futility_pruning;
      static_null_pruning = s.static_null_pruning;
      null_cuts = s.null_cuts;
      razored = s.razored;
      check_extensions = s.check_extensions;
      capture_extensions = s.capture_extensions;
      pawn_extensions = s.pawn_extensions;
      evasion_extensions = s.evasion_extensions;
      singular_extensions = s.singular_extensions;
      reduced = s.reduced;
      lmp = s.lmp;
      history_pruning = s.history_pruning;
      see_pruning = s.see_pruning;
      hash_hits = s.hash_hits;
      hash_searches = s.hash_searches;
#endif
      num_nodes = s.num_nodes.load();
#ifdef MOVE_ORDER_STATS
      move_order = s.move_order;
      move_order_count = s.move_order_count;
#endif
      end_of_game = s.end_of_game;
      multi_pvs = s.multi_pvs;
   }
}


// Assignment operator
// Needed because some members are atomic.
// Note: we only copy Statistics after search completion,
// so there is no multithreaded access then.
Statistics & Statistics::operator = (const Statistics &s)
{
   if (this != &s) {
      state = s.state;
      value = s.value;
      display_value = s.display_value;
      tb_value = s.tb_value;
      fromBook = s.fromBook;
      complete = s.complete;
      multipv_count = s.multipv_count;
      multipv_limit = s.multipv_limit;
      failHigh = s.failHigh;
      failLow = s.failLow;
      int i = 0;
      do {
         best_line[i] = s.best_line[i];
         i++;
      } while (i<Constants::MaxPly && !IsNull(best_line[i-1]));
      best_line_image = s.best_line_image;
      depth = s.depth;
      completedDepth = s.completedDepth;
      mvtot = s.mvtot;
      mvleft = s.mvleft;
      tb_probes = s.tb_probes;
      tb_hits = s.tb_hits.load();
#ifdef SEARCH_STATS
      num_qnodes = s.num_qnodes;
      reg_nodes = s.reg_nodes;
      moves_searched = s.moves_searched;
      futility_pruning = s.futility_pruning;
      static_null_pruning = s.static_null_pruning;
      null_cuts = s.null_cuts;
      razored = s.razored;
      check_extensions = s.check_extensions;
      capture_extensions = s.capture_extensions;
      pawn_extensions = s.pawn_extensions;
      evasion_extensions = s.evasion_extensions;
      singular_extensions = s.singular_extensions;
      reduced = s.reduced;
      lmp = s.lmp;
      history_pruning = s.history_pruning;
      see_pruning = s.see_pruning;
      hash_hits = s.hash_hits;
      hash_searches = s.hash_searches;
#endif
      num_nodes = s.num_nodes.load();
#ifdef MOVE_ORDER_STATS
      move_order = s.move_order;
      move_order_count = s.move_order_count;
#endif
      end_of_game = s.end_of_game;
      multi_pvs = s.multi_pvs;
   }
   return *this;
}

void Statistics::clear()
{
   state = NormalState;
   value = 0;
   tb_value = Constants::INVALID_SCORE;
   fromBook = false;
   complete = false;
   int i;
   best_line_image.clear();
   for (i = 0; i < Constants::MaxPly; i++) {
        best_line[i] = NullMove;
   }
   multipv_count = 0;
   multipv_limit = 1;
   failHigh = failLow = false;
   depth = completedDepth = 0;
   num_nodes = (uint64_t)0;
   display_value = Constants::INVALID_SCORE;
#ifdef SEARCH_STATS
   num_qnodes = reg_nodes = moves_searched = static_null_pruning =
       razored = reduced = (uint64_t)0;
   hash_hits = hash_searches = futility_pruning = null_cuts = (uint64_t)0;
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
}

void Statistics::sortMultiPVs() {
   // Ensure Multi PVs are in descending order by score (may not
   // happen automatically, esp. when finding mate scores).
    std::sort(multi_pvs.begin(), multi_pvs.begin()+multipv_limit,
              [] (const MultiPVEntry &a, const MultiPVEntry &b) {
                  return a.score > b.score; });
}

void Statistics::printNPS(ostream &s,uint64_t num_nodes, uint64_t elapsed_time) {
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


void Statistics::updatePV(const Board &board, Move *moves, int pv_length, int iteration_depth, bool uci,
                          int age, Hash &hashTable) {
   Board board_copy(board);
   best_line[0] = NullMove;
   int i = 0;
   best_line_image.clear();
   stringstream sstr;
   while (i < pv_length && i<Constants::MaxPly-1 && !IsNull(moves[i])) {
      ASSERT(i<Constants::MaxPly);
      best_line[i] = moves[i];
      ASSERT(legalMove(board_copy,best_line[i]));
      if (i!=0) sstr << ' ';
      Notation::image(board_copy,best_line[i],
                      uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::SAN,sstr);
      int len = (int)sstr.tellg();
      // limit the length
      if (len > 250) {
         break;
      }
      board_copy.doMove(best_line[i]);
      ++i;
      int rep_count;
      if (Scoring::isDraw(board_copy,rep_count,0)) {
         break;
      }
      if (pv_length < 2) {
         // get the next move from the hash table, if possible
         // (for pondering)
         HashEntry entry;
         HashEntry::ValueType result =
            hashTable.searchHash(board_copy,board_copy.hashCode(rep_count),
                                 0,age,
                                 iteration_depth,entry);
         if (result != HashEntry::NoHit) {
            Move hashMove = entry.bestMove(board_copy);
            if (!IsNull(hashMove)) {
               best_line[i] = hashMove;
               if (i!=0) sstr << ' ';
               Notation::image(board_copy,hashMove,
                               uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::SAN,sstr);
               ++i;
            }
            break;
         }
      }
   }
   best_line[i] = NullMove;
   best_line_image = sstr.str();
}

