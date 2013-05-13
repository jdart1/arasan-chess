// Copyright 1992-2002, by Jon Dart.  All Rights Reserved.

#include "book.h"
#include "movegen.h"
#include "globals.h"
#include "scoring.h"
#include "util.h"
extern "C"
{
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
};

Book::Book()
{
   time_t t;
   srand((unsigned)time(&t));
   reader = new BookReader(MAIN_BOOK_NAME,MAIN_MAPPING_NAME);
}       

Book::~Book()
{
   delete reader;
}

Move Book::book_move( const Board &b, BookInfo &info)
{
   info.clear();
   if (!reader->is_open())
   {
      return NullMove;
   }
   else
   {
      BookLocation loc;
      reader->head(b,loc);
      return reader->pick(b, loc, info);
   }
}

unsigned Book::book_moves( const Board &b, Move *moves, int *scores,
                           const unsigned limit)
{
   if (!reader->is_open())
      return 0;
   return reader->book_moves(b,moves,scores,limit);

}

void Book::update(const BookInfo &info, float learn_factor)
{
   BookLocation loc;
   info.getLocation(loc);
   static char msg[100];

   sprintf(msg,"page = %d index = %d factor = %7.2f\n",loc.page,
           loc.index,learn_factor);
   theLog->write(msg);

   reader->update(loc,learn_factor);
}


