// Copyright 2026 by Jon Dart. All Rights Reserved

#ifndef _FEATURE_UTILS
#define _FEATURE_UTILS

namespace FeatureUtils {

// square relative to side
inline static Square relativeSquare(ColorType perspective, Square sq) {
    return sq ^ (static_cast<int>(perspective) * 56);
}

// mask to XOR with a square to get the square relative to side, and horizontally
// mirrored if the side's king (on kp) is on files e-h
inline static unsigned squareFlipMask(ColorType perspective, Square kp) {
    return (static_cast<unsigned>(perspective) * 56) ^ ((File(kp) >= chess::EFILE) * 7);
}

// square relative to side and horizontally mirrored
inline static Square mirroredSquare(ColorType perspective, Square kp, Square sq) {
    return sq ^ squareFlipMask(perspective, kp);
}

}; // namespace FeatureUtils

#endif
