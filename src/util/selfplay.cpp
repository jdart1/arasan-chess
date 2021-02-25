// Copyright 2021 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "boardio.h"
#include "globals.h"
#include "eco.h"
#include "chessio.h"
#include "notation.h"
#include "search.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <mutex>
#include <random>
#include <string>
#include <vector>
#ifndef _MSC_VER
extern "C" {
    #include <time.h>
    #include <unistd.h>
}
#endif

static constexpr int THREAD_STACK_SIZE = 8*1024*1024;

#ifndef _WIN32
static pthread_attr_t stackSizeAttrib;
#endif

static ECO ecoCoder;

static const char *RESULT_TAG = "c1";

class SelfPlayHashTable {

public:

  SelfPlayHashTable() : used_hashes(new HashArray()) {
    init_hash();
  }

  virtual ~SelfPlayHashTable() {
    delete used_hashes;
  }

  bool check_and_replace_hash(hash_t new_hash) const noexcept {
    size_t index = new_hash & (HASH_TABLE_FOR_UNIQUENESS_SIZE - 1);
    hash_t &h = (*used_hashes)[index];
    std::unique_lock<std::mutex> lock(mtx);
    if (h == new_hash) {
      return true;
    }
    else {
      h = new_hash;
      return false;
    }
  }

private:  
  static constexpr size_t HASH_TABLE_FOR_UNIQUENESS_SIZE = 64*1024*1024; // entries

  using HashArray = std::array<hash_t,HASH_TABLE_FOR_UNIQUENESS_SIZE>;

  HashArray *used_hashes;

  std::mutex mtx;

  void init_hash() {
     for (hash_t &h : *used_hashes) h = 0ULL;
  }

} sp_hash_table;

LockDefine(outputLock);

static std::ofstream *game_out_file = nullptr, *pos_out_file = nullptr;

struct SelfPlayOptions 
{
    unsigned minOutPly = 8;
    unsigned maxOutPly = 400;
    unsigned cores = 1;
    unsigned gameCount = 1000000000;
    unsigned depthLimit = 9;
    bool adjudicateDraw = true;
    unsigned drawAdjudicationMoves = 5;
    unsigned drawAdjudicationMinPly = 100;
    std::string posFileName = "positions.epd";
    std::string gameFileName = "games.pgn";
    bool saveGames = false;
    unsigned maxBookPly = 8;
    unsigned sampleInterval = 16;
    unsigned maxSamplePerPhase = 4;
} sp_options;

struct ThreadData 
{
    unsigned index = 0;
    std::thread thread;
    SearchController *searcher = nullptr;
} threadDatas[Constants::MaxCPUs];

static void saveGame(const string &result, ofstream &file) 
{
    std::vector<ChessIO::Header> headers;
    headers.push_back(ChessIO::Header("Event","?"));
    char hostname[256];
#ifdef _MSC_VER
    DWORD size = 256;
    GetComputerName(hostname,&size);
    hostname[size] = '\0';
#else
    gethostname(hostname,256);
#endif    
    if (*hostname) {
        headers.push_back(ChessIO::Header("Site",hostname));
    } else {
        headers.push_back(ChessIO::Header("Site","?"));
    }
    char dateStr[15];
    time_t tm = time(NULL);
    struct tm *t = localtime(&tm);
    sprintf(dateStr,"%4d.%02d.%02d",t->tm_year+1900,t->tm_mon+1,t->tm_mday);
    headers.push_back(ChessIO::Header("Date",dateStr));
    headers.push_back(ChessIO::Header("Round","?"));
    std::stringstream s;
    s << "Arasan " << Arasan_Version;
    headers.push_back(ChessIO::Header("White",s.str()));
    headers.push_back(ChessIO::Header("Black",s.str()));
    std::string opening_name, eco;
    ecoCoder.classify(*gameMoves,eco,opening_name);
    headers.push_back(ChessIO::Header("ECO",eco));
    headers.push_back(ChessIO::Header("Result",result));
    Lock(outputLock);
    ChessIO::store_pgn(file, *gameMoves,
                       result,
                       headers);
    Unlock(outputLock);
}

static void selfplay(SearchController *searcher) 
{
    for (unsigned i = 0; i < sp_options.gameCount; i++) {
        if (sp_options.saveGames) {
            gameMoves->removeAll();
        }
        bool adjudicated = false, terminated = false;
        Statistics stats;
        Board board;
        std::vector<score_t> scores;
        unsigned ply = 0;
        enum class Result {WhiteWin, BlackWin, Draw} result;
        std::vector<std::string> fens;
        for (;!adjudicated && !terminated;++ply) {
            stats.clear();
            Move m = NullMove;
            if (ply < sp_options.maxBookPly) {
                m = openingBook.pick(board);
            }
            if (IsNull(m)) {
                m = searcher->findBestMove(board,
                                           FixedDepth,
                                           INFINITE_TIME,
                                           0, // extra time
                                           sp_options.depthLimit,
                                           false, // background
                                           false, // uci
                                           stats,
                                           TalkLevel::Silent);
            }
            scores.push_back(stats.display_value);
            if (stats.state == Resigns || stats.state == Checkmate) {
                result = board.sideToMove() == White ? Result::BlackWin : Result::WhiteWin;
                terminated = true;
            }
            if (stats.state == Draw || stats.state == Stalemate) {
                terminated = true;
                result = Result::Draw;
            }
            if (!terminated && sp_options.adjudicateDraw &&
                ply >= sp_options.drawAdjudicationMinPly &&
                scores.size() >= sp_options.drawAdjudicationMoves) {
                unsigned count = scores.size()-1;
                while (count) {
                    if (scores[i] != 0) break;
                    --count;
                }
                if (count == 0) {
                    // adjudicate draw
                    stats.state = Draw;
                    result = Result::Draw;
                    adjudicated = true;
                }
            }
            if (!IsNull(m)) {
              BoardState previous_state(board.state);
              board.doMove(m);
              if (!sp_hash_table.check_and_replace_hash(board.hashCode())) {
                  if (ply >= sp_options.minOutPly && ply <= sp_options.maxOutPly) {
                    std::stringstream s;
                    BoardIO::writeFEN(board,s,0);
                    fens.push_back(s.str());
                  }
                  if (sp_options.saveGames) {
                    string image;
                    Notation::image(board,m,Notation::OutputFormat::SAN,image);
                    gameMoves->add_move(board,previous_state,m,image,false);
                  }
                }
            } else {
                // unexpected null move
                if (!terminated && !adjudicated) break;
            }
        }
        if (sp_options.saveGames) {
            string resultStr;
            if (result == Result::WhiteWin)
                resultStr = "1-0";
            else if (result == Result::BlackWin)
                resultStr = "0-1";
            else
                resultStr = "1/2-1/2";
            saveGame(resultStr,*game_out_file);
        }
        for (const std::string &fen : fens) {
            string resultStr;
            if (result == Result::WhiteWin)
                resultStr = "1.0";
            else if (result == Result::BlackWin)
                resultStr = "0.0";
            else
                resultStr = "0.5";
            *pos_out_file << fen << ' ' << RESULT_TAG << " \"" << resultStr << "\";" << endl;
        }
    }
}

static void threadp(ThreadData *td)
{
   // set stack size for Linux/Mac
#ifdef _POSIX_VERSION
   size_t stackSize;
   if (pthread_attr_getstacksize(&stackSizeAttrib, &stackSize)) {
        perror("pthread_attr_getstacksize");
        return;
   }
   if (stackSize < THREAD_STACK_SIZE) {
      if (pthread_attr_setstacksize (&stackSizeAttrib, THREAD_STACK_SIZE)) {
         perror("error setting thread stack size");
         return;
      }
   }
#endif

   // allocate controller in the thread
   try {
      td->searcher = new SearchController();
   } catch(std::bad_alloc) {
      cerr << "out of memory, thread " << td->index << endl;
      return;
   }
   selfplay(td->searcher);
   delete td->searcher;
}

static void init_threads()
{
   // prepare threads
#ifdef _POSIX_VERSION
   if (pthread_attr_init (&stackSizeAttrib)) {
      perror("pthread_attr_init");
      return;
   }
#endif
   for (unsigned i = 0; i < sp_options.cores; i++) {
      threadDatas[i].index = i;
      threadDatas[i].searcher = nullptr;
   }
}

static void launch_threads()
{
   for (unsigned i = 0; i < sp_options.cores; i++) {
      threadDatas[i].index = i;
      threadDatas[i].thread = std::thread(threadp,&threadDatas[i]);
   }
   // wait for all searchers done
   for (unsigned i = 0; i < sp_options.cores; i++) {
      threadDatas[i].thread.join();
   }
}

// Generates labeled FEN positions for training from self-play games
int CDECL main(int argc, char **argv)
{
   Bitboard::init();
   initOptions(argv[0]);
   Attacks::init();
   Scoring::init();
   if (!initGlobals(argv[0], false)) {
      cleanupGlobals();
      exit(-1);
   }
   atexit(cleanupGlobals);
   delayedInit();
   if (EGTBMenCount) {
      cerr << "Initialized tablebases" << endl;
   }
   LockInit(outputLock);

   options.search.hash_table_size = 64*1024*1024;
   options.book.frequency = 10;
   options.book.weighting = 0;
   options.book.scoring = 0;

   int arg = 1;
   for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
      if (strcmp(argv[arg], "-c") == 0) {
          stringstream s(argv[++arg]);
          s >> sp_options.cores;
          if (s.bad()) {
              cerr << "error in core count after -c" << endl;
              return -1;
          }
          sp_options.cores = std::min<int>(Constants::MaxCPUs,sp_options.cores);
          
      }
      else if (strcmp(argv[arg], "-n") == 0) {
          stringstream s(argv[++arg]);
          s >> sp_options.gameCount;
          if (s.bad()) {
              cerr << "error in game count after -n" << endl;
              return -1;
          }
      }
   }

   pos_out_file = new std::ofstream(sp_options.posFileName,ios::out | ios::app);
   if (sp_options.saveGames)
       game_out_file = new std::ofstream(sp_options.gameFileName,ios::out | ios::app);

   init_threads();
   launch_threads();
   LockFree(outputLock);

   delete pos_out_file;
   delete game_out_file;
   
   return 0;
}

