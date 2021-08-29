// Copyright 1997 by Jon Dart.
#ifndef __BOARDIO_H__
#define __BOARDIO_H__

#include "board.h"
#include <fstream>
class BoardIO
{
public:
    static int readFEN(Board &board, const std::string &buf);
    static void writeFEN(const Board &board, std::ostream &out, int addMoveInfo);

};

#endif
