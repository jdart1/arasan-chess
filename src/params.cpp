// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 18-Nov-2019 08:37:59 by tuner -n 150 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0670421

//

#include "params.h"

const int Params::RB_ADJUST[6] = {37, -6, 80, 30, 29, 63};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -49, -79, -90};
const int Params::QR_ADJUST[5] = {21, 82, 31, -38, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -250, -132};
const int Params::MINOR_FOR_PAWNS[6] = {51, 53, 79, 100, 128, 149};
const int Params::QUEEN_VS_3MINORS[4] = {-80, 0, -94, -238};
const int Params::CASTLING[6] = {13, -38, -29, 0, 3, -25};
const int Params::KING_COVER[6][4] = {{8, 24, 15, 16},
{1, 4, 3, 3},
{-9, -26, -16, -17},
{-8, -25, -15, -17},
{-10, -31, -19, -21},
{-18, -53, -33, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 92;
const int Params::PIN_MULTIPLIER_END = 64;
const int Params::KRMINOR_VS_R_NO_PAWNS = -175;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -151;
const int Params::ENDGAME_PAWN_ADVANTAGE = 29;
const int Params::PAWN_ENDGAME1 = 45;
const int Params::PAWN_ATTACK_FACTOR = 13;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 25;
const int Params::ROOK_ATTACK_FACTOR = 53;
const int Params::ROOK_ATTACK_BOOST = 27;
const int Params::QUEEN_ATTACK_FACTOR = 60;
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
const int Params::WEAK_ON_OPEN_FILE_END = -15;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 15;
const int Params::TWO_ROOKS_ON_7TH_MID = 66;
const int Params::TWO_ROOKS_ON_7TH_END = 91;
const int Params::ROOK_ON_OPEN_FILE_MID = 28;
const int Params::ROOK_ON_OPEN_FILE_END = 12;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 28;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 16;
const int Params::KING_OPP_PAWN_DISTANCE = 9;
const int Params::SIDE_PROTECTED_PAWN = -2;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 33, 59, 93, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={194, 170, 138};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 22, 24, 26, 27, 29, 31, 33, 36, 38, 40, 43, 46, 49, 52, 55, 58, 61, 65, 69, 73, 77, 81, 85, 90, 95, 100, 105, 110, 116, 122, 128, 134, 140, 147, 154, 161, 168, 175, 183, 190, 198, 206, 215, 223, 231, 240, 249, 258, 266, 275, 285, 294, 303, 312, 321, 330, 339, 348, 357, 366, 375, 384, 393, 401, 409, 418, 426, 434, 441, 449, 456, 464, 471, 477, 484, 491, 497, 503, 509, 514, 520, 525, 530, 535, 539, 544, 548, 552, 556, 560, 564, 567, 570, 573, 576, 579, 582, 585, 587, 589, 592, 594, 596, 598, 600, 601, 603, 605, 606, 607, 609, 610, 611, 612, 613, 615, 615, 616, 617};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 12, 15, 21, 29, 35, 55, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 30, 113, 194}, {0, 0, 1, 0, 23, 86, 160, 288}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 2, 2, 19, 96, 0}, {0, 0, 0, 0, 2, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 24, 79, 114, 92}, {0, 0, 0, 0, 45, 63, 164, 291}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 30, 100, 179}, {0, 0, 0, 11, 14, 30, 62, 79}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 10, 7}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-31, -3, 0}, {-21, -33, -110}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-14, -45, -82}, {-48, -73, -128}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{30, 44, 128}, {21, 57, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{0, -20, 0}, {-34, -62, -122}};
const int Params::DOUBLED_PAWNS[2][8] = {{-40, 0, -13, -8, -8, -13, 0, -40}, {-37, -17, -21, -15, -15, -21, -17, -37}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -14, 0, 0, 0, 0, -14, 0}, {-47, -63, -33, -32, -32, -33, -63, -47}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -21, -26, -26, -21, -4, 0}, {-9, -14, -15, -21, -21, -15, -14, -9}};

const int Params::THREAT_BY_PAWN[2][5] = {{90, 101, 98, 47, 0}, {67, 84, 76, 55, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 51, 69, 128, 74}, {27, 24, 58, 72, 101}};
const int Params::THREAT_BY_BISHOP[2][5] = {{21, 52, 75, 84, 78}, {26, 48, 51, 67, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{19, 53, 57, 128, 105}, {28, 45, 52, 0, 128}};
const int Params::KNIGHT_PST[2][64] = {{-94, -37, -22, -15, -15, -22, -37, -94, -37, -34, -25, -2, -2, -25, -34, -37, -49, -25, -5, 20, 20, -5, -25, -49, -19, 3, 18, 22, 22, 18, 3, -19, -30, -15, 16, 10, 10, 16, -15, -30, -56, -52, -14, -9, -9, -14, -52, -56, -98, -89, -36, -27, -27, -36, -89, -98, -128, -128, -128, -128, -128, -128, -128, -128}, {-83, -46, -35, -34, -34, -35, -46, -83, -40, -31, -31, -19, -19, -31, -31, -40, -45, -30, -20, 0, 0, -20, -30, -45, -22, -12, 5, 15, 15, 5, -12, -22, -40, -34, -15, -10, -10, -15, -34, -40, -57, -41, -27, -26, -26, -27, -41, -57, -77, -53, -50, -38, -38, -50, -53, -77, -128, -75, -81, -70, -70, -81, -75, -128}};
const int Params::BISHOP_PST[2][64] = {{-3, 26, 13, 11, 11, 13, 26, -3, 16, 29, 23, 17, 17, 23, 29, 16, -2, 24, 24, 19, 19, 24, 24, -2, 3, 2, 12, 30, 30, 12, 2, 3, -22, 1, 1, 18, 18, 1, 1, -22, 20, -14, -27, -12, -12, -27, -14, 20, -22, -46, -41, -64, -64, -41, -46, -22, -10, -59, -128, -128, -128, -128, -59, -10}, {-28, -12, -3, -9, -9, -3, -12, -28, -12, -5, -6, 2, 2, -6, -5, -12, -10, 4, 9, 12, 12, 9, 4, -10, -7, 4, 13, 16, 16, 13, 4, -7, -18, -6, -7, -1, -1, -7, -6, -18, 4, -12, -10, -13, -13, -10, -12, 4, -5, -6, -16, -15, -15, -16, -6, -5, 2, -5, -8, -27, -27, -8, -5, 2}};
const int Params::ROOK_PST[2][64] = {{-51, -39, -33, -28, -28, -33, -39, -51, -63, -40, -34, -28, -28, -34, -40, -63, -60, -38, -38, -33, -33, -38, -38, -60, -58, -45, -46, -35, -35, -46, -45, -58, -39, -35, -26, -24, -24, -26, -35, -39, -38, -19, -24, -17, -17, -24, -19, -38, -50, -56, -42, -34, -34, -42, -56, -50, -46, -46, -128, -120, -120, -128, -46, -46}, {-28, -31, -21, -27, -27, -21, -31, -28, -41, -36, -29, -29, -29, -29, -36, -41, -35, -29, -23, -25, -25, -23, -29, -35, -19, -14, -8, -12, -12, -8, -14, -19, -7, -5, -1, -5, -5, -1, -5, -7, -7, -3, -0, -3, -3, -0, -3, -7, -19, -15, -13, -13, -13, -13, -15, -19, -0, 4, 5, -1, -1, 5, 4, -0}};
const int Params::QUEEN_PST[2][64] = {{-1, -3, -6, 5, 5, -6, -3, -1, -6, 9, 11, 13, 13, 11, 9, -6, -3, 9, 15, 6, 6, 15, 9, -3, 10, 16, 16, 16, 16, 16, 16, 10, 18, 18, 23, 23, 23, 23, 18, 18, -4, 18, 26, 20, 20, 26, 18, -4, -10, -15, 5, 7, 7, 5, -15, -10, 18, 12, -12, -38, -38, -12, 12, 18}, {-101, -107, -92, -77, -77, -92, -107, -101, -89, -80, -73, -56, -56, -73, -80, -89, -73, -41, -31, -42, -42, -31, -41, -73, -40, -20, -7, -6, -6, -7, -20, -40, -36, 2, 10, 26, 26, 10, 2, -36, -40, -6, 16, 31, 31, 16, -6, -40, -25, -4, 24, 32, 32, 24, -4, -25, -60, -43, -38, -32, -32, -38, -43, -60}};
const int Params::KING_PST[2][64] = {{17, 32, -28, -10, -10, -28, 32, 17, 54, 55, -0, 20, 20, -0, 55, 54, -37, -12, -84, -128, -128, -84, -12, -37, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -123, -128, -128, -128, -128, -123, -128, 30, -21, -128, -128, -128, -128, -21, 30, -44, -128, 128, -47, -47, 128, -128, -44}, {-30, -26, -31, -37, -37, -31, -26, -30, -13, -21, -28, -28, -28, -28, -21, -13, -23, -24, -22, -21, -21, -22, -24, -23, -19, -13, -8, -5, -5, -8, -13, -19, -5, 5, 13, 13, 13, 13, 5, -5, 2, 15, 18, 17, 17, 18, 15, 2, -13, 5, 8, 7, 7, 8, 5, -13, -53, -16, -13, -10, -10, -13, -16, -53}};
const int Params::KNIGHT_MOBILITY[9] = {-80, -34, -12, 1, 11, 21, 24, 24, 17};
const int Params::BISHOP_MOBILITY[15] = {-55, -28, -14, -3, 9, 16, 21, 25, 30, 33, 36, 37, 52, 31, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-87, -62, -51, -47, -46, -40, -36, -30, -26, -21, -16, -10, 2, 15, 7}, {-96, -68, -48, -33, -16, -10, -10, -6, -1, 4, 11, 17, 22, 14, 8}};
const int Params::QUEEN_MOBILITY[2][24] = {{-82, -26, -14, -2, 5, 11, 18, 23, 29, 34, 35, 38, 40, 40, 44, 45, 45, 55, 64, 71, 91, 95, 96, 96}, {-96, -96, -96, -96, -87, -71, -52, -51, -44, -43, -38, -33, -32, -28, -28, -27, -27, -30, -35, -36, -36, -37, -47, -55}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-71, -36, -19, -8, -5};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 57}, {21, 41}};
const int Params::BISHOP_OUTPOST[2][2] = {{13, 39}, {21, 27}};
const int Params::PAWN_STORM[4][2] = {{0, 26},{11, 6},{34, 1},{3, 0}};

