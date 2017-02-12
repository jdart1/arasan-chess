// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 11-Feb-2017 11:57:02 by "tuner -n 150 -c 24 -o adam /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 39, 199, 89};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 791, 357};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2487, -1304};
const int Scoring::Params::CASTLING[6] = {100, -212, -189, 0, 66, -148};
const int Scoring::Params::KING_COVER[6][4] = {{40, 81, 49, 27},
{6, 11, 7, 4},
{-87, -174, -106, -58},
{-53, -105, -64, -35},
{-150, -300, -182, -100},
{-214, -428, -259, -143}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 451;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -319;
const int Scoring::Params::MINOR_FOR_PAWNS = 205;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 230;
const int Scoring::Params::PAWN_ENDGAME2 = 253;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 14;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 14;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 28;
const int Scoring::Params::MINOR_ATTACK_BOOST = 43;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 49;
const int Scoring::Params::ROOK_ATTACK_BOOST = 23;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 35;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 37;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 53;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 60;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 22;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 81;
const int Scoring::Params::PIECE_THREAT_MM_MID = 157;
const int Scoring::Params::PIECE_THREAT_MR_MID = 258;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 195;
const int Scoring::Params::PIECE_THREAT_MM_END = 207;
const int Scoring::Params::PIECE_THREAT_MR_END = 442;
const int Scoring::Params::PIECE_THREAT_MQ_END = 649;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 72;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 170;
const int Scoring::Params::PIECE_THREAT_RM_MID = 123;
const int Scoring::Params::PIECE_THREAT_RR_MID = 68;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 390;
const int Scoring::Params::PIECE_THREAT_RM_END = 123;
const int Scoring::Params::PIECE_THREAT_RR_END = 73;
const int Scoring::Params::PIECE_THREAT_RQ_END = 126;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 104;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 158;
const int Scoring::Params::ENDGAME_KING_THREAT = 240;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 293;
const int Scoring::Params::BISHOP_PAIR_END = 511;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -76;
const int Scoring::Params::BAD_BISHOP_MID = -39;
const int Scoring::Params::BAD_BISHOP_END = -48;
const int Scoring::Params::CENTER_PAWN_BLOCK = -235;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -22;
const int Scoring::Params::WEAK_PAWN_END = -1;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -175;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -186;
const int Scoring::Params::SPACE = 34;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 6;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 211;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 277;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 441;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 190;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 143;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 228;
const int Scoring::Params::ROOK_BEHIND_PP_END = 0;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 212;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 82;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 22;
const int Scoring::Params::SUPPORTED_PASSER6 = 1000;
const int Scoring::Params::SUPPORTED_PASSER7 = 1500;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -33;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {22, 25, 23};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 30, 53, 63, 89};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 189, 436, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 187, 140};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {2, 5, 8, 11, 14, 18, 21, 25, 29, 33, 38, 42, 47, 52, 58, 64, 70, 76, 83, 90, 97, 105, 113, 122, 131, 140, 150, 161, 172, 184, 196, 209, 222, 236, 251, 267, 283, 300, 318, 337, 357, 377, 399, 421, 445, 469, 495, 521, 549, 578, 608, 640, 672, 706, 741, 778, 816, 855, 896, 938, 981, 1026, 1072, 1120, 1169, 1220, 1271, 1324, 1379, 1435, 1491, 1550, 1609, 1669, 1731, 1793, 1856, 1920, 1985, 2051, 2117, 2183, 2250, 2317, 2385, 2452, 2519, 2587, 2654, 2721, 2787, 2853, 2919, 2984, 3048, 3111, 3173, 3235, 3295, 3354, 3413, 3469, 3525, 3580, 3633, 3684, 3735, 3784, 3832, 3878, 3923, 3966, 4008, 4049, 4088, 4126, 4163, 4198, 4232, 4264, 4296, 4326, 4355, 4383, 4409, 4435, 4459, 4483, 4505, 4527, 4547, 4567, 4586, 4604, 4621, 4637, 4653, 4668, 4682, 4695, 4708, 4720, 4732, 4743, 4754, 4764, 4773, 4782, 4791, 4799};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 933, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 241, 749, 854}, {0, 0, 0, 0, 211, 641, 1000, 1750}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 20, 589, 0}, {0, 0, 0, 0, 0, 128, 609, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 89, 500, 1000, 1500}, {0, 0, 0, 1, 301, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 214, 716, 1350}, {0, 0, 0, 50, 80, 253, 549, 747}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-328, -280, -223, -161, -264, 0, -186, -285, -153, -317, -85, -212, 0, -324, -333, -121, -97, -333, 0, -333, 0}, {0, 0, 0, -147, -85, -11, -30, -74, -160, -126, -141, -92, -106, -77, -271, -105, -76, -128, 0, -32, -286}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -316, -279, -299, -333, -98, -7, -28, -190, -23, -39, -333, -200, -333}, {-135, -117, -93, -93, -115, -127, -171, -126, -72, -66, -84, -214, -82, -43, -83, -333, -192, -121, -333, -146, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -1, -179, -148, -148, -179, -1, -250}, {-250, -146, -224, -185, -185, -224, -146, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-394, -206, -166, 0, 0, -166, -206, -394}, {0, -400, -216, -302, -302, -216, -400, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -71, -87, -87, -71, 0, 0}, {-45, -76, -87, -128, -128, -87, -76, -45}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-477, -244, -70, -47, -47, -70, -244, -477, -116, -172, -137, 18, 18, -137, -172, -116, -239, -75, -2, 81, 81, -2, -75, -239, -140, -94, 58, 49, 49, 58, -94, -140, 18, -83, 64, 25, 25, 64, -83, 18, -118, -371, -336, -97, -97, -336, -371, -118, -286, -260, 1, -382, -382, 1, -260, -286, -500, -500, -500, -500, -500, -500, -500, -500}, {-422, -208, -203, -158, -158, -203, -208, -422, -176, -191, -79, -53, -53, -79, -191, -176, -199, -119, -88, 61, 61, -88, -119, -199, -83, -52, 108, 120, 120, 108, -52, -83, 5, -75, 18, -3, -3, 18, -75, 5, -112, -341, -225, -80, -80, -225, -341, -112, -280, -110, -108, 7, 7, -108, -110, -280, -500, -225, -320, -221, -221, -320, -225, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-20, 232, 50, 70, 70, 50, 232, -20, 135, 177, 196, 95, 95, 196, 177, 135, 2, 123, 119, 117, 117, 119, 123, 2, -62, -116, 25, 107, 107, 25, -116, -62, -267, -95, -78, -12, -12, -78, -95, -267, 73, -338, -500, -324, -324, -500, -338, 73, -100, -324, -230, -500, -500, -230, -324, -100, -165, -500, -500, -500, -500, -500, -500, -165}, {-172, -106, -12, -45, -45, -12, -106, -172, -84, -16, -29, 14, 14, -29, -16, -84, -90, -13, 52, 89, 89, 52, -13, -90, -60, 12, 70, 121, 121, 70, 12, -60, -98, -52, -88, -45, -45, -88, -52, -98, 177, -240, -259, -239, -239, -259, -240, 177, 73, 129, -12, -7, -7, -12, 129, 73, 97, 209, 315, 54, 54, 315, 209, 97}};
const int Scoring::Params::ROOK_PST[2][64] = {{-323, -239, -197, -172, -172, -197, -239, -323, -366, -187, -195, -195, -195, -195, -187, -366, -312, -185, -262, -288, -288, -262, -185, -312, -350, -303, -298, -245, -245, -298, -303, -350, -221, -184, -173, -155, -155, -173, -184, -221, -186, 23, -93, -104, -104, -93, 23, -186, -102, -272, -42, -99, -99, -42, -272, -102, -93, -172, -500, -500, -500, -500, -172, -93}, {12, 40, 94, 53, 53, 94, 40, 12, -46, -4, 45, 30, 30, 45, -4, -46, -15, 35, 34, 29, 29, 34, 35, -15, 54, 97, 106, 77, 77, 106, 97, 54, 139, 143, 192, 165, 165, 192, 143, 139, 200, 239, 240, 210, 210, 240, 239, 200, 94, 131, 192, 155, 155, 192, 131, 94, 229, 271, 262, 165, 165, 262, 271, 229}};
const int Scoring::Params::QUEEN_PST[2][64] = {{45, 59, 64, 164, 164, 64, 59, 45, 118, 135, 167, 191, 191, 167, 135, 118, 60, 122, 144, 70, 70, 144, 122, 60, 86, 113, 70, 96, 96, 70, 113, 86, 106, 70, 98, 110, 110, 98, 70, 106, 70, 80, 133, 86, 86, 133, 80, 70, -41, -96, -15, -57, -57, -15, -96, -41, -18, 186, 124, 97, 97, 124, 186, -18}, {20, -77, -42, -25, -25, -42, -77, 20, 61, -15, 10, 143, 143, 10, -15, 61, 55, 217, 254, 252, 252, 254, 217, 55, 209, 322, 335, 397, 397, 335, 322, 209, 298, 452, 430, 500, 500, 430, 452, 298, 308, 465, 500, 500, 500, 500, 465, 308, 397, 500, 500, 500, 500, 500, 500, 397, 209, 267, 309, 414, 414, 309, 267, 209}};
const int Scoring::Params::KING_PST[2][64] = {{274, 363, 107, 75, 75, 107, 363, 274, 465, 411, 231, 150, 150, 231, 411, 465, -48, 27, -217, -352, -352, -217, 27, -48, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -370, -500, -500, -500, -500, -370, -500, -500, -2, -500, -500, -500, -500, -2, -500}, {-444, -408, -423, -452, -452, -423, -408, -444, -319, -333, -371, -357, -357, -371, -333, -319, -363, -348, -309, -288, -288, -309, -348, -363, -316, -263, -170, -139, -139, -170, -263, -316, -192, -102, -32, 23, 23, -32, -102, -192, -115, -20, 79, 98, 98, 79, -20, -115, -64, 54, 74, 46, 46, 74, 54, -64, -404, -74, -32, -151, -151, -32, -74, -404}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-471, -247, -120, -58, -1, 54, 84, 105, 63};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-323, -185, -97, -31, 24, 68, 103, 131, 170, 190, 195, 241, 195, 153, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -228, -174, -149, -133, -81, -43, 34, 70, 87, 105, 128, 210, 170, 215}, {-333, -158, -26, 67, 124, 151, 148, 181, 228, 250, 292, 319, 333, 327, 318}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-189, -5, 0, 32, 71, 120, 159, 177, 206, 242, 243, 275, 269, 264, 264, 255, 288, 333, 333, 333, 333, 333, 200, 333, 333, -333, -333, -333, 210}, {-333, -333, -333, -246, -211, -23, 99, 123, 212, 187, 243, 258, 269, 333, 333, 333, 333, 333, 333, 333, 301, 317, 333, 333, 275, -3, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -165, -49, 32, 53};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 160, 135, 212, 212, 135, 160, 10, 0, 333, 333, 213, 213, 333, 333, 0, 22, 12, 44, 0, 0, 44, 12, 22, 0, 0, 0, 18, 18, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 70, 219, 261, 333, 333, 261, 219, 70, 2, 333, 333, 333, 333, 333, 333, 2, 99, 65, 163, 303, 303, 163, 65, 99, 0, 333, 333, 0, 0, 333, 333, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 39, 302, 333, 333, 302, 39, 10, 10, 264, 333, 333, 333, 333, 264, 10, 10, 333, 0, 333, 333, 0, 333, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 166, 102, 226, 273, 273, 226, 102, 166, 201, 333, 333, 333, 333, 333, 333, 201, 49, 0, 12, 0, 0, 12, 0, 49, 150, 116, 0, 0, 0, 0, 116, 150},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 192, 198, 289, 289, 198, 192, 0, 0, 333, 333, 333, 333, 333, 333, 0, 0, 291, 212, 188, 188, 212, 291, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 236, 225, 240, 240, 225, 236, 10, 10, 14, 264, 264, 264, 264, 14, 10, 10, 160, 164, 0, 0, 164, 160, 10, 10, 333, 333, 333, 333, 333, 333, 10}};
