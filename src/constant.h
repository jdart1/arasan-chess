// Copyright 1994-2017 by Jon Dart.  All Rights Reserved.

#ifndef _CONSTANT_H
#define _CONSTANT_H

class Constants
{

public:	

enum {MaxPly = 62};
enum {MATE = 32767 };
enum {MATE_RANGE = MATE-256 };
enum {TABLEBASE_WIN = MATE_RANGE-1};
enum {BITBASE_WIN = MATE_RANGE-1000};
enum {MaxMoves = 220};
enum {MaxCPUs = 64};		

};

const int INFINITE_TIME = 9999999;

#define STRINGIFY(x) #x
#define MAKE_STR(x) STRINGIFY(x)
#define DEPTH_INCREMENT 4
#define Arasan_Version MAKE_STR(ARASAN_VERSION)
#define Arasan_Copyright "Copyright 1994-2017 by Jon Dart. All Rights Reserved."

#endif
