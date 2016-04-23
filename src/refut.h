// Copyright 2015, 2016 by Jon Dart. All Rights Reserved.

#ifndef _REFUT_H
#define _REFUT_H

#include "chess.h"

class Refutations {
 public:

    Move getRefutation(Move prev) const {
       return refutations[refutationKey(prev)];
    }

    void setRefutation(Move prev, Move ref) {
       refutations[refutationKey(prev)] = ref;
    }

    void clear() {
       for (int i = 0; i < REFUTATION_TABLE_SIZE; i++) refutations[i] = NullMove;
    }

 private:

    static const int REFUTATION_TABLE_SIZE = 16*64;

    static int refutationKey(Move ref) {
      int key = ((int)PieceMoved(ref) << 6) | (int)DestSquare(ref);
      ASSERT(key<REFUTATION_TABLE_SIZE);
      return key;
    }

    Move refutations[REFUTATION_TABLE_SIZE];
};


#endif
