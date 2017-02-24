// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 23-Feb-2017 05:09:34 by "tuner -n 150 -c 24 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 41, 227, 108};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 770, 297};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2181, -1192};
const int Scoring::Params::CASTLING[6] = {100, -220, -200, 0, 63, -121};
const int Scoring::Params::KING_COVER[6][4] = {{65, 130, 61, 65},
{34, 68, 32, 33},
{-59, -118, -55, -58},
{-38, -77, -36, -38},
{-94, -188, -88, -93},
{-219, -439, -205, -218}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 407;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -200;
const int Scoring::Params::MINOR_FOR_PAWNS = 193;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 232;
const int Scoring::Params::PAWN_ENDGAME2 = 316;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 12;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 13;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 29;
const int Scoring::Params::MINOR_ATTACK_BOOST = 39;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 45;
const int Scoring::Params::ROOK_ATTACK_BOOST = 20;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 34;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 39;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 48;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 55;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_BASE = 7;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_SLOPE = 17;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 24;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 80;
const int Scoring::Params::PIECE_THREAT_MM_MID = 160;
const int Scoring::Params::PIECE_THREAT_MR_MID = 277;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 198;
const int Scoring::Params::PIECE_THREAT_MM_END = 209;
const int Scoring::Params::PIECE_THREAT_MR_END = 431;
const int Scoring::Params::PIECE_THREAT_MQ_END = 614;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 74;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 180;
const int Scoring::Params::PIECE_THREAT_RM_MID = 116;
const int Scoring::Params::PIECE_THREAT_RR_MID = 46;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 367;
const int Scoring::Params::PIECE_THREAT_RM_END = 124;
const int Scoring::Params::PIECE_THREAT_RR_END = 73;
const int Scoring::Params::PIECE_THREAT_RQ_END = 110;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 102;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 179;
const int Scoring::Params::ENDGAME_KING_THREAT = 248;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 288;
const int Scoring::Params::BISHOP_PAIR_END = 493;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -61;
const int Scoring::Params::BAD_BISHOP_MID = -38;
const int Scoring::Params::BAD_BISHOP_END = -49;
const int Scoring::Params::CENTER_PAWN_BLOCK = -240;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -25;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -173;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -182;
const int Scoring::Params::SPACE = 36;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 197;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 214;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 465;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 176;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 145;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 48;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 217;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 99;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 18;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 497;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 435;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -227;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -417;
const int Scoring::Params::WRONG_COLOR_BISHOP = -9;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -43;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {16, 19, 12};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 211, 463, 718, 750};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 176, 130};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {-19, -17, -15, -13, -11, -9, -6, -4, -1, 2, 5, 9, 12, 16, 20, 24, 29, 33, 38, 44, 50, 56, 62, 69, 76, 84, 92, 100, 109, 119, 129, 140, 151, 163, 176, 189, 203, 218, 234, 251, 268, 287, 306, 327, 349, 371, 395, 420, 447, 474, 503, 534, 565, 599, 633, 670, 707, 747, 787, 830, 874, 920, 967, 1016, 1066, 1118, 1172, 1227, 1284, 1342, 1401, 1462, 1524, 1587, 1650, 1715, 1781, 1848, 1915, 1982, 2050, 2118, 2187, 2255, 2323, 2391, 2458, 2525, 2591, 2657, 2721, 2785, 2847, 2909, 2969, 3028, 3086, 3142, 3196, 3249, 3301, 3351, 3399, 3446, 3491, 3535, 3577, 3617, 3656, 3693, 3729, 3763, 3795, 3827, 3857, 3885, 3912, 3938, 3963, 3987, 4009, 4030, 4050, 4070, 4088, 4105, 4121, 4137, 4152, 4166, 4179, 4191, 4203, 4214, 4225, 4235, 4244, 4253, 4261, 4269, 4277, 4284, 4290, 4297, 4303, 4308, 4313, 4318, 4323, 4327};
const int Scoring::Params::TRADE_DOWN[8] = {973, 709, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 207, 718, 817}, {0, 0, 0, 0, 201, 563, 1208, 1725}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {79, 82, 78, 72, 72, 78, 82, 79};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 27, 631, 0}, {0, 0, 0, 0, 10, 141, 645, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 126, 655, 1250, 1750}, {0, 0, 2, 3, 288, 606, 1250, 1750}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 200, 695, 1500}, {0, 0, 0, 55, 78, 236, 441, 877}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-300, -267, -227, -155, -275, -91, -173, -266, -160, -301, -62, -218, 0, -276, -130, -130, -126, -333, 0, -333, 0}, {0, 0, 0, -131, -67, 0, -4, -66, -126, -135, -191, -99, -95, -89, -270, -129, -101, -110, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -316, -278, -289, -330, -88, -18, -23, -185, 0, -24, -315, -146, -333}, {-119, -99, -81, -61, -108, -124, -163, -106, -66, -63, -71, -225, -93, -41, -99, -333, -192, -125, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -14, -180, -152, -152, -180, -14, -250}, {-250, -159, -224, -171, -171, -224, -159, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-286, -262, -179, 0, 0, -179, -262, -286}, {0, -425, -254, -370, -370, -254, -425, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -66, -92, -92, -66, 0, 0}, {-50, -79, -85, -132, -132, -85, -79, -50}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -245, -93, -62, -62, -93, -245, -500, -130, -177, -150, 4, 4, -150, -177, -130, -258, -85, -17, 70, 70, -17, -85, -258, -155, -91, 43, 35, 35, 43, -91, -155, 25, -93, 50, 15, 15, 50, -93, 25, -120, -397, -349, -116, -116, -349, -397, -120, -306, -285, 3, -303, -303, 3, -285, -306, -500, -500, -500, -476, -476, -500, -500, -500}, {-364, -175, -172, -131, -131, -172, -175, -364, -110, -154, -52, -25, -25, -52, -154, -110, -178, -97, -66, 84, 84, -66, -97, -178, -59, -30, 130, 146, 146, 130, -30, -59, 21, -50, 55, 22, 22, 55, -50, 21, -98, -320, -208, -63, -63, -208, -320, -98, -261, -108, -99, 22, 22, -99, -108, -261, -500, -201, -298, -213, -213, -298, -201, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-26, 224, 47, 65, 65, 47, 224, -26, 126, 171, 181, 86, 86, 181, 171, 126, -6, 118, 108, 108, 108, 108, 118, -6, -67, -131, 16, 94, 94, 16, -131, -67, -252, -100, -90, -29, -29, -90, -100, -252, 26, -371, -500, -345, -345, -500, -371, 26, -214, -372, -403, -500, -500, -403, -372, -214, -205, -500, -500, -500, -500, -500, -500, -205}, {-142, -71, 21, -20, -20, 21, -71, -142, -12, 22, 19, 49, 49, 19, 22, -12, -54, 18, 84, 119, 119, 84, 18, -54, -27, 39, 105, 154, 154, 105, 39, -27, -53, -21, -38, -17, -17, -38, -21, -53, 177, -218, -240, -208, -208, -240, -218, 177, 5, 93, -152, 17, 17, -152, 93, 5, 130, 229, 328, 67, 67, 328, 229, 130}};
const int Scoring::Params::ROOK_PST[2][64] = {{-359, -272, -225, -197, -197, -225, -272, -359, -401, -227, -242, -217, -217, -242, -227, -401, -348, -221, -294, -306, -306, -294, -221, -348, -365, -326, -341, -275, -275, -341, -326, -365, -249, -209, -190, -160, -160, -190, -209, -249, -212, -9, -116, -95, -95, -116, -9, -212, -110, -283, -34, -66, -66, -34, -283, -110, -129, -264, -500, -500, -500, -500, -264, -129}, {34, 61, 113, 73, 73, 113, 61, 34, -9, 33, 87, 67, 67, 87, 33, -9, 8, 62, 71, 56, 56, 71, 62, 8, 83, 130, 134, 113, 113, 134, 130, 83, 173, 171, 216, 193, 193, 216, 171, 173, 230, 272, 269, 244, 244, 269, 272, 230, 140, 170, 236, 196, 196, 236, 170, 140, 241, 282, 284, 183, 183, 284, 282, 241}};
const int Scoring::Params::QUEEN_PST[2][64] = {{48, 61, 72, 168, 168, 72, 61, 48, 128, 136, 170, 198, 198, 170, 136, 128, 62, 126, 151, 79, 79, 151, 126, 62, 91, 127, 78, 105, 105, 78, 127, 91, 121, 72, 99, 128, 128, 99, 72, 121, 65, 106, 129, 118, 118, 129, 106, 65, -50, -112, -15, -12, -12, -15, -112, -50, -25, 235, 96, 103, 103, 96, 235, -25}, {59, -75, -39, -13, -13, -39, -75, 59, 73, 14, 30, 151, 151, 30, 14, 73, 44, 219, 264, 262, 262, 264, 219, 44, 216, 348, 347, 415, 415, 347, 348, 216, 310, 496, 447, 500, 500, 447, 496, 310, 345, 475, 500, 500, 500, 500, 475, 345, 416, 500, 500, 500, 500, 500, 500, 416, 256, 265, 354, 437, 437, 354, 265, 256}};
const int Scoring::Params::KING_PST[2][64] = {{259, 353, 94, 156, 156, 94, 353, 259, 457, 399, 179, 290, 290, 179, 399, 457, 167, 244, -30, -141, -141, -30, 244, 167, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 361, -500, -500, -500, -500, 361, -500, -500, 367, -420, 500, 500, -420, 367, -500}, {-494, -465, -488, -500, -500, -488, -465, -494, -359, -388, -430, -417, -417, -430, -388, -359, -408, -395, -352, -335, -335, -352, -395, -408, -357, -304, -203, -170, -170, -203, -304, -357, -224, -129, -55, 12, 12, -55, -129, -224, -133, -30, 73, 108, 108, 73, -30, -133, -87, 46, 58, 46, 46, 58, 46, -87, -457, -128, -66, -154, -154, -66, -128, -457}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-463, -239, -114, -54, 6, 61, 91, 116, 72};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-314, -183, -94, -28, 30, 74, 110, 136, 175, 199, 206, 254, 212, 191, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -214, -160, -134, -117, -69, -27, 45, 82, 92, 114, 132, 222, 141, 167}, {-333, -155, -24, 76, 122, 154, 148, 181, 227, 255, 294, 322, 333, 333, 324}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-205, 16, 16, 55, 89, 139, 175, 198, 221, 257, 258, 282, 277, 279, 272, 266, 317, 333, 333, 333, 333, 333, 162, 333, 333, -333, -333, -333, 210}, {-333, -333, -333, -258, -175, -17, 113, 127, 206, 188, 256, 264, 279, 333, 333, 333, 333, 333, 333, 333, 328, 333, 333, 333, 301, 69, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -174, -50, 31, 55};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 159, 119, 215, 215, 119, 159, 19, 0, 333, 333, 226, 226, 333, 333, 0, 35, 23, 45, 4, 4, 45, 23, 35, 0, 0, 0, 25, 25, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59, 223, 253, 333, 333, 253, 223, 59, 0, 333, 333, 333, 333, 333, 333, 0, 0, 45, 133, 241, 241, 133, 45, 0, 0, 316, 333, 0, 0, 333, 316, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 50, 268, 333, 333, 268, 50, 10, 10, 206, 333, 333, 333, 333, 206, 10, 10, 110, 0, 333, 333, 0, 110, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 146, 104, 210, 279, 279, 210, 104, 146, 229, 333, 333, 333, 333, 333, 333, 229, 153, 61, 182, 0, 0, 182, 61, 153, 161, 133, 19, 0, 0, 19, 133, 161},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 191, 191, 289, 289, 191, 191, 0, 0, 333, 333, 333, 333, 333, 333, 0, 0, 17, 0, 0, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 221, 201, 248, 248, 201, 221, 10, 10, 58, 333, 230, 230, 333, 58, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 333, 328, 333, 333, 328, 333, 10}};

