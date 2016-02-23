#ifndef _NALIMOV_H_
#define _NALIMOV_H_

#include "board.h"

struct NalimovTb {
    // Initialize the tablebases. Character param is the
    // path to the TB directory.
    static int initTB(char *,uint64_t cache_size);

    // probe the tablebases. Return 1 if score was obtained,
    // 0 if not.
    static int probe_tb(const Board &b, int &score, int ply);
};

#endif
