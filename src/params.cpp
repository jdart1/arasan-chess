// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 15-Aug-2017 08:37:57 by "tuner -n 175 -c 24 -R 50 /home/jdart/chess/epd/big2.epd"
//

const int Scoring::Params::RB_ADJUST[6] = {328, -50, 602, 158, 140, 318};
const int Scoring::Params::RBN_ADJUST[6] = {500, -150, 200, -9, -236, -250};
const int Scoring::Params::QR_ADJUST[5] = {408, 917, 425, -178, -750};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2264, -1211};
const int Scoring::Params::CASTLING[6] = {100, -214, -195, 0, 65, -111};
const int Scoring::Params::KING_COVER[6][4] = {{95, 189, 98, 89},
{45, 91, 47, 43},
{-30, -59, -31, -28},
{-6, -13, -7, -6},
{-68, -136, -70, -64},
{-176, -352, -182, -166}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 494;
const int Scoring::Params::PIN_MULTIPLIER_END = 534;
const int Scoring::Params::KRMINOR_VS_R = 0;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -1348;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -1286;
const int Scoring::Params::MINOR_FOR_PAWNS = 228;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 313;
const int Scoring::Params::PAWN_ENDGAME2 = 73;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 9;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 10;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 30;
const int Scoring::Params::MINOR_ATTACK_BOOST = 40;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 47;
const int Scoring::Params::ROOK_ATTACK_BOOST = 19;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 37;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 47;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 47;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 54;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_BASE = 8;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_SLOPE = 20;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 16;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 89;
const int Scoring::Params::PIECE_THREAT_MM_MID = 177;
const int Scoring::Params::PIECE_THREAT_MR_MID = 421;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 361;
const int Scoring::Params::PIECE_THREAT_MM_END = 248;
const int Scoring::Params::PIECE_THREAT_MR_END = 482;
const int Scoring::Params::PIECE_THREAT_MQ_END = 750;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 72;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 186;
const int Scoring::Params::PIECE_THREAT_RM_MID = 151;
const int Scoring::Params::PIECE_THREAT_RR_MID = 128;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 546;
const int Scoring::Params::PIECE_THREAT_RM_END = 128;
const int Scoring::Params::PIECE_THREAT_RR_END = 108;
const int Scoring::Params::PIECE_THREAT_RQ_END = 750;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 110;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 226;
const int Scoring::Params::ENDGAME_KING_THREAT = 282;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 249;
const int Scoring::Params::BISHOP_PAIR_END = 477;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -33;
const int Scoring::Params::BAD_BISHOP_MID = -41;
const int Scoring::Params::BAD_BISHOP_END = -39;
const int Scoring::Params::CENTER_PAWN_BLOCK = -227;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -19;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -175;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -171;
const int Scoring::Params::SPACE = 30;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 189;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 203;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 650;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 169;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 115;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 106;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 128;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 73;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 17;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 564;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 420;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -210;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -456;
const int Scoring::Params::WRONG_COLOR_BISHOP = 0;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -41;
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 19, 175, 401, 648, 750};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={300, 225, 156};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {-15, -13, -11, -9, -6, -3, -1, 3, 6, 9, 13, 17, 21, 25, 30, 35, 40, 46, 52, 58, 65, 72, 80, 88, 97, 106, 116, 126, 137, 148, 160, 173, 187, 202, 217, 233, 251, 269, 288, 308, 330, 352, 376, 401, 428, 455, 485, 515, 548, 581, 617, 654, 693, 733, 776, 820, 866, 914, 964, 1015, 1069, 1125, 1182, 1241, 1302, 1365, 1429, 1495, 1563, 1632, 1703, 1775, 1848, 1923, 1998, 2074, 2151, 2228, 2305, 2383, 2461, 2539, 2617, 2694, 2771, 2847, 2923, 2997, 3070, 3142, 3213, 3282, 3350, 3417, 3481, 3544, 3605, 3665, 3722, 3778, 3832, 3884, 3934, 3982, 4028, 4072, 4115, 4156, 4195, 4232, 4268, 4302, 4334, 4365, 4394, 4422, 4449, 4474, 4498, 4521, 4542, 4563, 4582, 4600, 4618, 4634, 4649, 4664, 4678, 4691, 4703, 4715, 4726, 4736, 4746, 4755, 4764, 4772, 4780, 4787, 4794, 4800, 4806, 4812, 4817, 4822, 4827, 4831, 4835, 4839};
const int Scoring::Params::TRADE_DOWN[8] = {355, 269, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 165, 700, 763}, {0, 0, 0, 0, 117, 517, 1198, 1750}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {79, 82, 79, 76, 76, 79, 82, 79};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 21, 750, 0}, {0, 0, 0, 0, 0, 136, 750, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 30, 687, 1250, 1750}, {0, 0, 0, 0, 381, 692, 1250, 1750}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 160, 693, 1316}, {0, 0, 0, 50, 127, 250, 473, 740}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-333, -298, -331, -189, -329, 0, -186, -245, -41, -325, 0, -286, -49, -264, -86, -280, -112, -333, -72, -333, 0}, {-20, 0, -28, -93, -152, -125, -44, -119, -133, -217, -219, -83, -47, -52, -187, -184, -129, -309, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -329, -333, -333, -333, -136, -20, -58, -173, 0, 0, -333, -156, -333}, {-133, -105, -44, -104, -127, -62, -159, -50, -108, -67, -66, -194, -38, -25, -9, -326, -171, -141, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-365, -6, -177, -162, -162, -177, -6, -365}, {-341, -154, -230, -146, -146, -230, -154, -341}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{0, -404, -107, 0, 0, -107, -404, 0}, {0, -370, -97, -388, -388, -97, -370, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -78, -91, -91, -78, 0, 0}, {-35, -63, -73, -131, -131, -73, -63, -35}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-580, -278, -137, -111, -111, -137, -278, -580, -142, -236, -201, -23, -23, -201, -236, -142, -299, -133, -28, 59, 59, -28, -133, -299, -182, -140, 25, 20, 20, 25, -140, -182, -101, -127, 56, -64, -64, 56, -127, -101, -233, -581, -496, -170, -170, -496, -581, -233, -535, -403, -4, 7, 7, -4, -403, -535, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000}, {-358, -293, -227, -187, -187, -227, -293, -358, -232, -204, -118, -99, -99, -118, -204, -232, -272, -186, -126, 17, 17, -126, -186, -272, -133, -105, 70, 121, 121, 70, -105, -133, -128, -139, 10, -2, -2, 10, -139, -128, -219, -482, -365, -254, -254, -365, -482, -219, -429, -213, -201, -160, -160, -201, -213, -429, -1000, -442, -477, -370, -370, -477, -442, -1000}};
const int Scoring::Params::BISHOP_PST[2][64] = {{1, 247, 82, 85, 85, 82, 247, 1, 128, 184, 189, 108, 108, 189, 184, 128, -11, 138, 135, 127, 127, 135, 138, -11, -89, -109, 27, 89, 89, 27, -109, -89, -271, -141, -216, -90, -90, -216, -141, -271, -63, -575, -839, -514, -514, -839, -575, -63, -11, -396, -282, -815, -815, -282, -396, -11, -125, -557, -1000, -1000, -1000, -1000, -557, -125}, {-183, -45, -8, -49, -49, -8, -45, -183, -56, -4, -28, 18, 18, -28, -4, -56, -67, -14, 48, 75, 75, 48, -14, -67, -83, 12, 72, 130, 130, 72, 12, -83, -237, -88, -60, -2, -2, -60, -88, -237, 41, -291, -419, -292, -292, -419, -291, 41, -141, -261, -315, -396, -396, -315, -261, -141, 49, 28, -27, -0, -0, -27, 28, 49}};
const int Scoring::Params::ROOK_PST[2][64] = {{-616, -527, -488, -461, -461, -488, -527, -616, -668, -490, -502, -473, -473, -502, -490, -668, -609, -473, -549, -564, -564, -549, -473, -609, -607, -539, -603, -516, -516, -603, -539, -607, -510, -451, -438, -427, -427, -438, -451, -510, -529, -254, -315, -293, -293, -315, -254, -529, -434, -569, -247, -268, -268, -247, -569, -434, -355, -504, -1000, -1000, -1000, -1000, -504, -355}, {-55, -31, 19, -21, -21, 19, -31, -55, -74, -48, 17, 1, 1, 17, -48, -74, -84, -28, -17, -9, -9, -17, -28, -84, -14, 58, 57, 38, 38, 57, 58, -14, 74, 81, 96, 93, 93, 96, 81, 74, 65, 158, 158, 132, 132, 158, 158, 65, -8, 33, 99, 80, 80, 99, 33, -8, 190, 229, 226, 165, 165, 226, 229, 190}};
const int Scoring::Params::QUEEN_PST[2][64] = {{76, 83, 78, 167, 167, 78, 83, 76, 135, 143, 168, 203, 203, 168, 143, 135, 62, 139, 161, 88, 88, 161, 139, 62, 78, 137, 91, 117, 117, 91, 137, 78, 104, 74, 111, 124, 124, 111, 74, 104, 30, 46, 117, 126, 126, 117, 46, 30, -29, -157, -37, -117, -117, -37, -157, -29, -140, 360, -78, 137, 137, -78, 360, -140}, {-158, -186, -152, -88, -88, -152, -186, -158, -173, -88, -50, 91, 91, -50, -88, -173, -11, 141, 226, 215, 215, 226, 141, -11, 69, 263, 325, 418, 418, 325, 263, 69, 144, 416, 472, 623, 623, 472, 416, 144, 147, 332, 663, 667, 667, 663, 332, 147, 286, 378, 605, 764, 764, 605, 378, 286, 1, 36, 192, 272, 272, 192, 36, 1}};
const int Scoring::Params::KING_PST[2][64] = {{424, 536, 243, 327, 327, 243, 536, 424, 631, 592, 364, 505, 505, 364, 592, 631, 98, 409, -115, -106, -106, -115, 409, 98, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -1000, -997, -1000, -1000, -1000, -1000, -997, -1000, -533, -1000, 1000, -1000, -1000, 1000, -1000, -533}, {-670, -641, -662, -687, -687, -662, -641, -670, -555, -580, -618, -603, -603, -618, -580, -555, -593, -599, -559, -548, -548, -559, -599, -593, -554, -523, -460, -436, -436, -460, -523, -554, -459, -415, -336, -314, -314, -336, -415, -459, -442, -362, -311, -277, -277, -311, -362, -442, -432, -392, -364, -365, -365, -364, -392, -432, -692, -501, -488, -480, -480, -488, -501, -692}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-462, -223, -97, -31, 15, 70, 93, 85, 48};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-329, -204, -105, -42, 17, 73, 101, 113, 161, 177, 176, 214, 262, 132, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -215, -162, -137, -122, -73, -39, 38, 63, 84, 108, 110, 244, 223, 333}, {-333, -182, -65, 45, 102, 132, 142, 160, 210, 227, 260, 294, 305, 271, 210}};
const int Scoring::Params::QUEEN_MOBILITY[2][24] = {{-191, -54, -74, -33, 6, 44, 87, 112, 137, 176, 171, 200, 192, 201, 175, 169, 206, 251, 333, 333, 333, 333, 333, 333}, {-333, -333, -333, -268, -159, -36, 128, 117, 208, 173, 202, 267, 231, 308, 279, 285, 333, 333, 271, 268, 280, 283, 194, 91}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -112, 27, 92, 114};

const int Scoring::Params::KNIGHT_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{14, 0},{167, 160},{154, 109},{405, 118},{405, 118},{154, 109},{167, 160},{14, 0},{0, 0},{500, 341},{500, 360},{263, 382},{263, 382},{500, 360},{500, 341},{0, 0},{0, 10},{8, 1},{39, 5},{12, 54},{12, 54},{39, 5},{8, 1},{0, 10},{0, 0},{0, 2},{0, 0},{0, 8},{0, 8},{0, 0},{0, 2},{0, 0}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 48},{246, 200},{272, 225},{500, 275},{500, 275},{272, 225},{246, 200},{0, 48},{87, 220},{500, 440},{500, 478},{500, 407},{500, 407},{500, 478},{500, 440},{87, 220},{500, 0},{234, 166},{69, 181},{431, 0},{431, 0},{69, 181},{234, 166},{500, 0},{500, 21},{489, 274},{500, 500},{223, 296},{223, 296},{500, 500},{489, 274},{500, 21}}};
const int Scoring::Params::BISHOP_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{111, 283},{86, 155},{352, 176},{347, 179},{347, 179},{352, 176},{86, 155},{111, 283},{315, 199},{500, 354},{500, 500},{363, 374},{363, 374},{500, 500},{500, 354},{315, 199},{55, 229},{0, 393},{23, 311},{0, 373},{0, 373},{23, 311},{0, 393},{55, 229},{111, 88},{0, 129},{0, 203},{0, 14},{0, 14},{0, 203},{0, 129},{111, 88}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{264, 191},{385, 161},{495, 195},{495, 195},{385, 161},{264, 191},{0, 0},{0, 0},{500, 339},{500, 500},{500, 284},{500, 284},{500, 500},{500, 339},{0, 0},{0, 0},{500, 0},{62, 0},{500, 0},{500, 0},{62, 0},{500, 0},{0, 0},{0, 0},{500, 0},{239, 0},{500, 74},{500, 74},{239, 0},{500, 0},{0, 0}}};


