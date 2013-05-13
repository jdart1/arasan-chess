// Copyright 2004 by Jon Dart.  All Rights Reserved.
#ifndef _BITPROBE_H_
#define _BITPROBE_H_

#include "constant.h"
#include "types.h"
#include "board.h"

#define NOHIT -1;

extern int lookupBitbase(const Board &board);

// lookup a KPK bitbase result
extern int lookupBitbase(Square whiteK, Square whiteP, Square blackK,
   ColorType strongerSide, ColorType sideToMove);

//extern void generateBitBase();

#endif
