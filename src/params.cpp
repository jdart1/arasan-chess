// Copyright 2015, 2016 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {-231, 44, 194, 131};
const int Scoring::Params::RBN_ADJUST[4] = {-381, -556, -706, -881};
const int Scoring::Params::QR_ADJUST[4] = {-381, 119, 619, 619};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2281, -1381};
const int Scoring::Params::CASTLING[6] = {-10, -189, -194, 296, 236, -161};
const int Scoring::Params::KING_COVER[6][4] = {{166, 127, 174, 85},
{0, 0, 0, 0},
{-147, -112, -154, -75},
{-105, -80, -110, -54},
{-199, -152, -208, -101},
{-281, -214, -294, -143}};
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
const int Scoring::Params::PAWN_ENDGAME1 = 117;
const int Scoring::Params::PAWN_ENDGAME2 = 164;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 16;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 16;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 31;
const int Scoring::Params::MINOR_ATTACK_BOOST = 41;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 44;
const int Scoring::Params::ROOK_ATTACK_BOOST = 41;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 42;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 40;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 60;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 64;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 52;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 65;
const int Scoring::Params::PIECE_THREAT_MM_MID = 141;
const int Scoring::Params::PIECE_THREAT_MR_MID = 169;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 169;
const int Scoring::Params::PIECE_THREAT_MM_END = 169;
const int Scoring::Params::PIECE_THREAT_MR_END = 169;
const int Scoring::Params::PIECE_THREAT_MQ_END = 169;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 82;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 92;
const int Scoring::Params::PIECE_THREAT_RM_MID = 137;
const int Scoring::Params::PIECE_THREAT_RR_MID = 169;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 169;
const int Scoring::Params::PIECE_THREAT_RM_END = 169;
const int Scoring::Params::PIECE_THREAT_RR_END = 169;
const int Scoring::Params::PIECE_THREAT_RQ_END = 169;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 121;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 150;
const int Scoring::Params::ENDGAME_KING_THREAT = 169;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 411;
const int Scoring::Params::BISHOP_PAIR_END = 458;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -74;
const int Scoring::Params::BAD_BISHOP_MID = -32;
const int Scoring::Params::BAD_BISHOP_END = -61;
const int Scoring::Params::CENTER_PAWN_BLOCK = -246;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -44;
const int Scoring::Params::WEAK_PAWN_END = -25;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -162;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -188;
const int Scoring::Params::SPACE = 32;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 23;
const int Scoring::Params::ROOK_ON_7TH_MID = 237;
const int Scoring::Params::ROOK_ON_7TH_END = 267;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 251;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 346;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 202;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 189;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 37;
const int Scoring::Params::ROOK_BEHIND_PP_END = 76;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 243;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 26;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 73;
const int Scoring::Params::SUPPORTED_PASSER6 = 520;
const int Scoring::Params::SUPPORTED_PASSER7 = 847;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -44;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {13, 19, 29};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 29, 76, 20, 25};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 64, 159, 169, 179};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 166, 128};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 6, 12, 25, 29, 38, 53, 66, 79, 94, 102, 118, 135, 146, 164, 182, 200, 219, 239, 259, 278, 296, 322, 339, 363, 389, 411, 437, 463, 489, 517, 544, 567, 604, 626, 657, 691, 720, 753, 787, 821, 857, 894, 925, 964, 1000, 1034, 1072, 1111, 1153, 1192, 1236, 1361, 1321, 1360, 1399, 1448, 1490, 1537, 1585, 1636, 1723, 1834, 1801, 1819, 1871, 1922, 1968, 2018, 2070, 2111, 2158, 2203, 2249, 2290, 2339, 2384, 2422, 2571, 2534, 2541, 2581, 2622, 2662, 2695, 2733, 2758, 2806, 2839, 2867, 2894, 2944, 2966, 3024, 3024, 3052, 3087, 3107, 3258, 3167, 3195, 3220, 3252, 3273, 3292, 3316, 3339, 3364, 3385, 3404, 3426, 3442, 3464, 3481, 3502, 3513, 3529, 3547, 3559, 3573, 3586, 3599, 3611, 3622, 3635, 3643, 3652, 3661, 3669, 3676, 3683, 3689, 3694, 3699, 3703, 3706, 3709, 3711, 3712};
const int Scoring::Params::TRADE_DOWN[8] = {807, 483, 397, 311, 225, 139, 53, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 50, 70, 320, 793, 1122}, {0, 0, 0, 50, 221, 500, 959, 1529}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 87, 355, 0}, {0, 0, 0, 0, 13, 134, 472, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 50, 87, 346, 650, 890}, {0, 0, 0, 50, 207, 356, 674, 919}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 173, 425, 890}, {0, 0, 0, 50, 70, 266, 432, 825}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-134, -134, -134, -1, -134, 0, -149, -149, -25, -149, -149, -164, 0, -164, -164, -22, -179, -53, 0, -194, 0}, {-89, -69, -44, -133, -155, -3, -136, -163, -194, -205, -151, -130, -67, -152, -180, -53, -53, -53, -96, -96, -156}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-290, -204, -176, -161, -153, -147, -309, -214, -182, -166, -157, -333, -189, -127, -123, -212, -56, -34, -300, -115, -333}, {-231, -202, -172, -179, -177, -237, -235, -206, -191, -199, -235, -251, -87, -77, -193, -323, -152, -96, -333, -212, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-166, -10, -189, -105, -105, -189, -10, -166}, {-201, -119, -204, -186, -186, -204, -119, -201}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-189, -65, -183, -43, -43, -183, -65, -189}, {-4, -303, -166, -319, -319, -166, -303, -4}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{-63, 0, -120, -132, -132, -120, 0, -63}, {-97, -106, -104, -161, -161, -104, -106, -97}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-339, -242, -124, -118, -118, -124, -242, -339, -178, -179, -159, -32, -32, -159, -179, -178, -239, -93, 11, 88, 88, 11, -93, -239, -183, -149, 58, 53, 53, 58, -149, -183, -12, -26, 74, 68, 68, 74, -26, -12, -114, -15, -3, 66, 66, -3, -15, -114, -269, -179, 58, -149, -149, 58, -179, -269, -299, -209, -189, -179, -179, -189, -209, -299}, {-349, -309, -279, -269, -269, -279, -309, -349, -249, -209, -169, -146, -146, -169, -209, -249, -209, -169, -139, 6, 6, -139, -169, -209, -199, -144, 40, 39, 39, 40, -144, -199, -69, -126, 31, 30, 30, 31, -126, -69, -209, -159, 8, 15, 15, 8, -159, -209, -249, -189, -159, -149, -149, -159, -189, -249, -289, -249, -209, -199, -199, -209, -249, -289}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-106, -1, -36, -36, -36, -36, -1, -106, 88, 121, 119, 31, 31, 119, 121, 88, -30, 71, 54, 88, 88, 54, 71, -30, -96, -119, -2, 102, 102, -2, -119, -96, -119, -59, -38, 66, 66, -38, -59, -119, 181, 101, -19, 101, 101, -19, 101, 181, -87, -219, -162, -219, -219, -162, -219, -87, -219, -219, -219, -219, -219, -219, -219, -219}, {-194, -150, -64, -74, -74, -64, -150, -194, -151, -37, -64, -19, -19, -64, -37, -151, -121, -34, 45, 76, 76, 45, -34, -121, -74, 2, 46, 85, 85, 46, 2, -74, -37, 4, -4, 46, 46, -4, 4, -37, 114, -41, -54, 28, 28, -54, -41, 114, 3, 75, -25, -44, -44, -25, 75, 3, 44, 74, 114, -48, -48, 114, 74, 44}};
const int Scoring::Params::ROOK_PST[2][64] = {{-49, 30, 32, 58, 58, 32, 30, -49, -101, 30, -13, -10, -10, -13, 30, -101, -68, 48, -86, -88, -88, -86, 48, -68, 1, 4, -119, -33, -33, -119, 4, 1, 53, 80, 46, 71, 71, 46, 80, 53, 87, 119, 117, 118, 118, 117, 119, 87, 2, -119, 13, 6, 6, 13, -119, 2, 49, -119, -119, -119, -119, -119, -119, 49}, {-37, -20, 37, 6, 6, 37, -20, -37, -100, -67, -1, -51, -51, -1, -67, -100, -59, 16, -22, -50, -50, -22, 16, -59, 30, 50, 46, 15, 15, 46, 50, 30, 70, 79, 115, 81, 81, 115, 79, 70, 118, 119, 119, 105, 105, 119, 119, 118, 3, 23, 62, 4, 4, 62, 23, 3, 119, 119, -42, -119, -119, -42, 119, 119}};
const int Scoring::Params::QUEEN_PST[2][64] = {{-46, -61, -72, -3, -3, -72, -61, -46, -5, -17, 54, 62, 62, 54, -17, -5, -76, -8, 7, -56, -56, 7, -8, -76, -33, 15, 2, 9, 9, 2, 15, -33, 8, 7, 10, 28, 28, 10, 7, 8, 7, 20, 93, 42, 42, 93, 20, 7, -82, -119, 4, 1, 1, 4, -119, -82, 2, 77, 117, 8, 8, 117, 77, 2}, {-119, -119, -119, -119, -119, -119, -119, -119, -119, -119, -119, -83, -83, -119, -119, -119, -109, -5, 44, 31, 31, 44, -5, -109, -11, 119, 69, 119, 119, 69, 119, -11, 47, 119, 119, 119, 119, 119, 119, 47, 14, 119, 119, 119, 119, 119, 119, 14, 119, 119, 119, 119, 119, 119, 119, 119, -119, 3, 22, 88, 88, 22, 3, -119}};
const int Scoring::Params::KING_PST[2][64] = {{-79, 57, -100, -41, -41, -100, 57, -79, 119, 106, -5, -4, -4, -5, 106, 119, -138, -99, -179, -179, -179, -179, -99, -138, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -479, -241, -241, -479, -479, -479, -479, -241, -479, -479, -479, -479, -241, -479}, {-332, -193, -204, -206, -206, -204, -193, -332, -174, -125, -156, -128, -128, -156, -125, -174, -232, -159, -108, -84, -84, -108, -159, -232, -175, -83, 25, 74, 74, 25, -83, -175, -56, 73, 136, 187, 187, 136, 73, -56, 55, 172, 239, 260, 260, 239, 172, 55, 119, 249, 258, 248, 248, 258, 249, 119, -39, 146, 188, 111, 111, 188, 146, -39}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-299, -189, -86, -25, 34, 76, 91, 87, 1};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-289, -171, -85, -29, 14, 37, 48, 63, 72, 69, 69, 105, 55, 28, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-293, -80, -83, -48, -44, 6, 72, 133, 165, 178, 195, 206, 248, 224, 190}, {-333, -202, -114, 69, 93, 104, 87, 146, 196, 214, 246, 268, 280, 294, 301}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-198, -61, -43, -26, 14, 58, 89, 119, 149, 182, 180, 203, 201, 211, 218, 215, 243, 314, 276, 329, 258, 314, 91, 329, 329, 91, 91, 91, 210}, {-239, -179, -129, -109, -69, 2, 79, 98, 151, 160, 202, 235, 250, 333, 333, 333, 333, 333, 333, 333, 323, 333, 333, 333, 333, 141, 333, 141, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-319, -149, -62, 6, 13};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 61, 97, 88, 127, 127, 88, 97, 61, 0, 58, 71, 106, 106, 71, 58, 0, 0, 0, 57, 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 93, 159, 200, 259, 259, 200, 159, 93, 12, 124, 209, 259, 259, 209, 124, 12, 129, 34, 115, 137, 137, 115, 34, 129, 0, 129, 129, 0, 0, 129, 129, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 209, 259, 259, 209, 0, 10, 10, 0, 209, 219, 219, 209, 0, 10, 10, 129, 0, 179, 179, 0, 129, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 135, 150, 150, 135, 0, 0, 124, 52, 9, 25, 25, 9, 52, 124, 37, 25, 0, 0, 0, 0, 25, 37, 82, 106, 37, 0, 0, 37, 106, 82},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 31, 114, 114, 31, 17, 0, 0, 40, 76, 42, 42, 76, 40, 0, 18, 129, 52, 0, 0, 52, 129, 18, 0, 39, 0, 0, 0, 0, 39, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10, 12, 54, 54, 12, 10, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 129, 129, 129, 129, 129, 129, 10}};
