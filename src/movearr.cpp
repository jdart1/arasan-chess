// Copyright 1997-2002, 2008, 2012, 2017, 2021, 2024 by Jon Dart. All Rights Reserved.
//
#include "movearr.h"
#include "boardio.h"
#include "movegen.h"

#include <sstream>

MoveRecord::MoveRecord(const Board &board, const Move m,
                       const std::string &img, bool was_ponder, bool from_book, score_t s, unsigned d)
    : hashcode(board.state.hashCode), move(m), image(img),
      ponder(was_ponder), fromBook(from_book), score(s), depth(d) {
    std::stringstream buf;
    BoardIO::writeFEN(board, buf, 0);
    fen = buf.str();
}

MoveRecord::MoveRecord(const Board &board, const Move &m, const std::string &img,
                       const Statistics *stats, bool was_ponder)
    : MoveRecord(board, m, img, was_ponder, stats == nullptr ? false : stats->fromBook,
                 stats == nullptr ? 0 : stats->display_value,
                 stats == nullptr ? 0 : stats->completedDepth.load()) {
}

