// Copyright 2012 by Jon Dart. All Rights Reserved.
//
// Interface to Gaviota tablebase code

#include "gtb.h"
#include "gtb-probe.h"

static const int WDL_FRACTION = 96;
static const char **paths = tbpaths_init();

int GaviotaTb::initTB(const string &path, const string &scheme,
                      uint64 cache_size) {

    if (NULL == paths) {
        cerr << "error in path initialization for Gaviota TBs" << endl;
        return 0;
    }
    paths = tbpaths_add(paths, path.c_str());
    if (NULL == paths) {
        cerr << "error in path initialization for Gaviota TBs" << endl;
        return 0;
    }
    int intScheme;
    if (scheme == "cp1") 
        intScheme = tb_CP1;
    else if (scheme == "cp2")
        intScheme = tb_CP2;
    else if (scheme == "cp3")
        intScheme = tb_CP3;
    else if (scheme == "cp4")
        intScheme = tb_CP4;
    else {
        cerr << "unrecognized compression scheme for GTBs: " << scheme << endl;
        return 0;
    }
    char *initinfo = tb_init (0, intScheme, paths);
    tbcache_init((size_t)cache_size, WDL_FRACTION);
    tbstats_reset();
    if (initinfo != NULL) {
        cout << initinfo << endl;
    }
    int avail = tb_availability();
    if (avail & (1<<6)) return 6;
    else if (avail & (1<<4)) return 5;
    else if (avail & (1<<2)) return 4;
    else if (avail & 1) return 3;
    else return 0; // error?
}

static const unsigned int stm_map[2] = {tb_WHITE_TO_MOVE,tb_BLACK_TO_MOVE};
static const unsigned int castle_map[6] = 
        {0x3,0x2,0x1,0x0,0x0,0x0};

int GaviotaTb::probe_tb(const Board &board, int &score, int ply, bool hard) {

    unsigned int stm = stm_map[board.sideToMove()];
    Square epsq = board.enPassantSq();
    // map en-passant square to format used by GTBs
    unsigned int epsquare  = (epsq == InvalidSquare) ? tb_NOSQUARE : 
        MakeSquare(File(epsq),6,board.sideToMove());

    unsigned int castling = castle_map[board.castleStatus(Black)] |
        (castle_map[board.castleStatus(White)] << 2);
    unsigned info = tb_UNKNOWN;
    unsigned pliestomate;	
    unsigned int ws[17], bs[17];
    unsigned char wp[17], bp[17];

    // pack arrays ws, bs, wp, bp with the square/piece info from the
    // board:
    Square wsq;
    Bitboard temp(board.occupied[White]);
    int w = 0;
    while (temp.iterate(wsq)) {
        // our square & piece types map directly to the GTB piece types
        ws[w] = (unsigned int)wsq;
        wp[w] = (unsigned char)TypeOfPiece(board[wsq]);
        w++;
    }
    ws[w] = tb_NOSQUARE;
    wp[w] = tb_NOPIECE;
    Square bsq;

    Bitboard temp2(board.occupied[Black]);
    int b = 0;
    while (temp2.iterate(bsq)) {
        // our square & piece types map directly to the GTB piece types
        bs[b] = (unsigned int)bsq;
        bp[b] = (unsigned char)TypeOfPiece(board[bsq]);
        b++;
    }
    bs[b] = tb_NOSQUARE;
    bp[b] = tb_NOPIECE;

    int avail;
    if (hard) 
        avail = tb_probe_hard(stm, epsquare, castling, ws, bs, wp, bp, &info,
                         &pliestomate);
    else
        avail = tb_probe_soft(stm, epsquare, castling, ws, bs, wp, bp, &info,
                         &pliestomate);
    if (!avail) return 0;
    score = 0;
    if (info == tb_WMATE)
       score = board.sideToMove() == White ? Constants::MATE-(int)pliestomate :
          -Constants::MATE+(int)pliestomate;
    else if (info == tb_BMATE)
       score = board.sideToMove() == Black ? Constants::MATE-(int)pliestomate :
          -Constants::MATE+(int)pliestomate;
    // correct for search depth (a mate found far from the root is less
    // valuable than one found closer to the root)
    if (score > 0) 
        score -= ply;
    else if (score < 0)
        score += ply;
    return 1; /* success */
}

void GaviotaTb::freeTB() {
    tbcache_done();
    tb_done();
    paths = tbpaths_done(paths);
}
