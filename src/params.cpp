// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 09-Dec-2017 05:11:04 by "tuner -n 200 -R 25 -c 24 /home/jdart/chess/epd/big4.epd"
//

#include "params.h"

const int Params::RB_ADJUST[6] = {45, -6, 78, 25, 18, 48};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -9, -36, -32};
const int Params::QR_ADJUST[5] = {41, 95, 33, -43, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -292, -158};
const int Params::CASTLING[6] = {13, -34, -25, 0, 8, -13};
const int Params::KING_COVER[6][4] = {{13, 25, 16, 13},
{5, 11, 7, 5},
{-4, -9, -5, -4},
{-2, -4, -3, -2},
{-8, -16, -10, -8},
{-18, -36, -22, -18}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 73;
const int Params::PIN_MULTIPLIER_END = 74;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -185;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -165;
const int Params::MINOR_FOR_PAWNS = 34;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 44;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 8;
const int Params::PAWN_ATTACK_FACTOR2 = 8;
const int Params::MINOR_ATTACK_FACTOR = 31;
const int Params::MINOR_ATTACK_BOOST = 38;
const int Params::ROOK_ATTACK_FACTOR = 47;
const int Params::ROOK_ATTACK_BOOST = 20;
const int Params::ROOK_ATTACK_BOOST2 = 36;
const int Params::QUEEN_ATTACK_FACTOR = 49;
const int Params::QUEEN_ATTACK_BOOST = 45;
const int Params::QUEEN_ATTACK_BOOST2 = 53;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 9;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 167;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 12;
const int Params::PIECE_THREAT_MM_MID = 32;
const int Params::PIECE_THREAT_MR_MID = 81;
const int Params::PIECE_THREAT_MQ_MID = 55;
const int Params::PIECE_THREAT_MM_END = 32;
const int Params::PIECE_THREAT_MR_END = 76;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 8;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 13;
const int Params::PIECE_THREAT_RR_MID = 0;
const int Params::PIECE_THREAT_RQ_MID = 86;
const int Params::PIECE_THREAT_RM_END = 23;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 14;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 38;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 33;
const int Params::BISHOP_PAIR_END = 58;
const int Params::BISHOP_PAWN_PLACEMENT_END = -2;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -27;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -23;
const int Params::WEAK_ON_OPEN_FILE_END = -21;
const int Params::SPACE = 5;
const int Params::PAWN_CENTER_SCORE_MID = 0;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 24;
const int Params::TWO_ROOKS_ON_7TH_MID = 18;
const int Params::TWO_ROOKS_ON_7TH_END = 76;
const int Params::ROOK_ON_OPEN_FILE_MID = 24;
const int Params::ROOK_ON_OPEN_FILE_END = 16;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 14;
const int Params::QUEEN_OUT = -31;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 10;
const int Params::KING_OPP_PAWN_DISTANCE = 3;
const int Params::QUEENING_SQUARE_CONTROL_MID = 66;
const int Params::QUEENING_SQUARE_CONTROL_END = 51;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -24;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -49;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 1, 23, 51, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={300, 227, 158};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-2, -2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 21, 22, 24, 26, 28, 30, 33, 35, 38, 41, 44, 47, 50, 53, 57, 61, 65, 69, 73, 78, 83, 88, 93, 98, 104, 110, 116, 123, 130, 137, 144, 151, 159, 167, 175, 184, 193, 201, 211, 220, 229, 239, 249, 258, 268, 278, 289, 299, 309, 319, 329, 339, 349, 359, 369, 379, 389, 398, 407, 417, 426, 434, 443, 451, 459, 467, 475, 482, 489, 496, 502, 509, 515, 521, 526, 532, 537, 542, 546, 551, 555, 559, 563, 567, 570, 573, 576, 579, 582, 585, 587, 590, 592, 594, 596, 598, 600, 601, 603, 604, 606, 607, 608, 610, 611, 612, 613, 614, 614, 615, 616, 617, 617, 618, 619, 619, 620, 620, 621, 621};
const int Params::TRADE_DOWN[8] = {45, 40, 34, 29, 23, 18, 12, 7};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 24, 93, 114}, {0, 0, 0, 0, 16, 66, 150, 224}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {80, 82, 80, 76, 76, 80, 82, 80};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 7, 96, 0}, {0, 0, 0, 0, 0, 18, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 9, 95, 195, 288}, {0, 0, 0, 0, 49, 92, 255, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 5, 0, 24, 90, 176}, {0, 0, 0, 6, 17, 32, 59, 67}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-43, -39, -37, -22, -38, -9, -26, -31, -8, -37, 0, -35, 0, -29, -17, -40, -17, -43, -24, -43, 0}, {0, 0, -3, -11, -16, -10, -2, -14, -13, -27, -26, -6, -5, -7, -17, -20, -17, -35, -43, -43, -43}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-43, -43, -43, -43, -43, -43, -43, -43, -34, -38, -41, -43, -18, -5, -7, -26, 0, 0, -43, -20, -43}, {-17, -15, -6, -10, -12, -6, -22, -9, -14, -7, -11, -25, -6, -3, -3, -43, -22, -14, -43, -43, -43}};
const int Params::DOUBLED_PAWNS[2][8] = {{-45, -1, -22, -19, -19, -22, -1, -45}, {-44, -21, -29, -19, -19, -29, -21, -44}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -19, -13, 0, 0, -13, -19, 0}, {0, -63, -19, -54, -54, -19, -63, 0}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -12, -13, -13, -12, 0, 0}, {-4, -7, -9, -17, -17, -9, -7, -4}};

const int Params::KNIGHT_PST[2][64] = {{-76, -32, -15, -14, -14, -15, -32, -76, -23, -34, -27, -4, -4, -27, -34, -23, -42, -21, -6, 9, 9, -6, -21, -42, -25, -16, 3, 5, 5, 3, -16, -25, -15, -16, 8, -7, -7, 8, -16, -15, -37, -78, -57, -19, -19, -57, -78, -37, -80, -53, -8, -8, -8, -8, -53, -80, -128, -128, -128, -128, -128, -128, -128, -128}, {-56, -46, -35, -30, -30, -35, -46, -56, -39, -32, -25, -20, -20, -25, -32, -39, -44, -32, -23, -2, -2, -23, -32, -44, -23, -19, 5, 11, 11, 5, -19, -23, -22, -21, -6, -7, -7, -6, -21, -22, -44, -52, -36, -39, -39, -36, -52, -44, -60, -43, -32, -26, -26, -32, -43, -60, -128, -58, -67, -60, -60, -67, -58, -128}};
const int Params::BISHOP_PST[2][64] = {{3, 36, 17, 14, 14, 17, 36, 3, 20, 26, 26, 16, 16, 26, 26, 20, 0, 21, 22, 18, 18, 22, 21, 0, -9, -9, 5, 19, 19, 5, -9, -9, -37, -18, -24, -7, -7, -24, -18, -37, -13, -69, -97, -57, -57, -97, -69, -13, -39, -46, -35, -95, -95, -35, -46, -39, -38, -64, -128, -128, -128, -128, -64, -38}, {-24, -5, -1, -6, -6, -1, -5, -24, -4, -0, -5, 3, 3, -5, -0, -4, -8, 0, 7, 9, 9, 7, 0, -8, -11, 2, 11, 18, 18, 11, 2, -11, -26, -10, -7, 0, 0, -7, -10, -26, -7, -56, -52, -18, -18, -52, -56, -7, -9, -25, -32, -36, -36, -32, -25, -9, -7, 2, -24, -2, -2, -24, 2, -7}};
const int Params::ROOK_PST[2][64] = {{-79, -67, -62, -58, -58, -62, -67, -79, -90, -65, -64, -60, -60, -64, -65, -90, -82, -64, -67, -71, -71, -67, -64, -82, -83, -68, -74, -66, -66, -74, -68, -83, -64, -56, -53, -48, -48, -53, -56, -64, -62, -31, -37, -30, -30, -37, -31, -62, -54, -61, -36, -27, -27, -36, -61, -54, -50, -56, -128, -128, -128, -128, -56, -50}, {-8, -7, 0, -5, -5, 0, -7, -8, -12, -10, -1, -2, -2, -1, -10, -12, -13, -6, -4, -3, -3, -4, -6, -13, -4, 6, 8, 5, 5, 8, 6, -4, 9, 9, 14, 13, 13, 14, 9, 9, 10, 21, 21, 18, 18, 21, 21, 10, 2, 7, 14, 14, 14, 14, 7, 2, 27, 29, 30, 22, 22, 30, 29, 27}};
const int Params::QUEEN_PST[2][64] = {{17, 20, 20, 29, 29, 20, 20, 17, 20, 28, 30, 35, 35, 30, 28, 20, 13, 26, 30, 20, 20, 30, 26, 13, 25, 28, 27, 30, 30, 27, 28, 25, 27, 28, 30, 33, 33, 30, 28, 27, 17, 20, 34, 33, 33, 34, 20, 17, 7, -6, 16, 12, 12, 16, -6, 7, 1, 23, -6, -10, -10, -6, 23, 1}, {-26, -28, -25, -10, -10, -25, -28, -26, -14, -17, -7, 9, 9, -7, -17, -14, 3, 22, 31, 28, 28, 31, 22, 3, 12, 39, 48, 56, 56, 48, 39, 12, 25, 61, 66, 84, 84, 66, 61, 25, 23, 54, 90, 92, 92, 90, 54, 23, 46, 59, 86, 102, 102, 86, 59, 46, 16, 20, 28, 35, 35, 28, 20, 16}};
const int Params::KING_PST[2][64] = {{43, 61, 19, 32, 32, 19, 61, 43, 77, 74, 40, 59, 59, 40, 74, 77, 14, 56, -17, -38, -38, -17, 56, 14, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -123, -128, -128, -128, -128, -123, -128, -102, -60, -128, -128, -128, -128, -60, -102, -28, -128, -128, -128, -128, -128, -128, -28}, {-85, -82, -85, -89, -89, -85, -82, -85, -71, -75, -80, -78, -78, -80, -75, -71, -77, -78, -73, -72, -72, -73, -78, -77, -73, -68, -61, -58, -58, -61, -68, -73, -59, -53, -44, -42, -42, -44, -53, -59, -56, -45, -40, -37, -37, -40, -45, -56, -57, -49, -46, -46, -46, -46, -49, -57, -88, -66, -63, -62, -62, -63, -66, -88}};

const int Params::KNIGHT_MOBILITY[9] = {-60, -25, -8, 1, 8, 16, 18, 17, 11};
const int Params::BISHOP_MOBILITY[15] = {-42, -25, -13, -4, 4, 11, 16, 18, 24, 26, 27, 31, 39, 19, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-43, -28, -20, -17, -15, -9, -5, 4, 8, 13, 16, 16, 34, 33, 43}, {-43, -24, -8, 6, 16, 20, 22, 24, 30, 33, 37, 42, 43, 37, 30}};
const int Params::QUEEN_MOBILITY[2][24] = {{-43, -12, -11, -6, -0, 5, 10, 14, 18, 23, 23, 25, 26, 26, 25, 27, 34, 38, 43, 43, 43, 43, 43, 43}, {-43, -43, -43, -33, -20, -7, 12, 15, 26, 22, 29, 36, 32, 40, 39, 38, 43, 43, 41, 41, 43, 43, 36, 21}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-43, -13, 7, 16, 19};

const int Params::KNIGHT_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{16, 16},{19, 17},{54, 19},{54, 19},{19, 17},{16, 16},{0, 0},{0, 13},{62, 30},{64, 31},{29, 49},{29, 49},{64, 31},{62, 30},{0, 13},{0, 2},{1, 12},{3, 3},{0, 8},{0, 8},{3, 3},{1, 12},{0, 2},{0, 0},{0, 0},{0, 4},{0, 15},{0, 15},{0, 4},{0, 0},{0, 0}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 15},{28, 23},{43, 29},{64, 39},{64, 39},{43, 29},{28, 23},{0, 15},{18, 45},{64, 46},{64, 52},{64, 56},{64, 56},{64, 52},{64, 46},{18, 45},{64, 0},{30, 25},{10, 24},{46, 3},{46, 3},{10, 24},{30, 25},{64, 0},{10, 0},{64, 64},{64, 59},{47, 0},{47, 0},{64, 59},{64, 64},{10, 0}}};
const int Params::BISHOP_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{19, 30},{10, 20},{44, 24},{45, 25},{45, 25},{44, 24},{10, 20},{19, 30},{45, 36},{64, 64},{64, 64},{47, 28},{47, 28},{64, 64},{64, 64},{45, 36},{34, 24},{0, 44},{1, 36},{0, 36},{0, 36},{1, 36},{0, 44},{34, 24},{20, 26},{0, 19},{0, 45},{0, 3},{0, 3},{0, 45},{0, 19},{20, 26}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{11, 0},{34, 24},{51, 21},{64, 25},{64, 25},{51, 21},{34, 24},{11, 0},{0, 0},{64, 64},{64, 64},{64, 19},{64, 19},{64, 64},{64, 64},{0, 0},{0, 0},{64, 0},{13, 0},{64, 0},{64, 0},{13, 0},{64, 0},{0, 0},{0, 0},{64, 0},{64, 0},{64, 0},{64, 0},{64, 0},{64, 0},{0, 0}}};


