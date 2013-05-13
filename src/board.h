// Copyright 1994-2000, 2004, 2005 by Jon Dart.  All Rights Reserved.

#include "types.h"
#include "chess.h"

#ifndef _BOARD_H
#define _BOARD_H

#include "bitboard.h"
#include "attacks.h"
#include "material.h"

class Board;

extern const hash_t rep_codes[3];

enum CastleType { CanCastleEitherSide,
                  CanCastleKSide,
                  CanCastleQSide,
                  CastledKSide,
                  CastledQSide,
                  CantCastleEitherSide};
          
enum CheckStatusType { NotInCheck, InCheck, CheckUnknown };

struct BoardState {
   hash_t hashCode;
   Square enPassantSq;
   int moveCount;
   CheckStatusType checkStatus;
   CastleType castleStatus[2];
};

class Board
{
   // This class encapsulates the chess board, and provides functions
   // for making and undoing moves.          

public: 

   friend class BoardIO;
   friend class Scoring;
   
   // creates a board set up in its initial position, White to move.
   Board();
           
  ~Board();
  
   // Copy constructor and assignment operator  
   Board(const Board &);
   Board &operator = (const Board &);

   // resets board to initial position
   void reset();

   // remove all pieces
   void makeEmpty();
           
#ifdef _DEBUG
   const Piece &operator[]( const Square sq ) const;
#else
   const Piece &operator[]( const Square sq ) const
   {
       return contents[sq];
   }
#endif

   void setContents( const Piece p, const Square sq )
   {
       contents[sq] = p;
   }
   
   // update all fields of the board, based on the piece positions.
   void setSecondaryVars();

   CastleType castleStatus( ColorType side ) const
   {
       return state.castleStatus[side];        
   }
   
   void setCastleStatus( CastleType t, ColorType side )
   {
       state.castleStatus[side] = t;
   }

   int castled(ColorType side) const
   {
      return state.castleStatus[side] == CastledKSide ||
             state.castleStatus[side] == CastledQSide;
   }

   // side to move         
   ColorType sideToMove() const  {
     return side;  
   }
   
   ColorType oppositeSide() const {
       return OppositeColor(side);
   }
   
   void setSideToMove( ColorType color ) {
     side = color;
   }
   
   const Material &getMaterial( ColorType side ) const
   {
       return material[side];
   }
   
   hash_t pawnHash() const {
      return pawnHashCodeW ^ pawnHashCodeB;
   }

   Square enPassantSq() const {
      return state.enPassantSq;
   }
           
   CheckStatusType checkStatus() const {
     if (state.checkStatus != CheckUnknown) {
         return state.checkStatus;
     } else {
         return getCheckStatus();
     }
    }
   
   CheckStatusType checkStatus(Move lastMove) const;

   // returns InCheck if the move could cause check to the opponent
   // after being made (call before the move is made).
   CheckStatusType wouldCheck(Move lastMove) const;
   
   void setCheckStatus(CheckStatusType s) {
       state.checkStatus = s;
   }

   Square kingSquare(ColorType side) const {
        return kingPos[side];           
   }
           
   hash_t hashCode() const
   {
       return state.hashCode;
   }
   
   // returns a hash code factoring in the position repetition count
   hash_t hashCode(int rep_count) const
   {
       return state.hashCode ^ rep_codes[rep_count];
   }
   
   // returns what hash code will be after move
   hash_t hashCode( Move m ) const;

   int operator == ( const Board &b )
   {
       return state.hashCode == b.hashCode();
   }
   
   // set a bitmap of squares that lie in a straight line between
   // sq1 and sq2
   void between( Square sq1, Square sq2, Bitboard &) const;
   
   // returns "true" if there is a clear path between sq1 and sq2
   int clear( Square sq1, Square sq2 ) const;

   // makes a move
   void doMove(Move m);

   // makes a null move
   void doNull();

   // undoes a previous move.
   void undoMove( Move rmove, const BoardState &stat );
   
   // undoes a previous null move.
   void undoNull(const BoardState &oldState) {
      state = oldState;
       --repListHead;
      side = OppositeColor(side);
   }

   // Return true if move m would attack square "target" after it is
   // made (call before the move is made). This is somewhat imperfect
   // (does not handle irregular moves such as castling)
   int wouldAttack(Move m,Square target) const;

   // Return "TRUE" if any piece of color "side" attacks "sq".
   int anyAttacks(Square sq, ColorType side) const;

   inline int inCheck() const {
      return anyAttacks(kingSquare(sideToMove()),oppositeSide());
   }

   // Return "TRUE" if any piece of color "side" attacks "sq".
   // Also set "source" with the bit location of the attack piece.
   int anyAttacks(Square sq, ColorType side, Bitboard &source) const;

   // Return a bit vector of all pieces of color "side" that attack "sq".
   Bitboard calcAttacks(Square sq, ColorType side) const;
   
   // Return a bit vector of all pieces of color "side" that can move to
   // unoccupied "sq".
   Bitboard calcBlocks(Square sq, ColorType side) const;
   
   // Return the location of least-valued piece of color "side"
   // out of the bitmap.
   Square minAttacker(Bitboard, ColorType side) const;
   
   // Return all attackers of color "side" behind "attack_square" that
   // (indirectly) attack "square"
   Bitboard getXRay(Square attack_square, Square square,ColorType side) const;

   // Return all squares attacked by pawns of color "side".
   Bitboard allPawnAttacks(ColorType side) const;

   FORCEINLINE const Bitboard rookAttacks(Square sq) const {
     return Attacks::rookAttacks(sq,allOccupied);
   }
   
   // Get rook attacks considering same-side rook/queen as "transparent"
   const Bitboard rookAttacks(Square sq,ColorType side) const;

   FORCEINLINE const Bitboard bishopAttacks(Square sq) const {
     return Attacks::bishopAttacks(sq,allOccupied);
   }

   // get bishop attacks considering same-side queen as "transparent"
   const Bitboard bishopAttacks(Square sq,ColorType side) const;

   FORCEINLINE const Bitboard queenAttacks(Square sq) const {
     return Attacks::rookAttacks(sq,allOccupied) |
         Attacks::bishopAttacks(sq,allOccupied);
   }

   FORCEINLINE Bitboard fileAttacks(Square sq) const {
     return rookAttacks(sq) & Attacks::file_mask[File(sq)-1];
   }

   FORCEINLINE Bitboard fileAttacksDown(Square sq) const {
     return rookAttacks(sq) & Attacks::file_mask_down[sq];
   }

   FORCEINLINE Bitboard fileAttacksUp(Square sq) const {
     return rookAttacks(sq) & Attacks::file_mask_up[sq];
   }

   FORCEINLINE Bitboard rankAttacks(Square sq) const {
     return rookAttacks(sq) & Attacks::rank_mask[Rank<White>(sq)-1];
   }

   FORCEINLINE Bitboard rankAttacksRight(Square sq) const {
     return rookAttacks(sq) & Attacks::rank_mask_right[sq];
   }

   FORCEINLINE Bitboard rankAttacksLeft(Square sq) const {
     return rookAttacks(sq) & Attacks::rank_mask_left[sq];
   }

   FORCEINLINE Bitboard diagAttacksA1(Square sq) const {
     return bishopAttacks(sq) & (
            Attacks::diag_a1_upper_mask[sq] | Attacks::diag_a1_lower_mask[sq]);
   }

   FORCEINLINE Bitboard diagAttacksA8(Square sq) const {
     return bishopAttacks(sq) & (
            Attacks::diag_a8_upper_mask[sq] | Attacks::diag_a8_lower_mask[sq]);
   }

   FORCEINLINE Bitboard diagAttacksA1Upper(Square sq) const {
     return bishopAttacks(sq) & Attacks::diag_a1_upper_mask[sq];
   }

   FORCEINLINE Bitboard diagAttacksA1Lower(Square sq) const {
     return bishopAttacks(sq) & Attacks::diag_a1_lower_mask[sq];
   }

   FORCEINLINE Bitboard diagAttacksA8Upper(Square sq) const {
     return bishopAttacks(sq) & Attacks::diag_a8_upper_mask[sq];
   }

   FORCEINLINE Bitboard diagAttacksA8Lower(Square sq) const {
     return bishopAttacks(sq) & Attacks::diag_a8_lower_mask[sq];
   }

   // Return true if previous move was legal (did not involve a move into
   // check)
   int wasLegal(Move lastMove) const;

   int isPinned(ColorType kingColor, Piece p, Square source,Square dest) const;

   int isPinned(ColorType kingColor, Move m) const {
     return isPinned(kingColor,contents[StartSquare(m)],StartSquare(m),DestSquare(m));
   }

   FORCEINLINE int isPinned(ColorType kingColor, Square source, Square pinned,
		 Square kingSquare) const {
      int dir = Attacks::directions[source][pinned];
      if (dir == 0 || dir != Attacks::directions[pinned][kingSquare]) return 0;
      return clear(pinned,kingSquare);
   }

   // Get repetition count, stop if "target" count reached
   int repCount(int target = 2) const;

   // Return true if current position is material draw
   int materialDraw() const;

   static const Bitboard white_squares, black_squares;

   // flip the board position up/down and change side to move
   void flip();

   // flip board position right/left
   void flip2();

   friend istream & operator >> (istream &i, Board &board);
   friend ostream & operator << (ostream &o, const Board &board);

   private:

   static const int RepListSize = 1024;
           
   ALIGN_VAR(16) Piece contents[64];
   Square kingPos[2];
   Material material[2];

public:
   ColorType side; // side to move
   BoardState state;
   hash_t pawnHashCodeW, pawnHashCodeB;

   Bitboard pawn_bits[2];
   Bitboard knight_bits[2];
   Bitboard bishop_bits[2];
   Bitboard rook_bits[2];
   Bitboard queen_bits[2];
   Bitboard occupied[2];
   Bitboard allOccupied;

   Bitboard allPawns() const {
       return pawn_bits[White] | pawn_bits[Black];
   }

   hash_t repList[RepListSize]; //  move history for repetition detection
   hash_t *repListHead; // head of history list

private:

   static void setupInitialBoard();

   // calculate the check status
   CheckStatusType getCheckStatus() const;

   void undoCastling(Square kp, Square oldkingsq,
           Square newrooksq, Square oldrooksq);

   void setAll(ColorType color, Square sq) {
     allOccupied.set(sq);
     occupied[color].set(sq);
   } 

   void clearAll(ColorType color, Square sq) {
     allOccupied.clear(sq);
     occupied[color].clear(sq);
   } 
           
};

#endif

