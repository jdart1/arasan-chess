// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 05-Dec-2018 09:31:26 by tuner -n 175 -c 24 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0681167

//

#include "params.h"

const int Params::RB_ADJUST[6] = {47, -6, 92, 44, 41, 78};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -11, -42, -32};
const int Params::QR_ADJUST[5] = {18, 64, 26, -32, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -271, -153};
const int Params::CASTLING[6] = {13, -38, -26, 0, 5, -21};
const int Params::KING_COVER[6][4] = {{8, 23, 13, 15},
{3, 8, 5, 5},
{-6, -19, -11, -12},
{-7, -20, -12, -14},
{-8, -25, -15, -17},
{-15, -45, -26, -30}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -228;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -184;
const int Params::MINOR_FOR_PAWNS = 64;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 38;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 6;
const int Params::PAWN_ATTACK_FACTOR2 = 8;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 43;
const int Params::ROOK_ATTACK_FACTOR = 60;
const int Params::ROOK_ATTACK_BOOST = 22;
const int Params::ROOK_ATTACK_BOOST2 = 41;
const int Params::QUEEN_ATTACK_FACTOR = 61;
const int Params::QUEEN_ATTACK_BOOST = 45;
const int Params::QUEEN_ATTACK_BOOST2 = 63;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 6;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 136;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 86;
const int Params::OWN_ROOK_KING_PROXIMITY = 36;
const int Params::OWN_QUEEN_KING_PROXIMITY = 17;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 38;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 81;
const int Params::PIECE_THREAT_MM_END = 33;
const int Params::PIECE_THREAT_MR_END = 85;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 10;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 21;
const int Params::PIECE_THREAT_RR_MID = 11;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 33;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 42;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 55;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -24;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -22;
const int Params::WEAK_ON_OPEN_FILE_END = -17;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 28;
const int Params::TWO_ROOKS_ON_7TH_MID = 31;
const int Params::TWO_ROOKS_ON_7TH_END = 64;
const int Params::ROOK_ON_OPEN_FILE_MID = 34;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 4;
const int Params::ROOK_BEHIND_PP_END = 21;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 62;
const int Params::QUEENING_SQUARE_CONTROL_END = 56;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -17;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 24, 54, 81, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={247, 212, 146};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21, 22, 24, 26, 28, 30, 32, 34, 37, 39, 42, 44, 47, 50, 53, 57, 60, 64, 68, 72, 76, 80, 85, 90, 95, 100, 105, 111, 117, 123, 129, 135, 142, 149, 156, 163, 171, 179, 187, 195, 203, 212, 220, 229, 238, 247, 257, 266, 275, 285, 295, 304, 314, 323, 333, 343, 352, 362, 371, 381, 390, 399, 408, 417, 426, 434, 443, 451, 459, 467, 474, 482, 489, 496, 503, 509, 515, 522, 527, 533, 538, 544, 549, 554, 558, 563, 567, 571, 575, 578, 582, 585, 589, 592, 594, 597, 600, 602, 605, 607, 609, 611, 613, 615, 617, 618, 620, 621, 623, 624, 625, 626, 628, 629, 630, 631, 631, 632, 633, 634, 635, 635};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 0, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 35, 128, 187}, {0, 0, 0, 0, 26, 83, 160, 272}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 16, 96, 0}, {0, 0, 0, 0, 0, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 5, 7, 23, 102, 212, 320}, {0, 0, 0, 0, 38, 89, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 28, 12, 35, 96, 190}, {0, 0, 0, 9, 10, 26, 61, 68}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-35, -25, -34, -9, -34, -6, -21, -26, -5, -29, -51, -19, 0, -21, 0, -41, -13, -35, -22, -55, 0}, {0, 0, 0, -3, -12, 0, 0, -9, -7, -18, -25, -7, -3, -12, -14, -17, -17, -30, -53, -42, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-60, -75, -54, -52, -47, -51, -59, -54, -26, -30, -32, -43, -8, 0, -2, -32, 0, 0, -67, -36, -83}, {-10, -4, 0, -7, -2, -1, -16, -8, -7, -5, -4, -27, -12, -9, 0, -51, -21, -13, -53, -37, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-40, 0, -15, -11, -11, -15, 0, -40}, {-42, -19, -25, -20, -20, -25, -19, -42}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -27, -1, -13, -13, -1, -27, 0}, {-85, -72, -38, -40, -40, -38, -72, -85}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -16, -22, -22, -16, 0, 0}, {-5, -9, -11, -17, -17, -11, -9, -5}};

const int Params::KNIGHT_PST[2][64] = {{-83, -23, -8, -2, -2, -8, -23, -83, -21, -19, -11, 10, 10, -11, -19, -21, -40, -13, 5, 30, 30, 5, -13, -40, -14, 4, 25, 27, 27, 25, 4, -14, -25, -7, 25, 21, 21, 25, -7, -25, -49, -44, 1, 6, 6, 1, -44, -49, -102, -81, -25, -18, -18, -25, -81, -102, -128, -128, -128, -128, -128, -128, -128, -128}, {-75, -37, -25, -28, -28, -25, -37, -75, -34, -22, -21, -11, -11, -21, -22, -34, -44, -24, -15, 8, 8, -15, -24, -44, -19, -7, 12, 22, 22, 12, -7, -19, -31, -21, -0, 4, 4, -0, -21, -31, -51, -33, -14, -9, -9, -14, -33, -51, -71, -46, -44, -29, -29, -44, -46, -71, -128, -78, -75, -58, -58, -75, -78, -128}};
const int Params::BISHOP_PST[2][64] = {{16, 45, 33, 31, 31, 33, 45, 16, 37, 48, 39, 33, 33, 39, 48, 37, 15, 42, 40, 34, 34, 40, 42, 15, 3, 12, 23, 42, 42, 23, 12, 3, -33, -9, -2, 27, 27, -2, -9, -33, 5, -25, -40, -24, -24, -40, -25, 5, -31, -65, -61, -96, -96, -61, -65, -31, -17, -89, -128, -128, -128, -128, -89, -17}, {-8, 5, 11, 6, 6, 11, 5, -8, 12, 15, 10, 17, 17, 10, 15, 12, 5, 17, 23, 26, 26, 23, 17, 5, 4, 13, 27, 32, 32, 27, 13, 4, -16, -1, -1, 9, 9, -1, -1, -16, 5, -9, -8, -9, -9, -8, -9, 5, -4, -10, -17, -20, -20, -17, -10, -4, -0, -1, -6, -26, -26, -6, -1, -0}};
const int Params::ROOK_PST[2][64] = {{-50, -37, -31, -28, -28, -31, -37, -50, -63, -37, -33, -27, -27, -33, -37, -63, -62, -37, -38, -35, -35, -38, -37, -62, -60, -45, -42, -33, -33, -42, -45, -60, -38, -30, -17, -14, -14, -17, -30, -38, -26, 1, -9, 3, 3, -9, 1, -26, -23, -29, -3, -1, -1, -3, -29, -23, -14, -8, -106, -69, -69, -106, -8, -14}, {-6, -7, 2, -4, -4, 2, -7, -6, -16, -10, -2, -2, -2, -2, -10, -16, -12, -7, 0, -2, -2, 0, -7, -12, -1, 7, 13, 8, 8, 13, 7, -1, 14, 16, 22, 18, 18, 22, 16, 14, 20, 24, 29, 28, 28, 29, 24, 20, 8, 11, 17, 18, 18, 17, 11, 8, 30, 30, 29, 26, 26, 29, 30, 30}};
const int Params::QUEEN_PST[2][64] = {{33, 34, 29, 41, 41, 29, 34, 33, 26, 44, 46, 47, 47, 46, 44, 26, 26, 40, 44, 34, 34, 44, 40, 26, 38, 42, 44, 44, 44, 44, 42, 38, 41, 44, 48, 57, 57, 48, 44, 41, 32, 50, 55, 52, 52, 55, 50, 32, 34, 15, 39, 42, 42, 39, 15, 34, 46, 47, 23, 9, 9, 23, 47, 46}, {-48, -60, -45, -29, -29, -45, -60, -48, -34, -30, -23, -6, -6, -23, -30, -34, -23, 6, 17, 7, 7, 17, 6, -23, 8, 28, 35, 37, 37, 35, 28, 8, 15, 48, 53, 68, 68, 53, 48, 15, 11, 44, 65, 81, 81, 65, 44, 11, 27, 47, 73, 84, 84, 73, 47, 27, 4, 19, 17, 21, 21, 17, 19, 4}};
const int Params::KING_PST[2][64] = {{42, 59, -2, 18, 18, -2, 59, 42, 80, 81, 29, 49, 49, 29, 81, 80, -7, 16, -56, -113, -113, -56, 16, -7, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -81, -116, -128, -128, -128, -128, -116, -81, -23, 45, -128, -128, -128, -128, 45, -23, -128, -126, 128, -128, -128, 128, -126, -128}, {-61, -62, -66, -71, -71, -66, -62, -61, -50, -57, -64, -64, -64, -64, -57, -50, -59, -63, -60, -59, -59, -60, -63, -59, -57, -54, -49, -46, -46, -49, -54, -57, -44, -38, -32, -31, -31, -32, -38, -44, -37, -26, -26, -26, -26, -26, -26, -37, -46, -36, -33, -34, -34, -33, -36, -46, -75, -50, -49, -47, -47, -49, -50, -75}};

const int Params::KNIGHT_MOBILITY[9] = {-71, -27, -6, 6, 15, 23, 26, 24, 17};
const int Params::BISHOP_MOBILITY[15] = {-55, -28, -14, -3, 8, 15, 19, 22, 26, 30, 30, 32, 43, 26, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-61, -37, -25, -20, -19, -13, -8, 0, 4, 7, 9, 12, 23, 31, 26}, {-67, -30, -10, 7, 23, 30, 30, 33, 40, 45, 49, 54, 56, 45, 36}};
const int Params::QUEEN_MOBILITY[2][24] = {{-63, -4, 9, 19, 25, 32, 39, 45, 50, 54, 56, 57, 60, 61, 64, 68, 66, 80, 92, 96, 96, 96, 96, 96}, {-96, -96, -78, -37, 12, 25, 42, 48, 56, 60, 65, 71, 77, 80, 85, 83, 89, 86, 85, 88, 91, 93, 84, 79}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-94, -55, -34, -22, -17};

const int Params::KNIGHT_OUTPOST[2][2] = {{25, 62}, {20, 33}};
const int Params::BISHOP_OUTPOST[2][2] = {{34, 62}, {35, 27}};


