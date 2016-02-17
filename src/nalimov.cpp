// Copyright 2001, 2008, 2012, 2016 Jon Dart. All Rights Reserved.
//
// This module interfaces between the Nalimov tb code
// and Arasan's datatypes. Requires egtb.cpp (not anymore
// supplied with Arasan code).

#include "nalimov.h"
#include "constant.h"
#include "globals.h"
#include "debug.h"
extern "C" {
#ifndef _MAC
#include <malloc.h>
#endif
#include <stdio.h>
};

#include "egtb.cpp"

int NalimovTb::initTB(char *tbpath,uint64 tb_cache_size) {
    TB_CRC_CHECK = 0;
    int EGTBMenCount = IInitializeTb(tbpath);
    if(EGTBMenCount != 0) {
        void *egtb_cache = malloc((size_t)tb_cache_size);
        if (!egtb_cache) 
	  cerr << "Warning: failed to allocate memory for TB cache!" << endl; 
        FTbSetCacheSize(egtb_cache, (unsigned long)tb_cache_size);
    }
    return EGTBMenCount;
}

static inline void initSqCtrs(int *counter,unsigned int *squares,PieceType p,const Bitboard &bits) {
  int cPieces=0;
  Bitboard b(bits);
  Square sq;
  while (b.iterate(sq)) {
    ASSERT((p-1)*C_PIECES+cPieces<16);
    squares[(p-1)*C_PIECES+cPieces]=sq;
    cPieces++;
  }
  *counter=cPieces;
}

int NalimovTb::probe_tb(const Board &board, int &score, int ply) {

#ifdef _TRACE
    indent(ply); cout << "probing " << board << endl;
#endif
    int pcCount[10];
    unsigned int wSquares[16], bSquares[16];
    int iTB;
    int color;
    int invert;
    unsigned int *wp, *bp;
    int ep;
    INDEX index;
    int value;
    memset(wSquares,'\0',16*sizeof(int));
    memset(bSquares,'\0',16*sizeof(int));

    initSqCtrs(pcCount,   wSquares, Pawn, board.pawn_bits[White]);
    initSqCtrs(pcCount+1, wSquares, Knight, board.knight_bits[White]);
    initSqCtrs(pcCount+2, wSquares, Bishop, board.bishop_bits[White]);
    initSqCtrs(pcCount+3, wSquares, Rook, board.rook_bits[White]);
    initSqCtrs(pcCount+4, wSquares, Queen, board.queen_bits[White]);
    initSqCtrs(pcCount+5, bSquares, Pawn, board.pawn_bits[Black]);
    initSqCtrs(pcCount+6, bSquares, Knight, board.knight_bits[Black]);
    initSqCtrs(pcCount+7, bSquares, Bishop, board.bishop_bits[Black]);
    initSqCtrs(pcCount+8, bSquares, Rook, board.rook_bits[Black]);
    initSqCtrs(pcCount+9, bSquares, Queen, board.queen_bits[Black]);
    wSquares[15] = board.kingSquare(White);
    bSquares[15] = board.kingSquare(Black);

    iTB = IDescFindFromCounters(pcCount);
    if(iTB == 0) {
        return 0;
    }

    if(iTB > 0) {
        color = board.sideToMove() == White ? 0 : 1;
        invert = 0;
        wp = wSquares;
        bp = bSquares;
    } else {
        color = board.sideToMove() == White ? 1 : 0;
        invert = 1;
        wp = bSquares;
        bp = wSquares;
        iTB = - iTB;
    }

    if(!FRegisteredFun(iTB, color)) {
        return 0;
    }

    Square epsq = board.enPassantSq();
    ep  = (epsq == InvalidSquare) ? XX : MakeSquare(File(epsq),6,board.sideToMove());
    index = PfnIndCalcFun(iTB, color) (wp, bp, ep, invert);
    value = L_TbtProbeTable(iTB, color, index);
    if(value == bev_broken) {
        // illegal position
        cout << "tb probe: illegal position" << endl;
        return 0;
    }
    if(value > 0) {
        value = ((value-L_bev_mi1-1)*2+Constants::MATE-ply);
    }
    else if(value < 0) {
        value = ((value+L_bev_mi1)*2 - Constants::MATE+ply);
    }
#ifdef _TRACE
    indent(ply); cout << "hit, value=" << value << endl;
#endif
    score = value;
    return 1;

}
