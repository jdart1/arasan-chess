// Copyright 2000-2014, 2016-2019, 2021 by Jon Dart. All Rights Reserved.
#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <algorithm>
#include <string>
#include <sstream>

#include "types.h"

class Options
{
 public:
  struct BookOptions {
    BookOptions()
       : frequency(50),
         weighting(100),
         scoring(50),
         random(50),
         book_enabled(1)
    { }

    unsigned frequency, weighting, scoring, random;
    int book_enabled;
  } book;

  struct SearchOptions {
    SearchOptions();

   int checks_in_qsearch;
   size_t hash_table_size;
   int can_resign;
   int resign_threshold;
#ifdef SYZYGY_TBS
   int use_tablebases;
   std::string syzygy_path;
   int syzygy_50_move_rule;
   int syzygy_probe_depth;
#endif
   int strength; // 0 .. 100
   int multipv; // for UCI only
   int ncpus;
#ifdef NNUE
   int useNNUE;
   std::string nnueFile;
#endif    
   int easy_plies; // do wide search for "easy move" detection
   int easy_threshold; // wide search width in centipawns
#ifdef NUMA
   int set_processor_affinity; // lock threads to processors
#endif
   int move_overhead; // in milliseconds
   int minimum_search_time; // in milliseconds
  } search;

   struct LearningOptions {
     LearningOptions()
     : position_learning(1),
       position_learning_threshold(33),
       position_learning_horizon(5),
       position_learning_minDepth(7)
     {}
    int position_learning;
    int position_learning_threshold;
    int position_learning_horizon;
    int position_learning_minDepth;
   } learning;


   // Constructor, sets default options
   Options() :
     log_enabled(0),
     log_append(0),
     store_games(1)
   {
   }

   template <class T>
   static int setOption(const std::string &value, T &dest) {
       std::stringstream buf(value);
       T tmp;
       buf >> tmp;
       if (!buf.bad() && !buf.fail()) {
           dest = tmp;
           return 1;
       }
       return 0;
   }

   void setRating(int rating) {
     search.strength = std::max<int>(0,std::min<int>(100,(rating-1000)/16));
   }

   static void setMemoryOption(size_t &value, const std::string &valueString);

   std::string tbPath() const;

   // sets options based on a .rc file
   int init(const std::string &optionsFile);

   int log_enabled;
   int log_append;
   int store_games;
   std::string log_pathname;
   std::string game_pathname;

 private:

   void set_option(const std::string &name,const std::string &value);

   template <class T>
   int setOption(const std::string &name, const std::string &valueString, T &val);
};

#endif
