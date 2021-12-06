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
#ifdef NNUE
#include "nnue/nnue.h"
#endif

#include <string>
#include <mutex>

namespace globals {

extern Options options;
extern MoveArray *gameMoves;

extern std::string learnFileName;
extern int EGTBMenCount;
extern BookReader openingBook;
extern Log *theLog;

extern std::mutex input_lock;

#ifdef SYZYGY_TBS
extern std::mutex syzygy_lock;
#endif
#ifdef TUNE
extern Tune tune_params;
#endif
#ifdef NNUE
extern nnue::Network network;
extern bool nnueInitDone;
#endif
extern bool polling_terminated;
extern std::string debugPrefix;

// KPK bitbases:
extern CACHE_ALIGN const uint8_t baseKPKW[24576];
extern CACHE_ALIGN const uint8_t baseKPKB[24576];

#include <iostream>
#ifdef _THREAD_TRACE
#include <cstdio>
#endif

extern bool tb_init_done();

extern const char *DEFAULT_NETWORK_NAME;

extern const size_t LINUX_STACK_SIZE;

extern std::string derivePath(const std::string &fileName);
extern std::string derivePath(const std::string &base, const std::string &fileName);

extern int loadNetwork(const std::string &filename);

extern int initGlobals(bool initLog = true);

extern void CDECL cleanupGlobals(void);

extern void initOptions();

// Perform actions that need to be done before a game but after
// program startup (chiefly ones that depend on runtime options
// being set).
extern void delayedInit(bool verbose = true);

// Attempt to unload the tablebases (if in use),
extern void unloadTb();

}

#endif
