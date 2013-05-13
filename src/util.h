// Copyright 1994, 1997 by Jon Dart.  All Rights Reserved.

#ifndef _UTIL_H
#define _UTIL_H

#include "types.h"
#include "bitboard.h"

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

};

#endif
