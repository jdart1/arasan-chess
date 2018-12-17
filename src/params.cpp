// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 16-Dec-2018 07:54:51 by tuner -n 150 -R 25 -c 24 /home/jdart/chess/data/epd/samples-2018-labeled.epd
// Final objective value: 0.0640658

//

#include "params.h"

const int Params::RB_ADJUST[6] = {11, -6, 61, 11, 25, 51};
const int Params::RBN_ADJUST[6] = {64, -19, 26, 6, -53, -32};
const int Params::QR_ADJUST[5] = {51, 106, 109, 56, -83};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -279, -168};
const int Params::CASTLING[6] = {13, -25, 0, 0, 0, 0};
const int Params::KING_COVER[6][4] = {{11, 45, 30, 22},
{1, 2, 2, 1},
{-7, -30, -20, -15},
{-9, -36, -24, -18},
{-8, -32, -22, -16},
{-12, -48, -33, -24}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 74;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -215;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -173;
const int Params::MINOR_FOR_PAWNS = 60;
const int Params::ENDGAME_PAWN_ADVANTAGE = 31;
const int Params::PAWN_ENDGAME1 = 48;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::MINOR_ATTACK_FACTOR = 43;
const int Params::MINOR_ATTACK_BOOST = 51;
const int Params::ROOK_ATTACK_FACTOR = 66;
const int Params::ROOK_ATTACK_BOOST = 28;
const int Params::ROOK_ATTACK_BOOST2 = 62;
const int Params::QUEEN_ATTACK_FACTOR = 64;
const int Params::QUEEN_ATTACK_BOOST = 57;
const int Params::QUEEN_ATTACK_BOOST2 = 79;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 98;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 96;
const int Params::OWN_ROOK_KING_PROXIMITY = 45;
const int Params::OWN_QUEEN_KING_PROXIMITY = 24;
const int Params::PAWN_THREAT_ON_PIECE_MID = 7;
const int Params::PAWN_THREAT_ON_PIECE_END = 14;
const int Params::PIECE_THREAT_MM_MID = 38;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 96;
const int Params::PIECE_THREAT_MM_END = 40;
const int Params::PIECE_THREAT_MR_END = 96;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 4;
const int Params::MINOR_PAWN_THREAT_END = 22;
const int Params::PIECE_THREAT_RM_MID = 21;
const int Params::PIECE_THREAT_RR_MID = 32;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 22;
const int Params::PIECE_THREAT_RR_END = 15;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 9;
const int Params::ROOK_PAWN_THREAT_END = 29;
const int Params::ENDGAME_KING_THREAT = 44;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 48;
const int Params::BISHOP_PAIR_END = 42;
const int Params::BISHOP_PAWN_PLACEMENT_END = 0;
const int Params::BAD_BISHOP_MID = -4;
const int Params::BAD_BISHOP_END = -3;
const int Params::CENTER_PAWN_BLOCK = -15;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -1;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -18;
const int Params::WEAK_ON_OPEN_FILE_END = -19;
const int Params::SPACE = 2;
const int Params::PAWN_CENTER_SCORE_MID = 0;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 25;
const int Params::TWO_ROOKS_ON_7TH_MID = 51;
const int Params::TWO_ROOKS_ON_7TH_END = 82;
const int Params::ROOK_ON_OPEN_FILE_MID = 34;
const int Params::ROOK_ON_OPEN_FILE_END = 9;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 26;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 11;
const int Params::KING_OWN_PAWN_DISTANCE = 9;
const int Params::KING_OPP_PAWN_DISTANCE = 5;
const int Params::QUEENING_SQUARE_CONTROL_MID = 77;
const int Params::QUEENING_SQUARE_CONTROL_END = 49;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -15;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -65;
const int Params::WRONG_COLOR_BISHOP = -1;
const int Params::SIDE_PROTECTED_PAWN = -4;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 1, 21, 46, 66, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={300, 268, 159};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {1, 1, 2, 2, 2, 3, 4, 4, 5, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 19, 20, 21, 23, 24, 26, 28, 30, 31, 33, 35, 38, 40, 42, 45, 47, 50, 53, 56, 59, 62, 66, 69, 73, 77, 81, 85, 90, 94, 99, 104, 109, 114, 119, 125, 131, 137, 143, 149, 156, 163, 170, 177, 184, 192, 200, 207, 215, 224, 232, 240, 249, 258, 267, 276, 285, 294, 303, 313, 322, 331, 341, 350, 359, 369, 378, 388, 397, 406, 415, 424, 433, 442, 450, 459, 467, 475, 483, 491, 499, 507, 514, 521, 528, 535, 542, 548, 554, 560, 566, 572, 577, 582, 588, 592, 597, 602, 606, 610, 614, 618, 622, 625, 629, 632, 635, 638, 641, 644, 647, 649, 651, 654, 656, 658, 660, 662, 664, 665, 667, 669, 670, 672, 673, 674, 675, 677};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 72, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 1, 27, 125, 177}, {0, 0, 0, 0, 19, 72, 160, 288}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {69, 72, 71, 64, 64, 71, 72, 69};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 16, 96, 0}, {0, 0, 0, 0, 0, 9, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 16, 8, 2, 102, 140, 320}, {0, 0, 0, 0, 31, 84, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 38, 22, 53, 89, 192}, {0, 0, 0, 9, 11, 30, 45, 108}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-35, -29, -15, 0, -54, -8, -11, -28, 0, -38, -83, -15, -7, -61, -36, -36, -26, -71, -26, -83, 0}, {0, -6, 0, -4, -14, 0, -6, -10, -4, -25, -33, -10, -2, -16, -22, -16, -19, -30, -50, -40, -57}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-43, -75, -37, -42, -30, -51, -47, -39, -26, -31, -28, -32, -11, 0, -9, -32, -8, 0, -78, -55, -83}, {-15, 0, 0, -14, -5, -14, -20, -10, 0, -7, 0, -22, -10, -9, 0, -49, -18, -16, -51, -48, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-39, -9, -18, -1, -1, -18, -9, -39}, {-32, -15, -19, -24, -24, -19, -15, -32}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, 0, -8, 0, 0, -8, 0, 0}, {-96, -75, -40, -63, -63, -40, -75, -96}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -17, -17, -17, -17, 0, 0}, {-6, -5, -7, -14, -14, -7, -5, -6}};

const int Params::KNIGHT_PST[2][64] = {{-47, -18, 10, 22, 22, 10, -18, -47, 3, 2, 29, 28, 28, 29, 2, 3, -9, 9, 34, 48, 48, 34, 9, -9, -3, 22, 49, 54, 54, 49, 22, -3, -4, 20, 42, 39, 39, 42, 20, -4, -5, -24, 24, 33, 33, 24, -24, -5, -75, -61, -1, 22, 22, -1, -61, -75, -128, -128, -128, -128, -128, -128, -128, -128}, {-103, -58, -23, -28, -28, -23, -58, -103, -42, -22, -21, -8, -8, -21, -22, -42, -40, -20, -7, 12, 12, -7, -20, -40, -31, -7, 6, 20, 20, 6, -7, -31, -52, -32, -10, -2, -2, -10, -32, -52, -71, -51, -29, -21, -21, -29, -51, -71, -85, -64, -64, -41, -41, -64, -64, -85, -128, -120, -106, -93, -93, -106, -120, -128}};
const int Params::BISHOP_PST[2][64] = {{47, 59, 53, 52, 52, 53, 59, 47, 55, 69, 59, 50, 50, 59, 69, 55, 34, 57, 59, 54, 54, 59, 57, 34, 26, 19, 44, 55, 55, 44, 19, 26, -14, -6, 4, 36, 36, 4, -6, -14, 6, -18, -47, -15, -15, -47, -18, 6, 13, -72, -63, -67, -67, -63, -72, 13, -94, -128, -128, -128, -128, -128, -128, -94}, {-13, -7, 12, 4, 4, 12, -7, -13, 23, 22, 16, 23, 23, 16, 22, 23, 5, 19, 27, 30, 30, 27, 19, 5, 1, 10, 26, 35, 35, 26, 10, 1, -21, -2, -4, 8, 8, -4, -2, -21, -7, -19, -15, -15, -15, -15, -19, -7, -17, -21, -20, -26, -26, -20, -21, -17, -15, -19, -26, -37, -37, -26, -19, -15}};
const int Params::ROOK_PST[2][64] = {{6, 16, 24, 30, 30, 24, 16, 6, -5, 10, 18, 23, 23, 18, 10, -5, -9, 9, 15, 20, 20, 15, 9, -9, -1, -1, 8, 18, 18, 8, -1, -1, 23, 10, 42, 20, 20, 42, 10, 23, 12, 40, 22, 28, 28, 22, 40, 12, 30, 35, 38, 38, 38, 38, 35, 30, 30, 36, -128, -128, -128, -128, 36, 30}, {9, 11, 21, 18, 18, 21, 11, 9, 0, 12, 19, 12, 12, 19, 12, 0, 5, 10, 15, 14, 14, 15, 10, 5, 12, 19, 25, 19, 19, 25, 19, 12, 19, 25, 30, 28, 28, 30, 25, 19, 24, 27, 34, 33, 33, 34, 27, 24, 17, 19, 23, 23, 23, 23, 19, 17, 32, 31, 26, 24, 24, 26, 31, 32}};
const int Params::QUEEN_PST[2][64] = {{36, 35, 28, 54, 54, 28, 35, 36, 27, 57, 49, 50, 50, 49, 57, 27, 34, 48, 49, 40, 40, 49, 48, 34, 46, 51, 42, 48, 48, 42, 51, 46, 46, 51, 50, 66, 66, 50, 51, 46, 23, 51, 73, 52, 52, 73, 51, 23, 56, 23, 41, 58, 58, 41, 23, 56, 36, 64, -81, -32, -32, -81, 64, 36}, {-29, -33, -10, -2, -2, -10, -33, -29, -18, 2, 5, 30, 30, 5, 2, -18, -9, 15, 36, 24, 24, 36, 15, -9, 8, 35, 55, 52, 52, 55, 35, 8, 11, 50, 61, 71, 71, 61, 50, 11, 8, 40, 66, 83, 83, 66, 40, 8, 25, 47, 67, 83, 83, 67, 47, 25, -13, 10, 9, 26, 26, 9, 10, -13}};
const int Params::KING_PST[2][64] = {{45, 72, -24, -13, -13, -24, 72, 45, 91, 91, 8, 23, 23, 8, 91, 91, -14, 1, -99, -128, -128, -99, 1, -14, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -83, -128, -128, -128, -128, -83, -128, -63, -128, -128, -128, -128, -128, -128, -63, -128, 99, 128, 128, 128, 128, 99, -128}, {-30, -30, -33, -36, -36, -33, -30, -30, -18, -22, -27, -27, -27, -27, -22, -18, -22, -26, -23, -22, -22, -23, -26, -22, -22, -20, -17, -14, -14, -17, -20, -22, -13, -12, -6, -6, -6, -6, -12, -13, -10, -5, -6, -5, -5, -6, -5, -10, -17, -14, -10, -12, -12, -10, -14, -17, -36, -26, -25, -25, -25, -25, -26, -36}};

const int Params::KNIGHT_MOBILITY[9] = {-39, -9, 6, 16, 18, 24, 22, 21, 7};
const int Params::BISHOP_MOBILITY[15] = {-48, -21, -9, -2, 6, 13, 17, 16, 19, 21, 21, 20, 29, 18, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-68, -29, -18, -6, -6, -3, -1, 7, 12, 13, 12, 13, 16, 31, 5}, {-70, -19, 5, 16, 29, 37, 35, 36, 42, 43, 47, 52, 54, 46, 38}};
const int Params::QUEEN_MOBILITY[2][24] = {{-92, 10, 13, 24, 25, 36, 37, 45, 50, 53, 55, 52, 59, 62, 64, 59, 65, 69, 86, 96, 96, 96, 96, 96}, {-96, -96, -44, -3, 32, 61, 63, 63, 67, 65, 77, 74, 84, 84, 88, 89, 96, 88, 95, 93, 90, 91, 88, 85}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-62, -29, -9, 3, 10};

const int Params::KNIGHT_OUTPOST[2][2] = {{13, 47}, {20, 32}};
const int Params::BISHOP_OUTPOST[2][2] = {{38, 70}, {35, 28}};

const int Params::PAWN_ATTACK_FACTOR[2][4] = {{0, 40, 35, 0}, {15, 40, 0, 0}};


