#ifndef _VARPARAMS_H
#define _VARPARAMS_H

// scoring parameters that are not const, so can be
// modified during tuning

  static struct Params {
    
    // These have a 1-1 mapping to the tuning parameters
    int params[PARAM_ARRAY_SIZE];

    // The following tables are computed from tuning parameters.
    int MATERIAL_SCALE[32];
    int MIDGAME_THRESHOLD = 22;
    int ENDGAME_THRESHOLD;
    int KNIGHT_PST[2][64];
    int BISHOP_PST[2][64];
    int KNIGHT_OUTPOST[64];
    int BISHOP_OUTPOST[64];
    // TBD: rook PST
    int KING_PST[2][64];
    int KNIGHT_MOBILITY[9];
    int BISHOP_MOBILITY[15];
    int ROOK_MOBILITY[15];
    int QUEEN_MOBILITY[2][29];
    int KING_MOBILITY_ENDGAME[9];
    int PASSED_PAWN[2][8];
    int POTENTIAL_PASSER[2][8];
    int CONNECTED_PASSERS[2][8];
    int ADJACENT_PASSERS[2][8];
    int DOUBLED_PAWNS[2][8];
    int ISOLATED_PAWN[2][8];

    void write(ostream &);
  } params;

#endif
