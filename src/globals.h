// Copyright 1994-2005, 2014-2016, 2021, 2024 by Jon Dart.  All Rights Reserved.

#ifndef _GLOBALS_H
#define _GLOBALS_H

// global variables

#include "bookread.h"
#include "movearr.h"
#include "options.h"
#include "threadc.h"
#include "types.h"
#include "nnue/nnue.h"

#include <fstream>
#include <mutex>
#include <string>

namespace globals {

extern Options options;

extern MoveArray *gameMoves;

extern int EGTBMenCount;

extern BookReader openingBook;

extern std::ofstream game_file;

extern std::mutex input_lock;

//#ifdef SYZYGY_TBS
//extern std::mutex syzygy_lock;
//#endif
extern nnue::Network network;
extern bool nnueInitDone;
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

extern bool loadNetwork(const std::string &filename, bool verbose);

extern bool initGlobals();

extern void initGameFile();

extern void CDECL cleanupGlobals(void);

// If autoLoadRC is true, read the .rc file from the default path(s).
// If autoLoadRC is false and a non-null rcPath is specified, load the .rc
// file from that path.
extern bool initOptions(bool autoLoadRC = true, const char *rcPath = nullptr, bool memorySet = false,
                        bool cpusSet = false);

// Perform actions that need to be done before a game but after
// program startup (chiefly ones that depend on runtime options
// being set).
extern void delayedInit(bool verbose = true);

// Attempt to unload the tablebases (if in use),
extern void unloadTb();

} // namespace globals

#endif
