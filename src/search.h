// Copyright 1994-2013 by Jon Dart.  All Rights Reserved.

#ifndef _SEARCH_H
#define _SEARCH_H

#include "board.h"
#include "arasset.h"
#include "stats.h"
#include "searchc.h"
#include "scoring.h"
#include "movegen.h"
#include "threadp.h"
#include "options.h"
#include <memory.h>
#include <time.h>
#include <set>
using namespace std;

class MoveGenerator;

struct NodeInfo;

// Definition of a split point
struct SplitPoint {
    ArasanSet<ThreadInfo *,Constants::MaxCPUs> slaves;
    Move moves[Constants::MaxMoves];
    int ply;
    int depth;
    // Thread that is master of the split point
    ThreadInfo *master;
    // board position that the master node had pre-split
    Board savedBoard;
    // node at which split occurred
    NodeInfo *splitNode;
    // move index at which split occurred
    int moveIndex;
    // parent of this split point. When all searchers at
    // this split point are finished, the master node
    // will revert to the parent split point.
    SplitPoint *parent;
    // Move Generator instance for split node
    MoveGenerator * mg;
    lock_t mylock;
    SplitPoint() {
        LockInit(mylock);
    }
    ~SplitPoint() {
        LockDestroy(mylock);
    }
    void lock() {
        Lock(mylock);
    }
    void unlock() {
        Unlock(mylock);
    }
};
#define SPLIT_STACK_MAX_DEPTH 4

// Per-node info, part of search history stack
struct NodeInfo {
    NodeInfo() : cutoff(0),best(NullMove)
    {
    }
    volatile int best_score;
    volatile int alpha, beta;
    volatile int cutoff;
    int num_try;
    int flags; 
    Move threatMove;
    volatile Move best;
    Move last_move;
    int extensions; // mask of extensions
    int eval;
    volatile Move pv[Constants::MaxPly];
    volatile int pv_length;
    volatile Move done[Constants::MaxMoves];
#ifdef MOVE_ORDER_STATS
    int best_count;
#endif
    volatile int fpruned_moves;
    int ply, depth;

    int PV() const {
        return (beta > alpha+1);
    }
    
    int inBounds(int score) const {
      return score > alpha && score < beta;
    }

    int newBest(int score) const {
      return score > best_score && score < beta;
    }

   // pad to mult of 128 (Intel x64 platform)
   volatile char pad[112];
};

typedef NodeInfo NodeStack[Constants::MaxPly];

// There are 4 levels of verbosity.  Silent mode does no output to
// the console - it is used by the Windows GUI. Debug level is
// used to output debug info. Whisper is used to "whisper"
// comments on a chess server. "Trace" is used in trace mode
// (-t) of the chess server client.
enum TalkLevel { Silent, Debug, Whisper, Trace };
    
enum SearchType { FixedDepth, TimeLimit, FixedTime };

class Search;
class RootSearch;

typedef void (CDECL *PostFunction)(const Statistics &);
typedef int (CDECL *TerminateFunction)(const Statistics &);

class SearchController {
  friend class Search;
  friend class RootSearch;

 public:
   SearchController();

   ~SearchController();

   Move findBestMove(
      const Board &board,
      SearchType srcType,
      int time_limit,
      int xtra_time,
      int ply_limit,
      int background,
      int isUCI,
      Statistics &stat_buf,
      TalkLevel t,
      Move *exclude, int num_exclude);

   Move findBestMove( 
      const Board &board, 
      SearchType srcType,
      int time_limit,
      int xtra_time,
      int ply_limit,
      int background,
      int isUCI,
      Statistics &stats,
      TalkLevel t);

    unsigned getTimeLimit() const { 
        return time_target + (time_added ? xtra_time : 0);
    }

    void terminateNow();

    PostFunction registerPostFunction(PostFunction post) {
       PostFunction tmp = post_function;
       post_function = post;
       return tmp;
    }
     
    TerminateFunction registerTerminateFunction(TerminateFunction term) {
       TerminateFunction tmp = terminate_function;
       terminate_function = term;
       return tmp;
    }

    TalkLevel getTalkLevel() const {
      return talkLevel;
    }

    void setTalkLevel(TalkLevel t);

    void clearHashTables();

    void resizeHash(size_t newSize);

    void stopAllThreads();

    void clearStopFlags();
     
    void updateSearchOptions();

    void setBackground(int b) {
      background = b;
    }

    void setTimeLimit(int limit,int xtra) {
      typeOfSearch = TimeLimit;
      time_limit = time_target = limit;
      xtra_time = xtra;
    }

    int wasTerminated() const;

    void setRatingDiff(int diff);

    // Note: should not call this while searching
    void setThreadCount(int threads);

    int getIterationDepth() const;

    RootSearch *root() const {
      return rootSearch;
    }

    ColorType getComputerSide() const {
        return computerSide;
    }

    void uciSendInfos(Move move, int move_index, int depth);

    bool isActive() const {
        return active;
    }

    void stop() {
        stopped = true;
    }

    bool wasStopped() const {
        return stopped;
    }

    void setStop(bool status) {
        stopped = status;
    }

    void setThreadSplitDepth(int depth);

    Hash hashTable;

 private:

    // pointer to function, called to output status during
    // a search.
    void (CDECL *post_function)(const Statistics &);

    int (CDECL *terminate_function)(const Statistics &);

    // check console input
    int check_input(const Board &);

    void updateStats(NodeInfo *node,int iteration_depth,
		     int score, int alpha, int beta);

#ifdef _WIN32
    int pipe;
#endif
    int uci;
    int age;
    int explicit_excludes;
    TalkLevel talkLevel;
    // time limit is nominal time limit in centiseconds
    // time target is actual time to search in centiseconds
    int time_limit, time_target;
    int time_added;
    // Amount of time we can add if score is dropping:
    int xtra_time;
    int ply_limit;
    int background;
    // flag for UCI. When set the search will terminate at the
    // next time check interval:
    bool stopped;
    SearchType typeOfSearch;
    int time_check_counter;
#ifdef SMP_STATS
    int sample_counter;
#endif
    int threadSplitDepth;
    Statistics *stats;
    ColorType computerSide;
    int ratingDiff, ratingFactor;
    unsigned startTime;
    RootSearch *rootSearch;
    ThreadPool *pool;
    bool active;
    LockDefine(split_calc_lock);
};

class Search : public ThreadControl {

  friend class ThreadPool;
  friend class SearchController;

 public:

  Search(SearchController *c, ThreadInfo *ti);

  virtual ~Search();

  void init(NodeStack &ns, ThreadInfo *child_ti);

    int search(int alpha, int beta,
               int ply, int depth, int flags = 0) {
      PUSH(alpha,beta,flags,ply,depth);
      return POP(search());
    }

    // search based on current board & NodeInfo
    int search();

    int quiesce(int alpha, int beta,
                int ply, int depth) {
       PUSHQ(alpha,beta,ply);
       return POP(quiesce(ply,depth));
    }

    void storeHash(const Board &board, hash_t hash, Move hash_move, int depth);

    int quiesce(int ply,int depth);

    int wasTerminated() const {
      return terminate;
    }

    NodeInfo * getNode() const {
      return node;
    }

    int master() const { return ti->index == 0; }
    // perform a subsidiary search in a separate thread
    void searchSMP(ThreadInfo *);
    int maybeSplit(const Board &board, NodeInfo *node,
                   MoveGenerator *mg, int ply, int depth);
    void stop() {
        terminate = 1;
    }

    void clearStopFlag() {
        terminate = 0;
    }

    virtual void clearHashTables();

    void restoreFromSplit(const SplitPoint *split) {
        if (split) {
            // Restore state from prior split point. We are not quite
            // out of the search routine from which the split occurred,
            // so may still need to touch these variables before exiting.
            board = split->savedBoard;
            node = split->splitNode;
        }
    }

   // We maintain a local copy of the search options, to reduce
   // the need for each thread to query global memory. This
   // forces a reload of that cache from the global options:
   void setSearchOptions();

   int drawScore(const Board &board) const;

 protected:

    enum Extension_Type { RECAPTURE=1, CHECK=2, PAWN_PUSH=4, CAPTURE=8,
       FORCED=16, LMR=64 };

    enum SearchFlags { IID=1, VERIFY=2, EXACT=4 };

    enum { QS_CHECK_DEPTH = -1, QS_NOCHECK_DEPTH = 2};

    int movesRelated( Move lastMove, Move threatMove) const;

    int calcExtensions(const Board &board,
                       NodeInfo *node, NodeInfo *parentNode,
                       CheckStatusType in_check_after_move,
                       int moveIndex,
                       Move move);

    int qsearch_no_check(int ply, int depth, Move pv, hash_t hash, int tt_depth, int hash_depth);

    int qsearch_check(int ply, int depth, Move pv, hash_t hash, int tt_depth);

    int updateRootMove(const Board &board, NodeInfo *parentNode, 
                       NodeInfo *node, Move move, int score, int move_index);

    int updateMove(const Board &,
           NodeInfo *parentNode, NodeInfo* myNode, Move move, 
           int score, int ply, int depth);

   void updatePV(const Board &, Move m, int ply);

   void updatePV(const Board &board,NodeInfo *node,NodeInfo *fromNode,Move move, int ply);

   int checkTime(const Board &board,int ply);

   void showStatus(const Board &board, Move best,int faillow,
            int failhigh,int complete);

   int getEval(NodeInfo *node, const Board &board) {
       if (node->eval == Scoring::INVALID_SCORE) {
         node->eval = scoring.evalu8(board);
       }
       return node->eval;
    }

   FORCEINLINE void PUSH(int alpha,int beta,int flags,
                          int ply, int depth) {
       ++node; 
       node->alpha = node->best_score = alpha; 
       node->beta = beta; 
       node->flags = flags; 
       node->best = NullMove; 
       node->num_try = 0;
       node->ply = ply;
       node->depth = depth;
       node->cutoff = 0; 
       node->eval = Scoring::INVALID_SCORE;
       node->pv[ply] = NullMove;
       node->pv_length = 0;
    }

    FORCEINLINE void PUSHQ(int alpha,int beta, int ply) {
       ++node; 
       node->flags = 0;
       node->alpha = node->best_score = alpha; 
       node->beta = beta; 
       node->best = NullMove; 
       node->eval = Scoring::INVALID_SCORE;
       node->pv[ply] = NullMove;
       node->pv_length = 0;
    }

    FORCEINLINE int POP(int value)  {
       int val = value; 
       --node; 
       return val;
    }

    RootSearch *root() const {
      return controller->rootSearch;
    }

    void setVariablesFromController() {
       computerSide = controller->computerSide;
       talkLevel = controller->talkLevel;
       ratingDiff = controller->ratingDiff;
       ratingFactor = controller->ratingFactor;
    }

    void setRatingVariablesFromController() {
       ratingDiff = controller->ratingDiff;
       ratingFactor = controller->ratingFactor;
    }

    void setTalkLevelFromController() {
       talkLevel = controller->talkLevel;
    }

    void setSplitDepthFromController() {
       threadSplitDepth = controller->threadSplitDepth;
    }

    SearchController *controller;
    Board board;
    SearchContext context;
    int terminate;
    uint64 nodeCount;
    int nodeAccumulator;
    NodeInfo *node; // pointer into NodeStack array (external to class)
    // lock for the split stack
    LockDefine(splitLock);
    SplitPoint splitStack[SPLIT_STACK_MAX_DEPTH];
    int activeSplitPoints;
    // Split point to which this search instance is attached (may be null).
    // All slave nodes and the parent share a split point instance.
    SplitPoint *split; 
    Scoring scoring;
    ThreadInfo *ti; // thread now running this search
    int threadSplitDepth;
    // The following variables are maintained as local copies of 
    // state from the controller. Placing them in each thread instance
    // helps avoid global variable contention.
    Options::SearchOptions srcOpts;
    ColorType computerSide;
    int ratingDiff;
    int ratingFactor;
    TalkLevel talkLevel;
};

class RootSearch : public Search {

  friend class SearchController;
  friend class Search;

 public:

  RootSearch(SearchController *c, ThreadInfo *ti) 
    : Search(c,ti),iteration_depth(0) {
  }

  void init(const Board &board, NodeStack &ns);

  int getIterationDepth() const {
    return iteration_depth;
  }

  Move ply0_search(Move *exclude, int num_exclude);

  virtual void clearHashTables();

  const Board &getInitialBoard() const {
    return initialBoard;
  }

  int getWaitTime() const {
      return waitTime;
  }

 protected:

  int ply0_search(RootMoveGenerator &, int alpha, int beta,
     int iteration_depth,
     int depth, Move exclude [], int num_exclude);

  Board initialBoard;
  int iteration_depth;
  int multipv_count;
  Move easyMove;
  bool easy_adjust, fail_high_root_extend;
  int fail_high_root;
  int last_score;
  int waitTime;
  
};

#endif
