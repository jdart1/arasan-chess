// Copyright 2005-2010, 2012, 2013, 2016-2019, 2021 by Jon Dart. All Rights Reserved.

#include "threadp.h"
#include "search.h"
#include "globals.h"
#ifndef _WIN32

#include <cassert>
#include <errno.h>
#include <fcntl.h>
#endif

#ifdef NUMA
bitset<Constants::MaxCPUs> ThreadPool::rebindMask;
#endif

//#define _THREAD_TRACE

#ifdef _THREAD_TRACE
static std::mutex io_lock;

void log(const std::string &s) {
    std::unique_lock<std::mutex> lock(io_lock);
    std::cout << s.c_str() << std::endl << (flush);
}
void log(const std::string &s,int param) {
    std::ostringstream out;
    out << s;
    out << ": ";
    out << param << '\0';
    log(out.str());
}
#endif

void ThreadPool::idle_loop(ThreadInfo *ti) {
    while (ti->state != ThreadInfo::Terminating) {
#ifdef _THREAD_TRACE
        {
            std::ostringstream s;
            s << "thread " << ti->index << " in idle loop" << '\0';
            log(s.str());
        }
#endif
        ThreadPool *pool = ti->pool;
        {
            std::unique_lock<std::mutex> lock(pool->poolLock);
            ti->state = ThreadInfo::Idle;
#ifdef NUMA
            if (rebindMask.test(ti->index)) {
                if (pool->bind(ti->index)) {
                    cerr << "Warning: bind to CPU failed for thread " << ti->index << endl;
                }
                rebindMask.reset(ti->index);
            }
#endif
        }
        int result;
        if ((result = ti->wait()) != 0) {
            if (result == -1) continue; // was interrupted
            else break;
        }
#ifdef _THREAD_TRACE
        log("unblocked",ti->index);
#endif
        // We've been woken up. There are two possible reasons:
        // 1. This thread is terminating.
        // 2. This thread has been assigned some work.
        //
        if (ti->state == ThreadInfo::Terminating) {
            break;
        }
        assert(ti->work);
        {
            std::unique_lock<std::mutex> lock(pool->poolLock);
            pool->activeMask |= (1ULL << ti->index);
            ti->state = ThreadInfo::Working;
        }
        // allocate stack on which search will be done
        NodeInfo *searchStack = new NodeInfo[Search::SearchStackSize];
        ti->work->init(searchStack, ti);
#ifdef _THREAD_TRACE
        {
            std::ostringstream s;
            s << "search starting, thread " << ti->index;
            log(s.str());
        }
#endif
        ti->work->ply0_search();
        {
            std::unique_lock<std::mutex> lock(pool->poolLock);
            delete [] searchStack;
            // Mark thread completed
            pool->completedMask.set(ti->index);
#ifdef _THREAD_TRACE
            {
                std::ostringstream s;
                s << "# thread " << ti->index << " completed, mask=" <<
                    pool->completedMask << endl;
                log(s.str());
            }
#endif
            // remove thread from active list and set state back to Idle
            pool->activeMask.reset(ti->index);
            ti->state = ThreadInfo::Idle;
            // signal waiter if all threads done
            if (pool->allCompleted()) {
                pool->signal();
            }
        }
    }
}

void ThreadPool::waitAll()
{
    if (nThreads>1) {
#ifdef _THREAD_TRACE
        {
            std::ostringstream s;
            s << "waitAll: completed mask=" <<
                completedMask << " count=" << completedMask.count() << endl;
            log(s.str());
        }
#endif
        // Wait for class condition variable to be signalled
        wait();
    }
}

#ifdef _WIN32
static DWORD WINAPI parkingLot(void *x)
#else
static void * CDECL parkingLot(void *x)
#endif
{
   ThreadInfo *ti = (ThreadInfo*)x;
   if (ti->index) {
      ti->work = new Search(ti->pool->getController(),ti);
   }
   ThreadPool::idle_loop(ti);
   // Free Search instance
   delete ti->work;
   ti->work = nullptr;
#ifdef _THREAD_TRACE
   log("terminated, thread ",ti->index);
#endif
   return 0;
}

ThreadInfo::~ThreadInfo() {
}

void ThreadInfo::start() {
#ifdef _THREAD_TRACE
    log("start",index);
#endif
    signal();
}

ThreadInfo::ThreadInfo(ThreadPool *p, unsigned i)
 : state(Starting),
#ifdef _WIN32
   thread_id(nullptr),
   work(nullptr),
#else
   work(nullptr),
#endif
   pool(p),
   index(i)
{
#ifdef _THREAD_TRACE
  log("starting",i);
#endif
#ifdef _WIN32
      DWORD id;
      if (index == 0) {
         thread_id = GetCurrentThread();
      } else {
         thread_id = CreateThread(NULL,0,
            parkingLot,this,
            0,
            &id);
      }
#else
      if (index == 0) {
         thread_id = pthread_self();
      }
      else {
	if (pthread_create(&thread_id, &(pool->stackSizeAttrib), parkingLot, this)) {
            perror("thread creation failed");
         }
      }
#endif
}

ThreadPool::ThreadPool(SearchController *ctrl, unsigned n) :
    controller(ctrl), nThreads(n) {

   data.fill(nullptr);
#ifndef _WIN32
   if (pthread_attr_init (&stackSizeAttrib)) {
      perror("pthread_attr_init");
      return;
   }
   size_t stackSize;
   if (pthread_attr_getstacksize(&stackSizeAttrib, &stackSize)) {
        perror("pthread_attr_getstacksize");
        return;
   }
   if (stackSize < globals::LINUX_STACK_SIZE) {
      if (pthread_attr_setstacksize (&stackSizeAttrib, globals::LINUX_STACK_SIZE)) {
         perror("error setting thread stack size");
      }
   }
#endif
   for (unsigned i = 0; i < n; i++) {
#ifdef NUMA
      rebindMask.set(i);
#endif
      ThreadInfo *p = data[i] = new ThreadInfo(this,i);
      if (i==0) {
         p->work = new Search(controller,p);
         p->work->ti = p;
#ifdef NUMA
         // bind main thread
         if (bind(0)) {
             cerr << "Warning: bind to CPU failed for thread 0" << endl;
         }
#endif
      }
      else {
         // defer search creation until thread starts
         //p->work = new Search(controller,p);
      }
   }
   // Thread 0 (main thread) is always active:
   activeMask.set(0);
   for (size_t i = 0; i < n; i++) {
       availableMask.set(i);
   }
   // Wait for all threads to start up
   while (n >1) {
       unsigned cnt = 0;
       for (unsigned i = 1; i < n; i++) {
           if (data[i]->state == ThreadInfo::Idle) {
             ++cnt;
           }
       }
       if (cnt==n-1) break;
   }
}

ThreadPool::~ThreadPool() {
   shutDown();
#ifndef _WIN32
   if (pthread_attr_destroy(&stackSizeAttrib)) {
      perror("pthread_attr_destroy");
   }
#endif
}

void ThreadPool::shutDown() {
    std::unique_lock<std::mutex> lock(poolLock);
    ThreadInfo *p;
    // note: do not terminate thread 0 (main thread) in this loop
    for (unsigned i = 1; i < nThreads; i++) {
        p = data[i];
        // All threads should be idle when this function is called.
        if (p->state == ThreadInfo::Idle) {
          // Set the thread to the terminating state that will force thread
          // procedure exit
          p->state = ThreadInfo::Terminating;
          // unblock the thread
          p->signal();
       }
       // wait for the thread to terminate
#ifdef _WIN32
       WaitForSingleObject(p->thread_id,INFINITE);
#else
       void *value_ptr;
       pthread_join(p->thread_id,&value_ptr);
#endif
       // Free thread data
       delete p;
    }
    // now free main thread data
    delete data[0]->work;
    delete data[0]; // main thread structure
}

void ThreadPool::resize(unsigned n) {
    if (n >= 1 && n < Constants::MaxCPUs && n != nThreads) {
        std::unique_lock<std::mutex> lock(poolLock);
#ifdef NUMA
        topo.recalc();
#endif
        if (n>nThreads) {
            // growing
            while (n > nThreads) {
               data[nThreads] = new ThreadInfo(this,nThreads);
               nThreads++;
            }
        }
        else {
            // shrinking
            while (n < nThreads) {
                ThreadInfo *p = data[nThreads-1];
                p->state = ThreadInfo::Terminating;
                if (p->state == ThreadInfo::Idle) {
                    p->signal(); // unblock thread & exit thread proc
                    // wait for thread exit
#ifdef _WIN32
                    WaitForSingleObject(p->thread_id,INFINITE);
#else
                    void *value_ptr;
                    pthread_join(p->thread_id,&value_ptr);
#endif
                }
                delete p;
                data[nThreads] = nullptr;
                --nThreads;
            }
        }
    }
    assert(nThreads == n);
    for (size_t i = 0; i < Constants::MaxCPUs; i++) {
        if (i < n) {
            availableMask.set(i);
        } else {
            availableMask.reset(i);
        }
    }
}

int ThreadPool::activeCount() const {
    return static_cast<int>((activeMask & availableMask).count());
}

uint64_t ThreadPool::totalNodes() const
{
   uint64_t total = 0ULL;
   for (unsigned i = 0; i < nThreads; i++) {
      total += data[i]->work->stats.num_nodes;
   }
   return total;
}

uint64_t ThreadPool::totalHits() const
{
   uint64_t total = 0ULL;
   for (unsigned i = 0; i < nThreads; i++) {
      total += data[i]->work->stats.tb_hits;
   }
   return total;
}

