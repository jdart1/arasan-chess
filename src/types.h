// Copyright 1993-2009, 2012-2014, 2016 by Jon Dart.  All Rights Reserved.

#ifndef _TYPES_H
#define _TYPES_H

// Types and macros for OS, machine, and compiler portability

#define USE_SPINLOCK

// Spinlock appears not to work under address sanitizer

// Detect address sanitizer under clang
#if defined(__has_feature)
#  if __has_feature(address_sanitizer)
#undef USE_SPINLOCK
#endif
#endif
// Detect for gcc
#ifdef __SANITIZE_ADDRESS__
#undef USE_SPINLOCK
#endif

extern "C" {
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#if defined(__linux__) || defined(__GLIBC__)
#include <endian.h>
#include <byteswap.h>
#endif
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>
#include <malloc.h>
#else
#include <pthread.h>
#ifdef _MAC
#include <sys/ipc.h>
#include <sys/param.h>
#endif
#endif
};

#include "stdendian.h"

#include <thread>
#include <atomic>

typedef uint8_t byte;
typedef uint64_t hash_t;

#include <iostream>
using namespace std;

#ifdef _WIN32
typedef DWORD CLOCK_TYPE;
#else
typedef unsigned long CLOCK_TYPE;
#endif

#ifdef TUNE
typedef double score_t;
#else
typedef int score_t;
#endif

// Define this here since Bitboard class depends on it
typedef int Square;

enum {InvalidSquare = 127};

#if defined(_WIN64) || defined(__LP64__)
#define _64BIT 1
#else
#undef _64BIT
#endif

#ifndef _GNUC_PREREQ
#define _GNUC_PREREQ(major,minor) (__GNUC__ >= major || ( __GNUC__ == major && __GNUC_MINOR__ >= minor))
#endif

#ifdef _WIN32
#if !defined(_MSC_VER) && !defined(__MINGW32__)
extern "C" {
  #include <Windows32/Defines.h>
  #include <sys/time.h>
  #include <sys/types.h>
  #include <sys/times.h>
};
#endif
#else
extern "C" {
  #include <limits.h>
  #define MAX_PATH PATH_MAX
  #include <sys/time.h>
  #include <sys/types.h>
  #include <sys/times.h>
  #include <unistd.h>
};
#endif

#include <sstream>

inline CLOCK_TYPE getCurrentTime() {
#if defined(_MSC_VER) || defined(__MINGW32__)
  return (CLOCK_TYPE)timeGetTime();
#elif defined(_MAC)
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec*1000 + tv.tv_usec/1000;
#else
  struct timespec timeval;
  if (clock_gettime(CLOCK_REALTIME,&timeval)) {
    perror("clock_gettime");
  }
  return (CLOCK_TYPE)(timeval.tv_sec * 1000 + (timeval.tv_nsec / 1000000));
#endif
}

// get elapsed time in milliseconds
inline unsigned getElapsedTime(CLOCK_TYPE start,CLOCK_TYPE end) {
  return end - start;
}

#ifdef _WIN32
// force _cdecl even if compiler uses fastcall
#undef CDECL
#define CDECL __cdecl
#else
#define CDECL
#endif

#ifdef _MSC_VER

#define CACHE_ALIGN __declspec(align(128))
#define FORCEINLINE __forceinline
#define ALIGNED_MALLOC(ptr, type, size, alignment) ptr = (type*)_aligned_malloc(size, alignment)
#define ALIGNED_FREE(ptr) _aligned_free(ptr)

#elif defined(_MAC)

#define CACHE_ALIGN __attribute__ ((aligned (128)))
#define FORCEINLINE __inline
#define ALIGNED_MALLOC(ptr, type, size, alignment) ptr = (type*)malloc(size)
#define ALIGNED_FREE(ptr) free(ptr)

#elif defined(__MINGW32__)

#define CACHE_ALIGN __attribute__ ((aligned (128)))
//#define FORCEINLINE __inline
#define ALIGNED_MALLOC(ptr, type, size, alignment) ptr = (type*)__mingw_aligned_malloc(size, alignment)
#define ALIGNED_FREE(ptr) __mingw_aligned_free(ptr)

#else // Linux/GCC
#define CACHE_ALIGN __attribute__ ((aligned (128)))
#define FORCEINLINE __inline
#if defined(__sun) && defined(__SVR4)
// no posix_memalign
#define ALIGNED_MALLOC(ptr, type, size, alignment) ptr = (type*)memalign(alignment,size)
#else
#define ALIGNED_MALLOC(ptr, type, size, alignment) if (posix_memalign((void**)&ptr, alignment, size)) { \
                                                                        ptr = NULL; \
                                                       }
#endif
// POSIX says aligned memory can be freed with free()
#define ALIGNED_FREE(ptr) free(ptr)

#endif

#ifdef _WIN64
         #define ALIGN_POINTER(ptr,x) ((void*)((DWORD_PTR)ptr+x & (DWORD_PTR)~(x-1)))
#elif defined(_WIN32)
         #define ALIGN_POINTER(ptr,x) ((void*)((int)ptr+x & (int)~(x-1)))
#elif defined(__LP64__)
         #define ALIGN_POINTER(ptr,x) ((void*)((uintptr_t)ptr+x & (uintptr_t)~(x-1)))
#else
         #define ALIGN_POINTER(ptr,x) ((void*)((int)ptr+x & (int)~(x-1)))
#endif
#ifdef _MSC_VER
#define ALIGN_VAR(n) __declspec(align(n))
#else
#define ALIGN_VAR(n)
#endif

// multithreading support.
#ifdef _WIN32
#define LockDefine(x) CRITICAL_SECTION x
#define lock_t CRITICAL_SECTION
#define LockInit(x) InitializeCriticalSection(&x)
#define Lock(x) EnterCriticalSection(&x);
#define Unlock(x) LeaveCriticalSection(&x);
#define LockFree(x) DeleteCriticalSection(&x)
#define THREAD HANDLE
#elif defined(USE_SPINLOCK)
class Spinlock {
  atomic_flag locked;
        
 public:
  Spinlock() {
  }
  virtual ~Spinlock() {
  }
  void lock()
  {
    while (locked.test_and_set(std::memory_order_acquire)) { ; }
  }
         
  void unlock()
  {
    locked.clear(std::memory_order_release);
  }
};
#define LockDefine(x) Spinlock x
#define lock_t Spinlock
#define LockInit(x)
#define Lock(x) x.lock()
#define Unlock(x) x.unlock()
#define LockFree(x)
#define THREAD pthread_t
#else
#define LockDefine(x) std::mutex *x
#define lock_t std::mutex *
#define LockInit(x) x = new std::mutex
#define Lock(x) x->lock()
#define Unlock(x) x->unlock()
#define LockFree(x) delete x
#define THREAD pthread_t

#endif

#if _BYTE_ORDER == _BIG_ENDIAN
FORCEINLINE uint64_t swapEndian64(const byte *input) {
  return bswap64((uint64_t*)input);
}

FORCEINLINE uint32_t swapEndian32(const byte *input) {
  return bswap32((uint64_t*)input);
}

FORCEINLINE uint16_t swapEndian16(const byte *input) {
  return bswap16((uint64_t*)input);
}

#else
// no need to convert, just de-reference
#define swapEndian64(x) *((uint64_t*)(x))
#define swapEndian32(x) *((uint32_t*)(x))
#define swapEndian16(x) *((uint16_t*)(x))
#endif

#ifdef __INTEL_COMPILER
// Intel only supports #pragma, which can't be emitted by a macro.
// So we must deal with Intel support with #ifdefs in the code.
#define BEGIN_PACKED_STRUCT {
#define END_PACKED_STRUCT };
#elif defined (_MSC_VER)
#define BEGIN_PACKED_STRUCT \
{ \
__pragma(pack(push,1))
#define END_PACKED_STRUCT \
__pragma(pack(pop)) \
};
#else
#define BEGIN_PACKED_STRUCT { 
#define END_PACKED_STRUCT \
} \
__attribute__((__packed__));
#endif

#endif
