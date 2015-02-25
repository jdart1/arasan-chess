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
      SCALING_SIGMOID_MID,
      SCALING_SIGMOID_EXP,
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
      BISHOP_CENTER_OCCUPY_MID,
      BISHOP_CENTER_OCCUPY_END,
      BISHOP_CENTER_ATTACK_MID,
      BISHOP_CENTER_ATTACK_END,
      BISHOP_RANK67_ATTACK_MID,
      BISHOP_RANK67_ATTACK_END,
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
      KNIGHT_MOBILITY_MULT,
      KNIGHT_MOBILITY_OFFSET,
      BISHOP_MOBILITY0,
      BISHOP_MOBILITY_MULT,
      BISHOP_MOBILITY_OFFSET,
      ROOK_MOBILITY0,
      ROOK_MOBILITY_MULT,
      ROOK_MOBILITY_OFFSET,
      QUEEN_MOBILITY0_MID,
      QUEEN_MOBILITY_MULT_MID,
      QUEEN_MOBILITY_OFFSET_MID,
      QUEEN_MOBILITY0_END,
      QUEEN_MOBILITY_MULT_END,
      QUEEN_MOBILITY_OFFSET_END,
      KING_MOBILITY_ENDGAME0,
      KING_MOBILITY_ENDGAME_MULT,
      KING_MOBILITY_ENDGAME_OFFSET,
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

tune::TuneParam tune::scoring_params[Scoring::PARAM_ARRAY_SIZE] = {
   tune::TuneParam(Scoring::CASTLING0,"castling0",0,-100,100),
   tune::TuneParam(Scoring::CASTLING1,"castling1",-70,-300,0),
   tune::TuneParam(Scoring::CASTLING2,"castling2",-100,-300,0),
   tune::TuneParam(Scoring::CASTLING3,"castling3",280,0,500),
   tune::TuneParam(Scoring::CASTLING4,"castling4",200,0,500),
   tune::TuneParam(Scoring::CASTLING5,"castling5",-280,-500,0),
   tune::TuneParam(Scoring::KING_COVER0,"king_cover0",363,100,320),
   tune::TuneParam(Scoring::KING_COVER1,"king_cover1",227,100,450),
   tune::TuneParam(Scoring::KING_COVER2,"king_cover2",140,50,200),
   tune::TuneParam(Scoring::KING_COVER3,"king_cover3",23,0,100),
   tune::TuneParam(Scoring::KING_COVER4,"king_cover4",45,0,100),
   tune::TuneParam(Scoring::KING_FILE_OPEN,"king_file_open",-108,-300,0),
   tune::TuneParam(Scoring::KING_DISTANCE_BASIS,"king_distance_basis",312,200,400),
   tune::TuneParam(Scoring::KING_DISTANCE_MULT,"king_distance_mult",89,60,120),
   tune::TuneParam(Scoring::PIN_MULTIPLIER_MID,"pin_multiplier_mid",441,100,600),
   tune::TuneParam(Scoring::PIN_MULTIPLIER_END,"pin_multiplier_end",416,100,500),
   tune::TuneParam(Scoring::KING_ATTACK_PARAM1,"king_attack_param1",493,0,1000),
   tune::TuneParam(Scoring::KING_ATTACK_PARAM2,"king_attack_param2",243,0,640),
   tune::TuneParam(Scoring::KING_ATTACK_PARAM3,"king_attack_param3",1922,0,3000),
   tune::TuneParam(Scoring::KING_ATTACK_BOOST_THRESHOLD,"king_attack_boost_threshold",190,100,960),
   tune::TuneParam(Scoring::KING_ATTACK_BOOST_DIVISOR,"king_attack_boost_divisor",432,100,1000),
   tune::TuneParam(Scoring::BISHOP_TRAPPED,"bishop_trapped",-218,-2000,-400),
   tune::TuneParam(Scoring::BISHOP_PAIR_MID,"bishop_pair_mid",352,100,600),
   tune::TuneParam(Scoring::BISHOP_PAIR_END,"bishop_pair_end",782,125,750),
//   tune::TuneParam(Scoring::"bishop_pawn_placement_mid",-1,-200,0),
   tune::TuneParam(Scoring::BISHOP_PAWN_PLACEMENT_END,"bishop_pawn_placement_end",-149,-250,0),
   tune::TuneParam(Scoring::BAD_BISHOP_MID,"bad_bishop_mid",-66,-80,0),
   tune::TuneParam(Scoring::BAD_BISHOP_END,"bad_bishop_end",-73,-120,0),
   tune::TuneParam(Scoring::OUTPOST_NOT_DEFENDED,"outpost_not_defended",46,0,64),
   tune::TuneParam(Scoring::CENTER_PAWN_BLOCK,"center_pawn_block",-55,-300,0),
   tune::TuneParam(Scoring::OUTSIDE_PASSER_MID,"outside_passer_mid",113,0,250),
   tune::TuneParam(Scoring::OUTSIDE_PASSER_END,"outside_passer_end",257,0,500),
   tune::TuneParam(Scoring::WEAK_PAWN_MID,"weak_pawn_mid",-80,-250,0),
   tune::TuneParam(Scoring::WEAK_PAWN_END,"weak_pawn_end",-80,-250,0),
   tune::TuneParam(Scoring::WEAK_ON_OPEN_FILE_MID,"weak_on_open_file_mid",-100,-250,0),
   tune::TuneParam(Scoring::WEAK_ON_OPEN_FILE_END,"weak_on_open_file_end",-100,-250,0),
   tune::TuneParam(Scoring::SPACE,"space",20,0,80),
   tune::TuneParam(Scoring::PAWN_CENTER_SCORE_MID,"pawn_center_score_mid",30,0,100),
   tune::TuneParam(Scoring::ROOK_ON_7TH_MID,"rook_on_7th_mid",260,0,800),
   tune::TuneParam(Scoring::ROOK_ON_7TH_END,"rook_on_7th_end",260,0,800),
   tune::TuneParam(Scoring::TWO_ROOKS_ON_7TH_MID,"two_rooks_on_7th_mid",570,0,1200),
   tune::TuneParam(Scoring::TWO_ROOKS_ON_7TH_END,"two_rooks_on_7th_end",660,0,1200),
   tune::TuneParam(Scoring::ROOK_ON_OPEN_FILE_MID,"rook_on_open_file_mid",200,0,600),
   tune::TuneParam(Scoring::ROOK_ON_OPEN_FILE_END,"rook_on_open_file_end",0,0,600),
   tune::TuneParam(Scoring::ROOK_ATTACKS_WEAK_PAWN_MID,"rook_attacks_weak_pawn_mid",100,0,600),
   tune::TuneParam(Scoring::ROOK_ATTACKS_WEAK_PAWN_END,"rook_attacks_weak_pawn_end",100,0,600),
   tune::TuneParam(Scoring::ROOK_BEHIND_PP_MID,"rook_attacks_weak_pawn_mid",50,0,600),
   tune::TuneParam(Scoring::ROOK_BEHIND_PP_END,"rook_attacks_weak_pawn_end",100,0,600),
   tune::TuneParam(Scoring::QUEEN_OUT,"queen_out",-60,-200,0),
   tune::TuneParam(Scoring::PASSER_OWN_PIECE_BLOCK_MID,"passer_own_piece_block_mid",-20,-200,0),
   tune::TuneParam(Scoring::PASSER_OWN_PIECE_BLOCK_END,"passer_own_piece_block_end",-20,-200,0 ),
   tune::TuneParam(Scoring::PP_BLOCK_BASE_MID,"pp_block_base_mid",204,0,280),
   tune::TuneParam(Scoring::PP_BLOCK_BASE_END,"pp_block_base_end",176,0,280),
   tune::TuneParam(Scoring::PP_BLOCK_MULT_MID,"pp_block_mult_mid",16,0,180),
   tune::TuneParam(Scoring::PP_BLOCK_MULT_END,"pp_block_mult_end",3,0,80),
   tune::TuneParam(Scoring::ENDGAME_PAWN_BONUS,"endgame_pawn_bonus",-120,-500,0),
   tune::TuneParam(Scoring::KING_NEAR_PASSER,"king_near_passer",200,0,500),
   tune::TuneParam(Scoring::OPP_KING_NEAR_PASSER,"opp_king_near_passer",-280,-500,0),
   tune::TuneParam(Scoring::PAWN_SIDE_BONUS,"pawn_side_bonus",280,0,500),
   tune::TuneParam(Scoring::SUPPORTED_PASSER6,"supported_passer6",380,0,750),
   tune::TuneParam(Scoring::SUPPORTED_PASSER7,"supported_passer7",760,0,1500),
   tune::TuneParam(Scoring::SIDE_PROTECTED_PAWN,"side_protected_pawn",-100,-500,0),
};
   
tune::TuneParam tune::tune_params[tune::NUM_TUNING_PARAMS] = {
   tune::TuneParam(KING_PST_RANK1_MID,"king_pst_rank1_mid",50,-500,500),
   tune::TuneParam(KING_PST_RANK1_SLOPE_MID,"king_post_rank1_slope_mid",0,-250,250),
   tune::TuneParam(KING_PST_RANK2_MID,"king_pst_rank2_mid",-200,-500,250),
   tune::TuneParam(KING_PST_RANK2_SLOPE_MID,"king_pst_rank2_slope_mid",0,-250,250),
   tune::TuneParam(KING_PST_RANK3_MID,"king_pst_rank3_mid",-300,-600,250),
   tune::TuneParam(KING_PST_RANK3_SLOPE_MID,"king_pst_rank3_slope_mid",0,-250,250),
   tune::TuneParam(KING_PST_RANK4_MID,"king_pst_rank4_mid",-500,-1000,250),
   tune::TuneParam(KING_PST_RANK4_SLOPE_MID,"king_pst_rank4_slope_mid",0,-250,250),
   tune::TuneParam(KING_PST_RANK5_MID,"king_pst_rank5_mid",-500,-1500,250),
   tune::TuneParam(KING_PST_RANK5_SLOPE_MID,"king_pst_rank5_slope_mid",0,-500,250),
   tune::TuneParam(KING_PST_RANK6_MID,"king_pst_rank6_mid",-500,-1500,250),
   tune::TuneParam(KING_PST_RANK6_SLOPE_MID,"king_pst_rank6_slope_mid",0,-250,250),
   tune::TuneParam(KING_PST_RANK7_MID,"king_pst_rank7_mid",-500,-1500,250),
   tune::TuneParam(KING_PST_RANK7_SLOPE_MID,"king_pst_rank7_slope_mid",0,-250,250),
   tune::TuneParam(KING_PST_RANK8_MID,"king_pst_rank8_mid",-500,-1500,250),
   tune::TuneParam(KING_PST_RANK8_SLOPE_MID,"king_pst_rank8_slope_mid",0,-250,250),
   tune::TuneParam(KING_PST_RANK1_END,"king_pst_rank1_end",-280,-500,500),
   tune::TuneParam(KING_PST_RANK1_SLOPE_END,"king_post_rank1_slope_end",50,-250,250),
   tune::TuneParam(KING_PST_RANK2_END,"king_pst_rank2_end",-220,-500,250),
   tune::TuneParam(KING_PST_RANK2_SLOPE_END,"king_pst_rank2_slope_end",50,-250,250),
   tune::TuneParam(KING_PST_RANK3_END,"king_pst_rank3_end",-160,-500,250),
   tune::TuneParam(KING_PST_RANK3_SLOPE_END,"king_pst_rank3_slope_end",50,-250,250),
   tune::TuneParam(KING_PST_RANK4_END,"king_pst_rank4_end",-100,-500,250),
   tune::TuneParam(KING_PST_RANK4_SLOPE_END,"king_pst_rank4_slope_end",50,-250,250),
   tune::TuneParam(KING_PST_RANK5_END,"king_pst_rank5_end",-40,-500,250),
   tune::TuneParam(KING_PST_RANK5_SLOPE_END,"king_pst_rank5_slope_end",50,-500,250),
   tune::TuneParam(KING_PST_RANK6_END,"king_pst_rank6_end",20,-500,250),
   tune::TuneParam(KING_PST_RANK6_SLOPE_END,"king_pst_rank6_slope_end",50,-250,250),
   tune::TuneParam(KING_PST_RANK7_END,"king_pst_rank7_end",80,-500,300),
   tune::TuneParam(KING_PST_RANK7_SLOPE_END,"king_pst_rank7_slope_end",50,-250,250),
   tune::TuneParam(KING_PST_RANK8_END,"king_pst_rank8_end",80,-500,300),
   tune::TuneParam(KING_PST_RANK8_SLOPE_END,"king_pst_rank8_slope_end",50,-250,250),
   tune::TuneParam(SCALING_SIGMOID_MID,"sigmoid_mid",13,0,32),
   tune::TuneParam(SCALING_SIGMOID_EXP,"sigmoid_exp",372,50,1000),
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
   tune::TuneParam(BISHOP_CENTER_OCCUPY_MID,"bishop_center_occupy_mid",69,0,250),
   tune::TuneParam(BISHOP_CENTER_OCCUPY_END,"bishop_center_occupy_end",58,0,250),
   tune::TuneParam(BISHOP_CENTER_ATTACK_MID,"bishop_center_attack_mid",203,0,250),
   tune::TuneParam(BISHOP_CENTER_ATTACK_END,"bishop_center_attack_end",92,0,250),
   tune::TuneParam(BISHOP_RANK67_ATTACK_MID,"bishop_rank67_attack_mid",42,0,100),
   tune::TuneParam(BISHOP_RANK67_ATTACK_END,"bishop_rank67_attack_end",22,0,100),
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
   tune::TuneParam(KNIGHT_MOBILITY0,"knight_mobility0",-313,-800,-100),
   tune::TuneParam(KNIGHT_MOBILITY_MULT,"knight_mobility_mult",37,0,45),
   tune::TuneParam(KNIGHT_MOBILITY_OFFSET,"knight_mobility_offset",-63,-100,50),
   tune::TuneParam(BISHOP_MOBILITY0,"bishop_mobility0",-502,-800,-100),
   tune::TuneParam(BISHOP_MOBILITY_MULT,"bishop_mobility_mult",17,0,40),
   tune::TuneParam(BISHOP_MOBILITY_OFFSET,"bishop_mobility_offset",-37,-100,50),
   tune::TuneParam(ROOK_MOBILITY0,"rook_mobility0",-502,-800,-100),
   tune::TuneParam(ROOK_MOBILITY_MULT,"rook_mobility_mult",17,0,40),
   tune::TuneParam(ROOK_MOBILITY_OFFSET,"rook_mobility_offset",-37,-100,50),
   tune::TuneParam(QUEEN_MOBILITY0_MID,"queen_mobility0_mid",-502,-800,-100),
   tune::TuneParam(QUEEN_MOBILITY_MULT_MID,"queen_mobility_mult_mid",17,0,40),
   tune::TuneParam(QUEEN_MOBILITY_OFFSET_MID,"queen_mobility_offset_mid",-37,-100,100),
   tune::TuneParam(QUEEN_MOBILITY0_END,"queen_mobility0_end",-100,-600,-100),
   tune::TuneParam(QUEEN_MOBILITY_MULT_END,"queen_mobility_mult_end",17,0,40),
   tune::TuneParam(QUEEN_MOBILITY_OFFSET_END,"queen_mobility_offset_end",-37,-100,100),
   tune::TuneParam(KING_MOBILITY_ENDGAME0,"king_mobility_endgame0",-300,-800,50),
   tune::TuneParam(KING_MOBILITY_ENDGAME_MULT,"king_mobility_endgame_mult",37,0,50),
   tune::TuneParam(KING_MOBILITY_ENDGAME_OFFSET,"king_mobility_endgame_offset",0,-100,100),
   tune::TuneParam(PASSED_PAWN_BASE_MID,"passed_pawn_base_mid",16,0,250),
   tune::TuneParam(PASSED_PAWN_SLOPE_MID,"passed_pawn_slope_mid",-19,0,350),
   tune::TuneParam(PASSED_PAWN_POW_SLOPE_MID,"passed_pawn_pow_slope_mid",44,0,250),
   tune::TuneParam(PASSED_PAWN_POW_MID,"passed_pawn_pow_mid",68,48,96),
   tune::TuneParam(PASSED_PAWN_BASE_END,"passed_pawn_base_end",15,0,250),
   tune::TuneParam(PASSED_PAWN_SLOPE_END,"passed_pawn_slope_end",107,0,350),
   tune::TuneParam(PASSED_PAWN_POW_SLOPE_END,"passed_pawn_pow_slope_end",53,0,250),
   tune::TuneParam(PASSED_PAWN_POW_END,"passed_pawn_pow_end",65,48,96),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_BASE_MID,"potential_passed_pawn_base_mid",-6,0,250),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_SLOPE_MID,"potential_passed_pawn_slope_mid",38,0,350),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_POW_SLOPE_MID,"potential_passed_pawn_pow_slope_mid",0,0,250),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_POW_MID,"potential_passed_pawn_pow_mid",60,48,96),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_BASE_END,"potential_passed_pawn_base_end",18,0,250),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_SLOPE_END,"potential_passed_pawn_slope_end",47,0,350),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_POW_SLOPE_END,"potential_passed_pawn_pow_slope_end",28,0,250),
   tune::TuneParam(POTENTIAL_PASSED_PAWN_POW_END,"potential_passed_pawn_pow_end",75,48,96),
   tune::TuneParam(CONNECTED_PASSERS_BASE_MID,"connected_passers_base_mid",0,0,200),
   tune::TuneParam(CONNECTED_PASSERS_MULT_MID,"connected_passers_mult_mid",0,50,100),
   tune::TuneParam(CONNECTED_PASSERS_POW_MID,"connected_passers_pow_mid",64,48,96),
   tune::TuneParam(CONNECTED_PASSERS_BASE_END,"connected_passers_base_end",0,0,200),
   tune::TuneParam(CONNECTED_PASSERS_MULT_END,"connected_passers_mult_end",0,50,100),
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
      target[i] = tune::tune_params[tuning].current + tune::tune_params[tuning+1].current;
      target[7-i] = target[i];
   }
}

static void mobility_init(int *target, int tuning, int size) 
{
   for (int i = 0; i < size; i++) {
      if (i == 0) {
         *target++ = tune::tune_params[tuning].current;
      }
      else {
         *target++ = round(log(i)*tune::tune_params[tuning+1].current + tune::tune_params[tuning+2].current);
      }
   }
}

void tune::initParams()
{
//#ifdef _DEBUG
   for (int i = 0; i< Scoring::PARAM_ARRAY_SIZE; i++) {
      if (tune::scoring_params[i].index != i) 
         cerr << "warning: index mismatch in tune::scoring_params at " << tune::scoring_params[i].name << endl;
   }
   for (int i = 0; i<tune::NUM_TUNING_PARAMS; i++) {
      if (tune::tune_params[i].index != i) 
         cerr << "warning: index mismatch in tune::tune_params at " << tune::tune_params[i].name << endl;
   }
//#endif

   memset((void*)&Scoring::params,'\0',sizeof(Scoring::Params));
   
   for (int i = 0; i < Scoring::PARAM_ARRAY_SIZE; i++) {
      Scoring::params.params[i] = tune::scoring_params[i].current;
   }
   
   Scoring::params.ENDGAME_THRESHOLD=32;
   Scoring::params.MIDGAME_THRESHOLD=0;
   int mid_thresh_set = 0;
   for (int i = 0; i < 32; i++) {
      Scoring::params.MATERIAL_SCALE[i] = int(0.5 + 128.0*(1.0/(1+exp(-PARAM(SCALING_SIGMOID_EXP)*(i-PARAM(SCALING_SIGMOID_MID))/1000.0))));
      if ((128-Scoring::params.MATERIAL_SCALE[i])>128/6) {
         Scoring::params.ENDGAME_THRESHOLD=i;
      }
      if (!mid_thresh_set && Scoring::params.MATERIAL_SCALE[i]>128/6) {
         Scoring::params.MIDGAME_THRESHOLD=i-1;
         mid_thresh_set++;
      }
   }

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
         Scoring::params.KNIGHT_PST[phase][i] = score;
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
         if (rank == 8) {
            score += PARAM(BISHOP_RANK8_MID+phase);
         }
         if (Attacks::center.isSet(i)) {
            score += PARAM(BISHOP_CENTER_OCCUPY_MID+phase);
         }
         Bitboard atcks(Attacks::diag_mask[i]);
         if (atcks & Attacks::center) {
            score += PARAM(BISHOP_CENTER_ATTACK_MID+phase);
         }
         if (atcks & rank67mask) {
            score += PARAM(BISHOP_RANK67_ATTACK_MID+phase);
         }
         Scoring::params.BISHOP_PST[phase][i] = score;
      }
   }
   symmetric_table_init(Scoring::params.KING_PST[Scoring::Midgame],KING_PST_RANK1_MID,1);
   symmetric_table_init(Scoring::params.KING_PST[Scoring::Endgame],KING_PST_RANK1_END,1);
   symmetric_table_init(Scoring::params.KNIGHT_OUTPOST,KNIGHT_OUTPOST_RANK4,4);
   symmetric_table_init(Scoring::params.BISHOP_OUTPOST,BISHOP_OUTPOST_RANK4,4);
   
   mobility_init(Scoring::params.KNIGHT_MOBILITY,KNIGHT_MOBILITY0,9);
   mobility_init(Scoring::params.BISHOP_MOBILITY,BISHOP_MOBILITY0,15);
   mobility_init(Scoring::params.ROOK_MOBILITY,ROOK_MOBILITY0,15);
   mobility_init(Scoring::params.QUEEN_MOBILITY[Scoring::Midgame],QUEEN_MOBILITY0_MID,29);
   mobility_init(Scoring::params.QUEEN_MOBILITY[Scoring::Endgame],QUEEN_MOBILITY0_END,29);
   mobility_init(Scoring::params.KING_MOBILITY_ENDGAME,KING_MOBILITY_ENDGAME0,9);

   pawn_table_init(Scoring::params.DOUBLED_PAWNS[Scoring::Midgame],DOUBLED_PAWNS_BASE_MID);
   pawn_table_init(Scoring::params.DOUBLED_PAWNS[Scoring::Endgame],DOUBLED_PAWNS_BASE_END);
   pawn_table_init(Scoring::params.ISOLATED_PAWN[Scoring::Midgame],ISOLATED_PAWN_BASE_MID);
   pawn_table_init(Scoring::params.ISOLATED_PAWN[Scoring::Endgame],ISOLATED_PAWN_BASE_END);
   
   for (int i = 0; i < 8; i++) {
      Scoring::params.PASSED_PAWN[Scoring::Midgame][i] = passer_score(i,PARAM(PASSED_PAWN_BASE_MID),
                                               PARAM(PASSED_PAWN_SLOPE_MID),
                                             PARAM(PASSED_PAWN_POW_SLOPE_MID),
                                             PARAM(PASSED_PAWN_POW_MID));
      Scoring::params.PASSED_PAWN[Scoring::Endgame][i] = passer_score(i,PARAM(PASSED_PAWN_BASE_END),
                                             PARAM(PASSED_PAWN_SLOPE_END),
                                             PARAM(PASSED_PAWN_POW_SLOPE_END),
                                             PARAM(PASSED_PAWN_POW_END));
      Scoring::params.POTENTIAL_PASSER[Scoring::Midgame][i] = passer_score(i,PARAM(POTENTIAL_PASSED_PAWN_BASE_MID),
                                             PARAM(POTENTIAL_PASSED_PAWN_SLOPE_MID),
                                             PARAM(POTENTIAL_PASSED_PAWN_POW_SLOPE_MID),
                                             PARAM(POTENTIAL_PASSED_PAWN_POW_MID));
      Scoring::params.POTENTIAL_PASSER[Scoring::Endgame][i] = passer_score(i,PARAM(POTENTIAL_PASSED_PAWN_BASE_END),
                                             PARAM(POTENTIAL_PASSED_PAWN_SLOPE_END),
                                             PARAM(POTENTIAL_PASSED_PAWN_POW_SLOPE_END),
                                             PARAM(POTENTIAL_PASSED_PAWN_POW_END));
      Scoring::params.CONNECTED_PASSERS[Scoring::Midgame][i] = passer_score(i,PARAM(CONNECTED_PASSERS_BASE_MID),
                                                     0,
                                             PARAM(CONNECTED_PASSERS_MULT_MID),
                                             PARAM(CONNECTED_PASSERS_POW_MID));

      Scoring::params.CONNECTED_PASSERS[Scoring::Endgame][i] = passer_score(i,PARAM(CONNECTED_PASSERS_BASE_END),
                                                     0,
                                             PARAM(CONNECTED_PASSERS_MULT_END),
                                             PARAM(CONNECTED_PASSERS_POW_END));
      Scoring::params.ADJACENT_PASSERS[Scoring::Midgame][i] = passer_score(i,PARAM(ADJACENT_PASSERS_BASE_MID),
                                                     0,
                                             PARAM(ADJACENT_PASSERS_MULT_MID),
                                             PARAM(ADJACENT_PASSERS_POW_MID));

      Scoring::params.ADJACENT_PASSERS[Scoring::Endgame][i] = passer_score(i,PARAM(ADJACENT_PASSERS_BASE_END),
                                                     0,
                                             PARAM(ADJACENT_PASSERS_MULT_END),
                                             PARAM(ADJACENT_PASSERS_POW_END));
   }

}

void tune::writeX0(ostream &o) 
{
   o << "( ";
   int i ;
   for (i = 0; i < Scoring::PARAM_ARRAY_SIZE; i++) {
      o << tune::scoring_params[i].current << ' ';
   }
   int j;
   for (j=0; j < tune::NUM_TUNING_PARAMS; j++) {
      o << tune::tune_params[j].current;
      if (j < tune::NUM_TUNING_PARAMS-1) o << ' ';
   }
   o << ")" << endl;
}


void tune::readX0(istream &is) 
{
   int c;
   while (is.good() && (c = is.get()) != '(') ;
   for (int i = 0; is.good() && i < Scoring::PARAM_ARRAY_SIZE; i++) {
      is >> tune::scoring_params[i].current;
   }
   for (int i = 0; is.good() && i < tune::NUM_TUNING_PARAMS; i++) {
      is >> tune::tune_params[i].current;
   }
}



