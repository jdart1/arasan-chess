// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 21-Feb-2017 08:34:26 by "tuner -o adam -c 24 -n 150 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 36, 189, 62};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 825, 423};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2167, -1196};
const int Scoring::Params::CASTLING[6] = {100, -218, -201, 0, 74, -123};
const int Scoring::Params::KING_COVER[6][4] = {{103, 205, 120, 106},
{40, 80, 46, 41},
{-61, -123, -72, -64},
{-27, -54, -32, -28},
{-102, -204, -119, -106},
{-262, -523, -305, -271}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 440;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -193;
const int Scoring::Params::MINOR_FOR_PAWNS = 183;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 236;
const int Scoring::Params::PAWN_ENDGAME2 = 295;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 14;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 14;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 28;
const int Scoring::Params::MINOR_ATTACK_BOOST = 43;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 47;
const int Scoring::Params::ROOK_ATTACK_BOOST = 23;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 34;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 39;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 55;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 61;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 25;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 79;
const int Scoring::Params::PIECE_THREAT_MM_MID = 156;
const int Scoring::Params::PIECE_THREAT_MR_MID = 248;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 196;
const int Scoring::Params::PIECE_THREAT_MM_END = 202;
const int Scoring::Params::PIECE_THREAT_MR_END = 397;
const int Scoring::Params::PIECE_THREAT_MQ_END = 613;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 74;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 177;
const int Scoring::Params::PIECE_THREAT_RM_MID = 126;
const int Scoring::Params::PIECE_THREAT_RR_MID = 76;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 391;
const int Scoring::Params::PIECE_THREAT_RM_END = 129;
const int Scoring::Params::PIECE_THREAT_RR_END = 112;
const int Scoring::Params::PIECE_THREAT_RQ_END = 87;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 105;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 179;
const int Scoring::Params::ENDGAME_KING_THREAT = 258;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 291;
const int Scoring::Params::BISHOP_PAIR_END = 503;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -67;
const int Scoring::Params::BAD_BISHOP_MID = -39;
const int Scoring::Params::BAD_BISHOP_END = -49;
const int Scoring::Params::CENTER_PAWN_BLOCK = -242;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -25;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -179;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -183;
const int Scoring::Params::SPACE = 36;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 195;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 210;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 471;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 182;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 147;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 48;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 205;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 89;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 24;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 535;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 456;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -218;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -437;
const int Scoring::Params::WRONG_COLOR_BISHOP = -11;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -44;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {22, 26, 24};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 27, 48, 68, 71};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 190, 412, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 191, 141};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {2, 5, 8, 11, 14, 18, 21, 25, 29, 33, 38, 42, 47, 52, 58, 64, 70, 76, 83, 90, 97, 105, 113, 122, 131, 140, 150, 161, 172, 184, 196, 209, 222, 236, 251, 267, 283, 300, 318, 337, 357, 377, 399, 421, 445, 469, 495, 521, 549, 578, 608, 640, 672, 706, 741, 778, 816, 855, 896, 938, 981, 1026, 1072, 1120, 1169, 1220, 1271, 1324, 1379, 1435, 1491, 1550, 1609, 1669, 1731, 1793, 1856, 1920, 1985, 2051, 2117, 2183, 2250, 2317, 2385, 2452, 2519, 2587, 2654, 2721, 2787, 2853, 2919, 2984, 3048, 3111, 3173, 3235, 3295, 3354, 3413, 3469, 3525, 3580, 3633, 3684, 3735, 3784, 3832, 3878, 3923, 3966, 4008, 4049, 4088, 4126, 4163, 4198, 4232, 4264, 4296, 4326, 4355, 4383, 4409, 4435, 4459, 4483, 4505, 4527, 4547, 4567, 4586, 4604, 4621, 4637, 4653, 4668, 4682, 4695, 4708, 4720, 4732, 4743, 4754, 4764, 4773, 4782, 4791, 4799};
const int Scoring::Params::TRADE_DOWN[8] = {999, 797, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 218, 733, 822}, {0, 0, 0, 0, 203, 569, 1187, 1710}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {79, 81, 76, 70, 70, 76, 81, 79};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 22, 630, 0}, {0, 0, 0, 0, 9, 145, 641, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 103, 500, 1000, 1500}, {0, 0, 0, 5, 290, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 193, 672, 1500}, {0, 0, 0, 50, 74, 234, 410, 796}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-295, -245, -201, -133, -253, -4, -166, -256, -112, -268, -58, -202, 0, -268, -243, -109, -96, -333, 0, -333, 0}, {0, 0, 0, -137, -81, 0, -7, -68, -131, -138, -180, -99, -99, -91, -256, -116, -90, -99, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -328, -333, -333, -333, -311, -267, -283, -333, -92, -8, -22, -195, -10, -35, -333, -132, -333}, {-129, -103, -81, -67, -105, -141, -158, -108, -68, -61, -83, -220, -92, -42, -103, -333, -189, -124, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -22, -189, -160, -160, -189, -22, -250}, {-250, -160, -226, -172, -172, -226, -160, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-374, -188, -152, 0, 0, -152, -188, -374}, {0, -432, -266, -392, -392, -266, -432, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -73, -90, -90, -73, 0, 0}, {-49, -75, -84, -130, -130, -84, -75, -49}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -257, -88, -60, -60, -88, -257, -500, -141, -195, -156, 4, 4, -156, -195, -141, -257, -92, -20, 67, 67, -20, -92, -257, -157, -103, 43, 34, 34, 43, -103, -157, 16, -92, 48, 10, 10, 48, -92, 16, -125, -379, -351, -87, -87, -351, -379, -125, -303, -277, 8, -384, -384, 8, -277, -303, -500, -500, -500, -500, -500, -500, -500, -500}, {-376, -185, -181, -142, -142, -181, -185, -376, -127, -161, -60, -35, -35, -60, -161, -127, -180, -105, -70, 79, 79, -70, -105, -180, -60, -31, 129, 143, 143, 129, -31, -60, 23, -55, 42, 19, 19, 42, -55, 23, -107, -325, -210, -49, -49, -210, -325, -107, -264, -99, -94, 28, 28, -94, -99, -264, -500, -207, -309, -212, -212, -309, -207, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-32, 222, 36, 54, 54, 36, 222, -32, 115, 157, 177, 86, 86, 177, 157, 115, -11, 110, 102, 105, 105, 102, 110, -11, -63, -129, 12, 97, 97, 12, -129, -63, -268, -105, -83, -28, -28, -83, -105, -268, 93, -308, -500, -329, -329, -500, -308, 93, -174, -383, -403, -500, -500, -403, -383, -174, -185, -500, -500, -500, -500, -500, -500, -185}, {-153, -81, 11, -29, -29, 11, -81, -153, -21, 17, 11, 39, 39, 11, 17, -21, -63, 12, 77, 112, 112, 77, 12, -63, -35, 29, 98, 148, 148, 98, 29, -35, -72, -33, -56, -24, -24, -56, -33, -72, 201, -184, -246, -212, -212, -246, -184, 201, 28, 87, -168, 11, 11, -168, 87, 28, 130, 234, 310, 61, 61, 310, 234, 130}};
const int Scoring::Params::ROOK_PST[2][64] = {{-308, -222, -174, -146, -146, -174, -222, -308, -348, -169, -181, -169, -169, -181, -169, -348, -295, -169, -256, -263, -263, -256, -169, -295, -324, -280, -286, -218, -218, -286, -280, -324, -193, -149, -146, -120, -120, -146, -149, -193, -157, 46, -59, -45, -45, -59, 46, -157, -64, -239, 27, -36, -36, 27, -239, -64, -52, -169, -500, -500, -500, -500, -169, -52}, {29, 58, 107, 65, 65, 107, 58, 29, -13, 32, 85, 64, 64, 85, 32, -13, 2, 59, 71, 53, 53, 71, 59, 2, 82, 124, 132, 111, 111, 132, 124, 82, 171, 171, 214, 191, 191, 214, 171, 171, 228, 271, 268, 243, 243, 268, 271, 228, 135, 169, 232, 197, 197, 232, 169, 135, 238, 279, 289, 183, 183, 289, 279, 238}};
const int Scoring::Params::QUEEN_PST[2][64] = {{37, 53, 58, 155, 155, 58, 53, 37, 117, 131, 159, 188, 188, 159, 131, 117, 52, 112, 138, 64, 64, 138, 112, 52, 77, 111, 65, 95, 95, 65, 111, 77, 103, 69, 89, 109, 109, 89, 69, 103, 69, 83, 129, 101, 101, 129, 83, 69, -52, -108, -23, -62, -62, -23, -108, -52, -25, 164, 77, 94, 94, 77, 164, -25}, {54, -68, -42, -15, -15, -42, -68, 54, 63, 6, 25, 150, 150, 25, 6, 63, 61, 228, 265, 268, 268, 265, 228, 61, 221, 340, 356, 415, 415, 356, 340, 221, 317, 486, 440, 500, 500, 440, 486, 317, 329, 480, 500, 500, 500, 500, 480, 329, 409, 500, 500, 500, 500, 500, 500, 409, 231, 254, 342, 444, 444, 342, 254, 231}};
const int Scoring::Params::KING_PST[2][64] = {{182, 290, 20, 84, 84, 20, 290, 182, 388, 323, 109, 212, 212, 109, 323, 388, 66, 139, -137, -274, -274, -137, 139, 66, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -427, -500, -500, -500, -500, -427, -500, -500, 322, -500, -500, -500, -500, 322, -500}, {-406, -375, -394, -419, -419, -394, -375, -406, -284, -307, -343, -331, -331, -343, -307, -284, -334, -318, -279, -263, -263, -279, -318, -334, -289, -240, -148, -115, -115, -148, -240, -289, -167, -84, -13, 49, 49, -13, -84, -167, -86, 7, 105, 137, 137, 105, 7, -86, -44, 79, 86, 79, 79, 86, 79, -44, -398, -83, -27, -108, -108, -27, -83, -398}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-458, -236, -108, -47, 10, 67, 96, 118, 79};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-313, -177, -89, -23, 34, 79, 115, 144, 179, 201, 206, 255, 217, 194, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -222, -169, -141, -125, -77, -39, 36, 74, 94, 112, 139, 220, 187, 209}, {-333, -153, -23, 77, 126, 156, 152, 184, 231, 256, 297, 324, 333, 333, 324}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-199, -9, -4, 28, 67, 117, 156, 175, 203, 241, 241, 276, 270, 259, 263, 252, 300, 333, 333, 333, 333, 333, 177, 333, 333, -333, -270, -333, 210}, {-333, -333, -333, -256, -179, -2, 101, 132, 226, 197, 258, 268, 276, 333, 333, 333, 333, 333, 333, 333, 321, 333, 333, 333, 333, 14, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -168, -46, 33, 57};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 163, 136, 223, 223, 136, 163, 13, 0, 333, 333, 199, 199, 333, 333, 0, 31, 18, 34, 0, 0, 34, 18, 31, 0, 0, 0, 24, 24, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 221, 265, 333, 333, 265, 221, 48, 28, 333, 333, 333, 333, 333, 333, 28, 0, 47, 158, 277, 277, 158, 47, 0, 0, 276, 333, 0, 0, 333, 276, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 62, 289, 333, 333, 289, 62, 10, 10, 182, 333, 333, 333, 333, 182, 10, 10, 321, 0, 333, 333, 0, 321, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 163, 109, 223, 275, 275, 223, 109, 163, 183, 298, 333, 333, 333, 333, 298, 183, 121, 57, 193, 0, 0, 193, 57, 121, 147, 115, 24, 0, 0, 24, 115, 147},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 202, 197, 294, 294, 197, 202, 0, 0, 282, 333, 333, 333, 333, 282, 0, 0, 6, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 254, 218, 246, 246, 218, 254, 10, 10, 2, 333, 203, 203, 333, 2, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 290, 252, 333, 333, 252, 290, 10}};
