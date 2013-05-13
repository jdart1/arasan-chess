// Copyright 2005-2007, 2012 by Jon Dart. All Rights Reserved.
#ifndef _THREAD_C_H
#define _THREAD_C_H

#include "types.h"

// encapsulates an object usable for thread synchronization
class ThreadControl {
 public:
   ThreadControl();
   virtual ~ThreadControl();

   // wait for signal; return 1 if interrupted
   int wait();
   // return non-zero if caller would wait (but don't actually wait)
   int wouldWait();
   // wait for a fixed time interval (in milliseconds)
   int sleep(time_t t);
   // wake up a thread that is waiting
   void signal();
   // reset the signalled state of the object
   void reset();

 private:
#ifdef _WIN32
   HANDLE hEvent1;
#elif defined(_MAC)
   int sem;
#else
   sem_t sem;  // semaphore
#endif

};

#endif
