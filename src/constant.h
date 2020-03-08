// Copyright 1994-2018 by Jon Dart.  All Rights Reserved.

#ifndef _CONSTANT_H
#define _CONSTANT_H

class Constants
{

public:	

enum {MaxPly = 62};
enum {MATE = 32000 };
enum {MATE_RANGE = MATE-256 };
enum {TABLEBASE_WIN = MATE_RANGE-256};
enum {BITBASE_WIN = MATE_RANGE-1000};
enum {INVALID_SCORE = -MATE-1};
enum {MaxMoves = 220};
enum {MaxCaptures = 40};
enum {MaxChecks = 40};
enum {MaxCPUs = 256};

};

const int INFINITE_TIME = 9999999;

#define STRINGIFY(x) #x
#define MAKE_STR(x) STRINGIFY(x)
constexpr int DEPTH_INCREMENT = 1;
#define Arasan_Version MAKE_STR(ARASAN_VERSION)
#define Arasan_Copyright "Copyright 1994-2020 by Jon Dart. All Rights Reserved."

#endif
