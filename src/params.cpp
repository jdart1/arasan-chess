// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 08-Oct-2019 11:49:14 by tuner -c 24 -n 175 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.067329

//

#include "params.h"

const int Params::RB_ADJUST[6] = {50, -6, 93, 41, 38, 78};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -32, -57, -53};
const int Params::QR_ADJUST[5] = {10, 70, 25, -44, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -256, -138};
const int Params::MINOR_FOR_PAWNS[6] = {51, 55, 71, 90, 111, 121};
const int Params::CASTLING[6] = {13, -38, -30, 0, 4, -24};
const int Params::KING_COVER[6][4] = {{8, 24, 15, 16},
{1, 4, 2, 3},
{-8, -26, -16, -17},
{-8, -24, -15, -16},
{-11, -32, -20, -21},
{-17, -50, -31, -33}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 71;
const int Params::KRMINOR_VS_R_NO_PAWNS = -197;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -157;
const int Params::ENDGAME_PAWN_ADVANTAGE = 29;
const int Params::PAWN_ENDGAME1 = 39;
const int Params::PAWN_ATTACK_FACTOR = 12;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 23;
const int Params::ROOK_ATTACK_FACTOR = 55;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 60;
const int Params::QUEEN_ATTACK_BOOST = 41;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 114;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 6;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 96;
const int Params::OWN_ROOK_KING_PROXIMITY = 35;
const int Params::OWN_QUEEN_KING_PROXIMITY = 18;
const int Params::ENDGAME_KING_THREAT = 58;
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
const int Params::PAWN_CENTER_SCORE_MID = 4;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 23;
const int Params::TWO_ROOKS_ON_7TH_MID = 63;
const int Params::TWO_ROOKS_ON_7TH_END = 59;
const int Params::ROOK_ON_OPEN_FILE_MID = 31;
const int Params::ROOK_ON_OPEN_FILE_END = 10;
const int Params::ROOK_BEHIND_PP_MID = 9;
const int Params::ROOK_BEHIND_PP_END = 23;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 8;
const int Params::QUEENING_SQUARE_CONTROL_MID = 65;
const int Params::QUEENING_SQUARE_CONTROL_END = 61;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -16;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -43;
const int Params::SIDE_PROTECTED_PAWN = -4;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 24, 54, 82, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={237, 206, 162};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-3, -3, -3, -3, -2, -2, -2, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 21, 22, 24, 26, 28, 30, 33, 35, 38, 40, 43, 46, 50, 53, 57, 60, 64, 68, 73, 77, 82, 87, 92, 98, 103, 109, 116, 122, 129, 136, 143, 150, 158, 166, 174, 182, 191, 200, 209, 218, 227, 237, 246, 256, 266, 276, 286, 296, 306, 316, 326, 336, 346, 356, 366, 376, 385, 395, 404, 413, 422, 431, 439, 448, 456, 463, 471, 478, 486, 492, 499, 505, 511, 517, 523, 528, 533, 538, 543, 548, 552, 556, 560, 563, 567, 570, 573, 576, 579, 582, 585, 587, 589, 591, 593, 595, 597, 599, 600, 602, 603, 605, 606, 607, 608, 609, 610, 611, 612, 613, 614, 615, 615, 616};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 2, 38, 123, 198}, {0, 0, 3, 0, 27, 85, 160, 261}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 0, 19, 96, 0}, {0, 0, 0, 1, 3, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 6, 14, 26, 102, 183, 320}, {0, 0, 0, 0, 43, 93, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 28, 11, 36, 95, 180}, {0, 0, 0, 12, 15, 33, 70, 90}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-30, -24, -27, -5, -29, -14, -20, -26, -4, -22, -29, -18, -1, -18, -9, -34, -8, -43, -11, -53, 0}, {0, 0, 0, -8, -12, -6, 0, -8, -6, -18, -26, -7, -7, -13, -13, -20, -20, -26, -52, -40, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-51, -67, -47, -39, -34, -35, -47, -42, -20, -27, -27, -32, -1, 0, -2, -23, 0, 0, -50, -26, -83}, {-10, -2, 0, -5, -4, 0, -12, -2, 0, -4, -3, -25, -9, -8, -1, -52, -22, -13, -58, -32, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-41, 0, -14, -10, -10, -14, 0, -41}, {-39, -17, -20, -14, -14, -20, -17, -39}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -18, 0, 0, 0, 0, -18, 0}, {-70, -63, -29, -32, -32, -29, -63, -70}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -2, -19, -23, -23, -19, -2, 0}, {-9, -13, -15, -22, -22, -15, -13, -9}};

const int Params::THREAT_BY_PAWN[2][4] = {{64, 87, 83, 86}, {96, 64, 79, 70}};
const int Params::THREAT_BY_MINOR[2][4] = {{15, 41, 59, 96}, {27, 36, 49, 73}};
const int Params::THREAT_BY_ROOK[2][4] = {{22, 31, 40, 96}, {20, 25, 27, 0}};
const int Params::KNIGHT_PST[2][64] = {{-127, -47, -28, -27, -27, -28, -47, -127, -46, -42, -31, -10, -10, -31, -42, -46, -56, -25, -4, 19, 19, -4, -25, -56, -30, 2, 16, 18, 18, 16, 2, -30, -41, -27, 11, 7, 7, 11, -27, -41, -67, -52, -9, -7, -7, -9, -52, -67, -112, -93, -35, -29, -29, -35, -93, -112, -128, -128, -128, -128, -128, -128, -128, -128}, {-111, -66, -53, -53, -53, -53, -66, -111, -61, -48, -47, -37, -37, -47, -48, -61, -63, -43, -33, -15, -15, -33, -43, -63, -40, -26, -8, 1, 1, -8, -26, -40, -55, -47, -26, -22, -22, -26, -47, -55, -71, -54, -38, -37, -37, -38, -54, -71, -87, -68, -66, -55, -55, -66, -68, -87, -128, -90, -96, -85, -85, -96, -90, -128}};
const int Params::BISHOP_PST[2][64] = {{9, 38, 27, 25, 25, 27, 38, 9, 29, 42, 36, 29, 29, 36, 42, 29, 11, 42, 41, 34, 34, 41, 42, 11, 7, 18, 25, 41, 41, 25, 18, 7, -18, -2, 10, 24, 24, 10, -2, -18, 25, -6, -9, -5, -5, -9, -6, 25, -13, -44, -39, -73, -73, -39, -44, -13, -13, -56, -128, -128, -128, -128, -56, -13}, {-24, -8, -2, -7, -7, -2, -8, -24, -6, -3, -4, 3, 3, -4, -3, -6, -8, 9, 12, 16, 16, 12, 9, -8, -6, 7, 17, 19, 19, 17, 7, -6, -18, -3, -3, 3, 3, -3, -3, -18, 4, -11, -10, -10, -10, -10, -11, 4, -4, -10, -20, -22, -22, -20, -10, -4, -1, -1, -5, -27, -27, -5, -1, -1}};
const int Params::ROOK_PST[2][64] = {{-52, -40, -34, -30, -30, -34, -40, -52, -64, -42, -35, -31, -31, -35, -42, -64, -63, -42, -40, -35, -35, -40, -42, -63, -65, -50, -50, -39, -39, -50, -50, -65, -48, -43, -32, -27, -27, -32, -43, -48, -39, -19, -24, -14, -14, -24, -19, -39, -44, -50, -35, -27, -27, -35, -50, -44, -43, -46, -128, -128, -128, -128, -46, -43}, {-30, -33, -25, -30, -30, -25, -33, -30, -41, -37, -30, -30, -30, -30, -37, -41, -35, -29, -25, -26, -26, -25, -29, -35, -21, -15, -9, -14, -14, -9, -15, -21, -7, -5, -1, -4, -4, -1, -5, -7, -4, 1, 5, 2, 2, 5, 1, -4, -16, -12, -9, -8, -8, -9, -12, -16, 5, 5, 4, -1, -1, 4, 5, 5}};
const int Params::QUEEN_PST[2][64] = {{-3, -4, -5, 8, 8, -5, -4, -3, -8, 10, 12, 15, 15, 12, 10, -8, -5, 9, 16, 7, 7, 16, 9, -5, 6, 14, 14, 14, 14, 14, 14, 6, 11, 13, 19, 18, 18, 19, 13, 11, -9, 11, 20, 12, 12, 20, 11, -9, -9, -24, -4, -2, -2, -4, -24, -9, 3, -1, -32, -52, -52, -32, -1, 3}, {-103, -109, -94, -79, -79, -94, -109, -103, -92, -80, -73, -58, -58, -73, -80, -92, -75, -41, -31, -43, -43, -31, -41, -75, -47, -23, -11, -8, -8, -11, -23, -47, -40, -2, 9, 24, 24, 9, -2, -40, -40, -7, 15, 34, 34, 15, -7, -40, -25, -3, 25, 33, 33, 25, -3, -25, -58, -48, -42, -32, -32, -42, -48, -58}};
const int Params::KING_PST[2][64] = {{16, 32, -26, -9, -9, -26, 32, 16, 55, 54, 4, 23, 23, 4, 54, 55, -36, -11, -75, -128, -128, -75, -11, -36, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -116, -128, -128, -128, -128, -116, -128, 128, -73, -128, -128, -128, -128, -73, 128, -128, -128, 128, -44, -44, 128, -128, -128}, {-24, -20, -24, -30, -30, -24, -20, -24, -10, -15, -21, -21, -21, -21, -15, -10, -18, -18, -16, -16, -16, -16, -18, -18, -15, -9, -4, -2, -2, -4, -9, -15, -1, 9, 14, 14, 14, 14, 9, -1, 8, 21, 21, 20, 20, 21, 21, 8, -3, 12, 14, 11, 11, 14, 12, -3, -30, -6, -5, -4, -4, -5, -6, -30}};
const int Params::KNIGHT_MOBILITY[9] = {19, -4, 17, 29, 37, 46, 49, 48, 42};
const int Params::BISHOP_MOBILITY[15] = {-45, -19, -4, 6, 17, 24, 28, 32, 37, 40, 41, 44, 57, 40, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-82, -59, -48, -43, -42, -37, -33, -27, -24, -19, -14, -7, 5, 16, 10}, {-96, -60, -41, -25, -7, -1, 0, 4, 10, 14, 19, 25, 30, 20, 15}};
const int Params::QUEEN_MOBILITY[2][24] = {{-89, -26, -14, -2, 4, 11, 17, 21, 27, 30, 32, 33, 34, 34, 37, 39, 37, 49, 60, 72, 92, 92, 96, 96}, {-96, -96, -96, -96, -80, -62, -43, -41, -35, -32, -28, -25, -20, -18, -18, -16, -16, -20, -27, -28, -24, -25, -32, -43}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-76, -41, -23, -13, -12};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 71}, {16, 46}};
const int Params::BISHOP_OUTPOST[2][2] = {{20, 62}, {23, 29}};
const int Params::PAWN_STORM[4][2] = {{0, 27},{11, 5},{33, 2},{4, 0}};

