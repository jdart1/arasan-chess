// Copyright 1992, 1995 by Jon Dart.  All Rights Reserved.

#ifndef _BOOK_READER_H
#define _BOOK_READER_H

#include "bookdefs.h"
#include "bookinfo.h"
#include "board.h"
#include "hash.h"
#include "bookutil.h"

class BookReader
{
        // provides access to the opening book.

        public:

        BookReader(const char* fileName, const char* mappingName,
          bool create = false);
        // opens book file
                
        ~BookReader();
        // closes book file
                
        bool is_open() const {
           return open;
        }
                
        // Set "loc" to the location of the head of the hash chain
        // for the board position.  If style is non-zero, selects moves
        // matching a particular style.
        void head( const Board &b, BookLocation &loc);

        // Fetches the book entry from the file. "loc" is the location.
        // Reads need not be sequential.
        void fetch( const BookLocation &loc, BookInfo &book_entry );
        
        // Updates a book position with a new weight.
        void update(const BookLocation &loc, float learn_factor);
        
        // Updates a book position with a new learn value (used for
        // game result learning)
        void update(const BookLocation &loc,BookEntry *newEntry);

        // Randomly pick a move for board position "b". "loc" is
        // the starting location, obtained from the head function.
        // "info" is filled in with information on the move chosen.
        Move pick( const Board &b, const BookLocation &loc,
          BookInfo &info);

       void syncCurrentPage();
        
       int book_moves(const Board &b, Move *moves, int *scores,  const unsigned
       limit);

       int book_move_count(hash_t hashcode);
       
       protected:
               
       void head( hash_t hashcode, BookLocation &loc);

       Move pickRandom(const Board &b, BookEntry * candidates,
          int * candidate_weights,
          int candidate_count,int total_weight,
          BookLocation *locs,BookInfo &info);

       FILE_HANDLE book_file;
       void *pBook;
       byte *pPage;
       MEM_HANDLE hFileMapping;
       bool open;
       int current_page;
       BookHeader hdr;
       
       private:
           
       void fetch_page(int page);
       BookUtil bu;
};

#endif
