// Copyright 2000-2014, 2016-2019, 2021-2024 by Jon Dart. All Rights Reserved.
#ifndef _OPTIONS_H
#define _OPTIONS_H

#include <algorithm>
#include <sstream>
#include <string>
#include <type_traits>

#include "types.h"

class Options {

  public:
    static constexpr int MIN_RATING = 1000;
    static constexpr int MAX_RATING = 3450;

    struct BookOptions {
        BookOptions();

        unsigned variety;
        bool book_enabled;
        std::string book_path;
    } book;

    struct SearchOptions {
        SearchOptions();

        size_t hash_table_size;
        bool can_resign;
        int resign_threshold;
#ifdef SYZYGY_TBS
        bool use_tablebases;
        std::string syzygy_path;
        bool syzygy_50_move_rule;
        int syzygy_probe_depth;
#endif
        int strength; // 0 .. 100
        int multipv;
        int ncpus;
#ifdef NNUE
        bool useNNUE;
        bool pureNNUE;
        std::string nnueFile;
#endif
#ifdef NUMA
        bool set_processor_affinity; // lock threads to processors
#endif
        int move_overhead;       // in milliseconds
        int minimum_search_time; // in milliseconds
        int widePlies;           // do wide search for this number of plies
        score_t wideWindow;      // score range for wide window search
    } search;

    struct LearningOptions {
        LearningOptions()
            : position_learning(false), position_learning_threshold(33),
              position_learning_horizon(5), position_learning_minDepth(7) {}
        bool position_learning;
        int position_learning_threshold;
        int position_learning_horizon;
        int position_learning_minDepth;
        std::string learn_file_name;
    } learning;

    // Constructor, sets default options
    Options() : store_games(true) {}

    template <class T> static int setOption(const std::string &value, T &dest) {
        bool ok;
        if constexpr (std::is_same<bool, T>::value) {
            ok = value == "true" || value == "false";
            if (ok)
                dest = value == "true";
        } else {
            std::stringstream buf(value);
            T tmp;
            buf >> tmp;
            ok = !buf.bad() && !buf.fail();
            if (ok)
                dest = tmp;
        }
        return ok;
    }

    void setRating(int rating) { search.strength = getStrength(rating); }

    int getStrength(int elo) const noexcept {
        return std::max<int>(
            0, std::min<int>(100, 100 * (elo - MIN_RATING) / (MAX_RATING - MIN_RATING)));
    }

    int getRating(int strength) const noexcept {
        return MIN_RATING + (strength) * (MAX_RATING - MIN_RATING) / 100;
    }

    static void setMemoryOption(size_t &value, const std::string &valueString);

    std::string tbPath() const;

    // sets options based on a .rc file
    int init(const std::string &optionsFile);

    bool store_games;
    std::string log_pathname;
    std::string game_pathname;

  private:
    void set_option(const std::string &name, const std::string &value);

    template <class T>
    int setOption(const std::string &name, const std::string &valueString, T &val);
};

#endif
