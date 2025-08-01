// Copyright 1994-2025 by Jon Dart.  All Rights Reserved.

#ifndef _SEARCH_H
#define _SEARCH_H

#include "board.h"
#include "stats.h"
#include "searchc.h"
#include "scoring.h"
#include "movegen.h"
#include "threadp.h"
#include "globals.h"
#include "evaluate.h"
#include "options.h"
extern "C" {
#include <memory.h>
#include <time.h>
}
#include <atomic>
#include <cassert>
#include <functional>
#include <random>

class MoveGenerator;

// Per-node info, part of search history stack
struct NodeInfo {
    NodeInfo() : best_score(Constants::INVALID_SCORE),
                 alpha(Constants::INVALID_SCORE),
                 beta(Constants::INVALID_SCORE),
                 cutoff(0),
                 num_quiets(0),
                 num_captures(0),
                 num_legal(0),
                 flags(0),
                 best(NullMove),
                 last_move(NullMove),
                 excluded(NullMove),
                 eval(Constants::INVALID_SCORE),
                 staticEval(Constants::INVALID_SCORE),
                 pv_length(0),
#ifdef MOVE_ORDER_STATS
                 best_count(0),
#endif
                 ply(0),
                 depth(0),
                 stm(White)
        {
        }
    score_t best_score;
    score_t alpha, beta;
    int cutoff;
    int num_quiets;
    int num_captures;
    int num_legal;
    int flags;
    Move best;
    Move last_move;
    Move excluded;
    score_t eval, staticEval;
    Move pv[Constants::MaxPly];
    int pv_length;
    Move quiets[Constants::MaxMoves];
    Move captures[Constants::MaxCaptures];
#ifdef MOVE_ORDER_STATS
    int best_count;
#endif
    int ply, depth;
    // variables needed for incremental NNUE evaluation:
    nnue::Network::AccumulatorType accum;
    std::array<nnue::DirtyState, 3> dirty;
    unsigned dirty_num;
    ColorType stm;
    Square kingPos[2];

    int PV() const {
        return (beta > alpha+1);
    }

    int inBounds(score_t score) const {
        return score > alpha && score < beta;
    }

    int newBest(score_t score) const {
        return score > best_score && score < beta;
    }

    Square kingSquare(ColorType side) const noexcept {
        return kingPos[side];
    }
};

// Helper class to save/restore key node parameters
class NodeState
{
public:
    NodeState(NodeInfo *node)
        : n(node),
          alpha(node->alpha),
          beta(node->beta),
          ply(node->ply),
          depth(node->depth),
          flags(node->flags)
        {
        }

    ~NodeState() {
        // reset saved parameters
        n->alpha = alpha;
        n->beta = beta;
        n->depth = depth;
        n->flags = flags;
        // reset other parameters to pre-search state
        n->best_score = alpha;
        n->cutoff = 0;
        n->num_quiets = n->num_legal = 0;
        n->best = n->last_move = NullMove;
        (n+1)->pv[ply+1] = NullMove;
        (n+1)->pv_length = 0;
        n->pv[ply] = NullMove;
        n->pv_length = 0;
#ifdef MOVE_ORDER_STATS
        n->best_count = 0;
#endif
    }

private:
    NodeInfo *n;
    score_t alpha, beta;
    int ply, depth, flags;
};


// There are 4 levels of verbosity.  Silent mode does no output to
// the console - it is used by the Windows GUI. Test level is
// used for verbose output from the "test" command. Whisper is
// used to "whisper" comments on a chess server. "Debug" is used
// when -t is added to the command line or UCI debug mode has been
// enabled. It generates debug output to cout prefixed with '#' for
// Winboard or "info " for UCI.
enum class TalkLevel { Silent, Test, Whisper, Debug };

enum SearchType { FixedDepth, TimeLimit, FixedTime };

class SearchController;

class Search {

    friend class ThreadPool;
    friend class SearchController;

public:

    Search(SearchController *c, ThreadInfo *ti);

    virtual ~Search() = default;

    // one-time startup initialization
    static void init();

    void init(NodeInfo *nodeStack, ThreadInfo *child_ti);

    score_t search(score_t alpha, score_t beta,
                   int ply, int depth, int flags = 0, Move exclude = NullMove) {
        PUSH(alpha,beta,ply,depth,flags,exclude);
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

    // override global options setting
    void setMultiPV(int multipv_count) {
        srcOpts.multipv = multipv_count;
    }

    void updateTBOptions() {
#ifdef SYZYGY_TBS
        srcOpts.use_tablebases = globals::options.search.use_tablebases;
        srcOpts.syzygy_path = globals::options.search.syzygy_path;
        srcOpts.syzygy_50_move_rule = globals::options.search.syzygy_50_move_rule;
        srcOpts.syzygy_probe_depth = globals::options.search.syzygy_probe_depth;
#endif
    }

    score_t drawScore(const Board &board) const;

    // main entry point for top-level search; non-main threads enter here
    Move ply0_search(RootMoveGenerator::RootMoveList *moveList = nullptr);

    score_t ply0_search(RootMoveGenerator &, score_t alpha, score_t beta,
                        int iteration_depth, int depth, const MoveSet &exclude);

    bool mainThread() const {
       return ti->index == 0;
    }

    bool debugOut() const noexcept {
        return talkLevel == TalkLevel::Debug;
    }

    constexpr static int SearchStackSize = Constants::MaxPly + 10;

    using NodeStack = NodeInfo[SearchStackSize];

protected:

    enum SearchFlags { IID=1, VERIFY=2, EXACT=4, PROBCUT=8 };

    // Return true if move can be pruned (not called at root)
    bool prune(const Board &board,
               NodeInfo *node,
               CheckStatusType in_check_after_move,
               int moveIndex,
               int improving,
               Move move);

    int extend(const Board &board,
               NodeInfo *node,
               CheckStatusType in_check_after_move,
               Move move);

    int reduce(const Board &board,
               NodeInfo *node,
               int moveIndex,
               int improving,
               int newDepth,
               Move move);

    int updateRootMove(const Board &board,
                       NodeInfo *node, Move move, score_t score, int move_index);

    void updatePV(const Board &, Move m, int ply);

    int checkTime();

    void showStatus(const Board &board, Move best, bool faillow, bool failhigh);

    score_t tbScoreAdjust(const Board &board,
                          score_t score, int tb_hit, score_t tb_score) const;

    FORCEINLINE void PUSH(score_t alpha, score_t beta,
                          int ply, int depth, int flags, Move exclude) {
        assert(ply<Constants::MaxPly);
        ++node;
        node->alpha = node->best_score = alpha;
        node->beta = beta;
        node->flags = flags;
        node->excluded = exclude;
        node->best = NullMove;
        node->num_quiets = node->num_captures = node->num_legal = 0;
        node->ply = ply;
        node->depth = depth;
        node->cutoff = 0;
        node->pv[ply] = node->last_move = NullMove;
        node->pv_length = 0;
    }

    FORCEINLINE void PUSHQ(score_t alpha, score_t beta, int ply) {
        assert(ply<Constants::MaxPly);
        ++node;
        node->flags = 0;
        node->ply = ply;
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

    void updateStats(const Board &, NodeInfo *node, int iteration_depth,
		     score_t score);

    score_t evalu8(const Board &board);

    bool maxDepth(const NodeInfo *n) const noexcept {
        return n->ply >= Constants::MaxPly-1;
    }

    // called when preparing a recursive search w/o a move,
    // for example IID
    void clearNNUEState(NodeInfo *n) {
        (n+1)->accum.setEmpty();
        (n+1)->dirty_num = 0;
        (n+1)->stm = n->stm;
        (n+1)->kingPos[White] = n->kingPos[White];
        (n+1)->kingPos[Black] = n->kingPos[Black];
    }

    SearchController *controller;
    Board board;
    Statistics stats;
    int iterationDepth;
    SearchContext context;
    int terminate;
    int nodeAccumulator;
    NodeInfo *node; // pointer into node stack (external to class)
    Scoring scoring;
    ThreadInfo *ti; // thread now running this search
    // The following variables are maintained as local copies of
    // state from the controller. Placing them in each thread instance
    // helps avoid global variable contention.
    Options::SearchOptions srcOpts;
    ColorType computerSide;
    score_t contempt;
    int age;
    TalkLevel talkLevel; // copy of controller's talkLevel
    RootMoveGenerator::RootMoveList rootMoves; // root move list up until wide window limit
    std::mt19937_64 random_engine;
};

class SearchController {
    friend class Search;

public:
    SearchController();

    ~SearchController();

    typedef std::function<void (const Statistics &)> PostFunction;
    typedef std::function<bool (SearchController *,const Statistics &)> MonitorFunction;

    Move findBestMove(
        const Board &board,
        SearchType srcType,
        int time_limit,
        int xtra_time,
        int ply_limit,
        bool background,
        bool isUCI,
        Statistics &stat_buf,
        TalkLevel t,
        const MoveSet &exclude,
        const MoveSet &include,
        RootMoveGenerator::RootMoveList *moveList = nullptr);

    Move findBestMove(
        const Board &board,
        SearchType srcType,
        int time_limit,
        int xtra_time,
        int ply_limit,
        bool background,
        bool isUCI,
        Statistics &stats,
        TalkLevel t,
        RootMoveGenerator::RootMoveList *moveList = nullptr);

    // return a list of moves and their scores
    // ranked in descending order of score.
    void rankMoves(
        const Board &board,
        int ply_limit,
        RootMoveGenerator::RootMoveList &moveList);

    void terminateNow();

    // Set a "post" function that will be called from the
    // search to output status data (for Winboard; also used
    // in test mode). Returns the previous function instance.
    PostFunction registerPostFunction(PostFunction post) {
        PostFunction tmp = post_function;
        post_function = post;
        return tmp;
    }

    // Set a "monitor" function that will be called during the
    // search. This function returns 1 if the search should
    // terminate. This function returns the previous function
    // instance.
    MonitorFunction registerMonitorFunction(MonitorFunction func) {
        MonitorFunction tmp = monitor_function;
        monitor_function = func;
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

    void updateTBOptions();

    void setBackground(bool b) {
        background = b;
    }

    bool pondering() const noexcept {
        return is_searching && background;
    }

    bool searching() const noexcept {
        return is_searching;
    }

    void setTimeLimit(uint64_t limit,uint64_t xtra) {
        typeOfSearch = TimeLimit;
        time_limit = time_target = limit;
        xtra_time = xtra;
        // re-calculate bonus time
        applySearchHistoryFactors();
    }

    void setContempt(score_t contempt);

    score_t getContempt() const noexcept {
       return contempt;
    }

    // Note: should not call this while searching
    void setThreadCount(int threads);

    ColorType getComputerSide() const {
        return computerSide;
    }

    void uciSendInfos(const Board &, Move move, int move_index, int depth);

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

    score_t drawScore(const Board &board, const Statistics *s = nullptr) {
      // if we know the opponent's rating (which will be the case if playing
      // on ICC in xboard mode), or if the user has set a contempt value
      // (in UCI mode), factor that into the draw score - a draw against
      // a high-rated opponent is good; a draw against a lower-rated one is bad.
      // Add semi-random component to score to avoid 3-move rep blindness
      // (idea from Stockfish)
      score_t score = 0;
      if (s) score += (s->num_nodes % 2) - 1;
      if (contempt) {
         if (board.sideToMove() == computerSide)
            score -= contempt;
         else
            score += contempt;
      }
      return score;
   }

    uint64_t getElapsedTime() const {
       return elapsed_time;
    }

#ifdef SMP_STATS
	double getCpuPercentage() const {
      if (samples)
         return (100.0*threads)/samples;
      else
         return 0.0;
   }
#endif

   void updateGlobalStats(const Statistics &);

   struct BestThreadResults {
       Statistics *bestStats;
       Search *bestSearch;
   };

   void getBestThreadStats(BestThreadResults &best, bool trace) const;

   uint64_t totalNodes() const {
      return pool->totalNodes();
   }

   uint64_t totalHits() const {
      return pool->totalHits();
   }

   bool mainThreadCompleted() const noexcept {
       return pool->isCompleted(0);
   }

   bool isMonitorThread(unsigned id) const noexcept {
       return id == monitorThread;
   }

   void setMonitorThread(unsigned id) {
       monitorThread = id;
   }

   const Statistics &getGlobalStats() const noexcept {
       return *stats;
   }

#ifdef NUMA
   void recalcBindings() {
       pool->recalcBindings();
   }
#endif

   bool debugOut() const noexcept {
      return talkLevel == TalkLevel::Debug;
   }

   // override global options setting for MultiPV
   void setMultiPV(int limit);

private:

   // Adjust time usage after root fail high or fail low. A temporary
   // time extension is done to allow resolution of the fail high/low.
   // Called from main thread.
   void outOfBoundsTimeAdjust();

   // Calculate the time adjustment after a root search iteration has
   // completed (possibly with one or more fail high/fail lows).
   // Called from main thread.
   void historyBasedTimeAdjust(const Statistics &stats);

   // Apply search history factors to adjust time control
   void applySearchHistoryFactors();

    // compute time limit based on current search state
    uint64_t getTimeLimit() const noexcept;

    uint64_t getMaxTime() const {
        return time_target + xtra_time;
    }

    int computeTimeCheckInterval(uint64_t num_nodes) const noexcept;

    // pointer to function, called to output status during
    // a search.
    PostFunction post_function;

    MonitorFunction monitor_function;

    // check console input
    int check_input(const Board &);

    unsigned nextSearchDepth(unsigned current_depth, unsigned thread_id,
        unsigned max_depth);

    // possibly pick a suboptimal move (for strength reduction feaure).
    // If one was selected, return true.
    bool suboptimal(Statistics *bestStats,const Search *bestSearch);

    bool uci;
    int age;
    TalkLevel talkLevel;
    // time limit is nominal time limit in centiseconds
    // time target is actual time to search in centiseconds
    uint64_t time_limit, time_target;
    // Max amount of time we can add if score is dropping:
    uint64_t xtra_time;
    std::atomic<int64_t> bonus_time;
    bool fail_high_root_extend, fail_low_root_extend, fail_high_root;
    // Factors to use to adjust time up/down based on search history:
    double searchHistoryBoostFactor, searchHistoryReductionFactor, maxBoostFactor;
    int maxBoostDepth;
    int ply_limit;
    std::atomic<bool> background;
    std::atomic<bool> is_searching;
    // flag for UCI. When set the search will terminate at the
    // next time check interval:
    bool stopped;
    SearchType typeOfSearch;
    int time_check_counter;
    int timeCheckInterval;
    float timeCheckFactor;
    std::atomic<uint64_t> last_time_check;
#ifdef SMP_STATS
    int sample_counter;
#endif
    Statistics *stats;
    ColorType computerSide;
    score_t contempt;
    CLOCK_TYPE startTime;
    CLOCK_TYPE last_time;
    ThreadPool *pool;
    Search *rootSearch;
    int tb_root_probes, tb_root_hits;
    bool tb_probe_in_search;
    unsigned monitorThread;

    MoveSet include;
    MoveSet exclude;

    Options::SearchOptions srcOpts;

    struct SearchHistory
    {
        Move pv;
        score_t score;

        SearchHistory() : pv(NullMove), score(Constants::INVALID_SCORE)
        {
        }

        SearchHistory(Move m, score_t value) : pv(m), score(value)
        {
        }
    };

    std::array<SearchHistory,Constants::MaxPly> rootSearchHistory;

#ifdef SYZYGY_TBS
    int tb_hit, tb_dtz;
    score_t tb_score;
#endif

    Board initialBoard;
    score_t initialValue;
    RootMoveGenerator *mg;

    std::atomic<uint64_t> elapsed_time; // in milliseconds
    std::array <unsigned, Constants::MaxPly> search_counts;
    std::mutex search_count_mtx;

#ifdef SMP_STATS
    uint64_t samples, threads;
#endif

};

#endif
