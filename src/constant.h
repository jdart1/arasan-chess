// Copyright 1994 by Jon Dart.  All Rights Reserved.

#ifndef _CONSTANT_H
#define _CONSTANT_H

class Constants
{

public:	

enum {MaxPly = 62};
enum {MATE = 131072 };
enum {MATE_RANGE = MATE-256 };
enum {TABLEBASE_WIN = MATE_RANGE-1};
enum {MaxMoves = 220};
enum {MaxCPUs = 64};		

};

const int INFINITE_TIME = 9999999;

#define DEPTH_INCREMENT 4
#define Arasan_Version "19.2"
#define Arasan_Copyright "Copyright 1994-2016 by Jon Dart. All Rights Reserved."

#endif
