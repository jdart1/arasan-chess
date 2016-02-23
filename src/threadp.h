// Copyright 2005-2013 by Jon Dart. All Rights Reserved.

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "types.h"
#include "threadc.h"
#include "constant.h"

#include <functional>

using namespace std;

class Search;
class SearchController;
struct NodeInfo;
struct SplitPoint;

class ThreadPool;

struct ThreadInfo : public ThreadControl {
 
   enum State { Idle, Working, Terminating };
   ThreadInfo(ThreadPool *,int i);
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
    friend struct ThreadInfo;
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
      return (activeMask & availableMask) != availableMask;
   }

   // Threads that are waiting for work execute this function
   static void idle_loop(ThreadInfo *ti, const SplitPoint *split = NULL);

   // return a thread to the pool
   void checkIn(ThreadInfo *);

   int activeCount() const;

   // resize the thread pool
   void resize(unsigned n, SearchController *);

   template <void (Search::*fn)()>
      void forEachSearch() {
      Lock(poolLock);
      for (unsigned i = 0; i < nThreads; i++) {
          if (data[i] && data[i]->work) {
              std::mem_fun<void,Search>(fn)(data[i]->work);
          }
      }
      Unlock(poolLock);
   }

   SearchController *getController() const {
     return controller;
   }

private:
   void shutDown();

   // lock for the class. Static so idle_loop can access.
   static LockDefine(poolLock);
   ThreadInfo * data[Constants::MaxCPUs];
   unsigned nThreads;

   // mask of thread status - 0 if idle, 1 if active
   static uint64_t activeMask;
   static uint64_t availableMask;

#ifndef _WIN32
   pthread_attr_t stackSizeAttrib;
#endif

   SearchController *controller;
};

#ifdef _THREAD_TRACE
extern void log(const string &s);
extern void log(const string &s,int param);
#endif
#endif
