// Copyright 1994 by Jon Dart.  All Rights Reserved.

#ifndef _CONSTANT_H
#define _CONSTANT_H

class Constants
{

public:	

enum {MaxPly = 64};
enum {MATE = 10000 };
enum {MATE_RANGE = 10000-256 };
enum {MaxMoves = 200};		
enum {MaxCPUs = 64};		
enum {HISTORY_MAX =32768};

};

const int INFINITE_TIME = 9999999;

#define DEPTH_INCREMENT 4
#define Arasan_Version "16.0"
#define Arasan_Copyright "Copyright 1994-2013 by Jon Dart. All Rights Reserved."

#endif
