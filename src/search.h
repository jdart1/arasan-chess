// Copyright 1994-2018 by Jon Dart.  All Rights Reserved.

#ifndef _SEARCH_H
#define _SEARCH_H

#include "board.h"
#include "stats.h"
#include "searchc.h"
#include "scoring.h"
#include "movegen.h"
#include "threadp.h"
#include "options.h"
extern "C" {
#include <memory.h>
#include <time.h>
};
#include <random>
#include <set>
using namespace std;

class MoveGenerator;

struct NodeInfo;

// Per-node info, part of search history stack
struct NodeInfo {
    NodeInfo() : cutoff(0),best(NullMove)
        {
        }
    score_t best_score;
    score_t alpha, beta;
    int cutoff;
    int num_try;
    int flags;
    Move singularMove;
    Move best;
    Move last_move;
    int extensions; // mask of extensions
    score_t eval, staticEval;
    Move pv[Constants::MaxPly];
    int pv_length;
    Move done[Constants::MaxMoves];
#ifdef MOVE_ORDER_STATS
    int best_count;
#endif
    int ply, depth;
    char pad[110];

    int PV() const {
        return (beta > alpha+1);
    }

    int inBounds(score_t score) const {
        return score > alpha && score < beta;
    }
    int newBest(score_t score) const {
        return score > best_score && score < beta;
    }
};

typedef NodeInfo NodeStack[Constants::MaxPly];

// There are 4 levels of verbosity.  Silent mode does no output to
// the console - it is used by the Windows GUI. Debug level is
// used to output debug info. Whisper is used to "whisper"
// comments on a chess server. "Trace" is used in trace mode
// (-t) of the chess server client.
enum TalkLevel { Silent, Debug, Whisper, Trace };

enum SearchType { FixedDepth, TimeLimit, FixedTime };

class SearchController;

typedef void (CDECL *PostFunction)(const Statistics &);
typedef int (CDECL *TerminateFunction)(const Statistics &);

class Search : public ThreadControl {

    friend class ThreadPool;
    friend class SearchController;

public:

    Search(SearchController *c, ThreadInfo *ti);

    virtual ~Search();

    void init(NodeStack &ns, ThreadInfo *child_ti);

    score_t search(score_t alpha, score_t beta,
                   int ply, int depth, int flags = 0) {
        PUSH(alpha,beta,flags,ply,depth);
        return POP(search());
    }

    // search based on current board & NodeInfo
    score_t search();

    score_t quiesce(score_t alpha, score_t beta,
                    int ply, int depth) {
        PUSHQ(alpha,beta,ply);
        return POP(quiesce(ply,depth));
    }

    score_t quiesce(int ply,int depth);

    int wasTerminated() const {
        return terminate;
    }

    NodeInfo * getNode() const {
        return node;
    }

    int master() const { return ti->index == 0; }

    void stop() {
        terminate = 1;
    }

    void clearStopFlag() {
        terminate = 0;
    }

    virtual void clearHashTables();

    // We maintain a local copy of the search options, to reduce
    // the need for each thread to query global memory. This
    // forces a reload of that cache from the global options:
    void setSearchOptions();

    score_t drawScore(const Board &board) const;

#ifdef TUNE
    static const int LEARNING_SEARCH_WINDOW;
    static double func( double x );
#endif

    void terminateSlaveSearches();

    // main entry point for top-level search; non-main threads enter here
    Move ply0_search();

    score_t ply0_search(RootMoveGenerator &, score_t alpha, score_t beta,
                        int iteration_depth,
                        int depth,
                        const vector<Move> &exclude,
                        const vector<Move> &include);

    bool mainThread() const {
       return ti->index == 0;
    }

protected:

    enum Extension_Type { RECAPTURE=1, CHECK=2, PAWN_PUSH=4, CAPTURE=8,
                          FORCED=16, LMR=64, SINGULAR_EXT=128 };

    enum SearchFlags { IID=1, VERIFY=2, EXACT=4, SINGULAR=8, PROBCUT=16 };

    int calcExtensions(const Board &board,
                       NodeInfo *node, NodeInfo *parentNode,
                       CheckStatusType in_check_after_move,
                       int moveIndex,
                       Move move);

    void storeHash(const Board &board, hash_t hash, Move hash_move, int depth);

    int updateRootMove(const Board &board, NodeInfo *parentNode,
                       NodeInfo *node, Move move, score_t score, int move_index);

    int updateMove(const Board &,
                   NodeInfo *parentNode, NodeInfo* myNode, Move move,
                   score_t score, int ply, int depth);

    void updatePV(const Board &, Move m, int ply);

    void updatePV(const Board &board,NodeInfo *node,NodeInfo *fromNode,Move move, int ply);

    int checkTime(const Board &board,int ply);

    void showStatus(const Board &board, Move best, bool faillow, bool failhigh);

    score_t tbScoreAdjust(const Board &board,
                          score_t score, int tb_hit, score_t tb_score) const;

    score_t futilityMargin(int depth) const;

    score_t razorMargin(int depth) const;

    FORCEINLINE void PUSH(score_t alpha, score_t beta, int flags,
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
        node->pv[ply] = node->last_move = NullMove;
        node->pv_length = 0;
    }

    FORCEINLINE void PUSHQ(score_t alpha, score_t beta, int ply) {
        ++node;
        node->flags = 0;
        node->alpha = node->best_score = alpha;
        node->beta = beta;
        node->best = NullMove;
        node->pv[ply] = NullMove;
        node->pv_length = 0;
    }

    FORCEINLINE score_t POP(score_t value)  {
        --node;
        return value;
    }

    void setVariablesFromController();

    void setContemptFromController();

    void setTalkLevelFromController();

    void updateStats(const Board &, NodeInfo *node,int iteration_depth,
		     score_t score, score_t alpha, score_t beta);

    void updatePVinStats(Move *pv, int pv_length, int iteration_depth);

    SearchController *controller;
    Board board;
    Statistics stats;
    int iterationDepth;
    SearchContext context;
    int terminate;
    uint64_t nodeCount;
    int nodeAccumulator;
    NodeInfo *node; // pointer into NodeStack array (external to class)
    Scoring scoring;
    ThreadInfo *ti; // thread now running this search
    // The following variables are maintained as local copies of
    // state from the controller. Placing them in each thread instance
    // helps avoid global variable contention.
    Options::SearchOptions srcOpts;
    ColorType computerSide;
    score_t contempt;
    TalkLevel talkLevel;
};

class SearchController {
    friend class Search;

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
        const vector<Move> &exclude,
        const vector<Move> &include);

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

    uint64_t getTimeLimit() const {
        return time_target + time_added;
    }

    uint64_t getMaxTime() const {
        return time_target + xtra_time;
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

    void setTimeLimit(uint64_t limit,uint64_t xtra) {
        typeOfSearch = TimeLimit;
        time_limit = time_target = limit;
        xtra_time = xtra;
    }

    int wasTerminated() const;

    void setContempt(score_t contempt);

    // Note: should not call this while searching
    void setThreadCount(int threads);

    ColorType getComputerSide() const {
        return computerSide;
    }

    void uciSendInfos(const Board &, Move move, int move_index, int depth);

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

    Hash hashTable;

    score_t drawScore(const Board &board) {
      // if we know the opponent's rating (which will be the case if playing
      // on ICC in xboard mode), or if the user has set a contempt value
      // (in UCI mode), factor that into the draw score - a draw against
      // a high-rated opponent is good; a draw against a lower-rated one is bad.
      if (contempt) {
         if (board.sideToMove() == computerSide)
            return -contempt;
         else
            return contempt;
      }
      return 0;
   }

#ifdef NUMA
    void rebind() {
        pool->rebind();
    }

    void unbind() {
        pool->unbind();
    }
#endif

    uint64_t getElapsedTime() const {
       return elapsed_time;
    }
   
   double getCpuPercentage() const {
      if (samples)
         return (100.0*threads)/samples;
      else
         return 0.0;
   }

   void updateGlobalStats(const Statistics &);

private:

    // pointer to function, called to output status during
    // a search.
    void (CDECL *post_function)(const Statistics &);

    int (CDECL *terminate_function)(const Statistics &);

    // check console input
    int check_input(const Board &);

    unsigned random(unsigned max) {
       std::uniform_int_distribution<unsigned> dist(0,max);
       return dist(random_engine);
    }

    void suboptimal(RootMoveGenerator &mg, Move &m, score_t &val);


    int uci;
    int age;
    TalkLevel talkLevel;
    // time limit is nominal time limit in centiseconds
    // time target is actual time to search in centiseconds
    uint64_t time_limit, time_target;
    uint64_t time_added;
    // Amount of time we can add if score is dropping:
    uint64_t xtra_time;
    int ply_limit;
    int background;
    // flag for UCI. When set the search will terminate at the
    // next time check interval:
    bool stopped;
    SearchType typeOfSearch;
    int time_check_counter;
    int failLowFactor;
#ifdef SMP_STATS
    int sample_counter;
#endif
    Statistics *stats;
    ColorType computerSide;
    score_t contempt;
    CLOCK_TYPE startTime;
    CLOCK_TYPE last_time;
    ThreadPool *pool;
    bool active;
    LockDefine(split_calc_lock);
    Search *rootSearch;

    vector<Move> include;
    vector<Move> exclude;

#ifdef SYZYGY_TBS
    int tb_hit;
    score_t tb_score;
#endif

    Board initialBoard;
    score_t initialValue;
    Move easyMove;
    score_t easyScore;
    int depth_at_pv_change;
    bool easy_adjust, fail_high_root_extend, fail_low_root_extend;
    int fail_high_root;
    score_t last_score;
    int waitTime;
    int depth_adjust; // for strength feature
    int iteration_value[Constants::MaxPly];
    std::mt19937_64 random_engine;

    uint64_t elapsed_time; // in milliseconds
#ifdef SMP_STATS
    uint64_t samples, threads;
#endif

};

#endif
