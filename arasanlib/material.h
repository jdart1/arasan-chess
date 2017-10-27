// Copyright 1994, 2014, 2017 by Jon Dart.  All Rights Reserved.

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "chess.h"
#include "debug.h"
#include "params.h"

class Material
{
    friend class Board;
 public:
 Material():
    info(0), total(0), level(0), count(0)
      {
      }
	
    enum {K  = 0x100000};
    enum {KP = 0x100001};
    enum {KBP = 0x100101};
    enum {KBN = 0x100110};
    enum {KBB = 0x100200};
    enum {KRB = 0x101100};
    enum {KRN = 0x101010};
    enum {KRBN = 0x101110};
    enum {KNN = 0x100020};
    enum {KRR = 0x102000};
    enum {KB  = 0x100100};
    enum {KN  = 0x100010};
    enum {KR  = 0x101000};
    enum {KQ  = 0x110000};
    enum {KQN = 0x110010};
    enum {KQB = 0x110100};

    enum {PieceMask = 0x1ffff0};
        
    FORCEINLINE void addPiece(const PieceType p)
    {
        info += masks[(int)p];
        total += (stored_score_t)Params::PieceValue(p);
        level += levels[(int)p];
        count++;
        ASSERT(level >=0);
    }

    FORCEINLINE void removePiece(const PieceType p)
    {
        info -= masks[(int)p];
        total -= (stored_score_t)Params::PieceValue(p);
        level -= levels[(int)p];
        count--;
        ASSERT(level >=0);
    }

    FORCEINLINE void addPawn()
    {
        info += masks[(int)Pawn];
        total += (stored_score_t)Params::PAWN_VALUE;
        ++count; 
    }

    FORCEINLINE void removePawn()
    {
        info -= masks[(int)Pawn];
        total -= (stored_score_t)Params::PAWN_VALUE;
        --count;
    }

    // return the total material value:
    score_t value() const {
       return (score_t)total;
    }
	
    // return value of pieces (excluding pawns)
    score_t pieceValue() const {
       return (score_t)(total - (stored_score_t)Params::PAWN_VALUE*pawnCount());
    }
	
    uint32_t infobits() const	{
        return info;
    }

    uint32_t pieceBits() const {
        return info & PieceMask;
    }
	
    // return the total number of pieces + pawns
    int men() const {
        return count;
    }
        
    // return the number of pieces (excluding the King)
    int pieceCount() const {
        return count-pawnCount()-1;
    }
        
    // return the number of pawns
    int pawnCount() const
    {
        return (info & 0xf);
    }
        
    void clearPawns()
    {
        info &= ~0xf;
    }
        
    int queenCount() const
    {
        return (info & 0xf0000) >> 16;
    }

    int hasQueen() const
    {
        return (info & 0xf0000);
    }

    int rookCount() const
    {
        return (info & 0xf000) >> 12;
    }
        
    int hasRook() const
    {
        return (info & 0xf000);
    }

    int knightCount() const
    {
        return (info & 0xf0) >> 4;
    }
        
    int hasKnight() const
    {
        return (info & 0xf0);
    }

    int bishopCount() const
    {
        return (info & 0xf00) >> 8;
    }

    int hasBishop() const
    {
        return (info & 0xf00);
    }

    int minorCount() const 
    {
        return knightCount() + bishopCount();
    }

    int majorCount() const 
    {
        return 2*queenCount() + rookCount();
    }

    int hasMinor() const {
        return (info & 0xff0) != 0;
    }

    int hasMajor() const {
        return (info & 0xff000) != 0;
    }

    // True if only king and pawns:
    bool noPieces() const
    {
        return (info & (0xffffffff-0x100000-0xf)) == 0;
    }
        
    bool hasPieces() const
    {
        return !noPieces();
    }

    // True if no pawns
    bool noPawns() const
    {
        return (info & 0xf) == 0;
    }
	
    // True if has pawns
    bool hasPawns() const
    {
        return (info & 0xf) != 0;
    }
	
    // True if bare king:
    bool kingOnly() const
    {
        return (info & 0xfffff) == 0;
    }
        
    // This provides a rough measure of the material
    // (used mostly for king safety calculation)
    int materialLevel() const {
        return (level > 31) ? 31 : level;
    }

 private:
    void clear() {
        info = level = count = 0;
		total = 0;
    }

	uint32_t info;
	stored_score_t total;
	uint32_t level;
    int count;
    static const int32_t masks[8];
    static const int32_t masks2[8];
    static const int32_t levels[8];
};

#endif

