// Copyright 2005-2013 by Jon Dart. All Rights Reserved.

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "types.h"
#include "threadc.h"

#include <vector>

using namespace std;

class Search;
class SearchController;
struct NodeInfo;
struct SplitPoint;

class ThreadPool;

struct ThreadInfo : public ThreadControl {
   enum State { Idle, Working, Terminating };
   ThreadInfo(int i);
   virtual ~ThreadInfo();
   void start();
   volatile State state;
   Search *work;
   ThreadPool *pool;
   THREAD thread_id;
   int index;
   int operator == (const ThreadInfo &ti) const {
       return index == ti.index;
   }
   int operator != (const ThreadInfo &ti) const {
       return index != ti.index;
   }
};

class ThreadPool {
    friend class SearchController;
public:

   // create a thread pool with n threads
   ThreadPool(SearchController *,int n);

   virtual ~ThreadPool();

   ThreadInfo *mainThread() const {
     return data[0];
   }

   // obtain an idle thread if possible, returns 
   // non-null if successful
   ThreadInfo *checkOut(Search *,NodeInfo *,int ply,int depth);

   // Do a quick check for thread availability (w/o locking)
   int checkAvailable() {
       for (vector<ThreadInfo*>::const_iterator it = data.begin();
            it != data.end();
            it++) {
           if ((*it)->state == ThreadInfo::Idle) return 1;
      }
      return 0;
   }

   // Threads that are waiting for work execute this function
   static void idle_loop(ThreadInfo *ti, const SplitPoint *split = NULL);

   // return a thread to the pool
   void checkIn(ThreadInfo *);

   int activeCount();

   void clearHashTables();

   // resize the thread pool
   void resize(int n, SearchController *);

   void stopAllThreads();

   void clearStopFlags();

   void updateSearchOptions();

private:
   void shutDown();

   // lock for the class. Static so idle_loop can access.
   static LockDefine(poolLock);
   vector<ThreadInfo*> data;
};

#ifdef _THREAD_TRACE
extern void log(const string &s);
extern void log(const string &s,int param);
#endif
#endif
