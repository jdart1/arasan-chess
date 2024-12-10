// Copyright 1994, 1995 by Jon Dart.  All Rights Reserved.

#include "searchop.h"
#include "constant.h"

Search_Limit_Options::Search_Limit_Options()
{
}


int Search_Limit_Options::operator == (const Search_Limit_Options &opts) const
{
   if (search_type == 0)
      return opts.search_type == 0 && opts.var1 == var1;
   else
      return (search_type == opts.search_type &&
         var1 == opts.var1 &&
         var2 == opts.var2);
}


int Search_Limit_Options::operator != (const Search_Limit_Options &opts) const
{
   if (search_type == 0)
      return (opts.search_type != 0) || (opts.var1 != var1);
   else
      return (search_type != opts.search_type ||
         var1 != opts.var1 ||
         var2 != opts.var2);
}
