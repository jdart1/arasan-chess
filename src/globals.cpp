 // Copyright 1996-2012, 2014, 2016-2019, 2021 by Jon Dart.  All Rights Reserved.

#include "globals.h"
#include "hash.h"
#include "scoring.h"
#include "search.h"
#ifdef SYZYGY_TBS
#include "syzygy.h"
#endif
#include "bitbase.cpp"

#ifdef SYZYGY_TBS
static bool tb_init = false;

bool globals::tb_init_done()
{
   return tb_init;
}
int globals::EGTBMenCount = 0;
#endif

#ifdef _WIN32
static constexpr char PATH_CHAR = '\\';
#else
static constexpr char PATH_CHAR = '/';
#endif

MoveArray *globals::gameMoves;
Options globals::options;
BookReader globals::openingBook;
Log *globals::theLog = nullptr;
string globals::learnFileName;
LockDefine(globals::input_lock);
#ifdef SYZYGY_TBS
LockDefine(globals::syzygy_lock);
#endif
bool globals::polling_terminated;
ThreadControl globals::inputSem;
#ifdef TUNE
Tune globals::tune_params;
#endif
#ifdef NNUE
nnue::Network globals::network;
bool globals::nnueInitDone = false;
#endif

static const char * LEARN_FILE_NAME = "arasan.lrn";

static const char * DEFAULT_BOOK_NAME = "book.bin";

static const char * RC_FILE_NAME = "arasan.rc";

const size_t globals::LINUX_STACK_SIZE = 8*1024*1024;

static string programPath;

#ifdef NNUE

// TBD: add version/hash?
const char * globals::DEFAULT_NETWORK_NAME = "arasan.nnue";

static bool absolutePath(const std::string &fileName) {
#ifdef _WIN32
    auto pos = fileName.find(':');
    if (pos == std::string::npos) {
        pos = 0;
    } else if (pos+1 < fileName.size()) {
        ++pos;
    }
#else
    size_t pos = 0;
#endif
    return pos >= fileName.size() || fileName[pos] == PATH_CHAR;
}
#endif

string globals::derivePath(const std::string &fileName) {
   return derivePath(programPath,fileName);
}

string globals::derivePath(const std::string &base, const std::string &fileName) {
    std::string result(base);
    size_t pos;
    pos = result.rfind(PATH_CHAR,std::string::npos);
    if (pos == std::string::npos) {
      return fileName;
    }
    else {
      return result.substr(0,pos+1) + fileName;
    }
}

int globals::initGlobals(const char *pathName, bool initLog) {
   programPath = pathName;
   gameMoves = new MoveArray();
   polling_terminated = false;
   if (initLog) {
       theLog = new Log();
       theLog->clear();
       theLog->write_header();
   }
   LockInit(input_lock);
#ifdef SYZYGY_TBS
   LockInit(syzygy_lock);
#endif
#ifdef NNUE
   if (options.search.nnueFile == "") {
       // set default network path
       options.search.nnueFile = derivePath(DEFAULT_NETWORK_NAME);
   }
#endif
return 1;
}

#ifdef NNUE
int globals::loadNetwork(const std::string &fname) {
   std::ifstream in(fname,ios_base::in | ios_base::binary);
   in >> network;
   if (!in.good()) {
       return 0;
   }
   return 1;
}
#endif

void CDECL globals::cleanupGlobals(void) {
   openingBook.close();
   delete gameMoves;
   delete theLog;
   LockFree(input_lock);
#ifdef SYZYGY_TBS
   LockFree(syzygy_lock);
#endif
   Scoring::cleanup();
   Bitboard::cleanup();
   Board::cleanup();
}

void globals::initOptions(const char *pathName) {
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

void globals::delayedInit() {
#ifdef SYZYGY_TBS
    if (options.search.use_tablebases && !globals::tb_init_done()) {
       EGTBMenCount = 0;
       string path;
       if (options.search.syzygy_path == "") {
          options.search.syzygy_path=derivePath("syzygy");
       }
       path = options.search.syzygy_path;
       EGTBMenCount = SyzygyTb::initTB(options.search.syzygy_path);
       tb_init = true;
       if (EGTBMenCount) {
          stringstream msg;
          msg << "found " << EGTBMenCount << "-man Syzygy tablebases in directory " << path << endl;
          cerr << msg.str();
       }
    }
#endif
#ifdef NNUE
    if (options.search.useNNUE && !nnueInitDone) {
        const std::string &nnuePath = options.search.nnueFile;
        nnueInitDone = loadNetwork(absolutePath(nnuePath) ?
                                   nnuePath.c_str() :
                                   derivePath(nnuePath));
        if (nnueInitDone) {
            std::cerr << "loaded network from file ";
        } else {
            std::cerr << "warning: failed to load network file ";
        }
        std::cerr << nnuePath << std::endl;
    }
#endif
    // also initialize the book here
    if (options.book.book_enabled && !openingBook.is_open()) {
        openingBook.open(derivePath(DEFAULT_BOOK_NAME).c_str());
    }
}

void globals::unloadTb() {
#ifdef SYZYGY_TBS
   if (tb_init_done()) {
      // Note: Syzygy code will free any existing memory if
      // initialized more than once. So no need to do anything here.
      tb_init = false;
   }
#endif
}

