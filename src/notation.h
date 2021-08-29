// Copyright 1994, 1995, 2008, 2009, 2012, 2013, 2017-8 by Jon Dart.
// All Rights Reserved.

#ifndef _NOTATION_H
#define _NOTATION_H

#include "chess.h"
#include <iostream>

class Board;

class Notation {
    // This class converts our internal move representation to
    // and from standard text formats

 public:

    // WB is Winboard coordinate format
    enum class OutputFormat {SAN, WB, UCI};
	
    // WB is old Winboard coordinate format
    enum class InputFormat {SAN, WB, UCI};

    // Writes a string image of a move to "result".
    // "b" must be the board position before the move is made.
    static void image(const Board &b, const Move &m, OutputFormat format, std::ostream &result );

    // Same as above, but output to a string instead of stream
    static void image(const Board &b, const Move &m, OutputFormat format, std::string &result );

    // Parse "str", assuming it contains a move for side "color",
    // Attempts to be liberal about allowing deviations from SAN.
    //
    // Returns NullMove if the string cannot be parsed or if the
    // move is ambiguous.  Note: a successful return value does not
    // necessarily mean the move is legal.
    //
    // If checkLegal = false, relax even an incomplete legality check
    // (allows move into check).
    static Move value( const Board &b, 
                       ColorType color, InputFormat format,
                       const std::string &str,
                       bool checkLegal = true);

 protected:
    static Move parseCastling(ColorType color, const std::string &moveStr);
};

#endif
