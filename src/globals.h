// Copyright 1994-2005 by Jon Dart.  All Rights Reserved.

#ifndef _GLOBALS_H
#define _GLOBALS_H

// global variables

#include "types.h"
#include "options.h"
#include "log.h"
#include "book.h"
#include "movearr.h"

extern Options options;
extern MoveArray *gameMoves;

extern string learnFileName;
extern string programPath;
extern int EGTBMenCount;
extern Book *openingBook;
extern Log *theLog;
extern lock_t input_lock;

// KPK bitbases:
extern CACHE_ALIGN const byte baseKPKW[24576];
extern CACHE_ALIGN const byte baseKPKB[24576];

#include <iostream>
#ifdef _THREAD_TRACE
#include <stdio.h>
#endif

extern string derivePath(const char *fileName);
extern string derivePath(const char *base, const char *fileName);

extern int initGlobals(const char *pathName, bool initLog = true);

extern void CDECL cleanupGlobals(void);

extern void initOptions(const char *pathName);

// Perform actions that need to be done before a game but after
// program startup (chiefly ones that depend on runtime options
// being set)
extern void delayedInit();

#endif
