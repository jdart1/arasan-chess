// Copyright 1987-2022 by Jon Dart.  All Rights Reserved.

#include "search.h"
#include "globals.h"
#include "notation.h"
#include "movegen.h"
#include "hash.h"
#include "protocol.h"
#include "see.h"
#ifdef SYZYGY_TBS
#include "syzygy.h"
#endif
#include "legal.h"
#include "trace.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>

//#define _TRACE

static const int ASPIRATION_WINDOW[] =
    {(int)(0.375*Params::PAWN_VALUE),
     (int)(0.75*Params::PAWN_VALUE),
     (int)(1.5*Params::PAWN_VALUE),
     (int)(3.0*Params::PAWN_VALUE),
     (int)(6.0*Params::PAWN_VALUE),
      Constants::MATE};
static const int ASPIRATION_WINDOW_STEPS = 6;

#define STATIC_NULL_PRUNING
//#define RAZORING
#define SINGULAR_EXTENSION
#define MULTICUT
#define NON_SINGULAR_PRUNING

static const int IID_DEPTH[2] = {6*DEPTH_INCREMENT,8*DEPTH_INCREMENT};
static const int FUTILITY_DEPTH = 8*DEPTH_INCREMENT;
static const int FUTILITY_HISTORY_THRESHOLD[2] = {500, 250};
static const int HISTORY_PRUNING_THRESHOLD[2] = {0, 0};
#ifdef RAZORING
static const int RAZOR_DEPTH = DEPTH_INCREMENT;
#endif
static const int SEE_PRUNING_DEPTH = 5*DEPTH_INCREMENT;
static const int CHECK_EXTENSION = DEPTH_INCREMENT;
static const int PAWN_PUSH_EXTENSION = DEPTH_INCREMENT;
static const int CAPTURE_EXTENSION = DEPTH_INCREMENT/2;
static const score_t WIDE_WINDOW = 10*Params::PAWN_VALUE;
#ifdef SINGULAR_EXTENSION
static const int SINGULAR_EXTENSION_DEPTH = 8*DEPTH_INCREMENT;
#endif
static const int PROBCUT_DEPTH = 5*DEPTH_INCREMENT;
static const score_t PROBCUT_MARGIN = score_t(1.25*Params::PAWN_VALUE);
static const int LMR_DEPTH = 3*DEPTH_INCREMENT;
static constexpr double LMR_BASE[2] = {0.5, 0.3};
static constexpr double LMR_DIV[2] = {1.8,2.25};
// These tables are for the strength reduction feature:
static constexpr int STRENGTH_DEPTH_LIMITS[100] = {
    1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,
    2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,5,
    5,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,10,10,11,11,
    12,12,13,13,14,14,15,16,16,17,18,19,19,20,21,22,23,24,24,25,
    26,28,29,30,31,32,33,35,36,36,36,36,36,36,36,37,37,37,37,37};

#ifdef SINGULAR_EXTENSION
static score_t singularExtensionMargin(int depth)
{
    return (Params::PAWN_VALUE*depth)/(64*DEPTH_INCREMENT);
}

static int singularSearchDepth(int depth)
{
   return depth/2 - DEPTH_INCREMENT;
}
#endif

static int CACHE_ALIGN LMR_REDUCTION[2][64][64];

static const int LMP_DEPTH=13;

static const int LMP_MOVE_COUNT[2][16] = {{0, 2, 4, 7, 10, 16, 22, 30, 38, 49, 60, 73, 87, 102, 119, 140},
                                          {0, 4, 7, 12, 18, 26, 35, 46, 59, 73, 88, 105, 124, 145, 168}};

static constexpr score_t RAZOR_MARGIN = static_cast<score_t>(2.75*Params::PAWN_VALUE);

static constexpr score_t RAZOR_MARGIN_SLOPE = static_cast<score_t>(1.25*Params::PAWN_VALUE);

static constexpr score_t FUTILITY_MARGIN_BASE = static_cast<score_t>(0.25*Params::PAWN_VALUE);

static constexpr score_t FUTILITY_MARGIN_SLOPE = static_cast<score_t>(0.95*Params::PAWN_VALUE);

static constexpr int STATIC_NULL_PRUNING_DEPTH = 5*DEPTH_INCREMENT;

static constexpr score_t STATIC_NULL_MARGIN[2] = {static_cast<score_t>(0.75*Params::PAWN_VALUE),
    static_cast<score_t>(0.5*Params::PAWN_VALUE)};

static const score_t QSEARCH_FUTILITY_PRUNE_MARGIN = static_cast<score_t>(1.4*Params::PAWN_VALUE);
static const score_t QSEARCH_SEE_PRUNE_MARGIN = static_cast<score_t>(1.25*Params::PAWN_VALUE);

// global vars are updated only once this many nodes (to minimize
// thread contention for global memory):
static const int NODE_ACCUM_THRESHOLD = 16;

#ifdef SMP_STATS
static const int SAMPLE_INTERVAL = 10000/NODE_ACCUM_THRESHOLD;
#endif

static const int Illegal = Constants::INVALID_SCORE;

static void setCheckStatus(Board &board, CheckStatusType s)
{
   if (s != CheckUnknown) {
      // Here we take advantage of the fact that if we know the
      // check status before the move, we can set it in the board
      // structure after the move and avoid another calculation.
      board.setCheckStatus(s);
   }
}

static int FORCEINLINE passedPawnPush(const Board &board, Move move) {
    return (PieceMoved(move) == Pawn &&
            Rank(DestSquare(move),board.sideToMove()) == 7);
}

static int lmr(NodeInfo *node, int depth, int moveIndex) {
    return LMR_REDUCTION[node->PV()][depth/DEPTH_INCREMENT][std::min<int>(63,moveIndex)];
}

SearchController::SearchController()
    :
      post_function(nullptr),
      monitor_function(nullptr),
      uci(false),
      age(1),
      talkLevel(TalkLevel::Silent),
      time_limit(0),
      time_target(0),
      xtra_time(0),
      bonus_time(0),
      fail_high_root_extend(false),
      fail_low_root_extend(false),
      fail_high_root(false),
      searchHistoryBoostFactor(0.0),
      searchHistoryReductionFactor(0.0),
      maxBoostFactor(-1.0),
      maxBoostDepth(0),
      ply_limit(0),
      background(false),
      is_searching(false),
      stopped(false),
      typeOfSearch(TimeLimit),
      time_check_counter(0),
#ifdef SMP_STATS
      sample_counter(0),
#endif
      stats(NULL),
      computerSide(White),
      contempt(0),
      pool(nullptr),
      rootSearch(nullptr),
      tb_root_probes(0),
      tb_root_hits(0),
      tb_probe_in_search(true),
      monitorThread(0),
#ifdef SYZYGY_TBS
      tb_hit(0), tb_dtz(0), tb_score(Constants::INVALID_SCORE),
#endif
      initialValue(Constants::INVALID_SCORE),
      mg(NULL),
      elapsed_time(0)
#ifdef SMP_STATS
      , samples(0), threads(0)
#endif
{

#ifdef SMP_STATS
    sample_counter = SAMPLE_INTERVAL;
#endif
    pool = new ThreadPool(this,globals::options.search.ncpus);

    ThreadInfo *ti = pool->mainThread();
    ti->state = ThreadInfo::Working;
    hashTable.initHash((size_t)(globals::options.search.hash_table_size));
}

SearchController::~SearchController() {
   delete pool;
   hashTable.freeHash();
}

void Search::init() {
    for (int d = 0; d < 64; d++) {
        for (int moves = 0; moves < 64; moves++) {
            for (int p = 0; p < 2; p++) {
                LMR_REDUCTION[p][d][moves] = 0;
                if (d > 0 && moves > 0) {
                    const double reduction = LMR_BASE[p] + log(d) * log(moves) / LMR_DIV[p];
                    LMR_REDUCTION[p][d][moves] = static_cast<int>(DEPTH_INCREMENT*floor(2*reduction+0.5)/2);
                }
            }
        }
    }
/*
    for (int i = 3; i < 64; i++) {
      std::cout << "--- i=" << i << std::endl;
      for (int m=0; m<64; m++) {
         std::cout << m << " " <<
         1.0*LMR_REDUCTION[0][i][m]/DEPTH_INCREMENT << ' ' <<
         1.0*LMR_REDUCTION[1][i][m]/DEPTH_INCREMENT << ' ' <<
         std::endl;
      }}

 */
}

void SearchController::terminateNow() {
    stopAllThreads();
}

Move SearchController::findBestMove(
   const Board &board,
   SearchType srcType,
   int time_limit,
   int xtra_time,
   int ply_limit,
   int background,
   int isUCI,
   Statistics &stat_buf,
   TalkLevel t)
{
    MoveSet excludes, includes;
    Move result = findBestMove(board,srcType,time_limit,xtra_time,ply_limit,
                               background, isUCI, stat_buf, t, excludes, includes);
    return result;
}


Move SearchController::findBestMove(
   const Board &board,
   SearchType srcType,
   int search_time_limit,
   int search_xtra_time,
   int search_ply_limit,
   int isBackground,
   int isUCI,
   Statistics &stat_buf,
   TalkLevel t,
   const MoveSet &moves_to_exclude,
   const MoveSet &moves_to_include)
{
    typeOfSearch = srcType;
    initialBoard = board;
    time_limit = time_target = search_time_limit;
    exclude = moves_to_exclude;
    include = moves_to_include;
    fail_high_root_extend = fail_low_root_extend = false;
    fail_high_root = false;
    bonus_time = (int64_t)0;
    xtra_time = search_xtra_time;
    searchHistoryBoostFactor = searchHistoryReductionFactor = 0.0;
    maxBoostFactor = -1.0;
    maxBoostDepth = 0;
    search_counts.fill(0);
    search_counts[0] = globals::options.search.ncpus;
    if (srcType == FixedTime || srcType == TimeLimit) {
        ply_limit = Constants::MaxPly-1;
    }
    else {
        ply_limit = std::min<int>(ply_limit,Constants::MaxPly-1);
        ply_limit = std::max<int>(1,ply_limit);
    }
    ply_limit = search_ply_limit;
    background = isBackground != 0;
    uci = isUCI;
    talkLevel = t;
    stats = &stat_buf;
#ifdef SMP_STATS
    samples = threads = 0L;
#endif
    elapsed_time = 0ULL;

    timeCheckInterval = 4096/NODE_ACCUM_THRESHOLD;
    // reduce time check interval if time limit is very short (<1 sec)
    if (srcType == TimeLimit) {
       if (time_limit < 100) {
          timeCheckInterval = 1024/NODE_ACCUM_THRESHOLD;
       } else if (time_limit < 1000) {
          timeCheckInterval = 2048/NODE_ACCUM_THRESHOLD;
       }
    }
    tb_probe_in_search = true;
    computerSide = board.sideToMove();

#ifdef NUMA
    if (pool->rebindMask.test(0)) {
       // rebind main thread
       pool->bind(0);
       pool->rebindMask.reset(0);
    }
#endif

    stats->clear();

    // Positions are stored in the hashtable with an "age" to identify
    // which search they came from. "Newer" positions can replace
    // "older" ones. Update the age here since we are starting a
    // new search.
    age = (age + 1) % 256;

    // propagate controller variables to searches
    pool->forEachSearch<&Search::setVariablesFromController>();

    // reset global stop flag and terminate flag on all threads
    setStop(false);
    clearStopFlags();

    startTime = last_time = getCurrentTime();

    if (board.isLegalDraw() && !uci &&
        !(typeOfSearch == FixedTime && time_target == Constants::INFINITE_TIME)) {
        // If it's a legal draw situation before we even move, then
        // just return a draw score and don't search. (But don't do
        // this in analysis mode: return a move if possible. Also do
        // a search in all cases for UCI, since the engine cannot
        // claim draw and some interfaces may expect a move.)
        if (debugOut()) std::cout << globals::debugPrefix << "skipping search, draw" << std::endl;
        stats->state = Draw;
        stats->value = drawScore(board);
        return NullMove;
    }
   rootSearch = pool->rootSearch();
   // Generate the ply 0 moves here:
   mg = new RootMoveGenerator(board,&(rootSearch->context),NullMove,debugOut());
   if (mg->moveCount() == 0) {
      // Checkmate or statemate
      if (board.inCheck()) {
         stats->state = Checkmate;
         stats->value = stats->display_value = -Constants::MATE;
      }
      else {
         stats->state = Stalemate;
         stats->value = stats->display_value = drawScore(board);
      }
      delete mg;
      return NullMove;
   }

   // Set this only after early termination conditions checked (above);
   is_searching = true;

   // Implement strength reduction if enabled. But do not reduce
   // strength in analysis mode.
   if (globals::options.search.strength < 100 && (background || time_target != Constants::INFINITE_TIME)) {
      if (debugOut()) {
          std::cout << globals::debugPrefix << "strength=" << globals::options.search.strength << std::endl;
      }
      int new_ply_limit = STRENGTH_DEPTH_LIMITS[globals::options.search.strength];
      if (board.getMaterial(White).materialLevel() +
          board.getMaterial(Black).materialLevel() < 16 &&
          globals::options.search.strength > 10) {
          // increase ply limit in endgames
          new_ply_limit += std::min<int>(2,1+new_ply_limit/8);
      }
      ply_limit = std::min<int>(ply_limit,new_ply_limit);
      if (debugOut()) {
          std::cout << globals::debugPrefix << "ply limit=" << ply_limit << std::endl;
      }
   }

   time_check_counter = timeCheckInterval;

   score_t value = Constants::INVALID_SCORE;
#ifdef SYZYGY_TBS
   tb_hit = 0;
   updateSearchOptions();
   tb_score = Constants::INVALID_SCORE;
   tb_root_probes = tb_root_hits = 0;
   if (globals::options.search.use_tablebases) {
       // Lock because the following calls is not thread-safe. In normal use
       // we don't need to worry about this, but it is possible there are
       // two concurrent SearchController instances in a program, in which case
       // it matters.
       {
           std::unique_lock<std::mutex> lock(globals::syzygy_lock);
           tb_hit = mg->rank_root_moves();
       }
       if (tb_hit) {
           tb_root_probes += mg->moveCount();
           tb_root_hits += mg->moveCount();
           tb_score = mg->getMoveList()[0].tbScore;
           // Store the tb value but do not use it set the search score - search values are based
           // on DTM not DTZ.
           stats->tb_value = tb_score;
           // do not probe in the search
           tb_probe_in_search = false;
           updateSearchOptions();
           if (debugOut()) {
               std::cout << globals::debugPrefix << board << " root tb hit, score=";
               Scoring::printScore(tb_score,std::cout);
               std::cout << std::endl;
           }
       }
   }
#endif
   // Limit moves we will search.
   mg->filter(include);
#ifdef _TRACE
   std::cout << "filtered root moves:";
   for (const RootMove &m : mg->getMoveList()) {
       if (!(Flags(m.move) & Excluded)) {
           std::cout << ' ';
           Notation::image(board,m.move,Notation::OutputFormat::SAN,std::cout);
       }
   }
   std::cout << std::endl;
#endif

   if (value == Constants::INVALID_SCORE) {
      value = 0;
   }
   initialValue = value;
   stats->value = stats->display_value = value;

   // Start all searches
   pool->unblockAll();

   // Start searching in the main thread
   NodeInfo *rootStack = new NodeInfo[Search::SearchStackSize];
   rootSearch->init(rootStack,pool->mainThread());
   Move best = rootSearch->ply0_search();
   delete [] rootStack;
   // Mark thread 0 complete.
   pool->setCompleted(0);

   if (debugOut()) std::cout << globals::debugPrefix << "waiting for thread completion" << std::endl;

   // Wait for all threads to complete
   pool->waitAll();

   // We are finished with the move generator - can delete
   delete mg;

   if (debugOut()) {
       std::cout << globals::debugPrefix << "thread 0 depth=" << rootSearch->stats.completedDepth <<
           " score=";
       Scoring::printScore(rootSearch->stats.display_value,std::cout);
       std::cout << " failHigh=" << (int)stats->failHigh << " failLow=" <<
           (int)stats->failLow;
       std::cout << " pv=" << rootSearch->stats.best_line_image << std::endl;
   }

   if (globals::options.search.multipv == 1) {
       Statistics *bestStats = getBestThreadStats(debugOut());
       updateGlobalStats(*bestStats);
       best = bestStats->best_line[0];
   } else {
       updateGlobalStats(rootSearch->stats);
   }
#ifdef _TRACE
   std::cout << globals::debugPrefix << "best thread: score=";
   Scoring::printScore(stats->value,std::cout);
   std::cout << " pv=" << stats->best_line_image << std::endl;
#endif
   if (debugOut()) {
      std::cout << globals::debugPrefix << "best thread: depth=" << stats->completedDepth <<  " score=";
      Scoring::printScore(stats->value,std::cout);
      std::cout << " fail high=" << (int)stats->failHigh << " fail low=" << stats->failLow;
      std::cout << " pv=" << stats->best_line_image << std::endl;
   }

   // search done (all threads), set status and report statistics
   static const int end_of_game[] = {0, 1, 0, 1, 1, 1, 1};
   StateType &state = stats->state;
   stats->end_of_game = end_of_game[(int)stats->state];
   if (!uci && !stats->end_of_game && globals::options.search.can_resign) {
      const Material &ourMat = board.getMaterial(board.sideToMove());
      const Material &oppMat = board.getMaterial(board.oppositeSide());
      if (stats->display_value != Constants::INVALID_SCORE &&
          (100*stats->display_value)/Params::PAWN_VALUE <= globals::options.search.resign_threshold &&
          // don't resign KBN or KBB vs K unless near mate
          !(stats->display_value > -Constants::MATE &&
            ourMat.kingOnly () && !oppMat.hasPawns() &&
            (oppMat.pieceBits() == Material::KBN ||
             oppMat.pieceBits() == Material::KBB)) &&
          // don't resign KQ vs KR unless near mate
          !(stats->display_value > -Constants::MATE &&
            !ourMat.hasPawns() && !oppMat.hasPawns() &&
            (oppMat.pieceBits() == Material::KR ||
             oppMat.pieceBits() == Material::KQ))
#ifdef SYZYGY_TBS
          // Don't resign a tb lost position with large dtz,
          // unless we have a mate score, because the opponent can err
          // if not using TBs.
          && (stats->display_value != -Constants::TABLEBASE_WIN || tb_dtz < 30)
#endif
         ) {
         state = Resigns;
         stats->end_of_game = true;
      }
   }

   if (talkLevel == TalkLevel::Test) {
      std::ios_base::fmtflags original_flags = std::cout.flags();
      std::cout.setf(std::ios::fixed);
      std::cout << std::setprecision(2);
      if (elapsed_time > 0) {
         Statistics::printNPS(std::cout,stats->num_nodes,elapsed_time);
         std::cout << " nodes/second." << std::endl;
      }
#ifdef SEARCH_STATS
      std::cout << (stats->num_nodes-stats->num_qnodes) << " regular nodes, " <<
         stats->num_qnodes << " quiescence nodes." << std::endl;
      std::cout << stats->hash_searches << " searches of hash table, " <<
         stats->hash_hits << " successful";
      if (stats->hash_searches != 0)
         std::cout << " (" <<
            (int)((100.0*(float)stats->hash_hits)/((float)stats->hash_searches)) <<
            " percent).";
      std::cout << std::endl;
      std::cout << "hash table is " << std::setprecision(2) <<
          1.0F*hashTable.pctFull()/10.0F << "% full." << std::endl;
#endif
#ifdef MOVE_ORDER_STATS
      std::cout << "move ordering: ";
      static const char *labels[] = {"1st","2nd","3rd","4th"};
      for (int i = 0; i < 4; i++) {
         std::cout << std::setprecision(2) << labels[i] << " " <<
            (100.0*stats->move_order[i])/(float)stats->move_order_count << "% " ;
      }
      std::cout << std::endl;
#endif
#ifdef SEARCH_STATS
      std::cout << "pre-search pruning: " << std::endl;
      std::cout << ' ' << stats->razored << " (" << std::setprecision(2) << 100.0*stats->razored/stats->reg_nodes << "%) razoring" << std::endl;
      std::cout << ' ' << stats->static_null_pruning << " (" << std::setprecision(2) << 100.0*stats->static_null_pruning/stats->reg_nodes << "%) static null pruning" << std::endl;
      std::cout << ' ' << stats->null_cuts << " (" << std::setprecision(2) << 100.0*stats->null_cuts/stats->reg_nodes << "%) null cuts" << std::endl;
      std::cout << ' ' << stats->multicut << " (" << std::setprecision(2) << 100.0*stats->multicut/stats->reg_nodes << "%) multicut" << std::endl;
      std::cout << ' ' << stats->non_singular_pruning << " (" << std::setprecision(2) << 100.0*stats->non_singular_pruning/stats->reg_nodes << "%) non-singular pruning" << std::endl;
      std::cout << "search pruning: " << std::endl;
      std::cout << ' ' << stats->futility_pruning << " (" << std::setprecision(2) << 100.0*stats->futility_pruning/stats->moves_searched << "%) futility" << std::endl;
      std::cout << ' ' << stats->history_pruning << " (" << std::setprecision(2) << 100.0*stats->history_pruning/stats->moves_searched << "%) history" << std::endl;
      std::cout << ' ' << stats->lmp << " (" << std::setprecision(2) << 100.0*stats->lmp/stats->moves_searched << "%) lmp" << std::endl;
      std::cout << ' ' << stats->see_pruning << " (" << std::setprecision(2) << 100.0*stats->see_pruning/stats->moves_searched << "%) SEE" << std::endl;
      std::cout << ' ' << stats->reduced << "( " << std::setprecision(2) << 100.0*stats->reduced/stats->moves_searched << "%) reduced" << std::endl;
      std::cout << "extensions: " <<
          stats->check_extensions << " (" << 100.0*stats->check_extensions/stats->moves_searched << "%) check, " <<
          stats->capture_extensions << " (" << 100.0*stats->capture_extensions/stats->moves_searched << "%) capture, " <<
          stats->pawn_extensions << " (" << 100.0*stats->pawn_extensions/stats->moves_searched << "%) pawn";
#ifdef SINGULAR_EXTENSION
      std::cout << ", " << stats->singular_extensions << " (" << 100.0*stats->singular_extensions/stats->moves_searched << "%) singular" << std::endl;
      std::cout << stats->singular_searches << " singular searches done";
#endif
      std::cout << std::endl;
#endif
      std::cout << stats->tb_probes << " tablebase probes, " <<
         stats->tb_hits << " tablebase hits" << std::endl;
      std::cout << std::flush;
      std::cout.flags(original_flags);
   }

   is_searching = false;

   return best;
}

void SearchController::setContempt(score_t c)
{
    contempt = c;

    // propagate rating diff to searches
    pool->forEachSearch<&Search::setContemptFromController>();
}

void SearchController::setThreadCount(int threads) {
   pool->resize(threads);
}

void SearchController::clearHashTables()
{
    age = 0;
    pool->forEachSearch<&Search::clearHashTables>();
    hashTable.clearHash();
}

void SearchController::stopAllThreads() {
    pool->forEachSearch<&Search::stop>();
}

void SearchController::clearStopFlags() {
    pool->forEachSearch<&Search::clearStopFlag>();
}

void SearchController::updateSearchOptions() {
    // pool size is part of search options and may have changed,
    // so adjust that first:
    pool->resize(globals::options.search.ncpus);
    // update each search thread's local copy of the options:
    pool->forEachSearch<&Search::setSearchOptions>();
}

void SearchController::setTalkLevel(TalkLevel t) {
    talkLevel = t;
    pool->forEachSearch<&Search::setTalkLevelFromController>();
}

void SearchController::uciSendInfos(const Board &board, Move move, int move_index, int depth) {
   if (uci) {
      std::cout << "info depth " << depth;
      std::cout << " currmove ";
      Notation::image(board,move,Notation::OutputFormat::UCI,std::cout);
      std::cout << " currmovenumber " << move_index;
      std::cout << std::endl << std::flush;
   }
}

void SearchController::resizeHash(size_t newSize) {
   hashTable.resizeHash(newSize);
}

void SearchController::outOfBoundsTimeAdjust() {
    // Set flags to extend search time based on search status.
    // Note: do this even if in ponder search, so when we get a ponder
    // hit the flags will be set properly and will be applied to the
    // time target.
    // First find the current best search thread
    Statistics *bestStats = getBestThreadStats(false);
    if (bestStats->failHigh) {
        // root move is failing high, extend time until
        // fail-high is resolved
        fail_high_root_extend = true;
        // We may have initially extended time in the
        // search when first detecting fail-high. If so,
        // reset that flag here.
        fail_high_root = false;
    }
    else if (fail_high_root_extend) {
        fail_high_root_extend = false;
    }
    if (bestStats->failLow) {
        // root move is failing low, extend time until fail-low is resolved
        fail_low_root_extend = true;
    }
    else if (fail_low_root_extend) {
        fail_low_root_extend = false;
    }
}

void SearchController::historyBasedTimeAdjust(const Statistics &stats) {
    // Increase the time limit if pv has changed recently and/or score
    // is dropping over the past few iterations. Decrease limit if
    // score seems stable and is not dropping. Note: we calculate the
    // adjustment even if in a ponder search, so we can apply it later
    // if a ponder hit occurs.
    if (int(stats.depth) > MoveGenerator::EASY_PLIES+6) {
        // Look back over the past few iterations
        Move pv = stats.best_line[0];
        double pvChangeFactor = 0.0;
        const score_t score = stats.display_value;
        score_t max_score = -Constants::MATE;
        int divisor = 1;
        for (int depth = int(stats.depth)-2; depth >= 0 && depth>=int(stats.depth)-6; --depth, divisor *= 2) {
            if (!MovesEqual(rootSearchHistory[depth].pv,pv)) {
                pvChangeFactor += 1.0/divisor;
            }
            score_t old_score = rootSearchHistory[depth].score;
            if (old_score > max_score) {
                max_score = old_score;
            }
            pv = rootSearchHistory[depth].pv;
        }
        score_t score_diff = std::max<score_t>(0,max_score-score-score_t(0.1*Params::PAWN_VALUE));
        double scoreChange = std::max<double>(0.0,score_diff/(1.0*Params::PAWN_VALUE));
        searchHistoryBoostFactor = std::min<double>(1.0,0.25*pvChangeFactor + scoreChange*(1.5 + 0.25*pvChangeFactor));
        searchHistoryReductionFactor = 0.0;
        if (searchHistoryBoostFactor > maxBoostFactor) {
            maxBoostFactor = searchHistoryBoostFactor;
            maxBoostDepth = stats.depth;
        }
        if (searchHistoryBoostFactor==0.0 && maxBoostFactor<0.25) {
            // We have not changed pv recently, score is not dropping,
            // and thinking time was not increased. See if we can
            // reduce the time target.
            searchHistoryReductionFactor = 1.0-maxBoostFactor-0.75*double(maxBoostDepth)/stats.depth;
            assert(searchHistoryReductionFactor<=1.0);
            assert(searchHistoryReductionFactor>=0.0);
        }
    }
}

// Apply search history factor to adjust time control
void SearchController::applySearchHistoryFactors() {
    if (!background &&
        typeOfSearch == TimeLimit &&
        time_target != Constants::INFINITE_TIME &&
        xtra_time &&
        elapsed_time > (unsigned)time_target/3) {
        if (searchHistoryBoostFactor) {
            bonus_time = static_cast<int64_t>(xtra_time*searchHistoryBoostFactor);
        } else {
            bonus_time = -static_cast<int64_t>(std::floor(searchHistoryReductionFactor*elapsed_time/3));
        }
        if (debugOut() && typeOfSearch == TimeLimit && bonus_time) {
            std::cout << globals::debugPrefix << "bonus time=" << bonus_time << std::endl;
        }
    }
}

Search::Search(SearchController *c, ThreadInfo *threadInfo)
   :controller(c),
    iterationDepth(0),
    terminate(0),
    nodeAccumulator(0),
    node(nullptr),
    ti(threadInfo),
    computerSide(White),
    contempt(0),
    age(0),
    talkLevel(c->getTalkLevel())
{
    // Note: context was cleared in its constructor
    setSearchOptions();
    random_engine.seed(getRandomSeed());
}

int Search::checkTime() {
    if (controller->stopped) {
        controller->terminateNow();
    }
    if (terminate) {
       if (debugOut()) std::cout << globals::debugPrefix << "check time, already terminated" << std::endl;
       return 1; // already stopped search
    }

    CLOCK_TYPE current_time = getCurrentTime();
    controller->elapsed_time = getElapsedTime(controller->startTime,current_time);

    // Do not allow time-based termination if the search has not
    // completed even one iteration, since we may be failing low and
    // have no move
    if (controller->stats->completedDepth > 0) {
       if (controller->typeOfSearch == FixedTime) {
          if (controller->elapsed_time >= controller->time_target) {
             return 1;
          }
       }
       else if (controller->typeOfSearch == TimeLimit) {
          if (controller->elapsed_time > controller->getTimeLimit()) {
             if (debugOut()) {
                std::cout << globals::debugPrefix << "terminating, max time reached" << std::endl;
             }
             return 1;
          }
       }
    }

    int val = 0;
    // The following code is always only executed by one of the threads
    if (controller->monitor_function && controller->isMonitorThread(ti->index)) {
        if (controller->monitor_function(controller,stats)) {
            if (debugOut()) {
                std::cout << globals::debugPrefix << "terminating due to program or user input" << std::endl;
            }
            val = 1;
        } else {
            controller->updateGlobalStats(stats);
            if (controller->uci && getElapsedTime(controller->last_time,current_time) >= 3000) {
                const uint64_t total_nodes = controller->totalNodes();
                std::cout << "info";
                if (controller->elapsed_time>300) std::cout << " nps " <<
                                                      (long)((1000L*total_nodes)/controller->elapsed_time);
                std::cout << " nodes " << total_nodes << " hashfull " << controller->hashTable.pctFull() << std::endl;
                controller->last_time = current_time;
            }
        }
    }
    if (val) {
        controller->terminateNow();
    }
    return val;
}

void Search::showStatus(const Board &board, Move best, bool faillow,
bool failhigh)
{
    int ply = stats.depth;
    // Update controller statistics from main thread stats:
    controller->updateGlobalStats(stats);
    if (talkLevel == TalkLevel::Test) {
        // This is the output for the "test" command in verbose mode
        std::ios_base::fmtflags original_flags = std::cout.flags();
        std::cout.setf(std::ios::fixed);
        std::cout << std::setprecision(2);
        std::cout << ply << '\t';
        std::cout << controller->elapsed_time/1000.0 << '\t';
        if (faillow) {
            std::cout << " --";
        }
        else if (best != NullMove) {
            Notation::image(board, best, Notation::OutputFormat::SAN,std::cout);
            if (failhigh) std::cout << '!';
        }
        std::cout << '\t';
        Scoring::printScore(stats.display_value,std::cout);
        // Note: must access controller's stats to get node count
        // across all threads:
        std::cout << '\t' << controller->getGlobalStats().num_nodes << std::endl;
        std::cout.flags(original_flags);
    }
    // Post during ponder if UCI
    if ((!controller->background || controller->uci) && controller->post_function) {
       controller->post_function(*(controller->stats));
    }
}

score_t Search::drawScore(const Board & board) const {
    return controller->drawScore(board,&stats);
}

#ifdef SYZYGY_TBS
score_t Search::tbScoreAdjust(const Board &board,
                    score_t value,int tb_hit,score_t tb_score) const
{
#ifdef _TRACE
   if (mainThread()) {
      std::cout << "tb score adjust: input=";
      Scoring::printScore(tb_score,std::cout);
      std::cout << std::endl;
   }
#endif
   score_t output = value;
   if (tb_hit && !Scoring::mateScore(value)) {
      // If a Syzygy tablebase hit set the score based on that. But
      // don't override a mate score found with search.
      if (tb_score == Constants::TABLEBASE_WIN) {
          output = tb_score;
      }
      else if (tb_score == 0 || std::abs(tb_score) == SyzygyTb::CURSED_SCORE) {
         output = drawScore(board);
      }
      else if (tb_score == -Constants::TABLEBASE_WIN) {
         // loss
         output = -Constants::TABLEBASE_WIN;
      }
   }
#ifdef _TRACE
   if (mainThread()) {
      std::cout << "tb score adjust: output=";
      Scoring::printScore(output,std::cout);
      std::cout << std::endl;
   }
#endif
   return output;
}
#endif

static score_t futilityMargin(int depth)
{
    return FUTILITY_MARGIN_BASE + std::max<int>(depth/DEPTH_INCREMENT,1)*FUTILITY_MARGIN_SLOPE;
}

#ifdef STATIC_NULL_PRUNING
static score_t staticNullPruningMargin(int depth, int improving)
{
    return STATIC_NULL_MARGIN[improving]*std::max<int>(1,depth/DEPTH_INCREMENT);
}
#endif

static int lmpCount(int depth, int improving)
{
    return depth/DEPTH_INCREMENT <= LMP_DEPTH ?
        LMP_MOVE_COUNT[improving][std::max(DEPTH_INCREMENT,depth)/DEPTH_INCREMENT] : Constants::MaxMoves;
}

#ifdef RAZORING
static score_t razorMargin(int depth)
{
    return RAZOR_MARGIN + (std::max<int>(DEPTH_INCREMENT,depth)/DEPTH_INCREMENT)*RAZOR_MARGIN_SLOPE;
}
#endif

static score_t seePruningMargin(int depth, bool quiet)
{
    int p = depth/DEPTH_INCREMENT;
    return quiet ? -p*Params::PAWN_VALUE : -p*p*int(0.2*Params::PAWN_VALUE);
}

void Search::setVariablesFromController() {
   computerSide = controller->computerSide;
   talkLevel = controller->talkLevel;
   contempt = controller->contempt;
   age = controller->age;
}

void Search::setContemptFromController() {
   contempt = controller->contempt;
}

void Search::setTalkLevelFromController() {
   talkLevel = controller->talkLevel;
}

void Search::updateStats(const Board &board, NodeInfo *node, int iteration_depth,
                         score_t score)
{
    assert(stats.multipv_count < Statistics::MAX_PV);
    stats.value = score;
    stats.depth = iteration_depth;
    stats.display_value = stats.value;
#ifdef SYZYGY_TBS
    // Correct if necessary the display value, used for score
    // output and resign decisions, based on the tb information:
    if (stats.tb_value != Constants::INVALID_SCORE) {
       stats.display_value = tbScoreAdjust(board,
                                           stats.value,
                                           1,
                                           stats.tb_value);
    }
#endif
    // note: retain previous best line if we do not have one here
    if (IsNull(node->pv[0])) {
#ifdef _TRACE
        if (mainThread()) std::cout << globals::debugPrefix << "warning: pv is null\n";
#endif
        return;
    }
    else if (node->pv_length == 0) {
        return;
    }
    node->best = node->pv[0];                     // ensure "best" is non-null
    assert(!IsNull(node->best));
    Board board_copy(board);
    stats.best_line[0] = NullMove;
    int i = 0;
    stats.best_line_image.clear();
    std::stringstream sstr;
    const Move *moves = node->pv;
    while (i < node->pv_length && i<Constants::MaxPly-1 && !IsNull(moves[i])) {
       Move move = moves[i];
       stats.best_line[i] = move;
       assert(legalMove(board_copy,move));
       if (i!=0) {
          sstr << ' ';
       }
       Notation::image(board_copy, move,
                       controller->uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::SAN,sstr);
       int len = (int)sstr.tellg();
       // limit the length
       if (len > 250) {
          break;
       }
       board_copy.doMove(move);
       ++i;
       if (board_copy.isLegalDraw()) {
          break;
       }
       if (node->pv_length < 2) {
          // get the next move from the hash table, if possible
          // (for pondering)
          HashEntry entry;
          HashEntry::ValueType result =
              controller->hashTable.searchHash(board_copy.hashCode(board.repCount(2)),
                                               age,
                                               iteration_depth,entry);
          if (result != HashEntry::NoHit) {
             Move hashMove = entry.bestMove(board_copy);
             if (!IsNull(hashMove)) {
                stats.best_line[i] = hashMove;
                if (i!=0) sstr << ' ';
                Notation::image(board_copy,hashMove,
                                controller->uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::SAN,sstr);
                ++i;
             }
             break;
          }
       }
    }
    stats.best_line[i] = NullMove;
    stats.best_line_image = sstr.str();
}

void Search::suboptimal(RootMoveGenerator &mg,Move &m, score_t &val) {
    if (mg.moveCount() < 2) {
        return;
    }
    mg.reorderByScore();
    const int &strength = globals::options.search.strength;
    std::normal_distribution<double> dist(0.0,static_cast<score_t>(2.4*Params::PAWN_VALUE*(100-strength)/100.0));
    score_t tolerance = std::max<int>(0,75-strength)*Params::PAWN_VALUE/150 + static_cast<score_t>(std::fabs(dist(random_engine)));
    std::uniform_int_distribution<int> dist2(1,128);

    // In reduced strength mode sometimes, deliberately choose a move
    // that is not the best
    int ord;
    score_t best = val;
    const int x = std::max<int>(0,100-strength-(strength*strength)/200);
    for (int i = 0; i < 4; i++) {
        Move move = (board.checkStatus() == InCheck ? mg.nextEvasion(ord) :
                  mg.nextMove(ord));
        if (IsNull(move)) break;
        if (i == 0) {
            best = mg.getScore(move);
        }
        else {
            const score_t score = mg.getScore(move);
            const int r = dist2(random_engine);
            if ((best - score <= tolerance && r < x) ||
                r < x/(2+strength/10)) {
                if (mainThread() && controller->debugOut()) {
                    std::cout << globals::debugPrefix <<
                        "suboptimal: index= " << i <<
                        " diff=" << best - score <<
                        " tolerance=" << tolerance << std::endl;
                }
            }
            m = move;
            val = score;
        }
    }
}

Move Search::ply0_search()
{
   node->best = NullMove;
   // Incrementally search the board to greater depths - stop when
   // ply limit, time limit, interrupt, or a terminating condition
   // is reached.
   // Search the first few iterations with a wide window - for easy
   // move detection.
   score_t value = controller->initialValue;
   // Initialize this thread's move generator from the controller's,
   // which will have the ranked, sorted, filtered move list.
   RootMoveGenerator mg(*(controller->mg),&context);
   stats.multipv_limit = std::min<int>(mg.moveCount(),globals::options.search.multipv);
   iterationDepth = 0;
   while ((iterationDepth = controller->nextSearchDepth(iterationDepth,ti->index,
                                                        controller->ply_limit)) <= controller->ply_limit &&
          !terminate) {
      MoveSet excluded(controller->exclude);
      for (stats.multipv_count = 0;
           stats.multipv_count < stats.multipv_limit && !terminate;
           stats.multipv_count++) {
         score_t lo_window, hi_window;
         score_t aspirationWindow = ASPIRATION_WINDOW[0];

         if (srcOpts.multipv > 1) {
             stats.clearSearchState();
             if (iterationDepth > 1) {
                 // set value to previous iteration's value
                 value = stats.multi_pvs[stats.multipv_count].display_value;
             } else {
                 value = controller->initialValue;
             }
         }
         if (stats.multipv_count) {
            // Exclude the previous best move from the current
            // search, so we will select a different one.
            excluded.emplace(stats.multi_pvs[stats.multipv_count-1].best);
         }
         if (iterationDepth <= 1) {
            lo_window = -Constants::MATE;
            hi_window = Constants::MATE;
         } else if (iterationDepth <= MoveGenerator::EASY_PLIES) {
            lo_window = std::max<score_t>(-Constants::MATE,value - WIDE_WINDOW);
            hi_window = std::min<score_t>(Constants::MATE,value + WIDE_WINDOW + aspirationWindow/2);
         } else {
            lo_window = std::max<score_t>(-Constants::MATE,value - aspirationWindow/2);
            hi_window = std::min<score_t>(Constants::MATE,value + aspirationWindow/2);
         }
         if (mainThread() && debugOut() && controller->background) {
             std::cout << globals::debugPrefix;
             if (srcOpts.multipv > 1) {
                 std::cout << " multipv=" << stats.multipv_count << ": ";
             }
             std::cout << iterationDepth << ". move=";
             MoveImage(node->best,std::cout); std::cout << " score=";
             Scoring::printScore(node->best_score,std::cout);
             std::cout << " terminate=" << terminate << std::endl;
         }
         int fails = 0;
         int faillows = 0, failhighs = 0;
         do {
            stats.failHigh = stats.failLow = false;
#ifdef _TRACE
            if (mainThread()) {
               std::cout << "iteration " << iterationDepth << " window = [";
               Scoring::printScore(lo_window,std::cout);
               std::cout << ',';
               Scoring::printScore(hi_window,std::cout);
               std::cout << ']' << std::endl;
            }
#endif
            value = ply0_search(mg, lo_window, hi_window, iterationDepth,
                                DEPTH_INCREMENT*iterationDepth,
                                excluded);
            // If we did not even search one move in this iteration,
            // leave the search stats intact (with the previous
            // iteration's pv and score).
            if (!terminate || stats.mvleft != stats.mvtot) {
                updateStats(board, node, iterationDepth, value);
            }
#ifdef _TRACE
            if (mainThread()) {
               std::cout << "iteration " << iterationDepth << " raw result: ";
               Scoring::printScore(stats.value,std::cout);
               std::cout << " corrected result: ";
               Scoring::printScore(stats.display_value,std::cout);
               std::cout << std::endl;
            }
#endif
            StateType &state = stats.state;
            if (!terminate && (state == Checkmate || state == Stalemate)) {
                std::cout << globals::debugPrefix << "terminating due to checkmate or statemate, state="
                          << (int)state << std::endl;
                controller->terminateNow();
                break;
            }
            if (controller->elapsed_time > 200) {
               controller->timeCheckInterval = int((20L*stats.num_nodes)/(controller->elapsed_time*NODE_ACCUM_THRESHOLD));
               if ((int)controller->time_limit - (int)controller->elapsed_time < 100) {
                  controller->timeCheckInterval /= 2;
               }
               if (mainThread() && debugOut()) {
                  std::cout << globals::debugPrefix << "time check interval=" << controller->timeCheckInterval << " elapsed_time=" << controller->elapsed_time << " target=" << controller->getTimeLimit() << std::endl;
               }
            }
            stats.failHigh = value >= hi_window && (hi_window < Constants::MATE-iterationDepth-1);
            stats.failLow = value <= lo_window  && (lo_window > iterationDepth-Constants::MATE);
            if (stats.failLow) {
                faillows++;
            }
            else if (stats.failHigh) {
                failhighs++;
            }
            // store root search history entry
            if (mainThread()) {
                controller->rootSearchHistory[iterationDepth-1] = SearchController::SearchHistory(
                    node->best, stats.display_value);
            }
            // Peform any temporary adjustment of the time allocation based
            // on search status (fail high/low). Note: all threads call this;
            // adjustment is based on the current best search thread.
            controller->outOfBoundsTimeAdjust();
            // Show status (if main thread) and adjust aspiration
            // window as needed
            if (stats.failHigh) {
                if (mainThread()) {
                    if (stats.multipv_limit == 1) {
                        showStatus(board, node->best, stats.failLow, stats.failHigh);
                    }
                    if (debugOut()) {
                        std::cout << globals::debugPrefix << "ply 0 fail high, re-searching ... value=";
                        Scoring::printScore(value,std::cout);
                        std::cout << " fails=" << fails+1 << std::endl;
                    }
#ifdef _TRACE
                    std::cout << globals::debugPrefix << "ply 0 high cutoff, re-searching ... value=";
                    Scoring::printScore(value,std::cout);
                    std::cout << " fails=" << fails+1 << std::endl;
#endif
                }
                if (fails+1 >= ASPIRATION_WINDOW_STEPS) {
                    aspirationWindow = Constants::MATE;
                }
                else {
                    aspirationWindow = ASPIRATION_WINDOW[++fails];
                }
                if (aspirationWindow == Constants::MATE) {
                    hi_window = Constants::MATE-iterationDepth-1;
                } else {
                    if (iterationDepth <= MoveGenerator::EASY_PLIES) {
                        aspirationWindow += 2*WIDE_WINDOW;
                    }
                    hi_window = std::min<score_t>(Constants::MATE-iterationDepth-1,
                                                  lo_window + aspirationWindow);
                }
            }
            else if (stats.failLow) {
                if (mainThread()) {
                    if (stats.multipv_limit == 1) {
                        showStatus(board, node->best, stats.failLow, stats.failHigh);
                    }
                    if (debugOut()) {
                        std::cout << globals::debugPrefix << "ply 0 fail low, re-searching ... value=";
                        Scoring::printScore(value,std::cout);
                        std::cout << " fails=" << fails+1 << std::endl;
                    }
#ifdef _TRACE
                    std::cout << globals::debugPrefix << "ply 0 fail low, re-searching ... value=";
                    Scoring::printScore(value,std::cout);
                    std::cout << " fails=" << fails+1 << std::endl;
#endif
                }
                // continue loop with lower bound
                if (fails+1 >= ASPIRATION_WINDOW_STEPS) {
                    // TBD: Sometimes we can fail low after a bunch of fail highs. Allow the
                    // search to continue, but set the lower bound to the bottom of the range.
                    if (mainThread() && debugOut()) {
                        std::cout << globals::debugPrefix << "too many aspiration window steps, setting window to max width" << std::endl;
                    }
                    aspirationWindow = Constants::MATE;
                }
                else if (Scoring::mateScore(value)) {
                    // We got a mate score so don't bother doing any
                    // more aspiration steps, just widen to the max.
                    aspirationWindow = Constants::MATE;
                }
                else {
                    aspirationWindow = ASPIRATION_WINDOW[++fails];
                }
                if (aspirationWindow == Constants::MATE) {
                    // We can miss shallow mates but then find them in
                    // later iterations. Set the window to -Mate1 so we
                    // will never fail low and not get a pv.
                    lo_window = 1-Constants::MATE;
                } else {
                    if (iterationDepth <= MoveGenerator::EASY_PLIES) {
                        aspirationWindow += 2*WIDE_WINDOW;
                    }
                    lo_window = std::max<score_t>(iterationDepth-Constants::MATE,hi_window - aspirationWindow);
                }
            }
            // check time after adjustments have been made. Do not
            // allow time-based exit without one completed iteration though.
            if (!terminate) {
                if (checkTime()) {
                    if (debugOut()) {
                        std::cout << globals::debugPrefix << "time up" << std::endl;
                    }
                    controller->terminateNow();
                }
            }
         } while (!terminate && (stats.failLow || stats.failHigh));
         // Search value should now be in bounds (unless we are terminating)

         // Check for forced move, but only at depth 2 (so we get a
         // ponder move if possible).
         // Do not terminate here if a resign score is returned
         // (search deeper to get an accurate score). Do not exit
         // in analysis mode.
         if (!terminate && controller->typeOfSearch != FixedDepth &&
             !(controller->background || (controller->typeOfSearch == FixedTime && controller->time_target == Constants::INFINITE_TIME)) &&
             mg.moveCount() == 1 &&
             iterationDepth >= 2 &&
             !(srcOpts.can_resign && stats.display_value <= srcOpts.resign_threshold)) {
             if (mainThread() && debugOut()) {
                 std::cout << globals::debugPrefix << "single legal move, terminating" << std::endl;
             }
             controller->terminateNow();
         }
         if (!terminate) {
            if (mainThread()) {
                // Peform any adjustment of the time allocation based
                // on search status and history
                controller->historyBasedTimeAdjust(stats);
                controller->applySearchHistoryFactors();
                /*
                if (debugOut() && (controller->searchHistoryBoostFactor != 0 || controller->searchHistoryReductionFactor !=0)) {
                    std::cout << globals::debugPrefix << "searchHistoryBoostFactor=" << controller->searchHistoryBoostFactor <<
                        " searchHistoryReductionFactor=" << controller->searchHistoryReductionFactor << std::endl;
                }
                */
            }
            stats.completedDepth = iterationDepth;
            if (srcOpts.multipv > 1) {
               // Accumulate multiple pvs until we are ready to output
               // them.
               stats.multi_pvs[stats.multipv_count] = Statistics::MultiPVEntry(stats);
            }
#ifdef _TRACE
            if (mainThread()) {
               std::cout << iterationDepth << " ply search result: ";
               MoveImage(node->best,std::cout);
               std::cout << " value = ";
               Scoring::printScore(value,std::cout);
               std::cout << std::endl;
            }
#endif
            if (!controller->uci || controller->typeOfSearch == TimeLimit) {
                if (value <= iterationDepth - Constants::MATE && !IsNull(stats.best_line[0])) {
                    // We're either checkmated or we certainly will be, so
                    // quit searching.
                    if (mainThread() && debugOut()) std::cout << globals::debugPrefix << "terminating, low score" << std::endl;
#ifdef _TRACE
                    std::cout << "terminating, low score" << std::endl;
#endif
                    controller->terminateNow();
               }
               else if (value >= Constants::MATE - iterationDepth - 1 && iterationDepth>=2) {
                   // found a forced mate, terminate
                   if (mainThread() && debugOut()) std::cout << globals::debugPrefix << "terminating, high score" << std::endl;
#ifdef _TRACE
                   std::cout << "terminating, high score" << std::endl;
#endif
                   controller->terminateNow();
               }
            }
         }
      } // end multi-pv loop
      if (stats.multipv_limit>1) {
          // keep lines in score order
          stats.sortMultiPVs();
      }
      if (mainThread()) {
         showStatus(board, node->best, false, false);
      }
   } // end depth iteration loop
   // Make sure we have an active monitor thread
   controller->pool->lock();
   if (controller->monitor_function && controller->isMonitorThread(ti->index)) {
       for (unsigned i = 0; i < controller->pool->size(); ++i) {
          if (i != ti->index && !controller->pool->isCompletedNoLock(i)) {
              // appoint this the monitor thread
              controller->setMonitorThread(i);
              break;
          }
       }
   }
   controller->pool->unlock();
   if (mainThread() && debugOut()) {
       if (iterationDepth >= controller->ply_limit) {
           std::cout << globals::debugPrefix << "exiting search due to max depth" << std::endl;
       }
       std::cout << globals::debugPrefix << "out of search loop, move= ";
       MoveImage(node->best,std::cout);
       std::cout << std::endl << std::flush;
   }
   // In reduced-strength mode, sometimes play a suboptimal move
   if (globals::options.search.strength < 100 && static_cast<int>(stats.completedDepth) <= MoveGenerator::EASY_PLIES) {
      score_t val = stats.display_value;
      Move best = node->best;
      suboptimal(mg,best,val);
      if (!MovesEqual(node->best,best)) {
           node->best = best;
           stats.display_value = stats.value = val;
           stats.best_line[0] = best;
           stats.best_line[1] = NullMove;
           Notation::image(board,best,
                           controller->uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::SAN,stats.best_line_image);
      }
   }
   return node->best;
}

score_t Search::ply0_search(RootMoveGenerator &mg, score_t alpha, score_t beta,
                        int iterationDepth,
                        int depth,
                        const MoveSet &exclude)
{
    // implements alpha/beta search for the top most ply.  We use
    // the negascout algorithm.

    --controller->time_check_counter;
    nodeAccumulator++;

    int in_check = 0;

    const bool wide = iterationDepth <= MoveGenerator::EASY_PLIES;

    in_check = (board.checkStatus() == InCheck);
    BoardState save_state = board.state;

    score_t try_score = alpha;
    //
    // Re-sort the ply 0 moves and re-init move generator.
    if (iterationDepth>1) {
       mg.reorder(node->best,node->best_score,iterationDepth,false);
    } else {
       mg.reset();
    }
    stats.mvtot = stats.mvleft = mg.moveCount();

    // if in N-variation mode, exclude any moves we have searched already
    mg.exclude(exclude);

    //
    // Search the next ply
    //
    node->pv[0] = NullMove;
    node->pv_length = 0;
    node->cutoff = 0;
    node->num_quiets = node->num_legal = 0;
    node->alpha = alpha;
    node->beta = beta;
    node->best_score = node->alpha;
#ifdef MOVE_ORDER_STATS
    node->best_count = 0;
#endif
    node->ply = 0;
    node->depth = depth;
    node->excluded = NullMove;
    node->eval = Constants::INVALID_SCORE;

    int move_index = 0;
    score_t hibound = beta;
    while (!node->cutoff && !terminate) {
        Move move;
        if ((move = mg.nextMove(move_index))==NullMove) break;
        if (IsUsed(move) || IsExcluded(move)) {
            --stats.mvleft;
            continue;     // skip move
        }
        node->last_move = move;
        if (!CaptureOrPromotion(move)) {
            node->quiets[node->num_quiets++] = move;
        }
        node->num_legal++; // all generated moves are legal at ply 0
        if (mainThread() && controller->uci && controller->elapsed_time > 300) {
            controller->uciSendInfos(board, move, move_index, iterationDepth);
        }
#ifdef _TRACE
        if (mainThread()) {
           std::cout << "trying 0. ";
           MoveImage(move,std::cout);
           std::cout << " (" << move_index << "/" << mg.moveCount();
           std::cout << ")" << std::endl;
        }
#endif
        CheckStatusType in_check_after_move = board.wouldCheck(move);
        node->swap = Constants::INVALID_SCORE;
        // calculate extensions/reductions. No pruning at ply 0.
        int extension = extend(board, node, in_check_after_move, move);
        int newDepth = depth - DEPTH_INCREMENT + extension;
        int reduction = reduce(board, node, move_index, 1, newDepth, move);
#ifdef SEARCH_STATS
        if (reduction) ++stats.reduced;
#endif
        board.doMove(move,node);
        setCheckStatus(board,in_check_after_move);
        score_t lobound = wide ? node->alpha : node->best_score;
#ifdef _TRACE
        if (mainThread()) {
           std::cout << "window [" << -hibound << ", " << -lobound <<
              "]" << std::endl;
        }
#endif
        if (newDepth - reduction > 0) {
           try_score = -search(-hibound, -lobound,
                               1, newDepth - reduction);
        }
        else {
           try_score = -quiesce(-hibound, -lobound, 1, 0);
        }
        if (terminate) {
            board.undoMove(move,save_state);
            break;
        }
        if (try_score > node->best_score && reduction && !terminate) {
#ifdef _TRACE
           if (mainThread()) {
              std::cout << "window = [" << -hibound << "," << -lobound
                   << "]" << std::endl;
              std::cout << "score = " << try_score << " - LMR cutoff, researching .." << std::endl;
           }
#endif
           // We failed to get a cutoff, so re-search with no reduction.
           controller->fail_high_root = true;
           if (newDepth > 0)
              try_score=-search(-hibound,-lobound,1,newDepth);
           else
              try_score=-quiesce(-hibound,-lobound,1,0);
        }
        if (try_score > node->best_score && hibound < node->beta && !terminate) {
           if (mainThread() && controller->time_target != Constants::INFINITE_TIME) {
              controller->fail_high_root = true;
              if (debugOut()) {
                  std::cout << globals::debugPrefix << "researching at root, extending time" << std::endl;
              }
           }
#ifdef _TRACE
           if (mainThread()) {
              std::cout << "window = [" << -hibound << "," << -lobound
                   << "]" << std::endl;
              std::cout << "score = " << try_score << " - zero window cutoff, researching .." << std::endl;
           }
#endif
           hibound = node->beta;
           if (newDepth > 0)
              try_score=-search(-hibound,-lobound,1,newDepth);
           else
              try_score=-quiesce(-hibound,-lobound,1,0);
        }
#ifdef _TRACE
        if (mainThread()) {
            std::cout << "0. ";
            MoveImage(move,std::cout);
            std::cout << ' ' << try_score;
            std::cout << std::endl;
        }
#endif
        board.undoMove(move,save_state);
        if (wide) {
            mg.setScore(move_index,try_score);
        }
        // We have now resolved the fail-high if there is one.
        if (try_score > node->best_score && !terminate) {
           if (updateRootMove(board,node,move,try_score,move_index)) {
              // beta cutoff
              // ensure we send UCI output .. even in case of quick
              // termination due to checkmate or whatever
              if (mainThread() && !srcOpts.multipv) controller->uciSendInfos(board, move, move_index, iterationDepth);
              // keep fail_high_root true so we don't terminate
              break;
           }
        }
        if (mainThread()) {
           if (debugOut() && controller->fail_high_root) {
               std::cout << globals::debugPrefix << "resetting fail_high_root" << std::endl;
           }
           controller->fail_high_root = false;
        }
        if (!terminate) {
           // We got an accurate (non beta cutoff) score for the move,
           // so count it as analyzed
           --stats.mvleft;
        }
        if (globals::options.search.strength < 100 &&
            controller->time_target != Constants::INFINITE_TIME &&
            iterationDepth == controller->ply_limit &&
            move_index < mg.moveCount()) {
            // we are on the last iteration in reduced-strength mode,
            // see if we should slow down
            controller->elapsed_time = getElapsedTime(controller->startTime,getCurrentTime());
            auto limit = controller->getTimeLimit();
            if (controller->elapsed_time < 2*limit/3) {
                // waste some time
                int thisWait = 0, waitTime;
                // sleep in increments if the wait time is long
                while ((waitTime = (limit - controller->elapsed_time)/(4*(mg.moveCount()-move_index))) >= 50 && !terminate) {
                    thisWait = std::min<int>(waitTime,1000);
                    if (mainThread() && debugOut()) {
                        std::cout << globals::debugPrefix << "index=" << move_index << " waiting for " << thisWait << " ms." << std::endl;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(thisWait));
                    // check for input and update elapsed time
                    checkTime();
                }
            }
        }
        if (!wide) {
           hibound = node->best_score + 1;  // zero-width window
        }
    }

    if (node->cutoff) {
        return node->best_score;
    }
    if (node->num_legal == 0) {
        // no moves were tried
        if (in_check) {
            if (mg.moveCount() == 0) {           // mate
                node->best_score = -(Constants::MATE);
                if (!srcOpts.multipv) stats.state = Checkmate;
            }
        }
        else {                                    // stalemate
            if (!srcOpts.multipv) stats.state = Stalemate;
#ifdef _TRACE
            if (mainThread()) {
               std::cout << "stalemate!" << std::endl;
            }
#endif
            node->best_score = drawScore(board);
        }
    }
    else if (!IsNull(node->best) && !CaptureOrPromotion(node->best) &&
             board.checkStatus() != InCheck) {
        context.setKiller(node->best, node->ply);
        context.updateStats(board, node);
    }
#ifdef MOVE_ORDER_STATS
    if (node->num_legal && node->best_score > node->alpha) {
        stats.move_order_count++;
        if (node->best_count<4) {
            stats.move_order[node->best_count]++;
        }
    }
#endif
#ifdef _DEBUG
    if (node->best_score < -Constants::MATE ||
        node->best_score > Constants::MATE) {
        std::cout << globals::debugPrefix << board << std::endl;
        assert(0);
    }
#endif
    stats.num_nodes += nodeAccumulator;
    nodeAccumulator = 0;
    return node->best_score;
}

void SearchController::updateGlobalStats(const Statistics &mainStats) {
    *stats = mainStats;
    // Make sure the root probe is counted
    stats->tb_probes = tb_root_probes;
    stats->tb_hits = tb_root_hits;
    // clear all counters
    stats->num_nodes = 0ULL;
#ifdef SEARCH_STATS
    stats->num_qnodes = stats->reg_nodes = stats->moves_searched = stats->static_null_pruning =
       stats->razored = stats->reduced = (uint64_t)0;
    stats->hash_hits = stats->hash_searches = stats->futility_pruning = stats->null_cuts = (uint64_t)0;
    stats->history_pruning = stats->lmp = stats->see_pruning = (uint64_t)0;
    stats->check_extensions = stats->capture_extensions =
    stats->pawn_extensions = stats->singular_extensions = 0L;
#endif
#ifdef MOVE_ORDER_STATS
    stats->move_order_count = 0;
    for (int i = 0; i < 4; i++) stats->move_order[i] = 0;
#endif
    // Sum all counters across threads
    unsigned bestCompleted = 0;
    stats->completedDepth = 0;
    for (unsigned i = 0; i < pool->nThreads; i++) {
       if (pool->data[i]->work == nullptr) continue;
       const Statistics &s = pool->data[i]->work->stats;
       stats->tb_probes += s.tb_probes;
       stats->tb_hits += s.tb_hits;
       stats->num_nodes += s.num_nodes;
       if (s.completedDepth > bestCompleted) {
           stats->completedDepth = bestCompleted = s.completedDepth;
       }
#ifdef SEARCH_STATS
       stats->num_qnodes += s.num_qnodes;
       stats->reg_nodes += s.reg_nodes;
       stats->moves_searched += s.moves_searched;
       stats->futility_pruning += s.futility_pruning;
       stats->static_null_pruning += s.static_null_pruning;
       stats->null_cuts += s.null_cuts;
       stats->razored += s.razored;
       stats->check_extensions += s.check_extensions;
       stats->capture_extensions += s.capture_extensions;
       stats->pawn_extensions += s.pawn_extensions;
       stats->singular_extensions += s.singular_extensions;
       stats->reduced += s.reduced;
       stats->lmp += s.lmp;
       stats->history_pruning += s.history_pruning;
       stats->see_pruning += s.see_pruning;
       stats->hash_hits += s.hash_hits;
       stats->hash_searches += s.hash_searches;
#endif
#ifdef MOVE_ORDER_STATS
       stats->move_order_count += s.move_order_count;
       for (int i = 0; i < 4; i++) stats->move_order[i] += s.move_order[i];
#endif
    }
}

Statistics *SearchController::getBestThreadStats(bool trace) const
{
    Statistics * best = stats;
    for (int thread = 1; thread < globals::options.search.ncpus; thread++) {
        if (pool->data[thread]->work == nullptr) continue;
        Statistics &threadStats = pool->data[thread]->work->stats;
        if (trace) {
            std::cout << globals::debugPrefix << "thread " << thread << " depth=" <<
                threadStats.completedDepth << " score=";
            Scoring::printScore(threadStats.display_value,std::cout);
            std::cout << " failHigh=" << (int)threadStats.failHigh << " failLow=" <<
                (int)threadStats.failLow;
            std::cout << " pv=" << threadStats.best_line_image << std::endl;
        }
        if (!IsNull(threadStats.best_line[0])) {
            if (IsNull(best->best_line[0]) ||
                (threadStats.display_value > best->display_value &&
                (threadStats.completedDepth >= best->completedDepth ||
                 threadStats.value >= Constants::MATE_RANGE))) {
                best = &threadStats;
            }

        }
    }
    return best;
}

unsigned SearchController::nextSearchDepth(unsigned current_depth, unsigned thread_id,
    unsigned max_depth)
{
    unsigned d = current_depth+1;
    const unsigned ncpus = unsigned(globals::options.search.ncpus);
    std::unique_lock<std::mutex> lock(search_count_mtx);
    if (thread_id > 0) {
        if (current_depth == 0) {
            if (d < max_depth) d += ((thread_id+1) % 2 == 0);
            if (d < max_depth) d += ((thread_id+1) % 4 == 0);
        } else {
            unsigned inc = 1;
            while (inc < 8 && d < max_depth) {
                unsigned threshold = ncpus/(1<<inc);
                if (threshold == 0 || search_counts[d] < threshold) {
                    break;
                } else {
                    ++d;
                    ++inc;
                }
            }
        }
    }
    if (d<Constants::MaxPly) {
        search_counts[current_depth]--;
        search_counts[d]++;
    }
    return d;
}

#ifdef _TRACE
static void traceHash(char type, NodeInfo *node, score_t hashValue, const HashEntry &hashEntry)
{
    indent(node->ply);
    std::cout << "hash cutoff, type = " << type <<
        " alpha = " << node->alpha <<
        " beta = " << node->beta <<
        " value = " << hashValue <<
        " depth = " << hashEntry.depth() << std::endl;
}
#endif

score_t Search::quiesce(int ply,int depth)
{
   // recursive function, implements quiescence search.
   //
   assert(ply < Constants::MaxPly);
   if (++nodeAccumulator > NODE_ACCUM_THRESHOLD) {
      stats.num_nodes += nodeAccumulator;
      nodeAccumulator = 0;
#ifdef SMP_STATS
      --controller->sample_counter;
#endif
      if (--controller->time_check_counter <= 0) {
         controller->time_check_counter = controller->timeCheckInterval;
         if (checkTime()) {
            if (debugOut()) {
                std::cout << globals::debugPrefix << "terminating, time up" << std::endl;
            }
            controller->terminateNow();   // signal all searches to end
         }
      }
   }
   assert(depth<=0);
#ifdef SEARCH_STATS
   stats.num_qnodes++;
#endif
   int rep_count;
   if (terminate) return node->alpha;
   else if (maxDepth(node)) {
      if (!board.wasLegal((node-1)->last_move)) {
         return -Illegal;
      }
      node->flags |= EXACT;
      return evalu8(board);
   }
   else if (Scoring::isDraw(board,rep_count,ply)) {
#ifdef _TRACE
      if (mainThread()) {
         indent(ply); std::cout << "draw!" << std::endl;
      }
#endif
      node->flags |= EXACT;
      return drawScore(board);
   }
#ifdef _TRACE
   if (mainThread()) {
      indent(ply); std::cout << "window [" << node->alpha << ","
                        << node->beta << "]" << std::endl;
   }
#endif
   node->eval = node->staticEval = Constants::INVALID_SCORE;
   const hash_t hash = board.hashCode(rep_count);
   // Like Stockfish, only distinguish depths with checks vs depth without
   int tt_depth;
   const int inCheck = board.checkStatus((node-1)->last_move)==InCheck;
   if (inCheck || depth >= 1-srcOpts.checks_in_qsearch) {
      tt_depth = HashEntry::QSEARCH_CHECK_DEPTH;
   }
   else {
      tt_depth = HashEntry::QSEARCH_NO_CHECK_DEPTH;
   }
   Move hashMove = NullMove;
   score_t hashValue = Constants::INVALID_SCORE;
   HashEntry hashEntry;
   // Note: we copy the hash entry .. so mods by another thread do not
   // alter the copy
   HashEntry::ValueType result = controller->hashTable.searchHash(hash,
                                                                  tt_depth,age,hashEntry);
#ifdef SEARCH_STATS
   stats.hash_searches++;
#endif
   bool hashHit = (result != HashEntry::NoHit);
   if (hashHit) {
      // a valid hashtable entry was found
#ifdef SEARCH_STATS
      stats.hash_hits++;
#endif
      node->staticEval = hashEntry.staticValue();
      hashValue = HashEntry::hashValueToScore(hashEntry.getValue(),node->ply);
      if (result == HashEntry::Valid ||
           ((result == HashEntry::UpperBound && hashValue <= node->alpha) ||
            (result == HashEntry::LowerBound && hashValue >= node->beta))) {
          // hash cutoff
#ifdef _TRACE
          static constexpr char map[3] = {'E','U','L'};
          if (mainThread()) {
              traceHash(map[int(result)],node,hashValue,hashEntry);
          }
#endif
          return hashValue;
      }
      // Note: hash move may be usable even if score is not usable
      hashMove = hashEntry.bestMove(board);
      // Ensure the hash move if any, meets the conditions for this
      // depth in the q-search:
      if (!IsNull(hashMove) && hashEntry.depth() != tt_depth && !inCheck) {
          if (tt_depth == HashEntry::QSEARCH_NO_CHECK_DEPTH) {
              if (!CaptureOrPromotion(hashMove)) {
                  hashMove = NullMove;
              }
          } else {
              if (!
                  (CaptureOrPromotion(hashMove) ||
                   ((node->eval >= node->alpha - 2*Params::PAWN_VALUE) &&
                    board.wouldCheck(hashMove)))) {
                 hashMove = NullMove;
              }
          }
      }
   }
   if (inCheck) {
#ifdef _TRACE
       if (mainThread()) {
           indent(ply); std::cout << "in_check=1" << std::endl;
       }
#endif
       assert(board.anyAttacks(board.kingSquare(board.sideToMove()),board.oppositeSide()));
       score_t try_score;
       MoveGenerator mg(board, &context, node, ply, hashMove, mainThread());
       Move move;
       BoardState state = board.state;
       node->num_legal = 0;
       int noncaps = 0;
       int moveIndex = 0;
       while ((move = mg.nextEvasion(moveIndex)) != NullMove) {
           assert(OnBoard(StartSquare(move)));
           if (Capture(move) == King) {
#ifdef _TRACE
               if (mainThread()) {
                   indent(ply);
                   std::cout << "previous move illegal, exiting qsearch" << std::endl;
               }
#endif
               return -Illegal;
           }
#ifdef _TRACE
           if (mainThread()) {
               indent(ply);
               std::cout << "trying " << ply << ". ";
               MoveImage(move,std::cout);
               std::cout << std::endl;
           }
#endif
           if (!node->PV() &&
               noncaps > std::max<int>(1+depth,0) &&
               !Scoring::mateScore(node->beta) &&
               !CaptureOrPromotion(move) &&
               board.wouldCheck(move) == NotInCheck) {
               // We have searched one or more legal non-capture evasions
               // and failed to cutoff. So don't search any more.
#ifdef _TRACE
               if (mainThread()) {
                   indent(ply); std::cout << "pruned" << std::endl;
               }
#endif
               continue;
           }
           node->last_move = move;
           board.doMove(move,node);
           if (!board.wasLegal(move,true)) {
               board.undoMove(move,state);
               continue;
           }
           assert(!board.anyAttacks(board.kingSquare(board.oppositeSide()),board.sideToMove()));
           try_score = -quiesce(-node->beta, -node->best_score, ply+1, depth-1);
           board.undoMove(move,state);
           if (try_score != Illegal) {
#ifdef _TRACE
               if (mainThread()) {
                   indent(ply);
                   std::cout << ply << ". ";
                   MoveImage(move,std::cout);
                   std::cout << ' ' << try_score << std::endl;
               }
#endif
               node->num_legal++;
               if (try_score > node->best_score) {
                   node->best_score = try_score;
                   node->best = move;
                   if (try_score >= node->beta)
                       break;
                   if (node->best_score >= Constants::MATE-1-ply)
                       break;                        // mating move found
               }
               if (!CaptureOrPromotion(move)) noncaps++;
           }
#ifdef _TRACE
           else if (mainThread()) {
               indent(ply);
               std::cout << ply << ". ";
               MoveImage(move,std::cout);
               std::cout << " (illegal)" << std::endl;
           }
#endif
       }
       if (node->num_legal == 0) { // no legal evasions, so this is checkmate
           (node+1)->pv_length=0; // no PV from this point
           if (!IsNull(node->excluded)) {
               node->best_score = node->alpha;
           } else {
#ifdef _TRACE
               if (mainThread()) {
                   indent(ply); std::cout << "checkmate!" << std::endl;
               }
#endif
               node->best_score = -(Constants::MATE - ply);
               node->flags |= EXACT;
           }
       }
#ifdef _DEBUG
       if (node->best_score < -Constants::MATE ||
           node->best_score > Constants::MATE) {
           std::cout << globals::debugPrefix << board << std::endl;
           assert(0);
       }
#endif
       storeHash(hash,node->best,tt_depth);
       if (node->inBounds(node->best_score)) {
           if (!IsNull(node->best)) {
               updatePV(board,node->best,ply);
           }
       }
       return node->best_score;
   }
   else {
       // not in check
       // Establish a default score.  This score is returned if no
       // captures are generated, or if no captures generate a better
       // score (since we generally can choose whether or not to capture).
       assert(node->eval == Constants::INVALID_SCORE);
       bool had_eval = node->staticEval != Constants::INVALID_SCORE;
       if (had_eval) {
           node->eval = node->staticEval;
           assert(node->eval >= -Constants::MATE && node->eval <= Constants::MATE);
       }
       if (node->eval == Constants::INVALID_SCORE) {
           node->eval = node->staticEval = evalu8(board);
       }
       if (hashHit) {
           // Use the transposition table entry to provide a better score
           // for pruning decisions, if possible
           const score_t hashValue = HashEntry::hashValueToScore(hashEntry.getValue(),node->ply);
           if (result == (hashValue > node->eval ? HashEntry::LowerBound :
                          HashEntry::UpperBound)) {
               node->eval = hashValue;
               assert(node->eval >= -Constants::MATE && node->eval <= Constants::MATE);
           }
       }
       assert(node->eval != Constants::INVALID_SCORE);
#ifdef _TRACE
       if (mainThread()) {
           indent(ply);
           std::cout << "stand pat score = " << node->eval << std::endl;
       }
#endif
       node->best_score = node->alpha;
       if (node->eval > node->best_score) {
           node->best_score = node->eval;
           (node+1)->pv_length=0;
           if (node->eval >= node->beta) {
#ifdef _TRACE
               if (mainThread()) {
                   indent(ply); std::cout << "**CUTOFF**" << std::endl;
               }
#endif
               assert(!board.anyAttacks(board.kingSquare(board.oppositeSide()),board.sideToMove()));
               // store eval in hash table if not already fetched from there
               if (!had_eval) {
                   controller->hashTable.storeHash(hash, tt_depth,
                                                   age,
                                                   HashEntry::Eval,
                                                   HashEntry::scoreToHashValue(node->best_score,node->ply),
                                                   node->staticEval,
                                                   0,
                                                   hashMove);
               }
               return node->eval;
           }
       }
       score_t try_score;
       BoardState state(board.state);
       const ColorType oside = board.oppositeSide();
       Bitboard disc(board.getPinned(board.kingSquare(oside),board.sideToMove(),board.sideToMove()));
       QSearchMoveGenerator mg(board,hashMove);
       Move move;
       while (!IsNull(move=mg.nextMove())) {
           if (Capture(move) == King) {
#ifdef _TRACE
               if (mainThread()) {
                   indent(ply);
                   std::cout << "previous move illegal, exiting qsearch" << std::endl;
               }
#endif
               return -Illegal;
           }
#ifdef _TRACE
           if (mainThread()) {
               indent(ply);
               std::cout << "trying " << ply << ". ";
               MoveImage(move,std::cout);
               std::cout << std::endl;
           }
#endif
           if (!IsPromotion(move) &&
               !passedPawnPush(board,move) &&
               node->beta > -Constants::TABLEBASE_WIN) {
               // Futility pruning
               if ((Capture(move) == Pawn || board.getMaterial(oside).pieceCount() > 1) &&
                   (Params::Gain(move) + QSEARCH_FUTILITY_PRUNE_MARGIN + node->staticEval < node->best_score) &&
                   !board.wouldCheck(move)) {
#ifdef _TRACE
                   if (mainThread()) {
                       indent(ply); std::cout << "pruned (futility)" << std::endl;
                   }
#endif
                   continue;
               }
               // See pruning
               score_t neededGain = node->best_score - node->staticEval - QSEARCH_SEE_PRUNE_MARGIN;
               if (!disc.isSet(StartSquare(move)) &&
                   !seeSign(board,move,std::max<score_t>(0,neededGain))){
#ifdef _TRACE
                   if (mainThread()) {
                       indent(ply); std::cout << "pruned (SEE)" << std::endl;
                   }

#endif
                   continue;
               }
           }

           node->last_move = move;
           board.doMove(move,node);
           if (!board.wasLegal(move)) {
               board.undoMove(move,state);
               continue;
           }
           try_score = -quiesce(-node->beta, -node->best_score, ply+1, depth-1);
           board.undoMove(move,state);
           if (try_score != Illegal) {
#ifdef _TRACE
               if (mainThread()) {
                   indent(ply);
                   std::cout << ply << ". ";
                   MoveImage(move,std::cout);
                   std::cout << ' ' << try_score << std::endl;
               }
#endif
               if (try_score > node->best_score) {
                   node->best_score = try_score;
                   node->best = move;
                   if (try_score >= node->beta) {
#ifdef _TRACE
                       if (mainThread()) {
                           indent(ply);
                           std::cout << "**CUTOFF**" << std::endl;
                       }
#endif
                       goto search_end;
                   }
                   if (node->best_score >= Constants::MATE-1-ply)
                       goto search_end;          // mating move found
               }
           }
#ifdef _TRACE
           else if (mainThread()) {
               indent(ply);
               std::cout << ply << ". ";
               MoveImage(move,std::cout);
               std::cout << " (illegal)" << std::endl;
           }
#endif
       }
       // Do checks in qsearch
       if (depth >= 1-srcOpts.checks_in_qsearch) {
           QSearchCheckGenerator mg(board,disc);
           Move move;
           while (!IsNull(move = mg.nextCheck())) {
               if (MovesEqual(move,hashMove)) continue;
#ifdef _TRACE
               if (mainThread()) {
                   indent(ply);
                   std::cout << "trying " << ply << ". ";
                   MoveImage(move,std::cout);
                   std::cout << '+' << std::endl;
               }
#endif
               // prune checks that cause loss of the checking piece (but not
               // discovered checks)
               if (!disc.isSet(StartSquare(move)) && !seeSign(board,move,0)) {
#ifdef _TRACE
                   if (mainThread()) {
                       indent(ply); std::cout << "pruned" << std::endl;
                   }
#endif
                   continue;
               }
               else if (board.isPinned(board.sideToMove(),move)) {
                   // Move generator only produces pseudo-legal checking
                   // moves, and in the next ply we will only consider
                   // evasions. So need to ensure here that in making a
                   // check we do not expose our own King to check.
                   continue;
               }
               node->last_move = move;
               board.doMove(move,node);
               // verify opposite side in check:
               assert(board.anyAttacks(board.kingSquare(board.sideToMove()),board.oppositeSide()));
               // and verify quick check confirms it
               assert(board.checkStatus(move)==InCheck);
               // We know the check status so set it, so it does not
               // have to be computed
               board.setCheckStatus(InCheck);
               try_score = -quiesce(-node->beta,-node->best_score,
                                    ply+1,depth-1);
               board.undoMove(move,state);
               if (try_score != Illegal) {
#ifdef _TRACE
                   if (mainThread()) {
                       indent(ply);
                       std::cout << ply << ". ";
                       MoveImage(move,std::cout);
                       std::cout << ' ' << try_score << std::endl;
                   }
#endif
                   if (try_score > node->best_score) {
                       node->best_score = try_score;
                       node->best = move;
                       if (try_score >= node->beta) {
#ifdef _TRACE
                           if (mainThread()) {
                               indent(ply); std::cout << "**CUTOFF**" << std::endl;
                           }

#endif
                           goto search_end;
                       }
                       if (node->best_score >= Constants::MATE-1-ply)
                           goto search_end;      // mating move found
                   }
               }
#ifdef _TRACE
               else if (mainThread()) {
                   indent(ply);
                   std::cout << ply << ". ";
                   MoveImage(move,std::cout);
                   std::cout << " (illegal)" << std::endl;
               }
#endif
           }
       }
   search_end:
       assert(node->best_score >= -Constants::MATE && node->best_score <= Constants::MATE);
       storeHash(hash,node->best,tt_depth);
       if (node->inBounds(node->best_score)) {
           if (!IsNull(node->best)) {
               updatePV(board,node->best,ply);
           }
       }
       return node->best_score;
   }
}

void Search::storeHash(hash_t hash, Move hash_move, int depth) {
   // don't insert into the hash table if we are terminating - we may
   // not have an accurate score.
   if (!terminate) {
      // store the position in the hash table, if there's room
      score_t value = node->best_score;
      HashEntry::ValueType val_type;
      if (value <= node->alpha) {
         val_type = HashEntry::UpperBound;
         // We don't have a "best" move, because all moves
         // caused alpha cutoff.  But if there was a hash
         // move or an initial move produced by internal
         // interative deepening, save it in the hash table
         // so it will be tried first next time around.
         node->best = hash_move;
      }
      else if (value >= node->beta) {
         val_type = HashEntry::LowerBound;
      }
      else {
         val_type = HashEntry::Valid;
      }
#ifdef _TRACE
      static const char type_chars[5] =
         { 'E', 'U', 'L', 'X', 'X' };
      if (mainThread()) {
         indent(node->ply);
         std::cout << "storing type=" << type_chars[val_type] <<
            " ply=" << node->ply << " depth=" << depth << " value=";
         Scoring::printScore(value,std::cout);
         std::cout << " move=";
         MoveImage(node->best,std::cout);
         std::cout << std::endl;
      }
#endif
      // Note: adjust mate scores to reflect current ply.
      controller->hashTable.storeHash(hash,
                                      depth,
                                      age,
                                      val_type,
                                      HashEntry::scoreToHashValue(value,node->ply),
                                      node->staticEval,
                                      0,
                                      node->best);
   }
}

int Search::prune(const Board &board,
                  NodeInfo *node,
                  CheckStatusType in_check_after_move,
                  int moveIndex,
                  int improving,
                  Move move) {
    assert(node->swap == Constants::INVALID_SCORE);
    assert(node->ply > 0);
    if (node->num_legal &&
        board.checkStatus() == NotInCheck &&
        node->best_score > -Constants::MATE_RANGE) {
        const bool quiet = !CaptureOrPromotion(move);
        int depth = node->depth;
        // for pruning decisions, use modified depth but not the same as
        // LMR depth (idea from Laser)
        const int pruneDepth = quiet ? depth - lmr(node,depth,moveIndex) : depth;
        if (in_check_after_move != InCheck && quiet && board.getMaterial(board.sideToMove()).hasPieces()) {
            // do not use pruneDepth for LMP
            if (GetPhase(move) >= MoveGenerator::HISTORY_PHASE &&
                moveIndex > lmpCount(depth,improving)) {
#ifdef SEARCH_STATS
                ++stats.lmp;
#endif
#ifdef _TRACE
                if (mainThread()) {
                    indent(node->ply); std::cout << "LMP: pruned" << std::endl;
                }
#endif
                return 1;
            }
            // History pruning.
            if (pruneDepth <= (3-improving)*DEPTH_INCREMENT &&
                context.getCmHistory(node,move)<HISTORY_PRUNING_THRESHOLD[improving] &&
                context.getFuHistory(node,move)<HISTORY_PRUNING_THRESHOLD[improving]) {
#ifdef _TRACE
                if (mainThread()) {
                    indent(node->ply); std::cout << "history: pruned" << std::endl;
                }
#endif
#ifdef SEARCH_STATS
                ++stats.history_pruning;
#endif
                return 1;
            }
            // futility pruning, enabled at low depths. Do not prune
            // moves with good history.
            if (pruneDepth <= FUTILITY_DEPTH && context.scoreForOrdering(move,node,board.sideToMove())<
                FUTILITY_HISTORY_THRESHOLD[improving]){
                // Threshold was formerly increased with the move index
                // but this tests worse now.
                score_t threshold = node->alpha - futilityMargin(pruneDepth);
                if (node->eval == Constants::INVALID_SCORE) {
                    node->eval = node->staticEval = evalu8(board);
                }
                if (node->eval < threshold) {
#ifdef SEARCH_STATS
                    ++stats.futility_pruning;
#endif
#ifdef _TRACE
                    if (mainThread()) {
                        indent(node->ply); std::cout << "futility: pruned" << std::endl;
                    }
#endif
                    return 1;
                }
            }
        }
        // SEE pruning. Losing captures and checks and moves that put pieces en prise
        // are pruned at low depths.
        if (pruneDepth <= SEE_PRUNING_DEPTH &&
            //    extend <= 0 &&
            node->ply > 0 &&
            GetPhase(move) > MoveGenerator::WINNING_CAPTURE_PHASE) {
            const score_t margin = seePruningMargin(pruneDepth,quiet);
            bool seePrune;
            if (margin >= -Params::PAWN_VALUE/3 && node->swap != Constants::INVALID_SCORE) {
                seePrune = !node->swap;
            } else {
                seePrune = !seeSign(board,move,margin);
            }
            if (seePrune) {
#ifdef SEARCH_STATS
                ++stats.see_pruning;
#endif
#ifdef _TRACE
                if (mainThread()) {
                    indent(node->ply); std::cout << "SEE: pruned" << std::endl;
                }
#endif
                return 1;
            }
        }
    }
    return 0;
}

int Search::extend(const Board &board,
                   NodeInfo * /*node*/,
                   CheckStatusType in_check_after_move,
                   Move move) {
    // see if we should apply any extensions at this node.
    int extend = 0;

    if (in_check_after_move == InCheck) {
#ifdef SEARCH_STATS
        stats.check_extensions++;
#endif
        extend += CHECK_EXTENSION;
    }
    else if (passedPawnPush(board,move)) {
        extend += PAWN_PUSH_EXTENSION;
#ifdef SEARCH_STATS
        stats.pawn_extensions++;
#endif
    }
    else if (TypeOfMove(move) == Normal &&
             Capture(move) != Empty && Capture(move) != Pawn &&
             board.getMaterial(board.oppositeSide()).pieceCount() == 1 &&
             board.getMaterial(board.sideToMove()).noPieces()) {
        // Capture of last piece in endgame.
        extend += CAPTURE_EXTENSION;
#ifdef SEARCH_STATS
        ++stats.capture_extensions;
#endif
    }
    return std::min<int>(extend,DEPTH_INCREMENT);
}

int Search::reduce(const Board &board,
                   NodeInfo *node,
                   int moveIndex,
                   int improving,
                   int newDepth,
                   Move move) {
    int depth = node->depth;
    int reduction = 0;
    const bool quiet = !CaptureOrPromotion(move);

    // See if we do late move reduction.
    if (depth >= LMR_DEPTH && moveIndex >= 1+2*node->PV() && (quiet || moveIndex > lmpCount(depth,improving))) {
       reduction += lmr(node,depth,moveIndex);
        if (!quiet) {
            reduction -= DEPTH_INCREMENT;
        }
        else {
            if (!node->PV() && !improving) {
                reduction += DEPTH_INCREMENT;
            }
            if (node->ply > 0) {
                if (board.checkStatus() != InCheck && GetPhase(move) < MoveGenerator::HISTORY_PHASE) {
                    // killer or refutation move
                    reduction -= DEPTH_INCREMENT;
                }
                // reduce less for good history
                reduction -= std::max<int>(-2*DEPTH_INCREMENT,std::min<int>(2*DEPTH_INCREMENT,DEPTH_INCREMENT*context.scoreForOrdering(move,node,board.sideToMove())/512));
            }
        }
    }
    int r = std::min<int>(newDepth - DEPTH_INCREMENT,reduction);
    return r < DEPTH_INCREMENT ? 0 : r;
}

// Recursive function, implements alpha/beta search below ply 0 but
// above the quiescence search.
//
// The current board is this->board.
// The search parameters such as depth, ply are in this->node.
//
score_t Search::search()
{
    int rep_count;
    int using_tb;
    int move_index = 0;
#ifdef MOVE_ORDER_STATS
    node->best_count = 0;
#endif
#ifdef SEARCH_STATS
    ++stats.reg_nodes;
#endif
    int ply = node->ply;
    int depth = node->depth;
    assert(ply < Constants::MaxPly);
    if (++nodeAccumulator > NODE_ACCUM_THRESHOLD) {
        stats.num_nodes += nodeAccumulator;
        nodeAccumulator = 0;
#if defined(SMP_STATS)
        // sample thread usage
        if (--controller->sample_counter <=0) {
           controller->samples++;
           controller->threads += controller->pool->activeCount();
           controller->sample_counter = SAMPLE_INTERVAL;
        }
#endif
        if (--controller->time_check_counter <= 0) {
            controller->time_check_counter = controller->timeCheckInterval;
            if (checkTime()) {
               if (debugOut()) {
                   std::cout << globals::debugPrefix << "terminating, time up" << std::endl;
               }
               controller->terminateNow();   // signal all searches to end
            }
        }
    }
    if (terminate) {
        return node->alpha;
    }
    else if (maxDepth(node)) {
       if (!board.wasLegal((node-1)->last_move)) {
          return -Illegal;
       }
       node->flags |= EXACT;
       return evalu8(board);
    }
    else if (Scoring::isDraw(board,rep_count,ply)) {
        node->flags |= EXACT;
        if (!board.wasLegal((node-1)->last_move)) {
           return -Illegal;
        }
#ifdef _TRACE
        if (mainThread()) {
            indent(ply); std::cout << "draw!" << std::endl;
        }
#endif
        return drawScore(board);
    }
    Move hashMove = NullMove;
    using_tb = 0;
#ifdef SYZYGY_TBS
    if (srcOpts.use_tablebases) {
        using_tb = board.men() <= globals::EGTBMenCount &&
            controller->tb_probe_in_search &&
            node->depth/DEPTH_INCREMENT >= globals::options.search.syzygy_probe_depth;
    }
#endif
    HashEntry hashEntry;
    HashEntry::ValueType result;
    bool hashHit = false;
    score_t hashValue = Constants::INVALID_SCORE;
    if ((node->flags & IID) || !IsNull(node->excluded)) {
       result = HashEntry::NoHit;
    }
    else {
       // Search the hash table to see if we have hit this
       // position before.
       // Note: query the hash table before the tablebases, since TB info may
       // be cached.
       // Note: we copy the hash entry .. so mods by another thread do not
       // alter the copy
       result = controller->hashTable.searchHash(board.hashCode(rep_count),
                                                 depth,age,hashEntry);
#ifdef SEARCH_STATS
       stats.hash_searches++;
#endif
       hashHit = result != HashEntry::NoHit;
    }
    if (hashHit) {
#ifdef SEARCH_STATS
         stats.hash_hits++;
#endif
         // always accept a full-depth entry (cached tb hit)
         if (!hashEntry.tb()) {
            // if using TBs at this ply, do not pull a non-TB entry out of
            // the hashtable. Also do not allow hash based cutoff of a PV
            // node.
            if (using_tb || node->PV()) {
               result = HashEntry::Invalid;
            }
        }
        hashValue = HashEntry::hashValueToScore(hashEntry.getValue(),node->ply);
        // Note: hash move may be usable even if score is not usable
        hashMove = hashEntry.bestMove(board);
        if (result == HashEntry::Valid) {
          if (node->inBounds(hashValue)) {
              // parent node will consider this a new best line
              hashMove = hashEntry.bestMove(board);
              if (!IsNull(hashMove)) {
                  node->pv[ply] = hashMove;
                  node->pv_length = 1;
              }
#ifdef _DEBUG
              if (!IsNull(hashMove) && !legalMove(board,hashMove)) {
                  std::cout << board << std::endl << std::flush;
                  MoveImage(hashMove,std::cout);
                  std::cout << std::endl << std::flush;
              }
#endif
#ifdef _TRACE
             if (mainThread()) {
                  indent(ply); std::cout << "best line[ply][ply] = ";
                  MoveImage(hashMove,std::cout);
                  std::cout << std::endl;
             }
#endif
          }
          return hashValue;
        }
        else {
          if ((result == HashEntry::UpperBound && hashValue <= node->alpha) ||
              (result == HashEntry::LowerBound && hashValue >= node->beta)) {
#ifdef _TRACE
              if (mainThread()) {
                traceHash(result == HashEntry::LowerBound ? 'L' : 'U',node,hashValue,hashEntry);
              }
#endif
              if (!IsNull(hashMove) && !CaptureOrPromotion(hashMove)) {
                  if (hashValue >= node->beta) {
                      context.setKiller(hashMove,node->ply);
                  }
                  // history bonus for hash move generated beta cutoff,
                  // penalty for hash move below alpha
                  context.updateMove(board,node,hashMove,hashValue >= node->beta,hashValue <= node->alpha);
              }
              if (node->ply > 0 && (node-1)->num_legal <= 2 && !IsNull(node->last_move) && !CaptureOrPromotion(node->last_move)) {
                  context.updateMove(board,node-1,(node-1)->last_move,false,true);
              }
              return hashValue;
          }
        }
    }
#ifdef SYZYGY_TBS
    if (using_tb && rep_count==0 && !(node->flags & IID) && board.state.moveCount == 0 && !board.castlingPossible()) {
       stats.tb_probes++;
       score_t tb_score;
       int tb_hit = SyzygyTb::probe_wdl(board, tb_score, srcOpts.syzygy_50_move_rule != 0);
       if (tb_hit) {
            stats.tb_hits++;
#ifdef _TRACE
            if (mainThread()) {
                indent(ply); std::cout << "EGTB hit: score " << tb_score << std::endl;
            }
#endif
            // Put it in with a large depth so we will not
            // overwrite - this entry is "exact" at all
            // search depths, so effectively its depth is infinite.
            controller->hashTable.storeHash(board.hashCode(rep_count),
                (Constants::MaxPly-1)*DEPTH_INCREMENT,
                age,
                HashEntry::Valid,
                HashEntry::scoreToHashValue(tb_score,node->ply),
                Constants::INVALID_SCORE,
                HashEntry::TB_MASK,
                NullMove);
            node->best_score = tb_score;               // unadjusted score
            node->flags |= EXACT;
            return node->best_score;
        }
    }
#endif
    // At this point we need to know if we are in check or not.
    int in_check =
        (board.checkStatus((node-1)->last_move) == InCheck);
#ifdef _DEBUG
    if (in_check != board.inCheck()) {
        std::cout << globals::debugPrefix << board << std::endl;
        std::cout << globals::debugPrefix << "move=";
	MoveImage((node-1)->last_move,std::cout);
	std::cout << std::endl;
	assert(0);
    }
#endif
#ifdef _TRACE
    if (mainThread() && in_check) { indent(ply); std::cout << "in_check=" << in_check << std::endl;}
#endif
    // Compute (if needed) and store the static evaluation for the current
    // position and also update node->eval with an improved evaluation
    // from the hash table, if possible.
    node->eval = node->staticEval = Constants::INVALID_SCORE;
    if (hashHit) {
        // Use the cached static value if possible
        node->eval = node->staticEval = hashEntry.staticValue();
    }
    if (node->eval == Constants::INVALID_SCORE) {
        node->eval = node->staticEval = evalu8(board);
    }
    if (hashHit) {
        // Use the transposition table entry to provide a better score
        // for pruning decisions, if possible
        if (result == (hashValue > node->eval ? HashEntry::LowerBound :
                       HashEntry::UpperBound)) {
            node->eval = hashValue;
        }
    }
    assert(node->staticEval != Constants::INVALID_SCORE);

    // pre-search pruning conditions
    const bool pruneOk = !in_check &&
        !node->PV() &&
        !(node->flags & (VERIFY|IID));

    const int improving = ply >= 3 && !in_check &&
        (node-2)->staticEval != Constants::INVALID_SCORE &&
        (node->staticEval >= (node-2)->staticEval);

    // Reset killer moves for children (idea from Ethereal)
    context.clearKillers(node->ply+1);

#ifdef STATIC_NULL_PRUNING
    // static null pruning, aka reverse futility pruning,
    // as in Protector, Texel, etc.
    if (pruneOk && depth <= STATIC_NULL_PRUNING_DEPTH) {
        const score_t margin = staticNullPruningMargin(depth, improving);
        const score_t threshold = node->beta + margin;
        assert(node->eval != Constants::INVALID_SCORE);
        if (node->eval >= threshold && node->eval < Constants::MATE_RANGE) {
#ifdef _TRACE
           if (mainThread()) {
              indent(ply); std::cout << "static null pruned" << std::endl;
           }
#endif
#ifdef SEARCH_STATS
           ++stats.static_null_pruning;
#endif
           return node->eval - margin;
       }
    }
#endif

#ifdef RAZORING
    // razoring as in Stockfish
    if (pruneOk && depth <= RAZOR_DEPTH && board.getMaterial(board.sideToMove()).hasPieces()) {
        assert(node->eval != Constants::INVALID_SCORE);
        if (node->eval < node->beta - razorMargin(depth)) {
#ifdef NNUE
            (node+1)->clearNNUEState();
#endif
            score_t v = quiesce(node->alpha,node->beta,ply+1,0);
#ifdef _TRACE
            if (mainThread()) {
               indent(ply); std::cout << "razored node, score=" << v << std::endl;
            }
#endif
#ifdef SEARCH_STATS
                stats.razored++;
#endif
            node->best_score = v;
            return v;
        }
    }
#endif

    // Try to get a fast cutoff using a "null move".  Skip if the side
    // to move is in check or if material balance is low enough that
    // zugzwang is a possibility. Do not do null move if this is an
    // IID search, because it will only help us get a cutoff, not a move.
    // Also avoid null move near the 50-move draw limit.
    if (pruneOk &&
        (depth >= 2*DEPTH_INCREMENT) &&
        !IsNull((node-1)->last_move) &&
        (depth >= 4*DEPTH_INCREMENT || !CaptureOrPromotion((node-1)->last_move)) &&
        board.getMaterial(board.sideToMove()).hasPieces() &&
        IsNull(node->excluded) &&
        node->eval >= node->beta &&
        node->eval >= node->staticEval &&
        ((node->staticEval >= node->beta - int(0.25*Params::PAWN_VALUE) * (depth / DEPTH_INCREMENT - 6)) || (depth >= 12*DEPTH_INCREMENT)) &&
        !Scoring::mateScore(node->alpha) &&
        board.state.moveCount <= 98) {
        // Fixed reduction + some depth- and score-dependent
        // increment. Decrease reduction somewhat when material
        // is low.
        const int lowMat = board.getMaterial(board.sideToMove()).materialLevel() <= 3;
        int nu_depth = depth - 4*DEPTH_INCREMENT + (lowMat ? DEPTH_INCREMENT/2 : 0) - depth/(4+2*lowMat) - std::min<int>(3*DEPTH_INCREMENT,int(DEPTH_INCREMENT*(node->eval-node->beta)/(2*Params::PAWN_VALUE)));
        // Skip null move if likely to be futile according to hash info
        if (!hashHit || !hashEntry.avoidNull(nu_depth,node->beta)) {
            node->last_move = NullMove;
            BoardState state(board.state);
#ifdef _TRACE
            if (mainThread()) {
                indent(ply);
                std::cout << "trying " << ply << ". " << "(null)" << std::endl;
            }
#endif
            board.doNull(node);
            score_t nscore;
            if (nu_depth > 0)
                nscore = -search(-node->beta, 1-node->beta,
                                 ply+1, nu_depth);
            else
                nscore = -quiesce(-node->beta, 1-node->beta,
                                  ply+1, 0);
#ifdef _TRACE
            if (mainThread()) {
                indent(ply);
                std::cout << ply << ". " << "(null)" << ' ' << nscore << std::endl;
            }
#endif
            board.undoNull(state);
            if (terminate) {
                node->best_score = node->alpha;
                goto search_end2;
            }
            else if (nscore >= node->beta) {          // cutoff
                // don't return mate scores from the null move search
                if (nscore >= Constants::MATE_RANGE) nscore = node->beta;
                if (nu_depth > 0 && (depth >= 6*DEPTH_INCREMENT)) {
                    // Verify null cutoff with reduced-depth search
                    // (idea from Dieter Buerssner)
#ifdef _TRACE
                    if (mainThread()) {
                       indent(ply); std::cout << "verifying null move" << std::endl;
                    }
#endif
#ifdef NNUE
                    // entering a new node w/o a move, so reset next node state
                    (node+1)->clearNNUEState();
#endif
                    nscore = search(node->beta-1, node->beta, ply+1, nu_depth, VERIFY);
                    if (nscore == -Illegal) {
#ifdef _TRACE
                       if (mainThread()) {
                          indent(ply); std::cout << "previous move illegal" << std::endl;
                       }
#endif
                       return -Illegal;
                    }
#ifdef _TRACE
                    if (mainThread()) {
                        indent(ply);
                        if (nscore>=node->beta)
                            std::cout << "null cutoff verified, score=" << nscore;
                        else
                            std::cout << "null cutoff not verified";
                        std::cout << std::endl;
                    }
#endif
                }
                if (nscore >= node->beta) {            // null cutoff
#ifdef _TRACE
                    if (mainThread()) {
                         indent(ply);
                         std::cout << "**CUTOFF**" << std::endl;
                    }
#endif
#ifdef SEARCH_STATS
                    stats.null_cuts++;
#endif
                    return nscore;
                }
            }
        }
    }

    // ProbCut. Try to find a capture move that is signficantly above
    // beta, based on a limited-depth search. If found assume that a
    // regular depth search would cause beta cutoff, too.
    if (!node->PV() &&
        depth >= PROBCUT_DEPTH &&
        ((node->flags & PROBCUT)==0 || depth >= 9*DEPTH_INCREMENT) &&
        std::abs(node->beta) < Constants::MATE_RANGE) {
        const score_t probcut_beta = std::min<score_t>(Constants::MATE,node->beta + PROBCUT_MARGIN);
        const int nu_depth = depth - 4*DEPTH_INCREMENT - depth/8;
        const bool validHashValue = hashHit && hashEntry.depth() >= nu_depth + DEPTH_INCREMENT &&
            hashValue != Constants::INVALID_SCORE;
        if (!(validHashValue && result == HashEntry::UpperBound && hashValue < probcut_beta)) {
            const score_t needed_gain = probcut_beta - node->staticEval;
            BoardState state(board.state);
            NodeState nstate(node);
            Move move;
            // skip pawn captures because they will be below threshold
            ProbCutMoveGenerator mg(board, hashMove, board.occupied[board.oppositeSide()] & ~board.pawn_bits[board.oppositeSide()]);
            while (!IsNull(move = mg.nextMove())) {
                if (Capture(move)==King) {
#ifdef _TRACE
                    if (mainThread()) {
                        std::cout << "Probcut: previous move illegal" << std::endl;
                    }
                    return -Illegal;                  // previous move was illegal
#endif
                }
                else if (seeSign(board,move,needed_gain)) {
#ifdef _TRACE
                    if (mainThread()) {
                        indent(ply);
                        std::cout << "Probcut: trying " << ply << ". ";
                        MoveImage(move,std::cout);
                        std::cout << std::endl;
                    }
#endif
                    SetPhase(move,MoveGenerator::WINNING_CAPTURE_PHASE);
                    board.doMove(move,node);
                    if (!board.wasLegal(move)) {
                        board.undoMove(move,state);
                        continue;
                    }
                    node->last_move = move;
                    node->num_legal++;
                    score_t value = -search(-probcut_beta, -probcut_beta+1, ply+1, nu_depth, PROBCUT);
#ifdef _TRACE
                    if (mainThread()) {
                        indent(ply);
                        std::cout << ply << ". ";
                        MoveImage(move,std::cout);
                        std::cout << " " << value << std::endl;
                    }
#endif
                    board.undoMove(move,state);
                    if (value != Illegal && value >= probcut_beta) {
#ifdef _TRACE
                        if (mainThread()) {
                            indent(ply);
                            std::cout << "Probcut: cutoff" << std::endl;
                        }
#endif
                        // store ProbCut result if there is not already a hash entry with
                        // valid score & greater depth
                        if (!(hashHit && hashEntry.depth() >= nu_depth + DEPTH_INCREMENT &&
                              hashValue != Constants::INVALID_SCORE)) {
                            controller->hashTable.storeHash(board.hashCode(rep_count),
                                                            nu_depth + DEPTH_INCREMENT,
                                                            age,
                                                            HashEntry::LowerBound,
                                                            HashEntry::scoreToHashValue(value,node->ply),
                                                            node->staticEval,
                                                            0,
                                                            move);
                        }
                        return value;
                    }
                }
            }
        }
    }

    // Use "internal iterative deepening" to get an initial move to try if
    // there is no hash move .. an idea from Crafty (previously used by
    // Hitech).
    if (IsNull(hashMove) && depth >= IID_DEPTH[node->PV()]) {
        // reduced depth for search
        const int d = depth - IID_DEPTH[node->PV()] + DEPTH_INCREMENT;
#ifdef _TRACE
        if (mainThread()) {
            indent(ply); std::cout << "== start IID, depth = " << d
                << std::endl;
        }
#endif
        // Call search routine at lower depth to get a 1st move to try.
#ifdef NNUE
        (node+1)->clearNNUEState();
#endif
        //
        // Note: we do not push down the node stack because we want this
        // search to have all the same parameters (including ply) as the
        // current search, just reduced depth + the IID flag set.
        // Save state here: exit from block resets node state.
        NodeState state(node);
        node->flags |= IID;
        node->depth = d;
        score_t iid_score = search();
        // set hash move to IID search result (may still be null)
        hashMove = node->best;
        if (iid_score == -Illegal || ((node->flags) & EXACT)) {
            // previous move was illegal or search gave an exact score
#ifdef _TRACE
            if (mainThread()) {
                indent(ply);
                std::cout << "== exact result from IID" << std::endl;
            }
#endif
            return iid_score;
        }
        if (terminate) {
            return node->alpha;
        }
#ifdef _TRACE
        if (mainThread()) {
            indent(ply); std::cout << "== IID done.";
        }
#endif

#ifdef _TRACE
        if (mainThread()) {
            if (!IsNull(hashMove)) {
                indent(ply); std::cout << "  hashMove = ";
                MoveImage(hashMove,std::cout);
            }
            std::cout << std::endl;
        }
#endif
        if (iid_score <= node->alpha && node->eval > node->alpha) { // upper bound
            node->eval = iid_score;
        } else if (iid_score >= node->beta && node->eval < node->beta) { // lower bound
            node->eval = iid_score;
        }
    }
    {
        bool singularExtend = false;
        BoardState state(board.state);
#ifdef SINGULAR_EXTENSION
        if (depth >= SINGULAR_EXTENSION_DEPTH &&
            ply > 0 &&
            IsNull(node->excluded) &&
            hashHit &&
            hashEntry.type() == HashEntry::LowerBound &&
            !IsNull(hashMove) &&
            !Scoring::mateScore(hashValue) &&
            hashEntry.depth() >= depth - 3*DEPTH_INCREMENT) {
            // Verify hash move legal
            board.doMove(hashMove,node);
            const bool legal = board.wasLegal(hashMove);
            board.undoMove(hashMove,state);
            if (!legal) {
                hashMove = NullMove;
            } else {
                // Search all moves but the hash move at reduced depth. If all
                // fail low with a score significantly below the hash
                // move's score, then consider the hash move as "singular" and
                // extend its search depth.
                // This hash-based "singular extension" has been
                // implemented in the Ippo* series of engines (and
                // presumably in Rybka), and also now in Stockfish, Komodo,
                // Texel, Protector, etc.
#ifdef SEARCH_STATS
                ++stats.singular_searches;
#endif
                NodeState ns(node);
                score_t nu_beta = std::max<score_t>(hashValue - singularExtensionMargin(depth),-Constants::MATE);
#ifdef NNUE
                node->clearNNUEState();
#endif
                score_t result = search(nu_beta-1,nu_beta,node->ply+1,singularSearchDepth(depth),0,hashMove);
                if (result < nu_beta) {
#ifdef _TRACE
                    indent(ply); std::cout << "singular extension" << std::endl;
#endif
                    singularExtend = true;
#ifdef SEARCH_STATS
                    ++stats.singular_extensions;
#endif
#ifdef MULTICUT
                }
                else if (nu_beta >= node->beta) {
                    // We have completed the singular search but the hash
                    // move was not found to be singular. Cutoff if the
                    // singular beta is >= the high search window
                    // ("Multi-cut" pruning).
#ifdef SEARCH_STATS
                    ++stats.multicut;
#endif
                    return nu_beta;
#endif
#ifdef NON_SINGULAR_PRUNING
                }
                else if (hashValue >= node->beta) {
                    result = search(node->beta-1,node->beta,node->ply+1,(depth+3*DEPTH_INCREMENT)/2,0,hashMove);
                    if (result >= node->beta) {
                        // Another pruning idea from Stockfish: prune if the search is >= beta
                        // (note however current Stockfish reduces rather than prunes)
#ifdef _TRACE
                        indent(ply); std::cout << "non-hash move failed high: cutoff" << std::endl;
#endif
#ifdef SEARCH_STATS
                        ++stats.non_singular_pruning;
#endif
                        return node->beta;
                    }

#endif
                }
            }
        }
#endif
        MoveGenerator mg(board, &context, node, ply, hashMove, mainThread());
        score_t try_score;
        //
        // Now we are ready to loop through the moves from this position
        //
#ifdef _TRACE
        int first = 1;
#endif
        while (!node->cutoff && !terminate) {
            score_t hibound = node->num_legal == 0 ? node->beta : node->best_score +1;
            Move move;
            move = in_check ? mg.nextEvasion(move_index) : mg.nextMove(move_index);
            if (IsNull(move)) break;
            if (IsUsed(move) || MovesEqual(move,node->excluded)) continue;
#ifdef SEARCH_STATS
            ++stats.moves_searched;
#endif
#ifdef _TRACE
            if (mainThread()) {
                indent(ply);
                std::cout << "trying " << ply << ". ";
                MoveImage(move,std::cout);
               if (first) std::cout << "(pv)";
                std::cout << " [" << node->best_score << "," << hibound << "]";
                std::cout << " d:" << depth << std::endl;
            }
#endif
            if (Capture(move)==King) {
#ifdef _TRACE
                if (mainThread()) {
                     indent(ply);
                     std::cout << "king capture, previous move illegal" << std::endl;
                }
#endif
                return -Illegal;                  // previous move was illegal
            }
            assert(DestSquare(move) != InvalidSquare);
            assert(StartSquare(move) != InvalidSquare);
            node->last_move = move;
            if (!CaptureOrPromotion(move)) {
                assert(node->num_quiets<Constants::MaxMoves);
                node->quiets[node->num_quiets++] = move;
            }
            CheckStatusType in_check_after_move = board.wouldCheck(move);
            int extension = 0, reduction = 0, newDepth = depth - DEPTH_INCREMENT;
            node->swap = Constants::INVALID_SCORE;
            if (singularExtend && GetPhase(move) == MoveGenerator::HASH_MOVE_PHASE) {
                extension = DEPTH_INCREMENT;
                newDepth += extension;
#ifdef SEARCH_STATS
                ++stats.singular_extensions;
#endif
            }
            else {
                if (prune(board, node, in_check_after_move, move_index, improving, move)) {
                    continue;
                }
                extension = extend(board, node, in_check_after_move, move);
                newDepth += extension;
                reduction = reduce(board, node, move_index, improving, newDepth, move);
            }
            board.doMove(move,node);
            if (!board.wasLegal(move,in_check)) {
                  assert(board.anyAttacks(board.kingSquare(board.oppositeSide()),board.sideToMove()));
#ifdef _TRACE
               if (mainThread()) {
                  indent(ply); std::cout << "Illegal move!" << std::endl;
               }
#endif
               board.undoMove(move,state);
               continue;
            }
            setCheckStatus(board, in_check_after_move);
#ifdef SEARCH_STATS
            if (reduction) ++stats.reduced;
#endif
            if (newDepth - reduction > 0) {
                try_score = -search(-hibound, -node->best_score,
                    ply+1, newDepth - reduction);
            }
            else {
                try_score = -quiesce(-hibound, -node->best_score,
                    ply+1, 0);
            }
            if (try_score == Illegal) {
#if defined(_TRACE)
                if (mainThread()) {
                    indent(ply); std::cout << "Illegal move" << std::endl;
                }
#endif
                board.undoMove(move,state);
                continue;
            }
            node->num_legal++;
            if (try_score > node->best_score && reduction && !terminate) {
               // We failed to get a cutoff and must re-search
#ifdef _TRACE
               if (mainThread()) {
                  indent(ply); std::cout << ply << ". ";
                  MoveImage(move,std::cout);
                  std::cout << " score = " << try_score << " - no cutoff, researching .." << std::endl;
                    indent(ply); std::cout << "window = [" << node->best_score << "," << hibound << "]" << std::endl;
               }
#endif
               // re-search with no reduction
               if (newDepth > 0)
                  try_score=-search(-hibound, -node->best_score,ply+1,newDepth);
               else
                  try_score=-quiesce(-hibound,-node->best_score,ply+1,0);
            }
            if (try_score > node->best_score && hibound < node->beta && !terminate) {
               // widen window
               hibound = node->beta;
#ifdef _TRACE
               if (mainThread()) {
                  indent(ply); std::cout << ply << ". ";
                  MoveImage(move,std::cout);
                  std::cout << " score = " << try_score << " - no cutoff, researching .." << std::endl;
                    indent(ply); std::cout << "window = [" << node->best_score << "," << hibound << "]" << std::endl;
               }
#endif
               if (newDepth > 0)
                 try_score=-search(-hibound, -node->best_score,ply+1,newDepth);
               else
                 try_score=-quiesce(-hibound,-node->best_score,ply+1,0);
            }
            board.undoMove(move,state);
#ifdef _TRACE
            if (mainThread()) {
                indent(ply);
                std::cout << ply << ". ";
                MoveImage(move,std::cout);
                std::cout << ' ' << try_score;
                if (first) std::cout << " (pv)";
                std::cout << std::endl;
            }
            first = 0;
#endif
            if (terminate) {
#ifdef _TRACE
               if (mainThread()) {
                  indent(ply); std::cout << "terminating" << std::endl;
               }
#endif
               break;
            }
            if (try_score > node->best_score) {
                if (updateMove(node,move,try_score,ply)) {
                   // cutoff
                   break;
                }
            }
            if (try_score >= Constants::MATE-1-ply) {
                node->cutoff++;
                break;                            // mating move found
            }
        }                                         // end move loop
#ifdef _TRACE
        if (node->best_score >= node->beta && mainThread()) {
            indent(ply);
            std::cout << "**CUTOFF**" << std::endl;
        }
#endif
        if (terminate) {
            node->best_score = node->alpha;
            goto search_end2;
        }
        if (node->num_legal == 0) {
            // no legal moves
            if (in_check) {
#ifdef _TRACE
                if (mainThread()) {
                    indent(ply); std::cout << "mate" << std::endl;
                }
#endif
                node->best_score = -(Constants::MATE - ply);
                node->flags |= EXACT;
                goto search_end2;
            }
            else {                                // stalemate
#ifdef _TRACE
                if (mainThread()) {
                    indent(ply); std::cout << "stalemate!" << std::endl;
                }
#endif
                node->best_score = drawScore(board);
                node->flags |= EXACT;
                return node->best_score;
            }
        }
    }
    if (!IsNull(node->best) && !CaptureOrPromotion(node->best) &&
        board.checkStatus() != InCheck) {
        context.setKiller(node->best, node->ply);
        if (node->ply > 0) {
            context.setCounterMove(board,(node-1)->last_move,node->best);
        }
        context.updateStats(board,node);
    }

    // don't insert into the hash table if we are terminating - we may
    // not have an accurate score.
    if (!terminate && IsNull(node->excluded)) {
        if (IsNull(node->best)) node->best = hashMove;
        // store the position in the hash table, if there's room
        score_t value = node->best_score;
        HashEntry::ValueType val_type;
#ifdef _TRACE
        char typeChar;
#endif
        if (value <= node->alpha) {
            val_type = HashEntry::UpperBound;
#ifdef _TRACE
            typeChar = 'U';
#endif
            // We don't have a "best" move, because all moves
            // caused alpha cutoff.  But if there was a hash
            // move or an initial move produced by internal
            // interative deepening, save it in the hash table
            // so it will be tried first next time around.
            node->best = hashMove;
        }
        else if (value >= node->beta) {
            val_type = HashEntry::LowerBound;
#ifdef _TRACE
            typeChar = 'L';
#endif
        }
        else {
            val_type = HashEntry::Valid;
#ifdef _TRACE
            typeChar = 'E';
#endif
        }
#ifdef _TRACE
        if (mainThread()) {
            indent(ply);
            std::cout << "storing type=" << typeChar <<
                " ply=" << ply << " depth=" << depth << " value=" << value <<
                " move=";
            MoveImage(node->best,std::cout);
            std::cout << std::endl;
        }
#endif
        const hash_t hashCode = board.hashCode(rep_count);
        controller->hashTable.storeHash(hashCode, depth,
                                        age,
                                        val_type,
                                        HashEntry::scoreToHashValue(value,node->ply),
                                        node->staticEval,
                                        0,
                                        node->best);
    }
    search_end2:
#ifdef MOVE_ORDER_STATS
    if (node->num_legal && node->best_score != node->alpha) {
        stats.move_order_count++;
        assert(node->best_count>=0);
        if (node->best_count<4) {
            stats.move_order[node->best_count]++;
        }
    }
#endif
    score_t score = node->best_score;
    assert(score >= -Constants::MATE && score <= Constants::MATE);
    return score;
}

int Search::updateRootMove(const Board &board,
                           NodeInfo *node,
                           Move move, score_t score, int move_index)
{
   if (score > node->best_score)  {
      node->best = move;
      node->best_score = score;
#ifdef MOVE_ORDER_STATS
      node->best_count = node->num_legal-1;
#endif
      if (score >= node->beta) {
#ifdef _TRACE
         if (mainThread()) {
             std::cout << "ply 0 beta cutoff" << std::endl;
         }
#endif
         // set pv to this move so it is searched first the next time
         node->pv[0] = move;
         node->pv_length = 1;
         node->cutoff++;
         node->best_score = score;
         updateStats(board, node, iterationDepth,
                     node->best_score);
         if (mainThread()) {
            if (controller->uci && !srcOpts.multipv) {
               std::cout << "info score ";
               Scoring::printScoreUCI(score,std::cout);
               std::cout << " lowerbound" << std::endl;
            }
         }
         return 1;  // signal cutoff
      }
      updatePV(board,node,(node+1),move,0);
      updateStats(board, node, iterationDepth,
                  node->best_score);
      if (mainThread() && srcOpts.multipv == 1) {
         if (move_index>1) {
            // best move has changed, show new best move
            showStatus(board,move,score <= node->alpha,score >= node->beta);
         }
      }
   }
   return 0;   // no cutoff
}

int Search::updateMove(NodeInfo *node, Move move, score_t score, int ply)
{
   int cutoff = 0;
   node->best_score = score;
   node->best = move;
#ifdef MOVE_ORDER_STATS
   node->best_count = node->num_legal-1;
#endif
   if (score >= node->beta) {
#ifdef _TRACE
      if (mainThread()) {
         indent(ply); std::cout << "beta cutoff" << std::endl;
      }
#endif
      node->cutoff++;
      cutoff++;
   }
   else {
      node->best_score = score;
      updatePV(board,node,(node+1),move,ply);
   }
   return cutoff;
}

void Search::updatePV(const Board &board, Move m, int ply)
{
    updatePV(board,node,(node+1),m,ply);
}

#ifndef _MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
void Search::updatePV(const Board &board, NodeInfo *node, NodeInfo *fromNode, Move move, int ply)
{
#ifndef _MSC_VER
#pragma GCC diagnostic pop
#endif
    node->pv[ply] = move;
    if (fromNode->pv_length) {
        memcpy((void*)(node->pv+ply+1),(void*)(fromNode->pv+ply+1),
            sizeof(Move)*fromNode->pv_length);
    }
    node->pv_length = fromNode->pv_length+1;
#if defined(_DEBUG) || defined(_TRACE)
#ifdef _TRACE
    if (mainThread() && node->pv_length>0) {
        indent(ply); std::cout << "PV: ";
    }
#endif
    Board board_copy(board);
    for (int i = ply; i < node->pv_length+ply; i++) {
        assert(i<Constants::MaxPly);
#ifdef _TRACE
        if (mainThread()) {
            MoveImage(node->pv[i],std::cout);
            std::cout << ' ';
        }
#endif
        assert(legalMove(board_copy,node->pv[i]));
        board_copy.doMove(node->pv[i]);
    }
#ifdef _TRACE
    if (mainThread() && node->pv_length>0) {
        std::cout << std::endl;
    }
#endif
#endif
}

// Initialize a Search instance to prepare it for searching in a
// particular thread. This is called from the thread in which the
// search will execute.
void Search::init(NodeInfo *nodeStack, ThreadInfo *slave_ti) {
    this->board = controller->initialBoard;
    node = nodeStack;
    assert(node);
    nodeAccumulator = 0;
    ti = slave_ti;
    node->ply = 0;
    // depth will be set later
    stats.clear();

#ifdef SYZYGY_TBS
    // Propagate tb value from controller to stats
    stats.tb_value = controller->tb_score;
#endif

    // Clear killer since the side to move may have been different
    // in the previous use of this class.
    context.clearKiller();
}

void Search::clearHashTables() {
   scoring.clearHashTables();
   context.clear();
}

void Search::setSearchOptions() {
   srcOpts = globals::options.search;
}

score_t Search::evalu8(const Board &board) {
	score_t score;
#ifdef NNUE
    const Material &ourMat = board.getMaterial(board.sideToMove());
    const Material &oppMat = board.getMaterial(board.oppositeSide());
    //bool imbalance = std::abs(int(ourMat.materialLevel()) - int(oppMat.materialLevel()))>10;
    const bool useClassical = !srcOpts.pureNNUE &&
        (//imbalance ||
         ourMat.men() + oppMat.men() <= 7);
    if (!useClassical && globals::options.search.useNNUE && globals::nnueInitDone) {
        score = scoring.evalu8NNUE(board,node);
    } else {
        score = scoring.evalu8(board);
    }
#else
    score = scoring.evalu8(board);
#endif
   return score;
}
