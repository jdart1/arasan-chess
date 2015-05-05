// Copyright 2015 by Jon Dart. All Rights Reserved.
//
// definitions of scoring parameters, non-const for tuning

// These have a 1-1 mapping to the tuning parameters
int Scoring::Params::RB_ADJUST[4] = {250, 75, -75, -250};
int Scoring::Params::RBN_ADJUST[4] = {500, 675, 825, 1000};
int Scoring::Params::QR_ADJUST[4] = {-500, 0, 500, 500};
int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2400, -1500};
int Scoring::Params::PAWN_TRADE[3] = {-450, -250, -100};
int Scoring::Params::CASTLING[6] = {0, -70, -100, 280, 200, -280};
int Scoring::Params::KING_COVER[5] = {220, 310, 120, 30, 20};
int Scoring::Params::KING_FILE_OPEN = -150;
int Scoring::Params::KING_DISTANCE_BASIS = 320;
int Scoring::Params::KING_DISTANCE_MULT = 80;
int Scoring::Params::PIN_MULTIPLIER_MID = 200;
int Scoring::Params::PIN_MULTIPLIER_END = 300;
int Scoring::Params::KING_ATTACK_PARAM1 = 500;
int Scoring::Params::KING_ATTACK_PARAM2 = 0;
int Scoring::Params::KING_ATTACK_PARAM3 = 0;
int Scoring::Params::KING_ATTACK_PARAM4 = 0;
int Scoring::Params::KING_ATTACK_PARAM5 = 375;
int Scoring::Params::KING_ATTACK_PARAM6 = 0;
int Scoring::Params::KING_ATTACK_PARAM7 = 320;
int Scoring::Params::KING_ATTACK_PARAM8 = 0;
int Scoring::Params::KING_ATTACK_PARAM9 = 0;
int Scoring::Params::KING_ATTACK_BOOST_THRESHOLD = 480;
int Scoring::Params::KING_ATTACK_BOOST_DIVISOR = 500;
int Scoring::Params::BISHOP_TRAPPED = -1470;
int Scoring::Params::BISHOP_PAIR_MID = 420;
int Scoring::Params::BISHOP_PAIR_END = 550;
int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -160;
int Scoring::Params::BAD_BISHOP_MID = -40;
int Scoring::Params::BAD_BISHOP_END = -60;
int Scoring::Params::OUTPOST_NOT_DEFENDED = 42;
int Scoring::Params::CENTER_PAWN_BLOCK = -120;
int Scoring::Params::OUTSIDE_PASSER_MID = 120;
int Scoring::Params::OUTSIDE_PASSER_END = 250;
int Scoring::Params::WEAK_PAWN_MID = -80;
int Scoring::Params::WEAK_PAWN_END = -80;
int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -100;
int Scoring::Params::WEAK_ON_OPEN_FILE_END = -100;
int Scoring::Params::SPACE = 20;
int Scoring::Params::PAWN_CENTER_SCORE_MID = 30;
int Scoring::Params::ROOK_ON_7TH_MID = 260;
int Scoring::Params::ROOK_ON_7TH_END = 260;
int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 570;
int Scoring::Params::TWO_ROOKS_ON_7TH_END = 660;
int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 200;
int Scoring::Params::ROOK_ON_OPEN_FILE_END = 200;
int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_MID = 100;
int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_END = 100;
int Scoring::Params::ROOK_BEHIND_PP_MID = 50;
int Scoring::Params::ROOK_BEHIND_PP_END = 100;
int Scoring::Params::QUEEN_OUT = -60;
int Scoring::Params::PASSER_OWN_PIECE_BLOCK_MID = -20;
int Scoring::Params::PASSER_OWN_PIECE_BLOCK_END = -50;
int Scoring::Params::PP_BLOCK_BASE_MID = 140;
int Scoring::Params::PP_BLOCK_BASE_END = 140;
int Scoring::Params::PP_BLOCK_MULT_MID = 90;
int Scoring::Params::PP_BLOCK_MULT_END = 40;
int Scoring::Params::ENDGAME_PAWN_BONUS = 120;
int Scoring::Params::KING_NEAR_PASSER = 200;
int Scoring::Params::OPP_KING_NEAR_PASSER = -280;
int Scoring::Params::PAWN_SIDE_BONUS = 280;
int Scoring::Params::SUPPORTED_PASSER6 = 380;
int Scoring::Params::SUPPORTED_PASSER7 = 760;
int Scoring::Params::SIDE_PROTECTED_PAWN = -100;

// The following tables are computed from tuning parameters.
int Scoring::Params::KING_ATTACK_SCALE[512];
int Scoring::Params::TRADE_DOWN[16];
/*
// not tuned presently
int Scoring::Params::KNIGHT_PST[2][64];
int Scoring::Params::BISHOP_PST[2][64];
int Scoring::Params::KNIGHT_OUTPOST[64];
int Scoring::Params::BISHOP_OUTPOST[64];
// TBD: rook PST
int Scoring::Params::KING_PST[2][64];
int Scoring::Params::KNIGHT_MOBILITY[9];
int Scoring::Params::BISHOP_MOBILITY[15];
int Scoring::Params::ROOK_MOBILITY[2][15];
int Scoring::Params::QUEEN_MOBILITY[2][29];
int Scoring::Params::KING_MOBILITY_ENDGAME[9];
*/
int Scoring::Params::PASSED_PAWN[2][8];
int Scoring::Params::POTENTIAL_PASSER[2][8];
int Scoring::Params::CONNECTED_PASSERS[2][8];
int Scoring::Params::ADJACENT_PASSERS[2][8];
int Scoring::Params::DOUBLED_PAWNS[2][8];
int Scoring::Params::ISOLATED_PAWN[2][8];

