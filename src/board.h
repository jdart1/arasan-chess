// Copyright 1994-2000, 2004, 2005, 2013, 2015, 2019-2021 by Jon Dart.
// All Rights Reserved.

#include "types.h"
#include "chess.h"

#ifndef _BOARD_H
#define _BOARD_H

#include "bitboard.h"
#include "attacks.h"
#include "material.h"

struct NodeInfo;

class Board;

extern const hash_t rep_codes[3];

enum CastleType { CanCastleEitherSide,
                  CanCastleKSide,
                  CanCastleQSide,
                  CantCastleEitherSide};

enum CheckStatusType { NotInCheck, InCheck, CheckUnknown };

struct BoardState {
   hash_t hashCode;
   Square enPassantSq;
   int moveCount;
   int movesFromNull;
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

   // Global one-time initialization
   static void init();

   static void cleanup(); 

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

   CastleType castleStatus( ColorType c ) const
   {
       return state.castleStatus[c];
   }

   void setCastleStatus( CastleType t, ColorType side );

   // True if either side can castle
   bool castlingPossible() const
   {
      return castleStatus(White)<3 || castleStatus(Black)<3;
   }

   // True if side can castle
   bool canCastle(ColorType) const
   {
      return castleStatus(side)<3;
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

   const Material &getMaterial( ColorType c ) const
   {
       return material[c];
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

   // returns InCheck if the previous move caused check to the opponent
   // (call after the move is made).
   CheckStatusType checkStatus(Move lastMove) const;

   // returns InCheck if the move could cause check to the opponent
   // after being made (call before the move is made).
   CheckStatusType wouldCheck(Move lastMove) const;

   void setCheckStatus(CheckStatusType s) {
       state.checkStatus = s;
   }

   Square kingSquare(ColorType c) const {
        return kingPos[c];
   }

   hash_t hashCode() const {
       return state.hashCode;
   }

   // returns a hash code factoring in the position repetition count
   hash_t hashCode(int rep_count) const {
       return state.hashCode ^ rep_codes[rep_count];
   }

   // returns what hash code will be after move
   hash_t hashCode( Move m ) const;

   int operator == ( const Board &b ) {
       return state.hashCode == b.hashCode();
   }

   // set a bitmap of squares that lie in a straight line between
   // sq1 and sq2
   void between( Square sq1, Square sq2, Bitboard &result) const {
      result = Attacks::betweenSquares[sq1][sq2];
   }

   // returns "true" if there is a clear path between sq1 and sq2
   int clear( Square sq1, Square sq2 ) const {
      return Attacks::directions[sq1][sq2] &&
         Bitboard(allOccupied & Attacks::betweenSquares[sq1][sq2]).isClear();
   }

   // Makes a move. If a node pointer is passed, it is updated with change
   // info needed for incremental calc of NNUE.
   void doMove(Move m, NodeInfo * node = nullptr);

   // Makes a null move
   void doNull( NodeInfo *node = nullptr );

   // Undoes a previous move.
   void undoMove( Move rmove, const BoardState &stat );

   // Undoes a previous null move.
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

   // Return a bitboard of all squares attacked by 'side'
   Bitboard allAttacks(ColorType side) const;

   // Return a bit vector of all pieces of color "side" that attack "sq".
   Bitboard calcAttacks(Square sq, ColorType side) const;

   // Return a bit vector of all pieces of color "side" that can move to
   // unoccupied "sq".
   Bitboard calcBlocks(Square sq, ColorType side) const;

   // Return location of a piece of color "side" that attacks "square" in
   // direction "dir" (InvalidSquare if no such piece).
   Square getDirectionalAttack(Square sq, int dir, ColorType side) const;

   // Return all squares attacked by pawns of color "side".
   Bitboard allPawnAttacks(ColorType side) const {
       return allPawnAttacks(side,pawn_bits[side]);
   }

   // Return all squares attacked by pawns on the specified bit board
   Bitboard allPawnAttacks(ColorType side, Bitboard pawns) const;

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
   int wasLegal(Move lastMove, bool evastion = false) const;

   // True if moving from 'source' to 'dest' uncovers an attack by 'side' on
   // 'target'
   int discoversAttack(Square source, Square dest, Square target, ColorType side) const;

   // Return true if moving a piece from "source" to "dest" would be
   // prohibited due to a pin
   int isPinned(ColorType kingColor, Square source, Square dest) const {
     return discoversAttack(source,dest,kingSquare(kingColor),OppositeColor(kingColor));
   }

   // Return true if Move is illegal due to a pin
   int isPinned(ColorType kingColor, Move m) const {
     return isPinned(kingColor,StartSquare(m),DestSquare(m));
   }

   // True if the Rook or Queen on "sq" pins a Knight or Bishop to the opposing
   // king. okp is the opposining king square, oside is the opposing color.
   int pinOnRankOrFile(Square sq, Square okp, ColorType oside) const {
     if (Attacks::rank_mask[Rank<White>(sq)-1].isSet(okp)) {
        Bitboard kb(knight_bits[oside] | bishop_bits[oside]);
        if (Attacks::rank_mask[Rank<White>(sq)-1] & kb) {
          return pinCheck(sq,okp,kb);
        }
      }
     else if (Attacks::file_mask[File(sq)-1].isSet(okp)) {
        Bitboard kb(knight_bits[oside] | bishop_bits[oside]);
        if (Attacks::file_mask[File(sq)-1] & kb) {
          return pinCheck(sq,okp,kb);
        }
      }
      return 0;
   }

   // True if the Bishop or Queen on "sq" pins a Knight or Rook to the opposing
   // king. okp is the opposing king square, oside is the opposing color.
   int pinOnDiag(Square sq, Square okp, ColorType oside) const {
      if (Attacks::diag_a1_mask[sq].isSet(okp)) {
         // might be a pin
         Bitboard kr(knight_bits[oside] | rook_bits[oside]);
         if (Attacks::diag_a1_mask[sq] & kr) {
           return pinCheck(sq,okp,kr);
         }
      }
      else if (Attacks::diag_a8_mask[sq].isSet(okp)) {
         // might be a pin
         Bitboard kr(knight_bits[oside] | rook_bits[oside]);
         if (Attacks::diag_a8_mask[sq] & kr) {
           return pinCheck(sq,okp,kr);
         }
      }
      return 0;
   }

   // Return a bitboard of all pieces that are pinned (have attacker
   // of "pnnerSide" behind them that would attack "ksq" if the piece were
   // moved). If "pinnedSide" is the same side as pinnerSide then these are
   // pieces that could generate discovered check.
   Bitboard getPinned(Square ksq, ColorType pinnerSide, ColorType pinnedSide) const;

   // Get repetition count, stop if "target" count reached
   int repCount(int target = 2) const noexcept;

   // Return true if there has been a repetition since the last
   // move that reset the 50-move counter.
   bool anyRep() const noexcept;

   // return true if legal draw by repetition (3 repetitions)
   bool repetitionDraw() const noexcept {
      return repCount(2)==2;
   }

   // Return true if draw by 50-move rule
   bool fiftyMoveDraw() const noexcept;

   // Return true if current position is legal draw by insufficient material
   bool materialDraw() const noexcept;

   bool isLegalDraw() const noexcept  {
      return repetitionDraw() || materialDraw() || fiftyMoveDraw();
   }

   static const Bitboard white_squares, black_squares;

   // flip the board position up/down and change side to move
   void flip();

   friend std::istream & operator >> (std::istream &i, Board &board);
   friend std::ostream & operator << (std::ostream &o, const Board &board);

   // calculate the check status
   CheckStatusType getCheckStatus() const;

   int  men() const noexcept {
      return getMaterial(White).men() + getMaterial(Black).men();
   }

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

   int pinCheck(Square pinner, Square okp, const Bitboard &mask) const {
     Bitboard btwn;
     between(pinner,okp,btwn);
     Bitboard pin(btwn & mask);
     if (pin.singleBitSet()) {
       if ((btwn & allOccupied) == pin) {
         return 1;
       }
     }
     return 0;
   }

   int discAttackDiag(Square sq, Square ksq, ColorType side) const;

   int discAttackRankFile(Square sq, Square ksq, ColorType side) const;

   int discAttack(Square sq, Square ksq, ColorType side) const;

};

#endif

