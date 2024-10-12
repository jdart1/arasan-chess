// Copyright 2002-2014, 2016-2019, 2021, 2023-2024 by Jon Dart. All Rights Reserved.
#include "options.h"

#include <ctype.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdio.h>

#include "constant.h"
#include "globals.h"
#include "params.h"

#include <cctype>
#include <cmath>
#include <regex>

#ifdef SYZYGY_TBS
std::string Options::tbPath() const { return search.syzygy_path; }
#endif

Options::SearchOptions::SearchOptions()
    : hash_table_size(32 * 1024 * 1024), can_resign(true),
      resign_threshold(-500),
#ifdef SYZYGY_TBS
      use_tablebases(false), syzygy_path("syzygy"), syzygy_50_move_rule(true),
      syzygy_probe_depth(4),
#endif
      strength(100), multipv(1), ncpus(1),
      pureNNUE(false), nnueFile(MAKE_STR(NETWORK)),
#ifdef NUMA
      set_processor_affinity(false),
#endif
      move_overhead(30), minimum_search_time(10), widePlies(4),
      wideWindow(10 * Params::PAWN_VALUE) {
}

Options::BookOptions::BookOptions() : variety(50), book_enabled(true), book_path("") {
    globals::openingBook.setVariety(50);
}

void Options::setMemoryOption(size_t &value, const std::string &valueString) {
    std::string s(valueString);
    auto it = s.end() - 1;
    size_t mult = 1L;
    if (*it == 'k' || *it == 'K') {
        s.erase(it);
        mult = 1024L;
    } else if (*it == 'm' || *it == 'M') {
        s.erase(it);
        mult = 1024L * 1024L;
    } else if (*it == 'g' || *it == 'G') {
        s.erase(it);
        mult = 1024L * 1024L * 1024L;
    }
    std::stringstream ss(s);
    size_t val;
    if ((ss >> val).fail())
        return; // invalid value
    else
        value = (size_t)val * mult;
}



