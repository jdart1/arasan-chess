// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 24-Mar-2017 03:40:05 by "tuner -n 150 -c 24 -R 50 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 46, 242, 86};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 850, 286};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2143, -1195};
const int Scoring::Params::CASTLING[6] = {100, -227, -209, 0, 71, -110};
const int Scoring::Params::KING_COVER[6][4] = {{81, 161, 73, 75},
{51, 101, 46, 47},
{-39, -79, -36, -37},
{-12, -23, -11, -11},
{-77, -155, -71, -72},
{-219, -438, -200, -204}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 458;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -118;
const int Scoring::Params::MINOR_FOR_PAWNS = 177;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 248;
const int Scoring::Params::PAWN_ENDGAME2 = 304;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 9;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 12;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 27;
const int Scoring::Params::MINOR_ATTACK_BOOST = 40;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 45;
const int Scoring::Params::ROOK_ATTACK_BOOST = 14;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 33;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 38;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 45;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 57;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_BASE = 7;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_SLOPE = 18;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 18;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 81;
const int Scoring::Params::PIECE_THREAT_MM_MID = 180;
const int Scoring::Params::PIECE_THREAT_MR_MID = 369;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 273;
const int Scoring::Params::PIECE_THREAT_MM_END = 226;
const int Scoring::Params::PIECE_THREAT_MR_END = 424;
const int Scoring::Params::PIECE_THREAT_MQ_END = 750;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 76;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 199;
const int Scoring::Params::PIECE_THREAT_RM_MID = 148;
const int Scoring::Params::PIECE_THREAT_RR_MID = 105;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 533;
const int Scoring::Params::PIECE_THREAT_RM_END = 161;
const int Scoring::Params::PIECE_THREAT_RR_END = 139;
const int Scoring::Params::PIECE_THREAT_RQ_END = 272;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 119;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 206;
const int Scoring::Params::ENDGAME_KING_THREAT = 325;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 248;
const int Scoring::Params::BISHOP_PAIR_END = 483;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -54;
const int Scoring::Params::BAD_BISHOP_MID = -38;
const int Scoring::Params::BAD_BISHOP_END = -45;
const int Scoring::Params::CENTER_PAWN_BLOCK = -228;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -19;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -166;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -178;
const int Scoring::Params::SPACE = 37;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 165;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 252;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 557;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 165;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 127;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 57;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 206;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 100;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 12;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 649;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 429;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -211;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -375;
const int Scoring::Params::WRONG_COLOR_BISHOP = 0;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -24;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {22, 24, 18};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 210, 487, 742, 750};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 181, 130};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {-19, -17, -15, -13, -11, -8, -6, -3, 0, 3, 6, 9, 13, 17, 21, 25, 30, 34, 40, 45, 51, 57, 64, 71, 78, 86, 94, 103, 113, 122, 133, 144, 156, 168, 181, 195, 210, 226, 242, 260, 278, 297, 318, 339, 362, 386, 411, 437, 465, 494, 525, 557, 590, 626, 662, 701, 741, 783, 826, 871, 918, 967, 1018, 1070, 1124, 1180, 1238, 1297, 1358, 1420, 1484, 1550, 1616, 1685, 1754, 1824, 1896, 1968, 2041, 2114, 2188, 2263, 2337, 2411, 2486, 2560, 2634, 2707, 2779, 2851, 2922, 2992, 3060, 3128, 3194, 3259, 3322, 3383, 3443, 3502, 3558, 3613, 3666, 3718, 3767, 3815, 3861, 3905, 3948, 3989, 4028, 4065, 4101, 4135, 4168, 4199, 4229, 4257, 4284, 4310, 4334, 4358, 4380, 4401, 4420, 4439, 4457, 4474, 4490, 4505, 4519, 4533, 4545, 4558, 4569, 4580, 4590, 4599, 4609, 4617, 4625, 4633, 4640, 4647, 4653, 4659, 4665, 4670, 4675, 4680};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 933, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 197, 718, 770}, {0, 0, 0, 0, 181, 573, 1224, 1705}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {80, 82, 78, 72, 72, 78, 82, 80};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 18, 750, 0}, {0, 0, 0, 0, 0, 145, 746, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 31, 655, 1250, 1750}, {0, 0, 0, 19, 269, 685, 1250, 1750}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 212, 684, 1472}, {0, 0, 0, 50, 70, 225, 390, 699}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-333, -287, -283, -177, -333, 0, -190, -333, -187, -329, -208, -252, 0, -273, -251, -199, -112, -333, 0, -129, 0}, {0, 0, 0, -123, -114, 0, -7, -72, -126, -190, -111, -88, -67, -48, -260, -99, -77, -178, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -114, -7, -64, -170, 0, -16, -329, -134, -333}, {-124, -120, -84, -74, -80, -63, -143, -89, -113, -52, -72, -196, -53, -12, -28, -333, -166, -149, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -9, -196, -163, -163, -196, -9, -250}, {-250, -162, -220, -169, -169, -220, -162, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{0, -294, -128, 0, 0, -128, -294, 0}, {0, -493, -280, -452, -452, -280, -493, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -77, -106, -106, -77, 0, 0}, {-47, -75, -84, -129, -129, -84, -75, -47}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -252, -104, -74, -74, -104, -252, -500, -110, -202, -175, 6, 6, -175, -202, -110, -268, -91, -5, 83, 83, -5, -91, -268, -150, -127, 48, 44, 44, 48, -127, -150, 40, -105, 91, 35, 35, 91, -105, 40, -218, -369, -342, -57, -57, -342, -369, -218, -486, -337, 15, -185, -185, 15, -337, -486, -500, -500, -500, -500, -500, -500, -500, -500}, {-358, -156, -139, -111, -111, -139, -156, -358, -106, -150, -59, -20, -20, -59, -150, -106, -168, -100, -64, 100, 100, -64, -100, -168, -52, -6, 159, 184, 184, 159, -6, -52, 59, -56, 58, 26, 26, 58, -56, 59, -124, -316, -175, -23, -23, -175, -316, -124, -282, -106, -128, -1, -1, -128, -106, -282, -500, -232, -421, -246, -246, -421, -232, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{34, 269, 85, 92, 92, 85, 269, 34, 160, 197, 214, 116, 116, 214, 197, 160, 14, 158, 137, 141, 141, 137, 158, 14, -68, -127, 40, 99, 99, 40, -127, -68, -301, -122, -131, -22, -22, -131, -122, -301, 47, -404, -500, -404, -404, -500, -404, 47, -154, -385, -429, -500, -500, -429, -385, -154, -200, -500, -500, -500, -500, -500, -500, -200}, {-121, -41, 41, 3, 3, 41, -41, -121, 30, 39, 30, 56, 56, 30, 39, 30, -41, 26, 99, 125, 125, 99, 26, -41, -34, 51, 115, 162, 162, 115, 51, -34, -87, -48, -95, -56, -56, -95, -48, -87, 152, -191, -220, -206, -206, -220, -191, 152, 31, 118, -136, 8, 8, -136, 118, 31, 138, 249, 306, 77, 77, 306, 249, 138}};
const int Scoring::Params::ROOK_PST[2][64] = {{-383, -290, -250, -227, -227, -250, -290, -383, -416, -241, -257, -240, -240, -257, -241, -416, -362, -229, -324, -332, -332, -324, -229, -362, -359, -342, -383, -296, -296, -383, -342, -359, -282, -230, -209, -191, -191, -209, -230, -282, -249, -43, -91, -67, -67, -91, -43, -249, -192, -382, -44, -58, -58, -44, -382, -192, -145, -331, -500, -500, -500, -500, -331, -145}, {18, 38, 94, 51, 51, 94, 38, 18, -15, 22, 73, 55, 55, 73, 22, -15, -9, 36, 45, 45, 45, 45, 36, -9, 56, 104, 119, 97, 97, 119, 104, 56, 175, 174, 210, 195, 195, 210, 174, 175, 236, 302, 291, 248, 248, 291, 302, 236, 134, 158, 237, 204, 204, 237, 158, 134, 299, 334, 288, 192, 192, 288, 334, 299}};
const int Scoring::Params::QUEEN_PST[2][64] = {{50, 71, 79, 167, 167, 79, 71, 50, 150, 147, 176, 206, 206, 176, 147, 150, 71, 136, 158, 83, 83, 158, 136, 71, 98, 132, 87, 106, 106, 87, 132, 98, 111, 78, 79, 150, 150, 79, 78, 111, 60, 116, 126, 113, 113, 126, 116, 60, -81, -164, -8, -40, -40, -8, -164, -81, -155, 293, 170, -25, -25, 170, 293, -155}, {-53, -200, -182, -117, -117, -182, -200, -53, -63, -91, -69, 88, 88, -69, -91, -63, -47, 153, 233, 217, 217, 233, 153, -47, 101, 289, 311, 413, 413, 311, 289, 101, 303, 460, 482, 500, 500, 482, 460, 303, 230, 451, 500, 500, 500, 500, 451, 230, 358, 500, 500, 500, 500, 500, 500, 358, 126, 111, 267, 446, 446, 267, 111, 126}};
const int Scoring::Params::KING_PST[2][64] = {{270, 366, 84, 172, 172, 84, 366, 270, 442, 399, 180, 334, 334, 180, 399, 442, -156, 195, -327, -289, -289, -327, 195, -156, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -267, -500, -500, -267, -500, -500, -500, 500, -500, -500, -500, -500, 500, -500}, {-464, -449, -472, -500, -500, -472, -449, -464, -336, -368, -419, -404, -404, -419, -368, -336, -391, -388, -347, -333, -333, -347, -388, -391, -342, -294, -200, -180, -180, -200, -294, -342, -206, -101, -28, 29, 29, -28, -101, -206, -124, -40, 82, 94, 94, 82, -40, -124, -149, 0, 46, 31, 31, 46, 0, -149, -472, -89, -16, -182, -182, -16, -89, -472}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-476, -248, -121, -57, -5, 55, 81, 82, 32};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-324, -202, -103, -39, 19, 67, 100, 118, 168, 179, 197, 250, 188, 212, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -222, -175, -152, -138, -92, -56, 19, 48, 67, 84, 102, 254, 167, 241}, {-333, -139, 5, 113, 163, 184, 187, 207, 252, 281, 316, 333, 333, 307, 279}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-122, 36, -2, 40, 81, 117, 160, 185, 208, 247, 239, 280, 272, 273, 241, 248, 296, 333, 333, 333, 333, 333, 300, 333, 333, -333, -333, -10, 210}, {-333, -333, -333, -333, -139, 21, 200, 160, 255, 255, 279, 290, 290, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 333, 319, -113, 333, -296, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -143, -9, 66, 94};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 204, 146, 244, 244, 146, 204, 0, 0, 333, 333, 221, 221, 333, 333, 0, 13, 10, 42, 15, 15, 42, 10, 13, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 252, 283, 333, 333, 283, 252, 64, 75, 333, 333, 333, 333, 333, 333, 75, 86, 68, 97, 282, 282, 97, 68, 86, 0, 333, 333, 0, 0, 333, 333, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 90, 254, 333, 333, 254, 90, 10, 10, 145, 333, 333, 333, 333, 145, 10, 10, 333, 0, 333, 333, 0, 333, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 142, 267, 332, 332, 267, 142, 192, 220, 333, 333, 332, 332, 333, 333, 220, 158, 58, 181, 0, 0, 181, 58, 158, 158, 95, 0, 0, 0, 0, 95, 158},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 219, 252, 333, 333, 252, 219, 0, 0, 333, 333, 333, 333, 333, 333, 0, 0, 19, 0, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 302, 284, 280, 280, 284, 302, 10, 10, 59, 200, 276, 276, 200, 59, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 333, 118, 333, 333, 118, 333, 10}};
