// Copyright 2015, 2016 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {-231, 44, 141, 131};
const int Scoring::Params::RBN_ADJUST[4] = {-381, -556, -706, -881};
const int Scoring::Params::QR_ADJUST[4] = {-381, 119, 619, 619};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2281, -1381};
const int Scoring::Params::CASTLING[6] = {100, -189, -190, 201, 201, -161};
const int Scoring::Params::KING_COVER[5][4] = {{188, 375, 230, 273},
{147, 294, 181, 214},
{123, 246, 151, 179},
{35, 71, 43, 51},
{-225, -450, -277, -328}};
const int Scoring::Params::KING_COVER_BASE = -350;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 346;
const int Scoring::Params::PIN_MULTIPLIER_END = 408;
const int Scoring::Params::ROOK_VS_PAWNS = 214;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -219;
const int Scoring::Params::MINOR_FOR_PAWNS = 381;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 150;
const int Scoring::Params::PAWN_ENDGAME1 = 107;
const int Scoring::Params::PAWN_ENDGAME2 = 154;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 15;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 15;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 29;
const int Scoring::Params::MINOR_ATTACK_BOOST = 39;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 40;
const int Scoring::Params::ROOK_ATTACK_BOOST = 38;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 39;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 38;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 57;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 59;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 23;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 67;
const int Scoring::Params::PIECE_THREAT_MM_MID = 145;
const int Scoring::Params::PIECE_THREAT_MR_MID = 169;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 169;
const int Scoring::Params::PIECE_THREAT_MM_END = 169;
const int Scoring::Params::PIECE_THREAT_MR_END = 169;
const int Scoring::Params::PIECE_THREAT_MQ_END = 169;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 89;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 89;
const int Scoring::Params::PIECE_THREAT_RM_MID = 143;
const int Scoring::Params::PIECE_THREAT_RR_MID = 169;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 169;
const int Scoring::Params::PIECE_THREAT_RM_END = 169;
const int Scoring::Params::PIECE_THREAT_RR_END = 169;
const int Scoring::Params::PIECE_THREAT_RQ_END = 169;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 128;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 148;
const int Scoring::Params::ENDGAME_KING_THREAT = 169;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 425;
const int Scoring::Params::BISHOP_PAIR_END = 458;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -80;
const int Scoring::Params::BAD_BISHOP_MID = -31;
const int Scoring::Params::BAD_BISHOP_END = -60;
const int Scoring::Params::CENTER_PAWN_BLOCK = -246;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -28;
const int Scoring::Params::WEAK_PAWN_END = -31;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -163;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -190;
const int Scoring::Params::SPACE = 32;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 36;
const int Scoring::Params::ROOK_ON_7TH_MID = 237;
const int Scoring::Params::ROOK_ON_7TH_END = 267;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 131;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 354;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 198;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 186;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 118;
const int Scoring::Params::ROOK_BEHIND_PP_END = 79;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 243;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 24;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 80;
const int Scoring::Params::SUPPORTED_PASSER6 = 520;
const int Scoring::Params::SUPPORTED_PASSER7 = 847;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -47;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {10, 17, 27};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {0, 0, 13, 19, 45};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 60, 159, 169, 179};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 165, 128};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5, 11, 21, 31, 39, 52, 58, 72, 91, 106, 115, 133, 146, 164, 176, 199, 218, 237, 255, 275, 295, 318, 338, 362, 384, 411, 435, 460, 487, 514, 542, 571, 599, 627, 657, 689, 718, 754, 784, 818, 853, 890, 926, 957, 995, 1032, 1070, 1110, 1150, 1191, 1231, 1273, 1318, 1355, 1399, 1443, 1488, 1533, 1580, 1629, 1674, 1721, 1767, 1819, 1867, 1919, 1966, 2016, 2062, 2110, 2152, 2200, 2251, 2290, 2334, 2378, 2413, 2470, 2496, 2536, 2576, 2615, 2653, 2687, 2728, 2765, 2798, 2836, 2871, 2897, 2928, 3080, 2996, 3022, 3052, 3099, 3110, 3138, 3167, 3191, 3217, 3242, 3268, 3295, 3333, 3340, 3362, 3385, 3406, 3426, 3442, 3462, 3480, 3496, 3517, 3532, 3547, 3559, 3573, 3590, 3599, 3611, 3622, 3633, 3643, 3652, 3668, 3669, 3676, 3683, 3689, 3694, 3699, 3703, 3706, 3709, 3711, 3712};
const int Scoring::Params::TRADE_DOWN[8] = {807, 483, 397, 311, 225, 139, 53, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 50, 70, 329, 809, 1128}, {0, 0, 0, 50, 223, 500, 959, 1529}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 90, 355, 0}, {0, 0, 0, 0, 21, 140, 482, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 50, 87, 346, 650, 890}, {0, 0, 0, 50, 208, 356, 674, 919}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 195, 425, 890}, {0, 0, 0, 50, 70, 270, 436, 832}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-134, -134, -14, 0, -134, 0, -135, -149, -17, -149, -100, -164, 0, -164, -164, -15, -179, -52, 0, -194, 0}, {-87, -65, -31, -162, -161, -45, -134, -162, -188, -205, -149, -109, -71, -151, -198, -53, -53, -53, -96, -96, -158}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-290, -204, -176, -161, -153, -147, -309, -214, -182, -166, -157, -333, -188, -113, -113, -206, -49, -25, -290, -93, -333}, {-219, -194, -169, -175, -172, -217, -230, -200, -186, -190, -232, -250, -82, -73, -188, -323, -150, -91, -333, -208, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-166, -38, -201, -101, -101, -201, -38, -166}, {-201, -119, -203, -182, -182, -203, -119, -201}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-189, -73, -177, -43, -43, -177, -73, -189}, {-4, -303, -166, -323, -323, -166, -303, -4}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -111, -134, -134, -111, 0, 0}, {-100, -112, -105, -166, -166, -105, -112, -100}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-339, -248, -125, -123, -123, -125, -248, -339, -185, -179, -153, 9, 9, -153, -179, -185, -239, -75, 16, 100, 100, 16, -75, -239, -166, -120, 69, 66, 66, 69, -120, -166, 4, -27, 96, 78, 78, 96, -27, 4, -117, -9, -3, 72, 72, -3, -9, -117, -269, -179, 62, -149, -149, 62, -179, -269, -299, -209, -189, -179, -179, -189, -209, -299}, {-349, -309, -279, -269, -269, -279, -309, -349, -249, -209, -169, -159, -159, -169, -209, -249, -209, -169, -139, 4, 4, -139, -169, -209, -199, -152, 38, 38, 38, 38, -152, -199, -72, -29, 30, 29, 29, 30, -29, -72, -209, -159, 7, 16, 16, 7, -159, -209, -249, -189, -159, -142, -142, -159, -189, -249, -289, -249, -209, -199, -199, -209, -249, -289}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-106, -1, -42, -36, -36, -42, -1, -106, 89, 124, 119, 27, 27, 119, 124, 89, -32, 86, 56, 89, 89, 56, 86, -32, -95, -119, 10, 103, 103, 10, -119, -95, -119, -59, -32, 65, 65, -32, -59, -119, 173, -19, -19, -19, -19, -19, -19, 173, -70, -219, -196, -219, -219, -196, -219, -70, -219, -219, -219, -219, -219, -219, -219, -219}, {-194, -157, -73, -74, -74, -73, -157, -194, -162, -47, -72, -29, -29, -72, -47, -162, -124, -41, 44, 74, 74, 44, -41, -124, -74, -7, 40, 84, 84, 40, -7, -74, -41, -8, -4, 51, 51, -4, -8, -41, 114, -94, -54, 21, 21, -54, -94, 114, -5, 80, -29, -44, -44, -29, 80, -5, 40, 74, 114, -38, -38, 114, 74, 40}};
const int Scoring::Params::ROOK_PST[2][64] = {{-55, 24, 25, 48, 48, 25, 24, -55, -115, 14, -14, -9, -9, -14, 14, -115, -54, 65, -47, -76, -76, -47, 65, -54, -100, 2, -75, -23, -23, -75, 2, -100, 49, 63, 66, 65, 65, 66, 63, 49, 77, 119, 119, 118, 118, 119, 119, 77, 1, -119, 17, 7, 7, 17, -119, 1, 35, -119, -119, -119, -119, -119, -119, 35}, {-43, -26, 23, -12, -12, 23, -26, -43, -111, -76, 1, -56, -56, 1, -76, -111, -64, 14, -33, -53, -53, -33, 14, -64, 28, 47, 48, 13, 13, 48, 47, 28, 63, 80, 110, 78, 78, 110, 80, 63, 116, 119, 119, 103, 103, 119, 119, 116, 3, 20, 59, 4, 4, 59, 20, 3, 119, 119, -31, -119, -119, -31, 119, 119}};
const int Scoring::Params::QUEEN_PST[2][64] = {{-37, -76, -76, -2, -2, -76, -76, -37, -9, -15, 36, 59, 59, 36, -15, -9, -59, -4, 12, -41, -41, 12, -4, -59, -32, 21, 1, 11, 11, 1, 21, -32, 7, 3, 23, 34, 34, 23, 3, 7, 5, 72, 85, 53, 53, 85, 72, 5, -119, -119, 3, -119, -119, 3, -119, -119, -119, 85, 52, 5, 5, 52, 85, -119}, {-119, -119, -119, -119, -119, -119, -119, -119, -119, -119, -119, -98, -98, -119, -119, -119, -119, -15, 40, 11, 11, 40, -15, -119, -17, 102, 69, 119, 119, 69, 102, -17, 61, 119, 119, 119, 119, 119, 119, 61, 7, 119, 119, 119, 119, 119, 119, 7, 119, 119, 119, 119, 119, 119, 119, 119, -119, 1, 18, 66, 66, 18, 1, -119}};
const int Scoring::Params::KING_PST[2][64] = {{-49, 49, -119, 7, 7, -119, 49, -49, 119, 113, -3, 119, 119, -3, 113, 119, -65, 59, -67, -67, -67, -67, 59, -65, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -241, -479, -241, -241, -479, -241, -479, -479, -241, -454, -479, -479, -454, -241, -479}, {-332, -200, -209, -199, -199, -209, -200, -332, -181, -130, -167, -127, -127, -167, -130, -181, -220, -155, -106, -85, -85, -106, -155, -220, -166, -80, 25, 74, 74, 25, -80, -166, -52, 73, 131, 180, 180, 131, 73, -52, 58, 171, 239, 256, 256, 239, 171, 58, 124, 249, 250, 248, 248, 250, 249, 124, -39, 141, 189, 111, 111, 189, 141, -39}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-299, -189, -89, -28, 35, 79, 94, 106, 1};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-310, -181, -82, -28, 18, 38, 47, 62, 75, 69, 72, 93, 44, 20, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-290, -79, -41, -47, -43, 9, 74, 142, 173, 183, 198, 213, 270, 252, 311}, {-333, -209, -112, 61, 78, 96, 86, 145, 193, 215, 248, 268, 283, 297, 305}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-214, -61, -55, -25, 12, 58, 89, 119, 151, 187, 182, 212, 204, 209, 220, 216, 227, 314, 244, 314, 282, 329, 91, 329, 329, 91, 91, 91, 210}, {-239, -179, -129, -109, -69, -29, 76, 87, 139, 159, 202, 223, 248, 325, 322, 333, 333, 333, 333, 333, 313, 333, 333, 333, 333, 141, 333, 141, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-319, -158, -67, 7, 14};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 73, 82, 112, 112, 82, 73, 50, 0, 59, 67, 107, 107, 67, 59, 0, 0, 0, 63, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 109, 151, 199, 259, 259, 199, 151, 109, 13, 133, 209, 259, 259, 209, 133, 13, 129, 18, 124, 179, 179, 124, 18, 129, 0, 129, 129, 0, 0, 129, 129, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 209, 259, 259, 209, 0, 10, 10, 0, 209, 225, 225, 209, 0, 10, 10, 129, 0, 179, 179, 0, 129, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 128, 123, 123, 128, 0, 0, 124, 134, 14, 31, 31, 14, 134, 124, 40, 22, 0, 0, 0, 0, 22, 40, 76, 104, 31, 0, 0, 31, 104, 76},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 33, 112, 112, 33, 19, 0, 0, 48, 75, 66, 66, 75, 48, 0, 99, 129, 54, 0, 0, 54, 129, 99, 0, 42, 0, 0, 0, 0, 42, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 14, 13, 64, 64, 13, 14, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 9, 0, 0, 0, 0, 9, 10, 10, 129, 129, 129, 129, 129, 129, 10}};
