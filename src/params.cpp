// Copyright 2015 by Jon Dart. All Rights Reserved.
// This is a generated file. Do not edit.
//

const int Scoring::Params::RB_ADJUST[4] = {250, 75, -75, -250};
const int Scoring::Params::RBN_ADJUST[4] = {500, 675, 825, 1000};
const int Scoring::Params::QR_ADJUST[4] = {-500, 0, 500, 500};
const int Scoring::Params::KN_VS_PAWN_ADJUST[3] = {0, -2400, -1500};
const int Scoring::Params::PAWN_TRADE[3] = {-450, -250, -100};
const int Scoring::Params::CASTLING[6] = {0, -70, -100, 280, 200, -280};
const int Scoring::Params::KING_COVER[5] = {220, 310, 120, 30, 20};
const int Scoring::Params::KING_FILE_OPEN = -150;
const int Scoring::Params::KING_DISTANCE_BASIS = 320;
const int Scoring::Params::KING_DISTANCE_MULT = 80;
const int Scoring::Params::PIN_MULTIPLIER_MID = 200;
const int Scoring::Params::PIN_MULTIPLIER_END = 300;
const int Scoring::Params::KING_ATTACK_PARAM1 = 500;
const int Scoring::Params::KING_ATTACK_PARAM2 = 320;
const int Scoring::Params::KING_ATTACK_PARAM3 = 1500;
const int Scoring::Params::KING_ATTACK_BOOST_THRESHOLD = 480;
const int Scoring::Params::KING_ATTACK_BOOST_DIVISOR = 500;
const int Scoring::Params::BISHOP_TRAPPED = -1470;
const int Scoring::Params::BISHOP_PAIR_MID = 420;
const int Scoring::Params::BISHOP_PAIR_END = 550;
const int Scoring::Params::BISHOP_PAWN_PLACEMENT_END = -160;
const int Scoring::Params::BAD_BISHOP_MID = -40;
const int Scoring::Params::BAD_BISHOP_END = -60;
const int Scoring::Params::OUTPOST_NOT_DEFENDED = 42;
const int Scoring::Params::CENTER_PAWN_BLOCK = -120;
const int Scoring::Params::OUTSIDE_PASSER_MID = 120;
const int Scoring::Params::OUTSIDE_PASSER_END = 250;
const int Scoring::Params::WEAK_PAWN_MID = -80;
const int Scoring::Params::WEAK_PAWN_END = -80;
const int Scoring::Params::WEAK_ON_OPEN_FILE_MID = -100;
const int Scoring::Params::WEAK_ON_OPEN_FILE_END = -100;
const int Scoring::Params::SPACE = 20;
const int Scoring::Params::PAWN_CENTER_SCORE_MID = 30;
const int Scoring::Params::ROOK_ON_7TH_MID = 260;
const int Scoring::Params::ROOK_ON_7TH_END = 260;
const int Scoring::Params::TWO_ROOKS_ON_7TH_MID = 570;
const int Scoring::Params::TWO_ROOKS_ON_7TH_END = 660;
const int Scoring::Params::ROOK_ON_OPEN_FILE_MID = 200;
const int Scoring::Params::ROOK_ON_OPEN_FILE_END = 200;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_MID = 100;
const int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_END = 100;
const int Scoring::Params::ROOK_BEHIND_PP_MID = 50;
const int Scoring::Params::ROOK_BEHIND_PP_END = 100;
const int Scoring::Params::QUEEN_OUT = -60;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_MID = -20;
const int Scoring::Params::PASSER_OWN_PIECE_BLOCK_END = -50;
const int Scoring::Params::PP_BLOCK_BASE_MID = 140;
const int Scoring::Params::PP_BLOCK_BASE_END = 140;
const int Scoring::Params::PP_BLOCK_MULT_MID = 72;
const int Scoring::Params::PP_BLOCK_MULT_END = 32;
const int Scoring::Params::ENDGAME_PAWN_BONUS = 120;
const int Scoring::Params::KING_NEAR_PASSER = 200;
const int Scoring::Params::OPP_KING_NEAR_PASSER = -280;
const int Scoring::Params::PAWN_SIDE_BONUS = 280;
const int Scoring::Params::SUPPORTED_PASSER6 = 380;
const int Scoring::Params::SUPPORTED_PASSER7 = 760;
const int Scoring::Params::SIDE_PROTECTED_PAWN = -100;
const int Scoring::Params::MATERIAL_SCALE[32] = {0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 4, 5, 8, 11, 16, 22, 30, 40, 52, 64, 76, 88, 98, 106, 112, 117, 120, 123, 124, 126, 126, 127};
const int Scoring::Params::MIDGAME_THRESHOLD = 14;
const int Scoring::Params::ENDGAME_THRESHOLD = 23;
const int Scoring::Params::KNIGHT_PST[2][64] = 
{{-220 ,-130 ,-125 ,-120 ,-120 ,-125 ,-130 ,-220
,-120 ,10 ,35 ,40 ,40 ,35 ,10 ,-120
,-120 ,30 ,55 ,40 ,40 ,55 ,30 ,-120
,-120 ,30 ,35 ,70 ,70 ,35 ,30 ,-120
,-55 ,95 ,100 ,135 ,135 ,100 ,95 ,-55
,-70 ,80 ,105 ,90 ,90 ,105 ,80 ,-70
,-131 ,-1 ,24 ,29 ,29 ,24 ,-1 ,-131
,-151 ,-21 ,-16 ,-11 ,-11 ,-16 ,-21 ,-151
}, {-280 ,-180 ,-175 ,-170 ,-170 ,-175 ,-180 ,-280
,-130 ,10 ,35 ,40 ,40 ,35 ,10 ,-130
,-130 ,30 ,55 ,40 ,40 ,55 ,30 ,-130
,-130 ,30 ,35 ,30 ,30 ,35 ,30 ,-130
,-113 ,47 ,52 ,47 ,47 ,52 ,47 ,-113
,-103 ,57 ,82 ,67 ,67 ,82 ,57 ,-103
,-148 ,-8 ,17 ,22 ,22 ,17 ,-8 ,-148
,-149 ,-9 ,-4 ,1 ,1 ,-4 ,-9 ,-149
}};
const int Scoring::Params::BISHOP_PST[2][64] = 
{{-180 ,-120 ,-120 ,-120 ,-120 ,-120 ,-120 ,-180
,0 ,20 ,20 ,20 ,20 ,20 ,20 ,0
,0 ,20 ,40 ,40 ,40 ,40 ,20 ,0
,0 ,20 ,40 ,60 ,60 ,40 ,20 ,0
,0 ,20 ,40 ,60 ,60 ,40 ,20 ,0
,0 ,20 ,40 ,40 ,40 ,40 ,20 ,0
,-100 ,-80 ,-80 ,-80 ,-80 ,-80 ,-80 ,-100
,-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100
}, {-172 ,-112 ,-112 ,-112 ,-112 ,-112 ,-112 ,-172
,0 ,20 ,20 ,20 ,20 ,20 ,20 ,0
,0 ,20 ,40 ,40 ,40 ,40 ,20 ,0
,0 ,20 ,40 ,60 ,60 ,40 ,20 ,0
,0 ,20 ,40 ,60 ,60 ,40 ,20 ,0
,0 ,20 ,40 ,40 ,40 ,40 ,20 ,0
,-100 ,-80 ,-80 ,-80 ,-80 ,-80 ,-80 ,-100
,-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100
}};
const int Scoring::Params::KNIGHT_OUTPOST[64] = 
{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
,10 ,30 ,50 ,70 ,70 ,50 ,30 ,10
,10 ,45 ,80 ,115 ,115 ,80 ,45 ,10
,10 ,45 ,80 ,115 ,115 ,80 ,45 ,10
,10 ,30 ,50 ,70 ,70 ,50 ,30 ,10
,10 ,10 ,10 ,10 ,10 ,10 ,10 ,10
};
const int Scoring::Params::BISHOP_OUTPOST[64] = 
{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
,10 ,25 ,40 ,55 ,55 ,40 ,25 ,10
,10 ,40 ,70 ,100 ,100 ,70 ,40 ,10
,10 ,40 ,70 ,100 ,100 ,70 ,40 ,10
,10 ,25 ,40 ,55 ,55 ,40 ,25 ,10
,10 ,10 ,10 ,10 ,10 ,10 ,10 ,10
};
const int Scoring::Params::KING_PST[2][64] = {{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
,-60 ,-60 ,-60 ,-60 ,-60 ,-60 ,-60 ,-60
,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
}, {-280 ,-230 ,-180 ,-130 ,-130 ,-180 ,-230 ,-280
,-220 ,-170 ,-120 ,-70 ,-70 ,-120 ,-170 ,-220
,-160 ,-110 ,-60 ,-10 ,-10 ,-60 ,-110 ,-160
,-100 ,-50 ,0 ,50 ,50 ,0 ,-50 ,-100
,-40 ,10 ,60 ,110 ,110 ,60 ,10 ,-40
,20 ,70 ,120 ,170 ,170 ,120 ,70 ,20
,80 ,130 ,180 ,230 ,230 ,180 ,130 ,80
,80 ,130 ,180 ,230 ,230 ,180 ,130 ,80
}};
const int Scoring::Params::KNIGHT_MOBILITY[9] = {-180, -73, 3, 54, 87, 106, 115, 119, 120};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-200, -134, -80, -36, -1, 27, 47, 63, 73, 81, 85, 88, 89, 90, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-220, -147, -82, -23, 28, 73, 112, 144, 172, 193, 210, 223, 232, 237, 239}, {-300, -199, -108, -26, 45, 108, 162, 207, 245, 275, 299, 316, 329, 336, 339}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-100, -69, -40, -13, 11, 34, 55, 75, 92, 108, 123, 136, 148, 158, 167, 175, 182, 188, 193, 197, 201, 203, 206, 207, 208, 209, 210, 210, 210}, {-120, -73, -31, 8, 42, 73, 101, 125, 147, 166, 182, 196, 209, 219, 228, 235, 241, 246, 250, 253, 255, 257, 258, 259, 259, 260, 260, 260, 260}};
const int Scoring::Params::TRADE_DOWN[16] = {688, 645, 602, 559, 516, 473, 430, 387, 344, 301, 258, 215, 172, 129, 86, 43};
const int Scoring::Params::KING_MOBILITY_ENDGAME[9] = {-200, -129, -75, -35, -7, 12, 23, 28, 30};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 56, 125, 206, 300, 606, 1125}, {0, 0, 83, 185, 303, 440, 843, 1465}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 25, 50, 75, 100, 225, 150}, {0, 0, 33, 67, 101, 135, 308, 202}};
const int Scoring::Params::CONNECTED_PASSERS[2][8] = {{0, 0, 37, 90, 157, 240, 487, 850}, {0, 0, 37, 90, 157, 240, 487, 850}};
const int Scoring::Params::ADJACENT_PASSERS[2][8] = {{0, 0, 30, 70, 120, 180, 370, 690}, {0, 0, 30, 70, 120, 180, 370, 690}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-60, -80, -100, -120, -120, -100, -80, -60}, {-60, -80, -100, -120, -120, -100, -80, -60}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{-80, -80, -80, -80, -80, -80, -80, -80}, {-120, -120, -120, -120, -120, -120, -120, -120}};


