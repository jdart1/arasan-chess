// Copyright 2021-2022, 2024 by Jon Dart. All Rights Reserved.
#ifndef _NNUE_NNDEFS_H
#define _NNUE_NNDEFS_H

#ifdef AVX512
static constexpr size_t DEFAULT_ALIGN = 64;
#else
static constexpr size_t DEFAULT_ALIGN = 32;
#endif

static constexpr size_t MAX_INDICES = 34; // 16 pieces per side plus room for end of list marker

using IndexType = unsigned;

static constexpr IndexType LAST_INDEX = 1000000;

using IndexArray = std::array<IndexType,MAX_INDICES>;

#endif

