// Copyright 1996 by Jon Dart.  All Rights Reserved
#ifndef __BOOKDEFS_H__
#define __BOOKDEFS_H__

// definitions and constants related to the internal format
// of the opening book (BOOK.BIN)

#include "types.h"
#include "bookentr.h"

#define INVALID (uint16)0xffff

#define Entry_Size 17
#define BookVersion 11

// Header of the opening book.
struct BookHeader
{
   byte version;
   byte num_pages;
   uint32 page_capacity;
   uint32 hash_table_size;
};

// Information needed to specify the location
// of a single book entry
struct BookLocation
{
   BookLocation(int p, uint16 i)
    :page(p),index(i) {
    }
   BookLocation()
    :page(0),index(0) {
    }
   int page;
   uint16 index;
};

#define Header_Size sizeof(BookHeader)
#define MAIN_BOOK_NAME "book.bin"
#define MAIN_MAPPING_NAME "Arasan opening book"
#define POSITION_BOOK_NAME "position.bin"
#define POSITION_MAPPING_NAME "Arasan learning book"

#endif
