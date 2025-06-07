// Copyright 2021-2022, 2024, 2025 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_NNDEFS_H
#define _NNUE_NNDEFS_H

#include <cassert>
#include <iostream>

#ifdef AVX512
static constexpr size_t DEFAULT_ALIGN = 64;
#else
static constexpr size_t DEFAULT_ALIGN = 32;
#endif

static constexpr size_t MAX_INDICES = 34; // 16 pieces per side plus room for end of list marker

using IndexType = unsigned;

static constexpr IndexType LAST_INDEX = 1000000;

using IndexArray = std::array<IndexType,MAX_INDICES>;

#ifdef NNUE_TRACE
static inline void printIndices(const IndexArray &indices) {
    unsigned i = 0;
    unsigned index;
    while ((index  = indices[i++]) != LAST_INDEX) {
        assert(i<MAX_INDICES);
        if (i != 1) std::cout << ' ';
        std::cout << index;
    }
    std::cout << std::endl;
}
#endif

#endif

