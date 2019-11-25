// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
//
// definitions of scoring parameters, non-const for tuning

#include "params.h"

// These have a 1-1 mapping to the tuning parameters
score_t Params::RB_ADJUST[6];
score_t Params::RBN_ADJUST[6];
score_t Params::QR_ADJUST[5];
score_t Params::KN_VS_PAWN_ADJUST[3] = {0, -2400, -1500};
score_t Params::MINOR_FOR_PAWNS[6] = {68, 75, 115, 115, 115, 115};
score_t Params::QUEEN_VS_3MINORS[4];
score_t Params::CASTLING[6] = {0, -70, -100, 280, 200, -280};
#ifdef TUNE
score_t Params::KING_ATTACK_SCALE_MAX;
score_t Params::KING_ATTACK_SCALE_INFLECT;
score_t Params::KING_ATTACK_SCALE_FACTOR;
score_t Params::KING_ATTACK_SCALE_BIAS;
#endif
score_t Params::KING_COVER[6][4];
score_t Params::KING_COVER_BASE = -100;
score_t Params::KING_DISTANCE_BASIS = 320;
score_t Params::KING_DISTANCE_MULT = 80;
score_t Params::PIN_MULTIPLIER_MID = 200;
score_t Params::PIN_MULTIPLIER_END = 300;
score_t Params::ROOK_VS_PAWNS = 333;
score_t Params::KRMINOR_VS_R_NO_PAWNS = -500;
score_t Params::KQMINOR_VS_Q_NO_PAWNS = -500;
score_t Params::ENDGAME_PAWN_ADVANTAGE = 100;
score_t Params::PAWN_ENDGAME1 = 200;
score_t Params::PAWN_ATTACK_FACTOR = 4;
score_t Params::MINOR_ATTACK_FACTOR = 4;
score_t Params::MINOR_ATTACK_BOOST = 4;
score_t Params::ROOK_ATTACK_FACTOR = 44;
score_t Params::ROOK_ATTACK_BOOST = 8;
score_t Params::QUEEN_ATTACK_FACTOR = 52;
score_t Params::QUEEN_ATTACK_BOOST = 28;
score_t Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
score_t Params::OWN_MINOR_KING_PROXIMITY = 10;
score_t Params::OWN_ROOK_KING_PROXIMITY = 20;
score_t Params::OWN_QUEEN_KING_PROXIMITY = 10;

score_t Params::KING_ATTACK_COVER_BOOST_BASE;
score_t Params::KING_ATTACK_COVER_BOOST_SLOPE;
score_t Params::KING_ATTACK_COUNT = 6;
score_t Params::KING_ATTACK_SQUARES = 6;
score_t Params::PAWN_PUSH_THREAT_MID = 20;
score_t Params::PAWN_PUSH_THREAT_END = 20;
score_t Params::ENDGAME_KING_THREAT = -50;
score_t Params::BISHOP_TRAPPED = -1470;
score_t Params::BISHOP_PAIR_MID = 420;
score_t Params::BISHOP_PAIR_END = 550;
score_t Params::BISHOP_PAWN_PLACEMENT_END = -160;
score_t Params::BAD_BISHOP_MID = -40;
score_t Params::BAD_BISHOP_END = -60;
score_t Params::CENTER_PAWN_BLOCK = -120;
score_t Params::OUTSIDE_PASSER_MID = 120;
score_t Params::OUTSIDE_PASSER_END = 250;
score_t Params::WEAK_PAWN_MID = -80;
score_t Params::WEAK_PAWN_END = -80;
score_t Params::WEAK_ON_OPEN_FILE_MID = -100;
score_t Params::WEAK_ON_OPEN_FILE_END = -100;
score_t Params::SPACE = 20;
score_t Params::PAWN_CENTER_SCORE_MID = 30;
score_t Params::ROOK_ON_7TH_MID = 260;
score_t Params::ROOK_ON_7TH_END = 260;
score_t Params::TWO_ROOKS_ON_7TH_MID = 570;
score_t Params::TWO_ROOKS_ON_7TH_END = 660;
score_t Params::ROOK_ON_OPEN_FILE_MID = 200;
score_t Params::ROOK_ON_OPEN_FILE_END = 200;
score_t Params::ROOK_BEHIND_PP_MID = 50;
score_t Params::ROOK_BEHIND_PP_END = 100;
score_t Params::QUEEN_OUT = -60;
score_t Params::PAWN_SIDE_BONUS = 306;
score_t Params::KING_OWN_PAWN_DISTANCE = 50;
score_t Params::KING_OPP_PAWN_DISTANCE = 20;
score_t Params::SIDE_PROTECTED_PAWN = -92;
score_t Params::KING_OPP_PASSER_DISTANCE[6] = {10,20,30,40,50,60};
score_t Params::KNIGHT_PST[2][64];
score_t Params::BISHOP_PST[2][64];
score_t Params::ROOK_PST[2][64];
score_t Params::QUEEN_PST[2][64];
score_t Params::KING_PST[2][64];

// The following tables are computed from tuning parameters.
score_t Params::KING_POSITION_LOW_MATERIAL[3];
score_t Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE];
score_t Params::OWN_PIECE_KING_PROXIMITY_MULT[16];
score_t Params::PASSED_PAWN[2][8];
score_t Params::PASSED_PAWN_FILE_ADJUST[8] = {0,0,0,0,0,0,0,0};
score_t Params::POTENTIAL_PASSER[2][8];
score_t Params::CONNECTED_PASSER[2][8];
score_t Params::ADJACENT_PASSER[2][8];
score_t Params::QUEENING_PATH_CLEAR[2][6];
score_t Params::PP_OWN_PIECE_BLOCK[2][3];
score_t Params::PP_OPP_PIECE_BLOCK[2][3];
score_t Params::QUEENING_PATH_CONTROL[2][3];
score_t Params::QUEENING_PATH_OPP_CONTROL[2][3];
score_t Params::DOUBLED_PAWNS[2][8];
score_t Params::TRIPLED_PAWNS[2][8];
score_t Params::ISOLATED_PAWN[2][8];
score_t Params::THREAT_BY_PAWN[2][5];
score_t Params::THREAT_BY_KNIGHT[2][5];
score_t Params::THREAT_BY_BISHOP[2][5];
score_t Params::THREAT_BY_ROOK[2][5];
score_t Params::KNIGHT_OUTPOST[2][2];
score_t Params::BISHOP_OUTPOST[2][2];
score_t Params::KNIGHT_MOBILITY[9];
score_t Params::BISHOP_MOBILITY[15];
score_t Params::ROOK_MOBILITY[2][15];
score_t Params::QUEEN_MOBILITY[2][24];
score_t Params::KING_MOBILITY_ENDGAME[5];
score_t Params::PAWN_STORM[2][4][5];

