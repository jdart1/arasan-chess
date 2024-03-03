// Copyright 1997-2002, 2008, 2012, 2017, 2021, 2024 by Jon Dart. All Rights Reserved.a
//
#ifndef _MOVE_ARRAY_H
#define _MOVE_ARRAY_H

#include "board.h"
#include "stats.h"

#include <cassert>
#include <string>
#include <vector>

struct MoveRecord {
    // holds info on a move made during the game or during a search
    // operation.

    MoveRecord()
        : hashcode((hash_t)0ULL), move(NullMove), ponder(false), fromBook(false), score(0),
          depth(0) {}

    // "board" is the board position after the move.
    MoveRecord(const Board &board, const Move m,
               const std::string &img, bool was_ponder, bool from_book, score_t s, unsigned d);

    MoveRecord(const Board &board, const Move &m,
               const std::string &img, const Statistics *stats, bool was_ponder);

    virtual ~MoveRecord() = default;

    bool operator<(const MoveRecord &mr) const { return hashcode < mr.hashcode; }

    int operator==(const MoveRecord &mr) const { return hashcode == mr.hashcode; }

    int operator!=(const MoveRecord &mr) const { return hashcode != mr.hashcode; }

    bool wasPonder() const { return ponder; }

    std::string fen;
    hash_t hashcode;
    Move move;
    std::string image;
    bool ponder; // true if move was added provisionally during a ponder operation
    bool fromBook;
    score_t score;
    unsigned depth;
};

class MoveArray : public std::vector<MoveRecord> {
    // Maintains a list of moves made in the game so far or in
    // the search process.

  public:
    MoveArray() : gameResult("*") {}

    // add a move to the Move_Array. "board" is the position before making the move
    void add_move(const Board &board, Move m, const std::string &img, bool was_ponder,
                  bool from_book, score_t s = 0, int d = 0) {
        push_back(MoveRecord(board, m, img, was_ponder, from_book, s, d));
    }

    void add_move(const Board &board, Move m, const std::string &img, const Statistics *stats = nullptr, bool was_ponder = false) {
        push_back(MoveRecord(board, m, img, stats, was_ponder));
    }

    // remove the most recently added move to the Move_Array.
    void remove_last() {
        if (size() > 0) {
            pop_back();
        }
    }

    // return the total number of half-moves in the game:
    unsigned num_moves() const { return (unsigned)size(); }

    unsigned num_moves(const ColorType side) const;

    // return the nth move in the Move_Array.  0 <= n <= num_moves - 1.
    const Move &move(unsigned n) const {
        assert(n < num_moves());
        return (*this)[n].move;
    }

    // true if the specified board position results from a legal move
    // made from the last entered position in the array.
    bool derivesFromLast(const Board &b);

    const std::string &getResult() const noexcept {
        return gameResult;
    }

    void setResult(const std::string &result) noexcept {
        gameResult = result;
    }

    void reset() {
        clear();
        gameResult = "*";
    }
    
private:
    std::string gameResult;
};

#endif
