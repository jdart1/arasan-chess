// Copyright 1996-2012, 2014, 2016 by Jon Dart.  All Rights Reserved.

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

#include <map>

#if !defined(_MSC_VER) || (_MSC_VER >= 1800)
static map<Options::TbType,bool> tb_init_map =
{{Options::TbType::None, false},
{Options::TbType::NalimovTb, false},
{Options::TbType::GaviotaTb, false},
{Options::TbType::SyzygyTb, false}};
#else
static map<Options::TbType,bool> tb_init_map;
#endif

bool tb_init_done(const Options::TbType type)
{
   auto done = tb_init_map.find(type);
   if (done == tb_init_map.end())
      return false;
   else
      return (*done).second;
}

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

static const char * RC_FILE_NAME = "arasan.rc";

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
#if defined(_MSC_VER) && (_MSC_VER <1800)
   tb_init_map[Options::TbType::None] =
       tb_init_map[Options::TbType::NalimovTb] =
       tb_init_map[Options::TbType::GaviotaTb] =
       tb_init_map[Options::TbType::SyzygyTb] = false;
#endif
   return 1;
}

void CDECL cleanupGlobals(void) {
   openingBook.close();
   delete gameMoves;
   delete theLog;
   LockFree(input_lock);
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
    string rcPath = derivePath(RC_FILE_NAME);
    // try to read arasan.rc file
    options.init(rcPath);
#ifndef _WIN32
    // Also read .rc from the user's HOME,
    // if there is one.
    if (getenv("HOME")) {
       rcPath = derivePath(getenv("HOME"),RC_FILE_NAME);
       options.init(rcPath);
    }
#endif
    learnFileName = derivePath(LEARN_FILE_NAME);
}

void delayedInit() {
#if defined(NALIMOV_TBS) || defined(GAVIOTA_TBS) || defined(SYZYGY_TBS)
    if (options.search.use_tablebases && !tb_init_done(options.search.tablebase_type)) {
       EGTBMenCount = 0;
       string path;
#ifdef GAVIOTA_TBS
       if (options.search.tablebase_type == Options::TbType::GaviotaTb) {
            // If no tablebase directory was specified in the .rc file,
            // try to find the table bases under the arasan install dir
            if (options.search.gtb_path == "") {
                options.search.gtb_path=derivePath("gtb");
            }
            path = options.search.gtb_path;
            EGTBMenCount = GaviotaTb::initTB(options.search.gtb_path,
                              options.search.gtb_scheme,
                              options.search.gtb_cache_size);
            tb_init_map[Options::TbType::GaviotaTb] = true;
       }
#endif
#ifdef NALIMOV_TBS
       if (options.search.tablebase_type == Options::TbType::NalimovTb) {
            // If no tablebase directory was specified in the .rc file,
            // try to find the table bases under the arasan install dir
            if (options.search.nalimov_path == "") {
                options.search.nalimov_path=derivePath("TB");
            }
            path = options.search.nalimov_path;
            EGTBMenCount = NalimovTb::initTB(
                             (char*)options.search.nalimov_path.c_str(),
                             options.search.nalimov_cache_size);
            tb_init_map[Options::TbType::NalimovTb] = true;
       }
#endif
#ifdef SYZYGY_TBS
       if (options.search.tablebase_type == Options::TbType::SyzygyTb) {
          if (options.search.syzygy_path == "") {
             options.search.syzygy_path=derivePath("syzygy");
          }
          path = options.search.syzygy_path;
          EGTBMenCount = SyzygyTb::initTB(options.search.syzygy_path);
          tb_init_map[Options::TbType::SyzygyTb] = true;
       }
#endif
       if (EGTBMenCount)
          cerr << "found " << EGTBMenCount << "-man " <<
             Options::tbTypeToString(options.search.tablebase_type) << " tablebases in directory " << path << endl;
    }
#endif
    // also initialize the book here
    if (options.book.book_enabled && !openingBook.is_open()) {
        openingBook.open(derivePath(DEFAULT_BOOK_NAME).c_str());
    }
}

void unloadTb(Options::TbType type)
{
   if (tb_init_done(type)) {
      // Note: Only Gaviota tb code has an explicit free command.
      // Nalimov and Syzygy will free any existing memory if
      // initialized more than once.
#ifdef GAVIOTA_TBS
      if (type == Options::TbType::GaviotaTb) {
         GaviotaTb::freeTB();
      }
#endif
      tb_init_map[type] = false;
   }
}

