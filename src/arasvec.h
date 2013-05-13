// Copyright 2001 by Jon Dart. All Rights Reserved.
//
// Typesafe vector. We don't use STL <vector> because of
// portability problems and bugs.
// 
//
#ifndef _ArasanVector_h
#define _ArasanVector_h

#include <stddef.h>
#include <stdlib.h>
#include "debug.h"
#include "types.h"

template <class T> class ArasanVector;

template <class T>
class ArasanVector
{
 public:
  ArasanVector(int initialSize = 200)
      : _size(initialSize),_occupied(0)
  {
     _contents = new T[initialSize];
  }
  virtual ~ArasanVector()
  {
	  delete [] _contents;
  }
  void append(const T &data)
  {
     if (_occupied >= _size-1) {
       resize(_size+_size/2); 
     }
     _contents[_occupied++] = data;
  }
  void set(int index, const T&data)
  {
     ASSERT(index < _size);
     _contents[index] = data;
  }
  void truncate(int newsize)
  {
     ASSERT(newsize < _size);
     _occupied = newsize;
  }
  int length() const
  {
     return _occupied;
  }
  const T & operator [] (int i) const  {
     ASSERT(i<_occupied);
     return _contents[i];
  }
  void remove_last()
  {
    if (_occupied) {
      --_occupied;
    }
  }
  int empty() const {
     return _occupied == 0;
  }
  void removeAll()
  {
    _occupied = 0;
  }
  T &first() const
  {
     ASSERT(_occupied);
     return *_contents;
  }
  T &last() const
  {
    ASSERT(_occupied);
    return _contents[_occupied-1];
  }
  void sort(int (CDECL *compar)(const void *,const void*)) {
     ::qsort(_contents,_occupied,sizeof(T),compar);
  }
 private:
  void resize(int newsize) {
    //cout << "resizing .." << endl;
    T *_newcontents = new T[newsize];
    for (int i=0;i <_occupied; i++)
      _newcontents[i] = _contents[i]; 
    delete [] _contents;
    _contents = _newcontents;
    _size = newsize;
  }
  ArasanVector(const ArasanVector<T> &);
  ArasanVector<T> & operator = (const ArasanVector<T> &);

  int _size, _occupied;
  T *_contents;
};

#endif

