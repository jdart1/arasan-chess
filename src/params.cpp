// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 04-Sep-2019 07:45:05 by tuner -n 175 -c 16 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0675116

//

#include "params.h"

const int Params::RB_ADJUST[6] = {44, -6, 91, 41, 37, 67};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -30, -62, -69};
const int Params::QR_ADJUST[5] = {21, 77, 32, -33, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -249, -131};
const int Params::MINOR_FOR_PAWNS[6] = {51, 60, 73, 93, 115, 131};
const int Params::CASTLING[6] = {13, -38, -28, 0, 4, -22};
const int Params::KING_COVER[6][4] = {{8, 24, 14, 16},
{2, 6, 3, 4},
{-9, -26, -15, -17},
{-8, -25, -15, -17},
{-10, -31, -18, -20},
{-17, -51, -30, -34}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 94;
const int Params::PIN_MULTIPLIER_END = 83;
const int Params::KRMINOR_VS_R_NO_PAWNS = -200;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -150;
const int Params::ENDGAME_PAWN_ADVANTAGE = 31;
const int Params::PAWN_ENDGAME1 = 41;
const int Params::PAWN_ATTACK_FACTOR = 12;
const int Params::MINOR_ATTACK_FACTOR = 47;
const int Params::MINOR_ATTACK_BOOST = 22;
const int Params::ROOK_ATTACK_FACTOR = 58;
const int Params::ROOK_ATTACK_BOOST = 21;
const int Params::QUEEN_ATTACK_FACTOR = 55;
const int Params::QUEEN_ATTACK_BOOST = 40;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 111;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 7;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 96;
const int Params::OWN_ROOK_KING_PROXIMITY = 32;
const int Params::OWN_QUEEN_KING_PROXIMITY = 19;
const int Params::PAWN_THREAT_ON_PIECE_MID = 83;
const int Params::PAWN_THREAT_ON_PIECE_END = 72;
const int Params::PIECE_THREAT_MM_MID = 34;
const int Params::PIECE_THREAT_MR_MID = 90;
const int Params::PIECE_THREAT_MQ_MID = 78;
const int Params::PIECE_THREAT_MM_END = 32;
const int Params::PIECE_THREAT_MR_END = 74;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 12;
const int Params::MINOR_PAWN_THREAT_END = 25;
const int Params::PIECE_THREAT_RM_MID = 24;
const int Params::PIECE_THREAT_RR_MID = 13;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 34;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 42;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 59;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -22;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -4;
const int Params::WEAK_PAWN_END = -1;
const int Params::WEAK_ON_OPEN_FILE_MID = -21;
const int Params::WEAK_ON_OPEN_FILE_END = -14;
const int Params::SPACE = 5;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 25;
const int Params::TWO_ROOKS_ON_7TH_MID = 40;
const int Params::TWO_ROOKS_ON_7TH_END = 69;
const int Params::ROOK_ON_OPEN_FILE_MID = 34;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 3;
const int Params::ROOK_BEHIND_PP_END = 24;
const int Params::QUEEN_OUT = -29;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 14;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 61;
const int Params::QUEENING_SQUARE_CONTROL_END = 61;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -18;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -42;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 27, 58, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={226, 189, 149};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-2, -2, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 20, 21, 23, 25, 26, 28, 30, 33, 35, 37, 40, 43, 45, 48, 51, 55, 58, 62, 66, 70, 74, 78, 83, 87, 92, 98, 103, 109, 114, 120, 127, 133, 140, 147, 154, 161, 169, 177, 185, 193, 202, 210, 219, 228, 237, 246, 256, 265, 275, 284, 294, 304, 314, 323, 333, 343, 353, 362, 372, 381, 391, 400, 409, 418, 427, 435, 444, 452, 460, 468, 475, 483, 490, 497, 504, 510, 516, 522, 528, 534, 539, 544, 549, 554, 558, 563, 567, 571, 575, 578, 582, 585, 588, 591, 594, 597, 599, 602, 604, 606, 608, 610, 612, 614, 615, 617, 618, 620, 621, 622, 623, 625, 626, 627, 628, 628, 629, 630};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 1, 35, 124, 190}, {0, 0, 2, 0, 27, 85, 160, 264}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 0, 18, 96, 0}, {0, 0, 0, 0, 1, 18, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 7, 18, 102, 174, 315}, {0, 0, 0, 0, 36, 87, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 30, 12, 40, 98, 192}, {0, 0, 0, 10, 11, 27, 60, 77}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-32, -25, -27, -6, -30, -12, -18, -24, 0, -25, -33, -16, 0, -13, 0, -33, -7, -12, -10, -74, 0}, {0, 0, 0, -3, -9, -3, 0, -8, -5, -14, -22, -7, -3, -12, -15, -18, -17, -26, -50, -38, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-56, -68, -43, -40, -36, -45, -50, -42, -24, -28, -29, -36, -2, 0, -3, -26, -0, -1, -56, -35, -83}, {-8, -2, 0, -4, -4, -2, -11, -4, -2, -6, -4, -24, -11, -9, -1, -48, -23, -15, -52, -35, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-38, 0, -13, -9, -9, -13, 0, -38}, {-40, -16, -22, -17, -17, -22, -16, -40}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -21, 0, 0, 0, 0, -21, 0}, {-63, -65, -34, -35, -35, -34, -65, -63}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -1, -18, -23, -23, -18, -1, 0}, {-6, -10, -12, -18, -18, -12, -10, -6}};

const int Params::KNIGHT_PST[2][64] = {{-87, -32, -16, -11, -11, -16, -32, -87, -29, -29, -20, 1, 1, -20, -29, -29, -45, -17, 2, 25, 25, 2, -17, -45, -18, 8, 22, 22, 22, 22, 8, -18, -26, -9, 23, 15, 15, 23, -9, -26, -54, -45, -7, 1, 1, -7, -45, -54, -99, -77, -31, -24, -24, -31, -77, -99, -128, -128, -128, -128, -128, -128, -128, -128}, {-86, -48, -36, -37, -37, -36, -48, -86, -41, -30, -30, -22, -22, -30, -30, -41, -45, -26, -17, 3, 3, -17, -26, -45, -21, -7, 10, 19, 19, 10, -7, -21, -28, -18, 3, 7, 7, 3, -18, -28, -46, -29, -12, -6, -6, -12, -29, -46, -66, -43, -40, -28, -28, -40, -43, -66, -128, -68, -67, -57, -57, -67, -68, -128}};
const int Params::BISHOP_PST[2][64] = {{8, 36, 23, 21, 21, 23, 36, 8, 27, 39, 31, 23, 23, 31, 39, 27, 7, 36, 35, 28, 28, 35, 36, 7, 2, 8, 18, 37, 37, 18, 8, 2, -26, -7, -2, 25, 25, -2, -7, -26, 13, -18, -36, -24, -24, -36, -18, 13, -28, -61, -58, -86, -86, -58, -61, -28, -19, -94, -128, -128, -128, -128, -94, -19}, {-19, -5, -2, -7, -7, -2, -5, -19, 1, 3, -0, 5, 5, -0, 3, 1, -4, 14, 17, 20, 20, 17, 14, -4, -3, 10, 23, 28, 28, 23, 10, -3, -16, -0, 1, 11, 11, 1, -0, -16, 5, -8, -6, -7, -7, -6, -8, 5, -3, -7, -17, -19, -19, -17, -7, -3, 1, -0, -4, -25, -25, -4, -0, 1}};
const int Params::ROOK_PST[2][64] = {{-66, -53, -47, -43, -43, -47, -53, -66, -78, -53, -48, -43, -43, -48, -53, -78, -74, -49, -49, -48, -48, -49, -49, -74, -71, -56, -53, -45, -45, -53, -56, -71, -50, -41, -27, -25, -25, -27, -41, -50, -39, -15, -21, -8, -8, -21, -15, -39, -39, -46, -21, -19, -19, -21, -46, -39, -32, -30, -117, -88, -88, -117, -30, -32}, {-29, -31, -21, -27, -27, -21, -31, -29, -39, -33, -25, -26, -26, -25, -33, -39, -33, -24, -19, -22, -22, -19, -24, -33, -20, -10, -4, -10, -10, -4, -10, -20, -6, -2, 3, 0, 0, 3, -2, -6, -1, 5, 8, 6, 6, 8, 5, -1, -11, -8, -2, -2, -2, -2, -8, -11, 9, 7, 8, 3, 3, 8, 7, 9}};
const int Params::QUEEN_PST[2][64] = {{-6, -6, -8, 5, 5, -8, -6, -6, -11, 5, 7, 10, 10, 7, 5, -11, -9, 5, 11, 1, 1, 11, 5, -9, 3, 10, 10, 10, 10, 10, 10, 3, 8, 12, 16, 18, 18, 16, 12, 8, -11, 13, 21, 12, 12, 21, 13, -11, -3, -19, -2, -0, -0, -2, -19, -3, 6, 2, -19, -46, -46, -19, 2, 6}, {-103, -112, -96, -83, -83, -96, -112, -103, -95, -84, -75, -61, -61, -75, -84, -95, -76, -43, -34, -45, -45, -34, -43, -76, -47, -23, -12, -11, -11, -12, -23, -47, -39, -3, 7, 21, 21, 7, -3, -39, -41, -10, 14, 30, 30, 14, -10, -41, -28, -6, 19, 29, 29, 19, -6, -28, -61, -48, -46, -36, -36, -46, -48, -61}};
const int Params::KING_PST[2][64] = {{19, 35, -22, -4, -4, -22, 35, 19, 57, 57, 4, 26, 26, 4, 57, 57, -31, -7, -71, -128, -128, -71, -7, -31, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -96, -128, -128, -128, -128, -96, -128, 118, -32, -128, -128, -128, -128, -32, 118, -101, -128, 128, -128, -128, 128, -128, -101}, {-29, -26, -31, -36, -36, -31, -26, -29, -14, -20, -27, -26, -26, -27, -20, -14, -22, -23, -21, -20, -20, -21, -23, -22, -20, -14, -9, -6, -6, -9, -14, -20, -6, 3, 10, 11, 11, 10, 3, -6, 3, 16, 18, 17, 17, 18, 16, 3, -9, 6, 9, 7, 7, 9, 6, -9, -36, -13, -10, -9, -9, -10, -13, -36}};
const int Params::KNIGHT_MOBILITY[9] = {-73, -30, -8, 4, 13, 22, 25, 23, 16};
const int Params::BISHOP_MOBILITY[15] = {-54, -28, -14, -3, 7, 14, 18, 20, 25, 27, 27, 28, 38, 22, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-73, -49, -37, -32, -31, -25, -20, -12, -8, -5, -4, 1, 11, 19, 7}, {-96, -65, -45, -28, -13, -7, -7, -3, 3, 7, 11, 15, 17, 7, -1}};
const int Params::QUEEN_MOBILITY[2][24] = {{-94, -34, -21, -9, -3, 3, 9, 15, 20, 24, 26, 28, 30, 31, 35, 36, 37, 51, 62, 73, 88, 96, 96, 96}, {-96, -96, -96, -96, -86, -69, -52, -50, -43, -39, -35, -31, -28, -25, -23, -24, -24, -26, -33, -33, -31, -33, -42, -50}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-78, -45, -26, -15, -12};
const int Params::KNIGHT_OUTPOST[2][2] = {{21, 60}, {10, 33}};
const int Params::BISHOP_OUTPOST[2][2] = {{22, 55}, {26, 28}};
const int Params::PAWN_STORM[4][2] = {{0, 20},{10, 2},{34, 1},{4, 0}};

