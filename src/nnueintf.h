// Copyright 2021 by Jon Dart. All Rights Reserved.
#ifndef _NNUEINTF_H
#define _NNUEINTF_H

// Interface between Arasan data types and the NNUE submodule.

#include "nnue/nnue.h"
#include "board.h"

struct NodeInfo;

struct DirtyState {
    nnue::Square from, to;
    nnue::Piece piece;

    DirtyState()
        : from(nnue::InvalidSquare), to(nnue::InvalidSquare),
          piece(nnue::EmptyPiece) {}

    DirtyState(nnue::Square f, nnue::Square t, nnue::Piece p)
        : from(f), to(t), piece(p) {}
};

struct Position {
   Position(const Board &b, NodeInfo *n) :
      board(b),node(n) {
   }
   const Board &board;
   NodeInfo *node;
};

// Wrapper over the Position class that exposes a standard interface
class ChessInterface {

  public:
    ChessInterface(Position *p) : pos(p), stm(static_cast<nnue::Color>(p->board.sideToMove())),
                                              nodeIndex(0) {}

    ChessInterface(const ChessInterface &intf) : pos(intf.pos), stm(intf.stm), nodeIndex(intf.nodeIndex) {}

    virtual ~ChessInterface() = default;

    friend bool operator==(const ChessInterface &intf,
                           const ChessInterface &other);

    friend bool operator!=(const ChessInterface &intf,
                           const ChessInterface &other);

    // return the side to move
    nnue::Color sideToMove() const noexcept { return stm; }

    // Return the King square for the specified side
    nnue::Square kingSquare(nnue::Color side) const noexcept {
        return static_cast<nnue::Square>(pos->board.kingSquare(static_cast<ColorType>(side)));
    }

    // Return the associated accumulator for the current position
    nnue::Network::AccumulatorType &getAccumulator() const noexcept;

    unsigned getDirtyNum() const;

    void setDirtyNum(unsigned num);

    void getDirtyState(size_t index, nnue::Square &from,
                       nnue::Square &to, nnue::Piece &p) const;
  
    // Iterator returning std::pair<Square, Piece>
    auto begin() const noexcept { return Iterator(pos); }

    // Iterator in end state
    auto end() const noexcept { return Iterator(); }

    unsigned pieceCount() const noexcept { return pos->board.allOccupied.bitCount(); }

    void setDirtyState(int index, nnue::Square &from, nnue::Square &to,
                       nnue::Piece &p);

    // Change the state of this interface to the previous position.
    // Returns "false" if no previous position.
    bool previous();

    // Return the current node pointer.
    NodeInfo *node() const noexcept;

    bool hasPrevious() const noexcept;

  private:
    Position *pos;
    nnue::Color stm; // side to move
    int nodeIndex;

    class Iterator {
      friend class ChessInterface;
    public:
       using iterator_category = std::forward_iterator_tag;
       using value_type = std::pair<nnue::Square,nnue::Piece>;
       using difference_type = int;
       using pointer = std::pair<nnue::Square,nnue::Piece>*;
       using reference = std::pair<nnue::Square,nnue::Piece>&;
       Iterator(const Position *p) :
         pos(p),bits(p->board.allOccupied) {
         next();
       }
       Iterator(const Iterator &) = default;
       virtual ~Iterator() = default;
       Iterator& operator ++() {
         next();
         return *this;
       };
       Iterator operator ++(int) {
         Iterator retval = *this;
         ++(*this);
         return retval;
       }
       bool operator==(const Iterator &other) const {return current == other.current;}
       bool operator!=(const Iterator &other) const {return !(*this == other);}
       value_type operator*() {return current;}
       inline constexpr auto empty() const {return bits.data == 0ULL;}

       Iterator end() {
          return Iterator();
       }
    private:
       // construct empty iterator
       Iterator() : pos(nullptr), current(nnue::InvalidSquare,nnue::EmptyPiece) {
       }
       void next() {
         Square sq;
         if (bits.iterate(sq)) {
           current = value_type(static_cast<nnue::Square>(sq),
                                 static_cast<nnue::Piece>(this->pos->board[sq]));
         }
         else {
           current = empty_val;
         }
       }
       const Position *pos;
       Bitboard bits;
       static value_type empty_val;
       value_type current;
    };
};

inline bool operator==(const ChessInterface &intf,
                       const ChessInterface &other) {
    return intf.pos == other.pos && intf.nodeIndex == other.nodeIndex;
}

inline bool operator!=(const ChessInterface &intf,
                       const ChessInterface &other) {
    return intf.pos != other.pos || intf.nodeIndex != other.nodeIndex;;
}

#endif
