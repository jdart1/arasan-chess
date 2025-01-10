// Copyright 1987-2024 by Jon Dart.  All Rights Reserved.

#include "search.h"
#include "globals.h"
#include "notation.h"
#include "movegen.h"
#include "hash.h"
#include "protocol.h"
#include "see.h"
#include "tunable.h"
//#ifdef SYZYGY_TBS
//#include "syzygy.h"
//#endif
#include "legal.h"
#include "trace.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iterator>
//#define _TRACE

static const int ASPIRATION_WINDOW[] =
    {(int)(0.375*Params::PAWN_VALUE),
     (int)(0.75*Params::PAWN_VALUE),
     (int)(1.5*Params::PAWN_VALUE),
     (int)(3.0*Params::PAWN_VALUE),
     (int)(6.0*Params::PAWN_VALUE),
      Constants::MATE};
static constexpr int ASPIRATION_WINDOW_STEPS = 6;

#define STATIC_NULL_PRUNING
//#define RAZORING
#define SINGULAR_EXTENSION
#define MULTICUT
#define NON_SINGULAR_PRUNING

TUNABLE(IID_DEPTH_NON_PV, 6*DEPTH_INCREMENT, 4*DEPTH_INCREMENT, 10*DEPTH_INCREMENT);
TUNABLE(IID_DEPTH_PV, 8*DEPTH_INCREMENT, 4*DEPTH_INCREMENT, 10*DEPTH_INCREMENT);
TUNABLE(FUTILITY_DEPTH, 8*DEPTH_INCREMENT, 4*DEPTH_INCREMENT, 10*DEPTH_INCREMENT);
TUNABLE(FUTILITY_HISTORY_THRESHOLD_IMP, 1000, 500, 4000);
TUNABLE(FUTILITY_HISTORY_THRESHOLD_NON_IMP, 2000, 500, 4000);
TUNABLE(CAPTURE_FUTILITY_DEPTH, 5*DEPTH_INCREMENT, 3*DEPTH_INCREMENT, 8*DEPTH_INCREMENT);
TUNABLE(CAPTURE_FUTILITY_HISTORY_DIVISOR, 65, 20, 200);
TUNABLE(HISTORY_PRUNING_THRESHOLD_IMP, -250, -1000, 0);
TUNABLE(HISTORY_PRUNING_THRESHOLD_NON_IMP, -250, -1000, 0);
TUNABLE(HISTORY_REDUCTION_DIVISOR, 2200, 500, 4000);
#ifdef RAZORING
TUNABLE(RAZOR_DEPTH, DEPTH_INCREMENT, 0, 2*DEPTH_INCREMENT);
#endif
TUNABLE(SEE_PRUNING_DEPTH, 7*DEPTH_INCREMENT, 3*DEPTH_INCREMENT, 10*DEPTH_INCREMENT);
static constexpr int CHECK_EXTENSION = DEPTH_INCREMENT;
static constexpr int PAWN_PUSH_EXTENSION = DEPTH_INCREMENT;
static constexpr int CAPTURE_EXTENSION = DEPTH_INCREMENT/2;
#ifdef SINGULAR_EXTENSION
TUNABLE(SINGULAR_EXTENSION_DEPTH, 8*DEPTH_INCREMENT, 6*DEPTH_INCREMENT, 10*DEPTH_INCREMENT);
#endif
TUNABLE(PROBCUT_DEPTH, 5*DEPTH_INCREMENT, 3*DEPTH_INCREMENT, 8*DEPTH_INCREMENT);
TUNABLE(PROBCUT_MARGIN, static_cast<int>(1.25*Params::PAWN_VALUE), static_cast<int>(0.75*Params::PAWN_VALUE),
        static_cast<int>(1.75*Params::PAWN_VALUE));
TUNABLE(LMR_DEPTH, 3*DEPTH_INCREMENT, DEPTH_INCREMENT, 4*DEPTH_INCREMENT);
TUNABLE(LMR_BASE_NON_PV, 50, 0, 100);
TUNABLE(LMR_BASE_PV, 30, 0, 100);
TUNABLE(LMR_DIV_NON_PV, 180, 100, 360);
TUNABLE(LMR_DIV_PV, 225, 100, 360);
TUNABLE(NULL_MOVE_BASE_REDUCTION, 3, 3, 4);
TUNABLE(NULL_MOVE_DEPTH_DIVISOR, 3, 3, 6);
TUNABLE(NULL_MOVE_LOW_MAT_EXTENSION, 3, 0, 4);
TUNABLE(NULL_MOVE_DEPTH_DIVISOR_LOW_MAT, 3, 0, 6);
TUNABLE(NULL_MOVE_EVAL_FACTOR, 350, 160, 600);
TUNABLE(NULL_MOVE_MAX_EVAL_REDUCTION, 3, 2, 5);
static constexpr int CHECKS_IN_QSEARCH = 1;
// Depth limits for the strength reduction feature:
static constexpr int STRENGTH_DEPTH_LIMITS[40] = {
    1,1,1,1,1,1,1,2,2,3,3,3,4,5,6,7,7,8,9,9,
    9,9,9,9,9,9,10,10,10,11,11,11,12,12,13,13,13,14,16,20};

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
static int LMP_MOVE_COUNT[2][16];

TUNABLE(LMP_BASE_IMP, 4, 0, 6);
TUNABLE(LMP_BASE_NON_IMP, 2, 0, 4);
TUNABLE(LMP_DEPTH, 9, 8, 15);
TUNABLE(LMP_EXP_IMP, 204, 120, 230);
TUNABLE(LMP_EXP_NON_IMP, 195, 120, 230);
TUNABLE(LMP_SLOPE_IMP, 75, 10, 150);
TUNABLE(LMP_SLOPE_NON_IMP, 70, 10, 150);

#ifdef RAZORING
TUNABLE(RAZOR_MARGIN, static_cast<score_t>(2.75*Params::PAWN_VALUE),
        static_cast<int>(1.5*Params::PAWN_VALUE),
        static_cast<int>(3.5*Params::PAWN_VALUE));
TUNABLE(RAZOR_MARGIN_SLOPE, static_cast<score_t>(1.25*Params::PAWN_VALUE),
        static_cast<int>(0.75*Params::PAWN_VALUE),
        static_cast<int>(2.0*Params::PAWN_VALUE));
#endif
TUNABLE(FUTILITY_MARGIN_BASE, static_cast<int>(0.25*Params::PAWN_VALUE), 0,
        static_cast<int>(0.75*Params::PAWN_VALUE));
TUNABLE(FUTILITY_MARGIN_SLOPE, static_cast<int>(0.95*Params::PAWN_VALUE),
        static_cast<int>(0.5*Params::PAWN_VALUE),
        static_cast<int>(1.5*Params::PAWN_VALUE));
TUNABLE(CAPTURE_FUTILITY_MARGIN_BASE, static_cast<int>(2.0*Params::PAWN_VALUE), 0,
        static_cast<int>(3.5*Params::PAWN_VALUE));
TUNABLE(CAPTURE_FUTILITY_MARGIN_SLOPE, static_cast<int>(2.5*Params::PAWN_VALUE),
        static_cast<int>(1.0*Params::PAWN_VALUE),
        static_cast<int>(3.5*Params::PAWN_VALUE));
TUNABLE(STATIC_NULL_PRUNING_DEPTH, 6*DEPTH_INCREMENT, 4*DEPTH_INCREMENT, 10*DEPTH_INCREMENT);
TUNABLE(STATIC_NULL_MARGIN_MIN,static_cast<int>(0.25*Params::PAWN_VALUE), 0,
        static_cast<int>(1.0*Params::PAWN_VALUE));
TUNABLE(STATIC_NULL_MARGIN_SLOPE,static_cast<int>(0.75*Params::PAWN_VALUE),
        static_cast<int>(0.25*Params::PAWN_VALUE),
        static_cast<int>(1.225*Params::PAWN_VALUE));
TUNABLE(QSEARCH_FUTILITY_PRUNE_MARGIN, static_cast<int>(1.4*Params::PAWN_VALUE),
        static_cast<int>(0.9*Params::PAWN_VALUE),
        static_cast<int>(2.0*Params::PAWN_VALUE));
TUNABLE(QSEARCH_SEE_PRUNE_MARGIN, static_cast<int>(1.25*Params::PAWN_VALUE),
        static_cast<int>(0.5*Params::PAWN_VALUE),
        static_cast<int>(2.0*Params::PAWN_VALUE));

static inline int IIDDepth(bool pv) {
    return pv ? IID_DEPTH_PV : IID_DEPTH_NON_PV;
}

static inline int futilityHistoryThreshold(bool improving) {
    return improving ? FUTILITY_HISTORY_THRESHOLD_IMP : FUTILITY_HISTORY_THRESHOLD_NON_IMP;
}

static inline int historyPruningThreshold(bool improving) {
    return improving ? HISTORY_PRUNING_THRESHOLD_IMP : HISTORY_PRUNING_THRESHOLD_NON_IMP;
}

// global vars are updated only once this many nodes (to minimize
// thread contention for global memory):
static constexpr int NODE_ACCUM_THRESHOLD = 16;

#ifdef SMP_STATS
static constexpr int SAMPLE_INTERVAL = 10000/NODE_ACCUM_THRESHOLD;
#endif

static constexpr int Illegal = Constants::INVALID_SCORE;

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

static void updatePV([[maybe_unused]] const Board &board, NodeInfo *node, NodeInfo *fromNode, Move move, int ply, [[maybe_unused]] bool mainThread)
{
    node->pv[ply] = move;
    if (fromNode->pv_length) {
        memcpy((void*)(node->pv+ply+1),(void*)(fromNode->pv+ply+1),
            sizeof(Move)*fromNode->pv_length);
    }
    node->pv_length = fromNode->pv_length+1;
#if defined(_DEBUG) || defined(_TRACE)
#ifdef _TRACE
    if (mainThread && node->pv_length>0) {
        indent(ply); std::cout << "PV: ";
    }
#endif
    Board board_copy(board);
    for (int i = ply; i < node->pv_length+ply; i++) {
        assert(i<Constants::MaxPly);
#ifdef _TRACE
        if (mainThread) {
            MoveImage(node->pv[i],std::cout);
            std::cout << ' ';
        }
#endif
        assert(legalMove(board_copy,node->pv[i]));
        board_copy.doMove(node->pv[i]);
    }
#ifdef _TRACE
    if (mainThread && node->pv_length>0) {
        std::cout << std::endl;
    }
#endif
#endif
}

// record a new best move, return non-zero if cutoff occurs
static int updateMove(const Board &board, NodeInfo *node, Move move, score_t score, int ply, [[maybe_unused]] bool mainThread)
{
   int cutoff = 0;
   node->best_score = score;
   node->best = move;
#ifdef MOVE_ORDER_STATS
   node->best_count = node->num_legal-1;
#endif
   if (score >= node->beta) {
#ifdef _TRACE
      if (mainThread) {
         indent(ply); std::cout << "beta cutoff" << std::endl;
      }
#endif
      node->cutoff++;
      cutoff++;
   }
   else {
      node->best_score = score;
      ::updatePV(board,node,(node+1),move,ply,mainThread);
   }
   return cutoff;
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
      timeCheckInterval(1000),
      timeCheckFactor(1.0),
      last_time_check(0),
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
      srcOpts(globals::options.search),
//#ifdef SYZYGY_TBS
//      tb_hit(0), tb_dtz(0), tb_score(Constants::INVALID_SCORE),
//#endif
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
                auto lmr_div = (p ? LMR_DIV_PV : LMR_DIV_NON_PV)/100.0;
                auto lmr_base = (p ? LMR_BASE_PV : LMR_BASE_NON_PV)/100.0;
                LMR_REDUCTION[p][d][moves] = 0;
                if (d > 0 && moves > 0) {
                    const double reduction = lmr_base + (log(d) * log(moves)) / lmr_div;
                    LMR_REDUCTION[p][d][moves] = static_cast<int>(DEPTH_INCREMENT*floor(2*reduction+0.5)/2);
                }
            }
        }
    }
    for (int d = 0; d <= LMP_DEPTH; d++) {
        LMP_MOVE_COUNT[0][d] = static_cast<int>(std::round(LMP_BASE_NON_IMP + LMP_SLOPE_NON_IMP * std::pow(d, LMP_EXP_NON_IMP/100.0)/100.0));
        LMP_MOVE_COUNT[1][d] = static_cast<int>(std::round(LMP_BASE_IMP + LMP_SLOPE_IMP * std::pow(d, LMP_EXP_IMP/100.0)/100.0));
        //        std::cout << LMP_MOVE_COUNT[0][d] << ' ' << LMP_MOVE_COUNT[1][d] << std::endl;
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
   int search_time_limit,
   int search_xtra_time,
   int search_ply_limit,
   bool isBackground,
   bool isUCI,
   Statistics &stat_buf,
   TalkLevel t,
   RootMoveGenerator::RootMoveList *moveList)
{
    MoveSet excludes, includes;
    Move result = findBestMove(board, srcType, search_time_limit, search_xtra_time,
                               search_ply_limit, isBackground, isUCI, stat_buf, t,
                               excludes, includes, moveList);
    return result;
}


Move SearchController::findBestMove(
   const Board &board,
   SearchType srcType,
   int search_time_limit,
   int search_xtra_time,
   int search_ply_limit,
   bool isBackground,
   bool isUCI,
   Statistics &stat_buf,
   TalkLevel t,
   const MoveSet &moves_to_exclude,
   const MoveSet &moves_to_include,
   RootMoveGenerator::RootMoveList *moveList)
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
    timeCheckFactor = 1.0;
    last_time_check = 0;
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
      int new_ply_limit = STRENGTH_DEPTH_LIMITS[(2*globals::options.search.strength)/5];
      if (debugOut()) {
          std::cout << globals::debugPrefix << "strength=" << globals::options.search.strength << " ply limit=" <<
              new_ply_limit << std::endl;
      }
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
//#ifdef SYZYGY_TBS
//   tb_hit = 0;
//   tb_score = Constants::INVALID_SCORE;
//   tb_root_probes = tb_root_hits = 0;
//   if (srcOpts.use_tablebases) {
//       // Lock because the following calls is not thread-safe. In normal use
//       // we don't need to worry about this, but it is possible there are
//       // two concurrent SearchController instances in a program, in which case
//       // it matters.
//       {
//           std::unique_lock<std::mutex> lock(globals::syzygy_lock);
//           tb_hit = mg->rank_root_moves();
//       }
//       if (tb_hit) {
//           tb_root_probes += mg->moveCount();
//           tb_root_hits += mg->moveCount();
//           tb_score = mg->getMoveList()[0].tbScore;
//           // Store the tb value but do not use it set the search score - search values are based
//           // on DTM not DTZ.
//           stats->tb_value = tb_score;
//           // do not probe in the search
//           tb_probe_in_search = false;
//           if (debugOut()) {
//               std::cout << globals::debugPrefix << board << " root tb hit, score=";
//               Scoring::printScore(tb_score,std::cout);
//               std::cout << std::endl;
//           }
//       }
//   }
//#endif
   // set up move generator to obey include and exclude lists
   mg->filter(include,exclude);
   std::stringstream s;
   s << "filtered root moves:";
   for (const RootMoveGenerator::RootMove &m : mg->getMoveList()) {
       if (!(Flags(m.move) & Excluded)) {
           s << ' ';
           Notation::image(board,m.move,Notation::OutputFormat::SAN,s);
       }
   }
#ifdef _TRACE
   std::cout << s.str() << std::endl;
#endif
//#ifdef SYZYGY_TBS
//   if (debugOut() && tb_hit) {
//       std::cout << globals::debugPrefix << s.str() << std::endl;
//   }
//#endif
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
   Move best = rootSearch->ply0_search(moveList);
   delete [] rootStack;
   // Mark thread 0 complete.
   pool->setCompleted(0);

   if (debugOut()) std::cout << globals::debugPrefix << "waiting for thread completion" << std::endl;

   // Wait for all threads to complete
   pool->waitAll();

   if (debugOut()) {
       std::cout << globals::debugPrefix << "thread 0 depth=" << rootSearch->stats.completedDepth <<
           " score=";
       Scoring::printScore(rootSearch->stats.display_value,std::cout);
       std::cout << " failHigh=" << (int)stats->failHigh << " failLow=" <<
           (int)stats->failLow;
       std::cout << " pv=" << rootSearch->stats.best_line_image << std::endl;
   }
   bool subOpt = false;
   if (srcOpts.multipv == 1) {
       BestThreadResults res;
       getBestThreadStats(res,debugOut());
       // In reduced-strength mode, sometimes play a suboptimal move
       if (globals::options.search.strength < 100) {
           subOpt = suboptimal(res.bestStats,res.bestSearch);
       }
       best = res.bestStats->best_line[0];
       updateGlobalStats(*res.bestStats);
   } else {
       updateGlobalStats(rootSearch->stats);
   }

   // We are finished with the move generator - can delete
   delete mg;

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
   static constexpr int end_of_game[] = {0, 1, 0, 1, 1, 1, 1};
   StateType &state = stats->state;
   stats->end_of_game = end_of_game[(int)stats->state];
   // Don't resign if we selected a suboptimal move: allow it to be
   // played. If it is really bad, we will resign after the next
   // search.
   if (!uci && !stats->end_of_game && globals::options.search.can_resign && !subOpt) {
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
//#ifdef SYZYGY_TBS
//          // Don't resign a tb lost position with large dtz,
//          // unless we have a mate score, because the opponent can err
//          // if not using TBs.
//          && (stats->display_value != -Constants::TABLEBASE_WIN || tb_dtz < 30)
//#endif
         ) {
         if (debugOut()) {
             std::cout << globals::debugPrefix << "resigning game (low score)" << std::endl;
         }
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
      std::cout << ' ' << stats->futility_pruning_caps << " (" << std::setprecision(2) << 100.0*stats->futility_pruning_caps/stats->moves_searched << "%) futility (captures)" << std::endl;
      std::cout << ' ' << stats->history_pruning << " (" << std::setprecision(2) << 100.0*stats->history_pruning/stats->moves_searched << "%) history" << std::endl;
      std::cout << ' ' << stats->lmp << " (" << std::setprecision(2) << 100.0*stats->lmp/stats->moves_searched << "%) lmp" << std::endl;
      std::cout << ' ' << stats->see_pruning << " (" << std::setprecision(2) << 100.0*stats->see_pruning/stats->moves_searched << "%) SEE" << std::endl;
      std::cout << ' ' << stats->reduced << " (" << std::setprecision(2) << 100.0*stats->reduced/stats->moves_searched << "%) reduced" << std::endl;
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

void SearchController::rankMoves(
        const Board &board,
        int search_ply_limit,
        RootMoveGenerator::RootMoveList &moveList) {
    Statistics s;
    MoveSet includes,excludes;

    (void) findBestMove(board,FixedDepth,
                        Constants::INFINITE_TIME,
                        0,
                        search_ply_limit,
                        true,
                        false,
                        s,
                        TalkLevel::Silent,
                        includes,
                        excludes,
                        &moveList);
}

void SearchController::setContempt(score_t c)
{
    contempt = c;

    // propagate rating diff to searches
    pool->forEachSearch<&Search::setContemptFromController>();
}

void SearchController::setThreadCount(int count) {
   pool->resize(count);
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
    srcOpts = globals::options.search;
    // pool size is part of search options and may have changed,
    // so adjust that first:
    pool->resize(globals::options.search.ncpus);
    // update each search thread's local copy of the options:
    pool->forEachSearch<&Search::setSearchOptions>();
}

void SearchController::updateTBOptions() {
//#ifdef SYZYGY_TBS
//    srcOpts.use_tablebases = globals::options.search.use_tablebases;
//    srcOpts.syzygy_path = globals::options.search.syzygy_path;
//    srcOpts.syzygy_50_move_rule = globals::options.search.syzygy_50_move_rule;
//    srcOpts.syzygy_probe_depth = globals::options.search.syzygy_probe_depth;
//    pool->forEachSearch<&Search::updateTBOptions>();
//#endif
}

void SearchController::setMultiPV(int multipv_count) {
    srcOpts.multipv = multipv_count;
    // update each search thread's local copy of the options:
    pool->forEachSearch<&Search::setMultiPV>(multipv_count);
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
    BestThreadResults res;
    getBestThreadStats(res,false);
    Statistics *bestStats = res.bestStats;
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

void SearchController::historyBasedTimeAdjust(const Statistics &s) {
    // Increase the time limit if pv has changed recently and/or score
    // is dropping over the past few iterations. Decrease limit if
    // score seems stable and is not dropping. Note: we calculate the
    // adjustment even if in a ponder search, so we can apply it later
    // if a ponder hit occurs.
    if (int(s.depth) > globals::options.search.widePlies+6) {
        // Look back over the past few iterations
        Move pv = s.best_line[0];
        double pvChangeFactor = 0.0;
        const score_t score = s.display_value;
        score_t max_score = -Constants::MATE;
        int divisor = 1;
        for (int depth = int(s.depth)-2; depth >= 0 && depth>=int(s.depth)-6; --depth, divisor *= 2) {
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
            maxBoostDepth = s.depth;
        }
        if (searchHistoryBoostFactor==0.0 && maxBoostFactor<0.25) {
            // We have not changed pv recently, score is not dropping,
            // and thinking time was not increased. See if we can
            // reduce the time target.
            searchHistoryReductionFactor = 1.0-maxBoostFactor-0.75*double(maxBoostDepth)/s.depth;
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

uint64_t SearchController::getTimeLimit() const noexcept {
    if (typeOfSearch == TimeLimit && time_limit != Constants::INFINITE_TIME) {
        // time boost/decrease based on search history:
        int64_t extension = bonus_time;
        if (fail_low_root_extend) {
            // presently failing low, allow up to max extra time
            extension += int64_t(xtra_time);
        }
        else if (fail_high_root || fail_high_root_extend) {
            // extend time for fail high, but less than for
            // failing low
            extension += int64_t(xtra_time)/2;
        }
        extension = std::max<int64_t>(-int64_t(time_target),std::min<int64_t>(int64_t(xtra_time),extension));
        return uint64_t(int64_t(time_target) + extension);
    } else {
        return time_limit;
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
    random_engine.seed(getRandomSeed());
}

void Search::updatePV(const Board &b, Move m, int ply)
{
    ::updatePV(b, node, (node+1), m, ply, mainThread());
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
    uint64_t elapsed_time = controller->elapsed_time.exchange(getElapsedTime(controller->startTime,current_time));
    uint64_t last_time_check = controller->last_time_check.exchange(elapsed_time);

    // Do not allow time-based termination if the search has not
    // completed even one iteration, since we may be failing low and
    // have no move
    if (controller->stats->completedDepth > 0 && (controller->typeOfSearch == FixedTime ||
                                                  controller->typeOfSearch == TimeLimit)) {
       const uint64_t limit = controller->typeOfSearch == FixedTime ? controller->time_target : controller->getTimeLimit();
       if (elapsed_time >= limit) {
           return 1;
       }
       // If we are not checking time often enough, adjust down the time check interval
       if (last_time_check > 0 && elapsed_time >= 100 && (elapsed_time - last_time_check) > limit / 10) {
           controller->timeCheckFactor *= std::max(0.5, 1.0 - 3*static_cast<double>(elapsed_time - last_time_check)/limit);
           controller->timeCheckInterval = controller->computeTimeCheckInterval(stats.num_nodes);
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

void Search::showStatus(const Board &b, Move best, bool faillow,
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
            Notation::image(b, best, Notation::OutputFormat::SAN,std::cout);
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

score_t Search::drawScore(const Board & b) const {
    return controller->drawScore(b, &stats);
}

//#ifdef SYZYGY_TBS
//score_t Search::tbScoreAdjust(const Board &b,
//                    score_t value,int tb_hit,score_t tb_score) const
//{
//#ifdef _TRACE
//   if (mainThread()) {
//      std::cout << "tb score adjust: input=";
//      Scoring::printScore(tb_score,std::cout);
//      std::cout << std::endl;
//   }
//#endif
//   score_t output = value;
//   if (tb_hit && !Scoring::mateScore(value)) {
//      // If a Syzygy tablebase hit set the score based on that. But
//      // don't override a mate score found with search.
//      if (tb_score == Constants::TABLEBASE_WIN) {
//          output = tb_score;
//      }
//      else if (tb_score == 0 || std::abs(tb_score) == SyzygyTb::CURSED_SCORE) {
//         output = drawScore(b);
//      }
//      else if (tb_score == -Constants::TABLEBASE_WIN) {
//         // loss
//         output = -Constants::TABLEBASE_WIN;
//      }
//   }
//#ifdef _TRACE
//   if (mainThread()) {
//      std::cout << "tb score adjust: output=";
//      Scoring::printScore(output,std::cout);
//      std::cout << std::endl;
//   }
//#endif
//   return output;
//}
//#endif

template <bool quiet>
static score_t futilityMargin(int depth)
{
    if (quiet)
      return FUTILITY_MARGIN_BASE + std::max<int>(depth/DEPTH_INCREMENT,1)*FUTILITY_MARGIN_SLOPE;
    else
      return CAPTURE_FUTILITY_MARGIN_BASE + (depth/DEPTH_INCREMENT)*CAPTURE_FUTILITY_MARGIN_SLOPE;
}

#ifdef STATIC_NULL_PRUNING
static score_t staticNullPruningMargin(int depth, int improving)
{
    // formula similar to Obsidian
    int d = depth / DEPTH_INCREMENT - improving;
    return std::max<int>(STATIC_NULL_MARGIN_MIN, STATIC_NULL_MARGIN_SLOPE * d);
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
    return quiet ? -p*static_cast<int>(0.75*Params::PAWN_VALUE) :
        -p*p*static_cast<int>(0.2*Params::PAWN_VALUE);
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

void Search::updateStats(const Board &b, NodeInfo *n, int iteration_depth,
                         score_t score)
{
    assert(stats.multipv_count < Statistics::MAX_PV);
    stats.value = score;
    stats.depth = iteration_depth;
    stats.display_value = stats.value;
//#ifdef SYZYGY_TBS
//    // Correct if necessary the display value, used for score
//    // output and resign decisions, based on the tb information:
//    if (stats.tb_value != Constants::INVALID_SCORE) {
//       stats.display_value = tbScoreAdjust(b,
//                                           stats.value,
//                                           1,
//                                           stats.tb_value);
//    }
//#endif
    // note: retain previous best line if we do not have one here
    if (n->pv_length == 0) {
#ifdef _TRACE
        if (mainThread()) std::cout << "warning: pv length is zero." << std::endl;;
#endif
        return;
    }
    else if (IsNull(n->pv[0])) {
#ifdef _TRACE
        if (mainThread()) std::cout << "warning: pv is null." << std::endl;;
#endif
        return;
    }
    n->best = n->pv[0];                     // ensure "best" is non-null
    assert(!IsNull(n->best));
    Board board_copy(b);
    stats.best_line[0] = NullMove;
    int i = 0;
    stats.best_line_image.clear();
    std::stringstream sstr;
    const Move *moves = n->pv;
    while (i < n->pv_length && i<Constants::MaxPly-1 && !IsNull(moves[i])) {
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
       if (n->pv_length < 2) {
          // get the next move from the hash table, if possible
          // (for pondering)
          HashEntry entry;
          HashEntry::ValueType result =
              controller->hashTable.searchHash(board_copy.hashCode(b.repCount(2)),
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

Move Search::ply0_search(RootMoveGenerator::RootMoveList *moveList)
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
   iterationDepth = 0;
   for (int nominalDepth = 1;(iterationDepth = controller->nextSearchDepth(iterationDepth,ti->index,
            controller->ply_limit)) <= controller->ply_limit &&
            !terminate; ++nominalDepth) {
      MoveSet excluded(controller->exclude);
      for (stats.multipv_count = 0;
           stats.multipv_count < (stats.multipv_limit = std::min<int>(mg.moveCount(),srcOpts.multipv))
                                  && !terminate;
           stats.multipv_count++) {
         score_t lo_window, hi_window;
         score_t aspirationWindow = ASPIRATION_WINDOW[0];

         if (srcOpts.multipv > 1) {
             stats.clearSearchState();
             if (nominalDepth > 1) {
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
         } else if (iterationDepth <= srcOpts.widePlies) {
            lo_window = std::max<score_t>(-Constants::MATE,value - srcOpts.wideWindow - aspirationWindow/2);
            hi_window = std::min<score_t>(Constants::MATE,value + srcOpts.wideWindow + aspirationWindow/2);
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
            value = ply0_search(mg, lo_window, hi_window,
                                iterationDepth,
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
            stats.failHigh = value >= hi_window;
            stats.failLow = value <= lo_window;
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
                    hi_window = Constants::MATE;
                } else {
                    if (iterationDepth <= globals::options.search.widePlies) {
                        aspirationWindow += 2*globals::options.search.wideWindow;
                    }
                    hi_window = std::min<score_t>(Constants::MATE, lo_window + aspirationWindow);
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
                    if (iterationDepth <= globals::options.search.widePlies) {
                        aspirationWindow += 2*globals::options.search.wideWindow;
                    }
                    lo_window = std::max<score_t>(nominalDepth-Constants::MATE,hi_window - aspirationWindow);
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
         // set time check interval based on NPS
         if (controller->elapsed_time > 100) {
             controller->timeCheckInterval = controller->computeTimeCheckInterval(stats.num_nodes);
         }
         if (debugOut() && !terminate && mainThread() && controller->elapsed_time > 100) {
             std::cout << globals::debugPrefix << "elapsed time=" << controller->elapsed_time << " time check interval=" <<
                 controller->timeCheckInterval << std::endl;
         }
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
             !(globals::options.search.can_resign && stats.display_value <= srcOpts.resign_threshold)) {
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
            if (srcOpts.multipv == 1 && static_cast<int>(iterationDepth) <= globals::options.search.widePlies) {
                // save ranked moves
                mg.reorderByScore();
                rootMoves.clear();
                std::copy(mg.getMoveList().begin(),
                          mg.getMoveList().end(),std::back_inserter(rootMoves));
            }
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
                // Exit in case of forced checkmate.
                // Note: use nominalDepth for bound, in case we skipped an iteration
                if (value <= nominalDepth - Constants::MATE && !IsNull(stats.best_line[0])) {
                    // We're either checkmated or we certainly will be, so
                    // quit searching.
                    if (mainThread() && debugOut()) std::cout << globals::debugPrefix << "terminating, low score" << std::endl;
#ifdef _TRACE
                    std::cout << "terminating, low score" << std::endl;
#endif
                    controller->terminateNow();
               }
               else if (value >= Constants::MATE - nominalDepth - 1 && iterationDepth>=2) {
                   // found a forced mate, terminate
                   if (mainThread() && debugOut()) {
                       std::cout << globals::debugPrefix << "terminating, high score" << std::endl;
                   }
#ifdef _TRACE
                   if (mainThread()) {
                       std::cout << "terminating, high score" << std::endl;
                       std::cout << "nominalDepth=" << nominalDepth << " value=" << value << " mate threshold=" << Constants::MATE - nominalDepth - 1 << std::endl;
                   }
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
   if (mainThread() && moveList != nullptr) {
       mg.reorderByScore();
       moveList->clear();
       std::copy(mg.getMoveList().begin(),
                 mg.getMoveList().end(),std::back_inserter(*moveList));
   }
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
   return node->best;
}

score_t Search::ply0_search(RootMoveGenerator &mg, score_t alpha, score_t beta,
                            int iteration_depth,
                            int depth,
                            const MoveSet &exclude)
{
    // implements alpha/beta search for the top most ply.  We use
    // the negascout algorithm.

    --controller->time_check_counter;
    nodeAccumulator++;

    int in_check = 0;

    const bool wide = iteration_depth <= srcOpts.widePlies;

    in_check = (board.checkStatus() == InCheck);
    BoardState save_state = board.state;

    score_t try_score = alpha;
    //
    // Re-sort the ply 0 moves and re-init move generator.
    if (iteration_depth > 1) {
       mg.reorder(node->best,node->best_score,iteration_depth,false);
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
    node->num_quiets = node->num_captures = node->num_legal = 0;
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
    std::uniform_int_distribution<int> skip_move_dist(0,128);
    while (!node->cutoff && !terminate) {
        Move move;
        if ((move = mg.nextMove(move_index))==NullMove) break;
        if (IsUsed(move) || IsExcluded(move)) {
            --stats.mvleft;
            continue;     // skip move
        }
        node->last_move = move;
        if (CaptureOrPromotion(move)) {
            assert(node->num_captures<Constants::MaxCaptures);
            node->captures[node->num_captures++] = move;
        }
        else {
            assert(node->num_quiets<Constants::MaxMoves);
            node->quiets[node->num_quiets++] = move;
        }
        node->num_legal++; // all generated moves are legal at ply 0
        if (mainThread() && controller->uci && controller->elapsed_time > 300) {
            controller->uciSendInfos(board, move, move_index, iteration_depth);
        }
#ifdef _TRACE
        if (mainThread()) {
            std::cout << globals::debugPrefix << "trying 0. ";
           MoveImage(move,std::cout);
           std::cout << " (" << move_index << "/" << mg.moveCount();
           std::cout << ")" << std::endl;
        }
#endif
        CheckStatusType in_check_after_move = board.wouldCheck(move);
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
            std::cout << globals::debugPrefix << "0. ";
            MoveImage(move,std::cout);
            std::cout << " (" << move_index << '/' << mg.moveCount() << ") ";
            std::cout << try_score << std::endl;
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
               if (mainThread() && !srcOpts.multipv) controller->uciSendInfos(board, move, move_index, iteration_depth);
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
            iteration_depth == controller->ply_limit &&
            move_index < mg.moveCount()) {
            // we are on the last iteration in reduced-strength mode,
            // see if we should slow down
            controller->elapsed_time = getElapsedTime(controller->startTime,getCurrentTime());
            auto limit = controller->getTimeLimit();
            if (controller->elapsed_time < 2*limit/3) {
                // waste some time
                uint64_t waitTime;
                // sleep in increments if the wait time is long
                while (limit >= controller->elapsed_time &&
                      (waitTime = (limit - controller->elapsed_time)/(4*(mg.moveCount()-move_index))) >= 50 &&
                      !terminate) {
                    auto thisWait = std::min<uint64_t>(waitTime,1000);
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
    else if (!IsNull(node->best) && board.checkStatus() != InCheck) {
        if (!CaptureOrPromotion(node->best)) {
            context.setKiller(node->best, node->ply);
        }
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
    stats->hash_hits = stats->hash_searches = stats->futility_pruning =
        stats->futility_pruning_caps = (uint64_t)0;
    stats->history_pruning = stats->lmp = stats->see_pruning = stats->null_cuts = (uint64_t)0;
    stats->check_extensions = stats->capture_extensions = stats->pawn_extensions =
        stats->singular_extensions = 0L;
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
       stats->futility_pruning_caps += s.futility_pruning_caps;
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

void SearchController::getBestThreadStats(BestThreadResults &res, bool trace) const {
    Statistics *bestStats = &(rootSearch->stats);
    Search *bestSearch = rootSearch;
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
            if (IsNull(bestStats->best_line[0]) ||
                (threadStats.display_value > bestStats->display_value &&
                (threadStats.completedDepth >= bestStats->completedDepth ||
                 threadStats.value >= Constants::MATE_RANGE))) {
                bestSearch = pool->data[thread]->work;
                bestStats = &threadStats;
            }
        }
    }
    res.bestSearch = bestSearch;
    res.bestStats = bestStats;
}

unsigned SearchController::nextSearchDepth(unsigned current_depth, unsigned thread_id,
    unsigned max_depth)
{
    // Vary the search depth by thread, to make it less likely threads are searching
    // the same positions.
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

bool SearchController::suboptimal(Statistics *bestStats, const Search *bestSearch) {
    const int &strength = globals::options.search.strength;
    if (mg->moveCount() < 2) {
        return false;
    }
    const RootMoveGenerator::RootMoveList &rootMoves = bestSearch->rootMoves;
    auto &random_engine = rootSearch->random_engine;
    std::uniform_int_distribution<int> dist(0,100);
    RootMoveGenerator::RootMove substitute;
    const int n = rootMoves.size();
    assert(n);
    const int r  = dist(random_engine);
    score_t best = rootMoves[0].score;
    Move bestMove = rootMoves[0].move;
    static constexpr double tolerances[40] = {12.3, 11.8, 11.3, 10.4, 9.5, 8.9, 8.4, 7.7, 7.0, 5.9, 4.35, 3.1, 3.5, 3.25, 3.0, 2.0, 2.2, 2.1, 2.0, 1.9,
                                              1.7, 1.71, 1.66, 1.6, 1.53, 1.47, 1.44, 1.35, 1.28, 1.19, 1.1, 1.08, 1.03, 0.96, 0.88, 0.82, 0.85, 0.82, 0.79, 0.75 };
    const score_t tolerance = static_cast<score_t>(Params::PAWN_VALUE*tolerances[(2*strength)/5]);
    static constexpr int probs[40] = {73,70,65,61,59,52,49,46,42,34,32,29,27,26,26,26,25,25,23,20,
                                      18,17,17,16,15,15,15,14,13,13,13,12,11,10,9,8,8,8,6,5};
    int p = probs[(2*strength)/5];
    if (p == 0 || tolerance == 0) return false;
    int y = std::max<int>(0,(25-strength)/4) + std::max<int>(0,(10-strength)/4) +
            (strength <= 50 ? std::max<int>(0,(60-strength)/7) : 0);
    if (best >= Constants::MATE-1 || CaptureOrPromotion(bestMove) || initialBoard.checkStatus() == InCheck) {
        p -= static_cast<int>((p*(16-y))/32);
    }
    int start;
    std::string strategy;
    if (r < p/4 + y/2) {
        start = std::max<int>(0,n-5);
        strategy = "bottom";
    } else if (r < p/2 + y/2) {
        start = n/2;
        strategy = "middle";
    } else if (r < p) {
        start = 1;
        strategy = "top";
    } else {
        return false;
    }
    std::vector<int> candidates;
    if (debugOut()) {
        std::cout << globals::debugPrefix << "suboptimal: r=" << r << " p=" << p << " tolerance=" << tolerance << " strategy=" << strategy << std::endl;
    }
    for (int i = start; i < std::min<int>(n-1,start+5); ++i) {
        const RootMoveGenerator::RootMove &rm = rootMoves[i];
        if (debugOut()) {
            std::string move_image;
            Notation::image(initialBoard,rm.move,Notation::OutputFormat::SAN,move_image);
            std::cout << globals::debugPrefix << "suboptimal: move = " << move_image << ", diff = " << best-rm.score << std::endl;
        }
        // when strength < 50, sometimes allow moves worse than the tolerance value
        if (r >= 2*y && (best - rm.score > tolerance)) break;
        candidates.push_back(i);
    }
    if (debugOut()) {
        std::cout << globals::debugPrefix << "suboptimal: " << candidates.size() << " candidate(s)" << std::endl;
    }
    if (candidates.size() == 0) return false;
    else if (candidates.size() == 1) substitute = rootMoves[candidates[0]];
    else {
        std::uniform_int_distribution<int> pick(0,candidates.size()-1);
        substitute = rootMoves[candidates[pick(random_engine)]];
    }
    if (IsNull(substitute.move) || MovesEqual(substitute.move,bestStats->best_line[0])) {
        return false;
    } else {
        bestStats->best_line[0] = substitute.move;
        bestStats->best_line[1] = NullMove;
        std::string move_image;
        Notation::image(initialBoard,substitute.move,Notation::OutputFormat::SAN,move_image);
        bestStats->best_line_image = move_image;
        bestStats->display_value = bestStats->value = substitute.score;
        if (debugOut()) {
            std::cout << globals::debugPrefix << "selected suboptimal move " << move_image << std::endl;
        }
        return true;
    }
}

int SearchController::computeTimeCheckInterval(uint64_t num_nodes) const noexcept {
    return static_cast<int>((20L*num_nodes*timeCheckFactor)/(elapsed_time*NODE_ACCUM_THRESHOLD));
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
   if (inCheck || depth >= 1-CHECKS_IN_QSEARCH) {
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
      if (hashValue != Constants::INVALID_SCORE && (result == HashEntry::Valid ||
           ((result == HashEntry::UpperBound && hashValue <= node->alpha) ||
            (result == HashEntry::LowerBound && hashValue >= node->beta)))) {
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
           const score_t score = HashEntry::hashValueToScore(hashEntry.getValue(),node->ply);
           if (result == (score > node->eval ? HashEntry::LowerBound :
                          HashEntry::UpperBound)) {
               node->eval = score;
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
       QSearchMoveGenerator qmg(board,hashMove);
       Move move;
       while (!IsNull(move=qmg.nextMove())) {
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
       // Do checks in qsearch, if enabled
       if (depth >= 1-CHECKS_IN_QSEARCH) {
           QSearchCheckGenerator mg(board,disc);
           Move m;
           while (!IsNull(m = mg.nextCheck())) {
               if (MovesEqual(m,hashMove)) continue;
#ifdef _TRACE
               if (mainThread()) {
                   indent(ply);
                   std::cout << "trying " << ply << ". ";
                   MoveImage(m,std::cout);
                   std::cout << '+' << std::endl;
               }
#endif
               // prune checks that cause loss of the checking piece (but not
               // discovered checks)
               if (!disc.isSet(StartSquare(m)) && !seeSign(board,m,0)) {
#ifdef _TRACE
                   if (mainThread()) {
                       indent(ply); std::cout << "pruned" << std::endl;
                   }
#endif
                   continue;
               }
               else if (board.isPinned(board.sideToMove(),m)) {
                   // Move generator only produces pseudo-legal checking
                   // moves, and in the next ply we will only consider
                   // evasions. So need to ensure here that in making a
                   // check we do not expose our own King to check.
                   continue;
               }
               node->last_move = m;
               board.doMove(m,node);
               // verify opposite side in check:
               assert(board.anyAttacks(board.kingSquare(board.sideToMove()),board.oppositeSide()));
               // and verify quick check confirms it
               assert(board.checkStatus(m)==InCheck);
               // We know the check status so set it, so it does not
               // have to be computed
               board.setCheckStatus(InCheck);
               try_score = -quiesce(-node->beta,-node->best_score,
                                    ply+1,depth-1);
               board.undoMove(m,state);
               if (try_score != Illegal) {
#ifdef _TRACE
                   if (mainThread()) {
                       indent(ply);
                       std::cout << ply << ". ";
                       MoveImage(m,std::cout);
                       std::cout << ' ' << try_score << std::endl;
                   }
#endif
                   if (try_score > node->best_score) {
                       node->best_score = try_score;
                       node->best = m;
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
                   MoveImage(m,std::cout);
                   std::cout << " (illegal)" << std::endl;
               }
#endif
           }
       }
   }
   search_end:
   assert(node->best_score >= -Constants::MATE && node->best_score <= Constants::MATE);
   // do not store upper bound mate scores
   //   if (!(node->best_score >= Constants::MATE_RANGE) && node->best_score <= node->alpha) {
   //       storeHash(hash,node->best,tt_depth);
   //   }
   if (node->inBounds(node->best_score)) {
       if (!IsNull(node->best)) {
           updatePV(board,node->best,ply);
       }
   }
   return node->best_score;
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

bool Search::prune(const Board &b,
                   NodeInfo *n,
                   CheckStatusType in_check_after_move,
                   int moveIndex,
                   int improving,
                   Move m) {
    assert(n->ply > 0);
    if (n->num_legal &&
        b.checkStatus() == NotInCheck &&
        b.getMaterial(b.sideToMove()).hasPieces() &&
        n->best_score > -Constants::MATE_RANGE) {
        const bool quiet = !CaptureOrPromotion(m) && in_check_after_move != InCheck;
        int depth = n->depth;
        // for pruning decisions, use modified depth but not the same as
        // regular reduced search depth (idea from Laser)
        const int pruneDepth = quiet ? depth - lmr(n,depth,moveIndex) : depth;
        if (quiet) {
            // do not use pruneDepth for LMP
            if (GetPhase(m) >= MoveGenerator::HISTORY_PHASE &&
                moveIndex > lmpCount(depth,improving)) {
#ifdef SEARCH_STATS
                ++stats.lmp;
#endif
#ifdef _TRACE
                if (mainThread()) {
                    indent(n->ply); std::cout << "LMP: pruned" << std::endl;
                }
#endif
                return true;
            }
            // History pruning.
            const int hist = context.historyScore(m, n, board.sideToMove());
            if (pruneDepth <= (3-improving)*DEPTH_INCREMENT &&
                context.getCmHistory(n,m) < historyPruningThreshold(improving) &&
                context.getFuHistory(n,m) < historyPruningThreshold(improving)) {
#ifdef _TRACE
                if (mainThread()) {
                    indent(n->ply); std::cout << "history: pruned" << std::endl;
                }
#endif
#ifdef SEARCH_STATS
                ++stats.history_pruning;
#endif
                return true;
            }
            // futility pruning, enabled at low depths. Do not prune
            // moves with good history.
            if (pruneDepth <= FUTILITY_DEPTH && hist < futilityHistoryThreshold(improving)) {
                // Threshold was formerly increased with the move index
                // but this tests worse now.
                score_t threshold = n->alpha - futilityMargin<true>(pruneDepth);
                if (n->eval == Constants::INVALID_SCORE) {
                    n->eval = n->staticEval = evalu8(b);
                }
                if (n->eval < threshold) {
#ifdef SEARCH_STATS
                    ++stats.futility_pruning;
#endif
#ifdef _TRACE
                    if (mainThread()) {
                        indent(n->ply); std::cout << "futility: pruned" << std::endl;
                    }
#endif
                    return true;
                }
            }
        } else {
            if (pruneDepth <= CAPTURE_FUTILITY_DEPTH) {
                if (n->eval == Constants::INVALID_SCORE) {
                    n->eval = n->staticEval = evalu8(b);
                }
                score_t margin = futilityMargin<false>(pruneDepth) +
                    Params::maxValue(m) +
                    context.captureHistoryScore(b, m) / CAPTURE_FUTILITY_HISTORY_DIVISOR;
                if (n->eval + margin < node->alpha) {
#ifdef SEARCH_STATS
                    ++stats.futility_pruning_caps;
#endif
#ifdef _TRACE
                    if (mainThread()) {
                        indent(n->ply); std::cout << "futility (captures): pruned" << std::endl;
                    }
#endif
                    return true;
                }
            }
        }
        // SEE pruning. Losing captures and checks and moves that put pieces en prise
        // are pruned at low depths.
        if (pruneDepth <= SEE_PRUNING_DEPTH && GetPhase(m) > MoveGenerator::WINNING_CAPTURE_PHASE) {
            if (!seeSign(b,m,seePruningMargin(pruneDepth,quiet))) {
#ifdef SEARCH_STATS
                ++stats.see_pruning;
#endif
#ifdef _TRACE
                if (mainThread()) {
                    indent(n->ply); std::cout << "SEE: pruned" << std::endl;
                }
#endif
                return true;
            }
        }
    }
    return false;
}

int Search::extend(const Board &b,
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
    else if (passedPawnPush(b,move)) {
        extend += PAWN_PUSH_EXTENSION;
#ifdef SEARCH_STATS
        stats.pawn_extensions++;
#endif
    }
    else if (TypeOfMove(move) == Normal &&
             Capture(move) != Empty && Capture(move) != Pawn &&
             b.getMaterial(b.oppositeSide()).pieceCount() == 1 &&
             b.getMaterial(b.sideToMove()).noPieces()) {
        // Capture of last piece in endgame.
        extend += CAPTURE_EXTENSION;
#ifdef SEARCH_STATS
        ++stats.capture_extensions;
#endif
    }
    return std::min<int>(extend,DEPTH_INCREMENT);
}

int Search::reduce(const Board &b,
                   NodeInfo *n,
                   int moveIndex,
                   int improving,
                   int newDepth,
                   Move move) {
    int depth = n->depth;
    int reduction = 0;
    const bool quiet = !CaptureOrPromotion(move);

    // See if we do late move reduction.
    if (depth >= LMR_DEPTH && moveIndex >= 1+2*n->PV() && (quiet|| moveIndex > lmpCount(depth,improving))) {
       reduction += lmr(n,depth,moveIndex);
        if (!quiet) {
            reduction -= DEPTH_INCREMENT;
        }
        else {
            if (!n->PV() && !improving) {
                reduction += DEPTH_INCREMENT;
            }
            if (n->ply > 0) {
                if (b.checkStatus() != InCheck && GetPhase(move) < MoveGenerator::HISTORY_PHASE) {
                    // killer or refutation move
                    reduction -= DEPTH_INCREMENT;
                }
                // reduce less for good history
                reduction -= std::max<int>(
                    -2 * DEPTH_INCREMENT,
                    std::min<int>(2 * DEPTH_INCREMENT,
                                  DEPTH_INCREMENT *
                                      context.historyScore(move, n, board.sideToMove()) / HISTORY_REDUCTION_DIVISOR));
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
    else {
        // mate distance pruning
        score_t alpha = std::max<score_t>(node->alpha, -Constants::MATE + node->ply);
        score_t beta = std::min<score_t>(node->beta, Constants::MATE - node->ply - 1);
        if (alpha >= beta) return alpha;
    }
    Move hashMove = NullMove;
    using_tb = 0;
//#ifdef SYZYGY_TBS
//    if (srcOpts.use_tablebases) {
//        using_tb = board.men() <= globals::EGTBMenCount &&
//            controller->tb_probe_in_search &&
//            node->depth/DEPTH_INCREMENT >= srcOpts.syzygy_probe_depth;
//    }
//#endif
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
              if (!IsNull(hashMove)) {
                  if (CaptureOrPromotion(hashMove)) {
                      context.updateNonQuietMove(board, node, hashMove, hashValue >= node->beta);
                  }
                  else {
                      if (hashValue >= node->beta) {
                          context.setKiller(hashMove,node->ply);
                      }
                      // history bonus for hash move generated beta cutoff,
                      // penalty for hash move below alpha
                      context.updateQuietMove(board, node, hashMove, hashValue >= node->beta, hashValue <= node->alpha);
                  }
              }
              if (node->ply > 0 && (node-1)->num_legal <= 2 && !IsNull(node->last_move) && !CaptureOrPromotion(node->last_move)) {
                  context.updateQuietMove(board, node-1, (node-1)->last_move, false, true);
              }
              return hashValue;
          }
        }
    }
//#ifdef SYZYGY_TBS
//    if (using_tb && rep_count==0 && !(node->flags & IID) && board.state.moveCount == 0 && !board.castlingPossible()) {
//       stats.tb_probes++;
//       score_t tb_score;
//       int tb_hit = SyzygyTb::probe_wdl(board, tb_score, srcOpts.syzygy_50_move_rule != 0);
//       if (tb_hit) {
//            stats.tb_hits++;
//#ifdef _TRACE
//            if (mainThread()) {
//                indent(ply); std::cout << "EGTB hit: score " << tb_score << std::endl;
//            }
//#endif
//            // Put it in with a large depth so we will not
//            // overwrite - this entry is "exact" at all
//            // search depths, so effectively its depth is infinite.
//            controller->hashTable.storeHash(board.hashCode(rep_count),
//                (Constants::MaxPly-1)*DEPTH_INCREMENT,
//                age,
//                HashEntry::Valid,
//                HashEntry::scoreToHashValue(tb_score,node->ply),
//                Constants::INVALID_SCORE,
//                HashEntry::TB_MASK,
//                NullMove);
//            node->best_score = tb_score;               // unadjusted score
//            node->flags |= EXACT;
//            return node->best_score;
//        }
//    }
//#endif
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
    // Static null pruning, aka reverse futility pruning,
    // as in Protector, Texel, etc. Positions with very good
    // eval are pruned.
    if (pruneOk && depth <= STATIC_NULL_PRUNING_DEPTH && node->eval < Constants::TABLEBASE_WIN) {
        const score_t margin = staticNullPruningMargin(depth, improving);
        assert(node->eval != Constants::INVALID_SCORE);
        if (node->eval >= node->beta + margin) {
#ifdef _TRACE
           if (mainThread()) {
              indent(ply); std::cout << "static null pruned" << std::endl;
           }
#endif
#ifdef SEARCH_STATS
           ++stats.static_null_pruning;
#endif
           return (node->eval + node->beta)/2;
       }
    }
#endif

#ifdef RAZORING
    // razoring as in Stockfish
    if (pruneOk && depth <= RAZOR_DEPTH && board.getMaterial(board.sideToMove()).hasPieces()) {
        assert(node->eval != Constants::INVALID_SCORE);
        if (node->eval < node->beta - razorMargin(depth)) {
            (node+1)->clearNNUEState();
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
        int nu_depth = depth - NULL_MOVE_BASE_REDUCTION*DEPTH_INCREMENT -
            depth/(NULL_MOVE_DEPTH_DIVISOR + lowMat*NULL_MOVE_DEPTH_DIVISOR_LOW_MAT) -
            std::min<int>(NULL_MOVE_MAX_EVAL_REDUCTION*DEPTH_INCREMENT,
            int(DEPTH_INCREMENT*(node->eval-node->beta)/(NULL_MOVE_EVAL_FACTOR*Params::PAWN_VALUE/256)));
        if (lowMat) {
            nu_depth += DEPTH_INCREMENT*NULL_MOVE_LOW_MAT_EXTENSION / 4;
        }
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
                    // entering a new node w/o a move, so reset next node state
                    (node+1)->clearNNUEState();
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
#endif
                    return -Illegal;                  // previous move was illegal
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
                        if (!(hashHit && (hashEntry.depth() >= nu_depth + DEPTH_INCREMENT)) &&
                            value != -Constants::INVALID_SCORE) {
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
    if (IsNull(hashMove) && depth >= IIDDepth(node->PV())) {
        // reduced depth for search
        const int d = depth - IIDDepth(node->PV()) + DEPTH_INCREMENT;
#ifdef _TRACE
        if (mainThread()) {
            indent(ply); std::cout << "== start IID, depth = " << d
                << std::endl;
        }
#endif
        // Call search routine at lower depth to get a 1st move to try.
        (node+1)->clearNNUEState();
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
                node->clearNNUEState();
                score_t singularResult = search(nu_beta-1,nu_beta,node->ply+1,singularSearchDepth(depth),0,hashMove);
                if (singularResult < nu_beta) {
#ifdef _TRACE
                    if (mainThread()) {
                        indent(ply); std::cout << "singular extension" << std::endl;
                    }
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
                    score_t score = search(node->beta-1,node->beta,node->ply+1,(depth+3*DEPTH_INCREMENT)/2,0,hashMove);
                    if (score >= node->beta) {
                        // Another pruning idea from Stockfish: prune if the search is >= beta
                        // (note however current Stockfish reduces rather than prunes)
#ifdef _TRACE
                        if (mainThread()) {
                            indent(ply); std::cout << "non-hash move failed high: cutoff" << std::endl;
                        }

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
                  try_score=-search(-hibound, -node->best_score, ply+1, newDepth);
               else
                  try_score=-quiesce(-hibound,-node->best_score, ply+1, 0);
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
                  try_score=-search(-hibound, -node->best_score, ply+1, newDepth);
               else
                  try_score=-quiesce(-hibound,-node->best_score, ply+1, 0);
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
                if (updateMove(board,node,move,try_score,ply,mainThread())) {
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
    if (!IsNull(node->best) && board.checkStatus() != InCheck) {
        if (!CaptureOrPromotion(node->best)) {
            context.setKiller(node->best, node->ply);
            if (node->ply > 0) {
                context.setCounterMove(board,(node-1)->last_move,node->best);
            }
        }
        context.updateStats(board, node);
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

int Search::updateRootMove(const Board &b,
                           NodeInfo *n,
                           Move move, score_t score, int move_index)
{
   if (score > n->best_score)  {
      n->best = move;
      n->best_score = score;
#ifdef MOVE_ORDER_STATS
      n->best_count = n->num_legal-1;
#endif
      if (score >= n->beta) {
#ifdef _TRACE
         if (mainThread()) {
             std::cout << "ply 0 beta cutoff" << std::endl;
         }
#endif
         // set pv to this move so it is searched first the next time
         n->pv[0] = move;
         n->pv_length = 1;
         n->cutoff++;
         n->best_score = score;
         updateStats(board, n, iterationDepth,
                     n->best_score);
         if (mainThread()) {
            if (controller->uci && !srcOpts.multipv) {
               std::cout << "info score ";
               Scoring::printScoreUCI(score,std::cout);
               std::cout << " lowerbound" << std::endl;
            }
         }
         return 1;  // signal cutoff
      }
      ::updatePV(b,n,(n+1),move,0,mainThread());
      updateStats(b, n, iterationDepth,
                  n->best_score);
      if (mainThread() && srcOpts.multipv == 1) {
         if (move_index>1) {
            // best move has changed, show new best move
            showStatus(b,move,score <= n->alpha,score >= n->beta);
         }
      }
   }
   return 0;   // no cutoff
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
    // propagate options from controller to this search instance
    srcOpts = controller->srcOpts;
//#ifdef SYZYGY_TBS
//    // Propagate tb value from controller to stats
//    stats.tb_value = controller->tb_score;
//#endif

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

score_t Search::evalu8(const Board &b) {
	score_t score;
    const Material &ourMat = b.getMaterial(b.sideToMove());
    const Material &oppMat = b.getMaterial(b.oppositeSide());
    //bool imbalance = std::abs(int(ourMat.materialLevel()) - int(oppMat.materialLevel()))>10;
    const bool useClassical = !srcOpts.pureNNUE &&
        (//imbalance ||
         ourMat.men() + oppMat.men() <= 7);
    if (!useClassical && globals::nnueInitDone) {
        score = scoring.evalu8NNUE(b,node);
    } else {
        score = scoring.evalu8(b);
    }
   return score;
}
