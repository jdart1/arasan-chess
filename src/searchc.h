// Copyright 2006-2008 by Jon Dart. All Rights Reserved.

#ifndef _SEARCHC_H
#define _SEARCHC_H

#include "constant.h"
#include "chess.h"

struct NodeInfo;
class Board;

class SearchContext {
 public:
  SearchContext();

    void clearKiller(); 
    void setKiller(const Move & move,unsigned ply);
    void getKillers(unsigned ply,Move &k1,Move &k2) const;

    Move Killers1[Constants::MaxPly];
    Move Killers2[Constants::MaxPly];

};


#endif
