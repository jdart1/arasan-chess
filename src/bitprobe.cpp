// Copyright 2004, 2008, 2011, 2021 Jon Dart. All Rights Reserved.
//
// This module interfaces with bitbases and other in-memory scoring
// tables.

#include "bitprobe.h"
#include "globals.h"

#include <cassert>
#include <fstream>

static int getIndex(Square pawnSq, Square kingSq, Square oppKingSq) {
  return ((((pawnSq-8)<<6) | kingSq)<<6)| oppKingSq;
}

int lookupBitbase(const uint8_t *base, int index) {
   assert(index<24576*8);
   return (base[index/8] & (1<<(index%8))) != 0;
}

int lookupBitbase(Square whiteK, Square whiteP, Square blackK,
   ColorType strongerSide, ColorType sideToMove) {
   if (strongerSide == White) {
      if (sideToMove == White)
        return lookupBitbase(globals::baseKPKW,getIndex(whiteP,whiteK,blackK));
      else
        return lookupBitbase(globals::baseKPKB,getIndex(whiteP,whiteK,blackK));
   }
   else {
      if (sideToMove == White)
        return lookupBitbase(globals::baseKPKB,getIndex(Flip[whiteP],Flip[whiteK],Flip[blackK]));
      else
        return lookupBitbase(globals::baseKPKW,getIndex(Flip[whiteP],Flip[whiteK],Flip[blackK]));
   }
}

int lookupBitbase(const Board &board) {
   const Material &wMat = board.getMaterial(White);
   const Material &bMat = board.getMaterial(Black);
   int index;
   if (wMat.materialLevel() > 0 || 
       bMat.materialLevel() > 0) return NOHIT;
   if (wMat.infobits() == Material::KP &&
       bMat.kingOnly()) {
     Square pawnSq = board.pawn_bits[White].firstOne();
     index = getIndex(pawnSq,board.kingSquare(White),board.kingSquare(Black));
     if (board.sideToMove() == Black)
        return lookupBitbase(globals::baseKPKB,index);
     else
        return lookupBitbase(globals::baseKPKW,index);
   }
   else if (bMat.infobits() == Material::KP &&
       wMat.kingOnly()) {
     Square pawnSq = board.pawn_bits[Black].firstOne();
     index = getIndex(pawnSq,board.kingSquare(Black),board.kingSquare(White));
     if (board.sideToMove() == Black)
        return lookupBitbase(globals::baseKPKW,index);
     else
        return lookupBitbase(globals::baseKPKB,index);
   }
   return NOHIT; // shouldn't get here
}

/*
void generateBitBase() {
   uint8_t * bKPKW = new uint8_t[24576];
   uint8_t * bKPKB = new uint8_t[24576];
   memset(bKPKW,'\0',24576);
   memset(bKPKB,'\0',24576);
   extern int distances[64][64];
   for (Square pawnsq = 8; pawnsq < 56; pawnsq++) {
      for (Square wsq = 0; wsq < 64; wsq++) {
         for (Square bsq = 0; bsq < 64; bsq++) {
            extern int distances[64][64];
            if (bsq == pawnsq || wsq == pawnsq || bsq == wsq) continue;
            if (distances[bsq][wsq]<=1) continue;
            Board b;
            b.makeEmpty();
            b.setContents(WhitePawn,pawnsq);
            b.setContents(BlackKing,bsq);
            b.setContents(WhiteKing,wsq);
            b.setSideToMove(White);
            b.setCastleStatus(CantCastleEitherSide,White);
            b.setCastleStatus(CantCastleEitherSide,Black);
            b.setSecondaryVars();
            int score;
            int index = getIndex(pawnsq,wsq,bsq);
            int uint8_ti = index/8;
            if (uint8_ti>=24576) cout << "error" << endl;
            assert(uint8_ti<24576);
            if (probe_tb(b, score, 0)) {
               if (score>0) {
                  bKPKW[uint8_ti] |= 1<<(index%8);
               }
            }
            else cout << b << endl;
            b.setSideToMove(Black);
            if (probe_tb(b, score, 0)) {
               if (score<0) {
                  bKPKB[uint8_ti] |= 1<<(index%8);
               }
            }
            else cout << b << endl;
         }
      }
   }
   cout << "const CACHE_ALIGN uint8_t globals::baseKPKW[24576] = {" << endl;
   for (int i = 0; i < 24576; i++) {
       cout << " 0x" << (hex) << (unsigned int)bKPKW[i];
       if (i != 24575) cout << ",";
       if (((i+1) % 12) == 0) cout << endl;
   }
   cout << "};" << endl;
   cout << "const CACHE_ALIGN uint8_t globals::baseKPKB[24576] = {" << endl;
   for (int i = 0; i < 24576; i++) {
       cout << " 0x" << (hex) << (unsigned int)bKPKB[i];
       if (i != 24575) cout << ",";
       if (((i+1) % 12) == 0) cout << endl;
   }
   cout << "};" << endl;
   cout << (dec);
   delete [] bKPKB;
   delete [] bKPKW;
}
*/
