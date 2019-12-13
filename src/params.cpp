// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 12-Dec-2019 08:44:16 by tuner -n 150 -c 24 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0667521

//

#include "params.h"

const int Params::RB_ADJUST[6] = {30, -6, 69, 18, 19, 64};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -39, -69, -84};
const int Params::QR_ADJUST[5] = {-51, 38, 19, -29, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -240, -128};
const int Params::MINOR_FOR_PAWNS[6] = {51, 47, 78, 96, 125, 152};
const int Params::QUEEN_VS_3MINORS[4] = {-94, 0, -101, -223};
const int Params::CASTLING[6] = {13, -38, -29, 0, 4, -24};
const int Params::KING_COVER[6][4] = {{8, 23, 15, 16},
{1, 4, 3, 3},
{-9, -25, -16, -17},
{-8, -25, -16, -17},
{-10, -30, -19, -20},
{-17, -52, -33, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 92;
const int Params::PIN_MULTIPLIER_END = 65;
const int Params::KRMINOR_VS_R_NO_PAWNS = -168;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -145;
const int Params::ENDGAME_PAWN_ADVANTAGE = 31;
const int Params::PAWN_ENDGAME1 = 64;
const int Params::PAWN_ATTACK_FACTOR = 14;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 24;
const int Params::ROOK_ATTACK_FACTOR = 54;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 61;
const int Params::QUEEN_ATTACK_BOOST = 42;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 114;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 5;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 74;
const int Params::OWN_ROOK_KING_PROXIMITY = 24;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_PUSH_THREAT_MID = 24;
const int Params::PAWN_PUSH_THREAT_END = 14;
const int Params::ENDGAME_KING_THREAT = 37;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 38;
const int Params::BISHOP_PAIR_END = 60;
const int Params::BISHOP_PAWN_PLACEMENT_END = -2;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -23;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -8;
const int Params::WEAK_PAWN_END = -6;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -15;
const int Params::SPACE = 7;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 16;
const int Params::TWO_ROOKS_ON_7TH_MID = 62;
const int Params::TWO_ROOKS_ON_7TH_END = 90;
const int Params::ROOK_ON_OPEN_FILE_MID = 28;
const int Params::ROOK_ON_OPEN_FILE_END = 11;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 27;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 16;
const int Params::KING_OPP_PAWN_DISTANCE = 9;
const int Params::SIDE_PROTECTED_PAWN = -2;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 1, 30, 59, 96, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={192, 175, 139};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 20, 21, 23, 24, 26, 28, 30, 32, 34, 36, 38, 41, 44, 46, 49, 52, 55, 59, 62, 66, 69, 73, 77, 82, 86, 91, 95, 100, 106, 111, 117, 122, 128, 135, 141, 148, 154, 161, 169, 176, 184, 191, 199, 207, 216, 224, 232, 241, 250, 259, 268, 277, 286, 295, 304, 313, 322, 331, 340, 349, 358, 367, 376, 385, 394, 402, 411, 419, 427, 435, 443, 450, 458, 465, 472, 479, 485, 492, 498, 504, 510, 515, 521, 526, 531, 536, 541, 545, 549, 554, 558, 561, 565, 568, 572, 575, 578, 581, 584, 586, 589, 591, 593, 595, 597, 599, 601, 603, 605, 606, 608, 609, 611, 612, 613, 614, 615, 616, 617, 618, 619};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 12, 15, 21, 30, 36, 56, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 36, 121, 195}, {0, 0, 0, 0, 20, 72, 147, 238}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {65, 68, 67, 64, 64, 67, 68, 65};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 25, 76, 114, 95}, {0, 0, 0, 0, 43, 61, 167, 285}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 28, 98, 169}, {0, 0, 0, 10, 13, 32, 61, 79}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{11, 0, 0, 0, 4, 128}, {11, 0, 25, 34, 72, 120}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-36, -1, 0}, {-2, -3, -36}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-21, -48, -64}, {-32, -47, -72}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{28, 46, 128}, {25, 51, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{-4, -25, 0}, {-17, -32, -49}};
const int Params::DOUBLED_PAWNS[2][8] = {{-37, 0, -13, -8, -8, -13, 0, -37}, {-37, -16, -20, -17, -17, -20, -16, -37}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -13, 0, 0, 0, 0, -13, 0}, {-19, -63, -33, -37, -37, -33, -63, -19}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -21, -25, -25, -21, -4, 0}, {-10, -15, -15, -22, -22, -15, -15, -10}};

const int Params::THREAT_BY_PAWN[2][5] = {{91, 101, 100, 47, 0}, {66, 86, 76, 52, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 45, 68, 128, 74}, {27, 21, 59, 74, 102}};
const int Params::THREAT_BY_BISHOP[2][5] = {{21, 51, 77, 84, 78}, {27, 48, 55, 66, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{19, 53, 57, 128, 98}, {28, 45, 51, 0, 128}};
const int Params::KNIGHT_PST[2][64] = {{-99, -43, -29, -21, -21, -29, -43, -99, -42, -40, -31, -8, -8, -31, -40, -42, -55, -31, -11, 13, 13, -11, -31, -55, -25, -3, 12, 15, 15, 12, -3, -25, -36, -21, 11, 3, 3, 11, -21, -36, -63, -59, -19, -16, -16, -19, -59, -63, -104, -95, -40, -34, -34, -40, -95, -104, -128, -128, -128, -128, -128, -128, -128, -128}, {-90, -47, -37, -36, -36, -37, -47, -90, -43, -33, -33, -21, -21, -33, -33, -43, -47, -32, -23, -2, -2, -23, -32, -47, -25, -15, 2, 13, 13, 2, -15, -25, -42, -36, -18, -12, -12, -18, -36, -42, -61, -44, -30, -28, -28, -30, -44, -61, -80, -57, -54, -40, -40, -54, -57, -80, -128, -76, -85, -72, -72, -85, -76, -128}};
const int Params::BISHOP_PST[2][64] = {{-6, 23, 10, 9, 9, 10, 23, -6, 13, 26, 19, 14, 14, 19, 26, 13, -4, 20, 21, 16, 16, 21, 20, -4, -1, -0, 9, 26, 26, 9, -0, -1, -26, -2, -2, 16, 16, -2, -2, -26, 16, -18, -29, -16, -16, -29, -18, 16, -27, -48, -45, -67, -67, -45, -48, -27, -5, -60, -128, -128, -128, -128, -60, -5}, {-30, -13, -3, -7, -7, -3, -13, -30, -12, -4, -5, 3, 3, -5, -4, -12, -9, 5, 9, 13, 13, 9, 5, -9, -6, 5, 13, 16, 16, 13, 5, -6, -17, -5, -6, -2, -2, -6, -5, -17, 4, -11, -11, -13, -13, -11, -11, 4, -4, -7, -16, -16, -16, -16, -7, -4, 1, -4, -7, -25, -25, -7, -4, 1}};
const int Params::ROOK_PST[2][64] = {{-37, -25, -20, -15, -15, -20, -25, -37, -50, -27, -21, -15, -15, -21, -27, -50, -47, -27, -25, -21, -21, -25, -27, -47, -46, -34, -33, -22, -22, -33, -34, -46, -28, -23, -13, -12, -12, -13, -23, -28, -27, -7, -13, -5, -5, -13, -7, -27, -40, -47, -32, -24, -24, -32, -47, -40, -38, -33, -128, -112, -112, -128, -33, -38}, {-16, -19, -10, -15, -15, -10, -19, -16, -28, -23, -16, -15, -15, -16, -23, -28, -21, -14, -9, -10, -10, -9, -14, -21, -4, 2, 7, 3, 3, 7, 2, -4, 10, 11, 16, 12, 12, 16, 11, 10, 10, 13, 16, 13, 13, 16, 13, 10, -1, 2, 4, 3, 3, 4, 2, -1, 21, 23, 24, 18, 18, 24, 23, 21}};
const int Params::QUEEN_PST[2][64] = {{5, 3, -0, 11, 11, -0, 3, 5, -0, 14, 17, 19, 19, 17, 14, -0, 1, 14, 20, 11, 11, 20, 14, 1, 15, 20, 22, 22, 22, 22, 20, 15, 22, 25, 28, 29, 29, 28, 25, 22, 1, 22, 32, 24, 24, 32, 22, 1, -5, -10, 8, 12, 12, 8, -10, -5, 19, 14, -7, -29, -29, -7, 14, 19}, {-87, -91, -75, -59, -59, -75, -91, -87, -70, -61, -56, -39, -39, -56, -61, -70, -53, -22, -13, -24, -24, -13, -22, -53, -21, -1, 9, 9, 9, 9, -1, -21, -12, 22, 29, 44, 44, 29, 22, -12, -19, 15, 35, 49, 49, 35, 15, -19, -3, 16, 43, 52, 52, 43, 16, -3, -38, -22, -20, -16, -16, -20, -22, -38}};
const int Params::KING_PST[2][64] = {{21, 37, -21, -1, -1, -21, 37, 21, 57, 58, 8, 34, 34, 8, 58, 57, -41, -14, -73, -128, -128, -73, -14, -41, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -111, -128, -128, -128, -128, -111, -128, 52, 19, -128, -128, -128, -128, 19, 52, -58, -128, -107, -17, -17, -107, -128, -58}, {-31, -28, -33, -37, -37, -33, -28, -31, -15, -23, -29, -29, -29, -29, -23, -15, -23, -25, -23, -22, -22, -23, -25, -23, -19, -14, -9, -6, -6, -9, -14, -19, -5, 3, 12, 12, 12, 12, 3, -5, 2, 15, 16, 15, 15, 16, 15, 2, -12, 3, 7, 5, 5, 7, 3, -12, -43, -15, -13, -13, -13, -13, -15, -43}};
const int Params::KNIGHT_MOBILITY[9] = {-86, -40, -18, -5, 4, 14, 18, 17, 11};
const int Params::BISHOP_MOBILITY[15] = {-64, -38, -24, -13, -1, 6, 11, 15, 20, 24, 26, 28, 40, 22, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-74, -51, -40, -36, -34, -29, -25, -19, -15, -10, -5, 1, 13, 27, 22}, {-82, -40, -21, -6, 11, 17, 17, 22, 27, 32, 39, 45, 50, 42, 35}};
const int Params::QUEEN_MOBILITY[2][24] = {{-73, -19, -7, 5, 10, 17, 23, 29, 35, 39, 41, 43, 45, 45, 48, 49, 50, 59, 68, 74, 91, 96, 96, 96}, {-96, -96, -96, -96, -61, -45, -27, -27, -20, -17, -13, -8, -6, -1, -0, 0, 0, -2, -7, -7, -6, -5, -13, -21}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-76, -39, -21, -10, -7};
const int Params::KNIGHT_OUTPOST[2][2] = {{25, 57}, {23, 42}};
const int Params::BISHOP_OUTPOST[2][2] = {{13, 39}, {22, 28}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -20, 26, 4},{20, -25, 24, 30, -8},{20, 29, 14, 41, 10},{20, -17, 30, 42, 15}},{{10, 11, 1, -23, -10},{10, -64, -5, -7, -1},{10, 30, 20, 15, 8},{10, 64, -14, 12, -2}}};

