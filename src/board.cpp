// Copyright 1994-2012, 2015, 2017-2021 by Jon Dart.  All Rights Reserved.

#include "constant.h"
#include "chess.h"
#include "board.h"
#include "attacks.h"
#include "debug.h"
#include "boardio.h"
#include "bhash.h"
#include "movegen.h"
#include <memory.h>
#include <algorithm>
#include <assert.h>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <unordered_set>

using namespace std;

const Bitboard Board::black_squares(0xaa55aa55aa55aa55ULL);

const Bitboard Board::white_squares(0x55aa55aa55aa55aaULL);

const hash_t rep_codes[3] =
{
    0x194ca2c45c8e7baaULL,
    0x804e48e8e8f5544fULL,
    0xd4767986f0ab49a7ULL
};

static Board *initialBoard = nullptr;

void Board::setupInitialBoard() {
   initialBoard = (Board*)malloc(sizeof(Board));
   static PieceType pieces[] =
   {
      Rook,
      Knight,
      Bishop,
      Queen,
      King,
      Bishop,
      Knight,
      Rook
   };

   initialBoard->side = White;
   initialBoard->state.checkStatus = CheckUnknown;
   for (int i=0;i<64;i++)
   {
      const Square sq(i);
      if (Rank<White>(sq) == 1)
         initialBoard->contents[sq] = MakeWhitePiece( pieces[File(sq)-1]);
      else if (Rank<Black>(sq) == 1)
         initialBoard->contents[sq] = MakeBlackPiece( pieces[File(sq)-1]);
      else if (Rank<White>(sq) == 2)
         initialBoard->contents[sq] = WhitePawn;
      else if (Rank<Black>(sq) == 2)
         initialBoard->contents[sq] = BlackPawn;
      else
         initialBoard->contents[sq] = EmptyPiece;
   }
   initialBoard->state.enPassantSq = InvalidSquare;
   initialBoard->state.castleStatus[White] = initialBoard->state.castleStatus[Black] = CanCastleEitherSide;
   initialBoard->state.moveCount = 0;
   initialBoard->state.movesFromNull = 0;
   initialBoard->repListHead = initialBoard->repList;
   initialBoard->setSecondaryVars();
   *(initialBoard->repListHead)++ = initialBoard->hashCode();
}

void Board::init() {
   if (!initialBoard) {
       setupInitialBoard();
   }
}

void Board::cleanup() {
   free(reinterpret_cast<void*>(initialBoard));
}

void Board::setSecondaryVars()
{
   int i;

   material[White].clear();
   material[Black].clear();
   pawn_bits[White].clear();
   pawn_bits[Black].clear();
   knight_bits[White].clear();
   knight_bits[Black].clear();
   bishop_bits[White].clear();
   bishop_bits[Black].clear();
   rook_bits[White].clear();
   rook_bits[Black].clear();
   queen_bits[White].clear();
   queen_bits[Black].clear();
   occupied[White].clear();
   occupied[Black].clear();
   allOccupied.clear();
   kingPos[White] = InvalidSquare;
   kingPos[Black] = InvalidSquare;
   for (i=0;i<64;i++)
   {
      Square sq(i);
      if (contents[sq] != EmptyPiece)
      {
         const Piece piece = contents[sq];
         ColorType color = PieceColor(piece);
         occupied[color].set(sq);
         allOccupied.set(sq);
         material[color].addPiece(TypeOfPiece(piece));
         switch (TypeOfPiece(piece))
         {
         case King:
            kingPos[color] = sq;
            break;
         case Pawn:
            pawn_bits[color].set(sq);
            break;
         case Knight:
            knight_bits[color].set(sq);
            break;
         case Bishop:
            bishop_bits[color].set(sq);
            break;
         case Rook:
            rook_bits[color].set(sq);
            break;
         case Queen:
            queen_bits[color].set(sq);
            break;
         default:
            break;
         }
      }
   }
   state.hashCode = BoardHash::hashCode(*this);
   pawnHashCodeW = BoardHash::pawnHash(*this,White);
   pawnHashCodeB = BoardHash::pawnHash(*this,Black);
}

void Board::setCastleStatus( CastleType t, ColorType side )
{
   CastleType old = castleStatus(side);
   state.castleStatus[side] = t;
   if (side == White) {
      state.hashCode ^= w_castle_status[old];
      state.hashCode ^= w_castle_status[t];
   }
   else {
      state.hashCode ^= b_castle_status[old];
      state.hashCode ^= b_castle_status[t];
   }
   state.hashCode = BoardHash::setSideToMove(state.hashCode, sideToMove());
}

void Board::reset()
{
   assert(initialBoard);
   *this = *initialBoard;
}

void Board::makeEmpty() {
   for (Square sq = 0; sq < 64; sq++) contents[sq] = EmptyPiece;
   state.castleStatus[White] = state.castleStatus[Black] = CantCastleEitherSide;
}

Board::Board()
{
   reset();
}

Board::Board(const Board &b)
{
   // Copy all contents except the repetition list
   memcpy(&contents,&b.contents,(uint8_t*)repList-(uint8_t*)&contents);
   // Copy the repetition table
   int rep_entries = (int)(b.repListHead - b.repList);
   ASSERT(rep_entries>=0 && rep_entries<RepListSize);
   if (rep_entries) {
     memcpy(repList,b.repList,sizeof(hash_t)*rep_entries);
   }
   repListHead = repList + rep_entries;
}

Board &Board::operator = (const Board &b)
{
   if (&b != this)
   {
      // Copy all contents except the repetition list
      memcpy(&contents,&b.contents,(uint8_t*)repList-(uint8_t*)&contents);
      // Copy the repetition table
      int rep_entries = (int)(b.repListHead - b.repList);
      if (rep_entries) {
          memcpy(repList,b.repList,sizeof(hash_t)*rep_entries);
      }
      repListHead = repList + rep_entries;
   }
   return *this;
}

Board::~Board()
{
}

#ifdef _DEBUG
const Piece &Board::operator[]( const Square sq ) const
{
   ASSERT(OnBoard(sq));
   return contents[sq];
}
#endif

static inline CastleType UpdateCastleStatusW( CastleType cs, Square sq )
// after a move of or capture of the rook on 'sq', update castle status
// for 'side'
{
   ASSERT(cs<3);
   if (sq == chess::A1) // Queen Rook moved or captured
   {
      if (cs == CanCastleEitherSide)
         return CanCastleKSide;
      else if (cs == CanCastleQSide)
         return CantCastleEitherSide;
   }
   else if (sq == chess::H1) // King Rook moved or captured
   {
      if (cs == CanCastleEitherSide)
         return CanCastleQSide;
      else if (cs == CanCastleKSide)
         return CantCastleEitherSide;
   }
   return cs;
}

static inline CastleType UpdateCastleStatusB(CastleType cs, Square sq)
// after a move of or capture of the rook on 'sq', update castle status
// for 'side'
{
   ASSERT(cs<3);
   if (sq == chess::A8) // Queen Rook moved or captured
   {
      if (cs == CanCastleEitherSide)
         return CanCastleKSide;
      else if (cs == CanCastleQSide)
         return CantCastleEitherSide;
   }
   else if (sq==chess::H8) // King Rook moved or captured
   {
      if (cs == CanCastleEitherSide)
         return CanCastleQSide;
      else if (cs == CanCastleKSide)
         return CantCastleEitherSide;
   }
   return cs;
}

static FORCEINLINE void Xor(hash_t &h,Square sq,Piece piece) {
   h ^= hash_codes[sq][(int)piece];
}

void Board::doNull()
{
   state.checkStatus = CheckUnknown;
   // We can't reset the halfmove counter because we might overwrite
   // existing entries in the repList then. But we keep separate
   // track of how far we are from a null move (idea from Stockfish).
   state.movesFromNull = 0;
   if (state.enPassantSq != InvalidSquare)
   {
       state.hashCode ^= ep_codes[state.enPassantSq];
       state.hashCode ^= ep_codes[0];
   }
   state.enPassantSq = InvalidSquare;
   side = oppositeSide();
   state.hashCode = BoardHash::setSideToMove(state.hashCode,side);
   *repListHead++ = state.hashCode;
   ASSERT(repListHead-repList < (int)RepListSize);
   ASSERT(state.hashCode == BoardHash::hashCode(*this));
}

void Board::doMove( Move move )
{
   ASSERT(!IsNull(move));
   ASSERT(state.hashCode == BoardHash::hashCode(*this));
   state.checkStatus = CheckUnknown;
   ++state.moveCount;
   ++state.movesFromNull;
   if (state.enPassantSq != InvalidSquare)
   {
       state.hashCode ^= ep_codes[state.enPassantSq];
       state.hashCode ^= ep_codes[0];
   }
   Square old_epsq = state.enPassantSq;
   state.enPassantSq = InvalidSquare;

   ASSERT(PieceMoved(move) != Empty);

   const Square start = StartSquare(move);
   const Square dest = DestSquare(move);
   const MoveType moveType = TypeOfMove(move);
   ASSERT(PieceMoved(move) == TypeOfPiece(contents[start]));
#ifdef _DEBUG
   if (Capture(move) != Empty) {
           if (TypeOfMove(move) == EnPassant) {
                   ASSERT(contents[old_epsq] == MakePiece(Pawn,OppositeColor(side)));
           } else {
                   ASSERT(contents[dest] == MakePiece(Capture(move),OppositeColor(side)));
           }
   }
#endif
   if (side == White)
   {
      if (moveType == KCastle)
      {
         state.moveCount = 0;

         // update the hash code
         const Square kp = kingSquare(White);
         Xor(state.hashCode, kp+3, WhiteRook);
         Xor(state.hashCode, kp, WhiteKing);
         Xor(state.hashCode, kp+1, WhiteRook);
         Xor(state.hashCode, kp+2, WhiteKing);
         state.hashCode ^= w_castle_status[(int)state.castleStatus[White]];
         state.hashCode ^= w_castle_status[(int)CantCastleEitherSide];

         const int newkp = kp + 2;
         kingPos[White] = newkp;
         state.castleStatus[White] = CantCastleEitherSide;
         // find old square of rook
         Square oldrooksq = kp + 3;
         Square newrooksq = kp + 1;
         contents[kp] = contents[oldrooksq] = EmptyPiece;
         contents[newrooksq] = WhiteRook;
         contents[newkp] = WhiteKing;
         rook_bits[White].clear(oldrooksq);
         rook_bits[White].set(newrooksq);
         clearAll(White,kp);
         clearAll(White,oldrooksq);
         setAll(White,newkp);
         setAll(White,newrooksq);
      }
      else if (moveType == QCastle)
      {
         state.moveCount = 0;

         // update the hash code
         const Square kp = kingSquare(White);
         Xor(state.hashCode, kp-4, WhiteRook);
         Xor(state.hashCode, kp, WhiteKing);
         Xor(state.hashCode, kp-1, WhiteRook);
         Xor(state.hashCode, kp-2, WhiteKing);
         state.hashCode ^= w_castle_status[(int)state.castleStatus[White]];
         state.hashCode ^= w_castle_status[(int)CantCastleEitherSide];

         const int newkp = kp - 2;
         kingPos[White] = newkp;
         state.castleStatus[White] = CantCastleEitherSide;
         // find old square of rook
         Square oldrooksq = kp - 4;
         Square newrooksq = kp - 1;
         contents[kp] = contents[oldrooksq] = Piece();
         contents[newrooksq] = WhiteRook;
         contents[kp-2] = WhiteKing;
         rook_bits[White].clear(oldrooksq);
         rook_bits[White].set(newrooksq);
         clearAll(White,kp);
         clearAll(White,oldrooksq);
         setAll(White,newkp);
         setAll(White,newrooksq);
      }
      else // not castling
      {
         ASSERT(contents[start] != EmptyPiece);
         const Bitboard bits(Bitboard::mask[start] |
                             Bitboard::mask[dest]);
         Square target = dest; // where we captured
         Piece capture = contents[dest]; // what we captured
         switch (TypeOfPiece(contents[StartSquare(move)])) {
         case Empty: break;
         case Pawn:
            state.moveCount = 0;
            switch (moveType)
            {
            case EnPassant:
               // update hash code
               Xor(state.hashCode, start, WhitePawn);
               Xor(state.hashCode, dest, WhitePawn);
               Xor(pawnHashCodeW, start, WhitePawn);
               Xor(pawnHashCodeW, dest, WhitePawn);
               ASSERT(dest - 8 == old_epsq);
               target = old_epsq;
               capture = BlackPawn;
               contents[dest] = WhitePawn;
               pawn_bits[White].set(dest);
               break;
            case Promotion:
               // update hash code
               Xor(state.hashCode, start, WhitePawn);
               Xor(state.hashCode, dest, MakeWhitePiece(PromoteTo(move)));
               Xor(pawnHashCodeW, start, WhitePawn);
               contents[dest] = MakeWhitePiece(PromoteTo(move));
               material[White].removePawn();
               material[White].addPiece(PromoteTo(move));
               switch (PromoteTo(move))
               {
               case Knight:
                  knight_bits[White].set(dest);
                  break;
               case Bishop:
                  bishop_bits[White].set(dest);
                  break;
               case Rook:
                  rook_bits[White].set(dest);
                  break;
               case Queen:
                  queen_bits[White].set(dest);
                  break;
               default:
                  break;
               }
               break;
            default:
               Xor(state.hashCode, start, WhitePawn );
               Xor(state.hashCode, dest, WhitePawn );
               Xor(pawnHashCodeW, start, WhitePawn);
               Xor(pawnHashCodeW, dest, WhitePawn);
               contents[dest] = WhitePawn;
               if (dest - start == 16) // 2-square pawn advance
               {
                  if (TEST_MASK(Attacks::ep_mask[File(dest)-1][(int)White],pawn_bits[Black])) {
                    state.enPassantSq = dest;
                    state.hashCode ^= ep_codes[0];
                    state.hashCode ^= ep_codes[dest];
                  }
               }
               pawn_bits[White].set(dest);
               break;
            }
            pawn_bits[White].clear(start);
            break;
         case Knight:
            Xor(state.hashCode, start, WhiteKnight);
            Xor(state.hashCode, dest, WhiteKnight);
            contents[dest] = WhiteKnight;
            knight_bits[White].setClear(bits);
            break;
         case Bishop:
            Xor(state.hashCode, start, WhiteBishop);
            Xor(state.hashCode, dest, WhiteBishop);
            contents[dest] = WhiteBishop;
            bishop_bits[White].setClear(bits);
            break;
         case Rook:
            Xor(state.hashCode, start, WhiteRook );
            Xor(state.hashCode, dest, WhiteRook );
            contents[dest] = WhiteRook;
            rook_bits[White].setClear(bits);
            if ((int)state.castleStatus[White]<3) {
               state.hashCode ^= w_castle_status[(int)state.castleStatus[White]];
               state.castleStatus[White] = UpdateCastleStatusW(state.castleStatus[White],start);
               state.hashCode ^= w_castle_status[(int)state.castleStatus[White]];
            }
            break;
         case Queen:
            Xor(state.hashCode, start, WhiteQueen);
            Xor(state.hashCode, dest, WhiteQueen);
            contents[dest] = WhiteQueen;
            queen_bits[White].setClear(bits);
            break;
         case King:
            Xor(state.hashCode, start, WhiteKing );
            Xor(state.hashCode, dest, WhiteKing );
            contents[dest] = WhiteKing;
            kingPos[White] = dest;
            state.hashCode ^= w_castle_status[(int)castleStatus(White)];
            state.hashCode ^= w_castle_status[(int)CantCastleEitherSide];
            state.castleStatus[White] = CantCastleEitherSide;
            break;
         }
         contents[start] = EmptyPiece;
         if (capture != EmptyPiece)
         {
            state.moveCount = 0;
            ASSERT(target != InvalidSquare);
            occupied[Black].clear(target);
            Xor(state.hashCode, target, capture);
            switch (TypeOfPiece(capture))
            {
            case Empty: break;
            case Pawn:
               ASSERT(pawn_bits[Black].isSet(target));
               pawn_bits[Black].clear(target);
               Xor(pawnHashCodeB, target, capture);
               if (moveType == EnPassant)
               {
                  contents[target] = EmptyPiece;
                  clearAll(Black,target);
               }
               material[Black].removePawn();
               break;
            case Rook:
               rook_bits[Black].clear(target);
               material[Black].removePiece(Rook);
               if ((int)state.castleStatus[Black]<3) {
                  state.hashCode ^= b_castle_status[(int)state.castleStatus[Black]];
                  state.castleStatus[Black] = UpdateCastleStatusB(state.castleStatus[Black],dest);
                  state.hashCode ^= b_castle_status[(int)state.castleStatus[Black]];
               }
               break;
            case Knight:
               knight_bits[Black].clear(target);
               material[Black].removePiece(Knight);
               break;
            case Bishop:
               bishop_bits[Black].clear(target);
               material[Black].removePiece(Bishop);
               break;
            case Queen:
               queen_bits[Black].clear(target);
               material[Black].removePiece(Queen);
               break;
            case King:
               ASSERT(0);
               kingPos[Black] = InvalidSquare;
               state.castleStatus[Black] = CantCastleEitherSide;
               material[Black].removePiece(King);
               break;
            default:
               break;
            }
         }
      }
      setAll(White,dest);
      clearAll(White,start);

   }
   else // side == Black
   {
      if (moveType == KCastle)
      {
         state.moveCount = 0;
         const Square kp = kingSquare(Black);

         // update the hash code
         Xor(state.hashCode, kp+3, BlackRook);
         Xor(state.hashCode, kp, BlackKing);
         Xor(state.hashCode, kp+1, BlackRook);
         Xor(state.hashCode, kp+2, BlackKing);
         state.hashCode ^= b_castle_status[(int)state.castleStatus[Black]];
         state.hashCode ^= b_castle_status[(int)CantCastleEitherSide];

         const int newkp = kp + 2;
         kingPos[Black] = newkp;
         state.castleStatus[Black] = CantCastleEitherSide;
         // find old square of rook
         Square oldrooksq = kp + 3;
         Square newrooksq = kp + 1;
         contents[kp] = contents[oldrooksq] = EmptyPiece;
         contents[newrooksq] = BlackRook;
         contents[kp+2] = BlackKing;
         rook_bits[Black].clear(oldrooksq);
         rook_bits[Black].set(newrooksq);
         clearAll(Black,kp);
         clearAll(Black,oldrooksq);
         setAll(Black,newkp);
         setAll(Black,newrooksq);
      }
      else if (moveType == QCastle)
      {
         state.moveCount = 0;
         const Square kp = kingSquare(Black);

         // update the hash code
         Xor(state.hashCode, kp-4, BlackRook);
         Xor(state.hashCode, kp, BlackKing);
         Xor(state.hashCode, kp-1, BlackRook);
         Xor(state.hashCode, kp-2, BlackKing);
         state.hashCode ^= b_castle_status[(int)state.castleStatus[Black]];
         state.hashCode ^= b_castle_status[(int)CantCastleEitherSide];

         const int newkp = kp - 2;
         kingPos[Black] = newkp;
         state.castleStatus[Black] = CantCastleEitherSide;
         // find old square of rook
         Square oldrooksq = kp - 4;
         Square newrooksq = kp - 1;
         contents[kp] = contents[oldrooksq] = EmptyPiece;
         contents[newrooksq] = BlackRook;
         contents[kp-2] = BlackKing;
         rook_bits[Black].clear(oldrooksq);
         rook_bits[Black].set(newrooksq);
         clearAll(Black,kp);
         clearAll(Black,oldrooksq);
         setAll(Black,newkp);
         setAll(Black,newrooksq);
      }
      else // not castling
      {
         ASSERT(contents[start] != EmptyPiece);
         const Bitboard bits(Bitboard::mask[start] |
                           Bitboard::mask[dest]);
         Square target = dest; // where we captured
         Piece capture = contents[dest]; // what we captured
         switch (TypeOfPiece(contents[StartSquare(move)])) {
         case Empty: break;
         case Pawn:
            state.moveCount = 0;
            switch (moveType)
            {
            case EnPassant:
               // update hash code
               Xor(state.hashCode, start, BlackPawn);
               Xor(state.hashCode, dest, BlackPawn);
               Xor(pawnHashCodeB, start, BlackPawn);
               Xor(pawnHashCodeB, dest, BlackPawn);
               ASSERT(dest + 8 == old_epsq);
               target = old_epsq;
               capture = WhitePawn;
               contents[dest] = BlackPawn;
               pawn_bits[Black].set(dest);
               break;
            case Promotion:
               // update hash code
               Xor(state.hashCode, start, BlackPawn);
               Xor(state.hashCode, dest, MakeBlackPiece(PromoteTo(move)));
               Xor(pawnHashCodeB, start, BlackPawn);
               contents[dest] = MakeBlackPiece(PromoteTo(move));
               material[Black].removePawn();
               material[Black].addPiece(PromoteTo(move));
               switch (PromoteTo(move))
               {
               case Knight:
                  knight_bits[Black].set(dest);
                  break;
               case Bishop:
                  bishop_bits[Black].set(dest);
                  break;
               case Rook:
                  rook_bits[Black].set(dest);
                  break;
               case Queen:
                  queen_bits[Black].set(dest);
                  break;
               default:
                  break;
               }
               break;
            default:
               Xor(state.hashCode, start, BlackPawn );
               Xor(state.hashCode, dest, BlackPawn );
               Xor(pawnHashCodeB, start, BlackPawn);
               Xor(pawnHashCodeB, dest, BlackPawn);
               contents[dest] = BlackPawn;
               if (start - dest == 16) // 2-square pawn advance
               {
                  if (TEST_MASK(Attacks::ep_mask[File(dest)-1][(int)Black],pawn_bits[White])) {
                    state.enPassantSq = dest;
                    state.hashCode ^= ep_codes[0];
                    state.hashCode ^= ep_codes[dest];
                  }
               }
               pawn_bits[Black].set(dest);
               break;
            }
            pawn_bits[Black].clear(start);
            break;
         case Knight:
            Xor(state.hashCode, start, BlackKnight);
            Xor(state.hashCode, dest, BlackKnight);
            contents[dest] = BlackKnight;
            knight_bits[Black].setClear(bits);
            break;
         case Bishop:
            Xor(state.hashCode, start, BlackBishop);
            Xor(state.hashCode, dest, BlackBishop);
            contents[dest] = BlackBishop;
            bishop_bits[Black].setClear(bits);
            break;
         case Rook:
            Xor(state.hashCode, start, BlackRook );
            Xor(state.hashCode, dest, BlackRook );
            contents[dest] = BlackRook;
            rook_bits[Black].setClear(bits);
            if ((int)state.castleStatus[Black]<3) {
                state.hashCode ^= b_castle_status[(int)state.castleStatus[Black]];
                state.castleStatus[Black] = UpdateCastleStatusB(state.castleStatus[Black],start);
                state.hashCode ^= b_castle_status[(int)state.castleStatus[Black]];
            }
            break;
         case Queen:
            Xor(state.hashCode, start, BlackQueen);
            Xor(state.hashCode, dest, BlackQueen);
            contents[dest] = BlackQueen;
            queen_bits[Black].setClear(bits);
            break;
         case King:
            Xor(state.hashCode, start, BlackKing );
            Xor(state.hashCode, dest, BlackKing );
            contents[dest] = BlackKing;
            kingPos[Black] = dest;
            state.hashCode ^= b_castle_status[(int)castleStatus(Black)];
            state.hashCode ^= b_castle_status[(int)CantCastleEitherSide];
            state.castleStatus[Black] = CantCastleEitherSide;
            break;
         }
         contents[start] = EmptyPiece;
         if (capture != EmptyPiece)
         {
            state.moveCount = 0;
            ASSERT(OnBoard(target));
            occupied[White].clear(target);
            Xor(state.hashCode, target, capture);
            switch (TypeOfPiece(capture)) {
            case Empty: break;
            case Pawn:
               ASSERT(pawn_bits[White].isSet(target));
               pawn_bits[White].clear(target);
               Xor(pawnHashCodeW, target, capture);
               if (moveType == EnPassant)
               {
                  contents[target] = EmptyPiece;
                  clearAll(White,target);
               }
               material[White].removePawn();
               break;
            case Rook:
               rook_bits[White].clear(target);
               material[White].removePiece(Rook);
               if ((int)state.castleStatus[White]<3) {
                  state.hashCode ^= w_castle_status[(int)state.castleStatus[White]];
                  state.castleStatus[White] = UpdateCastleStatusW(state.castleStatus[White],dest);
                  state.hashCode ^= w_castle_status[(int)state.castleStatus[White]];
               }
               break;
            case Knight:
               knight_bits[White].clear(target);
               material[White].removePiece(Knight);
               break;
            case Bishop:
               bishop_bits[White].clear(target);
               material[White].removePiece(Bishop);
               break;
            case Queen:
               queen_bits[White].clear(target);
               material[White].removePiece(Queen);
               break;
            case King:
               ASSERT(0);
               kingPos[White] = InvalidSquare;
               state.castleStatus[White] = CantCastleEitherSide;
               material[White].removePiece(King);
               break;
            default:
               break;
            }
         }
         setAll(Black,dest);
         clearAll(Black,start);
      }
   }

   // changing side to move so flip those bits
   state.hashCode = BoardHash::setSideToMove(state.hashCode,oppositeSide());
   *repListHead++ = state.hashCode;
   //ASSERT(pawn_hash(White) == BoardHash::pawnHash(*this),White);
   ASSERT(getMaterial(sideToMove()).pawnCount() == (int)pawn_bits[side].bitCount());
   side = oppositeSide();
   ASSERT(getMaterial(sideToMove()).pawnCount() == (int)pawn_bits[side].bitCount());
   allOccupied = occupied[White] | occupied[Black];
   ASSERT(state.hashCode == BoardHash::hashCode(*this));
#if defined(_DEBUG) && defined(FULL_DEBUG)
   // verify correct updating of bitmaps:
   Board copy(*this);
   copy.setSecondaryVars();
   ASSERT(pawn_bits[White] == copy.pawn_bits[White]);
   ASSERT(knight_bits[White] == copy.knight_bits[White]);
   ASSERT(bishop_bits[White] == copy.bishop_bits[White]);
   ASSERT(rook_bits[White] == copy.rook_bits[White]);
   ASSERT(queen_bits[White] == copy.queen_bits[White]);
   ASSERT(occupied[White] == copy.occupied[White]);
   ASSERT(pawn_bits[Black] == copy.pawn_bits[Black]);
   ASSERT(knight_bits[Black] == copy.knight_bits[Black]);
   ASSERT(bishop_bits[Black] == copy.bishop_bits[Black]);
   ASSERT(rook_bits[Black] == copy.rook_bits[Black]);
   ASSERT(queen_bits[Black] == copy.queen_bits[Black]);
   ASSERT(occupied[Black] == copy.occupied[Black]);
   ASSERT(contents[kingPos[White]]==WhiteKing);
   ASSERT(contents[kingPos[Black]]==BlackKing);
   ASSERT(validPiece(contents[start]));
   ASSERT(validPiece(contents[dest]));
#endif
}

hash_t Board::hashCode( Move move ) const
{
   hash_t newHash = state.hashCode;
   if (state.enPassantSq != InvalidSquare)
   {
       newHash ^= ep_codes[state.enPassantSq];
       newHash ^= ep_codes[0];
   }
   const Square start = StartSquare(move);
   const Square dest = DestSquare(move);
   const MoveType moveType = TypeOfMove(move);
   if (side == White)
   {
      if (moveType == KCastle)
      {
         const Square kp = kingSquare(White);
         Xor(newHash, kp+3, WhiteRook);
         Xor(newHash, kp, WhiteKing);
         Xor(newHash, kp+1, WhiteRook);
         Xor(newHash, kp+2, WhiteKing);
         newHash ^= w_castle_status[(int)state.castleStatus[White]];
         newHash ^= w_castle_status[(int)CantCastleEitherSide];
      }
      else if (moveType == QCastle)
      {
         const Square kp = kingSquare(White);
         Xor(newHash, kp-4, WhiteRook);
         Xor(newHash, kp, WhiteKing);
         Xor(newHash, kp-1, WhiteRook);
         Xor(newHash, kp-2, WhiteKing);
         newHash ^= w_castle_status[(int)state.castleStatus[White]];
         newHash ^= w_castle_status[(int)CantCastleEitherSide];
      }
      else // not castling
      {
         Square target = dest; // where we captured
         switch (TypeOfPiece(contents[StartSquare(move)]))
         {
         case Empty: break;
         case Pawn:
            switch (moveType)
            {
            case EnPassant:
               // update hash code
               Xor(newHash, start, WhitePawn);
               Xor(newHash, dest, WhitePawn);
               target = state.enPassantSq;
               break;
            case Promotion:
               // update hash code
               Xor(newHash, start, WhitePawn);
               Xor(newHash, dest, MakeWhitePiece(PromoteTo(move)));
               break;
            default:
               Xor(newHash, start, WhitePawn );
               Xor(newHash, dest, WhitePawn );
               if (start - dest == 16) // 2-square pawn advance
               {
                  if (TEST_MASK(Attacks::ep_mask[File(dest)-1][(int)White],pawn_bits[Black])) {
                    newHash ^= ep_codes[0];
                    newHash ^= ep_codes[dest];
                  }
               }
               break;
            }
            break;
         case Knight:
            Xor(newHash, start, WhiteKnight);
            Xor(newHash, dest, WhiteKnight);
            break;
         case Bishop:
            Xor(newHash, start, WhiteBishop);
            Xor(newHash, dest, WhiteBishop);
            break;
         case Rook:
            Xor(newHash, start, WhiteRook );
            Xor(newHash, dest, WhiteRook );
            if ((int)state.castleStatus[White]<3) {
               newHash ^= w_castle_status[(int)state.castleStatus[White]];
               newHash ^= w_castle_status[(int)UpdateCastleStatusW(state.castleStatus[White],start)];
            }
            break;
         case Queen:
            Xor(newHash, start, WhiteQueen);
            Xor(newHash, dest, WhiteQueen);
            break;
         case King:
            Xor(newHash, start, WhiteKing );
            Xor(newHash, dest, WhiteKing );
            newHash ^= w_castle_status[(int)castleStatus(White)];
            newHash ^= w_castle_status[(int)CantCastleEitherSide];
            break;
         }
         if (Capture(move) != Empty)
         {
            Piece cap = MakeBlackPiece(Capture(move));
            ASSERT(OnBoard(target));
            Xor(newHash, target, cap);
            if (Capture(move) == Rook) {
               if ((int)state.castleStatus[Black]<3) {
                  newHash ^= b_castle_status[(int)state.castleStatus[Black]];
                  newHash ^= b_castle_status[(int)UpdateCastleStatusB(state.castleStatus[Black],dest)];
               }
            }
         }
      }

   }
   else // side == Black
   {
      if (moveType == KCastle)
      {
         const Square kp = kingSquare(Black);
         Xor(newHash, kp+3, BlackRook);
         Xor(newHash, kp, BlackKing);
         Xor(newHash, kp+1, BlackRook);
         Xor(newHash, kp+2, BlackKing);
         newHash ^= b_castle_status[(int)state.castleStatus[Black]];
         newHash ^= b_castle_status[(int)CantCastleEitherSide];
      }
      else if (moveType == QCastle)
      {
         const Square kp = kingSquare(Black);
         Xor(newHash, kp-4, BlackRook);
         Xor(newHash, kp, BlackKing);
         Xor(newHash, kp-1, BlackRook);
         Xor(newHash, kp-2, BlackKing);
         newHash ^= b_castle_status[(int)state.castleStatus[Black]];
         newHash ^= b_castle_status[(int)CantCastleEitherSide];
      }
      else // not castling
      {
         Square target = dest; // where we captured
         switch (TypeOfPiece(contents[StartSquare(move)]))
         {
         case Empty: break;
         case Pawn:
            switch (moveType)
            {
            case EnPassant:
               // update hash code
               Xor(newHash, start, BlackPawn);
               Xor(newHash, dest, BlackPawn);
               target = state.enPassantSq;
               break;
            case Promotion:
               // update hash code
               Xor(newHash, start, BlackPawn);
               Xor(newHash, dest, MakeBlackPiece(PromoteTo(move)));
               break;
            default:
               Xor(newHash, start, BlackPawn );
               Xor(newHash, dest, BlackPawn );
               if (dest - start == 16) // 2-square pawn advance
               {
                  if (TEST_MASK(Attacks::ep_mask[File(dest)-1][(int)Black],pawn_bits[White])) {
                    newHash ^= ep_codes[0];
                    newHash ^= ep_codes[dest];
                  }
               }
               break;
            }
            break;
         case Knight:
            Xor(newHash, start, BlackKnight);
            Xor(newHash, dest, BlackKnight);
            break;
         case Bishop:
            Xor(newHash, start, BlackBishop);
            Xor(newHash, dest, BlackBishop);
            break;
         case Rook:
            Xor(newHash, start, BlackRook );
            Xor(newHash, dest, BlackRook );
            if ((int)state.castleStatus[Black]<3) {
               newHash ^= b_castle_status[(int)state.castleStatus[Black]];
               newHash ^= b_castle_status[(int)UpdateCastleStatusB(state.castleStatus[Black],dest)];
            }
            break;
         case Queen:
            Xor(newHash, start, BlackQueen);
            Xor(newHash, dest, BlackQueen);
            break;
         case King:
            Xor(newHash, start, BlackKing );
            Xor(newHash, dest, BlackKing );
            newHash ^= b_castle_status[(int)castleStatus(Black)];
            newHash ^= b_castle_status[(int)CantCastleEitherSide];
            break;
         }
         if (Capture(move) != Empty)
         {
            Piece cap = MakeWhitePiece(Capture(move));
            ASSERT(OnBoard(target));
            Xor(newHash, target, cap);
            if (Capture(move) == Rook) {
               if ((int)state.castleStatus[White]<3) {
                  newHash ^= w_castle_status[(int)state.castleStatus[White]];
                  newHash ^= w_castle_status[(int)UpdateCastleStatusW(state.castleStatus[White],dest)];
               }
            }
         }

      }
   }

   if (sideToMove() == White)
      newHash |= (hash_t)1;
   else
      newHash &= (hash_t)~1;
   return newHash;
}

void Board::undoCastling(Square kp, Square oldkingsq, Square newrooksq,
                          Square oldrooksq)
{
   contents[kp] = EmptyPiece;
   contents[oldrooksq] = MakePiece(Rook,side);
   contents[newrooksq] = EmptyPiece;
   contents[oldkingsq] = MakePiece(King,side);
   kingPos[side] = oldkingsq;
   rook_bits[side].set(oldrooksq);
   rook_bits[side].clear(newrooksq);

   setAll(side,oldrooksq);
   setAll(side,oldkingsq);
   clearAll(side,kp);
   clearAll(side,newrooksq);
}

void Board::undoMove( Move move, const BoardState &old_state )
{
   side = OppositeColor(side);
   if (!IsNull(move))
   {
      const MoveType moveType = TypeOfMove(move);
      const Square start = StartSquare(move);
      const Square dest = DestSquare(move);
      if (moveType == KCastle)
      {
         Square kp = kingSquare(side);
         Square oldrooksq = kp+1;
         Square newrooksq = kp-1;
         Square oldkingsq = kp-2;
         undoCastling(kp,oldkingsq,newrooksq,oldrooksq);
      }
      else if (moveType == QCastle)
      {
         Square kp = kingSquare(side);
         Square oldrooksq = kp-2;
         Square newrooksq = kp+1;
         Square oldkingsq = kp+2;
         undoCastling(kp,oldkingsq,newrooksq,oldrooksq);
      }
      else if (side == White)
      {
         const Bitboard bits(Bitboard::mask[start] |
                           Bitboard::mask[dest]);
         // not castling
         Square target = dest;
         // fix up start square:
         if (moveType == Promotion || moveType == EnPassant)
         {
            contents[start] = WhitePawn;
         }
         else
         {
            contents[start] = contents[dest];
         }
         setAll(White,start);
         switch (TypeOfPiece(contents[start])) {
         case Empty: break;
         case Pawn:
            Xor(pawnHashCodeW,start,WhitePawn);
            switch (moveType) {
            case Promotion:
               material[White].addPawn();
               material[White].removePiece(PromoteTo(move));
               switch (PromoteTo(move))
               {
               case Knight:
                  knight_bits[White].clear(dest);
                  break;
               case Bishop:
                  bishop_bits[White].clear(dest);
                  break;
               case Rook:
                  rook_bits[White].clear(dest);
                  break;
               case Queen:
                  queen_bits[White].clear(dest);
                  break;
               default:
                  break;
               }
               break;
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
            case EnPassant:
               target = dest - 8;
               ASSERT(OnBoard(target));
               ASSERT(contents[target]==EmptyPiece);
               // note: falls through to normal case
            case Normal:
               pawn_bits[White].clear(dest);
               Xor(pawnHashCodeW,dest,WhitePawn);
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
            default:
               break;
            }
            pawn_bits[White].set(start);
            break;
         case Knight:
            knight_bits[White].setClear(bits);
            break;
         case Bishop:
            bishop_bits[White].setClear(bits);
            break;
         case Rook:
            rook_bits[White].setClear(bits);
            break;
         case Queen:
            queen_bits[White].setClear(bits);
            break;
         case King:
            kingPos[White] = start;
            break;
         default:
            break;
         }
         // fix up dest square
         clearAll(White,dest);
         contents[dest] = EmptyPiece;
         contents[target] = MakePiece(Capture(move),Black);
         if (Capture(move) != Empty)
         {
            switch (Capture(move))
            {
            case Pawn:
                           ASSERT(!pawn_bits[Black].isSet(target));
               pawn_bits[Black].set(target);
               Xor(pawnHashCodeB,target,BlackPawn);
               material[Black].addPawn();
               break;
            case Knight:
               knight_bits[Black].set(target);
               material[Black].addPiece(Knight);
               break;
            case Bishop:
               bishop_bits[Black].set(target);
               material[Black].addPiece(Bishop);
               break;
            case Rook:
               rook_bits[Black].set(target);
               material[Black].addPiece(Rook);
               break;
            case Queen:
               queen_bits[Black].set(target);
               material[Black].addPiece(Queen);
               break;
            case King:
               kingPos[Black] = target;
               material[Black].addPiece(King);
               break;
            default:
               break;
            }
            setAll(Black,target);
         }
      }
      else // side == Black
      {
         const Bitboard bits(Bitboard::mask[start] |
                           Bitboard::mask[dest]);
         // not castling
         Square target = dest;
         // fix up start square:
         if (moveType == Promotion || moveType == EnPassant)
         {
            contents[start] = BlackPawn;
         }
         else
         {
            contents[start] = contents[dest];
         }
         setAll(Black,start);
         switch (TypeOfPiece(contents[start])) {
         case Empty: break;
         case Pawn:
            Xor(pawnHashCodeB,start,BlackPawn);
            switch (moveType) {
            case Promotion:
            {
               material[Black].addPawn();
               material[Black].removePiece(PromoteTo(move));
               switch (PromoteTo(move))
               {
               case Knight:
                  knight_bits[Black].clear(dest);
                  break;
               case Bishop:
                  bishop_bits[Black].clear(dest);
                  break;
               case Rook:
                  rook_bits[Black].clear(dest);
                  break;
               case Queen:
                  queen_bits[Black].clear(dest);
                  break;
               default:
                  break;
               }
               break;
            }
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
            case EnPassant:
               target = dest + 8;
               ASSERT(OnBoard(target));
               ASSERT(contents[target]== EmptyPiece);
               // note: falls through to normal case
            case Normal:
               pawn_bits[Black].clear(dest);
               Xor(pawnHashCodeB,dest,BlackPawn);
               break;
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
            default:
               break;
            }
            pawn_bits[Black].set(start);
            break;
         case Knight:
            knight_bits[Black].setClear(bits);
            break;
         case Bishop:
            bishop_bits[Black].setClear(bits);
            break;
         case Rook:
            rook_bits[Black].setClear(bits);
            break;
         case Queen:
            queen_bits[Black].setClear(bits);
            break;
         case King:
            kingPos[Black] = start;
            break;
         default:
            break;
         }
         // fix up dest square
         clearAll(Black,dest);
         contents[dest] = EmptyPiece;
         contents[target] = MakePiece(Capture(move),White);
         if (Capture(move) != Empty)
         {
            switch (Capture(move))
            {
            case Pawn:
                           ASSERT(!pawn_bits[White].isSet(target));
               pawn_bits[White].set(target);
               Xor(pawnHashCodeW,target,WhitePawn);
               material[White].addPawn();
               break;
            case Knight:
               knight_bits[White].set(target);
               material[White].addPiece(Knight);
               break;
            case Bishop:
               bishop_bits[White].set(target);
               material[White].addPiece(Bishop);
               break;
            case Rook:
               rook_bits[White].set(target);
               material[White].addPiece(Rook);
               break;
            case Queen:
               queen_bits[White].set(target);
               material[White].addPiece(Queen);
               break;
            case King:
               kingPos[White] = target;
               material[White].addPiece(King);
               break;
            default:
               break;
            }
            setAll(White,target);
         }
      }
   }
   state = old_state;
   ASSERT(getMaterial(sideToMove()).pawnCount() == (int)pawn_bits[side].bitCount());
   ASSERT(getMaterial(oppositeSide()).pawnCount() == (int)pawn_bits[oppositeSide()].bitCount());

   --repListHead;
   allOccupied = Bitboard(occupied[White] | occupied[Black]);
   ASSERT(state.hashCode == BoardHash::hashCode(*this));
#if defined(_DEBUG) && defined(FULL_DEBUG)
   // verify correct updating of bitmaps:
   Board copy = *this;
   ASSERT(pawn_bits[White] == copy.pawn_bits[White]);
   ASSERT(knight_bits[White] == copy.knight_bits[White]);
   ASSERT(bishop_bits[White] == copy.bishop_bits[White]);
   ASSERT(rook_bits[White] == copy.rook_bits[White]);
   ASSERT(queen_bits[White] == copy.queen_bits[White]);
   ASSERT(occupied[White] == copy.occupied[White]);

   ASSERT(pawn_bits[Black] == copy.pawn_bits[Black]);
   ASSERT(knight_bits[Black] == copy.knight_bits[Black]);
   ASSERT(bishop_bits[Black] == copy.bishop_bits[Black]);
   ASSERT(rook_bits[Black] == copy.rook_bits[Black]);
   ASSERT(queen_bits[Black] == copy.queen_bits[Black]);
   ASSERT(occupied[Black] == copy.occupied[Black]);
   ASSERT(contents[kingPos[White]]==WhiteKing);
   ASSERT(contents[kingPos[Black]]==BlackKing);
   ASSERT(validPiece(contents[StartSquare(move)]));
   ASSERT(validPiece(contents[DestSquare(move)]));
#endif
}

int Board::wouldAttack(Move m,Square target) const {
  Bitboard attacks;
  Square sq = DestSquare(m);
  switch(PieceMoved(m)) {
  case Empty: break;
  case Pawn:
    attacks = Attacks::pawn_attacks[sq][OppositeColor(side)];
    break;
  case Knight:
    attacks = Attacks::knight_attacks[sq];
    break;
  case Bishop:
    attacks = bishopAttacks(sq); break;
  case Rook:
    attacks = rookAttacks(sq); break;
  case Queen:
    attacks = bishopAttacks(sq) | rookAttacks(sq); break;
  case King:
    attacks = Attacks::king_attacks[sq];
    break;
  }
  return attacks.isSet(target);
}

int Board::anyAttacks(const Square sq, ColorType side) const
{
   if (sq == InvalidSquare)
      return 0;
   if (TEST_MASK(Attacks::pawn_attacks[sq][side],pawn_bits[side])) return 1;
   if (TEST_MASK(Attacks::knight_attacks[sq],knight_bits[side])) return 1;
   if (Attacks::king_attacks[sq].isSet(kingSquare(side))) return 1;
   if (TEST_MASK(rook_bits[side] | queen_bits[side],rookAttacks(sq))) return 1;
   if (TEST_MASK(bishop_bits[side] | queen_bits[side],bishopAttacks(sq))) return 1;
   return 0;
}

int Board::anyAttacks(Square sq, ColorType side, Bitboard &source) const
{
   if (sq == InvalidSquare)
      return 0;
   source = Bitboard(Attacks::pawn_attacks[sq][side] & pawn_bits[side]);
   if (!source.isClear()) return 1;
   source = Bitboard(Attacks::knight_attacks[sq] & knight_bits[side]);
   if (!source.isClear()) return 1;
   source = Bitboard((uint64_t)Attacks::king_attacks[sq] & (1ULL<<kingSquare(side)));
   if (!source.isClear()) return 1;
   source = Bitboard((rook_bits[side] | queen_bits[side]) & rookAttacks(sq));
   if (!source.isClear()) return 1;
   source = Bitboard((bishop_bits[side] | queen_bits[side]) & bishopAttacks(sq));
   return !source.isClear();
}

Bitboard Board::allAttacks(ColorType side) const
{
   Square sq;
   Bitboard ret(allPawnAttacks(side));
   Bitboard knights(knight_bits[side]);
   while (knights.iterate(sq)) ret |= Attacks::knight_attacks[sq];
   Bitboard bishops(bishop_bits[side]);
   while (bishops.iterate(sq)) ret |= bishopAttacks(sq);
   Bitboard rooks(rook_bits[side]);
   while (rooks.iterate(sq)) ret |= rookAttacks(sq);
   Bitboard queens(queen_bits[side]);
   while (queens.iterate(sq)) ret |= queenAttacks(sq);
   return ret | Attacks::king_attacks[kingSquare(side)];
}

Bitboard Board::calcAttacks(Square sq, ColorType side) const
{
   ASSERT(sq != InvalidSquare);

   Bitboard retval;

   retval |= (Attacks::pawn_attacks[sq][side] & pawn_bits[side]);
   retval |= (Attacks::knight_attacks[sq] & knight_bits[side]);
   retval |= (Attacks::king_attacks[sq] & ((uint64_t)1<<kingSquare(side)));
   retval |= (rookAttacks(sq) & (rook_bits[side] | queen_bits[side]));
   retval |= (bishopAttacks(sq) & (bishop_bits[side] | queen_bits[side]));

   return retval;
}

Bitboard Board::calcBlocks(Square sq, ColorType side) const
{
   ASSERT(sq != InvalidSquare);

   Bitboard retval;

   if (side == Black)
   {
       Square origin = sq-8;
       if (OnBoard(origin) && contents[origin] == BlackPawn)
          retval.set(origin);
       if (Rank<Black>(sq) == 4 && contents[origin] == EmptyPiece &&
           contents[origin - 8] == BlackPawn)
          retval.set(origin - 8);
   }
   else
   {
       Square origin = sq+8;
       if (OnBoard(origin) && contents[origin] == WhitePawn)
          retval.set(origin);
       if (Rank<White>(sq) == 4 && contents[origin] == EmptyPiece &&
          contents[origin + 8] == WhitePawn)
          retval.set(origin + 8);
   }

   retval |= (Attacks::knight_attacks[sq] & knight_bits[side]);
   retval |= (rookAttacks(sq) & (rook_bits[side] | queen_bits[side]));
   retval |= (bishopAttacks(sq) & (bishop_bits[side] | queen_bits[side]));

   return retval;
}


Square Board::getDirectionalAttack(Square sq, int dir, ColorType side) const {
   Square attacker;
   switch (dir)
   {
   case 1:
      attacker = Bitboard(rankAttacksRight(sq) & allOccupied).firstOne();
      if (attacker == InvalidSquare ||
          (!Bitboard(rook_bits[side] | queen_bits[side]).isSet(attacker))) {
         return InvalidSquare;
      }
      else {
         return attacker;
      }
      break;
   case -1:
      attacker = Bitboard(rankAttacksLeft(sq) & allOccupied).lastOne();
      if (attacker == InvalidSquare ||
          (!Bitboard(rook_bits[side] | queen_bits[side]).isSet(attacker))) {
         return InvalidSquare;
      }
      else {
         return attacker;
      }
      break;
   case 8:
      attacker = Bitboard(fileAttacksUp(sq) & allOccupied).firstOne();
      if (attacker == InvalidSquare ||
          (!Bitboard(rook_bits[side] | queen_bits[side]).isSet(attacker))) {
         return InvalidSquare;
      }
      else {
         return attacker;
      }
      break;
   case -8:
      attacker = Bitboard(fileAttacksDown(sq) & allOccupied).lastOne();
      if (attacker == InvalidSquare ||
          (!Bitboard(rook_bits[side] | queen_bits[side]).isSet(attacker))) {
         return InvalidSquare;
      }
      else {
         return attacker;
      }
      break;
   case 7:
      attacker = Bitboard(diagAttacksA8Upper(sq) & allOccupied).lastOne();
      if (attacker == InvalidSquare ||
          (!Bitboard(bishop_bits[side] | queen_bits[side]).isSet(attacker))) {
         return InvalidSquare;
      }
      else {
         return attacker;
      }
      break;
   case -7:
      attacker = Bitboard(diagAttacksA8Lower(sq) & allOccupied).firstOne();
      if (attacker == InvalidSquare ||
          (!Bitboard(bishop_bits[side] | queen_bits[side]).isSet(attacker))) {
         return InvalidSquare;
      }
      else {
         return attacker;
      }
      break;
   case 9:
      attacker = Bitboard(diagAttacksA1Upper(sq) & allOccupied).firstOne();
      if (attacker == InvalidSquare ||
          (!Bitboard(bishop_bits[side] | queen_bits[side]).isSet(attacker))) {
         return InvalidSquare;
      }
      else {
         return attacker;
      }
      break;
   case -9:
      attacker = Bitboard(diagAttacksA1Lower(sq) & allOccupied).lastOne();
      if (attacker == InvalidSquare ||
          (!Bitboard(bishop_bits[side] | queen_bits[side]).isSet(attacker))) {
         return InvalidSquare;
      }
      else {
         return attacker;
      }
      break;
   default:
      return InvalidSquare;
   }
}

Bitboard Board::allPawnAttacks(ColorType side, Bitboard pawns) const
{
   if (side == Black)
   {
      Bitboard pawns2(pawns);
      pawns.shr(7);
      pawns &= Bitboard(~0x0101010101010101ULL);
      pawns2.shr(9);
      pawns2 &= Bitboard(~0x8080808080808080ULL);
      return (pawns | pawns2);
   }
   else
   {
      Bitboard pawns2(pawns);
      pawns.shl(7);
      pawns &= Bitboard(~0x8080808080808080ULL);
      pawns2.shl(9);
      pawns2 &= Bitboard(~0x0101010101010101ULL);
      return (pawns | pawns2);
   }
}

const Bitboard Board::rookAttacks(Square sq,ColorType side) const {
   Board &b = (Board&)*this;
   b.allOccupied &= ~(rook_bits[side] | queen_bits[side]);
   Bitboard attacks(rookAttacks(sq));
   b.allOccupied |= (rook_bits[side] | queen_bits[side]);
   return attacks;
}

const Bitboard Board::bishopAttacks(Square sq,ColorType side) const {
   Board &b = (Board&)*this;
   b.allOccupied &= ~(queen_bits[side] | bishop_bits[side]);
   Bitboard attacks(bishopAttacks(sq));
   b.allOccupied |= (queen_bits[side] | bishop_bits[side]);
   return attacks;
}

CheckStatusType Board::getCheckStatus() const
{
   if (anyAttacks(kingSquare(sideToMove()),oppositeSide()))
   {
      // This is a const function, but we cache its result
      Board &b = (Board&)*this;
      b.state.checkStatus = InCheck;
   }
   else
   {
      // This is a const function, but we cache its result
      Board &b = (Board&)*this;
      b.state.checkStatus = NotInCheck;
   }
   return state.checkStatus;
}

// This variant of CheckStatus sees if the last move made
// delivered check. It is generally faster than checkStatus
// with no param, because we can use the last move information
// to avoid calling anyAttacks, in many cases.
CheckStatusType Board::checkStatus(Move lastMove) const {
   if (state.checkStatus != CheckUnknown) {
      return state.checkStatus;
   }
   if (IsNull(lastMove)) {
      return checkStatus();
   }
   Square kp = kingPos[side];
   Square checker = DestSquare(lastMove);
   int d = Attacks::directions[checker][kp];
   Board &b = (Board&)*this;
   switch(PieceMoved(lastMove)) {
   case Pawn: {
      if (TypeOfMove(lastMove) != Normal)
         return checkStatus();
      if (Attacks::pawn_attacks[kp][oppositeSide()].isSet(checker)) {
         b.state.checkStatus = InCheck;
      }
      else if (Attacks::directions[kp][StartSquare(lastMove)] == 0) {
         b.state.checkStatus = NotInCheck;
      }
      if (state.checkStatus == CheckUnknown)
         return checkStatus();
      else
         return state.checkStatus;
   }
   case Rook:
      switch(d) {
      case 1:
      case -1:
      case 8:
      case -8:
         if (clear(checker,kp)) {
            b.state.checkStatus = InCheck;
         }
      default:
         break;
      }
      if (state.checkStatus == CheckUnknown) {
         // check for discovered attack
         if (discAttackDiag(StartSquare(lastMove),kp,oppositeSide())) {
            b.state.checkStatus = InCheck;
         }
         else {
            b.state.checkStatus = NotInCheck;
         }
      }
      break;
   case Bishop:
      switch(d) {
      case 7:
      case -7:
      case 9:
      case -9:
         if (clear(checker,kp)) {
            b.state.checkStatus = InCheck;
         }
      default:
         break;
      }
      if (state.checkStatus == CheckUnknown) {
         // check for discovered attack
         if (discAttackRankFile(StartSquare(lastMove),kp,oppositeSide())) {
            b.state.checkStatus = InCheck;
         }
         else {
            b.state.checkStatus = NotInCheck;
         }
      }
      break;
   case Knight:
      if (Attacks::knight_attacks[checker].isSet(kp)) {
         b.state.checkStatus = InCheck;
      }
      else {
         if (state.checkStatus == CheckUnknown) {
            // check for discovered attack
            if (discAttack(StartSquare(lastMove),kp,oppositeSide())) {
               b.state.checkStatus = InCheck;
            }
            else {
               b.state.checkStatus = NotInCheck;
            }
         }
      }
      break;
   case Queen:
      if (d && clear(checker,kp)) {
         b.state.checkStatus = InCheck;
      } else {
         b.state.checkStatus = NotInCheck;
      }
      break;
   case King:
      if (TypeOfMove(lastMove) != Normal) /* castling */
         return checkStatus();
      if (Attacks::king_attacks[checker].isSet(kp)) {
         b.state.checkStatus = InCheck;
         return InCheck;
      }
      else if (Attacks::directions[StartSquare(lastMove)][kp] == 0) {
         b.state.checkStatus = NotInCheck;
         return NotInCheck;
      }
      else {
         if (discAttack(StartSquare(lastMove),kp,oppositeSide())) {
            b.state.checkStatus = InCheck;
         }
         else {
            b.state.checkStatus = NotInCheck;
         }
      }
      break;
   default:
      break;
   } // end switch
   return checkStatus();
}


CheckStatusType Board::wouldCheck(Move lastMove) const {
   Square kp = kingPos[oppositeSide()];
   const Square checker = DestSquare(lastMove);
   // check for discovered check first
   if (isPinned(oppositeSide(),lastMove)) return InCheck;
   switch(PieceMoved(lastMove)) {
      case Pawn: {
          switch (TypeOfMove(lastMove)) {
          case EnPassant:
             if (Attacks::pawn_attacks[kp][sideToMove()].isSet(checker)) {
                 return InCheck;
             }
             return CheckUnknown;
          case Promotion:
             // see if the promoted to piece would check the King:
             switch(PromoteTo(lastMove)) {
             case Knight:
                 return Attacks::knight_attacks[checker].isSet(kp) ?
                 InCheck : NotInCheck;
             case Bishop: {
                 const int d = (int)Attacks::directions[checker][kp];
                 if (std::abs(d) == 7 || std::abs(d) == 9) {
                     Board &b = (Board&)*this;
                     b.allOccupied.clear(StartSquare(lastMove));
                     int in_check = bishopAttacks(checker).isSet(kp);
                     b.allOccupied.set(StartSquare(lastMove));
                     return in_check ? InCheck : NotInCheck;
                 } else {
                     return NotInCheck;
                 }
             }
             break;
             case Rook: {
                 const int d = (int)Attacks::directions[checker][kp];
                 if (std::abs(d) == 1 || std::abs(d) == 8) {
                     Board &b = (Board&)*this;
                     b.allOccupied.clear(StartSquare(lastMove));
                     int in_check = rookAttacks(checker).isSet(kp);
                     b.allOccupied.set(StartSquare(lastMove));
                     return in_check ? InCheck : NotInCheck;
                 }
                 else {
                     return NotInCheck;
                 }
             }
             case Queen: {
                 const int d = (int)Attacks::directions[checker][kp];
                 if (d) {
                     Board &b = (Board&)*this;
                     b.allOccupied.clear(StartSquare(lastMove));
                     int in_check = queenAttacks(checker).isSet(kp);
                     b.allOccupied.set(StartSquare(lastMove));
                     return in_check ? InCheck : NotInCheck;
                 } else {
                     return NotInCheck;
                 }
             }
             default:
              break;
             }
             break;
          case Normal: {
             if (Attacks::pawn_attacks[kp][sideToMove()].isSet(checker)) {
                 return InCheck;
             } else {
                 return NotInCheck;
             }
          }
          case KCastle:
          case QCastle:
          break;
          }
      }
      break;
      case Knight:
        if (Attacks::knight_attacks[checker].isSet(kp)) {
           return InCheck;
        } else {
           return NotInCheck;
        }
      case King: {
          if (TypeOfMove(lastMove) != Normal) {
             return CheckUnknown;
          } else if (Attacks::king_attacks[DestSquare(lastMove)].isSet(kp)) {
             return InCheck;
          } else {
             return NotInCheck;
          }
       }
       case Bishop: {
          const int d = (int)Attacks::directions[checker][kp];
          switch(d) {
            case 7:
            case -7:
            case 9:
            case -9:
               return ((Attacks::betweenSquares[checker][kp] & allOccupied) ? NotInCheck : InCheck);
            default:
              break;
          }
          break;
       }
       case Rook: {
          const int d = (int)Attacks::directions[checker][kp];
          switch(d) {
            case 1:
            case -1:
            case 8:
            case -8:
            return ((Attacks::betweenSquares[checker][kp] & allOccupied) ? NotInCheck : InCheck);

            default:
              break;
          }
          break;
        }
        case Queen:
          return clear(checker,kp) ? InCheck : NotInCheck;
        default:
         break;
   }
   return NotInCheck;
}

int Board::wasLegal(Move lastMove, bool evasion) const {
    if (IsNull(lastMove)) return 1;
    Square kp = kingSquare(oppositeSide());
    if (evasion) {
        if (GetPhase(lastMove) == MoveGenerator::HASH_MOVE_PHASE) {
            // Ensure that the hash move does actually evade check
            return !anyAttacks(kp,sideToMove());
        } else {
            // Non-hash move legality is ensured by move generator.
            return 1;
        }
    }
    switch (TypeOfMove(lastMove)) {
       case QCastle:
       case KCastle:
         return 1; // checked for legality in move generator
       case EnPassant:
         return !anyAttacks(kp,sideToMove());
       default:
         break;
    }
    return !anyAttacks(kp,sideToMove());
}


// True if moving from 'source' to 'dest' uncovers an attack by 'side' on
// 'target'
int Board::discoversAttack(Square source, Square dest, Square target, ColorType side) const
{
   ASSERT(OnBoard(source));
   ASSERT(OnBoard(dest));
   ASSERT(OnBoard(target));
   const int dir = Attacks::directions[source][target];
   // check that source is in a line to the King
   if (dir == 0) return 0;
   const int dir2 =  Attacks::directions[dest][target];
   // check for movement in direction of possible pin. Also exit
   // here if path is not clear to the King
   if (std::abs(dir) == std::abs(dir2) ||
       (Attacks::betweenSquares[source][target] & allOccupied)) return 0;

   Square attackSq = getDirectionalAttack(source,-dir,side);
   if (attackSq != InvalidSquare ) {
      // pinned if path is clear from attackSq to source
      return Bitboard(Attacks::betweenSquares[attackSq][source] & allOccupied).isClear();
   } else {
      return 0;
   }
}

static void set_bad( istream &i )
{
   i.clear( ios::badbit | i.rdstate() );
}

int Board::repCount(int target) const noexcept
{
    int entries = std::min<int>(state.movesFromNull,state.moveCount) - 2;
    if (entries <= 0) return 0;
    hash_t to_match = hashCode();
    int count = 0;
    for (hash_t *repList=repListHead-3;
       entries>=0;
       repList-=2,entries-=2)
    {
      if (*repList == to_match)
      {
         count++;
         if (count >= target)
         {
            return count;
         }
      }
   }
   return count;
}

bool Board::anyRep() const noexcept
{
   int entries = state.moveCount;
   // If only 2 entries side to move is different so the
   // hash codes cannot match:
   if (entries < 3) return 0;
   unordered_set<hash_t> codes;
   for (hash_t *repList=repListHead-1;
      entries>0;
      repList--,entries--) {
      if (!codes.emplace(*repList).second) {
         return 1;
      }
   }
   return 0;
}

bool Board::fiftyMoveDraw() const noexcept {
   // check the 50 move rule
   if (state.moveCount >= 100) {
      if (checkStatus() == InCheck) {

         // must verify side to move is not checkmated
         MoveGenerator mg(*this);
         Move moves[Constants::MaxMoves];
         return(mg.generateAllMoves(moves, 0) > 0);
      }
      else {
         return true;
      }
   }
   return false;
}

Bitboard Board::getPinned(Square ksq, ColorType pinnerSide, ColorType pinnedSide) const {
    // Same algorithm as Stockfish: get potential pinners then
    // determine which are actually pinning.
    Bitboard pinners( ((rook_bits[pinnerSide] | queen_bits[pinnerSide]) &
                 Attacks::rank_file_mask[ksq]) |
                ((bishop_bits[pinnerSide] | queen_bits[pinnerSide]) &
                 Attacks::diag_mask[ksq]));
    Square pinner;
    Bitboard result;
    while (pinners.iterate(pinner)) {
        Bitboard b;
        between(ksq, pinner, b);
        b &= allOccupied;
        if (b.singleBitSet()) {
            // Only one piece between "pinner" and King. See if it is
            // the correct color.
            result |= (b & occupied[pinnedSide]);
        }
    }
    return result;
}

bool Board::materialDraw() const noexcept {
    // check for insufficient material per FIDE Rules of Chess
    const Material &mat1 = getMaterial(White);
    const Material &mat2 = getMaterial(Black);
    if (mat1.pawnCount() || mat2.pawnCount()) {
        return false;
    }
    if ((mat1.kingOnly() || mat1.infobits() == Material::KB || mat1.infobits() == Material::KN) &&
        (mat2.kingOnly() || mat2.infobits() == Material::KB || mat2.infobits() == Material::KN)) {
        if (mat1.kingOnly() || mat2.kingOnly()) {
            // K vs K, or K vs KN, or K vs KB
            return true;
        }
        else if (mat1.infobits() == Material::KB && mat2.infobits() == Material::KB) {
            // drawn if same-color bishops
            if (TEST_MASK(bishop_bits[White],black_squares))
                return TEST_MASK(bishop_bits[Black],black_squares);
            else if (TEST_MASK(bishop_bits[White],white_squares))
                return TEST_MASK(bishop_bits[Black],white_squares);
        }
    }
    return false;
}

void Board::flip() {
   for (int i=0;i<4;i++) {
     for (int j=0;j<8;j++) {
        Piece tmp = contents[i*8+j];
        tmp = MakePiece(TypeOfPiece(tmp),OppositeColor(PieceColor(tmp)));
        Piece tmp2 = contents[(7-i)*8+j];
        tmp2 = MakePiece(TypeOfPiece(tmp2),OppositeColor(PieceColor(tmp2)));
        contents[i*8+j] = tmp2;
        contents[(7-i)*8+j] = tmp;
        if (i*8+j == state.enPassantSq) {
            state.enPassantSq = (7-i)*8+j;
        } else if ((7-i)*8+j == state.enPassantSq) {
            state.enPassantSq = i*8+j;
        }
     }
   }
   CastleType tmp = state.castleStatus[White];
   state.castleStatus[White] = state.castleStatus[Black];
   state.castleStatus[Black] = tmp;
   side = OppositeColor(side);
   setSecondaryVars();
}

istream & operator >> (istream &i, Board &board)
{
   // read in a board position in Forsythe-Edwards (FEN) notation.
   stringstream buf;
   char c = '\0';

   // skip leading spaces/newlines
   i >> c;

   if (i.bad() || i.eof()) return i;

   i.putback(c);

   int fields = 0;
   // read only the required 4 fields that are part of FEN:
   // leave any remaining text such as EPD operators unread.
   while (i.good() && !i.eof() && fields < 4) {
      if (!i.get(c) || c == '\n') break;
      buf << c;
      if (isspace(c))
         fields++;
   }

   if (i.fail())
      return i;

   if (!BoardIO::readFEN(board, buf.str())) {
       set_bad(i);
   }
   // Simple sanity check
   else if (board.kingSquare(White) == InvalidSquare ||
       board.kingSquare(Black) == InvalidSquare) {
      set_bad(i);
   }

   return i;
}

ostream & operator << (ostream &o, const Board &board)
{
   BoardIO::writeFEN(board,o,1);
   return o;
}

int Board::discAttackDiag(Square sq, Square ksq, ColorType side) const {
    Bitboard mask(Attacks::diag_mask[ksq]);
    if (mask.isSet(sq)) {
        Bitboard attackers((bishop_bits[side] | queen_bits[side]) & mask);
        Square attacker;
        while (attackers.iterate(attacker)) {
            if (clear(attacker,ksq)) {
                return 1;
            }
        }
    }
    return 0;
}

int Board::discAttackRankFile(Square sq, Square ksq, ColorType side) const {
    Bitboard mask(Attacks::rank_file_mask[ksq]);
    if (mask.isSet(sq)) {
        Bitboard attackers((rook_bits[side] | queen_bits[side]) & mask);
        Square attacker;
        while (attackers.iterate(attacker)) {
            if (clear(attacker,ksq)) {
                return 1;
            }
        }
    }
    return 0;
}

int Board::discAttack(Square sq, Square ksq, ColorType side) const {
    return discAttackRankFile(sq, ksq, side) ||
        discAttackDiag(sq,ksq,side);
}
