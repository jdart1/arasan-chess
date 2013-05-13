// Copyright 1994, 1995, 2008, 2009, 2012 by Jon Dart.  All Rights Reserved.

#ifndef _NOTATION_H
#define _NOTATION_H

#include "chess.h"
#include <iostream>
using namespace std;

class Board;

class Notation
{
	// This class converts our internal move representation to
	// and from standard algebraic notation.
	
public:
        // Create a Move from a SAN string
        static Move moveValue( const Board &board, const string &str, ColorType color );

	// Writes a human-readable (SAN) string image of a move to "result".
	// "b" must be the board position before the move is made.
	static void image(const Board &b, const Move &m, ostream &result );

	static void image(const Board &b, const Move &m, string &result );

        // Outputs a move in UCI format to stream 'image'
        static void UCIMoveImage(const Move &move, ostream &image);

	// Parse "str", assuming it contains a move for side "color",
	// in the same string format returned by image().  Attempts
        // to be liberal about allowing deviations from SAN.
        //
        // Returns NullMove if the string cannot be parsed or if the
        // move is ambiguous.  Note: a successful return value does not
	// necessarily mean the move is legal.
	static Move value( const Board &b,
		ColorType color, const char *str);
};

#endif
