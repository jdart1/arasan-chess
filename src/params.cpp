// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 14-Dec-2018 11:31:35 by tuner -n 175 -c 24 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0683726

//

#include "params.h"

const int Params::RB_ADJUST[6] = {49, -6, 94, 47, 42, 70};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -1, -28, -32};
const int Params::QR_ADJUST[5] = {14, 73, 43, -1, -95};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -266, -146};
const int Params::CASTLING[6] = {13, -38, -23, 0, 8, -18};
const int Params::KING_COVER[6][4] = {{4, 11, 6, 8},
{-0, -0, -0, -0},
{-7, -20, -12, -15},
{-3, -7, -4, -5},
{-12, -31, -18, -23},
{-15, -41, -24, -31}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 93;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -217;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -174;
const int Params::MINOR_FOR_PAWNS = 59;
const int Params::ENDGAME_PAWN_ADVANTAGE = 30;
const int Params::PAWN_ENDGAME1 = 44;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 30;
const int Params::PAWN_ATTACK_FACTOR2 = 30;
const int Params::PAWN_ATTACK_FACTOR3 = 0;
const int Params::MINOR_ATTACK_FACTOR = 38;
const int Params::MINOR_ATTACK_BOOST = 34;
const int Params::ROOK_ATTACK_FACTOR = 50;
const int Params::ROOK_ATTACK_BOOST = 17;
const int Params::ROOK_ATTACK_BOOST2 = 33;
const int Params::QUEEN_ATTACK_FACTOR = 55;
const int Params::QUEEN_ATTACK_BOOST = 34;
const int Params::QUEEN_ATTACK_BOOST2 = 50;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 9;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 133;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 96;
const int Params::OWN_ROOK_KING_PROXIMITY = 46;
const int Params::OWN_QUEEN_KING_PROXIMITY = 20;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 31;
const int Params::PIECE_THREAT_MR_MID = 87;
const int Params::PIECE_THREAT_MQ_MID = 81;
const int Params::PIECE_THREAT_MM_END = 33;
const int Params::PIECE_THREAT_MR_END = 80;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 11;
const int Params::MINOR_PAWN_THREAT_END = 25;
const int Params::PIECE_THREAT_RM_MID = 28;
const int Params::PIECE_THREAT_RR_MID = 22;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 34;
const int Params::PIECE_THREAT_RR_END = 7;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 29;
const int Params::ENDGAME_KING_THREAT = 42;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 40;
const int Params::BISHOP_PAIR_END = 55;
const int Params::BISHOP_PAWN_PLACEMENT_END = -1;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -25;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -4;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -22;
const int Params::WEAK_ON_OPEN_FILE_END = -17;
const int Params::SPACE = 7;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 27;
const int Params::TWO_ROOKS_ON_7TH_MID = 24;
const int Params::TWO_ROOKS_ON_7TH_END = 64;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 4;
const int Params::ROOK_BEHIND_PP_END = 23;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 12;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 65;
const int Params::QUEENING_SQUARE_CONTROL_END = 55;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -13;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 22, 51, 78, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={259, 214, 150};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-3, -2, -2, -2, -2, -1, -1, 0, 0, 0, 0, 0, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8, 9, 11, 12, 13, 15, 16, 18, 20, 22, 24, 26, 28, 31, 34, 37, 40, 43, 46, 50, 54, 58, 63, 68, 73, 78, 83, 89, 96, 102, 109, 116, 124, 132, 140, 149, 158, 167, 177, 187, 198, 208, 219, 231, 242, 254, 266, 279, 291, 304, 316, 329, 342, 355, 367, 380, 393, 405, 417, 429, 441, 453, 464, 475, 486, 497, 507, 517, 526, 535, 544, 552, 560, 568, 575, 582, 589, 595, 601, 607, 612, 617, 622, 626, 631, 635, 638, 642, 645, 648, 651, 654, 656, 659, 661, 663, 665, 667, 669, 670, 672, 673, 674, 676, 677, 678, 679, 680, 681, 681, 682, 683, 684, 684, 685, 685, 686, 686, 687, 687, 687, 688, 688, 688, 689, 689, 689, 689, 689};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 0, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 4, 40, 128, 196}, {0, 0, 1, 0, 25, 82, 160, 270}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 20, 96, 0}, {0, 0, 0, 0, 0, 15, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 2, 13, 18, 102, 236, 320}, {0, 0, 0, 0, 37, 86, 254, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 33, 15, 39, 101, 185}, {0, 0, 0, 9, 10, 25, 59, 68}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-31, -25, -30, -10, -33, 0, -21, -25, -6, -27, -25, -20, 0, -19, -5, -33, -17, -50, -23, -55, 0}, {0, 0, 0, -4, -12, 0, 0, -9, -5, -18, -26, -4, -2, -8, -12, -17, -17, -29, -53, -43, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-55, -68, -50, -48, -44, -48, -58, -51, -22, -31, -31, -46, -7, 0, -8, -32, 0, 0, -61, -28, -83}, {-10, -3, 0, -8, -3, -2, -16, -7, -7, -5, -4, -26, -11, -7, 0, -51, -20, -12, -54, -38, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-34, 0, -10, -9, -9, -10, 0, -34}, {-41, -18, -24, -20, -20, -24, -18, -41}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -16, 0, -3, -3, 0, -16, 0}, {-88, -69, -36, -40, -40, -36, -69, -88}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -18, -23, -23, -18, 0, 0}, {-5, -9, -11, -17, -17, -11, -9, -5}};

const int Params::KNIGHT_PST[2][64] = {{-90, -26, -12, -6, -6, -12, -26, -90, -21, -22, -13, 7, 7, -13, -22, -21, -43, -16, 3, 27, 27, 3, -16, -43, -17, -2, 20, 22, 22, 20, -2, -17, -30, -11, 20, 16, 16, 20, -11, -30, -50, -48, -1, 2, 2, -1, -48, -50, -105, -82, -32, -23, -23, -32, -82, -105, -128, -128, -128, -128, -128, -128, -128, -128}, {-76, -40, -26, -29, -29, -26, -40, -76, -36, -23, -24, -15, -15, -24, -23, -36, -46, -28, -18, 5, 5, -18, -28, -46, -21, -10, 10, 19, 19, 10, -10, -21, -36, -27, -7, -3, -3, -7, -27, -36, -59, -39, -21, -16, -16, -21, -39, -59, -77, -53, -49, -35, -35, -49, -53, -77, -128, -84, -81, -66, -66, -81, -84, -128}};
const int Params::BISHOP_PST[2][64] = {{10, 43, 31, 30, 30, 31, 43, 10, 35, 46, 37, 30, 30, 37, 46, 35, 11, 41, 39, 32, 32, 39, 41, 11, -2, 10, 19, 39, 39, 19, 10, -2, -35, -10, -7, 22, 22, -7, -10, -35, 4, -27, -37, -27, -27, -37, -27, 4, -33, -61, -64, -96, -96, -64, -61, -33, -19, -88, -128, -128, -128, -128, -88, -19}, {-9, 4, 9, 5, 5, 9, 4, -9, 11, 13, 8, 16, 16, 8, 13, 11, 3, 16, 21, 24, 24, 21, 16, 3, 4, 12, 25, 30, 30, 25, 12, 4, -18, -3, -4, 6, 6, -4, -3, -18, 2, -10, -11, -11, -11, -11, -10, 2, -6, -12, -20, -22, -22, -20, -12, -6, -2, -3, -7, -28, -28, -7, -3, -2}};
const int Params::ROOK_PST[2][64] = {{-47, -35, -29, -25, -25, -29, -35, -47, -59, -35, -31, -24, -24, -31, -35, -59, -58, -34, -34, -32, -32, -34, -34, -58, -56, -42, -39, -30, -30, -39, -42, -56, -37, -30, -17, -12, -12, -17, -30, -37, -24, 0, -8, 5, 5, -8, 0, -24, -21, -27, -2, 2, 2, -2, -27, -21, -11, -6, -76, -55, -55, -76, -6, -11}, {-10, -11, -2, -8, -8, -2, -11, -10, -20, -13, -6, -5, -5, -6, -13, -20, -16, -10, -4, -6, -6, -4, -10, -16, -4, 4, 9, 4, 4, 9, 4, -4, 11, 13, 18, 15, 15, 18, 13, 11, 17, 20, 25, 24, 24, 25, 20, 17, 5, 9, 14, 16, 16, 14, 9, 5, 25, 25, 25, 22, 22, 25, 25, 25}};
const int Params::QUEEN_PST[2][64] = {{17, 19, 14, 27, 27, 14, 19, 17, 12, 29, 31, 32, 32, 31, 29, 12, 10, 25, 29, 19, 19, 29, 25, 10, 23, 27, 28, 28, 28, 28, 27, 23, 25, 27, 31, 39, 39, 31, 27, 25, 14, 31, 37, 32, 32, 37, 31, 14, 17, 3, 23, 21, 21, 23, 3, 17, 31, 32, 12, 2, 2, 12, 32, 31}, {-55, -69, -51, -36, -36, -51, -69, -55, -40, -37, -29, -9, -9, -29, -37, -40, -23, -0, 11, 1, 1, 11, -0, -23, 5, 22, 28, 31, 31, 28, 22, 5, 10, 41, 48, 61, 61, 48, 41, 10, 7, 39, 56, 74, 74, 56, 39, 7, 20, 39, 67, 77, 77, 67, 39, 20, 0, 15, 15, 17, 17, 15, 15, 0}};
const int Params::KING_PST[2][64] = {{42, 59, -5, 12, 12, -5, 59, 42, 68, 68, 5, 11, 11, 5, 68, 68, -91, -61, -128, -128, -128, -128, -61, -91, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 33, -74, -128, -128, -128, -128, -74, 33, -128, -113, 128, -110, -110, 128, -113, -128}, {-40, -40, -45, -49, -49, -45, -40, -40, -29, -36, -42, -42, -42, -42, -36, -29, -38, -42, -39, -38, -38, -39, -42, -38, -37, -34, -29, -26, -26, -29, -34, -37, -23, -18, -11, -10, -10, -11, -18, -23, -17, -6, -5, -5, -5, -5, -6, -17, -26, -15, -12, -13, -13, -12, -15, -26, -51, -29, -28, -27, -27, -28, -29, -51}};

const int Params::KNIGHT_MOBILITY[9] = {-71, -26, -4, 8, 17, 26, 28, 27, 19};
const int Params::BISHOP_MOBILITY[15] = {-56, -27, -13, -3, 8, 15, 19, 22, 27, 30, 32, 34, 43, 27, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-60, -37, -25, -20, -18, -12, -7, 1, 5, 8, 10, 13, 23, 29, 25}, {-72, -35, -14, 2, 18, 25, 25, 29, 36, 40, 45, 49, 52, 42, 34}};
const int Params::QUEEN_MOBILITY[2][24] = {{-79, -14, 1, 11, 17, 23, 30, 36, 41, 45, 47, 48, 52, 51, 56, 58, 57, 73, 82, 96, 96, 96, 96, 96}, {-96, -96, -85, -56, -6, 11, 30, 36, 44, 46, 53, 57, 64, 68, 73, 72, 76, 73, 72, 72, 76, 77, 71, 66}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-73, -37, -16, -4, 2};

const int Params::KNIGHT_OUTPOST[2][2] = {{22, 61}, {25, 37}};
const int Params::BISHOP_OUTPOST[2][2] = {{30, 61}, {36, 28}};


