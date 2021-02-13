// Copyright 1996-2014, 2018 by Jon Dart.  All Rights Reserved
#ifndef __BOOKDEFS_H__
#define __BOOKDEFS_H__

// definitions and constants related to the internal format
// of the opening book (BOOK.BIN)

#include "types.h"
#include <cstring>

namespace book {

const int BOOK_VERSION = 15;

const int INDEX_PAGE_SIZE = 1024;
const int DATA_PAGE_SIZE = 2048;
const uint16_t NO_NEXT = 65535;
const uint16_t INVALID_INDEX = 65535;
const uint8_t NO_RECOMMEND = 255;
const uint8_t MAX_WEIGHT = 254;

#ifdef __INTEL_COMPILER
#pragma pack(push,1)
#endif

// Header of the opening book.
struct BookHeader
BEGIN_PACKED_STRUCT
   uint8_t version;
   uint16_t num_index_pages;
   BookHeader() : version(0), num_index_pages(0) {
   }
END_PACKED_STRUCT

// Information needed to specify the location
// of the head of a move chain
struct BookLocation
BEGIN_PACKED_STRUCT
   BookLocation(uint16_t p, uint16_t i)
    :page(p),index(i) {
    }
   BookLocation()
    :page(0),index(0) {
    }
    int isValid() const {
       return index != INVALID_INDEX;
    }
    uint16_t page;
    uint16_t index;
END_PACKED_STRUCT

// Entry in the index table
struct IndexEntry : public BookLocation
BEGIN_PACKED_STRUCT
    IndexEntry(hash_t hash, uint16_t p, uint16_t i) :
    BookLocation(p,i),hashCode(hash) {
    }
    IndexEntry() :
    BookLocation(), hashCode(0x0ULL) {
    }
    hash_t hashCode;
END_PACKED_STRUCT

struct IndexPage
BEGIN_PACKED_STRUCT
    uint32_t next_free;
    IndexEntry index[INDEX_PAGE_SIZE];
    IndexPage() : next_free(0) {
    }
END_PACKED_STRUCT

// Entry in a data page
struct DataEntry
BEGIN_PACKED_STRUCT 
    uint8_t index;
    uint8_t weight; // a priori weight, NO_RECOMMEND if not set
    uint16_t next;
    uint32_t win, loss ,draw;

    unsigned count() const {
       return win + loss + draw;
    }

    // ordering by decreasing frequency
    bool operator < (const DataEntry &entr) const noexcept {
       return entr.count() < count();
    }
END_PACKED_STRUCT

struct DataPage
BEGIN_PACKED_STRUCT
    uint32_t free_list;
    uint32_t num_free;
    DataEntry data[DATA_PAGE_SIZE];
    DataPage() : free_list(0), num_free(DATA_PAGE_SIZE) {
        // create the free list and link all entries into it
        memset(data,'\0',sizeof(DataEntry)*DATA_PAGE_SIZE);
        for (unsigned i = 0; i < DATA_PAGE_SIZE-1; i++) {
            data[i].next = static_cast<uint16_t>(i+1);
        }
        data[DATA_PAGE_SIZE-1].next = NO_NEXT;
    }
END_PACKED_STRUCT

#ifdef __INTEL_COMPILER
#pragma pack(pop)
#endif

}

#endif
