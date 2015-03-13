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
const int Scoring::Params::MATERIAL_SCALE[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 12, 24, 36, 48, 60, 72, 84, 96, 108, 116, 120, 128, 128, 128, 128, 128, 128, 128, 128 };
const int Scoring::Params::MIDGAME_THRESHOLD = 12;
const int Scoring::Params::ENDGAME_THRESHOLD = 23;

const int Scoring::Params::KNIGHT_PST[2][64] = 
{{-220 ,-140 ,-110 ,-100 ,-100 ,-110 ,-140 ,-220,
  -150 ,-60 ,-40 ,-30 ,-30 ,-40 ,-60 ,-150,
  -120 ,-40 ,-10 ,0 ,0 ,-10 ,-40 ,-120,
  -110 ,-30 ,0 ,30 ,30 ,0 ,-30 ,-110,
  -110 ,-30 ,0 ,30 ,30 ,0 ,-30 ,-110,
  -120 ,-40 ,-10 ,0 ,0 ,-10,-40 ,-120,
  -150 ,-60 ,-40 ,-30 ,-30 ,-40 ,-60 ,-150,
  -180 ,-90 ,-70 ,-60 ,-60 ,-70 ,-90 ,-180},
 {-230 ,-190 ,-160 ,-150 ,-150 ,-150 ,-160 ,-190,
  -130 ,-90 ,-50 ,-40 ,-40 , -50 ,-90,-130,
  -90 ,-50 ,-20 ,-10 ,-10 ,-20 ,-50 ,-90,
  -80 ,-40 ,-10 ,0 ,0 ,-10 ,-40 ,-80,
  -80 ,-30 ,0 ,10 ,10 ,0 ,-30,-80,
  -90 ,-40 ,0 ,0 ,0 ,0 ,-40 ,-90,
  -130 ,-70 ,-40 ,-30 ,-30 ,-40 ,-70 ,-130,
  -170 ,-130 ,-90 ,-80 ,-80 ,-90 ,-130 ,-170
 }};

const int Scoring::Params::BISHOP_PST[2][64] = 
{{-180 ,-120 ,-120 ,-120 ,-120 ,-120 ,-120 ,-180,
-10 ,80 ,0 ,60 ,60 ,0 ,80 ,-10,
-10 ,0 ,60 ,80 ,80 ,60 ,0 ,-10,
0 ,0 ,60 ,100 ,100 ,60 ,0 ,0,
0 ,60 ,60 ,60 ,60 ,60 ,60 ,0,
100 ,100 ,100 ,100 ,100 ,100 ,100 ,100,
-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100,
-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100},
{0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0}};

const int Scoring::Params::KNIGHT_OUTPOST[64] = 
{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
10 ,30 ,60 ,60 ,60 ,60 ,30 ,10,
10 ,40 ,90 ,140 ,140 ,90 ,40 ,10,
10 ,40 ,90 ,140 ,140 ,90 ,40 ,10,
10 ,10 ,60 ,60 ,60 ,60 ,10 ,10,
10 ,10 ,10 ,10 ,10 ,10 ,10 ,10
};
const int Scoring::Params::BISHOP_OUTPOST[64] = 
{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
10 ,20 ,50 ,50 ,50 ,50 ,20 ,10,
10 ,30 ,70 ,110 ,110 ,70 ,30 ,10,
10 ,30 ,70 ,110 ,110 ,70 ,30 ,10,
10 ,10 ,50 ,50 ,50 ,50 ,10 ,10,
10 ,10 ,10 ,10 ,10 ,10 ,10 ,10
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
const int Scoring::Params::KNIGHT_MOBILITY[9] = {-180, -70, -20, 0, 20, 50, 70, 100, 120};
const int Scoring::Params::BISHOP_MOBILITY[15] = {-200, -110, -70, -30, 0, 30, 60, 90, 90, 90, 90, 90, 90, 90, 90};
const int Scoring::Params::ROOK_MOBILITY[2][15] = {{-220, -120, -80, -30, 0, 30, 70, 100, 120, 140, 170, 190, 210, 230, 240}, {-300, -170, -110, -50, 0, 50, 90, 140, 170, 200, 230, 260, 290, 310, 320}};
const int Scoring::Params::QUEEN_MOBILITY[2][29] = {{-100, -50, -10, -10, 40, 70, 90, 110, 130, 140, 160, 170, 190, 200, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210}, {-120, -60, -10, 10, 50, 80, 110, 130, 160, 170, 200, 210, 230, 250, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260}};
const int Scoring::Params::TRADE_DOWN[16] = {682, 640, 409, 597, 516, 469, 426, 384, 341, 298, 256, 213, 171, 128, 85, 42};
const int Scoring::Params::KING_MOBILITY_ENDGAME[9] = {-200, -120, -60, 0, 10, 20, 30, 30, 30};
const int Scoring::Params::PASSED_PAWN[2][8] = {{0, 0, 60, 110, 180, 270, 560, 1110}, {0, 0, 90, 160, 280, 420, 840, 1410}};
const int Scoring::Params::POTENTIAL_PASSER[2][8] = {{0, 0, 20, 40, 60, 90, 200, 0}, {0, 0, 30, 60, 100, 150, 300, 0}};
const int Scoring::Params::CONNECTED_PASSERS[2][8] = {{0, 0, 0, 100, 190, 240, 480, 830}, {0, 0, 0, 100, 190, 240, 480, 830}};
const int Scoring::Params::ADJACENT_PASSERS[2][8] = {{0, 0, 0, 80, 150, 170, 340, 700},{0, 0, 0, 80, 150, 170, 340, 700}};
const int Scoring::Params::DOUBLED_PAWNS[2][8] = {{-60, -80, -100, -100, -100, -100, -80, -60}, {-100, -130, -170, -170, -170, -170, -130, -100}};
const int Scoring::Params::ISOLATED_PAWN[2][8] = {{-80, -80, -80, -80, -80, -80, -80, -80}, {-120, -120, -120, -120, -120, -120, -120, -120}};


