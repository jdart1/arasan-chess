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

//#ifdef SYZYGY_TBS
//std::string Options::tbPath() const { return search.syzygy_path; }
//#endif

Options::SearchOptions::SearchOptions()
    : hash_table_size(32 * 1024 * 1024), can_resign(true),
      resign_threshold(-500),
//#ifdef SYZYGY_TBS
//      use_tablebases(false), syzygy_path("syzygy"), syzygy_50_move_rule(true),
//      syzygy_probe_depth(4),
//#endif
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

template <class T>
bool Options::setOption(const std::string &name, const std::string &valueString, T &value) {
    if (!Options::setOption<T>(valueString, value)) {
        std::cerr << "warning: invalid value for option " << name << std::endl;
        return false;
    }
    return true;
}

static void set_strength_option(const std::string &name, int &value,
                                const std::string &valueString) {
    int tmp = value;
    if (sscanf(valueString.c_str(), "%d", &tmp) != 1 || tmp < 0 || tmp > 100) {
        std::cerr << "warning: invalid value for option " << name << " (expected integer 0..100)"
                  << std::endl;
        return;
    }
    value = tmp;
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

void Options::set_option(const std::string &name, const std::string &value) {
    if (name == "store_games") {
        setOption<bool>(name, value, games.store_games);
    } else if (name == "game_pathname") {
        games.game_pathname = value;
    } else if (name == "book.book_enabled") {
        setOption<bool>(name, value, book.book_enabled);
    } else if (name == "book.variety") {
        setOption<unsigned>(name, value, book.variety);
        globals::openingBook.setVariety(std::min<unsigned>(100,std::max<unsigned>(0, book.variety)));
    } else if (name == "learning.position_learning") {
        setOption<bool>(name, value, learning.position_learning);
    } else if (name == "learning.position_learning.threshold") {
        int tmp;
        if (setOption<int>(name, value, tmp)) {
            learning.position_learning_threshold = (64 * tmp) / 100;
        }
    } else if (name == "learning.position_learning.minDepth") {
        setOption<int>(name, value, learning.position_learning_minDepth);
    } else if (name == "search.can_resign") {
        setOption<bool>(name, value, search.can_resign);
    } else if (name == "search.resign_threshold") {
        setOption<int>(name, value, search.resign_threshold);
    } else if (name == "search.hash_table_size") {
        // command line option takes precedence
        if (!memorySet) setMemoryOption(search.hash_table_size, value);
    }
//#ifdef SYZYGY_TBS
//    else if (name == "search.use_tablebases") {
//        setOption<bool>(name, value, search.use_tablebases);
//    } else if (name == "search.syzygy_path") {
//        search.syzygy_path = value;
//    } else if (name == "search.syzygy_50_move_rule") {
//        setOption<bool>(name, value, search.syzygy_50_move_rule);
//    } else if (name == "search.syzygy_probe_depth") {
//        setOption<int>(name, value, search.syzygy_probe_depth);
//    }
//#endif
    else if (name == "search.strength") {
        set_strength_option(name, search.strength, value);
    } else if (name == "search.ncpus") {
        // command line option takes precdedence
        if (!cpusSet) setOption<int>(name, value, search.ncpus);
    } else if (name == "search.nnueFile") {
        search.nnueFile = value;
    }
#ifdef NUMA
    else if (name == "search.set_processor_affinity") {
        setOption<bool>(name, value, search.set_processor_affinity);
    }
#endif
    else if (name == "search.move_overhead") {
        setOption<int>(name, value, search.move_overhead);
    } else if (name == "search.minimum_search_time") {
        setOption<int>(name, value, search.minimum_search_time);
    } else
        std::cerr << "warning: unrecognized option name: " << name << std::endl;
}

bool Options::init(const std::string &fileName, bool memSet, bool cpuSet) {
    memorySet = memSet;
    cpusSet = cpuSet;
    std::ifstream infile(fileName.c_str());
    if (!infile.good()) {
        return false;
    }
    const auto pattern = std::regex("^([^\\s]+)=([^\\s]+([^\\s]+\\w+)*?)\\s*$");
    std::smatch match;
    std::string name, value, line;
    unsigned count = 0;
    while (infile.good() && !infile.eof()) {
        getline(infile, line);
        ++count;
        if (!line.empty() && line[0] == '#')
            continue;
        if (line.empty())
            continue;
        if (std::regex_match(line, match, pattern)) {
            auto it = match.begin() + 1;
            name = *it++;
            value = *it;
            set_option(name, value);
        } else {
            std::cerr << "warning: could not parse .rc line " << count << std::endl;
        }
    }
    return true;
}
