// Copyright 2015-2020 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 13-Dec-2020 06:46:31 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/data/epd/big7.epd
// Final objective value: 0.0591773

//

#include "params.h"

const int Params::KING_COVER[6][4] = {{5, 16, 10, 11},
{-0, -1, -1, -1},
{-10, -30, -19, -20},
{-10, -30, -19, -20},
{-12, -37, -24, -25},
{-20, -60, -38, -40}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 86;
const int Params::PIN_MULTIPLIER_END = 66;
const int Params::OPP_COLORED_BISHOPS_SCALE = 32;
const int Params::SAME_COLORED_BISHOPS_SCALE = 31;
const int Params::SINGLE_MINOR_NO_PAWNS = -269;
const int Params::RB_ADJUST_MIDGAME = -8;
const int Params::RB_ADJUST_ENDGAME = 124;
const int Params::RBN_ADJUST_MIDGAME = -79;
const int Params::RBN_ADJUST_ENDGAME = 126;
const int Params::PIECE_TRADE_DOWN = 0;
const int Params::PAWN_TRADE_DOWN = -7;
const int Params::PAWN_ENDGAME_ADJUST = 21;
const int Params::PAWN_ATTACK_FACTOR = 17;
const int Params::MINOR_ATTACK_FACTOR = 44;
const int Params::MINOR_ATTACK_BOOST = 27;
const int Params::ROOK_ATTACK_FACTOR = 52;
const int Params::ROOK_ATTACK_BOOST = 25;
const int Params::QUEEN_ATTACK_FACTOR = 57;
const int Params::QUEEN_ATTACK_BOOST = 43;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 4;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 122;
const int Params::KING_ATTACK_COUNT = 3;
const int Params::KING_ATTACK_SQUARES = 4;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 4;
const int Params::OWN_MINOR_KING_PROXIMITY = 69;
const int Params::OWN_ROOK_KING_PROXIMITY = 21;
const int Params::OWN_QUEEN_KING_PROXIMITY = 12;
const int Params::PAWN_PUSH_THREAT_MID = 25;
const int Params::PAWN_PUSH_THREAT_END = 13;
const int Params::ENDGAME_KING_THREAT = 34;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 45;
const int Params::BISHOP_PAIR_END = 76;
const int Params::BISHOP_PAWN_PLACEMENT_END = -8;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -6;
const int Params::CENTER_PAWN_BLOCK = -20;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -8;
const int Params::WEAK_PAWN_END = -7;
const int Params::WEAK_ON_OPEN_FILE_MID = -20;
const int Params::WEAK_ON_OPEN_FILE_END = -18;
const int Params::SPACE = 7;
const int Params::PAWN_CENTER_SCORE_MID = 5;
const int Params::ROOK_ON_7TH_MID = 14;
const int Params::ROOK_ON_7TH_END = 16;
const int Params::TWO_ROOKS_ON_7TH_MID = 60;
const int Params::TWO_ROOKS_ON_7TH_END = 83;
const int Params::TRAPPED_ROOK = 0;
const int Params::TRAPPED_ROOK_NO_CASTLE = -77;
const int Params::ROOK_ON_OPEN_FILE_MID = 30;
const int Params::ROOK_ON_OPEN_FILE_END = 12;
const int Params::ROOK_BEHIND_PP_MID = 8;
const int Params::ROOK_BEHIND_PP_END = 20;
const int Params::QUEEN_OUT = -27;
const int Params::PAWN_SIDE_BONUS = 21;
const int Params::KING_OWN_PAWN_DISTANCE = 17;
const int Params::KING_OPP_PAWN_DISTANCE = 6;
const int Params::SIDE_PROTECTED_PAWN = -2;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 24, 39, 63, 96, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={248, 179, 134};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18, 19, 21, 22, 24, 26, 27, 29, 31, 33, 35, 38, 40, 42, 45, 48, 51, 54, 57, 60, 63, 67, 71, 74, 78, 83, 87, 91, 96, 101, 106, 111, 117, 122, 128, 134, 140, 147, 153, 160, 167, 174, 181, 188, 196, 203, 211, 219, 227, 235, 244, 252, 261, 269, 278, 286, 295, 304, 312, 321, 330, 338, 347, 355, 364, 372, 380, 388, 396, 404, 412, 420, 427, 434, 441, 448, 455, 462, 468, 474, 480, 486, 492, 497, 503, 508, 513, 517, 522, 526, 531, 535, 539, 542, 546, 549, 553, 556, 559, 562, 565, 567, 570, 572, 574, 577, 579, 581, 583, 584, 586, 588, 589, 591, 592, 593, 595, 596, 597, 598, 599, 600, 601};
const int Params::OWN_PIECE_KING_PROXIMITY_MULT[16] = {0, 0, 0, 8, 13, 17, 24, 31, 40, 56, 64, 64, 64, 64, 64, 64};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 47, 124, 203}, {0, 0, 3, 0, 25, 79, 152, 263}};
const int Params::PASSED_PAWN_FILE_ADJUST[4] = {69, 71, 68, 64};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 2, 45, 74, 152, 320}, {0, 0, 0, 7, 49, 66, 187, 272}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 5, 4, 35, 102, 178}, {0, 0, 0, 19, 21, 39, 72, 78}};
const int Params::QUEENING_PATH_CLEAR[2][6] = {{29, 0, 0, 0, 26, 128}, {20, 8, 31, 45, 89, 128}};
const int Params::PP_OWN_PIECE_BLOCK[2][3] = {{-35, 0, 0}, {0, 0, -53}};
const int Params::PP_OPP_PIECE_BLOCK[2][3] = {{-23, -39, -40}, {-35, -49, -88}};
const int Params::QUEENING_PATH_CONTROL[2][3] = {{34, 57, 128}, {28, 57, 128}};
const int Params::QUEENING_PATH_OPP_CONTROL[2][3] = {{-4, -12, 0}, {-19, -31, -52}};
const int Params::DOUBLED_PAWNS[2][8] = {{-43, -15, -25, -11, 0, 0, 0, 0}, {0, 0, -2, -59, 0, 0, 0, 0}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -57, -64, -64, 0, 0, 0, 0}, {-84, -58, -34, -0, 0, 0, 0, 0}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -4, -22, -32, 0, 0, 0, 0}, {-14, -24, -24, -18, 0, 0, 0, 0}};

const int Params::THREAT_BY_PAWN[2][5] = {{76, 79, 74, 63, 0}, {51, 72, 53, 31, 128}};
const int Params::THREAT_BY_KNIGHT[2][5] = {{15, 23, 69, 117, 80}, {31, 38, 57, 65, 67}};
const int Params::THREAT_BY_BISHOP[2][5] = {{20, 48, 36, 66, 92}, {30, 47, 63, 57, 97}};
const int Params::THREAT_BY_ROOK[2][5] = {{24, 49, 53, 11, 91}, {26, 41, 48, 40, 104}};
const int Params::KNIGHT_PST[2][64] = {{-76, -28, -10, -2, -2, -10, -28, -76, -20, -20, -11, 12, 12, -11, -20, -20, -38, -13, 7, 35, 35, 7, -13, -38, -6, 14, 33, 39, 39, 33, 14, -6, -30, -11, 25, 15, 15, 25, -11, -30, -67, -51, -2, -4, -4, -2, -51, -67, -104, -81, -35, -28, -28, -35, -81, -104, -128, -112, -81, -80, -80, -81, -112, -128}, {-87, -46, -29, -27, -27, -29, -46, -87, -41, -28, -29, -17, -17, -29, -28, -41, -44, -28, -17, 3, 3, -17, -28, -44, -20, -12, 9, 19, 19, 9, -12, -20, -41, -36, -14, -9, -9, -14, -36, -41, -59, -44, -25, -25, -25, -25, -44, -59, -77, -57, -54, -42, -42, -54, -57, -77, -128, -77, -76, -70, -70, -76, -77, -128}};
const int Params::BISHOP_PST[2][64] = {{1, 33, 18, 20, 20, 18, 33, 1, 24, 33, 30, 23, 23, 30, 33, 24, 4, 30, 31, 26, 26, 31, 30, 4, 8, 11, 18, 42, 42, 18, 11, 8, -20, 8, 11, 28, 28, 11, 8, -20, 22, -6, 15, 2, 2, 15, -6, 22, -12, -18, -27, -35, -35, -27, -18, -12, 19, -30, -15, -106, -106, -15, -30, 19}, {-42, -20, -8, -12, -12, -8, -20, -42, -24, -11, -13, -4, -4, -13, -11, -24, -16, -3, 3, 5, 5, 3, -3, -16, -13, -4, 5, 10, 10, 5, -4, -13, -24, -12, -12, -9, -9, -12, -12, -24, -4, -18, -17, -20, -20, -17, -18, -4, -14, -13, -22, -22, -22, -22, -13, -14, -8, -12, -11, -28, -28, -11, -12, -8}};
const int Params::ROOK_PST[2][64] = {{-35, -26, -20, -17, -17, -20, -26, -35, -55, -29, -21, -16, -16, -21, -29, -55, -52, -30, -24, -21, -21, -24, -30, -52, -52, -39, -36, -20, -20, -36, -39, -52, -33, -26, -12, -8, -8, -12, -26, -33, -31, -12, -8, 7, 7, -8, -12, -31, -54, -64, -50, -42, -42, -50, -64, -54, -26, -18, -33, -23, -23, -33, -18, -26}, {-43, -45, -38, -43, -43, -38, -45, -43, -51, -45, -40, -41, -41, -40, -45, -51, -46, -41, -34, -35, -35, -34, -41, -46, -29, -25, -19, -21, -21, -19, -25, -29, -16, -14, -8, -11, -11, -8, -14, -16, -16, -11, -9, -10, -10, -9, -11, -16, -27, -21, -21, -20, -20, -21, -21, -27, -8, -3, -2, -11, -11, -2, -3, -8}};
const int Params::QUEEN_PST[2][64] = {{3, 6, 3, 11, 11, 3, 6, 3, 0, 16, 18, 21, 21, 18, 16, 0, 1, 16, 24, 13, 13, 24, 16, 1, 16, 22, 25, 25, 25, 25, 22, 16, 23, 27, 32, 32, 32, 32, 27, 23, -0, 23, 32, 28, 28, 32, 23, -0, -6, -9, 13, 5, 5, 13, -9, -6, 29, 12, 5, -30, -30, 5, 12, 29}, {-66, -75, -57, -39, -39, -57, -75, -66, -49, -52, -42, -29, -29, -42, -52, -49, -38, -10, -3, -16, -16, -3, -10, -38, -5, 12, 22, 20, 20, 22, 12, -5, -1, 32, 39, 52, 52, 39, 32, -1, -6, 30, 47, 59, 59, 47, 30, -6, 10, 29, 49, 58, 58, 49, 29, 10, -24, -9, -0, -2, -2, -0, -9, -24}};
const int Params::KING_PST[2][64] = {{-1, 14, -36, 2, 2, -36, 14, -1, 38, 37, -23, 2, 2, -23, 37, 38, -84, -59, -117, -128, -128, -117, -59, -84, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -114, -128, -128, -128, -128, -114, -128, 50, -80, -105, -128, -128, -105, -80, 50, -110, -128, 18, 65, 65, 18, -128, -110}, {-2, 3, 1, -5, -5, 1, 3, -2, 16, 12, 9, 9, 9, 9, 12, 16, 12, 11, 15, 15, 15, 15, 11, 12, 20, 25, 31, 34, 34, 31, 25, 20, 34, 46, 53, 53, 53, 53, 46, 34, 44, 57, 58, 56, 56, 58, 57, 44, 31, 43, 47, 47, 47, 47, 43, 31, -24, 21, 25, 29, 29, 25, 21, -24}};
const int Params::KNIGHT_MOBILITY[9] = {-80, -30, -8, 6, 17, 27, 31, 28, 18};
const int Params::BISHOP_MOBILITY[15] = {-49, -21, -5, 6, 19, 27, 32, 36, 43, 45, 46, 45, 56, 36, 38};
const int Params::ROOK_MOBILITY[2][15] = {{-91, -69, -57, -55, -54, -48, -45, -38, -33, -28, -22, -16, -5, 3, 5}, {-96, -74, -52, -36, -19, -14, -12, -6, -1, 6, 13, 19, 23, 19, 12}};
const int Params::QUEEN_MOBILITY[2][24] = {{-84, -12, 3, 11, 17, 25, 32, 37, 44, 48, 51, 54, 55, 56, 60, 61, 66, 74, 90, 93, 96, 96, 96, 96}, {-96, -96, -96, -67, -33, -18, -4, 1, 7, 10, 15, 17, 20, 26, 26, 24, 23, 20, 16, 11, 16, 13, 5, -6}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-65, -22, -4, 7, 9};
const int Params::KNIGHT_OUTPOST[2][2] = {{39, 70}, {23, 46}};
const int Params::BISHOP_OUTPOST[2][2] = {{15, 38}, {20, 27}};
const int Params::PAWN_STORM[2][4][5] = {{{20, -64, -31, 31, 8},{20, -47, 25, 35, -4},{20, 41, 15, 49, 13},{20, 1, 29, 45, 15}},{{10, 60, -1, -26, -11},{10, -64, -10, -5, -3},{10, 42, 26, 17, 8},{10, 64, -14, 11, -5}}};

