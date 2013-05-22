// Copyright 1994-2009, 2012 by Jon Dart. All Rights Reserved.

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
   int value;
   int display_value;
   int complete; // if at end of iteration
   int multipv_count; // only for UCI
   int multipv_limit; // only for UCI
   int failhigh, faillow;
   Move best_line[Constants::MaxPly];
   string best_line_image;
   unsigned elapsed_time; // in centiseconds
   unsigned depth;
   uint64 num_moves;
   uint64 num_nodes;
   uint64 num_qnodes;
   int mvtot; // total root moves 
   int mvleft; // moves left to analyze at current depth
   uint64 tb_probes; // tablebase probes
   uint64 tb_hits;   // tablebase hits
   uint64 futility_pruning;
   uint64 num_null_cuts;
   uint64 num_razored;
   uint64 check_extensions, recap_extensions,
     pawn_extensions, forced_extensions;
   uint64 hash_hits, hash_searches;
   uint64 splits;
   uint64 last_split_sample;
   uint64 last_split_time;
#ifdef SMP_STATS
   uint64 samples, threads;
#endif
#ifdef MOVE_ORDER_STATS
   int move_order[4];
   int move_order_count;
#endif
   int end_of_game;
   Statistics();
   void clear();
   void printNPS(ostream &);
};

#endif
