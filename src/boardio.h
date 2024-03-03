// Copyright 1997, 2013-2014, 2021, 2024 by Jon Dart.
#ifndef __BOARDIO_H__
#define __BOARDIO_H__

#include "board.h"
#include <fstream>

class BoardIO {
  public:
    static int readFEN(Board &board, const std::string &buf);

    static void writeFEN(const Board &board, std::ostream &out, bool addMoveInfo);

    static void writeFEN(const Board &board, std::string &out, bool addMoveInfo);
};

#endif
