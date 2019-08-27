// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 25-Aug-2019 11:39:55 by tuner -n 175 -c 16 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0678404

//

#include "params.h"

const int Params::RB_ADJUST[6] = {46, -6, 92, 43, 39, 72};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -28, -57, -63};
const int Params::QR_ADJUST[5] = {18, 75, 28, -39, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -251, -131};
const int Params::MINOR_FOR_PAWNS[6] = {51, 60, 72, 92, 113, 129};
const int Params::CASTLING[6] = {13, -38, -28, 0, 4, -21};
const int Params::KING_COVER[6][4] = {{8, 23, 14, 15},
{2, 7, 4, 5},
{-8, -24, -14, -16},
{-8, -24, -14, -16},
{-10, -30, -17, -20},
{-17, -51, -30, -33}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 84;
const int Params::KRMINOR_VS_R_NO_PAWNS = -199;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -148;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 41;
const int Params::PAWN_ATTACK_FACTOR = 12;
const int Params::MINOR_ATTACK_FACTOR = 47;
const int Params::MINOR_ATTACK_BOOST = 24;
const int Params::ROOK_ATTACK_FACTOR = 59;
const int Params::ROOK_ATTACK_BOOST = 23;
const int Params::QUEEN_ATTACK_FACTOR = 56;
const int Params::QUEEN_ATTACK_BOOST = 43;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 116;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 5;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 96;
const int Params::OWN_ROOK_KING_PROXIMITY = 33;
const int Params::OWN_QUEEN_KING_PROXIMITY = 21;
const int Params::PAWN_THREAT_ON_PIECE_MID = 4;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 37;
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
const int Params::ROOK_PAWN_THREAT_MID = 16;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 40;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 59;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -24;
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
const int Params::TWO_ROOKS_ON_7TH_MID = 45;
const int Params::TWO_ROOKS_ON_7TH_END = 69;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 25;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 60;
const int Params::QUEENING_SQUARE_CONTROL_END = 60;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -15;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -42;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 26, 57, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={228, 191, 151};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35, 37, 40, 42, 45, 48, 51, 54, 58, 61, 65, 69, 73, 77, 81, 86, 91, 96, 101, 106, 112, 118, 124, 130, 137, 144, 150, 158, 165, 173, 180, 188, 197, 205, 214, 222, 231, 240, 249, 259, 268, 278, 287, 297, 307, 316, 326, 336, 345, 355, 365, 374, 384, 393, 402, 412, 421, 429, 438, 447, 455, 463, 471, 479, 486, 493, 501, 507, 514, 520, 527, 533, 538, 544, 549, 554, 559, 564, 569, 573, 577, 581, 585, 589, 592, 595, 598, 601, 604, 607, 610, 612, 614, 616, 619, 621, 622, 624, 626, 628, 629, 631, 632, 633, 634, 636, 637, 638, 639, 640, 641, 641};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 1, 39, 128, 192}, {0, 0, 2, 0, 27, 85, 160, 264}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 0, 20, 96, 0}, {0, 0, 0, 0, 1, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 4, 21, 102, 184, 320}, {0, 0, 0, 0, 36, 89, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 29, 13, 39, 99, 185}, {0, 0, 0, 10, 11, 27, 60, 75}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-30, -22, -27, -5, -32, -9, -14, -24, 0, -23, -31, -16, 0, -23, 0, -37, -9, 0, -17, -79, 0}, {0, 0, 0, -2, -8, -4, 0, -9, -5, -16, -23, -7, -3, -11, -15, -18, -16, -27, -51, -38, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-58, -68, -48, -43, -40, -46, -54, -47, -25, -27, -27, -42, -5, 0, -1, -32, 0, -1, -62, -35, -83}, {-10, -5, 0, -8, -4, -1, -15, -8, -6, -5, -4, -28, -13, -9, -0, -52, -23, -14, -52, -36, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-39, 0, -14, -9, -9, -14, 0, -39}, {-40, -17, -23, -18, -18, -23, -17, -40}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -23, 0, 0, 0, 0, -23, 0}, {-73, -69, -36, -36, -36, -36, -69, -73}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -0, -18, -24, -24, -18, -0, 0}, {-6, -10, -12, -18, -18, -12, -10, -6}};

const int Params::KNIGHT_PST[2][64] = {{-83, -27, -11, -6, -6, -11, -27, -83, -25, -25, -15, 6, 6, -15, -25, -25, -44, -17, 2, 26, 26, 2, -17, -44, -17, 4, 21, 22, 22, 21, 4, -17, -28, -12, 20, 14, 14, 20, -12, -28, -60, -49, -9, -2, -2, -9, -49, -60, -105, -84, -34, -27, -27, -34, -84, -105, -128, -128, -128, -128, -128, -128, -128, -128}, {-81, -43, -31, -33, -33, -31, -43, -81, -37, -26, -26, -17, -17, -26, -26, -37, -47, -29, -20, 2, 2, -20, -29, -47, -23, -11, 7, 17, 17, 7, -11, -23, -33, -24, -3, 1, 1, -3, -24, -33, -53, -34, -18, -12, -12, -18, -34, -53, -72, -49, -45, -34, -34, -45, -49, -72, -128, -74, -75, -61, -61, -75, -74, -128}};
const int Params::BISHOP_PST[2][64] = {{12, 41, 28, 26, 26, 28, 41, 12, 32, 44, 35, 28, 28, 35, 44, 32, 9, 37, 36, 28, 28, 36, 37, 9, -1, 6, 17, 36, 36, 17, 6, -1, -36, -15, -10, 17, 17, -10, -15, -36, 1, -30, -47, -35, -35, -47, -30, 1, -36, -72, -67, -101, -101, -67, -72, -36, -27, -104, -128, -128, -128, -128, -104, -27}, {-15, -2, 3, -2, -2, 3, -2, -15, 5, 7, 3, 9, 9, 3, 7, 5, -2, 10, 15, 19, 19, 15, 10, -2, -2, 6, 19, 24, 24, 19, 6, -2, -22, -7, -7, 3, 3, -7, -7, -22, -0, -15, -13, -15, -15, -13, -15, -0, -9, -14, -22, -25, -25, -22, -14, -9, -4, -6, -9, -30, -30, -9, -6, -4}};
const int Params::ROOK_PST[2][64] = {{-69, -56, -50, -47, -47, -50, -56, -69, -82, -57, -51, -46, -46, -51, -57, -82, -81, -57, -56, -55, -55, -56, -57, -81, -78, -65, -61, -52, -52, -61, -65, -78, -58, -50, -36, -34, -34, -36, -50, -58, -44, -19, -27, -13, -13, -27, -19, -44, -42, -48, -23, -21, -21, -23, -48, -42, -35, -30, -128, -97, -97, -128, -30, -35}, {-29, -30, -21, -27, -27, -21, -30, -29, -40, -34, -26, -26, -26, -26, -34, -40, -36, -31, -24, -27, -27, -24, -31, -36, -24, -17, -11, -16, -16, -11, -17, -24, -9, -7, -1, -5, -5, -1, -7, -9, -3, 1, 6, 4, 4, 6, 1, -3, -14, -11, -5, -4, -4, -5, -11, -14, 8, 6, 6, 2, 2, 6, 6, 8}};
const int Params::QUEEN_PST[2][64] = {{-7, -6, -9, 5, 5, -9, -6, -7, -13, 4, 7, 9, 9, 7, 4, -13, -12, 1, 6, -4, -4, 6, 1, -12, -0, 5, 5, 6, 6, 5, 5, -0, 4, 6, 10, 15, 15, 10, 6, 4, -8, 12, 20, 11, 11, 20, 12, -8, -4, -22, 0, 1, 1, 0, -22, -4, 5, 6, -18, -50, -50, -18, 6, 5}, {-103, -112, -96, -83, -83, -96, -112, -103, -96, -86, -77, -61, -61, -77, -86, -96, -79, -49, -38, -48, -48, -38, -49, -79, -51, -29, -18, -17, -17, -18, -29, -51, -40, -8, 2, 15, 15, 2, -8, -40, -43, -11, 11, 26, 26, 11, -11, -43, -29, -8, 18, 28, 28, 18, -8, -29, -64, -51, -49, -41, -41, -49, -51, -64}};
const int Params::KING_PST[2][64] = {{22, 40, -19, 0, 0, -19, 40, 22, 63, 66, 16, 38, 38, 16, 66, 63, -24, 6, -55, -122, -122, -55, 6, -24, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -127, -47, -128, -128, -128, -128, -47, -127, 86, -32, -128, -128, -128, -128, -32, 86, -96, -128, 128, -128, -128, 128, -128, -96}, {-29, -26, -31, -35, -35, -31, -26, -29, -14, -20, -26, -26, -26, -26, -20, -14, -24, -26, -22, -20, -20, -22, -26, -24, -22, -16, -11, -7, -7, -11, -16, -22, -7, 1, 9, 10, 10, 9, 1, -7, 1, 14, 16, 16, 16, 16, 14, 1, -10, 5, 8, 6, 6, 8, 5, -10, -38, -14, -12, -10, -10, -12, -14, -38}};
const int Params::KNIGHT_MOBILITY[9] = {-78, -33, -11, 1, 10, 19, 22, 20, 13};
const int Params::BISHOP_MOBILITY[15] = {-59, -32, -18, -7, 4, 11, 15, 18, 23, 27, 27, 29, 39, 23, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-75, -51, -38, -34, -33, -27, -22, -14, -10, -7, -6, -1, 9, 18, 9}, {-96, -66, -46, -29, -13, -7, -7, -3, 3, 7, 11, 16, 18, 7, 0}};
const int Params::QUEEN_MOBILITY[2][24] = {{-96, -40, -26, -15, -8, -2, 6, 11, 17, 21, 23, 25, 28, 28, 33, 36, 34, 50, 59, 73, 89, 96, 96, 96}, {-96, -96, -96, -96, -86, -68, -54, -48, -40, -38, -33, -27, -25, -22, -19, -22, -20, -23, -29, -30, -27, -29, -37, -43}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-86, -50, -30, -19, -17};
const int Params::KNIGHT_OUTPOST[2][2] = {{28, 63}, {19, 31}};
const int Params::BISHOP_OUTPOST[2][2] = {{36, 65}, {35, 26}};
const int Params::PAWN_STORM[4][2] = {{0, 21},{10, 1},{34, 1},{3, 0}};

