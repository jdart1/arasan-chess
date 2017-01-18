// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 17-Jan-2017 09:15:30 by "tuner -n 120 -o adam -c 24 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {99, 146, 341, 75};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 706, 484};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2200, -1294};
const int Scoring::Params::CASTLING[6] = {100, -174, -159, 1, 98, -105};
const int Scoring::Params::KING_COVER[6][4] = {{72, 54, 56, 36},
{41, 31, 32, 21},
{-97, -73, -76, -49},
{0, 0, 0, 0},
{-209, -157, -163, -105},
{-285, -214, -223, -143}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 468;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -201;
const int Scoring::Params::MINOR_FOR_PAWNS = 227;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 243;
const int Scoring::Params::PAWN_ENDGAME1 = 154;
const int Scoring::Params::PAWN_ENDGAME2 = 263;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 8;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 8;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 25;
const int Scoring::Params::MINOR_ATTACK_BOOST = 33;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 31;
const int Scoring::Params::ROOK_ATTACK_BOOST = 24;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 34;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 33;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 50;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 48;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 30;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 88;
const int Scoring::Params::PIECE_THREAT_MM_MID = 166;
const int Scoring::Params::PIECE_THREAT_MR_MID = 278;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 241;
const int Scoring::Params::PIECE_THREAT_MM_END = 263;
const int Scoring::Params::PIECE_THREAT_MR_END = 536;
const int Scoring::Params::PIECE_THREAT_MQ_END = 750;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 78;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 148;
const int Scoring::Params::PIECE_THREAT_RM_MID = 134;
const int Scoring::Params::PIECE_THREAT_RR_MID = 120;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 462;
const int Scoring::Params::PIECE_THREAT_RM_END = 134;
const int Scoring::Params::PIECE_THREAT_RR_END = 312;
const int Scoring::Params::PIECE_THREAT_RQ_END = 725;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 123;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 153;
const int Scoring::Params::ENDGAME_KING_THREAT = 376;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 287;
const int Scoring::Params::BISHOP_PAIR_END = 441;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -66;
const int Scoring::Params::BAD_BISHOP_MID = -29;
const int Scoring::Params::BAD_BISHOP_END = -50;
const int Scoring::Params::CENTER_PAWN_BLOCK = -247;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -26;
const int Scoring::Params::WEAK_PAWN_END = -1;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -174;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -182;
const int Scoring::Params::SPACE = 24;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 201;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 227;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 509;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 184;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 128;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 163;
const int Scoring::Params::ROOK_BEHIND_PP_END = 0;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 278;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 83;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 0;
const int Scoring::Params::SUPPORTED_PASSER6 = 1000;
const int Scoring::Params::SUPPORTED_PASSER7 = 1500;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -30;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {25, 27, 6};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 31, 36, 37, 42};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 192, 412, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={150, 140, 128};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 31, 0, 0, 0, 3, 30, 0, 87, 74, 15, 10, 67, 106, 200, 104, 195, 109, 113, 142, 226, 269, 365, 315, 358, 201, 393, 420, 511, 528, 567, 505, 598, 631, 618, 676, 815, 816, 823, 854, 905, 994, 1091, 952, 1098, 1192, 1214, 1261, 1413, 1391, 1425, 1470, 1632, 1686, 1518, 1800, 1857, 1916, 1976, 2036, 2097, 2160, 2066, 2286, 2315, 2417, 2417, 2549, 2591, 2678, 2572, 2752, 2825, 2925, 2641, 3044, 3077, 3035, 3214, 3269, 3324, 3377, 3429, 3481, 3232, 3581, 3499, 3678, 3697, 3624, 3778, 3632, 3605, 3395, 3360, 3341, 3418, 3595, 2999, 2850, 2633, 2543, 3163, 2671, 3011, 3233, 3443, 2676, 2861, 3326, 2581, 2649, 3432, 4393, 4335, 4037, 4137, 4477, 4297, 3803, 3573, 3586, 3599, 3611, 3622, 3633, 3643, 3652, 3661, 3669, 3676, 3683, 3689, 3694, 3699, 3703, 3706, 3709, 3711, 3712};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 269, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 50, 70, 211, 698, 843}, {0, 0, 0, 50, 153, 500, 1000, 1750}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 17, 576, 0}, {0, 0, 0, 0, 0, 97, 438, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 50, 70, 496, 1000, 1500}, {0, 0, 0, 50, 301, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 105, 666, 1080}, {0, 0, 0, 50, 70, 267, 451, 562}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-279, -287, -236, -140, -222, -70, -235, -304, -230, -287, -167, -287, -50, -267, -306, -95, -236, -214, 0, -333, 0}, {-24, 0, -20, -155, -124, 0, -99, -150, -208, -205, -107, -85, -82, -70, -105, -26, -2, 0, -50, -17, -285}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -329, -315, -321, -333, -333, -333, -306, -297, -333, -176, -111, -112, -162, 0, -20, -316, -144, -333}, {-154, -189, -148, -189, -170, -227, -230, -225, -180, -190, -204, -189, -80, -59, -97, -257, -161, -58, -333, -219, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-249, 0, -187, -141, -141, -187, 0, -249}, {-250, -164, -250, -221, -221, -250, -164, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-273, -134, -98, 0, 0, -98, -134, -273}, {0, -466, -250, -404, -404, -250, -466, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -96, -67, -67, -96, 0, 0}, {-35, -52, -72, -109, -109, -72, -52, -35}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-453, -216, -58, -28, -28, -58, -216, -453, -85, -165, -115, 56, 56, -115, -165, -85, -211, -45, 42, 118, 118, 42, -45, -211, -98, -41, 97, 97, 97, 97, -41, -98, 78, -20, 131, 100, 100, 131, -20, 78, -58, -338, -379, -46, -46, -379, -338, -58, -255, -212, 54, -381, -381, 54, -212, -255, -500, -500, -500, -500, -500, -500, -500, -500}, {-500, -335, -344, -298, -298, -344, -335, -500, -261, -304, -197, -160, -160, -197, -304, -261, -308, -229, -177, -44, -44, -177, -229, -308, -206, -165, -7, 0, 0, -7, -165, -206, -128, -162, -87, -129, -129, -87, -162, -128, -230, -483, -336, -213, -213, -336, -483, -230, -363, -204, -173, -105, -105, -173, -204, -363, -500, -347, -490, -296, -296, -490, -347, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{2, 301, 100, 108, 108, 100, 301, 2, 200, 206, 268, 153, 153, 268, 206, 200, 49, 184, 171, 191, 191, 171, 184, 49, 9, -60, 93, 165, 165, 93, -60, 9, -169, -19, -26, 31, 31, -26, -19, -169, 173, -288, -500, -283, -283, -500, -288, 173, -20, -311, -174, -500, -500, -174, -311, -20, -39, -459, -500, -500, -500, -500, -459, -39}, {-264, -208, -125, -148, -148, -125, -208, -264, -168, -88, -117, -73, -73, -117, -88, -168, -178, -96, -39, 2, 2, -39, -96, -178, -159, -88, -44, -8, -8, -44, -88, -159, -230, -142, -210, -189, -189, -210, -142, -230, 102, -377, -489, -359, -359, -489, -377, 102, 31, 4, -101, -184, -184, -101, 4, 31, 62, 138, 135, -100, -100, 135, 138, 62}};
const int Scoring::Params::ROOK_PST[2][64] = {{-128, -37, 11, 32, 32, 11, -37, -128, -171, -7, -1, -9, -9, -1, -7, -171, -97, 30, -40, -92, -92, -40, 30, -97, -137, -78, -78, -68, -68, -78, -78, -137, -1, 1, 37, 28, 28, 37, 1, -1, 33, 204, 117, 90, 90, 117, 204, 33, 95, -38, 132, 120, 120, 132, -38, 95, -36, -409, -500, -500, -500, -500, -409, -36}, {-81, -57, -5, -33, -33, -5, -57, -81, -175, -133, -81, -103, -103, -81, -133, -175, -150, -89, -96, -93, -93, -96, -89, -150, -71, -30, -22, -45, -45, -22, -30, -71, 0, 6, 60, 41, 41, 60, 6, 0, 61, 106, 107, 76, 76, 107, 106, 61, -22, -4, 67, 13, 13, 67, -4, -22, 71, 100, -135, -300, -300, -135, 100, 71}};
const int Scoring::Params::QUEEN_PST[2][64] = {{62, 76, 79, 184, 184, 79, 76, 62, 159, 145, 199, 221, 221, 199, 145, 159, 99, 147, 182, 119, 119, 182, 147, 99, 111, 161, 110, 148, 148, 110, 161, 111, 150, 101, 138, 138, 138, 138, 101, 150, 115, 107, 178, 96, 96, 178, 107, 115, -31, -67, -8, -40, -40, -8, -67, -31, 34, 260, 182, 48, 48, 182, 260, 34}, {-272, -364, -326, -265, -265, -326, -364, -272, -242, -288, -258, -148, -148, -258, -288, -242, -222, -84, -55, -41, -41, -55, -84, -222, -69, 30, 13, 91, 91, 13, 30, -69, -13, 129, 113, 282, 282, 113, 129, -13, -19, 118, 244, 295, 295, 244, 118, -19, 84, 200, 245, 329, 329, 245, 200, 84, -299, -170, -63, 31, 31, -63, -170, -299}};
const int Scoring::Params::KING_PST[2][64] = {{305, 418, 145, 123, 123, 145, 418, 305, 448, 374, 204, 160, 160, 204, 374, 448, 39, 101, -206, -338, -338, -206, 101, 39, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 500, -475, -500, -500, -475, 500, -500}, {-489, -413, -422, -463, -463, -422, -413, -489, -344, -318, -346, -320, -320, -346, -318, -344, -377, -327, -258, -214, -214, -258, -327, -377, -333, -239, -98, -35, -35, -98, -239, -333, -200, -75, 48, 132, 132, 48, -75, -200, -117, 13, 179, 215, 215, 179, 13, -117, -57, 129, 179, 156, 156, 179, 129, -57, -477, -45, 36, -105, -105, 36, -45, -477}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-449, -235, -110, -53, 0, 50, 73, 83, 27};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-329, -201, -108, -42, 15, 59, 89, 118, 154, 167, 194, 237, 197, 161, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -229, -175, -147, -133, -83, -42, 38, 81, 94, 96, 99, 174, 180, 213}, {-333, -205, -90, -9, 10, 22, 24, 53, 96, 115, 157, 188, 220, 245, 226}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-196, -10, -4, 28, 71, 126, 170, 192, 224, 262, 261, 292, 279, 282, 283, 253, 273, 314, 248, 324, 258, 333, 125, 333, 333, -333, -333, -333, 210}, {-333, -333, -333, -196, -218, -58, 42, 47, 114, 92, 126, 131, 154, 210, 199, 215, 253, 259, 250, 173, 111, 128, 129, 242, 126, -203, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -243, -141, -71, -42};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 38, 124, 115, 194, 194, 115, 124, 38, 0, 333, 333, 192, 192, 333, 333, 0, 0, 0, 7, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 82, 178, 229, 333, 333, 229, 178, 82, 3, 333, 333, 333, 333, 333, 333, 3, 264, 4, 117, 294, 294, 117, 4, 264, 0, 333, 333, 0, 0, 333, 333, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 255, 333, 333, 255, 0, 10, 10, 190, 333, 333, 333, 333, 190, 10, 10, 168, 0, 333, 333, 0, 168, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 203, 65, 220, 261, 261, 220, 65, 203, 154, 333, 333, 327, 327, 333, 333, 154, 0, 0, 0, 0, 0, 0, 0, 0, 62, 43, 0, 0, 0, 0, 43, 62},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 162, 203, 315, 315, 203, 162, 0, 0, 333, 333, 333, 333, 333, 333, 0, 0, 261, 129, 190, 190, 129, 261, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 209, 208, 241, 241, 208, 209, 10, 10, 197, 300, 233, 233, 300, 197, 10, 10, 41, 65, 0, 0, 65, 41, 10, 10, 299, 314, 0, 0, 314, 299, 10}};
