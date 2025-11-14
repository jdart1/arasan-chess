// Copyright 2021-2025 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "boardio.h"
#include "binformat.h"
#include "chessio.h"
#include "eco.h"
#include "globals.h"
#include "movegen.h"
#include "notation.h"
#include "scoring.h"
#include "search.h"
#include <array>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#ifndef _MSC_VER
extern "C" {
#include <unistd.h>
}
#endif

static constexpr size_t LINUX_STACK_SIZE = 4 * 1024 * 1024;

using namespace std::placeholders;

// Utility to generate training positions from Arasan self-play games

static ECO ecoCoder;

class SelfPlayHashTable {

  public:
    SelfPlayHashTable() : used_hashes(new HashArray()) { init_hash(); }

    virtual ~SelfPlayHashTable() { delete used_hashes; }

    bool check_and_replace_hash(hash_t new_hash) {
        size_t index = new_hash & (HASH_TABLE_FOR_UNIQUENESS_SIZE - 1);
        std::unique_lock<std::mutex> lock(mtx);
        hash_t &h = (*used_hashes)[index];
        if (h == new_hash) {
            return true;
        } else {
            h = new_hash;
            return false;
        }
    }

  private:
    static constexpr size_t HASH_TABLE_FOR_UNIQUENESS_SIZE = 1ULL << 26; // entries

    using HashArray = std::array<hash_t, HASH_TABLE_FOR_UNIQUENESS_SIZE>;

    HashArray *used_hashes;

    std::mutex mtx;

    void init_hash() {
        for (hash_t &h : *used_hashes)
            h = 0ULL;
    }

} sp_hash_table;

std::mutex outputLock, bookLock;

static std::ofstream *game_out_file = nullptr, *pos_out_file = nullptr;

static std::atomic<unsigned> posCounter(0), wins(0), losses(0), draws(0);

static constexpr int MAX_MULTIPV = 10;

static struct SelfPlayOptions {
    // Note: not all options are command-line settable, currently.
    enum class RandomizeType { Nodes, MultiPV };
    unsigned minOutPly = 16;
    unsigned maxOutPly = 300;
    unsigned cores = 1;
    unsigned posCount = 10000000;
    unsigned depthLimit = 9;
    bool adjudicateDraw = true;
    bool adjudicateTB = true;
    int adjudicateTBMen = 3;
    int adjudicateTBMenPawnless = 5;
    score_t adjudicateWinScore = 20*Scoring::PAWN_VALUE;
    unsigned adjudicateWinPlies = 3;
    unsigned outputPlyFrequency = 1; // output every nth move
    unsigned drawAdjudicationPlies = 10;
    unsigned TBAdjudicationPlies = 4;
    unsigned drawAdjudicationMinPly = 150;
    int adjudicateMinMove50Count = 40; // mininum move 50 count
    std::string posFileName;
    std::string gameFileName = "games.pgn";
    bool saveGames = false;
    unsigned maxBookPly = 0;
    bool randomize = true;
    unsigned randomizeRange = 2;
    unsigned randomizeInterval = 1;
    int randomTolerance = 0.5*Scoring::PAWN_VALUE;
    bool limitEarlyKingMoves = true;
    bool semiRandomize = true;
    RandomizeType randomizeType = RandomizeType::MultiPV;
    unsigned semiRandomizeInterval = 1;
    unsigned semiRandomPerGame = 14;
    unsigned multipv_limit = 8;
    int multiPVMargin = static_cast<int>(0.3 * Scoring::PAWN_VALUE);
    bool skipNonQuiet = true;
    BinFormats::Format format = BinFormats::Format::StockfishBin;
    bool verbose = false;
    unsigned verboseReportingInterval = 1000000;
    bool checkHash = false; // use hash table to check for possible dups

    bool adjudicateTBOk(const Board &board) const noexcept {
        if (adjudicateTB) {
            return (board.men() <= std::min<int>(adjudicateTBMen,globals::EGTBMenCount)) ||
                (!board.hasPawns() && board.men() <= std::min<int>(adjudicateTBMenPawnless,globals::EGTBMenCount));
        }
        else {
            return false;
        }
    }
} sp_options;

struct ThreadData {
    unsigned index = 0;
    std::thread thread;
    SearchController *searcher = nullptr;
    MoveArray gameMoves;
    std::mt19937_64 engine;
} threadDatas[Constants::MaxCPUs];

static void saveGame(ThreadData &td, const std::string &result, std::ofstream &file) {
    std::vector<ChessIO::Header> headers;
    headers.push_back(ChessIO::Header("Event", "?"));
    char hostname[256];
#ifdef _MSC_VER
    DWORD size = 256;
    GetComputerName(hostname, &size);
    hostname[size] = '\0';
#else
    gethostname(hostname, 256);
#endif
    if (*hostname) {
        headers.push_back(ChessIO::Header("Site", hostname));
    } else {
        headers.push_back(ChessIO::Header("Site", "?"));
    }
    std::time_t tm = std::time(NULL);
    std::tm *t = std::localtime(&tm);
    std::stringstream dateStr;
    dateStr << std::put_time(t, "%Y.%m.%d");
    headers.push_back(ChessIO::Header("Date", dateStr.str()));
    headers.push_back(ChessIO::Header("Round", "?"));
    std::stringstream s;
    s << "Arasan " << Arasan_Version;
    headers.push_back(ChessIO::Header("White", s.str()));
    headers.push_back(ChessIO::Header("Black", s.str()));
    std::string opening_name, eco;
    ecoCoder.classify(td.gameMoves, eco, opening_name);
    headers.push_back(ChessIO::Header("Result", result));
    headers.push_back(ChessIO::Header("ECO", eco));
    {
        std::unique_lock<std::mutex> lock(outputLock);
        ChessIO::store_pgn(file, td.gameMoves, result, headers);
    }
}

struct MoveResult {
    MoveResult() : move(NullMove), score(0) {}
    MoveResult(const Move &m, int s): move(m), score(s) {}
    Move move;
    int score;
};

template <size_t size>
static Move pick(const std::array<Move,size> &moves, unsigned count, ThreadData &td) {
    assert(count);
    std::uniform_int_distribution<unsigned> dist(0, count - 1);
    int select = dist(td.engine);
    return moves[select];
}

static MoveResult pick(const MoveResult *moves, unsigned count, ThreadData &td) {
    assert(count);
    std::uniform_int_distribution<unsigned> dist(0, count - 1);
    int select = dist(td.engine);
    return moves[select];
}

static const Bitboard center(1ULL<<chess::D4 | 1ULL<<chess::E4 | 1ULL<<chess::D5 | 1ULL<<chess::E5);

static Bitboard centerAttacks(const Board &board, Move m) {
    Square start = StartSquare(m);
    Square dest = DestSquare(m);
    assert(board[start] != EmptyPiece);
    ColorType c = PieceColor(board[start]);
    switch (PieceMoved(m)) {
    case Pawn:
        return Attacks::pawn_attacks[dest][c] & center;
    case Knight:
        return Attacks::knight_attacks[dest] & center;
    case Bishop:
        return Attacks::bishopAttacks(dest,center);
    case Rook:
        return Attacks::rookAttacks(dest,center);
    case Queen:
        return Attacks::queenAttacks(dest,center);
    case King:
    default:
        return Bitboard(0);
    }
}

static Move randomMove(Board &board, Statistics &stats, ThreadData &td) {
    RootMoveGenerator mg(board);
    if (mg.moveCount() == 0) {
        if (board.isLegalDraw()) {
            stats.state = Draw;
            stats.display_value = 0;
        } else if (board.checkStatus() == InCheck) {
            stats.state = Checkmate;
            stats.display_value = -Constants::MATE;
        } else {
            stats.state = Stalemate;
            stats.display_value = 0;
        }
        return NullMove;
    }
    std::array<Move, Constants::MaxMoves*2> candidates;
    const bool inCheck = board.checkStatus() == InCheck;
    unsigned i = 0;
    RootMoveGenerator::RootMoveList rmg;
    rmg.assign(mg.getMoveList().begin(), mg.getMoveList().end());
    // collect moves that attack center
    auto new_end =
        std::remove_if(rmg.begin(), rmg.end(), [&](const RootMoveGenerator::RootMove &r) -> bool {
            BoardState state(board.state);
            board.doMove(r.move);
            int rep = board.repCount();
            board.undoMove(r.move, state);
            // avoid repetitions, losing moves and early king moves
            return IsCastling(r.move) ? false : (seeSign(board, r.move, -sp_options.randomTolerance) == 0 || rep > 0 ||
                    (!inCheck && sp_options.limitEarlyKingMoves &&
                     PieceMoved(r.move) == King));
        });
    for (auto it = rmg.begin(); it != new_end; it++) {
        Move m = (*it).move;
        candidates[i++] = m;
        if (centerAttacks(board,m)) {
            candidates[i++] = m;
            candidates[i++] = m;
            candidates[i++] = m;
        }
    }
    if (i) {
        return pick<Constants::MaxMoves*2>(candidates, i, td);
    } else { // no moves match criteria
        return NullMove;
    }
}

class Monitor {
  public:
    Monitor(uint64_t limit) : nodeTarget(limit) {}
    virtual ~Monitor() = default;
    int nodeMonitor(SearchController *, const Statistics &stats) {
        return (stats.completedDepth > 0 && stats.num_nodes >= nodeTarget);
    }

  private:
    uint64_t nodeTarget;
};

// Somewhat randomize moves by either doing a multi-PV search or
// doing a fixed-node instead of fixed-depth search.
static void semiRandomMove(const Board &board, SelfPlayOptions::RandomizeType type, ThreadData &td,
                           uint64_t prevNodes, StateType &state, MoveResult &mr,
                           int &numCandidates) {
    Statistics stats;
    state = NormalState;
    if (type == SelfPlayOptions::RandomizeType::Nodes) {
        // TBD: this doesn't work reliably
        // Base the target node count on the node count for the
        // previous search, but randomize it somewhat.
        std::uniform_int_distribution<uint64_t> node_dist(uint64_t(0.7 * prevNodes),
                                                          uint64_t(1.3 * prevNodes));
        Monitor monitor(node_dist(td.engine));
        auto oldMonitor = td.searcher->registerMonitorFunction(
            std::bind(&Monitor::nodeMonitor, &monitor, _1, _2));
        mr.move = td.searcher->findBestMove(board, FixedDepth, Constants::INFINITE_TIME,
                                            0, // extra time
                                            sp_options.depthLimit + 2,
                                            false, // background
                                            false, // uci
                                            stats, TalkLevel::Silent);
        td.searcher->registerMonitorFunction(oldMonitor);
        mr.score = stats.display_value;
    } else { // multiPV
        numCandidates = 0;
        td.searcher->setMultiPV(sp_options.multipv_limit);
        (void) td.searcher->findBestMove(
            board, FixedDepth, Constants::INFINITE_TIME, 0, sp_options.depthLimit,
            false, false, stats, TalkLevel::Silent);
        td.searcher->setMultiPV(1);
        if (stats.multipv_count == 0) {
            // no moves, could be checkmate/stalemate
            state = stats.state;
            mr = MoveResult();
            return;
        }
        // results are ranked in descending order by score
        int maxScore = stats.multi_pvs[0].display_value;
        MoveResult candidates[MAX_MULTIPV];
        for (size_t i = 0; i < stats.multipv_count; ++i) {
            const Statistics::MultiPVEntry &entry = stats.multi_pvs[i];
            if (!IsNull(entry.best) && (maxScore - entry.display_value) <= sp_options.multiPVMargin) {
                candidates[numCandidates++] = MoveResult(entry.best,entry.display_value);
            }
        }
        mr = numCandidates ? pick(candidates, numCandidates, td) : MoveResult();
    }
}

static void selfplay(ThreadData &td) {
    SearchController *searcher = td.searcher;
    std::uniform_int_distribution<unsigned> dist(1, sp_options.outputPlyFrequency);
    std::uniform_int_distribution<unsigned> rand_dist(1, sp_options.randomizeInterval);
    auto startTime = getCurrentTime();
    while (posCounter < sp_options.posCount) {
        if (sp_options.saveGames) {
            td.gameMoves.reset();
        }
        bool adjudicated = false, terminated = false;
        Statistics stats;
        Board board;
        unsigned low_score_count = 0, tb_score_count = 0, high_score_count = 0;
        enum class Result { WhiteWin, BlackWin, Draw, Unknown } result = Result::Unknown;
        std::vector<BinFormats::PositionData> output;
        uint64_t prevNodes = 0ULL;
        int prevScore = 0;
        unsigned noSemiRandom = 0, semiRandomCount = 0, prevDepth = 0;
        unsigned bookMoves = sp_options.maxBookPly;
        for (unsigned ply = 0; ply <= sp_options.maxOutPly && !adjudicated && !terminated; ++ply) {
            stats.clear();
            Move m = NullMove;
            if (sp_options.maxBookPly && ply < sp_options.maxBookPly) {
                std::unique_lock<std::mutex> lock(bookLock);
                m = globals::openingBook.pick(board, false);
                if (IsNull(m)) {
                    bookMoves = ply;
                }
            }
            score_t score = 0;
            if (IsNull(m)) {
                // Don't randomize if in TB range or if score very large
                bool skipRandom = (int(board.men()) <= globals::EGTBMenCount) ||
                                  (std::abs(prevScore) >= 10 * Scoring::PAWN_VALUE);
                if (sp_options.randomize &&
                    (ply >= bookMoves) &&
                    (ply < bookMoves + sp_options.randomizeRange) &&
                    rand_dist(td.engine) == sp_options.randomizeInterval) {
                    m = randomMove(board, stats, td);
                    // TBD: we don't associate any prior FENS with the current
                    // game result after a random move. Stockfish though does do
                    // this.
                    output.clear();
                    // We have no score from a random move, so reset the
                    // adjudication counter
                    low_score_count = 0;
                } else if (sp_options.semiRandomize && !skipRandom &&
                           (ply > bookMoves + sp_options.randomizeRange) &&
                           (semiRandomCount == 0 || noSemiRandom >= sp_options.semiRandomizeInterval) &&
                           (semiRandomCount < sp_options.semiRandomPerGame) &&
                           (int(board.men()) > globals::EGTBMenCount) &&
                           ((sp_options.randomizeType != SelfPlayOptions::RandomizeType::Nodes) ||
                            (prevNodes && prevDepth >= sp_options.depthLimit))) {
                    int candCount = 0;
                    MoveResult mr;
                    StateType state;
                    semiRandomMove(board, sp_options.randomizeType, td, prevNodes, state, mr,
                                   candCount);
                    m = mr.move;
                    stats.state = state;
                    stats.display_value = score = mr.score;
                    prevScore = score;
                    if (score == 0)
                        ++low_score_count;
                    else
                        low_score_count = 0;
                    if (sp_options.randomizeType == SelfPlayOptions::RandomizeType::Nodes ||
                        candCount > 1) {
                        noSemiRandom = 0;
                        ++semiRandomCount;
                    }
                } else {
                    m = searcher->findBestMove(board, FixedDepth, Constants::INFINITE_TIME,
                                               0, // extra time
                                               sp_options.depthLimit,
                                               false, // background
                                               false, // uci
                                               stats, TalkLevel::Silent);
                    score = stats.display_value;
                    prevNodes = stats.num_nodes;
                    prevDepth = stats.depth;
                    if (score <= 1 && score >= -1)
                        ++low_score_count;
                    else
                        low_score_count = 0;
                    ++noSemiRandom;
                    if (score >= sp_options.adjudicateWinScore)
                        ++high_score_count;
                    else
                        high_score_count = 0;
                    // adjudicate wins but not for low material - TB adjudication
                    // rules will apply to thoose
                    if (high_score_count >= sp_options.adjudicateWinPlies &&
                        (!sp_options.adjudicateTB || board.men() > 7)) {
                        adjudicated = true;
                    }
                }
                assert(IsNull(m) || legalMove(board, m));
                assert(!IsNull(m) || stats.state != NormalState);
            }
            if (stats.state == Resigns || stats.state == Checkmate) {
                result = board.sideToMove() == White ? Result::BlackWin : Result::WhiteWin;
                terminated = true;
            } else if (stats.state == Draw || stats.state == Stalemate) {
                terminated = true;
                result = Result::Draw;
            } else if (sp_options.adjudicateTBOk(board) &&
                       ++tb_score_count >= sp_options.TBAdjudicationPlies) {
                if (stats.display_value >= Constants::TABLEBASE_WIN) {
                    result =
                        (board.sideToMove() == White) ? Result::WhiteWin : Result::BlackWin;
                } else if (stats.display_value <= -Constants::TABLEBASE_WIN) {
                    result =
                        (board.sideToMove() == White) ? Result::BlackWin : Result::WhiteWin;
                } else {
                    // draw, blessed loss, or cursed win
                    result = Result::Draw;
                }
                adjudicated = true;
            } else if (board.state.moveCount >= sp_options.adjudicateMinMove50Count) {
                if (sp_options.adjudicateDraw &&
                    ply >= sp_options.drawAdjudicationMinPly &&
                    low_score_count >= sp_options.drawAdjudicationPlies) {
                    // adjudicate draw
                    stats.state = Draw;
                    result = Result::Draw;
                    adjudicated = true;
                }
            }
            if (IsNull(m)) {
                break;
            } else {
                if (!validMove(board,m)) {
                    std::cerr << "warning: invalid move from search: ";
                    MoveImage(m,std::cerr);
                    std::cerr << std::endl;
                    break;
                }
                std::string image;
                if (sp_options.saveGames) {
                    Notation::image(board, m, Notation::OutputFormat::SAN, image);
                }
                if (!(sp_options.skipNonQuiet && (CaptureOrPromotion(m) || board.checkStatus() == InCheck)) &&
                    ply >= sp_options.minOutPly &&
                    (ply > sp_options.maxBookPly + sp_options.randomizeRange) &&
                    !board.repCount(1) &&
                    (!sp_options.checkHash ||
                     !sp_hash_table.check_and_replace_hash(board.hashCode()))) {
                    if ((dist(td.engine) % sp_options.outputPlyFrequency) == 0) {
                        std::stringstream s;
                        BoardIO::writeFEN(board, s, 0);
                        BinFormats::PositionData data;
                        data.fen = s.str();
                        data.score = score; // stm POV
                        data.ply = ply;
                        data.move = m;
                        data.stm = board.sideToMove();
                        data.move50Count = board.state.moveCount;
                        output.push_back(data);
                    }
                }
                board.doMove(m);
                if (sp_options.saveGames) {
                    td.gameMoves.add_move(board, m, image);
                }
            }
        }
        if (result == Result::Unknown) {
            continue;
        }
        static const std::string resultStrs[] = {"1-0", "0-1", "1/2-1/2"};
        int i = 0;
        if (result == Result::WhiteWin) {
            ++wins;
        } else if (result == Result::BlackWin) {
            i = 1;
            ++losses;
        } else {
            i = 2;
            ++draws;
        }
        if (sp_options.saveGames) {
            saveGame(td, resultStrs[i], *game_out_file);
        }
        for (const BinFormats::PositionData &data : output) {
            if (posCounter++ >= sp_options.posCount)
                break;
            if (posCounter % sp_options.verboseReportingInterval == 0) {
                auto elapsedTime = getElapsedTime(startTime, getCurrentTime()) / 1000;
                std::ios_base::fmtflags original_flags = std::cout.flags();
                std::cout << std::setprecision(2) << posCounter << " positions ("
                          << (posCounter * 100.0) / sp_options.posCount << "% done),"
                          << " elapsed time: " << elapsedTime
                          << " sec., positions/second: " << posCounter / elapsedTime << std::flush
                          << std::endl;
                std::cout.flags(original_flags);
            }
            int resultVal = 0; // result from White POV
            if (result == Result::WhiteWin) {
                resultVal = 1;
            }
            else if (result == Result::BlackWin) {
                resultVal = -1;
            }
            std::unique_lock<std::mutex> lock(outputLock);
            bool writeResult = false;
            switch(sp_options.format) {
            case BinFormats::Format::StockfishBin:
                writeResult = BinFormats::write<BinFormats::Format::StockfishBin>(data, resultVal,
                                                                       *pos_out_file);
                break;
            case BinFormats::Format::Marlin:
                writeResult = BinFormats::write<BinFormats::Format::Marlin>(data, resultVal,
                                                                       *pos_out_file);
                break;
            case BinFormats::Format::Bullet:
                writeResult = BinFormats::write<BinFormats::Format::Bullet>(data, resultVal,
                                                                       *pos_out_file);
                break;
            case BinFormats::Format::Text:
                writeResult = BinFormats::write<BinFormats::Format::Text>(data, resultVal,
                                                                       *pos_out_file);
                break;
            default:
                break;
            }
            if (!writeResult) {
                std::cerr << "write error" << std::endl;
                break;
            }
        }
    }
}

static void threadp(ThreadData *td) {
    // allocate controller in the thread
    try {
        td->searcher = new SearchController();
    } catch (std::bad_alloc &ex) {
        std::cerr << "out of memory, thread " << td->index << std::endl;
        return;
    }
    selfplay(*td);
    delete td->searcher;
}

static void usage() {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "selfplay [-a (append)] [-d depth] [-v (verbose)] [-c cores] [-n positions]" << std::endl;
    std::cerr << "         [-m output every m positions] [-f output format] [-g "
                 "filename (save games)]"
              << std::endl;
}

static void init_threads() {
    // prepare threads
#ifndef _WIN32
    pthread_attr_t stackSizeAttrib;
    if (pthread_attr_init(&stackSizeAttrib)) {
        perror("pthread_attr_init");
        return;
    }
    // set stack size for Linux/Mac
    size_t stackSize;
    if (pthread_attr_getstacksize(&stackSizeAttrib, &stackSize)) {
        perror("pthread_attr_getstacksize");
        return;
    }
    if (stackSize < LINUX_STACK_SIZE) {
        if (pthread_attr_setstacksize(&stackSizeAttrib, LINUX_STACK_SIZE)) {
            perror("error setting thread stack size");
            return;
        }
    }
#endif
    uint64_t seed = getRandomSeed();
    for (unsigned i = 0; i < sp_options.cores; i++) {
        threadDatas[i].index = i;
        threadDatas[i].searcher = nullptr;
        threadDatas[i].engine.seed(seed + i * 4000000000L);
    }
}

static void launch_threads() {
    for (unsigned i = 0; i < sp_options.cores; i++) {
        threadDatas[i].index = i;
        threadDatas[i].thread = std::thread(threadp, &threadDatas[i]);
    }
    // wait for all searchers done
    for (unsigned i = 0; i < sp_options.cores; i++) {
        threadDatas[i].thread.join();
    }
}

int CDECL main(int argc, char **argv) {
    BitUtils::init();
    Board::init();
    globals::initOptions();
    Attacks::init();
    Search::init();

    if (!globals::initGlobals()) {
        globals::cleanupGlobals();
        exit(-1);
    }
    atexit(globals::cleanupGlobals);
    globals::delayedInit();
    if (!globals::EGTBMenCount) {
        std::cerr << "warning: no tablebases found." << std::endl;
    }

    // set after init so .rc values are overridden
    globals::options.search.hash_table_size = 128 * 1024 * 1024;
    globals::options.book.book_enabled = true;
    globals::options.book.variety = 75;
    globals::options.learning.position_learning = false;
    globals::options.search.can_resign = false; // adjudication options are used instead
    globals::options.search.ponder = false;
    globals::options.search.widePlies = 2;
    globals::options.search.wideWindow = 3 * Scoring::PAWN_VALUE;

    // update book to use new variety setting
    globals::openingBook.setVariety(globals::options.book.variety);
    // re-init book if needed
    globals::delayedInit();

    int arg = 1;
    bool append = false;
    for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
        if (strcmp(argv[arg], "-a") == 0) {
            append = true;
        } else if (strcmp(argv[arg], "-b") == 0) {
            std::stringstream s(argv[++arg]);
            s >> sp_options.maxBookPly;
            if (s.bad()) {
                std::cerr << "error in ply count after -b" << std::endl;
                return -1;
            }
        } else if (strcmp(argv[arg], "-r") == 0) {
            std::stringstream s(argv[++arg]);
            s >> sp_options.semiRandomizeInterval;
            s >> sp_options.semiRandomPerGame;
            if (s.bad()) {
                std::cerr << "expected two numbers after -r" << std::endl;
                return -1;
            }
            sp_options.semiRandomize = sp_options.semiRandomPerGame > 0;
        } else if (strcmp(argv[arg], "-c") == 0) {
            std::stringstream s(argv[++arg]);
            s >> sp_options.cores;
            if (s.bad()) {
                std::cerr << "error in core count after -c" << std::endl;
                return -1;
            }
            sp_options.cores = std::min<int>(Constants::MaxCPUs, sp_options.cores);

        } else if (strcmp(argv[arg], "-n") == 0) {
            std::stringstream s(argv[++arg]);
            s >> sp_options.posCount;
            if (s.bad()) {
                std::cerr << "error in position count after -n" << std::endl;
                return -1;
            }
        } else if (strcmp(argv[arg], "-d") == 0) {
            std::stringstream s(argv[++arg]);
            s >> sp_options.depthLimit;
            if (s.bad()) {
                std::cerr << "error in depth limit after -d" << std::endl;
                return -1;
            }
        } else if (strcmp(argv[arg], "-f") == 0) {
            if (arg + 1 >= argc) {
                std::cerr << "expected format name after -f" << std::endl;
                return -1;
            }
            std::string fmt(argv[++arg]);
            if (!BinFormats::fromString(fmt,sp_options.format)) {
                std::cerr << "unrecognized format after -f" << std::endl;
                return -1;
            }
        } else if (strcmp(argv[arg], "-g") == 0) {
            if (arg + 1 >= argc) {
                std::cerr << "expected file name after -g" << std::endl;
                return -1;
            }
            sp_options.gameFileName = argv[++arg];
            sp_options.saveGames = true;
        } else if (strcmp(argv[arg], "-h") == 0) {
            sp_options.checkHash = true;
        } else if (strcmp(argv[arg], "-o") == 0) {
            if (arg + 1 >= argc) {
                std::cerr << "expected file name after -o" << std::endl;
                return -1;
            }
            sp_options.posFileName = argv[++arg];
        } else if (strcmp(argv[arg], "-m") == 0) {
            if (arg + 1 >= argc) {
                std::cerr << "expected number after -m" << std::endl;
                return -1;
            }
            std::stringstream s(argv[++arg]);
            s >> sp_options.outputPlyFrequency;
            if (s.bad()) {
                std::cerr << "error: expected number after -m" << std::endl;
                return -1;
            }
        } else if (strcmp(argv[arg], "-v") == 0) {
            sp_options.verbose = true;
        } else {
            usage();
            return -1;
        }
    }

    if (sp_options.posFileName == "") {
        sp_options.posFileName = (sp_options.format == BinFormats::Format::StockfishBin)
                                     ? "positions.bin"
            : "positions." + BinFormats::toString(sp_options.format);
    }

    std::ios_base::openmode flags = std::ios::out | std::ios::binary;
    if (append) {
        flags = flags | std::ios::app;
    }
    pos_out_file = new std::ofstream(sp_options.posFileName, flags);
    if (pos_out_file->fail()) {
        std::cerr << "failed to open position output file " << sp_options.posFileName << std::endl;
        exit(-1);
    }
    if (sp_options.saveGames) {
        game_out_file = new std::ofstream(sp_options.gameFileName, std::ios::out);
        if (game_out_file->fail()) {
            std::cerr << "failed to open game output file " << sp_options.gameFileName << std::endl;
            exit(-1);
        }
    }
    if (sp_options.maxBookPly > 0 && !globals::openingBook.is_open()) {
        sp_options.maxBookPly = 0;
    }

    init_threads();
    launch_threads();

    std::cout << "White wins: " << wins << " Black wins: " << losses << " Draws: " << draws <<
        " draw percentage: " << std::setprecision(4) << draws*100.0/(draws + losses + wins) << "%" << std::endl;

    delete pos_out_file;
    delete game_out_file;

    return 0;
}
