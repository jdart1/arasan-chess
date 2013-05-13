// Copyright 1999-2002 by Jon Dart. All Rights Reserved.
//
#include "bookutil.h"
#include "types.h"
extern "C" {
#include <stdio.h>
};
#include <windows.h>
#include <iostream>

using namespace std;

// Windows implementation of BookUtil class

static void show_error(const char *prefix, const char *fileName)
{
   LPVOID errMsg;

   FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                NULL,
                GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR)&errMsg,
                0,
                NULL 
                );
   int len = (int)(strlen((LPCSTR)errMsg) + strlen(fileName) + strlen(prefix) + 2);

   char *msg = new char[len];
   strcpy(msg,prefix);
   strcat(msg,fileName);
   strcat(msg," ");
   strcat(msg,(LPCSTR)errMsg);
   fprintf(stderr,"%s\n",msg);
   LocalFree(errMsg);
   delete [] msg;
}

BookUtil::BookUtil()
{
}

FILE_HANDLE BookUtil::openFile(const char *path,bool readOnly)
{
      int mode, share;
      if (readOnly)
      {
         mode = GENERIC_READ;
         share = FILE_SHARE_READ;
      }
      else
      {
         mode = GENERIC_READ | GENERIC_WRITE;
         share = FILE_SHARE_READ | FILE_SHARE_WRITE;
      }
      FILE_HANDLE h = CreateFile(path,mode,share,
                             NULL /* security attributes */,
                             OPEN_EXISTING,
                             0,0);
      if (h == INVALID_HANDLE_VALUE)
      {
         //show_error(path); // caller will show error
         return NULL;
      }
      return h;
}

FILE_HANDLE BookUtil::createFile(const char *name, int initial_size)
{
      int mode, share;
      mode = GENERIC_READ | GENERIC_WRITE;
      share = 0;
      FILE_HANDLE h = CreateFile(name,mode,share,
                             NULL /* security attributes */,
                             CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
      if (h == INVALID_HANDLE_VALUE)
      {
         show_error("create failed",name);
         return NULL;
      }
      char buf[8192];
      memset(buf,'\0',8192);
      int i = 0;
      DWORD written;
      for (; i <initial_size-8192; i+=8192)
      {
         BOOL result = WriteFile(h,buf,(DWORD)8192,&written,NULL);
         if (!result)
         {
            break;
         }
      }
      int last = initial_size-i;
      BOOL result = WriteFile(h,buf,last,&written,NULL);
      CloseHandle(h);
      return openFile(name,false);
}


MEM_HANDLE BookUtil::createMap(FILE_HANDLE file,bool readOnly)
{
      MEM_HANDLE h = CreateFileMapping(file,
                                       NULL,
                                       readOnly ?
                                       PAGE_READONLY :
                                       PAGE_READWRITE,
                                       0,
                                       0,
                                       NULL /*mappingName*/);
      if (!h)
      {
         show_error("createMap failed:","");
         return NULL;
      }
      return h;
}

void *BookUtil::map(MEM_HANDLE h,size_t offset, size_t size,bool readOnly)
{
      void *pBook = MapViewOfFile(h,
         readOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS,
         0,(DWORD)offset,(DWORD)size);
      if (!pBook)
      {
         show_error("map operation failed:","");
         return NULL;
      }
      return pBook;
}

void BookUtil::unmap(void *p,size_t length)
{
      UnmapViewOfFile(p);
}

void BookUtil::sync(void *p, size_t size) {
   FlushViewOfFile(p, (DWORD)size);
}

void BookUtil::closeMap(MEM_HANDLE h)
{
   CloseHandle(h);
}

void BookUtil::closeFile(FILE_HANDLE h)
{
   CloseHandle(h);
}
