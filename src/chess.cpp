// Copyright 1994-2008, 2013-2014, 2017-2018. 2020 by Jon Dart. All Rights Reserved.

#include "chess.h"
#include "debug.h"
#include "board.h"
#include "bitboard.h"
#include <cstddef>
#include <cstring>
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
   chess::A8, chess::B8, chess::C8, chess::D8, chess::E8, chess::F8, chess::G8, chess::H8,
   chess::A7, chess::B7, chess::C7, chess::D7, chess::E7, chess::F7, chess::G7, chess::H7,
   chess::A6, chess::B6, chess::C6, chess::D6, chess::E6, chess::F6, chess::G6, chess::H6,
   chess::A5, chess::B5, chess::C5, chess::D5, chess::E5, chess::F5, chess::G5, chess::H5,
   chess::A4, chess::B4, chess::C4, chess::D4, chess::E4, chess::F4, chess::G4, chess::H4,
   chess::A3, chess::B3, chess::C3, chess::D3, chess::E3, chess::F3, chess::G3, chess::H3,
   chess::A2, chess::B2, chess::C2, chess::D2, chess::E2, chess::F2, chess::G2, chess::H2,
   chess::A1, chess::B1, chess::C1, chess::D1, chess::E1, chess::F1, chess::G1, chess::H1
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

Square SquareValue(char file, char rank) {
   if (file<'a' || file >'h') return InvalidSquare;
   if (rank<'1' || rank >'8') return InvalidSquare;
   return MakeSquare(file-'a'+1,rank-'1'+1,White);
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

bool validPiece(Piece p)
{
    static constexpr int valid[16] = {1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0};
    return ((int)p)<16 && valid[(int)p];
}

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
       if (std::abs((int)start - (int)dest) == 2)
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
   if (!OnBoard(start) || !OnBoard(dest))
   {
      return NullMove;
   }
   MoveType type = get_move_type(board,start,dest);
   PieceType piece_moved = TypeOfPiece(board[start]);
   if (piece_moved == Empty) {
      return NullMove;
   } else {
      PieceType capture;
      if (type == EnPassant)
         capture = Pawn;
      else
         capture = TypeOfPiece(board[dest]);
      MoveUnion mu(start,dest,piece_moved,promotion,capture,type);
      return (Move)mu;
   }
}

