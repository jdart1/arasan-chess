// Copyright 1993-1995, 2001, 2006, 2013 by Jon Dart.  All Rights Reserved.

#include "bookentr.h"

const unsigned BookEntry::NO_RECOMMEND = 1025;

const int BookEntry::MAX_WEIGHT = 1024;

BookEntry::BookEntry( hash_t hc, byte rec, byte mv_indx, 
   int last )
  : my_hash_code(hc), 
    learn_score(0.0),
    move_index(mv_indx),
    learned_result(0),
    weight(rec)
{
   set_last(last);
}

int BookEntry::get_learned_result()  const {
  unsigned x = (unsigned)(learned_result & 0x7f);
  if (x & 0x40)
    return -(int)(128-x);
  else
    return (int)x;
}

void BookEntry::update_learned_result(int diff) {
  int x = get_learned_result();
  int y = x + diff;
  if (y >= 64 || y <= -64)
    return;
  learned_result &= 0x80;
  if (y < 0)
    learned_result |= (byte)(128+y);
  else
    learned_result |= (byte)y;
}

