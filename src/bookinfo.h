// Copyright 1998 by Jon Dart.  All Rights Reserved.

#ifndef _BOOK_INFO_H
#define _BOOK_INFO_H

#include "types.h"
#include "bookdefs.h"
#include "bookentr.h"
#include "chess.h"

class BookInfo : public BookEntry {

     // This class is an extension of BookEntry. It is returned
     // when we search the book and has some information besides
     // what was in the book entry on disk.

public:

     BookInfo()
     :  BookEntry(),
        total_moves(0),
        loc(0,0),
        m(NullMove)
     {
     }

     BookInfo( hash_t hc, byte rec, byte mv_indx,
	         int last = 0)
        : BookEntry(hc,rec,mv_indx,last),total_moves(0),
          m(NullMove)
     {
        loc.index = INVALID;
     }

     BookInfo(const BookEntry &be) 
       :BookEntry(be),
       total_moves(0), 
       m(NullMove)
     {
     }

     void clear() {
        my_hash_code = (hash_t)0;
     }
     
     int isValid() const {
        return my_hash_code != (hash_t)0;
     }
     
     int getTotalMoves() const {
        return total_moves;
     }
     
     void setTotalMoves(int moves) {
        total_moves = moves;
     }
     
     void getLocation(BookLocation &locn) const {
        locn = loc;
     }
     
     void setLocation(const BookLocation &locn) {
        loc = locn;
     }
     
     Move getMove() const {
        return m;
     }
     
     void setMove(const Move move) {
        m = move;
     }

private:
     
     int total_moves;
     BookLocation loc; // head of the hash chain
     Move m; // move selected
};

#endif
