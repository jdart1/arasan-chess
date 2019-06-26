// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 25-Jun-2019 01:16:19 by tuner -n 175 -c 24 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0679692

//

#include "params.h"

const int Params::RB_ADJUST[6] = {46, -6, 92, 43, 39, 73};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -28, -57, -63};
const int Params::QR_ADJUST[5] = {18, 74, 28, -40, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -251, -131};
const int Params::MINOR_FOR_PAWNS[6] = {51, 59, 72, 93, 114, 129};
const int Params::CASTLING[6] = {13, -38, -27, 0, 5, -21};
const int Params::KING_COVER[6][4] = {{8, 23, 13, 15},
{3, 8, 4, 5},
{-7, -22, -12, -14},
{-7, -21, -12, -14},
{-9, -26, -15, -18},
{-15, -45, -26, -30}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R_NO_PAWNS = -199;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -149;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 40;
const int Params::PAWN_ATTACK_FACTOR = 15;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 42;
const int Params::ROOK_ATTACK_FACTOR = 60;
const int Params::ROOK_ATTACK_BOOST = 23;
const int Params::ROOK_ATTACK_BOOST2 = 37;
const int Params::QUEEN_ATTACK_FACTOR = 60;
const int Params::QUEEN_ATTACK_BOOST = 46;
const int Params::QUEEN_ATTACK_BOOST2 = 61;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 6;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 133;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 87;
const int Params::OWN_ROOK_KING_PROXIMITY = 36;
const int Params::OWN_QUEEN_KING_PROXIMITY = 17;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 37;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 79;
const int Params::PIECE_THREAT_MM_END = 31;
const int Params::PIECE_THREAT_MR_END = 77;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 11;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 24;
const int Params::PIECE_THREAT_RR_MID = 11;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 33;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 16;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 40;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 59;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -23;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -4;
const int Params::WEAK_PAWN_END = -1;
const int Params::WEAK_ON_OPEN_FILE_MID = -21;
const int Params::WEAK_ON_OPEN_FILE_END = -15;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 3;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 27;
const int Params::TWO_ROOKS_ON_7TH_MID = 40;
const int Params::TWO_ROOKS_ON_7TH_END = 69;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 25;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 58;
const int Params::QUEENING_SQUARE_CONTROL_END = 60;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -15;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -42;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 26, 57, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={228, 192, 151};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 25, 27, 29, 31, 33, 35, 37, 40, 42, 45, 48, 51, 54, 57, 61, 64, 68, 72, 76, 81, 85, 90, 95, 100, 105, 111, 116, 122, 128, 135, 141, 148, 155, 162, 170, 178, 185, 193, 202, 210, 219, 227, 236, 245, 254, 264, 273, 283, 292, 302, 311, 321, 331, 340, 350, 360, 369, 379, 388, 397, 407, 416, 424, 433, 442, 450, 459, 467, 474, 482, 490, 497, 504, 511, 517, 524, 530, 536, 542, 547, 552, 558, 563, 567, 572, 576, 580, 584, 588, 592, 595, 599, 602, 605, 608, 610, 613, 615, 618, 620, 622, 624, 626, 628, 630, 631, 633, 634, 636, 637, 638, 639, 641, 642, 643, 644, 644, 645, 646, 647, 648};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 1, 39, 128, 194}, {0, 0, 1, 0, 27, 85, 160, 264}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 0, 19, 96, 0}, {0, 0, 0, 0, 1, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 7, 22, 102, 184, 320}, {0, 0, 0, 0, 36, 89, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 28, 13, 38, 99, 181}, {0, 0, 0, 10, 11, 27, 60, 76}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-30, -20, -25, -3, -31, -10, -15, -21, 0, -23, -43, -15, 0, -15, -4, -36, -8, -18, -19, -67, 0}, {0, 0, 0, -2, -8, -4, 0, -9, -5, -17, -22, -7, -3, -11, -16, -18, -16, -26, -51, -38, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-58, -70, -47, -43, -41, -47, -55, -48, -24, -29, -29, -42, -6, 0, -2, -32, 0, -1, -62, -35, -83}, {-10, -5, 0, -7, -4, -1, -15, -8, -6, -4, -4, -28, -13, -9, -0, -52, -23, -14, -52, -36, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-39, 0, -14, -9, -9, -14, 0, -39}, {-40, -17, -23, -18, -18, -23, -17, -40}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -20, 0, -1, -1, 0, -20, 0}, {-75, -69, -36, -36, -36, -36, -69, -75}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -0, -19, -24, -24, -19, -0, 0}, {-6, -10, -12, -18, -18, -12, -10, -6}};

const int Params::KNIGHT_PST[2][64] = {{-85, -28, -13, -7, -7, -13, -28, -85, -27, -25, -17, 5, 5, -17, -25, -27, -45, -19, 1, 26, 26, 1, -19, -45, -18, -1, 20, 21, 21, 20, -1, -18, -30, -12, 20, 15, 15, 20, -12, -30, -55, -48, -6, 2, 2, -6, -48, -55, -105, -81, -31, -29, -29, -31, -81, -105, -128, -128, -128, -128, -128, -128, -128, -128}, {-80, -43, -31, -32, -32, -31, -43, -80, -36, -25, -25, -17, -17, -25, -25, -36, -46, -29, -20, 3, 3, -20, -29, -46, -23, -11, 7, 17, 17, 7, -11, -23, -34, -24, -4, 0, 0, -4, -24, -34, -54, -35, -18, -13, -13, -18, -35, -54, -73, -49, -46, -34, -34, -46, -49, -73, -128, -74, -74, -62, -62, -74, -74, -128}};
const int Params::BISHOP_PST[2][64] = {{10, 39, 26, 25, 25, 26, 39, 10, 31, 43, 33, 27, 27, 33, 43, 31, 8, 36, 35, 28, 28, 35, 36, 8, -3, 5, 16, 36, 36, 16, 5, -3, -41, -15, -9, 18, 18, -9, -15, -41, -1, -32, -46, -33, -33, -46, -32, -1, -36, -67, -67, -104, -104, -67, -67, -36, -29, -101, -128, -128, -128, -128, -101, -29}, {-15, -3, 2, -3, -3, 2, -3, -15, 5, 6, 2, 9, 9, 2, 6, 5, -2, 10, 15, 18, 18, 15, 10, -2, -3, 6, 19, 24, 24, 19, 6, -3, -22, -8, -8, 2, 2, -8, -8, -22, -2, -15, -15, -15, -15, -15, -15, -2, -10, -15, -23, -26, -26, -23, -15, -10, -5, -6, -11, -32, -32, -11, -6, -5}};
const int Params::ROOK_PST[2][64] = {{-69, -56, -50, -47, -47, -50, -56, -69, -82, -56, -52, -46, -46, -52, -56, -82, -80, -57, -56, -54, -54, -56, -57, -80, -78, -64, -61, -52, -52, -61, -64, -78, -57, -50, -37, -34, -34, -37, -50, -57, -44, -19, -27, -15, -15, -27, -19, -44, -42, -46, -22, -21, -21, -22, -46, -42, -35, -33, -128, -102, -102, -128, -33, -35}, {-29, -30, -21, -27, -27, -21, -30, -29, -40, -34, -26, -27, -27, -26, -34, -40, -36, -31, -24, -27, -27, -24, -31, -36, -24, -17, -11, -16, -16, -11, -17, -24, -9, -7, -1, -5, -5, -1, -7, -9, -3, 1, 6, 4, 4, 6, 1, -3, -14, -11, -5, -4, -4, -5, -11, -14, 8, 6, 6, 2, 2, 6, 6, 8}};
const int Params::QUEEN_PST[2][64] = {{-5, -5, -9, 3, 3, -9, -5, -5, -12, 5, 7, 8, 8, 7, 5, -12, -13, 0, 6, -5, -5, 6, 0, -13, -1, 4, 5, 5, 5, 5, 4, -1, 2, 4, 8, 15, 15, 8, 4, 2, -5, 16, 19, 14, 14, 19, 16, -5, -5, -21, 1, 1, 1, 1, -21, -5, 4, 7, -20, -31, -31, -20, 7, 4}, {-103, -114, -99, -84, -84, -99, -114, -103, -96, -87, -79, -62, -62, -79, -87, -96, -79, -52, -40, -50, -50, -40, -52, -79, -51, -30, -20, -18, -18, -20, -30, -51, -42, -9, 0, 14, 14, 0, -9, -42, -45, -14, 10, 24, 24, 10, -14, -45, -30, -8, 16, 26, 26, 16, -8, -30, -69, -53, -53, -45, -45, -53, -53, -69}};
const int Params::KING_PST[2][64] = {{40, 56, -4, 17, 17, -4, 56, 40, 80, 82, 28, 51, 51, 28, 82, 80, -4, 22, -52, -116, -116, -52, 22, -4, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -107, -78, -128, -128, -128, -128, -78, -107, 128, -20, -128, -128, -128, -128, -20, 128, -109, -128, 128, 51, 51, 128, -128, -109}, {-28, -26, -30, -35, -35, -30, -26, -28, -13, -19, -25, -25, -25, -25, -19, -13, -23, -25, -21, -20, -20, -21, -25, -23, -21, -15, -10, -7, -7, -10, -15, -21, -6, 1, 10, 11, 11, 10, 1, -6, 1, 14, 16, 17, 17, 16, 14, 1, -9, 5, 8, 7, 7, 8, 5, -9, -38, -13, -10, -10, -10, -10, -13, -38}};
const int Params::KNIGHT_MOBILITY[9] = {-78, -33, -11, 0, 9, 18, 21, 20, 12};
const int Params::BISHOP_MOBILITY[15] = {-59, -32, -17, -6, 4, 12, 16, 19, 24, 27, 28, 30, 39, 23, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-76, -52, -40, -35, -34, -28, -23, -15, -12, -8, -7, -3, 8, 15, 7}, {-96, -66, -47, -29, -13, -7, -7, -3, 3, 7, 11, 16, 18, 7, 0}};
const int Params::QUEEN_MOBILITY[2][24] = {{-96, -39, -26, -15, -8, -2, 6, 11, 16, 21, 23, 24, 27, 28, 32, 35, 34, 50, 60, 70, 95, 90, 96, 96}, {-96, -96, -96, -96, -87, -68, -53, -48, -38, -35, -33, -26, -24, -20, -17, -21, -18, -21, -27, -28, -25, -28, -35, -42}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-81, -44, -25, -14, -11};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 62}, {20, 31}};
const int Params::BISHOP_OUTPOST[2][2] = {{36, 64}, {35, 26}};
const int Params::PAWN_STORM[4][2] = {{0, 23},{10, 1},{33, 1},{4, 0}};

