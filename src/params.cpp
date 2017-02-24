// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 24-Feb-2017 08:07:10 by "tuner -n 150 -c 24 -R 50 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 44, 241, 85};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 853, 287};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2123, -1195};
const int Scoring::Params::CASTLING[6] = {100, -230, -210, 0, 70, -111};
const int Scoring::Params::KING_COVER[6][4] = {{73, 146, 67, 75},
{42, 84, 38, 43},
{-51, -102, -47, -53},
{-25, -50, -23, -26},
{-91, -182, -83, -94},
{-228, -456, -208, -235}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 457;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -122;
const int Scoring::Params::MINOR_FOR_PAWNS = 179;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 249;
const int Scoring::Params::PAWN_ENDGAME2 = 306;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 10;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 12;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 27;
const int Scoring::Params::MINOR_ATTACK_BOOST = 40;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 46;
const int Scoring::Params::ROOK_ATTACK_BOOST = 15;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 32;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 38;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 45;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 57;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_BASE = 7;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_SLOPE = 17;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 17;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 81;
const int Scoring::Params::PIECE_THREAT_MM_MID = 180;
const int Scoring::Params::PIECE_THREAT_MR_MID = 371;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 272;
const int Scoring::Params::PIECE_THREAT_MM_END = 228;
const int Scoring::Params::PIECE_THREAT_MR_END = 423;
const int Scoring::Params::PIECE_THREAT_MQ_END = 750;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 76;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 198;
const int Scoring::Params::PIECE_THREAT_RM_MID = 147;
const int Scoring::Params::PIECE_THREAT_RR_MID = 105;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 534;
const int Scoring::Params::PIECE_THREAT_RM_END = 157;
const int Scoring::Params::PIECE_THREAT_RR_END = 140;
const int Scoring::Params::PIECE_THREAT_RQ_END = 269;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 119;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 206;
const int Scoring::Params::ENDGAME_KING_THREAT = 326;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 248;
const int Scoring::Params::BISHOP_PAIR_END = 483;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -55;
const int Scoring::Params::BAD_BISHOP_MID = -38;
const int Scoring::Params::BAD_BISHOP_END = -45;
const int Scoring::Params::CENTER_PAWN_BLOCK = -230;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -18;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -166;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -178;
const int Scoring::Params::SPACE = 37;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 164;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 210;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 566;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 165;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 128;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 57;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 206;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 100;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 12;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 652;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 429;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -212;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -373;
const int Scoring::Params::WRONG_COLOR_BISHOP = 0;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -24;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {22, 24, 17};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 208, 486, 743, 750};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 181, 130};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {-22, -20, -19, -17, -15, -13, -10, -8, -5, -3, 0, 4, 7, 10, 14, 18, 22, 27, 32, 37, 42, 48, 54, 60, 67, 74, 82, 90, 99, 108, 118, 129, 140, 151, 164, 177, 191, 205, 221, 237, 254, 273, 292, 313, 334, 357, 381, 406, 433, 460, 490, 520, 553, 587, 622, 659, 698, 738, 781, 825, 870, 918, 967, 1018, 1071, 1126, 1183, 1241, 1301, 1363, 1426, 1491, 1558, 1625, 1694, 1765, 1836, 1908, 1981, 2055, 2129, 2204, 2279, 2354, 2429, 2504, 2579, 2653, 2726, 2799, 2871, 2942, 3011, 3080, 3147, 3213, 3277, 3340, 3401, 3460, 3518, 3574, 3628, 3680, 3731, 3779, 3826, 3871, 3914, 3956, 3996, 4034, 4070, 4105, 4138, 4170, 4200, 4229, 4256, 4282, 4307, 4330, 4352, 4374, 4394, 4412, 4430, 4447, 4463, 4479, 4493, 4506, 4519, 4531, 4543, 4553, 4564, 4573, 4582, 4591, 4599, 4606, 4613, 4620, 4626, 4632, 4638, 4643, 4648, 4653};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 927, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 197, 718, 770}, {0, 0, 0, 0, 181, 573, 1224, 1705}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {80, 82, 78, 72, 72, 78, 82, 80};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 15, 750, 0}, {0, 0, 0, 0, 0, 144, 750, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 33, 666, 1250, 1750}, {0, 0, 0, 21, 269, 685, 1250, 1750}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 208, 686, 1500}, {0, 0, 0, 50, 70, 224, 390, 698}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-333, -284, -285, -179, -333, 0, -188, -333, -180, -333, -161, -250, 0, -282, -251, -199, -89, -333, -1, -129, 0}, {0, 0, 0, -121, -113, 0, -8, -75, -127, -188, -104, -90, -67, -46, -264, -99, -76, -175, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -112, -2, -61, -171, 0, -16, -330, -134, -333}, {-126, -120, -88, -77, -78, -65, -145, -88, -118, -52, -69, -196, -54, -13, -29, -333, -167, -148, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -8, -197, -162, -162, -197, -8, -250}, {-250, -162, -219, -169, -169, -219, -162, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{0, -292, -128, 0, 0, -128, -292, 0}, {0, -500, -285, -448, -448, -285, -500, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -74, -105, -105, -74, 0, 0}, {-47, -75, -85, -130, -130, -85, -75, -47}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -251, -102, -73, -73, -102, -251, -500, -107, -199, -176, 7, 7, -176, -199, -107, -268, -90, -5, 84, 84, -5, -90, -268, -149, -125, 48, 45, 45, 48, -125, -149, 42, -103, 94, 38, 38, 94, -103, 42, -220, -370, -346, -54, -54, -346, -370, -220, -481, -343, 12, -181, -181, 12, -343, -481, -500, -500, -500, -500, -500, -500, -500, -500}, {-358, -158, -141, -114, -114, -141, -158, -358, -107, -153, -63, -22, -22, -63, -153, -107, -169, -103, -67, 98, 98, -67, -103, -169, -53, -5, 156, 181, 181, 156, -5, -53, 59, -58, 54, 23, 23, 54, -58, 59, -123, -319, -178, -23, -23, -178, -319, -123, -286, -106, -127, -1, -1, -127, -106, -286, -500, -229, -418, -246, -246, -418, -229, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{35, 270, 87, 94, 94, 87, 270, 35, 163, 202, 215, 117, 117, 215, 202, 163, 15, 158, 138, 143, 143, 138, 158, 15, -67, -130, 43, 104, 104, 43, -130, -67, -297, -118, -128, -17, -17, -128, -118, -297, 44, -406, -500, -405, -405, -500, -406, 44, -152, -387, -427, -500, -500, -427, -387, -152, -199, -500, -500, -500, -500, -500, -500, -199}, {-122, -41, 38, 0, 0, 38, -41, -122, 28, 35, 28, 54, 54, 28, 35, 28, -45, 24, 96, 123, 123, 96, 24, -45, -34, 50, 112, 161, 161, 112, 50, -34, -88, -49, -94, -60, -60, -94, -49, -88, 149, -193, -223, -209, -209, -223, -193, 149, 29, 119, -141, 3, 3, -141, 119, 29, 136, 246, 302, 75, 75, 302, 246, 136}};
const int Scoring::Params::ROOK_PST[2][64] = {{-380, -287, -247, -225, -225, -247, -287, -380, -414, -239, -254, -238, -238, -254, -239, -414, -362, -227, -319, -330, -330, -319, -227, -362, -357, -338, -379, -294, -294, -379, -338, -357, -282, -233, -205, -189, -189, -205, -233, -282, -242, -35, -89, -70, -70, -89, -35, -242, -193, -380, -41, -54, -54, -41, -380, -193, -141, -321, -500, -500, -500, -500, -321, -141}, {18, 37, 94, 51, 51, 94, 37, 18, -15, 21, 73, 55, 55, 73, 21, -15, -8, 33, 45, 44, 44, 45, 33, -8, 57, 103, 119, 97, 97, 119, 103, 57, 176, 175, 209, 195, 195, 209, 175, 176, 237, 301, 289, 247, 247, 289, 301, 237, 133, 156, 237, 202, 202, 237, 156, 133, 294, 337, 287, 193, 193, 287, 337, 294}};
const int Scoring::Params::QUEEN_PST[2][64] = {{43, 71, 80, 169, 169, 80, 71, 43, 154, 149, 176, 208, 208, 176, 149, 154, 72, 137, 160, 85, 85, 160, 137, 72, 97, 136, 89, 109, 109, 89, 136, 97, 110, 73, 76, 153, 153, 76, 73, 110, 58, 114, 123, 112, 112, 123, 114, 58, -78, -164, -6, -41, -41, -6, -164, -78, -158, 294, 183, -18, -18, 183, 294, -158}, {-42, -203, -181, -118, -118, -181, -203, -42, -66, -92, -67, 88, 88, -67, -92, -66, -45, 152, 230, 216, 216, 230, 152, -45, 105, 291, 311, 414, 414, 311, 291, 105, 304, 460, 483, 500, 500, 483, 460, 304, 240, 455, 500, 500, 500, 500, 455, 240, 350, 500, 500, 500, 500, 500, 500, 350, 131, 116, 263, 444, 444, 263, 116, 131}};
const int Scoring::Params::KING_PST[2][64] = {{280, 373, 92, 186, 186, 92, 373, 280, 452, 412, 198, 345, 345, 198, 412, 452, -222, 134, -392, -330, -330, -392, 134, -222, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 45, -203, -500, -500, -203, 45, -500, -500, 500, -500, -500, -500, -500, 500, -500}, {-464, -450, -472, -500, -500, -472, -450, -464, -337, -369, -421, -404, -404, -421, -369, -337, -393, -389, -348, -334, -334, -348, -389, -393, -344, -296, -200, -182, -182, -200, -296, -344, -207, -102, -30, 27, 27, -30, -102, -207, -126, -44, 82, 92, 92, 82, -44, -126, -149, 0, 47, 27, 27, 47, 0, -149, -480, -87, -26, -184, -184, -26, -87, -480}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-479, -248, -120, -56, -3, 56, 84, 84, 34};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-324, -202, -102, -38, 21, 68, 102, 119, 169, 178, 201, 252, 185, 218, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -221, -175, -152, -138, -91, -54, 20, 48, 69, 86, 103, 257, 154, 255}, {-333, -138, 4, 114, 163, 184, 186, 206, 252, 280, 314, 332, 333, 307, 279}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-126, 40, -3, 39, 80, 116, 160, 183, 207, 246, 239, 277, 269, 271, 238, 250, 300, 333, 333, 333, 333, 333, 301, 333, 333, -333, -333, -16, 210}, {-333, -333, -333, -332, -149, 16, 199, 158, 253, 249, 275, 291, 289, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 327, -257, 333, -231, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -141, -7, 69, 97};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 203, 144, 244, 244, 144, 203, 0, 0, 333, 333, 217, 217, 333, 333, 0, 14, 9, 42, 13, 13, 42, 9, 14, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 67, 251, 285, 333, 333, 285, 251, 67, 68, 333, 333, 333, 333, 333, 333, 68, 82, 67, 90, 278, 278, 90, 67, 82, 0, 333, 333, 0, 0, 333, 333, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 85, 253, 333, 333, 253, 85, 10, 10, 141, 333, 333, 333, 333, 141, 10, 10, 333, 0, 333, 333, 0, 333, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 190, 142, 266, 332, 332, 266, 142, 190, 221, 333, 333, 333, 333, 333, 333, 221, 157, 58, 182, 0, 0, 182, 58, 157, 156, 96, 0, 0, 0, 0, 96, 156},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 218, 250, 333, 333, 250, 218, 0, 0, 333, 333, 333, 333, 333, 333, 0, 0, 16, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 307, 287, 282, 282, 287, 307, 10, 10, 57, 190, 278, 278, 190, 57, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 333, 118, 333, 333, 118, 333, 10}};
