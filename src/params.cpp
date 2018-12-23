// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 22-Dec-2018 08:52:39 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0680894

//

#include "params.h"

const int Params::RB_ADJUST[6] = {47, -6, 92, 44, 41, 77};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -11, -42, -32};
const int Params::QR_ADJUST[5] = {18, 65, 27, -34, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -271, -153};
const int Params::CASTLING[6] = {13, -38, -26, 0, 6, -20};
const int Params::KING_COVER[6][4] = {{8, 23, 13, 15},
{2, 7, 4, 5},
{-7, -21, -12, -14},
{-7, -21, -12, -14},
{-8, -24, -14, -16},
{-15, -45, -26, -29}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -229;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -182;
const int Params::MINOR_FOR_PAWNS = 64;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 38;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR = 14;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 42;
const int Params::ROOK_ATTACK_FACTOR = 60;
const int Params::ROOK_ATTACK_BOOST = 22;
const int Params::ROOK_ATTACK_BOOST2 = 41;
const int Params::QUEEN_ATTACK_FACTOR = 61;
const int Params::QUEEN_ATTACK_BOOST = 45;
const int Params::QUEEN_ATTACK_BOOST2 = 63;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 6;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 136;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 88;
const int Params::OWN_ROOK_KING_PROXIMITY = 36;
const int Params::OWN_QUEEN_KING_PROXIMITY = 17;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 37;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 81;
const int Params::PIECE_THREAT_MM_END = 33;
const int Params::PIECE_THREAT_MR_END = 85;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 10;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 22;
const int Params::PIECE_THREAT_RR_MID = 11;
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
const int Params::CENTER_PAWN_BLOCK = -24;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -22;
const int Params::WEAK_ON_OPEN_FILE_END = -17;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 28;
const int Params::TWO_ROOKS_ON_7TH_MID = 34;
const int Params::TWO_ROOKS_ON_7TH_END = 64;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 22;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 62;
const int Params::QUEENING_SQUARE_CONTROL_END = 56;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -17;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 24, 54, 81, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={247, 212, 146};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 1, 1, 1, 2, 3, 3, 4, 4, 5, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 20, 21, 23, 24, 26, 28, 30, 32, 34, 37, 39, 42, 44, 47, 50, 53, 57, 60, 64, 67, 71, 75, 80, 84, 89, 94, 99, 104, 110, 115, 121, 127, 134, 140, 147, 154, 161, 169, 176, 184, 192, 201, 209, 218, 226, 235, 244, 253, 263, 272, 281, 291, 300, 310, 320, 329, 339, 349, 358, 368, 377, 386, 396, 405, 414, 423, 431, 440, 448, 456, 464, 472, 480, 487, 494, 501, 508, 514, 521, 527, 533, 538, 544, 549, 554, 559, 563, 568, 572, 576, 580, 584, 587, 591, 594, 597, 600, 603, 605, 608, 610, 613, 615, 617, 619, 621, 622, 624, 626, 627, 629, 630, 631, 632, 634, 635, 636, 637, 638, 639, 639, 640, 641, 642};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 0, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 35, 128, 193}, {0, 0, 0, 0, 26, 83, 160, 272}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 16, 96, 0}, {0, 0, 0, 0, 0, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 5, 6, 24, 102, 206, 320}, {0, 0, 0, 0, 37, 90, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 28, 12, 36, 97, 187}, {0, 0, 0, 9, 10, 26, 62, 68}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-36, -24, -32, -8, -34, -4, -21, -25, -4, -28, -54, -20, 0, -19, -1, -41, -8, -39, -21, -56, 0}, {0, 0, 0, -4, -12, 0, 0, -10, -7, -19, -26, -7, -3, -12, -14, -17, -17, -30, -53, -41, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-59, -75, -53, -51, -45, -51, -58, -55, -27, -31, -33, -43, -8, 0, -4, -31, 0, 0, -66, -37, -83}, {-10, -4, 0, -7, -2, -1, -16, -7, -7, -5, -4, -27, -13, -10, 0, -51, -21, -13, -53, -37, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-42, 0, -17, -11, -11, -17, 0, -42}, {-42, -19, -25, -20, -20, -25, -19, -42}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -26, -1, -12, -12, -1, -26, 0}, {-85, -73, -38, -40, -40, -38, -73, -85}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -17, -23, -23, -17, 0, 0}, {-5, -9, -11, -17, -17, -11, -9, -5}};

const int Params::KNIGHT_PST[2][64] = {{-82, -23, -9, -2, -2, -9, -23, -82, -21, -20, -11, 9, 9, -11, -20, -21, -40, -14, 5, 30, 30, 5, -14, -40, -14, 4, 24, 26, 26, 24, 4, -14, -26, -8, 24, 19, 19, 24, -8, -26, -50, -44, 1, 6, 6, 1, -44, -50, -101, -81, -26, -19, -19, -26, -81, -101, -128, -128, -128, -128, -128, -128, -128, -128}, {-75, -36, -25, -27, -27, -25, -36, -75, -33, -21, -20, -11, -11, -20, -21, -33, -43, -24, -14, 8, 8, -14, -24, -43, -18, -7, 13, 22, 22, 13, -7, -18, -30, -21, -0, 4, 4, -0, -21, -30, -51, -33, -14, -9, -9, -14, -33, -51, -71, -46, -44, -28, -28, -44, -46, -71, -128, -77, -75, -58, -58, -75, -77, -128}};
const int Params::BISHOP_PST[2][64] = {{15, 45, 32, 30, 30, 32, 45, 15, 36, 48, 39, 32, 32, 39, 48, 36, 14, 42, 40, 34, 34, 40, 42, 14, 2, 11, 22, 42, 42, 22, 11, 2, -33, -9, -3, 25, 25, -3, -9, -33, 5, -25, -39, -24, -24, -39, -25, 5, -29, -64, -62, -96, -96, -62, -64, -29, -20, -87, -128, -128, -128, -128, -87, -20}, {-7, 5, 11, 7, 7, 11, 5, -7, 12, 15, 10, 17, 17, 10, 15, 12, 5, 18, 23, 26, 26, 23, 18, 5, 4, 13, 27, 32, 32, 27, 13, 4, -16, -1, -1, 9, 9, -1, -1, -16, 5, -9, -8, -8, -8, -8, -9, 5, -4, -9, -17, -20, -20, -17, -9, -4, 0, -1, -6, -26, -26, -6, -1, 0}};
const int Params::ROOK_PST[2][64] = {{-50, -37, -31, -27, -27, -31, -37, -50, -63, -37, -33, -27, -27, -33, -37, -63, -61, -36, -37, -35, -35, -37, -36, -61, -59, -45, -41, -32, -32, -41, -45, -59, -38, -30, -17, -15, -15, -17, -30, -38, -25, 0, -9, 2, 2, -9, 0, -25, -23, -30, -5, -1, -1, -5, -30, -23, -14, -9, -107, -70, -70, -107, -9, -14}, {-5, -7, 2, -4, -4, 2, -7, -5, -16, -10, -2, -2, -2, -2, -10, -16, -13, -7, 0, -2, -2, 0, -7, -13, -1, 7, 13, 8, 8, 13, 7, -1, 14, 16, 22, 18, 18, 22, 16, 14, 20, 24, 29, 28, 28, 29, 24, 20, 8, 11, 17, 18, 18, 17, 11, 8, 30, 30, 29, 25, 25, 29, 30, 30}};
const int Params::QUEEN_PST[2][64] = {{35, 35, 31, 43, 43, 31, 35, 35, 28, 45, 47, 48, 48, 47, 45, 28, 27, 42, 46, 36, 36, 46, 42, 27, 40, 44, 46, 46, 46, 46, 44, 40, 42, 45, 49, 57, 57, 49, 45, 42, 33, 52, 56, 54, 54, 56, 52, 33, 36, 17, 40, 43, 43, 40, 17, 36, 48, 47, 23, 5, 5, 23, 47, 48}, {-47, -61, -44, -29, -29, -44, -61, -47, -34, -30, -22, -5, -5, -22, -30, -34, -22, 6, 17, 8, 8, 17, 6, -22, 8, 27, 36, 37, 37, 36, 27, 8, 17, 48, 54, 68, 68, 54, 48, 17, 11, 46, 65, 81, 81, 65, 46, 11, 27, 48, 73, 85, 85, 73, 48, 27, 4, 20, 17, 21, 21, 17, 20, 4}};
const int Params::KING_PST[2][64] = {{44, 61, -1, 21, 21, -1, 61, 44, 84, 86, 31, 52, 52, 31, 86, 84, 2, 26, -53, -113, -113, -53, 26, 2, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -109, -108, -128, -128, -128, -128, -108, -109, 35, 35, -128, -128, -128, -128, 35, 35, -128, -108, 128, -128, -128, 128, -108, -128}, {-61, -62, -66, -71, -71, -66, -62, -61, -49, -57, -63, -63, -63, -63, -57, -49, -59, -63, -60, -59, -59, -60, -63, -59, -56, -53, -49, -46, -46, -49, -53, -56, -43, -38, -31, -30, -30, -31, -38, -43, -37, -26, -25, -25, -25, -25, -26, -37, -46, -36, -32, -34, -34, -32, -36, -46, -74, -50, -49, -47, -47, -49, -50, -74}};
const int Params::KNIGHT_MOBILITY[9] = {-73, -28, -6, 6, 15, 23, 26, 24, 17};
const int Params::BISHOP_MOBILITY[15] = {-55, -28, -14, -3, 8, 15, 19, 22, 27, 30, 30, 32, 42, 26, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-61, -37, -25, -20, -19, -13, -8, -0, 4, 7, 9, 12, 22, 30, 26}, {-66, -30, -10, 7, 23, 30, 30, 34, 40, 45, 49, 54, 56, 45, 36}};
const int Params::QUEEN_MOBILITY[2][24] = {{-63, -4, 8, 18, 24, 31, 38, 44, 49, 54, 56, 57, 60, 60, 64, 67, 65, 80, 92, 96, 96, 96, 96, 96}, {-96, -96, -79, -35, 12, 27, 42, 49, 56, 60, 65, 71, 78, 81, 85, 83, 89, 85, 85, 88, 90, 93, 84, 79}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-93, -55, -34, -22, -16};
const int Params::KNIGHT_OUTPOST[2][2] = {{24, 62}, {20, 33}};
const int Params::BISHOP_OUTPOST[2][2] = {{34, 63}, {35, 27}};
const int Params::PAWN_STORM[4][2] = {{0, 19},{9, 0},{32, 0},{3, 0}};


