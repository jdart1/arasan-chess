// Copyright 2005-2010, 2012, 2013, 2016 by Jon Dart. All Rights Reserved.

#include "threadp.h"
#include "search.h"
#include "globals.h"
#ifndef _WIN32
#include <errno.h>
#include <fcntl.h>
#endif

uint64_t ThreadPool::activeMask = 0ULL;
uint64_t ThreadPool::availableMask = 0ULL;
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

void ThreadPool::idle_loop(ThreadInfo *ti, const SplitPoint *split) {
   while (ti->state != ThreadInfo::Terminating) {
#ifdef _THREAD_TRACE
      {
      std::ostringstream s;
      s << "thread " << ti->index << " in idle loop" << '\0';
      log(s.str());
      }
#endif
      ti->pool->lock();
#ifdef NUMA
      if (rebindMask.test(ti->index)) {
         if (ti->pool->bind(ti->index)) {
            cerr << "Warning: bind to CPU failed for thread " << ti->index << endl;
         }
         rebindMask.reset(ti->index);
      }
#endif
      if (ti->wouldWait()) {
        ti->state = ThreadInfo::Idle; // mark thread available again
        activeMask &= ~(1ULL << ti->index);
        ti->pool->unlock();
        int result;
        if ((result = ti->wait()) != 0) {
            if (result == -1) continue; // was interrupted
            else break;
        }
      } else {
#ifdef _THREAD_TRACE
         {
            std::ostringstream s;
            s << "thread " << ti->index << " already signalled, skipping wait()" << '\0';
            log(s.str());
         }
         
#endif
        // Avoid waiting if the thread state is already signalled. Also,
        // in this case, do not even temporarily set the state to Idle.
        ti->pool->unlock();
      }
#ifdef _THREAD_TRACE
      log("unblocked",ti->index);
#endif
      // We've been woken up. There are three possible reasons:
      // 1. This thread is terminating.
      // 2. We are a master at a split point and all our slave threads
      // are done.
      // 3. This thread (a slave) has been assigned some work.
      //
      if (ti->state == ThreadInfo::Terminating) {
          break;
      }
      else if (split && split->master == ti) {
          // This thread is master of a split point, test for condition #2
          Lock(split->master->work->splitLock);
          if (split->slaves.size()==0) {
#ifdef _THREAD_TRACE
              log("helpful master exiting idle loop -  thread #",ti->index);
#endif
              ASSERT(ti->state == ThreadInfo::Working);
              Unlock(split->master->work->splitLock);
              // This thread exits the thread pool and returns to what it
              // was previously doing.
              return;
          }
#ifdef _THREAD_TRACE
      {
      std::ostringstream s;
      s << "master thread " << ti->index << " woken" << '\0';
      log(s.str());
      }
#endif
          Unlock(split->master->work->splitLock);
      }
#ifdef _THREAD_TRACE
      log("wakeup",ti->index);
#endif
      ASSERT(ti->work);
#ifdef _DEBUG
      int state = ti->state;
#ifdef _THREAD_TRACE
      if (state != ThreadInfo::Working) log("error ",ti->index);
#endif
      ASSERT(state == ThreadInfo::Working);
#endif
      // child should have its split point set
      ASSERT(ti->work->split);
      NodeStack childStack; // stack on which child will search
      ti->work->init(childStack, ti);
#ifdef _THREAD_TRACE
      {
      std::ostringstream s;
      s << "search starting, thread " << ti->index << ", ply=" << ti->work->split->ply << '\0';
      log(s.str());
      }
#endif
      ti->work->searchSMP(ti);  
#ifdef _THREAD_TRACE
      {
      std::ostringstream s;
      s << "search completed, thread " << ti->index << ", ply=" << ti->work->split->ply << '\0';
      log(s.str());
      }
#endif
#ifdef _THREAD_TRACE
      log("search completed ",ti->index);
#endif
      ti->pool->checkIn(ti);
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
   ti->work = NULL;
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
 : state(Idle),
#ifdef _WIN32
   thread_id(NULL),
   work(NULL),
#else
   work(NULL),
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
    controller(ctrl), nThreads(n) {
   LockInit(poolLock);
   for (int i = 0; i < Constants::MaxCPUs; i++) {
      data[i] = NULL;
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
         p->work = new RootSearch(controller,p);
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

ThreadInfo * ThreadPool::checkOut(Search *parent, NodeInfo *forNode,
  int ply, int depth) {
    // lock against changes to the pool
    Lock(poolLock);
    // and aginst changes to the split stack in the parent
    Lock(parent->splitLock);
    // only loop over available threads
    Bitboard b(~activeMask & availableMask);
    b.clear(parent->ti->index);
    int i;
    while (b.iterate(i)) {
       ThreadInfo *p = data[i];
       ASSERT(p->state == ThreadInfo::Idle);
       Search *child = p->work;
       // lock the split stack in the child for the following test
       Lock(child->splitLock);
       // If this is a "master" thread it is not sufficient to just be
       // idle - assign it only to one of its slave threads at the
       // current top of the search stack.
       bool ok;
       if (child->activeSplitPoints) {
          auto slaves = child->splitStack[child->activeSplitPoints-1].slaves;
          ok = slaves.find(parent->ti) != slaves.end();
       } else {
          ok = true;
       }
       if (ok) {
         // We're working now - ensure we will not be allocated again
         p->state = ThreadInfo::Working; 
         activeMask |= (1ULL << p->index);
         Unlock(child->splitLock);
         Unlock(parent->splitLock);
         Unlock(poolLock);
         return p;
       }
       Unlock(child->splitLock);
    }
    // no luck, no free threads
    Unlock(parent->splitLock);
    Unlock(poolLock);
    return NULL;
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
                data[nThreads] = NULL;
                --nThreads;
            }
        }
        unlock();
    }
    ASSERT(nThreads == n);
    availableMask = (n == 64) ? 0xffffffffffffffffULL :
       (1ULL << n)-1;
}

void ThreadPool::checkIn(ThreadInfo *ti) {
#ifdef _THREAD_TRACE
    {
        std::ostringstream s;
        s << "checkIn: " << ti->index << " master=" <<
            ti->work->split->master->index << '\0';
        log(s.str());
    }
#endif
    Lock(poolLock);
    SplitPoint *split = ti->work->split;
    ThreadInfo *parent = split->master;
    Search *parentSearch = parent->work;
    // lock parent's stack
    Lock(parentSearch->splitLock);
    split->lock();
    // dissociate the thread from the parent
    set<ThreadInfo *> &slaves = split->slaves;
    // remove ti from the list of slave threads in the parent
#ifdef _THREAD_TRACE
    {
        std::ostringstream s;
        s << "removing slave thread " <<  ti->index << " from master "
          << split->master->index << '\0';
        log(s.str());
    }
#endif
#ifdef _DEBUG
    ASSERT(slaves.erase(ti)==1);
#else
    slaves.erase(ti);
#endif
    const unsigned remaining = (unsigned)slaves.size();
    const bool top = split - parentSearch->splitStack + 1 == parentSearch->activeSplitPoints;
#ifdef _THREAD_TRACE
    {
        std::ostringstream s;
        s << "after checkIn: " << ti->index << " remaining: " << remaining << " top=" << top << '\0';
        log(s.str());
    }
#endif
    if (!remaining && top) {
        // all slave threads are completed, so signal parent that it
        // can exit its wait state and pop the split stack.
        ASSERT(parent);
        // Set parent state to Working before it even wakes up. This
        // ensures it will not be allocated to another split point.
        parent->state = ThreadInfo::Working;
        activeMask |= (1ULL << parent->index);
#ifdef _THREAD_TRACE
        std::ostringstream s;
        s << "thread " << ti->index <<  
            " signaling parent (" << parent->index << ")" << 
            " parent state=" << parent->state << '\0';
        log(s.str());
#endif
        ASSERT(parent->index != ti->index);
#ifdef _THREAD_TRACE
        log("signal",parent->index);
#endif
        parent->signal();
    }
    // ensure we we will wait when back in the idle loop
    ti->reset();
    split->unlock();
    Unlock(parentSearch->splitLock);
    Unlock(poolLock);
}

int ThreadPool::activeCount() const {
   return Bitboard(activeMask & availableMask).bitCount();
}

