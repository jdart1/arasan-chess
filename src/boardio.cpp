// Copyright 1997, 2008 by Jon Dart.  All Rights Reserved.

#include "boardio.h"
#include "attacks.h"
#include "bhash.h"

int BoardIO::readFEN(Board &board, const string &buf)
{
   board.reset();
   for (int i = 0; i < 64; i++)
   {
      board.contents[i] = EmptyPiece;
   }

   char c;
   string::const_iterator bp = buf.begin();
   size_t first = buf.find_first_not_of(" \t");
   if (first != string::npos) {
      bp += first;
   }
   for (int line = 0; line < 8; line++)
   {
      int sqval = 56-line*8;
      while ((c = *bp) != ' ' && c != '/')
      {
         Piece piece;
         if (isdigit(c))
         {
            sqval += (*bp - '0');
            bp++;
         }
         else if (isalnum(c))
         {
            if (!OnBoard(sqval))
            {
	      return 0;
            }
            switch (c)
            {
            case 'p': piece = BlackPawn;
               break;
            case 'n': piece = BlackKnight;
               break;
            case 'b': piece = BlackBishop;
               break;
            case 'r': piece = BlackRook;
               break;
            case 'q': piece = BlackQueen;
               break;
            case 'k': piece = BlackKing;
               break;
            case 'P': piece = WhitePawn;
               break;
            case 'N': piece = WhiteKnight;
               break;
            case 'B': piece = WhiteBishop;
               break;
            case 'R': piece = WhiteRook;
               break;
            case 'Q': piece = WhiteQueen;
               break;
            case 'K': piece = WhiteKing;
               break;
            default:
	      return 0;
            }
            board.contents[sqval] = piece;
            sqval++;
            bp++;
         }
         else // not a letter or a digit
         {
	   return 0;
         }
      }
      if (c=='/') ++bp;  // skip delimiter
   }
   while (bp != buf.end() && isspace(*bp)) bp++;
   if (bp == buf.end()) return 0;
   if (toupper(*bp) == 'W')
      board.side = White;
   else if (toupper(*bp) == 'B')
      board.side = Black;
   else
   {
     return 0;
   }
   bp++;
   while (bp != buf.end() && isspace(*bp)) bp++;
   if (bp == buf.end()) return 0;
   c = *bp;
   if (c == '-')
   {
      board.state.castleStatus[White] = 
      board.state.castleStatus[Black] = CantCastleEitherSide;
      bp++;
   }
   else
   {
      int k = 0;
      for (; bp != buf.end() && !isspace(*bp); bp++)
      {
         if (*bp == 'K')
            k += 1;
         else if (*bp == 'Q')
            k += 2;
         else if (*bp == 'k')
            k += 4;
         else if (*bp == 'q')
            k += 8;
         else
         {
	   return 0;
         }
      }
      static const CastleType vals[4] = 
      { CantCastleEitherSide, CanCastleKSide,
         CanCastleQSide, CanCastleEitherSide};
      board.state.castleStatus[White] = vals[k % 4];
      board.state.castleStatus[Black] = vals[k / 4];
   }
   board.setSecondaryVars();
   while (bp != buf.end() && isspace(*bp)) bp++;
   if (bp == buf.end()) return 0;
   c = *bp;
   if (c == '-')
   {
     board.state.enPassantSq = InvalidSquare;
   }
   else if (isalpha(c))
   {
      char sqbuf[2];
      sqbuf[0] = *bp++;
      if (bp == buf.end()) return 0;
      sqbuf[1] = *bp++;
      Square epsq = SquareValue(sqbuf);
      if (epsq == InvalidSquare)
      {
         return 0;
      }
      board.state.enPassantSq = InvalidSquare;
      Square ep_candidate =  
        SquareValue(sqbuf) - (8 * Direction[board.sideToMove()]);
      // only actually set the e.p. square on the board if an en-passant capture
      // is truly possible:
      if (TEST_MASK(Attacks::ep_mask[File(ep_candidate)-1][(int)board.oppositeSide()],
		  board.pawn_bits[board.sideToMove()])) {
         board.state.enPassantSq = ep_candidate;
	 // re-calc hash code since ep has changed
	 board.state.hashCode = BoardHash::hashCode(board);
      }
   }
   else
   {
     return 0;
   }
   ASSERT(board.state.moveCount+1<Board::RepListSize);
   board.repList[board.state.moveCount++] = board.hashCode();

   if (board.kingPos[White] == InvalidSquare ||
       board.kingPos[Black] == InvalidSquare) {
     return 0;
   }
   
   return 1;
}

void BoardIO::writeFEN( const Board &board, ostream &o, int addMoveInfo)
{
   // write out the board in Forsythe-Edwards (FEN) notation.
   for (int i=1;i<=8;i++)
   {
      int j = 1;
      while (j <= 8)
      {
         int n = 0;
         Square sq;
         Piece p;
         do
         {
            sq = MakeSquare(j,i,Black);
            p = board.contents[sq];
            if (p != EmptyPiece)
               break;
            ++j; ++n;
         } while (j <= 8);
         if (n)
            o << (char)(n + '0');
         if (p != EmptyPiece)
         {
            char img = PieceImage(TypeOfPiece(p));
            if (PieceColor(p) == Black) img = tolower(img);
            o << img;
            j++;
         }
      }
      if (i != 8) o << '/';
   }
   if (board.sideToMove() == White)
      o << " w";
   else
      o << " b";

   // used in I/O of castling data:
   const bool kcastle[6] = { true, true, false, false, false, false};
   const bool qcastle[6] = { true, false, true, false, false, false};

   // note : unfortunately FEN doesn't allow recording if castling
   // has taken place, only whether or not it is possible.
   CastleType wcs = board.castleStatus(White);
   CastleType bcs = board.castleStatus(Black);
   o << ' ';
   if (!kcastle[(int)wcs] && !qcastle[(int)bcs])
      o << '-';
   else
   {
      if (kcastle[(int)wcs])
         o << 'K';
      if (qcastle[(int)wcs])
         o << 'Q';
      if (kcastle[(int)bcs])
         o << 'k';
      if (qcastle[(int)bcs])
         o << 'q';
   }
   o << ' ';
   Square epsq = board.enPassantSq();
   if (epsq == InvalidSquare) {
      o << '-';
   }
   else {
      // FEN stores the destination square for an en passant capture;
      // we store the location of the capturable pawn.
      Square target = (board.sideToMove() == White) ? epsq + 8 : epsq - 8;
      o << FileImage(target) << RankImage(target);
   }
   if (addMoveInfo)
   {
      // FEN is supposed to include the halfmove and fullmove numbers,
      // but these are attributes of the game - they are not stored in
      // the board.
      o << " 0 1";
   }
}
