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

// maximum number of threat features: at most 30 non-king pieces, each of which attacks at
// most 8 non-king pieces, plus room for end of list marker
static constexpr size_t MAX_THREAT_INDICES = 30 * 8 + 1;

using IndexType = unsigned;

static constexpr IndexType LAST_INDEX = 1000000;

template <size_t size>
using IndexArray = std::array<IndexType,size>;

using PSQIndexArray = IndexArray<MAX_PSQ_INDICES>;

using PPIndexArray = IndexArray<MAX_PP_INDICES>;

using ThreatIndexArray = IndexArray<MAX_THREAT_INDICES>;

// The features added to and removed from a half of an accumulator, for each feature set. To
// compute an accumulator half from scratch, only the "added" lists are used.
struct FeatureChanges {
    PSQIndexArray psqAdded, psqRemoved;
    size_t psqAddedCount = 0, psqRemovedCount = 0;
    PPIndexArray ppAdded, ppRemoved;
    size_t ppAddedCount = 0, ppRemovedCount = 0;
    ThreatIndexArray threatAdded, threatRemoved;
    size_t threatAddedCount = 0, threatRemovedCount = 0;
};

#endif

