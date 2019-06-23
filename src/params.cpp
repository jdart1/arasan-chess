// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 22-Jun-2019 08:34:00 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0681557

//

#include "params.h"

const int Params::RB_ADJUST[6] = {43, -6, 87, 39, 28, 61};
const int Params::RBN_ADJUST[6] = {64, -19, 26, 4, -13, -32};
const int Params::QR_ADJUST[5] = {12, 82, 49, -7, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -278, -158};
const int Params::CASTLING[6] = {13, -38, -27, 0, 5, -20};
const int Params::KING_COVER[6][4] = {{5, 16, 9, 11},
{1, 2, 1, 1},
{-9, -27, -15, -18},
{-9, -28, -16, -19},
{-10, -29, -16, -20},
{-17, -51, -29, -34}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 93;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R_NO_PAWNS = -230;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -183;
const int Params::MINOR_FOR_PAWNS = 0;
const int Params::ENDGAME_PAWN_ADVANTAGE = 30;
const int Params::PAWN_ENDGAME1 = 41;
const int Params::PAWN_ATTACK_FACTOR = 15;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 41;
const int Params::ROOK_ATTACK_FACTOR = 60;
const int Params::ROOK_ATTACK_BOOST = 23;
const int Params::ROOK_ATTACK_BOOST2 = 38;
const int Params::QUEEN_ATTACK_FACTOR = 59;
const int Params::QUEEN_ATTACK_BOOST = 45;
const int Params::QUEEN_ATTACK_BOOST2 = 60;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 5;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 128;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 87;
const int Params::OWN_ROOK_KING_PROXIMITY = 34;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 32;
const int Params::PIECE_THREAT_MR_MID = 91;
const int Params::PIECE_THREAT_MQ_MID = 79;
const int Params::PIECE_THREAT_MM_END = 31;
const int Params::PIECE_THREAT_MR_END = 77;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 10;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 26;
const int Params::PIECE_THREAT_RR_MID = 13;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 34;
const int Params::PIECE_THREAT_RR_END = 1;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 41;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 40;
const int Params::BISHOP_PAIR_END = 57;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -24;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = -0;
const int Params::WEAK_ON_OPEN_FILE_MID = -22;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 26;
const int Params::TWO_ROOKS_ON_7TH_MID = 35;
const int Params::TWO_ROOKS_ON_7TH_END = 65;
const int Params::ROOK_ON_OPEN_FILE_MID = 34;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 3;
const int Params::ROOK_BEHIND_PP_END = 23;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 14;
const int Params::KING_OPP_PAWN_DISTANCE = 6;
const int Params::QUEENING_SQUARE_CONTROL_MID = 63;
const int Params::QUEENING_SQUARE_CONTROL_END = 58;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -16;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -42;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 26, 58, 86, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={253, 188, 151};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, 0, 0, 0, 0, 0, 1, 1, 1, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 25, 27, 29, 31, 33, 36, 38, 41, 43, 46, 49, 52, 56, 59, 63, 67, 71, 75, 79, 84, 88, 93, 98, 104, 109, 115, 121, 128, 134, 141, 148, 155, 162, 170, 177, 185, 194, 202, 210, 219, 228, 237, 246, 255, 265, 274, 284, 293, 303, 312, 322, 332, 341, 351, 360, 370, 379, 388, 397, 406, 415, 424, 432, 440, 449, 456, 464, 472, 479, 486, 493, 500, 506, 512, 518, 524, 529, 535, 540, 545, 549, 554, 558, 562, 566, 570, 573, 577, 580, 583, 586, 589, 592, 594, 597, 599, 601, 603, 605, 607, 609, 610, 612, 613, 615, 616, 617, 619, 620, 621, 622, 623, 624, 624, 625, 626, 627, 627};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 34, 123, 188}, {0, 0, 2, 0, 27, 85, 160, 263}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {71, 73, 71, 64, 64, 71, 73, 71};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 16, 96, 0}, {0, 0, 0, 0, 1, 18, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 12, 21, 102, 211, 320}, {0, 0, 0, 0, 37, 90, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 30, 12, 36, 96, 192}, {0, 0, 0, 10, 11, 26, 60, 78}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-36, -26, -33, -9, -33, 0, -22, -27, -5, -23, -39, -18, 0, -15, -9, -35, -14, -29, -11, -57, 0}, {0, 0, 0, -1, -10, -4, 0, -9, -5, -16, -23, -7, -3, -11, -14, -20, -18, -28, -53, -40, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-61, -74, -52, -50, -44, -53, -60, -53, -28, -32, -34, -43, -9, 0, -5, -31, -1, 0, -61, -33, -83}, {-11, -5, 0, -9, -5, -4, -15, -8, -7, -5, -5, -28, -14, -10, -0, -52, -24, -15, -52, -38, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-43, 0, -17, -12, -12, -17, 0, -43}, {-40, -18, -23, -19, -19, -23, -18, -40}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -25, -4, -10, -10, -4, -25, 0}, {-84, -68, -37, -38, -38, -37, -68, -84}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -16, -22, -22, -16, 0, 0}, {-6, -10, -12, -18, -18, -12, -10, -6}};

const int Params::KNIGHT_PST[2][64] = {{-65, -7, 8, 13, 13, 8, -7, -65, -4, -5, 4, 26, 26, 4, -5, -4, -24, 2, 21, 46, 46, 21, 2, -24, 2, 20, 40, 41, 41, 40, 20, 2, -10, 8, 40, 34, 34, 40, 8, -10, -35, -29, 15, 20, 20, 15, -29, -35, -86, -63, -13, -8, -8, -13, -63, -86, -128, -128, -128, -128, -128, -128, -128, -128}, {-71, -32, -20, -22, -22, -20, -32, -71, -27, -16, -15, -6, -6, -15, -16, -27, -36, -18, -8, 14, 14, -8, -18, -36, -12, -1, 18, 28, 28, 18, -1, -12, -25, -15, 6, 10, 10, 6, -15, -25, -45, -26, -8, -2, -2, -8, -26, -45, -63, -41, -37, -26, -26, -37, -41, -63, -128, -68, -66, -54, -54, -66, -68, -128}};
const int Params::BISHOP_PST[2][64] = {{26, 55, 44, 42, 42, 44, 55, 26, 48, 59, 51, 44, 44, 51, 59, 48, 26, 53, 52, 45, 45, 52, 53, 26, 13, 23, 34, 53, 53, 34, 23, 13, -23, 2, 8, 35, 35, 8, 2, -23, 16, -13, -26, -15, -15, -26, -13, 16, -17, -49, -50, -84, -84, -50, -49, -17, -14, -70, -128, -128, -128, -128, -70, -14}, {-6, 7, 12, 7, 7, 12, 7, -6, 13, 16, 12, 18, 18, 12, 16, 13, 7, 19, 24, 28, 28, 24, 19, 7, 6, 16, 29, 33, 33, 29, 16, 6, -14, 1, 1, 11, 11, 1, 1, -14, 7, -6, -6, -6, -6, -6, -6, 7, -1, -7, -15, -17, -17, -15, -7, -1, 2, -0, -3, -23, -23, -3, -0, 2}};
const int Params::ROOK_PST[2][64] = {{-52, -39, -33, -29, -29, -33, -39, -52, -64, -39, -35, -29, -29, -35, -39, -64, -62, -39, -39, -37, -37, -39, -39, -62, -60, -46, -43, -34, -34, -43, -46, -60, -41, -33, -20, -17, -17, -20, -33, -41, -26, -3, -11, 1, 1, -11, -3, -26, -25, -30, -6, -3, -3, -6, -30, -25, -18, -12, -87, -62, -62, -87, -12, -18}, {-14, -15, -6, -12, -12, -6, -15, -14, -25, -19, -11, -12, -12, -11, -19, -25, -21, -16, -9, -12, -12, -9, -16, -21, -9, -2, 4, -1, -1, 4, -2, -9, 6, 8, 13, 10, 10, 13, 8, 6, 12, 16, 20, 18, 18, 20, 16, 12, 2, 5, 10, 12, 12, 10, 5, 2, 23, 22, 21, 17, 17, 21, 22, 23}};
const int Params::QUEEN_PST[2][64] = {{2, 4, -0, 12, 12, -0, 4, 2, -3, 14, 16, 17, 17, 16, 14, -3, -4, 10, 15, 5, 5, 15, 10, -4, 8, 12, 14, 14, 14, 14, 12, 8, 10, 13, 18, 24, 24, 18, 13, 10, 3, 21, 26, 21, 21, 26, 21, 3, 2, -12, 8, 8, 8, 8, -12, 2, 18, 12, -8, -20, -20, -8, 12, 18}, {-71, -86, -68, -53, -53, -68, -86, -71, -65, -55, -46, -30, -30, -46, -55, -65, -47, -18, -7, -16, -16, -7, -18, -47, -15, 4, 13, 16, 16, 13, 4, -15, -7, 24, 33, 46, 46, 33, 24, -7, -12, 21, 42, 56, 56, 42, 21, -12, 4, 22, 49, 58, 58, 49, 22, 4, -30, -17, -15, -8, -8, -15, -17, -30}};
const int Params::KING_PST[2][64] = {{14, 30, -29, -8, -8, -29, 30, 14, 55, 56, 3, 25, 25, 3, 56, 55, -52, -26, -103, -128, -128, -103, -26, -52, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -123, -128, -128, -128, -128, -123, -128, 107, -91, -128, -128, -128, -128, -91, 107, -123, -128, 128, -128, -128, 128, -128, -123}, {-21, -18, -22, -28, -28, -22, -18, -21, -5, -11, -17, -17, -17, -17, -11, -5, -14, -16, -13, -11, -11, -13, -16, -14, -12, -7, -1, 3, 3, -1, -7, -12, 2, 11, 19, 20, 20, 19, 11, 2, 11, 24, 26, 27, 27, 26, 24, 11, 1, 15, 18, 17, 17, 18, 15, 1, -30, -5, -1, 1, 1, -1, -5, -30}};
const int Params::KNIGHT_MOBILITY[9] = {-53, -9, 13, 25, 33, 42, 45, 42, 35};
const int Params::BISHOP_MOBILITY[15] = {-32, -5, 9, 20, 31, 38, 42, 45, 49, 52, 53, 54, 64, 46, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-66, -43, -31, -26, -25, -19, -14, -6, -3, 0, 2, 5, 16, 21, 15}, {-79, -43, -23, -6, 10, 16, 16, 20, 26, 30, 34, 38, 40, 30, 23}};
const int Params::QUEEN_MOBILITY[2][24] = {{-86, -27, -13, -4, 3, 9, 17, 22, 28, 32, 35, 36, 38, 39, 45, 47, 46, 62, 76, 87, 96, 96, 96, 96}, {-96, -96, -96, -92, -42, -25, -11, -5, 3, 7, 10, 15, 19, 21, 24, 22, 23, 20, 15, 15, 15, 17, 5, 2}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-52, -16, 3, 15, 17};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 62}, {21, 33}};
const int Params::BISHOP_OUTPOST[2][2] = {{35, 64}, {35, 26}};
const int Params::PAWN_STORM[4][2] = {{0, 21},{12, 2},{33, 1},{4, 0}};

