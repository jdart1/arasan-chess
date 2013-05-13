// Copyright 2005-2010, 2012 by Jon Dart. All Rights Reserved.

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
#elif defined(_MAC)
      sem = semget(IPC_PRIVATE,1,IPC_CREAT | 0777);
      if (sem == -1) {
        perror("semaphore creation failed");
        return;
      }
      union semun val;
      // initialize semaphore to 1
      val.val = 1; 
      if (semctl(sem, 0, SETVAL, val)) {
        perror("semctl");
        return;
      }
#else
      // set val to zero so threads are blocked, initially
      if (sem_init(&sem,0,0)) {
         perror("semaphore creation failed");
         return;
      }
#endif
}

ThreadControl::~ThreadControl() {
#ifdef _WIN32
  CloseHandle(hEvent1);
#elif defined(_MAC)
  if (semctl(sem,IPC_RMID,0)) perror("semctl");
#else
  if (sem_destroy(&sem)) perror("sem_destroy");
#endif
}

void ThreadControl::signal() {
   // unblock the thread so it can execute
#ifdef _WIN32
   if (!SetEvent(hEvent1)) {
      cerr << "error: SetEvent" << endl;
      ASSERT(0);
   }
#elif defined(_MAC)
    struct sembuf sb = { 0, -1, IPC_NOWAIT };
    if (semop(sem,&sb,1)&& errno!=EAGAIN) 
       perror("semop: signal");
#else // Linux
    if (sem_post(&sem) && errno!=EAGAIN) 
      perror("sem_post: signal");
#endif
}


void ThreadControl::reset() {
#ifdef _WIN32
   if (!ResetEvent(hEvent1)) {
        cerr << "ResetEvent failed" << endl;
        ASSERT(0);
   }
#elif defined(_MAC)
   // use semctl, not semop, to mimic Windows behavior where
   // multiple resets have no extra effect. 
   union semun val;
   // set semaphore to 1
   val.val = 1; 
   if (semctl(sem, 0, SETVAL, val)) {
      perror("semctl");
   }
#else
   // lock semaphore
   while (!sem_trywait(&sem)) ;
   if (errno == EAGAIN) {
      // expected, semaphore is locked
   } else {
      perror("reset");
   }
#endif
}

int ThreadControl::wait() {
#ifdef _WIN32
   return (WaitForSingleObject(hEvent1,INFINITE) != WAIT_OBJECT_0);
#elif defined(_MAC)
   struct sembuf sb = { 0, 0, 0 };
   // wait for semaphore value 0  
   if (semop(sem,&sb,1)) {
     if (errno == EINTR || errno == EIDRM) 
        return 1;
     else 
        perror("semop: wait");
   }
#else
   // wait for semaphore value 0  
   int ret;
   if ((ret = sem_wait(&sem)) != 0) {
      if (errno == EINTR) {
         return -1;
      }
      else if (errno == EIDRM) {
         return 1;
      }
      else {
         perror("semop: wait");
      }
   }
#endif
   return 0;
}

int ThreadControl::wouldWait() {
#ifdef _WIN32
   return (WaitForSingleObject(hEvent1,0) != WAIT_OBJECT_0);
#elif defined(_MAC)
    struct sembuf sb = { 0, 0, IPC_NOWAIT };
    if (!semop(sem,&sb,1)) {
       return 0;
    } else if (errno==EAGAIN) {
       // would wait, but IPC_NOWAIT means we just return with error
       return 1;
    } else { // shouldn't happen
       perror("semop");
       return 0;
    }
#else
    int val;
    if (sem_getvalue(&sem, &val)) {
       perror("sem_getvalue");
       return 0;
    } else {
       return val == 0;
    }
#endif
}

int ThreadControl::sleep(time_t t) {
#ifdef _WIN32
   return (WaitForSingleObject(hEvent1,t) != WAIT_OBJECT_0);
#else
   // Mac OS or Linux
   struct timespec ts, ts_remaining;
   ts.tv_sec = t/1000;
   ts.tv_nsec = (t % 1000000)*1000000;
   return nanosleep(&ts,&ts_remaining);
#endif
}

