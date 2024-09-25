// Copyright 2024 by Jon Dart. All Rights Reserved.
#include "binformat.h"
#include "board.h"
#include "boardio.h"
#include "chessio.h"
#include "globals.h"
#include "legal.h"
#include "notation.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <list>

// Converts between binary training formats

static void usage() {
    std::cerr << "convertbin -from <format> -to <format> -o output_file input file(s)" << std::endl;
}

template <BinFormats::Format from, BinFormats::Format to>
static bool convert(std::istream &bin_file, std::ostream &out, uint64_t &position) {
    int readResult;
    BinFormats::PositionData readData;
    while (BinFormats::read<from>(bin_file, readResult, readData)) {
        if (!BinFormats::write<to>(readData, readResult, out)) {
            std::cerr << "write error at position " << position << std::endl;
            return false;
        }
        ++position;
    }
    bool result = bin_file.eof() || !bin_file.fail();
    if (!result) {
        std::cerr << "read error at position " << position << std::endl;
    }
    return result;
}

int CDECL main(int argc, char **argv) {
    Bitboard::init();
    Board::init();
    globals::initOptions();
    Attacks::init();
    globals::options.search.hash_table_size = 64 * 1024 * 1024;
    /* Not needed since we use neither book nor TBs
    if (!globals::initGlobals()) {
        globals::cleanupGlobals();
        exit(-1);
    }
    */
    atexit(globals::cleanupGlobals);
    globals::options.book.book_enabled = false;
    globals::delayedInit();

    BinFormats::Format from = BinFormats::Format::StockfishBin;
    BinFormats::Format to = BinFormats::Format::Marlin;
    int formats = 0;
    std::string outname;
    int arg = 1;
    for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
        if (strcmp(argv[arg], "-from") == 0) {
            ++arg;
            if (arg < argc) {
                if (BinFormats::fromString(argv[arg], from)) {
                    ++formats;
                } else {
                    std::cerr << "Unrecognized 'from' format: " << argv[arg] << std::endl;
                    exit(-1);
                }
            }
        } else if (strcmp(argv[arg], "-to") == 0) {
            ++arg;
            if (arg < argc) {
                if (BinFormats::fromString(argv[arg], to)) {
                    ++formats;
                } else {
                    std::cerr << "Unrecognized 'to' format: " << argv[arg] << std::endl;
                    exit(-1);
                }
            }
        } else if (strcmp(argv[arg], "-o") == 0) {
            ++arg;
            if (arg < argc)
                outname = argv[arg];
        } else {
            std::cerr << "Unrecognized switch: " << argv[arg] << std::endl;
            exit(-1);
        }
    }
    if (formats != 2) {
        usage();
        exit(-1);
    }
    if (outname == "") {
        std::cerr << "no output file specified" << std::endl;
        usage();
        exit(-1);
    }
    for (; arg < argc; ++arg) {
        std::ifstream bin_file(argv[arg], std::ios::in | std::ios::binary);
        if (!bin_file.good()) {
            std::cerr << "could not open file " << argv[arg] << std::endl;
            exit(-1);
        } else {
            std::cout << argv[arg] << std::endl;
            uint64_t position = 0;
            BinFormats::PositionData readData;
            std::ofstream out_file(outname, std::ios::binary | std::ios::trunc);
            bool supported = false, result = false;
            switch (from) {
            case BinFormats::Format::StockfishBin:
                switch (to) {
                case BinFormats::Format::Marlin:
                    supported = true;
                    result = convert<BinFormats::Format::StockfishBin, BinFormats::Format::Marlin>(
                        bin_file, out_file, position);
                    break;
                case BinFormats::Format::Bullet:
                    supported = true;
                    result = convert<BinFormats::Format::StockfishBin, BinFormats::Format::Bullet>(
                        bin_file, out_file, position);
                    break;
                default:
                    break;
                }
            case BinFormats::Format::Marlin:
                break;
            default:
                break;
            }
            if (!supported) {
                std::cerr << "unsupported conversion" << std::endl;
                exit(-1);
            }
            if (!result) {
                std::cerr << "error in conversion of file " << argv[arg] << " at position "
                          << position << std::endl;
                exit(-1);
            }
        }
    }

    return 0;
}
