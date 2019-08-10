// Copyright 2015-2018 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 09-Aug-2019 04:42:04 by tuner -n 175 -R 25 -c 24 /home/jdart/chess/data/epd/big5.epd
// Final objective value: 0.0678704

//

#include "params.h"

const int Params::RB_ADJUST[6] = {46, -6, 92, 43, 38, 73};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -28, -57, -62};
const int Params::QR_ADJUST[5] = {18, 74, 28, -41, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -251, -131};
const int Params::MINOR_FOR_PAWNS[6] = {51, 59, 72, 92, 113, 128};
const int Params::CASTLING[6] = {13, -38, -27, 0, 5, -21};
const int Params::KING_COVER[6][4] = {{8, 23, 13, 15},
{3, 8, 4, 5},
{-7, -21, -12, -14},
{-7, -21, -12, -14},
{-9, -26, -15, -17},
{-15, -45, -26, -30}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 96;
const int Params::PIN_MULTIPLIER_END = 85;
const int Params::KRMINOR_VS_R_NO_PAWNS = -200;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -149;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 41;
const int Params::PAWN_ATTACK_FACTOR = 15;
const int Params::MINOR_ATTACK_FACTOR = 43;
const int Params::MINOR_ATTACK_BOOST = 41;
const int Params::ROOK_ATTACK_FACTOR = 60;
const int Params::ROOK_ATTACK_BOOST = 23;
const int Params::ROOK_ATTACK_BOOST2 = 38;
const int Params::QUEEN_ATTACK_FACTOR = 59;
const int Params::QUEEN_ATTACK_BOOST = 46;
const int Params::QUEEN_ATTACK_BOOST2 = 63;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 6;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 133;
const int Params::OWN_PIECE_KING_PROXIMITY_MIN = 12;
const int Params::OWN_PIECE_KING_PROXIMITY_MAX = 36;
const int Params::OWN_MINOR_KING_PROXIMITY = 83;
const int Params::OWN_ROOK_KING_PROXIMITY = 33;
const int Params::OWN_QUEEN_KING_PROXIMITY = 17;
const int Params::PAWN_THREAT_ON_PIECE_MID = 4;
const int Params::PAWN_THREAT_ON_PIECE_END = 13;
const int Params::PIECE_THREAT_MM_MID = 37;
const int Params::PIECE_THREAT_MR_MID = 96;
const int Params::PIECE_THREAT_MQ_MID = 79;
const int Params::PIECE_THREAT_MM_END = 31;
const int Params::PIECE_THREAT_MR_END = 77;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 11;
const int Params::MINOR_PAWN_THREAT_END = 25;
const int Params::PIECE_THREAT_RM_MID = 24;
const int Params::PIECE_THREAT_RR_MID = 11;
const int Params::PIECE_THREAT_RQ_MID = 96;
const int Params::PIECE_THREAT_RM_END = 34;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 16;
const int Params::ROOK_PAWN_THREAT_END = 28;
const int Params::ENDGAME_KING_THREAT = 40;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 39;
const int Params::BISHOP_PAIR_END = 59;
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
const int Params::TWO_ROOKS_ON_7TH_MID = 40;
const int Params::TWO_ROOKS_ON_7TH_END = 69;
const int Params::ROOK_ON_OPEN_FILE_MID = 35;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 5;
const int Params::ROOK_BEHIND_PP_END = 25;
const int Params::QUEEN_OUT = -30;
const int Params::PAWN_SIDE_BONUS = 15;
const int Params::KING_OWN_PAWN_DISTANCE = 13;
const int Params::KING_OPP_PAWN_DISTANCE = 7;
const int Params::QUEENING_SQUARE_CONTROL_MID = 58;
const int Params::QUEENING_SQUARE_CONTROL_END = 60;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -16;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -42;
const int Params::SIDE_PROTECTED_PAWN = -6;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 27, 57, 85, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={228, 192, 151};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21, 22, 24, 26, 28, 30, 32, 34, 36, 38, 41, 43, 46, 49, 52, 55, 59, 62, 66, 69, 73, 77, 82, 86, 91, 96, 101, 106, 112, 117, 123, 129, 136, 142, 149, 156, 163, 170, 178, 186, 194, 202, 210, 218, 227, 236, 245, 254, 263, 272, 281, 291, 300, 310, 319, 329, 338, 348, 357, 367, 376, 386, 395, 404, 413, 422, 430, 439, 447, 456, 464, 471, 479, 487, 494, 501, 508, 514, 521, 527, 533, 539, 544, 550, 555, 560, 565, 569, 574, 578, 582, 586, 590, 593, 597, 600, 603, 606, 609, 611, 614, 616, 619, 621, 623, 625, 627, 629, 630, 632, 633, 635, 636, 638, 639, 640, 641, 642, 643, 644, 645, 646, 647, 647};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 2, 39, 128, 195}, {0, 0, 1, 0, 27, 85, 160, 264}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {70, 73, 71, 64, 64, 71, 73, 70};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 1, 0, 19, 96, 0}, {0, 0, 0, 0, 1, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 4, 7, 20, 102, 181, 320}, {0, 0, 0, 0, 36, 89, 256, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 27, 13, 38, 98, 182}, {0, 0, 0, 10, 11, 27, 61, 76}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-30, -20, -26, -2, -31, -9, -15, -21, 0, -23, -39, -15, 0, -14, -5, -37, -10, -3, -17, -60, 0}, {0, 0, 0, -1, -9, -4, 0, -9, -5, -17, -22, -7, -3, -11, -15, -18, -16, -27, -51, -38, -83}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-59, -69, -46, -43, -40, -46, -55, -48, -25, -27, -29, -42, -6, 0, -2, -32, 0, -0, -62, -35, -83}, {-10, -5, 0, -7, -4, -1, -15, -8, -6, -4, -5, -28, -13, -9, -0, -52, -23, -14, -52, -36, -83}};
const int Params::DOUBLED_PAWNS[2][8] = {{-39, 0, -14, -9, -9, -14, 0, -39}, {-40, -17, -23, -18, -18, -23, -17, -40}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -21, 0, -0, -0, 0, -21, 0}, {-75, -69, -36, -37, -37, -36, -69, -75}};
const int Params::ISOLATED_PAWN[2][8] = {{0, -0, -19, -24, -24, -19, -0, 0}, {-6, -10, -12, -18, -18, -12, -10, -6}};

const int Params::KNIGHT_PST[2][64] = {{-85, -28, -13, -7, -7, -13, -28, -85, -27, -25, -16, 5, 5, -16, -25, -27, -45, -19, 1, 26, 26, 1, -19, -45, -18, -0, 20, 22, 22, 20, -0, -18, -30, -11, 20, 15, 15, 20, -11, -30, -54, -47, -6, 2, 2, -6, -47, -54, -105, -81, -31, -27, -27, -31, -81, -105, -128, -128, -128, -128, -128, -128, -128, -128}, {-80, -43, -31, -33, -33, -31, -43, -80, -37, -26, -25, -17, -17, -25, -26, -37, -47, -29, -20, 3, 3, -20, -29, -47, -23, -11, 7, 17, 17, 7, -11, -23, -33, -25, -4, 1, 1, -4, -25, -33, -54, -35, -18, -12, -12, -18, -35, -54, -72, -49, -45, -34, -34, -45, -49, -72, -128, -74, -74, -62, -62, -74, -74, -128}};
const int Params::BISHOP_PST[2][64] = {{11, 39, 26, 25, 25, 26, 39, 11, 31, 42, 33, 26, 26, 33, 42, 31, 8, 36, 34, 26, 26, 34, 36, 8, -3, 5, 15, 34, 34, 15, 5, -3, -39, -16, -10, 17, 17, -10, -16, -39, 0, -31, -46, -33, -33, -46, -31, 0, -37, -68, -66, -104, -104, -66, -68, -37, -25, -101, -128, -128, -128, -128, -101, -25}, {-16, -3, 2, -3, -3, 2, -3, -16, 5, 6, 2, 8, 8, 2, 6, 5, -2, 9, 15, 18, 18, 15, 9, -2, -3, 5, 19, 24, 24, 19, 5, -3, -23, -8, -8, 2, 2, -8, -8, -23, -2, -16, -15, -16, -16, -15, -16, -2, -10, -15, -23, -26, -26, -23, -15, -10, -5, -6, -11, -32, -32, -11, -6, -5}};
const int Params::ROOK_PST[2][64] = {{-71, -58, -52, -49, -49, -52, -58, -71, -84, -58, -54, -49, -49, -54, -58, -84, -83, -59, -59, -57, -57, -59, -59, -83, -81, -67, -63, -54, -54, -63, -67, -81, -59, -53, -39, -36, -36, -39, -53, -59, -46, -22, -29, -17, -17, -29, -22, -46, -45, -49, -25, -23, -23, -25, -49, -45, -38, -35, -128, -102, -102, -128, -35, -38}, {-29, -30, -21, -27, -27, -21, -30, -29, -40, -33, -25, -26, -26, -25, -33, -40, -36, -31, -24, -27, -27, -24, -31, -36, -24, -17, -11, -16, -16, -11, -17, -24, -9, -7, -1, -5, -5, -1, -7, -9, -3, 2, 6, 4, 4, 6, 2, -3, -14, -11, -5, -4, -4, -5, -11, -14, 8, 6, 6, 2, 2, 6, 6, 8}};
const int Params::QUEEN_PST[2][64] = {{-7, -7, -11, 3, 3, -11, -7, -7, -13, 4, 6, 7, 7, 6, 4, -13, -13, 0, 5, -5, -5, 5, 0, -13, -0, 4, 5, 5, 5, 5, 4, -0, 3, 5, 9, 15, 15, 9, 5, 3, -5, 15, 19, 14, 14, 19, 15, -5, -4, -21, 1, 1, 1, 1, -21, -4, 4, 7, -19, -31, -31, -19, 7, 4}, {-103, -114, -97, -84, -84, -97, -114, -103, -96, -86, -79, -62, -62, -79, -86, -96, -80, -51, -40, -49, -49, -40, -51, -80, -50, -30, -20, -18, -18, -20, -30, -50, -41, -9, 1, 14, 14, 1, -9, -41, -45, -13, 10, 25, 25, 10, -13, -45, -30, -8, 16, 26, 26, 16, -8, -30, -68, -52, -52, -44, -44, -52, -52, -68}};
const int Params::KING_PST[2][64] = {{39, 55, -6, 15, 15, -6, 55, 39, 79, 81, 26, 49, 49, 26, 81, 79, -5, 20, -54, -119, -119, -54, 20, -5, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -105, -74, -128, -128, -128, -128, -74, -105, 128, -44, -128, -128, -128, -128, -44, 128, -91, -128, 128, 48, 48, 128, -128, -91}, {-28, -25, -30, -35, -35, -30, -25, -28, -13, -19, -25, -25, -25, -25, -19, -13, -23, -25, -21, -20, -20, -21, -25, -23, -21, -15, -10, -7, -7, -10, -15, -21, -6, 2, 10, 11, 11, 10, 2, -6, 1, 14, 16, 17, 17, 16, 14, 1, -9, 6, 8, 7, 7, 8, 6, -9, -37, -13, -10, -10, -10, -10, -13, -37}};
const int Params::KNIGHT_MOBILITY[9] = {-78, -33, -11, 1, 10, 19, 22, 20, 13};
const int Params::BISHOP_MOBILITY[15] = {-58, -31, -17, -6, 5, 12, 16, 20, 25, 28, 28, 31, 40, 24, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-74, -51, -38, -34, -33, -26, -22, -14, -10, -7, -5, -1, 9, 17, 7}, {-96, -66, -47, -29, -13, -7, -7, -3, 3, 7, 12, 16, 18, 7, 0}};
const int Params::QUEEN_MOBILITY[2][24] = {{-96, -40, -27, -16, -8, -2, 5, 11, 16, 21, 23, 24, 27, 27, 32, 35, 34, 50, 60, 70, 95, 93, 96, 96}, {-96, -96, -96, -96, -86, -67, -53, -47, -39, -35, -32, -25, -23, -20, -17, -20, -18, -21, -27, -28, -24, -28, -35, -42}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-82, -44, -25, -14, -11};
const int Params::KNIGHT_OUTPOST[2][2] = {{26, 62}, {20, 31}};
const int Params::BISHOP_OUTPOST[2][2] = {{35, 65}, {35, 26}};
const int Params::PAWN_STORM[4][2] = {{0, 24},{10, 1},{33, 1},{4, 0}};

