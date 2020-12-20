// Copyright 2015-2020 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 06-Oct-2020 07:57:50 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/data/epd/big7.epd
// Final objective value: 0.0650207

//

#include "params.h"

const int Params::KN_VS_PAWN_ADJUST[3] = {0, -117, 20};
const int Params::KING_COVER[6][4] = {{9, 25, 16, 17},
{1, 4, 3, 3},
{-9, -26, -17, -18},
{-8, -25, -16, -17},
{-11, -32, -21, -22},
{-18, -54, -35, -37}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 94;
const int Params::PIN_MULTIPLIER_END = 65;
const int Params::MINOR_FOR_PAWNS_MIDGAME = 168;
const int Params::MINOR_FOR_PAWNS_ENDGAME = 95;
const int Params::RB_ADJUST_MIDGAME = 95;
const int Params::RB_ADJUST_ENDGAME = 67;
const int Params::RBN_ADJUST_MIDGAME = -102;
const int Params::RBN_ADJUST_ENDGAME = 29;
const int Params::QR_ADJUST_MIDGAME = 12;
const int Params::QR_ADJUST_ENDGAME = 36;
const int Params::Q_VS_3MINORS_MIDGAME = -282;
const int Params::Q_VS_3MINORS_ENDGAME = -12;
const int Params::KRMINOR_VS_R_NO_PAWNS = -149;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -126;
const int Params::TRADE_DOWN1 = -22;
const int Params::TRADE_DOWN2 = 44;
const int Params::TRADE_DOWN3 = 19;
const int Params::PAWN_ENDGAME_ADJUST = 12;
const int Params::PAWN_ATTACK_FACTOR = 16;
const int Params::MINOR_ATTACK_FACTOR = 46;
const int Params::MINOR_ATTACK_BOOST = 25;
const int Params::ROOK_ATTACK_FACTOR = 54;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 58;
const int Params::QUEEN_ATTACK_BOOST = 44;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 116;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 5;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 76;
const int Params::OWN_ROOK_KING_PROXIMITY = 23;
const int Params::OWN_QUEEN_KING_PROXIMITY = 15;
const int Params::PAWN_PUSH_THREAT_MID = 26;
const int Params::PAWN_PUSH_THREAT_END = 14;
const int Params::ENDGAME_KING_THREAT = 37;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 40;
const int Params::BISHOP_PAIR_END = 67;
const int Params::BISHOP_PAWN_PLACEMENT_END = -5;
const int Params::BAD_BISHOP_MID = -6;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -22;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -9;
const int Params::WEAK_PAWN_END = -5;
const int Params::WEAK_ON_OPEN_FILE_MID = -19;
const int Params::WEAK_ON_OPEN_FILE_END = -17;
const int Params::SPACE = 7;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 13;
const int Params::TWO_ROOKS_ON_7TH_MID = 66;
const int Params::TWO_ROOKS_ON_7TH_END = 87;
const int Params::TRAPPED_ROOK = 0;
const int Params::TRAPPED_ROOK_NO_CASTLE = -85;
const int Params::ROOK_ON_OPEN_FILE_MID = 29;
const int Params::ROOK_ON_OPEN_FILE_END = 11;
const int Params::ROOK_BEHIND_PP_MID = 3;
const int Params::ROOK_BEHIND_PP_END = 25;
const int Params::QUEEN_OUT = -27;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 17;
const int Params::KING_OPP_PAWN_DISTANCE = 8;
const int Params::SIDE_PROTECTED_PAWN = -3;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 3, 28, 58, 96, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={244, 162, 134};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 9, 10, 11, 12, 13, 15, 16, 17, 18, 20, 21, 23, 24, 26, 28, 30, 31, 33, 36, 38, 40, 43, 45, 48, 51, 54, 57, 60, 64, 67, 71, 75, 79, 83, 87, 92, 96, 101, 106, 112, 117, 123, 129, 135, 141, 147, 154, 160, 167, 174, 182, 189, 197, 205, 212, 220, 229, 237, 245, 254, 263, 271, 280, 289, 298, 307, 315, 324, 333, 342, 351, 360, 368, 377, 386, 394, 402, 410, 419, 426, 434, 442, 449, 457, 464, 471, 477, 484, 490, 497, 503, 508, 514, 520, 525, 530, 535, 539, 544, 548, 553, 557, 560, 564, 568, 571, 574, 577, 580, 583, 586, 589, 591, 593, 596, 598, 600, 602, 604, 606, 607, 609, 610, 612, 613, 614, 616, 617, 618, 619, 620, 621};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 13, 17, 23, 31, 38, 59, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 39, 125, 200}, {0, 0, 0, 0, 20, 74, 147, 236}};
const int Params::PASSED_PAWN_FILE_ADJUST[4] = {66, 69, 67, 64};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 24, 63, 114, 206}, {0, 0, 0, 0, 43, 68, 186, 283}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 31, 99, 171}, {0, 0, 0, 8, 11, 31, 67, 84}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{14, 0, 0, 0, 9, 128}, {17, 3, 29, 38, 79, 119}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-33, 0, 0}, {-3, -4, -45}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-18, -46, -53}, {-34, -47, -72}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{31, 51, 128}, {22, 47, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{-3, -23, 0}, {-18, -31, -44}};
const int Params::DOUBLED_PAWNS[2][8] = {{-36, 0, -14, -7, -7, -14, 0, -36}, {-37, -15, -20, -18, -18, -20, -15, -37}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -13, 0, 0, 0, 0, -13, 0}, {-67, -53, -35, -34, -34, -35, -53, -67}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -6, -23, -28, -28, -23, -6, 0}, {-11, -17, -16, -22, -22, -16, -17, -11}};

const int Params::THREAT_BY_PAWN[2][5] = {{96, 106, 96, 51, 0}, {66, 82, 72, 31, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{13, 41, 71, 127, 77}, {29, 35, 57, 76, 103}};
const int Params::THREAT_BY_BISHOP[2][5] = {{22, 55, 66, 85, 83}, {28, 48, 51, 66, 125}};
const int Params::THREAT_BY_ROOK[2][5] = {{20, 54, 60, 124, 87}, {27, 43, 49, 1, 128}};
const int Params::KNIGHT_PST[2][64] = {{-95, -48, -29, -20, -20, -29, -48, -95, -40, -37, -28, -5, -5, -28, -37, -40, -57, -29, -8, 19, 19, -8, -29, -57, -23, 0, 18, 22, 22, 18, 0, -23, -37, -19, 15, 8, 8, 15, -19, -37, -69, -60, -15, -17, -17, -15, -60, -69, -107, -94, -38, -34, -34, -38, -94, -107, -128, -128, -128, -128, -128, -128, -128, -128}, {-85, -43, -32, -31, -31, -32, -43, -85, -37, -28, -29, -18, -18, -29, -28, -37, -44, -28, -18, 1, 1, -18, -28, -44, -20, -11, 6, 17, 17, 6, -11, -20, -38, -33, -15, -9, -9, -15, -33, -38, -56, -41, -26, -25, -25, -26, -41, -56, -74, -53, -50, -36, -36, -50, -53, -74, -128, -77, -84, -71, -71, -84, -77, -128}};
const int Params::BISHOP_PST[2][64] = {{-7, 24, 9, 11, 11, 9, 24, -7, 13, 26, 22, 16, 16, 22, 26, 13, -5, 22, 23, 19, 19, 23, 22, -5, 0, 1, 11, 30, 30, 11, 1, 0, -26, -0, -0, 17, 17, -0, -0, -26, 18, -19, -23, -14, -14, -23, -19, 18, -21, -42, -43, -66, -66, -43, -42, -21, 9, -59, -128, -128, -128, -128, -59, 9}, {-31, -13, -1, -7, -7, -1, -13, -31, -15, -4, -6, 3, 3, -6, -4, -15, -10, 4, 9, 13, 13, 9, 4, -10, -5, 4, 13, 15, 15, 13, 4, -5, -15, -2, -5, -1, -1, -5, -2, -15, 6, -9, -9, -11, -11, -9, -9, 6, -3, -5, -16, -15, -15, -16, -5, -3, 1, -4, -7, -24, -24, -7, -4, 1}};
const int Params::ROOK_PST[2][64] = {{-23, -13, -8, -3, -3, -8, -13, -23, -45, -17, -8, -3, -3, -8, -17, -45, -41, -17, -14, -8, -8, -14, -17, -41, -40, -25, -24, -8, -8, -24, -25, -40, -21, -13, 0, 4, 4, 0, -13, -21, -20, 2, 1, 11, 11, 1, 2, -20, -31, -38, -24, -13, -13, -24, -38, -31, -28, -17, -128, -96, -96, -128, -17, -28}, {-16, -19, -10, -15, -15, -10, -19, -16, -25, -19, -12, -13, -13, -12, -19, -25, -18, -12, -6, -7, -7, -6, -12, -18, -1, 5, 10, 7, 7, 10, 5, -1, 14, 16, 21, 16, 16, 21, 16, 14, 15, 18, 20, 18, 18, 20, 18, 15, 4, 8, 10, 10, 10, 10, 8, 4, 23, 28, 28, 21, 21, 28, 28, 23}};
const int Params::QUEEN_PST[2][64] = {{-5, -5, -9, 1, 1, -9, -5, -5, -9, 6, 8, 11, 11, 8, 6, -9, -8, 6, 13, 3, 3, 13, 6, -8, 6, 13, 13, 15, 15, 13, 13, 6, 15, 17, 19, 21, 21, 19, 17, 15, -8, 12, 23, 14, 14, 23, 12, -8, -15, -21, -3, -5, -5, -3, -21, -15, 26, 16, -4, -30, -30, -4, 16, 26}, {-80, -86, -68, -52, -52, -68, -86, -80, -57, -58, -48, -33, -33, -48, -58, -57, -45, -18, -6, -19, -19, -6, -18, -45, -14, 3, 16, 16, 16, 16, 3, -14, -9, 30, 34, 49, 49, 34, 30, -9, -12, 20, 43, 58, 58, 43, 20, -12, -1, 21, 46, 60, 60, 46, 21, -1, -30, -7, -5, -2, -2, -5, -7, -30}};
const int Params::KING_PST[2][64] = {{8, 25, -26, 12, 12, -26, 25, 8, 47, 46, -17, 6, 6, -17, 46, 47, -59, -33, -104, -128, -128, -104, -33, -59, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -103, -128, -128, -128, -128, -103, -128, 10, 16, -128, -128, -128, -128, 16, 10, 53, -128, -111, 20, 20, -111, -128, 53}, {-6, -1, -5, -12, -12, -5, -1, -6, 11, 5, -0, 1, 1, -0, 5, 11, 4, 3, 7, 7, 7, 7, 3, 4, 11, 16, 22, 25, 25, 22, 16, 11, 25, 35, 43, 44, 44, 43, 35, 25, 34, 46, 48, 47, 47, 48, 46, 34, 21, 35, 39, 36, 36, 39, 35, 21, -22, 12, 14, 17, 17, 14, 12, -22}};
const int Params::KNIGHT_MOBILITY[9] = {-86, -38, -15, -2, 9, 19, 23, 22, 14};
const int Params::BISHOP_MOBILITY[15] = {-62, -34, -19, -7, 5, 13, 18, 22, 28, 31, 33, 33, 48, 25, 38};
const int Params::ROOK_MOBILITY[2][15] = {{-96, -77, -66, -63, -62, -56, -53, -46, -42, -37, -31, -26, -13, -1, 2}, {-96, -55, -34, -19, -2, 4, 4, 10, 15, 21, 29, 35, 40, 34, 27}};
const int Params::QUEEN_MOBILITY[2][24] = {{-87, -22, -5, 6, 13, 20, 27, 33, 40, 44, 46, 49, 50, 52, 55, 56, 56, 67, 77, 83, 95, 96, 96, 96}, {-96, -96, -96, -96, -58, -44, -30, -26, -19, -16, -11, -7, -4, -0, 0, 1, 0, -3, -6, -10, -8, -9, -15, -25}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-51, -13, 5, 17, 20};
const int Params::KNIGHT_OUTPOST[2][2] = {{29, 59}, {23, 43}};
const int Params::BISHOP_OUTPOST[2][2] = {{13, 41}, {19, 24}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -21, 30, 6},{20, -36, 17, 30, -8},{20, 27, 13, 45, 11},{20, 0, 24, 44, 17}},{{10, -46, 0, -28, -9},{10, -64, -10, -9, -2},{10, 37, 25, 15, 8},{10, 64, -13, 11, -1}}};

