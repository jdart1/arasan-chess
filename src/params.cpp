// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 12-Oct-2017 02:13:09 by "tuner -n 175 -R 50 -c 24 /home/jdart/chess/epd/big3.epd"
//

#include "params.h"

const int Params::RB_ADJUST[6] = {43, -6, 74, 22, 15, 41};
const int Params::RBN_ADJUST[6] = {64, -19, 26, -5, -34, -32};
const int Params::QR_ADJUST[5] = {53, 117, 54, -19, -96};
const int Params::KN_VS_PAWN_ADJUST[3] = {0, -290, -157};
const int Params::CASTLING[6] = {13, -28, -25, 0, 9, -14};
const int Params::KING_COVER[6][4] = {{12, 24, 13, 11},
{6, 11, 6, 5},
{-4, -8, -4, -4},
{-1, -2, -1, -1},
{-9, -17, -9, -8},
{-21, -41, -22, -20}};
const int Params::KING_COVER_BASE = -13;
const int Params::KING_DISTANCE_BASIS = 40;
const int Params::KING_DISTANCE_MULT = 10;
const int Params::PIN_MULTIPLIER_MID = 65;
const int Params::PIN_MULTIPLIER_END = 69;
const int Params::KRMINOR_VS_R = 0;
const int Params::KRMINOR_VS_R_NO_PAWNS = -179;
const int Params::KQMINOR_VS_Q = 0;
const int Params::KQMINOR_VS_Q_NO_PAWNS = -171;
const int Params::MINOR_FOR_PAWNS = 32;
const int Params::ENDGAME_PAWN_ADVANTAGE = 32;
const int Params::PAWN_ENDGAME1 = 45;
const int Params::PAWN_ENDGAME2 = 0;
const int Params::PAWN_ATTACK_FACTOR1 = 9;
const int Params::PAWN_ATTACK_FACTOR2 = 9;
const int Params::MINOR_ATTACK_FACTOR = 30;
const int Params::MINOR_ATTACK_BOOST = 39;
const int Params::ROOK_ATTACK_FACTOR = 46;
const int Params::ROOK_ATTACK_BOOST = 21;
const int Params::ROOK_ATTACK_BOOST2 = 38;
const int Params::QUEEN_ATTACK_FACTOR = 47;
const int Params::QUEEN_ATTACK_BOOST = 46;
const int Params::QUEEN_ATTACK_BOOST2 = 55;
const int Params::KING_ATTACK_COVER_BOOST_BASE = 8;
const int Params::KING_ATTACK_COVER_BOOST_SLOPE = 168;
const int Params::PAWN_THREAT_ON_PIECE_MID = 3;
const int Params::PAWN_THREAT_ON_PIECE_END = 11;
const int Params::PIECE_THREAT_MM_MID = 33;
const int Params::PIECE_THREAT_MR_MID = 74;
const int Params::PIECE_THREAT_MQ_MID = 46;
const int Params::PIECE_THREAT_MM_END = 32;
const int Params::PIECE_THREAT_MR_END = 77;
const int Params::PIECE_THREAT_MQ_END = 96;
const int Params::MINOR_PAWN_THREAT_MID = 9;
const int Params::MINOR_PAWN_THREAT_END = 23;
const int Params::PIECE_THREAT_RM_MID = 10;
const int Params::PIECE_THREAT_RR_MID = 0;
const int Params::PIECE_THREAT_RQ_MID = 79;
const int Params::PIECE_THREAT_RM_END = 18;
const int Params::PIECE_THREAT_RR_END = 0;
const int Params::PIECE_THREAT_RQ_END = 96;
const int Params::ROOK_PAWN_THREAT_MID = 14;
const int Params::ROOK_PAWN_THREAT_END = 29;
const int Params::ENDGAME_KING_THREAT = 36;
const int Params::BISHOP_TRAPPED = -188;
const int Params::BISHOP_PAIR_MID = 32;
const int Params::BISHOP_PAIR_END = 60;
const int Params::BISHOP_PAWN_PLACEMENT_END = -4;
const int Params::BAD_BISHOP_MID = -5;
const int Params::BAD_BISHOP_END = -5;
const int Params::CENTER_PAWN_BLOCK = -27;
const int Params::OUTSIDE_PASSER_MID = 14;
const int Params::OUTSIDE_PASSER_END = 32;
const int Params::WEAK_PAWN_MID = -3;
const int Params::WEAK_PAWN_END = 0;
const int Params::WEAK_ON_OPEN_FILE_MID = -22;
const int Params::WEAK_ON_OPEN_FILE_END = -22;
const int Params::SPACE = 4;
const int Params::PAWN_CENTER_SCORE_MID = 0;
const int Params::ROOK_ON_7TH_MID = 0;
const int Params::ROOK_ON_7TH_END = 23;
const int Params::TWO_ROOKS_ON_7TH_MID = 26;
const int Params::TWO_ROOKS_ON_7TH_END = 84;
const int Params::ROOK_ON_OPEN_FILE_MID = 22;
const int Params::ROOK_ON_OPEN_FILE_END = 15;
const int Params::ROOK_BEHIND_PP_MID = 0;
const int Params::ROOK_BEHIND_PP_END = 14;
const int Params::QUEEN_OUT = -29;
const int Params::PAWN_SIDE_BONUS = 16;
const int Params::KING_OWN_PAWN_DISTANCE = 9;
const int Params::KING_OPP_PAWN_DISTANCE = 3;
const int Params::QUEENING_SQUARE_CONTROL_MID = 65;
const int Params::QUEENING_SQUARE_CONTROL_END = 52;
const int Params::QUEENING_SQUARE_OPP_CONTROL_MID = -26;
const int Params::QUEENING_SQUARE_OPP_CONTROL_END = -55;
const int Params::WRONG_COLOR_BISHOP = 0;
const int Params::SIDE_PROTECTED_PAWN = -5;
const int Params::KING_OPP_PASSER_DISTANCE[6] = {0, 2, 22, 51, 84, 96};
const int Params::KING_POSITION_LOW_MATERIAL[3] ={300, 229, 158};
const int Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE] = {-2, -2, -1, -1, -1, -1, 0, 0, 0, 0, 0, 1, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 19, 21, 22, 24, 26, 28, 30, 33, 35, 38, 40, 43, 46, 49, 53, 56, 60, 64, 68, 72, 77, 82, 87, 92, 97, 103, 109, 115, 121, 128, 135, 142, 149, 157, 165, 173, 181, 190, 199, 208, 217, 226, 236, 245, 255, 265, 275, 285, 295, 305, 316, 326, 336, 346, 356, 366, 376, 385, 395, 404, 414, 423, 431, 440, 448, 457, 465, 472, 480, 487, 494, 501, 507, 514, 520, 525, 531, 536, 541, 546, 551, 555, 559, 563, 567, 571, 574, 577, 581, 583, 586, 589, 591, 594, 596, 598, 600, 602, 604, 605, 607, 608, 610, 611, 612, 614, 615, 616, 617, 618, 618, 619, 620, 621, 621, 622, 623, 623, 624, 624};
const int Params::TRADE_DOWN[8] = {45, 39, 34, 41, 23, 17, 12, 6};
const int Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 23, 91, 99}, {0, 0, 0, 0, 15, 66, 151, 224}};
const int Params::PASSED_PAWN_FILE_ADJUST[8] = {79, 82, 80, 76, 76, 80, 82, 79};
const int Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 3, 96, 0}, {0, 0, 0, 0, 0, 17, 96, 0}};
const int Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 5, 84, 167, 288}, {0, 0, 0, 0, 49, 90, 250, 320}};
const int Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 6, 0, 20, 87, 169}, {0, 0, 0, 6, 18, 35, 62, 72}};
const int Params::PP_OWN_PIECE_BLOCK[2][21] = {{-43, -37, -36, -23, -39, 0, -21, -28, -6, -31, -15, -34, -5, -28, -31, -38, -17, -43, -21, -43, 0}, {-2, 0, -4, -13, -18, -16, -4, -16, -15, -31, -30, -8, -3, -6, -25, -22, -19, -43, -43, -43, -43}};
const int Params::PP_OPP_PIECE_BLOCK[2][21] = {{-43, -43, -43, -43, -43, -43, -43, -43, -34, -39, -41, -43, -18, -1, -7, -24, 0, 0, -43, -21, -43}, {-18, -14, -6, -12, -15, -6, -21, -8, -14, -9, -7, -24, -4, -2, -2, -40, -21, -18, -43, -43, -43}};
const int Params::DOUBLED_PAWNS[2][8] = {{-47, -1, -23, -19, -19, -23, -1, -47}, {-43, -20, -30, -18, -18, -30, -20, -43}};
const int Params::TRIPLED_PAWNS[2][8] = {{0, -27, -13, 0, 0, -13, -27, 0}, {0, -54, -13, -56, -56, -13, -54, 0}};
const int Params::ISOLATED_PAWN[2][8] = {{0, 0, -11, -12, -12, -11, 0, 0}, {-5, -7, -9, -17, -17, -9, -7, -5}};

const int Params::KNIGHT_PST[2][64] = {{-73, -34, -16, -13, -13, -16, -34, -73, -22, -33, -27, -5, -5, -27, -33, -22, -41, -20, -6, 5, 5, -6, -20, -41, -26, -20, -0, 1, 1, -0, -20, -26, -17, -18, 6, -10, -10, 6, -18, -17, -34, -79, -66, -27, -27, -66, -79, -34, -70, -51, -6, -7, -7, -6, -51, -70, -128, -128, -128, -128, -128, -128, -128, -128}, {-47, -43, -31, -27, -27, -31, -43, -47, -35, -28, -19, -17, -17, -19, -28, -35, -39, -29, -21, -1, -1, -21, -29, -39, -20, -19, 5, 11, 11, 5, -19, -20, -22, -23, -4, -4, -4, -4, -23, -22, -44, -55, -35, -38, -38, -35, -55, -44, -58, -41, -33, -28, -28, -33, -41, -58, -128, -61, -66, -62, -62, -66, -61, -128}};
const int Params::BISHOP_PST[2][64] = {{6, 39, 16, 17, 17, 16, 39, 6, 19, 26, 26, 16, 16, 26, 26, 19, 1, 19, 19, 18, 18, 19, 19, 1, -10, -12, 5, 15, 15, 5, -12, -10, -37, -17, -27, -10, -10, -27, -17, -37, -14, -71, -106, -66, -66, -106, -71, -14, 1, -48, -31, -99, -99, -31, -48, 1, -29, -67, -128, -128, -128, -128, -67, -29}, {-23, -5, 0, -5, -5, 0, -5, -23, -4, 0, -3, 4, 4, -3, 0, -4, -8, -1, 8, 10, 10, 8, -1, -8, -12, 2, 10, 18, 18, 10, 2, -12, -28, -11, -7, 2, 2, -7, -11, -28, -4, -56, -53, -17, -17, -53, -56, -4, -8, -26, -32, -36, -36, -32, -26, -8, -6, 0, -27, 0, 0, -27, 0, -6}};
const int Params::ROOK_PST[2][64] = {{-76, -65, -60, -57, -57, -60, -65, -76, -85, -60, -61, -59, -59, -61, -60, -85, -77, -59, -69, -70, -70, -69, -59, -77, -79, -68, -74, -62, -62, -74, -68, -79, -65, -55, -54, -51, -51, -54, -55, -65, -65, -31, -40, -37, -37, -40, -31, -65, -53, -67, -33, -30, -30, -33, -67, -53, -42, -57, -128, -128, -128, -128, -57, -42}, {-5, -3, 4, -1, -1, 4, -3, -5, -8, -4, 3, 2, 2, 3, -4, -8, -9, -2, -1, 0, 0, -1, -2, -9, -1, 7, 9, 6, 6, 9, 7, -1, 10, 9, 14, 13, 13, 14, 9, 10, 10, 21, 21, 18, 18, 21, 21, 10, 2, 7, 15, 13, 13, 15, 7, 2, 27, 31, 30, 21, 21, 30, 31, 27}};
const int Params::QUEEN_PST[2][64] = {{12, 12, 11, 22, 22, 11, 12, 12, 18, 20, 23, 27, 27, 23, 20, 18, 9, 19, 22, 12, 12, 22, 19, 9, 18, 24, 18, 22, 22, 18, 24, 18, 20, 18, 22, 25, 25, 22, 18, 20, 10, 12, 25, 26, 26, 25, 12, 10, 3, -10, 6, 0, 0, 6, -10, 3, -15, 40, -10, 10, 10, -10, 40, -15}, {-18, -18, -14, -1, -1, -14, -18, -18, -9, -4, 1, 19, 19, 1, -4, -9, 8, 27, 38, 36, 36, 38, 27, 8, 15, 39, 51, 61, 61, 51, 39, 15, 24, 62, 70, 89, 89, 70, 62, 24, 26, 52, 94, 92, 92, 94, 52, 26, 47, 54, 85, 105, 105, 85, 54, 47, 12, 14, 38, 42, 42, 38, 14, 12}};
const int Params::KING_PST[2][64] = {{53, 68, 29, 41, 41, 29, 68, 53, 80, 76, 44, 64, 64, 44, 76, 80, 6, 48, -20, -29, -29, -20, 48, 6, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -112, -77, -128, -128, -128, -128, -77, -112, -128, -128, -27, -128, -128, -27, -128, -128}, {-84, -79, -82, -85, -85, -82, -79, -84, -68, -72, -76, -75, -75, -76, -72, -68, -73, -74, -69, -68, -68, -69, -74, -73, -69, -65, -57, -54, -54, -57, -65, -69, -56, -51, -41, -39, -39, -41, -51, -56, -56, -45, -38, -35, -35, -38, -45, -56, -54, -50, -46, -45, -45, -46, -50, -54, -85, -64, -60, -61, -61, -60, -64, -85}};

const int Params::KNIGHT_MOBILITY[9] = {-56, -23, -7, 1, 8, 15, 17, 17, 12};
const int Params::BISHOP_MOBILITY[15] = {-42, -25, -13, -5, 3, 10, 14, 16, 22, 24, 24, 29, 34, 17, 12};
const int Params::ROOK_MOBILITY[2][15] = {{-43, -28, -21, -17, -15, -9, -5, 5, 8, 10, 14, 14, 30, 31, 43}, {-43, -22, -6, 7, 15, 20, 21, 23, 29, 32, 36, 40, 42, 37, 29}};
const int Params::QUEEN_MOBILITY[2][24] = {{-25, -10, -10, -5, 0, 5, 11, 14, 17, 22, 21, 25, 25, 25, 21, 24, 30, 32, 43, 43, 43, 43, 43, 43}, {-43, -43, -43, -35, -21, -2, 16, 15, 27, 23, 27, 35, 30, 40, 38, 38, 43, 43, 42, 39, 40, 39, 33, 15}};
const int Params::KING_MOBILITY_ENDGAME[5] = {-43, -15, 4, 12, 15};

const int Params::KNIGHT_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{1, 0},{19, 19},{17, 16},{49, 14},{49, 14},{17, 16},{19, 19},{1, 0},{0, 13},{64, 32},{64, 29},{36, 48},{36, 48},{64, 29},{64, 32},{0, 13},{0, 1},{0, 10},{3, 3},{0, 10},{0, 10},{3, 3},{0, 10},{0, 1},{0, 0},{0, 0},{0, 0},{0, 11},{0, 11},{0, 0},{0, 0},{0, 0}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 12},{27, 26},{36, 30},{62, 35},{62, 35},{36, 30},{27, 26},{0, 12},{9, 42},{64, 39},{64, 50},{64, 53},{64, 53},{64, 50},{64, 39},{9, 42},{64, 0},{38, 32},{7, 23},{24, 0},{24, 0},{7, 23},{38, 32},{64, 0},{5, 0},{45, 57},{64, 64},{41, 0},{41, 0},{64, 64},{45, 57},{5, 0}}};
const int Params::BISHOP_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{17, 32},{9, 20},{45, 23},{44, 20},{44, 20},{45, 23},{9, 20},{17, 32},{48, 34},{64, 64},{64, 64},{49, 27},{49, 27},{64, 64},{64, 64},{48, 34},{8, 20},{0, 44},{1, 31},{0, 32},{0, 32},{1, 31},{0, 44},{8, 20},{25, 23},{0, 18},{0, 49},{0, 2},{0, 2},{0, 49},{0, 18},{25, 23}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{32, 24},{52, 21},{63, 23},{63, 23},{52, 21},{32, 24},{0, 0},{0, 0},{64, 62},{64, 64},{64, 19},{64, 19},{64, 64},{64, 62},{0, 0},{0, 0},{64, 0},{16, 0},{64, 0},{64, 0},{16, 0},{64, 0},{0, 0},{0, 0},{64, 0},{64, 0},{64, 0},{64, 0},{64, 0},{64, 0},{0, 0}}};


