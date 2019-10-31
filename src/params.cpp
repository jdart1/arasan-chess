// Copyright 2015-2019 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 30-Oct-2019 09:13:35 by tuner -n 150 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0670854

//

#include "params.h"

const int Params::RB_ADJUST[6] = {46, -6, 88, 38, 36, 73};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -37, -65, -64};
const int Params::QR_ADJUST[5] = {13, 75, 30, -37, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -259, -143};
const int Params::MINOR_FOR_PAWNS[6] = {51, 50, 73, 93, 117, 131};
const int Params::CASTLING[6] = {13, -38, -29, 0, 3, -24};
const int Params::KING_COVER[6][4] = {{8, 23, 15, 16},
{1, 4, 2, 2},
{-8, -25, -16, -17},
{-8, -24, -15, -16},
{-10, -31, -19, -20},
{-17, -52, -32, -35}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 71;
const int Params::KRMINOR_VS_R_NO_PAWNS = -196;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -158;
const int Params::ENDGAME_PAWN_ADVANTAGE = 27;
const int Params::PAWN_ENDGAME1 = 41;
const int Params::PAWN_ATTACK_FACTOR = 13;
const int Params::MINOR_ATTACK_FACTOR = 45;
const int Params::MINOR_ATTACK_BOOST = 24;
const int Params::ROOK_ATTACK_FACTOR = 52;
const int Params::ROOK_ATTACK_BOOST = 26;
const int Params::QUEEN_ATTACK_FACTOR = 60;
const int Params::QUEEN_ATTACK_BOOST = 43;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 117;
const int Params::KING_ATTACK_COUNT = 4;
const int Params::KING_ATTACK_SQUARES = 5;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 75;
const int Params::OWN_ROOK_KING_PROXIMITY = 26;
const int Params::OWN_QUEEN_KING_PROXIMITY = 14;
const int Params::PAWN_PUSH_THREAT_MID = 30;
const int Params::PAWN_PUSH_THREAT_END = 16;
const int Params::ENDGAME_KING_THREAT = 54;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 59;
const int Params::BISHOP_PAWN_PLACEMENT_END = -1;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -23;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -6;
const int Params::WEAK_PAWN_END = -4;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -16;
const int Params::SPACE = 6;
const int Params::PAWN_CENTER_SCORE_MID = 2;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 22;
const int Params::TWO_ROOKS_ON_7TH_MID = 60;
const int Params::TWO_ROOKS_ON_7TH_END = 66;
const int Params::ROOK_ON_OPEN_FILE_MID = 30;
const int Params::ROOK_ON_OPEN_FILE_END = 10;
const int Params::ROOK_BEHIND_PP_MID = 1;
const int Params::ROOK_BEHIND_PP_END = 24;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 14;
const int Params::KING_OPP_PAWN_DISTANCE = 10;
const int Params::SIDE_PROTECTED_PAWN = -3;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 29, 54, 89, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={209, 186, 150};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-1, -1, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 7, 8, 9, 10, 10, 11, 13, 14, 15, 16, 17, 19, 20, 22, 23, 25, 27, 29, 31, 33, 35, 38, 40, 43, 45, 48, 51, 54, 58, 61, 65, 68, 72, 76, 81, 85, 90, 95, 100, 105, 110, 116, 122, 128, 134, 140, 147, 154, 161, 168, 176, 183, 191, 199, 207, 216, 224, 233, 242, 250, 259, 268, 277, 287, 296, 305, 314, 324, 333, 342, 351, 360, 370, 378, 387, 396, 405, 413, 421, 430, 438, 445, 453, 461, 468, 475, 482, 488, 495, 501, 507, 513, 519, 524, 529, 534, 539, 544, 548, 553, 557, 561, 564, 568, 571, 575, 578, 581, 584, 586, 589, 591, 594, 596, 598, 600, 602, 604, 605, 607, 609, 610, 611, 613, 614, 615, 616, 617, 618, 619, 620, 621};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 13, 16, 21, 30, 36, 57, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 30, 115, 199}, {0, 0, 0, 0, 22, 87, 160, 288}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 72, 71, 64, 64, 71, 72, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 2, 0, 19, 96, 0}, {0, 0, 0, 1, 3, 20, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 24, 72, 111, 123}, {0, 0, 0, 0, 47, 67, 173, 300}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 0, 0, 29, 96, 171}, {0, 0, 0, 13, 15, 34, 69, 90}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 11, 4}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-27, 0, 0}, {-23, -36, -114}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-10, -44, -85}, {-54, -80, -128}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{32, 45, 128}, {20, 56, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{0, -20, -1}, {-37, -62, -122}};
const int Params::DOUBLED_PAWNS[2][8] = {{-42, 0, -13, -8, -8, -13, 0, -42}, {-37, -16, -19, -14, -14, -19, -16, -37}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -20, 0, 0, 0, 0, -20, 0}, {-62, -62, -29, -32, -32, -29, -62, -62}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -3, -19, -25, -25, -19, -3, 0}, {-10, -14, -16, -22, -22, -16, -14, -10}};

const int Params::THREAT_BY_PAWN[2][4] = {{64, 88, 86, 84}, {96, 64, 79, 79}};
const int Params::THREAT_BY_MINOR[2][4] = {{16, 42, 59, 96}, {28, 36, 50, 74}};
const int Params::THREAT_BY_ROOK[2][4] = {{23, 31, 40, 96}, {21, 27, 28, 0}};
const int Params::KNIGHT_PST[2][64] = {{-128, -47, -29, -26, -26, -29, -47, -128, -47, -43, -30, -8, -8, -30, -43, -47, -56, -25, -5, 20, 20, -5, -25, -56, -28, 5, 19, 23, 23, 19, 5, -28, -33, -18, 20, 13, 13, 20, -18, -33, -62, -43, 2, 3, 3, 2, -43, -62, -106, -87, -27, -20, -20, -27, -87, -106, -128, -128, -128, -128, -128, -128, -128, -128}, {-109, -63, -51, -51, -51, -51, -63, -109, -59, -46, -45, -34, -34, -45, -46, -59, -59, -40, -29, -11, -11, -29, -40, -59, -36, -23, -4, 5, 5, -4, -23, -36, -48, -41, -20, -15, -15, -20, -41, -48, -64, -47, -30, -30, -30, -30, -47, -64, -83, -61, -57, -47, -47, -57, -61, -83, -128, -82, -91, -80, -80, -91, -82, -128}};
const int Params::BISHOP_PST[2][64] = {{6, 35, 24, 22, 22, 24, 35, 6, 26, 37, 33, 27, 27, 33, 37, 26, 9, 37, 37, 32, 32, 37, 37, 9, 8, 17, 27, 39, 39, 27, 17, 8, -9, 8, 16, 28, 28, 16, 8, -9, 33, 2, 1, 5, 5, 1, 2, 33, -9, -38, -27, -63, -63, -27, -38, -9, -4, -49, -128, -128, -128, -128, -49, -4}, {-27, -13, -5, -10, -10, -5, -13, -27, -12, -6, -8, 1, 1, -8, -6, -12, -11, 6, 10, 14, 14, 10, 6, -11, -5, 6, 15, 18, 18, 15, 6, -5, -15, -2, -2, 2, 2, -2, -2, -15, 8, -9, -7, -10, -10, -7, -9, 8, -2, -4, -15, -14, -14, -15, -4, -2, 4, -1, -2, -23, -23, -2, -1, 4}};
const int Params::ROOK_PST[2][64] = {{-52, -41, -36, -30, -30, -36, -41, -52, -64, -43, -36, -31, -31, -36, -43, -64, -62, -41, -40, -34, -34, -40, -41, -62, -62, -49, -50, -37, -37, -50, -49, -62, -43, -40, -31, -27, -27, -31, -40, -43, -40, -22, -26, -17, -17, -26, -22, -40, -46, -52, -36, -27, -27, -36, -52, -46, -42, -39, -128, -123, -123, -128, -39, -42}, {-28, -31, -22, -28, -28, -22, -31, -28, -40, -36, -29, -28, -28, -29, -36, -40, -33, -28, -23, -25, -25, -23, -28, -33, -16, -10, -5, -10, -10, -5, -10, -16, -3, -0, 3, -0, -0, 3, -0, -3, -3, 2, 5, 3, 3, 5, 2, -3, -18, -13, -10, -9, -9, -10, -13, -18, 4, 7, 7, 3, 3, 7, 7, 4}};
const int Params::QUEEN_PST[2][64] = {{-1, -4, -6, 5, 5, -6, -4, -1, -5, 10, 12, 14, 14, 12, 10, -5, -4, 9, 16, 7, 7, 16, 9, -4, 10, 18, 17, 16, 16, 17, 18, 10, 20, 20, 25, 20, 20, 25, 20, 20, -8, 12, 19, 12, 12, 19, 12, -8, -17, -23, -4, -3, -3, -4, -23, -17, 11, 3, -25, -47, -47, -25, 3, 11}, {-96, -106, -90, -73, -73, -90, -106, -96, -85, -78, -69, -51, -51, -69, -78, -85, -73, -37, -27, -39, -39, -27, -37, -73, -37, -17, -3, -1, -1, -3, -17, -37, -36, 6, 14, 31, 31, 14, 6, -36, -36, -3, 22, 37, 37, 22, -3, -36, -22, 1, 28, 37, 37, 28, 1, -22, -57, -43, -33, -29, -29, -33, -43, -57}};
const int Params::KING_PST[2][64] = {{17, 31, -29, -12, -12, -29, 31, 17, 55, 55, 0, 20, 20, 0, 55, 55, -38, -10, -78, -128, -128, -78, -10, -38, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, 65, -30, -128, -128, -128, -128, -30, 65, -81, -128, 128, -34, -34, 128, -128, -81}, {-26, -21, -26, -31, -31, -26, -21, -26, -10, -16, -23, -23, -23, -23, -16, -10, -19, -20, -18, -18, -18, -18, -20, -19, -15, -9, -4, -2, -2, -4, -9, -15, -1, 10, 16, 15, 15, 16, 10, -1, 7, 20, 21, 19, 19, 21, 20, 7, -7, 10, 13, 10, 10, 13, 10, -7, -44, -10, -9, -7, -7, -9, -10, -44}};
const int Params::KNIGHT_MOBILITY[9] = {17, -7, 14, 26, 34, 43, 46, 44, 38};
const int Params::BISHOP_MOBILITY[15] = {-45, -18, -3, 8, 18, 26, 30, 34, 39, 42, 44, 46, 61, 41, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-75, -53, -41, -36, -35, -30, -26, -19, -15, -10, -6, 1, 12, 25, 10}, {-96, -60, -39, -23, -6, 0, 1, 6, 11, 16, 22, 28, 33, 24, 19}};
const int Params::QUEEN_MOBILITY[2][24] = {{-88, -27, -13, -2, 5, 11, 18, 23, 29, 32, 34, 35, 36, 36, 41, 41, 38, 51, 62, 64, 92, 95, 96, 96}, {-96, -96, -96, -96, -75, -64, -42, -42, -35, -33, -28, -24, -20, -18, -19, -16, -15, -20, -24, -26, -24, -24, -35, -44}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-70, -35, -18, -8, -7};
const int Params::KNIGHT_OUTPOST[2][2] = {{18, 64}, {13, 43}};
const int Params::BISHOP_OUTPOST[2][2] = {{8, 50}, {18, 29}};
const int Params::PAWN_STORM[4][2] = {{0, 28},{10, 6},{34, 0},{5, 0}};

