// Copyright 2014 by Jon Dart.  All Rights Reserved.

#ifndef _BOOK_WRITER_H
#define _BOOK_WRITER_H

#include "bookdefs.h"
#include "board.h"
#include <vector>
#include <exception>

using namespace std;

class BookFullException: public exception
{
 public:
  virtual const char* what() const throw()
  {
    return "too many moves in book, allocate more index space";
  }
};

class BookWriter {

        // writes to the opening book

        public:

        // "Pages" is the number of index pages.
        BookWriter(int index_pages);
                
        // closes book file
        ~BookWriter();
                
        // add a move to the file.
        void add(const hash_t hashCode, byte moveIndex, uint16_t weight,
                 int32_t count);

        // Write book contents out to the designated path. Returns 0
        // if no errors, -1 if error
        int write(const char* pathName);

 protected:
       int index_pages;
       book::IndexPage **index;
       vector<book::DataPage*> data;
};

#endif
