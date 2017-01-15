// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {69, 116, 338, 13};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 723, 340};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2077, -1255};
const int Scoring::Params::CASTLING[6] = {100, -141, -108, 44, 141, -87};
const int Scoring::Params::KING_COVER[6][4] = {{121, 126, 81, 81},
{0, 0, 0, 0},
{-209, -218, -140, -140},
{-25, -26, -17, -17},
{-77, -80, -52, -52},
{-214, -223, -143, -143}};
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 500;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -318;
const int Scoring::Params::MINOR_FOR_PAWNS = 155;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 241;
const int Scoring::Params::PAWN_ENDGAME1 = 186;
const int Scoring::Params::PAWN_ENDGAME2 = 272;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 8;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 8;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 22;
const int Scoring::Params::MINOR_ATTACK_BOOST = 35;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 31;
const int Scoring::Params::ROOK_ATTACK_BOOST = 25;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 34;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 28;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 51;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 50;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 21;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 95;
const int Scoring::Params::PIECE_THREAT_MM_MID = 189;
const int Scoring::Params::PIECE_THREAT_MR_MID = 370;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 376;
const int Scoring::Params::PIECE_THREAT_MM_END = 257;
const int Scoring::Params::PIECE_THREAT_MR_END = 457;
const int Scoring::Params::PIECE_THREAT_MQ_END = 750;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 83;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 162;
const int Scoring::Params::PIECE_THREAT_RM_MID = 166;
const int Scoring::Params::PIECE_THREAT_RR_MID = 237;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 628;
const int Scoring::Params::PIECE_THREAT_RM_END = 183;
const int Scoring::Params::PIECE_THREAT_RR_END = 305;
const int Scoring::Params::PIECE_THREAT_RQ_END = 750;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 152;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 172;
const int Scoring::Params::ENDGAME_KING_THREAT = 452;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 226;
const int Scoring::Params::BISHOP_PAIR_END = 443;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -62;
const int Scoring::Params::BAD_BISHOP_MID = -31;
const int Scoring::Params::BAD_BISHOP_END = -44;
const int Scoring::Params::CENTER_PAWN_BLOCK = -238;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -21;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -163;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -180;
const int Scoring::Params::SPACE = 24;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 173;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 137;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 560;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 199;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 112;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 168;
const int Scoring::Params::ROOK_BEHIND_PP_END = 0;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 261;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 94;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 0;
const int Scoring::Params::SUPPORTED_PASSER6 = 1000;
const int Scoring::Params::SUPPORTED_PASSER7 = 1500;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -11;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {26, 33, 120};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 27, 27, 20, 25};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 197, 404, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={150, 140, 128};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 0, 0, 73, 16, 79, 44, 0, 137, 33, 59, 79, 176, 182, 229, 166, 93, 172, 179, 387, 325, 365, 345, 410, 336, 348, 555, 606, 666, 599, 663, 714, 760, 619, 744, 836, 843, 829, 873, 990, 1040, 1008, 1100, 1176, 1222, 1176, 1229, 1364, 1420, 1389, 1490, 1525, 1580, 1524, 1472, 1696, 1763, 1736, 1798, 1960, 1885, 1836, 1988, 2061, 1932, 1984, 2085, 2184, 2243, 2341, 2331, 2538, 2610, 2470, 2449, 2462, 2447, 2649, 2755, 2685, 2449, 2558, 2855, 2722, 2707, 2785, 2881, 2972, 2854, 3177, 2852, 3085, 3231, 3102, 3387, 3316, 3422, 3408, 3636, 3631, 3519, 3605, 3726, 3288, 2859, 3119, 2927, 2697, 2777, 2413, 3173, 3329, 3898, 4031, 4415, 3932, 3507, 3404, 3958, 3465, 3470, 3440, 3679, 3865, 3887, 3773, 3964, 4033, 3989, 3935, 3882, 3787, 3761, 3708, 3699, 3703, 3706, 3709, 3711, 3712};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 269, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 50, 70, 202, 709, 859}, {0, 0, 0, 50, 124, 500, 1000, 1750}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 30, 728, 0}, {0, 0, 0, 0, 0, 110, 137, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 50, 70, 482, 1000, 1500}, {0, 0, 0, 50, 275, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 75, 609, 1014}, {0, 0, 0, 50, 70, 282, 432, 365}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-270, -220, -253, -46, -285, -147, -261, -307, -203, -293, -102, -283, -81, -269, -7, -130, -268, -242, 0, -39, 0}, {-69, 0, -74, -192, -167, -57, -105, -180, -223, -249, -117, -93, -30, -62, -65, -15, 0, 0, 0, 0, -311}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -285, -274, -333, -333, -333, -325, -317, -333, -208, -74, -159, -148, 0, 0, -295, -111, -333}, {-199, -141, -193, -175, -134, -212, -221, -203, -217, -163, -214, -160, -37, -21, -29, -265, -127, -70, -333, -215, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-249, -29, -222, -200, -200, -222, -29, -249}, {-250, -183, -250, -226, -226, -250, -183, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-459, -195, -30, 0, 0, -30, -195, -459}, {0, -500, -324, -500, -500, -324, -500, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -110, -90, -90, -110, 0, 0}, {-30, -47, -66, -100, -100, -66, -47, -30}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -228, -77, -53, -53, -77, -228, -500, -110, -203, -161, 44, 44, -161, -203, -110, -252, -61, 37, 118, 118, 37, -61, -252, -119, -109, 73, 86, 86, 73, -109, -119, 62, -39, 120, 68, 68, 120, -39, 62, -134, -347, -493, -33, -33, -493, -347, -134, -459, -312, 65, -311, -311, 65, -312, -459, -500, -500, -500, -500, -500, -500, -500, -500}, {-464, -289, -313, -276, -276, -313, -289, -464, -230, -271, -202, -128, -128, -202, -271, -230, -278, -225, -155, -13, -13, -155, -225, -278, -180, -144, 26, 42, 42, 26, -144, -180, -125, -145, -58, -119, -119, -58, -145, -125, -247, -439, -302, -168, -168, -302, -439, -247, -372, -204, -194, -136, -136, -194, -204, -372, -500, -381, -500, -459, -459, -500, -381, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{73, 310, 122, 135, 135, 122, 310, 73, 201, 229, 267, 177, 177, 267, 229, 201, 60, 202, 186, 208, 208, 186, 202, 60, 0, -61, 100, 154, 154, 100, -61, 0, -210, -42, -104, -2, -2, -104, -42, -210, 230, -318, -500, -346, -346, -500, -318, 230, 74, -380, -152, -500, -500, -152, -380, 74, -151, -500, -500, -500, -500, -500, -500, -151}, {-242, -180, -95, -122, -122, -95, -180, -242, -117, -60, -90, -46, -46, -90, -60, -117, -144, -76, -10, 17, 17, -10, -76, -144, -120, -71, -17, 3, 3, -17, -71, -120, -212, -151, -225, -218, -218, -225, -151, -212, 105, -316, -446, -326, -326, -446, -316, 105, 76, 73, -71, -189, -189, -71, 73, 76, 53, 204, 84, -149, -149, 84, 204, 53}};
const int Scoring::Params::ROOK_PST[2][64] = {{-137, -34, -1, 22, 22, -1, -34, -137, -167, -31, -58, -6, -6, -58, -31, -167, -126, -8, -118, -107, -107, -118, -8, -126, -124, -80, -180, -61, -61, -180, -80, -124, 1, 12, -56, 5, 5, -56, 12, 1, 32, 196, 72, 89, 89, 72, 196, 32, 88, -111, 66, 41, 41, 66, -111, 88, -92, -500, -500, -500, -500, -500, -500, -92}, {-68, -53, 9, -27, -27, 9, -53, -68, -159, -115, -59, -88, -88, -59, -115, -159, -125, -86, -74, -80, -80, -74, -86, -125, -66, -32, -7, -30, -30, -7, -32, -66, 43, 32, 84, 66, 66, 84, 32, 43, 86, 155, 152, 123, 123, 152, 155, 86, 1, 35, 95, 66, 66, 95, 35, 1, 177, 220, -166, -403, -403, -166, 220, 177}};
const int Scoring::Params::QUEEN_PST[2][64] = {{84, 82, 57, 173, 173, 57, 82, 84, 159, 145, 182, 209, 209, 182, 145, 159, 92, 141, 159, 107, 107, 159, 141, 92, 95, 174, 103, 148, 148, 103, 174, 95, 163, 113, 86, 165, 165, 86, 113, 163, 177, 221, 211, 178, 178, 211, 221, 177, -4, -93, 32, -83, -83, 32, -93, -4, -13, 395, 256, -147, -147, 256, 395, -13}, {-357, -424, -415, -347, -347, -415, -424, -357, -355, -369, -327, -179, -179, -327, -369, -355, -280, -120, -65, -62, -62, -65, -120, -280, -106, 15, 0, 92, 92, 0, 15, -106, -15, 119, 188, 325, 325, 188, 119, -15, -73, 96, 297, 357, 357, 297, 96, -73, 82, 220, 322, 452, 452, 322, 220, 82, -497, -393, -186, 0, 0, -186, -393, -497}};
const int Scoring::Params::KING_PST[2][64] = {{231, 382, 100, 53, 53, 100, 382, 231, 423, 356, 139, 126, 126, 139, 356, 423, -320, -26, -500, -485, -485, -500, -26, -320, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 500, -146, -500, -500, -146, 500, -500}, {-443, -374, -391, -439, -439, -391, -374, -443, -308, -285, -314, -291, -291, -314, -285, -308, -348, -304, -240, -204, -204, -240, -304, -348, -326, -214, -79, -39, -39, -79, -214, -326, -156, -27, 91, 162, 162, 91, -27, -156, -91, 16, 199, 198, 198, 199, 16, -91, -69, 96, 188, 158, 158, 188, 96, -69, -464, 85, 119, -128, -128, 119, 85, -464}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-450, -241, -117, -54, -9, 41, 58, 52, -27};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-333, -216, -119, -57, 0, 42, 71, 87, 132, 142, 185, 233, 214, 134, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -222, -173, -146, -139, -98, -64, 10, 30, 38, 49, 24, 185, 89, 265}, {-333, -193, -90, 15, 38, 44, 57, 71, 116, 140, 169, 183, 204, 221, 181}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-142, 0, -3, 37, 79, 128, 174, 204, 233, 270, 269, 297, 294, 294, 290, 291, 333, 315, 266, 333, 333, 333, -133, 333, 333, -333, -333, 23, 210}, {-333, -333, -333, -241, -189, -19, 82, 69, 153, 119, 131, 141, 115, 209, 217, 158, 236, 247, 253, 167, 122, 167, 117, 101, 191, -158, 108, -317, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -286, -179, -120, -89};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 67, 177, 143, 239, 239, 143, 177, 67, 0, 321, 333, 179, 179, 333, 321, 0, 0, 0, 15, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 95, 201, 267, 333, 333, 267, 201, 95, 60, 333, 333, 333, 333, 333, 333, 60, 309, 90, 76, 231, 231, 76, 90, 309, 0, 157, 260, 37, 37, 260, 157, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 41, 247, 333, 333, 247, 41, 10, 10, 163, 333, 332, 332, 333, 163, 10, 10, 226, 0, 333, 333, 0, 226, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 220, 119, 270, 309, 309, 270, 119, 220, 126, 333, 333, 296, 296, 333, 333, 126, 10, 0, 0, 0, 0, 0, 0, 10, 59, 0, 0, 0, 0, 0, 0, 59},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 188, 235, 333, 333, 235, 188, 0, 0, 333, 333, 333, 333, 333, 333, 0, 153, 248, 177, 245, 245, 177, 248, 153, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 246, 251, 283, 283, 251, 246, 10, 10, 166, 230, 206, 206, 230, 166, 10, 10, 131, 108, 0, 0, 108, 131, 10, 10, 263, 323, 289, 289, 323, 263, 10}};
