// Copyright 1994-2009, 2012-2017 by Jon Dart. All Rights Reserved.

#ifndef _STATS_H
#define _STATS_H

#include "chess.h"
#include "constant.h"
#include <array>
#include <string>
using namespace std;

enum StateType {NormalState,Terminated,Check,Checkmate,
                Stalemate,Draw,Resigns};

// This structure holds information about a search
// during and after completion.
struct Statistics
{
  static const unsigned MAX_PV = 10;

   StateType state;
   score_t value;
   score_t display_value;
   score_t tb_value;
   bool fromBook;
   int complete; // if at end of iteration
   int multipv_count; // only for UCI
   int multipv_limit; // only for UCI
   int failhigh, faillow;
   Move best_line[Constants::MaxPly];
   string best_line_image;
   uint64_t elapsed_time; // in milliseconds
   unsigned depth;
   int mvtot; // total root moves
   int mvleft; // moves left to analyze at current depth
   uint64_t tb_probes; // tablebase probes
   uint64_t tb_hits;   // tablebase hits
#ifdef SEARCH_STATS
   uint64_t num_qnodes;
   uint64_t reg_nodes;
   uint64_t moves_searched; // in regular search
   uint64_t futility_pruning;
   uint64_t static_null_pruning;
   uint64_t null_cuts;
   uint64_t razored;
   uint64_t check_extensions, capture_extensions,
     pawn_extensions, evasion_extensions, singular_extensions;
   uint64_t reduced;
   uint64_t lmp;
   uint64_t history_pruning;
   uint64_t see_pruning;
   uint64_t hash_hits, hash_searches;
#endif
   uint64_t num_nodes;
   uint64_t splits;
   uint64_t last_split_sample;
   CLOCK_TYPE last_split_time;
#ifdef SMP_STATS
   uint64_t samples, threads;
#endif
#ifdef MOVE_ORDER_STATS
   int move_order[4];
   int move_order_count;
#endif
   int end_of_game;

   struct MultiPVEntry {
      int depth;
      score_t score;
      uint64_t time;
      uint64_t nodes;
      uint64_t tb_hits;
      string best_line_image;
      Move best;
      MultiPVEntry() : depth(0),
                        score(0),
                        time(0),
                        nodes(0),
                        tb_hits(0),
                        best_line_image(""),
                        best(NullMove){}

      MultiPVEntry(const Statistics &stats)
        : depth(stats.depth),score(stats.value),
          time(stats.elapsed_time),nodes(stats.num_nodes),
          tb_hits(stats.tb_hits) {
            best_line_image = stats.best_line_image;
            best = stats.best_line[0];
      }

   };

   array<MultiPVEntry,MAX_PV> multi_pvs;

   Statistics();
   virtual ~Statistics();
   void clear();

   void clearPV() {
      int i;
      best_line_image.clear();
      for (i = 0; i < Constants::MaxPly; i++) {
         best_line[i] = NullMove;
      }
   }

   void sortMultiPVs();

   void printNPS(ostream &);

};

#endif
