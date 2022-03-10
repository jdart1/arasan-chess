// Copyright 2021-2022 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "boardio.h"
#include "chessio.h"
#include "eco.h"
#include "globals.h"
#include "movegen.h"
#include "notation.h"
#include "scoring.h"
#include "search.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
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

using namespace std::placeholders;

// Utility to generate training positions from Arasan self-play games

#ifndef _WIN32
static pthread_attr_t stackSizeAttrib;
#endif

static ECO ecoCoder;

static const char *RESULT_TAG = "c2";

class SelfPlayHashTable {

  public:
    SelfPlayHashTable() : used_hashes(new HashArray()) { init_hash(); }

    virtual ~SelfPlayHashTable() { delete used_hashes; }

    bool check_and_replace_hash(hash_t new_hash) {
        size_t index = new_hash & (HASH_TABLE_FOR_UNIQUENESS_SIZE - 1);
        hash_t &h = (*used_hashes)[index];
        std::unique_lock<std::mutex> lock(mtx);
        if (h == new_hash) {
            return true;
        } else {
            h = new_hash;
            return false;
        }
    }

  private:
    static constexpr size_t HASH_TABLE_FOR_UNIQUENESS_SIZE = 1ULL << 24; // entries

    using HashArray = std::array<hash_t, HASH_TABLE_FOR_UNIQUENESS_SIZE>;

    HashArray *used_hashes;

    std::mutex mtx;

    void init_hash() {
        for (hash_t &h : *used_hashes)
            h = 0ULL;
    }

} sp_hash_table;

struct OutputData {
    std::string fen;
    score_t score;
    unsigned ply;
    unsigned move50Count;
    ColorType stm;
    Move move;
};

std::mutex outputLock, bookLock;

static std::ofstream *game_out_file = nullptr, *pos_out_file = nullptr;

static std::atomic<unsigned> posCounter(0);

static struct SelfPlayOptions {
    // Note: not all options are command-line settable, currently.
    enum class OutputFormat { Epd, Bin };
    unsigned minOutPly = 8;
    unsigned maxOutPly = 400;
    unsigned cores = 1;
    unsigned posCount = 10000000;
    unsigned depthLimit = 6;
    bool adjudicateDraw = true;
    unsigned outputPlyFrequency = 1; // output every nth move
    unsigned drawAdjudicationMoves = 5;
    unsigned drawAdjudicationMinPly = 100;
    std::string posFileName;
    std::string gameFileName = "games.pgn";
    bool saveGames = false;
    unsigned maxBookPly = 0;
    bool randomize = true;
    unsigned randomizeRange = 10;
    unsigned randomizeInterval = 1;
    bool useSee = true;
    bool limitEarlyKingMoves = true;
    bool semiRandomize = true;
    unsigned semiRandomizeInterval = 15;
    bool skipNonQuiet = true;
    OutputFormat format = OutputFormat::Bin;
} sp_options;

struct ThreadData {
    unsigned index = 0;
    std::thread thread;
    SearchController *searcher = nullptr;
    MoveArray gameMoves;
    std::mt19937 engine;
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
    char dateStr[15];
    time_t tm = time(NULL);
    struct tm *t = localtime(&tm);
    sprintf(dateStr, "%4d.%02d.%02d", t->tm_year + 1900, t->tm_mon + 1,
            t->tm_mday);
    headers.push_back(ChessIO::Header("Date", dateStr));
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

class binEncoder {

    using PosOutputType = std::array<uint8_t, 32>;

  public:
    static void output(const OutputData &data, int result, std::ostream &out) {
        PosOutputType posData;
        unsigned pos = 0;
        posData.fill(0);
        pos = 0; // bit count
        Board board;
        BoardIO::readFEN(board, data.fen);
        pos = encode_bit(posData, board.sideToMove() == Black, pos);
        pos = encode_bits(posData, board.kingSquare(White), 6, pos);
        pos = encode_bits(posData, board.kingSquare(Black), 6, pos);
        // Encode board (minus Kings)
        pos = encode_board(posData, board, pos);
        CastleType wcs = board.castleStatus(White);
        CastleType bcs = board.castleStatus(Black);
        pos = encode_bit(
            posData, wcs == CanCastleKSide || wcs == CanCastleEitherSide, pos);
        pos = encode_bit(
            posData, wcs == CanCastleQSide || wcs == CanCastleEitherSide, pos);
        pos = encode_bit(
            posData, bcs == CanCastleKSide || bcs == CanCastleEitherSide, pos);
        pos = encode_bit(
            posData, bcs == CanCastleQSide || bcs == CanCastleEitherSide, pos);
        Square epsq = board.enPassantSq();
        if (epsq == InvalidSquare) {
            ++pos;
        } else {
            // Arasan's internal en passant square is not what Stockfish/FEN
            // expect, correct here:
            Square target = (board.sideToMove() == White) ? epsq + 8 : epsq - 8;
            pos = encode_bit(posData, 1, pos);
            pos = encode_bits(posData, target, 6, pos);
        }
        // 6 bits for Stockfish compatibility:
        pos = encode_bits(posData, data.move50Count, 6, pos);
        // fullmove counter
        pos = encode_bits(posData, 2 * (data.ply / 2), 8, pos);
        // next 8 bits of fullmove counter
        pos = encode_bits(posData, (2 * (data.ply / 2)) >> 8, 8, pos);
        // 7th bit of 50-move counter
        pos = encode_bit(posData, data.move50Count >> 6, pos);
        assert(pos <= 256);
        // output position
        out.write(reinterpret_cast<const char *>(posData.data()), 32);
        // score. Note: Arasan scores are always from side to moves's POV.
        serialize<int16_t>(out, static_cast<int16_t>((data.score)));
        serialize<uint16_t>(out, encode_move(board.sideToMove(), data.move));
        serialize<uint16_t>(out, static_cast<uint16_t>(data.ply));
        serialize<int8_t>(out, static_cast<int8_t>(result));
        serialize<uint8_t>(out, static_cast<uint8_t>(0xff));
        assert(out.tellp() % 40 == 0);
    }

  private:
    static inline unsigned encode_bit(PosOutputType &out, unsigned bit,
                                      unsigned pos) {
        if (bit) {
            out[pos / 8] |= (1 << (pos % 8));
        }
        return pos + 1;
    }

    static inline unsigned encode_bits(PosOutputType &out, unsigned bits,
                                       unsigned size, unsigned pos) {
        for (unsigned i = 0; i < size; i++) {
            pos = encode_bit(out, bits & (1 << i), pos);
        }
        return pos;
    }

    static unsigned encode_board(PosOutputType &out, const Board &b,
                                 unsigned pos) {
        static const std::array<unsigned, 6> HUFFMAN_TABLE = {0, 1, 3, 5, 7, 9};
        for (unsigned rank = 0; rank < 8; ++rank) {
            for (unsigned file = 0; file < 8; ++file) {
                Square sq = 56 - rank * 8 + file;
                const Piece &p = b[sq];
                if (IsEmptyPiece(p)) {
                    ++pos; // 1 bit
                } else if (TypeOfPiece(p) != King) {
                    assert(HUFFMAN_TABLE[TypeOfPiece(p)] <= 9);
                    pos =
                        encode_bits(out, HUFFMAN_TABLE[TypeOfPiece(p)], 4, pos);
                    pos = encode_bit(out, PieceColor(p) == Black, pos);
                }
            }
        }
        return pos;
    }

    static uint16_t encode_move(ColorType side, const Move move) {
        Square from = StartSquare(move);
        Square to = DestSquare(move);
        uint16_t data = 0;
        switch (TypeOfMove(move)) {
        case KCastle:
            to = (side == White) ? chess::A1 : chess::H1;
            data |= 3 << 14;
            break;
        case QCastle:
            to = (side == White) ? chess::A1 : chess::H1;
            data |= 3 << 14;
            break;
        case Promotion:
            data |= (PromoteTo(move) - 2) << 12;
            data |= 1 << 14;
            break;
        case EnPassant:
            data |= 2 << 14;
            break;
        case Normal:
            break;
        default:
            break;
        }
        data |= to;
        data |= (from << 6);
        assert(from != to);
        assert(from < 64 && to < 64);
        return data;
    }

    // serialize as little-endian
    template <typename T> static void serialize(std::ostream &o, const T &data) {
        switch (sizeof(T)) {
        case 1:
            o.write((char *)(&data), sizeof(T));
            break;
        case 2: {
            uint16_t out =
                swapEndian16(reinterpret_cast<const uint16_t *>(&data));
            o.write((char *)(&out), sizeof(T));
            break;
        }
        case 4: {
            uint32_t out =
                swapEndian32(reinterpret_cast<const uint32_t *>(&data));
            o.write((char *)(&out), sizeof(T));
            break;
        }
        default:
            std::cerr << "unsuppored size for .bin seralization" << std::endl;
            break;
        } // end switch
    }
};

static Move randomMove(const Board &board, RootMoveGenerator &mg, Statistics &stats, ThreadData &td) {
    unsigned n = mg.moveCount();
    if (n == 0) {
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
    std::uniform_int_distribution<unsigned> dist(0, n - 1);
    unsigned pick = dist(td.engine);
    Move m = NullMove;
    int ord = 0;
    for (unsigned i = 0; i <= pick; ++i) {
        m = (board.checkStatus() == InCheck) ? mg.nextEvasion(ord)
                                             : mg.nextMove(ord);
    }
    assert(!IsNull(m));
    return m;
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

// Somewhat randomize moves by doing a fixed-node instead of fixed-depth search.
static Move semiRandomMove(const Board &board, Statistics &stats,
                           ThreadData &td, uint64_t nodeTarget) {
    Monitor monitor(nodeTarget);
    auto oldMonitor = td.searcher->registerMonitorFunction(
        std::bind(&Monitor::nodeMonitor, &monitor, _1, _2));
    Move m = td.searcher->findBestMove(board, FixedDepth, Constants::INFINITE_TIME,
                                       0, // extra time
                                       sp_options.depthLimit + 2,
                                       false, // background
                                       false, // uci
                                       stats, TalkLevel::Silent);
    td.searcher->registerMonitorFunction(oldMonitor);
    assert(!(stats.state == NormalState && IsNull(m)));
    return m;
}

static void selfplay(ThreadData &td) {
    SearchController *searcher = td.searcher;
    std::uniform_int_distribution<unsigned> dist(1,
                                                 sp_options.outputPlyFrequency);
    std::uniform_int_distribution<unsigned> rand_dist(
        1, sp_options.randomizeInterval);
    std::uniform_int_distribution<unsigned> rand2_dist(
        1, sp_options.semiRandomizeInterval);
    while (posCounter < sp_options.posCount) {
        if (sp_options.saveGames) {
            td.gameMoves.removeAll();
        }
        bool adjudicated = false, terminated = false;
        Statistics stats;
        Board board;
        unsigned zero_score_count = 0;
        unsigned ply = 0;
        enum class Result { WhiteWin, BlackWin, Draw } result = Result::Draw;
        std::vector<OutputData> output;
        uint64_t prevNodes = 0ULL;
        unsigned prevDepth = 0;
        for (; !adjudicated && !terminated; ++ply) {
            stats.clear();
            Move m = NullMove;
            if (ply < sp_options.maxBookPly) {
                std::unique_lock<std::mutex> lock(bookLock);
                m = globals::openingBook.pick(board);
            }
            score_t score = 0;
            if (IsNull(m)) {
                // Don't randomize if in TB range
                bool skipRandom = int(board.men()) <= globals::EGTBMenCount;
                if (sp_options.randomize &&
                    ply + sp_options.maxBookPly < sp_options.randomizeRange &&
                    rand_dist(td.engine) == sp_options.randomizeInterval) {
                    RootMoveGenerator mg(board);
                    if (mg.moveCount() > 1 && (sp_options.useSee || sp_options.limitEarlyKingMoves)) {
                        for (int i = 0; i < 10; i++) {
                            mg.reset();
                            m = randomMove(board, mg, stats, td);
                            if (sp_options.useSee && !seeSign(board,m,0)) continue;
                            if (sp_options.limitEarlyKingMoves && PieceMoved(m) == King) continue;
                            break;
                        }
                    }
                    else {
                        m = randomMove(board, mg, stats, td);
                    }
                    // TBD: we don't associate any prior FENS with the current
                    // game result after a random move. Stockfish though does do
                    // this.
                    output.clear();
                    // We have no score from a random move, so reset the
                    // adjudication counter
                    zero_score_count = 0;
                } else if (sp_options.semiRandomize && !skipRandom &&
                           prevNodes && prevDepth >= sp_options.depthLimit &&
                           rand2_dist(td.engine) ==
                               sp_options.semiRandomizeInterval) {
                    // Base the target node count on the node count for the
                    // previous search, but randomize it somewhat.
                    std::uniform_int_distribution<uint64_t> node_dist(
                        uint64_t(0.7 * prevNodes), uint64_t(1.3 * prevNodes));
                    uint64_t limit = node_dist(td.engine);
                    m = semiRandomMove(board, stats, td, limit);
                    score = stats.display_value;
                    if (score == 0)
                        ++zero_score_count;
                    else
                        zero_score_count = 0;
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
                    if (score == 0)
                        ++zero_score_count;
                    else
                        zero_score_count = 0;
                }
                assert(IsNull(m) || legalMove(board, m));
            }
            if (stats.state == Resigns || stats.state == Checkmate) {
                result = board.sideToMove() == White ? Result::BlackWin
                                                     : Result::WhiteWin;
                terminated = true;
            } else if (stats.state == Draw || stats.state == Stalemate) {
                terminated = true;
                result = Result::Draw;
            } else if (!terminated && sp_options.adjudicateDraw &&
                       ply >= sp_options.drawAdjudicationMinPly &&
                       zero_score_count >= sp_options.drawAdjudicationMoves) {
                // adjudicate draw
                stats.state = Draw;
                result = Result::Draw;
                adjudicated = true;
            }
            if (!IsNull(m)) {
                std::string image;
                if (sp_options.saveGames) {
                    Notation::image(board, m, Notation::OutputFormat::SAN,
                                    image);
                }
                if (!(sp_options.skipNonQuiet && CaptureOrPromotion(m)) &&
                    !sp_hash_table.check_and_replace_hash(board.hashCode())) {
                    if (ply >= sp_options.minOutPly &&
                        ply <= sp_options.maxOutPly &&
                        (dist(td.engine) % sp_options.outputPlyFrequency) ==
                            0) {
                        std::stringstream s;
                        BoardIO::writeFEN(board, s, 0);
                        OutputData data;
                        data.fen = s.str();
                        data.score = score;
                        data.ply = ply;
                        data.move = m;
                        data.stm = board.sideToMove();
                        data.move50Count = board.state.moveCount;
                        output.push_back(data);
                    }
                }
                BoardState previousState(board.state);
                board.doMove(m);
                if (sp_options.saveGames) {
                    td.gameMoves.add_move(board, previousState, m, image,
                                          false);
                }
            } else {
                // unexpected null move
                if (!terminated && !adjudicated)
                    break;
            }
        }
        if (sp_options.saveGames) {
            std::string resultStr;
            if (result == Result::WhiteWin)
                resultStr = "1-0";
            else if (result == Result::BlackWin)
                resultStr = "0-1";
            else
                resultStr = "1/2-1/2";
            saveGame(td, resultStr, *game_out_file);
        }
        for (const OutputData &data : output) {
            if (posCounter++ >= sp_options.posCount) break;
            if (sp_options.format == SelfPlayOptions::OutputFormat::Epd) {
                std::string resultStr;
                if (result == Result::WhiteWin)
                    resultStr = "1.0";
                else if (result == Result::BlackWin)
                    resultStr = "0.0";
                else
                    resultStr = "0.5";
                std::unique_lock<std::mutex> lock(outputLock);
                *pos_out_file << data.fen << ' ' << RESULT_TAG << " \""
                              << resultStr << "\";" << std::endl;
            } else {
                int resultVal; // result from side to move POV
                if (result == Result::WhiteWin)
                    resultVal = data.stm == White ? 1 : -1;
                else if (result == Result::BlackWin)
                    resultVal = data.stm == Black ? 1 : -1;
                else
                    resultVal = 0;
                std::unique_lock<std::mutex> lock(outputLock);
                binEncoder::output(data, resultVal, *pos_out_file);
            }
        }
    }
}

static void threadp(ThreadData *td) {
    // set stack size for Linux/Mac
#ifdef _POSIX_VERSION
    size_t stackSize;
    if (pthread_attr_getstacksize(&stackSizeAttrib, &stackSize)) {
        perror("pthread_attr_getstacksize");
        return;
    }
    if (stackSize < globals::LINUX_STACK_SIZE) {
        if (pthread_attr_setstacksize(&stackSizeAttrib, globals::LINUX_STACK_SIZE)) {
            perror("error setting thread stack size");
            return;
        }
    }
#endif

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
    std::cerr << "selfplay [-a (append)][-c cores] [-n positions] [-o output file] [-m output "
                 "every m positions] [-f output format (bin or epd)] [-d depth]"
              << std::endl;
}

static void init_threads() {
    // prepare threads
#ifdef _POSIX_VERSION
    if (pthread_attr_init(&stackSizeAttrib)) {
        perror("pthread_attr_init");
        return;
    }
#endif
    uint64_t seed = getRandomSeed();
    for (unsigned i = 0; i < sp_options.cores; i++) {
        threadDatas[i].index = i;
        threadDatas[i].searcher = nullptr;
        threadDatas[i].engine.seed(seed + i);
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
    Bitboard::init();
    Board::init();
    globals::initOptions();
    Attacks::init();
    Scoring::init();
    Search::init();

    if (!globals::initGlobals(false)) {
        globals::cleanupGlobals();
        exit(-1);
    }
    atexit(globals::cleanupGlobals);
    globals::delayedInit();
    if (globals::EGTBMenCount) {
        std::cerr << "Initialized tablebases" << std::endl;
    }

    globals::options.search.hash_table_size = 128 * 1024 * 1024;
    globals::options.book.frequency = 25;
    globals::options.book.weighting = 10;
    globals::options.book.scoring = 25;
    globals::options.book.random = 100;
    globals::options.learning.position_learning = false;
    globals::options.search.can_resign = true;
    globals::options.search.resign_threshold = -Params::PAWN_VALUE*30;

    int arg = 1;
    bool append = false;
    for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
        if (strcmp(argv[arg], "-a") == 0) {
            append = true;
        }
        else if (strcmp(argv[arg], "-c") == 0) {
            std::stringstream s(argv[++arg]);
            s >> sp_options.cores;
            if (s.bad()) {
                std::cerr << "error in core count after -c" << std::endl;
                return -1;
            }
            sp_options.cores =
                std::min<int>(Constants::MaxCPUs, sp_options.cores);

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
                std::cerr << "expected bin or epd after -f" << std::endl;
                return -1;
            }
            std::string fmt(argv[++arg]);
            if (fmt == "bin")
                sp_options.format = SelfPlayOptions::OutputFormat::Bin;
            else if (fmt == "epd")
                sp_options.format = SelfPlayOptions::OutputFormat::Epd;
            else {
                std::cerr << "expected bin or epd after -f" << std::endl;
                return -1;
            }
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
        } else if (strcmp(argv[arg], "-p") == 0) {
            globals::options.search.pureNNUE = 1;
        } else {
            usage();
            return -1;
        }
    }

    if (sp_options.posFileName == "") {
        sp_options.posFileName =
            sp_options.format == SelfPlayOptions::OutputFormat::Bin
                ? "positions.bin"
                : "positions.epd";
    }

    std::ios_base::openmode flags = std::ios::out;
    if (sp_options.format == SelfPlayOptions::OutputFormat::Bin) {
        flags = flags | std::ios::binary;
    }
    if (append) {
        flags = flags | std::ios::app;
    }
    pos_out_file = new std::ofstream(sp_options.posFileName, flags);
    if (sp_options.saveGames)
        game_out_file =
            new std::ofstream(sp_options.gameFileName, std::ios::out | std::ios::app);

    init_threads();
    launch_threads();

    delete pos_out_file;
    delete game_out_file;

    return 0;
}
