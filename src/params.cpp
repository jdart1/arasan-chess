// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 13-Feb-2017 08:56:29 by "tuner -n 150 -c 24 -o adam /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 44, 198, 88};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 785, 365};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2489, -1295};
const int Scoring::Params::CASTLING[6] = {100, -214, -188, 0, 69, -149};
const int Scoring::Params::KING_COVER[6][4] = {{34, 69, 43, 23},
{0, 1, 1, 0},
{-93, -186, -116, -62},
{-56, -111, -70, -37},
{-150, -300, -188, -100},
{-214, -428, -268, -143}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 451;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -315;
const int Scoring::Params::MINOR_FOR_PAWNS = 205;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 230;
const int Scoring::Params::PAWN_ENDGAME2 = 246;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 14;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 14;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 28;
const int Scoring::Params::MINOR_ATTACK_BOOST = 42;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 50;
const int Scoring::Params::ROOK_ATTACK_BOOST = 23;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 36;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 37;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 53;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 60;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 21;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 81;
const int Scoring::Params::PIECE_THREAT_MM_MID = 157;
const int Scoring::Params::PIECE_THREAT_MR_MID = 259;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 196;
const int Scoring::Params::PIECE_THREAT_MM_END = 208;
const int Scoring::Params::PIECE_THREAT_MR_END = 446;
const int Scoring::Params::PIECE_THREAT_MQ_END = 654;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 72;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 169;
const int Scoring::Params::PIECE_THREAT_RM_MID = 123;
const int Scoring::Params::PIECE_THREAT_RR_MID = 67;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 387;
const int Scoring::Params::PIECE_THREAT_RM_END = 120;
const int Scoring::Params::PIECE_THREAT_RR_END = 68;
const int Scoring::Params::PIECE_THREAT_RQ_END = 149;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 102;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 162;
const int Scoring::Params::ENDGAME_KING_THREAT = 244;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 293;
const int Scoring::Params::BISHOP_PAIR_END = 510;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -76;
const int Scoring::Params::BAD_BISHOP_MID = -38;
const int Scoring::Params::BAD_BISHOP_END = -47;
const int Scoring::Params::CENTER_PAWN_BLOCK = -231;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -21;
const int Scoring::Params::WEAK_PAWN_END = -2;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -177;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -187;
const int Scoring::Params::SPACE = 33;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 10;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 202;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 256;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 450;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 188;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 148;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 92;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 215;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 84;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 19;
const int Scoring::Params::SUPPORTED_PASSER6 = 1000;
const int Scoring::Params::SUPPORTED_PASSER7 = 1500;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -32;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {22, 25, 23};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 24, 53, 65, 89};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 177, 428, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 185, 139};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {2, 5, 8, 11, 14, 18, 21, 25, 29, 33, 38, 42, 47, 52, 58, 64, 70, 76, 83, 90, 97, 105, 113, 122, 131, 140, 150, 161, 172, 184, 196, 209, 222, 236, 251, 267, 283, 300, 318, 337, 357, 377, 399, 421, 445, 469, 495, 521, 549, 578, 608, 640, 672, 706, 741, 778, 816, 855, 896, 938, 981, 1026, 1072, 1120, 1169, 1220, 1271, 1324, 1379, 1435, 1491, 1550, 1609, 1669, 1731, 1793, 1856, 1920, 1985, 2051, 2117, 2183, 2250, 2317, 2385, 2452, 2519, 2587, 2654, 2721, 2787, 2853, 2919, 2984, 3048, 3111, 3173, 3235, 3295, 3354, 3413, 3469, 3525, 3580, 3633, 3684, 3735, 3784, 3832, 3878, 3923, 3966, 4008, 4049, 4088, 4126, 4163, 4198, 4232, 4264, 4296, 4326, 4355, 4383, 4409, 4435, 4459, 4483, 4505, 4527, 4547, 4567, 4586, 4604, 4621, 4637, 4653, 4668, 4682, 4695, 4708, 4720, 4732, 4743, 4754, 4764, 4773, 4782, 4791, 4799};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 934, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 275, 786, 869}, {0, 0, 0, 0, 227, 652, 1000, 1750}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {-26, 66, -45, -90, -90, -45, 66, -26};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 18, 584, 0}, {0, 0, 0, 0, 0, 127, 611, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 98, 500, 1000, 1500}, {0, 0, 0, 0, 270, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 198, 673, 1325}, {0, 0, 0, 50, 70, 233, 547, 760}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-306, -246, -222, -145, -246, 0, -155, -258, -126, -280, 0, -157, 0, -290, -333, -107, -97, -333, 0, -333, 0}, {0, 0, 0, -151, -87, 0, -10, -70, -156, -137, -148, -87, -109, -83, -285, -109, -83, -115, 0, -16, -274}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -294, -257, -284, -286, -54, 0, 0, -184, -19, -36, -333, -197, -333}, {-141, -118, -90, -93, -118, -137, -162, -114, -63, -63, -78, -218, -90, -39, -94, -333, -196, -129, -333, -154, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -5, -184, -147, -147, -184, -5, -250}, {-250, -152, -222, -175, -175, -222, -152, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-396, -199, -154, 0, 0, -154, -199, -396}, {0, -408, -229, -303, -303, -229, -408, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -75, -87, -87, -75, 0, 0}, {-44, -76, -89, -130, -130, -89, -76, -44}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-475, -245, -71, -47, -47, -71, -245, -475, -115, -173, -137, 17, 17, -137, -173, -115, -240, -74, -1, 81, 81, -1, -74, -240, -139, -92, 58, 50, 50, 58, -92, -139, 19, -83, 66, 24, 24, 66, -83, 19, -118, -372, -336, -97, -97, -336, -372, -118, -286, -258, 0, -372, -372, 0, -258, -286, -500, -500, -500, -500, -500, -500, -500, -500}, {-425, -206, -203, -157, -157, -203, -206, -425, -170, -188, -80, -52, -52, -80, -188, -170, -197, -116, -87, 61, 61, -87, -116, -197, -83, -53, 110, 121, 121, 110, -53, -83, 1, -72, 21, 2, 2, 21, -72, 1, -117, -339, -226, -79, -79, -226, -339, -117, -280, -109, -108, 9, 9, -108, -109, -280, -500, -216, -317, -221, -221, -317, -216, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-18, 230, 49, 71, 71, 49, 230, -18, 135, 177, 195, 94, 94, 195, 177, 135, 3, 123, 120, 116, 116, 120, 123, 3, -62, -114, 24, 108, 108, 24, -114, -62, -265, -96, -73, -13, -13, -73, -96, -265, 68, -347, -500, -320, -320, -500, -347, 68, -122, -320, -228, -500, -500, -228, -320, -122, -168, -500, -500, -500, -500, -500, -500, -168}, {-170, -101, -11, -46, -46, -11, -101, -170, -79, -11, -26, 15, 15, -26, -11, -79, -87, -8, 55, 91, 91, 55, -8, -87, -59, 13, 72, 123, 123, 72, 13, -59, -95, -51, -83, -41, -41, -83, -51, -95, 171, -238, -259, -234, -234, -259, -238, 171, 54, 132, -9, -4, -4, -9, 132, 54, 94, 212, 315, 52, 52, 315, 212, 94}};
const int Scoring::Params::ROOK_PST[2][64] = {{-323, -240, -197, -173, -173, -197, -240, -323, -365, -190, -195, -194, -194, -195, -190, -365, -308, -182, -259, -288, -288, -259, -182, -308, -345, -303, -292, -243, -243, -292, -303, -345, -211, -179, -164, -149, -149, -164, -179, -211, -170, 24, -87, -98, -98, -87, 24, -170, -88, -267, -34, -96, -96, -34, -267, -88, -84, -167, -500, -500, -500, -500, -167, -84}, {4, 33, 85, 42, 42, 85, 33, 4, -54, -9, 34, 17, 17, 34, -9, -54, -21, 29, 26, 19, 19, 26, 29, -21, 47, 90, 100, 68, 68, 100, 90, 47, 135, 135, 182, 157, 157, 182, 135, 135, 192, 236, 235, 206, 206, 235, 236, 192, 95, 131, 194, 156, 156, 194, 131, 95, 222, 261, 262, 159, 159, 262, 261, 222}};
const int Scoring::Params::QUEEN_PST[2][64] = {{47, 60, 66, 166, 166, 66, 60, 47, 124, 136, 168, 191, 191, 168, 136, 124, 61, 124, 146, 71, 71, 146, 124, 61, 87, 116, 71, 99, 99, 71, 116, 87, 111, 69, 104, 115, 115, 104, 69, 111, 65, 68, 137, 92, 92, 137, 68, 65, -42, -91, -12, -55, -55, -12, -91, -42, -16, 190, 153, 99, 99, 153, 190, -16}, {13, -75, -50, -31, -31, -50, -75, 13, 63, -16, 8, 139, 139, 8, -16, 63, 53, 217, 251, 248, 248, 251, 217, 53, 203, 322, 334, 397, 397, 334, 322, 203, 289, 442, 423, 500, 500, 423, 442, 289, 308, 462, 500, 500, 500, 500, 462, 308, 386, 500, 500, 500, 500, 500, 500, 386, 209, 260, 300, 407, 407, 300, 260, 209}};
const int Scoring::Params::KING_PST[2][64] = {{282, 373, 116, 74, 74, 116, 373, 282, 477, 416, 239, 130, 130, 239, 416, 477, -62, 14, -236, -370, -370, -236, 14, -62, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -385, -500, -500, -500, -500, -385, -500, -500, -124, -500, -500, -500, -500, -124, -500}, {-452, -416, -430, -461, -461, -430, -416, -452, -323, -337, -374, -360, -360, -374, -337, -323, -368, -350, -309, -287, -287, -309, -350, -368, -323, -268, -168, -135, -135, -168, -268, -323, -198, -106, -29, 30, 30, -29, -106, -198, -119, -22, 81, 104, 104, 81, -22, -119, -67, 58, 80, 50, 50, 80, 58, -67, -409, -80, -29, -144, -144, -29, -80, -409}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-470, -247, -120, -58, -1, 54, 83, 105, 64};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-322, -186, -97, -32, 24, 68, 102, 131, 168, 190, 195, 237, 198, 156, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -229, -175, -150, -134, -81, -42, 35, 72, 90, 109, 130, 211, 178, 211}, {-333, -154, -26, 70, 124, 152, 149, 181, 228, 250, 293, 319, 333, 328, 320}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-189, -5, -2, 31, 70, 119, 159, 178, 205, 241, 243, 274, 267, 262, 265, 255, 291, 333, 333, 333, 333, 333, 186, 333, 333, -333, -333, -333, 210}, {-321, -333, -333, -261, -213, -25, 99, 120, 210, 188, 240, 254, 269, 333, 333, 333, 333, 333, 333, 333, 304, 313, 330, 333, 269, -22, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -168, -51, 30, 51};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 155, 133, 210, 210, 133, 155, 11, 0, 333, 333, 214, 214, 333, 333, 0, 23, 13, 43, 0, 0, 43, 13, 23, 0, 0, 0, 10, 10, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 66, 218, 261, 333, 333, 261, 218, 66, 0, 333, 333, 333, 333, 333, 333, 0, 58, 78, 168, 318, 318, 168, 78, 58, 0, 333, 333, 0, 0, 333, 333, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 43, 295, 333, 333, 295, 43, 10, 10, 244, 333, 333, 333, 333, 244, 10, 10, 333, 0, 333, 333, 0, 333, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 162, 98, 226, 272, 272, 226, 98, 162, 202, 333, 333, 333, 333, 333, 333, 202, 65, 0, 14, 0, 0, 14, 0, 65, 153, 110, 0, 0, 0, 0, 110, 153},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 190, 194, 287, 287, 194, 190, 0, 0, 333, 333, 333, 333, 333, 333, 0, 0, 299, 208, 213, 213, 208, 299, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 250, 207, 247, 247, 207, 250, 10, 10, 15, 259, 257, 257, 259, 15, 10, 10, 156, 171, 0, 0, 171, 156, 10, 10, 333, 333, 333, 333, 333, 333, 10}};
