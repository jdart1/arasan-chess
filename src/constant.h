// Copyright 1994-2025 by Jon Dart.  All Rights Reserved.

#ifndef _CONSTANT_H
#define _CONSTANT_H

#include "types.h"

struct Constants {

    static constexpr int MaxPly = 62;
    static constexpr score_t MATE = 32000;
    static constexpr score_t MATE_RANGE = MATE - 256;
    static constexpr score_t TABLEBASE_WIN = MATE_RANGE - 256;
    static constexpr score_t BITBASE_WIN = MATE_RANGE - 1000;
    static constexpr score_t INVALID_SCORE = -MATE - 1;
    static constexpr int MaxMoves = 220;
    static constexpr int MaxCaptures = 40;
    static constexpr int MaxChecks = 40;
    static constexpr int MaxCPUs = 512;
    static constexpr int INFINITE_TIME = 100000000;
};

#define STRINGIFY(x) #x
#define MAKE_STR(x) STRINGIFY(x)
constexpr int DEPTH_INCREMENT = 2;
#define Arasan_Version MAKE_STR(ARASAN_VERSION)
#define Arasan_Copyright "Copyright 1994-2026 by Jon Dart. All Rights Reserved."

#endif
