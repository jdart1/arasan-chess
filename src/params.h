// Copyright 2015 by Jon Dart. All Rights Reserved.
//
#ifndef _PARAMS_H
#define _PARAMS_H

#ifdef TUNE
// scoring parameters that are not const, so can be
// modified during tuning
#define PARAM_MOD
#else
// parameters are const so can be optimized better
#define PARAM_MOD const
#endif

  struct Params {
    
    static const int PARAM_ARRAY_SIZE = 79;

    // These have a 1-1 mapping to the tuning parameters
    static PARAM_MOD int params[PARAM_ARRAY_SIZE];

    // The following tables are computed from tuning parameters.
    static PARAM_MOD int MATERIAL_SCALE[32];
    static PARAM_MOD int MIDGAME_THRESHOLD;
    static PARAM_MOD int ENDGAME_THRESHOLD;
    static PARAM_MOD int KNIGHT_PST[2][64];
    static PARAM_MOD int BISHOP_PST[2][64];
    static PARAM_MOD int KNIGHT_OUTPOST[64];
    static PARAM_MOD int BISHOP_OUTPOST[64];
    // TBD: rook PST
    static PARAM_MOD int TRADE_DOWN[16];
    static PARAM_MOD int KING_PST[2][64];
    static PARAM_MOD int KNIGHT_MOBILITY[9];
    static PARAM_MOD int BISHOP_MOBILITY[15];
    static PARAM_MOD int ROOK_MOBILITY[15];
    static PARAM_MOD int QUEEN_MOBILITY[2][29];
    static PARAM_MOD int KING_MOBILITY_ENDGAME[9];
    static PARAM_MOD int PASSED_PAWN[2][8];
    static PARAM_MOD int POTENTIAL_PASSER[2][8];
    static PARAM_MOD int CONNECTED_PASSERS[2][8];
    static PARAM_MOD int ADJACENT_PASSERS[2][8];
    static PARAM_MOD int DOUBLED_PAWNS[2][8];
    static PARAM_MOD int ISOLATED_PAWN[2][8];

#ifdef TUNE
    static void write(ostream &);
#endif
  };

#endif
