// Copyright 1997-2002, 2008, 2012, 2017, 2021, 2024 by Jon Dart. All Rights Reserved.
//
#include "movearr.h"
#include "boardio.h"
#include "movegen.h"
#include "globals.h"

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

void MoveArray::add(const MoveRecord &entry)
{
   bool replaced = false;
   // moves are always added at the current position
   if (current >= size()) {
       push_back(entry);
       current = size();
   }
   else {
       const auto &prev = at(current);
       replaced = (prev.fen == entry.fen) && MovesEqual(prev.move, entry.move);
       (*this)[current] = entry;
   }
   ++current;
   // Adding a move at a given point removes any moves
   // after it in the array. Exception: we replaced a record
   // with the same position and move (can happen during
   // analysis mode for example, as we step forward through
   // the game).
   if (size() > current && !replaced) {
       resize(current);
   }
}
