// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 16-Feb-2017 08:40:10 by "tuner -n 100 -o adam -c 24 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {23, 28, 153, 23};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -312, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 880, 522};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2209, -1192};
const int Scoring::Params::CASTLING[6] = {100, -208, -186, 0, 74, -120};
const int Scoring::Params::KING_COVER[6][4] = {{50, 101, 59, 34},
{14, 27, 16, 9},
{-84, -168, -98, -56},
{-46, -93, -54, -31},
{-150, -300, -175, -100},
{-214, -428, -250, -143}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 460;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -216;
const int Scoring::Params::MINOR_FOR_PAWNS = 203;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 229;
const int Scoring::Params::PAWN_ENDGAME2 = 271;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 14;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 14;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 27;
const int Scoring::Params::MINOR_ATTACK_BOOST = 43;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 48;
const int Scoring::Params::ROOK_ATTACK_BOOST = 26;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 34;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 38;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 54;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 60;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 22;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 75;
const int Scoring::Params::PIECE_THREAT_MM_MID = 150;
const int Scoring::Params::PIECE_THREAT_MR_MID = 221;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 196;
const int Scoring::Params::PIECE_THREAT_MM_END = 197;
const int Scoring::Params::PIECE_THREAT_MR_END = 361;
const int Scoring::Params::PIECE_THREAT_MQ_END = 578;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 72;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 174;
const int Scoring::Params::PIECE_THREAT_RM_MID = 130;
const int Scoring::Params::PIECE_THREAT_RR_MID = 102;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 385;
const int Scoring::Params::PIECE_THREAT_RM_END = 138;
const int Scoring::Params::PIECE_THREAT_RR_END = 157;
const int Scoring::Params::PIECE_THREAT_RQ_END = 135;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 99;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 175;
const int Scoring::Params::ENDGAME_KING_THREAT = 268;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 293;
const int Scoring::Params::BISHOP_PAIR_END = 523;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -86;
const int Scoring::Params::BAD_BISHOP_MID = -39;
const int Scoring::Params::BAD_BISHOP_END = -48;
const int Scoring::Params::CENTER_PAWN_BLOCK = -230;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -23;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -175;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -190;
const int Scoring::Params::SPACE = 36;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 4;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 204;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 212;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 465;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 185;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 154;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 44;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 200;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 80;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 29;
const int Scoring::Params::SUPPORTED_PASSER6 = 1000;
const int Scoring::Params::SUPPORTED_PASSER7 = 1500;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -44;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {23, 28, 25};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 28, 52, 63, 82};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 166, 380, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 193, 148};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {2, 5, 8, 11, 14, 18, 21, 25, 29, 33, 38, 42, 47, 52, 58, 64, 70, 76, 83, 90, 97, 105, 113, 122, 131, 140, 150, 161, 172, 184, 196, 209, 222, 236, 251, 267, 283, 300, 318, 337, 357, 377, 399, 421, 445, 469, 495, 521, 549, 578, 608, 640, 672, 706, 741, 778, 816, 855, 896, 938, 981, 1026, 1072, 1120, 1169, 1220, 1271, 1324, 1379, 1435, 1491, 1550, 1609, 1669, 1731, 1793, 1856, 1920, 1985, 2051, 2117, 2183, 2250, 2317, 2385, 2452, 2519, 2587, 2654, 2721, 2787, 2853, 2919, 2984, 3048, 3111, 3173, 3235, 3295, 3354, 3413, 3469, 3525, 3580, 3633, 3684, 3735, 3784, 3832, 3878, 3923, 3966, 4008, 4049, 4088, 4126, 4163, 4198, 4232, 4264, 4296, 4326, 4355, 4383, 4409, 4435, 4459, 4483, 4505, 4527, 4547, 4567, 4586, 4604, 4621, 4637, 4653, 4668, 4682, 4695, 4708, 4720, 4732, 4743, 4754, 4764, 4773, 4782, 4791, 4799};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 932, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 229, 668, 735}, {0, 0, 0, 0, 207, 592, 1131, 1670}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {75, 77, 71, 65, 65, 71, 77, 75};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 17, 586, 0}, {0, 0, 0, 0, 10, 141, 645, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 96, 500, 993, 1500}, {0, 0, 0, 8, 281, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 209, 715, 1482}, {0, 0, 0, 50, 70, 222, 356, 501}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-275, -249, -224, -156, -220, 0, -183, -258, -142, -237, 0, -190, 0, -252, -284, -122, -92, -333, 0, -312, 0}, {0, 0, 0, -130, -86, 0, -7, -63, -141, -144, -152, -92, -111, -91, -275, -100, -71, -108, -153, -207, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -320, -308, -301, -333, -333, -283, -255, -257, -332, -90, 0, -17, -192, -22, -42, -333, -178, -333}, {-150, -123, -94, -92, -122, -154, -165, -117, -71, -67, -91, -218, -94, -47, -100, -333, -197, -123, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -3, -178, -141, -141, -178, -3, -250}, {-250, -161, -228, -176, -176, -228, -161, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-400, -193, -184, 0, 0, -184, -193, -400}, {0, -432, -238, -327, -327, -238, -432, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -67, -94, -94, -67, 0, 0}, {-44, -71, -84, -123, -123, -84, -71, -44}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -252, -84, -57, -57, -84, -252, -500, -129, -189, -150, 3, 3, -150, -189, -129, -255, -91, -18, 67, 67, -18, -91, -255, -156, -102, 46, 36, 36, 46, -102, -156, 14, -94, 59, 12, 12, 59, -94, 14, -133, -347, -319, -58, -58, -319, -347, -133, -298, -268, -4, -381, -381, -4, -268, -298, -500, -500, -500, -500, -500, -500, -500, -500}, {-409, -213, -208, -166, -166, -208, -213, -409, -171, -197, -84, -63, -63, -84, -197, -171, -209, -129, -101, 51, 51, -101, -129, -209, -94, -55, 102, 112, 112, 102, -55, -94, 1, -84, 19, -13, -13, 19, -84, 1, -135, -316, -200, -39, -39, -200, -316, -135, -291, -123, -113, 5, 5, -113, -123, -291, -500, -220, -331, -237, -237, -331, -220, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-24, 228, 46, 67, 67, 46, 228, -24, 128, 170, 189, 87, 87, 189, 170, 128, -3, 118, 111, 108, 108, 111, 118, -3, -66, -123, 18, 100, 100, 18, -123, -66, -276, -102, -78, -17, -17, -78, -102, -276, 118, -293, -500, -289, -289, -500, -293, 118, -140, -340, -235, -500, -500, -235, -340, -140, -172, -500, -500, -500, -500, -500, -500, -172}, {-173, -105, -29, -64, -64, -29, -105, -173, -71, -24, -31, 0, 0, -31, -24, -71, -101, -21, 44, 76, 76, 44, -21, -101, -71, -3, 59, 113, 113, 59, -3, -71, -114, -68, -89, -53, -53, -89, -68, -114, 199, -201, -263, -204, -204, -263, -201, 199, 35, 107, -30, -18, -18, -30, 107, 35, 105, 209, 281, 39, 39, 281, 209, 105}};
const int Scoring::Params::ROOK_PST[2][64] = {{-271, -189, -142, -117, -117, -142, -189, -271, -316, -139, -140, -139, -139, -140, -139, -316, -259, -133, -210, -234, -234, -210, -133, -259, -295, -250, -244, -189, -189, -244, -250, -295, -163, -121, -108, -90, -90, -108, -121, -163, -115, 81, -34, -26, -26, -34, 81, -115, -37, -207, 25, -23, -23, 25, -207, -37, -30, -120, -500, -500, -500, -500, -120, -30}, {20, 53, 103, 58, 58, 103, 53, 20, -19, 22, 67, 52, 52, 67, 22, -19, -9, 45, 51, 39, 39, 51, 45, -9, 68, 110, 121, 95, 95, 121, 110, 68, 156, 156, 199, 172, 172, 199, 156, 156, 208, 252, 251, 222, 222, 251, 252, 208, 113, 144, 208, 171, 171, 208, 144, 113, 239, 283, 291, 188, 188, 291, 283, 239}};
const int Scoring::Params::QUEEN_PST[2][64] = {{29, 44, 52, 150, 150, 52, 44, 29, 104, 122, 152, 176, 176, 152, 122, 104, 44, 106, 129, 52, 52, 129, 106, 44, 71, 96, 54, 80, 80, 54, 96, 71, 86, 52, 83, 95, 95, 83, 52, 86, 51, 67, 110, 75, 75, 110, 67, 51, -73, -115, -34, -75, -75, -34, -115, -73, -50, 185, 104, 89, 89, 104, 185, -50}, {13, -110, -81, -56, -56, -81, -110, 13, 56, -43, -20, 117, 117, -20, -43, 56, 29, 192, 230, 233, 233, 230, 192, 29, 176, 303, 318, 378, 378, 318, 303, 176, 276, 433, 409, 500, 500, 409, 433, 276, 295, 439, 500, 500, 500, 500, 439, 295, 366, 484, 500, 500, 500, 500, 484, 366, 199, 233, 305, 408, 408, 305, 233, 199}};
const int Scoring::Params::KING_PST[2][64] = {{160, 252, -14, -24, -24, -14, 252, 160, 355, 295, 97, 32, 32, 97, 295, 355, -117, -48, -299, -432, -432, -299, -48, -117, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -436, -500, -500, -500, -500, -436, -500, -500, -92, -500, -500, -500, -500, -92, -500}, {-344, -318, -331, -363, -363, -331, -318, -344, -225, -250, -286, -276, -276, -286, -250, -225, -282, -269, -233, -215, -215, -233, -269, -282, -248, -198, -106, -72, -72, -106, -198, -248, -130, -50, 22, 80, 80, 22, -50, -130, -61, 25, 129, 152, 152, 129, 25, -61, -14, 103, 121, 103, 103, 121, 103, -14, -357, -36, 23, -82, -82, 23, -36, -357}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-460, -228, -100, -38, 20, 76, 106, 128, 90};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-307, -172, -82, -16, 40, 84, 121, 150, 187, 210, 215, 258, 216, 181, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -219, -164, -138, -121, -69, -29, 47, 85, 105, 122, 148, 232, 195, 214}, {-333, -139, -10, 90, 140, 168, 162, 194, 239, 260, 302, 329, 333, 333, 329}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-192, -3, -4, 27, 66, 114, 153, 172, 200, 236, 237, 269, 263, 259, 263, 255, 293, 333, 333, 333, 333, 333, 209, 333, 333, -333, -333, -333, 210}, {-257, -333, -333, -217, -173, 11, 127, 151, 237, 212, 266, 277, 287, 333, 333, 333, 333, 333, 333, 333, 320, 327, 333, 333, 266, 3, 333, -312, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -172, -50, 34, 56};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 155, 124, 218, 218, 124, 155, 1, 0, 299, 288, 152, 152, 288, 299, 0, 22, 14, 37, 0, 0, 37, 14, 22, 0, 0, 0, 14, 14, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 51, 214, 249, 333, 333, 249, 214, 51, 20, 333, 333, 330, 330, 333, 333, 20, 0, 39, 128, 260, 260, 128, 39, 0, 0, 315, 332, 0, 0, 332, 315, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 42, 283, 333, 333, 283, 42, 10, 10, 116, 333, 304, 304, 333, 116, 10, 10, 262, 0, 333, 333, 0, 262, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 165, 101, 215, 267, 267, 215, 101, 165, 146, 286, 333, 290, 290, 333, 286, 146, 73, 9, 25, 0, 0, 25, 9, 73, 131, 107, 10, 0, 0, 10, 107, 131},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 195, 193, 289, 289, 193, 195, 0, 0, 260, 333, 328, 328, 333, 260, 0, 0, 182, 94, 98, 98, 94, 182, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 228, 201, 244, 244, 201, 228, 10, 10, 0, 211, 151, 151, 211, 0, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 294, 293, 252, 252, 293, 294, 10}};
