// Copyright 2000-2002 by Jon Dart. All Rights Reserved.

#include <string.h>
#include <stddef.h>
#include <iostream>
#include <errno.h>
#include "bookutil.h"
#include "bookdefs.h"

using namespace std;

// Linux implmentation of BookUtil class

BookUtil::BookUtil()
{
}

FILE_HANDLE BookUtil::openFile(const char *path,bool readOnly)
{
      return fopen(path,readOnly ? "r" : "r+");
}

FILE_HANDLE BookUtil::createFile(const char *name, int initial_size)
{
      // try to open for read
      FILE_HANDLE h = fopen(name,"r+");
      if (h == NULL) {
	// try to open for write
        h = fopen(name,"w+");
        cout << "creating " << name << endl;
      }
      if (h == NULL)
      {
        char buf[MAX_PATH+100]; 
	sprintf(buf,"could not open file %s",name);
        perror(buf);
        return NULL;
      }
         struct BookHeader header;
         header.version = BookVersion;
         header.num_pages = (byte)0;
         header.page_capacity = header.hash_table_size = 0;
         // write header
         fwrite(&header,sizeof(header),1,h);
         fflush(h);

      char buf[8192];
      memset(buf,'\0',8192);
      int i = 0;
      int written = 0;
      for (; i <initial_size-8192; i+=8192)
      {
 	 written = fwrite(buf,1,8192,h);
         if (written != 8192)
         {
 	    perror("error writing file");
            break;
         }
      }
      int last = initial_size-i;
      written = fwrite(buf,1,last,h);
      if (written != last)
      {
          perror("error writing file");
      }
      fclose(h);
      return openFile(name,false);
}


MEM_HANDLE BookUtil::createMap(FILE_HANDLE file,bool readOnly)
{
  return file;
}

void *BookUtil::map(MEM_HANDLE h,size_t offset, size_t size,bool readOnly)
{
  int prot = readOnly ? PROT_READ : (PROT_READ | PROT_WRITE);
  void* mh = mmap(NULL,size,prot,MAP_SHARED,fileno(h),offset);
  if (mh == MAP_FAILED)
  {
      perror("memory map failed");
      return NULL;
  }
  return mh;
}

void BookUtil::unmap(void *p,size_t length)
{
   munmap((void*)p,length);
}

void BookUtil::sync(void *start, size_t size) {
  if (msync(start,size,MS_SYNC)) {
    perror("book sync failed");
  }
}

void BookUtil::closeMap(MEM_HANDLE h)
{
  // no-op
}

void BookUtil::closeFile(FILE_HANDLE h)
{
  fclose(h);
}
