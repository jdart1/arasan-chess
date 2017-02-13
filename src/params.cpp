// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 13-Feb-2017 11:24:16 by "tuner -n 150 -c 24 -o adam /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 41, 199, 89};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 784, 354};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2485, -1288};
const int Scoring::Params::CASTLING[6] = {100, -214, -188, 0, 69, -148};
const int Scoring::Params::KING_COVER[6][4] = {{35, 69, 43, 23},
{1, 2, 1, 1},
{-93, -186, -116, -62},
{-56, -112, -70, -37},
{-150, -300, -188, -100},
{-214, -428, -267, -143}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 449;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -331;
const int Scoring::Params::MINOR_FOR_PAWNS = 206;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 227;
const int Scoring::Params::PAWN_ENDGAME2 = 248;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 14;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 13;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 28;
const int Scoring::Params::MINOR_ATTACK_BOOST = 42;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 49;
const int Scoring::Params::ROOK_ATTACK_BOOST = 23;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 36;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 37;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 53;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 59;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 21;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 81;
const int Scoring::Params::PIECE_THREAT_MM_MID = 157;
const int Scoring::Params::PIECE_THREAT_MR_MID = 259;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 195;
const int Scoring::Params::PIECE_THREAT_MM_END = 207;
const int Scoring::Params::PIECE_THREAT_MR_END = 444;
const int Scoring::Params::PIECE_THREAT_MQ_END = 650;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 72;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 168;
const int Scoring::Params::PIECE_THREAT_RM_MID = 123;
const int Scoring::Params::PIECE_THREAT_RR_MID = 67;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 389;
const int Scoring::Params::PIECE_THREAT_RM_END = 122;
const int Scoring::Params::PIECE_THREAT_RR_END = 70;
const int Scoring::Params::PIECE_THREAT_RQ_END = 145;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 104;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 157;
const int Scoring::Params::ENDGAME_KING_THREAT = 239;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 293;
const int Scoring::Params::BISHOP_PAIR_END = 512;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -77;
const int Scoring::Params::BAD_BISHOP_MID = -38;
const int Scoring::Params::BAD_BISHOP_END = -48;
const int Scoring::Params::CENTER_PAWN_BLOCK = -230;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -21;
const int Scoring::Params::WEAK_PAWN_END = -1;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -177;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -188;
const int Scoring::Params::SPACE = 33;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 10;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 212;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 275;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 432;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 190;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 141;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 225;
const int Scoring::Params::ROOK_BEHIND_PP_END = 0;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 215;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 83;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 20;
const int Scoring::Params::SUPPORTED_PASSER6 = 1000;
const int Scoring::Params::SUPPORTED_PASSER7 = 1500;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -33;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {22, 26, 23};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 24, 53, 65, 89};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 178, 430, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 185, 138};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {2, 5, 8, 11, 14, 18, 21, 25, 29, 33, 38, 42, 47, 52, 58, 64, 70, 76, 83, 90, 97, 105, 113, 122, 131, 140, 150, 161, 172, 184, 196, 209, 222, 236, 251, 267, 283, 300, 318, 337, 357, 377, 399, 421, 445, 469, 495, 521, 549, 578, 608, 640, 672, 706, 741, 778, 816, 855, 896, 938, 981, 1026, 1072, 1120, 1169, 1220, 1271, 1324, 1379, 1435, 1491, 1550, 1609, 1669, 1731, 1793, 1856, 1920, 1985, 2051, 2117, 2183, 2250, 2317, 2385, 2452, 2519, 2587, 2654, 2721, 2787, 2853, 2919, 2984, 3048, 3111, 3173, 3235, 3295, 3354, 3413, 3469, 3525, 3580, 3633, 3684, 3735, 3784, 3832, 3878, 3923, 3966, 4008, 4049, 4088, 4126, 4163, 4198, 4232, 4264, 4296, 4326, 4355, 4383, 4409, 4435, 4459, 4483, 4505, 4527, 4547, 4567, 4586, 4604, 4621, 4637, 4653, 4668, 4682, 4695, 4708, 4720, 4732, 4743, 4754, 4764, 4773, 4782, 4791, 4799};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 934, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 274, 789, 864}, {0, 0, 0, 0, 227, 655, 1000, 1750}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {-23, 68, -42, -86, -86, -42, 68, -23};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 18, 591, 0}, {0, 0, 0, 0, 0, 125, 610, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 96, 500, 1000, 1500}, {0, 0, 0, 0, 270, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 199, 679, 1337}, {0, 0, 0, 50, 70, 237, 547, 763}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-320, -272, -229, -151, -259, 0, -158, -272, -141, -310, -76, -175, 0, -320, -333, -122, -128, -333, 0, -333, 0}, {0, 0, 0, -148, -83, -11, -17, -67, -154, -126, -123, -92, -103, -85, -272, -108, -85, -127, 0, -19, -269}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -296, -258, -283, -288, -58, 0, 0, -184, -19, -34, -333, -205, -333}, {-133, -116, -88, -90, -112, -128, -154, -109, -60, -61, -72, -212, -84, -38, -83, -333, -193, -119, -333, -131, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -4, -184, -148, -148, -184, -4, -250}, {-250, -153, -224, -176, -176, -224, -153, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-399, -214, -168, 0, 0, -168, -214, -399}, {0, -408, -213, -299, -299, -213, -408, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -75, -86, -86, -75, 0, 0}, {-44, -76, -89, -129, -129, -89, -76, -44}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-476, -245, -71, -47, -47, -71, -245, -476, -114, -173, -138, 17, 17, -138, -173, -114, -240, -74, -2, 82, 82, -2, -74, -240, -139, -93, 58, 51, 51, 58, -93, -139, 17, -83, 66, 25, 25, 66, -83, 17, -121, -379, -336, -98, -98, -336, -379, -121, -288, -257, 0, -382, -382, 0, -257, -288, -500, -500, -500, -500, -500, -500, -500, -500}, {-422, -207, -204, -157, -157, -204, -207, -422, -175, -189, -80, -54, -54, -80, -189, -175, -201, -117, -88, 60, 60, -88, -117, -201, -84, -53, 109, 119, 119, 109, -53, -84, 3, -75, 18, 0, 0, 18, -75, 3, -115, -339, -225, -80, -80, -225, -339, -115, -280, -110, -107, 6, 6, -107, -110, -280, -500, -227, -317, -221, -221, -317, -227, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-18, 231, 49, 71, 71, 49, 231, -18, 136, 177, 196, 95, 95, 196, 177, 136, 4, 124, 121, 116, 116, 121, 124, 4, -62, -115, 25, 109, 109, 25, -115, -62, -266, -95, -74, -14, -14, -74, -95, -266, 68, -346, -500, -322, -322, -500, -346, 68, -114, -319, -233, -500, -500, -233, -319, -114, -163, -500, -500, -500, -500, -500, -500, -163}, {-171, -102, -11, -47, -47, -11, -102, -171, -82, -14, -29, 14, 14, -29, -14, -82, -89, -11, 52, 89, 89, 52, -11, -89, -60, 13, 71, 123, 123, 71, 13, -60, -99, -51, -87, -43, -43, -87, -51, -99, 170, -240, -259, -237, -237, -259, -240, 170, 55, 130, -16, -8, -8, -16, 130, 55, 94, 215, 315, 55, 55, 315, 215, 94}};
const int Scoring::Params::ROOK_PST[2][64] = {{-323, -240, -196, -173, -173, -196, -240, -323, -365, -188, -195, -194, -194, -195, -188, -365, -310, -185, -260, -287, -287, -260, -185, -310, -349, -304, -295, -246, -246, -295, -304, -349, -221, -182, -167, -154, -154, -167, -182, -221, -188, 21, -95, -105, -105, -95, 21, -188, -104, -274, -39, -101, -101, -39, -274, -104, -93, -182, -500, -500, -500, -500, -182, -93}, {12, 40, 92, 52, 52, 92, 40, 12, -46, -2, 44, 29, 29, 44, -2, -46, -14, 36, 34, 30, 30, 34, 36, -14, 52, 96, 105, 79, 79, 105, 96, 52, 138, 142, 193, 169, 169, 193, 142, 138, 199, 238, 241, 214, 214, 241, 238, 199, 93, 130, 194, 155, 155, 194, 130, 93, 227, 267, 258, 165, 165, 258, 267, 227}};
const int Scoring::Params::QUEEN_PST[2][64] = {{46, 59, 64, 164, 164, 64, 59, 46, 119, 134, 166, 189, 189, 166, 134, 119, 60, 122, 144, 69, 69, 144, 122, 60, 85, 114, 69, 97, 97, 69, 114, 85, 109, 66, 101, 115, 115, 101, 66, 109, 66, 64, 138, 87, 87, 138, 64, 66, -41, -95, -15, -50, -50, -15, -95, -41, -20, 178, 142, 91, 91, 142, 178, -20}, {16, -76, -48, -29, -29, -48, -76, 16, 62, -15, 4, 136, 136, 4, -15, 62, 53, 213, 249, 247, 247, 249, 213, 53, 202, 318, 331, 397, 397, 331, 318, 202, 291, 447, 422, 500, 500, 422, 447, 291, 306, 463, 500, 500, 500, 500, 463, 306, 390, 498, 500, 500, 500, 500, 498, 390, 194, 268, 308, 416, 416, 308, 268, 194}};
const int Scoring::Params::KING_PST[2][64] = {{281, 371, 116, 72, 72, 116, 371, 281, 476, 415, 239, 127, 127, 239, 415, 476, -66, 8, -238, -377, -377, -238, 8, -66, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 83, -500, -500, -500, -500, 83, -500, -500, -11, -500, -500, -500, -500, -11, -500}, {-455, -417, -432, -462, -462, -432, -417, -455, -328, -340, -377, -363, -363, -377, -340, -328, -371, -353, -314, -290, -290, -314, -353, -371, -325, -269, -172, -138, -138, -172, -269, -325, -204, -108, -31, 27, 27, -31, -108, -204, -124, -24, 79, 102, 102, 79, -24, -124, -75, 54, 78, 48, 48, 78, 54, -75, -407, -74, -36, -145, -145, -36, -74, -407}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-471, -247, -120, -58, -2, 54, 83, 106, 62};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-322, -186, -98, -32, 24, 68, 101, 130, 169, 189, 192, 240, 191, 150, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -229, -175, -150, -134, -81, -42, 35, 71, 89, 107, 128, 210, 171, 200}, {-333, -160, -29, 64, 122, 149, 148, 181, 228, 249, 293, 319, 333, 331, 318}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-193, -7, -3, 29, 68, 117, 156, 175, 203, 240, 241, 273, 267, 262, 262, 250, 285, 333, 333, 333, 333, 333, 185, 333, 333, -333, -333, -333, 210}, {-333, -333, -333, -265, -217, -22, 96, 125, 212, 187, 240, 260, 268, 333, 333, 333, 333, 333, 333, 333, 286, 315, 323, 333, 273, -65, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -174, -56, 25, 45};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 156, 135, 213, 213, 135, 156, 11, 0, 333, 333, 216, 216, 333, 333, 0, 22, 12, 42, 0, 0, 42, 12, 22, 0, 0, 0, 19, 19, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 69, 219, 261, 333, 333, 261, 219, 69, 2, 333, 333, 333, 333, 333, 333, 2, 66, 78, 166, 321, 321, 166, 78, 66, 0, 333, 333, 0, 0, 333, 333, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 45, 299, 333, 333, 299, 45, 10, 10, 263, 333, 333, 333, 333, 263, 10, 10, 333, 0, 333, 333, 0, 333, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 165, 100, 225, 275, 275, 225, 100, 165, 202, 333, 333, 333, 333, 333, 333, 202, 64, 0, 17, 0, 0, 17, 0, 64, 152, 110, 0, 0, 0, 0, 110, 152},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 191, 194, 289, 289, 194, 191, 0, 0, 333, 333, 333, 333, 333, 333, 0, 0, 302, 212, 215, 215, 212, 302, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 240, 228, 240, 240, 228, 240, 10, 10, 12, 258, 263, 263, 258, 12, 10, 10, 166, 0, 0, 0, 0, 166, 10, 10, 333, 333, 333, 333, 333, 333, 10}};
