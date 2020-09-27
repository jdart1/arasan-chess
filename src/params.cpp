// Copyright 2015-2020 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 27-Sep-2020 12:07:17 by tuner -n 180 -c 20 -R 20 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0657177

//

#include "params.h"

const int Params::KN_VS_PAWN_ADJUST[3] = {0, -116, 20};
const int Params::KING_COVER[6][4] = {{8, 24, 15, 16},
{1, 4, 3, 3},
{-9, -26, -16, -17},
{-8, -25, -15, -17},
{-10, -30, -19, -21},
{-17, -52, -32, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 93;
const int Params::PIN_MULTIPLIER_END = 64;
const int Params::MINOR_FOR_PAWNS_MIDGAME = 161;
const int Params::MINOR_FOR_PAWNS_ENDGAME = 91;
const int Params::RB_ADJUST_MIDGAME = 87;
const int Params::RB_ADJUST_ENDGAME = 64;
const int Params::RBN_ADJUST_MIDGAME = -99;
const int Params::RBN_ADJUST_ENDGAME = 28;
const int Params::QR_ADJUST_MIDGAME = -7;
const int Params::QR_ADJUST_ENDGAME = 33;
const int Params::Q_VS_3MINORS_MIDGAME = -285;
const int Params::Q_VS_3MINORS_ENDGAME = -14;
const int Params::KRMINOR_VS_R_NO_PAWNS = -152;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -132;
const int Params::TRADE_DOWN1 = -23;
const int Params::TRADE_DOWN2 = 42;
const int Params::TRADE_DOWN3 = 18;
const int Params::PAWN_ENDGAME_ADJUST = 8;
const int Params::PAWN_ATTACK_FACTOR = 14;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 25;
const int Params::ROOK_ATTACK_FACTOR = 54;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 60;
const int Params::QUEEN_ATTACK_BOOST = 43;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 118;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 5;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 73;
const int Params::OWN_ROOK_KING_PROXIMITY = 24;
const int Params::OWN_QUEEN_KING_PROXIMITY = 15;
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
const int Params::WEAK_PAWN_END = -4;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 7;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 13;
const int Params::TWO_ROOKS_ON_7TH_MID = 69;
const int Params::TWO_ROOKS_ON_7TH_END = 84;
const int Params::TRAPPED_ROOK = 0;
const int Params::TRAPPED_ROOK_NO_CASTLE = -82;
const int Params::ROOK_ON_OPEN_FILE_MID = 28;
const int Params::ROOK_ON_OPEN_FILE_END = 10;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 26;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 18;
const int Params::KING_OPP_PAWN_DISTANCE = 8;
const int Params::SIDE_PROTECTED_PAWN = -3;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 1, 29, 59, 96, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={250, 158, 132};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21, 22, 24, 26, 28, 30, 32, 34, 36, 38, 41, 43, 46, 49, 52, 55, 58, 62, 65, 69, 73, 77, 81, 86, 90, 95, 100, 105, 111, 116, 122, 128, 134, 141, 147, 154, 161, 168, 176, 183, 191, 199, 207, 215, 224, 232, 241, 250, 259, 267, 277, 286, 295, 304, 313, 322, 331, 340, 350, 359, 367, 376, 385, 394, 402, 411, 419, 427, 435, 443, 450, 458, 465, 472, 479, 485, 492, 498, 504, 510, 515, 521, 526, 531, 536, 541, 545, 549, 553, 557, 561, 565, 568, 572, 575, 578, 581, 583, 586, 588, 591, 593, 595, 597, 599, 601, 603, 604, 606, 607, 609, 610, 611, 613, 614, 615, 616, 617, 618, 619};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 13, 16, 22, 31, 36, 57, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 35, 122, 197}, {0, 0, 0, 0, 19, 73, 146, 233}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {66, 69, 67, 64, 64, 67, 69, 66};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 24, 74, 112, 147}, {0, 0, 0, 0, 42, 68, 185, 276}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 30, 100, 173}, {0, 0, 0, 8, 11, 30, 63, 83}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{4, 0, 0, 0, 0, 128}, {18, 3, 28, 37, 74, 112}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-32, -2, 0}, {-4, -7, -35}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-19, -47, -58}, {-33, -49, -74}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{30, 50, 128}, {21, 45, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{-2, -25, 0}, {-18, -33, -44}};
const int Params::DOUBLED_PAWNS[2][8] = {{-37, 0, -13, -8, -8, -13, 0, -37}, {-33, -14, -19, -16, -16, -19, -14, -33}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -19, 0, 0, 0, 0, -19, 0}, {-74, -57, -31, -38, -38, -31, -57, -74}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -21, -26, -26, -21, -4, 0}, {-11, -16, -15, -21, -21, -15, -16, -11}};

const int Params::THREAT_BY_PAWN[2][5] = {{94, 105, 98, 53, 0}, {68, 85, 77, 35, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 34, 69, 127, 77}, {28, 28, 57, 77, 109}};
const int Params::THREAT_BY_BISHOP[2][5] = {{21, 52, 70, 83, 79}, {27, 48, 52, 68, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{20, 54, 58, 128, 94}, {27, 41, 48, 5, 128}};
const int Params::KNIGHT_PST[2][64] = {{-98, -44, -27, -21, -21, -27, -44, -98, -40, -37, -28, -5, -5, -28, -37, -40, -53, -29, -9, 17, 17, -9, -29, -53, -23, -0, 15, 18, 18, 15, -0, -23, -35, -19, 14, 6, 6, 14, -19, -35, -65, -57, -18, -14, -14, -18, -57, -65, -106, -94, -39, -36, -36, -39, -94, -106, -128, -128, -128, -128, -128, -128, -128, -128}, {-88, -45, -34, -33, -33, -34, -45, -88, -40, -30, -31, -19, -19, -31, -30, -40, -44, -29, -19, 0, 0, -19, -29, -44, -21, -12, 5, 16, 16, 5, -12, -21, -39, -34, -15, -9, -9, -15, -34, -39, -57, -41, -28, -26, -26, -28, -41, -57, -74, -54, -52, -38, -38, -52, -54, -74, -128, -77, -83, -70, -70, -83, -77, -128}};
const int Params::BISHOP_PST[2][64] = {{-6, 22, 8, 9, 9, 8, 22, -6, 14, 26, 20, 14, 14, 20, 26, 14, -4, 21, 22, 17, 17, 22, 21, -4, -1, 1, 10, 28, 28, 10, 1, -1, -26, -1, -1, 16, 16, -1, -1, -26, 17, -18, -26, -14, -14, -26, -18, 17, -25, -46, -44, -67, -67, -44, -46, -25, 3, -54, -128, -128, -128, -128, -54, 3}, {-33, -15, -4, -9, -9, -4, -15, -33, -15, -6, -7, 1, 1, -7, -6, -15, -12, 3, 7, 11, 11, 7, 3, -12, -7, 2, 11, 13, 13, 11, 2, -7, -17, -5, -6, -2, -2, -6, -5, -17, 5, -12, -11, -13, -13, -11, -12, 5, -4, -7, -17, -16, -16, -17, -7, -4, -4, -6, -9, -27, -27, -9, -6, -4}};
const int Params::ROOK_PST[2][64] = {{-23, -13, -8, -3, -3, -8, -13, -23, -44, -18, -9, -4, -4, -9, -18, -44, -40, -18, -14, -10, -10, -14, -18, -40, -39, -25, -23, -11, -11, -23, -25, -39, -21, -15, -2, -0, -0, -2, -15, -21, -21, 1, -2, 6, 6, -2, 1, -21, -32, -37, -21, -13, -13, -21, -37, -32, -27, -23, -122, -99, -99, -122, -23, -27}, {-20, -22, -12, -18, -18, -12, -22, -20, -29, -23, -15, -15, -15, -15, -23, -29, -20, -15, -9, -10, -10, -9, -15, -20, -3, 3, 8, 3, 3, 8, 3, -3, 11, 13, 17, 12, 12, 17, 13, 11, 13, 15, 17, 15, 15, 17, 15, 13, 2, 5, 6, 5, 5, 6, 5, 2, 22, 25, 25, 17, 17, 25, 25, 22}};
const int Params::QUEEN_PST[2][64] = {{-4, -7, -10, 0, 0, -10, -7, -4, -10, 4, 7, 9, 9, 7, 4, -10, -8, 5, 11, 1, 1, 11, 5, -8, 6, 11, 12, 13, 13, 12, 11, 6, 13, 15, 20, 20, 20, 20, 15, 13, -8, 13, 22, 14, 14, 22, 13, -8, -15, -20, -0, 1, 1, -0, -20, -15, 17, 8, -23, -44, -44, -23, 8, 17}, {-88, -92, -73, -58, -58, -73, -92, -88, -68, -59, -54, -37, -37, -54, -59, -68, -50, -20, -11, -21, -21, -11, -20, -50, -22, -0, 10, 12, 12, 10, -0, -22, -12, 21, 30, 45, 45, 30, 21, -12, -17, 15, 37, 50, 50, 37, 15, -17, -1, 16, 43, 54, 54, 43, 16, -1, -34, -21, -14, -9, -9, -14, -21, -34}};
const int Params::KING_PST[2][64] = {{12, 29, -22, 17, 17, -22, 29, 12, 48, 48, -12, 8, 8, -12, 48, 48, -51, -28, -96, -128, -128, -96, -28, -51, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -113, -128, -128, -128, -128, -113, -128, 24, -25, -128, -128, -128, -128, -25, 24, -108, -128, -122, 30, 30, -122, -128, -108}, {-3, 1, -4, -10, -10, -4, 1, -3, 15, 7, 1, 2, 2, 1, 7, 15, 7, 5, 8, 9, 9, 8, 5, 7, 13, 18, 23, 26, 26, 23, 18, 13, 28, 37, 45, 45, 45, 45, 37, 28, 38, 49, 51, 50, 50, 51, 49, 38, 24, 40, 43, 40, 40, 43, 40, 24, -11, 19, 20, 21, 21, 20, 19, -11}};
const int Params::KNIGHT_MOBILITY[9] = {-86, -39, -17, -4, 5, 15, 19, 18, 10};
const int Params::BISHOP_MOBILITY[15] = {-60, -33, -19, -8, 3, 11, 15, 19, 24, 27, 30, 30, 43, 23, 38};
const int Params::ROOK_MOBILITY[2][15] = {{-94, -74, -64, -62, -61, -56, -53, -46, -42, -37, -32, -26, -15, -1, -9}, {-96, -56, -37, -22, -6, 1, 1, 7, 13, 18, 25, 31, 37, 29, 23}};
const int Params::QUEEN_MOBILITY[2][24] = {{-75, -20, -6, 5, 12, 18, 24, 30, 36, 41, 42, 45, 47, 48, 51, 52, 54, 63, 73, 78, 95, 96, 96, 96}, {-96, -96, -96, -96, -72, -53, -36, -34, -26, -24, -19, -14, -12, -9, -8, -8, -8, -10, -16, -18, -15, -17, -27, -35}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-49, -13, 5, 16, 19};
const int Params::KNIGHT_OUTPOST[2][2] = {{27, 58}, {22, 42}};
const int Params::BISHOP_OUTPOST[2][2] = {{13, 40}, {19, 24}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -20, 27, 4},{20, -30, 23, 30, -8},{20, 32, 15, 41, 10},{20, -6, 27, 42, 16}},{{10, 37, 2, -24, -9},{10, -64, -5, -8, -3},{10, 31, 21, 14, 8},{10, 64, -9, 10, -0}}};

