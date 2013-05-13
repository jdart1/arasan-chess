// Copyright 1994, 1995, 2008, 2009, 2012 by Jon Dart.  All Rights Reserved.

#include "notation.h"
#include "board.h"
#include "movegen.h"
#include "debug.h"
#include "util.h"
#include <algorithm>

Move Notation::moveValue( const Board &board, const string &str, ColorType color )
{
   Square source, dest;

   if (str == "O-O") {
      if (color == White)
         return CreateMove(E1,G1,King,Empty,Empty,KCastle);
      else
         return CreateMove(E8,G8,King,Empty,Empty,KCastle);
   }
   else if (str == "O-O-O") {
      if (color == White)
         return CreateMove(E1,C1,King,Empty,Empty,QCastle);
      else
         return CreateMove(E8,C8,King,Empty,Empty,QCastle);
   }

   string::const_iterator it = str.begin();
   while (isspace(*it)) it++;
   string sourceStr;
   sourceStr += *it++;
   sourceStr += *it++;
   source = SquareValue(sourceStr);
   if (*it == '-' || *it == 'x') it++;
   string destStr;
   destStr += *it++;
   destStr += *it++;
   dest = SquareValue(destStr);
   if ((source == InvalidSquare) || (dest == InvalidSquare))
      return NullMove;
   else {
      it++;
      PieceType promotion = Empty;
      if (*it == '=') {
         // promotion
         it++;
         promotion = PieceCharValue(*it);
         // check for promotion to valid piece:
         switch (promotion) {
            case Empty:
            case Pawn:
               return NullMove;
            case Knight:
            case Bishop:
            case Rook:
            case Queen:
               break;
            case King:
               return NullMove;
         }
      }
      return CreateMove(board,source,dest,promotion);
   }
}

void Notation::UCIMoveImage(const Move &move, ostream &image) {
    if (IsNull(move)) {
        image << "NULL";
    } else {
        image << FileImage(StartSquare(move));
        image << RankImage(StartSquare(move));
        image << FileImage(DestSquare(move));
        image << RankImage(DestSquare(move));
        if (TypeOfMove(move) == Promotion) {
            ASSERT(PromoteTo(move)<16);
            image << (char)tolower(PieceImage(TypeOfPiece(PromoteTo(move))));
        }
    }
}

void Notation::image(const Board &b, const Move &m, string &result ) {
    stringstream buf;
    image(b, m, buf);
    result = buf.str();
}

void Notation::image(const Board & b, const Move & m, ostream &image) {
   if (IsNull(m)) {
       image << "(null)";
      return;
   }

   PieceType p = PieceMoved(m);
   ASSERT(p != Empty);
   if (TypeOfMove(m) == KCastle) {
       image << "O-O";
   }
   else if (TypeOfMove(m) == QCastle) {
       image << "O-O-O";
   }
   else {
      if (p == Pawn) {
         if (Capture(m) == Empty) {
            image << FileImage(DestSquare(m));
            image << RankImage(DestSquare(m));
         }
         else {
            image << FileImage(StartSquare(m));
            image << 'x';
            image << FileImage(DestSquare(m));
            image << RankImage(DestSquare(m));
         }
         if (TypeOfMove(m) == Promotion) {
            image << '=';
            image << PieceImage(PromoteTo(m));
         }
      }
      else {
         image << PieceImage(p);
         Bitboard attacks =
            b.calcAttacks(DestSquare(m), b.sideToMove());
         unsigned n = attacks.bitCount();
         int dups = 0;
         int filedups = 0;
         int rankdups = 0;
         int files[9];
         int ranks[9];

         if (n > 1) {
            Square sq;
            while (attacks.iterate(sq)) {
               if (TypeOfPiece(b[sq]) == p) {
                  files[dups] = File(sq);
                  if (files[dups] == File(StartSquare(m)))
                     filedups++;
                  ranks[dups] = Rank(sq,White);
                  if (ranks[dups] == Rank(StartSquare(m),White))
                     rankdups++;
                  ++dups;
               }
            }
         }
         if (dups > 1) {
            // need to disambiguate move.
            if (filedups == 1) {
               image << FileImage(StartSquare(m));
            }
            else if (rankdups == 1) {
               image << RankImage(StartSquare(m));
            }
            else {
               // need both rank and file to disambiguate
               image << FileImage(StartSquare(m));
               image << RankImage(StartSquare(m));
            }
         }
         if (Capture(m) != Empty) {
            image << 'x';
         }
         image << FileImage(DestSquare(m));
         image << RankImage(DestSquare(m));
      }
   }
   Board board_copy(b);
   //ASSERT(legalMove(board_copy,m));
   board_copy.doMove(m);
   if (board_copy.checkStatus() == InCheck) {
      Move moves[Constants::MaxMoves];
      MoveGenerator mg(board_copy);
      if (mg.generateEvasions(moves))
         image << '+';
      else
         image << '#';                        // mate
   }
}


Move Notation::value(const Board & board, ColorType side, const char *image) 
{
   const char *p;
   int rank = 0;
   int file = 0;

   PieceType piece = Empty;
   PieceType promotion = Empty;
   Square dest, start = InvalidSquare;
   int capture = 0;

   for (p = image; isspace(*p); ++p);
   if (!isalpha(*p) && *p != '0')
      return NullMove;
   if (toupper(*p) == 'O' || *p == '0') {
      // castling, we presume
      string castle(p);
      // repair brain-dead variants of castling like "O-O-0"
      replace(castle.begin(), castle.end(), '0', 'O');
      size_t check = castle.find('+');
      if (check != string::npos) castle.erase(check,1);
      size_t mate = castle.find('#');
      if (mate != string::npos) castle.erase(mate,1);
      transform(castle.begin(), castle.end(), castle.begin(), ::toupper);
      return moveValue(board, castle.c_str(), side);
   }
   int have_start = 0;
   if (isupper(*p)) {
      piece = PieceCharValue(*p);
      ++p;
   }
   else {
      piece = Pawn;
      file = *p - 'a' + 1;
      // allow "dc4" as in Informant, instead of dxc4
      if (*(p+1) == 'x' || isalpha(*(p+1))) {
         capture = 1;
         ++p;
      }
      else if (isdigit(*(p+1))) {
         const char *q = p+2;
         if (*q == 'x' || *q == '-') {
            // long algebraic notation
            have_start++;
            start = SquareValue(p);
            p = q;
         }
      }
   }
   if (piece == Empty) {
      return NullMove;
   }
   if (piece != Pawn) {
     if (isalpha(*p) && isdigit(*(p+1)) && 
         (isalpha(p[2]) || p[2] == '-')) {
         // long algebraic notation, or a SAN move like Qd1d3
         start = SquareValue(p);
         p+=2;
         have_start++;
      }
      // also look for disambiguating file, e.g. '2' in "N2e4".
      else if (isdigit(*p)) {
         rank = *p - '0';
         ++p;
	 if (isdigit(*p)) {
	   file = *p - 'a' + 1;
	   ++p;
	 }
      }
      else if (*p != 'x' && isalpha(*p) && isalpha(*(p + 1))) {
         file = *p - 'a' + 1;
         ++p;
      }
   }

   if (*p == 'x') {
      capture = 1;
      ++p;
   }
   else if (*p == '-') { // allow long algebraic notation
      ++p;
   }
   // remainder of move should be a square identifier, e.g. "g7"
   dest = SquareValue(p);
   if (IsInvalid(dest)) {
      return NullMove;
   }
   p += 2;
   if (*p == '=') {
      promotion = PieceCharValue(*(p + 1));
      if (piece != Pawn || promotion == Empty)
         return NullMove;
   }
   else if (piece == Pawn && isupper(*p)) {
      // Quite a few "PGN" files have a8Q instead of a8=Q.
      promotion = PieceCharValue(*p);
      if (promotion == Empty || Rank(dest,side) != 8)
         return NullMove;
   } else if (piece == Pawn && Rank(dest,side) == 8) {
       // promotion but no piece specified, treat as error
       return NullMove;
   }

   // Informant does not use "x" for captures.  Assume that if the destination
   // is occupied, this is a capture move.
   if (board[dest] != EmptyPiece) {
      capture = 1;
   }
   // Do a sanity check on capture moves:
   if (capture && !IsEmptyPiece(board[dest]) && PieceColor(board[dest]) == board.sideToMove())
      return NullMove;

   // Ok, now we need to figure out where the start square is. For pawn
   // moves this is implicit.

   int dups = 0;

   if (!have_start) {
      if (capture && piece == Pawn && IsEmptyPiece(board[dest]) &&
      Rank(dest,board.sideToMove()) != 8) {
         // en passant capture, special case
         int start_rank = (board.sideToMove() == White) ?
            Rank(dest,White) - 1 :
         Rank(dest,White) + 1;

         start = MakeSquare(file, start_rank, White);
         dups = 1;
      }
      else if (piece == Pawn && board[dest] == EmptyPiece) {
         start = MakeSquare(file,Rank(dest,board.sideToMove())-1,board.sideToMove());
         if (board[start] == EmptyPiece && Rank(dest,board.sideToMove())==4) {
            start = MakeSquare(file,Rank(dest,board.sideToMove())-2,board.sideToMove());
         }
         if (board[start] == EmptyPiece) return NullMove;
         dups = 1;
      }
      else {
         Bitboard attacks = board.calcAttacks(dest,side);
         Square maybe;
         while (attacks.iterate(maybe)) {
            if (TypeOfPiece(board[maybe]) == piece &&
            PieceColor(board[maybe]) == board.sideToMove()) {
               if (file && File(maybe) != file)
                  continue;
               if (rank && Rank(maybe,White) != rank)
                  continue;
               if (PieceColor(board[maybe]) == board.sideToMove()) {
                  // Possible move to this square.  Make sure it is legal.
                  Board board_copy(board);
                  Move emove = CreateMove(board,maybe,dest,
                     promotion);
                  board_copy.doMove(emove);
                  if (!board_copy.anyAttacks(
                     board_copy.kingSquare(board_copy.oppositeSide()),
                  board_copy.sideToMove())) {
                     ++dups;
                     start = maybe;
                  }
               }
            }
         }
      }
   }
   if (dups == 1 || have_start) {
      if (start == InvalidSquare || board[start] == EmptyPiece)
         return NullMove;
      else
         return CreateMove(board, start, dest, promotion);
   }
   else                                           // ambiguous move
      return NullMove;
}
