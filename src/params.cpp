// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 18-Feb-2017 04:54:17 by "tuner -n 150 -c 24 -o adam /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 44, 197, 73};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 811, 401};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2166, -1188};
const int Scoring::Params::CASTLING[6] = {100, -213, -188, 0, 66, -147};
const int Scoring::Params::KING_COVER[6][4] = {{41, 81, 50, 27},
{5, 10, 6, 3},
{-88, -177, -109, -59},
{-53, -107, -66, -36},
{-150, -300, -185, -100},
{-214, -428, -264, -143}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 452;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -210;
const int Scoring::Params::MINOR_FOR_PAWNS = 190;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 232;
const int Scoring::Params::PAWN_ENDGAME2 = 295;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 14;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 14;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 28;
const int Scoring::Params::MINOR_ATTACK_BOOST = 43;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 49;
const int Scoring::Params::ROOK_ATTACK_BOOST = 24;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 36;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 38;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 54;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 59;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 22;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 78;
const int Scoring::Params::PIECE_THREAT_MM_MID = 157;
const int Scoring::Params::PIECE_THREAT_MR_MID = 255;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 195;
const int Scoring::Params::PIECE_THREAT_MM_END = 203;
const int Scoring::Params::PIECE_THREAT_MR_END = 414;
const int Scoring::Params::PIECE_THREAT_MQ_END = 622;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 72;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 177;
const int Scoring::Params::PIECE_THREAT_RM_MID = 124;
const int Scoring::Params::PIECE_THREAT_RR_MID = 68;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 391;
const int Scoring::Params::PIECE_THREAT_RM_END = 127;
const int Scoring::Params::PIECE_THREAT_RR_END = 92;
const int Scoring::Params::PIECE_THREAT_RQ_END = 102;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 101;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 179;
const int Scoring::Params::ENDGAME_KING_THREAT = 257;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 293;
const int Scoring::Params::BISHOP_PAIR_END = 497;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -63;
const int Scoring::Params::BAD_BISHOP_MID = -39;
const int Scoring::Params::BAD_BISHOP_END = -49;
const int Scoring::Params::CENTER_PAWN_BLOCK = -229;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -21;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -177;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -182;
const int Scoring::Params::SPACE = 36;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 4;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 195;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 196;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 467;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 187;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 145;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 49;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 205;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 87;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 24;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 549;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 458;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -223;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -442;
const int Scoring::Params::WRONG_COLOR_BISHOP = -11;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -44;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {22, 26, 23};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 29, 53, 63, 88};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 188, 415, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 191, 140};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {2, 5, 8, 11, 14, 18, 21, 25, 29, 33, 38, 42, 47, 52, 58, 64, 70, 76, 83, 90, 97, 105, 113, 122, 131, 140, 150, 161, 172, 184, 196, 209, 222, 236, 251, 267, 283, 300, 318, 337, 357, 377, 399, 421, 445, 469, 495, 521, 549, 578, 608, 640, 672, 706, 741, 778, 816, 855, 896, 938, 981, 1026, 1072, 1120, 1169, 1220, 1271, 1324, 1379, 1435, 1491, 1550, 1609, 1669, 1731, 1793, 1856, 1920, 1985, 2051, 2117, 2183, 2250, 2317, 2385, 2452, 2519, 2587, 2654, 2721, 2787, 2853, 2919, 2984, 3048, 3111, 3173, 3235, 3295, 3354, 3413, 3469, 3525, 3580, 3633, 3684, 3735, 3784, 3832, 3878, 3923, 3966, 4008, 4049, 4088, 4126, 4163, 4198, 4232, 4264, 4296, 4326, 4355, 4383, 4409, 4435, 4459, 4483, 4505, 4527, 4547, 4567, 4586, 4604, 4621, 4637, 4653, 4668, 4682, 4695, 4708, 4720, 4732, 4743, 4754, 4764, 4773, 4782, 4791, 4799};
const int Scoring::Params::TRADE_DOWN[8] = {988, 795, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 218, 733, 816}, {0, 0, 0, 0, 203, 571, 1188, 1714}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {79, 81, 76, 70, 70, 76, 81, 79};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 18, 615, 0}, {0, 0, 0, 0, 11, 147, 648, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 100, 500, 1000, 1500}, {0, 0, 0, 8, 291, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 198, 678, 1500}, {0, 0, 0, 51, 77, 235, 411, 795}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-316, -260, -214, -149, -254, -10, -170, -269, -129, -274, -33, -200, 0, -278, -276, -105, -103, -333, 0, -333, 0}, {0, 0, 0, -134, -79, 0, -5, -65, -129, -142, -173, -97, -94, -90, -244, -121, -90, -101, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -317, -277, -293, -333, -93, -6, -24, -193, -7, -36, -333, -133, -333}, {-128, -98, -81, -65, -100, -139, -158, -106, -64, -59, -77, -220, -91, -40, -103, -333, -186, -124, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -4, -181, -142, -142, -181, -4, -250}, {-250, -159, -225, -172, -172, -225, -159, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-422, -179, -163, 0, 0, -163, -179, -422}, {0, -433, -260, -391, -391, -260, -433, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -70, -86, -86, -70, 0, 0}, {-48, -75, -85, -131, -131, -85, -75, -48}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-488, -245, -75, -48, -48, -75, -245, -488, -123, -180, -138, 13, 13, -138, -180, -123, -245, -81, -8, 76, 76, -8, -81, -245, -145, -95, 55, 44, 44, 55, -95, -145, 21, -88, 61, 21, 21, 61, -88, 21, -121, -376, -341, -89, -89, -341, -376, -121, -300, -271, 1, -373, -373, 1, -271, -300, -500, -500, -500, -500, -500, -500, -500, -500}, {-374, -190, -184, -144, -144, -184, -190, -374, -131, -163, -62, -40, -40, -62, -163, -131, -183, -108, -74, 73, 73, -74, -108, -183, -64, -37, 125, 138, 138, 125, -37, -64, 22, -60, 37, 17, 17, 37, -60, 22, -110, -332, -216, -57, -57, -216, -332, -110, -266, -103, -93, 21, 21, -93, -103, -266, -500, -216, -310, -214, -214, -310, -216, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-17, 231, 49, 70, 70, 49, 231, -17, 131, 175, 193, 92, 92, 193, 175, 131, -1, 121, 115, 112, 112, 115, 121, -1, -65, -120, 23, 103, 103, 23, -120, -65, -275, -101, -76, -18, -18, -76, -101, -275, 75, -336, -500, -328, -328, -500, -336, 75, -191, -379, -415, -500, -500, -415, -379, -191, -175, -500, -500, -500, -500, -500, -500, -175}, {-157, -86, 7, -32, -32, 7, -86, -157, -26, 10, 7, 36, 36, 7, 10, -26, -67, 9, 73, 107, 107, 73, 9, -67, -38, 25, 94, 144, 144, 94, 25, -38, -77, -35, -61, -24, -24, -61, -35, -77, 187, -215, -249, -218, -218, -249, -215, 187, 15, 81, -182, 9, 9, -182, 81, 15, 124, 233, 321, 57, 57, 321, 233, 124}};
const int Scoring::Params::ROOK_PST[2][64] = {{-308, -227, -182, -155, -155, -182, -227, -308, -354, -175, -176, -177, -177, -176, -175, -354, -296, -172, -247, -272, -272, -247, -172, -296, -329, -287, -278, -229, -229, -278, -287, -329, -202, -160, -147, -130, -130, -147, -160, -202, -170, 35, -74, -78, -78, -74, 35, -170, -72, -253, -6, -65, -65, -6, -253, -72, -79, -184, -500, -500, -500, -500, -184, -79}, {22, 52, 102, 61, 61, 102, 52, 22, -17, 27, 78, 59, 59, 78, 27, -17, -2, 54, 65, 48, 48, 65, 54, -2, 75, 119, 126, 107, 107, 126, 119, 75, 167, 165, 210, 187, 187, 210, 165, 167, 223, 265, 264, 240, 240, 264, 265, 223, 132, 167, 230, 191, 191, 230, 167, 132, 231, 278, 281, 174, 174, 281, 278, 231}};
const int Scoring::Params::QUEEN_PST[2][64] = {{45, 60, 70, 168, 168, 70, 60, 45, 124, 142, 172, 197, 197, 172, 142, 124, 63, 127, 149, 73, 73, 149, 127, 63, 89, 118, 76, 101, 101, 76, 118, 89, 103, 71, 100, 117, 117, 100, 71, 103, 63, 74, 132, 101, 101, 132, 74, 63, -47, -95, -26, -57, -57, -26, -95, -47, -16, 198, 89, 90, 90, 89, 198, -16}, {51, -64, -42, -14, -14, -42, -64, 51, 63, 1, 25, 151, 151, 25, 1, 63, 62, 224, 261, 268, 268, 261, 224, 62, 212, 337, 353, 413, 413, 353, 337, 212, 318, 487, 439, 500, 500, 439, 487, 318, 340, 488, 500, 500, 500, 500, 488, 340, 413, 500, 500, 500, 500, 500, 500, 413, 224, 265, 341, 448, 448, 341, 265, 224}};
const int Scoring::Params::KING_PST[2][64] = {{253, 342, 85, 54, 54, 85, 342, 253, 447, 390, 210, 129, 129, 210, 390, 447, -72, 1, -250, -380, -380, -250, 1, -72, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 32, -500, -500, -500, -500, 32, -500, -500, 230, -500, -500, -500, -500, 230, -500}, {-430, -398, -416, -448, -448, -416, -398, -430, -308, -330, -367, -356, -356, -367, -330, -308, -358, -341, -302, -287, -287, -302, -341, -358, -315, -265, -172, -139, -139, -172, -265, -315, -194, -108, -36, 25, 25, -36, -108, -194, -112, -17, 79, 114, 114, 79, -17, -112, -75, 51, 63, 53, 53, 63, 51, -75, -419, -105, -53, -128, -128, -53, -105, -419}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-460, -237, -110, -48, 9, 66, 95, 120, 79};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-310, -176, -87, -21, 36, 79, 116, 143, 180, 200, 205, 254, 217, 193, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -224, -171, -145, -129, -76, -37, 38, 77, 95, 108, 135, 213, 173, 198}, {-333, -159, -30, 71, 121, 151, 148, 180, 228, 252, 294, 321, 333, 333, 322}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-182, 1, 3, 36, 73, 123, 162, 180, 208, 246, 245, 278, 271, 262, 263, 251, 304, 333, 333, 333, 322, 333, 198, 333, 333, -333, -328, -333, 210}, {-333, -333, -333, -273, -191, -10, 92, 127, 219, 190, 250, 269, 269, 333, 333, 333, 333, 333, 333, 333, 312, 333, 333, 333, 333, -28, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -182, -60, 22, 45};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 158, 131, 212, 212, 131, 158, 8, 0, 333, 333, 202, 202, 333, 333, 0, 32, 18, 31, 1, 1, 31, 18, 32, 0, 0, 0, 26, 26, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 46, 220, 264, 333, 333, 264, 220, 46, 17, 333, 333, 333, 333, 333, 333, 17, 0, 53, 165, 282, 282, 165, 53, 0, 0, 297, 333, 0, 0, 333, 297, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 60, 290, 333, 333, 290, 60, 10, 10, 203, 333, 333, 333, 333, 203, 10, 10, 323, 0, 333, 333, 0, 323, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 163, 103, 224, 271, 271, 224, 103, 163, 195, 325, 333, 333, 333, 333, 325, 195, 130, 60, 202, 0, 0, 202, 60, 130, 151, 116, 12, 0, 0, 12, 116, 151},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 201, 198, 289, 289, 198, 201, 0, 0, 313, 333, 333, 333, 333, 313, 0, 0, 10, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 251, 215, 245, 245, 215, 251, 10, 10, 10, 333, 216, 216, 333, 10, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 310, 271, 333, 333, 271, 310, 10}};
