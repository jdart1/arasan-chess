// Copyright 2012 by Jon Dart. All Rights Reserved.
#ifndef _GTB_H_
#define _GTB_H_

#include "board.h"

// Support for Gaviota-type tablebases.

struct GaviotaTb {
    
    // Initialize the tablebases. 'path' is the
    // path to the TB directories. 'scheme' is the compression
    // scheme. Returns the highest number of pieces that the tbs
    // support (3/4/5/6).
    static int initTB(const string &path, const string &scheme,
                      uint64 cache_size);

    // Probe the tablebases. Return 1 if score was obtained,
    // 0 if not. If 'hard' is set do a hard probe (read disk),
    // otherwise do a soft probe (read cache only).
    static int probe_tb(const Board &b, int &score, int ply, bool hard);

    // unload the TBs, free memory
    static void freeTB();
};

#endif
