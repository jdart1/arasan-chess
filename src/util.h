// Copyright 1994, 1997, 2015 by Jon Dart.  All Rights Reserved.

#ifndef _UTIL_H
#define _UTIL_H

#include "types.h"
#include "bitboard.h"

extern "C" {
#include <math.h>
};

struct BIGINT
{
  uint32 low;
  uint32 high;
};

union INT64UNION
{
  BIGINT b;
  hash_t u;
};

FORCEINLINE int32 quickmod(int64 dividend, int32 divisor)
{
   // Compute a 32-bit derived hash code between 0..divisor.
   // Avoid a 64-bit division, to save time.
   INT64UNION un;
   un.u = dividend;
   return (int32)(un.b.low % divisor);
}

class Util
{ 
    // commonly used utility functions
	    
    public:	
	
    static int Abs( const int x )
    {
	 return (x < 0) ? -x : x;    
    }
    
    static int Sign( const int x )
    {
	 if (x==0)
	     return 0;
	 else
	     return (x > 0) ? 1 : -1;
    }
    
    static int Max( int x, int y )
    {
	return (x > y) ? x : y;    
    }
  
    static int Min( int x, int y )
    {
	return (x > y) ? y : x;    
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
