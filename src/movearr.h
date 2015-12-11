// Copyright 1997-2002, 2008, 2012 by Jon Dart. All Rights Reserved.a
//
#ifndef _MOVE_ARRAY_H
#define _MOVE_ARRAY_H

#include "arasvec.h"
#include <string>

using namespace std;

#include "board.h"

class MoveRecord
{
     // holds info on a move made during the game or during a search
     // operation.	      

     public:

     MoveRecord() 
     : my_move(NullMove),my_hashcode((hash_t)0ULL), my_ponder(false)
     {
     }

     // "board" is the board position after the move.
     MoveRecord(const Board &board, const BoardState &previous_state,
                const Move &move, const string &image, bool ponder);
	     
     const Move &move() const {
 	     return my_move;
     }
     
     hash_t hashcode() const {
	     return my_hashcode;
     }

     const BoardState &state() const {
         return my_state;
     }
     
     bool operator < (const MoveRecord &mr) const {
        return my_hashcode < mr.my_hashcode;
     }
     
     int operator == ( const MoveRecord &l ) const {
        return my_hashcode == l.my_hashcode;
     }

     int operator != ( const MoveRecord &l ) const {
        return my_hashcode != l.my_hashcode;
     }

     const string & image () const {
        return my_image;
     }     

     bool wasPonder() const {
        return my_ponder;
     }

     private:
	     
     Move my_move;	    
     hash_t my_hashcode;	    
     BoardState my_state;
     string my_image;
     bool my_ponder; // true if move was added provisionally during a ponder operation
};

class MoveArray : public ArasanVector <MoveRecord>
{
     // Maintains a list of moves made in the game so far or in
     // the search process.	     
	     
     public:	     

     MoveArray() {
     }

     // add a move to the Move_Array. "board" is the position after the
     // move is made.	     
     void add_move( const Board &board, const BoardState &previous_state,
                    const Move &emove, const string &image, bool ponder );
	     
     // remove the most recently added move to the Move_Array.
     void remove_move();
	     
     // return the total number of half-moves in the game:
     unsigned num_moves() const
     {
       return (unsigned)length();
     }
     
     unsigned num_moves(const ColorType side) const;

     // return the nth move in the Move_Array.  0 <= n <= num_moves - 1.
     const Move &move( unsigned n ) const;
	
     private:
	     
};

#endif

