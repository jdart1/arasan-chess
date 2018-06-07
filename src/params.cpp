// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 06-Jun-2018 04:32:39 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/epd/big5.epd
// Final objective value: 0.0759694

//

#include "params.h"

const int Params::RB_ADJUST[6] = {54, -6, 89, 39, 31, 61};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -13, -37, -32};
const int Params::QR_ADJUST[5] = {22, 72, 25, -34, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -293, -165};
const int Params::CASTLING[6] = {13, -38, -24, 0, 5, -18};
const int Params::KING_COVER[6][4] = {{11, 22, 14, 12},
{5, 9, 6, 5},
{-6, -12, -8, -7},
{-5, -10, -7, -6},
{-9, -18, -11, -10},
{-18, -35, -23, -20}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 79;
const int Params::PIN_MULTIPLIER_END = 82;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -184;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -148;
const int Params::MINOR_FOR_PAWNS = 40;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 40;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 8;
const int Params::PAWN_ATTACK_FACTOR2 = 8;
const int Params::MINOR_ATTACK_FACTOR = 33;
const int Params::MINOR_ATTACK_BOOST = 39;
const int Params::ROOK_ATTACK_FACTOR = 47;
const int Params::ROOK_ATTACK_BOOST = 21;
const int Params::ROOK_ATTACK_BOOST2 = 33;
const int Params::QUEEN_ATTACK_FACTOR = 50;
const int Params::QUEEN_ATTACK_BOOST = 44;
const int Params::QUEEN_ATTACK_BOOST2 = 53;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 9;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 163;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 12;
const int Params::PIECE_THREAT_MM_MID = 32;
const int Params::PIECE_THREAT_MR_MID = 88;
const int Params::PIECE_THREAT_MQ_MID = 68;
const int Params::PIECE_THREAT_MM_END = 31;
const int Params::PIECE_THREAT_MR_END = 74;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 9;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 20;
const int Params::PIECE_THREAT_RR_MID = 3;
const int Params::PIECE_THREAT_RQ_MID = 93;
const int Params::PIECE_THREAT_RM_END = 30;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 27;
const int Params::ENDGAME_KING_THREAT = 39;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 33;
const int Params::BISHOP_PAIR_END = 57;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -6;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -26;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -2;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -24;
const int Params::WEAK_ON_OPEN_FILE_END = -19;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 0;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 25;
const int Params::TWO_ROOKS_ON_7TH_MID = 17;
const int Params::TWO_ROOKS_ON_7TH_END = 68;
const int Params::ROOK_ON_OPEN_FILE_MID = 28;
const int Params::ROOK_ON_OPEN_FILE_END = 16;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 20;
const int Params::QUEEN_OUT = -31;
const int Params::PAWN_SIDE_BONUS = 17;
const int Params::KING_OWN_PAWN_DISTANCE = 11;
const int Params::KING_OPP_PAWN_DISTANCE = 4;
const int Params::QUEENING_SQUARE_CONTROL_MID = 59;
const int Params::QUEENING_SQUARE_CONTROL_END = 47;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -22;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 23, 53, 86, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={286, 208, 146};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 13, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 34, 36, 39, 41, 44, 47, 51, 54, 58, 61, 65, 69, 74, 78, 83, 88, 94, 99, 105, 111, 117, 123, 130, 137, 144, 152, 159, 167, 175, 184, 192, 201, 210, 219, 229, 238, 248, 258, 267, 277, 287, 298, 308, 318, 328, 338, 348, 358, 368, 378, 387, 397, 406, 415, 424, 433, 442, 450, 458, 466, 474, 481, 489, 495, 502, 509, 515, 521, 527, 532, 537, 542, 547, 552, 556, 560, 564, 568, 572, 575, 578, 581, 584, 587, 590, 592, 594, 597, 599, 601, 603, 604, 606, 608, 609, 611, 612, 613, 614, 615, 616, 617, 618, 619, 620, 621, 621, 622, 623, 623, 624, 624, 625, 625};
const int Params::TRADE_DOWN[8] = {45, 40, 34, 29, 23, 18, 12, 7};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 25, 99, 162}, {0, 0, 0, 0, 17, 65, 145, 224}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {85, 87, 84, 80, 80, 84, 87, 85};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 10, 96, 0}, {0, 0, 0, 0, 0, 16, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 2, 3, 12, 96, 212, 288}, {0, 0, 0, 0, 43, 90, 247, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 22, 4, 25, 88, 157}, {0, 0, 0, 7, 13, 29, 57, 55}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-44, -39, -43, -20, -41, -7, -28, -32, -14, -31, -45, -32, 0, -20, -40, -41, -8, -54, -33, -96, 0}, {0, 0, -0, -10, -17, -15, -0, -14, -14, -26, -27, -6, -5, -9, -16, -20, -18, -29, -66, -58, -91}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-76, -69, -68, -56, -55, -53, -64, -58, -33, -38, -39, -49, -14, -1, -8, -28, 0, 0, -55, -30, -96}, {-12, -11, 0, -4, -8, -4, -22, -10, -13, -8, -10, -28, -11, -5, -2, -50, -21, -15, -67, -49, -96}};
const int Params::DOUBLED_PAWNS[2][8] = {{-44, 0, -20, -16, -16, -20, 0, -44}, {-44, -20, -27, -21, -21, -27, -20, -44}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -28, -5, -9, -9, -5, -28, 0}, {-9, -64, -29, -45, -45, -29, -64, -9}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -13, -16, -16, -13, 0, 0}, {-4, -8, -10, -17, -17, -10, -8, -4}};

const int Params::KNIGHT_PST[2][64] = {{-78, -27, -8, -9, -9, -8, -27, -78, -22, -28, -20, 2, 2, -20, -28, -22, -40, -18, 1, 20, 20, 1, -18, -40, -20, -4, 15, 15, 15, 15, -4, -20, -35, -18, 13, 9, 9, 13, -18, -35, -63, -47, -14, -5, -5, -14, -47, -63, -106, -84, -35, -29, -29, -35, -84, -106, -128, -128, -128, -128, -128, -128, -128, -128}, {-70, -48, -33, -31, -31, -33, -48, -70, -43, -29, -25, -19, -19, -25, -29, -43, -47, -30, -22, 1, 1, -22, -30, -47, -22, -15, 8, 15, 15, 8, -15, -22, -31, -22, -1, 3, 3, -1, -22, -31, -47, -32, -14, -6, -6, -14, -32, -47, -72, -46, -41, -30, -30, -41, -46, -72, -128, -68, -69, -54, -54, -69, -68, -128}};
const int Params::BISHOP_PST[2][64] = {{8, 40, 25, 21, 21, 25, 40, 8, 27, 36, 32, 24, 24, 32, 36, 27, 6, 32, 32, 25, 25, 32, 32, 6, -3, 4, 14, 31, 31, 14, 4, -3, -42, -18, -12, 12, 12, -12, -18, -42, -2, -39, -55, -36, -36, -55, -39, -2, -45, -73, -67, -105, -105, -67, -73, -45, -35, -76, -128, -128, -128, -128, -76, -35}, {-20, -2, -0, -4, -4, -0, -2, -20, -3, 1, -1, 6, 6, -1, 1, -3, -6, 4, 11, 14, 14, 11, 4, -6, -7, 5, 16, 21, 21, 16, 5, -7, -24, -11, -9, 1, 1, -9, -11, -24, -0, -16, -14, -14, -14, -14, -16, -0, -13, -13, -27, -29, -29, -27, -13, -13, -7, -6, -6, -28, -28, -6, -6, -7}};
const int Params::ROOK_PST[2][64] = {{-74, -60, -54, -51, -51, -54, -60, -74, -85, -60, -55, -52, -52, -55, -60, -85, -80, -58, -60, -61, -61, -60, -58, -80, -83, -66, -65, -56, -56, -65, -66, -83, -62, -50, -42, -34, -34, -42, -50, -62, -51, -23, -27, -15, -15, -27, -23, -51, -48, -55, -23, -18, -18, -23, -55, -48, -40, -37, -118, -84, -84, -118, -37, -40}, {-12, -11, -4, -9, -9, -4, -11, -12, -17, -13, -5, -5, -5, -5, -13, -17, -16, -8, -5, -5, -5, -5, -8, -16, -5, 6, 9, 4, 4, 9, 6, -5, 11, 12, 18, 14, 14, 18, 12, 11, 16, 23, 24, 23, 23, 24, 23, 16, 7, 11, 16, 16, 16, 16, 11, 7, 30, 32, 32, 26, 26, 32, 32, 30}};
const int Params::QUEEN_PST[2][64] = {{30, 34, 33, 43, 43, 33, 34, 30, 29, 41, 45, 48, 48, 45, 41, 29, 24, 38, 45, 36, 36, 45, 38, 24, 36, 43, 43, 44, 44, 43, 43, 36, 38, 40, 46, 51, 51, 46, 40, 38, 28, 36, 51, 49, 49, 51, 36, 28, 17, 6, 30, 25, 25, 30, 6, 17, 19, 28, 9, 6, 6, 9, 28, 19}, {-39, -48, -37, -25, -25, -37, -48, -39, -26, -28, -20, -3, -3, -20, -28, -26, -10, 12, 22, 17, 17, 22, 12, -10, 12, 31, 41, 47, 47, 41, 31, 12, 26, 56, 63, 80, 80, 63, 56, 26, 27, 56, 82, 89, 89, 82, 56, 27, 46, 57, 86, 98, 98, 86, 57, 46, 18, 31, 34, 38, 38, 34, 31, 18}};
const int Params::KING_PST[2][64] = {{33, 52, 5, 19, 19, 5, 52, 33, 73, 71, 38, 54, 54, 38, 71, 73, 8, 38, -22, -64, -64, -22, 38, 8, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -121, -111, -128, -128, -128, -128, -111, -121, -50, 128, -128, -128, -128, -128, 128, -50, -128, -128, 128, -128, -128, 128, -128, -128}, {-79, -76, -79, -83, -83, -79, -76, -79, -63, -69, -74, -72, -72, -74, -69, -63, -71, -72, -68, -66, -66, -68, -72, -71, -67, -61, -54, -52, -52, -54, -61, -67, -51, -43, -35, -34, -34, -35, -43, -51, -44, -33, -27, -26, -26, -27, -33, -44, -48, -38, -34, -34, -34, -34, -38, -48, -78, -54, -50, -53, -53, -50, -54, -78}};

const int Params::KNIGHT_MOBILITY[9] = {-65, -27, -9, 2, 10, 19, 21, 20, 14};
const int Params::BISHOP_MOBILITY[15] = {-43, -25, -12, -3, 7, 14, 19, 22, 27, 30, 30, 35, 40, 23, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-43, -28, -18, -14, -12, -6, -1, 8, 12, 17, 18, 21, 35, 36, 33}, {-43, -36, -17, -2, 12, 18, 20, 23, 30, 34, 38, 43, 43, 36, 29}};
const int Params::QUEEN_MOBILITY[2][24] = {{-43, -17, -11, -3, 3, 8, 14, 19, 23, 28, 29, 32, 33, 34, 35, 39, 42, 43, 43, 43, 43, 43, 43, 43}, {-43, -43, -43, -43, -26, -12, 9, 12, 24, 22, 29, 35, 35, 39, 42, 40, 43, 43, 43, 43, 43, 43, 37, 29}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-43, -15, 6, 17, 20};

const int Params::KNIGHT_OUTPOST[2][2] = {{29, 61}, {17, 29}};
const int Params::BISHOP_OUTPOST[2][2] = {{36, 61}, {33, 25}};


