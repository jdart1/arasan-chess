// Copyright 2026 by Jon Dart. All Rights Reserved

#ifndef _FEATURE_UTILS
#define _FEATURE_UTILS

namespace FeatureUtils {

// square relative to side
inline static Square relativeSquare(ColorType perspective, Square sq) {
    return sq ^ (static_cast<int>(perspective) * 56);
}

// square relative to side and horizontally mirrored
inline static Square mirroredSquare(ColorType perspective, Square kp, Square sq) {
    return relativeSquare(perspective, sq ^ ((File(kp) >= chess::EFILE) * 7));
}

}; // namespace FeatureUtils

#endif
