// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 20-Nov-2018 12:37:37 by tuner -c 24 -R 50 -n 150 /home/jdart/chess/data/epd/big4.epd
// Final objective value: 0.070405

//

#include "params.h"

const int Params::RB_ADJUST[6] = {72, -6, 106, 59, 49, 79};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -15, -35, -32};
const int Params::QR_ADJUST[5] = {10, 59, 25, -34, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -291, -175};
const int Params::CASTLING[6] = {13, -38, -27, 0, 6, -24};
const int Params::KING_COVER[6][4] = {{11, 22, 14, 15},
{5, 10, 6, 7},
{-7, -14, -9, -10},
{-7, -14, -9, -10},
{-10, -21, -13, -14},
{-19, -38, -24, -26}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 93;
const int Params::PIN_MULTIPLIER_END = 92;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -190;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -128;
const int Params::MINOR_FOR_PAWNS = 62;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 35;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 6;
const int Params::PAWN_ATTACK_FACTOR2 = 6;
const int Params::MINOR_ATTACK_FACTOR = 40;
const int Params::MINOR_ATTACK_BOOST = 39;
const int Params::ROOK_ATTACK_FACTOR = 55;
const int Params::ROOK_ATTACK_BOOST = 19;
const int Params::ROOK_ATTACK_BOOST2 = 34;
const int Params::QUEEN_ATTACK_FACTOR = 56;
const int Params::QUEEN_ATTACK_BOOST = 42;
const int Params::QUEEN_ATTACK_BOOST2 = 56;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 10;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 140;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 83;
const int Params::OWN_ROOK_KING_PROXIMITY = 33;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 35;
const int Params::PIECE_THREAT_MR_MID = 92;
const int Params::PIECE_THREAT_MQ_MID = 81;
const int Params::PIECE_THREAT_MM_END = 33;
const int Params::PIECE_THREAT_MR_END = 80;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 11;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 25;
const int Params::PIECE_THREAT_RR_MID = 13;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 34;
const int Params::PIECE_THREAT_RR_END = 1;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 16;
const int Params::ROOK_PAWN_THREAT_END = 27;
const int Params::ENDGAME_KING_THREAT = 43;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 37;
const int Params::BISHOP_PAIR_END = 56;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -6;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -26;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = -1;
const int Params::WEAK_ON_OPEN_FILE_MID = -23;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 29;
const int Params::TWO_ROOKS_ON_7TH_MID = 26;
const int Params::TWO_ROOKS_ON_7TH_END = 58;
const int Params::ROOK_ON_OPEN_FILE_MID = 34;
const int Params::ROOK_ON_OPEN_FILE_END = 17;
const int Params::ROOK_BEHIND_PP_MID = 3;
const int Params::ROOK_BEHIND_PP_END = 20;
const int Params::QUEEN_OUT = -29;
const int Params::PAWN_SIDE_BONUS = 18;
const int Params::KING_OWN_PAWN_DISTANCE = 14;
const int Params::KING_OPP_PAWN_DISTANCE = 6;
const int Params::QUEENING_SQUARE_CONTROL_MID = 58;
const int Params::QUEENING_SQUARE_CONTROL_END = 61;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -17;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -36;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 25, 55, 86, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={251, 191, 135};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 8, 9, 10, 12, 13, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 36, 38, 41, 44, 47, 50, 53, 57, 61, 64, 68, 73, 77, 82, 87, 92, 97, 103, 109, 115, 121, 128, 135, 142, 149, 157, 165, 173, 181, 190, 199, 208, 217, 226, 236, 246, 256, 266, 276, 287, 297, 308, 318, 329, 339, 350, 361, 371, 382, 392, 402, 412, 422, 432, 442, 451, 461, 470, 479, 487, 496, 504, 512, 520, 527, 534, 541, 548, 554, 560, 566, 572, 577, 582, 587, 592, 597, 601, 605, 609, 613, 616, 620, 623, 626, 629, 632, 634, 637, 639, 641, 643, 645, 647, 649, 650, 652, 653, 655, 656, 657, 659, 660, 661, 662, 663, 664, 664, 665, 666, 667, 667, 668, 668, 669, 669, 670};
const int Params::TRADE_DOWN[8] = {45, 40, 34, 29, 23, 18, 12, 7};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 39, 128, 190}, {0, 0, 2, 0, 26, 86, 160, 266}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {73, 74, 71, 64, 64, 71, 74, 73};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 17, 96, 0}, {0, 0, 0, 0, 1, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 5, 6, 26, 96, 223, 288}, {0, 0, 0, 0, 40, 87, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 25, 10, 33, 98, 187}, {0, 0, 0, 11, 12, 25, 59, 50}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-36, -26, -40, -18, -31, -2, -22, -24, -9, -25, -0, -19, 0, -2, 0, -39, -10, -23, -23, -46, 0}, {0, 0, 0, -6, -12, 0, 0, -10, -9, -17, -17, -5, -6, -12, -9, -20, -19, -19, -54, -44, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-66, -73, -57, -50, -51, -52, -59, -56, -25, -30, -32, -45, -6, 0, -2, -34, 0, -2, -64, -33, -83}, {-11, -9, 0, -6, -3, 0, -18, -6, -9, -5, -6, -31, -14, -7, -0, -55, -24, -13, -58, -34, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-44, 0, -16, -13, -13, -16, 0, -44}, {-44, -19, -25, -19, -19, -25, -19, -44}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -30, 0, -22, -22, 0, -30, 0}, {-64, -64, -37, -34, -34, -37, -64, -64}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -18, -23, -23, -18, 0, 0}, {-6, -11, -13, -18, -18, -13, -11, -6}};

const int Params::KNIGHT_PST[2][64] = {{-86, -31, -12, -8, -8, -12, -31, -86, -25, -24, -17, 7, 7, -17, -24, -25, -44, -17, 2, 28, 28, 2, -17, -44, -17, 1, 21, 22, 22, 21, 1, -17, -28, -11, 21, 17, 17, 21, -11, -28, -54, -45, -2, 1, 1, -2, -45, -54, -108, -77, -27, -24, -24, -27, -77, -108, -128, -128, -128, -128, -128, -128, -128, -128}, {-80, -43, -28, -29, -29, -28, -43, -80, -36, -27, -22, -14, -14, -22, -27, -36, -48, -27, -17, 6, 6, -17, -27, -48, -20, -9, 13, 21, 21, 13, -9, -20, -29, -20, 1, 5, 5, 1, -20, -29, -49, -28, -11, -6, -6, -11, -28, -49, -70, -46, -38, -28, -28, -38, -46, -70, -128, -66, -71, -48, -48, -71, -66, -128}};
const int Params::BISHOP_PST[2][64] = {{6, 38, 23, 22, 22, 23, 38, 6, 30, 38, 32, 25, 25, 32, 38, 30, 6, 35, 32, 26, 26, 32, 35, 6, -5, 5, 14, 35, 35, 14, 5, -5, -40, -15, -8, 21, 21, -8, -15, -40, 4, -31, -40, -31, -31, -40, -31, 4, -43, -64, -63, -100, -100, -63, -64, -43, -16, -79, -128, -128, -128, -128, -79, -16}, {-17, -1, 4, -0, -0, 4, -1, -17, -3, 6, 2, 10, 10, 2, 6, -3, -2, 10, 16, 19, 19, 16, 10, -2, -2, 8, 21, 25, 25, 21, 8, -2, -21, -6, -5, 5, 5, -5, -6, -21, 3, -11, -13, -12, -12, -13, -11, 3, -7, -12, -23, -25, -25, -23, -12, -7, -1, -1, -6, -27, -27, -6, -1, -1}};
const int Params::ROOK_PST[2][64] = {{-65, -51, -46, -42, -42, -46, -51, -65, -77, -49, -47, -41, -41, -47, -49, -77, -75, -49, -52, -50, -50, -52, -49, -75, -74, -58, -57, -46, -46, -57, -58, -74, -52, -43, -33, -26, -26, -33, -43, -52, -38, -14, -21, -6, -6, -21, -14, -38, -38, -46, -15, -12, -12, -15, -46, -38, -25, -21, -80, -46, -46, -80, -21, -25}, {-4, -5, 3, -3, -3, 3, -5, -4, -14, -10, -1, 0, 0, -1, -10, -14, -12, -6, 2, -1, -1, 2, -6, -12, 1, 10, 16, 11, 11, 16, 10, 1, 18, 21, 26, 21, 21, 26, 21, 18, 27, 31, 36, 34, 34, 36, 31, 27, 13, 18, 23, 25, 25, 23, 18, 13, 34, 37, 34, 31, 31, 34, 37, 34}};
const int Params::QUEEN_PST[2][64] = {{50, 50, 46, 56, 56, 46, 50, 50, 44, 58, 62, 63, 63, 62, 58, 44, 40, 56, 60, 50, 50, 60, 56, 40, 53, 59, 61, 60, 60, 61, 59, 53, 56, 59, 63, 73, 73, 63, 59, 56, 51, 63, 70, 70, 70, 70, 63, 51, 42, 32, 52, 53, 53, 52, 32, 42, 59, 58, 50, 41, 41, 50, 58, 59}, {-29, -48, -26, -10, -10, -26, -48, -29, -12, -13, -4, 14, 14, -4, -13, -12, 4, 31, 41, 32, 32, 41, 31, 4, 34, 54, 60, 64, 64, 60, 54, 34, 48, 78, 82, 97, 97, 82, 78, 48, 46, 79, 95, 111, 111, 95, 79, 46, 59, 78, 105, 115, 115, 105, 78, 59, 42, 58, 57, 54, 54, 57, 58, 42}};
const int Params::KING_PST[2][64] = {{39, 56, -1, 15, 15, -1, 56, 39, 79, 76, 30, 45, 45, 30, 76, 79, 5, 31, -35, -98, -98, -35, 31, 5, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -118, -128, -128, -128, -128, -128, -128, -118, 13, 95, -126, -128, -128, -126, 95, 13, -128, -128, -74, -128, -128, -74, -128, -128}, {-76, -76, -79, -84, -84, -79, -76, -76, -61, -69, -76, -74, -74, -76, -69, -61, -73, -75, -71, -69, -69, -71, -75, -73, -68, -63, -56, -55, -55, -56, -63, -68, -52, -40, -34, -33, -33, -34, -40, -52, -39, -25, -22, -24, -24, -22, -25, -39, -50, -31, -27, -28, -28, -27, -31, -50, -89, -53, -45, -43, -43, -45, -53, -89}};

const int Params::KNIGHT_MOBILITY[9] = {-66, -27, -5, 6, 15, 25, 28, 27, 20};
const int Params::BISHOP_MOBILITY[15] = {-43, -20, -6, 5, 15, 22, 26, 30, 35, 39, 39, 43, 43, 37, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-43, -27, -15, -11, -10, -4, 1, 9, 14, 17, 20, 24, 36, 40, 35}, {-43, -42, -23, -5, 10, 18, 17, 22, 29, 33, 38, 43, 43, 36, 33}};
const int Params::QUEEN_MOBILITY[2][24] = {{-43, -30, -16, -6, 2, 8, 15, 21, 26, 31, 33, 35, 37, 38, 41, 43, 42, 43, 43, 43, 43, 43, 43, 43}, {-43, -43, -43, -43, -40, -26, -5, 3, 11, 17, 21, 28, 33, 38, 41, 42, 43, 43, 43, 43, 43, 43, 42, 43}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-43, -14, 7, 20, 25};

const int Params::KNIGHT_OUTPOST[2][2] = {{27, 63}, {20, 32}};
const int Params::BISHOP_OUTPOST[2][2] = {{33, 60}, {35, 26}};


