// Copyright 1994-2013 by Jon Dart.  All Rights Reserved.

#ifndef _CHESS_H
#define _CHESS_H

#include "types.h"
#include "constant.h"
#include "debug.h"

class Board;

// basic types

enum ColorType {White,Black};

FORCEINLINE ColorType OppositeColor( ColorType color )
{
    return (ColorType)(1-(int)color);
}

extern const char *ColorImage( ColorType side );

extern const CACHE_ALIGN int Files[64];
extern const int Edge[64];
extern const int Colors[64];
extern const int Center[64];
extern const int Corners[64];
extern const int Rank7[64];
extern const int Flip[64];
extern const int Ranks[8][2];

FORCEINLINE Square MakeSquare( int file, int rank, ColorType side ) {
  return ((side == White) ? ((rank-1)*8 + file - 1) : (55 - (rank-1)*8 + file));
}
    
// returns "rank" from the perspective of "side". 1 = 1st
// rank for side, 8 = last rank.	    
template <ColorType side>
static FORCEINLINE int Rank(Square square) {
   return side == White ? 1+square/8 : 8 - square/8;
}

static FORCEINLINE int Rank(Square square, ColorType side) {
   return Ranks[square/8][side];
}

#define WhiteRank(square) Rank(square,White)
#define BlackRank(square) Rank(square,Black)

// returns file.  Queen rook file = 1, King rook file = 8.
#define File(square) (square%8+1)
	    
#define OnBoard(square) ((square & ~0x3f) == 0)

#define OnEdge(square) Edge[square]

#define InCenter(square) Center[square]

#define InCorner(square) Corners[square]
    
#define IsInvalid(square) (square == InvalidSquare)
    
#define SquareColor(square) ((ColorType)Colors[square])
    
// Parse a square value in algebraic notation (e.g. "g7") and
// return a corresponding Square type.
extern Square SquareValue(const char *p);
extern Square SquareValue(const string &s);
    
extern char FileImage(Square sq);

extern char RankImage(Square sq);

extern char *SquareImage(Square sq); 

// direction pawns move, by color
static const int Direction[2] = {1,-1};

// Algebraic notation to square mapping.
// Note: Arasan 10.0 and earlier used A8=0, B8=1, etc. but now we
// use the more conventional A1=0, B1=1, etc.
//
#define A1 0
#define B1 1
#define C1 2
#define D1 3
#define E1 4
#define F1 5
#define G1 6
#define H1 7
#define A2 8
#define B2 9
#define C2 10
#define D2 11
#define E2 12
#define F2 13
#define G2 14
#define H2 15
#define A3 16
#define B3 17
#define C3 18
#define D3 19
#define E3 20
#define F3 21
#define G3 22
#define H3 23
#define A4 24
#define B4 25
#define C4 26
#define D4 27
#define E4 28
#define F4 29
#define G4 30
#define H4 31
#define A5 32
#define B5 33
#define C5 34
#define D5 35
#define E5 36
#define F5 37
#define G5 38
#define H5 39
#define A6 40
#define B6 41
#define C6 42
#define D6 43
#define E6 44
#define F6 45
#define G6 46
#define H6 47
#define A7 48
#define B7 49
#define C7 50
#define D7 51
#define E7 52
#define F7 53
#define G7 54
#define H7 55
#define A8 56
#define B8 57
#define C8 58
#define D8 59
#define E8 60
#define F8 61
#define G8 62
#define H8 63

#define AFILE 1
#define BFILE 2
#define CFILE 3
#define DFILE 4
#define EFILE 5
#define FFILE 6
#define GFILE 7
#define HFILE 8

enum Piece {
  EmptyPiece = 0,
  WhitePawn = 1,
  WhiteKnight = 2,
  WhiteBishop = 3,
  WhiteRook = 4,
  WhiteQueen = 5,
  WhiteKing = 6,
  BlackPawn = 9,
  BlackKnight = 10,
  BlackBishop = 11,
  BlackRook = 12,
  BlackQueen = 13,
  BlackKing = 14
};

enum PieceType { Empty, Pawn, Knight, Bishop, Rook, Queen, King };

#define PAWN_VALUE 100
#define BISHOP_VALUE (int)(3.25*PAWN_VALUE)
#define KNIGHT_VALUE (int)(3.25*PAWN_VALUE)
#define ROOK_VALUE 5*PAWN_VALUE
#define QUEEN_VALUE (int)(9.75*PAWN_VALUE)
#define KING_VALUE 32*PAWN_VALUE

extern const int PieceValues[8];
extern const int _sliders[16];
extern const Piece _pieces[8][2];	   

FORCEINLINE int Sliding(Piece p) 
{
   return _sliders[p];
}

FORCEINLINE int Sliding(PieceType p) 
{
   return _sliders[p];
}

#define MakePiece( type, color ) _pieces[type][color]
#define MakeWhitePiece( type ) (Piece)((int)type)
#define MakeBlackPiece( type ) (Piece)((int)type + 8)
    
#define TypeOfPiece(piece) ((PieceType)((int)piece & 7))
   
#define PieceColor(piece) ((ColorType)((int)piece > 8))
#define ColorOfPiece(piece) ((ColorType)((int)piece > 8))

#define IsEmptyPiece(piece) (piece == EmptyPiece)
   
#define PieceValue(piece) PieceValues[TypeOfPiece(piece)]   

extern PieceType PieceCharValue( char );

// 1-character representation of piece
extern char PieceImage(const PieceType p);

typedef uint64 Move;

enum MoveFlags {NewMove = 0, Used = 1, Forced = 8,
  Forced2 = 16};

enum MoveType { Normal, KCastle, QCastle, EnPassant, Promotion };

union MoveUnion
{
   struct
   {
#ifdef __BIG_ENDIAN__
        // invert order so moves have the same numeric 64-bit value as on little-endian arch
        byte phase;
        byte flags;
        byte type;
        byte capture;
        byte promotion;
        byte piece_moved;
        byte dest;
        byte start;
#else
        byte start;
	byte dest;
        byte piece_moved;
        byte promotion;
        byte capture;
        byte type;
        byte flags;
        byte phase;
#endif
   } contents;

   uint64 intValue;

   struct
   {
#ifdef __BIG_ENDIAN__
      uint32 hipart;
      uint32 lopart;
#else
      uint32 lopart;
      uint32 hipart;
#endif
   } split;

   MoveUnion(Move m)
     :intValue(m)
   {
   }

   operator Move() const {
     return (Move)intValue;
   }

   MoveUnion(Square start, Square dest, PieceType pieceMoved,
	     PieceType promotion, 
             PieceType capture, MoveType type,
	     int flags = 0) 
   {
     contents.start = start;
     contents.dest = dest;
     contents.piece_moved = pieceMoved;
     contents.promotion = promotion;
     contents.capture = capture;
     contents.type = type;
     contents.flags = flags;
   }
};

FORCEINLINE Move CreateMove(Square start, Square dest, PieceType pieceMoved,
  PieceType capture=Empty, PieceType promotion=Empty, MoveType type=Normal) {
  ASSERT((type == Promotion) == (promotion != Empty));
  ASSERT((type == Promotion) == (pieceMoved == Pawn && (dest/8 == 0 || dest/8 == 7)));
  return (Move)(uint64(start | (dest<<8) | (pieceMoved << 16) |
		       ((uint64)promotion << 24) |
		       ((uint64)capture << 32) |
		       ((uint64)type << 40)));
}	

#define NullMove CreateMove(InvalidSquare,InvalidSquare,Empty)

extern Move CreateMove(const Board &board, Square start, Square dest, PieceType promotion );

FORCEINLINE int IsUsed(Move move) {
  return (int)(((union MoveUnion*)&(move))->contents.flags & Used);
}

FORCEINLINE void SetUsed(Move &move) {
  ((union MoveUnion*)&(move))->contents.flags |= Used;
}

FORCEINLINE void ClearUsed(Move &move) {
  ((union MoveUnion*)&(move))->contents.flags &= ~Used;
}

FORCEINLINE int IsForced(Move move) {
  return (int)(((union MoveUnion*)&(move))->contents.flags & Forced);
}

FORCEINLINE int IsForced2(Move move) {
  return (int)(((union MoveUnion*)&(move))->contents.flags & Forced2);
}

FORCEINLINE void SetForced(Move &move) {
  ((union MoveUnion*)&(move))->contents.flags |= Forced;
}

FORCEINLINE void SetForced2(Move &move) {
  ((union MoveUnion*)&(move))->contents.flags |= Forced2;
}

FORCEINLINE void SetType(Move &move, MoveType t) {
  ((union MoveUnion*)&(move))->contents.type |= (byte)t;
}

FORCEINLINE MoveType TypeOfMove(Move move) {
  return (MoveType)(((union MoveUnion*)&move)->contents.type);
}

FORCEINLINE int MovesEqual(Move move1,Move move2) {
  return ((union MoveUnion*)&move1)->split.lopart  == ((union MoveUnion*)&move2)->split.lopart;
}

FORCEINLINE Square StartSquare(Move move) {
  return (Square)(((union MoveUnion*)&move)->contents.start);
}

FORCEINLINE Square DestSquare(Move move) {
  return (Square)(((union MoveUnion*)&move)->contents.dest);
}

FORCEINLINE PieceType PromoteTo(Move move) {
  return ((PieceType)((union MoveUnion*)&move)->contents.promotion);
}

FORCEINLINE void SetPromotion(Move &move,PieceType p) {
    ((union MoveUnion*)&(move))->contents.promotion = (byte)p;
}

FORCEINLINE PieceType PieceMoved(Move move) {
  return ((PieceType)((union MoveUnion*)&move)->contents.piece_moved);
}

FORCEINLINE PieceType Capture(Move move) {
  return (PieceType)(((union MoveUnion*)&move)->contents.capture);
}

FORCEINLINE int CaptureOrPromotion(Move move) {
    return (Capture(move) != Empty || PromoteTo(move) != Empty);
}

FORCEINLINE int IsPromotion(Move move) {
    return (TypeOfMove(move) == Promotion);
}

FORCEINLINE int IsNull(Move move) {
    return (move == NullMove);
}
	
FORCEINLINE int Gain(Move move) {
    return ((TypeOfMove(move) == Promotion) ?
            PieceValue(Capture(move)) + PieceValue(PromoteTo(move)) - PAWN_VALUE 
            : PieceValue(Capture(move)));
}

FORCEINLINE int MVV_LVA(Move move) {
    return Gain(move) - PieceValue(PieceMoved(move));
}

extern void MoveImage(Move m,ostream &out);


#endif
