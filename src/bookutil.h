// Copyright 1999 by Jon Dart. All Rights Reserved.
#ifndef _BOOKUTIL_H_
#define _BOOKUTIL_H_

#ifdef _WIN32
#include <windows.h>
typedef HANDLE MEM_HANDLE;
typedef HANDLE FILE_HANDLE;
#else
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
typedef FILE * MEM_HANDLE;
typedef FILE * FILE_HANDLE;
#endif
#include <sys/types.h>

// Platform-independent interface for file and shared memory access

class BookUtil
{
   public:
   
   BookUtil();
   
   FILE_HANDLE openFile(const char *name, bool readOnly);
   
   FILE_HANDLE createFile(const char *name, int initial_size);

   MEM_HANDLE createMap(FILE_HANDLE, bool readOnly);
   
   void *map(MEM_HANDLE,size_t offset, size_t size, bool readOnly);
   
   void unmap(void *,size_t length);
   
   void sync(void *start, size_t size);

   void closeMap(MEM_HANDLE);

   void closeFile(FILE_HANDLE);
   
};

#endif
