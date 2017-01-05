// Copyright 1994-2009, 2012-2014 by Jon Dart. All Rights Reserved.

#ifndef _STATS_H
#define _STATS_H

#include "chess.h"
#include "constant.h"
#define MAX_PV 10
#include <string>
using namespace std;

enum StateType {NormalState,Terminated,Check,Checkmate,
                Stalemate,Draw,Resigns};

// This structure holds information about a search
// after it has completed.
struct Statistics
{
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
   unsigned elapsed_time; // in milliseconds
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
   uint64_t last_split_time;
#ifdef SMP_STATS
   uint64_t samples, threads;
#endif
#ifdef MOVE_ORDER_STATS
   int move_order[4];
   int move_order_count;
#endif
#ifdef TUNE
   double target;
   double target_out_window;
   uint64_t result_norm;
#endif
   int end_of_game;
   Statistics();
   virtual ~Statistics();
   void clear();
   void printNPS(ostream &);
};

#endif
