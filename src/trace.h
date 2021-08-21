// Copyright 1995, 2021 by Jon Dart. All Rights Reserved.
#ifndef __TRACE_H__
#define __TRACE_H__

#include <iostream>

inline void indent(const int ply)
{
    for (int k = 0; k < ply; k++) std::cout << ' ';
}

#endif
