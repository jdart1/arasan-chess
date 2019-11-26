// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 25-Nov-2019 08:41:05 by tuner -n 150 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.066728

//

#include "params.h"

const int Params::RB_ADJUST[6] = {30, -6, 68, 18, 20, 66};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -39, -68, -84};
const int Params::QR_ADJUST[5] = {-51, 38, 19, -34, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -241, -128};
const int Params::MINOR_FOR_PAWNS[6] = {51, 47, 77, 97, 126, 154};
const int Params::QUEEN_VS_3MINORS[4] = {-94, 0, -102, -224};
const int Params::CASTLING[6] = {13, -38, -29, 0, 4, -24};
const int Params::KING_COVER[6][4] = {{9, 27, 18, 18},
{1, 4, 3, 3},
{-8, -24, -16, -16},
{-8, -24, -16, -16},
{-10, -30, -20, -19},
{-17, -51, -34, -34}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 92;
const int Params::PIN_MULTIPLIER_END = 65;
const int Params::KRMINOR_VS_R_NO_PAWNS = -169;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -145;
const int Params::ENDGAME_PAWN_ADVANTAGE = 31;
const int Params::PAWN_ENDGAME1 = 64;
const int Params::PAWN_ATTACK_FACTOR = 13;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 23;
const int Params::ROOK_ATTACK_FACTOR = 54;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 61;
const int Params::QUEEN_ATTACK_BOOST = 42;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 111;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 6;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 75;
const int Params::OWN_ROOK_KING_PROXIMITY = 25;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_PUSH_THREAT_MID = 23;
const int Params::PAWN_PUSH_THREAT_END = 14;
const int Params::ENDGAME_KING_THREAT = 37;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 38;
const int Params::BISHOP_PAIR_END = 59;
const int Params::BISHOP_PAWN_PLACEMENT_END = -1;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -25;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -7;
const int Params::WEAK_PAWN_END = -5;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -15;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 16;
const int Params::TWO_ROOKS_ON_7TH_MID = 63;
const int Params::TWO_ROOKS_ON_7TH_END = 91;
const int Params::ROOK_ON_OPEN_FILE_MID = 29;
const int Params::ROOK_ON_OPEN_FILE_END = 11;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 28;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 15;
const int Params::KING_OPP_PAWN_DISTANCE = 9;
const int Params::SIDE_PROTECTED_PAWN = -2;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 29, 58, 96, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={196, 176, 140};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 22, 23, 25, 27, 29, 31, 33, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 64, 67, 71, 75, 79, 84, 88, 93, 98, 103, 108, 113, 119, 125, 131, 137, 144, 150, 157, 164, 172, 179, 187, 195, 202, 211, 219, 227, 236, 245, 253, 262, 271, 280, 289, 299, 308, 317, 326, 335, 345, 354, 363, 372, 381, 390, 398, 407, 415, 424, 432, 440, 448, 455, 463, 470, 477, 484, 491, 497, 504, 510, 516, 521, 527, 532, 537, 542, 547, 551, 556, 560, 564, 568, 572, 575, 578, 582, 585, 588, 590, 593, 596, 598, 600, 603, 605, 607, 609, 610, 612, 614, 615, 617, 618, 619, 621, 622, 623, 624, 625, 626, 627};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 12, 15, 21, 29, 35, 55, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 36, 122, 198}, {0, 0, 0, 0, 20, 73, 147, 239}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {65, 68, 67, 64, 64, 67, 68, 65};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 1, 18, 96, 0}, {0, 0, 0, 0, 2, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 28, 78, 119, 57}, {0, 0, 0, 0, 43, 61, 166, 288}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 29, 98, 168}, {0, 0, 0, 11, 13, 32, 61, 79}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{14, 0, 0, 0, 3, 128}, {11, 0, 24, 34, 73, 121}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-36, -4, 0}, {-2, -2, -36}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-20, -49, -66}, {-32, -46, -72}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{28, 45, 127}, {25, 51, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{-3, -26, 0}, {-17, -31, -49}};
const int Params::DOUBLED_PAWNS[2][8] = {{-37, 0, -15, -9, -9, -15, 0, -37}, {-37, -16, -21, -17, -17, -21, -16, -37}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -20, 0, 0, 0, 0, -20, 0}, {-37, -62, -33, -35, -35, -33, -62, -37}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -21, -25, -25, -21, -4, 0}, {-10, -14, -15, -21, -21, -15, -14, -10}};

const int Params::THREAT_BY_PAWN[2][5] = {{90, 100, 100, 47, 0}, {66, 86, 75, 53, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 49, 68, 128, 74}, {27, 25, 59, 74, 104}};
const int Params::THREAT_BY_BISHOP[2][5] = {{20, 51, 76, 84, 77}, {27, 48, 54, 66, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{19, 53, 57, 128, 99}, {28, 45, 51, 0, 128}};
const int Params::KNIGHT_PST[2][64] = {{-100, -44, -30, -23, -23, -30, -44, -100, -44, -42, -31, -9, -9, -31, -42, -44, -56, -32, -12, 13, 13, -12, -32, -56, -27, -4, 11, 14, 14, 11, -4, -27, -37, -21, 10, 3, 3, 10, -21, -37, -62, -59, -19, -16, -16, -19, -59, -62, -106, -95, -41, -34, -34, -41, -95, -106, -128, -128, -128, -128, -128, -128, -128, -128}, {-90, -46, -35, -35, -35, -35, -46, -90, -41, -31, -32, -20, -20, -32, -31, -41, -46, -31, -22, -1, -1, -22, -31, -46, -23, -14, 4, 15, 15, 4, -14, -23, -40, -34, -16, -10, -10, -16, -34, -40, -58, -42, -29, -27, -27, -29, -42, -58, -79, -55, -53, -38, -38, -53, -55, -79, -128, -74, -84, -70, -70, -84, -74, -128}};
const int Params::BISHOP_PST[2][64] = {{-8, 21, 9, 7, 7, 9, 21, -8, 12, 24, 18, 12, 12, 18, 24, 12, -6, 19, 19, 15, 15, 19, 19, -6, -2, -2, 8, 25, 25, 8, -2, -2, -26, -3, -3, 15, 15, -3, -3, -26, 16, -18, -30, -17, -17, -30, -18, 16, -26, -48, -45, -66, -66, -45, -48, -26, -5, -63, -128, -128, -128, -128, -63, -5}, {-29, -12, -1, -6, -6, -1, -12, -29, -11, -2, -3, 5, 5, -3, -2, -11, -8, 6, 11, 14, 14, 11, 6, -8, -5, 6, 15, 17, 17, 15, 6, -5, -15, -3, -4, 0, 0, -4, -3, -15, 7, -9, -8, -11, -11, -8, -9, 7, -2, -4, -14, -13, -13, -14, -4, -2, 2, -2, -4, -23, -23, -4, -2, 2}};
const int Params::ROOK_PST[2][64] = {{-39, -27, -22, -17, -17, -22, -27, -39, -52, -29, -23, -17, -17, -23, -29, -52, -50, -29, -27, -23, -23, -27, -29, -50, -48, -36, -36, -25, -25, -36, -36, -48, -30, -25, -15, -15, -15, -15, -25, -30, -30, -10, -15, -8, -8, -15, -10, -30, -42, -49, -35, -27, -27, -35, -49, -42, -39, -36, -128, -114, -114, -128, -36, -39}, {-15, -18, -9, -14, -14, -9, -18, -15, -27, -23, -15, -14, -14, -15, -23, -27, -20, -14, -8, -9, -9, -8, -14, -20, -3, 2, 8, 4, 4, 8, 2, -3, 10, 12, 16, 12, 12, 16, 12, 10, 11, 14, 16, 14, 14, 16, 14, 11, -0, 3, 5, 4, 4, 5, 3, -0, 22, 24, 25, 18, 18, 25, 24, 22}};
const int Params::QUEEN_PST[2][64] = {{5, 3, 0, 11, 11, 0, 3, 5, 0, 15, 17, 19, 19, 17, 15, 0, 1, 14, 20, 11, 11, 20, 14, 1, 15, 21, 21, 22, 22, 21, 21, 15, 22, 25, 28, 28, 28, 28, 25, 22, 1, 22, 32, 24, 24, 32, 22, 1, -4, -10, 8, 12, 12, 8, -10, -4, 21, 15, -6, -30, -30, -6, 15, 21}, {-87, -90, -76, -59, -59, -76, -90, -87, -70, -61, -55, -39, -39, -55, -61, -70, -53, -22, -13, -24, -24, -13, -22, -53, -21, -0, 9, 10, 10, 9, -0, -21, -13, 22, 29, 44, 44, 29, 22, -13, -18, 15, 35, 50, 50, 35, 15, -18, -3, 16, 44, 52, 52, 44, 16, -3, -38, -21, -20, -15, -15, -20, -21, -38}};
const int Params::KING_PST[2][64] = {{25, 41, -18, 1, 1, -18, 41, 25, 61, 62, 11, 35, 35, 11, 62, 61, -32, -5, -63, -123, -123, -63, -5, -32, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -103, -128, -128, -128, -128, -103, -128, 44, 9, -128, -128, -128, -128, 9, 44, -83, -128, -101, 5, 5, -101, -128, -83}, {-32, -28, -34, -38, -38, -34, -28, -32, -16, -24, -30, -30, -30, -30, -24, -16, -25, -26, -24, -23, -23, -24, -26, -25, -20, -15, -10, -8, -8, -10, -15, -20, -6, 2, 10, 10, 10, 10, 2, -6, 1, 13, 14, 13, 13, 14, 13, 1, -14, 2, 6, 4, 4, 6, 2, -14, -44, -15, -14, -15, -15, -14, -15, -44}};
const int Params::KNIGHT_MOBILITY[9] = {-86, -40, -19, -6, 4, 13, 17, 17, 11};
const int Params::BISHOP_MOBILITY[15] = {-64, -38, -23, -12, -1, 6, 11, 15, 20, 23, 26, 28, 40, 22, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-76, -52, -41, -37, -36, -31, -27, -20, -16, -11, -6, -1, 11, 25, 19}, {-82, -40, -20, -5, 12, 17, 18, 23, 28, 33, 40, 46, 51, 43, 35}};
const int Params::QUEEN_MOBILITY[2][24] = {{-75, -19, -7, 5, 11, 17, 23, 29, 35, 39, 41, 43, 45, 45, 48, 49, 50, 60, 67, 73, 91, 96, 96, 96}, {-96, -96, -96, -96, -59, -43, -25, -24, -17, -15, -11, -6, -4, 1, 2, 3, 3, 0, -4, -5, -3, -3, -11, -19}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-78, -41, -23, -12, -9};
const int Params::KNIGHT_OUTPOST[2][2] = {{24, 57}, {22, 42}};
const int Params::BISHOP_OUTPOST[2][2] = {{12, 38}, {21, 28}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -20, 26, 4},{20, -29, 22, 30, -8},{20, 27, 14, 39, 9},{20, -25, 28, 40, 16}},{{10, -10, 2, -24, -10},{10, -64, -1, -6, -1},{10, 30, 23, 16, 8},{10, 64, -13, 12, -3}}};

