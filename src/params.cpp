// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 24-Aug-2019 08:32:59 by tuner -n 175 -c 16 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0678441

//

#include "params.h"

const int Params::RB_ADJUST[6] = {46, -6, 92, 43, 39, 72};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -28, -57, -63};
const int Params::QR_ADJUST[5] = {18, 75, 28, -38, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -251, -131};
const int Params::MINOR_FOR_PAWNS[6] = {51, 59, 72, 92, 113, 129};
const int Params::CASTLING[6] = {13, -38, -28, 0, 4, -22};
const int Params::KING_COVER[6][4] = {{8, 23, 14, 16},
{2, 7, 4, 5},
{-8, -24, -14, -16},
{-8, -24, -14, -16},
{-10, -30, -17, -20},
{-17, -51, -30, -34}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 84;
const int Params::KRMINOR_VS_R_NO_PAWNS = -199;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -148;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 41;
const int Params::PAWN_ATTACK_FACTOR = 13;
const int Params::MINOR_ATTACK_FACTOR = 47;
const int Params::MINOR_ATTACK_BOOST = 25;
const int Params::ROOK_ATTACK_FACTOR = 58;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 55;
const int Params::QUEEN_ATTACK_BOOST = 44;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 115;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 4;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 96;
const int Params::OWN_ROOK_KING_PROXIMITY = 33;
const int Params::OWN_QUEEN_KING_PROXIMITY = 20;
const int Params::PAWN_THREAT_ON_PIECE_MID = 4;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 36;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 80;
const int Params::PIECE_THREAT_MM_END = 31;
const int Params::PIECE_THREAT_MR_END = 77;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 11;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 24;
const int Params::PIECE_THREAT_RR_MID = 10;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 34;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
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
const int Params::TWO_ROOKS_ON_7TH_MID = 44;
const int Params::TWO_ROOKS_ON_7TH_END = 69;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 4;
const int Params::ROOK_BEHIND_PP_END = 25;
const int Params::QUEEN_OUT = -31;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 60;
const int Params::QUEENING_SQUARE_CONTROL_END = 60;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -16;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -42;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 26, 57, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={229, 192, 151};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35, 37, 40, 43, 45, 48, 51, 54, 58, 61, 65, 69, 73, 77, 82, 86, 91, 96, 101, 107, 113, 118, 125, 131, 137, 144, 151, 158, 166, 173, 181, 189, 197, 206, 214, 223, 232, 241, 250, 259, 269, 278, 288, 297, 307, 317, 326, 336, 345, 355, 365, 374, 383, 392, 402, 411, 419, 428, 437, 445, 453, 461, 469, 476, 483, 491, 497, 504, 511, 517, 523, 529, 534, 539, 545, 550, 554, 559, 563, 567, 571, 575, 579, 582, 586, 589, 592, 595, 597, 600, 602, 605, 607, 609, 611, 613, 615, 616, 618, 619, 621, 622, 624, 625, 626, 627, 628, 629, 630, 631, 632};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 1, 39, 128, 194}, {0, 0, 2, 0, 27, 85, 160, 264}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 0, 20, 96, 0}, {0, 0, 0, 0, 1, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 5, 20, 102, 183, 320}, {0, 0, 0, 0, 36, 89, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 29, 13, 39, 100, 184}, {0, 0, 0, 10, 11, 27, 60, 76}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-30, -22, -27, -6, -31, -9, -14, -24, 0, -24, -43, -16, 0, -23, 0, -38, -10, -4, -18, -68, 0}, {0, 0, 0, -2, -9, -4, 0, -9, -5, -16, -22, -7, -3, -11, -15, -18, -16, -26, -51, -38, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-58, -68, -48, -43, -40, -47, -54, -47, -25, -27, -27, -42, -6, 0, -1, -32, 0, -1, -62, -35, -83}, {-11, -5, 0, -8, -4, -1, -15, -8, -6, -4, -4, -28, -14, -9, -0, -52, -23, -14, -52, -36, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-39, 0, -14, -9, -9, -14, 0, -39}, {-40, -17, -23, -18, -18, -23, -17, -40}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -22, 0, 0, 0, 0, -22, 0}, {-74, -69, -36, -35, -35, -36, -69, -74}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -0, -18, -24, -24, -18, -0, 0}, {-6, -10, -12, -18, -18, -12, -10, -6}};

const int Params::KNIGHT_PST[2][64] = {{-83, -27, -11, -6, -6, -11, -27, -83, -25, -25, -15, 6, 6, -15, -25, -25, -44, -17, 2, 26, 26, 2, -17, -44, -17, 3, 21, 22, 22, 21, 3, -17, -28, -11, 21, 14, 14, 21, -11, -28, -60, -47, -7, 0, 0, -7, -47, -60, -105, -83, -34, -29, -29, -34, -83, -105, -128, -128, -128, -128, -128, -128, -128, -128}, {-82, -44, -32, -34, -34, -32, -44, -82, -38, -27, -26, -18, -18, -26, -27, -38, -47, -30, -21, 2, 2, -21, -30, -47, -24, -12, 7, 16, 16, 7, -12, -24, -34, -25, -4, 0, 0, -4, -25, -34, -53, -35, -18, -13, -13, -18, -35, -53, -72, -49, -46, -34, -34, -46, -49, -72, -128, -74, -75, -62, -62, -75, -74, -128}};
const int Params::BISHOP_PST[2][64] = {{13, 41, 28, 26, 26, 28, 41, 13, 33, 44, 35, 28, 28, 35, 44, 33, 9, 37, 36, 28, 28, 36, 37, 9, -1, 6, 17, 36, 36, 17, 6, -1, -37, -16, -9, 18, 18, -9, -16, -37, 2, -30, -47, -34, -34, -47, -30, 2, -35, -72, -67, -101, -101, -67, -72, -35, -27, -103, -128, -128, -128, -128, -103, -27}, {-15, -2, 3, -3, -3, 3, -2, -15, 5, 6, 2, 8, 8, 2, 6, 5, -2, 10, 15, 18, 18, 15, 10, -2, -3, 6, 19, 24, 24, 19, 6, -3, -22, -7, -7, 2, 2, -7, -7, -22, -1, -15, -14, -15, -15, -14, -15, -1, -9, -15, -23, -25, -25, -23, -15, -9, -3, -6, -10, -31, -31, -10, -6, -3}};
const int Params::ROOK_PST[2][64] = {{-70, -57, -51, -47, -47, -51, -57, -70, -83, -57, -52, -47, -47, -52, -57, -83, -82, -57, -57, -55, -55, -57, -57, -82, -79, -65, -62, -53, -53, -62, -65, -79, -59, -50, -37, -35, -35, -37, -50, -59, -45, -20, -29, -15, -15, -29, -20, -45, -41, -48, -22, -20, -20, -22, -48, -41, -36, -29, -128, -99, -99, -128, -29, -36}, {-29, -30, -21, -27, -27, -21, -30, -29, -40, -34, -26, -26, -26, -26, -34, -40, -36, -31, -24, -27, -27, -24, -31, -36, -24, -17, -11, -16, -16, -11, -17, -24, -9, -7, -1, -5, -5, -1, -7, -9, -3, 1, 6, 4, 4, 6, 1, -3, -14, -11, -5, -4, -4, -5, -11, -14, 8, 6, 6, 2, 2, 6, 6, 8}};
const int Params::QUEEN_PST[2][64] = {{-7, -6, -9, 5, 5, -9, -6, -7, -12, 5, 7, 9, 9, 7, 5, -12, -12, 1, 7, -3, -3, 7, 1, -12, -0, 5, 6, 6, 6, 6, 5, -0, 3, 5, 10, 16, 16, 10, 5, 3, -8, 11, 20, 10, 10, 20, 11, -8, -3, -21, -1, 0, 0, -1, -21, -3, 7, 9, -20, -49, -49, -20, 9, 7}, {-103, -112, -95, -82, -82, -95, -112, -103, -95, -85, -76, -60, -60, -76, -85, -95, -79, -48, -37, -47, -47, -37, -48, -79, -51, -28, -18, -16, -16, -18, -28, -51, -40, -8, 3, 16, 16, 3, -8, -40, -42, -10, 12, 27, 27, 12, -10, -42, -28, -7, 19, 28, 28, 19, -7, -28, -63, -51, -48, -40, -40, -48, -51, -63}};
const int Params::KING_PST[2][64] = {{19, 35, -23, -4, -4, -23, 35, 19, 60, 61, 11, 33, 33, 11, 61, 60, -24, -0, -64, -128, -128, -64, -0, -24, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -125, -51, -128, -128, -128, -128, -51, -125, 71, -27, -128, -128, -128, -128, -27, 71, -112, -128, 128, -128, -128, 128, -128, -112}, {-29, -26, -31, -36, -36, -31, -26, -29, -14, -20, -26, -26, -26, -26, -20, -14, -24, -26, -22, -20, -20, -22, -26, -24, -22, -16, -11, -7, -7, -11, -16, -22, -7, 0, 9, 10, 10, 9, 0, -7, 1, 13, 16, 16, 16, 16, 13, 1, -10, 4, 7, 6, 6, 7, 4, -10, -38, -15, -12, -11, -11, -12, -15, -38}};
const int Params::KNIGHT_MOBILITY[9] = {-77, -32, -10, 2, 10, 19, 22, 21, 13};
const int Params::BISHOP_MOBILITY[15] = {-59, -32, -17, -7, 4, 12, 16, 19, 24, 27, 27, 30, 39, 24, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-73, -49, -37, -32, -31, -25, -20, -12, -9, -5, -4, 0, 11, 20, 10}, {-96, -67, -46, -29, -13, -7, -7, -3, 3, 7, 11, 16, 18, 7, 0}};
const int Params::QUEEN_MOBILITY[2][24] = {{-96, -39, -24, -14, -7, -0, 7, 12, 18, 22, 24, 26, 29, 30, 35, 37, 35, 52, 60, 74, 88, 96, 96, 96}, {-96, -96, -96, -96, -88, -69, -55, -49, -41, -38, -34, -28, -25, -23, -20, -23, -20, -24, -30, -30, -28, -30, -38, -44}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-87, -51, -32, -21, -18};
const int Params::KNIGHT_OUTPOST[2][2] = {{28, 63}, {19, 30}};
const int Params::BISHOP_OUTPOST[2][2] = {{36, 65}, {35, 25}};
const int Params::PAWN_STORM[4][2] = {{0, 21},{11, 2},{33, 0},{3, 0}};

