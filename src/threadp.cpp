// Copyright 2005-2010, 2012, 2013, 2016-2017 by Jon Dart. All Rights Reserved.

#include "threadp.h"
#include "search.h"
#include "globals.h"
#ifndef _WIN32
#include <errno.h>
#include <fcntl.h>
#endif

#ifdef NUMA
bitset<Constants::MaxCPUs> ThreadPool::rebindMask;
#endif

#ifndef _WIN32
static const size_t THREAD_STACK_SIZE = 8*1024*1024;
#endif

#ifdef _THREAD_TRACE
static lock_t io_lock;

void log(const string &s) {
  Lock(io_lock);
  puts(s.c_str());
  fflush(stdout);
  Unlock(io_lock);
}
void log(const string &s,int param) {
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
      ti->pool->lock();
      ti->state = ThreadInfo::Idle;
#ifdef NUMA
      if (rebindMask.test(ti->index)) {
         if (ti->pool->bind(ti->index)) {
            cerr << "Warning: bind to CPU failed for thread " << ti->index << endl;
         }
         rebindMask.reset(ti->index);
      }
#endif
      ti->pool->unlock();
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
      ASSERT(ti->work);
      ti->pool->lock();
      ti->pool->activeMask |= (1ULL << ti->index);
      ti->state = ThreadInfo::Working;
      ti->pool->unlock();
      NodeStack searchStack; // stack on which search will be done
      ti->work->init(searchStack, ti);
#ifdef _THREAD_TRACE
      {
      std::ostringstream s;
      s << "search starting, thread " << ti->index;
      log(s.str());
      }
#endif
      ti->work->ply0_search();
#ifdef _THREAD_TRACE
      {
      std::ostringstream s;
      s << "search completed, thread " << ti->index;
      log(s.str());
      }
#endif
      ti->pool->lock();
      // remove thread from active list and set state back to Idle
      ti->pool->activeMask &= ~(1ULL << ti->index);
      {
         std::lock_guard<std::mutex> (ti->pool->cvm);
         // mark search completed
         ti->pool->completedMask |= 1ULL << ti->index;
         if (ti->pool->allCompleted()) {
            // All threads complete, unblock thread waiting on completion
            ti->pool->cv.notify_one();
         }
      }
      ti->state = ThreadInfo::Idle;
      ti->pool->unlock();
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
    ASSERT(index>=0);
#ifdef _THREAD_TRACE
    log("start",index);
#endif
    signal();
}

ThreadInfo::ThreadInfo(ThreadPool *p, int i)
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

 ThreadPool::ThreadPool(SearchController *ctrl, int n) :
    controller(ctrl), nThreads(n),
    activeMask(0ULL),
    availableMask(0ULL),
    completedMask(0ULL) {

   LockInit(poolLock);
   for (int i = 0; i < Constants::MaxCPUs; i++) {
      data[i] = nullptr;
   }
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
   if (stackSize < THREAD_STACK_SIZE) {
      if (pthread_attr_setstacksize (&stackSizeAttrib, THREAD_STACK_SIZE)) {
         perror("error setting thread stack size");
      }
   }
#endif
#ifdef NUMA
   cout << topo.description() << endl;
   rebindMask.set();
   // bind main thread
   if (bind(0)) {
      cerr << "Warning: bind to CPU failed for thread 0" << endl;
   }
   rebindMask.set(0,0);
#endif
#ifdef _THREAD_TRACE
  LockInit(io_lock);
#endif
   LockInit(poolLock);
   for (int i = 0; i < n; i++) {
      ThreadInfo *p = new ThreadInfo(this,i);
      if (i==0) {
         p->work = new Search(controller,p);
         p->work->ti = p;
      }
      else {
         // defer search creation until thread starts
         //p->work = new Search(controller,p);
      }
      data[i] = p;
   }
   activeMask = 1ULL;
   availableMask = (n == 64) ? 0xffffffffffffffffULL :
      (1ULL << n)-1;
   // Wait for all threads to start up
   while (n >1) {
       int cnt = 0;
       for (int i = 1; i < n; i++)
          if (data[i]->state == ThreadInfo::Idle)
             ++cnt;
       if (cnt==n-1) break;
   }
}

ThreadPool::~ThreadPool() {
   shutDown();
#ifdef _THREAD_TRACE
   LockFree(io_lock);
#endif
#ifndef _WIN32
   if (pthread_attr_destroy(&stackSizeAttrib)) {
      perror("pthread_attr_destroy");
   }
#endif
   LockFree(poolLock);
}

void ThreadPool::shutDown() {
    Lock(poolLock);
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
    Unlock(poolLock);
}

void ThreadPool::resize(unsigned n, SearchController *controller) {
    if (n >= 1 && n < Constants::MaxCPUs && n != nThreads) {
        lock();
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
        unlock();
    }
    ASSERT(nThreads == n);
    availableMask = (n == 64) ? 0xffffffffffffffffULL :
       (1ULL << n)-1;
}

int ThreadPool::activeCount() const {
   return Bitboard(activeMask & availableMask).bitCount();
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

