// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 25-May-2018 05:21:17 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/epd/big5.epd
// Final objective value: 0.0760207

//

#include "params.h"

const int Params::RB_ADJUST[6] = {53, -6, 89, 39, 31, 61};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -11, -37, -32};
const int Params::QR_ADJUST[5] = {21, 73, 24, -37, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -291, -165};
const int Params::CASTLING[6] = {13, -38, -25, 0, 4, -18};
const int Params::KING_COVER[6][4] = {{11, 21, 14, 12},
{5, 9, 6, 5},
{-6, -13, -8, -7},
{-6, -11, -7, -6},
{-9, -18, -11, -10},
{-18, -37, -24, -21}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 79;
const int Params::PIN_MULTIPLIER_END = 83;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -184;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -150;
const int Params::MINOR_FOR_PAWNS = 40;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 40;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 8;
const int Params::PAWN_ATTACK_FACTOR2 = 8;
const int Params::MINOR_ATTACK_FACTOR = 33;
const int Params::MINOR_ATTACK_BOOST = 39;
const int Params::ROOK_ATTACK_FACTOR = 47;
const int Params::ROOK_ATTACK_BOOST = 21;
const int Params::ROOK_ATTACK_BOOST2 = 33;
const int Params::QUEEN_ATTACK_FACTOR = 50;
const int Params::QUEEN_ATTACK_BOOST = 44;
const int Params::QUEEN_ATTACK_BOOST2 = 54;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 9;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 158;
const int Params::PAWN_THREAT_ON_PIECE_MID = 2;
const int Params::PAWN_THREAT_ON_PIECE_END = 12;
const int Params::PIECE_THREAT_MM_MID = 32;
const int Params::PIECE_THREAT_MR_MID = 88;
const int Params::PIECE_THREAT_MQ_MID = 68;
const int Params::PIECE_THREAT_MM_END = 30;
const int Params::PIECE_THREAT_MR_END = 74;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 9;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 19;
const int Params::PIECE_THREAT_RR_MID = 3;
const int Params::PIECE_THREAT_RQ_MID = 94;
const int Params::PIECE_THREAT_RM_END = 31;
const int Params::PIECE_THREAT_RR_END = 1;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 40;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 33;
const int Params::BISHOP_PAIR_END = 57;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -6;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -26;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -2;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -24;
const int Params::WEAK_ON_OPEN_FILE_END = -19;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 0;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 25;
const int Params::TWO_ROOKS_ON_7TH_MID = 17;
const int Params::TWO_ROOKS_ON_7TH_END = 68;
const int Params::ROOK_ON_OPEN_FILE_MID = 28;
const int Params::ROOK_ON_OPEN_FILE_END = 16;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 13;
const int Params::QUEEN_OUT = -31;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 10;
const int Params::KING_OPP_PAWN_DISTANCE = 4;
const int Params::QUEENING_SQUARE_CONTROL_MID = 60;
const int Params::QUEENING_SQUARE_CONTROL_END = 48;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -21;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -44;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 22, 51, 86, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={294, 218, 153};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 18, 19, 21, 22, 24, 26, 28, 30, 33, 35, 38, 41, 43, 46, 50, 53, 56, 60, 64, 68, 73, 77, 82, 87, 92, 97, 103, 109, 115, 122, 128, 135, 142, 150, 157, 165, 173, 181, 190, 199, 208, 217, 226, 236, 245, 255, 265, 275, 285, 295, 305, 315, 325, 335, 345, 355, 365, 374, 384, 393, 403, 412, 421, 430, 438, 447, 455, 463, 470, 478, 485, 492, 498, 505, 511, 517, 523, 528, 533, 538, 543, 548, 552, 556, 560, 564, 567, 571, 574, 577, 580, 583, 585, 588, 590, 592, 594, 596, 598, 600, 602, 603, 605, 606, 607, 608, 610, 611, 612, 613, 614, 614, 615, 616, 617, 617, 618, 619, 619, 620, 620, 621};
const int Params::TRADE_DOWN[8] = {45, 40, 34, 29, 23, 18, 12, 7};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 27, 98, 135}, {0, 0, 0, 0, 18, 68, 149, 224}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {80, 82, 80, 75, 75, 80, 82, 80};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 10, 96, 0}, {0, 0, 0, 0, 0, 16, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 12, 94, 218, 288}, {0, 0, 0, 0, 43, 92, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 12, 0, 24, 90, 173}, {0, 0, 0, 8, 13, 28, 57, 56}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-41, -36, -38, -20, -34, -8, -27, -29, -11, -26, -33, -31, 0, -18, -32, -41, -8, -43, -25, -43, 0}, {0, 0, -1, -9, -18, -7, -1, -14, -13, -25, -26, -6, -5, -8, -15, -18, -16, -27, -43, -43, -43}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-43, -43, -43, -43, -43, -43, -43, -43, -31, -35, -36, -43, -12, -1, -8, -29, 0, 0, -43, -23, -43}, {-15, -13, 0, -6, -9, -4, -23, -11, -13, -7, -10, -27, -10, -3, -0, -43, -19, -11, -43, -37, -43}};
const int Params::DOUBLED_PAWNS[2][8] = {{-44, 0, -19, -16, -16, -19, 0, -44}, {-45, -21, -27, -21, -21, -27, -21, -45}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -28, -5, -8, -8, -5, -28, 0}, {-32, -64, -30, -44, -44, -30, -64, -32}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -14, -17, -17, -14, 0, 0}, {-4, -8, -10, -17, -17, -10, -8, -4}};

const int Params::KNIGHT_PST[2][64] = {{-78, -26, -7, -8, -8, -7, -26, -78, -21, -27, -19, 3, 3, -19, -27, -21, -39, -17, 2, 21, 21, 2, -17, -39, -18, -3, 16, 17, 17, 16, -3, -18, -33, -17, 14, 10, 10, 14, -17, -33, -61, -46, -14, -4, -4, -14, -46, -61, -108, -85, -35, -29, -29, -35, -85, -108, -128, -128, -128, -128, -128, -128, -128, -128}, {-65, -48, -34, -32, -32, -34, -48, -65, -42, -29, -26, -20, -20, -26, -29, -42, -48, -31, -23, 0, 0, -23, -31, -48, -23, -16, 7, 14, 14, 7, -16, -23, -32, -23, -2, 2, 2, -2, -23, -32, -48, -33, -15, -7, -7, -15, -33, -48, -72, -46, -41, -31, -31, -41, -46, -72, -128, -69, -71, -55, -55, -71, -69, -128}};
const int Params::BISHOP_PST[2][64] = {{9, 41, 26, 22, 22, 26, 41, 9, 28, 37, 33, 25, 25, 33, 37, 28, 8, 33, 33, 26, 26, 33, 33, 8, -2, 5, 15, 32, 32, 15, 5, -2, -41, -17, -11, 13, 13, -11, -17, -41, -0, -38, -54, -35, -35, -54, -38, -0, -45, -71, -66, -105, -105, -66, -71, -45, -33, -77, -128, -128, -128, -128, -77, -33}, {-21, -3, -1, -5, -5, -1, -3, -21, -2, 1, -1, 6, 6, -1, 1, -2, -6, 4, 10, 13, 13, 10, 4, -6, -8, 4, 15, 20, 20, 15, 4, -8, -26, -12, -9, -0, -0, -9, -12, -26, -1, -17, -15, -15, -15, -15, -17, -1, -14, -14, -28, -30, -30, -28, -14, -14, -8, -8, -8, -29, -29, -8, -8, -8}};
const int Params::ROOK_PST[2][64] = {{-73, -59, -53, -51, -51, -53, -59, -73, -84, -59, -54, -51, -51, -54, -59, -84, -79, -57, -59, -60, -60, -59, -57, -79, -80, -64, -64, -55, -55, -64, -64, -80, -59, -48, -41, -33, -33, -41, -48, -59, -48, -22, -27, -13, -13, -27, -22, -48, -47, -54, -21, -17, -17, -21, -54, -47, -39, -35, -118, -82, -82, -118, -35, -39}, {-13, -11, -4, -9, -9, -4, -11, -13, -17, -14, -6, -6, -6, -6, -14, -17, -17, -9, -6, -7, -7, -6, -9, -17, -8, 4, 7, 2, 2, 7, 4, -8, 8, 10, 16, 12, 12, 16, 10, 8, 13, 21, 22, 21, 21, 22, 21, 13, 3, 8, 14, 14, 14, 14, 8, 3, 27, 29, 29, 24, 24, 29, 29, 27}};
const int Params::QUEEN_PST[2][64] = {{31, 34, 33, 43, 43, 33, 34, 31, 29, 41, 45, 48, 48, 45, 41, 29, 24, 38, 45, 35, 35, 45, 38, 24, 36, 44, 43, 45, 45, 43, 44, 36, 38, 40, 47, 52, 52, 47, 40, 38, 28, 36, 52, 49, 49, 52, 36, 28, 15, 6, 30, 25, 25, 30, 6, 15, 19, 28, 7, 6, 6, 7, 28, 19}, {-39, -48, -38, -25, -25, -38, -48, -39, -25, -28, -21, -4, -4, -21, -28, -25, -11, 11, 21, 16, 16, 21, 11, -11, 10, 30, 40, 45, 45, 40, 30, 10, 24, 55, 63, 79, 79, 63, 55, 24, 24, 56, 81, 89, 89, 81, 56, 24, 44, 56, 85, 98, 98, 85, 56, 44, 14, 28, 30, 33, 33, 30, 28, 14}};
const int Params::KING_PST[2][64] = {{33, 52, 4, 18, 18, 4, 52, 33, 72, 70, 38, 53, 53, 38, 70, 72, 3, 35, -27, -67, -67, -27, 35, 3, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -127, -101, -128, -128, -128, -128, -101, -127, -113, 128, -128, -128, -128, -128, 128, -113, -47, -128, 128, -128, -128, 128, -128, -47}, {-76, -73, -76, -79, -79, -76, -73, -76, -61, -66, -71, -69, -69, -71, -66, -61, -68, -70, -65, -63, -63, -65, -70, -68, -65, -59, -52, -50, -50, -52, -59, -65, -49, -41, -34, -33, -33, -34, -41, -49, -43, -32, -27, -26, -26, -27, -32, -43, -47, -37, -33, -34, -34, -33, -37, -47, -78, -54, -50, -52, -52, -50, -54, -78}};

const int Params::KNIGHT_MOBILITY[9] = {-65, -27, -9, 2, 10, 19, 22, 20, 14};
const int Params::BISHOP_MOBILITY[15] = {-43, -25, -12, -3, 7, 15, 19, 22, 27, 30, 30, 35, 40, 22, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-43, -28, -18, -14, -12, -6, -1, 8, 12, 17, 19, 21, 35, 35, 33}, {-43, -34, -15, 0, 13, 19, 21, 23, 30, 34, 38, 43, 43, 37, 30}};
const int Params::QUEEN_MOBILITY[2][24] = {{-43, -16, -10, -3, 3, 9, 15, 19, 24, 29, 30, 32, 33, 34, 36, 40, 41, 43, 43, 43, 43, 43, 43, 43}, {-43, -43, -43, -43, -24, -10, 10, 14, 25, 23, 29, 35, 35, 39, 42, 41, 43, 43, 43, 42, 43, 43, 38, 28}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-43, -15, 6, 16, 19};

const int Params::KNIGHT_OUTPOST[2][2] = {{29, 61}, {17, 28}};
const int Params::BISHOP_OUTPOST[2][2] = {{36, 61}, {33, 24}};


