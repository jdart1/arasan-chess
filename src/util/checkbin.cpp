// Copyright 2024 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "boardio.h"
#include "binformat.h"
#include "notation.h"
#include "legal.h"
#include "globals.h"
#include "chessio.h"
#include <iostream>
#include <fstream>
#include <cctype>

// Sanity-checks .bin files (position files for training)

static void usage() 
{
   std::cerr << "checkbin bin_file(s)" << std::endl;
}

int CDECL main(int argc, char **argv)
{
   Bitboard::init();
   Board::init();
   globals::initOptions();
   Attacks::init();
   globals::options.search.hash_table_size = 64*1024*1024;
   if (!globals::initGlobals()) {
       globals::cleanupGlobals();
       exit(-1);
   }
   atexit(globals::cleanupGlobals);
   globals::options.book.book_enabled = false;
   globals::delayedInit();

   if (argc == 1) {
      usage();
      return -1;
   }
   else {
      for (int arg = 1; arg < argc; ++arg) {
         std::ifstream bin_file( argv[arg], std::ios::in | std::ios::binary);
         if (!bin_file.good()) {
            std::cerr << "could not open file " << argv[arg] << std::endl;
            exit(-1);
         }
         else {
             std::cout << argv[arg] << std::endl;
             uint64_t position = 0;
             BinFormats::PositionData readData;
             int readResult;
             Board board;
             while (BinFormats::read<BinFormats::Format::StockfishBin>(bin_file, readResult, readData)) {
                 if (!BoardIO::readFEN(board,readData.fen)) {
                     std::cerr << "position #" << position << ' ' << readData.fen << " : invalid position" << std::endl;
                 }
                 else {
                     if (!OnBoard(board.kingSquare(White)) || !OnBoard(board.kingSquare(Black)) ||
                         board[board.kingSquare(White)] != WhiteKing ||
                         board[board.kingSquare(Black)] != BlackKing) {
                         std::cerr << "position #" << position << ' ' << readData.fen << " : invalid position" << std::endl;
                     }
                     else {
                         Bitboard kings;
                         kings.set(board.kingSquare(White));
                         kings.set(board.kingSquare(Black));
                         if (Attacks::king_attacks[board.kingSquare(White)] & kings) {
                             std::cerr << "position #" << position << ' ' << readData.fen << " : invalid position" << std::endl;
                         }
                     }
                 }
                 if (!legalMove(board,readData.move)) {
                     std::cerr << "position #" << position << ' ' << readData.fen << " : invalid move ";
                     MoveImage(readData.move,std::cerr);
                     std::cerr << std::endl;
                 }
                 ++position;
             }
         }
      }
   }

   return 0;
}
