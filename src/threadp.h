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
#include <mutex>

using namespace std;

class Search;
class SearchController;
struct NodeInfo;

class ThreadPool;

struct ThreadInfo : public ThreadControl {
 
   enum State { Starting, Idle, Working, Terminating };
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

   // Threads that are waiting for work execute this function
   static void idle_loop(ThreadInfo *ti);

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

   void unblockAll() {
      // No need to unblock thread 0: that is the main thread
      for (unsigned i = 1; i < nThreads; i++) {
         data[i]->signal();
      }
      completedMask = 0ULL;
   }

   void waitAll() {
      if (nThreads>1) {
         std::unique_lock<std::mutex> lock(cvm);
         cv.wait(lock, [this]{ return allCompleted(); });
      }
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

   Search *rootSearch() const {
      return data[0]->work;
   }

   ThreadInfo *mainThread() const {
      return data[0];
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

   uint64_t totalNodes() const;

   uint64_t totalHits() const;

   bool allCompleted() const {
     return (completedMask & ~1ULL) == (availableMask & ~1ULL);
   }

private:
   void shutDown();

   // lock for the class.
   LockDefine(poolLock);
   SearchController *controller;
   unsigned nThreads;
   std::array<ThreadInfo *,Constants::MaxCPUs> data;

   // Used for signaling/waiting all threads completion:
   std::mutex cvm;
   std::condition_variable cv;

   // mask of thread status - 0 if idle, 1 if active
   uint64_t activeMask;
   uint64_t availableMask;
   uint64_t completedMask;

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
