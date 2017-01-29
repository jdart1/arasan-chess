// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 28-Jan-2017 01:07:49 by "tuner -n 150 -c 24 -o adam /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 36, 202, 93};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 797, 371};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2486, -1288};
const int Scoring::Params::CASTLING[6] = {100, -227, -212, 0, 78, -130};
const int Scoring::Params::KING_COVER[6][4] = {{42, 62, 33, 62},
{7, 11, 6, 11},
{-115, -171, -91, -171},
{-78, -116, -62, -116},
{-208, -310, -165, -310},
{-288, -428, -227, -428}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 436;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -302;
const int Scoring::Params::MINOR_FOR_PAWNS = 210;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 229;
const int Scoring::Params::PAWN_ENDGAME2 = 262;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 14;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 14;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 27;
const int Scoring::Params::MINOR_ATTACK_BOOST = 42;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 48;
const int Scoring::Params::ROOK_ATTACK_BOOST = 23;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 37;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 38;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 52;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 59;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 26;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 80;
const int Scoring::Params::PIECE_THREAT_MM_MID = 157;
const int Scoring::Params::PIECE_THREAT_MR_MID = 259;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 193;
const int Scoring::Params::PIECE_THREAT_MM_END = 207;
const int Scoring::Params::PIECE_THREAT_MR_END = 440;
const int Scoring::Params::PIECE_THREAT_MQ_END = 646;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 78;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 171;
const int Scoring::Params::PIECE_THREAT_RM_MID = 123;
const int Scoring::Params::PIECE_THREAT_RR_MID = 67;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 390;
const int Scoring::Params::PIECE_THREAT_RM_END = 126;
const int Scoring::Params::PIECE_THREAT_RR_END = 74;
const int Scoring::Params::PIECE_THREAT_RQ_END = 139;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 107;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 158;
const int Scoring::Params::ENDGAME_KING_THREAT = 238;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 291;
const int Scoring::Params::BISHOP_PAIR_END = 511;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -76;
const int Scoring::Params::BAD_BISHOP_MID = -40;
const int Scoring::Params::BAD_BISHOP_END = -48;
const int Scoring::Params::CENTER_PAWN_BLOCK = -262;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -21;
const int Scoring::Params::WEAK_PAWN_END = -1;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -183;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -186;
const int Scoring::Params::SPACE = 34;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 210;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 290;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 439;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 189;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 144;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 227;
const int Scoring::Params::ROOK_BEHIND_PP_END = 0;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 213;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 82;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 23;
const int Scoring::Params::SUPPORTED_PASSER6 = 1000;
const int Scoring::Params::SUPPORTED_PASSER7 = 1500;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -34;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {21, 26, 24};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 28, 37, 48, 74};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 186, 440, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 185, 138};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {2, 5, 8, 11, 14, 18, 21, 25, 29, 33, 38, 42, 47, 52, 58, 64, 70, 76, 83, 90, 97, 105, 113, 122, 131, 140, 150, 161, 172, 184, 196, 209, 222, 236, 251, 267, 283, 300, 318, 337, 357, 377, 399, 421, 445, 469, 495, 521, 549, 578, 608, 640, 672, 706, 741, 778, 816, 855, 896, 938, 981, 1026, 1072, 1120, 1169, 1220, 1271, 1324, 1379, 1435, 1491, 1550, 1609, 1669, 1731, 1793, 1856, 1920, 1985, 2051, 2117, 2183, 2250, 2317, 2385, 2452, 2519, 2587, 2654, 2721, 2787, 2853, 2919, 2984, 3048, 3111, 3173, 3235, 3295, 3354, 3413, 3469, 3525, 3580, 3633, 3684, 3735, 3784, 3832, 3878, 3923, 3966, 4008, 4049, 4088, 4126, 4163, 4198, 4232, 4264, 4296, 4326, 4355, 4383, 4409, 4435, 4459, 4483, 4505, 4527, 4547, 4567, 4586, 4604, 4621, 4637, 4653, 4668, 4682, 4695, 4708, 4720, 4732, 4743, 4754, 4764, 4773, 4782, 4791, 4799};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 925, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 50, 70, 244, 752, 871}, {0, 0, 0, 50, 208, 644, 1000, 1750}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 13, 595, 0}, {0, 0, 0, 0, 0, 128, 604, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 47, 500, 1000, 1500}, {0, 0, 0, 0, 306, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 181, 727, 1178}, {0, 0, 0, 50, 70, 254, 546, 740}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-318, -281, -217, -156, -256, 0, -240, -333, -203, -333, -192, -281, -17, -333, -333, -123, -72, -333, 0, -333, 0}, {0, 0, 0, -154, -87, -10, -73, -120, -202, -175, -186, -85, -103, -75, -272, -105, -77, -132, 0, -35, -278}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -333, -331, -333, -333, -162, -77, -97, -184, -24, -35, -333, -195, -333}, {-139, -119, -97, -90, -118, -130, -220, -178, -124, -116, -133, -216, -77, -41, -78, -333, -195, -119, -333, -147, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, 0, -170, -155, -155, -170, 0, -250}, {-250, -140, -222, -183, -183, -222, -140, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-478, -184, -161, 0, 0, -161, -184, -478}, {0, -398, -205, -295, -295, -205, -398, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -63, -90, -90, -63, 0, 0}, {-49, -78, -88, -130, -130, -88, -78, -49}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-488, -246, -71, -48, -48, -71, -246, -488, -111, -177, -142, 23, 23, -142, -177, -111, -238, -75, -1, 88, 88, -1, -75, -238, -136, -93, 60, 50, 50, 60, -93, -136, 16, -87, 59, 22, 22, 59, -87, 16, -130, -366, -341, -92, -92, -341, -366, -130, -315, -257, 15, -390, -390, 15, -257, -315, -500, -500, -500, -500, -500, -500, -500, -500}, {-406, -207, -199, -155, -155, -199, -207, -406, -175, -189, -78, -51, -51, -78, -189, -175, -197, -119, -86, 64, 64, -86, -119, -197, -81, -48, 109, 121, 121, 109, -48, -81, -5, -72, 17, -4, -4, 17, -72, -5, -113, -336, -224, -83, -83, -224, -336, -113, -266, -105, -109, 12, 12, -109, -105, -266, -500, -221, -313, -215, -215, -313, -221, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-23, 234, 44, 65, 65, 44, 234, -23, 135, 169, 195, 97, 97, 195, 169, 135, -3, 122, 116, 123, 123, 116, 122, -3, -59, -125, 23, 107, 107, 23, -125, -59, -270, -96, -84, -23, -23, -84, -96, -270, 67, -323, -500, -323, -323, -500, -323, 67, -131, -335, -235, -500, -500, -235, -335, -131, -178, -500, -500, -500, -500, -500, -500, -178}, {-170, -108, -11, -44, -44, -11, -108, -170, -81, -16, -29, 14, 14, -29, -16, -81, -89, -12, 53, 93, 93, 53, -12, -89, -60, 13, 69, 122, 122, 69, 13, -60, -108, -51, -87, -49, -49, -87, -51, -108, 173, -237, -257, -239, -239, -257, -237, 173, 86, 129, -8, -6, -6, -8, 129, 86, 102, 212, 319, 56, 56, 319, 212, 102}};
const int Scoring::Params::ROOK_PST[2][64] = {{-329, -238, -194, -168, -168, -194, -238, -329, -366, -194, -207, -193, -193, -207, -194, -366, -311, -186, -273, -286, -286, -273, -186, -311, -343, -300, -301, -241, -241, -301, -300, -343, -215, -186, -179, -152, -152, -179, -186, -215, -173, 30, -90, -84, -84, -90, 30, -173, -99, -261, -28, -82, -82, -28, -261, -99, -95, -170, -500, -500, -500, -500, -170, -95}, {17, 44, 97, 55, 55, 97, 44, 17, -42, 0, 48, 33, 33, 48, 0, -42, -12, 40, 39, 33, 33, 39, 40, -12, 58, 101, 111, 81, 81, 111, 101, 58, 143, 148, 195, 169, 169, 195, 148, 143, 204, 244, 248, 215, 215, 248, 244, 204, 103, 140, 200, 160, 160, 200, 140, 103, 237, 281, 270, 170, 170, 270, 281, 237}};
const int Scoring::Params::QUEEN_PST[2][64] = {{48, 59, 63, 163, 163, 63, 59, 48, 124, 128, 165, 194, 194, 165, 128, 124, 63, 119, 144, 76, 76, 144, 119, 63, 86, 115, 68, 102, 102, 68, 115, 86, 107, 73, 96, 110, 110, 96, 73, 107, 86, 99, 137, 91, 91, 137, 99, 86, -58, -105, -8, -43, -43, -8, -105, -58, -49, 152, 128, 138, 138, 128, 152, -49}, {22, -82, -35, -25, -25, -35, -82, 22, 58, -12, 11, 139, 139, 11, -12, 58, 62, 222, 255, 253, 253, 255, 222, 62, 212, 327, 337, 405, 405, 337, 327, 212, 308, 452, 435, 500, 500, 435, 452, 308, 312, 468, 500, 500, 500, 500, 468, 312, 407, 500, 500, 500, 500, 500, 500, 407, 224, 281, 316, 415, 415, 316, 281, 224}};
const int Scoring::Params::KING_PST[2][64] = {{242, 346, 74, 222, 222, 74, 346, 242, 438, 377, 174, 409, 409, 174, 377, 438, -75, -22, -264, -397, -397, -264, -22, -75, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 41, -500, -500, -500, -500, 41, -500}, {-454, -419, -433, -450, -450, -433, -419, -454, -330, -341, -379, -363, -363, -379, -341, -330, -371, -355, -317, -295, -295, -317, -355, -371, -324, -269, -176, -145, -145, -176, -269, -324, -197, -107, -37, 20, 20, -37, -107, -197, -122, -20, 77, 98, 98, 77, -20, -122, -66, 52, 70, 42, 42, 70, 52, -66, -415, -75, -41, -150, -150, -41, -75, -415}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-469, -246, -120, -58, -1, 55, 85, 105, 65};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-327, -189, -98, -31, 26, 71, 105, 134, 173, 194, 200, 245, 198, 156, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -227, -172, -147, -134, -84, -47, 27, 63, 82, 102, 125, 208, 165, 216}, {-333, -160, -24, 68, 124, 150, 149, 181, 228, 250, 292, 318, 333, 327, 317}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-205, -13, -5, 28, 69, 118, 158, 178, 205, 242, 244, 276, 270, 267, 265, 256, 289, 333, 333, 333, 333, 333, 152, 333, 333, -333, -333, -333, 210}, {-325, -333, -333, -243, -212, -24, 96, 120, 211, 189, 242, 257, 269, 333, 333, 333, 333, 333, 333, 333, 293, 311, 333, 333, 296, -27, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -158, -42, 40, 60};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 167, 143, 220, 220, 143, 167, 29, 0, 333, 333, 220, 220, 333, 333, 0, 18, 13, 48, 0, 0, 48, 13, 18, 0, 0, 0, 20, 20, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 78, 220, 264, 333, 333, 264, 220, 78, 10, 333, 333, 333, 333, 333, 333, 10, 266, 65, 172, 289, 289, 172, 65, 266, 0, 333, 333, 0, 0, 333, 333, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 38, 305, 333, 333, 305, 38, 10, 10, 251, 333, 333, 333, 333, 251, 10, 10, 333, 0, 333, 333, 0, 333, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 183, 107, 226, 277, 277, 226, 107, 183, 210, 333, 333, 333, 333, 333, 333, 210, 43, 0, 11, 0, 0, 11, 0, 43, 150, 117, 0, 0, 0, 0, 117, 150},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 191, 200, 296, 296, 200, 191, 0, 0, 333, 333, 333, 333, 333, 333, 0, 6, 302, 217, 191, 191, 217, 302, 6, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 235, 229, 247, 247, 229, 235, 10, 10, 12, 264, 271, 271, 264, 12, 10, 10, 157, 172, 0, 0, 172, 157, 10, 10, 333, 333, 333, 333, 333, 333, 10}};
