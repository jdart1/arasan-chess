// Copyright 1994, 1995 by Jon Dart.  All Rights Reserved.
#ifndef __SEARCHOP_H__
#define __SEARCHOP_H__

#include "types.h"
#include "timectrl.h"
#include <string>

using namespace std;

struct Search_Limit_Options
{
   // holds state of the dialog
   Search_Limit_Options();
   Search_Limit_Options( const Time_Control &tc, const Control c );

   BOOL parse( Time_Control &tc, const Control c );

   int operator == (const Search_Limit_Options &) const;
   int operator != (const Search_Limit_Options &) const;

   int search_type;
   int var1;                                      //seconds if fixed time, minutes for incremental, moves for tournament
   int var2;                                      //increment for incremental, minutes for tournament

};
#endif
