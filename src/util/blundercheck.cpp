// Copyright 2026 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "boardio.h"
#include "chessio.h"
#include "globals.h"
#include "hash.h"
#include "legal.h"
#include "notation.h"
#include "search.h"
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <thread>
#include <vector>

// Blunder-checks PGN files. Each game is searched move by move (up to a
// configurable ply limit). Moves that are still in the opening book are not
// analyzed. A shallow search is done for the position before each move; when a
// significant drop in evaluation is detected, the position is re-searched at a
// greater depth (with multipv=2) to confirm the drop. Confirmed blunders are
// annotated in the output PGN with the traditional "?" / "??" NAG suffixes.
// When the best move preserves or improves on the mover's score two plies
// earlier, it is added as a variation with a positional-evaluation NAG.
//
// Analysis can be run on multiple threads: the main thread reads and parses
// games and dispatches them to worker threads, each with its own searcher.
// Output is serialized with a lock (so games are never interleaved), but with
// more than one thread the order of games in the output is not preserved.

// Configuration, shared read-only by the worker threads once parsing starts.
static struct BlunderConfig {
    int depth = 8;              // initial scan depth
    int verifyDepth = 12;       // depth for the confirming re-search
    int plyLimit = 60;          // maximum number of plies to analyze
    score_t questionMargin = Scoring::PAWN_VALUE;     // drop annotated "?"
    score_t blunderMargin = 2 * Scoring::PAWN_VALUE;  // drop annotated "??"
    score_t stopThreshold = 5 * Scoring::PAWN_VALUE;  // stop once |score| exceeds
    bool verbose = false;
} config;

// Output (and, when verbose, diagnostic) serialization.
static std::mutex outputLock;

static void usage() {
    std::cerr << "blundercheck [-d <search depth>] [-D <verify depth>] [-p <ply limit>]"
              << std::endl;
    std::cerr << "             [-m <? margin (pawns)>] [-b <?? margin (pawns)>]"
              << std::endl;
    std::cerr << "             [-s <stop threshold (pawns)>] [-c <threads>] [-v] pgn_file(s)"
              << std::endl;
    std::cerr << "  -d  search depth for the initial scan (default 8)" << std::endl;
    std::cerr << "  -D  depth for the confirming re-search (default 12)" << std::endl;
    std::cerr << "  -p  maximum number of plies to analyze (default 60)" << std::endl;
    std::cerr << "  -m  score drop (in pawns) annotated as \"?\" (default 1.0)" << std::endl;
    std::cerr << "  -b  score drop (in pawns) annotated as \"??\" (default 2.0)" << std::endl;
    std::cerr << "  -s  stop analysis once |score| exceeds this (pawns, default 5.0)" << std::endl;
    std::cerr << "  -c  number of analysis threads (default 1)" << std::endl;
    std::cerr << "  -H  hash table size per thread, in MB (default 128)" << std::endl;
    std::cerr << "  -v  verbose: report scores and detected drops on stderr" << std::endl;
}

struct SearchResult {
    score_t score; // score from the perspective of the side to move
    Move best;     // best move found
};

// A parsed game queued for analysis. node j is the board before moves[j];
// nodeBoards has one extra entry for the position after the last move.
struct Job {
    MoveArray moves;
    std::vector<Board> nodeBoards;
    std::vector<bool> inBook;
    std::string result = "*";
    std::vector<ChessIO::Header> hdrs;
};

// Bounded, thread-safe queue handing parsed games from the reader to the
// worker threads.
class JobQueue {
  public:
    explicit JobQueue(size_t capacity) : cap(capacity) {}

    void push(Job &&job) {
        std::unique_lock<std::mutex> lock(mtx);
        notFull.wait(lock, [this] { return queue.size() < cap; });
        queue.push(std::move(job));
        notEmpty.notify_one();
    }

    // Retrieve a job, blocking until one is available. Returns false when the
    // queue is empty and no more jobs will be added.
    bool pop(Job &out) {
        std::unique_lock<std::mutex> lock(mtx);
        notEmpty.wait(lock, [this] { return !queue.empty() || done; });
        if (queue.empty())
            return false;
        out = std::move(queue.front());
        queue.pop();
        notFull.notify_one();
        return true;
    }

    void setDone() {
        std::unique_lock<std::mutex> lock(mtx);
        done = true;
        notEmpty.notify_all();
    }

  private:
    std::queue<Job> queue;
    std::mutex mtx;
    std::condition_variable notFull, notEmpty;
    const size_t cap;
    bool done = false;
};

// Run a fixed-depth search of "board" and return its score and best move.
static SearchResult searchPosition(SearchController *searcher, const Board &board, int depth,
                                   int multipv) {
    searcher->setMultiPV(multipv);
    Statistics stats;
    stats.clear();
    Move best = searcher->findBestMove(board, FixedTime, Constants::INFINITE_TIME,
                                       0,             /* extra time */
                                       depth, false,  /* background */
                                       false,         /* UCI */
                                       stats, TalkLevel::Silent);
    return {stats.display_value, best};
}

// Return the positional-evaluation NAG (PGN standard) for a score expressed
// from White's perspective (in internal score units).
static std::string evalNAG(score_t whiteScore) {
    const double pawns = static_cast<double>(whiteScore) / Scoring::PAWN_VALUE;
    const double a = std::fabs(pawns);
    int nag;
    if (a < 0.25)
        nag = 10; // equal
    else if (a < 0.75)
        nag = pawns > 0 ? 14 : 15; // slight advantage / disadvantage
    else if (a < 1.5)
        nag = pawns > 0 ? 16 : 17; // strong advantage / disadvantage
    else
        nag = pawns > 0 ? 18 : 19; // winning / losing
    return "$" + std::to_string(nag);
}

// Parse the next game from the stream into "job". Returns false at end of file.
// Invalid or empty games are skipped. The opening book is consulted here only,
// so it is always accessed from the single reader thread.
static bool parseGame(ChessIO::PGNReader &pgnReader, std::ifstream &pgn_file, Job &job) {
    while (!pgn_file.eof()) {
        job = Job();
        long first;
        pgnReader.collectHeaders(job.hdrs, first);
        Board board;
        board.reset();
        bool stillInBook = true;
        bool valid = true;
        int var = 0;
        ChessIO::TokenReader tokenReader(pgnReader);
        bool done = false;
        while (!done) {
            ChessIO::Token tok = tokenReader.nextToken();
            if (tok.type == ChessIO::Eof)
                break;
            switch (tok.type) {
            case ChessIO::Number:
                continue;
            case ChessIO::GameMove: {
                if (var)
                    continue;
                Move m = Notation::value(board, board.sideToMove(), Notation::InputFormat::SAN,
                                         tok.val);
                if (IsNull(m) || !legalMove(board, m)) {
                    std::cerr << "Illegal move: " << tok.val << std::endl;
                    valid = false;
                } else if (valid) {
                    // determine whether this move is still a book move
                    bool isBook = false;
                    if (stillInBook) {
                        std::vector<Move> bookMoves;
                        globals::openingBook.book_moves(board, bookMoves);
                        for (const Move &bm : bookMoves) {
                            if (MovesEqual(bm, m)) {
                                isBook = true;
                                break;
                            }
                        }
                        if (!isBook)
                            stillInBook = false;
                    }
                    std::string moveStr;
                    Notation::image(board, m, Notation::OutputFormat::SAN, moveStr);
                    job.nodeBoards.push_back(board);
                    job.inBook.push_back(isBook);
                    job.moves.add_move(board, m, moveStr, false, isBook);
                    board.doMove(m);
                }
                break;
            }
            case ChessIO::OpenVar:
                ++var;
                break;
            case ChessIO::CloseVar:
                --var;
                break;
            case ChessIO::Unknown:
                std::cerr << "Unrecognized text: " << tok.val << std::endl;
                valid = false;
                break;
            case ChessIO::Result:
                job.result = tok.val;
                done = true;
                break;
            default:
                break;
            } // end switch
        }
        if (!valid || job.moves.num_moves() == 0)
            continue;
        // append the terminal position (after the last move)
        job.nodeBoards.push_back(board);
        return true;
    }
    return false;
}

// Analyze a parsed game, annotate blunders, and write the result to stdout.
static void analyzeGame(SearchController *searcher, Job &job) {
    // Start each game from a clean hash so its analysis does not depend on
    // which games this searcher processed before it (and hence not on the
    // number of threads).
    searcher->clearHashTables();

    MoveArray &moves = job.moves;
    const std::vector<Board> &nodeBoards = job.nodeBoards;
    const std::vector<bool> &inBook = job.inBook;

    // Number of moves to analyze, and number of node positions needed.
    const int numMoves = static_cast<int>(moves.num_moves());
    const int lastMove = std::min(config.plyLimit, numMoves);

    // Score (side-to-move perspective) of each node, normalized to White's
    // perspective. Only computed for nodes we need.
    std::vector<score_t> wScore(lastMove + 1, 0);
    std::vector<bool> hasScore(lastMove + 1, false);

    // We can only evaluate moves up to lastEval-1 (decisive positions stop the
    // scan early).
    int lastEval = lastMove;
    for (int j = 0; j <= lastMove; j++) {
        // node j is needed as the "before" score of move j (if move j is
        // analyzed) or the "after" score of move j-1.
        bool needBefore = (j < lastMove) && !inBook[j];
        bool needAfter = (j > 0) && (j - 1 < lastMove) && !inBook[j - 1];
        if (!needBefore && !needAfter)
            continue;
        score_t raw = searchPosition(searcher, nodeBoards[j], config.depth, 1).score;
        // node j side to move is White when j is even
        wScore[j] = (j % 2 == 0) ? raw : -raw;
        hasScore[j] = true;
        if (config.verbose) {
            std::unique_lock<std::mutex> lock(outputLock);
            std::cerr << "node " << j << " raw=" << raw << " white=" << wScore[j]
                      << " inBook=" << ((j < lastMove) ? (int)inBook[j] : -1) << std::endl;
        }
        if (std::abs(raw) > config.stopThreshold) {
            // position is decisive; do not analyze beyond this point
            lastEval = j;
            break;
        }
    }

    // Evaluate each non-book move for a confirmed score drop.
    for (int i = 0; i < lastEval; i++) {
        if (inBook[i] || !hasScore[i] || !hasScore[i + 1])
            continue;
        const int moverSign = (i % 2 == 0) ? 1 : -1;
        score_t loss = moverSign * (wScore[i] - wScore[i + 1]);
        if (config.verbose && loss > 0) {
            std::unique_lock<std::mutex> lock(outputLock);
            std::cerr << "move " << i << " (" << moves[i].image << ") loss=" << loss << std::endl;
        }
        if (loss <= config.questionMargin)
            continue;
        // Candidate drop: re-search the before position at a greater depth with
        // multipv=2, and the after position to get the played move's refined
        // value.
        SearchResult before = searchPosition(searcher, nodeBoards[i], config.verifyDepth, 2);
        score_t after = searchPosition(searcher, nodeBoards[i + 1], config.verifyDepth, 1).score;
        score_t bestBefore = before.score;
        score_t wBefore = (i % 2 == 0) ? bestBefore : -bestBefore;
        score_t wAfter = ((i + 1) % 2 == 0) ? after : -after;
        score_t loss2 = moverSign * (wBefore - wAfter);
        const char *nag = nullptr;
        if (loss2 > config.blunderMargin) {
            nag = "??";
        } else if (loss2 > config.questionMargin) {
            nag = "?";
        }
        if (!nag)
            continue;
        moves[i].image += nag;
        if (config.verbose) {
            std::unique_lock<std::mutex> lock(outputLock);
            std::cerr << "move " << i << " (" << moves[i].image << ") confirmed loss=" << loss2
                      << std::endl;
        }
        // If the best move is much better than the move played and preserves or
        // improves on the score the mover had two plies earlier, append it as a
        // variation with a positional-evaluation NAG. The earlier position is
        // re-searched at the same depth so the comparison is like-for-like.
        if (!IsNull(before.best) && !MovesEqual(before.best, moves[i].move) && i >= 2) {
            // node i-2 has the same side to move as node i (the mover), so its
            // score is already from the mover's perspective.
            score_t prevMoverValue =
                searchPosition(searcher, nodeBoards[i - 2], config.verifyDepth, 1).score;
            std::string betterSAN;
            Notation::image(nodeBoards[i], before.best, Notation::OutputFormat::SAN, betterSAN);
            const bool preserves = bestBefore >= prevMoverValue;
            if (config.verbose) {
                std::unique_lock<std::mutex> lock(outputLock);
                std::cerr << "  best " << betterSAN << " " << evalNAG(wBefore)
                          << " (prev=" << prevMoverValue << " best=" << bestBefore
                          << (preserves ? " -> suggest)" : ")") << std::endl;
            }
            if (preserves) {
                std::stringstream varStr;
                varStr << " (" << (i / 2) + 1 << (i % 2 == 0 ? ". " : "... ") << betterSAN << ' '
                       << evalNAG(wBefore) << ')';
                moves[i].image += varStr.str();
            }
        }
    }
    // restore default multipv for the next game
    searcher->setMultiPV(1);

    std::unique_lock<std::mutex> lock(outputLock);
    ChessIO::store_pgn(std::cout, moves, job.result, job.hdrs);
}

// Worker thread: owns a searcher and analyzes games until the queue is drained.
static void worker(JobQueue &queue) {
    SearchController *searcher = nullptr;
    try {
        searcher = new SearchController();
    } catch (std::bad_alloc &) {
        std::cerr << "out of memory allocating searcher" << std::endl;
        return;
    }
    Job job;
    while (queue.pop(job)) {
        analyzeGame(searcher, job);
    }
    delete searcher;
}

int CDECL main(int argc, char **argv) {
    BitUtils::init();
    Board::init();
    globals::initOptions();
    Attacks::init();
    Search::init();
    // Each worker thread runs its own single-threaded searcher; parallelism
    // comes from running several of them.
    globals::options.search.ncpus = 1;
    if (!globals::initGlobals()) {
        globals::cleanupGlobals();
        exit(-1);
    }
    atexit(globals::cleanupGlobals);
    // Enable the opening book so book moves can be skipped.
    globals::options.book.book_enabled = true;
    globals::options.book.eco_enabled = false;
    globals::delayedInit(false);

    int threads = 1;
    int hashSizeMB = 128; // hash table size per thread, in megabytes

    if (argc == 1) {
        usage();
        return -1;
    }

    int arg = 1;
    auto processInt = [&arg, &argc, &argv](int &opt, const std::string &name) {
        if (++arg < argc) {
            std::stringstream s(argv[arg]);
            s >> opt;
            if (s.bad() || s.fail()) {
                std::cerr << "expected integer after -" << name << std::endl;
                exit(-1);
            }
        } else {
            std::cerr << "expected integer after -" << name << std::endl;
            exit(-1);
        }
    };
    auto processDouble = [&arg, &argc, &argv](double &opt, const std::string &name) {
        if (++arg < argc) {
            std::stringstream s(argv[arg]);
            s >> opt;
            if (s.bad() || s.fail()) {
                std::cerr << "expected number after -" << name << std::endl;
                exit(-1);
            }
        } else {
            std::cerr << "expected number after -" << name << std::endl;
            exit(-1);
        }
    };
    // margins parsed in pawns, then converted to internal score units
    double questionPawns = 1.0, blunderPawns = 2.0, stopPawns = 5.0;
    for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
        if (strcmp(argv[arg], "-d") == 0) {
            processInt(config.depth, "d");
        } else if (strcmp(argv[arg], "-D") == 0) {
            processInt(config.verifyDepth, "D");
        } else if (strcmp(argv[arg], "-p") == 0) {
            processInt(config.plyLimit, "p");
        } else if (strcmp(argv[arg], "-m") == 0) {
            processDouble(questionPawns, "m");
        } else if (strcmp(argv[arg], "-b") == 0) {
            processDouble(blunderPawns, "b");
        } else if (strcmp(argv[arg], "-s") == 0) {
            processDouble(stopPawns, "s");
        } else if (strcmp(argv[arg], "-c") == 0) {
            processInt(threads, "c");
        } else if (strcmp(argv[arg], "-H") == 0) {
            processInt(hashSizeMB, "H");
        } else if (strcmp(argv[arg], "-v") == 0) {
            config.verbose = true;
        } else {
            usage();
            exit(-1);
        }
    }
    if (arg >= argc) {
        usage();
        exit(-1);
    }
    if (threads < 1)
        threads = 1;
    threads = std::min<int>(threads, Constants::MaxCPUs);
    if (hashSizeMB < 1)
        hashSizeMB = 1;
    // Each worker's searcher allocates a hash table of this size at construction.
    globals::options.search.hash_table_size = static_cast<size_t>(hashSizeMB) * 1024 * 1024;

    config.questionMargin = static_cast<score_t>(questionPawns * Scoring::PAWN_VALUE);
    config.blunderMargin = static_cast<score_t>(blunderPawns * Scoring::PAWN_VALUE);
    config.stopThreshold = static_cast<score_t>(stopPawns * Scoring::PAWN_VALUE);

    JobQueue queue(2 * threads + 2);
    std::vector<std::thread> workers;
    for (int i = 0; i < threads; i++) {
        workers.emplace_back(worker, std::ref(queue));
    }

    // Reader: parse games from each file and dispatch them to the workers.
    for (; arg < argc; arg++) {
        std::ifstream pgn_file(argv[arg], std::ios::in);
        if (!pgn_file.good()) {
            std::cerr << "could not open file " << argv[arg] << std::endl;
            continue;
        }
        ChessIO::PGNReader pgnReader(pgn_file);
        Job job;
        while (parseGame(pgnReader, pgn_file, job)) {
            queue.push(std::move(job));
        }
    }
    queue.setDone();
    for (auto &t : workers) {
        t.join();
    }
    return 0;
}
