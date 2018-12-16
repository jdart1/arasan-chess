// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 15-Dec-2018 02:30:35 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0681295

//

#include "params.h"

const int Params::RB_ADJUST[6] = {48, -6, 93, 45, 41, 77};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -10, -41, -32};
const int Params::QR_ADJUST[5] = {17, 65, 28, -32, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -271, -153};
const int Params::CASTLING[6] = {13, -38, -26, 0, 5, -20};
const int Params::KING_COVER[6][4] = {{9, 27, 16, 17},
{2, 8, 5, 5},
{-7, -22, -13, -14},
{-8, -26, -15, -16},
{-9, -28, -16, -18},
{-16, -51, -30, -33}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -227;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -182;
const int Params::MINOR_FOR_PAWNS = 64;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 38;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::MINOR_ATTACK_FACTOR = 49;
const int Params::MINOR_ATTACK_BOOST = 48;
const int Params::ROOK_ATTACK_FACTOR = 68;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::ROOK_ATTACK_BOOST2 = 46;
const int Params::QUEEN_ATTACK_FACTOR = 66;
const int Params::QUEEN_ATTACK_BOOST = 51;
const int Params::QUEEN_ATTACK_BOOST2 = 73;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 128;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 83;
const int Params::OWN_ROOK_KING_PROXIMITY = 31;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 37;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 81;
const int Params::PIECE_THREAT_MM_END = 33;
const int Params::PIECE_THREAT_MR_END = 84;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 10;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 23;
const int Params::PIECE_THREAT_RR_MID = 12;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 33;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 42;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 54;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -25;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -22;
const int Params::WEAK_ON_OPEN_FILE_END = -17;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 29;
const int Params::TWO_ROOKS_ON_7TH_MID = 31;
const int Params::TWO_ROOKS_ON_7TH_END = 64;
const int Params::ROOK_ON_OPEN_FILE_MID = 34;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 4;
const int Params::ROOK_BEHIND_PP_END = 22;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 61;
const int Params::QUEENING_SQUARE_CONTROL_END = 56;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -17;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 23, 54, 81, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={249, 212, 146};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 18, 19, 20, 22, 23, 25, 26, 28, 30, 32, 34, 36, 38, 41, 43, 46, 49, 51, 54, 57, 61, 64, 67, 71, 75, 79, 83, 87, 92, 96, 101, 106, 111, 116, 122, 127, 133, 139, 145, 152, 158, 165, 172, 179, 186, 193, 201, 208, 216, 224, 232, 240, 248, 256, 264, 272, 281, 289, 298, 306, 314, 323, 331, 339, 348, 356, 364, 372, 380, 388, 395, 403, 410, 417, 425, 432, 438, 445, 452, 458, 464, 470, 476, 481, 487, 492, 497, 502, 507, 511, 516, 520, 524, 528, 532, 535, 539, 542, 545, 548, 551, 554, 557, 559, 562, 564, 566, 569, 571, 573, 574, 576, 578, 579, 581, 582, 584, 585, 586, 587, 589, 590};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 0, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 35, 128, 194}, {0, 0, 0, 0, 25, 83, 160, 272}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 16, 96, 0}, {0, 0, 0, 0, 0, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 5, 6, 22, 102, 210, 320}, {0, 0, 0, 0, 37, 90, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 27, 11, 36, 97, 185}, {0, 0, 0, 9, 10, 26, 61, 67}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-36, -23, -32, -8, -34, -4, -21, -25, -5, -27, -54, -20, 0, -17, -5, -42, -10, -27, -18, -61, 0}, {0, 0, 0, -4, -12, 0, 0, -10, -7, -18, -24, -7, -3, -11, -13, -17, -17, -30, -53, -41, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-60, -75, -54, -51, -44, -52, -58, -54, -26, -31, -34, -43, -9, 0, -3, -31, 0, 0, -66, -38, -83}, {-10, -4, 0, -7, -2, -2, -17, -8, -7, -5, -4, -27, -13, -9, 0, -51, -21, -13, -53, -37, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-42, -0, -17, -11, -11, -17, -0, -42}, {-42, -19, -25, -20, -20, -25, -19, -42}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -30, -1, -11, -11, -1, -30, 0}, {-81, -72, -38, -39, -39, -38, -72, -81}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -17, -23, -23, -17, 0, 0}, {-5, -9, -11, -17, -17, -11, -9, -5}};

const int Params::KNIGHT_PST[2][64] = {{-84, -23, -8, -3, -3, -8, -23, -84, -22, -20, -11, 9, 9, -11, -20, -22, -41, -14, 5, 30, 30, 5, -14, -41, -14, 4, 24, 26, 26, 24, 4, -14, -26, -7, 24, 19, 19, 24, -7, -26, -49, -45, 1, 6, 6, 1, -45, -49, -103, -82, -26, -19, -19, -26, -82, -103, -128, -128, -128, -128, -128, -128, -128, -128}, {-75, -37, -25, -28, -28, -25, -37, -75, -34, -22, -21, -12, -12, -21, -22, -34, -44, -25, -15, 8, 8, -15, -25, -44, -19, -7, 12, 21, 21, 12, -7, -19, -31, -22, -1, 3, 3, -1, -22, -31, -51, -34, -15, -10, -10, -15, -34, -51, -71, -47, -44, -29, -29, -44, -47, -71, -128, -77, -76, -58, -58, -76, -77, -128}};
const int Params::BISHOP_PST[2][64] = {{15, 45, 32, 30, 30, 32, 45, 15, 37, 47, 39, 32, 32, 39, 47, 37, 14, 42, 40, 34, 34, 40, 42, 14, 2, 12, 22, 42, 42, 22, 12, 2, -34, -9, -2, 26, 26, -2, -9, -34, 6, -26, -38, -25, -25, -38, -26, 6, -30, -65, -62, -93, -93, -62, -65, -30, -20, -84, -128, -128, -128, -128, -84, -20}, {-8, 5, 11, 6, 6, 11, 5, -8, 11, 15, 10, 17, 17, 10, 15, 11, 5, 17, 23, 26, 26, 23, 17, 5, 4, 13, 27, 32, 32, 27, 13, 4, -16, -2, -1, 8, 8, -1, -2, -16, 4, -9, -9, -9, -9, -9, -9, 4, -5, -10, -17, -21, -21, -17, -10, -5, -0, -2, -6, -26, -26, -6, -2, -0}};
const int Params::ROOK_PST[2][64] = {{-50, -37, -31, -27, -27, -31, -37, -50, -63, -37, -32, -27, -27, -32, -37, -63, -61, -37, -37, -35, -35, -37, -37, -61, -60, -45, -42, -33, -33, -42, -45, -60, -38, -30, -17, -15, -15, -17, -30, -38, -25, -0, -9, 3, 3, -9, -0, -25, -24, -29, -4, -1, -1, -4, -29, -24, -16, -9, -100, -71, -71, -100, -9, -16}, {-6, -7, 2, -4, -4, 2, -7, -6, -16, -10, -2, -2, -2, -2, -10, -16, -13, -7, -0, -3, -3, -0, -7, -13, -1, 7, 13, 8, 8, 13, 7, -1, 14, 16, 21, 17, 17, 21, 16, 14, 20, 23, 29, 27, 27, 29, 23, 20, 7, 11, 16, 18, 18, 16, 11, 7, 30, 29, 28, 25, 25, 28, 29, 30}};
const int Params::QUEEN_PST[2][64] = {{32, 33, 28, 40, 40, 28, 33, 32, 25, 43, 45, 46, 46, 45, 43, 25, 25, 39, 44, 34, 34, 44, 39, 25, 37, 42, 44, 44, 44, 44, 42, 37, 41, 44, 47, 56, 56, 47, 44, 41, 31, 50, 55, 52, 52, 55, 50, 31, 34, 16, 38, 41, 41, 38, 16, 34, 44, 45, 22, 4, 4, 22, 45, 44}, {-48, -61, -45, -30, -30, -45, -61, -48, -34, -30, -22, -6, -6, -22, -30, -34, -23, 5, 16, 7, 7, 16, 5, -23, 9, 27, 35, 37, 37, 35, 27, 9, 16, 48, 54, 68, 68, 54, 48, 16, 10, 44, 64, 81, 81, 64, 44, 10, 26, 47, 73, 84, 84, 73, 47, 26, 3, 19, 17, 21, 21, 17, 19, 3}};
const int Params::KING_PST[2][64] = {{41, 58, -3, 18, 18, -3, 58, 41, 81, 81, 29, 52, 52, 29, 81, 81, -5, 18, -66, -121, -121, -66, 18, -5, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -106, -108, -128, -128, -128, -128, -108, -106, 38, -26, -128, -128, -128, -128, -26, 38, -128, -128, 128, -128, -128, 128, -128, -128}, {-58, -60, -64, -68, -68, -64, -60, -58, -47, -55, -61, -61, -61, -61, -55, -47, -57, -61, -58, -56, -56, -58, -61, -57, -54, -51, -47, -44, -44, -47, -51, -54, -41, -36, -29, -28, -28, -29, -36, -41, -34, -24, -23, -23, -23, -23, -24, -34, -44, -34, -30, -32, -32, -30, -34, -44, -72, -47, -47, -45, -45, -47, -47, -72}};

const int Params::KNIGHT_MOBILITY[9] = {-72, -27, -5, 6, 15, 24, 27, 25, 17};
const int Params::BISHOP_MOBILITY[15] = {-55, -28, -13, -2, 8, 15, 19, 23, 27, 30, 30, 33, 42, 27, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-61, -37, -25, -20, -19, -13, -9, -0, 4, 7, 9, 12, 22, 30, 24}, {-67, -30, -10, 6, 22, 29, 29, 33, 40, 44, 49, 53, 55, 45, 36}};
const int Params::QUEEN_MOBILITY[2][24] = {{-66, -5, 8, 17, 24, 31, 38, 43, 49, 53, 56, 57, 60, 60, 64, 68, 66, 81, 93, 96, 96, 96, 96, 96}, {-96, -96, -78, -38, 11, 24, 41, 47, 55, 59, 64, 70, 76, 79, 84, 82, 88, 84, 84, 86, 89, 91, 83, 77}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-92, -53, -32, -20, -14};

const int Params::KNIGHT_OUTPOST[2][2] = {{25, 62}, {20, 33}};
const int Params::BISHOP_OUTPOST[2][2] = {{34, 62}, {35, 27}};

const int Params::PAWN_ATTACK_FACTOR[2][4] = {{0, 40, 40, 0}, {40, 0, 40, 0}};


