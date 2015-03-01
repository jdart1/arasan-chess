// Copyright 2015 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {388, 179, 83, -262};
const int Scoring::Params::RBN_ADJUST[4] = {547, 562, 737, 1054};
const int Scoring::Params::QR_ADJUST[4] = {-447, -147, 489, 559};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {-34, -2633, -1526};
const int Scoring::Params::PAWN_TRADE[3] = {-415, -347, -128};
const int Scoring::Params::CASTLING[6] = {-52, -47, -64, 185, 283, -226};
const int Scoring::Params::KING_COVER[5] = {287, 270, 103, 22, 35};
const int Scoring::Params::KING_FILE_OPEN = -108;
const int Scoring::Params::KING_DISTANCE_BASIS = 281;
const int Scoring::Params::KING_DISTANCE_MULT = 97;
const int Scoring::Params::PIN_MULTIPLIER_MID = 408;
const int Scoring::Params::PIN_MULTIPLIER_END = 353;
const int Scoring::Params::KING_ATTACK_PARAM1 = 484;
const int Scoring::Params::KING_ATTACK_PARAM2 = 248;
const int Scoring::Params::KING_ATTACK_PARAM3 = 2733;
const int Scoring::Params::KING_ATTACK_BOOST_THRESHOLD = 155;
const int Scoring::Params::KING_ATTACK_BOOST_DIVISOR = 620;
const int Scoring::Params::BISHOP_TRAPPED = -455;
const int Scoring::Params::BISHOP_PAIR_MID = 380;
const int Scoring::Params::BISHOP_PAIR_END = 617;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -234;
const int Scoring::Params::BAD_BISHOP_MID = -76;
const int Scoring::Params::BAD_BISHOP_END = -70;
const int Scoring::Params::OUTPOST_NOT_DEFENDED = 43;
const int Scoring::Params::CENTER_PAWN_BLOCK = -15;
const int Scoring::Params::OUTSIDE_PASSER_MID = 102;
const int Scoring::Params::OUTSIDE_PASSER_END = 252;
const int Scoring::Params::WEAK_PAWN_MID = -92;
const int Scoring::Params::WEAK_PAWN_END = -96;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -109;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -134;
const int Scoring::Params::SPACE = 13;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 28;
const int Scoring::Params::ROOK_ON_7TH_MID = 353;
const int Scoring::Params::ROOK_ON_7TH_END = 278;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 525;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 757;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 148;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 10;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_MID = 162;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_END = 199;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 37;
const int Scoring::Params::ROOK_BEHIND_PP_END = 182;
const int Scoring::Params::QUEEN_OUT = -48;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_MID = -38;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_END = -8;
const int Scoring::Params::PP_BLOCK_BASE_MID = 156;
const int Scoring::Params::PP_BLOCK_BASE_END = 195;
const int Scoring::Params::PP_BLOCK_MULT_MID = 0;
const int Scoring::Params::PP_BLOCK_MULT_END = 5;
const int Scoring::Params::ENDGAME_PAWN_BONUS = 132;
const int Scoring::Params::KING_NEAR_PASSER = 160;
const int Scoring::Params::OPP_KING_NEAR_PASSER = -333;
const int Scoring::Params::PAWN_SIDE_BONUS = 244;
const int Scoring::Params::SUPPORTED_PASSER6 = 451;
const int Scoring::Params::SUPPORTED_PASSER7 = 731;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -79;
const int Scoring::Params::MATERIAL_SCALE[32] = {0, 1, 1, 2, 3, 4, 7, 11, 16, 24, 35, 49, 64, 79, 93, 104, 112, 117, 121, 124, 125, 126, 127, 127, 128, 128, 128, 128, 128, 128, 128, 128};
const int Scoring::Params::MIDGAME_THRESHOLD = 8;
const int Scoring::Params::ENDGAME_THRESHOLD = 15;
const int Scoring::Params::KNIGHT_PST[2][64] = 
{{-700 ,-380 ,-430 ,-480 ,-480 ,-430 ,-380 ,-700 ,-237 ,-100 ,-96 ,-146 ,-146 ,-96 ,-100 ,-237 ,-237 ,-46 ,-42 ,-146 ,-146 ,-42 ,-46 ,-237 ,-237 ,-46 ,-96 ,46 ,46 ,-96 ,-46 ,-237 ,-168 ,23 ,-27 ,115 ,115 ,-27 ,23 ,-168 ,-268 ,-77 ,-73 ,-177 ,-177 ,-73 ,-77 ,-268 ,-280 ,-143 ,-139 ,-189 ,-189 ,-139 ,-143 ,-280 ,-227 ,-90 ,-140 ,-190 ,-190 ,-140 ,-90 ,-227 }, {-344 ,-138 ,-109 ,-80 ,-80 ,-109 ,-138 ,-344 ,-102 ,58 ,191 ,220 ,220 ,191 ,58 ,-102 ,-102 ,162 ,295 ,220 ,220 ,295 ,162 ,-102 ,-102 ,162 ,191 ,179 ,179 ,191 ,162 ,-102 ,-80 ,184 ,213 ,201 ,201 ,213 ,184 ,-80 ,-79 ,185 ,318 ,243 ,243 ,318 ,185 ,-79 ,-170 ,-10 ,123 ,152 ,152 ,123 ,-10 ,-170 ,-149 ,11 ,40 ,69 ,69 ,40 ,11 ,-149 }};
const int Scoring::Params::BISHOP_PST[2][64] = 
{{-33 ,152 ,-84 ,-128 ,-128 ,-84 ,152 ,-33 ,280 ,280 ,280 ,44 ,44 ,280 ,280 ,280 ,44 ,280 ,280 ,280 ,280 ,280 ,280 ,44 ,44 ,44 ,280 ,386 ,386 ,280 ,44 ,44 ,44 ,44 ,280 ,386 ,386 ,280 ,44 ,44 ,44 ,280 ,280 ,280 ,280 ,280 ,280 ,44 ,280 ,280 ,280 ,44 ,44 ,280 ,280 ,280 ,73 ,73 ,-163 ,-163 ,-163 ,-163 ,73 ,73 }, {-44 ,65 ,-54 ,-73 ,-73 ,-54 ,65 ,-44 ,138 ,138 ,138 ,19 ,19 ,138 ,138 ,138 ,19 ,138 ,138 ,138 ,138 ,138 ,138 ,19 ,19 ,19 ,138 ,178 ,178 ,138 ,19 ,19 ,19 ,19 ,138 ,178 ,178 ,138 ,19 ,19 ,19 ,138 ,138 ,138 ,138 ,138 ,138 ,19 ,138 ,138 ,138 ,19 ,19 ,138 ,138 ,138 ,23 ,23 ,-96 ,-96 ,-96 ,-96 ,23 ,23 }};
const int Scoring::Params::KNIGHT_OUTPOST[64] = 
{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,-21 ,17 ,55 ,93 ,93 ,55 ,17 ,-21 ,289 ,451 ,613 ,775 ,775 ,613 ,451 ,289 ,438 ,453 ,468 ,483 ,483 ,468 ,453 ,438 ,259 ,291 ,323 ,355 ,355 ,323 ,291 ,259 ,87 ,44 ,1 ,-42 ,-42 ,1 ,44 ,87 };
const int Scoring::Params::BISHOP_OUTPOST[64] = 
{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,158 ,201 ,244 ,287 ,287 ,244 ,201 ,158 ,334 ,301 ,268 ,235 ,235 ,268 ,301 ,334 ,268 ,341 ,414 ,487 ,487 ,414 ,341 ,268 ,-139 ,-212 ,-285 ,-358 ,-358 ,-285 ,-212 ,-139 ,217 ,226 ,235 ,244 ,244 ,235 ,226 ,217 };
const int Scoring::Params::KING_PST[2][64] = {{65 ,36 ,7 ,-22 ,-22 ,7 ,36 ,65 ,-147 ,-151 ,-155 ,-159 ,-159 ,-155 ,-151 ,-147 ,-306 ,-330 ,-354 ,-378 ,-378 ,-354 ,-330 ,-306 ,-588 ,-568 ,-548 ,-528 ,-528 ,-548 ,-568 ,-588 ,-276 ,-250 ,-224 ,-198 ,-198 ,-224 ,-250 ,-276 ,-349 ,-361 ,-373 ,-385 ,-385 ,-373 ,-361 ,-349 ,-658 ,-621 ,-584 ,-547 ,-547 ,-584 ,-621 ,-658 ,-697 ,-638 ,-579 ,-520 ,-520 ,-579 ,-638 ,-697 }, {-477 ,-402 ,-327 ,-252 ,-252 ,-327 ,-402 ,-477 ,-305 ,-253 ,-201 ,-149 ,-149 ,-201 ,-253 ,-305 ,-232 ,-160 ,-88 ,-16 ,-16 ,-88 ,-160 ,-232 ,-7 ,79 ,165 ,251 ,251 ,165 ,79 ,-7 ,131 ,190 ,249 ,308 ,308 ,249 ,190 ,131 ,123 ,166 ,209 ,252 ,252 ,209 ,166 ,123 ,106 ,169 ,232 ,295 ,295 ,232 ,169 ,106 ,-110 ,-85 ,-60 ,-35 ,-35 ,-60 ,-85 ,-110 }};
const int Scoring::Params::KNIGHT_MOBILITY[9] = {-451, -99, -76, -63, -53, -46, -40, -35, -30};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-680, -21, -1, 11, 19, 26, 31, 35, 39, 43, 46, 49, 51, 53, 56};
const int Scoring::Params::ROOK_MOBILITY[15] = {-602, -67, -47, -35, -27, -20, -15, -11, -7, -3, 0, 3, 5, 7, 10};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-531, -51, -37, -29, -23, -19, -15, -12, -9, -7, -5, -3, -1, 0, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 13, 14, 15, 16}, {-101, -43, -27, -18, -11, -6, -2, 2, 5, 8, 10, 12, 14, 16, 18, 19, 21, 22, 23, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34}};
const int Scoring::Params::TRADE_DOWN[16] = {72, 67, 62, 57, 53, 48, 43, 39, 34, 30, 25, 21, 17, 12, 8, 4};
const int Scoring::Params::KING_MOBILITY_ENDGAME[9] = {-525, 20, 48, 64, 75, 84, 92, 98, 103};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 12, 12, 55, 197, 448, 815, 1300}, {0, 84, 84, 130, 289, 584, 1031, 1641}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 6, 6, 15, 42, 87, 150, 231}, {0, 155, 155, 180, 213, 254, 303, 360}};
const int Scoring::Params::CONNECTED_PASSERS[2][8] = {{0, 17, 17, 117, 368, 749, 1250, 1865}, {0, 6, 6, 66, 262, 604, 1099, 1750}};
const int Scoring::Params::ADJACENT_PASSERS[2][8] = {{0, 34, 34, 65, 140, 253, 400, 579}, {0, 13, 13, 72, 179, 319, 485, 672}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-75, -82, -89, -96, -96, -89, -82, -75}, {-69, -81, -93, -105, -105, -93, -81, -69}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{-101, -94, -87, -80, -80, -87, -94, -101}, {-112, -160, -208, -256, -256, -208, -160, -112}};


