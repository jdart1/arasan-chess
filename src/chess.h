// Copyright 1994-2017, 2019, 2021 by Jon Dart.  All Rights Reserved.

#ifndef _CHESS_H
#define _CHESS_H

#include "types.h"
#include "constant.h"

#include <cassert>
#include <vector>
#include <unordered_set>

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

FORCEINLINE ColorType SquareColor(Square square) {
  return ((ColorType)Colors[square]);
}

// Parse a square value in algebraic notation (e.g. "g7") and
// return a corresponding Square type.
extern Square SquareValue(const char *p);
extern Square SquareValue(const std::string &s);
extern Square SquareValue(char file, char rank);

extern char FileImage(Square sq);

extern char RankImage(Square sq);

extern char *SquareImage(Square sq);

// direction pawns move, by color
static const int Direction[2] = {1,-1};

// Algebraic notation to square mapping.
// Note: Arasan 10.0 and earlier used A8=0, B8=1, etc. but now we
// use the more conventional A1=0, B1=1, etc.
//
namespace chess {
  static const Square A1=0;
  static const Square B1=1;
  static const Square C1=2;
  static const Square D1=3;
  static const Square E1=4;
  static const Square F1=5;
  static const Square G1=6;
  static const Square H1=7;
  static const Square A2=8;
  static const Square B2=9;
  static const Square C2=10;
  static const Square D2=11;
  static const Square E2=12;
  static const Square F2=13;
  static const Square G2=14;
  static const Square H2=15;
  static const Square A3=16;
  static const Square B3=17;
  static const Square C3=18;
  static const Square D3=19;
  static const Square E3=20;
  static const Square F3=21;
  static const Square G3=22;
  static const Square H3=23;
  static const Square A4=24;
  static const Square B4=25;
  static const Square C4=26;
  static const Square D4=27;
  static const Square E4=28;
  static const Square F4=29;
  static const Square G4=30;
  static const Square H4=31;
  static const Square A5=32;
  static const Square B5=33;
  static const Square C5=34;
  static const Square D5=35;
  static const Square E5=36;
  static const Square F5=37;
  static const Square G5=38;
  static const Square H5=39;
  static const Square A6=40;
  static const Square B6=41;
  static const Square C6=42;
  static const Square D6=43;
  static const Square E6=44;
  static const Square F6=45;
  static const Square G6=46;
  static const Square H6=47;
  static const Square A7=48;
  static const Square B7=49;
  static const Square C7=50;
  static const Square D7=51;
  static const Square E7=52;
  static const Square F7=53;
  static const Square G7=54;
  static const Square H7=55;
  static const Square A8=56;
  static const Square B8=57;
  static const Square C8=58;
  static const Square D8=59;
  static const Square E8=60;
  static const Square F8=61;
  static const Square G8=62;
  static const Square H8=63;

  static const int AFILE=1;
  static const int BFILE=2;
  static const int CFILE=3;
  static const int DFILE=4;
  static const int EFILE=5;
  static const int FFILE=6;
  static const int GFILE=7;
  static const int HFILE=8;
}

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

extern bool validPiece(Piece p);

enum PieceType { Empty, Pawn, Knight, Bishop, Rook, Queen, King };

extern const int _sliders[16];
extern const Piece _pieces[8][2];

FORCEINLINE Piece MakePiece( PieceType type, ColorType color ) {
  return _pieces[type][color];
}

FORCEINLINE Piece MakeWhitePiece( PieceType type ) {
  return (Piece)((int)type);
}

FORCEINLINE Piece MakeBlackPiece( PieceType type ) {
  return (Piece)((int)type + 8);
}

FORCEINLINE PieceType TypeOfPiece( Piece piece ) {
  return ((PieceType)((int)piece & 7));
}

FORCEINLINE int Sliding(Piece p)
{
   return _sliders[TypeOfPiece(p)];
}

FORCEINLINE int Sliding(PieceType p)
{
   return _sliders[p];
}

FORCEINLINE ColorType PieceColor( Piece piece ) {
  return ((ColorType)((int)piece > 8));
}

FORCEINLINE ColorType ColorOfPiece( Piece piece ) {
  return ((ColorType)((int)piece > 8));
}

FORCEINLINE int IsEmptyPiece( Piece piece ) {
  return (piece == EmptyPiece);
}

extern PieceType PieceCharValue( char );

// 1-character representation of piece
extern char PieceImage(const PieceType p);

typedef uint64_t Move;

enum MoveFlags {NewMove = 0, Used = 1, Excluded = 2};

enum MoveType { Normal, KCastle, QCastle, EnPassant, Promotion };

union MoveUnion
{
   struct
   {
#if _BYTE_ORDER == _BIG_ENDIAN
        // invert order so moves have the same numeric 64-bit value as on little-endian arch
        uint8_t phase;
        uint8_t flags;
        uint8_t type;
        uint8_t capture;
        uint8_t promotion;
        uint8_t piece_moved;
        uint8_t dest;
        uint8_t start;
#else
        uint8_t start;
	uint8_t dest;
        uint8_t piece_moved;
        uint8_t promotion;
        uint8_t capture;
        uint8_t type;
        uint8_t flags;
        uint8_t phase;
#endif
   } contents;

   uint64_t intValue;

   struct
   {
#ifdef __BIG_ENDIAN__
      uint32_t hipart;
      uint32_t lopart;
#else
      uint32_t lopart;
      uint32_t hipart;
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
	     unsigned flags = 0)
   {
     contents.start = (uint8_t) start;
     contents.dest = (uint8_t) dest;
     contents.piece_moved = (uint8_t) pieceMoved;
     contents.promotion = (uint8_t) promotion;
     contents.capture = (uint8_t) capture;
     contents.type = (uint8_t) type;
     contents.flags = (uint8_t) flags;
   }
};

FORCEINLINE Move CreateMove(Square start, Square dest, PieceType pieceMoved,
  PieceType capture=Empty, PieceType promotion=Empty, MoveType type=Normal) {
  assert((type == Promotion) == (promotion != Empty));
  assert((type == Promotion) == (pieceMoved == Pawn && (dest/8 == 0 || dest/8 == 7)));
  return (Move)(uint64_t(start | (dest<<8) | (pieceMoved << 16) |
		       ((uint64_t)promotion << 24) |
		       ((uint64_t)capture << 32) |
		       ((uint64_t)type << 40)));
}

#define NullMove CreateMove(InvalidSquare,InvalidSquare,Empty)

extern Move CreateMove(const Board &board, Square start, Square dest, PieceType promotion );

FORCEINLINE unsigned Flags(const Move &move) {
  return (unsigned)((union MoveUnion*)&(move))->contents.flags;
}

FORCEINLINE void SetFlags(Move &move,uint8_t flags) {
  ((union MoveUnion*)&(move))->contents.flags = flags;
}

FORCEINLINE int IsUsed(Move move) {
  return (int)(((union MoveUnion*)&(move))->contents.flags & Used);
}

FORCEINLINE void SetUsed(Move &move) {
  ((union MoveUnion*)&(move))->contents.flags |= Used;
}

FORCEINLINE void ClearUsed(Move &move) {
  ((union MoveUnion*)&(move))->contents.flags &= ~Used;
}

FORCEINLINE int IsExcluded(Move move) {
  return (int)(((union MoveUnion*)&(move))->contents.flags & Excluded);
}

FORCEINLINE void SetExcluded(Move &move) {
  ((union MoveUnion*)&(move))->contents.flags |= Excluded;
}

FORCEINLINE void ClearExcluded(Move &move) {
  ((union MoveUnion*)&(move))->contents.flags &= ~Excluded;
}

FORCEINLINE void SetType(Move &move, MoveType t) {
  ((union MoveUnion*)&(move))->contents.type |= (uint8_t)t;
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
    ((union MoveUnion*)&(move))->contents.promotion = (uint8_t)p;
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

extern void MoveImage(Move m, std::ostream &out);

struct MoveHash
{
   size_t operator() (const Move &move) const
   {
      return std::hash<uint32_t>()(((union MoveUnion*)&move)->split.lopart);
   }
};

struct MoveCmp
{
   bool operator() (const Move &move1, const Move &move2) const {
      return MovesEqual(move1,move2);
   }
};

typedef std::unordered_set<Move,MoveHash,MoveCmp> MoveSet;

struct RootMove 
{
    Move move;
    score_t score;
    score_t tbRank;
    score_t tbScore;

RootMove(Move m, score_t s, score_t r, score_t ts) :
    move(m), score(s), tbRank(r), tbScore(ts)
    {
    }
};

typedef std::vector<RootMove> RootMoveList;

#endif
