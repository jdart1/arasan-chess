// Copyright 1996-2012, 2014 by Jon Dart.  All Rights Reserved.

#include "globals.h"
#include "hash.h"
#include "bitprobe.h"
#include "scoring.h"
#include "bitbase.cpp"
#ifdef GAVIOTA_TBS
#include "gtb.h"
#endif
#ifdef NALIMOV_TBS
#include "nalimov.h"
#endif
#ifdef SYZYGY_TBS
#include "syzygy.h"
#endif

int EGTBMenCount = 0;
MoveArray *gameMoves;
Options options;
BookReader openingBook;
Log *theLog = NULL;
string learnFileName;
LockDefine(input_lock);
#ifdef TUNE
Tune tune_params;
#endif

static const char * LEARN_FILE_NAME = "arasan.lrn";

static const char * DEFAULT_BOOK_NAME = "book.bin";

#ifdef UCI_LOG
fstream ucilog;
#endif

string programPath;

string derivePath(const char *fileName) {
   return derivePath(programPath.c_str(),fileName);
}

string derivePath(const char *base, const char *fileName) {
    string result(base);
    size_t pos;
#ifdef _WIN32
    pos = result.rfind('\\',string::npos);
#else
    pos = result.rfind('/',string::npos);
#endif
    if (pos == string::npos) {
      return fileName;
    }
    else {
      return result.substr(0,pos+1) + fileName;
    }
}

int initGlobals(const char *pathName, bool initLog) {
   programPath = pathName;
   gameMoves = new MoveArray();
   if (initLog) {
       theLog = new Log();
       theLog->clear();
       theLog->write_header();
   }
   LockInit(input_lock);
#ifdef UCI_LOG
    ucilog.open(derivePath("ucilog").c_str(),ios::out|ios::app);
    ucilog << "starting up" << endl;
#endif
   return 1;
}

void CDECL cleanupGlobals(void) {
   openingBook.close();
   delete gameMoves;
   delete theLog;
   LockDestroy(input_lock);
   Scoring::cleanup();
#ifdef UCI_LOG
    ucilog.close();
#endif
#ifdef GAVIOTA_TBS
   GaviotaTb::freeTB();
#endif   
}

void initOptions(const char *pathName) {
    programPath = pathName;
    string rcPath = derivePath("arasan.rc");
    // try to read arasan.rc file
    options.init(rcPath);
#ifndef _WIN32
    // Also read .rc from the user's HOME,
    // if there is one.
    if (getenv("HOME")) {
       rcPath = derivePath(getenv("HOME"),"arasan.rc");
       options.init(rcPath);       
    }
#endif
    learnFileName = derivePath(LEARN_FILE_NAME);
}

void delayedInit() {
#if defined(NALIMOV_TBS) || defined(GAVIOTA_TBS) || defined(SYZYGY_TBS)
    if (options.search.use_tablebases) {
#ifdef GAVIOTA_TBS
        if (options.search.tablebase_type == Options::GAVIOTA_TYPE) {
            // If no tablebase directory was specified in the .rc file,
            // try to find the table bases under the arasan install dir
            if (options.search.gtb_path == "") {
                options.search.gtb_path=derivePath("gtb");
            }
            EGTBMenCount = GaviotaTb::initTB(options.search.gtb_path,
                              options.search.gtb_scheme,
                              options.search.gtb_cache_size);
        }
#endif
#ifdef NALIMOV_TBS
        if (options.search.tablebase_type == Options::NALIMOV_TYPE) {
            // If no tablebase directory was specified in the .rc file,
            // try to find the table bases under the arasan install dir
            if (options.search.nalimov_path == "") {
                options.search.nalimov_path=derivePath("TB");
            }
            EGTBMenCount = NalimovTb::initTB(
                             (char*)options.search.nalimov_path.c_str(),
                             options.search.nalimov_cache_size);
        }
#endif
#ifdef SYZYGY_TBS
        if (options.search.tablebase_type == Options::SYZYGY_TYPE) {
           if (options.search.syzygy_path == "") {
              options.search.syzygy_path=derivePath("syzygy");
           }
           EGTBMenCount = SyzygyTb::initTB(options.search.syzygy_path);
        }
#endif
    }
#endif
    // also initialize the book here
    if (options.book.book_enabled) {
        openingBook.open(derivePath(DEFAULT_BOOK_NAME).c_str());
    }
}
