// Copyright 2015 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {211, 34, -77, -257};
const int Scoring::Params::RBN_ADJUST[4] = {495, 585, 908, 1075};
const int Scoring::Params::QR_ADJUST[4] = {-516, -92, 492, 476};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {-19, -2385, -1553};
const int Scoring::Params::PAWN_TRADE[3] = {-367, -260, -89};
const int Scoring::Params::CASTLING[6] = {3, -55, -105, 284, 191, -265};
const int Scoring::Params::KING_COVER[5] = {312, 218, 156, 27, 44};
const int Scoring::Params::KING_FILE_OPEN = -94;
const int Scoring::Params::KING_DISTANCE_BASIS = 306;
const int Scoring::Params::KING_DISTANCE_MULT = 88;
const int Scoring::Params::PIN_MULTIPLIER_MID = 437;
const int Scoring::Params::PIN_MULTIPLIER_END = 422;
const int Scoring::Params::KING_ATTACK_PARAM1 = 491;
const int Scoring::Params::KING_ATTACK_PARAM2 = 243;
const int Scoring::Params::KING_ATTACK_PARAM3 = 1757;
const int Scoring::Params::KING_ATTACK_BOOST_THRESHOLD = 140;
const int Scoring::Params::KING_ATTACK_BOOST_DIVISOR = 483;
const int Scoring::Params::BISHOP_TRAPPED = -416;
const int Scoring::Params::BISHOP_PAIR_MID = 329;
const int Scoring::Params::BISHOP_PAIR_END = 745;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -144;
const int Scoring::Params::BAD_BISHOP_MID = -72;
const int Scoring::Params::BAD_BISHOP_END = -72;
const int Scoring::Params::OUTPOST_NOT_DEFENDED = 42;
const int Scoring::Params::CENTER_PAWN_BLOCK = -52;
const int Scoring::Params::OUTSIDE_PASSER_MID = 134;
const int Scoring::Params::OUTSIDE_PASSER_END = 247;
const int Scoring::Params::WEAK_PAWN_MID = -75;
const int Scoring::Params::WEAK_PAWN_END = -83;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -98;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -108;
const int Scoring::Params::SPACE = 24;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 33;
const int Scoring::Params::ROOK_ON_7TH_MID = 194;
const int Scoring::Params::ROOK_ON_7TH_END = 286;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 515;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 675;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 174;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 12;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_MID = 48;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_END = 108;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 46;
const int Scoring::Params::ROOK_BEHIND_PP_END = 97;
const int Scoring::Params::QUEEN_OUT = -63;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_MID = -8;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_END = -18;
const int Scoring::Params::PP_BLOCK_BASE_MID = 227;
const int Scoring::Params::PP_BLOCK_BASE_END = 169;
const int Scoring::Params::PP_BLOCK_MULT_MID = 19;
const int Scoring::Params::PP_BLOCK_MULT_END = 5;
const int Scoring::Params::ENDGAME_PAWN_BONUS = 132;
const int Scoring::Params::KING_NEAR_PASSER = 203;
const int Scoring::Params::OPP_KING_NEAR_PASSER = -296;
const int Scoring::Params::PAWN_SIDE_BONUS = 310;
const int Scoring::Params::SUPPORTED_PASSER6 = 394;
const int Scoring::Params::SUPPORTED_PASSER7 = 786;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -113;
const int Scoring::Params::MATERIAL_SCALE[32] = {0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 4, 5, 8, 11, 16, 22, 30, 40, 52, 64, 76, 88, 98, 106, 112, 117, 120, 123, 124, 126, 126, 127};
const int Scoring::Params::MIDGAME_THRESHOLD = 14;
const int Scoring::Params::ENDGAME_THRESHOLD = 23;
const int Scoring::Params::KNIGHT_PST[2][64] = 
{{-687 ,-312 ,-334 ,-356 ,-356 ,-334 ,-312 ,-687 ,-209 ,-44 ,-2 ,-24 ,-24 ,-2 ,-44 ,-209 ,-209 ,20 ,62 ,-24 ,-24 ,62 ,20 ,-209 ,-209 ,20 ,-2 ,159 ,159 ,-2 ,20 ,-209 ,-136 ,93 ,71 ,232 ,232 ,71 ,93 ,-136 ,-226 ,3 ,45 ,-41 ,-41 ,45 ,3 ,-226 ,-215 ,-50 ,-8 ,-30 ,-30 ,-8 ,-50 ,-215 ,-247 ,-82 ,-104 ,-126 ,-126 ,-104 ,-82 ,-247 }, {-429 ,-113 ,-38 ,37 ,37 ,-38 ,-113 ,-429 ,-87 ,150 ,407 ,482 ,482 ,407 ,150 ,-87 ,-87 ,332 ,589 ,482 ,482 ,589 ,332 ,-87 ,-87 ,332 ,407 ,332 ,332 ,407 ,332 ,-87 ,-70 ,349 ,424 ,349 ,349 ,424 ,349 ,-70 ,-56 ,363 ,620 ,513 ,513 ,620 ,363 ,-56 ,-122 ,115 ,372 ,447 ,447 ,372 ,115 ,-122 ,-94 ,143 ,218 ,293 ,293 ,218 ,143 ,-94 }};
const int Scoring::Params::BISHOP_PST[2][64] = 
{{-306 ,-142 ,-142 ,-142 ,-142 ,-142 ,-142 ,-306 ,77 ,99 ,99 ,99 ,99 ,99 ,99 ,77 ,77 ,99 ,121 ,121 ,121 ,121 ,99 ,77 ,77 ,99 ,121 ,143 ,143 ,121 ,99 ,77 ,77 ,99 ,121 ,143 ,143 ,121 ,99 ,77 ,77 ,99 ,121 ,121 ,121 ,121 ,99 ,77 ,77 ,99 ,99 ,99 ,99 ,99 ,99 ,77 ,-92 ,-92 ,-92 ,-92 ,-92 ,-92 ,-92 ,-92 }, {-245 ,-125 ,-125 ,-125 ,-125 ,-125 ,-125 ,-245 ,49 ,63 ,63 ,63 ,63 ,63 ,63 ,49 ,49 ,63 ,77 ,77 ,77 ,77 ,63 ,49 ,49 ,63 ,77 ,91 ,91 ,77 ,63 ,49 ,49 ,63 ,77 ,91 ,91 ,77 ,63 ,49 ,49 ,63 ,77 ,77 ,77 ,77 ,63 ,49 ,49 ,63 ,63 ,63 ,63 ,63 ,63 ,49 ,-130 ,-130 ,-130 ,-130 ,-130 ,-130 ,-130 ,-130 }};
const int Scoring::Params::KNIGHT_OUTPOST[64] = 
{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,204 ,157 ,110 ,63 ,63 ,110 ,157 ,204 ,229 ,265 ,301 ,337 ,337 ,301 ,265 ,229 ,262 ,285 ,308 ,331 ,331 ,308 ,285 ,262 ,41 ,65 ,89 ,113 ,113 ,89 ,65 ,41 ,163 ,185 ,207 ,229 ,229 ,207 ,185 ,163 };
const int Scoring::Params::BISHOP_OUTPOST[64] = 
{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,240 ,273 ,306 ,339 ,339 ,306 ,273 ,240 ,369 ,319 ,269 ,219 ,219 ,269 ,319 ,369 ,206 ,247 ,288 ,329 ,329 ,288 ,247 ,206 ,97 ,120 ,143 ,166 ,166 ,143 ,120 ,97 ,155 ,171 ,187 ,203 ,203 ,187 ,171 ,155 };
const int Scoring::Params::KING_PST[2][64] = {{33 ,37 ,41 ,45 ,45 ,41 ,37 ,33 ,-201 ,-210 ,-219 ,-228 ,-228 ,-219 ,-210 ,-201 ,-334 ,-356 ,-378 ,-400 ,-400 ,-378 ,-356 ,-334 ,-437 ,-442 ,-447 ,-452 ,-452 ,-447 ,-442 ,-437 ,-540 ,-540 ,-540 ,-540 ,-540 ,-540 ,-540 ,-540 ,-504 ,-511 ,-518 ,-525 ,-525 ,-518 ,-511 ,-504 ,-500 ,-496 ,-492 ,-488 ,-488 ,-492 ,-496 ,-500 ,-551 ,-550 ,-549 ,-548 ,-548 ,-549 ,-550 ,-551 }, {-218 ,-178 ,-138 ,-98 ,-98 ,-138 ,-178 ,-218 ,-222 ,-192 ,-162 ,-132 ,-132 ,-162 ,-192 ,-222 ,-195 ,-155 ,-115 ,-75 ,-75 ,-115 ,-155 ,-195 ,-89 ,-29 ,31 ,91 ,91 ,31 ,-29 ,-89 ,-23 ,28 ,79 ,130 ,130 ,79 ,28 ,-23 ,35 ,93 ,151 ,209 ,209 ,151 ,93 ,35 ,109 ,159 ,209 ,259 ,259 ,209 ,159 ,109 ,35 ,94 ,153 ,212 ,212 ,153 ,94 ,35 }};
const int Scoring::Params::KNIGHT_MOBILITY[9] = {-147, -79, -22, 24, 61, 88, 107, 119, 125};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-197, -158, -122, -89, -59, -31, -6, 16, 36, 53, 67, 79, 88, 94, 98};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-237, -176, -119, -66, -17, 27, 68, 104, 137, 165, 189, 208, 224, 235, 242}, {-307, -226, -151, -81, -17, 41, 94, 142, 183, 220, 250, 275, 295, 309, 317}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-78, -70, -62, -55, -48, -41, -35, -29, -23, -17, -12, -7, -3, 2, 6, 9, 13, 16, 19, 21, 24, 26, 28, 29, 30, 31, 32, 33, 33}, {-76, -68, -60, -52, -44, -37, -30, -23, -17, -11, -5, 1, 6, 11, 16, 20, 24, 28, 32, 35, 38, 41, 43, 45, 47, 48, 49, 50, 51}};
const int Scoring::Params::TRADE_DOWN[16] = {740, 694, 647, 601, 554, 508, 462, 415, 369, 323, 277, 230, 184, 138, 92, 46};
const int Scoring::Params::KING_MOBILITY_ENDGAME[9] = {-193, -146, -104, -68, -38, -13, 6, 19, 27};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 98, 98, 244, 455, 748, 1133, 1617}, {0, 124, 124, 285, 546, 912, 1386, 1970}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 58, 58, 149, 309, 535, 824, 1172}, {0, 63, 63, 159, 361, 680, 1123, 1693}};
const int Scoring::Params::CONNECTED_PASSERS[2][8] = {{0, 1, 1, 98, 364, 788, 1363, 2086}, {0, 6, 6, 77, 329, 791, 1479, 2406}};
const int Scoring::Params::ADJACENT_PASSERS[2][8] = {{0, 0, 0, 58, 232, 522, 928, 1450}, {0, 2, 2, 57, 226, 514, 920, 1447}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-54, -67, -80, -93, -93, -80, -67, -54}, {-58, -62, -66, -70, -70, -66, -62, -58}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{-84, -65, -46, -27, -27, -46, -65, -84}, {-93, -95, -97, -99, -99, -97, -95, -93}};


