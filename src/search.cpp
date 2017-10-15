// Copyright 1987-2017 by Jon Dart.  All Rights Reserved.

#include "search.h"
#include "globals.h"
#include "notation.h"
#include "movegen.h"
#include "hash.h"
#include "see.h"
#ifdef GAVIOTA_TBS
#include "gtb.h"
#endif
#ifdef NALIMOV_TBS
#include "nalimov.h"
#endif
#ifdef SYZYGY_TBS
#include "syzygy.h"
#endif
#include "legal.h"
#ifndef _WIN32
#include <errno.h>
#endif

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <list>
#include <vector>

#ifdef UCI_LOG
#include <fstream>
extern fstream ucilog;
#endif

static const int ASPIRATION_WINDOW[] =
    {(int)(0.375*Params::PAWN_VALUE),
     (int)(0.75*Params::PAWN_VALUE),
     (int)(1.5*Params::PAWN_VALUE),
     (int)(3.0*Params::PAWN_VALUE),
     (int)(6.0*Params::PAWN_VALUE),
      Constants::MATE};
static const int ASPIRATION_WINDOW_STEPS = 6;

#define VERIFY_NULL_SEARCH
#define STATIC_NULL_PRUNING
#define RAZORING
#define HELPFUL_MASTER
//#define SINGULAR_EXTENSION

static const int FUTILITY_DEPTH = 5*DEPTH_INCREMENT;
static const int RAZOR_DEPTH = 3*DEPTH_INCREMENT;
static const int SEE_PRUNING_DEPTH = 3*DEPTH_INCREMENT;
static const int PV_CHECK_EXTENSION = 3*DEPTH_INCREMENT/4;
static const int NONPV_CHECK_EXTENSION = DEPTH_INCREMENT/2;
static const int PAWN_PUSH_EXTENSION = DEPTH_INCREMENT;
static const int CAPTURE_EXTENSION = DEPTH_INCREMENT/2;
#ifdef SINGULAR_EXTENSION
static const int SINGULAR_EXTENSION_DEPTH = 6*DEPTH_INCREMENT;
#endif
static const int PROBCUT_DEPTH = 4*DEPTH_INCREMENT;
static const score_t PROBCUT_MARGIN = 2*Params::PAWN_VALUE;
static const score_t PROBCUT_MARGIN2 = int(0.33*Params::PAWN_VALUE);
static const int LMR_DEPTH = 3*DEPTH_INCREMENT;
static const double LMR_BASE[2] = {0.5,0.3};
static const double LMR_DIV[2] = {1.8,2.5};
static const int MAX_SPLIT_DEPTH=16*DEPTH_INCREMENT;
static const int MIN_SPLIT_DEPTH=5*DEPTH_INCREMENT;

#ifdef SINGULAR_EXTENSION
static int singularExtensionMargin(int depth)
{
//   return (depth*Params::PAWN_VALUE)/(100*DEPTH_INCREMENT);
   return Params::PAWN_VALUE/4;
}

static int singularExtensionDepth(int depth)
{
   return depth/2;
}
#endif

static int CACHE_ALIGN LMR_REDUCTION[2][64][64];

static const int LMP_DEPTH=10;

static const int LMP_MOVE_COUNT[11] = {3, 3, 5, 9, 15, 23, 33, 45, 59, 75, 93};

static const score_t RAZOR_MARGIN1 = score_t(0.9*Params::PAWN_VALUE);
static const score_t RAZOR_MARGIN2 = score_t(2.75*Params::PAWN_VALUE);
static const int RAZOR_MARGIN_DEPTH_FACTOR = 6;

static const score_t FUTILITY_MARGIN_BASE = (score_t)(0.25*Params::PAWN_VALUE);
static const score_t FUTILITY_MARGIN_SLOPE = (score_t)(0.5*Params::PAWN_VALUE);
static const score_t FUTILITY_MARGIN_SLOPE2 = (score_t)(0.2*Params::PAWN_VALUE);

static const int STATIC_NULL_PRUNING_DEPTH = 5*DEPTH_INCREMENT;

static const score_t QSEARCH_FORWARD_PRUNE_MARGIN = score_t(0.6*Params::PAWN_VALUE);

// global vars are updated only once this many nodes (to minimize
// thread contention for global memory):
static const int NODE_ACCUM_THRESHOLD = 16;

#ifdef SMP_STATS
static const int SAMPLE_INTERVAL = 10000/NODE_ACCUM_THRESHOLD;
#endif

static int Time_Check_Interval;

static const int Illegal = Scoring::INVALID_SCORE;
static const int PRUNE = -Constants::MATE;

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

static int FORCEINLINE passedPawnMove(const Board &board, Move move, int rank) {
  extern CACHE_ALIGN Bitboard passedW[64];
  extern CACHE_ALIGN Bitboard passedB[64];
  if (PieceMoved(move) == Pawn && Rank(DestSquare(move),board.sideToMove()) >= rank) {
    if (board.sideToMove() == White) {
        return Bitboard(board.pawn_bits[Black] & passedW[DestSquare(move)]).isClear();
    }
    else {
        return Bitboard(board.pawn_bits[White] & passedB[DestSquare(move)]).isClear();
    }
  }
  else
    return 0;
}

SearchController::SearchController()
  : post_function(NULL),
    terminate_function(NULL),
    age(1),
    talkLevel(Silent),
    stopped(false),
    ratingDiff(0),
    ratingFactor(0),
    active(false) {

#ifdef SMP_STATS
    sample_counter = SAMPLE_INTERVAL;
#endif
    LockInit(split_calc_lock);
    pool = new ThreadPool(this,options.search.ncpus);
    ThreadInfo *ti = pool->mainThread();
    ti->state = ThreadInfo::Working;
    rootSearch = (RootSearch*)ti->work;
    for (int d = 0; d < 64; d++) {
      for (int moves = 0; moves < 64; moves++) {
        LMR_REDUCTION[0][d][moves] =
           LMR_REDUCTION[1][d][moves] = 0;
        if (d > 2 && moves > 0) {
           // Formula similar to Protector & Toga. Last modified Aug. 2016.
           const double f = log((double)d) * log((double)moves+1);
           for (int i = 0; i < 2; i++) {
              const double reduction = LMR_BASE[i] + f/LMR_DIV[i];
              int r = static_cast<int>(DEPTH_INCREMENT*floor(2*reduction+0.5)/2);
              // do not reduce into the qsearch:
              r = std::min<int>(r,d*DEPTH_INCREMENT-1);
              // do not do reductions < 1 ply
              if (r < DEPTH_INCREMENT) r = 0;
              LMR_REDUCTION[i][d][moves] = r;
           }
        }
      }
    }
/*
    for (int i = 3; i < 64; i++) {
      cout << "--- i=" << i << endl;
      for (int m=0; m<64; m++) {
      cout << m << " " << 1.0*LMR_REDUCTION[0][i][m]/DEPTH_INCREMENT << ' ' << 1.0*LMR_REDUCTION[1][i][m]/DEPTH_INCREMENT << endl;
      }
    }
*/
    hashTable.initHash((size_t)(options.search.hash_table_size));
}

SearchController::~SearchController() {
   delete pool;
   hashTable.freeHash();
   LockFree(split_calc_lock);
}

void SearchController::terminateNow() {
    if (talkLevel == Trace)
        cout << "# terminating search (controller)" << endl;
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
    active = true;
    vector<Move> excludes, includes;
    Move result = findBestMove(board,srcType,time_limit,xtra_time,ply_limit,
                               background, isUCI, stat_buf, t, excludes, includes);
    active = false;
    return result;
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
   TalkLevel t,
   const vector<Move> &exclude,
   const vector<Move> &include)
{
    this->typeOfSearch = srcType;
    this->time_limit = time_target = time_limit;
    time_added = 0;
    this->xtra_time = xtra_time;
    if (srcType == FixedTime || srcType == TimeLimit) {
        ply_limit = Constants::MaxPly-1;
    }
    else {
        ply_limit = std::min<int>(ply_limit,Constants::MaxPly-1);
        ply_limit = std::max<int>(1,ply_limit);
    }
    this->ply_limit = ply_limit;
    this->background = background;
    this->uci = isUCI;
    this->talkLevel = t;
    this->stats = &stat_buf;

    // set initial thread split depth based on number of CPUS and material
    threadSplitDepth = 6*DEPTH_INCREMENT + (options.search.ncpus/8)*DEPTH_INCREMENT/2;
    int mat = board.getMaterial(board.sideToMove()).materialLevel();
    if (mat < 16) threadSplitDepth += DEPTH_INCREMENT/2;
    if (mat < 12) threadSplitDepth += DEPTH_INCREMENT;

    Time_Check_Interval = 4096/NODE_ACCUM_THRESHOLD;
    // reduce time check interval if time limit is very short (<1 sec)
    if (srcType == TimeLimit) {
       if (time_limit < 100) {
          Time_Check_Interval = 1024/NODE_ACCUM_THRESHOLD;
       } else if (time_limit < 1000) {
          Time_Check_Interval = 2048/NODE_ACCUM_THRESHOLD;
       }
    }
    computerSide = board.sideToMove();

#ifdef NUMA
    if (pool->rebindMask.test(0)) {
       // rebind main thread
       pool->bind(0);
       pool->rebindMask.reset(0);
    }
#endif

    // propagate controller variables to searches
    pool->forEachSearch<&Search::setVariablesFromController>();

    stats->clear();

    // Positions are stored in the hashtable with an "age" to identify
    // which search they came from. "Newer" positions can replace
    // "older" ones. Update the age here since we are starting a
    // new search.
    age = (age + 1) % 256;

    // reset terminate flag on all threads
    clearStopFlags();

    NodeStack rootStack;
    rootSearch->init(board,rootStack);
    startTime = last_time = getCurrentTime();
    return rootSearch->ply0_search(exclude,include);
}

void SearchController::setRatingDiff(int rdiff)
{
    ratingDiff = rdiff;
    ratingFactor = (Params::PAWN_VALUE*rdiff)/350;

    // propagate rating diff to searches
    pool->forEachSearch<&Search::setRatingVariablesFromController>();
}

int SearchController::wasTerminated() const {
   return rootSearch->wasTerminated();
}

void SearchController::setThreadCount(int threads) {
   pool->resize(threads,this);
}

int SearchController::getIterationDepth() const {
   return rootSearch->getIterationDepth();
}


void SearchController::setThreadSplitDepth(int depth) {
   threadSplitDepth = depth;
   pool->forEachSearch<&Search::setSplitDepthFromController>();
}

void SearchController::updateStats(NodeInfo *node, int iteration_depth,
                                   score_t score, score_t alpha, score_t beta)
{
    stats->elapsed_time = getElapsedTime(startTime,getCurrentTime());
    stats->multipv_count = rootSearch->multipv_count;
    ASSERT(stats->multipv_count >= 0 && (unsigned)stats->multipv_count < Statistics::MAX_PV);
    stats->value = score;
    stats->depth = iteration_depth;
    // if failing low, keep the current value for display purposes,
    // not the bottom of the window
    if (stats->value > alpha) {
       stats->display_value = stats->value;
    }
    if (talkLevel == Trace && stats->elapsed_time >= 5) {
       cout << "# elapsed time=" << stats->elapsed_time << " target=" << getTimeLimit() << endl;
    }
    //stats->state = state;
    Board board_copy(rootSearch->getInitialBoard());

    // note: retain previous best line if we do not have one here
    if (IsNull(node->pv[0])) {
#ifdef _TRACE
        cout << "# warning: pv is null\n";
#endif
        return;
    }
    else if (node->pv_length == 0) {
        return;
    }
    node->best = node->pv[0];                     // ensure "best" is non-null
    ASSERT(!IsNull(node->best));
    stats->best_line[0] = NullMove;
    int i = 0;
    stats->best_line_image.clear();
#ifdef _TRACE
    cout << "best line:" << endl;
#endif
    stringstream best_line_image;
    while (i < node->pv_length && !IsNull(node->pv[i])) {
        ASSERT(i<Constants::MaxPly);
        stats->best_line[i] = node->pv[i];
#ifdef _TRACE
        MoveImage(node->pv[i],cout); cout << ' ' << (flush);
#endif
        ASSERT(legalMove(board_copy,stats->best_line[i]));
        if (i!=0) best_line_image << ' ';
        Notation::image(board_copy,stats->best_line[i],
                        uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::SAN,best_line_image);
        int len = (int)best_line_image.tellg();
        // limit the length
        if (len > 250) {
            break;
        }
        board_copy.doMove(stats->best_line[i]);
        ++i;
        int rep_count;
        if (Scoring::isDraw(board_copy,rep_count,0)) {
            break;
        }
        if (node->pv_length < 2) {
            // get the next move from the hash table, if possible
            // (for pondering)
            HashEntry entry;
            HashEntry::ValueType result =
                hashTable.searchHash(board_copy,board_copy.hashCode(rep_count),
                           0,age,
                           iteration_depth,entry);
            if (result != HashEntry::NoHit) {
                Move hashMove = entry.bestMove(board_copy);
                if (!IsNull(hashMove)) {
                    stats->best_line[i] = hashMove;
                    if (i!=0) best_line_image << ' ';
                    Notation::image(board_copy,hashMove,
                                    uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::SAN,best_line_image);
                    ++i;
                }
                break;
            }
        }
    }
#ifdef _TRACE
    cout << endl;
#endif
    stats->best_line[i] = NullMove;
    stats->best_line_image = best_line_image.str();
#ifdef _TRACE
    cout << "best line image: " << stats->best_line_image << endl;
#endif
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
    pool->resize(options.search.ncpus,this);
    // update each search thread's local copy of the options:
    pool->forEachSearch<&Search::setSearchOptions>();
}

void SearchController::setTalkLevel(TalkLevel t) {
    pool->forEachSearch<&Search::setTalkLevelFromController>();
}

void SearchController::uciSendInfos(const Board &board, Move move, int move_index, int depth) {
   if (uci) {
      cout << "info depth " << depth;
      cout << " currmove ";
      Notation::image(board,move,Notation::OutputFormat::UCI,cout);
      cout << " currmovenumber " << move_index;
      cout << endl << (flush);
#ifdef UCI_LOG
      ucilog << "info depth " << depth;
      ucilog << " currmove ";
      Notation::image(board,move,Notation::OutputFormat::UCI,ucilog);
      ucilog << " currmovenumber " << move_index;
      ucilog << endl << (flush);
#endif
   }
}

void SearchController::resizeHash(size_t newSize) {
   hashTable.resizeHash(newSize);
}

Search::Search(SearchController *c, ThreadInfo *threadInfo)
   :controller(c),terminate(0),
    nodeCount(0ULL),
    nodeAccumulator(0),
    node(NULL),
    activeSplitPoints(0),
    split(NULL),
    ti(threadInfo),
    threadSplitDepth(0),
    computerSide(White),
    ratingDiff(0),
    ratingFactor(0),
    talkLevel(c->getTalkLevel()) {
    LockInit(splitLock);
    // Note: context was cleared in its constructor
    setSearchOptions();
}

Search::~Search() {
    LockFree(splitLock);
}

int Search::checkTime(const Board &board,int ply) {
    if (controller->stopped) {
        controller->terminateNow();
    }
    if (terminate) {
       if (talkLevel==Trace) cout << "# check time, already terminated" << endl;
       return 1; // already stopped search
    }

    Statistics *stats = controller->stats;
    CLOCK_TYPE current_time = getCurrentTime();
    stats->elapsed_time = getElapsedTime(controller->startTime,current_time);
    // dynamically change the thread split depth based on # of splits
    if (srcOpts.ncpus >1 && stats->elapsed_time > 100) {
        // Lock the stats structure since other threads may try to
        // modify it
        Lock(controller->split_calc_lock);
        uint64_t interval;
        if ((interval=getElapsedTime(stats->last_split_time,current_time)) > 50 &&
            stats->splits-stats->last_split_sample > 0) {
            int splitsPerSec = int((stats->splits-stats->last_split_sample*1000)/interval);
            int target = srcOpts.ncpus*120;
            if (splitsPerSec > 3*target/2) {
               controller->setThreadSplitDepth(
                  std::min<int>(MAX_SPLIT_DEPTH,
                             controller->threadSplitDepth + DEPTH_INCREMENT/2));
            } else if (splitsPerSec < target/2) {
               controller->setThreadSplitDepth(
                  std::max<int>(MIN_SPLIT_DEPTH,
                              controller->threadSplitDepth - DEPTH_INCREMENT/2));
            }
            stats->last_split_sample = stats->splits;
            stats->last_split_time = current_time;
        }
        Unlock(controller->split_calc_lock);
    }
    if (controller->typeOfSearch == FixedTime) {
       if (stats->elapsed_time >= controller->time_target) {
          return 1;
       }
    }
    else if (controller->typeOfSearch == TimeLimit) {
       if (controller->xtra_time > 0 &&
           controller->time_target != INFINITE_TIME &&
           stats->elapsed_time > controller->getTimeLimit()) {
          if (root()->fail_high_root) {
             // root move is failing high, extend time
             // until fail-high is resolved.
             controller->time_added = controller->xtra_time;
             if (talkLevel == Trace) {
                cout << "# adding time due to root fail high, new target=" << controller->getTimeLimit() << endl;
             }
             // Set flag that we extended time.
             root()->fail_high_root_extend = true;
          }
          else if (stats->faillow) {
             // root move is failing low, extend time until
             // fail-low is resolved
             controller->time_added = controller->xtra_time;
             root()->fail_low_root_extend = true;
             if (talkLevel == Trace) {
                cout << "# adding time due to root fail low, new target=" << controller->getTimeLimit() << endl;
             }
          }
       }
       // check time limit after any time extensions have been made
       if (stats->elapsed_time > controller->getTimeLimit()) {
          if (talkLevel == Trace) {
             cout << "# terminating, time up" << endl;
          }
          return 1;
       }
    }
    if (controller->uci && getElapsedTime(controller->last_time,current_time) >= 2000) {
        cout << "info";
        if (stats->elapsed_time>300) cout << " nps " <<
                (long)((1000L*stats->num_nodes)/stats->elapsed_time);
        cout << " nodes " << stats->num_nodes << " hashfull " << controller->hashTable.pctFull() << endl;
        controller->last_time = current_time;
    }
    return 0;
}

void Search::showStatus(const Board &board, Move best,int faillow,
int failhigh,int complete)
{
    if (terminate)
        return;
    Statistics *stats = controller->stats;
    stats->faillow = faillow;
    stats->failhigh = failhigh;
    int ply = stats->depth;
    stats->complete = complete;
    if (talkLevel == Debug) {
        // This is the output for the "test" command in verbose mode
        std::ios_base::fmtflags original_flags = cout.flags();
        cout.setf(ios::fixed);
        cout << setprecision(2);
        cout << ply << '\t';
        cout << stats->elapsed_time/1000.0 << '\t';
        if (faillow) {
            cout << " --";
        }
        else if (best != NullMove) {
            Notation::image(board, best, Notation::OutputFormat::SAN,cout);
            if (failhigh) cout << '!';
        }
        cout << '\t';
        Scoring::printScore(stats->display_value,cout);
        cout << '\t' << stats->num_nodes << endl;
        cout.flags(original_flags);
    }
    // Post during ponder if UCI
    if ((!controller->background || controller->uci)) {
        if (srcOpts.multipv > 1) {
            // Accumulate multiple pvs until we are ready to output
            // them.
            stats->multi_pvs[stats->multipv_count] = Statistics::MultiPVEntry(*stats);
            if (stats->multipv_count >= stats->multipv_limit) {
                stats->sortMultiPVs();
            }
        }
        if (controller->post_function) {
            controller->post_function(*stats);
        }
    }
}

score_t Search::drawScore(const Board & board) const {
    score_t score = 0;

    // if we know the opponent's rating (which will be the case if playing
    // on ICC), factor that into the draw score - a draw against a high-rated
    // opponent is good; a draw against a lower-rated one is bad.
    if (ratingDiff != 0) {
        if (board.sideToMove() == computerSide)
           score += ratingFactor;
        else
           score -= ratingFactor;
    }
    return score;
}

void Search::terminateSlaveSearches()
{
   Lock(splitLock);
   if (split) {
      split->failHigh++;
      for (int i = 0; i <activeSplitPoints; i++) {
         splitStack[i].failHigh++;
      }
   }
   Unlock(splitLock);
}

void RootSearch::init(const Board &board, NodeStack &stack) {
  this->board = initialBoard = board;
#ifdef SINGULAR_EXTENSION
  for (int i = 0; i < Constants::MaxPly; i++) {
     stack[i].singularMove = NullMove;
  }
#endif
  // clean the killer table (but not other tables)
  context.clearKiller();
  node = stack;
  nodeAccumulator = 0;
  // local copy:
  threadSplitDepth = controller->threadSplitDepth;
}

#ifdef SYZYGY_TBS
score_t Search::tbScoreAdjust(const Board &board,
                    score_t value,int tb_hit,Options::TbType tablebase_type,score_t tb_score) const 
{
   if (tb_hit && tablebase_type == Options::TbType::SyzygyTb &&
       !Scoring::mateScore(value)) {
      // If a Syzygy tablebase hit set the score based on that. But
      // don't override a mate score found with search.
      if (tb_score == Constants::TABLEBASE_WIN) {
         return tb_score;
      }
      else if (tb_score >= 0) {
         return drawScore(board);
      }
      else {
         // loss or cursed draw position
         return -Constants::TABLEBASE_WIN;
      }
   }
   else {
      return value;
   }
}
#endif

score_t Search::futilityMargin(int depth) const
{
    int d = std::max(depth,int(1.5*DEPTH_INCREMENT));
    return FUTILITY_MARGIN_BASE + d*FUTILITY_MARGIN_SLOPE/DEPTH_INCREMENT + d*d*FUTILITY_MARGIN_SLOPE2/(DEPTH_INCREMENT*DEPTH_INCREMENT);
}

score_t Search::razorMargin(int depth) const
{
    return(depth<=DEPTH_INCREMENT) ?
        RAZOR_MARGIN1 : RAZOR_MARGIN2 + (Params::PAWN_VALUE*depth)/(RAZOR_MARGIN_DEPTH_FACTOR*DEPTH_INCREMENT);
}

Move RootSearch::ply0_search(const vector<Move> &exclude,
                             const vector<Move> &include)
{
   easy_adjust = false;
   fail_high_root_extend = fail_low_root_extend = false;
   last_score = -Constants::MATE;
   node->best = NullMove;
   if (scoring.isLegalDraw(board) && !controller->uci &&
       !(controller->typeOfSearch == FixedTime && controller->time_target == INFINITE_TIME)) {
      // If it's a legal draw situation before we even move, then
      // just return a draw score and don't search. (But don't do
      // this in analysis mode: return a move if possible. Also do
      // a search in all cases for UCI, since the engine cannot
      // claim draw and some interfaces may expect a move.)
      if (talkLevel == Trace) {
          cout << "# skipping search, draw" << endl;
      }
      controller->stats->state = Draw;
      controller->stats->value = drawScore(board);
      return NullMove;
   }
   // Generate the ply 0 moves here:
   RootMoveGenerator mg(board,&context,NullMove,
      talkLevel == Trace);
   if (controller->uci) {
       controller->stats->multipv_limit = std::min<int>(mg.moveCount(),srcOpts.multipv);
   }
   controller->time_check_counter = Time_Check_Interval;

   score_t value = Scoring::INVALID_SCORE;
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS) || defined(SYZYGY_TBS)
   int tb_hit = 0, tb_pieces = 0;
   options.search.tb_probe_in_search = 1;
   controller->updateSearchOptions();
   score_t tb_score = Scoring::INVALID_SCORE;
   if (srcOpts.use_tablebases) {
      const Material &wMat = board.getMaterial(White);
      const Material &bMat = board.getMaterial(Black);
      tb_pieces = wMat.men() + bMat.men();
      if(tb_pieces <= EGTBMenCount) {
         controller->stats->tb_probes++;
#ifdef NALIMOV_TBS
         if (srcOpts.tablebase_type == Options::TbType::NalimovTb) {
             tb_hit = NalimovTb::probe_tb(board, tb_score, 0);
             value = tb_score;
         }
#endif
#ifdef GAVIOTA_TBS
         if (srcOpts.tablebase_type == Options::TbType::GaviotaTb) {
             tb_hit = GaviotaTb::probe_tb(board, tb_score, 0, true);
             value = tb_score;
         }
#endif
#ifdef SYZYGY_TBS
         if (srcOpts.tablebase_type == Options::TbType::SyzygyTb) {
            set<Move> moves;
            // If include set is non-empty, ensure all "include" moves
            // will be in the set to search, even if some are losing moves.
            std::copy(include.begin(), include.end(), std::inserter(moves, moves.end()));
            tb_hit = SyzygyTb::probe_root(board, tb_score, moves);
            if (tb_hit) {
               // restrict the search to moves that preserve the
               // win or draw, if there is one.
               mg.filter(moves);
               if (mg.moveCount() == 0) {
                   // should not happen
                   if (talkLevel == Trace) {
                       cout << "# warning: no moves after Syzygy move filtering" << endl;
                   }
                   tb_hit = 0;
               } else {
                   // Note: do not set the value - search values are based
                   // on DTM not DTZ.
                   controller->stats->tb_value = tb_score;
                   // do not probe in the search
                   options.search.tb_probe_in_search = 0;
                   controller->updateSearchOptions();
               }
            }
         }
#endif
         if (tb_hit) {
            controller->stats->tb_hits++;
            if (talkLevel == Trace) {
               cout << "# tb hit, score=";
               Scoring::printScore(value,cout);
               cout << endl;
            }
         }
      }
   }
#endif
   easyMove = NullMove;
   if (value == Scoring::INVALID_SCORE) {
      value = 0;
   }
   waitTime = 0;
   depth_adjust = 0;
   // Reduce strength but not in analysis mode:
   if (srcOpts.strength < 100 && (controller->typeOfSearch ==
                                  FixedDepth ||
                                  controller->time_target != INFINITE_TIME)) {
       int mgCount = mg.moveCount();
       if (mgCount) {
           const double factor = 1.0/controller->ply_limit + (100-srcOpts.strength)/250.0;
           const int max = int(0.3F*controller->time_target/mgCount);
           // wait time is in milliseconds
           waitTime = int((max*factor));
           if (talkLevel == Trace) {
               cout << "# waitTime=" << waitTime << endl;
           }
           // adjust time check interval since we are lowering nps
           Time_Check_Interval = std::max<int>(1,Time_Check_Interval / (1+8*int(factor)));
           if (srcOpts.strength <= 95) {
               const double limit = pow(2.1,srcOpts.strength/25.0)-0.25;
               double int_limit;
               double frac_limit = modf(limit,&int_limit);
               int ply_limit = std::max(1,int(int_limit));
               if (board.getMaterial(White).materialLevel() +
                   board.getMaterial(Black).materialLevel() < 16 &&
                   srcOpts.strength > 10) {
                   // increase ply limit in endgames
                   ply_limit += std::min<int>(2,1+ply_limit/8);
               }
               controller->ply_limit = std::min<int>(ply_limit,
                                                 controller->ply_limit);
               if (limit > 1.0) {
                  depth_adjust = (int)std::round(DEPTH_INCREMENT*frac_limit);
               }
               if (talkLevel == Trace) {
                   cout << "# ply limit =" << controller->ply_limit << endl;
                   cout << "# depth adjust =" << depth_adjust << endl;
               }
           }
       }
   }

   value = controller->stats->value = controller->stats->display_value = value;

   // Incrementally search the board to greater depths - stop when
   // ply limit, time limit, interrupt, or a terminating condition
   // is reached.
   // Search the first few iterations with a wide window - for easy
   // move detection.
   node->best = node->pv[0] = NullMove;
   int depth_at_pv_change = 0;
   controller->failLowFactor = 0;
   for (iteration_depth = 1;
        iteration_depth <= controller->ply_limit && !terminate;
        iteration_depth++) {
      vector<Move> excluded(exclude);
      controller->stats->multipv_count = 0;
      for (multipv_count=0; multipv_count < srcOpts.multipv && !terminate; multipv_count++) {
         score_t lo_window, hi_window;
         score_t aspirationWindow = ASPIRATION_WINDOW[0];
         if (srcOpts.multipv > 1) controller->stats->clearPV();
         if (multipv_count) {
             excluded.push_back(controller->stats->multi_pvs[multipv_count-1].best);
             if (iteration_depth > 1) {
                 // set value to previous iteration's value
                 value = controller->stats->multi_pvs[multipv_count].score;
             }
         }
         if (iteration_depth <= 1) {
            lo_window = -Constants::MATE;
            hi_window = Constants::MATE;
         } else if (iteration_depth <= MoveGenerator::EASY_PLIES) {
            lo_window = std::max<score_t>(-Constants::MATE,value - options.search.easy_threshold);
            hi_window = std::min<score_t>(Constants::MATE,value + options.search.easy_threshold + aspirationWindow/2);
         } else {
            lo_window = std::max<score_t>(-Constants::MATE,value - aspirationWindow/2);
            hi_window = std::min<score_t>(Constants::MATE,value + aspirationWindow/2);
         }
         if (talkLevel == Trace && controller->background) {
            cout << "# " << iteration_depth << ". move=";
            MoveImage(node->best,cout); cout << " score=" << node->best_score
                                             << " terminate=" << terminate << endl;
         }
         bool failLow = true;
         bool failHigh = true;
         int fails = 0;
         int faillows = 0;
         controller->stats->faillow = 0;
         controller->stats->failhigh = 0;
         while (!terminate && (failLow || failHigh)) {
            failHigh = failLow = false;
#ifdef _TRACE
            cout << "iteration " << iteration_depth << " window = [" <<
               lo_window << "," << hi_window << "]" << endl;
#endif
            value = ply0_search(mg, lo_window, hi_window, iteration_depth,
                                DEPTH_INCREMENT*iteration_depth + depth_adjust,
                                excluded,include);
#ifdef _TRACE
            cout << "iteration " << iteration_depth << " result: " <<
               value << endl;
#endif
            controller->updateStats(node,iteration_depth,
                                    value,lo_window,hi_window);
#ifdef SYZYGY_TBS
            // Correct if necessary the display value, used for score
            // output and resign decisions, based on the tb information:
            controller->stats->display_value = tbScoreAdjust(board,
                                                      controller->stats->display_value,
                                                      tb_hit,
                                                      options.search.tablebase_type,
                                                      tb_score);
#endif
            // check for forced move, but only at depth 2
            // (so we get a ponder move if possible). But exit immediately
            // if a tb hit because deeper search will hit the q-search and
            // the score will be inaccurate. Do not terminate here if a
            // resign score is returned (search deeper to get an accurate
            // score). Do not exit in analysis mode.
            if (!(controller->background || (controller->typeOfSearch == FixedTime && controller->time_target == INFINITE_TIME)) &&
                mg.moveCount() == 1 &&
                (iteration_depth >= 2) &&
                (!srcOpts.can_resign ||
                 (controller->stats->display_value >
                  srcOpts.resign_threshold))) {
               if (talkLevel == Trace)
                  cout << "# single legal move, terminating" << endl;
               controller->terminateNow();
            }
            Statistics *stats = controller->stats;
            StateType &state = stats->state;
            if (!terminate && (state == Checkmate || state == Stalemate)) {
               if (talkLevel == Trace)
                  cout << "# terminating due to checkmate or statemate, state="
                       << (int)state << endl;
               controller->terminateNow();
               break;
            }
            if (stats->elapsed_time > 200) {
               Time_Check_Interval = int((20L*stats->num_nodes)/(stats->elapsed_time*NODE_ACCUM_THRESHOLD));
               if ((int)controller->time_limit - (int)stats->elapsed_time < 100) {
                  Time_Check_Interval /= 2;
               }
               if (talkLevel == Trace) {
                  cout << "# time check interval=" << Time_Check_Interval << " elapsed_time=" << stats->elapsed_time << " target=" << controller->getTimeLimit() << endl;
               }
            }
            if (terminate) {
               break;
            }
            else {
               if (checkTime(board,0)) {
                  if (talkLevel == Trace)
                     cout << "# time up" << endl;
                  controller->terminateNow();
               }
               else if (controller->terminate_function &&
                        controller->terminate_function(*stats)) {
                  if (talkLevel == Trace)
                     cout << "# terminating due to program or user input" << endl;
                  controller->terminateNow();
               }
            }
            failHigh = value >= hi_window && (hi_window < Constants::MATE-iteration_depth-1);
            failLow = value <= lo_window  && (lo_window > iteration_depth-Constants::MATE);
            if (failHigh) {
               showStatus(board, node->best, failLow, failHigh, 0);
#ifdef _TRACE
               cout << "# ply 0 high cutoff, re-searching ... value=";
               Scoring::printScore(value,cout);
               cout << " fails=" << fails+1 << endl;
#endif
               if (fails+1 >= ASPIRATION_WINDOW_STEPS) {
                  if (talkLevel == Trace) {
                     cout << "# warning, too many aspiration window steps" << endl;
                  }
                  aspirationWindow = Constants::MATE;
               }
               else {
                  aspirationWindow = ASPIRATION_WINDOW[++fails];
               }
               if (aspirationWindow == Constants::MATE) {
                  hi_window = Constants::MATE-iteration_depth-1;
               } else {
                  if (iteration_depth <= MoveGenerator::EASY_PLIES) {
                     aspirationWindow += 2*options.search.easy_threshold;
                  }
                  hi_window = std::min<score_t>(Constants::MATE-iteration_depth-1,
                                        lo_window + aspirationWindow);
               }
            }
            else if (failLow) {
               showStatus(board, node->best, failLow, failHigh, 0);
               if (talkLevel == Trace) {
                   cout << "# ply 0 fail low, re-searching ... value=";
                   Scoring::printScore(value,cout);
                   cout << " fails=" << fails+1 << endl;
               }
#ifdef _TRACE
               cout << "# ply 0 fail low, re-searching ... value=";
               Scoring::printScore(value,cout);
               cout << " fails=" << fails+1 << endl;
#endif
               faillows++;
               // continue loop with lower bound
               if (fails+1 >= ASPIRATION_WINDOW_STEPS) {
                  // TBD: Sometimes we can fail low after a bunch of fail highs. Allow the
                  // search to continue, but set the lower bound to the bottom of the range.
                  if (talkLevel == Trace) {
                     cout << "# warning, too many aspiration window steps" << endl;
                  }
                  aspirationWindow = Constants::MATE;
               }
               else {
                  aspirationWindow = ASPIRATION_WINDOW[++fails];
               }
               if (aspirationWindow == Constants::MATE) {
                  lo_window = iteration_depth-Constants::MATE-1;
               } else {
                  if (iteration_depth <= MoveGenerator::EASY_PLIES) {
                     aspirationWindow += 2*options.search.easy_threshold;
                  }
                  lo_window = std::max<score_t>(iteration_depth-Constants::MATE,hi_window - aspirationWindow);
               }
            }
         }
         if (faillows) {
            controller->failLowFactor += (1<<faillows)*iteration_depth/2;
         }
         // search value should now be in bounds (unless we are terminating)
         if (!terminate) {
            showStatus(board, node->best, 0, 0, 0);
            if (fail_low_root_extend) {
               // We extended time to get the fail-low resolved. Now
               // we have a score.
               fail_low_root_extend = false;
               // Continue searching based on how bad the fail-low was
               controller->time_added = controller->xtra_time*std::min<int>(100,controller->failLowFactor)/100;
               if (talkLevel == Trace) {
                    cout << "# fail low resolved, new time target = " <<
                        controller->getTimeLimit() << endl;
               }
            }
            else if (fail_high_root_extend) {
               // We extended the time to get the fail high resolved,
               // but it is resolved now, so reduce time limit again.
               controller->time_added = 0;
               fail_high_root_extend = false;
               if (talkLevel == Trace) {
                  cout << "# resetting time_added - fail high is resolved" << endl;
               }
            }
         }

         if (!MovesEqual(node->best,easyMove)) {
            depth_at_pv_change = iteration_depth;
         }
#ifdef _TRACE
         cout << iteration_depth << " ply search result: ";
         MoveImage(node->best,cout);
         cout << " value = ";
         Scoring::printScore(value,cout);
         cout << endl;
#endif
         last_score = value;
         if (srcOpts.multipv==1 && !(controller->uci && controller->time_limit == INFINITE_TIME)) {
            // Allow early termination on a tablebase position, but not
            // if we have >=6 man tbs in use (because tb set may be
            // incomplete - in that case it is better to allow us to
            // search deeper those nodes that don't produce a tb hit).
            // Also do not terminate if using Syzygy tbs and move list is
            // >1. We want in that case to keep searching to find the
            // best move (according to the engine) among the available
            // tb moves.
            //
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS) || defined(SYZYGY_TBS)
            if (tb_hit && tb_pieces<6 && iteration_depth>=3 && !IsNull(node->pv[0]) && !(options.search.tablebase_type == Options::TbType::SyzygyTb && mg.moveCount()>1)) {
               if (talkLevel == Trace)
                  cout << "# terminating, tablebase hit" << endl;
#ifdef _TRACE
               cout << "terminating, tablebase hit" << endl;
#endif
               controller->terminateNow();
               break;
            }
            else
#endif
            if (!controller->background &&
                     !controller->time_added &&
                     !easy_adjust &&
                     depth_at_pv_change <= MoveGenerator::EASY_PLIES &&
                     MovesEqual(easyMove,node->best) &&
                     !faillows &&
                     (controller->stats->elapsed_time >
                      (unsigned)controller->time_target/3)) {
               easy_adjust = true;
               if (talkLevel == Trace) {
                  cout << "# easy move, adjusting time lower" << endl;
               }
               controller->time_target /= 3;
            }
            if (value <= iteration_depth - Constants::MATE) {
               // We're either checkmated or we certainly will be, so
               // quit searching.
               if (talkLevel == Trace)
                  cout << "# terminating, low score" << endl;
#ifdef _TRACE
               cout << "terminating, low score" << endl;
#endif
               controller->terminateNow();
               break;
            }
            else if (value >= Constants::MATE - iteration_depth - 1) {
               // found a forced mate, terminate
               if (iteration_depth>=2) {
                  if (talkLevel == Trace)
                     cout << "# terminating, mate score" << endl;
#ifdef _TRACE
                  cout << "terminating, mate score" << endl;
#endif
                  controller->terminateNow();
                  break;
               }
            }
         }
      }
   }

#ifdef UCI_LOG
   ucilog << "out of search loop " << endl << (flush);
#endif
   // search done, set status and report statistics
   static const int end_of_game[] = {0, 1, 0, 1, 1, 1, 1};
   Statistics *stats = controller->stats;
   StateType &state = stats->state;
   stats->end_of_game = end_of_game[(int)stats->state];
   if (!controller->uci && !stats->end_of_game && srcOpts.can_resign) {
      if (stats->display_value != Scoring::INVALID_SCORE &&
         (100*stats->display_value)/Params::PAWN_VALUE <= srcOpts.resign_threshold) {
         state = Resigns;
         stats->end_of_game = end_of_game[(int)state];
      }
   }
   if (srcOpts.strength < 100) {
       Move m = node->best;
       score_t val = node->best_score;
       suboptimal(mg,m,val);
       if (!MovesEqual(node->best,m)) {
           node->best = m;
           stats->display_value = stats->value = val;
           stats->best_line[0] = m;
           stats->best_line[1] = NullMove;
           Notation::image(board,m,
                           controller->uci ? Notation::OutputFormat::UCI : Notation::OutputFormat::SAN,stats->best_line_image);
       }
   }

   if (talkLevel == Debug) {
      std::ios_base::fmtflags original_flags = cout.flags();
      cout.setf(ios::fixed);
      cout << setprecision(2);
      if (stats->elapsed_time > 0) {
         stats->printNPS(cout);
         cout << " nodes/second." << endl;
      }
#ifdef SEARCH_STATS
      cout << (stats->num_nodes-stats->num_qnodes) << " regular nodes, " <<
         stats->num_qnodes << " quiescence nodes." << endl;
      cout << stats->hash_searches << " searches of hash table, " <<
         stats->hash_hits << " successful";
      if (stats->hash_searches != 0)
         cout << " (" <<
            (int)((100.0*(float)stats->hash_hits)/((float)stats->hash_searches)) <<
            " percent).";
      cout << endl;
      cout << "hash table is " << setprecision(2) <<
          1.0F*controller->hashTable.pctFull()/10.0F << "% full." << endl;
#endif
#ifdef MOVE_ORDER_STATS
      cout << "move ordering: ";
      static const char *labels[] = {"1st","2nd","3rd","4th"};
      for (int i = 0; i < 4; i++) {
         cout << setprecision(2) << labels[i] << " " <<
            (100.0*stats->move_order[i])/(float)stats->move_order_count << "% " ;
      }
      cout << endl;
#endif
#ifdef SEARCH_STATS
      cout << "pre-search pruning: " << endl;
      cout << ' ' << setprecision(2) << 100.0*stats->razored/stats->reg_nodes << "% razoring" << endl;
      cout << ' ' << setprecision(2) << 100.0*stats->static_null_pruning/stats->reg_nodes << "% static null pruning" << endl;
      cout << ' ' << setprecision(2) << 100.0*stats->null_cuts/stats->reg_nodes << "% null cuts" << endl;
      cout << "search pruning: " << endl;
      cout << ' ' << setprecision(2) << 100.0*stats->futility_pruning/stats->moves_searched << "% futility" << endl;
      cout << ' ' << setprecision(2) << 100.0*stats->history_pruning/stats->moves_searched << "% history" << endl;
      cout << ' ' << setprecision(2) << 100.0*stats->lmp/stats->moves_searched << "% lmp" << endl;
      cout << ' ' << setprecision(2) << 100.0*stats->see_pruning/stats->moves_searched << "% SEE" << endl;
      cout << ' ' << setprecision(2) << 100.0*stats->reduced/stats->moves_searched << "% reduced" << endl;
      cout << "extensions: " <<
         100.0*stats->check_extensions/stats->moves_searched << "% check, " <<
         100.0*stats->evasion_extensions/stats->moves_searched << "% evasions, " <<
         100.0*stats->capture_extensions/stats->moves_searched << "% capture, " <<
         100.0*stats->pawn_extensions/stats->moves_searched << "% pawn, " <<
         100.0*stats->singular_extensions/stats->moves_searched << "% singular" << endl;

#endif
      cout << stats->tb_probes << " tablebase probes, " <<
         stats->tb_hits << " tablebase hits" << endl;
#if defined(SMP_STATS)
      cout << stats->splits << " splits," <<
         " average thread usage=" << (float)(stats->threads)/(float)stats->samples << endl;
#endif
      cout << (flush);
      cout.flags(original_flags);
   }
#ifdef UCI_LOG
   ucilog << "out of search" << endl << (flush);
#endif
   if (talkLevel == Trace) {
      cout << "# exiting root search, move = ";
      MoveImage(node->best,cout);
      cout << endl;
   }
   return node->best;
}

score_t RootSearch::ply0_search(RootMoveGenerator &mg, score_t alpha, score_t beta,
                        int iteration_depth,
                        int depth,
                        const vector<Move> &exclude,
                        const vector<Move> &include)
{
    // implements alpha/beta search for the top most ply.  We use
    // the negascout algorithm.

    --controller->time_check_counter;
    nodeAccumulator++;

#ifdef _TRACE
    int in_pv = 1;
#endif
    int in_check = 0;

    const bool wide = iteration_depth <= MoveGenerator::EASY_PLIES;

    in_check = (board.checkStatus() == InCheck);
    BoardState save_state = board.state;

    score_t try_score = alpha;
    //
    // Re-sort the ply 0 moves and re-init move generator.
    mg.reorder(node->best,controller->getIterationDepth());
    // if in N-variation mode, exclude any moves we have searched already
    mg.exclude(exclude);

    if (controller->getIterationDepth() == MoveGenerator::EASY_PLIES+1) {
        auto list = mg.getMoveList();
        // Note: do not do "easy move" if capturing the last piece in
        // the endgame .. this can be tricky as the resulting pawn
        // endgame may be lost.
        if (list.size() > 1 && (list[0].score >= list[1].score + options.search.easy_threshold) && TypeOfMove(node->best) == Normal &&
            Capture(node->best) != Empty && Capture(node->best) != Pawn &&
            board.getMaterial(board.oppositeSide()).pieceCount() == 1 &&
            board.getMaterial(board.sideToMove()).pieceCount() <= 1) {
            easyMove = node->best;
            if (talkLevel == Trace) {
                cout << "#easy move: ";
                MoveImage(easyMove,cout);
                cout << endl;
            }
        }
    }

    //
    // Search the next ply
    //
    node->pv[0] = NullMove;
    node->pv_length = 0;
    node->cutoff = 0;
    node->extensions = 0;
    node->num_try = 0;                            // # of legal moves tried
    node->alpha = alpha;
    node->beta = beta;
    node->best_score = node->alpha;
#ifdef MOVE_ORDER_STATS
    node->best_count = 0;
#endif
    node->ply = 0;
    node->depth = depth;
    node->eval = Scoring::INVALID_SCORE;
    // clear split point stack:
    activeSplitPoints = 0;

    int move_index = 0;
    score_t hibound = beta;
    fail_high_root = 0;
    while (!node->cutoff && !terminate) {
        Move move;
        if ((move = mg.nextMove(split,move_index))==NullMove) break;
        if (IsUsed(move) || IsExcluded(move) ||
            (!include.empty() && include.end() == std::find_if(include.begin(),
             include.end(),[&move](const Move &m) {return MovesEqual(m,move);}))) {
            continue;     // skip move
        }
        node->last_move = move;
        controller->stats->mvleft = controller->stats->mvtot-move_index;
        if (controller->uci && controller->stats->elapsed_time > 300) {
            controller->uciSendInfos(board, move, move_index, controller->getIterationDepth());
        }
#ifdef _TRACE
        cout << "trying 0. ";
        MoveImage(move,cout);
        cout << " (" << move_index << "/" << mg.moveCount();
        cout << ")" << endl;
#endif
        node->last_move = move;
        node->extensions = 0;
        CheckStatusType in_check_after_move = board.wouldCheck(move);
        int extend = calcExtensions(board,node,node,in_check_after_move,
                                    move_index,
                                    move);
        if (extend == PRUNE) {
#ifdef _TRACE
            cout << "fwd pruned." << endl;
#endif
            continue;
        }
        board.doMove(move);
        setCheckStatus(board,in_check_after_move);
        node->done[node->num_try++] = move;
        score_t lobound = wide ? node->alpha : node->best_score;
#ifdef _TRACE
        cout << "window [" << -hibound << ", " << -lobound <<
          "]" << endl;
#endif
        if (depth+extend-DEPTH_INCREMENT > 0) {
           try_score = -search(-hibound, -lobound,
                               1, depth+extend-DEPTH_INCREMENT);
        }
        else {
           try_score = -quiesce(-hibound, -lobound, 1, 0);
        }
        if (terminate) {
            board.undoMove(move,save_state);
            break;
        }
#ifdef _TRACE
        cout << "0. ";
        MoveImage(move,cout);
        cout << ' ' << try_score;
        if (in_pv) cout << " (pv)";
        cout << endl;
#endif
        while (try_score > node->best_score &&
               (extend < 0 || hibound < node->beta) &&
               !((node+1)->flags & EXACT) &&
               !terminate) {
           // We failed to get a cutoff and must re-search
           // Set flag if we may be getting a new best move:
           fail_high_root++;
#ifdef _TRACE
           cout << "window = [" << -hibound << "," << node->best_score
                << "]" << endl;
           cout << "score = " << try_score << " - no cutoff, researching .." << endl;
#endif
           if (extend >= -DEPTH_INCREMENT) {
              hibound = node->beta;
           }
           if (extend < 0) {
              extend = node->extensions = 0;
           }
           if (depth+extend-DEPTH_INCREMENT > 0)
              try_score=-search(-hibound,-lobound,1,depth+extend-DEPTH_INCREMENT);
           else
              try_score=-quiesce(-hibound,-lobound,1,0);
#ifdef _TRACE
           cout << "0. ";
           MoveImage(move,cout);
           cout << ' ' << try_score;
           cout << endl;
#endif
        }
        board.undoMove(move,save_state);
        if (wide) {
           mg.setScore(move,try_score);
        }
	if (split) split->lock();
        if (try_score > node->best_score && !terminate) {
           if (updateRootMove(board,node,node,move,try_score,move_index)) {
              // beta cutoff
              // ensure we send UCI output .. even in case of quick
              // termination due to checkmate or whatever
              controller->uciSendInfos(board, move, move_index, controller->getIterationDepth());
              // don't reset this until after the PV update, in case
              // it causes us to terminate:
              fail_high_root = 0;
              if (split) split->unlock();
              break;
           }
        }
        if (split) split->unlock();
        fail_high_root = 0;
        if (waitTime) {
            // we are in reduced strength mode, waste some time
            sleep(waitTime);
        }
        if (!wide) {
           hibound = node->best_score + 1;  // zero-width window
        }
#ifdef _TRACE
        in_pv = 0;
#endif
        if (srcOpts.ncpus>1 && depth >= threadSplitDepth &&
            maybeSplit(board, node, &mg, 0, depth)) {
            // remaining moves are searched by searchSMP
            break;
        }
    }

    if (node->cutoff) {
        return node->best_score;
    }
    if (node->num_try == 0) {
        // no moves were tried
        if (in_check) {
            if (mg.moveCount() == 0) {           // mate
                node->best_score = -(Constants::MATE);
                controller->stats->state = Checkmate;
            }
        }
        else {                                    // stalemate
            controller->stats->state = Stalemate;
#ifdef _TRACE
            cout << "stalemate!" << endl;
#endif
            node->best_score = drawScore(board);
        }
    }
    else if (!IsNull(node->best) && !CaptureOrPromotion(node->best) &&
             board.checkStatus() != InCheck) {
        context.setKiller((const Move)node->best, node->ply);
        context.updateStats(board, node, node->best, 0,
           board.sideToMove());
    }
#ifdef MOVE_ORDER_STATS
    if (node->num_try && node->best_score > node->alpha) {
        controller->stats->move_order_count++;
        if (node->best_count<4) {
            controller->stats->move_order[node->best_count]++;
        }
    }
#endif
    ASSERT(node->best_score >= -Constants::MATE && node->best_score <= Constants::MATE);
    controller->stats->num_nodes += nodeAccumulator;
    nodeAccumulator = 0;
    return node->best_score;
}

void RootSearch::suboptimal(RootMoveGenerator &mg,Move &m, score_t &val) {
    if (mg.moveCount() < 2) {
        return;
    }
    mg.reorderByScore();
    unsigned threshold_base = unsigned(750.0/(1.0 + 0.25*pow(srcOpts.strength/10.0,2.0)));
    const unsigned r = random(1024);
    // In reduced strength mode sometimes, deliberately choose a move
    // that is not the best
    int ord;
    board = initialBoard;
    BoardState state(board.state);
    score_t first_val = val;
    for (int i = 0; i <= 4; i++) {
        Move move = (board.checkStatus() == InCheck ? mg.nextEvasion(ord) :
                  mg.nextMove(ord));
        if (IsNull(move)) break;
        // For this search do not reduce strength
        int tmp = srcOpts.strength;
        srcOpts.strength = 100;
        node->last_move = move;
        board.doMove(move);
        int n = std::max(3,controller->ply_limit-2);
        score_t score = -search(-Constants::MATE,Constants::MATE,
                                1,n*DEPTH_INCREMENT);
        srcOpts.strength = tmp;
        board.undoMove(move,state);
        if (i == 0) {
           first_val = score;
        } else {
           unsigned threshold;
           if (score > val || val-score > 10*Params::PAWN_VALUE) {
              threshold = 0;
           }
           else {
              double diff = exp((val-score)/(3.0*Params::PAWN_VALUE))-1.0;
              threshold = unsigned(threshold_base/(2*diff+i));
           }
           if (r < threshold) {
              if (controller->talkLevel == Trace) {
                 cout << "# suboptimal: index= " << i <<
                    " score=" << score << " val=" << first_val <<
                    " threshold=" << threshold <<
                    " r=" << r << endl;
              }
              m = move;
              val = score;
           }
        }
    }
}

score_t Search::quiesce(int ply,int depth)
{
   // recursive function, implements quiescence search.
   //
   ASSERT(ply < Constants::MaxPly);
   if (++nodeAccumulator > NODE_ACCUM_THRESHOLD) {
      controller->stats->num_nodes += nodeAccumulator;
      nodeAccumulator = 0;
#ifdef SMP_STATS
      --controller->sample_counter;
#endif
      if (--controller->time_check_counter <= 0) {
         controller->time_check_counter = Time_Check_Interval;
         if (checkTime(board,ply)) {
            if (talkLevel == Trace) {
               cout << "# terminating, time up" << endl;
            }
            controller->terminateNow();   // signal all searches to end
         }
      }
   }
   ASSERT(depth<=0);
#ifdef SEARCH_STATS
   controller->stats->num_qnodes++;
#endif
   int rep_count;
   if (terminate) return node->alpha;
   else if (ply >= Constants::MaxPly-1) {
      if (!board.wasLegal((node-1)->last_move)) {
         return -Illegal;
      }
      node->flags |= EXACT;
      return scoring.evalu8(board);
   }
   else if (Scoring::isDraw(board,rep_count,ply)) {
	  // Verify previous move was legal
      if (!board.wasLegal((node-1)->last_move)) {
         return -Illegal;
      }
#ifdef _TRACE
      if (master()) {
         indent(ply); cout << "draw!" << endl;
      }
#endif
      node->flags |= EXACT;
      return drawScore(board);
   }
#ifdef _TRACE
   if (master()) {
      indent(ply); cout << "window [" << node->alpha << ","
                        << node->beta << "]" << endl;
   }
#endif
   node->eval = node->staticEval = Scoring::INVALID_SCORE;
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
   score_t hashValue;
   HashEntry::ValueType result = HashEntry::NoHit;
   HashEntry hashEntry;
   // Note: we copy the hash entry .. so mods by another thread do not
   // alter the copy
   result = controller->hashTable.searchHash(board,hash,
                                             ply,tt_depth,controller->age,hashEntry);
#ifdef SEARCH_STATS
   controller->stats->hash_searches++;
#endif
   bool hashHit = (result != HashEntry::NoHit);
   if (hashHit) {
      // a valid hashtable entry was found
#ifdef SEARCH_STATS
      controller->stats->hash_hits++;
#endif
      node->staticEval = hashEntry.staticValue();
      hashValue = hashEntry.getValue();
      // If this is a mate score, adjust it to reflect the
      // current ply depth.
      if (hashValue >= Constants::TABLEBASE_WIN) {
         hashValue -= ply;
      }
      else if (hashValue <= -Constants::TABLEBASE_WIN) {
         hashValue += ply;
      }
      switch (result) {
      case HashEntry::Valid:
#ifdef _TRACE
         if (master()) {
            indent(ply);
            cout << "hash cutoff, type = E" <<
               " alpha = " << node->alpha <<
               " beta = " << node->beta <<
               " value = " << hashValue << endl;
         }
#endif
         if (node->inBounds(hashValue)) {
            // parent node will consider this a new best line
            hashMove = hashEntry.bestMove(board);
            if (!IsNull(hashMove)) {
               node->pv[ply] = hashMove;
               node->pv_length = 1;
            }
#ifdef _TRACE
            if (master()) {
               indent(ply); cout << "best line[ply][ply] = ";
               MoveImage(hashMove,cout);
               cout << endl;
            }
#endif
         }
         return hashValue;
      case HashEntry::UpperBound:
         if (hashValue <= node->alpha) {
#ifdef _TRACE
            if (master()) {
               indent(ply);
               cout << "hash cutoff, type = U" <<
                  " alpha = " << node->alpha <<
                  " beta = " << node->beta <<
                  " value = " << hashValue << endl;
            }
#endif
            return hashValue;                     // cutoff
         }
         break;
      case HashEntry::LowerBound:
         if (hashValue >= node->beta) {
#ifdef _TRACE
            if (master()) {
               indent(ply);
               cout << "hash cutoff, type = L" <<
                  " alpha = " << node->alpha <<
                  " beta = " << node->beta <<
                  " value = " << hashValue << endl;
            }
#endif
            return hashValue;                     // cutoff
         }
         break;
      default:
         break;
      } // end switch
      // Note: hash move may be usable even if score is not usable
      hashMove = hashEntry.bestMove(board);
   }
   if (tt_depth == HashEntry::QSEARCH_NO_CHECK_DEPTH && !(inCheck || CaptureOrPromotion(hashMove))) {
      // don't fetch a non-capture/promotion checking move from the
      // hash table if we aren't at a depth where checks are allowed.
      hashMove = NullMove;
   }
   if (inCheck) {
#ifdef _TRACE
      indent(ply); cout << "in_check=1" << endl;
#endif
      // If last move was a checking move, ensure that in making it we did
      // not move a pinned piece or move the king into check (normally we
      // would detect this by finding the King can be captured, but if in
      // check we only generate evasions and will not find this).
      ASSERT(board.anyAttacks(board.kingSquare(board.sideToMove()),board.oppositeSide()));
      if (!board.wasLegal((node-1)->last_move)) {
         return -Illegal;
      }
      score_t try_score;
      MoveGenerator mg(board, &context, ply, hashMove, (node-1)->last_move, master());
      Move move;
      BoardState state = board.state;
      node->num_try = 0;
      int noncaps = 0;
      int moveIndex = 0;
      while ((move = mg.nextEvasion(moveIndex)) != NullMove) {
         ASSERT(OnBoard(StartSquare(move)));
         if (Capture(move) == King) {
#ifdef _TRACE
            if (master()) {
               indent(ply);
               cout << "previous move illegal, exiting qsearch" << endl;
            }
#endif
            return -Illegal;
         }
#ifdef _TRACE
         if (master()) {
            indent(ply);
            cout << "trying " << ply << ". ";
            MoveImage(move,cout);
            cout << endl;
         }
#endif
         if (!node->PV() &&
             noncaps > std::max<int>(1+depth,0) &&
             !Scoring::mateScore(node->beta) &&
             !IsForced(move) && !IsForced2(move) &&
             !CaptureOrPromotion(move) &&
             board.wouldCheck(move) == NotInCheck) {
            // We have searched one or more legal non-capture evasions
            // and failed to cutoff. So don't search any more.
#ifdef _TRACE
            indent(ply); cout << "pruned" << endl;
#endif
            continue;
         }
         node->last_move = move;
         board.doMove(move);
         ASSERT(!board.anyAttacks(board.kingSquare(board.oppositeSide()),board.sideToMove()));
         try_score = -quiesce(-node->beta, -node->best_score, ply+1, depth-1);
         board.undoMove(move,state);
         if (try_score != Illegal) {
#ifdef _TRACE
            if (master()) {
               indent(ply);
               cout << ply << ". ";
               MoveImage(move,cout);
               cout << ' ' << try_score << endl;
            }
#endif
            node->num_try++;
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
         else if (master()) {
            indent(ply);
            cout << ply << ". ";
            MoveImage(move,cout);
            cout << " (illegal)" << endl;
         }
#endif
      }
      if (node->num_try == 0) { // no legal evasions, so this is checkmate
#ifdef _TRACE
         if (master()) {
            indent(ply); cout << "checkmate!" << endl;
         }
#endif
         node->best_score = -(Constants::MATE - ply);
         (node+1)->pv_length=0; // no PV from this point
         node->flags |= EXACT;
      }
      ASSERT(node->best_score >= -Constants::MATE && node->best_score <= Constants::MATE);
      storeHash(board,hash,node->best,tt_depth);
      if (node->inBounds(node->best_score)) {
         if (!IsNull(node->best)) {
            updatePV(board,node->best,ply);
         }
      }
      return node->best_score;
   }
   else {
      // not in check
      bool had_eval = false;
      // Establish a default score.  This score is returned if no
      // captures are generated, or if no captures generate a better
      // score (since we generally can choose whether or not to capture).
      ASSERT(node->eval == Scoring::INVALID_SCORE);
      had_eval = node->staticEval != Scoring::INVALID_SCORE;
      if (had_eval) {
          node->eval = node->staticEval;
          ASSERT(node->eval >= -Constants::MATE && node->eval <= Constants::MATE);
      }
      if (node->eval == Scoring::INVALID_SCORE) {
          node->eval = node->staticEval = scoring.evalu8(board);
      }
      if (hashHit) {
          // Use the transposition table entry to provide a better score
          // for pruning decisions, if possible
          const score_t hashValue = hashEntry.getValue();
          if (result == (hashValue > node->eval ? HashEntry::LowerBound :
                         HashEntry::UpperBound)) {
              node->eval = hashValue;
              ASSERT(node->eval >= -Constants::MATE && node->eval <= Constants::MATE);
          }
      }
      ASSERT(node->eval != Scoring::INVALID_SCORE);
#ifdef _TRACE
      if (master()) {
         indent(ply);
         cout << "stand pat score = " << node->eval << endl;
      }
#endif
      node->best_score = node->alpha;
      if (node->eval > node->best_score) {
         node->best_score = node->eval;
         (node+1)->pv_length=0;
         if (node->eval >= node->beta) {
#ifdef _TRACE
            if (master()) {
               indent(ply); cout << "**CUTOFF**" << endl;
            }
#endif
            // check legality of prev move but not at depth == 0 (because
            // regular search checked already)
            if (depth < 0 && !board.wasLegal((node-1)->last_move)) {
               return -Illegal;
            }
            ASSERT(!board.anyAttacks(board.kingSquare(board.oppositeSide()),board.sideToMove()));
            // store eval in hash table if not already fetched from there
            if (!had_eval) {
               controller->hashTable.storeHash(hash, tt_depth,
                                               controller->age,
                                               HashEntry::Eval,
                                               node->best_score,
                                               node->staticEval,
                                               0,
                                               node->best);
            }
            return node->eval;
         }
      }
      int move_index = 0;
      score_t try_score;
      BoardState state(board.state);
      const ColorType oside = board.oppositeSide();
      Bitboard disc(board.getPinned(board.kingSquare(oside),board.sideToMove(),board.sideToMove()));
      // Isn't really a loop: but we code this way so can use
      // break to exit the following block.
      while (!IsNull(hashMove) && validMove(board,hashMove)) {
         if (Capture(hashMove) == King) {
#ifdef _TRACE
            if (master()) {
               indent(ply);
               cout << "previous move illegal, exiting qsearch" << endl;
            }
#endif
            return -Illegal;
         }
         node->last_move = hashMove;
#ifdef _TRACE
         if (master()) {
            indent(ply);
            cout << "trying " << ply << ". ";
            MoveImage(hashMove,cout);
            cout << endl;
         }
#endif
         if (!board.wouldCheck(hashMove) &&
             !passedPawnPush(board,hashMove) &&
             node->beta > -Constants::TABLEBASE_WIN &&
             (Capture(hashMove) == Pawn || board.getMaterial(oside).pieceCount() > 1)) {
            const score_t optScore = Params::Gain(hashMove) + QSEARCH_FORWARD_PRUNE_MARGIN + node->eval;
            if (optScore < node->best_score) {
#ifdef _TRACE
               if (master()) {
                  indent(ply); cout << "pruned (futility)" << endl;
               }
#endif
               node->best_score = std::max<score_t>(node->best_score,optScore);
               break;
            }
         }
         // Don't do see pruning for the hash move. The hash move
         // already passed a SEE test, although possibly with
         // different bounds. Doing SEE here tests worse.
         board.doMove(hashMove);
         ASSERT(!board.anyAttacks(board.kingSquare(board.oppositeSide()),board.sideToMove()));
         try_score = -quiesce(-node->beta, -node->best_score, ply+1, depth-1);
         board.undoMove(hashMove,state);
         if (try_score != Illegal) {
#ifdef _TRACE
            if (master()) {
               indent(ply);
               cout << ply << ". ";
               MoveImage(hashMove,cout);
               cout << "(pv) " << try_score << endl;
            }
#endif
            if (try_score > node->best_score) {
               node->best_score = try_score;
               node->best = hashMove;
               if (try_score >= node->beta)
                  goto search_end;
               if (node->best_score >= Constants::MATE-1-ply)
                  goto search_end;              // mating move found
            }
         }
         break;
      }
      {
         MoveGenerator mg(board, &context, ply,
                          NullMove, (node-1)->last_move, master());
         Move *moves = (Move*)node->done;
         // generate all the capture moves
         int move_count = mg.generateCaptures(moves,board.occupied[oside]);
         mg.initialSortCaptures(moves, move_count);
         while (move_index < move_count) {
            Move move = moves[move_index++];
            if (MovesEqual(move,hashMove)) continue;  // already did this one
            if (Capture(move) == King) {
#ifdef _TRACE
               if (master()) {
                  indent(ply);
                  cout << "previous move illegal, exiting qsearch" << endl;
               }
#endif
               return -Illegal;
            }
#ifdef _TRACE
            if (master()) {
               indent(ply);
               cout << "trying " << ply << ". ";
               MoveImage(move,cout);
               cout << endl;
            }
#endif
            // Futility pruning
            if (!board.wouldCheck(move) &&
                !passedPawnPush(board,move) &&
                node->beta > -Constants::TABLEBASE_WIN &&
                (Capture(move) == Pawn || board.getMaterial(oside).pieceCount() > 1)) {
               const score_t optScore = Params::Gain(move) + QSEARCH_FORWARD_PRUNE_MARGIN + node->eval;
               if (optScore < node->best_score) {
#ifdef _TRACE
                  if (master()) {
                     indent(ply); cout << "pruned (futility)" << endl;
                  }
#endif
                  continue;
               }
            }
            // See pruning
            score_t neededGain = node->best_score - node->eval - QSEARCH_FORWARD_PRUNE_MARGIN;
            if (Params::PieceValue(Capture(move)) - Params::PieceValue(PieceMoved(move)) <= neededGain &&
                node->beta > -Constants::TABLEBASE_WIN &&
                !passedPawnPush(board,move) &&
                !disc.isSet(StartSquare(move)) &&
                !seeSign(board,move,std::max<score_t>(0,neededGain))) {
#ifdef _TRACE
               if (master()) {
                  indent(ply); cout << "pruned (SEE)" << endl;
               }

#endif
               continue;
            }
            node->last_move = move;
            board.doMove(move);
            try_score = -quiesce(-node->beta, -node->best_score, ply+1, depth-1);
            board.undoMove(move,state);
            if (try_score != Illegal) {
#ifdef _TRACE
               if (master()) {
                  indent(ply);
                  cout << ply << ". ";
                  MoveImage(move,cout);
                  cout << ' ' << try_score << endl;
               }
#endif
               if (try_score > node->best_score) {
                  node->best_score = try_score;
                  node->best = move;
                  if (try_score >= node->beta) {
#ifdef _TRACE
                     if (master()) {
                        indent(ply);
                        cout << "**CUTOFF**" << endl;
                     }
#endif
                     goto search_end;
                  }
                  if (node->best_score >= Constants::MATE-1-ply)
                     goto search_end;          // mating move found
               }
            }
#ifdef _TRACE
            else if (master()) {
               indent(ply);
               cout << ply << ". ";
               MoveImage(move,cout);
               cout << " (illegal)" << endl;
            }
#endif
         }
         // Do checks in qsearch
         if ((node->eval >= node->alpha - 2*Params::PAWN_VALUE) &&
             (depth >= 1-srcOpts.checks_in_qsearch)) {
            move_count = mg.generateChecks(moves,disc);
            move_index = 0;
#ifdef _TRACE
            if (master()) {
               if (move_count) {
                  indent(ply);
                  cout << move_count << " checks generated" << endl;;
               }
            }
#endif
            while (move_index < move_count) {
               Move move = moves[move_index++];
               if (MovesEqual(move,hashMove)) continue;
#ifdef _TRACE
               if (master()) {
                  indent(ply);
                  cout << "trying " << ply << ". ";
                  MoveImage(move,cout);
                  cout << endl;
               }
#endif
               // prune checks that cause loss of the checking piece (but not
               // discovered checks)
               if (!disc.isSet(StartSquare(move)) && !seeSign(board,move,0)) {
#ifdef _TRACE
                  if (master()) {
                     indent(ply); cout << "pruned" << endl;
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
               board.doMove(move);
               // verify opposite side in check:
               ASSERT(board.anyAttacks(board.kingSquare(board.sideToMove()),board.oppositeSide()));
               // and verify quick check confirms it
               ASSERT(board.checkStatus(move)==InCheck);
               // We know the check status so set it, so it does not
               // have to be computed
               board.setCheckStatus(InCheck);
               try_score = -quiesce(-node->beta,-node->best_score,
                                    ply+1,depth-1);
               board.undoMove(move,state);
               if (try_score != Illegal) {
#ifdef _TRACE
                  if (master()) {
                     indent(ply);
                     cout << ply << ". ";
                     MoveImage(move,cout);
                     cout << ' ' << try_score << endl;
                  }
#endif
                  if (try_score > node->best_score) {
                     node->best_score = try_score;
                     node->best = move;
                     if (try_score >= node->beta) {
#ifdef _TRACE
                        if (master()) {
                           indent(ply); cout << "**CUTOFF**" << endl;
                        }

#endif
                        goto search_end;
                     }
                     if (node->best_score >= Constants::MATE-1-ply)
                        goto search_end;      // mating move found
                  }
               }
#ifdef _TRACE
               else if (master()) {
                  indent(ply);
                  cout << ply << ". ";
                  MoveImage(move,cout);
                  cout << " (illegal)" << endl;
               }
#endif
            }
         }
      }
   search_end:
      ASSERT(node->best_score >= -Constants::MATE && node->best_score <= Constants::MATE);
      storeHash(board,hash,node->best,tt_depth);
      if (node->inBounds(node->best_score)) {
         if (!IsNull(node->best)) {
            updatePV(board,node->best,ply);
         }
      }
      return node->best_score;
   }
}

void Search::storeHash(const Board &board, hash_t hash, Move hash_move, int depth) {
    // don't insert into the hash table if we are terminating - we may
    // not have an accurate score.
    if (!terminate) {
        // store the position in the hash table, if there's room
        score_t value = node->best_score;
        HashEntry::ValueType val_type;
        // Adjust mate scores to reflect current ply.
        if (value <= -Constants::MATE_RANGE) {
             value -= node->ply;
             val_type = HashEntry::Valid;
        }
        else if (value >= Constants::MATE_RANGE) {
             value += node->ply;
             val_type = HashEntry::Valid;
        }
        else {
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
        }
#ifdef _TRACE
        static const char type_chars[5] =
            { 'E', 'U', 'L', 'X', 'X' };
        if (master()) {
            indent(node->ply);
            cout << "storing type=" << type_chars[val_type] <<
                " ply=" << node->ply << " depth=" << depth << " value=" << value <<
                " move=";
            MoveImage(node->best,cout);
            cout << endl;
        }
#endif
        controller->hashTable.storeHash(hash, depth,
            controller->age,
            val_type,
            node->best_score, node->staticEval,
            (IsForced(node->best) ? HashEntry::FORCED_MASK : 0),
            node->best);
    }
}

int Search::calcExtensions(const Board &board,
                           NodeInfo *node, NodeInfo *parentNode,
                           CheckStatusType in_check_after_move,
                           int moveIndex,
                           Move move) {
   // see if we should apply any extensions at this node.
   int depth = node->depth;
   node->extensions = 0;
   int extend = 0;
   int pruneOk = board.checkStatus() != InCheck;
   score_t swap = Scoring::INVALID_SCORE;
   if (in_check_after_move == InCheck) { // move is a checking move
      // extend if check does not lose material or is a discovered check
      if ((swap = seeSign(board,move,0)) ||
          board.isPinned(board.oppositeSide(),move)) {
          node->extensions |= CHECK;
#ifdef SEARCH_STATS
          controller->stats->check_extensions++;
#endif
          extend += node->PV() ? PV_CHECK_EXTENSION : NONPV_CHECK_EXTENSION;
      }
      else {
         // Bad checks can be reduced or pruned (SEE pruning only)
         pruneOk = 0;
      }
   }
   if (passedPawnPush(board,move)) {
      node->extensions |= PAWN_PUSH;
      extend += PAWN_PUSH_EXTENSION;
#ifdef SEARCH_STATS
      controller->stats->pawn_extensions++;
#endif
   }
   else if (TypeOfMove(move) == Normal &&
            Capture(move) != Empty && Capture(move) != Pawn &&
            board.getMaterial(board.oppositeSide()).pieceCount() == 1 &&
            board.getMaterial(board.sideToMove()).noPieces()) {
      // Capture of last piece in endgame.
      node->extensions |= CAPTURE;
      extend += CAPTURE_EXTENSION;
#ifdef SEARCH_STATS
      ++controller->stats->capture_extensions;
#endif
   }
   if (extend) {
      return std::min<int>(extend,DEPTH_INCREMENT);
   }

   // See if we do late move reduction. Moves in the history phase of move
   // generation can be searched with reduced depth.
   if (depth >= LMR_DEPTH && moveIndex >= 1+2*node->PV() &&
       (board.checkStatus() == InCheck ? GetPhase(move) > MoveGenerator::WINNING_CAPTURE_PHASE : GetPhase(move) == MoveGenerator::HISTORY_PHASE) &&
       !passedPawnMove(board,move,6)) {
       extend -= LMR_REDUCTION[node->PV()][depth/DEPTH_INCREMENT][std::min<int>(63,moveIndex)];
       if (extend) {
           // history based reductions
           int hist = context.scoreForOrdering(move,(node->ply == 0) ? NullMove\
                                               : (node-1)->last_move,board.sideToMove())/128;
           extend += hist*DEPTH_INCREMENT;
           extend = std::max(extend,1-depth);
           if (extend <= -DEPTH_INCREMENT) {
               node->extensions |= LMR;
#ifdef SEARCH_STATS
               ++controller->stats->reduced;
#endif
           } else {
               // do not reduce < 1 ply
               extend = 0;
           }
       }
   }

   // for pruning decisions, use depth after LMR
   const int predictedDepth = depth + extend;

   pruneOk &= !node->PV() && parentNode->num_try &&
       Capture(move) == Empty &&
       TypeOfMove(move) == Normal &&
       !passedPawnMove(board,move,5) &&
       !Scoring::mateScore(parentNode->alpha);

   if (pruneOk) {
      // do not use predictedDepth for LMP
      if(depth/DEPTH_INCREMENT <= LMP_DEPTH &&
         GetPhase(move) >= MoveGenerator::HISTORY_PHASE &&
         moveIndex >= LMP_MOVE_COUNT[depth/DEPTH_INCREMENT]) {
#ifdef SEARCH_STATS
         ++controller->stats->lmp;
#endif
#ifdef _TRACE
         indent(node->ply); cout << "LMP: pruned" << endl;
#endif
         return PRUNE;
      }
      // futility pruning, enabled at low depths
      if (predictedDepth <= FUTILITY_DEPTH) {
         // Threshold was formerly increased with the move index
         // but this tests worse now.
          score_t threshold = parentNode->beta - futilityMargin(predictedDepth);
         if (node->eval == Scoring::INVALID_SCORE) {
            node->eval = node->staticEval = scoring.evalu8(board);
         }
         if (node->eval < threshold) {
#ifdef SEARCH_STATS
            controller->stats->futility_pruning++;
#endif
#ifdef _TRACE
            indent(node->ply); cout << "futility: pruned" << endl;
#endif
            return PRUNE;
         }
      }
   }
   // See pruning. Losing captures and moves that put pieces en prise
   // are pruned at low depths. Losing checks can be pruned.
   if (predictedDepth <= SEE_PRUNING_DEPTH &&
       !node->PV() && parentNode->num_try &&
       !Scoring::mateScore(node->alpha) &&
       board.checkStatus() == NotInCheck &&
       GetPhase(move) > MoveGenerator::WINNING_CAPTURE_PHASE) {
       if (GetPhase(move) == MoveGenerator::LOSERS_PHASE) {
           swap = 0;
       }
       if (swap == Scoring::INVALID_SCORE) swap = seeSign(board,move,0);
       if (!swap) {
#ifdef SEARCH_STATS
          ++controller->stats->see_pruning;
#endif
          return PRUNE;
       }
   }
   return extend;
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
    ++controller->stats->reg_nodes;
#endif
    int ply = node->ply;
    int depth = node->depth;
    ASSERT(ply < Constants::MaxPly);
    if (++nodeAccumulator > NODE_ACCUM_THRESHOLD) {
        controller->stats->num_nodes += nodeAccumulator;
        nodeAccumulator = 0;
#if defined(SMP_STATS)
        // sample thread usage
        Statistics *stats = controller->stats;
        if (--controller->sample_counter <=0) {
           stats->samples++;
           stats->threads += controller->pool->activeCount();
           controller->sample_counter = SAMPLE_INTERVAL;
        }
#endif
        if (--controller->time_check_counter <= 0) {
            controller->time_check_counter = Time_Check_Interval;
            if (checkTime(board,ply)) {
               if (talkLevel == Trace) {
                  cout << "# terminating, time up" << endl;
               }
               controller->terminateNow();   // signal all searches to end
            }
        }
    }
    if (terminate) {
        return node->alpha;
    }
    else if (ply >= Constants::MaxPly-1) {
       if (!board.wasLegal((node-1)->last_move)) {
          return -Illegal;
       }
       node->flags |= EXACT;
       return scoring.evalu8(board);
    }

    if (Scoring::isDraw(board,rep_count,ply)) {
        node->flags |= EXACT;
        if (!board.wasLegal((node-1)->last_move)) {
           return -Illegal;
        }
#ifdef _TRACE
        if (master()) {
            indent(ply); cout << "draw!" << endl;
        }
#endif
        return drawScore(board);
    }
    Move hashMove = NullMove;
    using_tb = 0;
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS) || defined(SYZYGY_TBS)
    int egtbDepth = Constants::MaxPly*DEPTH_INCREMENT;
    if (srcOpts.use_tablebases) {
        const Material &wMat = board.getMaterial(White);
        const Material &bMat = board.getMaterial(Black);
        egtbDepth = 3*DEPTH_INCREMENT*root()->getIterationDepth()/4;
        using_tb = (wMat.men() + bMat.men() <= EGTBMenCount) &&
#ifdef SYZYGY_TBS
            srcOpts.tb_probe_in_search &&
           (srcOpts.tablebase_type == Options::TbType::SyzygyTb ?
            (node->depth/DEPTH_INCREMENT >= options.search.syzygy_probe_depth)
            : (depth >= egtbDepth || ply <= 2));
#else
			(depth >= egtbDepth || ply <= 2);
#endif
    }
#endif
    HashEntry hashEntry;
    HashEntry::ValueType result;
    bool hashHit = false;
    score_t hashValue = Scoring::INVALID_SCORE;
    if (node->flags & IID) {
       // already did hash probe, with no hit
       result = HashEntry::NoHit;
    }
#ifdef SINGULAR_EXTENSION
    else if (node->flags & SINGULAR) {
        hashMove = node->singularMove;
        result = HashEntry::Invalid;
    }
#endif
    else {
       // Search the hash table to see if we have hit this
       // position before.
       // Note: query the hash table before the tablebases, since TB info may
       // be cached.
       // Note: we copy the hash entry .. so mods by another thread do not
       // alter the copy
       result = controller->hashTable.searchHash(board,board.hashCode(rep_count),
                                                 ply,depth,controller->age,hashEntry);
#ifdef SEARCH_STATS
       controller->stats->hash_searches++;
#endif
       hashHit = result != HashEntry::NoHit;
    }
    if (hashHit) {
#ifdef SEARCH_STATS
        controller->stats->hash_hits++;
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
        hashValue = hashEntry.getValue();
        switch (result) {
            case HashEntry::Valid:
#ifdef _TRACE
                if (master()) {
                    indent(ply);
                    cout << "hash cutoff, type = E" <<
                        " alpha = " << node->alpha <<
                        " beta = " << node->beta <<
                        " value = " << hashValue << endl;
                }
#endif
                // If this is a mate score, adjust it to reflect the
                // current ply depth.
                //
                if (hashValue >= Constants::MATE_RANGE) {
                    hashValue -= ply;
                }
                else if (hashValue <= -Constants::MATE_RANGE) {
                    hashValue += ply;
                }
                if (node->inBounds(hashValue)) {
                    // parent node will consider this a new best line
                    hashMove = hashEntry.bestMove(board);
                    if (!IsNull(hashMove)) {
                        node->pv[ply] = hashMove;
                        node->pv_length = 1;
                    }
#ifdef _DEBUG
                    if (!IsNull(hashMove) && !legalMove(board,hashMove)) {
                       cout << '#' << board << endl << (flush);
                       cout << '#';
                       MoveImage(hashMove,cout);
                       cout << endl << (flush);
                    }
#endif
#ifdef _TRACE
                    if (master()) {
                        indent(ply); cout << "best line[ply][ply] = ";
                        MoveImage(hashMove,cout);
                        cout << endl;
                    }
#endif
                }
                node->flags |= EXACT;
                return hashValue;
            case HashEntry::UpperBound:
                if (hashValue <= node->alpha) {
#ifdef _TRACE
                    if (master()) {
                        indent(ply);
                        cout << "hash cutoff, type = U" <<
                            " alpha = " << node->alpha <<
                            " beta = " << node->beta <<
                            " value = " << hashValue << endl;
                    }
#endif
                    return hashValue;                     // cutoff
                }
                break;
            case HashEntry::LowerBound:
                if (hashValue >= node->beta) {
#ifdef _TRACE
                    if (master()) {
                        indent(ply);
                        cout << "hash cutoff, type = L" <<
                            " alpha = " << node->alpha <<
                            " beta = " << node->beta <<
                            " value = " << hashValue << endl;
                    }
#endif
                    if (board.checkStatus() != InCheck) {
                       Move best = hashEntry.bestMove(board);
                       if (!IsNull(best) && !CaptureOrPromotion(best)) {
                           context.updateStats(board, node, best,
                             node->depth, board.sideToMove());
                          context.setKiller(best, node->ply);
                       }
                    }
                    return hashValue;                     // cutoff
                }
                break;
            default:
                break;
        } // end switch
        // Note: hash move may be usable even if score is not usable
        hashMove = hashEntry.bestMove(board);
    }
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS) || defined(SYZYGY_TBS)
    if (using_tb && rep_count==0 && !(node->flags & (IID|VERIFY|SINGULAR|PROBCUT))) {
       int tb_hit = 0;
       controller->stats->tb_probes++;
       score_t tb_score;
#ifdef NALIMOV_TBS
       if (srcOpts.tablebase_type == Options::TbType::NalimovTb) {
          tb_hit = NalimovTb::probe_tb(board, tb_score, ply);
       }
#endif
#ifdef GAVIOTA_TBS
       if (srcOpts.tablebase_type == Options::TbType::GaviotaTb) {
          // TBD: use soft probing at lower depths
          tb_hit = GaviotaTb::probe_tb(board, tb_score, ply, true);
       }
#endif
#ifdef SYZYGY_TBS
       if (srcOpts.tablebase_type == Options::TbType::SyzygyTb) {
          tb_hit = SyzygyTb::probe_wdl(board, tb_score,
                                       srcOpts.syzygy_50_move_rule != 0);
       }
#endif
       if (tb_hit) {
            controller->stats->tb_hits++;
#ifdef _TRACE
            if (master()) {
                indent(ply); cout << "EGTB hit: score " << tb_score << endl;
            }
#endif
            score_t score = tb_score;
            // insert TB info in hash table. Adjust mate scores for
            // plies from root. Note: do not adjust TABLEBASE_WIN scores.
            if (score <= -Constants::MATE_RANGE) {
                score -= ply;
            }
            else if (score >= Constants::MATE_RANGE) {
                score += ply;
            }
#ifdef _TRACE
            if (master() && tb_score != score) {
                indent(ply); cout << "adjusted score " << score << endl;
            }
#endif
            // Put it in with a large depth so we will not
            // overwrite - this entry is "exact" at all
            // search depths, so effectively its depth is infinite.
            controller->hashTable.storeHash(board.hashCode(rep_count),
                (Constants::MaxPly-1)*DEPTH_INCREMENT,
                controller->age,
                HashEntry::Valid,
                score,
                Scoring::INVALID_SCORE,
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
	   cout << "# " << board << endl;
	   cout << "# move=";
	   MoveImage((node-1)->last_move,cout);
	   cout << endl;
	   ASSERT(0);
    }
#endif
#ifdef _TRACE
    if (master() && in_check) { indent(ply); cout << "in_check=" << in_check << endl;}
#endif
    // Note: for a singular search, leave the eval and staticEval
    // fields alone: they have already been set.
    if (!(node->flags & SINGULAR)) {
       node->eval = node->staticEval = Scoring::INVALID_SCORE;
       if (hashHit) {
          // Use the cached static value if possible
          node->eval = node->staticEval = hashEntry.staticValue();
       }
       if (node->eval == Scoring::INVALID_SCORE) {
          node->eval = node->staticEval = scoring.evalu8(board);
       }
       if (hashHit) {
          // Use the transposition table entry to provide a better score
          // for pruning decisions, if possible
          if (result == (hashValue > node->eval ? HashEntry::LowerBound :
                         HashEntry::UpperBound)) {
             node->eval = hashValue;
          }
       }
    }

    const bool pruneOk = !in_check &&
        !node->PV() &&
        !(node->flags & (IID|VERIFY|SINGULAR|PROBCUT)) &&
        board.getMaterial(board.sideToMove()).hasPieces();

#ifdef STATIC_NULL_PRUNING
    // static null pruning, aka reverse futility pruning,
    // as in Protector, Texel, etc.
    if (pruneOk && depth <= STATIC_NULL_PRUNING_DEPTH &&
        node->beta < Constants::TABLEBASE_WIN) {
        const score_t margin = futilityMargin(depth);
       const score_t threshold = node->beta+margin;
       ASSERT(node->eval != Scoring::INVALID_SCORE);
       if (node->eval >= threshold) {
#ifdef _TRACE
          indent(ply); cout << "static null pruned" << endl;
#endif
#ifdef SEARCH_STATS
          ++controller->stats->static_null_pruning;
#endif
          node->best_score = node->eval - margin;
          goto hash_insert;
       }
    }
#endif

#ifdef RAZORING
    // razoring as in Glaurung & Toga
    if (pruneOk && node->beta < Constants::MATE_RANGE &&
        depth <= RAZOR_DEPTH) {
        const score_t threshold = node->beta - razorMargin(depth);
        ASSERT(node->eval != Scoring::INVALID_SCORE);
        if (node->eval < threshold) {
            // Note: use threshold as the bounds here, not beta, as
            // was done in Toga 3.0:
            score_t v = quiesce(threshold-1,threshold,ply+1,0);
            if (v != -Illegal && v < threshold) {
#ifdef _TRACE
                indent(ply); cout << "razored node, score=" << v << endl;
#endif
#ifdef SEARCH_STATS
                controller->stats->razored++;
#endif
                node->best_score = v;
                goto hash_insert;
            }
        }
    }
#endif

    // Try to get a fast cutoff using a "null move".  Skip if the side
    // to move is in check or if material balance is low enough that
    // zugzwang is a possibility. Do not do null move if this is an
    // IID search, because it will only help us get a cutoff, not a move.
    // Also avoid null move near the 50-move draw limit.
    if (pruneOk && depth >= DEPTH_INCREMENT &&
        !IsNull((node-1)->last_move) &&
        ((node->staticEval >= node->beta - int(0.25*Params::PAWN_VALUE) * (depth / DEPTH_INCREMENT - 6)) || (depth >= 12*DEPTH_INCREMENT)) &&
        !Scoring::mateScore(node->alpha) &&
        board.state.moveCount <= 98) {
        int nu_depth;
        // R=3 + some depth-dependent increment
        nu_depth = depth - 4*DEPTH_INCREMENT - depth/6;

        // Skip null move if likely to be futile according to hash info
        if (!hashHit || !hashEntry.avoidNull(nu_depth,node->beta)) {
            node->last_move = NullMove;
            BoardState state = board.state;
            board.doNull();
#ifdef _TRACE
            if (master()) {
                indent(ply);
                cout << "trying " << ply << ". " << "(null)" << endl;
            }
#endif
            score_t nscore;
            if (nu_depth > 0)
                nscore = -search(-node->beta, 1-node->beta,
                                 ply+1, nu_depth);
            else
                nscore = -quiesce(-node->beta, 1-node->beta,
                                  ply+1, 0);
#ifdef _TRACE
            if (master()) {
                indent(ply);
                cout << ply << ". " << "(null)" << ' ' << nscore << endl;
            }
#endif
            board.undoNull(state);
            if (terminate) {
                node->best_score = node->alpha;
                goto search_end2;
            }
            else if (nscore >= node->beta) {          // cutoff
#ifdef VERIFY_NULL_SEARCH
                if (depth >= 6*DEPTH_INCREMENT) {
                    // Verify null cutoff with reduced-depth search
                    // (idea from Dieter Buerssner)
                    nu_depth = depth-5*DEPTH_INCREMENT;
                    ASSERT(nu_depth > 0);
                    nscore = search(node->alpha, node->beta,
                                    ply+1, nu_depth, VERIFY);
                    if (nscore == -Illegal) {
#ifdef _TRACE
                        indent(ply); cout << "previous move illegal" << endl;
#endif
                        return -Illegal;
                    }
#ifdef _TRACE
                    if (master()) {
                        indent(ply);
                        if (nscore>=node->beta)
                            cout << "null cutoff verified, score=" << nscore;
                        else
                            cout << "null cutoff not verified";
                        cout << endl;
                    }
#endif
                }
                if (nscore >= node->beta)             // null cutoff
#endif
                    {
#ifdef _TRACE
                        if (master()) {
                            indent(ply);
                            cout << "**CUTOFF**" << endl;
                        }
#endif
#ifdef SEARCH_STATS
                        controller->stats->null_cuts++;
#endif
                        // Do not return a mate score from the null move search.
                        node->best_score = nscore >= Constants::MATE-ply ? node->beta :
                            nscore;
                        goto hash_insert;
                    }
            }
        }
    }

    // ProbCut
    if (!node->PV() && board.checkStatus() == NotInCheck &&
        depth >= PROBCUT_DEPTH &&
        node->beta < Constants::MATE_RANGE) {
       const score_t threshold = std::min<score_t>(Constants::MATE,node->beta + PROBCUT_MARGIN);
       const int nu_depth = depth - 4*DEPTH_INCREMENT;
       BoardState state(board.state);
       if (!IsNull(hashMove) && Capture(hashMove) > Pawn && node->eval + Params::Gain(hashMove) >= threshold - PROBCUT_MARGIN2 && validMove(board,hashMove) && seeSign(board,hashMove,PROBCUT_MARGIN)) {
#ifdef _TRACE
           indent(ply);
           cout << "Probcut: trying " << ply << ". ";
           MoveImage(hashMove,cout);
#endif
           board.doMove(hashMove);
           if (!board.wasLegal(hashMove)) {
               board.undoMove(hashMove,state);
               goto probcut_search;
           }
           SetPhase(hashMove,MoveGenerator::WINNING_CAPTURE_PHASE);
           node->last_move = hashMove;
           node->num_try++;
           //int extension = 0;
           //if (board.checkStatus() == InCheck) extension += DEPTH_INCREMENT;
           score_t value = -search(-threshold-1, -threshold,
                                   ply+1, nu_depth, PROBCUT);
#ifdef _TRACE
           indent(ply);
           cout << ply << ". ";
           MoveImage(hashMove,cout);
           cout << " " << value << endl;
#endif
           board.undoMove(hashMove,state);
           if (value != Illegal && value > threshold) {
               // We have found a good capture .. so assume this
               // refutes the previous move and do not search
               // further
#ifdef _TRACE
               indent(ply);
               cout << "Probcut: cutoff" << endl;
#endif
               node->best_score = value;
               node->best = hashMove;
               goto hash_insert;
           }
       }
    probcut_search:
       {
          Move moves[40];
          MoveGenerator mg(board, &context, ply, hashMove, (node-1)->last_move, master());
          // skip pawn captures because they will be below threshold
          int moveCount = mg.generateCaptures(moves,board.occupied[board.oppositeSide()] & ~board.pawn_bits[board.oppositeSide()]);
          for (int i = 0; i<moveCount; i++) {
             if (MovesEqual(hashMove,moves[i])) {
                continue;
             }
             else if (Capture(moves[i])==King) {
                return -Illegal;                  // previous move was illegal
             }
             else if (node->eval + Params::Gain(moves[i]) >= threshold - PROBCUT_MARGIN2 && seeSign(board,moves[i],threshold)) {
#ifdef _TRACE
                indent(ply);
                cout << "Probcut: trying " << ply << ". ";
                MoveImage(moves[i],cout);
#endif
                board.doMove(moves[i]);
                if (!board.wasLegal(moves[i])) {
                   board.undoMove(moves[i],state);
                   continue;
                }
                SetPhase(moves[i],MoveGenerator::WINNING_CAPTURE_PHASE);
                node->last_move = moves[i];
                node->num_try++;
                //int extension = 0;
                //if (board.checkStatus() == InCheck) extension += DEPTH_INCREMENT;
                int value = -search(-threshold-1, -threshold,
                                    ply+1, nu_depth, PROBCUT);
#ifdef _TRACE
                indent(ply);
                cout << ply << ". ";
                MoveImage(moves[i],cout);
                cout << " " << value << endl;
#endif
                board.undoMove(moves[i],state);
                if (value != Illegal && value > threshold) {
                   // We have found a good capture .. so assume this
                   // refutes the previous move and do not search
                   // further
#ifdef _TRACE
                   indent(ply);
                   cout << "Probcut: cutoff" << endl;
#endif
                   node->best_score = value;
                   node->best = moves[i];
                   goto hash_insert;
                }
             }
          }
       }
       node->num_try = 0;
       node->last_move = NullMove;
    }

    // Use "internal iterative deepening" to get an initial move to try if
    // there is no hash move .. an idea from Crafty (previously used by
    // Hitech).
    if (IsNull(hashMove) &&
        (depth >= (node->PV() ? 4*DEPTH_INCREMENT : 6*DEPTH_INCREMENT)) &&
        (node->PV() ||
         (board.checkStatus() == NotInCheck &&
          node->eval >= node->beta - Params::PAWN_VALUE))) {
        int d;
        d = depth/2;
        if (!node->PV()) d-=DEPTH_INCREMENT;
#ifdef _TRACE
        if (master()) {
            indent(ply); cout << "== start IID, depth = " << d
                << endl;
        }
#endif
        // Call search routine at lower depth to get a 1st move to try.
        // ("Internal iterative deepening").
        //
        // Note: we do not push down the node stack because we want this
        // search to have all the same parameters (including ply) as the
        // current search, just reduced depth + the IID flag set.
        int old_flags = node->flags;
        node->flags |= IID;
        score_t alpha = node->alpha;
        node->depth = d;
        score_t iid_score = -search();
        // set hash move to IID search result (may still be null)
        hashMove = node->best;
        // reset key params
        node->flags = old_flags;
        node->num_try = 0;
        node->cutoff = 0;
        node->depth = depth;
        node->alpha = node->best_score = alpha;
        node->best = NullMove;
        node->last_move = NullMove;
        // do not retain any pv information from the IID search
        // (can screw up non-IID pv).
        (node+1)->pv[ply+1] = NullMove;
        (node+1)->pv_length = 0;
        node->pv[ply] = NullMove;
        node->pv_length = 0;
        if (iid_score == Illegal || (node->flags & EXACT)) {
           // previous move was illegal or was an exact score
#ifdef _TRACE
          if (master()) {
             indent(ply);
             cout << "== exact result from IID" << endl;
          }
#endif
           return -iid_score;
        }
        if (terminate) {
            return node->alpha;
        }
#ifdef _TRACE
        if (master()) {
            indent(ply); cout << "== IID done.";
        }
#endif

#ifdef _TRACE
        if (master()) {
            if (!IsNull(hashMove)) {
                indent(ply); cout << "  hashMove = ";
                MoveImage(hashMove,cout);
            }
            cout << endl;
        }
#endif
    }
    {
        bool singularExtend = false;
#ifdef SINGULAR_EXTENSION
        if (depth >= SINGULAR_EXTENSION_DEPTH &&
            !(node->flags & SINGULAR) &&
            hashHit &&
            hashEntry.depth() >= depth - 3*DEPTH_INCREMENT &&
            !IsNull(hashMove) &&
            std::abs(hashValue) < Constants::MATE_RANGE &&
            result != HashEntry::UpperBound &&
            calcExtensions(board,node,node,board.wouldCheck(hashMove),
                           0,hashMove) < DEPTH_INCREMENT &&
            validMove(board,hashMove)) {
           // Search all moves but the hash move at reduced depth. If all
           // fail low with a score significantly below the hash
           // move's score, then consider the hash move as "singular" and
           // extend its search depth.
           // This hash-based "singular extension" has been
           // implemented in the Ippo* series of engines (and
           // presumably in Rybka), and also now in Stockfish, Komodo,
           // Texel, Protector, etc.
           score_t nu_beta = hashValue - singularExtensionMargin(depth);
           int nu_depth = singularExtensionDepth(depth);
           // save current bounds & flags
           score_t old_alpha = node->alpha;
           score_t old_beta = node->beta;
           int old_flags = node->flags;
           node->depth = nu_depth;
           node->singularMove = hashMove;
           node->alpha = nu_beta-1;
           node->beta = nu_beta;
           node->flags |= SINGULAR;
           // perform a search w/o pushing down the node stack
           int singularScore = search();
           singularExtend = singularScore <= nu_beta-1;
           // reset all params
           (node+1)->pv[ply+1] = NullMove;
           (node+1)->pv_length = 0;
           node->flags = old_flags;
           node->num_try = 0;
           node->cutoff = 0;
           node->depth = depth;
           node->alpha = node->best_score = old_alpha;
           node->beta = old_beta;
           node->singularMove = NullMove;
           node->last_move = NullMove;
           node->best = NullMove;
           node->pv[ply] = NullMove;
           node->pv_length = 0;
        }
#endif
        MoveGenerator mg(board, &context, ply, hashMove, (node-1)->last_move, master());
        BoardState state = board.state;
        score_t try_score;
        // we do not split if in check because generally there will
        // be few moves to search there.
        const int canSplit = srcOpts.ncpus>1 && !in_check &&
            depth >= threadSplitDepth;
        //
        // Now we are ready to loop through the moves from this position
        //
#ifdef _TRACE
        int first = 1;
#endif
        while (!node->cutoff && !terminate) {
            score_t hibound = node->num_try == 0 ? node->beta : node->best_score +1;
            Move move;
            // we do not bother to lock here, because if we split
            // the node, we don't come back to this part of the loop
            move = in_check ? mg.nextEvasion(move_index) : mg.nextMove(move_index);
            if (IsNull(move)) break;
#ifdef SINGULAR_EXTENSION
            if (IsUsed(move) || MovesEqual(node->singularMove,move)) continue;
#else
            if (IsUsed(move)) continue;
#endif
#ifdef SEARCH_STATS
            ++controller->stats->moves_searched;
#endif
            if (Capture(move)==King) {
                return -Illegal;                  // previous move was illegal
            }
            ASSERT(DestSquare(move) != InvalidSquare);
            ASSERT(StartSquare(move) != InvalidSquare);
#ifdef _TRACE
            if (master()) {
                indent(ply);
                cout << "trying " << ply << ". ";
                MoveImage(move,cout);
               if (first) cout << "(pv)";
                cout << " [" << node->best_score << "," << hibound << "]";
                cout << " d:" << depth << endl;
            }
#endif
            node->last_move = move;
            CheckStatusType in_check_after_move = board.wouldCheck(move);
            int extend;
            if (singularExtend &&
                GetPhase(move) == MoveGenerator::HASH_MOVE_PHASE) {
               extend = DEPTH_INCREMENT;
               node->extensions |= SINGULAR_EXT;
#ifdef SEARCH_STATS
               ++controller->stats->singular_extensions;
#endif
            }
            else {
               extend = calcExtensions(board,node,node,in_check_after_move,
                                        move_index,move);
            }
            if (extend == PRUNE) {
#ifdef _TRACE
              if (master()) {
                indent(ply); cout << "fwd pruned." << endl;
              }
#endif
              continue;
            }
            board.doMove(move);
            if (!in_check && !board.wasLegal(move)) {
                  ASSERT(board.anyAttacks(board.kingSquare(board.oppositeSide()),board.sideToMove()));
#ifdef _TRACE
               if (master()) {
                  indent(ply); cout << "Illegal move!" << endl;
               }
#endif
               board.undoMove(move,state);
               continue;
            }
            setCheckStatus(board, in_check_after_move);
            if (depth+extend-DEPTH_INCREMENT > 0) {
                try_score = -search(-hibound, -node->best_score,
                    ply+1, depth+extend-DEPTH_INCREMENT);
            }
            else {
                try_score = -quiesce(-hibound, -node->best_score,
                    ply+1, 0);
            }
            if (try_score == Illegal) {
#if defined(_TRACE)
                if (master()) {
                    indent(ply); cout << "Illegal move" << endl;
                }
#endif
                board.undoMove(move,state);
                continue;
            }
            while (try_score > node->best_score &&
               (extend < 0 || hibound < node->beta) &&
                !((node+1)->flags & EXACT) &&
                !terminate) {
               // We failed to get a cutoff and must re-search
#ifdef _TRACE
               if (master()) {
                  indent(ply); cout << ply << ". ";
                  MoveImage(move,cout);
                  cout << " score = " << try_score << " - no cutoff, researching .." << endl;
                    indent(ply); cout << "window = [" << node->best_score << "," << hibound << "]" << endl;
               }
#endif
               if (extend >= -DEPTH_INCREMENT) {
                  hibound = node->beta;
               }
               if (extend < 0) {
                  extend = node->extensions = 0;
               }
               if (depth+extend-DEPTH_INCREMENT > 0)
                 try_score=-search(-hibound, -node->best_score,ply+1,depth+extend-DEPTH_INCREMENT);
               else
                 try_score=-quiesce(-hibound,-node->best_score,ply+1,0);
#ifdef _TRACE
               if (master()) {
                  indent(ply);
                  cout << ply << ". ";
                  MoveImage(move,cout);
                  cout << ' ' << try_score << endl;
               }
#endif
            }
            board.undoMove(move,state);
            if (terminate) {
                break;
            }
#ifdef _TRACE
            if (master()) {
                indent(ply);
                cout << ply << ". ";
                MoveImage(move,cout);
                cout << ' ' << try_score;
                if (first) cout << " (pv)";
                cout << endl;
            }
            first = 0;
#endif
            ASSERT(node->num_try<Constants::MaxMoves);
            node->done[node->num_try++] = move;
            ASSERT(node->num_try<Constants::MaxMoves);
            if (try_score > node->best_score) {
                if (updateMove(board,node,node,move,try_score,ply,depth)) {
                   // cutoff
                   break;
                }
            }
            if (try_score >= Constants::MATE-1-ply) {
                node->cutoff++;
                break;                            // mating move found
            }
            if (canSplit && maybeSplit(board, node, &mg, ply, depth)) {
                break;
            }
        }                                         // end move loop
#ifdef _TRACE
        if (node->best_score >= node->beta && master()) {
            indent(ply);
            cout << "**CUTOFF**" << endl;
        }
#endif
        if (terminate) {
            node->best_score = node->alpha;
            goto search_end2;
        }
        if (node->num_try == 0) {
            // no moves were tried
           if (node->flags & SINGULAR) {
              // Do not return mate or stalemate, because we have
              // a valid hash move. Return a fail low score.
              return node->alpha;
           } else {
#ifdef _DEBUG
              RootMoveGenerator rmg(board);
              ASSERT(rmg.moveCount() == 0);
#endif
              if (in_check) {
#ifdef _TRACE
                 if (master()) {
                    indent(ply); cout << "mate" << endl;
                 }
#endif
                 node->best_score = -(Constants::MATE - ply);
                 node->flags |= EXACT;
                 goto search_end2;
              }
              else {                                // stalemate
#ifdef _TRACE
                 if (master()) {
                    indent(ply); cout << "stalemate!" << endl;
                 }
#endif
                 node->best_score = drawScore(board);
                 node->flags |= EXACT;
                 return node->best_score;
              }
           }
        }
    }
    if (!IsNull(node->best) && !CaptureOrPromotion(node->best) &&
        board.checkStatus() != InCheck) {
        context.setKiller((const Move)node->best, node->ply);
        if (node->ply > 0) {
           context.setRefutation((node-1)->last_move,node->best);
        }
        context.updateStats(board,node,node->best,
            depth,
            board.sideToMove());
    }

    // don't insert into the hash table if we are terminating - we may
    // not have an accurate score.
 hash_insert:
    if (!terminate && !(node->flags & SINGULAR)) {
        if (IsNull(node->best)) node->best = hashMove;
        // store the position in the hash table, if there's room
        score_t value = node->best_score;
        HashEntry::ValueType val_type;
        // Adjust mate scores to reflect current ply. But only
        // if the score is in bounds.
        if (value > node->alpha && value < node->beta) {
            if (value <= -Constants::MATE_RANGE) {
                value -= ply;
            }
            else if (value >= Constants::MATE_RANGE) {
                value += ply;
            }
        }
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
        if (master()) {
            indent(ply);
            cout << "storing type=" << typeChar <<
                " ply=" << ply << " depth=" << depth << " value=" << value <<
                " move=";
            MoveImage(node->best,cout);
            cout << endl;
        }
#endif
        const hash_t hashCode = board.hashCode(rep_count);
        controller->hashTable.storeHash(hashCode, depth,
                                        controller->age,
                                        val_type,
                                        node->best_score,
                                        node->staticEval,
                                        (IsForced(node->best) ? HashEntry::FORCED_MASK : 0),
                                        node->best);
    }
    search_end2:
#ifdef MOVE_ORDER_STATS
    if (node->num_try && node->best_score != node->alpha) {
        controller->stats->move_order_count++;
        ASSERT(node->best_count>=0);
        if (node->best_count<4) {
            controller->stats->move_order[node->best_count]++;
        }
    }
#endif
    score_t score = node->best_score;
    ASSERT(score >= -Constants::MATE && score <= Constants::MATE);
    return score;
}

int Search::updateRootMove(const Board &board,
                           NodeInfo *parentNode, NodeInfo *node,
                           Move move, score_t score, int move_index)
{
   if (score > parentNode->best_score)  {
      parentNode->best = move;
      parentNode->best_score = score;
#ifdef MOVE_ORDER_STATS
      parentNode->best_count = node->num_try-1;
#endif
      if (score >= parentNode->beta) {
#ifdef _TRACE
         if (master())
             cout << "ply 0 beta cutoff" << endl;
#endif
         // set pv to this move so it is searched first the next time
         parentNode->pv[0] = move;
         parentNode->pv_length = 1;
         parentNode->cutoff++;
         parentNode->best_score = score;
         controller->updateStats(parentNode, controller->getIterationDepth(),
                            parentNode->best_score,
                            parentNode->alpha,parentNode->beta);
         if (controller->uci) {
            cout << "info score ";
            Scoring::printScoreUCI(score,cout);
            cout << " lowerbound" << endl;
         }
         return 1;  // signal cutoff
      }
      updatePV(board,parentNode,(node+1),move,0);
      controller->updateStats(parentNode, controller->getIterationDepth(),
                              parentNode->best_score,
                              parentNode->alpha,parentNode->beta);
      if (move_index>1) {
          // best move has changed, show new best move
          showStatus(board,move,score <= parentNode->alpha,score >= parentNode->beta,0);
      }
   }
   return 0;   // no cutoff
}

// Perform search in a separate thread. We have always searched
// at least one move before calling this.
void Search::searchSMP(ThreadInfo *ti)
{
    Move move;
    const int in_check = board.checkStatus() == InCheck;
    BoardState state(board.state);
    const int ply = node->ply;
    const int depth = node->depth;
    int moveIndex;
    // get node from which we were split
    NodeInfo *parentNode = split->splitNode;
    // Initialize top of our node stack from parent node
    node->alpha = parentNode->alpha;
    node->beta = parentNode->beta;
    node->best_score = parentNode->best_score;
    node->best = parentNode->best;
    node->last_move = parentNode->last_move;
    node->extensions = parentNode->extensions;
    node->eval = parentNode->eval;
    node->staticEval = parentNode->staticEval;
#ifdef MOVE_ORDER_STATS
    node->best_count = parentNode->best_count;
#endif
    node->ply = parentNode->ply;
    node->depth = parentNode->depth;
    node->cutoff = 0;

    score_t best_score = parentNode->best_score;
#ifdef _THREAD_TRACE
    log("searchSMP",ti->index);
#endif
    MoveGenerator *mg = ti->work->split->mg;
    bool fhr = false;
    ASSERT(split);
    while (!terminate && !split->failHigh) {
        move = in_check ?
           mg->nextEvasion(split,moveIndex) :
           mg->nextMove(split,moveIndex);
        if (IsNull(move)) break;
        if (IsUsed(move)) continue;
#ifdef SEARCH_STATS
        ++controller->stats->moves_searched;
#endif
        if (ply == 0) {
            fhr = false;
        }
#ifdef _TRACE
        if (master() || ply==0) {
            indent(ply); cout << "trying " << ply << ". ";
            MoveImage(move,cout); cout << endl;
        }
#endif
        ASSERT(Capture(move) != King);
        score_t try_score;

        node->extensions = 0;
        node->last_move = move;
        CheckStatusType in_check_after_move = board.wouldCheck(move);
        int extend = calcExtensions(board,node,parentNode,in_check_after_move,
                                    moveIndex,move);
        if (extend == PRUNE) {
#ifdef _TRACE
           if (master()) {
              indent(ply); cout << "fwd pruned." << endl;
           }
#endif
           continue;
        }
        board.doMove(move);
        if (!in_check && !board.wasLegal(move)) {
#ifdef _TRACE
           if (master()) {
               indent(ply); cout << "Illegal move!" << endl;
           }
#endif
           board.undoMove(move,state);
           continue;
        }
        setCheckStatus(board,in_check_after_move);
#ifdef _TRACE
        if (master()) {
            indent(ply); cout << "window [" << best_score <<
                "," << best_score+1 << "]" << endl;
        }
#endif
        if (depth+extend-DEPTH_INCREMENT > 0) {
            try_score = -search(-best_score-1, -best_score,
                ply+1, depth+extend-DEPTH_INCREMENT);
        }
        else {
            try_score = -quiesce(-best_score-1, -best_score,
                ply+1, 0);
        }
        if (try_score == Illegal) {
#ifdef _TRACE
            if (master()) {
                indent(ply); cout << "Illegal move" << endl;
            }
#endif
            board.undoMove(move,state);
            continue;
        }
#ifdef _TRACE
        if (master() || ply==0) {
            indent(ply); cout << ply << ". ";
            MoveImage(move,cout);
            cout << " " << try_score << endl;
        }
#endif
        if (terminate || split->failHigh) {
           board.undoMove(move,state);
           break;
        }
        score_t hibound = node->best_score+1;
        while (try_score > best_score &&
               (extend < 0 || hibound < node->beta) &&
               !((node+1)->flags & EXACT) &&
               !split->failHigh &&
               !terminate) {
            // We got a new best move but with a zero-width search or with
            // reduction enabled, so we must re-search.
#ifdef _TRACE
            if (master() || ply==0) {
                indent(ply); cout << ply << ". ";
                MoveImage(move,cout);
                cout << "score = " << try_score << " - no cutoff, researching";
                cout << " (smp ";
                cout << ti->index << ')' << endl;
            }
#endif
            if (extend >= -DEPTH_INCREMENT) {
               hibound = node->beta;
            }
            if (ply == 0) {
               fhr = true;
               root()->fail_high_root++;
            }
            if (extend < 0) {
               extend = node->extensions = 0;
            }
            if (depth+extend-DEPTH_INCREMENT > 0)
               try_score=-search(-hibound,-best_score,ply+1,depth+extend-DEPTH_INCREMENT);
            else
               try_score=-quiesce(-hibound,-best_score,ply+1,0);
        }
        board.undoMove(move,state);
        if (ply == 0 && controller->getIterationDepth()<=MoveGenerator::EASY_PLIES) {
            ((RootMoveGenerator*)mg)->setScore(move,try_score);
        }
#ifdef _TRACE
        if (master() || ply==0) {
            indent(ply);
            cout << ply << ". ";
            MoveImage(move,cout);
            cout << " " << try_score << endl;
        }
#endif
        if (!terminate) {
            split->lock();
            ASSERT(parentNode->num_try<Constants::MaxMoves);
            parentNode->done[parentNode->num_try++] = move;
            // update our window in case parent best score changed
            if (try_score > parentNode->best_score && !split->failHigh) {
                // search produced a new best move or cutoff, update parent node
#if defined (_THREAD_TRACE) || defined(_TRACE)
              stringstream s;
                s << "new best smp, thread=" << ti->index << " ply= " << ply << '\0';
#ifdef _TRACE
                cout << s.str() << endl;
#endif
#ifdef _THREAD_TRACE
                log(s.str());
#endif
#endif
                if (ply == 0) {
                    node->cutoff |= ((RootSearch*)(split->master->work))->updateRootMove(board,parentNode,node,move,try_score,parentNode->num_try);
                }
                else
                   node->cutoff |= updateMove(board,parentNode,node,move,try_score,ply,depth,split);
                best_score = try_score;
                if (fhr) {
                    fhr = false;
                    root()->fail_high_root = 0;
                }
            }
            if (node->cutoff || try_score >= Constants::MATE-1-ply) {
                split->failHigh++;
                parentNode->cutoff++;
                split->unlock();
                break;
            }
            split->unlock();
        }
        if (fhr) {
            fhr = false;
            root()->fail_high_root = 0;
        }
        if (ply == 0 && root()->getWaitTime()) {
            sleep(root()->getWaitTime());
        }
    }
    // this search is done, make this thread available for
    // other tasks
}

int Search::updateMove(const Board &board, NodeInfo *parentNode, NodeInfo *node, Move move, score_t score, int ply, int depth, SplitPoint *s)
{
   int cutoff = 0;
   parentNode->best_score = score;
   parentNode->best = move;
#ifdef MOVE_ORDER_STATS
   parentNode->best_count = parentNode->num_try-1;
#endif
   if (score >= parentNode->beta) {
#ifdef _TRACE
      if (master()) {
         indent(ply); cout << "beta cutoff" << endl;
      }
#endif
      parentNode->cutoff++;
      cutoff++;
   }
   else {
      parentNode->best_score = score;
      // update pv from slave node to master
      updatePV(board,parentNode,(node+1),move,ply);
   }
   return cutoff;
}

void Search::updatePV(const Board &board, Move m, int ply)
{
    updatePV(board,node,(node+1),m,ply);
#ifdef _DEBUG
    Board board_copy(board);
#ifdef _TRACE
    if (master()) {
        indent(ply); cout << "update_pv, ply " << ply << endl;
    }
#endif
    for (int i = ply; i < node->pv_length+ply; i++) {
       if (ply == 0) {
            MoveImage(node->pv[i],cout); cout << " " << (flush);
       }
#ifdef _TRACE
        if (master()) {
            MoveImage(node->pv[i],cout); cout << " " << (flush);
        }
#endif
        ASSERT(legalMove(board_copy,node->pv[i]));
        board_copy.doMove(node->pv[i]);
    }
#endif
#ifdef _TRACE
    if (master()) cout << endl;
#endif
}


void Search::updatePV(const Board &board,NodeInfo *node,NodeInfo *fromNode,Move move, int ply)
{
#ifdef _TRACE
    if (master()) {
        indent(ply); cout << "update_pv, ply " << ply << endl;
    }
#endif
    node->pv[ply] = move;
    if (fromNode->pv_length) {
        memcpy((void*)(node->pv+ply+1),(void*)(fromNode->pv+ply+1),
            sizeof(Move)*fromNode->pv_length);
    }
    node->pv_length = fromNode->pv_length+1;
#ifdef _DEBUG
    Board board_copy(board);
    for (int i = ply; i < node->pv_length+ply; i++) {
        ASSERT(i<Constants::MaxPly);
#ifdef _TRACE
        if (master()) {
            MoveImage(node->pv[i],cout); cout << " " << (flush);
        }
#endif
        ASSERT(legalMove(board_copy,node->pv[i]));
        board_copy.doMove(node->pv[i]);
    }
#endif
}

int Search::maybeSplit(const Board &board, NodeInfo *node,
                       MoveGenerator *mg, int ply, int depth)
{
    // Now that we have searched at least one valid move, we can
    // consider using multiple threads to search the rest (YBWC).
    int splits = 0;
    if (!terminate && mg->more() &&
        activeSplitPoints < SPLIT_STACK_MAX_DEPTH &&
#if defined(GAVIOTA_TBS) || defined(NALIMOV_TBS) || defined(SYZYGY_TBS)
        (!srcOpts.use_tablebases ||
        board.getMaterial(White).men() +
        board.getMaterial(Black).men() > EGTBMenCount) &&
#endif
        controller->pool->checkAvailable()) {

        ThreadInfo *slave_ti;
        ThreadInfo *slaves[Constants::MaxCPUs];
        int remaining = 100;
        // Keep trying to get/assign threads until no threads are
        // available or no more moves are available to search.
        while (remaining>1 && (slave_ti = controller->pool->checkOut(this,node,ply,depth))!=NULL) {
            // A thread is available.
            ASSERT(slave_ti != ti);
            if (!splits) {
               // Save the current split point if any (may be NULL)
               SplitPoint *parent = split;
               // We are about the change the parent's stack, so lock it
               Lock(splitLock);
               // create a split point for this search instance
               split = &splitStack[activeSplitPoints++];
#ifdef _THREAD_TRACE
               ostringstream os;
               os << "splitting from thread " << ti->index <<
                   " split stack size now " << activeSplitPoints << '\0';
               log(os.str());
#endif
               // initialize the new split point
               split->master = ti;
               split->parent = parent;
               split->ply = ply;
               split->depth = depth;
               split->mg = mg;
               split->splitNode = node;
               // save master's current state
               split->savedBoard = board;
#ifndef _WIN32
               // ensure parent thread will wait when back in idle loop
               ti->reset();
#endif
               Unlock(splitLock);
               // Force all remaining moves to be generated, since the
               // MoveGenerator class is not thread-safe otherwise (it
               // maintains a pointer to the board and when accessed by
               // multiple threads this pointer may not always be at the
               // current position). Also we want to know how many moves
               // remain.
               remaining = mg->generateAllMoves(node,split);
            }
            split->lock();
            ASSERT(slave_ti != ti);
            // Add new slave to the list of slaves in the parent split point
            split->slaves.insert(slave_ti);
            split->unlock();
            // set new slave Search's "split" variable
            slave_ti->work->split = split;
            // Defer further initialization of slave Search until its
            // thread runs, because we need access to its thread-specific
            // search stack.
#ifdef _THREAD_TRACE
            log("adding slave thread ", slave_ti->index);
#endif
            slaves[splits++] = slave_ti;
#ifdef SMP_STATS
            controller->stats->splits++;
#endif
#ifdef _THREAD_TRACE
            log("split ply",ply);
            log("depth",depth);
#endif
            --remaining;
        }
        // now start the slave threads (if we allocated any)
        if (splits) {
           Lock(splitLock);
           for (int i = 0; i < splits; i++) {
              // Start searching in the new thread:
              slaves[i]->start();
           }
           Unlock(splitLock);
        }
    }
    if (splits) {
        ASSERT(activeSplitPoints);
        // Go ahead and try to grab/search moves in this thread, in parallel
        // with its slaves:
        searchSMP(ti);
        // The master thread has completed its work. If any slave threads
        // remain, be a "helpful master" and make this thread available to
        // them. When all slave threads are done, the parent will be
        // signalled and exit the idle loop.
        //
        // Important to lock here - otherwise there is a race condition with
        // ThreadPool::checkIn.
        Lock(splitLock);
        split->lock();
#ifdef _THREAD_TRACE
        {
            ostringstream os;
            os << "search with master " << ti->index << " done, slaves count=" <<
                split->slaves.size() << " active=";
           for (auto it = split->slaves.begin();
                it != split->slaves.end();
                it++) {
               os << (*it)->index << ' ';
            }
            os << '\0';
            log(os.str());
        }
#endif
        if (split->slaves.size()) {
           ASSERT(split->slaves.count(ti) == 0);
#ifdef _DEBUG
           for (auto it = split->slaves.begin();
                it != split->slaves.end();
                it++) {
                // If idle, thread must be master of a split point.
                // If idle and not split, we will wait forever on it.
              ASSERT((*it)->state == ThreadInfo::Working || (*it)->work->activeSplitPoints);
            }
#endif
#ifdef _THREAD_TRACE
            log("helpful master entering idle_loop, thread #",ti->index);
#endif
            // ensure parent will wait when back in loop
            ti->reset();
#ifdef HELPFUL_MASTER
            SplitPoint *currentSplit = split;
            split->unlock();
            Unlock(splitLock);
            ThreadPool::idle_loop(ti, split);
#ifdef _THREAD_TRACE
            log("helpful master out of idle_loop, thread #",ti->index);
#endif
            Lock(splitLock);
            ASSERT(ti->state == ThreadInfo::Working);
            //ASSERT(split->slaves.size() == 0);
            // The master returns to whatever split point it was previously
            // working on.
            split = currentSplit;
            ASSERT(split->master == ti);
            Unlock(splitLock);
#else
            split->unlock();
            Unlock(splitLock);
            // wait to be signalled by last child thread exiting
            ti->wait();
#endif
        } else {
            // No additional work at this split point so no use being a helpful master
            split->unlock();
            Unlock(splitLock);
        }
        // Now all work at the split point is finished, so pop the stack
        Lock(splitLock);
        restoreFromSplit(split);
        --activeSplitPoints;
        split = split->parent;
#ifdef _THREAD_TRACE
        {
            std::ostringstream s;
            s << "split stack " << ti->index << " popped, active count now="
              << activeSplitPoints << '\0';
            log(s.str());
        }
#endif
        Unlock(splitLock);
    }
    return splits;
}


// Initialize a Search instance to prepare it for searching at a split point.
// This is called from the thread in which the search will execute.
void Search::init(NodeStack &ns, ThreadInfo *slave_ti) {
    SplitPoint *s = split;
    // copy in new state
    board = s->savedBoard;
    node = ns+s->ply;
    // The split variable holds the split point to which this Search
    // instance is attached
    split = s;
    split->failHigh = 0;
    nodeAccumulator = 0;
    ti = slave_ti;
    node->ply = s->ply;
    node->depth = s->depth;
    if (node->ply > 0) {
        // previous move, needed for counter move history 
        (node-1)->last_move = ((s->splitNode)-1)->last_move;
    }
    
    // Rest of new node initialization is done in searchSMP().
    // Clear killer since the side to move may have been different
    // in the previous use of this class.
    context.clearKiller();
}

void Search::clearHashTables() {
   scoring.clearHashTables();
   context.clear();
}

void Search::setSearchOptions() {
   srcOpts = options.search;
}

