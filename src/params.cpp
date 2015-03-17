// Copyright 2015 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {250, 75, -75, -250};
const int Scoring::Params::RBN_ADJUST[4] = {500, 675, 825, 1000};
const int Scoring::Params::QR_ADJUST[4] = {-500, 0, 500, 500};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2400, -1500};
const int Scoring::Params::PAWN_TRADE[3] = {-450, -250, -100};
const int Scoring::Params::CASTLING[6] = {0, -70, -100, 280, 200, -280};
const int Scoring::Params::KING_COVER[5] = {220, 310, 120, 30, 20};
const int Scoring::Params::KING_FILE_OPEN = -150;
const int Scoring::Params::KING_DISTANCE_BASIS = 320;
const int Scoring::Params::KING_DISTANCE_MULT = 80;
const int Scoring::Params::PIN_MULTIPLIER_MID = 200;
const int Scoring::Params::PIN_MULTIPLIER_END = 300;
const int Scoring::Params::KING_ATTACK_PARAM1 = 500;
const int Scoring::Params::KING_ATTACK_PARAM2 = 320;
const int Scoring::Params::KING_ATTACK_PARAM3 = 1500;
const int Scoring::Params::KING_ATTACK_BOOST_THRESHOLD = 480;
const int Scoring::Params::KING_ATTACK_BOOST_DIVISOR = 500;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 420;
const int Scoring::Params::BISHOP_PAIR_END = 550;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -160;
const int Scoring::Params::BAD_BISHOP_MID = -40;
const int Scoring::Params::BAD_BISHOP_END = -60;
const int Scoring::Params::OUTPOST_NOT_DEFENDED = 42;
const int Scoring::Params::CENTER_PAWN_BLOCK = -120;
const int Scoring::Params::OUTSIDE_PASSER_MID = 120;
const int Scoring::Params::OUTSIDE_PASSER_END = 250;
const int Scoring::Params::WEAK_PAWN_MID = -80;
const int Scoring::Params::WEAK_PAWN_END = -80;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -100;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -100;
const int Scoring::Params::SPACE = 20;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 30;
const int Scoring::Params::ROOK_ON_7TH_MID = 260;
const int Scoring::Params::ROOK_ON_7TH_END = 260;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 570;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 660;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 200;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 200;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_MID = 100;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_END = 100;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 50;
const int Scoring::Params::ROOK_BEHIND_PP_END = 100;
const int Scoring::Params::QUEEN_OUT = -60;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_MID = -20;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_END = -50;

const int Scoring::Params::PP_BLOCK_BASE_MID = 140;
const int Scoring::Params::PP_BLOCK_BASE_END = 140;
const int Scoring::Params::PP_BLOCK_MULT_MID = 72;
const int Scoring::Params::PP_BLOCK_MULT_END = 32;
const int Scoring::Params::ENDGAME_PAWN_BONUS = 120;
const int Scoring::Params::KING_NEAR_PASSER = 200;
const int Scoring::Params::OPP_KING_NEAR_PASSER = -280;
const int Scoring::Params::PAWN_SIDE_BONUS = 280;
const int Scoring::Params::SUPPORTED_PASSER6 = 380;
const int Scoring::Params::SUPPORTED_PASSER7 = 760;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -100;

const int Scoring::Params::TRADE_DOWN[16] = {682, 640, 409, 597, 516, 469, 426, 384, 341, 298, 256, 213, 171, 128, 85, 42};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 60, 110, 180, 270, 560, 1110}, {0, 0, 90, 160, 280, 420, 840, 1410}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 20, 40, 60, 90, 200, 0}, {0, 0, 30, 60, 100, 150, 300, 0}};
const int Scoring::Params::CONNECTED_PASSERS[2][8] = {{0, 0, 0, 100, 190, 240, 480, 830}, {0, 0, 0, 100, 190, 240, 480, 830}};
const int Scoring::Params::ADJACENT_PASSERS[2][8] = {{0, 0, 0, 80, 150, 170, 340, 700},{0, 0, 0, 80, 150, 170, 340, 700}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-60, -80, -100, -100, -100, -100, -80, -60}, {-100, -130, -170, -170, -170, -170, -130, -100}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{-80, -80, -80, -80, -80, -80, -80, -80}, {-120, -120, -120, -120, -120, -120, -120, -120}};


