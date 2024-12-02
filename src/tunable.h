// Copyright 2024 by Jon Dart. All Rights Reserved.
#ifndef TUNABLE_H
#define TUNABLE_H

#include "constant.h"
#include <string>

#ifdef TUNE
#include <memory>
#include <map>

struct Tunable {
    std::string name;
    int default_value, current_value, min_value, max_value;

    using tune_map = std::map<std::string, Tunable *>;
    static inline tune_map tunables;

    Tunable(const std::string &n, int defaultval, int minval, int maxval)
        : name(n), default_value(defaultval), current_value(defaultval), min_value(minval),
          max_value(maxval) {
        tunables.insert({name, this});
    }

    Tunable() : name(""), default_value(0), current_value(0), min_value(0), max_value(0) {}

    Tunable(const Tunable &t) = default;

    void setCurrent(int curr) { current_value = curr; }

    inline operator int() const { return current_value; }
};

#define TUNABLE(name, value, minval, maxval) static inline Tunable name { MAKE_STR(name), value, minval, maxval }
#else
#define TUNABLE(name, value, minval, maxval) static constexpr int name = value
#endif

#endif
