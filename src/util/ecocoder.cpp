// Copyright 1996, 2013, 2014, 2017, 2021-2026 by Jon Dart. All Rights Reserved.

// Utility to add ECO codes to PGN game files.

#include "board.h"
#include "chessio.h"
#include "eco.h"
#include "globals.h"
#include "legal.h"
#include "movearr.h"
#include "movegen.h"
#include "notation.h"
#include "options.h"
#include "scoring.h"

extern "C" {
#include <ctype.h>
#include <stddef.h>
#include <string.h>
};
#include <algorithm>
#include <ctype.h>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

static struct ECOptions {
    ECOptions() : force(false), names(false) {}

    bool force, names;
} opts;

static void show_usage() {
    std::cerr << "Usage: ecocoder [-f (force)] [-n (opening names)] pgn_file" << std::endl;
}

int CDECL main(int argc, char **argv) {
    BitUtils::init();
    Board::init();
    Attacks::init();
    if (!globals::initGlobals()) {
        globals::cleanupGlobals();
        exit(-1);
    }
    atexit(globals::cleanupGlobals);

    std::string ecoPath(globals::derivePath(globals::ECO_DIR));
    if (!globals::eco->init(ecoPath)) {
        std::cerr << "failed to load ECO database from " << ecoPath << std::endl;
        exit(-1);
    }

    // One nesting level of PGN movetext being parsed: "board" is the
    // current position at this level, "boardBeforeLast" is the position
    // before the last move added to "seq" (needed if a variation branches
    // off that move), and "plyAtStart" is the ply (0-based half-move
    // index) of the first move in "seq".
    struct VarFrame {
        Board boardBeforeLast;
        Board board;
        int plyAtStart;
        std::vector<ChessIO::MoveNode> seq;
    };

    Board board;
    int arg = 1;

    for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
        if (strcmp(argv[arg], "-f") == 0) {
            opts.force = true;
        } else if (strcmp(argv[arg], "-n") == 0) {
            opts.names = true;
        } else {
            show_usage();
            exit(-1);
        }
    }
    if (arg >= argc) {
        show_usage();
        exit(-1);
    }
    std::ifstream pgn_file(argv[arg], std::ios::in | std::ios::binary);
    std::string result, white, black;
    if (!pgn_file.good()) {
        std::cerr << "could not open file " << argv[arg] << std::endl;
        exit(-1);
    } else {
        std::vector<ChessIO::Header> hdrs;
        ChessIO::PGNReader pgnReader(pgn_file);
        long first;
        for (;;) {
            bool ok = true;
            bool done = false;
            bool exit = false;
            hdrs.clear();
            pgnReader.collectHeaders(hdrs, first);
            if (!hdrs.size())
                break;
            board.reset();
            MoveArray moves;
            std::vector<VarFrame> varStack;
            varStack.push_back(VarFrame{board, board, 0, {}});
            ChessIO::TokenReader tokenReader(pgnReader);
            // read game body
            while (ok && !exit && !done) {
                ChessIO::Token tok = tokenReader.nextToken();
                switch (tok.type) {
                case ChessIO::Eof: {
                    exit = true;
                    break;
                }
                case ChessIO::Number: {
                    // move numbers are regenerated on output, input value not needed
                    break;
                }
                case ChessIO::GameMove: {
                    VarFrame &top = varStack.back();
                    // parse the move
                    Move m = Notation::value(top.board, top.board.sideToMove(),
                                             Notation::InputFormat::SAN, tok.val);
                    if (IsNull(m) || !legalMove(top.board, m)) {
                        // echo to both stdout and stderr
                        std::cerr << "Illegal move: " << tok.val << std::endl;
                        std::cout << "Illegal move: " << tok.val << std::endl;
                        ok = false;
                    } else {
                        std::string img;
                        // convert to SAN
                        Notation::image(top.board, m, Notation::OutputFormat::SAN, img);
                        if (varStack.size() == 1) {
                            // only the mainline is used for ECO classification
                            moves.add_move(top.board, m, img);
                        }
                        top.boardBeforeLast = top.board;
                        top.board.doMove(m);
                        top.seq.push_back(ChessIO::MoveNode{img, {}, "", {}});
                    }
                    break;
                }
                case ChessIO::Unknown: {
                    std::cerr << "Unrecognized text: " << tok.val << std::endl;
                    break;
                }
                case ChessIO::Comment: {
                    std::string text = tok.val;
                    if (text.size() >= 2 && text.front() == '{' && text.back() == '}') {
                        text = text.substr(1, text.size() - 2);
                    }
                    std::replace(text.begin(), text.end(), '\n', ' ');
                    std::replace(text.begin(), text.end(), '\r', ' ');
                    VarFrame &top = varStack.back();
                    if (!top.seq.empty()) {
                        std::string &comment = top.seq.back().comment;
                        if (!comment.empty())
                            comment += ' ';
                        comment += text;
                    } else {
                        std::cerr << "Warning: misplaced comment, ignored" << std::endl;
                    }
                    break;
                }
                case ChessIO::NAG: {
                    VarFrame &top = varStack.back();
                    if (!top.seq.empty()) {
                        top.seq.back().nags.push_back(tok.val);
                    } else {
                        std::cerr << "Warning: misplaced NAG, ignored" << std::endl;
                    }
                    break;
                }
                case ChessIO::OpenVar: {
                    VarFrame &top = varStack.back();
                    if (top.seq.empty()) {
                        std::cerr << "Warning: misplaced variation start, ignored" << std::endl;
                        varStack.push_back(VarFrame{top.board, top.board, top.plyAtStart, {}});
                    } else {
                        int plyAtStart = top.plyAtStart + (int)top.seq.size() - 1;
                        varStack.push_back(
                            VarFrame{top.boardBeforeLast, top.boardBeforeLast, plyAtStart, {}});
                    }
                    break;
                }
                case ChessIO::CloseVar: {
                    if (varStack.size() <= 1) {
                        std::cerr << "Warning: unmatched ')', ignored" << std::endl;
                    } else {
                        std::vector<ChessIO::MoveNode> finished = std::move(varStack.back().seq);
                        varStack.pop_back();
                        if (!finished.empty() && !varStack.back().seq.empty()) {
                            varStack.back().seq.back().variations.push_back(std::move(finished));
                        }
                    }
                    break;
                }
                case ChessIO::Result: {
                    result = tok.val;
                    done = true;
                    break;
                }
                default:
                    break;

                } // end switch
            }
            // close any variations left open by malformed/truncated input
            while (varStack.size() > 1) {
                std::vector<ChessIO::MoveNode> finished = std::move(varStack.back().seq);
                varStack.pop_back();
                if (!finished.empty() && !varStack.back().seq.empty()) {
                    varStack.back().seq.back().variations.push_back(std::move(finished));
                }
            }
            // output headers
            std::string ecoC, name;
            globals::eco->classify(moves, ecoC, name);
            std::string currentECO;
            bool found = ChessIO::get_header(hdrs, "ECO", currentECO);
            if (!found || (opts.force && ecoC != "")) {
                if (opts.force) {
                    if (name == "") {
                        ChessIO::remove_header(hdrs, "ECO");
                    } else if (found) {
                        ChessIO::replace_header(hdrs, "ECO", ecoC);
                    } else {
                        hdrs.push_back(ChessIO::Header("ECO", ecoC));
                    }
                } else if (!found && name != "") {
                    hdrs.push_back(ChessIO::Header("ECO", ecoC));
                }
            }
            if (opts.names) {
                std::string currentOpening;
                found = ChessIO::get_header(hdrs, "Opening", currentOpening);
                if (opts.force) {
                    if (name == "") {
                        ChessIO::remove_header(hdrs, "Opening");
                    } else if (found) {
                        ChessIO::replace_header(hdrs, "Opening", name);
                    } else {
                        hdrs.push_back(ChessIO::Header("Opening", name));
                    }
                } else if (!found && name != "") {
                    hdrs.push_back(ChessIO::Header("Opening", name));
                }
            }
            if (moves.num_moves() > 0)
                ChessIO::store_pgn(std::cout, varStack.front().seq, result, hdrs);
        }
    }
    return 0;
}
