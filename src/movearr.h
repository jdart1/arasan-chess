// Copyright 1997-2002, 2008, 2012, 2017, 2021 by Jon Dart. All Rights Reserved.a
//
#ifndef _MOVE_ARRAY_H
#define _MOVE_ARRAY_H

#include "board.h"

#include <cassert>
#include <string>
#include <vector>

class MoveRecord
{
    // holds info on a move made during the game or during a search
    // operation.

public:

    MoveRecord()
        : my_move(NullMove),my_hashcode((hash_t)0ULL), my_ponder(false) {
    }

    // "board" is the board position after the move.
    MoveRecord(const Board &board, const BoardState &previous_state,
               const Move &move, const std::string &image, bool ponder);

    const Move &move() const {
        return my_move;
    }

    hash_t hashcode() const {
        return my_hashcode;
    }

    const BoardState &state() const {
        return my_state;
    }

    bool operator < (const MoveRecord &mr) const {
        return my_hashcode < mr.my_hashcode;
    }

    int operator == ( const MoveRecord &l ) const {
        return my_hashcode == l.my_hashcode;
    }

    int operator != ( const MoveRecord &l ) const {
        return my_hashcode != l.my_hashcode;
    }

    const std::string & image () const {
        return my_image;
    }

    bool wasPonder() const {
        return my_ponder;
    }

private:

    Move my_move;
    hash_t my_hashcode;
    BoardState my_state;
    std::string my_image;
    bool my_ponder; // true if move was added provisionally during a ponder operation
};

class MoveArray
{
    // Maintains a list of moves made in the game so far or in
    // the search process.

public:

    MoveArray() {
    }

    // add a move to the Move_Array. "board" is the position after the
    // move is made.
    void add_move( const Board &board, const BoardState &previous_state,
                   const Move &emove, const std::string &image, bool ponder );

    // remove the most recently added move to the Move_Array.
    void remove_move();

    // return the total number of half-moves in the game:
    unsigned num_moves() const {
        return (unsigned)entries.size();
    }

    unsigned num_moves(const ColorType side) const;

    // return the nth move in the Move_Array.  0 <= n <= num_moves - 1.
    const Move &move( unsigned n ) const {
        assert(n<num_moves());
        return entries[n].move();
    }

    const MoveRecord &operator [](size_t index) const {
        return entries[index];
    }

    void append(const MoveRecord &m) {
        entries.push_back(m);
    }

    void removeAll() {
        entries.clear();
    }

    const MoveRecord &last() const {
        assert(num_moves());
        return entries.back();
    }

    void truncate(unsigned size) {
        entries.resize(size);
    }

private:

    std::vector<MoveRecord> entries;

};

#endif

