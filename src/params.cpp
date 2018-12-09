// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 08-Dec-2018 01:31:12 by tuner -n 175 -c 24 -R 25 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0680893

//

#include "params.h"

const int Params::RB_ADJUST[6] = {48, -6, 93, 45, 41, 76};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -11, -41, -32};
const int Params::QR_ADJUST[5] = {17, 65, 27, -31, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -271, -153};
const int Params::CASTLING[6] = {13, -38, -27, 0, 5, -21};
const int Params::KING_COVER[6][4] = {{9, 27, 16, 18},
{2, 7, 4, 5},
{-8, -24, -14, -16},
{-9, -27, -16, -18},
{-9, -26, -16, -17},
{-16, -48, -29, -31}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 98;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -228;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -182;
const int Params::MINOR_FOR_PAWNS = 64;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 38;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 4;
const int Params::PAWN_ATTACK_FACTOR2 = 4;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 43;
const int Params::ROOK_ATTACK_FACTOR = 62;
const int Params::ROOK_ATTACK_BOOST = 23;
const int Params::ROOK_ATTACK_BOOST2 = 43;
const int Params::QUEEN_ATTACK_FACTOR = 63;
const int Params::QUEEN_ATTACK_BOOST = 47;
const int Params::QUEEN_ATTACK_BOOST2 = 65;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 6;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 126;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 86;
const int Params::OWN_ROOK_KING_PROXIMITY = 35;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 37;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 80;
const int Params::PIECE_THREAT_MM_END = 33;
const int Params::PIECE_THREAT_MR_END = 85;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 10;
const int Params::MINOR_PAWN_THREAT_END = 24;
const int Params::PIECE_THREAT_RM_MID = 22;
const int Params::PIECE_THREAT_RR_MID = 11;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 34;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 16;
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
const int Params::WEAK_PAWN_MID = -4;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -21;
const int Params::WEAK_ON_OPEN_FILE_END = -17;
const int Params::SPACE = 5;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 28;
const int Params::TWO_ROOKS_ON_7TH_MID = 28;
const int Params::TWO_ROOKS_ON_7TH_END = 64;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 21;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 61;
const int Params::QUEENING_SQUARE_CONTROL_END = 56;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -16;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -41;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 24, 54, 81, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={248, 212, 146};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 20, 22, 23, 25, 27, 29, 31, 33, 35, 38, 40, 43, 46, 48, 51, 55, 58, 61, 65, 69, 73, 77, 82, 86, 91, 96, 101, 106, 112, 118, 124, 130, 136, 143, 150, 157, 164, 172, 180, 187, 196, 204, 212, 221, 230, 239, 248, 257, 266, 275, 285, 294, 304, 313, 323, 332, 342, 351, 361, 370, 380, 389, 398, 407, 416, 424, 433, 441, 449, 457, 465, 473, 480, 487, 494, 501, 507, 514, 520, 526, 531, 537, 542, 547, 552, 557, 561, 565, 570, 573, 577, 581, 584, 587, 591, 593, 596, 599, 602, 604, 606, 608, 611, 612, 614, 616, 618, 619, 621, 622, 624, 625, 626, 627, 629, 630, 631, 631, 632, 633, 634, 635, 635};
const int Params::TRADE_DOWN[8] = {2, 0, 0, 0, 0, 0, 0, 0};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 35, 128, 190}, {0, 0, 0, 0, 25, 83, 160, 272}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 15, 96, 0}, {0, 0, 0, 0, 0, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 6, 23, 102, 199, 320}, {0, 0, 0, 0, 37, 90, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 26, 11, 36, 95, 189}, {0, 0, 0, 9, 10, 26, 62, 67}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-37, -24, -33, -9, -34, -11, -21, -26, -4, -26, -69, -19, 0, -16, 0, -40, -11, -41, -19, -52, 0}, {0, 0, 0, -3, -12, 0, 0, -10, -7, -18, -26, -7, -2, -11, -14, -17, -18, -30, -53, -42, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-60, -75, -54, -53, -46, -53, -58, -54, -26, -31, -32, -43, -8, 0, -3, -32, 0, -1, -67, -35, -83}, {-10, -4, 0, -7, -2, -1, -16, -7, -7, -5, -4, -28, -12, -9, -0, -51, -21, -13, -53, -37, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-42, -2, -18, -11, -11, -18, -2, -42}, {-42, -19, -24, -20, -20, -24, -19, -42}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -27, -2, -16, -16, -2, -27, 0}, {-87, -72, -37, -39, -39, -37, -72, -87}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -17, -23, -23, -17, 0, 0}, {-5, -9, -11, -17, -17, -11, -9, -5}};

const int Params::KNIGHT_PST[2][64] = {{-81, -24, -7, -2, -2, -7, -24, -81, -21, -20, -10, 10, 10, -10, -20, -21, -40, -13, 6, 31, 31, 6, -13, -40, -13, 4, 26, 27, 27, 26, 4, -13, -24, -7, 25, 20, 20, 25, -7, -24, -48, -42, 3, 8, 8, 3, -42, -48, -100, -81, -25, -17, -17, -25, -81, -100, -128, -128, -128, -128, -128, -128, -128, -128}, {-76, -38, -26, -28, -28, -26, -38, -76, -35, -22, -21, -12, -12, -21, -22, -35, -45, -25, -15, 7, 7, -15, -25, -45, -19, -8, 11, 21, 21, 11, -8, -19, -31, -22, -1, 4, 4, -1, -22, -31, -51, -33, -15, -9, -9, -15, -33, -51, -71, -47, -44, -29, -29, -44, -47, -71, -128, -78, -76, -58, -58, -76, -78, -128}};
const int Params::BISHOP_PST[2][64] = {{18, 45, 33, 31, 31, 33, 45, 18, 37, 48, 40, 33, 33, 40, 48, 37, 15, 42, 41, 34, 34, 41, 42, 15, 3, 13, 23, 43, 43, 23, 13, 3, -32, -8, -1, 27, 27, -1, -8, -32, 7, -24, -38, -23, -23, -38, -24, 7, -28, -63, -59, -96, -96, -59, -63, -28, -21, -81, -128, -128, -128, -128, -81, -21}, {-9, 5, 11, 6, 6, 11, 5, -9, 11, 14, 10, 17, 17, 10, 14, 11, 4, 17, 23, 26, 26, 23, 17, 4, 4, 13, 26, 32, 32, 26, 13, 4, -16, -2, -1, 9, 9, -1, -2, -16, 4, -9, -9, -9, -9, -9, -9, 4, -5, -10, -17, -20, -20, -17, -10, -5, 0, -2, -6, -26, -26, -6, -2, 0}};
const int Params::ROOK_PST[2][64] = {{-46, -32, -27, -23, -23, -27, -32, -46, -59, -32, -28, -23, -23, -28, -32, -59, -57, -32, -33, -31, -31, -33, -32, -57, -55, -40, -37, -29, -29, -37, -40, -55, -33, -25, -13, -10, -10, -13, -25, -33, -21, 5, -5, 7, 7, -5, 5, -21, -19, -25, 0, 3, 3, 0, -25, -19, -12, -6, -98, -62, -62, -98, -6, -12}, {-6, -7, 1, -4, -4, 1, -7, -6, -17, -10, -3, -3, -3, -3, -10, -17, -13, -7, -0, -3, -3, -0, -7, -13, -1, 7, 13, 8, 8, 13, 7, -1, 14, 16, 21, 17, 17, 21, 16, 14, 19, 23, 28, 27, 27, 28, 23, 19, 8, 11, 16, 18, 18, 16, 11, 8, 29, 30, 29, 25, 25, 29, 30, 29}};
const int Params::QUEEN_PST[2][64] = {{29, 31, 27, 38, 38, 27, 31, 29, 23, 41, 43, 44, 44, 43, 41, 23, 23, 37, 41, 31, 31, 41, 37, 23, 35, 39, 41, 41, 41, 41, 39, 35, 38, 41, 45, 53, 53, 45, 41, 38, 28, 46, 51, 48, 48, 51, 46, 28, 30, 12, 35, 38, 38, 35, 12, 30, 44, 42, 19, 6, 6, 19, 42, 44}, {-46, -61, -45, -29, -29, -45, -61, -46, -34, -30, -23, -6, -6, -23, -30, -34, -23, 6, 17, 8, 8, 17, 6, -23, 9, 29, 35, 37, 37, 35, 29, 9, 16, 48, 54, 68, 68, 54, 48, 16, 10, 45, 64, 81, 81, 64, 45, 10, 27, 47, 74, 85, 85, 74, 47, 27, 5, 21, 18, 23, 23, 18, 21, 5}};
const int Params::KING_PST[2][64] = {{30, 47, -14, 9, 9, -14, 47, 30, 75, 75, 22, 47, 47, 22, 75, 75, -6, 16, -60, -120, -120, -60, 16, -6, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -108, -128, -128, -128, -128, -108, -128, 50, -56, -128, -128, -128, -128, -56, 50, -128, -128, 128, -128, -128, 128, -128, -128}, {-60, -61, -66, -70, -70, -66, -61, -60, -49, -56, -63, -63, -63, -63, -56, -49, -58, -62, -59, -58, -58, -59, -62, -58, -56, -53, -48, -45, -45, -48, -53, -56, -42, -37, -30, -29, -29, -30, -37, -42, -36, -25, -24, -24, -24, -24, -25, -36, -45, -35, -31, -33, -33, -31, -35, -45, -73, -49, -48, -46, -46, -48, -49, -73}};

const int Params::KNIGHT_MOBILITY[9] = {-71, -27, -5, 7, 15, 24, 27, 25, 17};
const int Params::BISHOP_MOBILITY[15] = {-54, -27, -13, -2, 8, 16, 20, 23, 27, 30, 31, 33, 43, 27, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-62, -39, -27, -22, -21, -15, -10, -2, 2, 5, 7, 10, 20, 27, 25}, {-67, -30, -10, 7, 22, 30, 30, 33, 40, 44, 49, 54, 56, 45, 36}};
const int Params::QUEEN_MOBILITY[2][24] = {{-58, 0, 13, 22, 29, 36, 42, 48, 54, 58, 60, 61, 64, 65, 68, 71, 70, 83, 96, 96, 96, 96, 96, 96}, {-96, -96, -77, -38, 14, 25, 41, 47, 55, 60, 65, 71, 77, 80, 84, 83, 88, 85, 84, 87, 90, 93, 83, 78}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-92, -54, -32, -21, -15};

const int Params::KNIGHT_OUTPOST[2][2] = {{24, 62}, {20, 33}};
const int Params::BISHOP_OUTPOST[2][2] = {{33, 63}, {35, 27}};

const int Params::PAWN_STORM[2][2][4] = {{{0, -14, -20, -4},{0, -89, -36, -10}},
{{-23, -25, 0, 0},{-119, -31, -2, 0}}};

