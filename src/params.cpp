// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 25-Jul-2017 09:47:47 by "tuner -n 150 -c 24 -R 50 /home/jdart/chess/epd/big2.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 5, 144, 32};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 776, 271};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -1957, -1075};
const int Scoring::Params::CASTLING[6] = {100, -220, -183, 0, 52, -102};
const int Scoring::Params::KING_COVER[6][4] = {{83, 166, 77, 75},
{51, 101, 47, 46},
{-32, -63, -30, -29},
{-5, -11, -5, -5},
{-63, -126, -59, -57},
{-198, -395, -184, -178}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 441;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -788;
const int Scoring::Params::MINOR_FOR_PAWNS = 106;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 247;
const int Scoring::Params::PAWN_ENDGAME2 = 179;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 9;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 11;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 30;
const int Scoring::Params::MINOR_ATTACK_BOOST = 40;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 48;
const int Scoring::Params::ROOK_ATTACK_BOOST = 19;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 29;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 49;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 44;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 56;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_BASE = 9;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_SLOPE = 19;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 13;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 81;
const int Scoring::Params::PIECE_THREAT_MM_MID = 176;
const int Scoring::Params::PIECE_THREAT_MR_MID = 382;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 234;
const int Scoring::Params::PIECE_THREAT_MM_END = 232;
const int Scoring::Params::PIECE_THREAT_MR_END = 432;
const int Scoring::Params::PIECE_THREAT_MQ_END = 750;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 64;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 180;
const int Scoring::Params::PIECE_THREAT_RM_MID = 131;
const int Scoring::Params::PIECE_THREAT_RR_MID = 79;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 508;
const int Scoring::Params::PIECE_THREAT_RM_END = 122;
const int Scoring::Params::PIECE_THREAT_RR_END = 117;
const int Scoring::Params::PIECE_THREAT_RQ_END = 462;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 103;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 199;
const int Scoring::Params::ENDGAME_KING_THREAT = 316;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 217;
const int Scoring::Params::BISHOP_PAIR_END = 471;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -43;
const int Scoring::Params::BAD_BISHOP_MID = -39;
const int Scoring::Params::BAD_BISHOP_END = -45;
const int Scoring::Params::CENTER_PAWN_BLOCK = -206;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -8;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -171;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -189;
const int Scoring::Params::SPACE = 32;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 161;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 352;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 591;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 154;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 118;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 65;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 180;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 94;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 7;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 598;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 418;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -214;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -357;
const int Scoring::Params::WRONG_COLOR_BISHOP = 0;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -29;
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 204, 463, 725, 750};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 182, 134};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {-21, -19, -17, -15, -13, -11, -8, -5, -3, 0, 4, 7, 11, 14, 19, 23, 28, 33, 38, 43, 49, 56, 63, 70, 78, 86, 94, 104, 113, 124, 135, 146, 159, 172, 186, 201, 217, 233, 251, 269, 289, 310, 332, 355, 379, 405, 432, 461, 491, 523, 556, 591, 627, 665, 705, 747, 790, 836, 883, 932, 983, 1035, 1090, 1146, 1205, 1264, 1326, 1389, 1454, 1520, 1588, 1657, 1727, 1798, 1870, 1943, 2016, 2090, 2164, 2239, 2313, 2387, 2462, 2535, 2608, 2681, 2752, 2822, 2892, 2960, 3027, 3092, 3156, 3218, 3278, 3337, 3394, 3449, 3503, 3554, 3604, 3652, 3698, 3742, 3784, 3825, 3863, 3900, 3936, 3970, 4002, 4032, 4061, 4089, 4115, 4140, 4164, 4186, 4207, 4228, 4247, 4264, 4281, 4297, 4313, 4327, 4340, 4353, 4365, 4376, 4387, 4397, 4406, 4415, 4423, 4431, 4439, 4446, 4452, 4458, 4464, 4469, 4474, 4479, 4484, 4488, 4492, 4496, 4499, 4502};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 624, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 156, 691, 791}, {0, 0, 0, 0, 131, 535, 1189, 1701}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {77, 80, 76, 71, 71, 76, 80, 77};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 0, 709, 0}, {0, 0, 0, 0, 0, 113, 750, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 6, 635, 1250, 1750}, {0, 0, 0, 0, 293, 692, 1250, 1750}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 176, 684, 1158}, {0, 0, 0, 50, 70, 244, 381, 712}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-333, -305, -264, -189, -333, 0, -214, -333, -219, -332, -133, -288, -36, -300, 0, -228, -110, -333, 0, -151, 0}, {-22, 0, -67, -165, -134, -127, -61, -127, -138, -215, -226, -71, -57, -53, -215, -122, -89, -255, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -145, -23, -77, -145, 0, 0, -307, -148, -333}, {-171, -136, -134, -117, -128, -137, -194, -106, -105, -89, -92, -172, -48, -25, -27, -333, -160, -122, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -21, -203, -161, -161, -203, -21, -250}, {-250, -173, -243, -188, -188, -243, -173, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{0, -366, -188, -60, -60, -188, -366, 0}, {0, -448, -278, -445, -445, -278, -448, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -63, -93, -93, -63, 0, 0}, {-40, -67, -63, -111, -111, -63, -67, -40}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -251, -86, -75, -75, -86, -251, -500, -107, -202, -182, -2, -2, -182, -202, -107, -280, -96, -4, 81, 81, -4, -96, -280, -154, -119, 44, 33, 33, 44, -119, -154, 23, -92, 78, -27, -27, 78, -92, 23, -235, -500, -495, -127, -127, -495, -500, -235, -478, -385, -9, -168, -168, -9, -385, -478, -500, -500, -500, -500, -500, -500, -500, -500}, {-327, -163, -176, -116, -116, -176, -163, -327, -121, -163, -63, -40, -40, -63, -163, -121, -210, -112, -69, 78, 78, -69, -112, -210, -64, -26, 133, 178, 178, 133, -26, -64, -35, -64, 52, 64, 64, 52, -64, -35, -201, -468, -312, -139, -139, -312, -468, -201, -342, -156, -114, -66, -66, -114, -156, -342, -500, -317, -404, -259, -259, -404, -317, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{70, 280, 108, 123, 123, 108, 280, 70, 169, 214, 218, 133, 133, 218, 214, 169, 43, 176, 163, 161, 161, 163, 176, 43, -44, -89, 64, 121, 121, 64, -89, -44, -260, -101, -157, -53, -53, -157, -101, -260, -50, -500, -500, -440, -440, -500, -500, -50, -3, -374, -284, -500, -500, -284, -374, -3, -114, -462, -500, -500, -500, -500, -462, -114}, {-102, -57, 36, 8, 8, 36, -57, -102, 20, 47, 13, 60, 60, 13, 47, 20, -46, 31, 97, 127, 127, 97, 31, -46, -52, 41, 111, 154, 154, 111, 41, -52, -162, -20, -8, 53, 53, -8, -20, -162, 117, -174, -399, -268, -268, -399, -174, 117, 21, 8, -242, -262, -262, -242, 8, 21, 116, 156, 122, -177, -177, 122, 156, 116}};
const int Scoring::Params::ROOK_PST[2][64] = {{-387, -294, -253, -231, -231, -253, -294, -387, -425, -256, -268, -241, -241, -268, -256, -425, -351, -220, -318, -327, -327, -318, -220, -351, -377, -333, -383, -292, -292, -383, -333, -377, -274, -217, -221, -180, -180, -221, -217, -274, -254, -74, -89, -60, -60, -89, -74, -254, -209, -384, -15, -71, -71, -15, -384, -209, -100, -310, -500, -500, -500, -500, -310, -100}, {-12, 13, 61, 23, 23, 61, 13, -12, -23, 3, 47, 34, 34, 47, 3, -23, -21, 18, 25, 28, 28, 25, 18, -21, 14, 107, 87, 72, 72, 87, 107, 14, 136, 151, 155, 150, 150, 155, 151, 136, 165, 243, 241, 206, 206, 241, 243, 165, 90, 117, 199, 165, 165, 199, 117, 90, 259, 311, 264, 179, 179, 264, 311, 259}};
const int Scoring::Params::QUEEN_PST[2][64] = {{44, 70, 92, 172, 172, 92, 70, 44, 165, 148, 177, 205, 205, 177, 148, 165, 84, 147, 164, 94, 94, 164, 147, 84, 101, 139, 82, 111, 111, 82, 139, 101, 112, 88, 86, 126, 126, 86, 88, 112, 37, 108, 124, 96, 96, 124, 108, 37, -48, -162, -23, -73, -73, -23, -162, -48, 35, 329, 159, 272, 272, 159, 329, 35}, {-139, -282, -274, -185, -185, -274, -282, -139, -141, -164, -157, -11, -11, -157, -164, -141, -125, 73, 127, 130, 130, 127, 73, -125, -1, 192, 240, 301, 301, 240, 192, -1, 140, 352, 361, 500, 500, 361, 352, 140, 129, 300, 500, 500, 500, 500, 300, 129, 269, 358, 500, 500, 500, 500, 358, 269, 85, 112, 128, 209, 209, 128, 112, 85}};
const int Scoring::Params::KING_PST[2][64] = {{273, 373, 113, 183, 183, 113, 373, 273, 470, 406, 183, 347, 347, 183, 406, 470, -79, 203, -209, -157, -157, -209, 203, -79, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 142, -104, -500, -500, -104, 142, -500, -500, -500, -500, -500, -500, -500, -500, -500}, {-459, -445, -470, -493, -493, -470, -445, -459, -332, -357, -414, -392, -392, -414, -357, -332, -386, -379, -335, -325, -325, -335, -379, -386, -332, -284, -196, -178, -178, -196, -284, -332, -214, -117, -30, -8, -8, -30, -117, -214, -140, -12, 72, 85, 85, 72, -12, -140, -202, -23, 21, 46, 46, 21, -23, -202, -485, -171, -97, -153, -153, -97, -171, -485}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-484, -257, -136, -74, -25, 29, 47, 39, 8};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-333, -225, -133, -70, -17, 33, 63, 79, 117, 132, 142, 173, 197, 96, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -233, -183, -157, -146, -100, -63, 10, 40, 57, 84, 102, 227, 180, 225}, {-333, -133, -2, 111, 167, 189, 200, 215, 255, 287, 313, 332, 333, 296, 288}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-67, 38, 17, 59, 91, 130, 170, 196, 217, 256, 245, 281, 285, 287, 246, 273, 293, 333, 319, 333, 333, 333, 277, 333, 333, -333, -333, 333, 210}, {-266, -333, -333, -333, -103, 15, 168, 170, 245, 246, 288, 304, 305, 333, 333, 326, 333, 333, 333, 261, 260, 252, 256, 140, 206, 138, 119, -258, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -152, -22, 46, 74};

const int Scoring::Params::KNIGHT_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{19, 0},{130, 189},{175, 125},{365, 118},{365, 118},{175, 125},{130, 189},{19, 0},{0, 38},{500, 432},{500, 429},{245, 360},{245, 360},{500, 429},{500, 432},{0, 38},{8, 39},{13, 47},{38, 36},{14, 51},{14, 51},{38, 36},{13, 47},{8, 39},{0, 0},{0, 39},{0, 14},{0, 34},{0, 34},{0, 14},{0, 39},{0, 0}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{83, 0},{224, 169},{255, 272},{484, 294},{484, 294},{255, 272},{224, 169},{83, 0},{54, 53},{500, 473},{500, 500},{500, 399},{500, 399},{500, 500},{500, 473},{54, 53},{344, 0},{272, 0},{120, 189},{491, 56},{491, 56},{120, 189},{272, 0},{344, 0},{0, 0},{500, 86},{500, 432},{500, 9},{500, 9},{500, 432},{500, 86},{0, 0}}};
const int Scoring::Params::BISHOP_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{166, 256},{143, 145},{341, 170},{317, 223},{317, 223},{341, 170},{143, 145},{166, 256},{336, 233},{500, 304},{500, 500},{379, 409},{379, 409},{500, 500},{500, 304},{336, 233},{90, 162},{0, 200},{33, 316},{0, 322},{0, 322},{33, 316},{0, 200},{90, 162},{147, 146},{18, 164},{0, 176},{0, 270},{0, 270},{0, 176},{18, 164},{147, 146}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 26},{272, 164},{351, 152},{455, 160},{455, 160},{351, 152},{272, 164},{0, 26},{0, 0},{479, 319},{500, 500},{500, 359},{500, 359},{500, 500},{479, 319},{0, 0},{0, 0},{500, 0},{0, 2},{383, 0},{383, 0},{0, 2},{500, 0},{0, 0},{0, 0},{18, 0},{500, 0},{264, 8},{264, 8},{500, 0},{18, 0},{0, 0}}};


