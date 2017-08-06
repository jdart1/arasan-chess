// Copyright 2015-2017 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
// Generated 05-Aug-2017 08:27:10 by "tuner -n 175 -c 24 -R 50 /home/jdart/chess/epd/big.epd"
//

const int Scoring::Params::RB_ADJUST[4] = {100, 54, 222, 102};
const int Scoring::Params::RBN_ADJUST[4] = {-150, -300, -500, -500};
const int Scoring::Params::QR_ADJUST[4] = {-150, 500, 863, 250};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2197, -1178};
const int Scoring::Params::CASTLING[6] = {100, -224, -205, 0, 73, -105};
const int Scoring::Params::KING_COVER[6][4] = {{91, 182, 86, 83},
{52, 103, 49, 47},
{-32, -64, -30, -29},
{-3, -5, -3, -3},
{-72, -144, -68, -66},
{-206, -412, -195, -188}};
const int Scoring::Params::KING_COVER_BASE = -100;
const int Scoring::Params::KING_DISTANCE_BASIS = 312;
const int Scoring::Params::KING_DISTANCE_MULT = 77;
const int Scoring::Params::PIN_MULTIPLIER_MID = 455;
const int Scoring::Params::PIN_MULTIPLIER_END = 500;
const int Scoring::Params::ROOK_VS_PAWNS = 0;
const int Scoring::Params::KRMINOR_VS_R = -100;
const int Scoring::Params::KRMINOR_VS_R_NO_PAWNS = -250;
const int Scoring::Params::KQMINOR_VS_Q = 0;
const int Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = -655;
const int Scoring::Params::MINOR_FOR_PAWNS = 179;
const int Scoring::Params::ENDGAME_PAWN_ADVANTAGE = 250;
const int Scoring::Params::PAWN_ENDGAME1 = 246;
const int Scoring::Params::PAWN_ENDGAME2 = 245;
const int Scoring::Params::PAWN_ATTACK_FACTOR1 = 9;
const int Scoring::Params::PAWN_ATTACK_FACTOR2 = 11;
const int Scoring::Params::MINOR_ATTACK_FACTOR = 30;
const int Scoring::Params::MINOR_ATTACK_BOOST = 40;
const int Scoring::Params::ROOK_ATTACK_FACTOR = 49;
const int Scoring::Params::ROOK_ATTACK_BOOST = 17;
const int Scoring::Params::ROOK_ATTACK_BOOST2 = 30;
const int Scoring::Params::QUEEN_ATTACK_FACTOR = 47;
const int Scoring::Params::QUEEN_ATTACK_BOOST = 44;
const int Scoring::Params::QUEEN_ATTACK_BOOST2 = 56;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_BASE = 8;
const int Scoring::Params::KING_ATTACK_COVER_BOOST_SLOPE = 18;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_MID = 12;
const int Scoring::Params::PAWN_THREAT_ON_PIECE_END = 84;
const int Scoring::Params::PIECE_THREAT_MM_MID = 183;
const int Scoring::Params::PIECE_THREAT_MR_MID = 400;
const int Scoring::Params::PIECE_THREAT_MQ_MID = 283;
const int Scoring::Params::PIECE_THREAT_MM_END = 244;
const int Scoring::Params::PIECE_THREAT_MR_END = 430;
const int Scoring::Params::PIECE_THREAT_MQ_END = 750;
const int Scoring::Params::MINOR_PAWN_THREAT_MID = 73;
const int Scoring::Params::MINOR_PAWN_THREAT_END = 195;
const int Scoring::Params::PIECE_THREAT_RM_MID = 148;
const int Scoring::Params::PIECE_THREAT_RR_MID = 105;
const int Scoring::Params::PIECE_THREAT_RQ_MID = 527;
const int Scoring::Params::PIECE_THREAT_RM_END = 129;
const int Scoring::Params::PIECE_THREAT_RR_END = 128;
const int Scoring::Params::PIECE_THREAT_RQ_END = 482;
const int Scoring::Params::ROOK_PAWN_THREAT_MID = 121;
const int Scoring::Params::ROOK_PAWN_THREAT_END = 213;
const int Scoring::Params::ENDGAME_KING_THREAT = 321;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 238;
const int Scoring::Params::BISHOP_PAIR_END = 477;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -50;
const int Scoring::Params::BAD_BISHOP_MID = -39;
const int Scoring::Params::BAD_BISHOP_END = -45;
const int Scoring::Params::CENTER_PAWN_BLOCK = -225;
const int Scoring::Params::OUTSIDE_PASSER_MID = 110;
const int Scoring::Params::OUTSIDE_PASSER_END = 234;
const int Scoring::Params::WEAK_PAWN_MID = -15;
const int Scoring::Params::WEAK_PAWN_END = 0;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -165;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -182;
const int Scoring::Params::SPACE = 36;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_MID = 0;
const int Scoring::Params::ROOK_ON_7TH_END = 170;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 280;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 603;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 168;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 123;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 0;
const int Scoring::Params::ROOK_BEHIND_PP_END = 67;
const int Scoring::Params::QUEEN_OUT = -68;
const int Scoring::Params::PAWN_SIDE_BONUS = 191;
const int Scoring::Params::KING_OWN_PAWN_DISTANCE = 98;
const int Scoring::Params::KING_OPP_PAWN_DISTANCE = 12;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_MID = 626;
const int Scoring::Params::QUEENING_SQUARE_CONTROL_END = 455;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_MID = -214;
const int Scoring::Params::QUEENING_SQUARE_OPP_CONTROL_END = -372;
const int Scoring::Params::WRONG_COLOR_BISHOP = 0;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -26;
const int Scoring::Params::KING_OPP_PASSER_DISTANCE[6] = {0, 0, 222, 481, 750, 750};
const int Scoring::Params::KING_POSITION_LOW_MATERIAL[3] ={200, 179, 129};
const int Scoring::Params::KING_ATTACK_SCALE[Scoring::Params::KING_ATTACK_SCALE_SIZE] = {-16, -14, -12, -10, -7, -4, -2, 1, 5, 8, 12, 15, 20, 24, 28, 33, 39, 44, 50, 56, 63, 70, 77, 85, 94, 103, 112, 122, 133, 144, 156, 169, 183, 197, 212, 228, 245, 263, 282, 302, 323, 345, 368, 393, 419, 446, 475, 505, 537, 570, 605, 641, 680, 720, 761, 805, 850, 897, 946, 997, 1049, 1104, 1160, 1218, 1278, 1340, 1403, 1468, 1535, 1603, 1672, 1743, 1814, 1887, 1961, 2036, 2111, 2187, 2263, 2339, 2415, 2492, 2568, 2643, 2718, 2793, 2866, 2939, 3011, 3081, 3150, 3218, 3284, 3349, 3412, 3473, 3533, 3591, 3647, 3701, 3753, 3804, 3852, 3899, 3944, 3987, 4028, 4068, 4106, 4142, 4177, 4210, 4241, 4271, 4299, 4327, 4352, 4377, 4400, 4422, 4443, 4462, 4481, 4499, 4516, 4531, 4546, 4561, 4574, 4586, 4598, 4610, 4620, 4630, 4640, 4648, 4657, 4665, 4672, 4679, 4686, 4692, 4698, 4703, 4708, 4713, 4718, 4722, 4726, 4730};
const int Scoring::Params::TRADE_DOWN[8] = {1021, 935, 183, 97, 11, 0, 0, 0};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 0, 0, 0, 194, 708, 787}, {0, 0, 0, 0, 164, 561, 1217, 1699}};
const int Scoring::Params::PASSED_PAWN_FILE_ADJUST[8] = {78, 83, 77, 72, 72, 77, 83, 78};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 0, 0, 0, 23, 750, 0}, {0, 0, 0, 0, 0, 136, 750, 0}};
const int Scoring::Params::CONNECTED_PASSER[2][8] = {{0, 0, 0, 0, 19, 647, 1250, 1750}, {0, 0, 0, 3, 311, 715, 1250, 1750}};
const int Scoring::Params::ADJACENT_PASSER[2][8] = {{0, 0, 0, 50, 70, 216, 709, 1427}, {0, 0, 0, 50, 74, 230, 382, 739}};
const int Scoring::Params::PP_OWN_PIECE_BLOCK[2][21] = {{-333, -281, -260, -168, -333, 0, -182, -333, -194, -333, 0, -283, -48, -302, -35, -213, -61, -333, 0, -146, 0}, {0, 0, -30, -126, -94, -55, -15, -125, -132, -211, -179, -85, -58, -47, -255, -116, -89, -236, -333, -333, -333}};
const int Scoring::Params::PP_OPP_PIECE_BLOCK[2][21] = {{-333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -333, -101, 0, -67, -168, 0, -8, -333, -124, -333}, {-137, -90, -102, -73, -110, -99, -155, -104, -115, -67, -62, -183, -49, -16, -10, -333, -177, -147, -333, -333, -333}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-250, -7, -195, -162, -162, -195, -7, -250}, {-250, -161, -224, -168, -168, -224, -161, -250}};
const int Scoring::Params::TRIPLED_PAWNS[2][8] = {{0, -229, -119, 0, 0, -119, -229, 0}, {0, -486, -267, -420, -420, -267, -486, 0}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{0, 0, -81, -106, -106, -81, 0, 0}, {-49, -84, -75, -126, -126, -75, -84, -49}};

const int Scoring::Params::KNIGHT_PST[2][64] = {{-500, -243, -88, -60, -60, -88, -243, -500, -99, -186, -159, 21, 21, -159, -186, -99, -256, -78, 15, 99, 99, 15, -78, -256, -138, -115, 66, 59, 59, 66, -115, -138, 25, -86, 106, -7, -7, 106, -86, 25, -199, -500, -474, -115, -115, -474, -500, -199, -457, -334, 18, -90, -90, 18, -334, -457, -500, -500, -500, -500, -500, -500, -500, -500}, {-332, -147, -140, -96, -96, -140, -147, -332, -100, -135, -47, -8, -8, -47, -135, -100, -165, -87, -50, 113, 113, -50, -87, -165, -48, 1, 171, 211, 211, 171, 1, -48, 11, -49, 56, 101, 101, 56, -49, 11, -147, -500, -287, -126, -126, -287, -500, -147, -312, -127, -104, -34, -34, -104, -127, -312, -500, -308, -432, -280, -280, -432, -308, -500}};
const int Scoring::Params::BISHOP_PST[2][64] = {{59, 294, 114, 130, 130, 114, 294, 59, 186, 224, 240, 146, 146, 240, 224, 186, 45, 190, 170, 173, 173, 170, 190, 45, -38, -86, 75, 134, 134, 75, -86, -38, -251, -94, -148, -28, -28, -148, -94, -251, -45, -500, -500, -453, -453, -500, -500, -45, 42, -359, -233, -500, -500, -233, -359, 42, -109, -500, -500, -500, -500, -500, -500, -109}, {-111, -17, 57, 22, 22, 57, -17, -111, 34, 60, 44, 79, 79, 44, 60, 34, -22, 43, 118, 148, 148, 118, 43, -22, -31, 70, 135, 187, 187, 135, 70, -31, -146, -7, 12, 66, 66, 12, -7, -146, 140, -214, -371, -325, -325, -371, -214, 140, -10, -31, -281, -258, -258, -281, -31, -10, 111, 191, 146, -94, -94, 146, 191, 111}};
const int Scoring::Params::ROOK_PST[2][64] = {{-392, -296, -261, -237, -237, -261, -296, -392, -423, -250, -269, -253, -253, -269, -250, -423, -367, -244, -328, -343, -343, -328, -244, -367, -361, -335, -379, -306, -306, -379, -335, -361, -286, -229, -217, -199, -199, -217, -229, -286, -260, -52, -72, -74, -74, -72, -52, -260, -199, -378, -42, -78, -78, -42, -378, -199, -121, -329, -500, -500, -500, -500, -329, -121}, {26, 51, 105, 61, 61, 105, 51, 26, 3, 46, 88, 72, 72, 88, 46, 3, 17, 64, 58, 59, 59, 58, 64, 17, 72, 155, 121, 122, 122, 121, 155, 72, 200, 197, 202, 205, 205, 202, 197, 200, 214, 307, 296, 255, 255, 296, 307, 214, 121, 163, 253, 213, 213, 253, 163, 121, 296, 361, 291, 210, 210, 291, 361, 296}};
const int Scoring::Params::QUEEN_PST[2][64] = {{67, 96, 96, 187, 187, 96, 96, 67, 180, 169, 197, 224, 224, 197, 169, 180, 99, 161, 181, 107, 107, 181, 161, 99, 107, 162, 110, 131, 131, 110, 162, 107, 139, 104, 107, 171, 171, 107, 104, 139, 72, 106, 165, 140, 140, 165, 106, 72, -32, -138, 24, -34, -34, 24, -138, -32, -34, 380, 209, 248, 248, 209, 380, -34}, {-71, -214, -226, -145, -145, -226, -214, -71, -130, -140, -106, 48, 48, -106, -140, -130, -110, 99, 197, 184, 184, 197, 99, -110, 91, 233, 255, 368, 368, 255, 233, 91, 216, 395, 445, 500, 500, 445, 395, 216, 176, 369, 500, 500, 500, 500, 369, 176, 294, 406, 500, 500, 500, 500, 406, 294, 75, 112, 128, 229, 229, 128, 112, 75}};
const int Scoring::Params::KING_PST[2][64] = {{292, 392, 107, 194, 194, 107, 392, 292, 482, 427, 206, 365, 365, 206, 427, 482, -77, 272, -228, -165, -165, -228, 272, -77, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, -500, 500, -387, -500, -500, -387, 500, -500, -500, -500, -500, -500, -500, -500, -500, -500}, {-499, -470, -497, -500, -500, -497, -470, -499, -360, -382, -445, -420, -420, -445, -382, -360, -422, -409, -369, -353, -353, -369, -409, -422, -364, -304, -222, -201, -201, -222, -304, -364, -248, -143, -44, -5, -5, -44, -143, -248, -166, -46, 58, 73, 73, 58, -46, -166, -202, -35, 15, 15, 15, 15, -35, -202, -490, -211, -131, -159, -159, -131, -211, -490}};

const int Scoring::Params::KNIGHT_MOBILITY[9] = {-490, -252, -128, -64, -15, 39, 67, 71, 41};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-333, -217, -120, -55, 0, 51, 79, 98, 143, 153, 160, 222, 174, 172, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-333, -223, -173, -150, -137, -89, -55, 20, 50, 66, 96, 106, 247, 196, 329}, {-333, -142, -13, 103, 160, 183, 185, 204, 253, 278, 310, 330, 333, 306, 289}};
const int Scoring::Params::QUEEN_MOBILITY[2][24] = {{-116, 40, 7, 47, 85, 123, 164, 189, 215, 252, 240, 280, 280, 278, 253, 259, 305, 333, 333, 333, 333, 333, 303, 333}, {-333, -333, -333, -287, -117, 48, 210, 180, 271, 275, 294, 328, 310, 333, 333, 333, 333, 333, 333, 273, 287, 272, 275, 144}};
const int Scoring::Params::KING_MOBILITY_ENDGAME[5] = {-333, -131, 2, 75, 102};

const int Scoring::Params::KNIGHT_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{34, 0},{173, 260},{159, 162},{391, 123},{391, 123},{159, 162},{173, 260},{34, 0},{0, 5},{500, 500},{500, 426},{258, 385},{258, 385},{500, 426},{500, 500},{0, 5},{3, 32},{15, 45},{60, 36},{42, 59},{42, 59},{60, 36},{15, 45},{3, 32},{0, 0},{0, 33},{0, 0},{0, 25},{0, 25},{0, 0},{0, 33},{0, 0}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{107, 0},{234, 199},{277, 303},{500, 299},{500, 299},{277, 303},{234, 199},{107, 0},{1, 158},{500, 500},{500, 500},{500, 427},{500, 427},{500, 500},{500, 500},{1, 158},{500, 0},{199, 111},{75, 175},{490, 77},{490, 77},{75, 175},{199, 111},{500, 0},{0, 0},{500, 112},{500, 500},{476, 36},{476, 36},{500, 500},{500, 112},{0, 0}}};
const int Scoring::Params::BISHOP_OUTPOST[2][64][2] = {{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{140, 290},{110, 166},{364, 177},{338, 253},{338, 253},{364, 177},{110, 166},{140, 290},{361, 248},{500, 376},{500, 500},{364, 498},{364, 498},{500, 500},{500, 376},{361, 248},{70, 220},{0, 267},{18, 376},{0, 354},{0, 354},{18, 376},{0, 267},{70, 220},{156, 176},{0, 162},{0, 180},{0, 193},{0, 193},{0, 180},{0, 162},{156, 176}},{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 0},{0, 59},{286, 174},{352, 152},{458, 181},{458, 181},{352, 152},{286, 174},{0, 59},{0, 0},{460, 384},{500, 500},{500, 430},{500, 430},{500, 500},{460, 384},{0, 0},{330, 0},{500, 0},{160, 0},{500, 0},{500, 0},{160, 0},{500, 0},{330, 0},{0, 0},{500, 0},{381, 0},{500, 0},{500, 0},{381, 0},{500, 0},{0, 0}}};


