// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 18-Aug-2019 02:56:25 by tuner -n 175 -c 16 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0678411

//

#include "params.h"

const int Params::RB_ADJUST[6] = {46, -6, 92, 43, 39, 72};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -28, -58, -63};
const int Params::QR_ADJUST[5] = {18, 75, 30, -38, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -251, -131};
const int Params::MINOR_FOR_PAWNS[6] = {51, 59, 72, 92, 113, 128};
const int Params::CASTLING[6] = {13, -38, -28, 0, 4, -21};
const int Params::KING_COVER[6][4] = {{8, 24, 14, 16},
{2, 7, 4, 4},
{-8, -24, -14, -16},
{-8, -24, -14, -16},
{-10, -30, -18, -20},
{-17, -51, -29, -33}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 84;
const int Params::KRMINOR_VS_R_NO_PAWNS = -199;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -148;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 41;
const int Params::PAWN_ATTACK_FACTOR = 14;
const int Params::MINOR_ATTACK_FACTOR = 47;
const int Params::MINOR_ATTACK_BOOST = 26;
const int Params::ROOK_ATTACK_FACTOR = 59;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 55;
const int Params::QUEEN_ATTACK_BOOST = 45;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 118;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 3;
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
const int Params::PIECE_THREAT_RM_END = 33;
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
const int Params::TWO_ROOKS_ON_7TH_MID = 45;
const int Params::TWO_ROOKS_ON_7TH_END = 69;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 4;
const int Params::ROOK_BEHIND_PP_END = 25;
const int Params::QUEEN_OUT = -31;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 59;
const int Params::QUEENING_SQUARE_CONTROL_END = 60;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -15;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -42;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 26, 57, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={229, 192, 151};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 25, 27, 29, 31, 33, 36, 38, 40, 43, 46, 49, 52, 55, 59, 62, 66, 70, 74, 78, 83, 87, 92, 97, 103, 108, 114, 120, 126, 132, 139, 146, 153, 160, 167, 175, 183, 191, 199, 208, 216, 225, 234, 243, 252, 261, 271, 280, 290, 299, 309, 319, 328, 338, 348, 357, 367, 376, 385, 394, 404, 413, 421, 430, 438, 447, 455, 463, 470, 478, 485, 492, 499, 506, 512, 518, 524, 530, 536, 541, 546, 551, 556, 560, 564, 569, 573, 576, 580, 583, 587, 590, 593, 596, 598, 601, 603, 606, 608, 610, 612, 614, 616, 617, 619, 620, 622, 623, 624, 626, 627, 628, 629, 630, 631, 632, 632};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 1, 39, 128, 194}, {0, 0, 2, 0, 27, 85, 160, 264}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 0, 20, 96, 0}, {0, 0, 0, 0, 1, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 5, 22, 102, 186, 320}, {0, 0, 0, 0, 36, 89, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 28, 13, 39, 99, 184}, {0, 0, 0, 10, 11, 27, 60, 75}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-30, -22, -27, -6, -31, -14, -14, -24, 0, -25, -37, -16, 0, -22, 0, -38, -8, -6, -16, -74, 0}, {0, 0, 0, -2, -9, -3, 0, -9, -5, -16, -22, -7, -3, -11, -15, -18, -16, -27, -51, -38, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-58, -69, -49, -42, -40, -46, -54, -47, -25, -27, -27, -42, -6, 0, -1, -32, 0, -1, -61, -35, -83}, {-11, -5, 0, -7, -3, -1, -15, -8, -6, -5, -4, -28, -13, -9, -0, -52, -23, -14, -52, -36, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-39, 0, -14, -9, -9, -14, 0, -39}, {-40, -17, -23, -18, -18, -23, -17, -40}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -22, 0, 0, 0, 0, -22, 0}, {-74, -69, -36, -36, -36, -36, -69, -74}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -0, -18, -24, -24, -18, -0, 0}, {-6, -10, -12, -18, -18, -12, -10, -6}};

const int Params::KNIGHT_PST[2][64] = {{-83, -27, -11, -6, -6, -11, -27, -83, -26, -25, -15, 6, 6, -15, -25, -26, -44, -17, 2, 26, 26, 2, -17, -44, -17, 2, 21, 22, 22, 21, 2, -17, -28, -11, 21, 14, 14, 21, -11, -28, -60, -47, -7, 0, 0, -7, -47, -60, -105, -83, -34, -28, -28, -34, -83, -105, -128, -128, -128, -128, -128, -128, -128, -128}, {-81, -44, -32, -34, -34, -32, -44, -81, -38, -27, -26, -18, -18, -26, -27, -38, -47, -30, -21, 2, 2, -21, -30, -47, -24, -12, 7, 16, 16, 7, -12, -24, -34, -25, -4, 1, 1, -4, -25, -34, -54, -35, -18, -13, -13, -18, -35, -54, -72, -49, -46, -34, -34, -46, -49, -72, -128, -74, -75, -62, -62, -75, -74, -128}};
const int Params::BISHOP_PST[2][64] = {{13, 41, 29, 26, 26, 29, 41, 13, 33, 45, 35, 28, 28, 35, 45, 33, 10, 37, 37, 28, 28, 37, 37, 10, -1, 6, 17, 36, 36, 17, 6, -1, -37, -16, -9, 17, 17, -9, -16, -37, 3, -31, -47, -35, -35, -47, -31, 3, -36, -72, -67, -101, -101, -67, -72, -36, -27, -105, -128, -128, -128, -128, -105, -27}, {-15, -2, 3, -3, -3, 3, -2, -15, 5, 7, 2, 9, 9, 2, 7, 5, -2, 10, 15, 18, 18, 15, 10, -2, -3, 6, 19, 24, 24, 19, 6, -3, -22, -7, -7, 2, 2, -7, -7, -22, -1, -15, -14, -15, -15, -14, -15, -1, -9, -15, -23, -25, -25, -23, -15, -9, -3, -6, -10, -31, -31, -10, -6, -3}};
const int Params::ROOK_PST[2][64] = {{-70, -57, -51, -48, -48, -51, -57, -70, -83, -58, -52, -47, -47, -52, -58, -83, -82, -57, -57, -55, -55, -57, -57, -82, -79, -65, -62, -53, -53, -62, -65, -79, -58, -51, -37, -35, -35, -37, -51, -58, -45, -20, -29, -14, -14, -29, -20, -45, -42, -48, -23, -21, -21, -23, -48, -42, -36, -30, -128, -99, -99, -128, -30, -36}, {-29, -30, -21, -27, -27, -21, -30, -29, -40, -34, -26, -26, -26, -26, -34, -40, -36, -31, -24, -27, -27, -24, -31, -36, -24, -17, -11, -16, -16, -11, -17, -24, -9, -7, -1, -5, -5, -1, -7, -9, -3, 1, 6, 4, 4, 6, 1, -3, -14, -10, -5, -4, -4, -5, -10, -14, 8, 6, 6, 2, 2, 6, 6, 8}};
const int Params::QUEEN_PST[2][64] = {{-7, -5, -9, 5, 5, -9, -5, -7, -12, 5, 7, 9, 9, 7, 5, -12, -12, 1, 7, -3, -3, 7, 1, -12, 0, 6, 6, 7, 7, 6, 6, 0, 4, 6, 11, 16, 16, 11, 6, 4, -7, 12, 21, 11, 11, 21, 12, -7, -4, -22, -1, 1, 1, -1, -22, -4, 6, 9, -21, -51, -51, -21, 9, 6}, {-103, -111, -95, -83, -83, -95, -111, -103, -94, -85, -76, -60, -60, -76, -85, -94, -78, -48, -37, -47, -47, -37, -48, -78, -50, -28, -18, -15, -15, -18, -28, -50, -40, -8, 3, 16, 16, 3, -8, -40, -41, -10, 12, 27, 27, 12, -10, -41, -28, -7, 18, 28, 28, 18, -7, -28, -63, -51, -48, -40, -40, -48, -51, -63}};
const int Params::KING_PST[2][64] = {{17, 34, -25, -5, -5, -25, 34, 17, 60, 61, 11, 33, 33, 11, 61, 60, -24, -0, -63, -128, -128, -63, -0, -24, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -116, -58, -128, -128, -128, -128, -58, -116, 82, -24, -128, -128, -128, -128, -24, 82, -110, -128, 128, -128, -128, 128, -128, -110}, {-30, -27, -31, -36, -36, -31, -27, -30, -15, -20, -26, -26, -26, -26, -20, -15, -24, -26, -22, -21, -21, -22, -26, -24, -22, -16, -11, -8, -8, -11, -16, -22, -8, 0, 8, 10, 10, 8, 0, -8, 0, 13, 15, 16, 16, 15, 13, 0, -10, 4, 7, 6, 6, 7, 4, -10, -38, -15, -12, -11, -11, -12, -15, -38}};
const int Params::KNIGHT_MOBILITY[9] = {-77, -32, -10, 1, 10, 19, 22, 21, 13};
const int Params::BISHOP_MOBILITY[15] = {-59, -32, -18, -7, 4, 11, 15, 19, 23, 27, 27, 30, 39, 23, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-73, -49, -36, -32, -30, -24, -20, -12, -8, -5, -4, 1, 11, 20, 11}, {-96, -67, -47, -29, -13, -7, -7, -3, 3, 7, 11, 16, 18, 7, -0}};
const int Params::QUEEN_MOBILITY[2][24] = {{-96, -39, -24, -14, -7, -0, 7, 12, 18, 22, 24, 26, 28, 29, 35, 37, 34, 51, 60, 74, 90, 96, 96, 96}, {-96, -96, -96, -96, -88, -69, -55, -49, -41, -38, -35, -28, -26, -23, -20, -23, -21, -24, -31, -30, -28, -30, -38, -45}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-87, -51, -32, -21, -18};
const int Params::KNIGHT_OUTPOST[2][2] = {{28, 63}, {19, 31}};
const int Params::BISHOP_OUTPOST[2][2] = {{36, 65}, {35, 26}};
const int Params::PAWN_STORM[4][2] = {{0, 24},{11, 2},{34, 1},{3, 0}};

