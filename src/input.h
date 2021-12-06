// Copyright 2021 by Jon Dart. All Rights Reserved.
//
#ifndef _INPUT_H
#define _INPUT_H

#include <string>
#include <mutex>
#include <vector>

// Handles input from stdin. Note: methods in this class are not
// thread-safe.
class Input {

 public:

  Input();

  virtual ~Input() = default;

  // non-blocking read
  bool checkInput(std::vector<std::string> &, std::mutex &);

  // blocking read
  bool readInput(std::vector<std::string> &, std::mutex &);

 protected:
  static unsigned constexpr BUF_SIZE = 1024;
  char buf[BUF_SIZE];
  unsigned buf_index;

};

#endif

