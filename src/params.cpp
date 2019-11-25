// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 24-Nov-2019 11:09:43 by tuner -n 150 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0668628

//

#include "params.h"

const int Params::RB_ADJUST[6] = {25, -6, 69, 19, 21, 66};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -39, -70, -85};
const int Params::QR_ADJUST[5] = {-51, 37, 19, -31, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -242, -128};
const int Params::MINOR_FOR_PAWNS[6] = {51, 46, 76, 97, 127, 154};
const int Params::QUEEN_VS_3MINORS[4] = {-94, 0, -100, -222};
const int Params::CASTLING[6] = {13, -38, -29, 0, 4, -25};
const int Params::KING_COVER[6][4] = {{8, 24, 15, 16},
{1, 4, 3, 3},
{-8, -25, -16, -17},
{-8, -25, -16, -16},
{-10, -30, -19, -20},
{-17, -52, -33, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 92;
const int Params::PIN_MULTIPLIER_END = 65;
const int Params::KRMINOR_VS_R_NO_PAWNS = -168;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -143;
const int Params::ENDGAME_PAWN_ADVANTAGE = 31;
const int Params::PAWN_ENDGAME1 = 64;
const int Params::PAWN_ATTACK_FACTOR = 13;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 24;
const int Params::ROOK_ATTACK_FACTOR = 54;
const int Params::ROOK_ATTACK_BOOST = 26;
const int Params::QUEEN_ATTACK_FACTOR = 61;
const int Params::QUEEN_ATTACK_BOOST = 42;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 114;
const int Params::KING_ATTACK_COUNT = 5;
const int Params::KING_ATTACK_SQUARES = 5;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 74;
const int Params::OWN_ROOK_KING_PROXIMITY = 24;
const int Params::OWN_QUEEN_KING_PROXIMITY = 16;
const int Params::PAWN_PUSH_THREAT_MID = 24;
const int Params::PAWN_PUSH_THREAT_END = 12;
const int Params::ENDGAME_KING_THREAT = 38;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 38;
const int Params::BISHOP_PAIR_END = 60;
const int Params::BISHOP_PAWN_PLACEMENT_END = -2;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -24;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -7;
const int Params::WEAK_PAWN_END = -5;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 1;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 16;
const int Params::TWO_ROOKS_ON_7TH_MID = 63;
const int Params::TWO_ROOKS_ON_7TH_END = 89;
const int Params::ROOK_ON_OPEN_FILE_MID = 28;
const int Params::ROOK_ON_OPEN_FILE_END = 12;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 28;
const int Params::QUEEN_OUT = -28;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 15;
const int Params::KING_OPP_PAWN_DISTANCE = 9;
const int Params::SIDE_PROTECTED_PAWN = -2;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 1, 33, 59, 93, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={186, 170, 137};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 20, 21, 23, 24, 26, 28, 30, 32, 34, 36, 39, 41, 44, 47, 49, 52, 56, 59, 62, 66, 70, 74, 78, 82, 86, 91, 96, 101, 106, 112, 117, 123, 129, 135, 142, 148, 155, 162, 169, 177, 184, 192, 200, 208, 216, 225, 233, 242, 250, 259, 268, 277, 286, 295, 304, 314, 323, 332, 341, 350, 359, 368, 377, 386, 394, 403, 411, 419, 428, 435, 443, 451, 458, 465, 473, 479, 486, 492, 499, 505, 511, 516, 522, 527, 532, 537, 541, 546, 550, 554, 558, 562, 566, 569, 573, 576, 579, 582, 584, 587, 589, 592, 594, 596, 598, 600, 602, 604, 606, 607, 609, 610, 611, 613, 614, 615, 616, 617, 618, 619, 620};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 12, 15, 21, 30, 36, 56, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 31, 114, 194}, {0, 0, 0, 0, 22, 84, 160, 288}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 72, 71, 64, 64, 71, 72, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 2, 18, 96, 0}, {0, 0, 0, 0, 1, 19, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 25, 76, 118, 57}, {0, 0, 0, 0, 45, 61, 163, 288}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 28, 96, 169}, {0, 0, 0, 9, 12, 30, 62, 78}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 9, 5}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-30, -4, 0}, {-20, -33, -111}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-15, -44, -76}, {-48, -73, -128}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{29, 42, 128}, {21, 57, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{0, -20, -2}, {-33, -62, -123}};
const int Params::DOUBLED_PAWNS[2][8] = {{-37, 0, -14, -9, -9, -14, 0, -37}, {-38, -17, -21, -16, -16, -21, -17, -38}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -17, 0, 0, 0, 0, -17, 0}, {-56, -64, -33, -34, -34, -33, -64, -56}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -20, -25, -25, -20, -4, 0}, {-9, -13, -15, -21, -21, -15, -13, -9}};

const int Params::THREAT_BY_PAWN[2][5] = {{91, 101, 99, 47, 0}, {65, 84, 75, 56, 0}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{11, 50, 68, 128, 74}, {27, 23, 59, 73, 102}};
const int Params::THREAT_BY_BISHOP[2][5] = {{21, 51, 77, 84, 77}, {26, 47, 53, 65, 128}};
const int Params::THREAT_BY_ROOK[2][5] = {{19, 52, 57, 128, 99}, {29, 45, 52, 3, 128}};
const int Params::KNIGHT_PST[2][64] = {{-100, -43, -29, -22, -22, -29, -43, -100, -42, -41, -31, -8, -8, -31, -41, -42, -55, -31, -11, 14, 14, -11, -31, -55, -26, -3, 11, 15, 15, 11, -3, -26, -36, -21, 10, 4, 4, 10, -21, -36, -63, -59, -19, -16, -16, -19, -59, -63, -105, -95, -41, -34, -34, -41, -95, -105, -128, -128, -128, -128, -128, -128, -128, -128}, {-85, -48, -38, -38, -38, -38, -48, -85, -43, -35, -34, -22, -22, -34, -35, -43, -49, -33, -24, -3, -3, -24, -33, -49, -26, -16, 1, 11, 11, 1, -16, -26, -44, -38, -19, -14, -14, -19, -38, -44, -62, -45, -31, -30, -30, -31, -45, -62, -82, -58, -55, -42, -42, -55, -58, -82, -128, -80, -86, -74, -74, -86, -80, -128}};
const int Params::BISHOP_PST[2][64] = {{-7, 22, 10, 8, 8, 10, 22, -7, 12, 25, 19, 13, 13, 19, 25, 12, -5, 20, 20, 16, 16, 20, 20, -5, -1, -1, 9, 26, 26, 9, -1, -1, -27, -3, -3, 15, 15, -3, -3, -27, 16, -19, -31, -17, -17, -31, -19, 16, -26, -49, -45, -67, -67, -45, -49, -26, -7, -58, -128, -128, -128, -128, -58, -7}, {-29, -13, -4, -9, -9, -4, -13, -29, -12, -5, -6, 2, 2, -6, -5, -12, -11, 4, 8, 12, 12, 8, 4, -11, -8, 3, 12, 15, 15, 12, 3, -8, -20, -7, -8, -3, -3, -8, -7, -20, 2, -14, -11, -14, -14, -11, -14, 2, -7, -8, -18, -17, -17, -18, -8, -7, -1, -7, -9, -28, -28, -9, -7, -1}};
const int Params::ROOK_PST[2][64] = {{-40, -28, -23, -18, -18, -23, -28, -40, -53, -30, -24, -18, -18, -24, -30, -53, -50, -29, -27, -23, -23, -27, -29, -50, -48, -36, -36, -25, -25, -36, -36, -48, -30, -25, -15, -14, -14, -15, -25, -30, -29, -10, -15, -7, -7, -15, -10, -29, -41, -48, -33, -25, -25, -33, -48, -41, -36, -34, -128, -113, -113, -128, -34, -36}, {-13, -16, -6, -12, -12, -6, -16, -13, -26, -21, -14, -14, -14, -14, -21, -26, -19, -14, -8, -9, -9, -8, -14, -19, -4, 1, 7, 3, 3, 7, 1, -4, 8, 10, 14, 10, 10, 14, 10, 8, 8, 12, 15, 12, 12, 15, 12, 8, -5, -0, 1, 1, 1, 1, -0, -5, 15, 19, 21, 15, 15, 21, 19, 15}};
const int Params::QUEEN_PST[2][64] = {{4, 2, -0, 10, 10, -0, 2, 4, -1, 14, 16, 19, 19, 16, 14, -1, 1, 14, 20, 11, 11, 20, 14, 1, 15, 21, 21, 21, 21, 21, 21, 15, 22, 24, 28, 28, 28, 28, 24, 22, 1, 22, 32, 24, 24, 32, 22, 1, -4, -11, 9, 12, 12, 9, -11, -4, 22, 15, -4, -26, -26, -4, 15, 22}, {-83, -92, -77, -62, -62, -77, -92, -83, -71, -65, -58, -42, -42, -58, -65, -71, -56, -26, -16, -27, -27, -16, -26, -56, -25, -5, 7, 8, 8, 7, -5, -25, -20, 17, 25, 41, 41, 25, 17, -20, -25, 10, 32, 46, 46, 32, 10, -25, -9, 10, 39, 47, 47, 39, 10, -9, -46, -27, -24, -20, -20, -24, -27, -46}};
const int Params::KING_PST[2][64] = {{17, 33, -25, -5, -5, -25, 33, 17, 53, 55, 5, 31, 31, 5, 55, 53, -44, -16, -73, -128, -128, -73, -16, -44, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -116, -128, -128, -128, -128, -116, -128, 21, 22, -128, -128, -128, -128, 22, 21, -38, -128, 128, -120, -120, 128, -128, -38}, {-30, -26, -31, -36, -36, -31, -26, -30, -13, -20, -27, -27, -27, -27, -20, -13, -22, -24, -21, -20, -20, -21, -24, -22, -19, -13, -7, -5, -5, -7, -13, -19, -5, 5, 14, 14, 14, 14, 5, -5, 2, 16, 18, 17, 17, 18, 16, 2, -14, 4, 8, 7, 7, 8, 4, -14, -52, -16, -12, -10, -10, -12, -16, -52}};
const int Params::KNIGHT_MOBILITY[9] = {-87, -41, -19, -6, 4, 14, 17, 17, 10};
const int Params::BISHOP_MOBILITY[15] = {-64, -38, -24, -12, -1, 6, 11, 15, 20, 24, 27, 28, 42, 22, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-71, -48, -37, -32, -31, -26, -22, -15, -11, -6, -2, 4, 16, 30, 23}, {-85, -46, -26, -11, 6, 12, 12, 17, 22, 27, 34, 39, 44, 37, 31}};
const int Params::QUEEN_MOBILITY[2][24] = {{-74, -19, -6, 5, 11, 18, 24, 29, 35, 40, 42, 43, 46, 46, 50, 50, 51, 61, 69, 76, 92, 96, 96, 96}, {-96, -96, -96, -96, -63, -49, -31, -30, -24, -21, -16, -12, -10, -6, -5, -4, -5, -6, -11, -11, -10, -10, -21, -27}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-70, -36, -18, -7, -5};
const int Params::KNIGHT_OUTPOST[2][2] = {{25, 57}, {22, 42}};
const int Params::BISHOP_OUTPOST[2][2] = {{13, 39}, {22, 28}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -22, 26, 4},{20, -28, 19, 30, -8},{20, 26, 13, 40, 10},{20, -16, 30, 41, 16}},{{10, 4, 3, -22, -10},{10, -64, -2, -6, -2},{10, 31, 22, 16, 8},{10, 64, -12, 13, -3}}};

