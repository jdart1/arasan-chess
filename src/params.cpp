// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 07-Jun-2018 09:28:47 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/epd/big5.epd
// Final objective value: 0.0759573

//

#include "params.h"

const int Params::RB_ADJUST[6] = {54, -6, 89, 39, 31, 61};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -12, -38, -32};
const int Params::QR_ADJUST[5] = {22, 72, 25, -35, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -292, -165};
const int Params::CASTLING[6] = {13, -38, -24, 0, 5, -18};
const int Params::KING_COVER[6][4] = {{11, 22, 14, 12},
{5, 9, 6, 5},
{-6, -12, -8, -7},
{-5, -10, -7, -6},
{-9, -18, -11, -10},
{-18, -35, -23, -20}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 80;
const int Params::PIN_MULTIPLIER_END = 83;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -184;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -149;
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
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 162;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 12;
const int Params::PIECE_THREAT_MM_MID = 32;
const int Params::PIECE_THREAT_MR_MID = 88;
const int Params::PIECE_THREAT_MQ_MID = 68;
const int Params::PIECE_THREAT_MM_END = 31;
const int Params::PIECE_THREAT_MR_END = 74;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 9;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 19;
const int Params::PIECE_THREAT_RR_MID = 3;
const int Params::PIECE_THREAT_RQ_MID = 93;
const int Params::PIECE_THREAT_RM_END = 30;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 27;
const int Params::ENDGAME_KING_THREAT = 39;
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
const int Params::ROOK_BEHIND_PP_END = 18;
const int Params::QUEEN_OUT = -31;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 11;
const int Params::KING_OPP_PAWN_DISTANCE = 4;
const int Params::QUEENING_SQUARE_CONTROL_MID = 60;
const int Params::QUEENING_SQUARE_CONTROL_END = 48;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -22;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -42;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 1, 23, 52, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={289, 211, 148};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 13, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 36, 39, 41, 44, 47, 50, 54, 57, 61, 65, 69, 74, 78, 83, 88, 93, 99, 105, 111, 117, 123, 130, 137, 144, 152, 159, 167, 175, 184, 192, 201, 210, 219, 229, 238, 248, 258, 267, 277, 288, 298, 308, 318, 328, 338, 348, 358, 368, 378, 388, 397, 407, 416, 425, 434, 442, 451, 459, 467, 475, 482, 489, 496, 503, 509, 516, 522, 527, 533, 538, 543, 548, 553, 557, 561, 565, 569, 573, 576, 579, 582, 585, 588, 591, 593, 596, 598, 600, 602, 604, 606, 607, 609, 610, 612, 613, 614, 616, 617, 618, 619, 620, 620, 621, 622, 623, 623, 624, 625, 625, 626, 626, 627};
const int Params::TRADE_DOWN[8] = {45, 40, 34, 29, 23, 18, 12, 7};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 25, 100, 156}, {0, 0, 0, 0, 17, 66, 147, 224}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {84, 85, 83, 79, 79, 83, 85, 84};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 10, 96, 0}, {0, 0, 0, 0, 0, 16, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 1, 3, 12, 96, 212, 288}, {0, 0, 0, 0, 43, 90, 248, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 21, 4, 25, 88, 159}, {0, 0, 0, 7, 13, 29, 57, 55}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-44, -39, -42, -20, -40, -6, -28, -31, -13, -30, -45, -32, 0, -19, -43, -40, -7, -57, -32, -83, 0}, {0, 0, 0, -9, -18, -13, -0, -14, -14, -26, -28, -6, -5, -9, -16, -20, -18, -29, -66, -57, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-75, -69, -68, -56, -56, -52, -64, -58, -33, -37, -39, -49, -14, -1, -8, -28, 0, 0, -55, -30, -83}, {-13, -11, 0, -4, -8, -4, -22, -10, -13, -7, -10, -27, -11, -5, -2, -50, -21, -14, -66, -47, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-44, 0, -20, -16, -16, -20, 0, -44}, {-45, -21, -27, -21, -21, -27, -21, -45}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -27, -6, -8, -8, -6, -27, 0}, {-11, -64, -30, -45, -45, -30, -64, -11}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -13, -16, -16, -13, 0, 0}, {-4, -8, -10, -16, -16, -10, -8, -4}};

const int Params::KNIGHT_PST[2][64] = {{-78, -27, -8, -9, -9, -8, -27, -78, -22, -28, -20, 2, 2, -20, -28, -22, -40, -18, 1, 20, 20, 1, -18, -40, -19, -4, 15, 15, 15, 15, -4, -19, -34, -18, 13, 9, 9, 13, -18, -34, -62, -47, -14, -5, -5, -14, -47, -62, -106, -84, -35, -30, -30, -35, -84, -106, -128, -128, -128, -128, -128, -128, -128, -128}, {-69, -48, -33, -31, -31, -33, -48, -69, -43, -29, -26, -20, -20, -26, -29, -43, -48, -31, -22, 1, 1, -22, -31, -48, -22, -15, 8, 15, 15, 8, -15, -22, -31, -22, -1, 2, 2, -1, -22, -31, -47, -32, -14, -6, -6, -14, -32, -47, -72, -45, -41, -31, -31, -41, -45, -72, -128, -69, -69, -54, -54, -69, -69, -128}};
const int Params::BISHOP_PST[2][64] = {{8, 40, 25, 21, 21, 25, 40, 8, 28, 36, 32, 24, 24, 32, 36, 28, 7, 32, 32, 25, 25, 32, 32, 7, -3, 4, 14, 31, 31, 14, 4, -3, -42, -18, -12, 12, 12, -12, -18, -42, -2, -39, -55, -36, -36, -55, -39, -2, -45, -73, -67, -105, -105, -67, -73, -45, -35, -74, -128, -128, -128, -128, -74, -35}, {-21, -3, -0, -4, -4, -0, -3, -21, -3, 1, -1, 6, 6, -1, 1, -3, -6, 4, 11, 13, 13, 11, 4, -6, -7, 4, 15, 21, 21, 15, 4, -7, -25, -11, -9, 0, 0, -9, -11, -25, -0, -16, -15, -15, -15, -15, -16, -0, -13, -13, -28, -29, -29, -28, -13, -13, -7, -7, -7, -29, -29, -7, -7, -7}};
const int Params::ROOK_PST[2][64] = {{-73, -59, -54, -51, -51, -54, -59, -73, -85, -60, -55, -52, -52, -55, -60, -85, -80, -58, -60, -61, -61, -60, -58, -80, -82, -65, -65, -56, -56, -65, -65, -82, -62, -50, -42, -34, -34, -42, -50, -62, -51, -23, -27, -14, -14, -27, -23, -51, -47, -54, -22, -18, -18, -22, -54, -47, -41, -37, -118, -83, -83, -118, -37, -41}, {-12, -11, -4, -9, -9, -4, -11, -12, -17, -14, -5, -6, -6, -5, -14, -17, -16, -8, -5, -6, -6, -5, -8, -16, -6, 5, 9, 3, 3, 9, 5, -6, 10, 12, 17, 13, 13, 17, 12, 10, 15, 22, 24, 22, 22, 24, 22, 15, 6, 10, 15, 15, 15, 15, 10, 6, 30, 31, 31, 25, 25, 31, 31, 30}};
const int Params::QUEEN_PST[2][64] = {{31, 34, 33, 43, 43, 33, 34, 31, 29, 41, 46, 49, 49, 46, 41, 29, 24, 38, 45, 36, 36, 45, 38, 24, 36, 43, 43, 44, 44, 43, 43, 36, 38, 40, 46, 52, 52, 46, 40, 38, 28, 36, 52, 49, 49, 52, 36, 28, 17, 6, 31, 25, 25, 31, 6, 17, 19, 28, 7, 7, 7, 7, 28, 19}, {-39, -47, -37, -25, -25, -37, -47, -39, -26, -28, -20, -3, -3, -20, -28, -26, -10, 11, 22, 17, 17, 22, 11, -10, 11, 31, 41, 46, 46, 41, 31, 11, 25, 56, 63, 80, 80, 63, 56, 25, 25, 57, 82, 90, 90, 82, 57, 25, 46, 58, 86, 99, 99, 86, 58, 46, 16, 30, 31, 34, 34, 31, 30, 16}};
const int Params::KING_PST[2][64] = {{33, 53, 5, 19, 19, 5, 53, 33, 73, 71, 38, 54, 54, 38, 71, 73, 6, 38, -22, -64, -64, -22, 38, 6, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -114, -108, -128, -128, -128, -128, -108, -114, -125, 128, -128, -128, -128, -128, 128, -125, -73, -128, 127, -128, -128, 127, -128, -73}, {-79, -76, -79, -82, -82, -79, -76, -79, -63, -69, -74, -72, -72, -74, -69, -63, -71, -72, -68, -66, -66, -68, -72, -71, -67, -61, -54, -52, -52, -54, -61, -67, -51, -43, -35, -35, -35, -35, -43, -51, -45, -33, -28, -27, -27, -28, -33, -45, -48, -38, -34, -35, -35, -34, -38, -48, -78, -55, -50, -52, -52, -50, -55, -78}};

const int Params::KNIGHT_MOBILITY[9] = {-65, -27, -9, 2, 10, 19, 21, 20, 14};
const int Params::BISHOP_MOBILITY[15] = {-43, -25, -12, -3, 7, 14, 19, 22, 27, 30, 30, 34, 40, 23, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-43, -28, -18, -14, -12, -6, -1, 8, 12, 17, 18, 21, 35, 36, 34}, {-43, -36, -17, -1, 12, 18, 20, 23, 30, 34, 38, 43, 43, 36, 30}};
const int Params::QUEEN_MOBILITY[2][24] = {{-43, -17, -11, -3, 3, 8, 14, 19, 23, 28, 29, 31, 33, 33, 36, 39, 42, 43, 43, 43, 43, 43, 43, 43}, {-43, -43, -43, -43, -25, -11, 9, 13, 24, 23, 29, 35, 35, 39, 42, 40, 43, 43, 43, 43, 43, 43, 37, 29}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-43, -15, 6, 17, 20};

const int Params::KNIGHT_OUTPOST[2][2] = {{29, 61}, {16, 28}};
const int Params::BISHOP_OUTPOST[2][2] = {{36, 61}, {33, 25}};


