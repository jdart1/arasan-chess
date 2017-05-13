// Copyright 2015-2017 by Jon Dart. All Rights Reserved.

#ifndef _REFUT_H
#define _REFUT_H

#include "board.h"
#include <array>

class NodeInfo;

class Refutations {
 public:

   using CmhArray = std::array<std::array<int, 64>, 6>;

   using CmhMatrix = std::array< std::array< CmhArray, 64>, 6 >;

   Refutations() {
      counterMoveHistory = new CmhMatrix;
      clear();
   }

   virtual ~Refutations() {
      delete counterMoveHistory;
   }

    Move getRefutation(Move prev) const {
       return refutations[refutationKey(prev)];
    }

    void setRefutation(Move prev, Move ref) {
       refutations[refutationKey(prev)] = ref;
    }

    void clear() {
       for (int i = 0; i < REFUTATION_TABLE_SIZE; i++) refutations[i] = NullMove;
       for (int i = 0; i < 6; i++)
         for (int j = 0; j < 64; j++)
           for (int k = 0; k < 6; k++)
             for (int l = 0; l < 64; l++)
               (*counterMoveHistory)[i][j][k][l] = 0;
    }

    void updateCounterMoveHistory(const Board &,
       NodeInfo *parentNode, Move best, int depth, ColorType side);

 private:

    static const int REFUTATION_TABLE_SIZE = 16*64;

    static int refutationKey(Move ref) {
      int key = ((int)PieceMoved(ref) << 6) | (int)DestSquare(ref);
      ASSERT(key<REFUTATION_TABLE_SIZE);
      return key;
    }

    array<Move,REFUTATION_TABLE_SIZE> refutations;

    CmhMatrix *counterMoveHistory;
};


#endif
