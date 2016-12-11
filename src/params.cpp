// Copyright 2015, 2016 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {-194, 35, 156, 107};
const int Scoring::Params::RBN_ADJUST[4] = {-344, -531, -676, -954};
const int Scoring::Params::QR_ADJUST[4] = {-344, 156, 640, 647};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {-156, -2555, -1344};
const int Scoring::Params::CASTLING[6] = {100, -45, -36, 162, 273, -136};
const int Scoring::Params::KING_COVER[5][4] = {{225, 450, 332, 351},
{151, 303, 224, 236},
{125, 250, 185, 195},
{18, 36, 26, 28},
{-117, -235, -174, -183}};
const int Scoring::Params::KING_COVER_BASE = -350;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 330;
const int Scoring::Params::PIN_MULTIPLIER_END = 414;
const int Scoring::Params::ROOK_VS_PAWNS = 177;
const int Scoring::Params::KRMINOR_VS_R = -256;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -406;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -256;
const int Scoring::Params::MINOR_FOR_PAWNS = 502;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 0;
const int Scoring::Params::PAWN_ENDGAME1 = 177;
const int Scoring::Params::PAWN_ENDGAME2 = 165;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 2;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 5;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 19;
const int Scoring::Params::MINOR_ATTACK_BOOST = 35;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 36;
const int Scoring::Params::ROOK_ATTACK_BOOST = 35;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 40;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 38;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 46;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 73;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 10;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 90;
const int Scoring::Params::PIECE_THREAT_MM_MID = 157;
const int Scoring::Params::PIECE_THREAT_MR_MID = 191;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 206;
const int Scoring::Params::PIECE_THREAT_MM_END = 90;
const int Scoring::Params::PIECE_THREAT_MR_END = 186;
const int Scoring::Params::PIECE_THREAT_MQ_END = 206;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 125;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 108;
const int Scoring::Params::PIECE_THREAT_RM_MID = 153;
const int Scoring::Params::PIECE_THREAT_RR_MID = 177;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 206;
const int Scoring::Params::PIECE_THREAT_RM_END = 163;
const int Scoring::Params::PIECE_THREAT_RR_END = 203;
const int Scoring::Params::PIECE_THREAT_RQ_END = 206;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 116;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 168;
const int Scoring::Params::ENDGAME_KING_THREAT = 167;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 337;
const int Scoring::Params::BISHOP_PAIR_END = 449;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -45;
const int Scoring::Params::BAD_BISHOP_MID = -36;
const int Scoring::Params::BAD_BISHOP_END = -52;
const int Scoring::Params::CENTER_PAWN_BLOCK = -258;
const int Scoring::Params::OUTSIDE_PASSER_MID = 0;
const int Scoring::Params::OUTSIDE_PASSER_END = 57;
const int Scoring::Params::WEAK_PAWN_MID = 0;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -168;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -216;
const int Scoring::Params::SPACE = 62;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 307;
const int Scoring::Params::ROOK_ON_7TH_END = 163;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 379;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 376;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 153;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 72;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 168;
const int Scoring::Params::ROOK_BEHIND_PP_END = 0;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 177;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 0;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 174;
const int Scoring::Params::SUPPORTED_PASSER6 = 276;
const int Scoring::Params::SUPPORTED_PASSER7 = 821;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -48;
const int Scoring::Params::KING_ATTACK_COUNT_BOOST[3] = {2, 35, 67};
const int Scoring::Params::KING_ATTACK_COVER_BOOST[5] = {0, 0, 33, 55, 71};
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 66, 170, 199, 216};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={309, 128, 128};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 34, 0, 3, 4, 0, 20, 30, 106, 108, 120, 148, 129, 167, 178, 189, 209, 227, 265, 259, 304, 333, 350, 397, 414, 425, 481, 545, 552, 579, 601, 622, 660, 694, 756, 777, 829, 836, 892, 949, 996, 987, 1029, 1102, 1140, 1202, 1236, 1286, 1312, 1311, 1336, 1471, 1454, 1490, 1565, 1636, 1670, 1763, 1808, 1871, 1910, 1882, 1959, 1977, 2008, 2030, 2076, 2192, 2186, 2201, 2264, 2332, 2355, 2427, 2448, 2507, 2625, 2658, 2672, 2711, 2751, 2783, 2825, 2837, 2859, 2984, 2953, 3003, 3044, 3096, 3119, 3108, 3126, 3154, 3139, 3200, 3236, 3254, 3289, 3291, 3354, 3406, 3391, 3437, 3422, 3409, 3436, 3521, 3572, 3553, 3520, 3554, 3595, 3612, 3606, 3613, 3669, 3645, 3609, 3633, 3694, 3685, 3638, 3636, 3653, 3644, 3743, 3688, 3725, 3767, 3764, 3787, 3777, 3785, 3816};
const int Scoring::Params::TRADE_DOWN[16] = {532, 809, 770, 715, 693, 332, 586, 220, 488, 145, 120, 107, 16, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 50, 70, 293, 700, 1250}, {0, 0, 0, 50, 175, 496, 938, 1278}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 32, 372, 0}, {0, 0, 0, 0, 0, 143, 519, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 50, 72, 75, 686, 927}, {0, 0, 0, 50, 152, 383, 711, 956}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 75, 450, 916}, {0, 0, 0, 50, 70, 183, 236, 645}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-153, -148, -159, -23, -145, 0, -186, -186, -119, -167, -186, -125, 0, -104, 0, 0, -143, 0, 0, 0, 0}, {-97, -106, -47, -50, -143, -169, -133, -168, -138, -207, -224, -43, 0, 0, -186, -38, -32, -45, -76, -124, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-287, -229, -213, -198, -184, -111, -326, -243, -194, -181, -179, -333, -191, -172, -102, -331, -42, 0, -204, -40, -228}, {-62, -40, 0, 0, 0, -56, -220, -115, -123, -92, -225, -229, -233, -87, -57, -255, -242, -71, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-200, -48, -243, -170, -170, -243, -48, -200}, {-230, -217, -216, -161, -161, -216, -217, -230}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{0, 0, -10, -13, -13, -10, 0, 0}, {-279, -337, -433, -135, -135, -433, -337, -279}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -36, -91, -91, -36, 0, 0}, {-61, -29, -36, -141, -141, -36, -29, -61}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-376, -218, -60, -180, -180, -60, -218, -376, -196, -189, -142, -15, -15, -142, -189, -196, -256, -118, -72, 123, 123, -72, -118, -256, -185, 106, 97, 155, 155, 97, 106, -185, 5, -53, 138, 93, 93, 138, -53, 5, -158, -174, 130, 128, 128, 130, -174, -158, -298, 54, 13, 106, 106, 13, 54, -298, -336, -246, -128, 96, 96, -128, -246, -336}, {-386, -326, -298, -286, -286, -298, -326, -386, -270, -230, -204, -187, -187, -204, -230, -270, -237, -206, -175, -157, -157, -175, -206, -237, -199, -173, 22, 31, 31, 22, -173, -199, -232, -161, -119, -31, -31, -119, -161, -232, -185, -192, -98, -126, -126, -98, -192, -185, -243, -218, -159, -180, -180, -159, -218, -243, -326, -244, -218, -206, -206, -218, -244, -326}};
const int Scoring::Params::BISHOP_PST[2][64] = {{-331, 36, -46, -215, -215, -46, 36, -331, -21, 116, 97, 10, 10, 97, 116, -21, -117, 25, 89, 68, 68, 89, 25, -117, -17, -123, -68, 200, 200, -68, -123, -17, -141, -88, 99, 146, 146, 99, -88, -141, 240, 199, 211, 215, 215, 211, 199, 240, -58, -58, -88, 34, 34, -88, -58, -58, -242, 37, 44, -252, -252, 44, 37, -242}, {-171, -173, -127, -87, -87, -127, -173, -171, -154, -88, -83, -46, -46, -83, -88, -154, -104, -24, -6, -4, -4, -6, -24, -104, -85, -31, 85, 77, 77, 85, -31, -85, -45, -52, 36, 35, 35, 36, -52, -45, 90, -34, 33, -27, -27, 33, -34, 90, -39, 32, 42, -48, -48, 42, 32, -39, 50, 88, 57, -70, -70, 57, 88, 50}};
const int Scoring::Params::ROOK_PST[2][64] = {{-145, -35, -11, 39, 39, -11, -35, -145, -142, -116, -77, 57, 57, -77, -116, -142, -151, 73, -33, 77, 77, -33, 73, -151, -149, 44, -80, 24, 24, -80, 44, -149, 97, 127, 143, 124, 124, 143, 127, 97, 93, 138, 142, 146, 146, 142, 138, 93, -38, -102, 134, 131, 131, 134, -102, -38, 146, 147, 153, 156, 156, 153, 147, 146}, {-127, -129, -133, -141, -141, -133, -129, -127, -123, -129, -100, -97, -97, -100, -129, -123, -40, 78, 50, -56, -56, 50, 78, -40, -67, 16, 105, 18, 18, 105, 16, -67, 42, 96, 116, 133, 133, 116, 96, 42, 130, 123, 129, 138, 138, 129, 123, 130, 12, 31, 30, 27, 27, 30, 31, 12, 134, 137, 131, 114, 114, 131, 137, 134}};
const int Scoring::Params::QUEEN_PST[2][64] = {{62, -77, -49, 70, 70, -49, -77, 62, -120, -71, 55, 68, 68, 55, -71, -120, -113, -43, 70, 9, 9, 70, -43, -113, -37, 76, 76, 50, 50, 76, 76, -37, -17, -43, 136, 84, 84, 136, -43, -17, -106, 118, 81, 140, 140, 81, 118, -106, 110, -146, -15, 139, 139, -15, -146, 110, -47, 133, 137, 149, 149, 137, 133, -47}, {-156, -156, -156, -156, -156, -156, -156, -156, -149, -149, -155, -147, -147, -155, -149, -149, -143, -126, -114, -133, -133, -114, -126, -143, -84, -31, 116, 115, 115, 116, -31, -84, 73, 130, 144, 143, 143, 144, 130, 73, 145, 114, 143, 149, 149, 143, 114, 145, -53, -47, 139, 142, 142, 139, -47, -53, -7, 10, 139, 139, 139, 139, 10, -7}};
const int Scoring::Params::KING_PST[2][64] = {{-135, 55, -148, 48, 48, -148, 55, -135, 130, 130, 58, 135, 135, 58, 130, 130, 77, 96, 9, 47, 47, 9, 96, 77, -211, -204, -224, -500, -500, -224, -204, -211, -500, -204, -206, -500, -500, -206, -204, -500, -204, -204, -212, -500, -500, -212, -204, -204, -204, -205, -204, -500, -500, -204, -205, -204, -204, -204, -204, -500, -500, -204, -204, -204}, {-108, -81, -114, -160, -160, -114, -81, -108, -93, -32, -95, -57, -57, -95, -32, -93, -160, -146, -86, -70, -70, -86, -146, -160, -215, -60, -13, -20, -20, -13, -60, -215, -150, -97, -21, 109, 109, -21, -97, -150, -109, -40, 2, 37, 37, 2, -40, -109, -54, 125, 31, 83, 83, 31, 125, -54, 9, 190, 46, 139, 139, 46, 190, 9}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-320, -203, -124, -53, 27, 137, 152, 119, -13};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-332, -221, -134, -70, 14, 67, 111, 133, 142, 127, 113, 95, -29, -23, -66};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-278, -142, -57, -70, -95, -10, 38, 85, 192, 214, 214, 284, 313, 383, 396}, {-445, -288, -216, -82, 9, 83, 123, 185, 270, 310, 335, 335, 415, 308, 195}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-236, -162, -74, -79, -25, 17, 57, 126, 139, 200, 234, 255, 269, 304, 327, 304, 339, 342, 360, 366, 366, 366, 366, 366, 366, 366, 54, 188, 54}, {-276, -213, -149, -116, -78, -47, -23, -3, 22, 28, 55, 144, 181, 307, 301, 333, 299, 185, 118, 104, 104, 104, 104, 104, 104, 104, 104, 104, 104}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-324, -188, -58, 18, -1};

const int Scoring::Params::KNIGHT_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 52, 61, 155, 155, 61, 52, 96, 0, 0, 0, 73, 73, 0, 0, 0, 0, 0, 0, 32, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 145, 79, 227, 268, 268, 227, 79, 145, 166, 0, 226, 255, 255, 226, 0, 166, 0, 146, 93, 191, 191, 93, 146, 0, 0, 166, 167, 166, 166, 167, 166, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 155, 195, 274, 274, 195, 155, 0, 0, 0, 222, 277, 277, 222, 0, 0, 0, 123, 0, 0, 0, 0, 123, 0, 0, 166, 166, 166, 166, 166, 166, 0}};
const int Scoring::Params::BISHOP_OUTPOST[3][64] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 134, 34, 174, 174, 34, 134, 29, 138, 0, 46, 122, 122, 46, 0, 138, 50, 13, 0, 33, 33, 0, 13, 50, 128, 141, 98, 0, 0, 98, 141, 128},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 186, 97, 162, 162, 97, 186, 0, 0, 60, 14, 113, 113, 14, 60, 0, 0, 109, 159, 0, 0, 159, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0},{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 179, 230, 230, 179, 102, 0, 0, 0, 0, 242, 242, 0, 0, 0, 0, 166, 206, 0, 0, 206, 166, 0, 0, 166, 0, 166, 166, 0, 166, 0}};
