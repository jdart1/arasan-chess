// Copyright 1992-2015, 2021 by Jon Dart. All Rights Reserved.

#ifndef _BOARDHASH_H_
#define _BOARDHASH_H_

#include "board.h"

#include <cassert>

extern const CACHE_ALIGN hash_t hash_codes[64][16];
extern const CACHE_ALIGN hash_t ep_codes[64];
extern const CACHE_ALIGN hash_t b_castle_status[6];
extern const CACHE_ALIGN hash_t w_castle_status[6];

class BoardHash
{
	public:
		
	static hash_t hashCode(const Board &);
	
        static hash_t pawnHash(const Board &, ColorType side);

        static FORCEINLINE hash_t Xor( hash_t h, const Square sq, const Piece piece ) {
           assert(OnBoard(sq));
           return h ^ hash_codes[sq][(int)piece];
        }

        static hash_t setSideToMove(hash_t input, ColorType side) {
            if (side == Black) {
                return input & 0x7fffffffffffffffULL;
            } else {
                return input | 0x8000000000000000ULL;
            }
        }

        static ColorType sideToMove(hash_t input) {
            return (input & 0x8000000000000000ULL) ? White : Black;
        }

        static hash_t kingPawnHash(const Board &b) {
           hash_t h = b.pawnHashCodeB;
           h ^= b.pawnHashCodeW;
           h = Xor(Xor(h,b.kingSquare(White), WhiteKing), b.kingSquare(Black), BlackKing);
           return setSideToMove(h,b.sideToMove());
        }

        static hash_t kingPawnHash(const Board &board, ColorType side) {
          if (side == White) 
            return board.pawnHashCodeW ^ board.pawnHashCodeB ^ hash_codes[board.kingSquare(White)][WhiteKing] ^ w_castle_status[board.castleStatus(White)];
          else
            return board.pawnHashCodeW ^ board.pawnHashCodeB ^ hash_codes[board.kingSquare(Black)][BlackKing] ^ b_castle_status[board.castleStatus(Black)];
	}
};

#endif

