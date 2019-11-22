// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 21-Nov-2019 09:14:31 by tuner -n 150 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0668944

//

#include "params.h"

const int Params::RB_ADJUST[6] = {25, -6, 69, 19, 21, 65};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -39, -71, -85};
const int Params::QR_ADJUST[5] = {-51, 38, 19, -29, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -242, -128};
const int Params::MINOR_FOR_PAWNS[6] = {51, 46, 77, 97, 128, 154};
const int Params::QUEEN_VS_3MINORS[4] = {-96, 0, -100, -221};
const int Params::CASTLING[6] = {13, -38, -30, 0, 4, -25};
const int Params::KING_COVER[6][4] = {{8, 24, 15, 16},
{1, 4, 3, 3},
{-9, -26, -16, -17},
{-8, -25, -15, -16},
{-10, -30, -19, -20},
{-18, -53, -32, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 92;
const int Params::PIN_MULTIPLIER_END = 65;
const int Params::KRMINOR_VS_R_NO_PAWNS = -168;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -143;
const int Params::ENDGAME_PAWN_ADVANTAGE = 31;
const int Params::PAWN_ENDGAME1 = 64;
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
const int Params::PAWN_PUSH_THREAT_MID = 23;
const int Params::PAWN_PUSH_THREAT_END = 12;
const int Params::ENDGAME_KING_THREAT = 38;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 38;
const int Params::BISHOP_PAIR_END = 60;
const int Params::BISHOP_PAWN_PLACEMENT_END = -2;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -24;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -7;
const int Params::WEAK_PAWN_END = -5;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 16;
const int Params::TWO_ROOKS_ON_7TH_MID = 66;
const int Params::TWO_ROOKS_ON_7TH_END = 88;
const int Params::ROOK_ON_OPEN_FILE_MID = 28;
const int Params::ROOK_ON_OPEN_FILE_END = 12;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 28;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 15;
const int Params::KING_OPP_PAWN_DISTANCE = 9;
const int Params::SIDE_PROTECTED_PAWN = -1;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 1, 33, 59, 94, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={186, 171, 137};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 22, 24, 25, 27, 29, 31, 33, 35, 38, 40, 43, 45, 48, 51, 54, 57, 61, 64, 68, 72, 76, 80, 85, 89, 94, 99, 104, 109, 115, 121, 127, 133, 139, 146, 153, 159, 167, 174, 181, 189, 197, 205, 213, 222, 230, 239, 247, 256, 265, 274, 283, 292, 301, 310, 319, 328, 338, 347, 356, 364, 373, 382, 391, 399, 408, 416, 424, 432, 439, 447, 454, 462, 469, 475, 482, 488, 495, 501, 507, 512, 518, 523, 528, 533, 537, 542, 546, 550, 554, 558, 561, 565, 568, 571, 574, 577, 580, 582, 585, 587, 590, 592, 594, 596, 597, 599, 601, 602, 604, 605, 607, 608, 609, 610, 611, 612, 613, 614, 615};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 12, 15, 21, 29, 35, 55, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 30, 113, 194}, {0, 0, 0, 0, 22, 85, 160, 288}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 72, 71, 64, 64, 71, 72, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 2, 19, 96, 0}, {0, 0, 0, 0, 1, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 24, 77, 114, 80}, {0, 0, 0, 0, 45, 62, 162, 286}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 30, 99, 178}, {0, 0, 0, 9, 12, 30, 62, 78}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 9, 5}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-29, -5, 0}, {-20, -32, -110}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-14, -45, -83}, {-48, -73, -128}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{29, 44, 128}, {21, 58, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{0, -21, -0}, {-33, -62, -122}};
const int Params::DOUBLED_PAWNS[2][8] = {{-40, 0, -13, -9, -9, -13, 0, -40}, {-38, -17, -21, -15, -15, -21, -17, -38}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -15, 0, 0, 0, 0, -15, 0}, {-62, -65, -33, -32, -32, -33, -65, -62}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -20, -25, -25, -20, -4, 0}, {-9, -13, -15, -21, -21, -15, -13, -9}};

const int Params::THREAT_BY_PAWN[2][5] = {{90, 101, 99, 47, 0}, {66, 84, 76, 55, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 53, 68, 128, 74}, {27, 26, 59, 73, 102}};
const int Params::THREAT_BY_BISHOP[2][5] = {{21, 51, 76, 84, 77}, {26, 48, 52, 65, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{19, 53, 57, 128, 99}, {29, 45, 52, 3, 128}};
const int Params::KNIGHT_PST[2][64] = {{-99, -43, -29, -21, -21, -29, -43, -99, -42, -40, -31, -8, -8, -31, -40, -42, -55, -31, -11, 14, 14, -11, -31, -55, -25, -3, 12, 15, 15, 12, -3, -25, -35, -21, 10, 4, 4, 10, -21, -35, -63, -59, -20, -16, -16, -20, -59, -63, -105, -95, -42, -34, -34, -42, -95, -105, -128, -128, -128, -128, -128, -128, -128, -128}, {-86, -48, -38, -38, -38, -38, -48, -86, -43, -35, -34, -23, -23, -34, -35, -43, -49, -33, -24, -3, -3, -24, -33, -49, -26, -16, 1, 11, 11, 1, -16, -26, -45, -38, -19, -14, -14, -19, -38, -45, -62, -45, -31, -30, -30, -31, -45, -62, -82, -58, -54, -42, -42, -54, -58, -82, -128, -80, -85, -74, -74, -85, -80, -128}};
const int Params::BISHOP_PST[2][64] = {{-6, 23, 10, 8, 8, 10, 23, -6, 13, 26, 20, 13, 13, 20, 26, 13, -4, 21, 21, 16, 16, 21, 21, -4, -0, -1, 9, 26, 26, 9, -1, -0, -25, -2, -2, 15, 15, -2, -2, -25, 17, -17, -29, -16, -16, -29, -17, 17, -26, -49, -45, -66, -66, -45, -49, -26, -7, -55, -128, -128, -128, -128, -55, -7}, {-30, -13, -4, -9, -9, -4, -13, -30, -12, -6, -6, 1, 1, -6, -6, -12, -11, 3, 8, 11, 11, 8, 3, -11, -8, 3, 12, 15, 15, 12, 3, -8, -20, -7, -8, -3, -3, -8, -7, -20, 2, -14, -12, -15, -15, -12, -14, 2, -7, -8, -18, -18, -18, -18, -8, -7, -1, -8, -9, -29, -29, -9, -8, -1}};
const int Params::ROOK_PST[2][64] = {{-38, -26, -21, -16, -16, -21, -26, -38, -51, -27, -21, -15, -15, -21, -27, -51, -48, -25, -25, -21, -21, -25, -25, -48, -45, -33, -33, -23, -23, -33, -33, -45, -27, -23, -13, -12, -12, -13, -23, -27, -26, -7, -12, -4, -4, -12, -7, -26, -38, -44, -30, -21, -21, -30, -44, -38, -35, -31, -128, -111, -111, -128, -31, -35}, {-12, -15, -6, -12, -12, -6, -15, -12, -26, -21, -14, -14, -14, -14, -21, -26, -19, -14, -8, -9, -9, -8, -14, -19, -4, 1, 7, 4, 4, 7, 1, -4, 8, 10, 14, 10, 10, 14, 10, 8, 8, 12, 15, 13, 13, 15, 12, 8, -5, -0, 2, 1, 1, 2, -0, -5, 15, 19, 20, 15, 15, 20, 19, 15}};
const int Params::QUEEN_PST[2][64] = {{4, 2, -1, 10, 10, -1, 2, 4, -1, 14, 16, 19, 19, 16, 14, -1, 2, 14, 20, 11, 11, 20, 14, 2, 15, 21, 21, 22, 22, 21, 21, 15, 23, 23, 28, 29, 29, 28, 23, 23, 1, 22, 31, 25, 25, 31, 22, 1, -5, -10, 9, 12, 12, 9, -10, -5, 24, 16, -6, -29, -29, -6, 16, 24}, {-84, -91, -76, -61, -61, -76, -91, -84, -71, -64, -58, -41, -41, -58, -64, -71, -55, -25, -15, -26, -26, -15, -25, -55, -24, -4, 8, 9, 9, 8, -4, -24, -20, 18, 25, 41, 41, 25, 18, -20, -23, 10, 31, 46, 46, 31, 10, -23, -9, 11, 39, 47, 47, 39, 11, -9, -45, -26, -23, -18, -18, -23, -26, -45}};
const int Params::KING_PST[2][64] = {{17, 33, -28, -10, -10, -28, 33, 17, 54, 55, 1, 21, 21, 1, 55, 54, -36, -11, -84, -128, -128, -84, -11, -36, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -107, -128, -128, -128, -128, -107, -128, -2, -8, -128, -128, -128, -128, -8, -2, -12, -128, 128, -128, -128, 128, -128, -12}, {-30, -25, -30, -36, -36, -30, -25, -30, -13, -20, -27, -27, -27, -27, -20, -13, -22, -23, -21, -20, -20, -21, -23, -22, -18, -13, -7, -4, -4, -7, -13, -18, -4, 5, 14, 14, 14, 14, 5, -4, 2, 16, 18, 17, 17, 18, 16, 2, -13, 5, 8, 7, 7, 8, 5, -13, -52, -16, -11, -9, -9, -11, -16, -52}};
const int Params::KNIGHT_MOBILITY[9] = {-86, -41, -18, -5, 4, 14, 17, 17, 11};
const int Params::BISHOP_MOBILITY[15] = {-64, -38, -23, -12, -1, 7, 11, 15, 21, 24, 27, 28, 42, 23, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-72, -48, -37, -33, -32, -27, -23, -16, -12, -7, -2, 3, 15, 29, 24}, {-85, -46, -26, -11, 6, 12, 12, 17, 22, 27, 34, 40, 45, 38, 31}};
const int Params::QUEEN_MOBILITY[2][24] = {{-73, -18, -7, 5, 11, 18, 24, 30, 36, 40, 42, 44, 46, 46, 50, 51, 52, 62, 70, 77, 96, 96, 96, 96}, {-96, -96, -96, -96, -64, -49, -30, -30, -24, -21, -16, -12, -9, -6, -5, -4, -4, -6, -11, -11, -10, -11, -21, -26}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-69, -35, -17, -6, -4};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 57}, {22, 42}};
const int Params::BISHOP_OUTPOST[2][2] = {{13, 39}, {22, 28}};
const int Params::PAWN_STORM[4][2] = {{0, 25},{11, 6},{34, 1},{4, 0}};

