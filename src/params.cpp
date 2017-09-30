// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 29-Sep-2017 12:04:28 by "tuner -R 50 -c 24 -n 175 /home/jdart/chess/epd/big3.epd"
//

#include "params.h"

const int Params::RB_ADJUST[6] = {43, -6, 74, 22, 15, 40};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -4, -32, -32};
const int Params::QR_ADJUST[5] = {53, 117, 54, -18, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -288, -156};
const int Params::CASTLING[6] = {13, -28, -25, 0, 9, -14};
const int Params::KING_COVER[6][4] = {{12, 24, 13, 12},
{6, 11, 6, 5},
{-4, -8, -4, -4},
{-1, -2, -1, -1},
{-9, -17, -9, -8},
{-21, -42, -22, -20}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 65;
const int Params::PIN_MULTIPLIER_END = 69;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -179;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -171;
const int Params::MINOR_FOR_PAWNS = 31;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 46;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 9;
const int Params::PAWN_ATTACK_FACTOR2 = 9;
const int Params::MINOR_ATTACK_FACTOR = 30;
const int Params::MINOR_ATTACK_BOOST = 39;
const int Params::ROOK_ATTACK_FACTOR = 46;
const int Params::ROOK_ATTACK_BOOST = 21;
const int Params::ROOK_ATTACK_BOOST2 = 39;
const int Params::QUEEN_ATTACK_FACTOR = 47;
const int Params::QUEEN_ATTACK_BOOST = 46;
const int Params::QUEEN_ATTACK_BOOST2 = 56;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 8;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 167;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 11;
const int Params::PIECE_THREAT_MM_MID = 33;
const int Params::PIECE_THREAT_MR_MID = 75;
const int Params::PIECE_THREAT_MQ_MID = 46;
const int Params::PIECE_THREAT_MM_END = 32;
const int Params::PIECE_THREAT_MR_END = 77;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 9;
const int Params::MINOR_PAWN_THREAT_END = 23;
const int Params::PIECE_THREAT_RM_MID = 10;
const int Params::PIECE_THREAT_RR_MID = 0;
const int Params::PIECE_THREAT_RQ_MID = 79;
const int Params::PIECE_THREAT_RM_END = 19;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 14;
const int Params::ROOK_PAWN_THREAT_END = 29;
const int Params::ENDGAME_KING_THREAT = 36;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 33;
const int Params::BISHOP_PAIR_END = 60;
const int Params::BISHOP_PAWN_PLACEMENT_END = -4;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -27;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -22;
const int Params::WEAK_ON_OPEN_FILE_END = -22;
const int Params::SPACE = 4;
const int Params::PAWN_CENTER_SCORE_MID = 0;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 23;
const int Params::TWO_ROOKS_ON_7TH_MID = 26;
const int Params::TWO_ROOKS_ON_7TH_END = 85;
const int Params::ROOK_ON_OPEN_FILE_MID = 22;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 14;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 9;
const int Params::KING_OPP_PAWN_DISTANCE = 3;
const int Params::QUEENING_SQUARE_CONTROL_MID = 64;
const int Params::QUEENING_SQUARE_CONTROL_END = 52;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -26;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -55;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -5;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 2, 22, 51, 83, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={300, 230, 158};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-2, -2, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 21, 22, 24, 26, 28, 30, 33, 35, 38, 40, 43, 46, 49, 53, 56, 60, 64, 68, 72, 77, 82, 87, 92, 97, 103, 109, 115, 122, 128, 135, 142, 150, 157, 165, 173, 182, 190, 199, 208, 217, 227, 236, 246, 256, 266, 276, 286, 296, 306, 316, 326, 337, 347, 357, 367, 377, 386, 396, 405, 415, 424, 433, 441, 450, 458, 466, 474, 482, 489, 496, 503, 509, 516, 522, 527, 533, 538, 543, 548, 553, 557, 562, 566, 570, 573, 577, 580, 583, 586, 589, 592, 594, 596, 599, 601, 603, 605, 606, 608, 610, 611, 613, 614, 615, 616, 618, 619, 620, 621, 621, 622, 623, 624, 624, 625, 626, 626, 627, 627};
const int Params::TRADE_DOWN[8] = {45, 39, 34, 42, 23, 17, 12, 6};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 23, 90, 99}, {0, 0, 0, 0, 15, 66, 151, 224}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {79, 82, 80, 76, 76, 80, 82, 79};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 3, 96, 0}, {0, 0, 0, 0, 0, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 5, 83, 171, 288}, {0, 0, 0, 0, 49, 89, 249, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 6, 0, 21, 88, 172}, {0, 0, 0, 6, 18, 34, 61, 72}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-43, -38, -36, -22, -39, 0, -21, -28, -5, -31, -11, -34, -6, -29, -33, -38, -16, -43, -21, -43, 0}, {-2, 0, -4, -12, -19, -18, -4, -16, -14, -31, -30, -7, -3, -6, -26, -22, -19, -43, -43, -43, -43}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-43, -43, -43, -43, -43, -43, -43, -43, -34, -38, -41, -43, -17, -1, -6, -24, 0, 0, -43, -21, -43}, {-18, -15, -6, -12, -14, -6, -21, -7, -14, -9, -7, -24, -4, -2, -2, -40, -20, -17, -43, -43, -43}};
const int Params::DOUBLED_PAWNS[2][8] = {{-47, -1, -23, -19, -19, -23, -1, -47}, {-43, -20, -30, -18, -18, -30, -20, -43}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -30, -13, 0, 0, -13, -30, 0}, {0, -53, -12, -56, -56, -12, -53, 0}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -11, -12, -12, -11, 0, 0}, {-5, -7, -9, -17, -17, -9, -7, -5}};

const int Params::KNIGHT_PST[2][64] = {{-73, -34, -16, -13, -13, -16, -34, -73, -22, -32, -27, -5, -5, -27, -32, -22, -41, -20, -6, 6, 6, -6, -20, -41, -26, -20, -0, 1, 1, -0, -20, -26, -16, -18, 6, -10, -10, 6, -18, -16, -35, -79, -66, -27, -27, -66, -79, -35, -70, -52, -6, -6, -6, -6, -52, -70, -128, -128, -128, -128, -128, -128, -128, -128}, {-47, -42, -30, -26, -26, -30, -42, -47, -35, -28, -19, -17, -17, -19, -28, -35, -39, -28, -20, -0, -0, -20, -28, -39, -20, -18, 6, 12, 12, 6, -18, -20, -21, -22, -4, -3, -3, -4, -22, -21, -44, -54, -34, -38, -38, -34, -54, -44, -58, -41, -33, -29, -29, -33, -41, -58, -128, -61, -64, -61, -61, -64, -61, -128}};
const int Params::BISHOP_PST[2][64] = {{6, 39, 16, 17, 17, 16, 39, 6, 19, 26, 26, 16, 16, 26, 26, 19, 1, 19, 19, 18, 18, 19, 19, 1, -9, -11, 5, 15, 15, 5, -11, -9, -37, -17, -27, -9, -9, -27, -17, -37, -15, -71, -106, -65, -65, -106, -71, -15, 1, -47, -31, -100, -100, -31, -47, 1, -29, -68, -128, -128, -128, -128, -68, -29}, {-22, -4, 1, -4, -4, 1, -4, -22, -3, 1, -3, 5, 5, -3, 1, -3, -7, -0, 8, 10, 10, 8, -0, -7, -11, 2, 11, 19, 19, 11, 2, -11, -27, -10, -6, 3, 3, -6, -10, -27, -4, -56, -52, -16, -16, -52, -56, -4, -8, -26, -31, -36, -36, -31, -26, -8, -6, 1, -26, 1, 1, -26, 1, -6}};
const int Params::ROOK_PST[2][64] = {{-76, -64, -59, -56, -56, -59, -64, -76, -84, -60, -61, -58, -58, -61, -60, -84, -76, -58, -68, -69, -69, -68, -58, -76, -79, -67, -73, -62, -62, -73, -67, -79, -63, -54, -53, -51, -51, -53, -54, -63, -64, -31, -39, -37, -37, -39, -31, -64, -53, -67, -32, -29, -29, -32, -67, -53, -41, -57, -128, -128, -128, -128, -57, -41}, {-5, -3, 4, -1, -1, 4, -3, -5, -8, -4, 3, 2, 2, 3, -4, -8, -9, -2, -1, 0, 0, -1, -2, -9, -0, 7, 9, 6, 6, 9, 7, -0, 10, 10, 14, 13, 13, 14, 10, 10, 10, 22, 22, 19, 19, 22, 22, 10, 2, 8, 14, 13, 13, 14, 8, 2, 28, 30, 30, 21, 21, 30, 30, 28}};
const int Params::QUEEN_PST[2][64] = {{13, 13, 12, 23, 23, 12, 13, 13, 19, 21, 24, 28, 28, 24, 21, 19, 10, 20, 23, 13, 13, 23, 20, 10, 19, 24, 19, 23, 23, 19, 24, 19, 20, 19, 23, 26, 26, 23, 19, 20, 11, 13, 27, 27, 27, 27, 13, 11, 4, -9, 8, 1, 1, 8, -9, 4, -15, 40, -16, 13, 13, -16, 40, -15}, {-18, -19, -14, -2, -2, -14, -19, -18, -10, -4, 1, 19, 19, 1, -4, -10, 7, 27, 37, 36, 36, 37, 27, 7, 13, 39, 50, 61, 61, 50, 39, 13, 22, 62, 69, 88, 88, 69, 62, 22, 25, 51, 94, 93, 93, 94, 51, 25, 47, 55, 85, 106, 106, 85, 55, 47, 12, 13, 35, 40, 40, 35, 13, 12}};
const int Params::KING_PST[2][64] = {{54, 69, 30, 42, 42, 30, 69, 54, 81, 77, 44, 65, 65, 44, 77, 81, 7, 48, -19, -28, -28, -19, 48, 7, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -73, -128, -128, -128, -128, -73, -128, -128, -101, 36, -128, -128, 36, -101, -128}, {-80, -75, -78, -81, -81, -78, -75, -80, -65, -68, -73, -71, -71, -73, -68, -65, -70, -71, -66, -64, -64, -66, -71, -70, -66, -61, -54, -50, -50, -54, -61, -66, -53, -48, -38, -36, -36, -38, -48, -53, -53, -41, -35, -31, -31, -35, -41, -53, -51, -46, -42, -42, -42, -42, -46, -51, -83, -61, -56, -58, -58, -56, -61, -83}};

const int Params::KNIGHT_MOBILITY[9] = {-55, -23, -7, 1, 8, 15, 18, 17, 12};
const int Params::BISHOP_MOBILITY[15] = {-42, -25, -13, -4, 3, 11, 14, 16, 22, 25, 24, 29, 35, 17, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-43, -27, -21, -17, -15, -9, -5, 5, 8, 10, 14, 14, 30, 31, 43}, {-43, -22, -6, 7, 15, 20, 21, 23, 29, 32, 35, 40, 42, 37, 29}};
const int Params::QUEEN_MOBILITY[2][24] = {{-25, -9, -10, -4, 0, 5, 11, 14, 17, 23, 21, 25, 25, 26, 21, 23, 31, 32, 43, 43, 43, 43, 43, 43}, {-43, -43, -43, -35, -19, -2, 14, 16, 27, 23, 27, 35, 30, 40, 38, 38, 43, 43, 40, 38, 41, 39, 31, 17}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-43, -15, 4, 12, 15};

const int Params::KNIGHT_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{1, 0},{18, 19},{18, 16},{49, 14},{49, 14},{18, 16},{18, 19},{1, 0},{0, 13},{64, 32},{64, 29},{37, 48},{37, 48},{64, 29},{64, 32},{0, 13},{0, 1},{0, 10},{3, 3},{0, 12},{0, 12},{3, 3},{0, 10},{0, 1},{0, 0},{0, 0},{0, 0},{0, 11},{0, 11},{0, 0},{0, 0},{0, 0}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 11},{27, 26},{36, 29},{62, 35},{62, 35},{36, 29},{27, 26},{0, 11},{10, 42},{64, 39},{64, 50},{64, 53},{64, 53},{64, 50},{64, 39},{10, 42},{64, 0},{38, 31},{7, 22},{24, 0},{24, 0},{7, 22},{38, 31},{64, 0},{5, 0},{49, 57},{64, 64},{39, 0},{39, 0},{64, 64},{49, 57},{5, 0}}};
const int Params::BISHOP_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{18, 32},{9, 19},{45, 23},{44, 20},{44, 20},{45, 23},{9, 19},{18, 32},{48, 34},{64, 64},{64, 64},{48, 27},{48, 27},{64, 64},{64, 64},{48, 34},{7, 20},{0, 44},{1, 30},{0, 33},{0, 33},{1, 30},{0, 44},{7, 20},{27, 24},{0, 19},{0, 49},{0, 2},{0, 2},{0, 49},{0, 19},{27, 24}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{32, 24},{52, 22},{63, 23},{63, 23},{52, 22},{32, 24},{0, 0},{0, 0},{64, 62},{64, 64},{64, 19},{64, 19},{64, 64},{64, 62},{0, 0},{0, 0},{64, 0},{17, 0},{64, 0},{64, 0},{17, 0},{64, 0},{0, 0},{0, 0},{64, 0},{64, 0},{64, 0},{64, 0},{64, 0},{64, 0},{0, 0}}};


