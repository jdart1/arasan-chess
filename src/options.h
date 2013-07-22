// Copyright 2000-2013 by Jon Dart. All Rights Reserved.
#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <string>
#include <sstream>
using namespace std;

#include "types.h"

class Options
{
 public:
    static const string NALIMOV_TYPE;
    static const string GAVIOTA_TYPE;

  struct BookOptions {
    BookOptions() 
      : selectivity(50),
        random(0),
        book_enabled(1)
    { }

    int selectivity;
    int random;
    int book_enabled;
  } book;

  struct SearchOptions {
    SearchOptions(); 

   int checks_in_qsearch;
   size_t hash_table_size;
   int can_resign;
   int resign_threshold;
#if defined(NALIMOV_TBS) || defined(GAVIOTA_TBS)
   int use_tablebases;
   string tablebase_type;
#endif
#ifdef GAVIOTA_TBS
   size_t gtb_cache_size;
   string gtb_scheme;
   string gtb_path;
#endif
#ifdef NALIMOV_TBS
   size_t nalimov_cache_size;
   string nalimov_path;
#endif
   int strength; // 0 .. 100
   int multipv; // for UCI only
   int ncpus;
#ifdef SELFPLAY
   int mod;
#endif
  } search;

   struct LearningOptions {
     LearningOptions() 
     : position_learning(1),
       position_learning_threshold(33),
       position_learning_horizon(5),
       position_learning_minDepth(7),
       score_learning(1),
       score_learning_horizon(5),
       score_learning_threshold(33),
       score_learning_minDepth(7),
       result_learning(1)
     {}
    int position_learning;
    int position_learning_threshold;
    int position_learning_horizon;
    int position_learning_minDepth;
    int score_learning;
    int score_learning_horizon;
    int score_learning_threshold;
    int score_learning_minDepth;
    int result_learning;
   } learning;


   // Constructor, sets default options
   Options() :
     log_enabled(1),
     log_append(0),
     store_games(1)
   {
   }

   template <class T>
   static int setOption(const string &value, T &dest) {
       stringstream buf(value);
       T tmp;
       buf >> tmp;
       if (!buf.bad() && !buf.fail()) {
           dest = tmp;
           return 1;
       }
       return 0;
   }

   static void setMemoryOption(size_t &value, const string &valueString);

   // sets options based on a .rc file
   int init(const string &optionsFile);

   int log_enabled;
   int log_append;
   int store_games;
   string log_pathname;
   string game_pathname;

 private:
   void set_option(const string &name,const string &value);

   template <class T>
   void setOption(const string &name, const string &valueString, T &val);
};

#endif
