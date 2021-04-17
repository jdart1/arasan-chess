// Copyright 2015-2021 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 15-Apr-2021 03:37:33 by tuner -n 175 -c 24 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0591396

//

#include "params.h"

const int Params::KN_VS_PAWN_ADJUST[3] = {0, -119, 39};
const int Params::KING_COVER[6][4] = {{7, 23, 14, 15},
{2, 5, 3, 3},
{-8, -25, -16, -16},
{-8, -24, -15, -16},
{-11, -32, -20, -21},
{-17, -52, -33, -34}};
const int Params::PAWN_VALUE_MIDGAME = 128;
const int Params::PAWN_VALUE_ENDGAME = 137;
const int Params::KNIGHT_VALUE_MIDGAME = 504;
const int Params::KNIGHT_VALUE_ENDGAME = 483;
const int Params::BISHOP_VALUE_MIDGAME = 519;
const int Params::BISHOP_VALUE_ENDGAME = 513;
const int Params::ROOK_VALUE_MIDGAME = 760;
const int Params::ROOK_VALUE_ENDGAME = 794;
const int Params::QUEEN_VALUE_MIDGAME = 1566;
const int Params::QUEEN_VALUE_ENDGAME = 1507;
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 91;
const int Params::PIN_MULTIPLIER_END = 62;
const int Params::MINOR_FOR_PAWNS_MIDGAME = 91;
const int Params::MINOR_FOR_PAWNS_ENDGAME = 67;
const int Params::RB_ADJUST_MIDGAME = 64;
const int Params::RB_ADJUST_ENDGAME = 44;
const int Params::RBN_ADJUST_MIDGAME = -59;
const int Params::RBN_ADJUST_ENDGAME = 35;
const int Params::QR_ADJUST_MIDGAME = 45;
const int Params::QR_ADJUST_ENDGAME = 47;
const int Params::Q_VS_3MINORS_MIDGAME = -224;
const int Params::Q_VS_3MINORS_ENDGAME = -15;
const int Params::KRMINOR_VS_R_NO_PAWNS = -210;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -194;
const int Params::TRADE_DOWN1 = -27;
const int Params::TRADE_DOWN2 = 37;
const int Params::TRADE_DOWN3 = 19;
const int Params::PAWN_ATTACK_FACTOR = 14;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 21;
const int Params::ROOK_ATTACK_FACTOR = 51;
const int Params::ROOK_ATTACK_BOOST = 20;
const int Params::QUEEN_ATTACK_FACTOR = 57;
const int Params::QUEEN_ATTACK_BOOST = 39;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 119;
const int Params::KING_ATTACK_COUNT = 3;
const int Params::KING_ATTACK_SQUARES = 7;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 69;
const int Params::OWN_ROOK_KING_PROXIMITY = 27;
const int Params::OWN_QUEEN_KING_PROXIMITY = 13;
const int Params::PAWN_PUSH_THREAT_MID = 25;
const int Params::PAWN_PUSH_THREAT_END = 16;
const int Params::ENDGAME_KING_THREAT = 35;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 42;
const int Params::BISHOP_PAIR_END = 69;
const int Params::BISHOP_PAWN_PLACEMENT_END = -2;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -4;
const int Params::CENTER_PAWN_BLOCK = -22;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -9;
const int Params::WEAK_PAWN_END = -6;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -15;
const int Params::SPACE = 7;
const int Params::PAWN_CENTER_SCORE_MID = 4;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 12;
const int Params::TWO_ROOKS_ON_7TH_MID = 64;
const int Params::TWO_ROOKS_ON_7TH_END = 82;
const int Params::TRAPPED_ROOK = 0;
const int Params::TRAPPED_ROOK_NO_CASTLE = -75;
const int Params::ROOK_ON_OPEN_FILE_MID = 28;
const int Params::ROOK_ON_OPEN_FILE_END = 13;
const int Params::ROOK_BEHIND_PP_MID = 1;
const int Params::ROOK_BEHIND_PP_END = 25;
const int Params::QUEEN_OUT = -27;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 27;
const int Params::KING_OPP_PAWN_DISTANCE = 4;
const int Params::SIDE_PROTECTED_PAWN = -4;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 55, 58, 79, 96, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={224, 144, 128};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 22, 24, 26, 27, 29, 31, 33, 36, 38, 40, 43, 45, 48, 51, 54, 58, 61, 64, 68, 72, 76, 80, 85, 89, 94, 99, 104, 109, 115, 121, 127, 133, 139, 146, 153, 160, 167, 174, 182, 190, 198, 206, 214, 223, 231, 240, 249, 258, 267, 276, 286, 295, 305, 314, 324, 333, 343, 352, 362, 371, 380, 390, 399, 408, 417, 426, 434, 443, 451, 460, 468, 475, 483, 491, 498, 505, 512, 518, 525, 531, 537, 543, 549, 554, 559, 564, 569, 574, 578, 582, 586, 590, 594, 598, 601, 604, 608, 611, 613, 616, 619, 621, 624, 626, 628, 630, 632, 634, 635, 637, 639, 640, 642, 643, 644, 645, 646, 648, 649, 650, 650};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 13, 16, 22, 30, 36, 54, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 43, 128, 223}, {0, 0, 11, 9, 37, 88, 160, 276}};
const int Params::PASSED_PAWN_FILE_ADJUST[4] = {68, 70, 67, 64};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 42, 84, 144, 320}, {0, 0, 0, 3, 44, 71, 206, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 8, 1, 37, 120, 192}, {0, 0, 0, 13, 18, 38, 68, 76}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{11, 0, 0, 0, 20, 128}, {21, 6, 27, 40, 82, 128}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-31, 0, 0}, {0, -0, -58}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-23, -46, -67}, {-30, -53, -105}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{35, 64, 128}, {24, 48, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{-3, -22, 0}, {-17, -34, -70}};
const int Params::DOUBLED_PAWNS[2][8] = {{-39, -12, -21, -8, 0, 0, 0, 0}, {0, 0, 0, -47, 0, 0, 0, 0}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -64, -64, -64, 0, 0, 0, 0}, {0, -34, -17, -14, 0, 0, 0, 0}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -23, -32, 0, 0, 0, 0}, {-20, -29, -25, -14, 0, 0, 0, 0}};

const int Params::THREAT_BY_PAWN[2][5] = {{76, 76, 76, 65, 0}, {56, 80, 56, 14, 127}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 13, 67, 115, 79}, {30, 36, 58, 67, 77}};
const int Params::THREAT_BY_BISHOP[2][5] = {{21, 48, 36, 64, 89}, {28, 48, 86, 58, 113}};
const int Params::THREAT_BY_ROOK[2][5] = {{20, 47, 50, 0, 95}, {29, 39, 45, 48, 121}};
const int Params::KNIGHT_PST[2][64] = {{-85, -30, -13, -8, -8, -13, -30, -85, -26, -26, -16, 7, 7, -16, -26, -26, -39, -16, 2, 28, 28, 2, -16, -39, -10, 12, 26, 31, 31, 26, 12, -10, -23, -10, 26, 16, 16, 26, -10, -23, -58, -42, 3, 5, 5, 3, -42, -58, -97, -78, -27, -25, -25, -27, -78, -97, -128, -111, -96, -74, -74, -96, -111, -128}, {-75, -38, -25, -22, -22, -25, -38, -75, -36, -23, -22, -11, -11, -22, -23, -36, -36, -23, -13, 8, 8, -13, -23, -36, -15, -6, 14, 26, 26, 14, -6, -15, -32, -25, -4, 2, 2, -4, -25, -32, -49, -32, -16, -14, -14, -16, -32, -49, -66, -46, -44, -33, -33, -44, -46, -66, -128, -63, -63, -54, -54, -63, -63, -128}};
const int Params::BISHOP_PST[2][64] = {{-13, 18, 5, 6, 6, 5, 18, -13, 9, 21, 15, 9, 9, 15, 21, 9, -8, 16, 17, 11, 11, 17, 16, -8, -6, -3, 3, 25, 25, 3, -3, -6, -31, -4, -3, 16, 16, -3, -4, -31, 13, -16, 5, -8, -8, 5, -16, 13, -24, -29, -37, -42, -42, -37, -29, -24, 12, -26, -19, -105, -105, -19, -26, 12}, {-47, -30, -22, -27, -27, -22, -30, -47, -34, -24, -25, -18, -18, -25, -24, -34, -29, -16, -11, -8, -8, -11, -16, -29, -25, -15, -7, -2, -2, -7, -15, -25, -25, -15, -14, -9, -9, -14, -15, -25, -5, -19, -18, -21, -21, -18, -19, -5, -12, -13, -21, -23, -23, -21, -13, -12, -12, -12, -12, -27, -27, -12, -12, -12}};
const int Params::ROOK_PST[2][64] = {{-22, -14, -9, -4, -4, -9, -14, -22, -42, -18, -10, -4, -4, -10, -18, -42, -40, -19, -12, -10, -10, -12, -19, -40, -39, -26, -22, -9, -9, -22, -26, -39, -19, -12, 1, 2, 2, 1, -12, -19, -17, 4, 3, 19, 19, 3, 4, -17, -27, -32, -18, -12, -12, -18, -32, -27, -16, -13, -21, -10, -10, -21, -13, -16}, {-39, -41, -33, -39, -39, -33, -41, -39, -45, -39, -33, -33, -33, -33, -39, -45, -38, -34, -28, -28, -28, -28, -34, -38, -22, -18, -12, -15, -15, -12, -18, -22, -9, -8, -4, -6, -6, -4, -8, -9, -9, -6, -4, -6, -6, -4, -6, -9, -19, -14, -13, -14, -14, -13, -14, -19, -0, 4, 4, -7, -7, 4, 4, -0}};
const int Params::QUEEN_PST[2][64] = {{-7, -9, -10, -3, -3, -10, -9, -7, -14, 1, 4, 6, 6, 4, 1, -14, -12, 1, 8, -2, -2, 8, 1, -12, 3, 8, 10, 9, 9, 10, 8, 3, 10, 12, 19, 19, 19, 19, 12, 10, -14, 10, 17, 14, 14, 17, 10, -14, -13, -19, 2, -8, -8, 2, -19, -13, -2, -20, -22, -63, -63, -22, -20, -2}, {-86, -85, -74, -58, -58, -74, -85, -86, -72, -63, -59, -43, -43, -59, -63, -72, -56, -25, -17, -30, -30, -17, -25, -56, -19, -5, 6, 4, 4, 6, -5, -19, -19, 14, 20, 36, 36, 20, 14, -19, -26, 12, 28, 40, 40, 28, 12, -26, -3, 13, 35, 44, 44, 35, 13, -3, -32, -24, -18, -17, -17, -18, -24, -32}};
const int Params::KING_PST[2][64] = {{28, 43, -6, 32, 32, -6, 43, 28, 63, 61, 1, 23, 23, 1, 61, 63, -35, -11, -69, -128, -128, -69, -11, -35, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -92, -128, -128, -128, -128, -92, -128, 80, -126, -128, -128, -128, -128, -126, 80, -45, -107, -47, 91, 91, -47, -107, -45}, {48, 49, 45, 38, 38, 45, 49, 48, 68, 58, 50, 51, 51, 50, 58, 68, 59, 56, 58, 58, 58, 58, 56, 59, 67, 71, 76, 78, 78, 76, 71, 67, 85, 95, 102, 102, 102, 102, 95, 85, 96, 106, 110, 107, 107, 110, 106, 96, 80, 94, 99, 97, 97, 99, 94, 80, 33, 69, 76, 80, 80, 76, 69, 33}};
const int Params::KNIGHT_MOBILITY[9] = {-84, -35, -14, -0, 10, 20, 25, 23, 15};
const int Params::BISHOP_MOBILITY[15] = {-58, -33, -18, -7, 4, 12, 17, 22, 27, 30, 31, 32, 42, 25, 38};
const int Params::ROOK_MOBILITY[2][15] = {{-71, -50, -40, -38, -38, -32, -29, -22, -18, -13, -6, -0, 10, 15, 18}, {-96, -63, -44, -29, -12, -5, -4, 2, 8, 14, 21, 28, 34, 30, 24}};
const int Params::QUEEN_MOBILITY[2][24] = {{-88, -25, -14, -4, 1, 8, 13, 19, 24, 28, 31, 33, 35, 36, 40, 42, 46, 53, 68, 63, 96, 96, 96, 96}, {-96, -96, -96, -86, -47, -29, -11, -8, 2, 3, 9, 12, 16, 20, 21, 20, 21, 20, 13, 12, 16, 13, 4, -7}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-58, -16, -0, 10, 13};
const int Params::KNIGHT_OUTPOST[2][2] = {{29, 58}, {19, 40}};
const int Params::BISHOP_OUTPOST[2][2] = {{11, 34}, {7, 16}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -28, 28, 6},{20, -38, 23, 33, -4},{20, 32, 12, 43, 11},{20, -23, 24, 43, 15}},{{10, 64, -3, -22, -9},{10, -64, -10, -6, -1},{10, 33, 18, 15, 8},{10, 64, -16, 10, -1}}};

