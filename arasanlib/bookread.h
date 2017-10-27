// Copyright 1992, 1995, 2013, 2014, 2017 by Jon Dart.  All Rights Reserved.

#ifndef _BOOK_READER_H
#define _BOOK_READER_H

#include "bookdefs.h"
#include "board.h"
#include "hash.h"
#include <fstream>
#include <random>
#include <vector>

using namespace std;

class BookReader
{
    // provides read access to the opening book.

 public:

    BookReader();

    ~BookReader();
                
    // opens the book. Returns 0 if success
    int open(const char* pathName);

    // closes the book file.
    void close();

    bool is_open() const {
        return book_file.is_open();
    }
                
    // Randomly pick a move for board position "b". 
    Move pick( const Board &b);

    // Return a vector of all book moves and associated scores,
    // for a given position.
    // Returns number of moves found.
    int book_moves(const Board &b, vector< pair<Move,int> > &results);

protected:
               
    // Return the move data structures for a given board position.
    // Return value is # of entries retrieved, -1 if error.
    int lookup(const Board &board, vector<book::DataEntry> &results);

    int filterAndNormalize(const Board &board,
                           vector<book::DataEntry> &rawMoves,
                           vector< pair<Move,int> > &moves);

    Move pickRandom(const Board &b, const vector< pair<Move,int> > &moves);

    ifstream book_file;
    book::BookHeader hdr;

    std::mt19937_64 engine;
};

#endif
