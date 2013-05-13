#ifndef _BOARDHASH_H_
#define _BOARDHASH_H_

#include "board.h"
#include "debug.h"

extern const CACHE_ALIGN hash_t hash_codes[64][16];
extern const CACHE_ALIGN hash_t ep_codes[64];
extern const CACHE_ALIGN hash_t b_castle_status[6];
extern const CACHE_ALIGN hash_t w_castle_status[6];

class BoardHash
{
	public:
		
	static void free();
	
	static hash_t hashCode(const Board &);
	
        static hash_t pawnHash(const Board &, ColorType side);

        static FORCEINLINE hash_t Xor( hash_t h, const Square sq, const Piece piece ) {
           ASSERT(OnBoard(sq));
           return h ^ hash_codes[sq][(int)piece];
        }

        static hash_t kingPawnHash(const Board &b) {
           hash_t h = b.pawnHashCodeB;
           h ^= b.pawnHashCodeW;
           h = Xor(Xor(h,b.kingSquare(White), WhiteKing), b.kingSquare(Black), BlackKing);
           if (b.sideToMove() == Black)
	      h |= 1;
           else
	      h &= ~1;
           return h;
        }

        static hash_t kingCoverHash(const Board &board, ColorType side) {
          if (side == White) 
            return board.pawnHashCodeW ^ hash_codes[board.kingSquare(White)][WhiteKing];
          else
            return board.pawnHashCodeB ^ hash_codes[board.kingSquare(Black)][BlackKing];
	}

};

#endif

