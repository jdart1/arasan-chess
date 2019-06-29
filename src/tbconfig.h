/*
 * tbconfig.h
 * (C) 2015 basil, all rights reserved,
 * Modifications Copyright 2016 Jon Dart
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

// This is the Arasan customized version of tbconfig.h from the Fathom
// library. Because it is ahead of the library version in the include
// path, it overrides the library version.
//
#ifndef TBCONFIG_H
#define TBCONFIG_H

#include "types.h"
#include "bitboard.h"
#include "constant.h"

/****************************************************************************/
/* BUILD CONFIG:                                                            */
/****************************************************************************/

/*
 * Define TB_CUSTOM_POP_COUNT to override the internal popcount
 * implementation. To do this supply a macro or function definition
 * here:
 */
#define TB_CUSTOM_POP_COUNT(x) (unsigned)Bitboard(x).bitCount()

/*
 * Define TB_CUSTOM_LSB to override the internal lsb
 * implementation. To do this supply a macro or function definition
 * here:
 */
#define TB_CUSTOM_LSB(x) (unsigned)Bitboard(x).firstOne()

/*
 * Define TB_CUSTOM_BSWAP32 to override the internal bswap32
 * implementation. To do this supply a macro or function definition
 * here:
 */
#define TB_CUSTOM_BSWAP32(x) bswap32(x)

/*
 * Define TB_CUSTOM_BSWAP64 to override the internal bswap32
 * implementation. To do this supply a macro or function definition
 * here:
 */
#define TB_CUSTOM_BSWAP64(x) bswap64(x)

/* Define TB_NO_STDINT if you do not want to use <stdint.h> or it is not
 * available. */
/* #define TB_NO_STDINT */

/*
 * Define TB_NO_STDBOOL if you do not want to use <stdbool.h> or it is not
 * available or unnecessary (e.g. C++).
 */
#ifdef _WIN32
#define TB_NO_STDBOOL
#endif

/*
 * Define TB_NO_THREADS if your program is not multi-threaded.
 */
/* #define TB_NO_THREADS */

/*
 * Define TB_NO_HELPER_API if you do not need the helper API.
 */
#define TB_NO_HELPER_API

/*
 * If no custom popcount function is defined, and if the following
 * define is not set, the code will attempt to use an available hardware
 * popcnt (currently supported on x86_64 architecture only) and otherwise
 * will fall back to a software implementation.
 */
//#define TB_NO_HW_POP_COUNT

/**
 * Define TB_USE_ATOMIC to use C++ 11 (or higher) <atomic> feature 
 * (recommended if using C++ and compiler supports it).
 */
#define TB_USE_ATOMIC

/***************************************************************************/
/* SCORING CONSTANTS                                                       */
/***************************************************************************/
/*
 * Fathom can produce scores for tablebase moves. These depend on the
 * value of a pawn, and the magnitude of mate scores. The following
 * constants are representative values but will likely need
 * modification to adapt to an engine's own internal score values.
 */
#define TB_VALUE_PAWN 128  /* value of pawn in endgame */
#define TB_VALUE_MATE Constants::MATE
#define TB_VALUE_INFINITE 32767 /* value above all normal score values */
#define TB_VALUE_DRAW 0
// Fathom returns TB_VALUE_MATE-TB_MAX_MATE_PLY-1 as a tablebase win
// score. Set the mate ply cosntant here so that equates to our
// Constants::TABLEBASE_WIN
#define TB_MAX_MATE_PLY 511

/***************************************************************************/
/* ENGINE INTEGRATION CONFIG                                               */
/***************************************************************************/

/*
 * If you are integrating tbprobe into an engine, you can replace some of
 * tbprobe's built-in functionality with that already provided by the engine.
 * This is OPTIONAL.  If no definition are provided then tbprobe will use its
 * own internal defaults.  That said, for engines it is generally a good idea
 * to avoid redundancy.
 */

/*
 * Define TB_KING_ATTACKS(square) to return the king attacks bitboard for a
 * king at `square'.
 */
/* #define TB_KING_ATTACKS(square)          <DEFINITION> */

/*
 * Define TB_KNIGHT_ATTACKS(square) to return the knight attacks bitboard for
 * a knight at `square'.
 */
/* #define TB_KNIGHT_ATTACKS(square)        <DEFINITION> */

/*
 * Define TB_ROOK_ATTACKS(square, occ) to return the rook attacks bitboard
 * for a rook at `square' assuming the given `occ' occupancy bitboard.
 */
/* #define TB_ROOK_ATTACKS(square, occ)     <DEFINITION> */

/*
 * Define TB_BISHOP_ATTACKS(square, occ) to return the bishop attacks bitboard
 * for a bishop at `square' assuming the given `occ' occupancy bitboard.
 */
/* #define TB_BISHOP_ATTACKS(square, occ)   <DEFINITION> */

/*
 * Define TB_QUEEN_ATTACKS(square, occ) to return the queen attacks bitboard
 * for a queen at `square' assuming the given `occ' occupancy bitboard.
 * NOTE: If no definition is provided then tbprobe will use:
 *       TB_ROOK_ATTACKS(square, occ) | TB_BISHOP_ATTACKS(square, occ)
 */
/* #define TB_QUEEN_ATTACKS(square, occ)    <DEFINITION> */

/*
 * Define TB_PAWN_ATTACKS(square, color) to return the pawn attacks bitboard
 * for a `color' pawn at `square'.
 * NOTE: This definition must work for pawns on ranks 1 and 8.  For example,
 *       a white pawn on e1 attacks d2 and f2.  A black pawn on e1 attacks
 *       nothing.  Etc.
 * NOTE: This definition must not include en passant captures.
 */
/* #define TB_PAWN_ATTACKS(square, color)   <DEFINITION> */

#endif
