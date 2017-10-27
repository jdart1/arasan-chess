// Copyright 1995 by Jon Dart. All Rights Reserved.
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <iostream>
#include <assert.h>

using namespace std;

#undef ASSERT

// This header provides a macro (UNCOND_ASSERT) that can be used to insert an
// unconditional assert into the code.  I.e. the assert is present even
// in non-debug builds (unlike the MFC ASSERT macro).

#ifdef _DEBUG
#define ASSERT(e) assert(e)
#if !defined(_WIN32)
#define UNCOND_ASSERT(e) assert(e)
#else
extern void uncond_assert(const char *expr, const char *file, const int line);
#define UNCOND_ASSERT(e) if (!(e)) uncond_assert(  #e, __FILE__, __LINE__)
#endif
#else
#define ASSERT(e)
#define UNCOND_ASSERT(e)
#endif

#ifndef _WIN32
#ifdef _TRACE
#define TRACE(s) printf(s);
#define TRACE2(s1,s2) printf(s1,s2)
#else
#define TRACE(s)
#define TRACE2(s1,s2)
#endif
#endif

//#ifdef _TRACE
// this is used to support tracing.
inline void indent(const int ply)
{
   for (int k = 0; k < ply; k++) cout << ' ';
}
//#endif

#endif
