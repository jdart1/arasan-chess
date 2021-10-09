// Copyright 1992, 1997-2000, 2008-2013, 2017 by Jon Dart.  All Rights Reserved.

#ifndef _ATTACK_H
#define _ATTACK_H

#include "types.h"
#include "chess.h"
#include "bitboard.h"

#ifdef BMI2
extern "C" {
#include <immintrin.h>
};
#endif

class Attacks
{
     // Attack bitmaps & related info

     public:

     // if the two square indices are in a straight or diagonal line
     // then directions[a][b] is the increment that is needed to 
     // move one square at a time from a to b.
     static const CACHE_ALIGN int directions[64][64];

     static const CACHE_ALIGN Bitboard betweenSquares[64][64];

     static const CACHE_ALIGN Bitboard knight_attacks[64];
     static const CACHE_ALIGN Bitboard pawn_attacks[64][2];
     static const CACHE_ALIGN Bitboard king_attacks[64];
     static const CACHE_ALIGN Bitboard file_mask[8];
     static const CACHE_ALIGN Bitboard rank_mask[8];
     static const CACHE_ALIGN Bitboard rank7mask[2];
     static const CACHE_ALIGN Bitboard rank_file_mask[64];
     static const CACHE_ALIGN Bitboard file_mask_down[64],file_mask_up[64];
     static const CACHE_ALIGN Bitboard rank_mask_right[64],rank_mask_left[64];
     static const CACHE_ALIGN Bitboard diag_mask[64];
     static const CACHE_ALIGN Bitboard diag_a1_lower_mask[64];
     static const CACHE_ALIGN Bitboard diag_a1_upper_mask[64];
     static const CACHE_ALIGN Bitboard diag_a8_lower_mask[64];
     static const CACHE_ALIGN Bitboard diag_a8_upper_mask[64];
     static const CACHE_ALIGN Bitboard diag_a1_mask[64];
     static const CACHE_ALIGN Bitboard diag_a8_mask[64];
     
     static const Bitboard center, abcd_mask, efgh_mask;

     // mask for e.p. checking
     static const Bitboard ep_mask[8][2];

     // arrays for "magic" attack generator

#if defined(BMI2) && defined(_64BIT)
     struct MagicData {
         uint16_t *data;
         Bitboard mask1;
         Bitboard mask2;
     };

     static CACHE_ALIGN uint16_t magicmovesdb[107648];

#else
     struct MagicData {
         Bitboard mask;
         Bitboard magic;
         Bitboard *moves;
         unsigned shift;
     };

     static CACHE_ALIGN Bitboard magicmovesbdb[5248];
     static CACHE_ALIGN Bitboard magicmovesrdb[102400];

     // constants, used for initialization:
     static const CACHE_ALIGN Bitboard b_magic[64];
     static const CACHE_ALIGN unsigned b_shift[64];
     static const CACHE_ALIGN Bitboard r_magic[64];
     static const CACHE_ALIGN unsigned r_shift[64];

#endif

     static CACHE_ALIGN MagicData bishopMagicData[64];
     static CACHE_ALIGN MagicData rookMagicData[64];

     FORCEINLINE static Bitboard fileMask(Square sq) {
       return file_mask[Files[sq]-1];
     }
     
     FORCEINLINE static const Bitboard rookAttacks(Square sq,
                                                   const Bitboard &occupied) {
#ifdef _64BIT
#ifdef BMI2
         return _pdep_u64(rookMagicData[sq].data[_pext_u64(occupied,rookMagicData[sq].mask1)], rookMagicData[sq].mask2);
#else
         return *(rookMagicData[sq].moves+(int)
                (((occupied & rookMagicData[sq].mask)*rookMagicData[sq].magic)>>rookMagicData[sq].shift));
#endif
#else
         Bitboard b(rookMagicData[sq].mask & occupied);
         return *(rookMagicData[sq].moves+(int)(
                ((b.lovalue()*rookMagicData[sq].magic.lovalue()) ^
                (b.hivalue()*rookMagicData[sq].magic.hivalue()))>>
                rookMagicData[sq].shift));
#endif
     }

   FORCEINLINE static const Bitboard bishopAttacks(Square sq, 
					  const Bitboard &occupied) {
#ifdef _64BIT
#ifdef BMI2
      return _pdep_u64(bishopMagicData[sq].data[_pext_u64(occupied,bishopMagicData[sq].mask1)], bishopMagicData[sq].mask2);
#else
      return *(bishopMagicData[sq].moves+(int)
	      (((occupied & bishopMagicData[sq].mask)*bishopMagicData[sq].magic)>>bishopMagicData[sq].shift));
#endif
#else
      Bitboard b(bishopMagicData[sq].mask & occupied);
      return *(bishopMagicData[sq].moves+(int)(
             ((b.lovalue()*bishopMagicData[sq].magic.lovalue()) ^
             (b.hivalue()*bishopMagicData[sq].magic.hivalue()))>>
             bishopMagicData[sq].shift));
#endif
   }

   FORCEINLINE static const Bitboard queenAttacks(Square sq,
	  				       const Bitboard &occupied) {
     return (rookAttacks(sq,occupied) & bishopAttacks(sq,occupied));
   }

     // Initialize the bitmaps.  Call before using this class.
     static void init();

 private:
#if !defined(BMI2) || !defined(_64BIT)
     static void setRookAttacks(Square sq,
                                const Bitboard &occupied, const Bitboard &value) {
#ifdef _64BIT
         *(Bitboard*)(rookMagicData[sq].moves+(int)
            (((occupied & rookMagicData[sq].mask)*
            rookMagicData[sq].magic)>>rookMagicData[sq].shift)) = value;
#else
         Bitboard b(rookMagicData[sq].mask & occupied);
         *(rookMagicData[sq].moves+(int)(
            ((b.lovalue()*rookMagicData[sq].magic.lovalue()) ^
            (b.hivalue()*rookMagicData[sq].magic.hivalue()))>>
            rookMagicData[sq].shift)) = value;
#endif
     }

     static void setBishopAttacks(Square sq,
		       const Bitboard &occupied, const Bitboard &value) {
#ifdef _64BIT
         *(Bitboard*)(bishopMagicData[sq].moves+(int)
            (((occupied & bishopMagicData[sq].mask)*
            bishopMagicData[sq].magic)>>bishopMagicData[sq].shift)) = value;
#else
         Bitboard b(bishopMagicData[sq].mask & occupied);
         *(bishopMagicData[sq].moves+(int)(
            ((b.lovalue()*bishopMagicData[sq].magic.lovalue()) ^
            (b.hivalue()*bishopMagicData[sq].magic.hivalue()))>>
            bishopMagicData[sq].shift)) = value;
#endif
     }
#endif

     static void initMagicData(void);
};

#endif
