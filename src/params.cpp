// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 16-Jan-2017 02:48:54 by "tuner -o adam -n 120 -c 24 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {88, 131, 340, 56};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 735, 462};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2205, -1299};
const int Scoring::Params::CASTLING[6] = {100, -141, -120, 32, 136, -103};
const int Scoring::Params::KING_COVER[6][4] = {{125, 136, 84, 84},
{97, 105, 65, 65},
{-237, -257, -158, -158},
{-35, -38, -24, -24},
{-110, -119, -74, -74},
{-214, -232, -143, -143}};
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 491;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -217;
const int Scoring::Params::MINOR_FOR_PAWNS = 186;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 240;
const int Scoring::Params::PAWN_ENDGAME1 = 169;
const int Scoring::Params::PAWN_ENDGAME2 = 270;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 8;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 8;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 23;
const int Scoring::Params::MINOR_ATTACK_BOOST = 35;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 31;
const int Scoring::Params::ROOK_ATTACK_BOOST = 25;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 35;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 30;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 50;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 50;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 32;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 88;
const int Scoring::Params::PIECE_THREAT_MM_MID = 162;
const int Scoring::Params::PIECE_THREAT_MR_MID = 270;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 243;
const int Scoring::Params::PIECE_THREAT_MM_END = 260;
const int Scoring::Params::PIECE_THREAT_MR_END = 530;
const int Scoring::Params::PIECE_THREAT_MQ_END = 749;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 78;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 150;
const int Scoring::Params::PIECE_THREAT_RM_MID = 137;
const int Scoring::Params::PIECE_THREAT_RR_MID = 116;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 458;
const int Scoring::Params::PIECE_THREAT_RM_END = 137;
const int Scoring::Params::PIECE_THREAT_RR_END = 315;
const int Scoring::Params::PIECE_THREAT_RQ_END = 716;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 137;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 152;
const int Scoring::Params::ENDGAME_KING_THREAT = 377;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 277;
const int Scoring::Params::BISHOP_PAIR_END = 445;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -67;
const int Scoring::Params::BAD_BISHOP_MID = -31;
const int Scoring::Params::BAD_BISHOP_END = -50;
const int Scoring::Params::CENTER_PAWN_BLOCK = -245;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -35;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -168;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -185;
const int Scoring::Params::SPACE = 24;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 206;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 168;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 506;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 204;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 129;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 182;
const int Scoring::Params::ROOK_BEHIND_PP_END = 0;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 273;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 95;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 0;
const int Scoring::Params::SUPPORTED_PASSER6 = 1000;
const int Scoring::Params::SUPPORTED_PASSER7 = 1500;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -30;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {26, 32, 89};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {5, 25, 23, 20, 25};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 202, 412, 500, 500};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={150, 140, 128};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 91, 0, 0, 42, 6, 63, 60, 24, 143, 14, 50, 114, 179, 141, 240, 188, 96, 190, 156, 225, 418, 380, 325, 450, 465, 316, 433, 570, 611, 656, 619, 633, 741, 648, 604, 835, 903, 829, 848, 903, 1040, 1084, 1091, 1176, 1208, 1263, 1223, 1361, 1420, 1472, 1481, 1538, 1632, 1608, 1547, 1634, 1827, 1914, 1755, 2012, 2097, 2068, 2107, 2275, 2225, 2203, 2417, 2488, 2445, 2525, 2411, 2671, 2747, 2925, 2605, 2401, 2814, 2577, 3151, 2763, 3043, 3059, 2959, 3100, 2895, 3261, 3298, 3442, 3533, 3249, 3570, 3540, 3803, 3578, 3912, 4030, 4062, 3801, 3595, 3176, 3558, 3569, 3151, 2990, 3045, 2343, 2794, 3232, 2949, 3699, 3414, 2674, 3024, 2894, 3312, 4184, 3636, 4061, 4358, 4745, 4345, 4626, 4294, 4093, 4081, 3728, 3728, 3652, 3661, 3669, 3676, 3683, 3689, 3694, 3699, 3703, 3706, 3709, 3711, 3712};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 269, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 50, 70, 203, 710, 891}, {0, 0, 0, 50, 151, 500, 1000, 1750}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 32, 618, 0}, {0, 0, 0, 0, 0, 96, 433, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 50, 70, 491, 926, 1500}, {0, 0, 0, 50, 305, 500, 1000, 1500}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 95, 635, 1146}, {0, 0, 0, 50, 70, 268, 453, 560}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-247, -263, -192, -64, -231, 0, -221, -281, -159, -290, -213, -273, -30, -291, -303, -97, -225, -210, 0, -333, 0}, {-34, 0, -25, -161, -120, 0, -106, -154, -207, -205, -96, -87, -77, -68, -89, -29, 0, 0, -48, -7, -283}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -322, -283, -310, -333, -333, -333, -293, -307, -333, -182, -111, -117, -160, 0, -4, -301, -135, -333}, {-166, -185, -158, -188, -176, -230, -230, -232, -186, -202, -213, -188, -82, -53, -98, -257, -162, -60, -333, -227, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -32, -210, -192, -192, -210, -32, -250}, {-250, -177, -250, -225, -225, -250, -177, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{-322, -212, -72, -17, -17, -72, -212, -322}, {0, -438, -285, -443, -443, -285, -438, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -105, -86, -86, -105, 0, 0}, {-36, -54, -70, -105, -105, -70, -54, -36}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-487, -242, -83, -57, -57, -83, -242, -487, -133, -200, -157, 31, 31, -157, -200, -133, -234, -64, 22, 98, 98, 22, -64, -234, -139, -112, 64, 70, 70, 64, -112, -139, 31, -47, 73, 38, 38, 73, -47, 31, -99, -354, -421, -76, -76, -421, -354, -99, -237, -233, 41, -471, -471, 41, -233, -237, -500, -500, -500, -500, -500, -500, -500, -500}, {-500, -314, -321, -276, -276, -321, -314, -500, -246, -283, -172, -138, -138, -172, -283, -246, -287, -204, -153, -22, -22, -153, -204, -287, -183, -139, 12, 20, 20, 12, -139, -183, -125, -144, -69, -119, -119, -69, -144, -125, -211, -462, -314, -197, -197, -314, -462, -211, -344, -183, -153, -83, -83, -153, -183, -344, -500, -329, -463, -281, -281, -463, -329, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-22, 252, 62, 71, 71, 62, 252, -22, 147, 178, 219, 130, 130, 219, 178, 147, 31, 153, 144, 159, 159, 144, 153, 31, -19, -93, 57, 132, 132, 57, -93, -19, -186, -43, -57, -17, -17, -57, -43, -186, 189, -308, -500, -310, -310, -500, -308, 189, 39, -323, -171, -500, -500, -171, -323, 39, -106, -500, -500, -500, -500, -500, -500, -106}, {-242, -181, -106, -128, -128, -106, -181, -242, -150, -70, -96, -53, -53, -96, -70, -150, -156, -76, -20, 26, 26, -20, -76, -156, -135, -65, -20, 12, 12, -20, -65, -135, -204, -121, -185, -176, -176, -185, -121, -204, 131, -358, -469, -344, -344, -469, -358, 131, 42, 21, -82, -171, -171, -82, 21, 42, 84, 167, 153, -81, -81, 153, 167, 84}};
const int Scoring::Params::ROOK_PST[2][64] = {{-136, -37, 3, 34, 34, 3, -37, -136, -174, -30, -62, -17, -17, -62, -30, -174, -116, -7, -113, -98, -98, -113, -7, -116, -147, -99, -169, -65, -65, -169, -99, -147, -21, 0, -56, 22, 22, -56, 0, -21, 35, 195, 49, 77, 77, 49, 195, 35, 102, -61, 102, 74, 74, 102, -61, 102, -71, -422, -500, -500, -500, -500, -422, -71}, {-70, -50, 5, -26, -26, 5, -50, -70, -164, -121, -70, -92, -92, -70, -121, -164, -136, -74, -80, -82, -82, -80, -74, -136, -59, -24, -11, -37, -37, -11, -24, -59, 10, 13, 76, 47, 47, 76, 13, 10, 71, 115, 121, 81, 81, 121, 115, 71, -17, 0, 74, 18, 18, 74, 0, -17, 85, 109, -126, -290, -290, -126, 109, 85}};
const int Scoring::Params::QUEEN_PST[2][64] = {{59, 61, 57, 170, 170, 57, 61, 59, 151, 126, 182, 209, 209, 182, 126, 151, 81, 136, 150, 99, 99, 150, 136, 81, 96, 132, 85, 125, 125, 85, 132, 96, 146, 108, 100, 125, 125, 100, 108, 146, 155, 156, 194, 101, 101, 194, 156, 155, 24, -55, 0, -73, -73, 0, -55, 24, 89, 245, 233, 98, 98, 233, 245, 89}, {-263, -346, -313, -261, -261, -313, -346, -263, -236, -285, -261, -148, -148, -261, -285, -236, -205, -82, -33, -24, -24, -33, -82, -205, -49, 44, 22, 99, 99, 22, 44, -49, 7, 146, 129, 287, 287, 129, 146, 7, -15, 127, 260, 297, 297, 260, 127, -15, 83, 180, 245, 346, 346, 245, 180, 83, -313, -164, -72, 31, 31, -72, -164, -313}};
const int Scoring::Params::KING_PST[2][64] = {{215, 365, 92, 18, 18, 92, 365, 215, 389, 330, 127, 86, 86, 127, 330, 389, 53, 95, -209, -305, -305, -209, 95, 53, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 500, -360, -500, -500, -360, 500, -500}, {-452, -381, -400, -438, -438, -400, -381, -452, -315, -297, -324, -303, -303, -324, -297, -315, -345, -302, -239, -197, -197, -239, -302, -345, -299, -214, -75, -19, -19, -75, -214, -299, -167, -51, 74, 151, 151, 74, -51, -167, -86, 43, 199, 232, 232, 199, 43, -86, -24, 153, 212, 182, 182, 212, 153, -24, -449, 6, 74, -65, -65, 74, 6, -449}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-449, -241, -119, -60, -10, 41, 62, 67, 16};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-333, -209, -114, -49, 6, 49, 80, 108, 140, 161, 183, 228, 191, 158, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -225, -165, -142, -127, -90, -54, 17, 47, 59, 71, 75, 159, 158, 158}, {-333, -202, -94, -10, 16, 27, 29, 59, 103, 119, 160, 191, 222, 248, 228}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-213, -18, -8, 17, 70, 122, 163, 188, 223, 260, 264, 289, 278, 284, 280, 259, 286, 333, 277, 333, 293, 333, 107, 333, 333, -333, -333, -333, 210}, {-333, -333, -333, -205, -219, -48, 44, 65, 122, 105, 135, 152, 172, 226, 222, 232, 268, 279, 277, 206, 136, 141, 130, 258, 181, -278, 333, -333, 260}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -234, -132, -65, -33};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 60, 143, 129, 228, 228, 129, 143, 60, 0, 333, 333, 201, 201, 333, 333, 0, 0, 0, 9, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 94, 188, 238, 333, 333, 238, 188, 94, 10, 333, 333, 333, 333, 333, 333, 10, 246, 25, 104, 266, 266, 104, 25, 246, 0, 333, 333, 0, 0, 333, 333, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 259, 333, 333, 259, 0, 10, 10, 213, 333, 333, 333, 333, 213, 10, 10, 184, 0, 333, 333, 0, 184, 10, 10, 10, 10, 10, 10, 10, 10, 10}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 199, 85, 215, 275, 275, 215, 85, 199, 141, 333, 333, 330, 330, 333, 333, 141, 0, 0, 0, 0, 0, 0, 0, 0, 65, 30, 0, 0, 0, 0, 30, 65},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 164, 196, 320, 320, 196, 164, 0, 0, 333, 333, 333, 333, 333, 333, 0, 0, 256, 111, 187, 187, 111, 256, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 210, 201, 224, 224, 201, 210, 10, 10, 219, 282, 193, 193, 282, 219, 10, 10, 58, 37, 0, 0, 37, 58, 10, 10, 293, 305, 0, 0, 305, 293, 10}};
