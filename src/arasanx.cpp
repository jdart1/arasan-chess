// Main module for Arasan chess engine.
// Copyright 1997-2018, 2020-2021 by Jon Dart. All Rights Reserved.
//

#include "types.h"
#include "bench.h"
#include "debug.h"
#include "globals.h"
#include "options.h"
#include "protocol.h"
#include "threadc.h"
#ifdef UNIT_TESTS
#include "unit.h"
#endif

#include <cctype>
#include <iostream>
#include <memory>

extern "C"
{
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#include <sys/select.h>
#endif
}
using namespace std;

static THREAD pollingThreadHandle;
static string cmd_buf;

static void processCmdChars(Protocol *p,char *buf,int len) {
    // try to parse the buffer into command lines
    for (int i = 0; i < len; i++) {
        char c = buf[i];
        if (c == '\r' || c == '\n') {
            // handle CR + LF if present
            if (i+1 < len && (buf[i+1] == '\r' || buf[i+1] == '\n')) i++;
            if (cmd_buf.length()) {
                p->dispatchCmd(cmd_buf);
                cmd_buf.clear();
            }
        } else {
            cmd_buf += c;
        }
    }
    Lock(input_lock);
    if (p->hasPending() && !p->isSearching()) {
        inputSem.signal();
    }
    Unlock(input_lock);
}

#ifdef _WIN32

static DWORD WINAPI inputPoll(void *x) {
   HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
   Protocol *p = static_cast<Protocol *>(x);
   if (p->traceOn()) cout << "# starting poll thread" << endl;
   char buf[1024];
   while (!polling_terminated) {
      BOOL bSuccess;
      DWORD dwRead;
      if (_isatty(_fileno(stdin))) {
         // we are reading direct from the console, enable echo & control-char
         // processing
         if (!SetConsoleMode(hStdin, ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT |
                         ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT)) {
            cerr << "SetConsoleMode failed" << endl;
         }
         bSuccess = ReadConsole(hStdin, buf, 1024, &dwRead, NULL);
      }
      else {
         bSuccess = ReadFile(hStdin, buf, 1024, &dwRead, NULL);
         if (! bSuccess || dwRead == 0) {
            if (p->traceOn()) cout << "# read error from input pipe" << endl;
            break;
	 }
      }
      processCmdChars(p,buf,(int)dwRead);
   }
   if (p->traceOn()) cout << "input polling thread terminated" << endl;
   return 0;
}

#else

static void * CDECL inputPoll(void *x) {
    Protocol *p = static_cast<Protocol*>(x);
    if (p->traceOn()) cout << "# starting poll thread" << endl;
    static const int INPUT_BUF_SIZE = 1024;
    char buf[INPUT_BUF_SIZE];
    while (!polling_terminated) {
        fd_set readfds;
        struct timeval tv;
        int data;

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        // set a timeout so we can interrupt the polling thread
        // with no input.
        tv.tv_sec=2;
        tv.tv_usec=0;
        int ret = select(16, &readfds, 0, 0, &tv);
        if (ret == 0) {
            // no data available
            continue;
        } else if (ret == -1) {
            perror("select");
            continue;
        }
        data=FD_ISSET(STDIN_FILENO, &readfds);
        if (data>0) {
            // we have something to read
            int bytes = read(STDIN_FILENO,buf,INPUT_BUF_SIZE);
            if (bytes == -1) {
                perror("input poll: read");
                continue;
            }
            if (bytes) {
                processCmdChars(p,buf,bytes);
            }
        }
    }
    if (p->traceOn()) {
        cout << "input polling thread terminated" << endl;
    }
    return nullptr;
}

#endif

int CDECL main(int argc, char **argv) {
    signal(SIGINT,SIG_IGN);

    // Show a message on the console
    cout << "Arasan " Arasan_Version << ' ' << Arasan_Copyright << endl;
    // Must use unbuffered console
    setbuf(stdin,NULL);
    setbuf(stdout, NULL);
    std::cout.rdbuf()->pubsetbuf(NULL, 0);
    std::cin.rdbuf()->pubsetbuf(NULL, 0);

    Bitboard::init();
    Board::init();
    initOptions(argv[0]);
    Attacks::init();
    Scoring::init();
    Search::init();
    if (!initGlobals(argv[0], true)) {
        cleanupGlobals();
        exit(-1);
    }
    atexit(cleanupGlobals);

    Board board;
    int arg = 1;

    bool ics = false, trace = false, cpusSet = false, memorySet = false;

    if (argc > 1) {
        while (arg < argc && *(argv[arg]) == '-') {
            char c = *(argv[arg]+1);
            switch (c) {
            case 'c':
                ++arg;
                options.search.ncpus = std::min<int>(Constants::MaxCPUs,atol(argv[arg]));
                cpusSet = true;
                if (options.search.ncpus<=0) {
                    cerr << "-c parameter must be >=1" << endl;
                    exit(-1);
                }
                break;
            case 'f':
            {
                ++arg;
                cout << "loading " << argv[arg] << endl;
                ifstream pos_file( argv[arg], ios::in);
                if (pos_file.good()) {
                    pos_file >> board;
                }
                else {
                    cerr << "file not found: " << argv[arg] << endl;
                    return -1;
                }
                break;
            }
            case 'i':
                if (strcmp(argv[arg]+1,"ics")==0)
                    ics = true;
                else {
                    cerr << "Warning: unknown option: " << argv[arg]+1 << endl;
                }
                break;
            case 'H':
                ++arg;
                Options::setMemoryOption(options.search.hash_table_size,
                                         string(argv[arg]));
                memorySet = true;
                break;
            case 't':
                trace = true;
                break;
            default:
                cerr << "Warning: unknown option: " << argv[arg]+1 <<
                    endl;
                break;
            }
            ++arg;
        }
    }
    if (arg < argc) {
        if (strcmp(argv[arg],"bench") == 0) {
            Bench b;
            Bench::Results res = b.bench(true);
            cout << res;
            return 0;
        } else {
            cerr << "unrecognized text on command line: " << argv[arg] << endl;
            return -1;
        }
    }

    Protocol *p = new Protocol(board,trace,ics,cpusSet,memorySet);

#ifdef _WIN32
    // setup polling thread for input from engine
    polling_terminated = false;
    DWORD id;
    pollingThreadHandle = CreateThread(NULL,0,
                                       inputPoll,p,
                                       0,
                                       &id);
    if (pollingThreadHandle == NULL) {
        int nError = GetLastError();
        LPSTR errMsg = NULL;

        int nChars = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            nError,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  // Default language
            errMsg,
            512,
            NULL
            );
        if (nChars) {
            printf("Input thread failed to start: %s\n", errMsg);
        }
        else {
            printf("Input thread failed to start, error code %d\n", nError);
        }
        LocalFree(errMsg);
        exit(-1);
    }
#else
    if (pthread_create(&pollingThreadHandle, NULL, inputPoll, p)) {
        perror("input thread creation failed");
        exit(-1);
    }
#endif

#ifdef UNIT_TESTS
    int errs = doUnit();
    cout << "Unit tests ran: " << errs << " error(s)" << endl;
#endif

    p->poll(polling_terminated);

#ifdef _WIN32
    TerminateThread(pollingThreadHandle,0);
#else
    polling_terminated = true;
    void *value_ptr;
    pthread_join(pollingThreadHandle,&value_ptr);
#endif
    delete p;
    return 0;
}
