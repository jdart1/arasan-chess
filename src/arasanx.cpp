// Main module for Arasan chess engine.
// Copyright 1997-2018, 2020-2021, 2024 by Jon Dart. All Rights Reserved.
//

#include "types.h"
#include "bench.h"
#include "globals.h"
#include "options.h"
#include "protocol.h"
#include "threadc.h"
#ifdef UNIT_TESTS
#include "unit.h"
#endif

#include <iostream>
#include <fstream>

extern "C"
{
#include <signal.h>
#ifndef _WIN32
#include <time.h>
#endif
}

int CDECL main(int argc, char **argv) {
    signal(SIGINT,SIG_IGN);

    // Show a message on the console
    std::cout << "Arasan " Arasan_Version << ' ' << Arasan_Copyright << std::endl;
    // Must use unbuffered console
    setbuf(stdin,NULL);
    setbuf(stdout, NULL);
    std::cout.rdbuf()->pubsetbuf(NULL, 0);
    std::cin.rdbuf()->pubsetbuf(NULL, 0);

    char *rcName = nullptr;
    bool ics = false, trace = false, cpusSet = false, memorySet = false, autoLoadRC = false;
    int arg = 1;

    while (arg < argc && *(argv[arg]) == '-') {
        char c = *(argv[arg] + 1);
        switch (c) {
        case 'a':
            autoLoadRC = true;
            break;
        case 'c':
            if (arg >= argc) {
                std::cerr << "expected number after -c" << std::endl;
                return -1;
            }
            ++arg;
            globals::options.search.ncpus = std::min<int>(Constants::MaxCPUs, atol(argv[arg]));
            cpusSet = true;
            if (globals::options.search.ncpus <= 0) {
                std::cerr << "-c parameter must be >=1" << std::endl;
                return -1;
            }
            break;
        case 'i':
            if (strcmp(argv[arg] + 1, "ics") == 0)
                ics = true;
            else {
                std::cerr << "Warning: unknown option: " << argv[arg] + 1 << std::endl;
            }
            break;
        case 'H':
            ++arg;
            Options::setMemoryOption(globals::options.search.hash_table_size, argv[arg]);
            memorySet = true;
            break;
        case 'r':
            ++arg;
            if (arg >= argc) {
                std::cerr << "expected filename after -r" << std::endl;
                return -1;
            } else {
                rcName = argv[arg];
            }
            break;
        case 't':
            if (strcmp(argv[arg], "-t") == 0) {
                trace = true;
            }
            break;
        default:
            std::cerr << "Warning: unknown option: " << argv[arg] + 1 << std::endl;
            break;
        }
        ++arg;
    }

    if (autoLoadRC && rcName != nullptr) {
        std::cerr << "-a and -r options are incompatible.";
        return -1;
    }

    Bitboard::init();
    Board::init();
    // do not auto load the .rc file
    if (!globals::initOptions(autoLoadRC, rcName, memorySet, cpusSet)) return -1;
    Attacks::init();
    Search::init();
    if (!globals::initGlobals()) {
        globals::cleanupGlobals();
        exit(-1);
    }
    atexit(globals::cleanupGlobals);

    if (arg < argc) {
        if (strcmp(argv[arg],"bench") == 0) {
            Bench b;
            globals::delayedInit();
            Bench::Results res = b.bench();
            std::cout << res;
            return 0;
        } else {
            std::cerr << "unrecognized text on command line: " << argv[arg] << std::endl;
            return -1;
        }
    }

    Board board;
    Protocol *p = new Protocol(board,trace,ics,cpusSet,memorySet);

#ifdef UNIT_TESTS
    globals::delayedInit(); // ensure all init is done including TBs, network
    int errs = doUnit();
    std::cout << "Unit tests ran: " << errs << " error(s)" << std::endl;
#endif

    p->poll(globals::polling_terminated);

    delete p;
    return 0;
}
