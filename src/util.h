// Copyright 1994, 1997, 2015, 2017 by Jon Dart.  All Rights Reserved.

#ifndef _UTIL_H
#define _UTIL_H

#include "types.h"
#include "bitboard.h"

extern "C" {
#include <math.h>
};

class Util
{ 
    // commonly used utility functions
	    
    public:	
	
    static int Sign( const int x )
    {
	 if (x==0)
	     return 0;
	 else
	     return (x > 0) ? 1 : -1;
    }
    
    static int Round(double x) {
#if defined(_MSC_VER) && __cplusplus < 201103L
      // MSVC lacks round()
      return int((x >= 0.0) ? floor(x + 0.5) : ceil(x - 0.5));
#else
      return round(x);
#endif
    }
};

#endif
