// Copyright 2006-2008, 2011 by Jon Dart. All Rights Reserved.

#include "searchc.h"
#include "constant.h"
#include "board.h"
#include "search.h"
#include <memory.h>

SearchContext::SearchContext() {
   clearKiller();
}

void SearchContext::clearKiller() {
   for (int i = 0; i < Constants::MaxPly; i++) {
      Killers1[i] = Killers2[i] = NullMove;
   }
}

void SearchContext::setKiller(const Move & move,unsigned ply) {
    if (!MovesEqual(move,Killers1[ply])) {
       Killers2[ply] = Killers1[ply];
    }
    Killers1[ply] = move;
}

void SearchContext::getKillers(unsigned ply,Move &k1,Move &k2) const {
    k1 = Killers1[ply]; k2 = Killers2[ply];
}



