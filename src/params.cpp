// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 24-Jun-2019 05:48:22 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0679678

//

#include "params.h"

const int Params::RB_ADJUST[6] = {47, -6, 93, 44, 40, 75};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -30, -57, -56};
const int Params::QR_ADJUST[5] = {17, 74, 28, -40, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -250, -129};
const int Params::MINOR_FOR_PAWNS[3] = {68, 75, 115};
const int Params::CASTLING[6] = {13, -38, -27, 0, 5, -21};
const int Params::KING_COVER[6][4] = {{8, 23, 13, 15},
{3, 8, 4, 5},
{-7, -21, -12, -14},
{-7, -21, -12, -14},
{-8, -26, -15, -17},
{-15, -45, -26, -29}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 86;
const int Params::KRMINOR_VS_R_NO_PAWNS = -197;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -146;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 40;
const int Params::PAWN_ATTACK_FACTOR = 15;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 42;
const int Params::ROOK_ATTACK_FACTOR = 60;
const int Params::ROOK_ATTACK_BOOST = 23;
const int Params::ROOK_ATTACK_BOOST2 = 37;
const int Params::QUEEN_ATTACK_FACTOR = 60;
const int Params::QUEEN_ATTACK_BOOST = 46;
const int Params::QUEEN_ATTACK_BOOST2 = 61;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 6;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 134;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 87;
const int Params::OWN_ROOK_KING_PROXIMITY = 35;
const int Params::OWN_QUEEN_KING_PROXIMITY = 17;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 38;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 79;
const int Params::PIECE_THREAT_MM_END = 31;
const int Params::PIECE_THREAT_MR_END = 77;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 11;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 22;
const int Params::PIECE_THREAT_RR_MID = 11;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 33;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 15;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 40;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 38;
const int Params::BISHOP_PAIR_END = 60;
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
const int Params::TWO_ROOKS_ON_7TH_MID = 39;
const int Params::TWO_ROOKS_ON_7TH_END = 70;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 25;
const int Params::QUEEN_OUT = -29;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 14;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 58;
const int Params::QUEENING_SQUARE_CONTROL_END = 60;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -16;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::SIDE_PROTECTED_PAWN = -5;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 27, 58, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={224, 193, 151};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 25, 27, 29, 31, 33, 35, 37, 40, 42, 45, 48, 51, 54, 57, 61, 64, 68, 72, 76, 81, 85, 90, 95, 100, 105, 111, 116, 122, 129, 135, 142, 148, 155, 163, 170, 178, 185, 194, 202, 210, 219, 227, 236, 245, 255, 264, 273, 283, 292, 302, 311, 321, 331, 340, 350, 360, 369, 379, 388, 397, 407, 416, 425, 433, 442, 450, 459, 467, 475, 482, 490, 497, 504, 511, 518, 524, 530, 536, 542, 547, 553, 558, 563, 568, 572, 576, 581, 585, 588, 592, 596, 599, 602, 605, 608, 611, 613, 616, 618, 620, 623, 625, 627, 628, 630, 632, 633, 635, 636, 637, 639, 640, 641, 642, 643, 644, 645, 646, 647, 647, 648};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 1, 38, 128, 193}, {0, 0, 2, 0, 28, 85, 160, 264}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 18, 96, 0}, {0, 0, 0, 0, 1, 18, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 8, 23, 102, 187, 320}, {0, 0, 0, 0, 36, 90, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 27, 13, 38, 97, 185}, {0, 0, 0, 10, 11, 27, 61, 76}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-31, -21, -27, -3, -31, -11, -16, -22, 0, -23, -45, -15, 0, -14, -6, -36, -8, -17, -17, -66, 0}, {0, 0, 0, -1, -9, -4, 0, -8, -5, -16, -22, -7, -3, -11, -15, -18, -16, -26, -51, -38, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-59, -71, -48, -45, -43, -47, -56, -49, -25, -29, -30, -42, -6, 0, -2, -32, 0, -0, -62, -35, -83}, {-10, -4, 0, -7, -3, -1, -15, -8, -5, -4, -4, -28, -13, -8, -0, -52, -23, -14, -52, -36, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-40, 0, -15, -9, -9, -15, 0, -40}, {-40, -17, -23, -18, -18, -23, -17, -40}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -20, 0, -2, -2, 0, -20, 0}, {-75, -69, -36, -34, -34, -36, -69, -75}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -0, -18, -24, -24, -18, -0, 0}, {-6, -10, -12, -18, -18, -12, -10, -6}};

const int Params::KNIGHT_PST[2][64] = {{-82, -25, -10, -4, -4, -10, -25, -82, -24, -22, -13, 8, 8, -13, -22, -24, -41, -15, 4, 29, 29, 4, -15, -41, -15, 2, 23, 25, 25, 23, 2, -15, -27, -9, 23, 18, 18, 23, -9, -27, -52, -45, -3, 4, 4, -3, -45, -52, -101, -78, -28, -26, -26, -28, -78, -101, -128, -128, -128, -128, -128, -128, -128, -128}, {-81, -44, -32, -34, -34, -32, -44, -81, -38, -27, -26, -18, -18, -26, -27, -38, -48, -30, -21, 2, 2, -21, -30, -48, -24, -12, 6, 16, 16, 6, -12, -24, -35, -26, -5, -1, -1, -5, -26, -35, -55, -36, -19, -14, -14, -19, -36, -55, -74, -50, -47, -35, -35, -47, -50, -74, -128, -75, -76, -63, -63, -76, -75, -128}};
const int Params::BISHOP_PST[2][64] = {{13, 41, 29, 27, 27, 29, 41, 13, 34, 45, 36, 29, 29, 36, 45, 34, 11, 39, 38, 31, 31, 38, 39, 11, -0, 8, 19, 39, 39, 19, 8, -0, -38, -12, -6, 21, 21, -6, -12, -38, 2, -29, -44, -30, -30, -44, -29, 2, -33, -66, -63, -101, -101, -63, -66, -33, -24, -100, -128, -128, -128, -128, -100, -24}, {-17, -5, 1, -5, -5, 1, -5, -17, 3, 4, 0, 7, 7, 0, 4, 3, -4, 8, 13, 16, 16, 13, 8, -4, -5, 4, 17, 22, 22, 17, 4, -5, -24, -10, -10, 0, 0, -10, -10, -24, -3, -17, -16, -17, -17, -16, -17, -3, -11, -17, -25, -27, -27, -25, -17, -11, -6, -8, -13, -33, -33, -13, -8, -6}};
const int Params::ROOK_PST[2][64] = {{-68, -55, -50, -46, -46, -50, -55, -68, -81, -56, -52, -46, -46, -52, -56, -81, -80, -56, -56, -54, -54, -56, -56, -80, -77, -64, -60, -51, -51, -60, -64, -77, -57, -49, -36, -33, -33, -36, -49, -57, -44, -19, -27, -14, -14, -27, -19, -44, -41, -46, -22, -20, -20, -22, -46, -41, -35, -31, -128, -102, -102, -128, -31, -35}, {-30, -32, -22, -28, -28, -22, -32, -30, -41, -35, -27, -28, -28, -27, -35, -41, -37, -32, -25, -28, -28, -25, -32, -37, -25, -18, -12, -17, -17, -12, -18, -25, -10, -8, -3, -6, -6, -3, -8, -10, -5, 0, 5, 3, 3, 5, 0, -5, -15, -12, -6, -5, -5, -6, -12, -15, 7, 5, 4, 1, 1, 4, 5, 7}};
const int Params::QUEEN_PST[2][64] = {{-7, -7, -10, 1, 1, -10, -7, -7, -14, 3, 5, 7, 7, 5, 3, -14, -14, -1, 4, -6, -6, 4, -1, -14, -2, 2, 3, 3, 3, 3, 2, -2, 1, 3, 7, 13, 13, 7, 3, 1, -7, 13, 17, 12, 12, 17, 13, -7, -6, -22, -1, -1, -1, -1, -22, -6, 4, 6, -21, -32, -32, -21, 6, 4}, {-103, -115, -100, -83, -83, -100, -115, -103, -97, -87, -80, -63, -63, -80, -87, -97, -80, -52, -41, -50, -50, -41, -52, -80, -51, -30, -20, -19, -19, -20, -30, -51, -42, -9, -0, 14, 14, -0, -9, -42, -45, -14, 10, 24, 24, 10, -14, -45, -30, -9, 15, 25, 25, 15, -9, -30, -69, -53, -53, -46, -46, -53, -53, -69}};
const int Params::KING_PST[2][64] = {{39, 56, -5, 16, 16, -5, 56, 39, 80, 81, 27, 50, 50, 27, 81, 80, -3, 22, -50, -114, -114, -50, 22, -3, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -108, -71, -128, -128, -128, -128, -71, -108, 128, -13, -128, -128, -128, -128, -13, 128, -112, -128, 128, 91, 91, 128, -128, -112}, {-25, -22, -26, -31, -31, -26, -22, -25, -10, -16, -22, -22, -22, -22, -16, -10, -20, -22, -18, -16, -16, -18, -22, -20, -17, -12, -7, -3, -3, -7, -12, -17, -3, 5, 13, 14, 14, 13, 5, -3, 5, 18, 20, 20, 20, 20, 18, 5, -6, 9, 11, 10, 10, 11, 9, -6, -34, -10, -7, -7, -7, -7, -10, -34}};
const int Params::KNIGHT_MOBILITY[9] = {-77, -32, -10, 1, 10, 19, 22, 21, 13};
const int Params::BISHOP_MOBILITY[15] = {-57, -30, -16, -5, 6, 13, 17, 20, 25, 28, 29, 32, 41, 25, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-77, -53, -41, -37, -36, -30, -25, -17, -13, -10, -8, -5, 6, 14, 5}, {-96, -66, -46, -29, -13, -7, -7, -3, 3, 7, 12, 16, 18, 7, 1}};
const int Params::QUEEN_MOBILITY[2][24] = {{-96, -41, -28, -17, -10, -4, 4, 9, 15, 19, 21, 22, 25, 26, 30, 33, 33, 48, 58, 68, 92, 88, 96, 96}, {-96, -96, -96, -96, -87, -67, -54, -48, -39, -36, -33, -27, -24, -21, -18, -22, -19, -22, -28, -28, -26, -28, -36, -43}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-80, -43, -24, -13, -10};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 62}, {19, 31}};
const int Params::BISHOP_OUTPOST[2][2] = {{36, 64}, {35, 26}};
const int Params::PAWN_STORM[4][2] = {{0, 22},{9, 1},{33, 0},{4, 0}};

