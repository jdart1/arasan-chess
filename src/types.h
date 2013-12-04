// Copyright 1993-2009, 2012, 2013 by Jon Dart.  All Rights Reserved.

#ifndef _TYPES_H
#define _TYPES_H

// Types and macros for OS, machine, and compiler portability

extern "C" {
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#include <malloc.h>
#else
#include <pthread.h>
#ifdef _MAC
#include <sys/ipc.h>
#include <sys/param.h>
#include <sys/sem.h>
#else
#include <semaphore.h>
#endif
#endif
};

#include <iostream>
using namespace std;

#ifdef _WIN32
typedef DWORD CLOCK_TYPE;
#else
typedef unsigned long CLOCK_TYPE;
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

typedef unsigned char byte;
typedef int int32;
typedef unsigned int uint32;
typedef short int16;
typedef unsigned short uint16;

#ifdef _WIN32
#ifdef _MSC_VER
  typedef unsigned __int64 hash_t;
  typedef _int64 int64;
  typedef unsigned __int64 uint64;
#else // GCC
  typedef unsigned long long hash_t;
  typedef long long int64;
  typedef unsigned long long uint64;
#ifndef __MINGW32__
extern "C" {
  #include <Windows32/Defines.h>
  #include <sys/time.h>
  #include <sys/types.h>
  #include <sys/times.h>
};
#endif
#endif
#else
typedef unsigned long long hash_t;
typedef long long int64;
typedef unsigned long long uint64;
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
#define CDECL _cdecl
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
#define LockDestroy(x) DeleteCriticalSection(&x)
#define LockFree(x) DeleteCriticalSection(&x)
#define THREAD HANDLE
#elif !_GNUC_PREREQ(4,2)
// POSIX spinlock, for systems w/o gcc builtins
#define THREAD pthread_t
#define LockDefine(x) pthread_spinlock_t x
#define lock_t pthread_spinlock_t
#define LockInit(x) pthread_spin_init(&x, PTHREAD_PROCESS_PRIVATE)
#define Lock(x) pthread_spin_lock(&x)
#define Unlock(x) pthread_spin_unlock(&x)
#define LockDestroy(x) pthread_spin_destroy(&x)
#define LockFree(x)
#else
// GCC with builtins
#define LockDefine(x) volatile int x
#define lock_t volatile int
#define LockInit(x) x = 0
static inline void Lock(lock_t &x) {
  while(__sync_lock_test_and_set(&x, 1))
    ;
}
static inline void Unlock(lock_t &x) {
   __sync_lock_release(&x);
}
#define LockDestroy(x)
#define LockFree(x)
#define THREAD pthread_t
#endif

#if !defined(__BIG_ENDIAN__) && !defined(__LITTLE_ENDIAN__)
#ifdef _WIN32
// Assume Windows is little endian
#define       __LITTLE_ENDIAN__
#elif defined(__sparc)
#define       __BIG_ENDIAN__
#endif
#endif

#ifdef __BIG_ENDIAN__
typedef union endian_convert64
{
    uint64 data;
    byte bytes[8];
};

typedef union endian_convert32
{
    uint32 data;
    byte bytes[4];
};

typedef union endian_convert16
{
    uint16 data;
    byte bytes[2];
};

typedef union endian_convert_float
{
    float data;
    byte bytes[4];
};

FORCEINLINE uint64 swapEndian64(const byte *input) {
    endian_convert64 ret;
    ret.bytes[0] = input[7];
    ret.bytes[1] = input[6];
    ret.bytes[2] = input[5];
    ret.bytes[3] = input[4];
    ret.bytes[4] = input[3];
    ret.bytes[5] = input[2];
    ret.bytes[6] = input[1];
    ret.bytes[7] = input[0];
    return ret.data;
}

FORCEINLINE uint32 swapEndian32(const byte *input) {
    endian_convert32 ret;
    ret.bytes[0] = input[3];
    ret.bytes[1] = input[2];
    ret.bytes[2] = input[1];
    ret.bytes[3] = input[0];
    return ret.data;
}

FORCEINLINE uint16 swapEndian16(const byte *input) {
    endian_convert16 ret;
    ret.bytes[0] = input[1];
    ret.bytes[1] = input[0];
    return ret.data;
}

FORCEINLINE float swapEndianFloat(const byte *input) {
    endian_convert_float ret;
    ret.bytes[0] = input[3];
    ret.bytes[1] = input[2];
    ret.bytes[2] = input[1];
    ret.bytes[3] = input[0];
    return ret.data;
}

#else
// no need to convert, just de-reference
#define swapEndian64(x) *((uint64*)(x))
#define swapEndian32(x) *((uint32*)(x))
#define swapEndian16(x) *((uint16*)(x))
#define swapEndianFloat(x) *((float*)(x))
#endif
#endif
