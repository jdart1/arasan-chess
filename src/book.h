// Copyright 1992-9 by Jon Dart.  All Rights Reserved.

#ifndef _BOOK_H
#define _BOOK_H

#include "bookread.h"
#include "bookdefs.h"
#include "log.h"
#include <fstream>

class Book
{
    // interface to the opening book.

    public:
		
    Book();
	
    ~Book();
		
    // Return a "book move" if there is one, NullMove otherwise.
    // NOTE: the promotion field of book_move may be wrong;
    // we don't set it.
    Move book_move( const Board &b, BookInfo &inf);
		
    // Fill array "moves" with a list of all the book moves for
    // the given position.  Moves are sorted in decreasing order
    // of "goodness".  Moves that are not recommended at all are
    // are not included.  "limit" is the maximum number of moves
    // that can be stored in "moves".  The return value is the
    // number actually stored.	"scores" is filled with the
    // score (0-100) for each move.
    unsigned book_moves( const Board &b, Move *moves, int *scores,
    const unsigned limit);
    
    int find_move( const Move &target, unsigned &node);
    void update(const BookInfo &info, float learn_factor);
	
    // Updates a book position with a new learn value (used for
    // game result learning)
    void update(BookInfo *newEntry) {
       BookLocation loc;
       newEntry->getLocation(loc);
       reader->update(loc,newEntry);
    }

    BookReader *reader;
    
};

#endif

