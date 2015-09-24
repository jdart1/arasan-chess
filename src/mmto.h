// Copyright 2015 by Jon Dart. All Rights Reserved
#ifndef _MMTO_H
#define _MMTO_H

namespace mmto {

typedef unsigned short PackedMove;

PackedMove pack_move(Move m) 
{
   return IsNull(m) ? 0 : (PackedMove)(StartSquare(m) | (DestSquare(m)<<6) | (PromoteTo(m) << 12));
}

Move unpack_move(const Board &board,PackedMove p) 
{
   if (p == 0) return NullMove;
   return CreateMove(board,(Square)(p&0x3f),(Square)((p>>6)&0x3f),
                     (PieceType)((p>>12) & 0xf));
}

// Could be a vector, but we use a more compact array representation:
 struct Pv {
   int value;
   int len;
   PackedMove *pv;
 Pv(int val, const PackedMove *moves, int length)
 : value(val),len(length),pv(NULL)
   {
     if (len) {
       pv = new PackedMove[len];
       memcpy(pv,moves,sizeof(PackedMove)*length);
     }
   }
   
   ~Pv() 
   {
     delete [] pv;
   }
   Pv(const Pv &source) 
   {
     len = source.len;
     value = source.value;
     if (len) {
       pv = new PackedMove[len];
       memcpy(pv,source.pv,sizeof(PackedMove)*len);
     }
     else {
       pv = NULL;
     }
   }
   Pv & operator = (const Pv &source) 
   {
     delete [] pv;
     pv = NULL;
     len = source.len;
     value = source.value;
     if (len) {
       pv = new PackedMove[len];
       memcpy(pv,source.pv,sizeof(PackedMove)*len);
     }
     return *this;
   }
 };

struct PositionInfo 
{
   Move record_move;
   vector<Pv> pvs;
   PositionInfo(const Move record):
   record_move(record) 
      {
      }
};

typedef vector <PositionInfo> GameInfo;

};

#endif
