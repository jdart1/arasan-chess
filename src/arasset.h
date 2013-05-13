// Copyright 2007 by Jon Dart. All Rights Reserved.
//
// Typesafe set (fixed size). 
//
#ifndef _ArasanSet_h
#define _ArasanSet_h

#include <stddef.h>
#include <stdlib.h>
#include "debug.h"
#include "types.h"

template <class T, const int size> class ArasanSet;

template <class T, const int setsize> class ArasanSet {
 public:
  ArasanSet()
      : _size(setsize),_occupied(0)
  {
  }
  int add(const T &data)
  {
     if (exists(data)) {
        return 0;
     } 
     if (_occupied >= _size) {
       ASSERT(0);
       return 0;
     }
     _contents[_occupied++] = data;
     return 1;
  }
  int remove(const T&data)
  {
     for (int i = 0; i < _occupied; i++) {
       if (_contents[i] == data) {
          for (int j = i; j < _occupied-1; j++) {
             _contents[j] = _contents[j+1];
          }
          --_occupied;
          return 1;         
       }
     }
     return 0;
  }
  int size() const
  {
     return _occupied;
  }
  const T & operator [] (int i) const  {
     ASSERT(i<_occupied);
     return _contents[i];
  }
  int empty() const {
     return _occupied == 0;
  }
  void clear() {
     _occupied = 0;
  }
  int exists(const T&data) const {
     for (int i = 0; i < _occupied; i++) {
       if (_contents[i] == data) {
          return 1;
       }
     }
     return 0;
  }
 private:

  int _size, _occupied;
  T _contents[setsize];
};

#endif

