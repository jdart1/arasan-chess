// Copyright 2015-2020 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 20-Dec-2020 05:08:36 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/data/epd/big7.epd
// Final objective value: 0.0585482

//

#include "params.h"

const int PAWN_VALUE_MIDGAME = 128;
const int PAWN_VALUE_ENDGAME = 144;
const int KNIGHT_VALUE_MIDGAME = int(4.0*PAWN_VALUE_MIDGAME);
const int KNIGHT_VALUE_ENDGAME = int(4.0)*PAWN_VALUE_MIDGAME);
const int BISHOP_VALUE_MIDGAME = int(4.0*PAWN_VALUE_MIDGAME);
const int BISHOP_VALUE_ENDGAME = int(4.0*PAWN_VALUE_MIDGAME);
const int ROOK_VALUE_MIDGAME = int(6.0*PAWN_VALUE_MIDGAME);
const int ROOK_VALUE_ENDGAME = int(6.0*PAWN_VALUE_MIDGAME);
const int QUEEN_VALUE_MIDGAME = int(12.0*PAWN_VALUE_MIDGAME);
const int QUEEN_VALUE_ENDGAME  = int(12.0*PAWN_VALUE_MIDGAME);
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -107, 57};
const int Params::KING_COVER[6][4] = {{8, 23, 15, 15},
{1, 4, 2, 3},
{-9, -26, -17, -17},
{-8, -24, -16, -16},
{-11, -33, -21, -22},
{-18, -55, -36, -37}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 91;
const int Params::PIN_MULTIPLIER_END = 65;
const int Params::MINOR_FOR_PAWNS_MIDGAME = 141;
const int Params::MINOR_FOR_PAWNS_ENDGAME = 85;
const int Params::RB_ADJUST_MIDGAME = 95;
const int Params::RB_ADJUST_ENDGAME = 60;
const int Params::RBN_ADJUST_MIDGAME = -62;
const int Params::RBN_ADJUST_ENDGAME = 69;
const int Params::QR_ADJUST_MIDGAME = 60;
const int Params::QR_ADJUST_ENDGAME = 42;
const int Params::Q_VS_3MINORS_MIDGAME = -190;
const int Params::Q_VS_3MINORS_ENDGAME = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -167;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -152;
const int Params::TRADE_DOWN1 = -15;
const int Params::TRADE_DOWN2 = 71;
const int Params::TRADE_DOWN3 = 20;
const int Params::PAWN_ENDGAME_ADJUST = 25;
const int Params::PAWN_ATTACK_FACTOR = 17;
const int Params::MINOR_ATTACK_FACTOR = 46;
const int Params::MINOR_ATTACK_BOOST = 22;
const int Params::ROOK_ATTACK_FACTOR = 53;
const int Params::ROOK_ATTACK_BOOST = 21;
const int Params::QUEEN_ATTACK_FACTOR = 55;
const int Params::QUEEN_ATTACK_BOOST = 41;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 118;
const int Params::KING_ATTACK_COUNT = 4;
const int Params::KING_ATTACK_SQUARES = 7;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 72;
const int Params::OWN_ROOK_KING_PROXIMITY = 24;
const int Params::OWN_QUEEN_KING_PROXIMITY = 13;
const int Params::PAWN_PUSH_THREAT_MID = 26;
const int Params::PAWN_PUSH_THREAT_END = 14;
const int Params::ENDGAME_KING_THREAT = 36;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 45;
const int Params::BISHOP_PAIR_END = 73;
const int Params::BISHOP_PAWN_PLACEMENT_END = -6;
const int Params::BAD_BISHOP_MID = -6;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -21;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -9;
const int Params::WEAK_PAWN_END = -5;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -21;
const int Params::SPACE = 7;
const int Params::PAWN_CENTER_SCORE_MID = 4;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 13;
const int Params::TWO_ROOKS_ON_7TH_MID = 58;
const int Params::TWO_ROOKS_ON_7TH_END = 86;
const int Params::TRAPPED_ROOK = 0;
const int Params::TRAPPED_ROOK_NO_CASTLE = -77;
const int Params::ROOK_ON_OPEN_FILE_MID = 29;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 3;
const int Params::ROOK_BEHIND_PP_END = 26;
const int Params::QUEEN_OUT = -27;
const int Params::PAWN_SIDE_BONUS = 17;
const int Params::KING_OWN_PAWN_DISTANCE = 23;
const int Params::KING_OPP_PAWN_DISTANCE = 5;
const int Params::SIDE_PROTECTED_PAWN = -4;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 60, 61, 78, 96, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={236, 140, 128};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 23, 24, 26, 28, 30, 32, 34, 36, 38, 40, 43, 45, 48, 51, 54, 57, 60, 63, 67, 71, 74, 78, 82, 87, 91, 96, 101, 106, 111, 116, 122, 127, 133, 139, 146, 152, 159, 165, 172, 180, 187, 195, 202, 210, 218, 226, 234, 243, 251, 260, 269, 277, 286, 295, 304, 313, 322, 331, 340, 349, 357, 366, 375, 384, 392, 401, 409, 418, 426, 434, 442, 449, 457, 464, 472, 479, 485, 492, 499, 505, 511, 517, 523, 528, 534, 539, 544, 549, 554, 558, 562, 567, 571, 574, 578, 582, 585, 588, 591, 594, 597, 600, 602, 605, 607, 610, 612, 614, 616, 618, 619, 621, 623, 624, 626, 627, 628, 630, 631, 632, 633, 634};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 13, 17, 22, 31, 38, 57, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 47, 128, 223}, {0, 0, 1, 0, 29, 81, 154, 279}};
const int Params::PASSED_PAWN_FILE_ADJUST[4] = {67, 69, 66, 64};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 40, 74, 131, 320}, {0, 0, 0, 3, 46, 72, 207, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 6, 5, 37, 119, 192}, {0, 0, 0, 12, 16, 37, 71, 78}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{37, 0, 0, 0, 26, 128}, {22, 5, 26, 41, 85, 128}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-33, 0, 0}, {0, 0, -75}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-22, -41, -57}, {-31, -55, -109}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{36, 64, 128}, {25, 48, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{-3, -16, 0}, {-17, -35, -75}};
const int Params::DOUBLED_PAWNS[2][8] = {{-40, -13, -23, -9, 0, 0, 0, 0}, {0, 0, -0, -64, 0, 0, 0, 0}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -64, -64, -64, 0, 0, 0, 0}, {-56, -46, -33, -8, 0, 0, 0, 0}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -23, -32, 0, 0, 0, 0}, {-17, -28, -25, -12, 0, 0, 0, 0}};

const int Params::THREAT_BY_PAWN[2][5] = {{78, 79, 76, 65, 0}, {55, 79, 54, 18, 128}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{13, 12, 68, 117, 79}, {31, 50, 57, 67, 73}};
const int Params::THREAT_BY_BISHOP[2][5] = {{22, 49, 31, 65, 94}, {29, 49, 82, 56, 104}};
const int Params::THREAT_BY_ROOK[2][5] = {{21, 48, 51, 0, 86}, {30, 42, 48, 46, 116}};
const int Params::KNIGHT_PST[2][64] = {{-69, -20, -4, 5, 5, -4, -20, -69, -15, -14, -4, 18, 18, -4, -14, -15, -32, -6, 14, 41, 41, 14, -6, -32, 1, 23, 40, 46, 46, 40, 23, 1, -15, 1, 38, 29, 29, 38, 1, -15, -49, -34, 14, 13, 13, 14, -34, -49, -86, -65, -15, -10, -10, -15, -65, -86, -128, -99, -77, -57, -57, -77, -99, -128}, {-64, -24, -11, -8, -8, -11, -24, -64, -21, -9, -10, 1, 1, -10, -9, -21, -25, -10, 1, 22, 22, 1, -10, -25, -2, 7, 27, 39, 39, 27, 7, -2, -20, -13, 9, 15, 15, 9, -13, -20, -38, -20, -2, -1, -1, -2, -20, -38, -57, -36, -30, -17, -17, -30, -36, -57, -128, -53, -49, -42, -42, -49, -53, -128}};
const int Params::BISHOP_PST[2][64] = {{3, 35, 22, 25, 25, 22, 35, 3, 25, 37, 33, 27, 27, 33, 37, 25, 7, 32, 34, 30, 30, 34, 32, 7, 11, 14, 21, 45, 45, 21, 14, 11, -14, 13, 15, 33, 33, 15, 13, -14, 30, 0, 23, 10, 10, 23, 0, 30, -4, -9, -19, -23, -23, -19, -9, -4, 39, -22, -2, -85, -85, -2, -22, 39}, {-18, 1, 11, 7, 7, 11, 1, -18, -5, 8, 6, 15, 15, 6, 8, -5, 3, 15, 22, 24, 24, 22, 15, 3, 8, 17, 25, 30, 30, 25, 17, 8, 8, 18, 19, 23, 23, 19, 18, 8, 27, 15, 15, 12, 12, 15, 15, 27, 19, 19, 10, 9, 9, 10, 19, 19, 27, 21, 21, 6, 6, 21, 21, 27}};
const int Params::ROOK_PST[2][64] = {{-22, -13, -8, -3, -3, -8, -13, -22, -42, -16, -8, -2, -2, -8, -16, -42, -40, -17, -11, -7, -7, -11, -17, -40, -38, -24, -21, -5, -5, -21, -24, -38, -17, -9, 5, 9, 9, 5, -9, -17, -16, 5, 8, 23, 23, 8, 5, -16, -27, -32, -19, -11, -11, -19, -32, -27, -12, -6, -16, -9, -9, -16, -6, -12}, {1, -1, 6, 0, 0, 6, -1, 1, -5, 2, 7, 6, 6, 7, 2, -5, 1, 7, 13, 11, 11, 13, 7, 1, 17, 22, 27, 24, 24, 27, 22, 17, 29, 30, 36, 32, 32, 36, 30, 29, 27, 32, 34, 33, 33, 34, 32, 27, 17, 24, 25, 24, 24, 25, 24, 17, 38, 44, 45, 36, 36, 45, 44, 38}};
const int Params::QUEEN_PST[2][64] = {{8, 9, 7, 15, 15, 7, 9, 8, 3, 19, 21, 25, 25, 21, 19, 3, 5, 19, 27, 17, 17, 27, 19, 5, 20, 26, 28, 29, 29, 28, 26, 20, 27, 31, 36, 37, 37, 36, 31, 27, 3, 27, 35, 29, 29, 35, 27, 3, 1, -6, 14, 7, 7, 14, -6, 1, 24, 10, 3, -30, -30, 3, 10, 24}, {-48, -52, -38, -23, -23, -38, -52, -48, -27, -32, -22, -10, -10, -22, -32, -27, -18, 9, 18, 3, 3, 18, 9, -18, 14, 30, 41, 38, 38, 41, 30, 14, 16, 51, 55, 71, 71, 55, 51, 16, 10, 49, 67, 78, 78, 67, 49, 10, 26, 48, 69, 81, 81, 69, 48, 26, 6, 18, 22, 22, 22, 22, 18, 6}};
const int Params::KING_PST[2][64] = {{10, 26, -24, 14, 14, -24, 26, 10, 48, 47, -17, 9, 9, -17, 47, 48, -55, -28, -90, -128, -128, -90, -28, -55, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -81, -128, -128, -128, -128, -81, -128, 32, -116, -123, -128, -128, -123, -116, 32, -73, -128, -88, -16, -16, -88, -128, -73}, {39, 42, 39, 32, 32, 39, 42, 39, 59, 52, 46, 47, 47, 46, 52, 59, 52, 51, 54, 54, 54, 54, 51, 52, 61, 67, 72, 75, 75, 72, 67, 61, 78, 89, 98, 99, 99, 98, 89, 78, 88, 100, 105, 104, 104, 105, 100, 88, 73, 87, 94, 92, 92, 94, 87, 73, 12, 60, 64, 72, 72, 64, 60, 12}};
const int Params::KNIGHT_MOBILITY[9] = {-72, -21, 1, 15, 26, 36, 41, 39, 31};
const int Params::BISHOP_MOBILITY[15] = {-38, -11, 5, 17, 29, 37, 43, 47, 53, 57, 57, 57, 70, 49, 38};
const int Params::ROOK_MOBILITY[2][15] = {{-72, -51, -40, -37, -37, -31, -27, -20, -15, -10, -4, 2, 13, 19, 24}, {-78, -34, -12, 4, 22, 28, 29, 36, 41, 48, 55, 62, 67, 64, 55}};
const int Params::QUEEN_MOBILITY[2][24] = {{-77, -8, 6, 15, 22, 29, 36, 41, 48, 52, 54, 57, 58, 60, 64, 63, 68, 77, 90, 87, 96, 96, 96, 96}, {-96, -96, -80, -33, -3, 12, 29, 34, 42, 45, 52, 54, 58, 64, 64, 64, 64, 61, 59, 56, 62, 59, 53, 41}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-56, -12, 5, 16, 19};
const int Params::KNIGHT_OUTPOST[2][2] = {{30, 59}, {19, 39}};
const int Params::BISHOP_OUTPOST[2][2] = {{10, 36}, {7, 15}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -31, 30, 7},{20, -47, 16, 34, -4},{20, 39, 11, 48, 13},{20, -7, 21, 45, 17}},{{10, 64, -4, -26, -10},{10, -64, -16, -6, -0},{10, 39, 22, 17, 9},{10, 64, -18, 12, -2}}};

