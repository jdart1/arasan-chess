// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 07-Aug-2017 05:33:37 by "tuner -n 175 -c 24 -R 50 /home/jdart/chess/epd/big2.epd"
//

const int Scoring::Params::RB_ADJUST[6] = {150, -50, 494, 101, 84, 176};
const int Scoring::Params::RBN_ADJUST[6] = {350, -300, 50, -100, -250, -400};
const int Scoring::Params::QR_ADJUST[5] = {350, 1000, 814, 354, -200};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2154, -1207};
const int Scoring::Params::CASTLING[6] = {100, -223, -202, 0, 71, -104};
const int Scoring::Params::KING_COVER[6][4] = {{79, 158, 70, 71},
{51, 102, 45, 46},
{-37, -73, -32, -33},
{-10, -20, -9, -9},
{-74, -149, -66, -67},
{-214, -428, -188, -194}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 457;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::KRMINOR_VS_R = 0;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -997;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -925;
const int Scoring::Params::MINOR_FOR_PAWNS = 132;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 335;
const int Scoring::Params::PAWN_ENDGAME2 = 192;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 9;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 11;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 30;
const int Scoring::Params::MINOR_ATTACK_BOOST = 40;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 49;
const int Scoring::Params::ROOK_ATTACK_BOOST = 17;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 31;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 47;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 44;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 56;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_BASE = 8;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_SLOPE = 18;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 13;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 81;
const int Scoring::Params::PIECE_THREAT_MM_MID = 176;
const int Scoring::Params::PIECE_THREAT_MR_MID = 366;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 274;
const int Scoring::Params::PIECE_THREAT_MM_END = 242;
const int Scoring::Params::PIECE_THREAT_MR_END = 440;
const int Scoring::Params::PIECE_THREAT_MQ_END = 750;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 73;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 194;
const int Scoring::Params::PIECE_THREAT_RM_MID = 153;
const int Scoring::Params::PIECE_THREAT_RR_MID = 117;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 519;
const int Scoring::Params::PIECE_THREAT_RM_END = 128;
const int Scoring::Params::PIECE_THREAT_RR_END = 116;
const int Scoring::Params::PIECE_THREAT_RQ_END = 641;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 116;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 223;
const int Scoring::Params::ENDGAME_KING_THREAT = 307;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 244;
const int Scoring::Params::BISHOP_PAIR_END = 474;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -44;
const int Scoring::Params::BAD_BISHOP_MID = -39;
const int Scoring::Params::BAD_BISHOP_END = -45;
const int Scoring::Params::CENTER_PAWN_BLOCK = -227;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -14;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -168;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -170;
const int Scoring::Params::SPACE = 35;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 162;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 304;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 603;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 166;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 119;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 88;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 169;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 97;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 18;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 633;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 410;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -221;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -342;
const int Scoring::Params::WRONG_COLOR_BISHOP = 0;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -35;
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 229, 486, 744, 750};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 179, 134};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {-18, -16, -14, -11, -9, -6, -4, -1, 2, 5, 9, 13, 17, 21, 25, 30, 35, 40, 46, 52, 59, 65, 73, 80, 89, 97, 107, 116, 127, 138, 150, 162, 175, 189, 204, 220, 236, 254, 272, 292, 313, 334, 357, 382, 407, 434, 463, 493, 524, 557, 591, 628, 666, 705, 747, 790, 835, 882, 931, 982, 1034, 1089, 1145, 1203, 1264, 1325, 1389, 1454, 1521, 1590, 1660, 1731, 1803, 1877, 1951, 2027, 2103, 2179, 2257, 2334, 2411, 2489, 2566, 2643, 2719, 2794, 2869, 2943, 3016, 3087, 3157, 3226, 3294, 3359, 3423, 3486, 3546, 3605, 3662, 3717, 3770, 3821, 3871, 3918, 3964, 4008, 4049, 4090, 4128, 4165, 4200, 4233, 4265, 4295, 4324, 4351, 4377, 4402, 4425, 4447, 4468, 4488, 4507, 4525, 4542, 4557, 4572, 4587, 4600, 4613, 4625, 4636, 4646, 4656, 4666, 4675, 4683, 4691, 4698, 4705, 4712, 4718, 4724, 4729, 4734, 4739, 4744, 4748, 4752, 4756};
const int Scoring::Params::TRADE_DOWN[8] = {355, 269, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 191, 725, 791}, {0, 0, 0, 0, 157, 559, 1184, 1689}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {76, 82, 77, 72, 72, 77, 82, 76};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 24, 750, 0}, {0, 0, 0, 0, 0, 132, 750, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 36, 651, 1250, 1750}, {0, 0, 0, 0, 317, 663, 1250, 1750}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 214, 705, 1405}, {0, 0, 0, 50, 70, 225, 406, 722}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-333, -279, -283, -157, -333, 0, -175, -326, -199, -333, 0, -270, -31, -309, 0, -205, -33, -333, 0, -226, 0}, {0, 0, -31, -128, -121, -83, -35, -130, -136, -182, -219, -87, -61, -30, -256, -148, -113, -211, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -103, -6, -68, -170, 0, -6, -333, -124, -333}, {-116, -119, -95, -97, -138, -119, -159, -85, -118, -82, -80, -165, -54, -21, -16, -333, -182, -146, -333, -326, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -5, -195, -166, -166, -195, -5, -250}, {-250, -166, -226, -161, -161, -226, -166, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{0, -265, -105, 0, 0, -105, -265, 0}, {0, -500, -296, -451, -451, -296, -500, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -79, -105, -105, -79, 0, 0}, {-49, -76, -76, -128, -128, -76, -76, -49}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -229, -74, -47, -47, -74, -229, -500, -80, -171, -154, 34, 34, -154, -171, -80, -238, -62, 26, 113, 113, 26, -62, -238, -127, -93, 78, 71, 71, 78, -93, -127, 44, -75, 116, 4, 4, 116, -75, 44, -174, -489, -448, -102, -102, -448, -489, -174, -427, -327, 18, -145, -145, 18, -327, -427, -500, -500, -500, -500, -500, -500, -500, -500}, {-343, -144, -138, -91, -91, -138, -144, -343, -101, -133, -47, -3, -3, -47, -133, -101, -161, -81, -44, 120, 120, -44, -81, -161, -43, 6, 175, 210, 210, 175, 6, -43, 19, -46, 71, 93, 93, 71, -46, 19, -138, -435, -283, -106, -106, -283, -435, -138, -291, -117, -84, -36, -36, -84, -117, -291, -500, -272, -449, -276, -276, -449, -272, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{58, 296, 117, 129, 129, 117, 296, 58, 189, 229, 244, 146, 146, 244, 229, 189, 51, 192, 170, 173, 173, 170, 192, 51, -37, -92, 74, 134, 134, 74, -92, -37, -251, -98, -158, -24, -24, -158, -98, -251, -31, -500, -500, -448, -448, -500, -500, -31, 37, -366, -273, -500, -500, -273, -366, 37, -127, -469, -500, -500, -500, -500, -469, -127}, {-96, -7, 60, 25, 25, 60, -7, -96, 35, 61, 52, 85, 85, 52, 61, 35, -21, 49, 121, 152, 152, 121, 49, -21, -19, 73, 143, 183, 183, 143, 73, -19, -145, -2, 20, 70, 70, 20, -2, -145, 158, -172, -364, -264, -264, -364, -172, 158, 32, -1, -206, -230, -230, -206, -1, 32, 129, 175, 153, -107, -107, 153, 175, 129}};
const int Scoring::Params::ROOK_PST[2][64] = {{-406, -313, -274, -251, -251, -274, -313, -406, -439, -263, -282, -265, -265, -282, -263, -439, -382, -249, -341, -357, -357, -341, -249, -382, -375, -350, -398, -323, -323, -398, -350, -375, -298, -249, -226, -207, -207, -226, -249, -298, -252, -37, -100, -96, -96, -100, -37, -252, -220, -400, -61, -90, -90, -61, -400, -220, -109, -317, -500, -500, -500, -500, -317, -109}, {-24, 4, 53, 11, 11, 53, 4, -24, -46, 2, 49, 32, 32, 49, 2, -46, -31, 25, 21, 25, 25, 21, 25, -31, 20, 105, 88, 86, 86, 88, 105, 20, 144, 145, 151, 149, 149, 151, 145, 144, 160, 224, 225, 195, 195, 225, 224, 160, 77, 107, 197, 165, 165, 197, 107, 77, 252, 295, 262, 168, 168, 262, 295, 252}};
const int Scoring::Params::QUEEN_PST[2][64] = {{44, 64, 75, 162, 162, 75, 64, 44, 146, 139, 167, 197, 197, 167, 139, 146, 74, 133, 156, 77, 77, 156, 133, 74, 84, 130, 84, 102, 102, 84, 130, 84, 113, 68, 82, 134, 134, 82, 68, 113, 36, 103, 118, 103, 103, 118, 103, 36, -56, -161, -38, -99, -99, -38, -161, -56, -60, 356, 182, 214, 214, 182, 356, -60}, {-167, -299, -318, -235, -235, -318, -299, -167, -209, -217, -188, -46, -46, -188, -217, -209, -210, 24, 94, 102, 102, 94, 24, -210, -11, 138, 170, 281, 281, 170, 138, -11, 125, 318, 344, 473, 473, 344, 318, 125, 73, 231, 482, 500, 500, 482, 231, 73, 179, 327, 462, 500, 500, 462, 327, 179, 20, -8, 54, 136, 136, 54, -8, 20}};
const int Scoring::Params::KING_PST[2][64] = {{244, 341, 60, 148, 148, 60, 341, 244, 432, 377, 155, 305, 305, 155, 377, 432, -188, 150, -314, -272, -272, -314, 150, -188, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -137, 106, -500, -500, 106, -137, -500, -500, -500, -500, -500, -500, -500, -500, -500}, {-407, -388, -415, -450, -450, -415, -388, -407, -289, -309, -369, -348, -348, -369, -309, -289, -330, -328, -294, -281, -281, -294, -328, -330, -279, -236, -157, -133, -133, -157, -236, -279, -187, -71, 9, 46, 46, 9, -71, -187, -102, 57, 114, 145, 145, 114, 57, -102, -120, 16, 64, 63, 63, 64, 16, -120, -448, -121, -74, -63, -63, -74, -121, -448}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-467, -235, -110, -46, 6, 64, 90, 86, 50};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-311, -191, -94, -28, 28, 79, 109, 126, 171, 182, 193, 235, 191, 195, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -228, -179, -157, -143, -95, -59, 15, 46, 63, 91, 104, 250, 186, 317}, {-333, -150, -14, 100, 151, 177, 182, 200, 248, 271, 307, 333, 333, 300, 260}};
const int Scoring::Params::QUEEN_MOBILITY[2][24] = {{-98, 26, -10, 36, 72, 111, 154, 178, 204, 241, 231, 270, 273, 264, 237, 248, 280, 333, 324, 333, 333, 333, 333, 333}, {-333, -333, -333, -283, -123, 21, 174, 166, 247, 234, 272, 300, 274, 333, 333, 293, 333, 333, 324, 202, 237, 210, 175, 54}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -124, 5, 72, 99};

const int Scoring::Params::KNIGHT_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{35, 0},{175, 250},{164, 131},{384, 135},{384, 135},{164, 131},{175, 250},{35, 0},{0, 11},{500, 440},{500, 419},{241, 345},{241, 345},{500, 419},{500, 440},{0, 11},{8, 28},{17, 35},{58, 34},{19, 54},{19, 54},{58, 34},{17, 35},{8, 28},{0, 0},{0, 38},{0, 3},{0, 33},{0, 33},{0, 3},{0, 38},{0, 0}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{116, 0},{239, 196},{274, 295},{498, 309},{498, 309},{274, 295},{239, 196},{116, 0},{22, 145},{500, 458},{500, 500},{500, 412},{500, 412},{500, 500},{500, 458},{22, 145},{500, 0},{219, 146},{97, 168},{475, 74},{475, 74},{97, 168},{219, 146},{500, 0},{0, 0},{500, 310},{500, 484},{468, 0},{468, 0},{500, 484},{500, 310},{0, 0}}};
const int Scoring::Params::BISHOP_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{143, 279},{115, 166},{364, 170},{339, 226},{339, 226},{364, 170},{115, 166},{143, 279},{339, 226},{500, 334},{500, 500},{373, 432},{373, 432},{500, 500},{500, 334},{339, 226},{75, 191},{0, 242},{24, 326},{0, 333},{0, 333},{24, 326},{0, 242},{75, 191},{142, 163},{8, 174},{0, 166},{0, 205},{0, 205},{0, 166},{8, 174},{142, 163}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 77},{289, 172},{364, 156},{452, 181},{452, 181},{364, 156},{289, 172},{0, 77},{0, 0},{464, 336},{500, 500},{500, 377},{500, 377},{500, 500},{464, 336},{0, 0},{162, 0},{500, 0},{128, 4},{500, 0},{500, 0},{128, 4},{500, 0},{162, 0},{0, 0},{500, 0},{500, 0},{500, 0},{500, 0},{500, 0},{500, 0},{0, 0}}};


