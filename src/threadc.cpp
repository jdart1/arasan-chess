// Copyright 2005-2010, 2012, 2013, 2016 by Jon Dart. All Rights Reserved.

#include "threadc.h"
#include "debug.h"
#include <chrono>

ThreadControl::ThreadControl() : state(0) /* locked */ {
}

ThreadControl::~ThreadControl() {
}

void ThreadControl::signal() {
   // unblock the thread so it can execute
   std::unique_lock<std::mutex> lk(mutex);
   state = 1;
   cond.notify_one();
}


void ThreadControl::reset() {
   std::unique_lock<std::mutex> lk(mutex);
   state = 0;
}

int ThreadControl::wait() {
   // wait for condition variable > 0
   std::unique_lock<std::mutex> lk(mutex);
   while (!state) {
      cond.wait(lk);
   }
   // reset state
   state = 0;
   return 0;
}

int ThreadControl::wouldWait() {
   return state == 0;
}

void ThreadControl::sleep(time_t t) {
   std::this_thread::sleep_for (std::chrono::milliseconds(t));
}
