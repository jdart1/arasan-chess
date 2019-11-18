// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 17-Nov-2019 01:00:27 by tuner -n 150 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0669191

//

#include "params.h"

const int Params::RB_ADJUST[6] = {40, -6, 83, 33, 31, 64};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -44, -74, -88};
const int Params::QR_ADJUST[5] = {20, 81, 31, -37, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -254, -135};
const int Params::MINOR_FOR_PAWNS[6] = {51, 51, 77, 97, 125, 148};
const int Params::QUEEN_VS_3MINORS[4] = {-75, 0, -90, -236};
const int Params::CASTLING[6] = {13, -38, -29, 0, 3, -24};
const int Params::KING_COVER[6][4] = {{8, 25, 15, 16},
{1, 4, 3, 3},
{-9, -25, -16, -17},
{-8, -25, -15, -17},
{-10, -31, -19, -21},
{-18, -53, -33, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 93;
const int Params::PIN_MULTIPLIER_END = 63;
const int Params::KRMINOR_VS_R_NO_PAWNS = -188;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -154;
const int Params::ENDGAME_PAWN_ADVANTAGE = 29;
const int Params::PAWN_ENDGAME1 = 44;
const int Params::PAWN_ATTACK_FACTOR = 13;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 25;
const int Params::ROOK_ATTACK_FACTOR = 53;
const int Params::ROOK_ATTACK_BOOST = 27;
const int Params::QUEEN_ATTACK_FACTOR = 61;
const int Params::QUEEN_ATTACK_BOOST = 43;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 114;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 5;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 73;
const int Params::OWN_ROOK_KING_PROXIMITY = 24;
const int Params::OWN_QUEEN_KING_PROXIMITY = 15;
const int Params::PAWN_PUSH_THREAT_MID = 24;
const int Params::PAWN_PUSH_THREAT_END = 12;
const int Params::ENDGAME_KING_THREAT = 38;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 59;
const int Params::BISHOP_PAWN_PLACEMENT_END = -1;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -23;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -7;
const int Params::WEAK_PAWN_END = -5;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -15;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 15;
const int Params::TWO_ROOKS_ON_7TH_MID = 66;
const int Params::TWO_ROOKS_ON_7TH_END = 91;
const int Params::ROOK_ON_OPEN_FILE_MID = 28;
const int Params::ROOK_ON_OPEN_FILE_END = 13;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 28;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 16;
const int Params::KING_OPP_PAWN_DISTANCE = 9;
const int Params::SIDE_PROTECTED_PAWN = -2;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 33, 59, 94, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={195, 170, 138};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 22, 24, 25, 27, 29, 31, 33, 35, 38, 40, 43, 46, 48, 51, 54, 58, 61, 65, 68, 72, 76, 81, 85, 90, 94, 99, 105, 110, 116, 121, 127, 134, 140, 147, 153, 160, 167, 175, 182, 190, 198, 206, 214, 223, 231, 240, 248, 257, 266, 275, 284, 293, 303, 312, 321, 330, 339, 348, 357, 366, 375, 384, 392, 401, 409, 418, 426, 434, 441, 449, 456, 463, 471, 477, 484, 490, 497, 503, 508, 514, 519, 525, 530, 534, 539, 544, 548, 552, 556, 560, 563, 567, 570, 573, 576, 579, 582, 584, 587, 589, 591, 593, 595, 597, 599, 601, 603, 604, 606, 607, 608, 610, 611, 612, 613, 614, 615, 616, 617};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 12, 15, 21, 29, 35, 55, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 31, 113, 193}, {0, 0, 1, 0, 23, 86, 160, 288}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {71, 73, 71, 64, 64, 71, 73, 71};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 2, 2, 19, 96, 0}, {0, 0, 0, 0, 2, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 24, 80, 119, 119}, {0, 0, 0, 0, 45, 63, 164, 292}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 30, 100, 177}, {0, 0, 0, 11, 14, 31, 62, 80}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 10, 7}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-30, -3, 0}, {-21, -33, -111}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-15, -45, -81}, {-48, -73, -128}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{29, 44, 128}, {21, 57, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{0, -20, -0}, {-34, -61, -122}};
const int Params::DOUBLED_PAWNS[2][8] = {{-40, 0, -13, -8, -8, -13, 0, -40}, {-37, -16, -21, -15, -15, -21, -16, -37}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -15, 0, 0, 0, 0, -15, 0}, {-48, -62, -32, -32, -32, -32, -62, -48}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -21, -26, -26, -21, -4, 0}, {-9, -14, -15, -21, -21, -15, -14, -9}};

const int Params::THREAT_BY_PAWN[2][5] = {{90, 101, 98, 47, 0}, {67, 84, 76, 54, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 51, 69, 128, 74}, {27, 25, 58, 73, 100}};
const int Params::THREAT_BY_BISHOP[2][5] = {{21, 52, 76, 84, 78}, {26, 48, 52, 67, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{19, 53, 57, 128, 105}, {28, 44, 51, 0, 128}};
const int Params::KNIGHT_PST[2][64] = {{-94, -37, -22, -15, -15, -22, -37, -94, -37, -34, -25, -2, -2, -25, -34, -37, -49, -25, -5, 20, 20, -5, -25, -49, -19, 3, 18, 22, 22, 18, 3, -19, -30, -15, 16, 10, 10, 16, -15, -30, -57, -52, -14, -9, -9, -14, -52, -57, -98, -88, -36, -27, -27, -36, -88, -98, -128, -128, -128, -128, -128, -128, -128, -128}, {-80, -44, -33, -33, -33, -33, -44, -80, -38, -28, -29, -17, -17, -29, -28, -38, -43, -28, -19, 2, 2, -19, -28, -43, -20, -10, 7, 17, 17, 7, -10, -20, -38, -32, -13, -8, -8, -13, -32, -38, -55, -39, -25, -24, -24, -25, -39, -55, -76, -52, -48, -36, -36, -48, -52, -76, -128, -71, -78, -68, -68, -78, -71, -128}};
const int Params::BISHOP_PST[2][64] = {{-4, 26, 13, 11, 11, 13, 26, -4, 16, 29, 23, 16, 16, 23, 29, 16, -2, 24, 24, 19, 19, 24, 24, -2, 2, 2, 12, 30, 30, 12, 2, 2, -22, 1, 1, 18, 18, 1, 1, -22, 20, -14, -27, -12, -12, -27, -14, 20, -23, -46, -41, -65, -65, -41, -46, -23, -11, -58, -128, -128, -128, -128, -58, -11}, {-26, -10, -1, -7, -7, -1, -10, -26, -9, -3, -4, 4, 4, -4, -3, -9, -8, 6, 11, 14, 14, 11, 6, -8, -4, 6, 15, 18, 18, 15, 6, -4, -16, -4, -5, 0, 0, -5, -4, -16, 6, -10, -9, -12, -12, -9, -10, 6, -4, -5, -14, -14, -14, -14, -5, -4, 4, -3, -6, -25, -25, -6, -3, 4}};
const int Params::ROOK_PST[2][64] = {{-50, -38, -33, -27, -27, -33, -38, -50, -63, -39, -33, -27, -27, -33, -39, -63, -59, -37, -37, -32, -32, -37, -37, -59, -57, -44, -45, -35, -35, -45, -44, -57, -39, -34, -25, -24, -24, -25, -34, -39, -38, -18, -23, -16, -16, -23, -18, -38, -49, -55, -41, -33, -33, -41, -55, -49, -46, -44, -128, -117, -117, -128, -44, -46}, {-27, -30, -20, -27, -27, -20, -30, -27, -40, -35, -28, -28, -28, -28, -35, -40, -34, -28, -23, -24, -24, -23, -28, -34, -19, -13, -8, -11, -11, -8, -13, -19, -7, -4, -1, -4, -4, -1, -4, -7, -7, -3, 0, -2, -2, 0, -3, -7, -19, -15, -13, -13, -13, -13, -15, -19, -0, 4, 5, -1, -1, 5, 4, -0}};
const int Params::QUEEN_PST[2][64] = {{-0, -2, -5, 6, 6, -5, -2, -0, -5, 10, 12, 14, 14, 12, 10, -5, -3, 10, 16, 7, 7, 16, 10, -3, 11, 17, 17, 17, 17, 17, 17, 11, 19, 19, 24, 24, 24, 24, 19, 19, -3, 19, 27, 21, 21, 27, 19, -3, -10, -14, 5, 8, 8, 5, -14, -10, 18, 14, -12, -38, -38, -12, 14, 18}, {-100, -106, -91, -75, -75, -91, -106, -100, -87, -79, -71, -54, -54, -71, -79, -87, -71, -39, -29, -40, -40, -29, -39, -71, -38, -18, -6, -4, -4, -6, -18, -38, -35, 4, 11, 28, 28, 11, 4, -35, -38, -4, 18, 33, 33, 18, -4, -38, -23, -2, 25, 33, 33, 25, -2, -23, -58, -41, -36, -29, -29, -36, -41, -58}};
const int Params::KING_PST[2][64] = {{18, 33, -28, -10, -10, -28, 33, 18, 55, 56, -0, 20, 20, -0, 56, 55, -37, -11, -84, -128, -128, -84, -11, -37, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -115, -128, -128, -128, -128, -115, -128, 18, 1, -128, -128, -128, -128, 1, 18, -61, -128, 83, 24, 24, 83, -128, -61}, {-29, -24, -30, -36, -36, -30, -24, -29, -12, -20, -27, -27, -27, -27, -20, -12, -21, -23, -21, -20, -20, -21, -23, -21, -18, -12, -7, -4, -4, -7, -12, -18, -4, 6, 14, 14, 14, 14, 6, -4, 4, 17, 19, 18, 18, 19, 17, 4, -12, 6, 10, 8, 8, 10, 6, -12, -52, -14, -11, -9, -9, -11, -14, -52}};
const int Params::KNIGHT_MOBILITY[9] = {-79, -33, -11, 2, 12, 22, 25, 25, 20};
const int Params::BISHOP_MOBILITY[15] = {-53, -27, -13, -1, 10, 17, 22, 26, 32, 35, 38, 40, 53, 35, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-86, -61, -50, -46, -45, -40, -36, -29, -25, -20, -15, -9, 3, 16, 7}, {-96, -67, -47, -32, -14, -9, -8, -4, 1, 6, 12, 18, 23, 16, 12}};
const int Params::QUEEN_MOBILITY[2][24] = {{-80, -25, -13, -1, 6, 12, 19, 24, 30, 35, 36, 39, 41, 41, 45, 46, 46, 57, 65, 73, 93, 96, 96, 96}, {-96, -96, -96, -96, -84, -68, -49, -48, -41, -40, -35, -30, -29, -25, -24, -24, -23, -27, -32, -33, -32, -34, -44, -51}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-73, -39, -21, -10, -8};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 57}, {21, 42}};
const int Params::BISHOP_OUTPOST[2][2] = {{13, 39}, {21, 28}};
const int Params::PAWN_STORM[4][2] = {{0, 25},{11, 7},{34, 1},{3, 0}};

