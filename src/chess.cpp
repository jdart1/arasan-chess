// Copyright 1994-2008 by Jon Dart. All Rights Reserved.

#include "chess.h"
#include "debug.h"
#include "board.h"
#include "bitboard.h"
#include "util.h"
#include <iostream>
#include <iomanip>

using namespace std;

const int CACHE_ALIGN Files[64] =
{
   1, 2, 3, 4, 5, 6, 7, 8,
   1, 2, 3, 4, 5, 6, 7, 8,
   1, 2, 3, 4, 5, 6, 7, 8,
   1, 2, 3, 4, 5, 6, 7, 8,
   1, 2, 3, 4, 5, 6, 7, 8,
   1, 2, 3, 4, 5, 6, 7, 8,
   1, 2, 3, 4, 5, 6, 7, 8,
   1, 2, 3, 4, 5, 6, 7, 8
};

const int Ranks[8][2] =
{
  {1,8},
  {2,7},
  {3,6},
  {4,5},
  {5,4},
  {6,3},
  {7,2},
  {8,1}
};

const int Edge[64] =
{
   1, 1, 1, 1, 1, 1, 1, 1,
   1, 0, 0, 0, 0, 0, 0, 1,
   1, 0, 0, 0, 0, 0, 0, 1,
   1, 0, 0, 0, 0, 0, 0, 1,
   1, 0, 0, 0, 0, 0, 0, 1,
   1, 0, 0, 0, 0, 0, 0, 1,
   1, 0, 0, 0, 0, 0, 0, 1,
   1, 1, 1, 1, 1, 1, 1, 1
};

const int Colors[64] =
{
   1, 0, 1, 0, 1, 0, 1, 0,
   0, 1, 0, 1, 0, 1, 0, 1,
   1, 0, 1, 0, 1, 0, 1, 0,
   0, 1, 0, 1, 0, 1, 0, 1,
   1, 0, 1, 0, 1, 0, 1, 0,
   0, 1, 0, 1, 0, 1, 0, 1,
   1, 0, 1, 0, 1, 0, 1, 0,
   0, 1, 0, 1, 0, 1, 0, 1
};

const int Center[64] =
{
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 1, 1, 0, 0, 0,
   0, 0, 0, 1, 1, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0
};

const int Corners[64] =
{
   1, 0, 0, 0, 0, 0, 0, 1,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   1, 0, 0, 0, 0, 0, 0, 1
};

const int Rank7[64] =
{
   0, 0, 0, 0, 0, 0, 0, 0,
   1, 1, 1, 1, 1, 1, 1, 1,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0,
   1, 1, 1, 1, 1, 1, 1, 1,
   0, 0, 0, 0, 0, 0, 0, 0
};

const int Flip[64] =
{
        A8, B8, C8, D8, E8, F8, G8, H8,
        A7, B7, C7, D7, E7, F7, G7, H7,
        A6, B6, C6, D6, E6, F6, G6, H6,
        A5, B5, C5, D5, E5, F5, G5, H5,
        A4, B4, C4, D4, E4, F4, G4, H4,
        A3, B3, C3, D3, E3, F3, G3, H3,
        A2, B2, C2, D2, E2, F2, G2, H2,
        A1, B1, C1, D1, E1, F1, G1, H1
};

const char *ColorImage( ColorType side )
{
    static const char *images[2] = { "White", "Black" };
    return images[side];
}

Square SquareValue(const char *p )
{
    int rank, file;

    if ((*p >= 'a') && (*p <= 'h'))
       file = *p - 'a' + 1;
    else
       return InvalidSquare;
    ++p;
    if ((*p >= '1') && (*p <= '8'))
       rank = *p - '1' + 1;
    else
       return InvalidSquare;
    return MakeSquare(file,rank,White);
}

Square SquareValue(const string &str)
{
    int rank, file;

    string::const_iterator it = str.begin();
    if ((*it >= 'a') && (*it <= 'h'))
       file = *it - 'a' + 1;
    else
       return InvalidSquare;
    it++;
    if ((*it >= '1') && (*it <= '8'))
       rank = *it - '1' + 1;
    else
       return InvalidSquare;
    return MakeSquare(file,rank,White);
}

char FileImage(Square sq)
{
    return 'a' + File(sq) - 1;
}

char RankImage(Square sq)
{
    return '1' + sq/8;
}

char *SquareImage(Square sq) {
    static char chars[3];
    chars[0] = FileImage(sq);
    chars[1] = RankImage(sq);
    chars[2] = '\0';
    return chars;
}

const int PieceValues[8] = 
{
   0,
   PAWN_VALUE,
   KNIGHT_VALUE,
   BISHOP_VALUE,
   ROOK_VALUE,
   QUEEN_VALUE,
   KING_VALUE,
   0
};

const Piece _pieces[8][2] =
{ {EmptyPiece,EmptyPiece},
{ WhitePawn, BlackPawn },
{ WhiteKnight, BlackKnight },
{ WhiteBishop, BlackBishop },
{ WhiteRook, BlackRook },
{ WhiteQueen, BlackQueen },
{ WhiteKing, BlackKing },
{ EmptyPiece, EmptyPiece} };

const int _sliders[16] =
{ 0, 0, 0, 1, 1, 1, 0, 0,
  0, 0, 0, 1, 1, 1, 0, 0 };

static const char Images[] = "?PNBRQK?";

PieceType PieceCharValue( char c )
{
    const char *p = Images;
    p = strchr(p,c);
    if (p)
       return (PieceType)(p-Images);
    else
       return Empty;
}

char PieceImage(const PieceType p)
{
    return Images[p];
}

void MoveImage(Move m,ostream &out) {
    char image[10];
    if (IsNull(m))
    {
       cout << "(null)";
       return;
    }
    image[0] = FileImage(StartSquare(m));
    image[1] = RankImage(StartSquare(m));
    image[2] = Capture(m) == Empty ? '-' : 'x';
    image[3] = FileImage(DestSquare(m));
    image[4] = RankImage(DestSquare(m));
    int i = 5;
    if (PromoteTo(m) != Empty) {
       image[i++] = '=';
       image[i++] = PieceImage(PromoteTo(m));
    }
    image[i] = '\0';
    out << image;
}

static MoveType get_move_type(const Board &board, Square start, Square dest)
{
    MoveType move_type = Normal;
    switch (TypeOfPiece(board[start])) {
    case Pawn:
       if (Rank(dest,board.sideToMove()) == 8)
       {
          move_type = Promotion;
       }
       else if (IsEmptyPiece(board[dest]))
       {
          if (File(dest) != File(start))
	  {
	     move_type = EnPassant;
	  }
       }
       break;
    case King:
       if (Util::Abs((int)start - (int)dest) == 2)
       {
          if (start > dest)
	         move_type = QCastle;
          else
	         move_type = KCastle;
       }
       break;
    default:
       break;
    }
    return move_type;
}

Move CreateMove(const Board &board, Square start, Square dest, PieceType promotion )
{
   if (IsInvalid(start))
   {
        return NullMove;
   }
   MoveType type = get_move_type(board,start,dest);
   PieceType piece_moved = TypeOfPiece(board[start]);
   ASSERT(piece_moved != Empty);
   PieceType capture;
   if (type == EnPassant)
     capture = Pawn;
   else
     capture = TypeOfPiece(board[dest]);

   MoveUnion mu(start,dest,piece_moved,promotion,capture,type);
   return (Move)mu;
}	

