// Copyright 2002-2014, 2016-2017 by Jon Dart. All Rights Reserved.
#include "options.h"

#include <fstream>
#include <ctype.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <iterator>

#include "constant.h"

#include <cmath>

using namespace std;

const string Options::NALIMOV_TYPE = "Nalimov";
const string Options::GAVIOTA_TYPE = "Gaviota";
const string Options::SYZYGY_TYPE = "Syzygy";

Options::TbType Options::stringToTbType(const string &value) 
{
#ifdef GAVIOTA_TBS
   if (value == GAVIOTA_TYPE) {
      return Options::TbType::GaviotaTb;
   }
#endif
#ifdef NALIMOV_TBS
   if (value == NALIMOV_TYPE) {
      return Options::TbType::NalimovTb;
   }
#endif
#ifdef SYZYGY_TBS
   if (value == SYZYGY_TYPE) {
      return Options::TbType::SyzygyTb;
   }
#endif
   return Options::TbType::None;
}

string Options::tbTypeToString(TbType type) 
{
#ifdef GAVIOTA_TBS
   if (type == Options::TbType::GaviotaTb) {
      return GAVIOTA_TYPE;
   }
#endif
#ifdef NALIMOV_TBS
   if (type == Options::TbType::NalimovTb) {
      return NALIMOV_TYPE;
   }
#endif
#ifdef SYZYGY_TBS
   if (type == Options::TbType::SyzygyTb) {
      return SYZYGY_TYPE;
   }
#endif
   return "None";
}

string Options::tbPath(Options::TbType type) const
{
#ifdef GAVIOTA_TBS
   if (type == Options::TbType::GaviotaTb) {
      return search.gtb_path;
   }
#endif
#ifdef NALIMOV_TBS
   if (type == Options::TbType::NalimovTb) {
      return search.nalimov_path;
   }
#endif
#ifdef SYZYGY_TBS
   if (type == Options::TbType::SyzygyTb) {
      return search.syzygy_path;
   }
#endif
   return "";
}

Options::SearchOptions::SearchOptions() :
      checks_in_qsearch(1),
      hash_table_size(32*1024*1024),
      can_resign(1),
      resign_threshold(-500),
#if defined(NALIMOV_TBS) || defined(GAVIOTA_TBS) || defined(SYZYGY_TBS)
      use_tablebases(0),
      tablebase_type(TbType::None),
      tb_probe_in_search(1),
#endif
#ifdef GAVIOTA_TBS
      gtb_cache_size((size_t)8*1024L*1024L),
      gtb_scheme("cp4"),
      gtb_path("gtb"),
#endif
#ifdef NALIMOV_TBS
      nalimov_cache_size((size_t)8*1024L*1024L),
      nalimov_path("TB"),
#endif
#ifdef SYZYGY_TBS
      syzygy_path("syzygy"),
      syzygy_50_move_rule(1),
      syzygy_probe_depth(4),
#endif
      strength(100),
      multipv(1),
      ncpus(1),
      easy_plies(3),
      easy_threshold(2000)
#ifdef NUMA
      ,
      set_processor_affinity(0),
      affinity_offset(0)
#endif
{
}


template <class T>
int Options::setOption(const string &name,
                        const string &valueString, T &value) {
    if (!Options::setOption<T>(valueString,value)) {
        cerr << "warning: invalid value for option " << name << endl;
        return 0;
    }
    return 1;
}

static void set_strength_option(const string &name,
			   int &value, const string &valueString) {
  int tmp = value;
  if (sscanf(valueString.c_str(),"%d",&tmp) != 1 || tmp < 0 || tmp > 100) {
    cerr << "warning: invalid value for option " << name << " (expected integer 0..100)"  << endl;
    return;
  }
  value = tmp;
}

void Options::setMemoryOption(size_t &value, const string &valueString) {
  string s = valueString;
  auto it = s.end()-1;
  size_t mult = 1L;
  if (*it == 'k' || *it == 'K') {
    s.erase(it);
    mult = 1024L;
  }
  else if (*it == 'm' || *it == 'M') {
    s.erase(it);
    mult = 1024L*1024L;
  }
  else if (*it == 'g' || *it == 'G') {
    s.erase(it);
    mult = 1024L*1024L*1024L;
  }
  stringstream ss(s);
  size_t val;
  if((ss >> val).fail())
     return; // invalid value
  else
     value = (size_t)val*mult;
}

static void set_boolean_option(string name,string valueString,int &value) {
  if (valueString == "true")
    value = 1;
  else if (valueString == "false")
    value = 0;
  else
    cerr << "warning: invalid value for option " << name << " (expected 'true' or 'false')"  << endl;
}

void Options::set_option(const string &name, const string &value) {
  if (name == "log.enabled") {
     set_boolean_option(name,value,log_enabled);
  }
  else if (name == "log.append") {
     set_boolean_option(name,value,log_append);
  }
  else if (name == "log.pathName") {
    log_pathname = value;
  }
  else if (name == "store_games") {
    set_boolean_option(name,value,store_games);
  }
  else if (name == "game_pathname") {
    game_pathname = value;
  }
  else if (name == "book.book_enabled") {
    set_boolean_option(name,value,book.book_enabled);
  }
  else if (name == "book.selectivity") {
     setOption<int>(name,value,book.selectivity);
  }
  else if (name == "book.random") {
     setOption<int>(name,value,book.random);
  }
  else if (name == "learning.position_learning") {
    set_boolean_option(name,value,learning.position_learning);
  }
  else if (name == "learning.position_learning.threshold") {
    int tmp;
    if (setOption<int>(name,value,tmp)) {
       learning.position_learning_threshold = (64*tmp)/100;
    }
  }
  else if (name == "learning.position_learning.minDepth") {
     setOption<int>(name,value,learning.position_learning_minDepth);
  }
  else if (name == "search.checks_in_qsearch") {
     setOption<int>(name,value,search.checks_in_qsearch);
  }
  else if (name == "search.can_resign") {
    set_boolean_option(name,value,search.can_resign);
  }
  else if (name == "search.resign_threshold") {
    setOption<int>(name,value,search.resign_threshold);
  }
  else if (name == "search.hash_table_size") {
    setMemoryOption(search.hash_table_size,value);
  }
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS) || defined(SYZYGY_TBS)
  else if (name == "search.use_tablebases") {
    set_boolean_option(name,value,search.use_tablebases);
  }
  else if (name == "search.tablebase_type") {
     TbType tmp = search.tablebase_type;
     search.tablebase_type = stringToTbType(value);
     if (search.tablebase_type == TbType::None) {
        cerr << "Invalid tablebase type: " << value << endl;
        search.tablebase_type = tmp;
     }
  }
#endif
#ifdef GAVIOTA_TBS
  else if (name == "search.gtb_cache_size") {
    setMemoryOption(search.gtb_cache_size,value);
  }
  else if (name == "search.gtb_path") {
    search.gtb_path = value;
  }
  else if (name == "search.gtb_scheme") {
    search.gtb_scheme = value;
  }
#endif
#ifdef NALIMOV_TBS
  else if (name == "search.nalimov_cache_size") {
    setMemoryOption(search.nalimov_cache_size,value);
  }
  else if (name == "search.nalimov_path") {
    search.nalimov_path = value;
  }
#endif
#ifdef SYZYGY_TBS
  else if (name == "search.syzygy_path") {
    search.syzygy_path = value;
  }
  else if (name == "search.syzygy_50_move_rule") {
    setOption<int>(name,value,search.syzygy_50_move_rule);
  }
  else if (name == "search.syzygy_probe_depth") {
    setOption<int>(name,value,search.syzygy_probe_depth);
  }
#endif
  else if (name == "search.strength") {
    set_strength_option(name,search.strength,value);
  }
  else if (name == "search.ncpus") {
    setOption<int>(name,value,search.ncpus);
  }
#ifdef NUMA
  else if (name == "search.set_processor_affinity") {
    set_boolean_option(name,value,search.set_processor_affinity);
  }
  else if (name == "search.affinity_offset") {
    setOption<int>(name,value,search.affinity_offset);
  }
#endif
  else
    cerr << "warning: unrecognized option name: " << name << endl;
}

int Options::init(const string& fileName) {

  ifstream infile(fileName.c_str());
  if (!infile.good()) {
     return -1;
  }
  string contents;
  while (infile.good() && !infile.eof()) {
      getline(infile,contents);
      string::const_iterator it = contents.begin();
      while (it != contents.end() && isspace(*it)) it++;
      if (it == contents.end()) break;
      if (*it == '#') continue;
      string name, value;
      while (it != contents.end() && *it != '=') {
          name += *it++;
      }
      if (it != contents.end()) it++;
      while (it != contents.end() && !isspace(*it)) {
          value += *it++;
      }
      if (name != "") {
         set_option(name,value);
      }
  }
  return 0;
}

