// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 21-Jan-2017 07:59:37 by "tuner -n 120 -o adam -c 24 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {36, 14, 138, 41};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -316, -500, -506};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 868, 497};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2477, -1313};
const int Scoring::Params::CASTLING[6] = {100, -232, -207, 18, 98, -87};
const int Scoring::Params::KING_COVER[6][4] = {{18, 25, 15, 27},
{-18, -25, -15, -28},
{-122, -172, -103, -186},
{-103, -145, -87, -157},
{-213, -299, -179, -325},
{-281, -394, -236, -428}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 434;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -303;
const int Scoring::Params::MINOR_FOR_PAWNS = 217;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 219;
const int Scoring::Params::PAWN_ENDGAME2 = 243;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 14;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 12;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 28;
const int Scoring::Params::MINOR_ATTACK_BOOST = 40;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 48;
const int Scoring::Params::ROOK_ATTACK_BOOST = 22;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 39;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 35;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 50;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 64;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 27;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 78;
const int Scoring::Params::PIECE_THREAT_MM_MID = 152;
const int Scoring::Params::PIECE_THREAT_MR_MID = 234;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 194;
const int Scoring::Params::PIECE_THREAT_MM_END = 198;
const int Scoring::Params::PIECE_THREAT_MR_END = 367;
const int Scoring::Params::PIECE_THREAT_MQ_END = 575;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 76;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 166;
const int Scoring::Params::PIECE_THREAT_RM_MID = 128;
const int Scoring::Params::PIECE_THREAT_RR_MID = 100;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 389;
const int Scoring::Params::PIECE_THREAT_RM_END = 136;
const int Scoring::Params::PIECE_THREAT_RR_END = 151;
const int Scoring::Params::PIECE_THREAT_RQ_END = 123;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 104;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 152;
const int Scoring::Params::ENDGAME_KING_THREAT = 234;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 291;
const int Scoring::Params::BISHOP_PAIR_END = 527;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -87;
const int Scoring::Params::BAD_BISHOP_MID = -39;
const int Scoring::Params::BAD_BISHOP_END = -46;
const int Scoring::Params::CENTER_PAWN_BLOCK = -264;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -22;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -181;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -190;
const int Scoring::Params::SPACE = 33;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 2;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 209;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 279;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 426;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 185;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 144;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 221;
const int Scoring::Params::ROOK_BEHIND_PP_END = 0;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 213;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 75;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 22;
const int Scoring::Params::SUPPORTED_PASSER6 = 1000;
const int Scoring::Params::SUPPORTED_PASSER7 = 1500;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -33;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {13, 21, 30};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 40, 69, 60, 94};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 178, 339, 483, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 188, 141};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 45, 0, 35, 48, 18, 19, 16, 104, 46, 128, 133, 81, 93, 92, 99, 140, 153, 249, 203, 245, 284, 269, 299, 352, 323, 323, 439, 376, 439, 479, 536, 562, 580, 519, 561, 660, 677, 670, 717, 812, 797, 950, 921, 850, 982, 1106, 1061, 1033, 1165, 1186, 1082, 1210, 1377, 1213, 1429, 1591, 1408, 1400, 1963, 1696, 1569, 1643, 1718, 1674, 1986, 2192, 1959, 2253, 2368, 2017, 2068, 2322, 2219, 1993, 2700, 2696, 2593, 2866, 2985, 2658, 2751, 3085, 3053, 2830, 3704, 3315, 3351, 3727, 3659, 2810, 3768, 3989, 3316, 3768, 3658, 3555, 3172, 4257, 4292, 3415, 3652, 3438, 3471, 4452, 4201, 4509, 4537, 3982, 4107, 3739, 3601, 3668, 4143, 4440, 3515, 3625, 3262, 2933, 3320, 3063, 3300, 3537, 3413, 3242, 2441, 3421, 4757, 4465, 4210, 3816, 4393, 4009, 4941, 4945, 4754, 4949};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 935, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 50, 70, 272, 748, 870}, {0, 0, 0, 50, 192, 500, 1000, 1750}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 2, 583, 0}, {0, 0, 0, 0, 0, 120, 601, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 50, 70, 500, 1000, 1500}, {0, 0, 0, 50, 306, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 189, 729, 1365}, {0, 0, 0, 50, 70, 317, 574, 694}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-284, -281, -210, -139, -208, 0, -243, -312, -231, -289, -159, -270, 0, -288, -280, -127, -126, -333, 0, -333, 0}, {0, 0, 0, -155, -98, -10, -78, -130, -207, -189, -193, -81, -99, -71, -263, -13, 0, -22, -9, -59, -325}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -329, -326, -314, -333, -333, -321, -306, -296, -333, -151, -77, -91, -212, -41, -72, -333, -194, -333}, {-141, -129, -101, -99, -130, -150, -220, -186, -138, -134, -150, -215, -76, -45, -83, -277, -124, -33, -333, -183, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, 0, -173, -151, -151, -173, 0, -250}, {-250, -140, -223, -180, -180, -223, -140, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-440, -181, -127, 0, 0, -127, -181, -440}, {0, -393, -201, -297, -297, -201, -393, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -72, -84, -84, -72, 0, 0}, {-46, -75, -85, -129, -129, -85, -75, -46}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-478, -245, -80, -51, -51, -80, -245, -478, -109, -183, -140, 18, 18, -140, -183, -109, -243, -82, -9, 78, 78, -9, -82, -243, -149, -114, 50, 38, 38, 50, -114, -149, 6, -102, 55, 9, 9, 55, -102, 6, -145, -375, -339, -71, -71, -339, -375, -145, -335, -268, 1, -356, -356, 1, -268, -335, -500, -500, -500, -500, -500, -500, -500, -500}, {-440, -231, -234, -186, -186, -234, -231, -440, -199, -222, -114, -82, -82, -114, -222, -199, -236, -150, -118, 30, 30, -118, -150, -236, -110, -75, 75, 88, 88, 75, -75, -110, -34, -97, -8, -35, -35, -8, -97, -34, -149, -351, -221, -64, -64, -221, -351, -149, -293, -138, -115, -18, -18, -115, -138, -293, -500, -259, -343, -240, -240, -343, -259, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-25, 225, 50, 71, 71, 50, 225, -25, 140, 170, 199, 100, 100, 199, 170, 140, -9, 129, 120, 121, 121, 120, 129, -9, -55, -127, 21, 103, 103, 21, -127, -55, -276, -100, -102, -19, -19, -102, -100, -276, 86, -334, -500, -317, -317, -500, -334, 86, -166, -327, -257, -500, -500, -257, -327, -166, -174, -447, -500, -500, -500, -500, -447, -174}, {-205, -138, -44, -75, -75, -44, -138, -205, -111, -46, -60, -16, -16, -60, -46, -111, -123, -42, 23, 63, 63, 23, -42, -123, -96, -20, 34, 91, 91, 34, -20, -96, -144, -83, -123, -77, -77, -123, -83, -144, 188, -247, -287, -242, -242, -287, -247, 188, 54, 102, -31, -37, -37, -31, 102, 54, 97, 185, 278, 19, 19, 278, 185, 97}};
const int Scoring::Params::ROOK_PST[2][64] = {{-282, -191, -144, -116, -116, -144, -191, -282, -316, -154, -155, -142, -142, -155, -154, -316, -274, -151, -227, -235, -235, -227, -151, -274, -306, -267, -260, -201, -201, -260, -267, -306, -184, -154, -130, -99, -99, -130, -154, -184, -152, 41, -42, -69, -69, -42, 41, -152, -74, -216, 3, -44, -44, 3, -216, -74, -56, -116, -500, -500, -500, -500, -116, -56}, {12, 39, 90, 50, 50, 90, 39, 12, -44, -7, 42, 27, 27, 42, -7, -44, -20, 34, 34, 28, 28, 34, 34, -20, 60, 101, 108, 79, 79, 108, 101, 60, 139, 143, 192, 167, 167, 192, 143, 139, 203, 244, 245, 210, 210, 245, 244, 203, 102, 140, 198, 156, 156, 198, 140, 102, 231, 277, 268, 171, 171, 268, 277, 231}};
const int Scoring::Params::QUEEN_PST[2][64] = {{27, 35, 47, 143, 143, 47, 35, 27, 112, 110, 151, 180, 180, 151, 110, 112, 54, 102, 131, 70, 70, 131, 102, 54, 65, 95, 56, 90, 90, 56, 95, 65, 83, 40, 77, 93, 93, 77, 40, 83, 22, 52, 95, 54, 54, 95, 52, 22, -90, -135, -70, -48, -48, -70, -135, -90, -93, 118, 119, 86, 86, 119, 118, -93}, {3, -101, -78, -51, -51, -78, -101, 3, 32, -41, -16, 116, 116, -16, -41, 32, 23, 199, 233, 233, 233, 233, 199, 23, 187, 305, 309, 379, 379, 309, 305, 187, 281, 431, 415, 500, 500, 415, 431, 281, 298, 456, 500, 500, 500, 500, 456, 298, 384, 493, 500, 500, 500, 500, 493, 384, 216, 254, 305, 392, 392, 305, 254, 216}};
const int Scoring::Params::KING_PST[2][64] = {{150, 247, -24, 183, 183, -24, 247, 150, 374, 311, 138, 423, 423, 138, 311, 374, -239, -192, -427, -500, -500, -427, -192, -239, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -437, -437, -500, -500, -500, -500, 58, -500, -500, -500, -500, 58, -500}, {-389, -344, -355, -368, -368, -355, -344, -389, -261, -265, -298, -281, -281, -298, -265, -261, -302, -278, -238, -214, -214, -238, -278, -302, -256, -195, -102, -67, -67, -102, -195, -256, -133, -37, 32, 91, 91, 32, -37, -133, -60, 48, 145, 166, 166, 145, 48, -60, -2, 116, 134, 105, 105, 134, 116, -2, -346, -15, 20, -86, -86, 20, -15, -346}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-457, -232, -103, -41, 15, 72, 102, 122, 79};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-318, -182, -89, -22, 33, 80, 113, 140, 182, 200, 208, 254, 183, 177, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -213, -155, -133, -117, -68, -32, 42, 80, 92, 116, 137, 218, 177, 245}, {-333, -149, -15, 76, 131, 158, 155, 189, 234, 258, 299, 326, 333, 333, 325}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-197, -7, -8, 24, 67, 117, 157, 176, 204, 240, 238, 272, 258, 266, 270, 250, 296, 333, 333, 333, 333, 333, 209, 333, 333, -333, -333, -333, 210}, {-333, -333, -333, -199, -180, -9, 104, 129, 215, 190, 243, 259, 276, 333, 333, 333, 333, 333, 333, 333, 288, 315, 333, 333, 270, -20, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -175, -58, 19, 37};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 153, 131, 213, 213, 131, 153, 24, 0, 321, 298, 155, 155, 298, 321, 0, 11, 12, 17, 0, 0, 17, 12, 11, 0, 0, 0, 16, 16, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 73, 219, 250, 333, 333, 250, 219, 73, 18, 333, 333, 333, 333, 333, 333, 18, 236, 51, 180, 247, 247, 180, 51, 236, 0, 333, 333, 0, 0, 333, 333, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 19, 287, 333, 333, 287, 19, 10, 10, 166, 333, 333, 333, 333, 166, 10, 10, 135, 0, 333, 333, 0, 135, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 187, 104, 229, 272, 272, 229, 104, 187, 166, 321, 333, 298, 298, 333, 321, 166, 43, 0, 9, 0, 0, 9, 0, 43, 122, 103, 3, 0, 0, 3, 103, 122},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 191, 207, 292, 292, 207, 191, 0, 0, 319, 333, 333, 333, 333, 319, 0, 0, 252, 195, 160, 160, 195, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 251, 233, 243, 243, 233, 251, 10, 10, 2, 252, 224, 224, 252, 2, 10, 10, 135, 0, 0, 0, 0, 135, 10, 10, 318, 323, 293, 293, 323, 318, 10}};
