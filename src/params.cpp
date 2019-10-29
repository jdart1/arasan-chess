// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 27-Oct-2019 02:45:43 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0671505

//

#include "params.h"

const int Params::RB_ADJUST[6] = {46, -6, 87, 38, 36, 73};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -37, -66, -67};
const int Params::QR_ADJUST[5] = {14, 75, 31, -35, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -259, -143};
const int Params::MINOR_FOR_PAWNS[6] = {51, 49, 73, 93, 119, 132};
const int Params::CASTLING[6] = {13, -38, -30, 0, 3, -24};
const int Params::KING_COVER[6][4] = {{8, 23, 15, 15},
{1, 3, 2, 2},
{-9, -26, -16, -17},
{-8, -25, -15, -16},
{-11, -32, -20, -21},
{-17, -53, -33, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 70;
const int Params::KRMINOR_VS_R_NO_PAWNS = -196;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -158;
const int Params::ENDGAME_PAWN_ADVANTAGE = 28;
const int Params::PAWN_ENDGAME1 = 41;
const int Params::PAWN_ATTACK_FACTOR = 13;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 24;
const int Params::ROOK_ATTACK_FACTOR = 55;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 60;
const int Params::QUEEN_ATTACK_BOOST = 42;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 112;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 6;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 96;
const int Params::OWN_ROOK_KING_PROXIMITY = 33;
const int Params::OWN_QUEEN_KING_PROXIMITY = 18;
const int Params::PAWN_PUSH_THREAT_MID = 30;
const int Params::PAWN_PUSH_THREAT_END = 16;
const int Params::ENDGAME_KING_THREAT = 54;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 59;
const int Params::BISHOP_PAWN_PLACEMENT_END = -1;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -23;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -6;
const int Params::WEAK_PAWN_END = -4;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 22;
const int Params::TWO_ROOKS_ON_7TH_MID = 60;
const int Params::TWO_ROOKS_ON_7TH_END = 66;
const int Params::ROOK_ON_OPEN_FILE_MID = 30;
const int Params::ROOK_ON_OPEN_FILE_END = 10;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 24;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 14;
const int Params::KING_OPP_PAWN_DISTANCE = 10;
const int Params::SIDE_PROTECTED_PAWN = -3;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 29, 54, 89, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={210, 187, 150};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-3, -3, -2, -2, -2, -2, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 20, 22, 23, 25, 27, 29, 32, 34, 36, 39, 42, 45, 48, 51, 54, 58, 62, 66, 70, 74, 79, 83, 88, 93, 99, 105, 110, 117, 123, 129, 136, 143, 151, 158, 166, 174, 182, 191, 199, 208, 217, 226, 236, 245, 255, 264, 274, 284, 294, 303, 313, 323, 333, 343, 353, 362, 372, 381, 390, 400, 409, 417, 426, 434, 443, 451, 458, 466, 473, 480, 487, 494, 500, 506, 512, 517, 523, 528, 533, 538, 542, 546, 551, 554, 558, 562, 565, 568, 571, 574, 577, 580, 582, 584, 587, 589, 591, 592, 594, 596, 597, 599, 600, 602, 603, 604, 605, 606, 607, 608, 609, 610, 611, 611, 612};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 30, 115, 199}, {0, 0, 0, 0, 22, 87, 160, 288}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 72, 71, 64, 64, 71, 72, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 2, 0, 19, 96, 0}, {0, 0, 0, 1, 3, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 24, 73, 113, 119}, {0, 0, 0, 0, 48, 67, 173, 301}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 29, 96, 168}, {0, 0, 0, 13, 15, 34, 69, 90}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 11, 4}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-29, 0, 0}, {-23, -36, -114}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-10, -44, -86}, {-54, -80, -128}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{31, 44, 128}, {20, 56, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{0, -21, -0}, {-37, -62, -122}};
const int Params::DOUBLED_PAWNS[2][8] = {{-42, 0, -13, -9, -9, -13, 0, -42}, {-37, -16, -19, -14, -14, -19, -16, -37}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -15, 0, 0, 0, 0, -15, 0}, {-60, -62, -30, -32, -32, -30, -62, -60}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -3, -19, -25, -25, -19, -3, 0}, {-10, -14, -16, -22, -22, -16, -14, -10}};

const int Params::THREAT_BY_PAWN[2][4] = {{64, 88, 85, 84}, {96, 64, 80, 78}};
const int Params::THREAT_BY_MINOR[2][4] = {{16, 41, 60, 96}, {28, 36, 50, 73}};
const int Params::THREAT_BY_ROOK[2][4] = {{23, 31, 41, 96}, {21, 26, 28, 0}};
const int Params::KNIGHT_PST[2][64] = {{-128, -46, -27, -25, -25, -27, -46, -128, -44, -41, -28, -8, -8, -28, -41, -44, -55, -23, -2, 21, 21, -2, -23, -55, -28, 4, 19, 22, 22, 19, 4, -28, -35, -19, 18, 11, 11, 18, -19, -35, -63, -45, -1, -1, -1, -1, -45, -63, -110, -88, -29, -23, -23, -29, -88, -110, -128, -128, -128, -128, -128, -128, -128, -128}, {-110, -64, -52, -51, -51, -52, -64, -110, -60, -47, -46, -35, -35, -46, -47, -60, -60, -40, -30, -11, -11, -30, -40, -60, -37, -23, -4, 5, 5, -4, -23, -37, -49, -41, -20, -15, -15, -20, -41, -49, -65, -46, -30, -29, -29, -30, -46, -65, -83, -61, -57, -46, -46, -57, -61, -83, -128, -81, -91, -79, -79, -91, -81, -128}};
const int Params::BISHOP_PST[2][64] = {{5, 35, 24, 22, 22, 24, 35, 5, 26, 39, 33, 26, 26, 33, 39, 26, 9, 38, 38, 31, 31, 38, 38, 9, 7, 17, 25, 39, 39, 25, 17, 7, -11, 6, 15, 27, 27, 15, 6, -11, 31, 1, -2, 2, 2, -2, 1, 31, -12, -42, -30, -64, -64, -30, -42, -12, -1, -49, -128, -128, -128, -128, -49, -1}, {-29, -14, -6, -11, -11, -6, -14, -29, -13, -7, -8, -0, -0, -8, -7, -13, -12, 5, 9, 13, 13, 9, 5, -12, -6, 6, 14, 17, 17, 14, 6, -6, -15, -1, -2, 2, 2, -2, -1, -15, 7, -9, -7, -9, -9, -7, -9, 7, -2, -3, -14, -13, -13, -14, -3, -2, 4, -1, -2, -23, -23, -2, -1, 4}};
const int Params::ROOK_PST[2][64] = {{-46, -35, -29, -24, -24, -29, -35, -46, -59, -37, -30, -25, -25, -30, -37, -59, -56, -35, -34, -28, -28, -34, -35, -56, -56, -43, -45, -31, -31, -45, -43, -56, -38, -34, -25, -21, -21, -25, -34, -38, -34, -17, -21, -12, -12, -21, -17, -34, -42, -47, -31, -24, -24, -31, -47, -42, -33, -35, -128, -123, -123, -128, -35, -33}, {-27, -30, -22, -28, -28, -22, -30, -27, -40, -35, -28, -28, -28, -28, -35, -40, -33, -27, -23, -24, -24, -23, -27, -33, -16, -10, -5, -10, -10, -5, -10, -16, -2, -0, 4, -0, -0, 4, -0, -2, -2, 3, 6, 3, 3, 6, 3, -2, -17, -13, -9, -9, -9, -9, -13, -17, 4, 8, 8, 3, 3, 8, 8, 4}};
const int Params::QUEEN_PST[2][64] = {{-1, -4, -6, 8, 8, -6, -4, -1, -6, 10, 13, 15, 15, 13, 10, -6, -4, 9, 16, 7, 7, 16, 9, -4, 10, 18, 17, 16, 16, 17, 18, 10, 20, 20, 26, 20, 20, 26, 20, 20, -10, 9, 19, 9, 9, 19, 9, -10, -18, -26, -6, -3, -3, -6, -26, -18, 7, -1, -29, -47, -47, -29, -1, 7}, {-97, -105, -89, -75, -75, -89, -105, -97, -86, -76, -69, -51, -51, -69, -76, -86, -72, -36, -25, -37, -37, -25, -36, -72, -37, -16, -3, -0, -0, -3, -16, -37, -34, 7, 15, 32, 32, 15, 7, -34, -34, -3, 24, 38, 38, 24, -3, -34, -21, 1, 28, 38, 38, 28, 1, -21, -57, -41, -33, -29, -29, -33, -41, -57}};
const int Params::KING_PST[2][64] = {{13, 29, -29, -14, -14, -29, 29, 13, 51, 51, -1, 19, 19, -1, 51, 51, -43, -15, -81, -128, -128, -81, -15, -43, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -126, -128, -128, -128, -128, -126, -128, 71, -68, -128, -128, -128, -128, -68, 71, -92, -128, 128, -42, -42, 128, -128, -92}, {-25, -20, -26, -31, -31, -26, -20, -25, -10, -16, -23, -23, -23, -23, -16, -10, -18, -19, -18, -17, -17, -18, -19, -18, -15, -8, -4, -2, -2, -4, -8, -15, -1, 10, 16, 15, 15, 16, 10, -1, 8, 21, 21, 19, 19, 21, 21, 8, -7, 11, 14, 10, 10, 14, 11, -7, -44, -9, -9, -7, -7, -9, -9, -44}};
const int Params::KNIGHT_MOBILITY[9] = {18, -6, 14, 26, 35, 44, 46, 44, 38};
const int Params::BISHOP_MOBILITY[15] = {-43, -16, -2, 9, 20, 27, 31, 35, 40, 43, 45, 47, 62, 42, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-79, -57, -44, -40, -39, -33, -30, -23, -19, -14, -9, -3, 9, 21, 7}, {-96, -59, -40, -23, -5, 1, 1, 6, 11, 16, 22, 28, 33, 25, 20}};
const int Params::QUEEN_MOBILITY[2][24] = {{-85, -27, -12, -0, 6, 13, 19, 24, 30, 34, 35, 36, 38, 38, 42, 42, 41, 54, 61, 66, 91, 94, 96, 96}, {-96, -96, -96, -96, -79, -63, -42, -43, -35, -33, -27, -24, -20, -18, -19, -16, -15, -20, -25, -26, -24, -24, -36, -45}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-71, -35, -18, -8, -7};
const int Params::KNIGHT_OUTPOST[2][2] = {{20, 66}, {12, 42}};
const int Params::BISHOP_OUTPOST[2][2] = {{8, 52}, {17, 28}};
const int Params::PAWN_STORM[4][2] = {{0, 27},{10, 6},{33, 1},{3, 0}};

