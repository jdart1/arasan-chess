// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 07-Dec-2018 10:29:30 by tuner -n 175 -c 24 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0680972

//

#include "params.h"

const int Params::RB_ADJUST[6] = {49, -6, 94, 46, 42, 76};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -9, -39, -32};
const int Params::QR_ADJUST[5] = {16, 66, 29, -28, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -270, -152};
const int Params::CASTLING[6] = {13, -38, -28, 0, 5, -21};
const int Params::KING_COVER[6][4] = {{8, 24, 14, 16},
{2, 6, 4, 4},
{-8, -25, -15, -17},
{-10, -29, -17, -19},
{-9, -27, -16, -18},
{-16, -49, -28, -33}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -225;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -180;
const int Params::MINOR_FOR_PAWNS = 63;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 39;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 4;
const int Params::PAWN_ATTACK_FACTOR2 = 4;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 43;
const int Params::ROOK_ATTACK_FACTOR = 61;
const int Params::ROOK_ATTACK_BOOST = 22;
const int Params::ROOK_ATTACK_BOOST2 = 43;
const int Params::QUEEN_ATTACK_FACTOR = 63;
const int Params::QUEEN_ATTACK_BOOST = 46;
const int Params::QUEEN_ATTACK_BOOST2 = 64;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 6;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 126;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 85;
const int Params::OWN_ROOK_KING_PROXIMITY = 34;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 35;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 81;
const int Params::PIECE_THREAT_MM_END = 33;
const int Params::PIECE_THREAT_MR_END = 84;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 10;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 24;
const int Params::PIECE_THREAT_RR_MID = 11;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 33;
const int Params::PIECE_THREAT_RR_END = 1;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 42;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 54;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -24;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -4;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -21;
const int Params::WEAK_ON_OPEN_FILE_END = -17;
const int Params::SPACE = 5;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 28;
const int Params::TWO_ROOKS_ON_7TH_MID = 29;
const int Params::TWO_ROOKS_ON_7TH_END = 64;
const int Params::ROOK_ON_OPEN_FILE_MID = 34;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 21;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 62;
const int Params::QUEENING_SQUARE_CONTROL_END = 56;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -16;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 23, 53, 81, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={253, 213, 146};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 20, 22, 24, 25, 27, 29, 31, 33, 36, 38, 41, 43, 46, 49, 52, 55, 59, 62, 66, 70, 74, 79, 83, 88, 93, 98, 103, 109, 114, 120, 126, 133, 139, 146, 153, 161, 168, 176, 184, 192, 200, 208, 217, 226, 235, 244, 253, 262, 271, 281, 290, 300, 310, 319, 329, 338, 348, 357, 367, 376, 385, 395, 404, 412, 421, 430, 438, 446, 454, 462, 470, 477, 484, 491, 498, 505, 511, 517, 523, 529, 534, 539, 544, 549, 554, 558, 562, 567, 570, 574, 578, 581, 584, 587, 590, 593, 596, 598, 601, 603, 605, 607, 609, 611, 613, 614, 616, 617, 619, 620, 621, 622, 624, 625, 626, 627, 627, 628, 629, 630, 631, 631};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 0, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 36, 128, 191}, {0, 0, 0, 0, 25, 82, 160, 271}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 15, 96, 0}, {0, 0, 0, 0, 0, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 5, 7, 23, 102, 203, 320}, {0, 0, 0, 0, 37, 89, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 28, 13, 36, 96, 190}, {0, 0, 0, 9, 10, 26, 61, 67}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-36, -24, -32, -8, -35, -6, -21, -26, -4, -27, -49, -19, 0, -17, 0, -40, -12, -39, -16, -55, 0}, {0, 0, 0, -3, -12, 0, 0, -10, -7, -19, -26, -6, -2, -11, -13, -17, -17, -29, -53, -42, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-60, -73, -54, -53, -46, -52, -59, -54, -26, -31, -32, -43, -8, 0, -3, -32, 0, -1, -67, -35, -83}, {-10, -3, 0, -7, -2, -1, -17, -8, -7, -5, -4, -27, -12, -9, 0, -51, -21, -13, -54, -38, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-42, 0, -18, -11, -11, -18, 0, -42}, {-42, -19, -25, -20, -20, -25, -19, -42}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -27, -2, -17, -17, -2, -27, 0}, {-84, -73, -37, -40, -40, -37, -73, -84}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -17, -23, -23, -17, 0, 0}, {-5, -9, -11, -17, -17, -11, -9, -5}};

const int Params::KNIGHT_PST[2][64] = {{-81, -24, -8, -2, -2, -8, -24, -81, -21, -20, -11, 10, 10, -11, -20, -21, -40, -14, 6, 31, 31, 6, -14, -40, -13, 4, 25, 27, 27, 25, 4, -13, -24, -7, 25, 20, 20, 25, -7, -24, -49, -43, 3, 8, 8, 3, -43, -49, -101, -81, -25, -17, -17, -25, -81, -101, -128, -128, -128, -128, -128, -128, -128, -128}, {-77, -39, -27, -30, -30, -27, -39, -77, -36, -24, -23, -14, -14, -23, -24, -36, -46, -27, -17, 5, 5, -17, -27, -46, -21, -9, 10, 19, 19, 10, -9, -21, -33, -24, -3, 1, 1, -3, -24, -33, -53, -36, -17, -11, -11, -17, -36, -53, -73, -49, -47, -31, -31, -47, -49, -73, -128, -79, -78, -60, -60, -78, -79, -128}};
const int Params::BISHOP_PST[2][64] = {{18, 45, 33, 31, 31, 33, 45, 18, 37, 49, 40, 33, 33, 40, 49, 37, 15, 42, 41, 34, 34, 41, 42, 15, 3, 12, 23, 43, 43, 23, 12, 3, -32, -8, -1, 27, 27, -1, -8, -32, 6, -24, -38, -23, -23, -38, -24, 6, -29, -63, -60, -96, -96, -60, -63, -29, -19, -84, -128, -128, -128, -128, -84, -19}, {-10, 4, 9, 5, 5, 9, 4, -10, 10, 13, 8, 15, 15, 8, 13, 10, 3, 16, 21, 24, 24, 21, 16, 3, 2, 11, 25, 30, 30, 25, 11, 2, -18, -3, -3, 7, 7, -3, -3, -18, 3, -10, -10, -10, -10, -10, -10, 3, -6, -11, -18, -22, -22, -18, -11, -6, -2, -3, -8, -27, -27, -8, -3, -2}};
const int Params::ROOK_PST[2][64] = {{-47, -33, -27, -24, -24, -27, -33, -47, -60, -33, -29, -24, -24, -29, -33, -60, -58, -33, -33, -32, -32, -33, -33, -58, -56, -41, -38, -29, -29, -38, -41, -56, -34, -26, -13, -11, -11, -13, -26, -34, -22, 5, -6, 7, 7, -6, 5, -22, -20, -26, -0, 3, 3, -0, -26, -20, -12, -6, -98, -63, -63, -98, -6, -12}, {-8, -9, -0, -6, -6, -0, -9, -8, -18, -12, -4, -4, -4, -4, -12, -18, -14, -9, -2, -4, -4, -2, -9, -14, -3, 6, 11, 6, 6, 11, 6, -3, 12, 14, 19, 16, 16, 19, 14, 12, 18, 22, 27, 26, 26, 27, 22, 18, 6, 9, 15, 16, 16, 15, 9, 6, 28, 28, 27, 23, 23, 27, 28, 28}};
const int Params::QUEEN_PST[2][64] = {{29, 30, 26, 38, 38, 26, 30, 29, 23, 40, 42, 43, 43, 42, 40, 23, 22, 37, 41, 31, 31, 41, 37, 22, 35, 39, 41, 41, 41, 41, 39, 35, 37, 41, 45, 53, 53, 45, 41, 37, 27, 47, 51, 48, 48, 51, 47, 27, 31, 12, 35, 38, 38, 35, 12, 31, 43, 44, 19, 7, 7, 19, 44, 43}, {-51, -64, -48, -32, -32, -48, -64, -51, -36, -33, -26, -9, -9, -26, -33, -36, -25, 3, 14, 5, 5, 14, 3, -25, 6, 25, 32, 34, 34, 32, 25, 6, 14, 45, 51, 65, 65, 51, 45, 14, 6, 42, 62, 79, 79, 62, 42, 6, 25, 45, 71, 82, 82, 71, 45, 25, 1, 17, 15, 18, 18, 15, 17, 1}};
const int Params::KING_PST[2][64] = {{27, 43, -17, 7, 7, -17, 43, 27, 71, 72, 19, 44, 44, 19, 72, 71, -15, 7, -72, -127, -127, -72, 7, -15, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -112, -128, -128, -128, -128, -112, -128, 20, -37, -128, -128, -128, -128, -37, 20, -128, -128, 128, -128, -128, 128, -128, -128}, {-56, -57, -61, -66, -66, -61, -57, -56, -45, -52, -58, -58, -58, -58, -52, -45, -54, -58, -55, -53, -53, -55, -58, -54, -51, -48, -44, -41, -41, -44, -48, -51, -38, -33, -26, -25, -25, -26, -33, -38, -32, -21, -20, -20, -20, -20, -21, -32, -41, -31, -28, -29, -29, -28, -31, -41, -68, -45, -44, -42, -42, -44, -45, -68}};

const int Params::KNIGHT_MOBILITY[9] = {-71, -26, -5, 7, 16, 25, 27, 25, 18};
const int Params::BISHOP_MOBILITY[15] = {-54, -27, -13, -2, 9, 16, 20, 23, 27, 31, 31, 33, 43, 27, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-61, -37, -25, -21, -20, -13, -9, -1, 3, 6, 8, 12, 22, 29, 26}, {-69, -32, -12, 4, 20, 27, 28, 31, 38, 42, 47, 51, 54, 43, 34}};
const int Params::QUEEN_MOBILITY[2][24] = {{-59, 1, 14, 23, 29, 36, 43, 49, 54, 58, 60, 61, 64, 65, 68, 71, 70, 84, 96, 96, 96, 96, 96, 96}, {-96, -96, -84, -43, 9, 20, 37, 43, 51, 55, 60, 66, 72, 75, 80, 78, 84, 80, 80, 83, 85, 88, 79, 74}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-89, -51, -30, -18, -13};

const int Params::KNIGHT_OUTPOST[2][2] = {{24, 62}, {20, 33}};
const int Params::BISHOP_OUTPOST[2][2] = {{33, 63}, {35, 27}};

const int Params::PAWN_STORM[2][2][4] = {{{0, -14, -21, -4},{0, -89, -36, -10}},
{{-22, -25, 0, 0},{-119, -31, -1, 0}}};

