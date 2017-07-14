// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 09-Jul-2017 07:08:17 by "tuner -n 150 -c 24 -R 50 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 54, 242, 93};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 857, 250};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2107, -1194};
const int Scoring::Params::CASTLING[6] = {100, -227, -204, 0, 70, -105};
const int Scoring::Params::KING_COVER[6][4] = {{83, 166, 75, 75},
{52, 104, 47, 47},
{-37, -74, -33, -34},
{-9, -19, -8, -8},
{-77, -155, -70, -70},
{-218, -436, -197, -197}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 457;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -129;
const int Scoring::Params::MINOR_FOR_PAWNS = 175;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 242;
const int Scoring::Params::PAWN_ENDGAME2 = 301;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 9;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 12;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 26;
const int Scoring::Params::MINOR_ATTACK_BOOST = 41;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 44;
const int Scoring::Params::ROOK_ATTACK_BOOST = 16;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 31;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 37;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 46;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 56;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_BASE = 7;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_SLOPE = 18;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 15;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 83;
const int Scoring::Params::PIECE_THREAT_MM_MID = 179;
const int Scoring::Params::PIECE_THREAT_MR_MID = 374;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 279;
const int Scoring::Params::PIECE_THREAT_MM_END = 227;
const int Scoring::Params::PIECE_THREAT_MR_END = 416;
const int Scoring::Params::PIECE_THREAT_MQ_END = 750;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 74;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 194;
const int Scoring::Params::PIECE_THREAT_RM_MID = 148;
const int Scoring::Params::PIECE_THREAT_RR_MID = 99;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 525;
const int Scoring::Params::PIECE_THREAT_RM_END = 155;
const int Scoring::Params::PIECE_THREAT_RR_END = 151;
const int Scoring::Params::PIECE_THREAT_RQ_END = 403;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 121;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 204;
const int Scoring::Params::ENDGAME_KING_THREAT = 323;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 241;
const int Scoring::Params::BISHOP_PAIR_END = 479;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -50;
const int Scoring::Params::BAD_BISHOP_MID = -39;
const int Scoring::Params::BAD_BISHOP_END = -45;
const int Scoring::Params::CENTER_PAWN_BLOCK = -227;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -15;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -167;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -180;
const int Scoring::Params::SPACE = 36;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 167;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 283;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 594;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 166;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 127;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 54;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 206;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 101;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 11;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 627;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 456;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -210;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -375;
const int Scoring::Params::WRONG_COLOR_BISHOP = 0;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -24;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {22, 24, 16};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 209, 486, 741, 750};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 180, 130};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {-18, -16, -14, -12, -9, -7, -4, -1, 2, 5, 8, 12, 15, 19, 23, 28, 33, 38, 43, 49, 55, 61, 68, 76, 83, 91, 100, 109, 119, 129, 140, 152, 164, 177, 190, 205, 220, 236, 253, 271, 291, 311, 332, 354, 378, 403, 429, 456, 485, 515, 547, 580, 615, 652, 690, 730, 771, 815, 860, 907, 956, 1007, 1060, 1114, 1171, 1229, 1289, 1351, 1415, 1480, 1547, 1616, 1686, 1758, 1830, 1904, 1979, 2055, 2132, 2210, 2288, 2366, 2445, 2524, 2603, 2681, 2759, 2837, 2914, 2990, 3066, 3140, 3213, 3285, 3356, 3425, 3493, 3559, 3623, 3686, 3746, 3805, 3862, 3918, 3971, 4023, 4072, 4120, 4166, 4210, 4252, 4293, 4332, 4369, 4404, 4438, 4470, 4501, 4531, 4558, 4585, 4610, 4634, 4657, 4679, 4699, 4718, 4737, 4754, 4771, 4786, 4801, 4815, 4828, 4841, 4853, 4864, 4874, 4884, 4893, 4902, 4911, 4918, 4926, 4933, 4939, 4946, 4951, 4957, 4962};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 903, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 196, 713, 774}, {0, 0, 0, 0, 177, 570, 1224, 1701}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {80, 82, 78, 72, 72, 78, 82, 80};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 19, 750, 0}, {0, 0, 0, 0, 0, 139, 750, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 39, 673, 1250, 1750}, {0, 0, 0, 18, 273, 692, 1250, 1750}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 211, 694, 1459}, {0, 0, 0, 50, 70, 230, 384, 706}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-333, -288, -284, -166, -333, 0, -185, -333, -203, -333, 0, -277, -16, -275, -333, -205, -36, -333, 0, -207, 0}, {0, 0, -2, -131, -105, -26, -9, -89, -133, -211, -164, -83, -68, -40, -228, -110, -97, -211, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -107, 0, -66, -169, 0, -14, -333, -125, -333}, {-129, -101, -80, -81, -97, -86, -147, -93, -109, -53, -64, -198, -54, -13, -25, -333, -170, -148, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -8, -195, -162, -162, -195, -8, -250}, {-250, -163, -222, -169, -169, -222, -163, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{0, -255, -114, 0, 0, -114, -255, 0}, {0, -482, -306, -441, -441, -306, -482, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -78, -105, -105, -78, 0, 0}, {-47, -76, -82, -130, -130, -82, -76, -47}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -243, -96, -66, -66, -96, -243, -500, -103, -189, -168, 18, 18, -168, -189, -103, -259, -78, 9, 95, 95, 9, -78, -259, -141, -112, 60, 56, 56, 60, -112, -141, 13, -92, 106, -9, -9, 106, -92, 13, -190, -500, -481, -110, -110, -481, -500, -190, -443, -331, 20, -157, -157, 20, -331, -443, -500, -500, -500, -500, -500, -500, -500, -500}, {-357, -154, -142, -111, -111, -142, -154, -357, -106, -150, -57, -18, -18, -57, -150, -106, -166, -101, -61, 102, 102, -61, -101, -166, -53, 0, 160, 184, 184, 160, 0, -53, 70, -60, 52, 84, 84, 52, -60, 70, -130, -475, -297, -112, -112, -297, -475, -130, -314, -153, -117, -16, -16, -117, -153, -314, -500, -262, -437, -268, -268, -437, -262, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{50, 290, 107, 118, 118, 107, 290, 50, 177, 217, 234, 138, 138, 234, 217, 177, 38, 182, 161, 164, 164, 161, 182, 38, -44, -105, 64, 123, 123, 64, -105, -44, -250, -102, -156, -33, -33, -156, -102, -250, -60, -500, -500, -455, -455, -500, -500, -60, 48, -356, -257, -500, -500, -257, -356, 48, -144, -500, -500, -500, -500, -500, -500, -144}, {-116, -30, 47, 12, 12, 47, -30, -116, 36, 50, 41, 66, 66, 41, 50, 36, -38, 38, 109, 137, 137, 109, 38, -38, -24, 60, 129, 177, 177, 129, 60, -24, -142, -30, -2, 58, 58, -2, -30, -142, 149, -210, -383, -274, -274, -383, -210, 149, -10, -39, -276, -255, -255, -276, -39, -10, 133, 188, 141, -101, -101, 141, 188, 133}};
const int Scoring::Params::ROOK_PST[2][64] = {{-382, -286, -250, -225, -225, -250, -286, -382, -411, -240, -254, -241, -241, -254, -240, -411, -358, -226, -321, -332, -332, -321, -226, -358, -354, -334, -382, -298, -298, -382, -334, -354, -277, -221, -209, -186, -186, -209, -221, -277, -237, -33, -81, -76, -76, -81, -33, -237, -197, -386, -44, -66, -66, -44, -386, -197, -127, -289, -500, -500, -500, -500, -289, -127}, {12, 31, 87, 44, 44, 87, 31, 12, -21, 12, 63, 47, 47, 63, 12, -21, -17, 32, 42, 40, 40, 42, 32, -17, 49, 94, 113, 91, 91, 113, 94, 49, 170, 174, 203, 189, 189, 203, 174, 170, 233, 298, 284, 236, 236, 284, 298, 233, 130, 157, 237, 200, 200, 237, 157, 130, 294, 314, 269, 185, 185, 269, 314, 294}};
const int Scoring::Params::QUEEN_PST[2][64] = {{54, 75, 80, 169, 169, 80, 75, 54, 155, 150, 177, 207, 207, 177, 150, 155, 79, 140, 162, 84, 84, 162, 140, 79, 97, 140, 88, 107, 107, 88, 140, 97, 114, 74, 79, 154, 154, 79, 74, 114, 57, 105, 145, 132, 132, 145, 105, 57, -60, -156, -15, -41, -41, -15, -156, -60, -62, 390, 170, 237, 237, 170, 390, -62}, {-76, -192, -201, -127, -127, -201, -192, -76, -78, -122, -88, 77, 77, -88, -122, -78, -64, 127, 212, 207, 207, 212, 127, -64, 91, 264, 307, 399, 399, 307, 264, 91, 279, 459, 496, 500, 500, 496, 459, 279, 200, 452, 500, 500, 500, 500, 452, 200, 361, 500, 500, 500, 500, 500, 500, 361, 93, 82, 202, 323, 323, 202, 82, 93}};
const int Scoring::Params::KING_PST[2][64] = {{268, 366, 82, 171, 171, 82, 366, 268, 444, 399, 172, 320, 320, 172, 399, 444, -164, 188, -258, -251, -251, -258, 188, -164, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -129, -500, -500, -500, -500, -129, -500, -500, -373, -500, -18, -18, -500, -373, -500}, {-466, -449, -474, -500, -500, -474, -449, -466, -335, -367, -420, -402, -402, -420, -367, -335, -392, -389, -349, -333, -333, -349, -389, -392, -346, -292, -195, -179, -179, -195, -292, -346, -213, -104, -25, 28, 28, -25, -104, -213, -117, -32, 85, 98, 98, 85, -32, -117, -156, -1, 45, 30, 30, 45, -1, -156, -471, -105, -12, -185, -185, -12, -105, -471}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-479, -248, -122, -59, -8, 50, 75, 75, 27};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-333, -209, -111, -47, 9, 59, 90, 109, 154, 164, 180, 230, 195, 186, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -222, -173, -149, -136, -89, -53, 21, 50, 67, 94, 106, 244, 200, 244}, {-333, -144, 0, 115, 162, 184, 184, 205, 254, 282, 311, 333, 333, 302, 278}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-121, 44, 7, 45, 86, 126, 169, 193, 217, 256, 250, 286, 283, 277, 253, 265, 299, 333, 333, 333, 333, 333, 256, 333, 333, -333, -333, 21, 210}, {-333, -333, -333, -302, -136, 20, 194, 160, 251, 246, 267, 302, 284, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, -126, 333, -305, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -127, 5, 80, 109};

const int Scoring::Params::KNIGHT_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{53, 0},{181, 237},{154, 160},{387, 112},{387, 112},{154, 160},{181, 237},{53, 0},{0, 5},{500, 500},{500, 445},{257, 331},{257, 331},{500, 445},{500, 500},{0, 5},{9, 31},{12, 59},{58, 33},{12, 51},{12, 51},{58, 33},{12, 59},{9, 31},{0, 0},{0, 28},{0, 0},{0, 40},{0, 40},{0, 0},{0, 28},{0, 0}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{127, 0},{243, 209},{278, 306},{496, 316},{496, 316},{278, 306},{243, 209},{127, 0},{0, 129},{500, 500},{500, 500},{500, 402},{500, 402},{500, 500},{500, 500},{0, 129},{443, 0},{231, 0},{109, 156},{500, 116},{500, 116},{109, 156},{231, 0},{443, 0},{0, 0},{500, 347},{500, 500},{496, 0},{496, 0},{500, 500},{500, 347},{0, 0}}};
const int Scoring::Params::BISHOP_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{149, 268},{101, 169},{361, 173},{331, 249},{331, 249},{361, 173},{101, 169},{149, 268},{364, 231},{500, 357},{500, 500},{362, 428},{362, 428},{500, 500},{500, 357},{364, 231},{54, 206},{0, 255},{12, 347},{0, 326},{0, 326},{12, 347},{0, 255},{54, 206},{124, 177},{0, 168},{0, 172},{0, 196},{0, 196},{0, 172},{0, 168},{124, 177}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 37},{277, 183},{348, 154},{453, 179},{453, 179},{348, 154},{277, 183},{0, 37},{0, 0},{458, 372},{500, 500},{500, 365},{500, 365},{500, 500},{458, 372},{0, 0},{427, 0},{500, 0},{145, 0},{500, 0},{500, 0},{145, 0},{500, 0},{427, 0},{0, 0},{500, 0},{500, 0},{418, 0},{418, 0},{500, 0},{500, 0},{0, 0}}};


