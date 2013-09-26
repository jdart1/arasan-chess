// Copyright 2005-2010, 2012, 2013 by Jon Dart. All Rights Reserved.

#include "threadc.h"
#include "debug.h"
#ifndef _WIN32
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#endif

ThreadControl::ThreadControl() {
#ifdef _WIN32
      // initial state is FALSE (non-signaled), so the
      // controller blocks the thread
      hEvent1 = CreateEvent(NULL,FALSE,FALSE,NULL);
#else
      if (pthread_cond_init(&cond, NULL)) {
         perror("pthread_cond_init");
      }
      if (pthread_mutex_init(&mutex, NULL)) {
         perror("pthread_mutex_init");
         return;
      }
      state = 0; // locked
#endif
}

ThreadControl::~ThreadControl() {
#ifdef _WIN32
   CloseHandle(hEvent1);
#else
   if (pthread_cond_destroy(&cond)) {
      perror("pthread_cond_destroy");
   }
   if (pthread_mutex_destroy(&mutex)) {
      perror("pthread_mutex_destroy");
   }
#endif
}

void ThreadControl::signal() {
   // unblock the thread so it can execute
#ifdef _WIN32
   if (!SetEvent(hEvent1)) {
      cerr << "error: SetEvent" << endl;
      ASSERT(0);
   }
#else
   if (pthread_mutex_lock(&mutex)) {
      perror("signal: pthread_mutex_lock");
      return;
   }
   state = 1;
   if (pthread_cond_signal(&cond)) {
      perror("pthread_cond_signal");
   }
   if (pthread_mutex_unlock(&mutex)) {
      perror("signal: pthread_mutex_unlock");
   }
#endif
}


void ThreadControl::reset() {
#ifdef _WIN32
   if (!ResetEvent(hEvent1)) {
        cerr << "ResetEvent failed" << endl;
        ASSERT(0);
   }
#else
   if (pthread_mutex_lock(&mutex)) {
      perror("reset: pthread_mutex_lock");
      return;
   }
   state = 0;
   if (pthread_mutex_unlock(&mutex)) {
      perror("reset: pthread_mutex_unlock");
   }
#endif
}

int ThreadControl::wait() {
#ifdef _WIN32
   return (WaitForSingleObject(hEvent1,INFINITE) != WAIT_OBJECT_0);
#else
   // wait for condition variable > 0
   if (pthread_mutex_lock(&mutex)) {
      perror("wait: pthread_mutex_lock");
      return 1;
   }
   while (!state) {
      if (pthread_cond_wait(&cond, &mutex)) {
         perror("wait: pthread_cond_wait");
         break;
      }
   }
   // reset state
   state = 0;
   if (pthread_mutex_unlock(&mutex)) {
      perror("wait: pthread_mutex_unlock");
   }
#endif
   return 0;
}

int ThreadControl::wouldWait() {
#ifdef _WIN32
   return (WaitForSingleObject(hEvent1,0) != WAIT_OBJECT_0);
#else
   return state == 0;
#endif
}

int ThreadControl::sleep(time_t t) {
#ifdef _WIN32
   return (WaitForSingleObject(hEvent1,(DWORD)t) != WAIT_OBJECT_0);
#else
   // Mac OS or Linux
   struct timespec ts, ts_remaining;
   ts.tv_sec = t/1000;
   ts.tv_nsec = (t % 1000000)*1000000;
   return nanosleep(&ts,&ts_remaining);
#endif
}

