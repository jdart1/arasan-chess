// Copyright 2021, 2024-2025 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_H
#define _NNUE_H

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <memory>
#include <array>
#include <vector>
#ifdef __cpp_lib_bitops
#include <bit>
#endif

#ifdef SIMD
#include "simd.h"
#endif

#include "board.h"

struct NodeInfo;

namespace nnue {
#include "network.h"
}


#endif
