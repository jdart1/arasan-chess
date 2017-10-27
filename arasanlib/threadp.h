// Copyright 2005-2013, 2016 by Jon Dart. All Rights Reserved.

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "types.h"
#include "threadc.h"
#include "constant.h"
#ifdef NUMA
#include "topo.h"
#endif
#include <array>
#include <bitset>
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
   atomic<State> state;
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
      lock();
      for (unsigned i = 0; i < nThreads; i++) {
          if (data[i] && data[i]->work) {
              std::mem_fun<void,Search>(fn)(data[i]->work);
          }
      }
      unlock();
   }

   SearchController *getController() const {
     return controller;
   }

   void lock() {
     Lock(poolLock);
   }

   void unlock() {
     Unlock(poolLock);
   }

#ifdef NUMA
   int bind(int index) {
     return topo.bind(index);
   }

   void rebind() {
     // set flags so threads will be rebound
     rebindMask.set();
   }

   void unbind() {
     topo.reset();
     // set flags so threads will be rebound
     rebind();
   }
#endif

private:
   void shutDown();

   // lock for the class.
   LockDefine(poolLock);
   SearchController *controller;
   unsigned nThreads;
   std::array<ThreadInfo *,Constants::MaxCPUs> data;

   // mask of thread status - 0 if idle, 1 if active
   static uint64_t activeMask;
   static uint64_t availableMask;

#ifndef _WIN32
   pthread_attr_t stackSizeAttrib;
#endif

#ifdef NUMA
   static std::bitset<Constants::MaxCPUs> rebindMask;
   Topology topo;
#endif
};

#ifdef _THREAD_TRACE
extern void log(const string &s);
extern void log(const string &s,int param);
#endif
#endif
