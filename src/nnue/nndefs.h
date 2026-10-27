// Copyright 2021-2022, 2024-2026 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_NNDEFS_H
#define _NNUE_NNDEFS_H

#ifdef AVX512
static constexpr size_t DEFAULT_ALIGN = 64;
#else
static constexpr size_t DEFAULT_ALIGN = 32;
#endif

// maximum number of PSQ features: 16 pieces per side plus room for end of list marker
static constexpr size_t MAX_PSQ_INDICES = 34;

// maximum number of pawn pair features: up to 16 pawns, so 16*15/2 pairs, plus room for
// end of list marker
static constexpr size_t MAX_PP_INDICES = 16 * 15 / 2 + 1;

using IndexType = unsigned;

static constexpr IndexType LAST_INDEX = 1000000;

template <size_t size>
using IndexArray = std::array<IndexType,size>;

using PSQIndexArray = IndexArray<MAX_PSQ_INDICES>;

using PPIndexArray = IndexArray<MAX_PP_INDICES>;

#endif

