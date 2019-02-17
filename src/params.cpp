// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 16-Feb-2019 10:44:42 by tuner -n 175 -c 24 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0682673

//

#include "params.h"

const int Params::RB_ADJUST[6] = {56, -6, 100, 52, 46, 77};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -14, -40, -32};
const int Params::QR_ADJUST[5] = {2, 56, 20, -44, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -280, -160};
const int Params::CASTLING[6] = {13, -38, -27, 0, 5, -20};
const int Params::KING_COVER[6][4] = {{8, 23, 13, 15},
{2, 7, 4, 5},
{-7, -21, -12, -14},
{-7, -21, -12, -14},
{-8, -25, -14, -16},
{-15, -45, -26, -30}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R_NO_PAWNS = -232;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -185;
const int Params::MINOR_FOR_PAWNS = 58;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 24;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR = 14;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 42;
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
const int Params::OWN_MINOR_KING_PROXIMITY = 88;
const int Params::OWN_ROOK_KING_PROXIMITY = 36;
const int Params::OWN_QUEEN_KING_PROXIMITY = 17;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 37;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 81;
const int Params::PIECE_THREAT_MM_END = 32;
const int Params::PIECE_THREAT_MR_END = 84;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 10;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 22;
const int Params::PIECE_THREAT_RR_MID = 11;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 34;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 42;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 56;
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
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 28;
const int Params::TWO_ROOKS_ON_7TH_MID = 35;
const int Params::TWO_ROOKS_ON_7TH_END = 64;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 22;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 60;
const int Params::QUEENING_SQUARE_CONTROL_END = 56;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -16;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 24, 54, 82, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={243, 208, 149};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 20, 21, 23, 25, 26, 28, 30, 32, 35, 37, 39, 42, 45, 48, 51, 54, 57, 60, 64, 68, 72, 76, 80, 85, 90, 94, 100, 105, 110, 116, 122, 128, 135, 141, 148, 155, 162, 170, 177, 185, 193, 202, 210, 219, 227, 236, 245, 255, 264, 273, 283, 292, 302, 312, 321, 331, 341, 350, 360, 369, 379, 388, 398, 407, 416, 425, 433, 442, 450, 459, 467, 474, 482, 490, 497, 504, 511, 517, 523, 530, 535, 541, 547, 552, 557, 562, 567, 571, 575, 579, 583, 587, 591, 594, 597, 600, 603, 606, 609, 612, 614, 616, 618, 621, 623, 624, 626, 628, 629, 631, 632, 634, 635, 636, 638, 639, 640, 641, 642, 642, 643, 644, 645, 646};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 17, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 35, 128, 193}, {0, 0, 1, 0, 26, 84, 160, 273}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 17, 96, 0}, {0, 0, 0, 0, 0, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 5, 5, 25, 102, 206, 320}, {0, 0, 0, 0, 37, 91, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 27, 12, 36, 97, 188}, {0, 0, 0, 10, 11, 26, 63, 68}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-34, -23, -30, -7, -33, -3, -20, -25, -3, -27, -57, -19, 0, -18, 0, -42, -11, -35, -23, -50, 0}, {0, 0, 0, -4, -12, 0, 0, -10, -6, -17, -25, -7, -3, -12, -15, -18, -18, -29, -52, -42, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-60, -75, -51, -49, -44, -50, -58, -52, -26, -30, -32, -43, -8, 0, -3, -32, 0, 0, -66, -37, -83}, {-11, -3, 0, -7, -3, -2, -16, -7, -6, -5, -4, -28, -13, -10, 0, -52, -22, -13, -53, -36, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-41, 0, -16, -10, -10, -16, 0, -41}, {-42, -19, -25, -20, -20, -25, -19, -42}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -28, 0, -8, -8, 0, -28, 0}, {-88, -70, -37, -40, -40, -37, -70, -88}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -0, -17, -23, -23, -17, -0, 0}, {-5, -9, -11, -17, -17, -11, -9, -5}};

const int Params::KNIGHT_PST[2][64] = {{-73, -14, 1, 8, 8, 1, -14, -73, -12, -10, -1, 19, 19, -1, -10, -12, -31, -4, 15, 40, 40, 15, -4, -31, -4, 14, 34, 36, 36, 34, 14, -4, -16, 2, 34, 30, 30, 34, 2, -16, -39, -34, 11, 16, 16, 11, -34, -39, -92, -70, -15, -9, -9, -15, -70, -92, -128, -128, -128, -128, -128, -128, -128, -128}, {-70, -32, -21, -23, -23, -21, -32, -70, -29, -17, -16, -7, -7, -16, -17, -29, -39, -20, -10, 12, 12, -10, -20, -39, -14, -3, 17, 26, 26, 17, -3, -14, -26, -17, 4, 8, 8, 4, -17, -26, -46, -28, -9, -5, -5, -9, -28, -46, -65, -42, -39, -24, -24, -39, -42, -65, -128, -74, -71, -53, -53, -71, -74, -128}};
const int Params::BISHOP_PST[2][64] = {{21, 50, 38, 36, 36, 38, 50, 21, 42, 54, 45, 38, 38, 45, 54, 42, 19, 47, 46, 39, 39, 46, 47, 19, 7, 17, 27, 47, 47, 27, 17, 7, -29, -4, 2, 30, 30, 2, -4, -29, 10, -21, -34, -20, -20, -34, -21, 10, -25, -59, -57, -91, -91, -57, -59, -25, -25, -85, -128, -128, -128, -128, -85, -25}, {-9, 4, 10, 5, 5, 10, 4, -9, 11, 14, 9, 16, 16, 9, 14, 11, 4, 16, 22, 25, 25, 22, 16, 4, 3, 12, 26, 31, 31, 26, 12, 3, -17, -2, -2, 8, 8, -2, -2, -17, 4, -9, -9, -9, -9, -9, -9, 4, -5, -10, -17, -21, -21, -17, -10, -5, -1, -2, -6, -26, -26, -6, -2, -1}};
const int Params::ROOK_PST[2][64] = {{-59, -46, -40, -37, -37, -40, -46, -59, -72, -46, -42, -36, -36, -42, -46, -72, -70, -46, -46, -44, -44, -46, -46, -70, -68, -54, -50, -42, -42, -50, -54, -68, -47, -39, -26, -24, -24, -26, -39, -47, -34, -9, -18, -7, -7, -18, -9, -34, -32, -38, -13, -11, -11, -13, -38, -32, -24, -19, -111, -80, -80, -111, -19, -24}, {-19, -20, -11, -17, -17, -11, -20, -19, -29, -23, -15, -15, -15, -15, -23, -29, -26, -20, -13, -16, -16, -13, -20, -26, -15, -6, -0, -5, -5, -0, -6, -15, 1, 3, 8, 5, 5, 8, 3, 1, 7, 10, 16, 14, 14, 16, 10, 7, -5, -2, 4, 5, 5, 4, -2, -5, 17, 16, 15, 12, 12, 15, 16, 17}};
const int Params::QUEEN_PST[2][64] = {{11, 11, 7, 19, 19, 7, 11, 11, 4, 21, 23, 24, 24, 23, 21, 4, 3, 17, 22, 12, 12, 22, 17, 3, 16, 20, 22, 22, 22, 22, 20, 16, 19, 21, 25, 33, 33, 25, 21, 19, 9, 29, 33, 29, 29, 33, 29, 9, 12, -6, 16, 18, 18, 16, -6, 12, 22, 22, -0, -14, -14, -0, 22, 22}, {-77, -91, -75, -58, -58, -75, -91, -77, -64, -59, -52, -36, -36, -52, -59, -64, -53, -25, -13, -23, -23, -13, -25, -53, -24, -4, 5, 7, 7, 5, -4, -24, -16, 17, 23, 38, 38, 23, 17, -16, -21, 13, 34, 51, 51, 34, 13, -21, -3, 17, 42, 54, 54, 42, 17, -3, -28, -11, -13, -10, -10, -13, -11, -28}};
const int Params::KING_PST[2][64] = {{44, 61, -1, 21, 21, -1, 61, 44, 84, 86, 31, 52, 52, 31, 86, 84, 1, 24, -56, -116, -116, -56, 24, 1, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -100, -128, -128, -128, -128, -128, -128, -100, 128, 10, -128, -128, -128, -128, 10, 128, -128, -103, 128, -128, -128, 128, -103, -128}, {-47, -48, -53, -58, -58, -53, -48, -47, -36, -44, -50, -50, -50, -50, -44, -36, -46, -50, -47, -45, -45, -47, -50, -46, -43, -40, -35, -33, -33, -35, -40, -43, -30, -24, -17, -16, -16, -17, -24, -30, -23, -12, -11, -11, -11, -11, -12, -23, -32, -22, -18, -20, -20, -18, -22, -32, -62, -37, -36, -33, -33, -36, -37, -62}};
const int Params::KNIGHT_MOBILITY[9] = {-63, -18, 3, 15, 24, 33, 36, 34, 25};
const int Params::BISHOP_MOBILITY[15] = {-41, -14, 1, 12, 22, 30, 34, 37, 41, 44, 45, 47, 56, 39, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-71, -47, -35, -31, -29, -23, -18, -10, -6, -3, -1, 2, 13, 21, 16}, {-87, -51, -31, -15, 2, 9, 9, 12, 19, 23, 27, 32, 34, 23, 15}};
const int Params::QUEEN_MOBILITY[2][24] = {{-86, -27, -13, -2, 4, 11, 18, 24, 29, 34, 36, 37, 40, 40, 45, 47, 46, 61, 73, 87, 96, 96, 96, 96}, {-96, -96, -96, -95, -48, -33, -17, -11, -3, 0, 6, 11, 17, 21, 25, 24, 29, 25, 25, 26, 27, 31, 22, 17}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-87, -49, -28, -16, -11};
const int Params::KNIGHT_OUTPOST[2][2] = {{25, 62}, {19, 32}};
const int Params::BISHOP_OUTPOST[2][2] = {{34, 63}, {35, 26}};
const int Params::PAWN_STORM[4][2] = {{0, 21},{9, 0},{32, 0},{3, 0}};

