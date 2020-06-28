// Copyright 1994, 1995, 2008, 2009, 2013, 2017-8, 2020 by Jon Dart.
// All Rights Reserved.

#include "notation.h"
#include "board.h"
#include "movegen.h"
#include "debug.h"
#include <algorithm>

static void UCIMoveImage(const Move &move, ostream &image) {
    if (IsNull(move)) {
        image << "NULL";
    } else {
        image << FileImage(StartSquare(move));
        image << RankImage(StartSquare(move));
        image << FileImage(DestSquare(move));
        image << RankImage(DestSquare(move));
        if (TypeOfMove(move) == Promotion) {
            ASSERT(PromoteTo(move)<16);
            image << (char)tolower(PieceImage(PromoteTo(move)));
        }
    }
}

void Notation::image(const Board &b, const Move &m, OutputFormat format, string &result ) {
    stringstream buf;
    image(b, m, format, buf);
    result = buf.str();
}

void Notation::image(const Board & b, const Move & m, OutputFormat format, ostream &image) {
   if (format == OutputFormat::UCI) {
      return UCIMoveImage(m,image);
   }
   else if (format == OutputFormat::WB) {
      if (TypeOfMove(m) == KCastle) {
          image << "O-O";
      }
      else if (TypeOfMove(m) == QCastle) {
          image << "O-O-O";
      }
      else {
         image << FileImage(StartSquare(m));
         image << RankImage(StartSquare(m));
         image << FileImage(DestSquare(m));
         image << RankImage(DestSquare(m));
         if (TypeOfMove(m) == Promotion) {
            // N.b. ICS requires lower case.
            image << (char)tolower((int)PieceImage(PromoteTo(m)));
         }
      }
      return;
   }
   // format is SAN
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
   board_copy.doMove(m);
   if (board_copy.checkStatus() == InCheck) {
      Move moves[Constants::MaxMoves];
      mg::EvasionInfo info(board_copy);
      if (mg::generateEvasions(board_copy, info, moves))
         image << '+';
      else
         image << '#';                        // mate
   }
}

static int is_file(char c) {
    return c >= 'a' && c <= 'h';
}


Move Notation::value(const Board & board, ColorType side, InputFormat format, const string &image, bool checkLegal) 
{
    if (format == InputFormat::UCI) {
        if (image.length() >= 4) {
            Square start = SquareValue(image.substr(0,2));
            Square dest = SquareValue(image.substr(2,2));
            PieceType promotion = Empty;
            if (image.length() > 4) {
                switch (image[4]) {
                case 'q': promotion = Queen; break;
                case 'n': promotion = Knight; break;
                case 'b': promotion = Bishop; break;
                case 'r': promotion = Rook; break;
                default: promotion = Empty; break;
                }
            }
            if (!OnBoard(start) || !OnBoard(dest))
                return NullMove;
            else
                return CreateMove(board,start,dest,promotion);
        }
        else {
            return NullMove;
        }
    }

    int rank = 0;
    int file = 0;

    PieceType piece = Empty;
    PieceType promotion = Empty;
    Square dest = InvalidSquare, start = InvalidSquare;
    int capture = 0;

    stringstream s(image);
    string::const_iterator it = image.begin();
    int i = 0;
    while (it != image.end() && isspace(*it)) {
        it++;
        i++;
    }
    if (it == image.end() || !isalpha(*it)) return NullMove;
    string img(image,i); // string w/o leading spaces
    ASSERT(img.length());
    it = img.begin();
    if (*it == 'O' || *it == '0') {
       // castling, we presume
       return parseCastling(side, img);
    } else if (format == InputFormat::WB) {
       if (img.length() < 4) return NullMove;
       Square start = SquareValue(img.substr(0,2));
       if (!OnBoard(start)) return NullMove;
       Square dest = SquareValue(img.substr(2,2));
       if (!OnBoard(dest)) return NullMove;
       PieceType promotion = Empty;
       if (img.length() > 4) {
          promotion = PieceCharValue(toupper(img[4]));
       }
       return CreateMove(board,start,dest,promotion);
    }
    int have_start = 0;
    if (isupper(*it)) {
       piece = PieceCharValue(*it);
       it++;
    }
    else {
       piece = Pawn;
       if ((it+1) != img.end()) {
          char next = *it;
          file = next-'a'+1;
          if (file < 1 || file > 8) return NullMove;
          char next2 = *(it+1);
          if (next2 == 'x' || is_file(next2)) {
             // allow "dc4" as in Informant, instead of dxc4
             it++;
             capture = 1;
          }
          else if (isdigit(next2) && img.length()>2) {
             char next3 = *(it+2);
             if ((next3 == 'x' || next3 == '-') && img.length()>=5) {
                // long algebraic notation
                have_start++;
                start = SquareValue(next,next2);
                if (start == InvalidSquare) return NullMove;
                it+=3; // point to dest
                piece = TypeOfPiece(board[start]);
             }
          }
       }
    }
    if (piece == Empty) {
       return NullMove;
    }
    if (piece != Pawn && !have_start && it != img.end()) {
       char next = *it;
       char next2 = '\0';
       if (it + 1 != img.end()) next2 = *(it+1);
       if (is_file(next) && isdigit(next2) && img.length()>=5) {
          // long algebraic notation, or a SAN move like Qd1d3
          start = SquareValue(next,next2);
          if (IsInvalid(start)) return NullMove;
          it+=2;
          have_start++;
       }
       // also look for disambiguating rank, e.g. '2' in "N2e4".
       else if (isdigit(next)) {
          rank = next - '0';
          if (rank < 1 || rank > 8) return NullMove;
          it++;
       }
       else if (is_file(next) && isalpha(next2)) {
          // disamiguating rank, e.g. "Rfd1"
          file = next - 'a' + 1;
          if (file < 1 || file > 8) return NullMove;
          it++;
       }
    }

    if (it != img.end() && *it == 'x') {
       capture = 1;
       it++;
    }
    if (it != img.end() && (it+1) != img.end()) {
       // remainder of move should be a square identifier, e.g. "g7"
       dest = SquareValue(*it,*(it+1));
       it += 2;
    }
    if (IsInvalid(dest)) {
       return NullMove;
    }
    if (it != img.end() && *it == '=') {
       it++;
       if (it == img.end()) {
          return NullMove;
       } else {
          promotion = PieceCharValue(*it);
          if (piece != Pawn || promotion == Empty)
             return NullMove;
          it++;
       }
    }
    else if (piece == Pawn && it != img.end() && isupper(*it)) {
       // Quite a few "PGN" files have a8Q instead of a8=Q.
       promotion = PieceCharValue(*it);
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
    if (capture && !IsEmptyPiece(board[dest]) && PieceColor(board[dest]) == board.sideToMove()) {
       return NullMove;
    }

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
                   if (!checkLegal || !board_copy.anyAttacks(
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

Move Notation::parseCastling(ColorType color, const string &moveStr) {
   // repair brain-dead variants of castling like "O-O-0"
   string castle(moveStr);
   replace(castle.begin(), castle.end(), '0', 'O');
   size_t check = castle.find('+');
   if (check != string::npos) castle.erase(check,1);
   size_t mate = castle.find('#');
   if (mate != string::npos) castle.erase(mate,1);
   transform(castle.begin(), castle.end(), castle.begin(), ::toupper);
   if (castle == "O-O") {
      if (color == White)
         return CreateMove(chess::E1,chess::G1,King,Empty,Empty,KCastle);
      else
         return CreateMove(chess::E8,chess::G8,King,Empty,Empty,KCastle);
   }
   else if (castle == "O-O-O") {
      if (color == White)
         return CreateMove(chess::E1,chess::C1,King,Empty,Empty,QCastle);
      else
         return CreateMove(chess::E8,chess::C8,King,Empty,Empty,QCastle);
   } else {
      return NullMove;
   }
}
