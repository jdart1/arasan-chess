// Copyright 1993-1995, 2001, 2006, 2013 by Jon Dart.  All Rights Reserved.

#include "bookentr.h"

const int BookEntry::NO_RECOMMEND = 101;

BookEntry::BookEntry( hash_t hc, byte rec, byte mv_indx, 
   int last )
  : my_hash_code(hc), 
    move_index(mv_indx), flags(last ? 0x80 : 0), winloss(0), 
    learn_score(0.0), flags2(0), weight(rec)
{
}

int BookEntry::is_last() const
{
   return (flags & 0x80) != 0;
}

void BookEntry::set_frequency(uint32 f)
{
   if (f >= 65536) {
     frequency = 255;
     flags |= 0x40;
     flags &= ~0x20;
   }
   else if (f >= 4096)
   {
      frequency = f/256;
      flags |= 0x40;
      flags &= ~0x20;
   }
   else if (f >= 256)
   {
      frequency = f/32;
      flags &= ~0x40;
      flags |= 0x20;
   }
   else
   {
      frequency = f;
      flags &= ~0x40;
      flags &= ~0x20;
   }
}

uint32 BookEntry::get_frequency() const
{
   if (flags & 0x40)
   {
      return frequency*256;
   }
   else if (flags & 0x20)
   {
      return frequency*32;
   }
   else
     return frequency;
}


void BookEntry::set_last(int b)
{
   if (b)
      flags |= 0x80;
   else
      flags &= ~0x80;
}

unsigned BookEntry::get_recommend() const
{
   return (unsigned)weight;
}

void BookEntry::set_basic(int b) {
  if (b) flags2 |= 0x80;
  else flags2 &= ~0x80; 
}

int BookEntry::is_basic() const {
  return (flags2 & 0x80);
}

int BookEntry::get_learned_result()  const {
  unsigned x = (unsigned)(flags2 & 0x7f);
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
  flags2 = flags2 & 0x80;
  if (y < 0)
    flags2 |= (byte)(128+y);
  else
    flags2 |= (byte)y;
}

