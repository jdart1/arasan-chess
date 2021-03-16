// Copyright 1994-2005, 2014-2016, 2021 by Jon Dart.  All Rights Reserved.

#ifndef _GLOBALS_H
#define _GLOBALS_H

// global variables

#include "types.h"
#include "options.h"
#include "log.h"
#include "bookread.h"
#include "movearr.h"
#include "threadc.h"
#ifdef TUNE
#include "tune.h"
#endif

extern Options options;
extern MoveArray *gameMoves;

extern string learnFileName;
extern string programPath;
extern int EGTBMenCount;
extern BookReader openingBook;
extern Log *theLog;
extern lock_t input_lock;
#ifdef SYZYGY_TBS
extern lock_t syzygy_lock;
#endif
#ifdef TUNE
extern Tune tune_params;
#endif
extern bool polling_terminated;
extern ThreadControl inputSem;

// KPK bitbases:
extern CACHE_ALIGN const uint8_t baseKPKW[24576];
extern CACHE_ALIGN const uint8_t baseKPKB[24576];

#include <iostream>
#ifdef _THREAD_TRACE
#include <stdio.h>
#endif

extern bool tb_init_done(const Options::TbType);

extern string derivePath(const char *fileName);
extern string derivePath(const char *base, const char *fileName);

extern int initGlobals(const char *pathName, bool initLog = true);

extern void CDECL cleanupGlobals(void);

extern void initOptions(const char *pathName);

// Perform actions that need to be done before a game but after
// program startup (chiefly ones that depend on runtime options
// being set).
extern void delayedInit();

// Attempt to unload the tablebases (if in use),
extern void unloadTb();

#endif
