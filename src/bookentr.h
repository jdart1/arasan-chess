// Copyright 1993, 2013 by Jon Dart. All Rights Reserved.

#ifndef _BOOK_ENTRY_H
#define _BOOK_ENTRY_H

#include "types.h"

class BookEntry {
     // defines one entry in the opening book's hash table.

public:

     static const unsigned NO_RECOMMEND;

     static const int MAX_WEIGHT;

     BookEntry()
       :my_hash_code(0),learn_score(0.0),move_index(0),
        learned_result(0),
        weight(0)
     {
     }

     BookEntry( hash_t hc, byte rec, byte mv_indx,
	         int last = 0);

     hash_t hashCode() const {
        return my_hash_code;
     }

     void setHashCode( hash_t h ) {
        my_hash_code = h;
     }
     
     byte get_move_index() const { 
        return move_index;
     }
     
     int is_last() const {
       return weight & 0x8000;
     }

     void set_last(int b = 1) {
       weight &= 0x7fff;
       if (b) {
         weight |= 0x8000;
       }
     }

     unsigned getWeight() const {
       return (unsigned)(weight & 0x7fff);
     }

     void setWeight(unsigned w) {
       weight &= (uint16)~0x7fff;
       weight |= (uint16)w;
     }

     int get_learned_result() const;

     void update_learned_result(int diff);

     int operator == (const BookEntry &b) {
        return (my_hash_code == b.my_hash_code);
     }

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
     hash_t my_hash_code;
     float learn_score;
     byte move_index;
     byte learned_result;
     // manual tuned weight, if any
     uint16 weight;
#ifdef _MSC_VER
#pragma pack(pop)
#endif
}
#ifndef _MSC_VER
__attribute__((__packed__))
#endif
;

#define Entry_Size sizeof(BookEntry)

#endif
