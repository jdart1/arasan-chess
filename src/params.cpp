// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 18-Dec-2018 04:58:28 by tuner -c 24 -R 25 -n 150 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0681193

//

#include "params.h"

const int Params::RB_ADJUST[6] = {48, -6, 93, 45, 41, 77};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -10, -41, -32};
const int Params::QR_ADJUST[5] = {17, 65, 28, -32, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -271, -153};
const int Params::CASTLING[6] = {13, -38, -26, 0, 6, -20};
const int Params::KING_COVER[6][4] = {{7, 22, 12, 14},
{2, 7, 4, 5},
{-7, -22, -12, -15},
{-7, -22, -12, -14},
{-8, -25, -14, -16},
{-15, -46, -26, -30}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -227;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -182;
const int Params::MINOR_FOR_PAWNS = 63;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 38;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR = 12;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 42;
const int Params::ROOK_ATTACK_FACTOR = 60;
const int Params::ROOK_ATTACK_BOOST = 22;
const int Params::ROOK_ATTACK_BOOST2 = 41;
const int Params::QUEEN_ATTACK_FACTOR = 61;
const int Params::QUEEN_ATTACK_BOOST = 45;
const int Params::QUEEN_ATTACK_BOOST2 = 63;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 6;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 135;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 88;
const int Params::OWN_ROOK_KING_PROXIMITY = 35;
const int Params::OWN_QUEEN_KING_PROXIMITY = 17;
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
const int Params::TWO_ROOKS_ON_7TH_MID = 33;
const int Params::TWO_ROOKS_ON_7TH_END = 64;
const int Params::ROOK_ON_OPEN_FILE_MID = 34;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 21;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 14;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 63;
const int Params::QUEENING_SQUARE_CONTROL_END = 56;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -17;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 23, 53, 81, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={249, 212, 146};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 25, 27, 29, 31, 33, 35, 38, 40, 43, 46, 49, 52, 55, 58, 62, 66, 70, 74, 78, 82, 87, 92, 97, 102, 108, 113, 119, 125, 132, 138, 145, 152, 159, 167, 174, 182, 190, 199, 207, 216, 224, 233, 243, 252, 261, 270, 280, 290, 299, 309, 319, 328, 338, 348, 357, 367, 376, 386, 395, 404, 413, 422, 431, 439, 448, 456, 464, 472, 479, 487, 494, 501, 508, 514, 520, 526, 532, 538, 543, 548, 553, 558, 563, 567, 571, 575, 579, 583, 586, 590, 593, 596, 599, 602, 604, 607, 609, 611, 613, 616, 617, 619, 621, 623, 624, 626, 627, 628, 630, 631, 632, 633, 634, 635, 636, 637, 637, 638, 639, 639};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 0, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 35, 128, 187}, {0, 0, 0, 0, 25, 83, 160, 272}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 16, 96, 0}, {0, 0, 0, 0, 0, 16, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 5, 6, 26, 102, 206, 320}, {0, 0, 0, 0, 37, 89, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 28, 12, 36, 97, 192}, {0, 0, 0, 9, 10, 26, 61, 68}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-35, -23, -31, -8, -34, -2, -21, -25, -5, -29, -55, -19, 0, -18, 0, -40, -8, -29, -20, -60, 0}, {0, 0, 0, -4, -12, 0, 0, -10, -7, -19, -24, -6, -3, -11, -14, -17, -18, -29, -53, -41, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-58, -75, -52, -51, -44, -51, -58, -54, -27, -31, -32, -43, -8, 0, -4, -31, 0, 0, -67, -36, -83}, {-10, -3, 0, -7, -2, -2, -16, -8, -7, -5, -4, -27, -12, -10, 0, -51, -21, -13, -54, -38, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-42, 0, -16, -11, -11, -16, 0, -42}, {-42, -19, -25, -20, -20, -25, -19, -42}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -28, -1, -12, -12, -1, -28, 0}, {-88, -72, -38, -40, -40, -38, -72, -88}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -17, -23, -23, -17, 0, 0}, {-5, -9, -11, -17, -17, -11, -9, -5}};

const int Params::KNIGHT_PST[2][64] = {{-82, -24, -9, -3, -3, -9, -24, -82, -21, -21, -11, 9, 9, -11, -21, -21, -41, -14, 5, 30, 30, 5, -14, -41, -14, 3, 24, 26, 26, 24, 3, -14, -26, -8, 24, 19, 19, 24, -8, -26, -49, -44, 1, 6, 6, 1, -44, -49, -102, -82, -27, -18, -18, -27, -82, -102, -128, -128, -128, -128, -128, -128, -128, -128}, {-75, -37, -25, -28, -28, -25, -37, -75, -33, -22, -21, -12, -12, -21, -22, -33, -44, -25, -15, 7, 7, -15, -25, -44, -19, -7, 12, 21, 21, 12, -7, -19, -31, -21, -1, 3, 3, -1, -21, -31, -51, -34, -15, -10, -10, -15, -34, -51, -71, -47, -44, -29, -29, -44, -47, -71, -128, -78, -76, -58, -58, -76, -78, -128}};
const int Params::BISHOP_PST[2][64] = {{15, 45, 32, 30, 30, 32, 45, 15, 37, 48, 39, 32, 32, 39, 48, 37, 14, 41, 40, 33, 33, 40, 41, 14, 2, 11, 22, 42, 42, 22, 11, 2, -34, -9, -3, 25, 25, -3, -9, -34, 5, -25, -39, -25, -25, -39, -25, 5, -29, -64, -62, -95, -95, -62, -64, -29, -20, -84, -128, -128, -128, -128, -84, -20}, {-8, 5, 11, 6, 6, 11, 5, -8, 12, 15, 10, 17, 17, 10, 15, 12, 5, 17, 23, 26, 26, 23, 17, 5, 4, 13, 27, 32, 32, 27, 13, 4, -16, -2, -1, 8, 8, -1, -2, -16, 4, -9, -9, -9, -9, -9, -9, 4, -5, -10, -17, -21, -21, -17, -10, -5, -0, -2, -7, -26, -26, -7, -2, -0}};
const int Params::ROOK_PST[2][64] = {{-50, -37, -31, -27, -27, -31, -37, -50, -62, -37, -32, -27, -27, -32, -37, -62, -61, -36, -36, -35, -35, -36, -36, -61, -59, -44, -41, -32, -32, -41, -44, -59, -38, -30, -17, -15, -15, -17, -30, -38, -25, 0, -9, 3, 3, -9, 0, -25, -23, -29, -4, -1, -1, -4, -29, -23, -15, -8, -96, -67, -67, -96, -8, -15}, {-6, -7, 2, -4, -4, 2, -7, -6, -17, -11, -3, -3, -3, -3, -11, -17, -13, -7, -0, -3, -3, -0, -7, -13, -2, 7, 13, 8, 8, 13, 7, -2, 14, 16, 21, 17, 17, 21, 16, 14, 19, 23, 28, 27, 27, 28, 23, 19, 8, 11, 16, 18, 18, 16, 11, 8, 29, 29, 28, 25, 25, 28, 29, 29}};
const int Params::QUEEN_PST[2][64] = {{34, 34, 30, 42, 42, 30, 34, 34, 27, 44, 46, 47, 47, 46, 44, 27, 26, 40, 45, 35, 35, 45, 40, 26, 38, 43, 45, 45, 45, 45, 43, 38, 41, 44, 48, 56, 56, 48, 44, 41, 32, 52, 55, 52, 52, 55, 52, 32, 35, 16, 39, 41, 41, 39, 16, 35, 46, 47, 23, 8, 8, 23, 47, 46}, {-48, -61, -46, -30, -30, -46, -61, -48, -35, -31, -23, -6, -6, -23, -31, -35, -23, 5, 16, 7, 7, 16, 5, -23, 8, 27, 34, 36, 36, 34, 27, 8, 16, 47, 53, 68, 68, 53, 47, 16, 11, 45, 64, 80, 80, 64, 45, 11, 26, 47, 73, 84, 84, 73, 47, 26, 4, 20, 17, 20, 20, 17, 20, 4}};
const int Params::KING_PST[2][64] = {{43, 59, -2, 20, 20, -2, 59, 43, 81, 84, 29, 50, 50, 29, 84, 81, -8, 21, -56, -117, -117, -56, 21, -8, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -96, -107, -128, -128, -128, -128, -107, -96, -19, 29, -128, -128, -128, -128, 29, -19, -128, -128, 128, -128, -128, 128, -128, -128}, {-58, -59, -64, -68, -68, -64, -59, -58, -47, -55, -61, -61, -61, -61, -55, -47, -57, -61, -58, -56, -56, -58, -61, -57, -54, -51, -47, -44, -44, -47, -51, -54, -41, -36, -29, -28, -28, -29, -36, -41, -34, -24, -23, -23, -23, -23, -24, -34, -43, -34, -30, -32, -32, -30, -34, -43, -71, -48, -47, -45, -45, -47, -48, -71}};
const int Params::KNIGHT_MOBILITY[9] = {-72, -27, -6, 6, 15, 24, 27, 25, 17};
const int Params::BISHOP_MOBILITY[15] = {-55, -28, -13, -3, 8, 15, 19, 23, 27, 30, 30, 33, 43, 27, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-61, -37, -25, -20, -19, -13, -8, 0, 4, 7, 9, 12, 23, 29, 26}, {-67, -31, -10, 6, 22, 29, 29, 33, 40, 44, 49, 53, 55, 45, 36}};
const int Params::QUEEN_MOBILITY[2][24] = {{-64, -4, 9, 18, 24, 31, 38, 44, 49, 53, 56, 57, 59, 60, 64, 67, 65, 80, 93, 96, 96, 96, 96, 96}, {-96, -96, -82, -38, 10, 26, 40, 47, 54, 59, 63, 69, 76, 79, 83, 82, 87, 84, 83, 85, 89, 91, 82, 77}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-91, -53, -32, -20, -14};
const int Params::KNIGHT_OUTPOST[2][2] = {{24, 61}, {20, 33}};
const int Params::BISHOP_OUTPOST[2][2] = {{34, 63}, {36, 27}};
const int Params::PAWN_STORM[2][2] = {{32, 1}, {3, 0}};


