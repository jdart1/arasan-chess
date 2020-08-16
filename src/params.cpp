// Copyright 2015-2020 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 15-Aug-2020 04:09:52 by tuner -n 160 -R 20 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0657537

//

#include "params.h"

const int Params::KN_VS_PAWN_ADJUST[3] = {0, -105, 30};
const int Params::CASTLING[6] = {13, -38, -30, 0, 4, -25};
const int Params::KING_COVER[6][4] = {{8, 25, 16, 17},
{2, 5, 3, 3},
{-8, -25, -16, -17},
{-8, -24, -15, -16},
{-10, -31, -20, -20},
{-17, -52, -33, -34}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 93;
const int Params::PIN_MULTIPLIER_END = 64;
const int Params::MINOR_FOR_PAWNS_MIDGAME = 159;
const int Params::MINOR_FOR_PAWNS_ENDGAME = 92;
const int Params::RB_ADJUST_MIDGAME = 85;
const int Params::RB_ADJUST_ENDGAME = 67;
const int Params::RBN_ADJUST_MIDGAME = -101;
const int Params::RBN_ADJUST_ENDGAME = 28;
const int Params::QR_ADJUST_MIDGAME = 2;
const int Params::QR_ADJUST_ENDGAME = 28;
const int Params::Q_VS_3MINORS_MIDGAME = -284;
const int Params::Q_VS_3MINORS_ENDGAME = -15;
const int Params::KRMINOR_VS_R_NO_PAWNS = -154;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -134;
const int Params::TRADE_DOWN1 = -25;
const int Params::TRADE_DOWN2 = 44;
const int Params::TRADE_DOWN3 = 19;
const int Params::PAWN_ENDGAME_ADJUST = 8;
const int Params::PAWN_ATTACK_FACTOR = 14;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 24;
const int Params::ROOK_ATTACK_FACTOR = 54;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 60;
const int Params::QUEEN_ATTACK_BOOST = 43;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 114;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 5;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 74;
const int Params::OWN_ROOK_KING_PROXIMITY = 25;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
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
const int Params::WEAK_PAWN_END = -5;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 7;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 13;
const int Params::TWO_ROOKS_ON_7TH_MID = 65;
const int Params::TWO_ROOKS_ON_7TH_END = 85;
const int Params::ROOK_ON_OPEN_FILE_MID = 29;
const int Params::ROOK_ON_OPEN_FILE_END = 11;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 27;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 18;
const int Params::KING_OPP_PAWN_DISTANCE = 8;
const int Params::SIDE_PROTECTED_PAWN = -3;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 1, 29, 59, 96, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={247, 157, 131};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 9, 10, 11, 12, 13, 15, 16, 17, 19, 20, 21, 23, 25, 27, 28, 30, 32, 35, 37, 39, 42, 44, 47, 50, 53, 56, 60, 63, 67, 70, 74, 79, 83, 87, 92, 97, 102, 107, 113, 119, 124, 130, 137, 143, 150, 157, 164, 171, 179, 186, 194, 202, 210, 219, 227, 236, 245, 253, 262, 271, 281, 290, 299, 308, 318, 327, 336, 345, 355, 364, 373, 382, 391, 400, 408, 417, 425, 433, 441, 449, 457, 465, 472, 479, 486, 493, 499, 506, 512, 518, 523, 529, 534, 540, 544, 549, 554, 558, 562, 566, 570, 574, 578, 581, 584, 587, 590, 593, 596, 598, 601, 603, 605, 607, 609, 611, 613, 615, 616, 618, 619, 621, 622, 623, 624, 625, 626, 627, 628, 629};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 12, 15, 21, 29, 35, 56, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 35, 122, 198}, {0, 0, 0, 0, 20, 73, 146, 232}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {66, 69, 67, 64, 64, 67, 69, 66};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 24, 76, 110, 138}, {0, 0, 0, 0, 42, 67, 184, 291}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 30, 100, 172}, {0, 0, 0, 8, 11, 30, 64, 83}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{2, 0, 0, 0, 0, 128}, {17, 3, 28, 37, 74, 113}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-33, -4, 0}, {-3, -6, -34}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-19, -48, -60}, {-33, -49, -73}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{30, 51, 128}, {21, 45, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{-2, -25, 0}, {-18, -33, -43}};
const int Params::DOUBLED_PAWNS[2][8] = {{-37, 0, -13, -8, -8, -13, 0, -37}, {-33, -14, -19, -16, -16, -19, -14, -33}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -14, 0, 0, 0, 0, -14, 0}, {-78, -56, -31, -36, -36, -31, -56, -78}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -5, -21, -26, -26, -21, -5, 0}, {-11, -17, -15, -22, -22, -15, -17, -11}};

const int Params::THREAT_BY_PAWN[2][5] = {{91, 101, 98, 51, 0}, {69, 85, 77, 33, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 31, 69, 127, 74}, {28, 25, 57, 77, 111}};
const int Params::THREAT_BY_BISHOP[2][5] = {{20, 51, 73, 82, 77}, {27, 47, 48, 68, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{19, 54, 58, 118, 94}, {27, 42, 47, 5, 128}};
const int Params::KNIGHT_PST[2][64] = {{-98, -42, -27, -20, -20, -27, -42, -98, -41, -39, -29, -6, -6, -29, -39, -41, -54, -29, -9, 16, 16, -9, -29, -54, -24, -1, 14, 17, 17, 14, -1, -24, -35, -19, 13, 5, 5, 13, -19, -35, -66, -58, -17, -14, -14, -17, -58, -66, -103, -94, -38, -37, -37, -38, -94, -103, -128, -128, -128, -128, -128, -128, -128, -128}, {-87, -44, -33, -33, -33, -33, -44, -87, -40, -29, -30, -18, -18, -30, -29, -40, -44, -29, -19, 1, 1, -19, -29, -44, -21, -11, 6, 16, 16, 6, -11, -21, -38, -32, -14, -7, -7, -14, -32, -38, -54, -39, -26, -24, -24, -26, -39, -54, -73, -52, -50, -36, -36, -50, -52, -73, -128, -75, -82, -69, -69, -82, -75, -128}};
const int Params::BISHOP_PST[2][64] = {{-8, 22, 9, 8, 8, 9, 22, -8, 12, 24, 18, 13, 13, 18, 24, 12, -5, 19, 21, 15, 15, 21, 19, -5, -2, -1, 8, 27, 27, 8, -1, -2, -26, -2, -3, 16, 16, -3, -2, -26, 16, -18, -27, -15, -15, -27, -18, 16, -27, -46, -43, -67, -67, -43, -46, -27, -6, -63, -128, -128, -128, -128, -63, -6}, {-33, -15, -4, -9, -9, -4, -15, -33, -15, -5, -7, 2, 2, -7, -5, -15, -11, 3, 8, 11, 11, 8, 3, -11, -7, 3, 12, 14, 14, 12, 3, -7, -15, -3, -4, 1, 1, -4, -3, -15, 7, -10, -9, -11, -11, -9, -10, 7, -2, -5, -14, -14, -14, -14, -5, -2, 2, -4, -6, -24, -24, -6, -4, 2}};
const int Params::ROOK_PST[2][64] = {{-46, -33, -28, -23, -23, -28, -33, -46, -59, -35, -29, -24, -24, -29, -35, -59, -56, -35, -33, -29, -29, -33, -35, -56, -55, -43, -42, -31, -31, -42, -43, -55, -37, -32, -21, -20, -20, -21, -32, -37, -37, -17, -22, -13, -13, -22, -17, -37, -49, -56, -41, -33, -33, -41, -56, -49, -43, -42, -128, -113, -113, -128, -42, -43}, {-22, -24, -15, -21, -21, -15, -24, -22, -32, -25, -18, -17, -17, -18, -25, -32, -23, -17, -11, -12, -12, -11, -17, -23, -6, 1, 6, 1, 1, 6, 1, -6, 9, 11, 14, 10, 10, 14, 11, 9, 10, 12, 15, 12, 12, 15, 12, 10, -0, 3, 4, 3, 3, 4, 3, -0, 19, 22, 23, 15, 15, 23, 22, 19}};
const int Params::QUEEN_PST[2][64] = {{2, -0, -3, 7, 7, -3, -0, 2, -4, 11, 14, 16, 16, 14, 11, -4, -2, 10, 17, 8, 8, 17, 10, -2, 12, 17, 18, 19, 19, 18, 17, 12, 19, 22, 25, 26, 26, 25, 22, 19, -3, 18, 28, 20, 20, 28, 18, -3, -7, -13, 4, 7, 7, 4, -13, -7, 16, 11, -15, -36, -36, -15, 11, 16}, {-95, -98, -80, -66, -66, -80, -98, -95, -75, -66, -61, -44, -44, -61, -66, -75, -57, -27, -18, -29, -29, -18, -27, -57, -27, -7, 3, 4, 4, 3, -7, -27, -19, 14, 23, 38, 38, 23, 14, -19, -23, 8, 31, 43, 43, 31, 8, -23, -8, 9, 36, 47, 47, 36, 9, -8, -40, -28, -22, -16, -16, -22, -28, -40}};
const int Params::KING_PST[2][64] = {{14, 31, -29, -9, -9, -29, 31, 14, 51, 51, 2, 27, 27, 2, 51, 51, -44, -19, -79, -128, -128, -79, -19, -44, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -113, -128, -128, -128, -128, -113, -128, 13, -4, -128, -128, -128, -128, -4, 13, -105, -128, -128, 7, 7, -128, -128, -105}, {-2, 2, -3, -8, -8, -3, 2, -2, 16, 8, 2, 3, 3, 2, 8, 16, 8, 6, 9, 10, 10, 9, 6, 8, 14, 19, 25, 27, 27, 25, 19, 14, 30, 38, 46, 46, 46, 46, 38, 30, 40, 50, 53, 51, 51, 53, 50, 40, 25, 41, 44, 41, 41, 44, 41, 25, -10, 19, 21, 22, 22, 21, 19, -10}};
const int Params::KNIGHT_MOBILITY[9] = {-84, -37, -16, -3, 7, 17, 21, 19, 12};
const int Params::BISHOP_MOBILITY[15] = {-58, -31, -17, -6, 5, 13, 17, 21, 26, 29, 31, 32, 45, 24, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-80, -56, -45, -41, -40, -35, -31, -24, -20, -16, -11, -5, 6, 20, 11}, {-96, -53, -34, -18, -2, 5, 5, 11, 16, 22, 29, 35, 41, 33, 27}};
const int Params::QUEEN_MOBILITY[2][24] = {{-76, -22, -10, 1, 7, 13, 20, 25, 31, 36, 37, 39, 42, 43, 46, 47, 49, 58, 67, 71, 94, 95, 96, 96}, {-96, -96, -96, -96, -58, -40, -24, -21, -13, -10, -6, -1, 1, 5, 5, 6, 5, 3, -2, -4, -1, -3, -13, -21}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-59, -23, -6, 5, 8};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 58}, {21, 41}};
const int Params::BISHOP_OUTPOST[2][2] = {{12, 39}, {17, 23}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -20, 26, 3},{20, -27, 22, 30, -8},{20, 30, 14, 40, 10},{20, -16, 29, 41, 15}},{{10, 12, 1, -25, -9},{10, -64, -4, -8, -1},{10, 30, 21, 15, 8},{10, 64, -12, 12, -2}}};

