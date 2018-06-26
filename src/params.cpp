// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 26-Jun-2018 09:22:43 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/epd/big5.epd
// Final objective value: 0.0759147

//

#include "params.h"

const int Params::RB_ADJUST[6] = {54, -6, 89, 39, 30, 60};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -13, -37, -32};
const int Params::QR_ADJUST[5] = {21, 72, 23, -35, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -293, -165};
const int Params::CASTLING[6] = {13, -38, -24, 0, 5, -18};
const int Params::KING_COVER[6][4] = {{11, 22, 14, 14},
{5, 10, 6, 6},
{-5, -10, -6, -6},
{-5, -10, -6, -6},
{-8, -16, -10, -10},
{-16, -32, -20, -21}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 86;
const int Params::PIN_MULTIPLIER_END = 82;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -184;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -149;
const int Params::MINOR_FOR_PAWNS = 39;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 40;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 6;
const int Params::PAWN_ATTACK_FACTOR2 = 7;
const int Params::MINOR_ATTACK_FACTOR = 39;
const int Params::MINOR_ATTACK_BOOST = 39;
const int Params::ROOK_ATTACK_FACTOR = 53;
const int Params::ROOK_ATTACK_BOOST = 19;
const int Params::ROOK_ATTACK_BOOST2 = 31;
const int Params::QUEEN_ATTACK_FACTOR = 57;
const int Params::QUEEN_ATTACK_BOOST = 42;
const int Params::QUEEN_ATTACK_BOOST2 = 53;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 10;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 163;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 82;
const int Params::OWN_ROOK_KING_PROXIMITY = 32;
const int Params::OWN_QUEEN_KING_PROXIMITY = 15;
const int Params::PAWN_THREAT_ON_PIECE_MID = 2;
const int Params::PAWN_THREAT_ON_PIECE_END = 12;
const int Params::PIECE_THREAT_MM_MID = 32;
const int Params::PIECE_THREAT_MR_MID = 87;
const int Params::PIECE_THREAT_MQ_MID = 64;
const int Params::PIECE_THREAT_MM_END = 31;
const int Params::PIECE_THREAT_MR_END = 74;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 9;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 20;
const int Params::PIECE_THREAT_RR_MID = 5;
const int Params::PIECE_THREAT_RQ_MID = 92;
const int Params::PIECE_THREAT_RM_END = 30;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 14;
const int Params::ROOK_PAWN_THREAT_END = 27;
const int Params::ENDGAME_KING_THREAT = 39;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 34;
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
const int Params::TWO_ROOKS_ON_7TH_MID = 18;
const int Params::TWO_ROOKS_ON_7TH_END = 68;
const int Params::ROOK_ON_OPEN_FILE_MID = 29;
const int Params::ROOK_ON_OPEN_FILE_END = 16;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 18;
const int Params::QUEEN_OUT = -29;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 11;
const int Params::KING_OPP_PAWN_DISTANCE = 5;
const int Params::QUEENING_SQUARE_CONTROL_MID = 59;
const int Params::QUEENING_SQUARE_CONTROL_END = 47;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -23;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -43;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 23, 52, 86, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={286, 209, 147};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 18, 19, 21, 23, 25, 27, 29, 31, 33, 36, 39, 41, 44, 48, 51, 54, 58, 62, 66, 71, 75, 80, 85, 90, 96, 101, 107, 114, 120, 127, 134, 141, 149, 157, 165, 173, 182, 191, 200, 209, 219, 228, 238, 248, 258, 269, 279, 290, 300, 311, 321, 332, 343, 353, 363, 374, 384, 394, 404, 414, 423, 433, 442, 451, 460, 468, 476, 484, 492, 499, 507, 514, 520, 527, 533, 539, 544, 550, 555, 560, 564, 569, 573, 577, 581, 584, 588, 591, 594, 597, 600, 602, 605, 607, 609, 611, 613, 615, 617, 618, 620, 621, 623, 624, 625, 626, 627, 628, 629, 630, 631, 632, 633, 633, 634, 635, 635, 636, 636, 637, 637, 637, 638, 638};
const int Params::TRADE_DOWN[8] = {45, 40, 34, 29, 23, 18, 12, 7};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 26, 101, 155}, {0, 0, 0, 0, 17, 66, 147, 224}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {84, 86, 84, 79, 79, 84, 86, 84};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 10, 96, 0}, {0, 0, 0, 0, 0, 16, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 3, 2, 14, 96, 221, 288}, {0, 0, 0, 0, 43, 90, 251, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 23, 6, 27, 91, 168}, {0, 0, 0, 7, 13, 28, 57, 55}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-44, -34, -41, -17, -43, -14, -27, -28, -7, -28, -29, -30, 0, -15, -33, -39, -8, -50, -30, -83, 0}, {0, 0, 0, -9, -16, -13, -0, -13, -14, -27, -27, -6, -5, -9, -17, -20, -17, -29, -67, -58, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-73, -67, -68, -54, -54, -53, -64, -59, -32, -38, -42, -51, -12, -3, -9, -29, 0, -1, -57, -32, -83}, {-12, -10, 0, -5, -8, -4, -22, -10, -13, -7, -9, -27, -11, -5, -2, -50, -21, -14, -65, -48, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-44, 0, -19, -16, -16, -19, 0, -44}, {-45, -20, -27, -21, -21, -27, -20, -45}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -27, -5, -10, -10, -5, -27, 0}, {0, -64, -30, -44, -44, -30, -64, 0}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -13, -16, -16, -13, 0, 0}, {-4, -8, -10, -17, -17, -10, -8, -4}};

const int Params::KNIGHT_PST[2][64] = {{-82, -29, -15, -10, -10, -15, -29, -82, -25, -31, -23, 0, 0, -23, -31, -25, -42, -21, -3, 19, 19, -3, -21, -42, -21, -7, 13, 14, 14, 13, -7, -21, -34, -15, 14, 11, 11, 14, -15, -34, -61, -45, -11, -4, -4, -11, -45, -61, -106, -79, -34, -29, -29, -34, -79, -106, -128, -128, -128, -128, -128, -128, -128, -128}, {-68, -46, -32, -30, -30, -32, -46, -68, -41, -28, -25, -18, -18, -25, -28, -41, -47, -30, -21, 2, 2, -21, -30, -47, -21, -15, 8, 16, 16, 8, -15, -21, -31, -22, -1, 2, 2, -1, -22, -31, -47, -32, -14, -6, -6, -14, -32, -47, -70, -46, -40, -30, -30, -40, -46, -70, -128, -68, -69, -54, -54, -69, -68, -128}};
const int Params::BISHOP_PST[2][64] = {{3, 36, 20, 19, 19, 20, 36, 3, 25, 32, 29, 20, 20, 29, 32, 25, 3, 29, 27, 22, 22, 27, 29, 3, -7, 1, 11, 27, 27, 11, 1, -7, -46, -19, -16, 9, 9, -16, -19, -46, -6, -40, -61, -39, -39, -61, -40, -6, -47, -72, -69, -110, -110, -69, -72, -47, -38, -76, -128, -128, -128, -128, -76, -38}, {-20, -2, -0, -4, -4, -0, -2, -20, -3, 2, -1, 6, 6, -1, 2, -3, -6, 4, 11, 13, 13, 11, 4, -6, -7, 4, 15, 21, 21, 15, 4, -7, -24, -11, -8, 1, 1, -8, -11, -24, 0, -16, -14, -14, -14, -14, -16, 0, -12, -13, -27, -28, -28, -27, -13, -12, -6, -6, -6, -28, -28, -6, -6, -6}};
const int Params::ROOK_PST[2][64] = {{-72, -58, -53, -49, -49, -53, -58, -72, -83, -57, -54, -49, -49, -54, -57, -83, -78, -56, -59, -58, -58, -59, -56, -78, -80, -63, -63, -54, -54, -63, -63, -80, -60, -47, -41, -31, -31, -41, -47, -60, -48, -22, -26, -14, -14, -26, -22, -48, -44, -50, -20, -14, -14, -20, -50, -44, -34, -31, -114, -78, -78, -114, -31, -34}, {-11, -10, -3, -9, -9, -3, -10, -11, -16, -13, -5, -5, -5, -5, -13, -16, -15, -8, -5, -5, -5, -5, -8, -15, -5, 6, 9, 4, 4, 9, 6, -5, 11, 12, 18, 14, 14, 18, 12, 11, 16, 23, 24, 23, 23, 24, 23, 16, 6, 10, 16, 16, 16, 16, 10, 6, 30, 31, 31, 26, 26, 31, 31, 30}};
const int Params::QUEEN_PST[2][64] = {{33, 35, 32, 43, 43, 32, 35, 33, 31, 43, 45, 48, 48, 45, 43, 31, 26, 39, 45, 35, 35, 45, 39, 26, 38, 43, 43, 43, 43, 43, 43, 38, 39, 43, 48, 52, 52, 48, 43, 39, 33, 45, 54, 56, 56, 54, 45, 33, 27, 15, 38, 35, 35, 38, 15, 27, 41, 47, 18, 18, 18, 18, 47, 41}, {-34, -43, -33, -20, -20, -33, -43, -34, -21, -23, -17, 1, 1, -17, -23, -21, -7, 15, 24, 19, 19, 24, 15, -7, 15, 34, 44, 49, 49, 44, 34, 15, 28, 58, 66, 82, 82, 66, 58, 28, 27, 58, 84, 90, 90, 84, 58, 27, 48, 59, 88, 100, 100, 88, 59, 48, 19, 32, 37, 41, 41, 37, 32, 19}};
const int Params::KING_PST[2][64] = {{39, 56, 5, 20, 20, 5, 56, 39, 76, 75, 30, 50, 50, 30, 75, 76, 17, 46, -25, -73, -73, -25, 46, 17, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -118, -92, -128, -128, -128, -128, -92, -118, -85, 128, -128, -128, -128, -128, 128, -85, -118, -128, 72, -128, -128, 72, -128, -118}, {-81, -78, -80, -84, -84, -80, -78, -81, -65, -70, -75, -73, -73, -75, -70, -65, -73, -74, -69, -67, -67, -69, -74, -73, -69, -63, -56, -54, -54, -56, -63, -69, -53, -44, -36, -36, -36, -36, -44, -53, -46, -34, -29, -28, -28, -29, -34, -46, -49, -40, -35, -36, -36, -35, -40, -49, -79, -56, -51, -54, -54, -51, -56, -79}};

const int Params::KNIGHT_MOBILITY[9] = {-66, -28, -10, 1, 9, 18, 21, 19, 13};
const int Params::BISHOP_MOBILITY[15] = {-43, -25, -12, -2, 7, 15, 19, 22, 27, 30, 30, 35, 40, 23, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-43, -28, -18, -15, -13, -7, -2, 7, 11, 15, 17, 20, 34, 37, 33}, {-43, -35, -16, -1, 12, 18, 20, 23, 30, 34, 38, 43, 43, 36, 30}};
const int Params::QUEEN_MOBILITY[2][24] = {{-43, -16, -10, -2, 4, 9, 15, 20, 24, 29, 30, 32, 34, 33, 36, 41, 41, 43, 43, 43, 43, 43, 43, 43}, {-43, -43, -43, -43, -28, -13, 8, 12, 24, 22, 27, 33, 33, 38, 41, 39, 43, 43, 43, 42, 43, 43, 37, 28}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-43, -16, 6, 16, 19};

const int Params::KNIGHT_OUTPOST[2][2] = {{27, 58}, {17, 29}};
const int Params::BISHOP_OUTPOST[2][2] = {{36, 61}, {33, 24}};


