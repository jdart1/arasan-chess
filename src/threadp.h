// Copyright 2005-2013, 2016-2019, 2021-2022 by Jon Dart. All Rights Reserved.

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include "bitboard.h"
#include "threadc.h"
#include "constant.h"
#ifdef NUMA
#include "topo.h"
#endif
#include <array>
#include <atomic>
#include <bitset>
#include <functional>
#include <mutex>

class Search;
class SearchController;
struct NodeInfo;

class ThreadPool;

struct ThreadInfo : public ThreadControl {

   enum State { Starting, Idle, Working, Terminating };
   ThreadInfo(ThreadPool *,unsigned i);
   virtual ~ThreadInfo();
   void start();
   std::atomic<State> state;
   Search *work;
   ThreadPool *pool;
   THREAD thread_id;
   unsigned index;
   int operator == (const ThreadInfo &ti) const {
       return index == ti.index;
   }
   int operator != (const ThreadInfo &ti) const {
       return index != ti.index;
   }
};

class ThreadPool : protected ThreadControl {
    friend class SearchController;
    friend struct ThreadInfo;
public:

   // create a thread pool with n threads
   ThreadPool(SearchController *,unsigned n);

   virtual ~ThreadPool();

   // Threads that are waiting for work execute this function
   static void idle_loop(ThreadInfo *ti);

   int activeCount() const;

   // resize the thread pool
   void resize(unsigned n);

   template <void (Search::*fn)()>
      void forEachSearch() {
      std::unique_lock<std::mutex> lock(poolLock);
      for (unsigned i = 0; i < nThreads; i++) {
          if (data[i] && data[i]->work) {
              std::mem_fn(fn)(data[i]->work);
          }
      }
   }

   void unblockAll() {
      completedMask = 0ULL;
      // No need to unblock thread 0: that is the main thread
      for (unsigned i = 1; i < nThreads; i++) {
         data[i]->signal();
      }
   }

   void waitAll();

   SearchController *getController() const {
     return controller;
   }

   Search *rootSearch() const {
      return data[0]->work;
   }

   ThreadInfo *mainThread() const {
      return data[0];
   }

#ifdef NUMA
   int bind(int index) {
     return topo.bind(data[index]);
   }

   void recalcBindings() {
     topo.recalc();
     // set flags so threads will be rebound
     rebindMask.set();
   }
#endif

   uint64_t totalNodes() const;

   uint64_t totalHits() const;

   bool isCompleted(unsigned index) {
       std::unique_lock<std::mutex> lock(poolLock);
       bool val = completedMask.test(index);
       return val;
   }

   bool isCompletedNoLock(unsigned index) {
       return completedMask.test(index);
   }

   void setCompleted(unsigned index) {
       std::unique_lock<std::mutex> lock(poolLock);
       completedMask.set(static_cast<size_t>(index));
       if (completedMask.count() == nThreads) {
           signal();
       }
   }

   void lock() {
       poolLock.lock();
   }

   void unlock() {
       poolLock.unlock();
   }

   unsigned size() const noexcept {
       return nThreads;
   }

private:
   // Note: does not lock
   bool allCompleted() const {
       return completedMask.count() == nThreads;
   }

   void shutDown();

   // lock for the class.
   std::mutex poolLock;
   SearchController *controller;
   unsigned nThreads;
   std::array<ThreadInfo *,Constants::MaxCPUs> data;

   // mask of thread status - 0 if idle, 1 if active
   std::bitset<Constants::MaxCPUs> activeMask, availableMask, completedMask;

#ifndef _WIN32
   pthread_attr_t stackSizeAttrib;
#endif

#ifdef NUMA
   static std::bitset<Constants::MaxCPUs> rebindMask;
   Topology topo;
#endif
};

#ifdef _THREAD_TRACE
extern void log(const std::string &s);
extern void log(const std::string &s,int param);
#endif
#endif
