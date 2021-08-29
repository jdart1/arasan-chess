 // Copyright 1994-2009, 2012-2018, 2020 by Jon Dart. All Rights Reserved.

#ifndef _STATS_H
#define _STATS_H

#include "chess.h"
#include "constant.h"
#include "hash.h"
#include <array>
#include <atomic>
#include <string>

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
   bool complete; // if at end of iteration
   std::array <Move,Constants::MaxPly> best_line;
   std::string best_line_image;
   unsigned depth;
   std::atomic<unsigned> completedDepth;

   unsigned multipv_count; // only for UCI
   unsigned multipv_limit; // only for UCI
   std::atomic<bool> failHigh, failLow;
   int mvtot; // total root moves
   int mvleft; // moves left to analyze at current depth
   uint64_t tb_probes; // tablebase probes
   // atomic because may need to be read during a search:
   std::atomic<uint64_t> tb_hits;   // tablebase hits
#ifdef SEARCH_STATS
   uint64_t num_qnodes;
   uint64_t reg_nodes;
   uint64_t moves_searched; // in regular search
   uint64_t futility_pruning;
   uint64_t static_null_pruning;
   uint64_t null_cuts;
   uint64_t razored;
   uint64_t check_extensions, capture_extensions,
     pawn_extensions, singular_extensions;
   uint64_t singular_searches;
   uint64_t multicut;
   uint64_t non_singular_pruning;
   uint64_t reduced;
   uint64_t lmp;
   uint64_t history_pruning;
   uint64_t see_pruning;
   uint64_t hash_hits;
   uint64_t hash_searches;
#endif
   // atomic because may need to be read during a search:
   std::atomic<uint64_t> num_nodes;
#ifdef MOVE_ORDER_STATS
   array<int,4> move_order;
   int move_order_count;
#endif
   int end_of_game;

    struct MultiPVEntry {

        score_t value;
        score_t display_value;
        score_t tb_value;
        bool fromBook;
        bool complete; // if at end of iteration
        std::array <Move,Constants::MaxPly> best_line;
        std::string best_line_image;
        unsigned depth;
        unsigned completedDepth;

        Move best;
        MultiPVEntry() : value(0),
                         display_value(0),
                         tb_value(0),
                         fromBook(false),
                         complete(false),
                         best_line_image(""),
                         depth(0),
                         completedDepth(0),
                         best(NullMove){}

        MultiPVEntry(const Statistics &stats) :
              value(stats.value),
              display_value(stats.display_value),
              tb_value(stats.tb_value),
              fromBook(stats.fromBook),
              complete(stats.complete),
              best_line(stats.best_line),
              best_line_image(stats.best_line_image),
              depth(stats.depth),
              completedDepth(stats.completedDepth),
              best(stats.best_line[0])
            {
            }
   };

   std::array<MultiPVEntry,MAX_PV> multi_pvs;

   Statistics();

   virtual ~Statistics();

   Statistics(const Statistics &);

   Statistics & operator = (const Statistics &);

   void clear();

   void clearSearchState();

   void clearPV() {
      best_line_image.clear();
      best_line.fill(NullMove);
   }

   void sortMultiPVs();

   static void printNPS(std::ostream &s,uint64_t num_nodes, uint64_t elapsed_time);

};

#endif
