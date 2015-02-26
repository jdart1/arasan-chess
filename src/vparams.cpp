// Copyright 2015 by Jon Dart. All Rights Reserved.
//
// definitions of scoring parameters, non-const for tuning

// These have a 1-1 mapping to the tuning parameters
int Scoring::Params::params[Scoring::Params::PARAM_ARRAY_SIZE];

// The following tables are computed from tuning parameters.
int Scoring::Params::MATERIAL_SCALE[32];
int Scoring::Params::MIDGAME_THRESHOLD = 22;
int Scoring::Params::ENDGAME_THRESHOLD;
int Scoring::Params::KNIGHT_PST[2][64];
int Scoring::Params::BISHOP_PST[2][64];
int Scoring::Params::KNIGHT_OUTPOST[64];
int Scoring::Params::BISHOP_OUTPOST[64];
// TBD: rook PST
int Scoring::Params::KING_PST[2][64];
int Scoring::Params::KNIGHT_MOBILITY[9];
int Scoring::Params::BISHOP_MOBILITY[15];
int Scoring::Params::ROOK_MOBILITY[15];
int Scoring::Params::QUEEN_MOBILITY[2][29];
int Scoring::Params::KING_MOBILITY_ENDGAME[9];
int Scoring::Params::PASSED_PAWN[2][8];
int Scoring::Params::POTENTIAL_PASSER[2][8];
int Scoring::Params::CONNECTED_PASSERS[2][8];
int Scoring::Params::ADJACENT_PASSERS[2][8];
int Scoring::Params::DOUBLED_PAWNS[2][8];
int Scoring::Params::ISOLATED_PAWN[2][8];

