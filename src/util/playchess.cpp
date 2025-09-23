// Copyright 2010, 2011, 2012, 2017, 2020-2021, 2023-2025 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "chessio.h"
#include "globals.h"
#include "hash.h"
#include "legal.h"
#include "notation.h"
#include "search.h"
#include <cctype>
#include <fstream>
#include <iostream>

// Filters PGN games and removes those for which the terminal
// position's eval differs significantly from the game result.

static void usage() {
    std::cerr << "playchess [-d <depth limit>] [-t <time limit (sec.)>] [-x] [-min <min ply>] [-e "
                 "<min ELO>] pgn_file(s)"
              << std::endl;
    std::cerr << "-x outputs games that do not pass criteria" << std::endl;
}

static std::string trim(std::string s) {
    std::string res(s);
    res.erase(0, res.find_first_not_of(' '));
    res.erase(res.find_last_not_of(' ') + 1);
    return res;
}

int CDECL main(int argc, char **argv) {
    BitUtils::init();
    Board::init();
    globals::initOptions();
    Attacks::init();
    Search::init();
    globals::options.search.hash_table_size = 64 * 1024 * 1024;
    if (!globals::initGlobals()) {
        globals::cleanupGlobals();
        exit(-1);
    }
    atexit(globals::cleanupGlobals);
    globals::options.book.book_enabled = false;
    globals::delayedInit(false);
    if (globals::EGTBMenCount) {
        std::cerr << "Initialized tablebases" << std::endl;
    }

    int minMoves = 30;
    int minELO = 0;
    // time limit in ms.
    int timeLimit = 5000;
    int depthLimit = 12;
    enum LimitType { Depth, Time } searchType = Depth;
    bool x_flag = false;

    if (argc == 1) {
        usage();
        return -1;
    } else {
        int arg = 1;
        int count = 0;
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
        for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
            if (strcmp(argv[arg], "-min") == 0) {
                processInt(minMoves, "m");
            } else if (strcmp(argv[arg], "-t") == 0) {
                ++count;
                processInt(timeLimit, "t");
                timeLimit *= 1000; // convert to milliseconds
                searchType = Time;
            } else if (strcmp(argv[arg], "-d") == 0) {
                ++count;
                processInt(depthLimit, "d");
                searchType = Depth;
            } else if (strcmp(argv[arg], "-e") == 0) {
                processInt(minELO, "e");
            } else if (strcmp(argv[arg], "-x") == 0) {
                x_flag = true;
            } else {
                usage();
                exit(-1);
            }
        }
        if (arg >= argc) {
            usage();
            exit(-1);
        }
        if (count > 1) {
            std::cerr << "-d and -t options are not compatible" << std::endl;
            exit(-1);
        }
        SearchController *searcher = new SearchController();
        Statistics stats;
        for (; arg < argc; arg++) {
            std::ifstream pgn_file(argv[arg], std::ios::in);
            ColorType side;
            std::string result, white, black;
            if (!pgn_file.good()) {
                std::cerr << "could not open file " << argv[arg] << std::endl;
                exit(-1);
            } else {
                std::vector<ChessIO::Header> hdrs;
                ChessIO::PGNReader pgnReader(pgn_file);
                Board board;
                while (!pgn_file.eof()) {
                    hdrs.clear();
                    long first;
                    pgnReader.collectHeaders(hdrs, first);
                    MoveArray moves;
                    board.reset();
                    side = White;
                    float last_score = -1;
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
                            // parse the move
                            Move m;
                            m = Notation::value(board, board.sideToMove(),
                                                Notation::InputFormat::SAN, tok.val);
                            if (IsNull(m) || !legalMove(board, m)) {
                                // echo to both stdout and stderr
                                std::cerr << "Illegal move: " << tok.val << std::endl;
                                std::cout << "Illegal move: " << tok.val << std::endl;
                                valid = false;

                            } else if (valid) {
                                std::string moveStr;
                                Notation::image(board, m, Notation::OutputFormat::SAN, moveStr);
                                moves.add_move(board, m, moveStr);
                                board.doMove(m);
                            }
                            side = OppositeColor(side);
                            break;
                        }
                        case ChessIO::OpenVar:
                            ++var;
                            break;
                        case ChessIO::CloseVar:
                            --var;
                            break;
                            ;
                        case ChessIO::Unknown:
                            std::cerr << "Unrecognized text: " << tok.val << std::endl;
                            valid = false;
                            break;
                        case ChessIO::Comment:
                            break;
                        case ChessIO::Result: {
                            result = tok.val;
                            if (result == "#") {
                                last_score = 10000.0F;
                            } else if (result == "1/2-1/2" &&
                                       (board.isLegalDraw() || Scoring::theoreticalDraw(board))) {
                                last_score = 0.0;
                            } else {
                                stats.clear();
                                searcher->findBestMove(
                                    board, FixedTime,
                                    searchType == Time ? timeLimit : Constants::INFINITE_TIME,
                                    0, /* extra time allowed */
                                    searchType == Depth ? depthLimit : Constants::MaxPly,
                                    false, /* background */
                                    false, /* UCI */
                                    stats, TalkLevel::Silent);

                                last_score = float(stats.display_value) / Scoring::PAWN_VALUE;
                            }
                            if (board.sideToMove() != White)
                                last_score = -last_score;
                            done = true;
                            break;
                        }
                        default:
                            break;
                        } // end switch
                    }
                    if (!valid)
                        continue;

                    bool ok = true;

                    // done with a game
                    if (moves.num_moves() < (unsigned)minMoves || !valid) {
                        ok = false;
                    } else if (strcmp(result.c_str(), "1/2-1/2") == 0) {
                        if (last_score >= 1.0 || last_score <= -1.0) {
                            ok = false;
                        }
                    } else if (strcmp(result.c_str(), "1-0") == 0) {
                        if (last_score <= 1.5)
                            ok = false;
                    } else if (strcmp(result.c_str(), "0-1") == 0) {
                        if (last_score >= -1.5)
                            ok = false;
                    }

                    std::string eco;
                    ChessIO::get_header(hdrs, "ECO", eco);

                    // output header
                    if (valid) {
                        std::string whiteELOStr, blackELOStr;
                        if (minELO > 0) {
                            if (ChessIO::get_header(hdrs, "WhiteElo", whiteELOStr) &&
                                ChessIO::get_header(hdrs, "BlackElo", blackELOStr)) {
                                int whiteElo = 0, blackElo = 0;
                                if (sscanf(whiteELOStr.c_str(), "%d", &whiteElo) &&
                                    sscanf(blackELOStr.c_str(), "%d", &blackElo)) {
                                    if (whiteElo < minELO || blackElo < minELO)
                                        ok = false;
                                }
                            } else // no ELO info available
                                ok = false;
                        }
                    }

                    if (x_flag)
                        ok = !ok;
                    if (!ok)
                        continue;

                    // trim header values
                    for (auto &hdr : hdrs) {
                        hdr.second = trim(hdr.second);
                    }

                    ChessIO::store_pgn(std::cout, moves, result, hdrs);
                }
            }
        }
    }

    return 0;
}
