// Copyright 2015, 2016 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {-222, 0, 117, 98};
const int Scoring::Params::RBN_ADJUST[4] = {-354, -537, -664, -793};
const int Scoring::Params::QR_ADJUST[4] = {-353, 134, 573, 575};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2159, -1337};
const int Scoring::Params::CASTLING[6] = {4, -82, -112, 168, 182, -153};
const int Scoring::Params::KING_COVER[5][4] = {{199, 250, 188, 246},
{171, 214, 161, 211},
{82, 103, 77, 101},
{28, 35, 26, 34},
{-236, -296, -222, -291}};
const int Scoring::Params::KING_COVER_BASE = -350;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 259;
const int Scoring::Params::PIN_MULTIPLIER_END = 319;
const int Scoring::Params::ROOK_VS_PAWNS = 199;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -213;
const int Scoring::Params::MINOR_FOR_PAWNS = 340;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 38;
const int Scoring::Params::PAWN_ENDGAME1 = 92;
const int Scoring::Params::PAWN_ENDGAME2 = 142;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 8;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 8;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 25;
const int Scoring::Params::MINOR_ATTACK_BOOST = 35;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 31;
const int Scoring::Params::ROOK_ATTACK_BOOST = 32;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 34;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 33;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 50;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 50;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 40;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 76;
const int Scoring::Params::PIECE_THREAT_MM_MID = 104;
const int Scoring::Params::PIECE_THREAT_MR_MID = 115;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 119;
const int Scoring::Params::PIECE_THREAT_MM_END = 113;
const int Scoring::Params::PIECE_THREAT_MR_END = 127;
const int Scoring::Params::PIECE_THREAT_MQ_END = 131;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 82;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 94;
const int Scoring::Params::PIECE_THREAT_RM_MID = 104;
const int Scoring::Params::PIECE_THREAT_RR_MID = 113;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 123;
const int Scoring::Params::PIECE_THREAT_RM_END = 112;
const int Scoring::Params::PIECE_THREAT_RR_END = 125;
const int Scoring::Params::PIECE_THREAT_RQ_END = 130;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 106;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 99;
const int Scoring::Params::ENDGAME_KING_THREAT = 118;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 355;
const int Scoring::Params::BISHOP_PAIR_END = 442;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -66;
const int Scoring::Params::BAD_BISHOP_MID = -25;
const int Scoring::Params::BAD_BISHOP_END = -49;
const int Scoring::Params::CENTER_PAWN_BLOCK = -137;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -56;
const int Scoring::Params::WEAK_PAWN_END = -42;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -118;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -118;
const int Scoring::Params::SPACE = 17;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 27;
const int Scoring::Params::ROOK_ON_7TH_MID = 95;
const int Scoring::Params::ROOK_ON_7TH_END = 270;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 216;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 314;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 193;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 121;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 53;
const int Scoring::Params::ROOK_BEHIND_PP_END = 19;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 291;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 20;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 64;
const int Scoring::Params::SUPPORTED_PASSER6 = 539;
const int Scoring::Params::SUPPORTED_PASSER7 = 941;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -35;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {7, 11, 15};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {0, 0, 0, 16, 21};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 49, 74, 85, 94};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={150, 140, 128};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 30, 36, 55, 54, 47, 87, 120, 115, 115, 147, 185, 191, 266, 209, 279, 212, 318, 364, 371, 395, 428, 456, 480, 499, 536, 572, 592, 611, 656, 693, 713, 741, 772, 821, 848, 878, 913, 971, 1008, 1030, 1079, 1135, 1175, 1210, 1241, 1320, 1367, 1402, 1354, 1506, 1551, 1615, 1595, 1727, 1762, 1829, 1829, 1923, 1987, 2047, 2010, 2134, 2116, 2250, 2283, 2344, 2405, 2470, 2470, 2533, 2566, 2644, 2638, 2704, 2573, 2571, 2880, 2860, 2891, 2994, 3071, 3007, 3094, 2838, 3217, 3238, 3195, 3249, 3075, 2757, 2901, 2744, 3003, 3316, 3288, 2904, 3133, 3417, 3737, 3392, 3278, 3493, 3687, 3559, 3423, 3442, 3750, 3599, 3496, 3513, 3529, 3812, 3692, 3573, 3586, 3599, 3611, 3622, 3633, 3643, 3652, 3661, 3669, 3676, 3683, 3689, 3694, 3699, 3703, 3706, 3709, 3711, 3712};
const int Scoring::Params::TRADE_DOWN[8] = {761, 607, 357, 266, 180, 110, 38, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 50, 70, 239, 718, 856}, {0, 0, 0, 50, 181, 437, 867, 1540}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 52, 307, 0}, {0, 0, 0, 0, 35, 132, 426, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 50, 95, 247, 626, 964}, {0, 0, 0, 50, 199, 259, 651, 980}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 75, 382, 938}, {0, 0, 0, 50, 70, 179, 375, 545}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-27, -27, -26, 0, -27, -4, -42, -43, -37, -43, 0, -59, 0, -57, -1, 0, -74, -73, 0, -89, 0}, {-53, -16, 0, -55, -55, -12, -72, -97, -98, -98, -90, -62, -41, -106, -140, -61, -61, -61, -104, -104, -148}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-183, -97, -70, -55, -46, -41, -203, -108, -76, -59, -51, -229, -120, -83, -66, -172, -16, -13, -308, -128, -333}, {-158, -157, -148, -157, -142, -159, -170, -162, -138, -157, -170, -190, -62, -70, -130, -217, -176, -94, -290, -235, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-54, 0, -114, -93, -93, -114, 0, -54}, {-89, -114, -194, -185, -185, -194, -114, -89}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-104, -57, -179, -41, -41, -179, -57, -104}, {0, -217, -276, -313, -313, -276, -217, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{-12, 0, -75, -100, -100, -75, 0, -12}, {-79, -75, -84, -114, -114, -84, -75, -79}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-328, -213, -74, -37, -37, -74, -213, -328, -153, -135, -111, 19, 19, -111, -135, -153, -198, -56, 17, 56, 56, 17, -56, -198, -149, -79, 54, 55, 55, 54, -79, -149, 3, -9, 86, 101, 101, 86, -9, 3, -72, -20, -121, 69, 69, -121, -20, -72, -228, -158, 42, -157, -157, 42, -158, -228, -320, -226, -212, -196, -196, -212, -226, -320}, {-344, -277, -260, -245, -245, -260, -277, -344, -227, -195, -124, -119, -119, -124, -195, -227, -197, -152, -119, -35, -35, -119, -152, -197, -179, -139, 0, -2, -2, 0, -139, -179, -134, -106, 0, -32, -32, 0, -106, -134, -190, -146, -75, -65, -65, -75, -146, -190, -240, -172, -139, -121, -121, -139, -172, -240, -313, -250, -218, -202, -202, -218, -250, -313}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-42, 30, -19, 0, 0, -19, 30, -42, 85, 153, 95, 50, 50, 95, 153, 85, -25, 67, 105, 111, 111, 105, 67, -25, -64, -80, 37, 105, 105, 37, -80, -64, -80, -14, -8, 83, 83, -8, -14, -80, 191, 5, -55, 0, 0, -55, 5, 191, -32, -203, -174, -235, -235, -174, -203, -32, -170, -226, -237, -238, -238, -237, -226, -170}, {-176, -127, -92, -59, -59, -92, -127, -176, -125, -58, -64, -19, -19, -64, -58, -125, -99, -58, 7, 42, 42, 7, -58, -99, -64, -15, 0, 50, 50, 0, -15, -64, -49, -25, 0, 24, 24, 0, -25, -49, 85, -76, -60, 0, 0, -60, -76, 85, -15, 38, -65, -50, -50, -65, 38, -15, 16, 40, 76, -64, -64, 76, 40, 16}};
const int Scoring::Params::ROOK_PST[2][64] = {{-52, 19, 39, 52, 52, 39, 19, -52, -81, 55, 25, 0, 0, 25, 55, -81, -41, 68, 2, -61, -61, 2, 68, -41, -68, -42, -54, -51, -51, -54, -42, -68, 49, 49, 19, 14, 14, 19, 49, 49, 72, 103, 87, 69, 69, 87, 103, 72, 121, -26, 81, 73, 73, 81, -26, 121, -12, -143, -151, -150, -150, -151, -143, -12}, {-41, 0, 34, 25, 25, 34, 0, -41, -62, -36, 23, -3, -3, 23, -36, -62, -33, 13, -14, -5, -5, -14, 13, -33, 18, 45, 55, 21, 21, 55, 45, 18, 44, 47, 78, 67, 67, 78, 47, 44, 73, 89, 87, 77, 77, 87, 89, 73, -15, 14, 59, 0, 0, 59, 14, -15, 84, 83, -85, -114, -114, -85, 83, 84}};
const int Scoring::Params::QUEEN_PST[2][64] = {{-36, -46, -40, 50, 50, -40, -46, -36, 32, 39, 59, 70, 70, 59, 39, 32, -40, 20, 58, 6, 6, 58, 20, -40, 4, 64, -6, 28, 28, -6, 64, 4, 81, 34, 35, 60, 60, 35, 34, 81, 79, 92, 90, 43, 43, 90, 92, 79, -76, -70, -12, -82, -82, -12, -70, -76, -67, 103, 106, -77, -77, 106, 103, -67}, {-97, -107, -98, -104, -104, -98, -107, -97, -100, -100, -90, -48, -48, -90, -100, -100, -76, 0, 34, 9, 9, 34, 0, -76, 6, 70, 68, 87, 87, 68, 70, 6, 53, 82, 90, 108, 108, 90, 82, 53, -12, 78, 106, 105, 105, 106, 78, -12, 55, 108, 107, 116, 116, 107, 108, 55, -122, -101, -61, 59, 59, -61, -101, -122}};
const int Scoring::Params::KING_PST[2][64] = {{50, 110, -49, 36, 36, -49, 110, 50, 107, 78, -55, 34, 34, -55, 78, 107, -100, 9, -164, -174, -174, -164, 9, -100, -498, -490, -493, -500, -500, -493, -490, -498, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -493, -500, -500, -500, -500, -493, -500, -498, -125, -485, -500, -500, -485, -125, -498}, {-350, -251, -226, -208, -208, -226, -251, -350, -223, -156, -161, -119, -119, -161, -156, -223, -233, -169, -112, -61, -61, -112, -169, -233, -190, -111, 19, 66, 66, 19, -111, -190, -110, 40, 118, 178, 178, 118, 40, -110, -43, 132, 198, 241, 241, 198, 132, -43, 77, 206, 233, 198, 198, 233, 206, 77, -75, 43, 66, 20, 20, 66, 43, -75}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-231, -115, -55, -26, 19, 69, 78, 72, 0};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-250, -147, -83, -14, 24, 66, 88, 89, 116, 110, 108, 152, 144, 43, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-256, -133, -67, -46, -28, 13, 55, 127, 152, 169, 194, 198, 237, 202, 292}, {-333, -198, -60, 2, 36, 69, 69, 109, 162, 176, 219, 250, 287, 315, 313}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-146, -87, -45, -27, 0, 55, 107, 126, 154, 175, 184, 205, 213, 218, 222, 192, 215, 251, 185, 250, 113, 258, 61, 264, 275, 48, 57, 40, 210}, {-175, -114, -63, -42, -15, 0, 79, 69, 142, 172, 213, 206, 231, 285, 292, 300, 306, 307, 305, 301, 245, 292, 296, 301, 288, 105, 318, 101, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-257, -170, -98, -38, -39};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 33, 20, 79, 79, 20, 33, 16, 0, 29, 61, 77, 77, 61, 29, 0, 0, 0, 38, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 53, 101, 152, 152, 101, 53, 21, 0, 51, 103, 152, 152, 103, 51, 0, 24, 0, 71, 72, 72, 71, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 102, 152, 152, 102, 0, 10, 10, 0, 103, 106, 106, 103, 0, 10, 10, 23, 0, 73, 73, 0, 23, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 48, 72, 72, 48, 0, 3, 17, 32, 5, 37, 37, 5, 32, 17, 0, 0, 0, 0, 0, 0, 0, 0, 15, 14, 0, 0, 0, 0, 14, 15},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 45, 133, 133, 45, 14, 0, 0, 35, 86, 78, 78, 86, 35, 0, 0, 22, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 2, 32, 60, 60, 32, 2, 10, 10, 0, 23, 0, 0, 23, 0, 10, 10, 0, 0, 0, 0, 0, 0, 10, 10, 23, 10, 21, 21, 10, 23, 10}};
