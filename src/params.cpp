// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 21-Nov-2018 08:10:30 by tuner -c 24 -R 25 -n 150 /home/jdart/chess/data/epd/big4.epd
// Final objective value: 0.0703616

//

#include "params.h"

const int Params::RB_ADJUST[6] = {68, -6, 102, 55, 46, 85};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -19, -40, -32};
const int Params::QR_ADJUST[5] = {12, 57, 19, -52, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -298, -180};
const int Params::CASTLING[6] = {13, -38, -28, 0, 6, -23};
const int Params::KING_COVER[6][4] = {{7, 20, 11, 14},
{3, 8, 4, 5},
{-7, -21, -12, -14},
{-7, -22, -12, -15},
{-10, -28, -16, -19},
{-17, -48, -27, -33}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 94;
const int Params::PIN_MULTIPLIER_END = 92;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -196;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -130;
const int Params::MINOR_FOR_PAWNS = 64;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 33;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 6;
const int Params::PAWN_ATTACK_FACTOR2 = 8;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 43;
const int Params::ROOK_ATTACK_FACTOR = 60;
const int Params::ROOK_ATTACK_BOOST = 22;
const int Params::ROOK_ATTACK_BOOST2 = 37;
const int Params::QUEEN_ATTACK_FACTOR = 62;
const int Params::QUEEN_ATTACK_BOOST = 44;
const int Params::QUEEN_ATTACK_BOOST2 = 62;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 6;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 131;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 82;
const int Params::OWN_ROOK_KING_PROXIMITY = 33;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 38;
const int Params::PIECE_THREAT_MR_MID = 95;
const int Params::PIECE_THREAT_MQ_MID = 79;
const int Params::PIECE_THREAT_MM_END = 32;
const int Params::PIECE_THREAT_MR_END = 79;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 11;
const int Params::MINOR_PAWN_THREAT_END = 25;
const int Params::PIECE_THREAT_RM_MID = 22;
const int Params::PIECE_THREAT_RR_MID = 9;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 35;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 16;
const int Params::ROOK_PAWN_THREAT_END = 27;
const int Params::ENDGAME_KING_THREAT = 44;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 37;
const int Params::BISHOP_PAIR_END = 57;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -24;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = -1;
const int Params::WEAK_ON_OPEN_FILE_MID = -23;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 29;
const int Params::TWO_ROOKS_ON_7TH_MID = 28;
const int Params::TWO_ROOKS_ON_7TH_END = 57;
const int Params::ROOK_ON_OPEN_FILE_MID = 34;
const int Params::ROOK_ON_OPEN_FILE_END = 16;
const int Params::ROOK_BEHIND_PP_MID = 4;
const int Params::ROOK_BEHIND_PP_END = 20;
const int Params::QUEEN_OUT = -29;
const int Params::PAWN_SIDE_BONUS = 19;
const int Params::KING_OWN_PAWN_DISTANCE = 15;
const int Params::KING_OPP_PAWN_DISTANCE = 5;
const int Params::QUEENING_SQUARE_CONTROL_MID = 54;
const int Params::QUEENING_SQUARE_CONTROL_END = 61;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -17;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -34;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -5;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 26, 56, 86, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={247, 190, 137};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 20, 21, 23, 25, 26, 28, 30, 33, 35, 37, 40, 43, 45, 48, 52, 55, 58, 62, 66, 70, 74, 78, 83, 88, 93, 98, 103, 109, 114, 121, 127, 133, 140, 147, 154, 161, 169, 177, 185, 193, 201, 210, 219, 228, 237, 246, 255, 264, 274, 283, 293, 302, 312, 322, 331, 341, 350, 360, 369, 378, 387, 396, 405, 414, 422, 431, 439, 447, 454, 462, 469, 476, 483, 490, 496, 503, 509, 514, 520, 525, 531, 535, 540, 545, 549, 553, 557, 561, 564, 568, 571, 574, 577, 580, 583, 585, 588, 590, 592, 594, 596, 598, 600, 601, 603, 604, 606, 607, 608, 610, 611, 612, 613, 614, 615, 615, 616, 617, 618, 618, 619};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 0, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 38, 128, 188}, {0, 0, 2, 0, 27, 87, 160, 266}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {73, 74, 71, 64, 64, 71, 74, 73};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 17, 96, 0}, {0, 0, 0, 0, 2, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 4, 25, 102, 229, 320}, {0, 0, 0, 0, 40, 89, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 24, 9, 32, 96, 182}, {0, 0, 0, 12, 13, 25, 61, 52}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-36, -24, -37, -12, -32, -5, -22, -24, -4, -25, -13, -20, 0, -3, 0, -40, -11, -13, -19, -56, 0}, {0, 0, 0, -6, -13, 0, 0, -10, -10, -18, -20, -6, -6, -12, -8, -20, -18, -20, -55, -42, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-66, -73, -55, -52, -49, -51, -59, -56, -26, -30, -32, -47, -7, 0, -2, -33, 0, -1, -64, -34, -83}, {-11, -8, 0, -5, -3, 0, -17, -6, -9, -3, -5, -31, -13, -8, -0, -55, -23, -13, -59, -34, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-41, 0, -15, -13, -13, -15, 0, -41}, {-43, -19, -25, -19, -19, -25, -19, -43}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -34, -1, -19, -19, -1, -34, 0}, {-81, -68, -37, -33, -33, -37, -68, -81}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -0, -17, -23, -23, -17, -0, 0}, {-6, -11, -13, -19, -19, -13, -11, -6}};

const int Params::KNIGHT_PST[2][64] = {{-86, -27, -12, -6, -6, -12, -27, -86, -26, -24, -17, 6, 6, -17, -24, -26, -45, -18, 0, 27, 27, 0, -18, -45, -17, -0, 20, 22, 22, 20, -0, -17, -29, -12, 20, 16, 16, 20, -12, -29, -58, -47, -4, -1, -1, -4, -47, -58, -109, -85, -28, -27, -27, -28, -85, -109, -128, -128, -128, -128, -128, -128, -128, -128}, {-70, -30, -22, -22, -22, -22, -30, -70, -30, -19, -16, -7, -7, -16, -19, -30, -41, -21, -11, 12, 12, -11, -21, -41, -12, -2, 19, 27, 27, 19, -2, -12, -21, -14, 7, 11, 11, 7, -14, -21, -42, -22, -5, -0, -0, -5, -22, -42, -65, -39, -33, -21, -21, -33, -39, -65, -128, -66, -62, -42, -42, -62, -66, -128}};
const int Params::BISHOP_PST[2][64] = {{12, 42, 28, 27, 27, 28, 42, 12, 35, 43, 36, 28, 28, 36, 43, 35, 10, 39, 36, 30, 30, 36, 39, 10, -2, 9, 18, 39, 39, 18, 9, -2, -38, -12, -5, 24, 24, -5, -12, -38, 5, -28, -41, -29, -29, -41, -28, 5, -39, -64, -64, -102, -102, -64, -64, -39, -5, -73, -128, -128, -128, -128, -73, -5}, {-5, 10, 15, 10, 10, 15, 10, -5, 8, 16, 13, 20, 20, 13, 16, 8, 8, 20, 26, 29, 29, 26, 20, 8, 8, 18, 30, 35, 35, 30, 18, 8, -12, 3, 3, 12, 12, 3, 3, -12, 12, -3, -4, -4, -4, -4, -3, 12, 2, -3, -13, -16, -16, -13, -3, 2, 8, 7, 4, -22, -22, 4, 7, 8}};
const int Params::ROOK_PST[2][64] = {{-61, -47, -42, -39, -39, -42, -47, -61, -73, -46, -43, -37, -37, -43, -46, -73, -71, -45, -48, -45, -45, -48, -45, -71, -70, -54, -52, -43, -43, -52, -54, -70, -49, -39, -30, -22, -22, -30, -39, -49, -35, -10, -18, -4, -4, -18, -10, -35, -34, -42, -12, -10, -10, -12, -42, -34, -22, -19, -85, -47, -47, -85, -19, -22}, {-4, -7, 2, -4, -4, 2, -7, -4, -15, -12, -2, -1, -1, -2, -12, -15, -13, -7, 0, -2, -2, 0, -7, -13, -0, 9, 15, 10, 10, 15, 9, -0, 17, 20, 26, 20, 20, 26, 20, 17, 26, 30, 35, 33, 33, 35, 30, 26, 12, 17, 22, 24, 24, 22, 17, 12, 34, 35, 33, 30, 30, 33, 35, 34}};
const int Params::QUEEN_PST[2][64] = {{37, 38, 34, 45, 45, 34, 38, 37, 32, 47, 50, 52, 52, 50, 47, 32, 30, 44, 48, 39, 39, 48, 44, 30, 42, 46, 49, 49, 49, 49, 46, 42, 45, 47, 52, 60, 60, 52, 47, 45, 37, 54, 56, 56, 56, 56, 54, 37, 34, 18, 41, 42, 42, 41, 18, 34, 53, 45, 40, 19, 19, 40, 45, 53}, {-49, -68, -48, -35, -35, -48, -68, -49, -38, -40, -29, -11, -11, -29, -40, -38, -23, 7, 17, 6, 6, 17, 7, -23, 12, 29, 35, 36, 36, 35, 29, 12, 25, 54, 56, 72, 72, 56, 54, 25, 19, 54, 69, 88, 88, 69, 54, 19, 34, 54, 84, 93, 93, 84, 54, 34, 16, 33, 27, 26, 26, 27, 33, 16}};
const int Params::KING_PST[2][64] = {{49, 63, 6, 26, 26, 6, 63, 49, 86, 86, 38, 56, 56, 38, 86, 86, -5, 18, -53, -109, -109, -53, 18, -5, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 128, 73, -73, -128, -128, -73, 73, 128, -128, -128, 48, -128, -128, 48, -128, -128}, {-82, -83, -86, -92, -92, -86, -83, -82, -68, -76, -83, -81, -81, -83, -76, -68, -79, -82, -78, -76, -76, -78, -82, -79, -75, -70, -63, -61, -61, -63, -70, -75, -58, -47, -40, -40, -40, -40, -47, -58, -47, -32, -28, -29, -29, -28, -32, -47, -57, -38, -34, -34, -34, -34, -38, -57, -96, -61, -53, -48, -48, -53, -61, -96}};

const int Params::KNIGHT_MOBILITY[9] = {-74, -29, -7, 5, 14, 24, 27, 26, 19};
const int Params::BISHOP_MOBILITY[15] = {-54, -27, -12, -1, 10, 17, 21, 25, 30, 34, 35, 40, 47, 33, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-62, -39, -27, -23, -22, -15, -10, -2, 3, 6, 8, 12, 24, 29, 27}, {-64, -29, -9, 8, 24, 31, 31, 36, 43, 47, 53, 58, 60, 50, 46}};
const int Params::QUEEN_MOBILITY[2][24] = {{-56, 1, 15, 24, 31, 37, 45, 51, 56, 60, 63, 64, 67, 67, 72, 76, 72, 89, 96, 96, 96, 96, 96, 96}, {-96, -96, -90, -41, 10, 18, 40, 49, 59, 63, 67, 75, 79, 83, 87, 88, 91, 91, 87, 93, 96, 96, 89, 95}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-96, -63, -41, -29, -24};

const int Params::KNIGHT_OUTPOST[2][2] = {{27, 64}, {21, 32}};
const int Params::BISHOP_OUTPOST[2][2] = {{32, 61}, {37, 27}};


