// Main module for Arasan chess engine.
// Copyright 1997-2018, 2020-2022 by Jon Dart. All Rights Reserved.
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
#include <sys/resource.h>
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

    Bitboard::init();
    Board::init();
    globals::initOptions();
    Attacks::init();
    Scoring::init();
    Search::init();
    if (!globals::initGlobals(true)) {
        globals::cleanupGlobals();
        exit(-1);
    }
    atexit(globals::cleanupGlobals);

#ifndef _WIN32
    struct rlimit rl;
    const rlim_t STACK_MAX = static_cast<rlim_t>(globals::LINUX_STACK_SIZE);
    auto result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < STACK_MAX)
        {
            rl.rlim_cur = STACK_MAX;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result)
            {
                std::cerr << "failed to increase stack size" << std::endl;
                exit(-1);
            }
        }
    }
#endif

    Board board;
    int arg = 1;

    bool ics = false, trace = false, cpusSet = false, memorySet = false;

    if (argc > 1) {
        while (arg < argc && *(argv[arg]) == '-') {
            char c = *(argv[arg]+1);
            switch (c) {
            case 'c':
                ++arg;
                globals::options.search.ncpus = std::min<int>(Constants::MaxCPUs,atol(argv[arg]));
                cpusSet = true;
                if (globals::options.search.ncpus<=0) {
                    std::cerr << "-c parameter must be >=1" << std::endl;
                    exit(-1);
                }
                break;
            case 'f':
            {
                ++arg;
                std::cout << "loading " << argv[arg] << std::endl;
                std::ifstream pos_file( argv[arg], std::ios::in);
                if (pos_file.good()) {
                    pos_file >> board;
                }
                else {
                    std::cerr << "file not found: " << argv[arg] << std::endl;
                    return -1;
                }
                break;
            }
            case 'i':
                if (strcmp(argv[arg]+1,"ics")==0)
                    ics = true;
                else {
                    std::cerr << "Warning: unknown option: " << argv[arg]+1 << std::endl;
                }
                break;
            case 'H':
                ++arg;
                Options::setMemoryOption(globals::options.search.hash_table_size,
                                         argv[arg]);
                memorySet = true;
                break;
            case 't':
                trace = true;
                break;
            default:
                std::cerr << "Warning: unknown option: " << argv[arg]+1 <<
                    std::endl;
                break;
            }
            ++arg;
        }
    }
    if (arg < argc) {
        if (strcmp(argv[arg],"bench") == 0) {
            Bench b;
            Bench::Results res = b.bench(true);
            std::cout << res;
            return 0;
        } else {
            std::cerr << "unrecognized text on command line: " << argv[arg] << std::endl;
            return -1;
        }
    }

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
