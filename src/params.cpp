// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 28-Dec-2017 10:55:28 by "tuner -n 175 -R 25 -c 24 /home/jdart/chess/epd/big5.epd"
//

#include "params.h"

const int Params::RB_ADJUST[6] = {53, -6, 89, 40, 31, 60};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -12, -39, -32};
const int Params::QR_ADJUST[5] = {21, 73, 24, -36, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -292, -166};
const int Params::CASTLING[6] = {13, -38, -24, 0, 5, -18};
const int Params::KING_COVER[6][4] = {{11, 22, 14, 13},
{5, 9, 6, 5},
{-6, -12, -8, -7},
{-5, -11, -7, -6},
{-9, -18, -11, -10},
{-18, -37, -23, -21}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 79;
const int Params::PIN_MULTIPLIER_END = 82;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -184;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -149;
const int Params::MINOR_FOR_PAWNS = 41;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 39;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 8;
const int Params::PAWN_ATTACK_FACTOR2 = 8;
const int Params::MINOR_ATTACK_FACTOR = 33;
const int Params::MINOR_ATTACK_BOOST = 38;
const int Params::ROOK_ATTACK_FACTOR = 47;
const int Params::ROOK_ATTACK_BOOST = 21;
const int Params::ROOK_ATTACK_BOOST2 = 32;
const int Params::QUEEN_ATTACK_FACTOR = 50;
const int Params::QUEEN_ATTACK_BOOST = 45;
const int Params::QUEEN_ATTACK_BOOST2 = 53;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 9;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 159;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 12;
const int Params::PIECE_THREAT_MM_MID = 30;
const int Params::PIECE_THREAT_MR_MID = 91;
const int Params::PIECE_THREAT_MQ_MID = 67;
const int Params::PIECE_THREAT_MM_END = 30;
const int Params::PIECE_THREAT_MR_END = 74;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 9;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 21;
const int Params::PIECE_THREAT_RR_MID = 0;
const int Params::PIECE_THREAT_RQ_MID = 94;
const int Params::PIECE_THREAT_RM_END = 31;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 40;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 34;
const int Params::BISHOP_PAIR_END = 57;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -6;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -27;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -24;
const int Params::WEAK_ON_OPEN_FILE_END = -19;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 0;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 25;
const int Params::TWO_ROOKS_ON_7TH_MID = 16;
const int Params::TWO_ROOKS_ON_7TH_END = 69;
const int Params::ROOK_ON_OPEN_FILE_MID = 27;
const int Params::ROOK_ON_OPEN_FILE_END = 16;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 13;
const int Params::QUEEN_OUT = -31;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 10;
const int Params::KING_OPP_PAWN_DISTANCE = 4;
const int Params::QUEENING_SQUARE_CONTROL_MID = 57;
const int Params::QUEENING_SQUARE_CONTROL_END = 51;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -21;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -44;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 22, 51, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={295, 219, 153};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-2, -2, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 20, 22, 24, 26, 28, 30, 32, 35, 37, 40, 43, 46, 49, 53, 56, 60, 64, 68, 72, 77, 81, 86, 92, 97, 103, 109, 115, 121, 128, 135, 142, 150, 157, 165, 173, 182, 190, 199, 208, 217, 226, 236, 246, 255, 265, 275, 285, 296, 306, 316, 326, 336, 346, 356, 366, 376, 385, 395, 404, 414, 423, 431, 440, 448, 456, 464, 472, 479, 487, 493, 500, 507, 513, 519, 524, 530, 535, 540, 545, 549, 554, 558, 562, 565, 569, 572, 575, 578, 581, 584, 587, 589, 591, 593, 596, 597, 599, 601, 603, 604, 606, 607, 608, 609, 611, 612, 613, 614, 615, 615, 616, 617, 618, 618, 619, 619, 620, 621, 621, 621};
const int Params::TRADE_DOWN[8] = {45, 40, 34, 29, 23, 18, 12, 7};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 27, 98, 134}, {0, 0, 0, 0, 18, 68, 150, 224}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {80, 82, 80, 75, 75, 80, 82, 80};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 10, 96, 0}, {0, 0, 0, 0, 0, 16, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 15, 94, 220, 288}, {0, 0, 0, 0, 43, 93, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 12, 1, 24, 90, 174}, {0, 0, 0, 8, 13, 28, 56, 56}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-40, -37, -37, -22, -35, -7, -27, -28, -13, -27, -30, -32, 0, -17, -28, -39, -8, -42, -26, -43, 0}, {0, 0, 0, -9, -17, -8, -1, -13, -13, -26, -25, -5, -4, -8, -15, -18, -16, -27, -43, -43, -43}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-43, -43, -43, -43, -43, -43, -43, -43, -32, -36, -37, -43, -12, -0, -7, -30, 0, 0, -43, -22, -43}, {-15, -13, 0, -6, -9, -4, -23, -11, -12, -6, -10, -27, -10, -4, -1, -43, -19, -11, -43, -38, -43}};
const int Params::DOUBLED_PAWNS[2][8] = {{-44, 0, -20, -16, -16, -20, 0, -44}, {-45, -20, -27, -21, -21, -27, -20, -45}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -29, -6, -7, -7, -6, -29, 0}, {-29, -64, -30, -44, -44, -30, -64, -29}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -14, -17, -17, -14, 0, 0}, {-4, -8, -10, -17, -17, -10, -8, -4}};

const int Params::KNIGHT_PST[2][64] = {{-79, -29, -10, -11, -11, -10, -29, -79, -23, -29, -22, 0, 0, -22, -29, -23, -42, -20, -2, 18, 18, -2, -20, -42, -21, -6, 13, 13, 13, 13, -6, -21, -12, -12, 15, 2, 2, 15, -12, -12, -35, -52, -19, 1, 1, -19, -52, -35, -83, -62, -9, -5, -5, -9, -62, -83, -128, -128, -128, -128, -128, -128, -128, -128}, {-67, -50, -35, -33, -33, -35, -50, -67, -43, -31, -28, -22, -22, -28, -31, -43, -50, -33, -24, -1, -1, -24, -33, -50, -24, -18, 6, 13, 13, 6, -18, -24, -25, -20, -1, -10, -10, -1, -20, -25, -48, -38, -31, -30, -30, -31, -38, -48, -59, -44, -33, -24, -24, -33, -44, -59, -128, -54, -61, -56, -56, -61, -54, -128}};
const int Params::BISHOP_PST[2][64] = {{4, 36, 21, 17, 17, 21, 36, 4, 24, 32, 29, 20, 20, 29, 32, 24, 3, 28, 28, 21, 21, 28, 28, 3, -6, 0, 10, 27, 27, 10, 0, -6, -35, -17, -15, 5, 5, -15, -17, -35, -14, -55, -76, -42, -42, -76, -55, -14, -41, -43, -35, -82, -82, -35, -43, -41, -9, -52, -128, -128, -128, -128, -52, -9}, {-24, -7, -4, -8, -8, -4, -7, -24, -5, -2, -4, 3, 3, -4, -2, -5, -9, 1, 7, 10, 10, 7, 1, -9, -11, 1, 12, 17, 17, 12, 1, -11, -20, -8, -6, -1, -1, -6, -8, -20, -5, -48, -51, -13, -13, -51, -48, -5, -9, -16, -29, -34, -34, -29, -16, -9, -5, 3, -8, -2, -2, -8, 3, -5}};
const int Params::ROOK_PST[2][64] = {{-73, -59, -54, -51, -51, -54, -59, -73, -84, -59, -55, -51, -51, -55, -59, -84, -80, -58, -60, -61, -61, -60, -58, -80, -81, -65, -65, -56, -56, -65, -65, -81, -60, -49, -42, -33, -33, -42, -49, -60, -49, -23, -27, -14, -14, -27, -23, -49, -47, -54, -22, -17, -17, -22, -54, -47, -39, -36, -115, -82, -82, -115, -36, -39}, {-13, -12, -5, -10, -10, -5, -12, -13, -17, -14, -6, -7, -7, -6, -14, -17, -17, -9, -6, -7, -7, -6, -9, -17, -8, 4, 7, 2, 2, 7, 4, -8, 8, 9, 15, 11, 11, 15, 9, 8, 13, 20, 22, 20, 20, 22, 20, 13, 3, 7, 13, 14, 14, 13, 7, 3, 26, 28, 28, 23, 23, 28, 28, 26}};
const int Params::QUEEN_PST[2][64] = {{28, 31, 30, 40, 40, 30, 31, 28, 26, 38, 43, 46, 46, 43, 38, 26, 21, 35, 42, 32, 32, 42, 35, 21, 33, 41, 40, 41, 41, 40, 41, 33, 35, 37, 44, 49, 49, 44, 37, 35, 26, 34, 48, 46, 46, 48, 34, 26, 13, 2, 26, 22, 22, 26, 2, 13, 15, 25, 5, 2, 2, 5, 25, 15}, {-39, -48, -37, -25, -25, -37, -48, -39, -24, -28, -20, -3, -3, -20, -28, -24, -10, 11, 22, 17, 17, 22, 11, -10, 11, 31, 41, 46, 46, 41, 31, 11, 24, 55, 63, 79, 79, 63, 55, 24, 25, 56, 82, 88, 88, 82, 56, 25, 44, 56, 86, 98, 98, 86, 56, 44, 16, 29, 33, 38, 38, 33, 29, 16}};
const int Params::KING_PST[2][64] = {{31, 50, 3, 17, 17, 3, 50, 31, 70, 69, 35, 51, 51, 35, 69, 70, 2, 33, -27, -67, -67, -27, 33, 2, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -125, -105, -128, -128, -128, -128, -105, -125, -39, 128, -128, -128, -128, -128, 128, -39, -128, -128, 128, -128, -128, 128, -128, -128}, {-78, -75, -78, -81, -81, -78, -75, -78, -63, -68, -73, -71, -71, -73, -68, -63, -70, -71, -67, -65, -65, -67, -71, -70, -67, -61, -54, -52, -52, -54, -61, -67, -52, -43, -36, -35, -35, -36, -43, -52, -45, -34, -29, -28, -28, -29, -34, -45, -50, -39, -35, -36, -36, -35, -39, -50, -80, -56, -51, -54, -54, -51, -56, -80}};

const int Params::KNIGHT_MOBILITY[9] = {-64, -26, -7, 3, 12, 20, 23, 21, 15};
const int Params::BISHOP_MOBILITY[15] = {-42, -22, -9, -0, 9, 17, 21, 24, 30, 33, 33, 38, 43, 27, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-43, -28, -18, -15, -13, -7, -2, 7, 12, 17, 18, 21, 34, 35, 32}, {-43, -34, -15, 0, 13, 19, 21, 24, 30, 34, 38, 43, 43, 37, 31}};
const int Params::QUEEN_MOBILITY[2][24] = {{-43, -17, -11, -4, 3, 8, 14, 19, 23, 28, 29, 31, 33, 33, 35, 39, 40, 43, 43, 43, 43, 43, 43, 43}, {-43, -43, -43, -43, -24, -11, 10, 14, 25, 23, 29, 35, 35, 39, 42, 41, 43, 43, 43, 42, 43, 43, 36, 27}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-43, -16, 5, 16, 19};

const int Params::KNIGHT_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 7},{15, 14},{22, 14},{54, 24},{54, 24},{22, 14},{15, 14},{0, 7},{0, 15},{35, 19},{30, 31},{13, 41},{13, 41},{30, 31},{35, 19},{0, 15},{2, 2},{0, 12},{0, 6},{0, 9},{0, 9},{0, 6},{0, 12},{2, 2},{0, 0},{0, 0},{0, 5},{0, 17},{0, 17},{0, 5},{0, 0},{0, 0}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{7, 21},{34, 16},{48, 26},{64, 44},{64, 44},{48, 26},{34, 16},{7, 21},{31, 31},{58, 34},{64, 47},{64, 45},{64, 45},{64, 47},{58, 34},{31, 31},{38, 0},{46, 19},{44, 16},{57, 3},{57, 3},{44, 16},{46, 19},{38, 0},{6, 0},{34, 35},{64, 56},{59, 0},{59, 0},{64, 56},{34, 35},{6, 0}}};
const int Params::BISHOP_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{24, 25},{15, 19},{44, 24},{42, 29},{42, 29},{44, 24},{15, 19},{24, 25},{48, 35},{55, 57},{64, 64},{37, 28},{37, 28},{64, 64},{55, 57},{48, 35},{28, 27},{0, 34},{2, 34},{0, 38},{0, 38},{2, 34},{0, 34},{28, 27},{9, 27},{0, 21},{0, 32},{0, 4},{0, 4},{0, 32},{0, 21},{9, 27}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{9, 0},{44, 24},{49, 21},{64, 25},{64, 25},{49, 21},{44, 24},{9, 0},{0, 0},{64, 61},{64, 64},{64, 19},{64, 19},{64, 64},{64, 61},{0, 0},{0, 0},{64, 0},{12, 0},{64, 0},{64, 0},{12, 0},{64, 0},{0, 0},{0, 0},{64, 0},{64, 0},{64, 0},{64, 0},{64, 0},{64, 0},{0, 0}}};


