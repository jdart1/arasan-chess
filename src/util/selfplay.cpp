// Copyright 2021-2024 by Jon Dart. All Rights Reserved.
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
#include <iomanip>
#include <iostream>
#include <mutex>
#include <random>
#include <sstream>
#include <string>
#include <vector>
#ifndef _MSC_VER
extern "C" {
#include <time.h>
#include <unistd.h>
}
#endif

static constexpr size_t LINUX_STACK_SIZE = 4 * 1024 * 1024;

using namespace std::placeholders;

// Utility to generate training positions from Arasan self-play games

static ECO ecoCoder;

static const char *RESULT_TAG = "c2";

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

static constexpr int MAX_MULTIPV = 4;

static struct SelfPlayOptions {
    // Note: not all options are command-line settable, currently.
    enum class OutputFormat { Epd, Bin };
    enum class RandomizeType { Nodes, MultiPV };
    unsigned minOutPly = 16;
    unsigned maxOutPly = 300;
    unsigned cores = 1;
    unsigned posCount = 10000000;
    unsigned depthLimit = 6;
    bool adjudicateDraw = true;
    bool adjudicate7manDraw = false;
    bool adjudicateTB = true;
    unsigned outputPlyFrequency = 1; // output every nth move
    unsigned drawAdjudicationPlies = 10;
    unsigned TBAdjudicationPlies = 10;
    unsigned drawAdjudicationMinPly = 150;
    int adjudicateMinMove50Count = 40; // mininum move 50 count
    std::string posFileName;
    std::string gameFileName = "games.pgn";
    bool saveGames = false;
    unsigned maxBookPly = 8;
    bool randomize = true;
    unsigned randomizeRange = 8;
    unsigned randomizeInterval = 2;
    int randomTolerance = 0.75 * Params::PAWN_VALUE;
    bool useRanking = true;
    bool limitEarlyKingMoves = true;
    bool semiRandomize = true;
    RandomizeType randomizeType = RandomizeType::MultiPV;
    unsigned semiRandomizeInterval = 11;
    unsigned semiRandomPerGame = 4;
    int multiPVMargin = static_cast<int>(0.2 * Params::PAWN_VALUE);
    bool skipNonQuiet = true;
    OutputFormat format = OutputFormat::Bin;
    bool verbose = false;
    unsigned verboseReportingInterval = 1000000;
    bool checkHash = false; // use hash table to check for possible dups
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
    char dateStr[32];
    time_t tm = time(NULL);
    struct tm *t = localtime(&tm);
    snprintf(dateStr, 15, "%4d.%02d.%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
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
        pos = encode_bit(posData, wcs == CanCastleKSide || wcs == CanCastleEitherSide, pos);
        pos = encode_bit(posData, wcs == CanCastleQSide || wcs == CanCastleEitherSide, pos);
        pos = encode_bit(posData, bcs == CanCastleKSide || bcs == CanCastleEitherSide, pos);
        pos = encode_bit(posData, bcs == CanCastleQSide || bcs == CanCastleEitherSide, pos);
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
        if (out.bad()) {
            std::cerr << "error writing output file." << std::endl;
        }
        assert(out.tellp() % 40 == 0);
    }

  private:
    static inline unsigned encode_bit(PosOutputType &out, unsigned bit, unsigned pos) {
        if (bit) {
            out[pos / 8] |= (1 << (pos % 8));
        }
        return pos + 1;
    }

    static inline unsigned encode_bits(PosOutputType &out, unsigned bits, unsigned size,
                                       unsigned pos) {
        for (unsigned i = 0; i < size; i++) {
            pos = encode_bit(out, bits & (1 << i), pos);
        }
        return pos;
    }

    static unsigned encode_board(PosOutputType &out, const Board &b, unsigned pos) {
        static const std::array<unsigned, 6> HUFFMAN_TABLE = {0, 1, 3, 5, 7, 9};
        for (unsigned rank = 0; rank < 8; ++rank) {
            for (unsigned file = 0; file < 8; ++file) {
                Square sq = 56 - rank * 8 + file;
                const Piece &p = b[sq];
                if (IsEmptyPiece(p)) {
                    ++pos; // 1 bit
                } else if (TypeOfPiece(p) != King) {
                    assert(HUFFMAN_TABLE[TypeOfPiece(p)] <= 9);
                    pos = encode_bits(out, HUFFMAN_TABLE[TypeOfPiece(p)], 4, pos);
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
            to = (side == White) ? chess::H1 : chess::H8;
            data |= 3 << 14;
            break;
        case QCastle:
            to = (side == White) ? chess::A1 : chess::A8;
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
            uint16_t out = swapEndian16(reinterpret_cast<const uint16_t *>(&data));
            o.write((char *)(&out), sizeof(T));
            break;
        }
        case 4: {
            uint32_t out = swapEndian32(reinterpret_cast<const uint32_t *>(&data));
            o.write((char *)(&out), sizeof(T));
            break;
        }
        default:
            std::cerr << "unsuppored size for .bin seralization" << std::endl;
            break;
        } // end switch
    }
};

static Move randomMove(const Board &board, RootMoveGenerator &mg, Statistics &stats,
                       ThreadData &td) {
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
    RootMoveGenerator::RootMoveList ml(mg.getMoveList());
    assert(ml.size() != 0);
    std::uniform_int_distribution<unsigned> dist(0, ml.size() - 1);
    unsigned pick = dist(td.engine);
    Move m = ml[pick].move;
    return m;
}

struct MoveResult {
    MoveResult() : move(NullMove), score(0) {}
    Move move;
    int score;
};

static Move pick(const Move *moves, unsigned count, ThreadData &td) {
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
        // We don't actually use the multipv option here because it
        // is global, and we only want multipv for this thread. So
        // we do multiple searches
        MoveSet includes, excludes;
        RootMoveGenerator mg(board);
        unsigned limit = std::min<unsigned>(mg.moveCount(), MAX_MULTIPV);
        numCandidates = 0;
        if (limit == 0) {
            // there are no moves, but search to get a score (loss/win/draw)
            Move result = td.searcher->findBestMove(board, FixedDepth, Constants::INFINITE_TIME, 0,
                                                    sp_options.depthLimit, false, false, stats,
                                                    TalkLevel::Silent, excludes, includes);
            numCandidates = 0;
            mr.move = result;
            mr.score = stats.display_value;
            state = stats.state;
            return;
        }
        int maxScore = -Constants::MATE, candCount = 0;
        unsigned count = 0;
        MoveResult moves[MAX_MULTIPV], candidates[MAX_MULTIPV];
        for (unsigned index = 0; index < limit; ++index, ++count) {
            Move result;
            result = moves[index].move = td.searcher->findBestMove(
                board, FixedDepth, Constants::INFINITE_TIME, 0, sp_options.depthLimit, false, false,
                stats, TalkLevel::Silent, excludes, includes);
            if (stats.state == Checkmate || stats.state == Stalemate || stats.state == Draw) {
                state = stats.state;
                break;
            }
            if (stats.display_value > maxScore)
                maxScore = stats.display_value;
            moves[index].score = stats.display_value;
            excludes.insert(result);
        }
        for (unsigned index = 0; index < count; ++index) {
            if (maxScore - moves[index].score <= sp_options.multiPVMargin) {
                candidates[candCount++] = moves[index];
            }
        }
        numCandidates = candCount;
        mr = candCount ? pick(candidates, candCount, td) : MoveResult();
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
        unsigned low_score_count = 0, tb_score_count = 0;
        enum class Result { WhiteWin, BlackWin, Draw } result = Result::Draw;
        std::vector<OutputData> output;
        uint64_t prevNodes = 0ULL;
        int prevScore = 0;
        unsigned noSemiRandom = 0, semiRandomCount = 0, prevDepth = 0;
        for (unsigned ply = 0; ply <= sp_options.maxOutPly && !adjudicated && !terminated; ++ply) {
            stats.clear();
            Move m = NullMove;
            if (ply < sp_options.maxBookPly) {
                std::unique_lock<std::mutex> lock(bookLock);
                m = globals::openingBook.pick(board, false);
            }
            score_t score = 0;
            if (IsNull(m)) {
                // Don't randomize if in TB range or if score very large
                bool skipRandom = (int(board.men()) <= globals::EGTBMenCount) ||
                                  (std::abs(prevScore) >= 10 * Params::PAWN_VALUE);
                if (sp_options.randomize &&
                    (ply <= sp_options.maxBookPly + sp_options.randomizeRange) &&
                    rand_dist(td.engine) == sp_options.randomizeInterval) {
                    RootMoveGenerator mg(board);
                    if (mg.moveCount() > 1 &&
                        (sp_options.useRanking || sp_options.limitEarlyKingMoves)) {
                        Move candidates[Constants::MaxMoves], all[Constants::MaxMoves];
                        bool inCheck = board.checkStatus() == InCheck;
                        int index;
                        unsigned i = 0, j = 0;
                        RootMoveGenerator::RootMoveList mra;
                        if (sp_options.useRanking) {
                            searcher->rankMoves(board, 2, mra);
                            for (const RootMoveGenerator::RootMove &rm : mra) {
                                all[j++] = rm.move;
                            }
                            if (mg.moveCount() >= 2) {
                                assert(mra.size() > 0);
                                const int best = mra[0].score;
                                auto new_end = std::remove_if(
                                    mra.begin() + 1, mra.end(),
                                    [&](const RootMoveGenerator::RootMove &r) -> bool {
                                        if (r.score < best - sp_options.randomTolerance)
                                            return true;
                                        BoardState state(board.state);
                                        board.doMove(r.move);
                                        int rep = board.repCount();
                                        board.undoMove(r.move, state);
                                        // avoid repetitions
                                        return rep > 0;
                                    });
                                candidates[i++] = mra[0].move;
                                for (auto it = mra.begin() + 1; it != new_end; ++it) {
                                    assert((*it).score <= best);
                                    assert((*it).score >= best - sp_options.randomTolerance);
                                    if (!inCheck && !IsCastling(m) &&
                                        sp_options.limitEarlyKingMoves && PieceMoved(m) == King)
                                        continue;
                                    candidates[i++] = (*it).move;
                                }
                                assert(i);
                            }
                        } else {
                            while (!IsNull(
                                m = (inCheck ? mg.nextEvasion(index) : mg.nextMove(index)))) {
                                assert(j < Constants::MaxMoves);
                                all[j++] = m;
                                if (!inCheck && !IsCastling(m) && sp_options.limitEarlyKingMoves &&
                                    PieceMoved(m) == King)
                                    continue;
                                assert(i < Constants::MaxMoves);
                                candidates[i++] = m;
                            }
                        }
                        if (i) {
                            m = pick(candidates, i, td);
                        } else {
                            m = pick(all, j, td);
                        }
                    } else {
                        m = randomMove(board, mg, stats, td);
                    }
                    // TBD: we don't associate any prior FENS with the current
                    // game result after a random move. Stockfish though does do
                    // this.
                    output.clear();
                    // We have no score from a random move, so reset the
                    // adjudication counter
                    low_score_count = 0;
                } else if (sp_options.semiRandomize && !skipRandom &&
                           (noSemiRandom >= sp_options.semiRandomizeInterval) &&
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
            } else if (!terminated &&
                       board.state.moveCount >= sp_options.adjudicateMinMove50Count) {
                if ((sp_options.adjudicateDraw ||
                     (sp_options.adjudicate7manDraw && board.men() <= 7)) &&
                    ply >= sp_options.drawAdjudicationMinPly &&
                    low_score_count >= sp_options.drawAdjudicationPlies) {
                    // adjudicate draw
                    stats.state = Draw;
                    result = Result::Draw;
                    adjudicated = true;
                } else if (sp_options.adjudicateTB && board.men() <= globals::EGTBMenCount &&
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
                }
            }
            if (!IsNull(m)) {
                assert(validMove(board, m));
                std::string image;
                if (sp_options.saveGames) {
                    Notation::image(board, m, Notation::OutputFormat::SAN, image);
                }
                if (!(sp_options.skipNonQuiet && CaptureOrPromotion(m)) &&
                    ply >= sp_options.minOutPly &&
                    (ply > sp_options.maxBookPly + sp_options.randomizeRange) &&
                    !board.repCount(1) &&
                    (!sp_options.checkHash ||
                     !sp_hash_table.check_and_replace_hash(board.hashCode()))) {
                    if ((dist(td.engine) % sp_options.outputPlyFrequency) == 0) {
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
                board.doMove(m);
                if (sp_options.saveGames) {
                    td.gameMoves.add_move(board, m, image);
                }
            } else {
                if (!terminated && !adjudicated) {
                    // unexpected null move
                    std::cerr << "null move from search!" << std::flush << std::endl;
                }
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
            if (sp_options.format == SelfPlayOptions::OutputFormat::Epd) {
                std::string resultStr;
                if (result == Result::WhiteWin)
                    resultStr = "1.0";
                else if (result == Result::BlackWin)
                    resultStr = "0.0";
                else
                    resultStr = "0.5";
                std::unique_lock<std::mutex> lock(outputLock);
                *pos_out_file << data.fen << ' ' << RESULT_TAG << " \"" << resultStr << "\";"
                              << std::endl;
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
    std::cerr << "selfplay [-a (append)] [-d depth] [-v (verbose)] [-c cores] [-n positions] [-o "
                 "output file]"
              << std::endl;
    std::cerr << "         [-m output every m positions] [-f output format (bin or epd)] [-g "
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
    Bitboard::init();
    Board::init();
    globals::initOptions();
    Attacks::init();
    Scoring::init();
    Search::init();

    globals::options.search.hash_table_size = 128 * 1024 * 1024;
    globals::options.book.book_enabled = true;
    globals::options.book.variety = 80;
    globals::options.learning.position_learning = false;
    globals::options.search.can_resign = true;
    globals::options.search.resign_threshold = -Params::PAWN_VALUE * 30;
    globals::options.search.widePlies = 1;
    globals::options.search.wideWindow = 3 * Params::PAWN_VALUE;

    if (!globals::initGlobals()) {
        globals::cleanupGlobals();
        exit(-1);
    }
    atexit(globals::cleanupGlobals);
    globals::delayedInit();
    if (!globals::EGTBMenCount) {
        std::cerr << "warning: no tablebases found." << std::endl;
    }

    int arg = 1;
    bool append = false;
    for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
        if (strcmp(argv[arg], "-a") == 0) {
            append = true;
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
        } else if (strcmp(argv[arg], "-p") == 0) {
            globals::options.search.pureNNUE = 1;
        } else if (strcmp(argv[arg], "-v") == 0) {
            sp_options.verbose = true;
        } else {
            usage();
            return -1;
        }
    }

    if (sp_options.posFileName == "") {
        sp_options.posFileName = sp_options.format == SelfPlayOptions::OutputFormat::Bin
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

    delete pos_out_file;
    delete game_out_file;

    return 0;
}
