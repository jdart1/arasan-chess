// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 14-Nov-2019 09:26:00 by tuner -n 150 -R 25 -c 16 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0669378

//

#include "params.h"

const int Params::RB_ADJUST[6] = {44, -6, 87, 37, 36, 68};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -40, -69, -79};
const int Params::QR_ADJUST[5] = {18, 80, 33, -36, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -255, -137};
const int Params::MINOR_FOR_PAWNS[6] = {51, 51, 76, 96, 123, 143};
const int Params::CASTLING[6] = {13, -38, -30, 0, 3, -24};
const int Params::KING_COVER[6][4] = {{8, 25, 15, 16},
{1, 4, 3, 3},
{-9, -25, -16, -17},
{-8, -25, -15, -16},
{-10, -31, -19, -21},
{-18, -53, -33, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 93;
const int Params::PIN_MULTIPLIER_END = 63;
const int Params::KRMINOR_VS_R_NO_PAWNS = -189;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -155;
const int Params::ENDGAME_PAWN_ADVANTAGE = 29;
const int Params::PAWN_ENDGAME1 = 43;
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
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 59;
const int Params::BISHOP_PAWN_PLACEMENT_END = -1;
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
const int Params::ROOK_ON_OPEN_FILE_END = 13;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 28;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 16;
const int Params::KING_OPP_PAWN_DISTANCE = 9;
const int Params::SIDE_PROTECTED_PAWN = -2;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 33, 59, 94, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={195, 169, 137};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 22, 24, 25, 27, 29, 31, 33, 36, 38, 40, 43, 46, 48, 51, 55, 58, 61, 65, 68, 72, 76, 81, 85, 90, 95, 100, 105, 110, 116, 121, 127, 134, 140, 147, 153, 160, 168, 175, 182, 190, 198, 206, 214, 223, 231, 240, 249, 257, 266, 275, 284, 293, 303, 312, 321, 330, 339, 348, 357, 366, 375, 384, 392, 401, 409, 418, 426, 434, 441, 449, 456, 464, 471, 477, 484, 490, 497, 503, 508, 514, 519, 525, 530, 534, 539, 544, 548, 552, 556, 560, 563, 567, 570, 573, 576, 579, 582, 584, 587, 589, 591, 593, 595, 597, 599, 601, 603, 604, 606, 607, 608, 610, 611, 612, 613, 614, 615, 616, 617};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 12, 15, 21, 29, 35, 56, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 31, 113, 193}, {0, 0, 1, 0, 23, 86, 160, 288}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {71, 73, 71, 64, 64, 71, 73, 71};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 2, 2, 19, 96, 0}, {0, 0, 0, 0, 2, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 24, 79, 111, 126}, {0, 0, 0, 0, 45, 63, 165, 291}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 31, 101, 179}, {0, 0, 0, 11, 14, 31, 62, 79}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 10, 7}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-30, -3, 0}, {-21, -34, -111}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-15, -45, -81}, {-48, -73, -128}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{29, 44, 128}, {21, 57, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{0, -20, 0}, {-34, -61, -122}};
const int Params::DOUBLED_PAWNS[2][8] = {{-40, 0, -13, -8, -8, -13, 0, -40}, {-37, -16, -21, -15, -15, -21, -16, -37}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -16, 0, 0, 0, 0, -16, 0}, {-48, -61, -32, -32, -32, -32, -61, -48}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -21, -26, -26, -21, -4, 0}, {-9, -14, -15, -21, -21, -15, -14, -9}};

const int Params::THREAT_BY_PAWN[2][5] = {{90, 100, 99, 48, 0}, {67, 84, 76, 54, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 52, 69, 128, 74}, {27, 25, 58, 73, 102}};
const int Params::THREAT_BY_BISHOP[2][5] = {{21, 52, 75, 85, 78}, {26, 48, 52, 67, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{19, 53, 57, 128, 105}, {28, 44, 51, 0, 128}};
const int Params::KNIGHT_PST[2][64] = {{-91, -34, -19, -12, -12, -19, -34, -91, -33, -31, -21, 1, 1, -21, -31, -33, -46, -21, -2, 24, 24, -2, -21, -46, -16, 7, 21, 25, 25, 21, 7, -16, -27, -12, 20, 13, 13, 20, -12, -27, -54, -49, -11, -6, -6, -11, -49, -54, -96, -86, -33, -24, -24, -33, -86, -96, -128, -128, -128, -128, -128, -128, -128, -128}, {-81, -45, -33, -33, -33, -33, -45, -81, -38, -29, -29, -18, -18, -29, -29, -38, -43, -28, -19, 2, 2, -19, -28, -43, -20, -10, 6, 16, 16, 6, -10, -20, -38, -32, -14, -8, -8, -14, -32, -38, -55, -39, -25, -24, -24, -25, -39, -55, -76, -52, -48, -36, -36, -48, -52, -76, -128, -71, -79, -68, -68, -79, -71, -128}};
const int Params::BISHOP_PST[2][64] = {{-1, 29, 16, 14, 14, 16, 29, -1, 19, 32, 26, 20, 20, 26, 32, 19, 1, 27, 27, 22, 22, 27, 27, 1, 5, 5, 15, 33, 33, 15, 5, 5, -19, 4, 4, 21, 21, 4, 4, -19, 22, -12, -24, -10, -10, -24, -12, 22, -20, -44, -39, -62, -62, -39, -44, -20, -9, -56, -128, -128, -128, -128, -56, -9}, {-26, -11, -2, -7, -7, -2, -11, -26, -10, -3, -4, 3, 3, -4, -3, -10, -8, 6, 10, 14, 14, 10, 6, -8, -5, 6, 14, 17, 17, 14, 6, -5, -17, -4, -6, 0, 0, -6, -4, -17, 5, -11, -9, -12, -12, -9, -11, 5, -4, -5, -15, -14, -14, -15, -5, -4, 3, -4, -6, -25, -25, -6, -4, 3}};
const int Params::ROOK_PST[2][64] = {{-50, -38, -32, -27, -27, -32, -38, -50, -63, -39, -33, -27, -27, -33, -39, -63, -59, -37, -37, -32, -32, -37, -37, -59, -57, -44, -45, -34, -34, -45, -44, -57, -38, -34, -24, -23, -23, -24, -34, -38, -38, -18, -23, -16, -16, -23, -18, -38, -49, -55, -41, -33, -33, -41, -55, -49, -46, -44, -128, -118, -118, -128, -44, -46}, {-28, -30, -21, -27, -27, -21, -30, -28, -40, -35, -29, -29, -29, -29, -35, -40, -34, -28, -23, -25, -25, -23, -28, -34, -19, -13, -8, -12, -12, -8, -13, -19, -7, -5, -1, -5, -5, -1, -5, -7, -7, -3, -0, -3, -3, -0, -3, -7, -19, -15, -13, -14, -14, -13, -15, -19, -1, 4, 4, -1, -1, 4, 4, -1}};
const int Params::QUEEN_PST[2][64] = {{-3, -5, -8, 3, 3, -8, -5, -3, -9, 7, 9, 11, 11, 9, 7, -9, -6, 6, 12, 3, 3, 12, 6, -6, 7, 14, 13, 14, 14, 13, 14, 7, 16, 16, 21, 21, 21, 21, 16, 16, -6, 15, 24, 18, 18, 24, 15, -6, -13, -18, 3, 4, 4, 3, -18, -13, 16, 11, -15, -42, -42, -15, 11, 16}, {-100, -106, -92, -76, -76, -92, -106, -100, -87, -79, -71, -55, -55, -71, -79, -87, -72, -40, -30, -40, -40, -30, -40, -72, -39, -18, -6, -5, -5, -6, -18, -39, -35, 4, 11, 28, 28, 11, 4, -35, -38, -5, 17, 32, 32, 17, -5, -38, -24, -3, 25, 33, 33, 25, -3, -24, -59, -42, -36, -30, -30, -36, -42, -59}};
const int Params::KING_PST[2][64] = {{18, 33, -28, -10, -10, -28, 33, 18, 55, 56, 0, 20, 20, 0, 56, 55, -36, -12, -84, -128, -128, -84, -12, -36, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 16, -0, -128, -128, -128, -128, -0, 16, -25, -128, 128, -101, -101, 128, -128, -25}, {-28, -24, -30, -35, -35, -30, -24, -28, -12, -19, -26, -26, -26, -26, -19, -12, -21, -22, -20, -20, -20, -20, -22, -21, -17, -12, -6, -4, -4, -6, -12, -17, -4, 6, 15, 15, 15, 15, 6, -4, 4, 17, 20, 18, 18, 20, 17, 4, -11, 7, 10, 9, 9, 10, 7, -11, -51, -13, -10, -8, -8, -10, -13, -51}};
const int Params::KNIGHT_MOBILITY[9] = {-77, -31, -8, 5, 14, 24, 28, 27, 22};
const int Params::BISHOP_MOBILITY[15] = {-51, -25, -10, 1, 12, 20, 25, 29, 34, 37, 40, 42, 56, 37, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-86, -61, -50, -46, -45, -39, -35, -29, -25, -20, -14, -9, 4, 16, 7}, {-96, -67, -48, -32, -15, -10, -9, -5, -0, 5, 11, 17, 22, 15, 11}};
const int Params::QUEEN_MOBILITY[2][24] = {{-84, -29, -17, -5, 2, 8, 14, 20, 26, 31, 32, 35, 37, 37, 41, 42, 42, 52, 62, 69, 89, 95, 96, 96}, {-96, -96, -96, -96, -86, -69, -50, -49, -42, -41, -36, -31, -29, -26, -25, -24, -24, -28, -32, -34, -33, -34, -44, -53}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-73, -39, -21, -11, -8};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 58}, {21, 41}};
const int Params::BISHOP_OUTPOST[2][2] = {{13, 39}, {21, 27}};
const int Params::PAWN_STORM[4][2] = {{0, 26},{10, 6},{34, 1},{3, 0}};

