// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 09-Oct-2019 02:47:56 by tuner -c 24 -n 175 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0672719

//

#include "params.h"

const int Params::RB_ADJUST[6] = {51, -6, 93, 42, 39, 78};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -32, -58, -55};
const int Params::QR_ADJUST[5] = {10, 70, 24, -47, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -256, -139};
const int Params::MINOR_FOR_PAWNS[6] = {51, 55, 72, 90, 112, 123};
const int Params::CASTLING[6] = {13, -38, -30, 0, 4, -24};
const int Params::KING_COVER[6][4] = {{8, 25, 16, 17},
{1, 4, 2, 2},
{-8, -25, -16, -17},
{-8, -23, -15, -15},
{-11, -32, -21, -21},
{-17, -51, -32, -33}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 71;
const int Params::KRMINOR_VS_R_NO_PAWNS = -197;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -157;
const int Params::ENDGAME_PAWN_ADVANTAGE = 29;
const int Params::PAWN_ENDGAME1 = 40;
const int Params::PAWN_ATTACK_FACTOR = 12;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 23;
const int Params::ROOK_ATTACK_FACTOR = 55;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 60;
const int Params::QUEEN_ATTACK_BOOST = 41;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 112;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 6;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 96;
const int Params::OWN_ROOK_KING_PROXIMITY = 35;
const int Params::OWN_QUEEN_KING_PROXIMITY = 18;
const int Params::PAWN_PUSH_THREAT_MID = 30;
const int Params::PAWN_PUSH_THREAT_END = 14;
const int Params::ENDGAME_KING_THREAT = 57;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 38;
const int Params::BISHOP_PAIR_END = 57;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -22;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -5;
const int Params::WEAK_PAWN_END = -4;
const int Params::WEAK_ON_OPEN_FILE_MID = -21;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 3;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 23;
const int Params::TWO_ROOKS_ON_7TH_MID = 63;
const int Params::TWO_ROOKS_ON_7TH_END = 60;
const int Params::ROOK_ON_OPEN_FILE_MID = 31;
const int Params::ROOK_ON_OPEN_FILE_END = 10;
const int Params::ROOK_BEHIND_PP_MID = 9;
const int Params::ROOK_BEHIND_PP_END = 23;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 9;
const int Params::QUEENING_SQUARE_CONTROL_MID = 65;
const int Params::QUEENING_SQUARE_CONTROL_END = 61;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -17;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -45;
const int Params::SIDE_PROTECTED_PAWN = -4;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 25, 56, 82, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={236, 207, 162};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-3, -3, -2, -2, -2, -2, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 20, 22, 23, 25, 27, 29, 32, 34, 36, 39, 42, 45, 48, 51, 54, 58, 62, 66, 70, 74, 79, 84, 89, 94, 99, 105, 111, 117, 123, 130, 137, 144, 151, 159, 167, 175, 183, 191, 200, 209, 218, 227, 237, 246, 256, 265, 275, 285, 295, 305, 315, 325, 335, 345, 355, 364, 374, 383, 393, 402, 411, 420, 429, 437, 445, 453, 461, 469, 476, 483, 490, 497, 503, 509, 515, 521, 526, 532, 537, 541, 546, 550, 554, 558, 562, 566, 569, 572, 575, 578, 581, 584, 586, 589, 591, 593, 595, 597, 599, 600, 602, 603, 605, 606, 607, 608, 610, 611, 612, 613, 613, 614, 615, 616, 616};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 34, 122, 198}, {0, 0, 2, 0, 26, 85, 160, 262}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 18, 96, 0}, {0, 0, 0, 0, 2, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 5, 13, 24, 102, 182, 320}, {0, 0, 0, 0, 42, 91, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 27, 9, 35, 93, 175}, {0, 0, 0, 12, 15, 32, 68, 90}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-30, -27, -33, -5, -28, -13, -22, -31, -5, -24, -27, -18, 0, -17, -3, -30, -4, -32, -7, -33, 0}, {0, 0, 0, -8, -11, -4, 0, -9, -6, -19, -25, -7, -6, -13, -14, -18, -19, -26, -51, -39, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-50, -66, -47, -40, -36, -35, -47, -41, -21, -26, -28, -30, -1, 0, -3, -19, 0, 0, -48, -24, -83}, {-9, -1, 0, -4, -4, 0, -12, -2, -1, -4, -3, -24, -9, -7, -1, -51, -21, -13, -56, -32, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-40, -0, -14, -10, -10, -14, -0, -40}, {-39, -17, -20, -14, -14, -20, -17, -39}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -21, 0, 0, 0, 0, -21, 0}, {-94, -62, -30, -31, -31, -30, -62, -94}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -2, -19, -23, -23, -19, -2, 0}, {-9, -13, -15, -22, -22, -15, -13, -9}};

const int Params::THREAT_BY_PAWN[2][4] = {{64, 87, 84, 85}, {96, 64, 79, 69}};
const int Params::THREAT_BY_MINOR[2][4] = {{15, 41, 59, 96}, {28, 36, 49, 73}};
const int Params::THREAT_BY_ROOK[2][4] = {{23, 31, 40, 96}, {20, 25, 28, 0}};
const int Params::KNIGHT_PST[2][64] = {{-128, -49, -30, -29, -29, -30, -49, -128, -48, -45, -32, -11, -11, -32, -45, -48, -58, -26, -5, 18, 18, -5, -26, -58, -31, 1, 16, 18, 18, 16, 1, -31, -37, -21, 15, 8, 8, 15, -21, -37, -65, -49, -6, -5, -5, -6, -49, -65, -107, -90, -32, -26, -26, -32, -90, -107, -128, -128, -128, -128, -128, -128, -128, -128}, {-112, -67, -54, -54, -54, -54, -67, -112, -62, -49, -47, -38, -38, -47, -49, -62, -64, -44, -33, -15, -15, -33, -44, -64, -41, -26, -8, 1, 1, -8, -26, -41, -51, -43, -22, -18, -18, -22, -43, -51, -68, -50, -34, -34, -34, -34, -50, -68, -85, -64, -62, -52, -52, -62, -64, -85, -128, -86, -93, -82, -82, -93, -86, -128}};
const int Params::BISHOP_PST[2][64] = {{6, 35, 25, 22, 22, 25, 35, 6, 27, 39, 34, 27, 27, 34, 39, 27, 9, 39, 39, 32, 32, 39, 39, 9, 8, 18, 26, 39, 39, 26, 18, 8, -9, 8, 17, 28, 28, 17, 8, -9, 31, 0, -4, 1, 1, -4, 0, 31, -8, -40, -31, -68, -68, -31, -40, -8, -4, -46, -128, -128, -128, -128, -46, -4}, {-26, -10, -4, -9, -9, -4, -10, -26, -7, -5, -6, 1, 1, -6, -5, -7, -10, 7, 10, 14, 14, 10, 7, -10, -7, 6, 15, 17, 17, 15, 6, -7, -14, 0, 0, 5, 5, 0, 0, -14, 7, -8, -8, -8, -8, -8, -8, 7, -1, -7, -17, -19, -19, -17, -7, -1, 2, 2, -2, -23, -23, -2, 2, 2}};
const int Params::ROOK_PST[2][64] = {{-52, -41, -35, -31, -31, -35, -41, -52, -65, -43, -36, -31, -31, -36, -43, -65, -63, -41, -40, -35, -35, -40, -41, -63, -64, -48, -50, -39, -39, -50, -48, -64, -46, -40, -30, -27, -27, -30, -40, -46, -40, -20, -25, -16, -16, -25, -20, -40, -45, -51, -37, -28, -28, -37, -51, -45, -44, -47, -128, -128, -128, -128, -47, -44}, {-30, -33, -25, -31, -31, -25, -33, -30, -41, -37, -29, -30, -30, -29, -37, -41, -34, -29, -24, -26, -26, -24, -29, -34, -20, -14, -9, -13, -13, -9, -14, -20, -6, -4, 0, -3, -3, 0, -4, -6, -4, 1, 5, 2, 2, 5, 1, -4, -16, -12, -9, -8, -8, -9, -12, -16, 5, 5, 4, -1, -1, 4, 5, 5}};
const int Params::QUEEN_PST[2][64] = {{-4, -5, -6, 7, 7, -6, -5, -4, -8, 8, 11, 14, 14, 11, 8, -8, -5, 8, 15, 6, 6, 15, 8, -5, 9, 17, 15, 13, 13, 15, 17, 9, 20, 20, 24, 19, 19, 24, 20, 20, -11, 9, 18, 8, 8, 18, 9, -11, -12, -26, -6, -4, -4, -6, -26, -12, 5, -3, -34, -51, -51, -34, -3, 5}, {-102, -110, -94, -80, -80, -94, -110, -102, -91, -80, -72, -57, -57, -72, -80, -91, -74, -40, -31, -42, -42, -31, -40, -74, -45, -22, -8, -7, -7, -8, -22, -45, -39, 1, 11, 26, 26, 11, 1, -39, -40, -8, 15, 33, 33, 15, -8, -40, -25, -2, 25, 33, 33, 25, -2, -25, -60, -47, -40, -32, -32, -40, -47, -60}};
const int Params::KING_PST[2][64] = {{16, 32, -26, -10, -10, -26, 32, 16, 55, 54, 3, 22, 22, 3, 54, 55, -37, -10, -74, -128, -128, -74, -10, -37, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -107, -128, -128, -128, -128, -107, -128, 111, -32, -128, -128, -128, -128, -32, 111, -118, -128, 128, 7, 7, 128, -128, -118}, {-24, -19, -24, -30, -30, -24, -19, -24, -9, -15, -21, -21, -21, -21, -15, -9, -17, -18, -16, -16, -16, -16, -18, -17, -14, -8, -4, -2, -2, -4, -8, -14, -0, 9, 15, 14, 14, 15, 9, -0, 9, 21, 22, 20, 20, 22, 21, 9, -2, 12, 14, 11, 11, 14, 12, -2, -29, -6, -5, -4, -4, -5, -6, -29}};
const int Params::KNIGHT_MOBILITY[9] = {21, -2, 18, 30, 38, 47, 50, 48, 43};
const int Params::BISHOP_MOBILITY[15] = {-44, -17, -3, 8, 18, 26, 30, 35, 40, 44, 45, 47, 61, 44, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-82, -59, -48, -43, -42, -37, -33, -27, -23, -18, -13, -7, 6, 17, 12}, {-96, -61, -41, -25, -7, -1, -0, 3, 10, 14, 19, 25, 29, 21, 15}};
const int Params::QUEEN_MOBILITY[2][24] = {{-94, -28, -16, -4, 3, 10, 16, 21, 26, 30, 32, 33, 35, 35, 39, 39, 39, 53, 61, 73, 93, 93, 96, 96}, {-96, -96, -96, -96, -81, -62, -44, -43, -36, -33, -27, -25, -20, -19, -19, -17, -16, -21, -28, -28, -25, -26, -33, -45}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-78, -43, -25, -14, -13};
const int Params::KNIGHT_OUTPOST[2][2] = {{21, 66}, {12, 42}};
const int Params::BISHOP_OUTPOST[2][2] = {{10, 52}, {18, 24}};
const int Params::PAWN_STORM[4][2] = {{0, 28},{9, 5},{33, 2},{3, 0}};

