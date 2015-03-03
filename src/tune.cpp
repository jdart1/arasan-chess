// Copyright 2014-2015 by Jon Dart. All Rights Reserved.
#include "tune.h"
#include "chess.h"
#include "attacks.h"
#include "scoring.h"

extern "C" {
#include <math.h>
#include <string.h>
};

#define PARAM(x) tune::tune_params[x].current

enum {
   RB_ADJUST1,
   RB_ADJUST2,
   RB_ADJUST3,
   RB_ADJUST4,
   RBN_ADJUST1,
   RBN_ADJUST2,
   RBN_ADJUST3,
   RBN_ADJUST4,
   QR_ADJUST0,
   QR_ADJUST1,
   QR_ADJUST2,
   QR_ADJUST3,
   KN_VS_PAWN_ADJUST0,
   KN_VS_PAWN_ADJUST1,
   KN_VS_PAWN_ADJUST2,
   PAWN_TRADE0,
   PAWN_TRADE1,
   PAWN_TRADE2,
   CASTLING0,
   CASTLING1,
   CASTLING2,
   CASTLING3,
   CASTLING4,
   CASTLING5,
   KING_COVER0,
   KING_COVER1,
   KING_COVER2,
   KING_COVER3,
   KING_COVER4,
   KING_FILE_OPEN,
   KING_DISTANCE_BASIS,
   KING_DISTANCE_MULT,
   PIN_MULTIPLIER_MID,
   PIN_MULTIPLIER_END,
   KING_ATTACK_PARAM1,
   KING_ATTACK_PARAM2,
   KING_ATTACK_PARAM3,
   KING_ATTACK_BOOST_THRESHOLD,
   KING_ATTACK_BOOST_DIVISOR,
   BISHOP_TRAPPED,
   BISHOP_PAIR_MID,
   BISHOP_PAIR_END,
   BISHOP_PAWN_PLACEMENT_END,
   BAD_BISHOP_MID,
   BAD_BISHOP_END,
   OUTPOST_NOT_DEFENDED,
   CENTER_PAWN_BLOCK,
   OUTSIDE_PASSER_MID,
   OUTSIDE_PASSER_END,
   WEAK_PAWN_MID,
   WEAK_PAWN_END,
   WEAK_ON_OPEN_FILE_MID,
   WEAK_ON_OPEN_FILE_END,
   SPACE,
   PAWN_CENTER_SCORE_MID,
   ROOK_ON_7TH_MID,
   ROOK_ON_7TH_END,
   TWO_ROOKS_ON_7TH_MID,
   TWO_ROOKS_ON_7TH_END,
   ROOK_ON_OPEN_FILE_MID,
   ROOK_ON_OPEN_FILE_END,
   ROOK_ATTACKS_WEAK_PAWN_MID,
   ROOK_ATTACKS_WEAK_PAWN_END,
   ROOK_BEHIND_PP_MID,
   ROOK_BEHIND_PP_END,
   QUEEN_OUT,
   PASSER_OWN_PIECE_BLOCK_MID,
   PASSER_OWN_PIECE_BLOCK_END,
   PP_BLOCK_BASE_MID,
   PP_BLOCK_BASE_END,
   PP_BLOCK_MULT_MID,
   PP_BLOCK_MULT_END,
   ENDGAME_PAWN_BONUS,
   KING_NEAR_PASSER,
   OPP_KING_NEAR_PASSER,
   PAWN_SIDE_BONUS,
   SUPPORTED_PASSER6,
   SUPPORTED_PASSER7,
   SIDE_PROTECTED_PAWN,
   TRADE_DOWN_LINEAR,
   TRADE_DOWN_SQ,
   KING_PST_RANK1_MID,
   KING_PST_RANK1_SLOPE_MID,
   KING_PST_RANK2_MID,
   KING_PST_RANK2_SLOPE_MID,
   KING_PST_RANK3_MID,
   KING_PST_RANK3_SLOPE_MID,
   KING_PST_RANK4_MID,
   KING_PST_RANK4_SLOPE_MID,
   KING_PST_RANK5_MID,
   KING_PST_RANK5_SLOPE_MID,
   KING_PST_RANK6_MID,
   KING_PST_RANK6_SLOPE_MID,
   KING_PST_RANK7_MID,
   KING_PST_RANK7_SLOPE_MID,
   KING_PST_RANK8_MID,
   KING_PST_RANK8_SLOPE_MID,
   KING_PST_RANK1_END,
   KING_PST_RANK1_SLOPE_END,
   KING_PST_RANK2_END,
   KING_PST_RANK2_SLOPE_END,
   KING_PST_RANK3_END,
   KING_PST_RANK3_SLOPE_END,
   KING_PST_RANK4_END,
   KING_PST_RANK4_SLOPE_END,
   KING_PST_RANK5_END,
   KING_PST_RANK5_SLOPE_END,
   KING_PST_RANK6_END,
   KING_PST_RANK6_SLOPE_END,
   KING_PST_RANK7_END,
   KING_PST_RANK7_SLOPE_END,
   KING_PST_RANK8_END,
   KING_PST_RANK8_SLOPE_END,
//   SCALING_START,
//   SCALING_SIGMOID_MID,
//   SCALING_SIGMOID_EXP,
   KNIGHT_BACK_MID,
   KNIGHT_BACK_END,
   KNIGHT_BACK_CORNER_MID,
   KNIGHT_BACK_CORNER_END,
   KNIGHT_ON_RIM_MID,
   KNIGHT_ON_RIM_END,
   KNIGHT_CENTER_OCCUPY_MID,
   KNIGHT_CENTER_OCCUPY_END,
   KNIGHT_CENTER_CONTROL_MID,
   KNIGHT_CENTER_CONTROL_END,
   KNIGHT_FILE_CENTRALITY_MID,
   KNIGHT_FILE_CENTRALITY_END,
   KNIGHT_RANK5_MID,
   KNIGHT_RANK5_END,
   KNIGHT_RANK6_MID,
   KNIGHT_RANK6_END,
   KNIGHT_RANK7_MID,
   KNIGHT_RANK7_END,
   KNIGHT_RANK8_MID,
   KNIGHT_RANK8_END,
   BISHOP_BACK_MID,
   BISHOP_BACK_END,
   BISHOP_BACK_CORNER_MID,
   BISHOP_BACK_CORNER_END,
   BISHOP_DIAG_SIZE_MID,
   BISHOP_DIAG_SIZE_END,
   BISHOP_RANK8_MID,
   BISHOP_RANK8_END,
   KNIGHT_OUTPOST_RANK4,
   KNIGHT_OUTPOST_RANK4_SLOPE,
   KNIGHT_OUTPOST_RANK5,
   KNIGHT_OUTPOST_RANK5_SLOPE,
   KNIGHT_OUTPOST_RANK6,
   KNIGHT_OUTPOST_RANK6_SLOPE,
   KNIGHT_OUTPOST_RANK7,
   KNIGHT_OUTPOST_RANK7_SLOPE,
   KNIGHT_OUTPOST_RANK8,
   KNIGHT_OUTPOST_RANK8_SLOPE,
   BISHOP_OUTPOST_RANK4,
   BISHOP_OUTPOST_RANK4_SLOPE,
   BISHOP_OUTPOST_RANK5,
   BISHOP_OUTPOST_RANK5_SLOPE,
   BISHOP_OUTPOST_RANK6,
   BISHOP_OUTPOST_RANK6_SLOPE,
   BISHOP_OUTPOST_RANK7,
   BISHOP_OUTPOST_RANK7_SLOPE,
   BISHOP_OUTPOST_RANK8,
   BISHOP_OUTPOST_RANK8_SLOPE,
   KNIGHT_MOBILITY0,
   KNIGHT_MOBILITY_MAX,
   KNIGHT_MOBILITY_POW,
   BISHOP_MOBILITY0,
   BISHOP_MOBILITY_MAX,
   BISHOP_MOBILITY_POW,
   ROOK_MOBILITY0_MID,
   ROOK_MOBILITY_MAX_MID,
   ROOK_MOBILITY_POW_MID,
   ROOK_MOBILITY0_END,
   ROOK_MOBILITY_MAX_END,
   ROOK_MOBILITY_POW_END,
   QUEEN_MOBILITY0_MID,
   QUEEN_MOBILITY_MAX_MID,
   QUEEN_MOBILITY_POW_MID,
   QUEEN_MOBILITY0_END,
   QUEEN_MOBILITY_MAX_END,
   QUEEN_MOBILITY_POW_END,
   KING_MOBILITY_ENDGAME0,
   KING_MOBILITY_ENDGAME_MAX,
   KING_MOBILITY_ENDGAME_POW,
   PASSED_PAWN_BASE_MID,
   PASSED_PAWN_SLOPE_MID,
   PASSED_PAWN_POW_SLOPE_MID,
   PASSED_PAWN_POW_MID,
   PASSED_PAWN_BASE_END,
   PASSED_PAWN_SLOPE_END,
   PASSED_PAWN_POW_SLOPE_END,
   PASSED_PAWN_POW_END,
   POTENTIAL_PASSED_PAWN_BASE_MID,
   POTENTIAL_PASSED_PAWN_SLOPE_MID,
   POTENTIAL_PASSED_PAWN_POW_SLOPE_MID,
   POTENTIAL_PASSED_PAWN_POW_MID,
   POTENTIAL_PASSED_PAWN_BASE_END,
   POTENTIAL_PASSED_PAWN_SLOPE_END,
   POTENTIAL_PASSED_PAWN_POW_SLOPE_END,
   POTENTIAL_PASSED_PAWN_POW_END,
   CONNECTED_PASSERS_BASE_MID,
   CONNECTED_PASSERS_MULT_MID,
   CONNECTED_PASSERS_POW_MID,
   CONNECTED_PASSERS_BASE_END,
   CONNECTED_PASSERS_MULT_END,
   CONNECTED_PASSERS_POW_END,
   ADJACENT_PASSERS_BASE_MID,
   ADJACENT_PASSERS_MULT_MID,
   ADJACENT_PASSERS_POW_MID,
   ADJACENT_PASSERS_BASE_END,
   ADJACENT_PASSERS_MULT_END,
   ADJACENT_PASSERS_POW_END,
   DOUBLED_PAWNS_BASE_MID,
   DOUBLED_PAWNS_SLOPE_MID,
   DOUBLED_PAWNS_BASE_END,
   DOUBLED_PAWNS_SLOPE_END,
   ISOLATED_PAWN_BASE_MID,
   ISOLATED_PAWN_SLOPE_MID,
   ISOLATED_PAWN_BASE_END,
   ISOLATED_PAWN_SLOPE_END
};
   
tune::TuneParam tune::tune_params[tune::NUM_TUNING_PARAMS] = {
   tune::TuneParam(RB_ADJUST1,"rb_adjust1",250,-100,400),
   tune::TuneParam(RB_ADJUST2,"rb_adjust2",75,-300,400),
   tune::TuneParam(RB_ADJUST3,"rb_adjust3",-75,-400,300),
   tune::TuneParam(RB_ADJUST4,"rb_adjust4",-250,-500,150),
   tune::TuneParam(RBN_ADJUST1,"rbn_adjust1",500,250,750),
   tune::TuneParam(RBN_ADJUST2,"rbn_adjust2",625,300,900),
   tune::TuneParam(RBN_ADJUST3,"rbn_adjust3",875,500,1200),
   tune::TuneParam(RBN_ADJUST4,"rbn_adjust4",1000,500,1500),
   tune::TuneParam(QR_ADJUST0,"qr_adjust0",-500,-750,-250),
   tune::TuneParam(QR_ADJUST1,"qr_adjust1",0,-500,500),
   tune::TuneParam(QR_ADJUST2,"qr_adjust2",500,250,750),
   tune::TuneParam(QR_ADJUST3,"qr_adjust3",500,250,750),
   tune::TuneParam(KN_VS_PAWN_ADJUST0,"kn_vs_pawn_adjust0",0,-250,250),
   tune::TuneParam(KN_VS_PAWN_ADJUST1,"kn_vs_pawn_adjust1",-2400,-3600,-1200),
   tune::TuneParam(KN_VS_PAWN_ADJUST2,"kn_vs_pawn_adjust2",-1500,-2000,-1000),
   tune::TuneParam(PAWN_TRADE0,"pawn_trade0",-450,-900,0),
   tune::TuneParam(PAWN_TRADE1,"pawn_trade1",-250,-500,0),
   tune::TuneParam(PAWN_TRADE2,"pawn_trade2",-100,-250,0),
   tune::TuneParam(CASTLING0,"castling0",0,-100,100),
   tune::TuneParam(CASTLING1,"castling1",-70,-300,0),
   tune::TuneParam(CASTLING2,"castling2",-100,-300,0),
   tune::TuneParam(CASTLING3,"castling3",280,0,500),
   tune::TuneParam(CASTLING4,"castling4",200,0,500),
   tune::TuneParam(CASTLING5,"castling5",-280,-500,0),
   tune::TuneParam(KING_COVER0,"king_cover0",363,100,320),
   tune::TuneParam(KING_COVER1,"king_cover1",227,100,450),
   tune::TuneParam(KING_COVER2,"king_cover2",140,50,200),
   tune::TuneParam(KING_COVER3,"king_cover3",23,0,100),
   tune::TuneParam(KING_COVER4,"king_cover4",45,0,100),
   tune::TuneParam(KING_FILE_OPEN,"king_file_open",-108,-300,0),
   tune::TuneParam(KING_DISTANCE_BASIS,"king_distance_basis",312,200,400),
   tune::TuneParam(KING_DISTANCE_MULT,"king_distance_mult",89,60,120),
   tune::TuneParam(PIN_MULTIPLIER_MID,"pin_multiplier_mid",441,100,600),
   tune::TuneParam(PIN_MULTIPLIER_END,"pin_multiplier_end",416,100,500),
   tune::TuneParam(KING_ATTACK_PARAM1,"king_attack_param1",493,0,1000),
   tune::TuneParam(KING_ATTACK_PARAM2,"king_attack_param2",243,0,640),
   tune::TuneParam(KING_ATTACK_PARAM3,"king_attack_param3",1922,0,3000),
   tune::TuneParam(KING_ATTACK_BOOST_THRESHOLD,"king_attack_boost_threshold",190,100,960),
   tune::TuneParam(KING_ATTACK_BOOST_DIVISOR,"king_attack_boost_divisor",432,100,1000),
   tune::TuneParam(BISHOP_TRAPPED,"bishop_trapped",-218,-2000,-400),
   tune::TuneParam(BISHOP_PAIR_MID,"bishop_pair_mid",352,100,600),
   tune::TuneParam(BISHOP_PAIR_END,"bishop_pair_end",782,125,750),
//   tune::TuneParam("bishop_pawn_placement_mid",-1,-200,0),
   tune::TuneParam(BISHOP_PAWN_PLACEMENT_END,"bishop_pawn_placement_end",-149,-250,0),
   tune::TuneParam(BAD_BISHOP_MID,"bad_bishop_mid",-66,-80,0),
   tune::TuneParam(BAD_BISHOP_END,"bad_bishop_end",-73,-120,0),
   tune::TuneParam(OUTPOST_NOT_DEFENDED,"outpost_not_defended",46,0,64),
   tune::TuneParam(CENTER_PAWN_BLOCK,"center_pawn_block",-55,-300,0),
   tune::TuneParam(OUTSIDE_PASSER_MID,"outside_passer_mid",113,0,250),
   tune::TuneParam(OUTSIDE_PASSER_END,"outside_passer_end",257,0,500),
   tune::TuneParam(WEAK_PAWN_MID,"weak_pawn_mid",-80,-250,0),
   tune::TuneParam(WEAK_PAWN_END,"weak_pawn_end",-80,-250,0),
   tune::TuneParam(WEAK_ON_OPEN_FILE_MID,"weak_on_open_file_mid",-100,-250,0),
   tune::TuneParam(WEAK_ON_OPEN_FILE_END,"weak_on_open_file_end",-100,-250,0),
   tune::TuneParam(SPACE,"space",20,0,80),
   tune::TuneParam(PAWN_CENTER_SCORE_MID,"pawn_center_score_mid",30,0,100),
   tune::TuneParam(ROOK_ON_7TH_MID,"rook_on_7th_mid",260,0,800),
   tune::TuneParam(ROOK_ON_7TH_END,"rook_on_7th_end",260,0,800),
   tune::TuneParam(TWO_ROOKS_ON_7TH_MID,"two_rooks_on_7th_mid",570,0,1200),
   tune::TuneParam(TWO_ROOKS_ON_7TH_END,"two_rooks_on_7th_end",660,0,1200),
   tune::TuneParam(ROOK_ON_OPEN_FILE_MID,"rook_on_open_file_mid",200,0,600),
   tune::TuneParam(ROOK_ON_OPEN_FILE_END,"rook_on_open_file_end",0,0,600),
   tune::TuneParam(ROOK_ATTACKS_WEAK_PAWN_MID,"rook_attacks_weak_pawn_mid",100,0,600),
   tune::TuneParam(ROOK_ATTACKS_WEAK_PAWN_END,"rook_attacks_weak_pawn_end",100,0,600),
   tune::TuneParam(ROOK_BEHIND_PP_MID,"rook_behind_pp_mid",50,0,600),
   tune::TuneParam(ROOK_BEHIND_PP_END,"rook_behind_pp_end",100,0,600),
   tune::TuneParam(QUEEN_OUT,"queen_out",-60,-200,0),
   tune::TuneParam(PASSER_OWN_PIECE_BLOCK_MID,"passer_own_piece_block_mid",-20,-200,0),
   tune::TuneParam(PASSER_OWN_PIECE_BLOCK_END,"passer_own_piece_block_end",-20,-200,0 ),
   tune::TuneParam(PP_BLOCK_BASE_MID,"pp_block_base_mid",204,0,280),
   tune::TuneParam(PP_BLOCK_BASE_END,"pp_block_base_end",176,0,280),
   tune::TuneParam(PP_BLOCK_MULT_MID,"pp_block_mult_mid",16,0,180),
   tune::TuneParam(PP_BLOCK_MULT_END,"pp_block_mult_end",3,0,80),
   tune::TuneParam(ENDGAME_PAWN_BONUS,"endgame_pawn_bonus",120,0,250),
   tune::TuneParam(KING_NEAR_PASSER,"king_near_passer",200,0,500),
   tune::TuneParam(OPP_KING_NEAR_PASSER,"opp_king_near_passer",-280,-500,0),
   tune::TuneParam(PAWN_SIDE_BONUS,"pawn_side_bonus",280,0,500),
   tune::TuneParam(SUPPORTED_PASSER6,"supported_passer6",380,0,750),
   tune::TuneParam(SUPPORTED_PASSER7,"supported_passer7",760,0,1500),
   tune::TuneParam(SIDE_PROTECTED_PAWN,"side_protected_pawn",-100,-500,0),
   tune::TuneParam(TRADE_DOWN_LINEAR,"trade_down_linear",40,0,150),
   tune::TuneParam(TRADE_DOWN_SQ,"trade_down_sq",0,0,150),
   tune::TuneParam(KING_PST_RANK1_MID,"king_pst_rank1_mid",50,0,250),
   tune::TuneParam(KING_PST_RANK1_SLOPE_MID,"king_pst_rank1_slope_mid",0,-50,50),
   tune::TuneParam(KING_PST_RANK2_MID,"king_pst_rank2_mid",-200,-500,100),
   tune::TuneParam(KING_PST_RANK2_SLOPE_MID,"king_pst_rank2_slope_mid",0,-100,100),
   tune::TuneParam(KING_PST_RANK3_MID,"king_pst_rank3_mid",-300,-600,0),
   tune::TuneParam(KING_PST_RANK3_SLOPE_MID,"king_pst_rank3_slope_mid",0,-100,100),
   tune::TuneParam(KING_PST_RANK4_MID,"king_pst_rank4_mid",-500,-1000,0),
   tune::TuneParam(KING_PST_RANK4_SLOPE_MID,"king_pst_rank4_slope_mid",0,-100,100),
   tune::TuneParam(KING_PST_RANK5_MID,"king_pst_rank5_mid",-500,-1500,0),
   tune::TuneParam(KING_PST_RANK5_SLOPE_MID,"king_pst_rank5_slope_mid",0,-100,100),
   tune::TuneParam(KING_PST_RANK6_MID,"king_pst_rank6_mid",-500,-1500,0),
   tune::TuneParam(KING_PST_RANK6_SLOPE_MID,"king_pst_rank6_slope_mid",0,-100,100),
   tune::TuneParam(KING_PST_RANK7_MID,"king_pst_rank7_mid",-500,-1500,0),
   tune::TuneParam(KING_PST_RANK7_SLOPE_MID,"king_pst_rank7_slope_mid",0,-100,100),
   tune::TuneParam(KING_PST_RANK8_MID,"king_pst_rank8_mid",-500,-1500,0),
   tune::TuneParam(KING_PST_RANK8_SLOPE_MID,"king_pst_rank8_slope_mid",0,-100,100),
   tune::TuneParam(KING_PST_RANK1_END,"king_pst_rank1_end",-280,-500,500),
   tune::TuneParam(KING_PST_RANK1_SLOPE_END,"king_pst_rank1_slope_end",50,-100,100),
   tune::TuneParam(KING_PST_RANK2_END,"king_pst_rank2_end",-220,-500,250),
   tune::TuneParam(KING_PST_RANK2_SLOPE_END,"king_pst_rank2_slope_end",50,-100,100),
   tune::TuneParam(KING_PST_RANK3_END,"king_pst_rank3_end",-160,-500,250),
   tune::TuneParam(KING_PST_RANK3_SLOPE_END,"king_pst_rank3_slope_end",50,-100,100),
   tune::TuneParam(KING_PST_RANK4_END,"king_pst_rank4_end",-100,-500,250),
   tune::TuneParam(KING_PST_RANK4_SLOPE_END,"king_pst_rank4_slope_end",50,-100,100),
   tune::TuneParam(KING_PST_RANK5_END,"king_pst_rank5_end",-40,-500,250),
   tune::TuneParam(KING_PST_RANK5_SLOPE_END,"king_pst_rank5_slope_end",50,-100,100),
   tune::TuneParam(KING_PST_RANK6_END,"king_pst_rank6_end",20,-500,250),
   tune::TuneParam(KING_PST_RANK6_SLOPE_END,"king_pst_rank6_slope_end",50,-100,100),
   tune::TuneParam(KING_PST_RANK7_END,"king_pst_rank7_end",80,-500,300),
   tune::TuneParam(KING_PST_RANK7_SLOPE_END,"king_pst_rank7_slope_end",50,-100,100),
   tune::TuneParam(KING_PST_RANK8_END,"king_pst_rank8_end",80,-500,300),
   tune::TuneParam(KING_PST_RANK8_SLOPE_END,"king_pst_rank8_slope_end",50,-100,100),
//   tune::TuneParam(SCALING_START,"scaling_start",8,0,16),
//   tune::TuneParam(SCALING_SIGMOID_MID,"sigmoid_mid",50,25,75),
//   tune::TuneParam(SCALING_SIGMOID_EXP,"sigmoid_exp",372,50,1000),
   tune::TuneParam(KNIGHT_BACK_MID,"knight_back_mid",-272,-300,-50),
   tune::TuneParam(KNIGHT_BACK_END,"knight_back_end",-241,-300,-50),
   tune::TuneParam(KNIGHT_BACK_CORNER_MID,"knight_back_corner_mid",-181,-350,0),
   tune::TuneParam(KNIGHT_BACK_CORNER_END,"knight_back_corner_end",-76,-350,0),
   tune::TuneParam(KNIGHT_ON_RIM_MID,"knight_on_rim_mid",-204,-250,0),
   tune::TuneParam(KNIGHT_ON_RIM_END,"knight_on_rim_end",-94,-250,0),
   tune::TuneParam(KNIGHT_CENTER_OCCUPY_MID,"knight_center_occupy_mid",263,0,250),
   tune::TuneParam(KNIGHT_CENTER_OCCUPY_END,"knight_center_occupy_end",41,0,250),
   tune::TuneParam(KNIGHT_CENTER_CONTROL_MID,"knight_center_control_mid",66,0,250),
   tune::TuneParam(KNIGHT_CENTER_CONTROL_END,"knight_center_control_end",161,0,250),
   tune::TuneParam(KNIGHT_FILE_CENTRALITY_MID,"knight_file_centrality_mid",-24,-50,100),
   tune::TuneParam(KNIGHT_FILE_CENTRALITY_END,"knight_file_centrality_end",146,0,100),
   tune::TuneParam(KNIGHT_RANK5_MID,"knight_rank5_mid",65,0,100),
   tune::TuneParam(KNIGHT_RANK5_END,"knight_rank5_end",17,-50,100),
   tune::TuneParam(KNIGHT_RANK6_MID,"knight_rank6_mid",-5,-50,100),
   tune::TuneParam(KNIGHT_RANK6_END,"knight_rank6_end",27,0,100),
   tune::TuneParam(KNIGHT_RANK7_MID,"knight_rank7_mid",-11,-100,100),
   tune::TuneParam(KNIGHT_RANK7_END,"knight_rank7_end",-18,-100,100),
   tune::TuneParam(KNIGHT_RANK8_MID,"knight_rank8_mid",-31,-100,100),
   tune::TuneParam(KNIGHT_RANK8_END,"knight_rank8_end",-19,-100,100),
   tune::TuneParam(BISHOP_BACK_MID,"bishop_back_mid",-120,-250,0),
   tune::TuneParam(BISHOP_BACK_END,"bishop_back_end",-112,-250,0),
   tune::TuneParam(BISHOP_BACK_CORNER_MID,"bishop_back_corner_mid",-166,-350,0),
   tune::TuneParam(BISHOP_BACK_CORNER_END,"bishop_back_corner_end",-136,-350,0),
   tune::TuneParam(BISHOP_DIAG_SIZE_MID,"bishop_diag_size_mid",10,0,50),
   tune::TuneParam(BISHOP_DIAG_SIZE_END,"bishop_diag_size_end",10,0,50),
   tune::TuneParam(BISHOP_RANK8_MID,"bishop_rank8_mid",-88,-250,100),
   tune::TuneParam(BISHOP_RANK8_END,"bishop_rank8_end",-166,-250,100),
   tune::TuneParam(KNIGHT_OUTPOST_RANK4,"knight_outpost_rank4",200,-500,500),
   tune::TuneParam(KNIGHT_OUTPOST_RANK4_SLOPE,"knight_outpost_rank4_slope",0,-250,250),
   tune::TuneParam(KNIGHT_OUTPOST_RANK5,"knight_outpost_rank5",300,-500,500),
   tune::TuneParam(KNIGHT_OUTPOST_RANK5_SLOPE,"knight_outpost_rank5_slope",0,-500,250),
   tune::TuneParam(KNIGHT_OUTPOST_RANK6,"knight_outpost_rank6",300,-500,500),
   tune::TuneParam(KNIGHT_OUTPOST_RANK6_SLOPE,"knight_outpost_rank6_scope",0,-250,250),
   tune::TuneParam(KNIGHT_OUTPOST_RANK7,"knight_outpost_rank7",100,-500,500),
   tune::TuneParam(KNIGHT_OUTPOST_RANK7_SLOPE,"knight_outpost_rank7_slope",0,-250,250),
   tune::TuneParam(KNIGHT_OUTPOST_RANK8,"knight_outpost_rank8",100,-500,500),
   tune::TuneParam(KNIGHT_OUTPOST_RANK8_SLOPE,"knight_outpost_rank8_scope",0,-250,250),
   tune::TuneParam(BISHOP_OUTPOST_RANK4,"bishop_outpost_rank4",200,-500,500),
   tune::TuneParam(BISHOP_OUTPOST_RANK4_SLOPE,"bishop_outpost_rank4_slope",0,-250,250),
   tune::TuneParam(BISHOP_OUTPOST_RANK5,"bishop_outpost_rank5",300,-500,500),
   tune::TuneParam(BISHOP_OUTPOST_RANK5_SLOPE,"bishop_outpost_rank5_slope",0,-500,250),
   tune::TuneParam(BISHOP_OUTPOST_RANK6,"bishop_outpost_rank6",300,-500,500),
   tune::TuneParam(BISHOP_OUTPOST_RANK6_SLOPE,"bishop_outpost_rank6_slope",0,-250,250),
   tune::TuneParam(BISHOP_OUTPOST_RANK7,"bishop_outpost_rank7",100,-500,500),
   tune::TuneParam(BISHOP_OUTPOST_RANK7_SLOPE,"bishop_outpost_rank7_slope",0,-250,250),
   tune::TuneParam(BISHOP_OUTPOST_RANK8,"bishop_outpost_rank8",100,-500,500),
   tune::TuneParam(BISHOP_OUTPOST_RANK8_SLOPE,"bishop_outpost_rank8_slope",0,-250,250),
   tune::TuneParam(KNIGHT_MOBILITY0,"knight_mobility0",-180,-800,-50),
   tune::TuneParam(KNIGHT_MOBILITY_MAX,"knight_mobility_max",120,-25,250),
   tune::TuneParam(KNIGHT_MOBILITY_POW,"knight_mobility_pow",75,48,120),
   tune::TuneParam(BISHOP_MOBILITY0,"bishop_mobility0",-200,-800,-50),
   tune::TuneParam(BISHOP_MOBILITY_MAX,"bishop_mobility_max",90,-25,250),
   tune::TuneParam(BISHOP_MOBILITY_POW,"bishop_mobility_pow",64,48,96),
   tune::TuneParam(ROOK_MOBILITY0_MID,"rook_mobility0_mid",-220,-800,-50),
   tune::TuneParam(ROOK_MOBILITY_MAX_MID,"rook_mobility_max_mid",240,-25,500),
   tune::TuneParam(ROOK_MOBILITY_POW_MID,"rook_mobility_pow_mid",64,48,96),
   tune::TuneParam(ROOK_MOBILITY0_END,"rook_mobility0_end",-300,-800,-50),
   tune::TuneParam(ROOK_MOBILITY_MAX_END,"rook_mobility_max_mid",340,-25,500),
   tune::TuneParam(ROOK_MOBILITY_POW_END,"rook_mobility_pow_end",64,48,96),
   tune::TuneParam(QUEEN_MOBILITY0_MID,"queen_mobility0_mid",-100,-800,-50),
   tune::TuneParam(QUEEN_MOBILITY_MAX_MID,"queen_mobility_max_mid",50,-25,250),
   tune::TuneParam(QUEEN_MOBILITY_POW_MID,"queen_mobility_pow_mid",64,48,96),
   tune::TuneParam(QUEEN_MOBILITY0_END,"queen_mobility0_end",-120,-600,-50),
   tune::TuneParam(QUEEN_MOBILITY_MAX_END,"queen_mobility_max_end",50,-25,250),
   tune::TuneParam(QUEEN_MOBILITY_POW_END,"queen_mobility_pow_end",64,48,96),
   tune::TuneParam(KING_MOBILITY_ENDGAME0,"king_mobility_endgame0",-300,-800,50),
   tune::TuneParam(KING_MOBILITY_ENDGAME_MAX,"king_mobility_endgame_max",37,-25,250),
   tune::TuneParam(KING_MOBILITY_ENDGAME_POW,"king_mobility_endgame_pow",64,48,96),
   tune::TuneParam(PASSED_PAWN_BASE_MID,"passed_pawn_base_mid",100,0,250),
   tune::TuneParam(PASSED_PAWN_SLOPE_MID,"passed_pawn_slope_mid",100,0,350),
   tune::TuneParam(PASSED_PAWN_POW_SLOPE_MID,"passed_pawn_pow_slope_mid",44,0,250),
   tune::TuneParam(PASSED_PAWN_POW_MID,"passed_pawn_pow_mid",68,48,96),
   tune::TuneParam(PASSED_PAWN_BASE_END,"passed_pawn_base_end",120,0,250),
   tune::TuneParam(PASSED_PAWN_SLOPE_END,"passed_pawn_slope_end",120,0,350),
   tune::TuneParam(PASSED_PAWN_POW_SLOPE_END,"passed_pawn_pow_slope_end",53,0,250),
   tune::TuneParam(PASSED_PAWN_POW_END,"passed_pawn_pow_end",65,48,96),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_BASE_MID,"potential_passed_pawn_base_mid",50,0,250),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_SLOPE_MID,"potential_passed_pawn_slope_mid",50,0,350),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_POW_SLOPE_MID,"potential_passed_pawn_pow_slope_mid",50,0,250),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_POW_MID,"potential_passed_pawn_pow_mid",60,48,96),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_BASE_END,"potential_passed_pawn_base_end",60,0,250),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_SLOPE_END,"potential_passed_pawn_slope_end",60,0,350),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_POW_SLOPE_END,"potential_passed_pawn_pow_slope_end",25,0,250),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_POW_END,"potential_passed_pawn_pow_end",64,48,96),
   tune::TuneParam(CONNECTED_PASSERS_BASE_MID,"connected_passers_base_mid",0,0,200),
   tune::TuneParam(CONNECTED_PASSERS_MULT_MID,"connected_passers_mult_mid",0,50,100),
   tune::TuneParam(CONNECTED_PASSERS_POW_MID,"connected_passers_pow_mid",64,48,96),
   tune::TuneParam(CONNECTED_PASSERS_BASE_END,"connected_passers_base_end",0,0,200),
   tune::TuneParam(CONNECTED_PASSERS_MULT_END,"connected_passers_mult_end",61,0,100),
   tune::TuneParam(CONNECTED_PASSERS_POW_END,"connected_passers_pow_end",64,48,96),
   tune::TuneParam(ADJACENT_PASSERS_BASE_MID,"adjacent_passers_base_mid",0,0,200),
   tune::TuneParam(ADJACENT_PASSERS_MULT_MID,"adjacent_passers_mult_mid",0,30,100),
   tune::TuneParam(ADJACENT_PASSERS_POW_MID,"adjacent_passers_pow_mid",64,48,96),
   tune::TuneParam(ADJACENT_PASSERS_BASE_END,"adjacent_passers_base_end",0,0,200),
   tune::TuneParam(ADJACENT_PASSERS_MULT_END,"adjacent_passers_mult_end",0,30,100),
   tune::TuneParam(ADJACENT_PASSERS_POW_END,"adjacent_passers_pow_end",64,48,96),
   tune::TuneParam(DOUBLED_PAWNS_BASE_MID,"doubled_pawns_base_mid",-60,-200,0),
   tune::TuneParam(DOUBLED_PAWNS_SLOPE_MID,"doubled_pawns_slope_mid",-20,-100,100),
   tune::TuneParam(DOUBLED_PAWNS_BASE_END,"doubled_pawns_base_end",-60,-200,0),
   tune::TuneParam(DOUBLED_PAWNS_SLOPE_END,"doubled_pawns_slope_end",-20,-100,100),
   tune::TuneParam(ISOLATED_PAWN_BASE_MID,"isolated_pawn_base_mid",-80,-200,0),
   tune::TuneParam(ISOLATED_PAWN_SLOPE_MID,"isolated_pawn_slope_mid",0,-100,100),
   tune::TuneParam(ISOLATED_PAWN_BASE_END,"isolated_pawn_base_end",-100,-200,0),
   tune::TuneParam(ISOLATED_PAWN_SLOPE_END,"isolated_pawn_slope_end",0,-100,100)
};

static const int centrality(int file) {
   int f = file <=4 ? 4-file : file-5;
   return 4-f;
}

static int passer_score(int i, int base, int slope, int pow_slope, int power) 
{
   if (i == 0) return 0;
   int pp = base;
   if(i>1) {
      pp += int(slope*(i-2)+pow_slope*pow(double(i-2),double(power)/32.0));
   }
   return pp;
}

static void symmetric_table_init(int *target, int tuning, int start_rank) 
{
   memset((void*)target,'\0',64*sizeof(int));
   for (int rank = start_rank; rank <=8; rank++) {
      int start = tune::tune_params[tuning++].current;
      int slope = tune::tune_params[tuning++].current;
      for (int file = 1; file <= 4; file++ ) {
         Square sq = MakeSquare(file,rank,White);
         target[sq] = start + slope*(file-1);
      }
      for (int file = 5; file <= 8; file++) {
         Square sq = MakeSquare(file,rank,White);
         Square model = MakeSquare((9-file),rank,White);
         target[sq] = target[model];
      }
   }
}

static void pawn_table_init(int *target, int tuning) 
{
   for (int i = 0; i < 4; i++) {
      target[i] = tune::tune_params[tuning].current + i*tune::tune_params[tuning+1].current;
      target[7-i] = target[i];
   }
}

static void mobility_init(int *target, int tuning, int size) 
{
   int span = tune::tune_params[tuning+1].current-tune::tune_params[tuning].current;
   const int base = tune::tune_params[tuning].current;
   double power = tune::tune_params[tuning+2].current/32.0;
   for (int i = 0; i < size; i++) {
      if (i == 0) {
         *target++ = base;
      }
      else {
         double factor = (1.0-pow(1.0-double(i)/size,power));
         *target++ = round(base + factor*span);
      }
   }
}

void tune::checkParams() 
{
   for (int i = 0; i<tune::NUM_TUNING_PARAMS; i++) {
      if (tune::tune_params[i].index != i) 
         cerr << "warning: index mismatch in tune::tune_params at " << tune::tune_params[i].name << endl;
      if (tune::tune_params[i].current < tune::tune_params[i].min_value) {
         cerr << "warning: param " << tune_params[i].name << " has current < min" << endl;
         cerr << "resetting to " << tune::tune_params[i].min_value << endl;
         tune::tune_params[i].current = tune::tune_params[i].min_value;
      }
      if (tune::tune_params[i].current > tune::tune_params[i].max_value) {
         cerr << "warning: param " << tune_params[i].name << " has current > max" << endl;
         cerr << "resetting to " << tune::tune_params[i].max_value << endl;
         tune::tune_params[i].current = tune::tune_params[i].max_value;
      }
      if (tune::tune_params[i].min_value > tune::tune_params[i].max_value) {
         cerr << "warning: param " << tune_params[i].name << " has min>max" << endl;
      }
   }
}

void tune::initParams()
{
   checkParams();

   int *dest = Scoring::Params::RB_ADJUST;
   int i, j = 0;
   for (i = 0; i < 4; i++) {
      *dest++ = tune::tune_params[j++].current;
   }
   dest = Scoring::Params::RBN_ADJUST;
   for (i = 0; i < 4; i++) {
      *dest++ = tune::tune_params[j++].current;
   }
   dest = Scoring::Params::QR_ADJUST;
   for (i = 0; i < 4; i++) {
      *dest++ = tune::tune_params[j++].current;
   }
   dest = Scoring::Params::KN_VS_PAWN_ADJUST;
   for (i = 0; i < 3; i++) {
      *dest++ = tune::tune_params[j++].current;
   }
   dest = Scoring::Params::PAWN_TRADE;
   for (i = 0; i < 3; i++) {
      *dest++ = tune::tune_params[j++].current;
   }
   dest = Scoring::Params::CASTLING;
   for (i = 0; i < 6; i++) {
      *dest++ = tune::tune_params[j++].current;
   }
   dest = Scoring::Params::KING_COVER;
   for (i = 0; i < 5; i++) {
      *dest++ = tune::tune_params[j++].current;
   }

   for (int i = 0; i < 16; i++) {
      int j = 16-i;
      Scoring::Params::TRADE_DOWN[i] = round(PARAM(TRADE_DOWN_LINEAR)*j +
                                             PARAM(TRADE_DOWN_SQ)*j*j/64.0);
   }
/*
   Scoring::Params::ENDGAME_THRESHOLD=32;
   Scoring::Params::MIDGAME_THRESHOLD=0;
   int mid_thresh_set = 0;
   for (int i = 0; i < 32; i++) {
      if (i < 32*PARAM(SCALING_START)/100) {
         Scoring::Params::MATERIAL_SCALE[i] = 0;
      } else {
         double mid = PARAM(SCALING_SIGMOID_MID)/100.0;
         double mid2 = int(mid*(32-PARAM(SCALING_START))+PARAM(SCALING_START));
         int j = i-mid2;
         Scoring::Params::MATERIAL_SCALE[i] = int(0.5 + 128.0*(1.0/(1+exp(-PARAM(SCALING_SIGMOID_EXP)*j/1000.00))));
      }
      if ((128-Scoring::Params::MATERIAL_SCALE[i])>128/6) {
         Scoring::Params::ENDGAME_THRESHOLD=i;
      }
      if (!mid_thresh_set && Scoring::Params::MATERIAL_SCALE[i]>128/6) {
         Scoring::Params::MIDGAME_THRESHOLD=i-1;
         mid_thresh_set++;
      }
   }
*/
   // fixed for now
   const int MATERIAL_SCALE[32] = {0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 4, 5, 8, 11, 16, 22, 30, 40, 52, 64, 76, 88, 98, 106, 112, 117, 120, 123, 124, 126, 126, 127};
   memcpy(Scoring::Params::MATERIAL_SCALE,MATERIAL_SCALE,sizeof(int)*32);
   Scoring::Params::MIDGAME_THRESHOLD = 14;
   Scoring::Params::ENDGAME_THRESHOLD = 23;

   for (Square i = 0; i<64; i++) {
      int rank = Rank(i,White);
      int file = File(i);
      for (int phase = 0; phase < 2; phase++) {
         int score = 0;
         if (rank == 1) {
            score += PARAM(KNIGHT_BACK_MID+phase);
            if (i == chess::A1 || i == chess::H1) {
               score += PARAM(KNIGHT_BACK_CORNER_MID+phase);
            }
         }
         if (rank == 5) {
            score += PARAM(KNIGHT_RANK5_MID+phase);
         }
         if (rank == 6) {
            score += PARAM(KNIGHT_RANK6_MID+phase);
         }
         if (rank == 7) {
            score += PARAM(KNIGHT_RANK7_MID+phase);
         }
         if (rank == 8) {
            score += PARAM(KNIGHT_RANK8_MID+phase);
         }
         if (file == chess::AFILE || File(i) == chess::HFILE) {
            score += PARAM(KNIGHT_ON_RIM_MID+phase);
         } else {
            int centr = centrality(file);
            score += centr*PARAM(KNIGHT_FILE_CENTRALITY_MID+phase);
         }
         if (Attacks::center.isSet(i)) {
            score += PARAM(KNIGHT_CENTER_OCCUPY_MID+phase);
         }
         int control = (Attacks::knight_attacks[i] & Attacks::center).bitCount();
         score += control*PARAM(KNIGHT_CENTER_CONTROL_MID+phase);
         Scoring::Params::KNIGHT_PST[phase][i] = score;
      }
   }
   Bitboard rank67mask(Attacks::rank_mask[6-1] |
                       Attacks::rank_mask[7-1]);
   for (Square i = 0; i < 64; i++) {
      for (int phase = 0; phase < 2; phase++) {
         int score = 0;
         int rank = Rank(i,White);
         int file = File(i);
         if (rank == 1) {
            score += PARAM(BISHOP_BACK_MID+phase);
            if (i == chess::A1 || i == chess::H1) {
               score += PARAM(BISHOP_BACK_CORNER_MID+phase);
            }
         }
         else if (rank == 8) {
            score += PARAM(BISHOP_RANK8_MID+phase);
         }
         else {
            Bitboard atcks(Attacks::diag_mask[i]);
            score += PARAM(BISHOP_DIAG_SIZE_MID+phase)*atcks.bitCount();
         }
         Scoring::Params::BISHOP_PST[phase][i] = score;
      }
   }
   symmetric_table_init(Scoring::Params::KING_PST[Scoring::Midgame],KING_PST_RANK1_MID,1);
   symmetric_table_init(Scoring::Params::KING_PST[Scoring::Endgame],KING_PST_RANK1_END,1);
   symmetric_table_init(Scoring::Params::KNIGHT_OUTPOST,KNIGHT_OUTPOST_RANK4,4);
   symmetric_table_init(Scoring::Params::BISHOP_OUTPOST,BISHOP_OUTPOST_RANK4,4);
   
   mobility_init(Scoring::Params::KNIGHT_MOBILITY,KNIGHT_MOBILITY0,9);
   mobility_init(Scoring::Params::BISHOP_MOBILITY,BISHOP_MOBILITY0,15);
   mobility_init(Scoring::Params::ROOK_MOBILITY[Scoring::Midgame],ROOK_MOBILITY0_MID,15);
   mobility_init(Scoring::Params::ROOK_MOBILITY[Scoring::Endgame],ROOK_MOBILITY0_END,15);
   mobility_init(Scoring::Params::QUEEN_MOBILITY[Scoring::Midgame],QUEEN_MOBILITY0_MID,29);
   mobility_init(Scoring::Params::QUEEN_MOBILITY[Scoring::Endgame],QUEEN_MOBILITY0_END,29);
   mobility_init(Scoring::Params::KING_MOBILITY_ENDGAME,KING_MOBILITY_ENDGAME0,9);

   pawn_table_init(Scoring::Params::DOUBLED_PAWNS[Scoring::Midgame],DOUBLED_PAWNS_BASE_MID);
   pawn_table_init(Scoring::Params::DOUBLED_PAWNS[Scoring::Endgame],DOUBLED_PAWNS_BASE_END);
   pawn_table_init(Scoring::Params::ISOLATED_PAWN[Scoring::Midgame],ISOLATED_PAWN_BASE_MID);
   pawn_table_init(Scoring::Params::ISOLATED_PAWN[Scoring::Endgame],ISOLATED_PAWN_BASE_END);
   
   for (int i = 0; i < 8; i++) {
      Scoring::Params::PASSED_PAWN[Scoring::Midgame][i] = passer_score(i,PARAM(PASSED_PAWN_BASE_MID),
                                               PARAM(PASSED_PAWN_SLOPE_MID),
                                             PARAM(PASSED_PAWN_POW_SLOPE_MID),
                                             PARAM(PASSED_PAWN_POW_MID));
      Scoring::Params::PASSED_PAWN[Scoring::Endgame][i] = passer_score(i,PARAM(PASSED_PAWN_BASE_END),
                                             PARAM(PASSED_PAWN_SLOPE_END),
                                             PARAM(PASSED_PAWN_POW_SLOPE_END),
                                             PARAM(PASSED_PAWN_POW_END));
      Scoring::Params::POTENTIAL_PASSER[Scoring::Midgame][i] = passer_score(i,PARAM(POTENTIAL_PASSED_PAWN_BASE_MID),
                                             PARAM(POTENTIAL_PASSED_PAWN_SLOPE_MID),
                                             PARAM(POTENTIAL_PASSED_PAWN_POW_SLOPE_MID),
                                             PARAM(POTENTIAL_PASSED_PAWN_POW_MID));
      Scoring::Params::POTENTIAL_PASSER[Scoring::Endgame][i] = passer_score(i,PARAM(POTENTIAL_PASSED_PAWN_BASE_END),
                                             PARAM(POTENTIAL_PASSED_PAWN_SLOPE_END),
                                             PARAM(POTENTIAL_PASSED_PAWN_POW_SLOPE_END),
                                             PARAM(POTENTIAL_PASSED_PAWN_POW_END));
      Scoring::Params::CONNECTED_PASSERS[Scoring::Midgame][i] = passer_score(i,PARAM(CONNECTED_PASSERS_BASE_MID),
                                                     0,
                                             PARAM(CONNECTED_PASSERS_MULT_MID),
                                             PARAM(CONNECTED_PASSERS_POW_MID));

      Scoring::Params::CONNECTED_PASSERS[Scoring::Endgame][i] = passer_score(i,PARAM(CONNECTED_PASSERS_BASE_END),
                                                     0,
                                             PARAM(CONNECTED_PASSERS_MULT_END),
                                             PARAM(CONNECTED_PASSERS_POW_END));
      Scoring::Params::ADJACENT_PASSERS[Scoring::Midgame][i] = passer_score(i,PARAM(ADJACENT_PASSERS_BASE_MID),
                                                     0,
                                             PARAM(ADJACENT_PASSERS_MULT_MID),
                                             PARAM(ADJACENT_PASSERS_POW_MID));

      Scoring::Params::ADJACENT_PASSERS[Scoring::Endgame][i] = passer_score(i,PARAM(ADJACENT_PASSERS_BASE_END),
                                                     0,
                                             PARAM(ADJACENT_PASSERS_MULT_END),
                                             PARAM(ADJACENT_PASSERS_POW_END));
   }
}

void tune::writeX0(ostream &o) 
{
   o << "( ";
   for (int i=0; i < tune::NUM_TUNING_PARAMS; i++) {
      o << tune::tune_params[i].current;
      if (i < tune::NUM_TUNING_PARAMS-1) o << ' ';
   }
   o << ")" << endl;
}


void tune::readX0(istream &is) 
{
   int c;
   while (is.good() && (c = is.get()) != '(') ;
   for (int i = 0; is.good() && i < tune::NUM_TUNING_PARAMS; i++) {
      is >> tune::tune_params[i].current;
   }
}



