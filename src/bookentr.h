// Copyright 1993, 2013 by Jon Dart. All Rights Reserved.

#ifndef _BOOK_ENTRY_H
#define _BOOK_ENTRY_H

#include "types.h"

class BookEntry {
     // defines one entry in the opening book's hash table.

public:

     static const int NO_RECOMMEND;

     BookEntry()
       :my_hash_code(0),frequency(0),move_index(0),flags(0),
        winloss(0),learn_score(0.0),flags2(0),weight(0)
     {
     }

     BookEntry( hash_t hc, byte rec, byte mv_indx,
	         int last = 0);

     hash_t hash_code() const
     {
        return my_hash_code;
     }

     void set_hash_code( hash_t h ) {
        my_hash_code = h;
     }
     
     byte get_move_index() const
     { 
	    return move_index;
     }
     
     int get_winloss() const
     {
        return (int)winloss;
     }
     
     void set_winloss(int w)
     {
        winloss = w;
     }
     
     uint32 get_frequency() const;
     
     void set_frequency(uint32 f);
     
     int is_last() const;

     void set_last(int b = 1);

     unsigned get_recommend() const;

     void set_basic(int b = 1);

     int is_basic() const;

     int get_learned_result() const;

     void update_learned_result(int diff);

     int operator == (const BookEntry &b) {
       return (my_hash_code == b.my_hash_code);
     }

#ifdef _MSC_VER
#pragma pack(push,1)
#endif
     hash_t my_hash_code;
     byte frequency;
     byte move_index;
     byte flags;
     signed char winloss;
     float learn_score;
     byte flags2;
     // manual tuned weight, if any 0-100
     byte weight;
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
