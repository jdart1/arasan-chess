// Copyright 2015-2020 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 20-Sep-2020 12:25:57 by tuner -n 175 -c 24 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0657226

//

#include "params.h"

const int Params::KN_VS_PAWN_ADJUST[3] = {0, -113, 23};
const int Params::KING_COVER[6][4] = {{8, 23, 15, 16},
{1, 4, 3, 3},
{-9, -25, -16, -17},
{-8, -25, -15, -17},
{-10, -30, -19, -21},
{-17, -51, -32, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 94;
const int Params::PIN_MULTIPLIER_END = 64;
const int Params::MINOR_FOR_PAWNS_MIDGAME = 160;
const int Params::MINOR_FOR_PAWNS_ENDGAME = 91;
const int Params::RB_ADJUST_MIDGAME = 88;
const int Params::RB_ADJUST_ENDGAME = 65;
const int Params::RBN_ADJUST_MIDGAME = -98;
const int Params::RBN_ADJUST_ENDGAME = 29;
const int Params::QR_ADJUST_MIDGAME = -10;
const int Params::QR_ADJUST_ENDGAME = 32;
const int Params::Q_VS_3MINORS_MIDGAME = -282;
const int Params::Q_VS_3MINORS_ENDGAME = -14;
const int Params::KRMINOR_VS_R_NO_PAWNS = -153;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -132;
const int Params::TRADE_DOWN1 = -24;
const int Params::TRADE_DOWN2 = 43;
const int Params::TRADE_DOWN3 = 19;
const int Params::PAWN_ENDGAME_ADJUST = 8;
const int Params::PAWN_ATTACK_FACTOR = 14;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 24;
const int Params::ROOK_ATTACK_FACTOR = 54;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 60;
const int Params::QUEEN_ATTACK_BOOST = 43;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 119;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 5;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 76;
const int Params::OWN_ROOK_KING_PROXIMITY = 25;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_PUSH_THREAT_MID = 24;
const int Params::PAWN_PUSH_THREAT_END = 14;
const int Params::ENDGAME_KING_THREAT = 38;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 37;
const int Params::BISHOP_PAIR_END = 64;
const int Params::BISHOP_PAWN_PLACEMENT_END = -3;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -23;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -8;
const int Params::WEAK_PAWN_END = -5;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 7;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 13;
const int Params::TWO_ROOKS_ON_7TH_MID = 64;
const int Params::TWO_ROOKS_ON_7TH_END = 85;
const int Params::TRAPPED_ROOK_NO_CASTLE = -67;
const int Params::ROOK_ON_OPEN_FILE_MID = 29;
const int Params::ROOK_ON_OPEN_FILE_END = 10;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 26;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 18;
const int Params::KING_OPP_PAWN_DISTANCE = 8;
const int Params::SIDE_PROTECTED_PAWN = -3;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 1, 29, 59, 96, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={249, 158, 132};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21, 22, 24, 26, 28, 30, 32, 34, 36, 38, 41, 43, 46, 49, 52, 55, 59, 62, 66, 69, 73, 78, 82, 86, 91, 96, 101, 106, 112, 117, 123, 129, 135, 142, 149, 155, 163, 170, 177, 185, 193, 201, 209, 217, 226, 234, 243, 252, 261, 270, 279, 288, 297, 307, 316, 325, 334, 343, 353, 362, 371, 380, 388, 397, 406, 414, 423, 431, 439, 446, 454, 462, 469, 476, 483, 489, 496, 502, 508, 514, 520, 525, 530, 535, 540, 545, 549, 554, 558, 562, 565, 569, 573, 576, 579, 582, 585, 588, 590, 593, 595, 597, 599, 601, 603, 605, 607, 608, 610, 612, 613, 614, 616, 617, 618, 619, 620, 621, 622, 623};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 12, 15, 21, 30, 36, 56, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 34, 121, 197}, {0, 0, 0, 0, 19, 73, 147, 233}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {66, 69, 67, 64, 64, 67, 69, 66};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 25, 75, 113, 170}, {0, 0, 0, 0, 42, 67, 184, 276}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 29, 100, 172}, {0, 0, 0, 9, 11, 30, 64, 83}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{0, 0, 0, 0, 0, 128}, {17, 3, 28, 37, 74, 112}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-32, -0, 0}, {-3, -7, -34}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-19, -47, -58}, {-33, -49, -74}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{30, 50, 128}, {21, 45, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{-2, -25, 0}, {-18, -33, -44}};
const int Params::DOUBLED_PAWNS[2][8] = {{-37, 0, -14, -8, -8, -14, 0, -37}, {-33, -14, -19, -16, -16, -19, -14, -33}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -18, 0, 0, 0, 0, -18, 0}, {-78, -55, -31, -37, -37, -31, -55, -78}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -5, -21, -26, -26, -21, -5, 0}, {-11, -16, -15, -21, -21, -15, -16, -11}};

const int Params::THREAT_BY_PAWN[2][5] = {{94, 104, 96, 51, 0}, {69, 85, 78, 40, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 44, 69, 127, 76}, {28, 30, 57, 78, 109}};
const int Params::THREAT_BY_BISHOP[2][5] = {{21, 52, 72, 83, 77}, {27, 47, 50, 69, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{19, 54, 58, 124, 93}, {27, 42, 47, 6, 128}};
const int Params::KNIGHT_PST[2][64] = {{-99, -44, -29, -22, -22, -29, -44, -99, -40, -38, -28, -6, -6, -28, -38, -40, -53, -29, -9, 16, 16, -9, -29, -53, -24, -1, 14, 18, 18, 14, -1, -24, -36, -20, 13, 6, 6, 13, -20, -36, -65, -57, -18, -14, -14, -18, -57, -65, -106, -94, -40, -35, -35, -40, -94, -106, -128, -128, -128, -128, -128, -128, -128, -128}, {-87, -44, -33, -33, -33, -33, -44, -87, -40, -30, -31, -18, -18, -31, -30, -40, -44, -29, -19, 0, 0, -19, -29, -44, -21, -12, 5, 16, 16, 5, -12, -21, -39, -34, -15, -9, -9, -15, -34, -39, -56, -41, -28, -26, -26, -28, -41, -56, -73, -54, -52, -38, -38, -52, -54, -73, -128, -77, -83, -69, -69, -83, -77, -128}};
const int Params::BISHOP_PST[2][64] = {{-7, 20, 7, 6, 6, 7, 20, -7, 13, 26, 19, 14, 14, 19, 26, 13, -5, 20, 21, 16, 16, 21, 20, -5, -1, 0, 10, 28, 28, 10, 0, -1, -27, -2, -2, 16, 16, -2, -2, -27, 16, -19, -27, -15, -15, -27, -19, 16, -27, -47, -44, -68, -68, -44, -47, -27, -0, -54, -128, -128, -128, -128, -54, -0}, {-33, -15, -4, -9, -9, -4, -15, -33, -15, -6, -7, 1, 1, -7, -6, -15, -12, 3, 7, 11, 11, 7, 3, -12, -7, 2, 11, 13, 13, 11, 2, -7, -17, -5, -6, -1, -1, -6, -5, -17, 5, -12, -10, -13, -13, -10, -12, 5, -4, -6, -17, -16, -16, -17, -6, -4, 0, -6, -8, -26, -26, -8, -6, 0}};
const int Params::ROOK_PST[2][64] = {{-26, -17, -12, -6, -6, -12, -17, -26, -42, -18, -13, -8, -8, -13, -18, -42, -36, -15, -17, -13, -13, -17, -15, -36, -36, -25, -26, -15, -15, -26, -25, -36, -18, -16, -5, -4, -4, -5, -16, -18, -21, -1, -5, 3, 3, -5, -1, -21, -33, -39, -24, -15, -15, -24, -39, -33, -30, -25, -128, -101, -101, -128, -25, -30}, {-20, -22, -13, -18, -18, -13, -22, -20, -29, -23, -16, -15, -15, -16, -23, -29, -21, -15, -9, -10, -10, -9, -15, -21, -4, 3, 7, 3, 3, 7, 3, -4, 11, 13, 16, 12, 12, 16, 13, 11, 12, 14, 17, 14, 14, 17, 14, 12, 2, 5, 6, 5, 5, 6, 5, 2, 21, 25, 25, 17, 17, 25, 25, 21}};
const int Params::QUEEN_PST[2][64] = {{-4, -7, -10, -0, -0, -10, -7, -4, -10, 5, 8, 10, 10, 8, 5, -10, -8, 5, 12, 2, 2, 12, 5, -8, 6, 11, 13, 14, 14, 13, 11, 6, 14, 16, 20, 21, 21, 20, 16, 14, -9, 13, 22, 15, 15, 22, 13, -9, -14, -19, -1, 1, 1, -1, -19, -14, 15, 6, -22, -45, -45, -22, 6, 15}, {-88, -91, -74, -58, -58, -74, -91, -88, -67, -60, -54, -37, -37, -54, -60, -67, -50, -21, -11, -22, -22, -11, -21, -50, -20, -0, 11, 12, 12, 11, -0, -20, -13, 22, 30, 44, 44, 30, 22, -13, -18, 15, 37, 50, 50, 37, 15, -18, -2, 16, 44, 53, 53, 44, 16, -2, -33, -20, -14, -10, -10, -14, -20, -33}};
const int Params::KING_PST[2][64] = {{12, 28, -26, 4, 4, -26, 28, 12, 48, 48, -6, 14, 14, -6, 48, 48, -52, -28, -90, -128, -128, -90, -28, -52, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -127, -128, -128, -128, -128, -127, -128, 45, -29, -128, -128, -128, -128, -29, 45, -87, -128, -73, 13, 13, -73, -128, -87}, {-3, 1, -4, -11, -11, -4, 1, -3, 15, 7, 1, 1, 1, 1, 7, 15, 7, 5, 8, 9, 9, 8, 5, 7, 13, 18, 23, 25, 25, 23, 18, 13, 28, 36, 45, 45, 45, 45, 36, 28, 38, 49, 51, 49, 49, 51, 49, 38, 23, 39, 42, 40, 40, 42, 39, 23, -11, 18, 20, 21, 21, 20, 18, -11}};
const int Params::KNIGHT_MOBILITY[9] = {-85, -39, -17, -4, 6, 16, 19, 18, 10};
const int Params::BISHOP_MOBILITY[15] = {-60, -33, -19, -8, 3, 11, 15, 20, 25, 28, 30, 30, 44, 23, 38};
const int Params::ROOK_MOBILITY[2][15] = {{-92, -71, -60, -59, -60, -55, -51, -45, -41, -36, -31, -26, -15, -2, -7}, {-96, -55, -36, -21, -6, 0, 1, 7, 12, 18, 24, 31, 36, 29, 23}};
const int Params::QUEEN_MOBILITY[2][24] = {{-76, -20, -7, 4, 11, 18, 24, 29, 35, 40, 42, 44, 46, 46, 50, 51, 53, 61, 73, 77, 95, 96, 96, 96}, {-96, -96, -96, -96, -70, -54, -36, -34, -27, -24, -19, -14, -12, -8, -8, -8, -8, -10, -16, -18, -15, -17, -25, -34}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-50, -14, 3, 14, 17};
const int Params::KNIGHT_OUTPOST[2][2] = {{27, 58}, {22, 42}};
const int Params::BISHOP_OUTPOST[2][2] = {{13, 40}, {19, 24}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -21, 26, 3},{20, -29, 22, 30, -8},{20, 30, 15, 41, 10},{20, -11, 27, 41, 16}},{{10, -61, 1, -24, -10},{10, -64, -5, -8, -3},{10, 32, 20, 15, 9},{10, 64, -11, 11, -1}}};

