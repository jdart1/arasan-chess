// Copyright 2015 by Jon Dart. All Rights Reserved.

#ifndef _REFUT_H
#define _REFUT_H

#include "chess.h"

class Refutations {
 public:

    static Move getRefutation(Move prev) {
      return refutations[refutationKey(prev)];
    }
    static void setRefutation(Move prev, Move ref) {
      if (!IsNull(prev) && !CaptureOrPromotion(ref)) {
        refutations[refutationKey(prev)] = ref;
      }
    }

    static void clearRefutations() {
      for (int i = 0; i < REFUTATION_TABLE_SIZE; i++) refutations[i] = NullMove;
    }

 private:

    static const int REFUTATION_TABLE_SIZE = 16*64;

    static int refutationKey(Move ref) {
      int key = ((int)PieceMoved(ref) << 6) | (int)DestSquare(ref);
      ASSERT(key<REFUTATION_TABLE_SIZE);
      return key;
    }

    static Move refutations[REFUTATION_TABLE_SIZE];
};


#endif
