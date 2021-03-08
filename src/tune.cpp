// Copyright 2014-2020 by Jon Dart. All Rights Reserved.
#include "tune.h"
#include "chess.h"
#include "attacks.h"
#include "params.h"
#include "scoring.h"

#include <algorithm>
#include <cmath>
#include <cstring>

#define VAL(x) (Params::PAWN_VALUE*x)

static const score_t MOBILITY_RANGE = VAL(0.75);
static const score_t OUTPOST_RANGE = VAL(0.65);
static const score_t PST_RANGE = VAL(1.0);
static const score_t MAT_ADJUST_RANGE = VAL(1.75);
static const score_t THREAT_RANGE = VAL(1.0);
static const score_t ENDGAME_KING_POS_RANGE = VAL(0.75);
static const score_t SPAN_CONTROL_RANGE = VAL(0.75);
static const score_t KING_COVER_RANGE = score_t(0.35*Params::PAWN_VALUE);
static const score_t KING_ATTACK_COVER_BOOST_RANGE = Params::KING_ATTACK_FACTOR_RESOLUTION*30;

// Non-const scoring parameters, modifiable by tuner
score_t Params::PAWN_VALUE_MIDGAME;
score_t Params::PAWN_VALUE_ENDGAME;
score_t Params::KNIGHT_VALUE_MIDGAME;
score_t Params::KNIGHT_VALUE_ENDGAME;
score_t Params::BISHOP_VALUE_MIDGAME;
score_t Params::BISHOP_VALUE_ENDGAME;
score_t Params::ROOK_VALUE_MIDGAME;
score_t Params::ROOK_VALUE_ENDGAME;
score_t Params::QUEEN_VALUE_MIDGAME;
score_t Params::QUEEN_VALUE_ENDGAME;
score_t Params::KN_VS_PAWN_ADJUST[3];
score_t Params::MINOR_FOR_PAWNS_MIDGAME;
score_t Params::MINOR_FOR_PAWNS_ENDGAME;
score_t Params::KING_ATTACK_SCALE_MAX;
score_t Params::KING_ATTACK_SCALE_INFLECT;
score_t Params::KING_ATTACK_SCALE_FACTOR;
score_t Params::KING_ATTACK_SCALE_BIAS;
score_t Params::KING_COVER[6][4];
score_t Params::KING_COVER_BASE;
score_t Params::KING_DISTANCE_BASIS;
score_t Params::KING_DISTANCE_MULT;
score_t Params::PIN_MULTIPLIER_MID;
score_t Params::PIN_MULTIPLIER_END;
score_t Params::RB_ADJUST_MIDGAME;
score_t Params::RB_ADJUST_ENDGAME;
score_t Params::RBN_ADJUST_MIDGAME;
score_t Params::RBN_ADJUST_ENDGAME;
score_t Params::QR_ADJUST_MIDGAME;
score_t Params::QR_ADJUST_ENDGAME;
score_t Params::Q_VS_3MINORS_MIDGAME;
score_t Params::Q_VS_3MINORS_ENDGAME;
score_t Params::KRMINOR_VS_R_NO_PAWNS;
score_t Params::KQMINOR_VS_Q_NO_PAWNS;
score_t Params::TRADE_DOWN1;
score_t Params::TRADE_DOWN2;
score_t Params::TRADE_DOWN3;
score_t Params::PAWN_ATTACK_FACTOR;
score_t Params::MINOR_ATTACK_FACTOR;
score_t Params::MINOR_ATTACK_BOOST;
score_t Params::ROOK_ATTACK_FACTOR;
score_t Params::ROOK_ATTACK_BOOST;
score_t Params::QUEEN_ATTACK_FACTOR;
score_t Params::QUEEN_ATTACK_BOOST;
score_t Params::OWN_PIECE_KING_PROXIMITY_MIN;
score_t Params::OWN_MINOR_KING_PROXIMITY;
score_t Params::OWN_ROOK_KING_PROXIMITY;
score_t Params::OWN_QUEEN_KING_PROXIMITY;
score_t Params::KING_ATTACK_COVER_BOOST_BASE;
score_t Params::KING_ATTACK_COVER_BOOST_SLOPE;
score_t Params::KING_ATTACK_COUNT;
score_t Params::KING_ATTACK_SQUARES;
score_t Params::PAWN_PUSH_THREAT_MID;
score_t Params::PAWN_PUSH_THREAT_END;
score_t Params::ENDGAME_KING_THREAT;
score_t Params::BISHOP_TRAPPED;
score_t Params::BISHOP_PAIR_MID;
score_t Params::BISHOP_PAIR_END;
score_t Params::BISHOP_PAWN_PLACEMENT_END;
score_t Params::BAD_BISHOP_MID;
score_t Params::BAD_BISHOP_END;
score_t Params::CENTER_PAWN_BLOCK;
score_t Params::OUTSIDE_PASSER_MID;
score_t Params::OUTSIDE_PASSER_END;
score_t Params::WEAK_PAWN_MID;
score_t Params::WEAK_PAWN_END;
score_t Params::WEAK_ON_OPEN_FILE_MID;
score_t Params::WEAK_ON_OPEN_FILE_END;
score_t Params::SPACE;
score_t Params::PAWN_CENTER_SCORE_MID;
score_t Params::ROOK_ON_7TH_MID;
score_t Params::ROOK_ON_7TH_END;
score_t Params::TWO_ROOKS_ON_7TH_MID;
score_t Params::TWO_ROOKS_ON_7TH_END;
score_t Params::TRAPPED_ROOK;
score_t Params::TRAPPED_ROOK_NO_CASTLE;
score_t Params::ROOK_ON_OPEN_FILE_MID;
score_t Params::ROOK_ON_OPEN_FILE_END;
score_t Params::ROOK_BEHIND_PP_MID;
score_t Params::ROOK_BEHIND_PP_END;
score_t Params::QUEEN_OUT;
score_t Params::PAWN_SIDE_BONUS;
score_t Params::KING_OWN_PAWN_DISTANCE;
score_t Params::KING_OPP_PAWN_DISTANCE;
score_t Params::SIDE_PROTECTED_PAWN;
score_t Params::KING_OPP_PASSER_DISTANCE[6];
score_t Params::KNIGHT_PST[2][64];
score_t Params::BISHOP_PST[2][64];
score_t Params::ROOK_PST[2][64];
score_t Params::QUEEN_PST[2][64];
score_t Params::KING_PST[2][64];
score_t Params::KING_POSITION_LOW_MATERIAL[3];
score_t Params::KING_ATTACK_SCALE[Params::KING_ATTACK_SCALE_SIZE];
score_t Params::OWN_PIECE_KING_PROXIMITY_MULT[16];
score_t Params::PASSED_PAWN[2][8];
score_t Params::PASSED_PAWN_FILE_ADJUST[4];
score_t Params::CONNECTED_PASSER[2][8];
score_t Params::ADJACENT_PASSER[2][8];
score_t Params::QUEENING_PATH_CLEAR[2][6];
score_t Params::PP_OWN_PIECE_BLOCK[2][3];
score_t Params::PP_OPP_PIECE_BLOCK[2][3];
score_t Params::QUEENING_PATH_CONTROL[2][3];
score_t Params::QUEENING_PATH_OPP_CONTROL[2][3];
score_t Params::DOUBLED_PAWNS[2][8];
score_t Params::TRIPLED_PAWNS[2][8];
score_t Params::ISOLATED_PAWN[2][8];
score_t Params::THREAT_BY_PAWN[2][5];
score_t Params::THREAT_BY_KNIGHT[2][5];
score_t Params::THREAT_BY_BISHOP[2][5];
score_t Params::THREAT_BY_ROOK[2][5];
score_t Params::KNIGHT_OUTPOST[2][2];
score_t Params::BISHOP_OUTPOST[2][2];
score_t Params::KNIGHT_MOBILITY[9];
score_t Params::BISHOP_MOBILITY[15];
score_t Params::ROOK_MOBILITY[2][15];
score_t Params::QUEEN_MOBILITY[2][24];
score_t Params::KING_MOBILITY_ENDGAME[5];
score_t Params::PAWN_STORM[2][4][5];

int Tune::numTuningParams() const
{
   return (int)size();
}

int Tune::paramArraySize() const
{
   return (int)SIDE_PROTECTED_PAWN-(int)KING_COVER_BASE+1;
}

#define PARAM(x) (*this)[x].current

Tune::Tune()
{
    // Tuning params for most parameters (except PSTs, mobility).
    // These are initialized to some reasonable but not optimal values.
    static TuneParam initial_params[Tune::NUM_MISC_PARAMS] = {
        TuneParam(Tune::PAWN_VALUE_MIDGAME,"pawn_value_midgame",Params::PAWN_VALUE,100,200,&Params::PAWN_VALUE_MIDGAME,TuneParam::Midgame,0),
        TuneParam(Tune::PAWN_VALUE_ENDGAME,"pawn_value_endgame",Params::PAWN_VALUE,100,200,&Params::PAWN_VALUE_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::KNIGHT_VALUE_MIDGAME,"knight_value_midgame",score_t(4.0*Params::PAWN_VALUE),score_t(2.75*Params::PAWN_VALUE),score_t(4.5*Params::PAWN_VALUE),&Params::KNIGHT_VALUE_MIDGAME,TuneParam::Midgame,1),
        TuneParam(Tune::KNIGHT_VALUE_ENDGAME,"knight_value_endgame",score_t(4.0*Params::PAWN_VALUE),score_t(2.75*Params::PAWN_VALUE),score_t(4.5*Params::PAWN_VALUE),&Params::KNIGHT_VALUE_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::BISHOP_VALUE_MIDGAME,"bishop_value_midgame",score_t(4.0*Params::PAWN_VALUE),score_t(2.75*Params::PAWN_VALUE),score_t(4.5*Params::PAWN_VALUE),&Params::BISHOP_VALUE_MIDGAME,TuneParam::Midgame,1),
        TuneParam(Tune::BISHOP_VALUE_ENDGAME,"bishop_value_endgame",score_t(4.0*Params::PAWN_VALUE),score_t(2.75*Params::PAWN_VALUE),score_t(4.5*Params::PAWN_VALUE),&Params::BISHOP_VALUE_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::ROOK_VALUE_MIDGAME,"rook_value_midgame",score_t(6.0*Params::PAWN_VALUE),score_t(3.0*Params::PAWN_VALUE),score_t(7.0*Params::PAWN_VALUE),&Params::ROOK_VALUE_MIDGAME,TuneParam::Midgame,1),
        TuneParam(Tune::ROOK_VALUE_ENDGAME,"rook_value_endgame",score_t(6.0*Params::PAWN_VALUE),score_t(3.0*Params::PAWN_VALUE),score_t(7.0*Params::PAWN_VALUE),&Params::ROOK_VALUE_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::QUEEN_VALUE_MIDGAME,"queen_value_midgame",score_t(12.0*Params::PAWN_VALUE),score_t(9.0*Params::PAWN_VALUE),score_t(15.0*Params::PAWN_VALUE),&Params::QUEEN_VALUE_MIDGAME,TuneParam::Midgame,1),
        TuneParam(Tune::QUEEN_VALUE_ENDGAME,"queen_value_endgame",score_t(12.0*Params::PAWN_VALUE),score_t(9.0*Params::PAWN_VALUE),score_t(15.0*Params::PAWN_VALUE),&Params::QUEEN_VALUE_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::KN_VS_PAWN_ADJUST0,"kn_vs_pawn_adjust0",0,VAL(-0.5),VAL(0.5),&Params::KN_VS_PAWN_ADJUST[0],TuneParam::Any,1),
        TuneParam(Tune::KN_VS_PAWN_ADJUST1,"kn_vs_pawn_adjust1",VAL(-1.5),VAL(-2.5),VAL(1.0),&Params::KN_VS_PAWN_ADJUST[1],TuneParam::Any,1),
        TuneParam(Tune::KN_VS_PAWN_ADJUST2,"kn_vs_pawn_adjust2",VAL(0),VAL(-2.5),VAL(1.0),&Params::KN_VS_PAWN_ADJUST[2],TuneParam::Any,1),
        TuneParam(Tune::KING_ATTACK_SCALE_MAX,"king_attack_scale_max",VAL(5.0),VAL(3.5),VAL(6.5),&Params::KING_ATTACK_SCALE_MAX,TuneParam::Midgame,1),
        TuneParam(Tune::KING_ATTACK_SCALE_INFLECT,"king_attack_scale_inflect",80,60,120,&Params::KING_ATTACK_SCALE_INFLECT,TuneParam::Midgame,1),
        TuneParam(Tune::KING_ATTACK_SCALE_FACTOR,"king_attack_scale_factor",60,33,150,&Params::KING_ATTACK_SCALE_FACTOR,TuneParam::Midgame,1),
        TuneParam(Tune::KING_ATTACK_SCALE_BIAS,"king_attack_scale_bias",VAL(-0.048),VAL(-0.2),0,&Params::KING_ATTACK_SCALE_BIAS,TuneParam::Any,0),
        TuneParam(Tune::KING_COVER1,"king_cover1",VAL(0.05),VAL(0),KING_COVER_RANGE/2,nullptr,TuneParam::Midgame,1),
        TuneParam(Tune::KING_COVER2,"king_cover2",VAL(-0.1),-2*KING_COVER_RANGE/3,2*KING_COVER_RANGE/3,nullptr,TuneParam::Midgame,1),
        TuneParam(Tune::KING_COVER3,"king_cover3",VAL(-0.15),-KING_COVER_RANGE,0,nullptr,TuneParam::Midgame,1),
        TuneParam(Tune::KING_COVER4,"king_cover4",VAL(-0.2),-KING_COVER_RANGE,0,nullptr,TuneParam::Midgame,1),
        TuneParam(Tune::KING_FILE_HALF_OPEN,"king_file_half_open",VAL(-0.2),-KING_COVER_RANGE,0,nullptr,TuneParam::Midgame,1),
        TuneParam(Tune::KING_FILE_OPEN,"king_file_open",VAL(-0.285),-KING_COVER_RANGE,0,nullptr,TuneParam::Midgame,1),
        TuneParam(Tune::KING_COVER_FILE_FACTOR0,"king_cover_file_factor0",48,32,128,nullptr,TuneParam::Midgame,1),
        TuneParam(Tune::KING_COVER_FILE_FACTOR1,"king_cover_file_factor1",96,32,128,nullptr,TuneParam::Midgame,1),
        TuneParam(Tune::KING_COVER_FILE_FACTOR2,"king_cover_file_factor2",60,32,128,nullptr,TuneParam::Midgame,1),
        TuneParam(Tune::KING_COVER_FILE_FACTOR3,"king_cover_file_factor3",64,32,128,nullptr,TuneParam::Midgame,0),
        TuneParam(Tune::KING_COVER_BASE,"king_cover_base",VAL(-0.1),-KING_COVER_RANGE,0,&Params::KING_COVER_BASE,TuneParam::Midgame,0),
        TuneParam(Tune::KING_DISTANCE_BASIS,"king_distance_basis",VAL(0.312),VAL(0.2),VAL(0.4),&Params::KING_DISTANCE_BASIS,TuneParam::Endgame,0),
        TuneParam(Tune::KING_DISTANCE_MULT,"king_distance_mult",VAL(0.077),VAL(0.04),VAL(0.12),&Params::KING_DISTANCE_MULT,TuneParam::Endgame,0),
        TuneParam(Tune::PIN_MULTIPLIER_MID,"pin_multiplier_mid",VAL(0.227),0,VAL(0.75),&Params::PIN_MULTIPLIER_MID,TuneParam::Midgame,1),
        TuneParam(Tune::PIN_MULTIPLIER_END,"pin_multiplier_end",VAL(0.289),0,VAL(0.750),&Params::PIN_MULTIPLIER_END,TuneParam::Endgame,1),
        TuneParam(Tune::MINOR_FOR_PAWNS_MIDGAME,"minor_for_pawns_midgame",VAL(1.25),-MAT_ADJUST_RANGE,MAT_ADJUST_RANGE,&Params::MINOR_FOR_PAWNS_MIDGAME,TuneParam::Midgame,1),
        TuneParam(Tune::MINOR_FOR_PAWNS_ENDGAME,"minor_for_pawns_endgame",VAL(0.75),-MAT_ADJUST_RANGE,MAT_ADJUST_RANGE,&Params::MINOR_FOR_PAWNS_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::RB_ADJUST_MIDGAME,"rb_adjust_midgame",VAL(1.25),-MAT_ADJUST_RANGE,MAT_ADJUST_RANGE,&Params::RB_ADJUST_MIDGAME,TuneParam::Midgame,1),
        TuneParam(Tune::RB_ADJUST_ENDGAME,"rb_adjust_endgame",VAL(0.6),-MAT_ADJUST_RANGE,MAT_ADJUST_RANGE,&Params::RB_ADJUST_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::RBN_ADJUST_MIDGAME,"rbn_adjust_midgame",-VAL(0.9),-MAT_ADJUST_RANGE,MAT_ADJUST_RANGE,&Params::RBN_ADJUST_MIDGAME,TuneParam::Midgame,1),
        TuneParam(Tune::RBN_ADJUST_ENDGAME,"rbn_adjust_endgame",VAL(0.4),-MAT_ADJUST_RANGE,MAT_ADJUST_RANGE,&Params::RBN_ADJUST_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::QR_ADJUST_MIDGAME,"qr_adjust_midgame",0,-MAT_ADJUST_RANGE,MAT_ADJUST_RANGE,&Params::QR_ADJUST_MIDGAME,TuneParam::Midgame,1),
        TuneParam(Tune::QR_ADJUST_ENDGAME,"qr_adjust_endgame",VAL(0.5),-MAT_ADJUST_RANGE,MAT_ADJUST_RANGE,&Params::QR_ADJUST_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::Q_VS_3MINORS_MIDGAME,"q_vs_3minors_midgame",-VAL(1.0),-MAT_ADJUST_RANGE-VAL(1.0),0,&Params::Q_VS_3MINORS_MIDGAME,TuneParam::Midgame,1),
        TuneParam(Tune::Q_VS_3MINORS_ENDGAME,"q_vs_3minors_endgame",-VAL(0.5),-MAT_ADJUST_RANGE-VAL(1.0),0,&Params::Q_VS_3MINORS_ENDGAME,TuneParam::Endgame,1),
        TuneParam(Tune::KRMINOR_VS_R_NO_PAWNS,"krminor_vs_r_no_pawns",VAL(-0.5),VAL(-2.0),VAL(0),&Params::KRMINOR_VS_R_NO_PAWNS,TuneParam::Any,1),
        TuneParam(Tune::KQMINOR_VS_Q_NO_PAWNS,"kqminor_vs_q_no_pawns",VAL(-0.5),VAL(-3.0),0,&Params::KQMINOR_VS_Q_NO_PAWNS,TuneParam::Any,1),
        TuneParam(Tune::TRADE_DOWN1,"trade_down1",VAL(0.3),VAL(-0.5),VAL(0.75),&Params::TRADE_DOWN1,TuneParam::Any,1),
        TuneParam(Tune::TRADE_DOWN2,"trade_down2",VAL(0.3),VAL(0),VAL(0.75),&Params::TRADE_DOWN2,TuneParam::Any,1),
        TuneParam(Tune::TRADE_DOWN3,"trade_down3",VAL(0.1),VAL(0),VAL(0.75),&Params::TRADE_DOWN3,TuneParam::Any,1),
        TuneParam(Tune::PAWN_ATTACK_FACTOR,"pawn_attack_factor",8,0,100,&Params::PAWN_ATTACK_FACTOR,TuneParam::Midgame,1),
        TuneParam(Tune::MINOR_ATTACK_FACTOR,"minor_attack_factor",45,20,100,&Params::MINOR_ATTACK_FACTOR,TuneParam::Midgame,1),
        TuneParam(Tune::MINOR_ATTACK_BOOST,"minor_attack_boost",40,0,100,&Params::MINOR_ATTACK_BOOST,TuneParam::Midgame,1),
        TuneParam(Tune::ROOK_ATTACK_FACTOR,"rook_attack_factor",60,20,100,&Params::ROOK_ATTACK_FACTOR,TuneParam::Midgame,1),
        TuneParam(Tune::ROOK_ATTACK_BOOST,"rook_attack_boost",34,0,100,&Params::ROOK_ATTACK_BOOST,TuneParam::Midgame,1),
        TuneParam(Tune::QUEEN_ATTACK_FACTOR,"queen_attack_factor",60,40,100,&Params::QUEEN_ATTACK_FACTOR,TuneParam::Midgame,1),
        TuneParam(Tune::QUEEN_ATTACK_BOOST,"queen_attack_boost",40,0,100,&Params::QUEEN_ATTACK_BOOST,TuneParam::Midgame,1),
        TuneParam(Tune::KING_ATTACK_COVER_BOOST_BASE,"king_attack_cover_boost_base",6,4,30,&Params::KING_ATTACK_COVER_BOOST_BASE,TuneParam::Midgame,1),
        TuneParam(Tune::KING_ATTACK_COVER_BOOST_SLOPE,"king_attack_cover_boost_slope",140,40,300,&Params::KING_ATTACK_COVER_BOOST_SLOPE,TuneParam::Midgame,1),
        TuneParam(Tune::KING_ATTACK_COUNT,"king_attack_count",6,0,60,&Params::KING_ATTACK_COUNT,TuneParam::Midgame,1),
        TuneParam(Tune::KING_ATTACK_SQUARES,"king_attack_squares",6,0,60,&Params::KING_ATTACK_SQUARES,TuneParam::Midgame,1),
        TuneParam(Tune::OWN_PIECE_KING_PROXIMITY_MIN,"own_piece_king_proximity_min",4,0,10,&Params::OWN_PIECE_KING_PROXIMITY_MIN,TuneParam::Any,0),
        TuneParam(Tune::OWN_MINOR_KING_PROXIMITY,"own_minor_king_proximity",VAL(0.7),VAL(0),VAL(1.2),&Params::OWN_MINOR_KING_PROXIMITY,TuneParam::Midgame,1),
        TuneParam(Tune::OWN_ROOK_KING_PROXIMITY,"own_rook_king_proximity",VAL(0.1),VAL(0),VAL(0.5),&Params::OWN_ROOK_KING_PROXIMITY,TuneParam::Midgame,1),
        TuneParam(Tune::OWN_QUEEN_KING_PROXIMITY,"own_queen_king_proximity",VAL(0.1),VAL(0),VAL(0.5),&Params::OWN_QUEEN_KING_PROXIMITY,TuneParam::Midgame,1),
        TuneParam(Tune::PAWN_PUSH_THREAT_MID,"pawn_push_threat_mid",VAL(0.2),0,THREAT_RANGE,&Params::PAWN_PUSH_THREAT_MID,TuneParam::Midgame,1),
        TuneParam(Tune::PAWN_PUSH_THREAT_END,"pawn_push_threat_end",VAL(0.2),0,THREAT_RANGE,&Params::PAWN_PUSH_THREAT_END,TuneParam::Endgame,1),
        TuneParam(Tune::ENDGAME_KING_THREAT,"endgame_king_threat",VAL(0.35),0,THREAT_RANGE,&Params::ENDGAME_KING_THREAT,TuneParam::Endgame,1),
        TuneParam(Tune::BISHOP_TRAPPED,"bishop_trapped",VAL(-1.47),VAL(-2.0),VAL(-0.4),&Params::BISHOP_TRAPPED,TuneParam::Any,1),
        TuneParam(Tune::BISHOP_PAIR_MID,"bishop_pair_mid",VAL(0.447),VAL(0.1),VAL(0.6),&Params::BISHOP_PAIR_MID,TuneParam::Midgame,1),
        TuneParam(Tune::BISHOP_PAIR_END,"bishop_pair_end",VAL(0.577),VAL(0.125),VAL(0.75),&Params::BISHOP_PAIR_END,TuneParam::Endgame,1),
        TuneParam(Tune::BISHOP_PAWN_PLACEMENT_END,"bishop_pawn_placement_end",VAL(-0.17),VAL(-0.25),0,&Params::BISHOP_PAWN_PLACEMENT_END,TuneParam::Endgame,1),
        TuneParam(Tune::BAD_BISHOP_MID,"bad_bishop_mid",VAL(-0.04),VAL(-0.15),VAL(0),&Params::BAD_BISHOP_MID,TuneParam::Midgame,1),
        TuneParam(Tune::BAD_BISHOP_END,"bad_bishop_end",VAL(-0.06),VAL(-0.15),VAL(0),&Params::BAD_BISHOP_END,TuneParam::Endgame,1),
        TuneParam(Tune::CENTER_PAWN_BLOCK,"center_pawn_block",VAL(-0.2),VAL(-0.35),VAL(0),&Params::CENTER_PAWN_BLOCK,TuneParam::Midgame,1),
        TuneParam(Tune::OUTSIDE_PASSER_MID,"outside_passer_mid",VAL(0.11),VAL(0),VAL(0.25),&Params::OUTSIDE_PASSER_MID,TuneParam::Midgame,1),
        TuneParam(Tune::OUTSIDE_PASSER_END,"outside_passer_end",VAL(0.25),VAL(0),VAL(0.5),&Params::OUTSIDE_PASSER_END,TuneParam::Endgame,1),
        TuneParam(Tune::WEAK_PAWN_MID,"weak_pawn_mid",VAL(-0.02),VAL(-0.25),VAL(0),&Params::WEAK_PAWN_MID,TuneParam::Midgame,1),
        TuneParam(Tune::WEAK_PAWN_END,"weak_pawn_end",VAL(-0.02),VAL(-0.25),VAL(0),&Params::WEAK_PAWN_END,TuneParam::Endgame,1),
        TuneParam(Tune::WEAK_ON_OPEN_FILE_MID,"weak_on_open_file_mid",VAL(-0.15),VAL(-0.25),VAL(0),&Params::WEAK_ON_OPEN_FILE_MID,TuneParam::Midgame,1),
        TuneParam(Tune::WEAK_ON_OPEN_FILE_END,"weak_on_open_file_end",VAL(-0.15),VAL(-0.25),VAL(0),&Params::WEAK_ON_OPEN_FILE_END,TuneParam::Endgame,1),
        TuneParam(Tune::SPACE,"space",VAL(0.03),VAL(0),VAL(0.12),&Params::SPACE,TuneParam::Any,1),
        TuneParam(Tune::PAWN_CENTER_SCORE_MID,"pawn_center_score_mid",VAL(0.02),VAL(0),VAL(0.1),&Params::PAWN_CENTER_SCORE_MID,TuneParam::Midgame,1),
        TuneParam(Tune::ROOK_ON_7TH_MID,"rook_on_7th_mid",VAL(0.235),VAL(0),VAL(0.8),&Params::ROOK_ON_7TH_MID,TuneParam::Midgame,1),
        TuneParam(Tune::ROOK_ON_7TH_END,"rook_on_7th_end",VAL(0.25),VAL(0),VAL(0.8),&Params::ROOK_ON_7TH_END,TuneParam::Endgame,1),
        TuneParam(Tune::TWO_ROOKS_ON_7TH_MID,"two_rooks_on_7th_mid",VAL(0.15),VAL(0),VAL(0.8),&Params::TWO_ROOKS_ON_7TH_MID,TuneParam::Midgame,1),
        TuneParam(Tune::TWO_ROOKS_ON_7TH_END,"two_rooks_on_7th_end",VAL(0.4),VAL(0),VAL(0.8),&Params::TWO_ROOKS_ON_7TH_END,TuneParam::Endgame,1),
        TuneParam(Tune::TRAPPED_ROOK,"trapped_rook",-VAL(0.3),-VAL(1.0),VAL(0),&Params::TRAPPED_ROOK,TuneParam::Midgame,1),
        TuneParam(Tune::TRAPPED_ROOK_NO_CASTLE,"trapped_rook_no_castle",-VAL(0.6),-VAL(1.0),VAL(0),&Params::TRAPPED_ROOK_NO_CASTLE,TuneParam::Midgame,1),
        TuneParam(Tune::ROOK_ON_OPEN_FILE_MID,"rook_on_open_file_mid",VAL(0.17),VAL(0),VAL(0.6),&Params::ROOK_ON_OPEN_FILE_MID,TuneParam::Midgame,1),
        TuneParam(Tune::ROOK_ON_OPEN_FILE_END,"rook_on_open_file_end",VAL(0.18),VAL(0),VAL(0.6),&Params::ROOK_ON_OPEN_FILE_END,TuneParam::Endgame,1),
        TuneParam(Tune::ROOK_BEHIND_PP_MID,"rook_behind_pp_mid",VAL(0.025),0,VAL(0.25),&Params::ROOK_BEHIND_PP_MID,TuneParam::Midgame,1),
        TuneParam(Tune::ROOK_BEHIND_PP_END,"rook_behind_pp_end",VAL(0.07),VAL(0),VAL(0.25),&Params::ROOK_BEHIND_PP_END,TuneParam::Endgame,1),
        TuneParam(Tune::QUEEN_OUT,"queen_out",VAL(-0.2),VAL(-0.35),VAL(0),&Params::QUEEN_OUT,TuneParam::Midgame,1),
        TuneParam(Tune::PAWN_SIDE_BONUS,"pawn_side_bonus",VAL(0.125),VAL(0),VAL(0.5),&Params::PAWN_SIDE_BONUS,TuneParam::Endgame,1),
        TuneParam(Tune::KING_OWN_PAWN_DISTANCE,"king_own_pawn_distance",VAL(0.075),VAL(0),VAL(0.5),&Params::KING_OWN_PAWN_DISTANCE,TuneParam::Endgame,1),
        TuneParam(Tune::KING_OPP_PAWN_DISTANCE,"king_opp_pawn_distance",VAL(0.02),VAL(0),VAL(0.5),&Params::KING_OPP_PAWN_DISTANCE,TuneParam::Endgame,1),
        TuneParam(Tune::SIDE_PROTECTED_PAWN,"side_protected_pawn",VAL(-0.05),VAL(-0.25),VAL(0),&Params::SIDE_PROTECTED_PAWN,TuneParam::Endgame,1),
        TuneParam(Tune::KING_POSITION_LOW_MATERIAL0,"king_position_low_material0",250,128,300,&Params::KING_POSITION_LOW_MATERIAL[0],TuneParam::Endgame,1),
        TuneParam(Tune::KING_POSITION_LOW_MATERIAL1,"king_position_low_material1",225,128,300,&Params::KING_POSITION_LOW_MATERIAL[1],TuneParam::Endgame,1),
        TuneParam(Tune::KING_POSITION_LOW_MATERIAL2,"king_position_low_material2",130,128,256,&Params::KING_POSITION_LOW_MATERIAL[2],TuneParam::Endgame,1),
        TuneParam(Tune::PASSED_PAWN_MID2,"passed_pawn_mid2",VAL(0.0),VAL(0),VAL(0.3),&Params::PASSED_PAWN[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::PASSED_PAWN_MID3,"passed_pawn_mid3",VAL(0.0),VAL(0),VAL(0.5),&Params::PASSED_PAWN[0][3],TuneParam::Midgame,1),
        TuneParam(Tune::PASSED_PAWN_MID4,"passed_pawn_mid4",VAL(0.0),VAL(0),VAL(0.5),&Params::PASSED_PAWN[0][4],TuneParam::Midgame,1),
        TuneParam(Tune::PASSED_PAWN_MID5,"passed_pawn_mid5",VAL(0.3),VAL(0),VAL(0.75),&Params::PASSED_PAWN[0][5],TuneParam::Midgame,1),
        TuneParam(Tune::PASSED_PAWN_MID6,"passed_pawn_mid6",VAL(0.8),VAL(0.3),VAL(1.0),&Params::PASSED_PAWN[0][6],TuneParam::Midgame,1),
        TuneParam(Tune::PASSED_PAWN_MID7,"passed_pawn_mid7",VAL(1.25),VAL(0.5),VAL(1.75),&Params::PASSED_PAWN[0][7],TuneParam::Midgame,1),
        TuneParam(Tune::PASSED_PAWN_END2,"passed_pawn_end2",VAL(0.0),VAL(0),VAL(0.3),&Params::PASSED_PAWN[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::PASSED_PAWN_END3,"passed_pawn_end3",VAL(0.0),VAL(0),VAL(0.5),&Params::PASSED_PAWN[1][3],TuneParam::Endgame,1),
        TuneParam(Tune::PASSED_PAWN_END4,"passed_pawn_end4",VAL(0.0),VAL(0),VAL(0.5),&Params::PASSED_PAWN[1][4],TuneParam::Endgame,1),
        TuneParam(Tune::PASSED_PAWN_END5,"passed_pawn_end5",VAL(0.6),VAL(0),VAL(0.75),&Params::PASSED_PAWN[1][5],TuneParam::Endgame,1),
        TuneParam(Tune::PASSED_PAWN_END6,"passed_pawn_end6",VAL(1.1),VAL(0.5),VAL(1.25),&Params::PASSED_PAWN[1][6],TuneParam::Endgame,1),
        TuneParam(Tune::PASSED_PAWN_END7,"passed_pawn_end7",VAL(1.75),VAL(0.5),VAL(2.25),&Params::PASSED_PAWN[1][7],TuneParam::Endgame,1),
        TuneParam(Tune::PASSED_PAWN_FILE_ADJUST1,"passed_pawn_file_adjust1",64,48,96,&Params::PASSED_PAWN_FILE_ADJUST[0],TuneParam::Any,1),
        TuneParam(Tune::PASSED_PAWN_FILE_ADJUST2,"passed_pawn_file_adjust2",70,48,96,&Params::PASSED_PAWN_FILE_ADJUST[1],TuneParam::Any,1),
        TuneParam(Tune::PASSED_PAWN_FILE_ADJUST3,"passed_pawn_file_adjust3",70,48,96,&Params::PASSED_PAWN_FILE_ADJUST[2],TuneParam::Any,1),
        TuneParam(Tune::PASSED_PAWN_FILE_ADJUST4,"passed_pawn_file_adjust4",64,48,80,&Params::PASSED_PAWN_FILE_ADJUST[3],TuneParam::Any,0),
        TuneParam(Tune::CONNECTED_PASSER_MID2,"connected_passer_mid2",VAL(0),VAL(0),VAL(0.25),&Params::CONNECTED_PASSER[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::CONNECTED_PASSER_MID3,"connected_passer_mid3",VAL(0.08),VAL(0),VAL(0.3),&Params::CONNECTED_PASSER[0][3],TuneParam::Midgame,1),
        TuneParam(Tune::CONNECTED_PASSER_MID4,"connected_passer_mid4",VAL(0.3),VAL(0),VAL(0.5),&Params::CONNECTED_PASSER[0][4],TuneParam::Midgame,1),
        TuneParam(Tune::CONNECTED_PASSER_MID5,"connected_passer_mid5",VAL(0.6),VAL(0),VAL(0.8),&Params::CONNECTED_PASSER[0][5],TuneParam::Midgame,1),
        TuneParam(Tune::CONNECTED_PASSER_MID6,"connected_passer_mid6",VAL(1.75),VAL(0.1),VAL(2.5),&Params::CONNECTED_PASSER[0][6],TuneParam::Midgame,1),
        TuneParam(Tune::CONNECTED_PASSER_MID7,"connected_passer_mid7",VAL(2.0),VAL(0.25),VAL(2.5),&Params::CONNECTED_PASSER[0][7],TuneParam::Midgame,1),
        TuneParam(Tune::CONNECTED_PASSER_END2,"connected_passer_end2",VAL(0),VAL(0),VAL(0.25),&Params::CONNECTED_PASSER[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::CONNECTED_PASSER_END3,"connected_passer_end3",VAL(0.08),VAL(0),VAL(0.3),&Params::CONNECTED_PASSER[1][3],TuneParam::Endgame,1),
        TuneParam(Tune::CONNECTED_PASSER_END4,"connected_passer_end4",VAL(0.2),VAL(0),VAL(0.4),&Params::CONNECTED_PASSER[1][4],TuneParam::Endgame,1),
        TuneParam(Tune::CONNECTED_PASSER_END5,"connected_passer_end5",VAL(0.227),VAL(0),VAL(1.0),&Params::CONNECTED_PASSER[1][5],TuneParam::Endgame,1),
        TuneParam(Tune::CONNECTED_PASSER_END6,"connected_passer_end6",VAL(0.5),VAL(0.1),VAL(2.0),&Params::CONNECTED_PASSER[1][6],TuneParam::Endgame,1),
        TuneParam(Tune::CONNECTED_PASSER_END7,"connected_passer_end7",VAL(0.77),VAL(0.25),VAL(2.5),&Params::CONNECTED_PASSER[1][7],TuneParam::Endgame,1),
        TuneParam(Tune::ADJACENT_PASSER_MID2,"adjacent_passer_mid2",VAL(0),VAL(0),VAL(0.25),&Params::ADJACENT_PASSER[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::ADJACENT_PASSER_MID3,"adjacent_passer_mid3",VAL(0.1),VAL(0),VAL(0.3),&Params::ADJACENT_PASSER[0][3],TuneParam::Midgame,1),
        TuneParam(Tune::ADJACENT_PASSER_MID4,"adjacent_passer_mid4",VAL(0.15),VAL(0),VAL(0.4),&Params::ADJACENT_PASSER[0][4],TuneParam::Midgame,1),
        TuneParam(Tune::ADJACENT_PASSER_MID5,"adjacent_passer_mid5",VAL(0.15),VAL(0),VAL(0.5),&Params::ADJACENT_PASSER[0][5],TuneParam::Midgame,1),
        TuneParam(Tune::ADJACENT_PASSER_MID6,"adjacent_passer_mid6",VAL(0.3),VAL(0),VAL(1.0),&Params::ADJACENT_PASSER[0][6],TuneParam::Midgame,1),
        TuneParam(Tune::ADJACENT_PASSER_MID7,"adjacent_passer_mid7",VAL(0.7),VAL(0.15),VAL(1.5),&Params::ADJACENT_PASSER[0][7],TuneParam::Midgame,1),
        TuneParam(Tune::ADJACENT_PASSER_END2,"adjacent_passer_end2",VAL(0),VAL(0),VAL(0.25),&Params::ADJACENT_PASSER[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::ADJACENT_PASSER_END3,"adjacent_passer_end3",VAL(0.1),VAL(0),VAL(0.3),&Params::ADJACENT_PASSER[1][3],TuneParam::Endgame,1),
        TuneParam(Tune::ADJACENT_PASSER_END4,"adjacent_passer_end4",VAL(0.15),VAL(0),VAL(0.4),&Params::ADJACENT_PASSER[1][4],TuneParam::Endgame,1),
        TuneParam(Tune::ADJACENT_PASSER_END5,"adjacent_passer_end5",VAL(0.15),VAL(0),VAL(0.5),&Params::ADJACENT_PASSER[1][5],TuneParam::Endgame,1),
        TuneParam(Tune::ADJACENT_PASSER_END6,"adjacent_passer_end6",VAL(0.3),VAL(0),VAL(1.0),&Params::ADJACENT_PASSER[1][6],TuneParam::Endgame,1),
        TuneParam(Tune::ADJACENT_PASSER_END7,"adjacent_passer_end7",VAL(0.7),VAL(0.15),VAL(1.5),&Params::ADJACENT_PASSER[1][7],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_MID2,"queening_path_clear_mid2",VAL(0.1),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[0][0],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_MID3,"queening_path_clear_mid3",VAL(0.2),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[0][1],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_MID4,"queening_path_clear_mid4",VAL(0.3),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_MID5,"queening_path_clear_mid5",VAL(0.4),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[0][3],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_MID6,"queening_path_clear_mid6",VAL(0.5),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[0][4],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_MID7,"queening_path_clear_mid7",VAL(0.6),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[0][5],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_END2,"queening_path_clear_end2",VAL(0.1),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[1][0],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_END3,"queening_path_clear_end3",VAL(0.2),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[1][1],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_END4,"queening_path_clear_end4",VAL(0.3),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_END5,"queening_path_clear_end5",VAL(0.4),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[1][3],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_END6,"queening_path_clear_end6",VAL(0.5),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[1][4],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_CLEAR_END7,"queening_path_clear_end7",VAL(0.6),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CLEAR[1][5],TuneParam::Endgame,1),
        TuneParam(Tune::PP_OWN_PIECE_BLOCK_MID5,"pp_own_piece_block_mid5",-VAL(0.2),-VAL(1.0),VAL(0),&Params::PP_OWN_PIECE_BLOCK[0][0],TuneParam::Midgame,1),
        TuneParam(Tune::PP_OWN_PIECE_BLOCK_MID6,"pp_own_piece_block_mid6",-VAL(0.3),-VAL(1.0),VAL(0),&Params::PP_OWN_PIECE_BLOCK[0][1],TuneParam::Midgame,1),
        TuneParam(Tune::PP_OWN_PIECE_BLOCK_MID7,"pp_own_piece_block_mid7",-VAL(0.4),-VAL(1.0),VAL(0),&Params::PP_OWN_PIECE_BLOCK[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::PP_OWN_PIECE_BLOCK_END5,"pp_own_piece_block_end5",-VAL(0.2),-VAL(1.0),VAL(0),&Params::PP_OWN_PIECE_BLOCK[1][0],TuneParam::Endgame,1),
        TuneParam(Tune::PP_OWN_PIECE_BLOCK_END6,"pp_own_piece_block_end6",-VAL(0.3),-VAL(1.0),VAL(0),&Params::PP_OWN_PIECE_BLOCK[1][1],TuneParam::Endgame,1),
        TuneParam(Tune::PP_OWN_PIECE_BLOCK_END7,"pp_own_piece_block_end7",-VAL(0.4),-VAL(1.0),VAL(0),&Params::PP_OWN_PIECE_BLOCK[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::PP_OPP_PIECE_BLOCK_MID5,"pp_opp_piece_block_mid5",-VAL(0.2),-VAL(1.0),VAL(0),&Params::PP_OPP_PIECE_BLOCK[0][0],TuneParam::Midgame,1),
        TuneParam(Tune::PP_OPP_PIECE_BLOCK_MID6,"pp_opp_piece_block_mid6",-VAL(0.3),-VAL(1.0),VAL(0),&Params::PP_OPP_PIECE_BLOCK[0][1],TuneParam::Midgame,1),
        TuneParam(Tune::PP_OPP_PIECE_BLOCK_MID7,"pp_opp_piece_block_mid7",-VAL(0.4),-VAL(1.0),VAL(0),&Params::PP_OPP_PIECE_BLOCK[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::PP_OPP_PIECE_BLOCK_END5,"pp_opp_piece_block_end5",-VAL(0.2),-VAL(1.0),VAL(0),&Params::PP_OPP_PIECE_BLOCK[1][0],TuneParam::Endgame,1),
        TuneParam(Tune::PP_OPP_PIECE_BLOCK_END6,"pp_opp_piece_block_end6",-VAL(0.3),-VAL(1.0),VAL(0),&Params::PP_OPP_PIECE_BLOCK[1][1],TuneParam::Endgame,1),
        TuneParam(Tune::PP_OPP_PIECE_BLOCK_END7,"pp_opp_piece_block_end7",-VAL(0.4),-VAL(1.0),VAL(0),&Params::PP_OPP_PIECE_BLOCK[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_CONTROL_MID5,"queening_path_control_mid5",VAL(0.2),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CONTROL[0][0],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_CONTROL_MID6,"queening_path_control_mid6",VAL(0.3),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CONTROL[0][1],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_CONTROL_MID7,"queening_path_control_mid7",VAL(0.4),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CONTROL[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_CONTROL_END5,"queening_path_control_end5",VAL(0.2),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CONTROL[1][0],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_CONTROL_END6,"queening_path_control_end6",VAL(0.3),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CONTROL[1][1],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_CONTROL_END7,"queening_path_control_end7",VAL(0.4),VAL(0),VAL(1.0),&Params::QUEENING_PATH_CONTROL[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_OPP_CONTROL_MID5,"queening_path_opp_control_mid5",-VAL(0.2),-VAL(1.0),VAL(0),&Params::QUEENING_PATH_OPP_CONTROL[0][0],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_OPP_CONTROL_MID6,"queening_path_opp_control_mid6",-VAL(0.3),-VAL(1.0),VAL(0),&Params::QUEENING_PATH_OPP_CONTROL[0][1],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_OPP_CONTROL_MID7,"queening_path_opp_control_mid7",-VAL(0.4),-VAL(1.0),VAL(0),&Params::QUEENING_PATH_OPP_CONTROL[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::QUEENING_PATH_OPP_CONTROL_END5,"queening_path_opp_control_end5",-VAL(0.2),-VAL(1.0),VAL(0),&Params::QUEENING_PATH_OPP_CONTROL[1][0],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_OPP_CONTROL_END6,"queening_path_opp_control_end6",-VAL(0.3),-VAL(1.0),VAL(0),&Params::QUEENING_PATH_OPP_CONTROL[1][1],TuneParam::Endgame,1),
        TuneParam(Tune::QUEENING_PATH_OPP_CONTROL_END7,"queening_path_opp_control_end7",-VAL(0.4),-VAL(1.0),VAL(0),&Params::QUEENING_PATH_OPP_CONTROL[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::DOUBLED_PAWNS_MID1,"doubled_pawns_mid1",VAL(-0.05),VAL(-0.5),VAL(0),&Params::DOUBLED_PAWNS[0][0],TuneParam::Midgame,1),
        TuneParam(Tune::DOUBLED_PAWNS_MID2,"doubled_pawns_mid2",VAL(-0.06),VAL(-0.5),VAL(0),&Params::DOUBLED_PAWNS[0][1],TuneParam::Midgame,1),
        TuneParam(Tune::DOUBLED_PAWNS_MID3,"doubled_pawns_mid3",VAL(-0.08),VAL(-0.5),VAL(0),&Params::DOUBLED_PAWNS[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::DOUBLED_PAWNS_MID4,"doubled_pawns_mid4",VAL(-0.1),VAL(-0.5),VAL(0),&Params::DOUBLED_PAWNS[0][3],TuneParam::Midgame,1),
        TuneParam(Tune::DOUBLED_PAWNS_END1,"doubled_pawns_end1",VAL(-0.05),VAL(-0.5),VAL(0),&Params::DOUBLED_PAWNS[1][0],TuneParam::Endgame,1),
        TuneParam(Tune::DOUBLED_PAWNS_END2,"doubled_pawns_end2",VAL(-0.06),VAL(-0.5),VAL(0),&Params::DOUBLED_PAWNS[1][1],TuneParam::Endgame,1),
        TuneParam(Tune::DOUBLED_PAWNS_END3,"doubled_pawns_end3",VAL(-0.08),VAL(-0.5),VAL(0),&Params::DOUBLED_PAWNS[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::DOUBLED_PAWNS_END4,"doubled_pawns_end4",VAL(-0.1),VAL(-0.5),VAL(0),&Params::DOUBLED_PAWNS[1][3],TuneParam::Endgame,1),
        TuneParam(Tune::TRIPLED_PAWNS_MID1,"tripled_pawns_mid1",VAL(-0.07),VAL(-0.5),VAL(0),&Params::TRIPLED_PAWNS[0][0],TuneParam::Midgame,1),
        TuneParam(Tune::TRIPLED_PAWNS_MID2,"tripled_pawns_mid2",VAL(-0.1),VAL(-0.5),VAL(0),&Params::TRIPLED_PAWNS[0][1],TuneParam::Midgame,1),
        TuneParam(Tune::TRIPLED_PAWNS_MID3,"tripled_pawns_mid3",VAL(-0.16),VAL(-0.5),VAL(0),&Params::TRIPLED_PAWNS[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::TRIPLED_PAWNS_MID4,"tripled_pawns_mid4",VAL(-0.2),VAL(-0.5),VAL(0),&Params::TRIPLED_PAWNS[0][3],TuneParam::Midgame,1),
        TuneParam(Tune::TRIPLED_PAWNS_END1,"tripled_pawns_end1",VAL(-0.25),VAL(-0.75),VAL(0),&Params::TRIPLED_PAWNS[1][0],TuneParam::Endgame,1),
        TuneParam(Tune::TRIPLED_PAWNS_END2,"tripled_pawns_end2",VAL(-0.25),VAL(-0.75),VAL(0),&Params::TRIPLED_PAWNS[1][1],TuneParam::Endgame,1),
        TuneParam(Tune::TRIPLED_PAWNS_END3,"tripled_pawns_end3",VAL(-0.5),VAL(-0.75),VAL(0),&Params::TRIPLED_PAWNS[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::TRIPLED_PAWNS_END4,"tripled_pawns_end4",VAL(-0.5),VAL(-0.75),VAL(0),&Params::TRIPLED_PAWNS[1][3],TuneParam::Endgame,1),
        TuneParam(Tune::ISOLATED_PAWN_MID1,"isolated_pawn_mid1",VAL(-0.07),VAL(-0.25),VAL(0),&Params::ISOLATED_PAWN[0][0],TuneParam::Midgame,1),
        TuneParam(Tune::ISOLATED_PAWN_MID2,"isolated_pawn_mid2",VAL(-0.07),VAL(-0.25),VAL(0),&Params::ISOLATED_PAWN[0][1],TuneParam::Midgame,1),
        TuneParam(Tune::ISOLATED_PAWN_MID3,"isolated_pawn_mid3",VAL(-0.07),VAL(-0.25),VAL(0),&Params::ISOLATED_PAWN[0][2],TuneParam::Midgame,1),
        TuneParam(Tune::ISOLATED_PAWN_MID4,"isolated_pawn_mid4",VAL(-0.1),VAL(-0.25),VAL(0),&Params::ISOLATED_PAWN[0][3],TuneParam::Midgame,1),
        TuneParam(Tune::ISOLATED_PAWN_END1,"isolated_pawn_end1",VAL(-0.07),VAL(-0.25),VAL(0),&Params::ISOLATED_PAWN[1][0],TuneParam::Endgame,1),
        TuneParam(Tune::ISOLATED_PAWN_END2,"isolated_pawn_end2",VAL(-0.07),VAL(-0.25),VAL(0),&Params::ISOLATED_PAWN[1][1],TuneParam::Endgame,1),
        TuneParam(Tune::ISOLATED_PAWN_END3,"isolated_pawn_end3",VAL(-0.07),VAL(-0.25),VAL(0),&Params::ISOLATED_PAWN[1][2],TuneParam::Endgame,1),
        TuneParam(Tune::ISOLATED_PAWN_END4,"isolated_pawn_end4",VAL(-0.1),VAL(-0.25),VAL(0),&Params::ISOLATED_PAWN[1][3],TuneParam::Endgame,1)
    };

   static const score_t KNIGHT_MOBILITY_INIT[9] = {VAL(-0.6), VAL(-0.28), VAL(-0.125), 0, VAL(0.05), VAL(0.125), VAL(0.16), VAL(0.16), VAL(0.16)};
   static const score_t BISHOP_MOBILITY_INIT[15] = {VAL(-0.5), VAL(-0.25), VAL(-0.125), VAL(-0.03), 0, VAL(0.05), VAL(0.08), VAL(0.125), VAL(0.16), VAL(0.20), VAL(0.25), VAL(0.25), VAL(0.3), VAL(0.3), VAL(0.3)};
   static const score_t ROOK_MOBILITY_INIT[2][15] = {{VAL(-0.625), VAL(-0.4), VAL(-0.35), VAL(-0.32), VAL(-0.27), VAL(-0.24), VAL(-0.19), VAL(-0.16), VAL(-0.125), VAL(-0.08), VAL(-0.05), VAL(-0.05), VAL(0.05), VAL(0.08), VAL(0.12)}, {VAL(-0.75), VAL(-0.40), VAL(-0.26), VAL(-0.14), 0, VAL(-0.01), VAL(0.04), VAL(0.08), VAL(0.125), VAL(0.17), VAL(0.22), VAL(0.26), VAL(0.29), VAL(0.31), VAL(0.32)}};
   static const score_t QUEEN_MOBILITY_INIT[2][24] = {{VAL(-0.6), VAL(-0.28), -VAL(0.09), VAL(0), VAL(0.05), VAL(0.1), VAL(0.15), VAL(0.2), VAL(0.25), VAL(0.26), VAL(0.28), VAL(0.29), VAL(0.31), VAL(0.33), VAL(0.35), VAL(0.36), VAL(0.38), VAL(0.45), VAL(0.52), VAL(0.55), VAL(0.75), VAL(0.75), VAL(0.75), VAL(0.75)}, {VAL(-0.75), VAL(-0.75), VAL(-0.75), VAL(-0.75), VAL(-0.45), VAL(-0.3), -VAL(0.18), VAL(-0.16), VAL(-0.1), VAL(-0.08), VAL(-0.05), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
   static const score_t KING_MOBILITY_ENDGAME_INIT[5] = {VAL(-0.2), VAL(-0.12), VAL(-0.06), 0, VAL(0.01)};

   static const score_t KNIGHT_PST_INIT[2][32] = {{-78, -31, -21, -7, -31, -25, -15, 7, -46, -22, -3, 26, -3, 7, 12, 26, -26, -38, 6, 4, -54, -46, -17, -30, -84, -85, -28, -10, -128, -81, -70, -51}, {-65, -24, -20, -24, -14, -20, -33, -18, -39, -30, -18, -0, -13, -19, 6, 13, -37, -28, -18, -14, -64, -49, -33, -39, -84, -62, -61, -46, -128, -82, -89, -77}};
   static const score_t BISHOP_PST_INIT[2][32] = {{-1, 12, 5, 6, 7, 26, 16, 10, -6, 14, 17, 12, -6, 1, -0, 22, -36, 4, -14, -7, 9, -30, -40, -27, -26, -52, -51, -55, -21, -47, -69, -66}, {-27, -11, -8, -14, -9, -11, -16, -3, -21, -4, -3, 1, -21, -12, -4, 4, -25, -26, -22, -22, -14, -26, -30, -37, -18, -24, -36, -40, -7, -22, -35, -48}};
   static const score_t ROOK_PST_INIT[2][32] = {{-11, -9, -5, -4, -33, -7, -6, -0, -42, -13, -17, -6, -46, -25, -31, -18, -35, -36, -16, -12, -29, -18, -20, -5, -60, -71, -48, -38, -15, -17, -46, -42}, {-22, -20, -15, -13, -28, -30, -24, -25, -25, -23, -15, -17, -13, -7, -1, -5, 2, 2, 4, 0, -0, 7, 6, 1, -10, -9, -4, -11, 11, 10, 9, 3}};
   static const score_t QUEEN_PST_INIT[2][32] = {{-15, -26, -23, -8, -15, 1, 7, 7, -15, -5, 0, -0, -2, 5, 7, 11, 13, 8, 11, 18, -10, 13, 13, 3, -28, -23, -13, -20, 35, 26, 11, -8}, {-79, -80, -79, -54, -61, -59, -56, -41, -51, -17, -13, -20, -25, -17, 6, 11, -28, 0, 15, 32, -38, -13, 20, 30, -30, -4, 23, 42, -50, -46, -34, -22}};
   static const score_t KING_PST_INIT[2][32] = {{-11, 1, -47, -4, 32, 29, -40, -21, -89, -82, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -128, -20, -86, -128, -126, -43, -128, -88, -67}, {-11, -8, -11, -13, 10, 2, -4, -5, 2, 0, 5, 6, 10, 15, 23, 24, 16, 23, 31, 32, 29, 44, 44, 42, 20, 32, 42, 37, -15, 8, 21, 21}};

   static const score_t KING_OPP_PASSER_DISTANCE_INIT[6] = {0, 0, VAL(0.2), VAL(0.4), VAL(0.6), VAL(0.7)};

   static const score_t PAWN_STORM_INIT[2][4][5] = {{{20, -64, -20, 26, 3},{20, -27, 22, 30, -8},{20, 30, 14, 40, 10},{20, -16, 29, 41, 15}},{{10, 2, 1, -24, -10},{10, -64, -4, -8, -1},{10, 30, 20, 15, 8},{10, 64, -12, 12, -2}}};

   int i = 0;
   for (;i < NUM_MISC_PARAMS; i++) {
      push_back(initial_params[i]);
   }
   static const TuneParam::Scaling scales[2] = {TuneParam::Midgame,
                                                TuneParam::Endgame};
   ASSERT(i==KING_OPP_PASSER_DISTANCE);
   for (int x = 0; x < 6; x++) {
      stringstream name;
      name << "king_opp_passer_distance_rank" << x+2;
      push_back(TuneParam(i++,name.str(),KING_OPP_PASSER_DISTANCE_INIT[x],0,ENDGAME_KING_POS_RANGE,&Params::KING_OPP_PASSER_DISTANCE[x],TuneParam::Endgame,1));
   }
   ASSERT(i==OWN_PIECE_KING_PROXIMITY_MULT);
   int mult = -16;
   for (int x = 0; x < 16; x++) {
      stringstream name;
      name << "own_piece_king_proximity_mult" << x;
      push_back(TuneParam(i++,name.str(),mult<0 ? 0 : mult,0,64,&Params::OWN_PIECE_KING_PROXIMITY_MULT[x],TuneParam::Midgame,x>=4 && mult<64));
      mult += 8;
      if (mult > 64) mult = 64;
   }
   static const string names[] =
      {"knight_pst","bishop_pst","rook_pst","queen_pst","king_pst"};

   // add PSTs
   ASSERT(i == KNIGHT_PST_MIDGAME);
   for (int n = 0; n < 5; n++) {
      for (int phase = 0; phase < 2; phase++) {
         for (int j = 0; j < 32; j++) {
            stringstream name;
            name << names[n];
            if (phase == 0)
               name << "_mid";
            else
               name << "_end";
            name << j;
            score_t val = 0;
            score_t *param = nullptr;
            int paramIdx = 8*(j/4)+(j%4);
            switch(n) {
            case 0:
               val = KNIGHT_PST_INIT[phase][j];
               param = &Params::KNIGHT_PST[phase][paramIdx];
               break;
            case 1:
               val = BISHOP_PST_INIT[phase][j];
               param = &Params::BISHOP_PST[phase][paramIdx];
               break;
            case 2:
               val = ROOK_PST_INIT[phase][j];
               param = &Params::ROOK_PST[phase][paramIdx];
               break;
            case 3:
               val = QUEEN_PST_INIT[phase][j];
               param = &Params::QUEEN_PST[phase][paramIdx];
               break;
            case 4:
               val = KING_PST_INIT[phase][j];
               param = &Params::KING_PST[phase][paramIdx];
               break;
            default:
               break;
            }
            push_back(TuneParam(i++,name.str(),val,-PST_RANGE,PST_RANGE,param,scales[phase],1));
         }
      }
   }
   ASSERT(i==THREAT_BY_PAWN);
   for (int phase = 0; phase < 2; phase++) {
       for (int j = 0; j < 5; j++) {
           stringstream name;
           name << "threat_by_pawn" << j;
           if (phase == 0) {
               name << "_mid";
           }
           else {
               name << "_end";
           }
           push_back(TuneParam(i++,name.str(),VAL(0.5),0,THREAT_RANGE,&Params::THREAT_BY_PAWN[phase][j],scales[phase],1));
       }
   }
   ASSERT(i==THREAT_BY_KNIGHT);
   for (int phase = 0; phase < 2; phase++) {
       for (int j = 0; j < 5; j++) {
           stringstream name;
           name << "threat_by_knight" << j;
           if (phase == 0) {
               name << "_mid";
           }
           else {
               name << "_end";
           }
           push_back(TuneParam(i++,name.str(),phase ? VAL(0.4) : VAL(0.6),0,THREAT_RANGE,&Params::THREAT_BY_KNIGHT[phase][j],scales[phase],1));
       }
   }
   ASSERT(i==THREAT_BY_BISHOP);
   for (int phase = 0; phase < 2; phase++) {
       for (int j = 0; j < 5; j++) {
           stringstream name;
           name << "threat_by_bishop" << j;
           if (phase == 0) {
               name << "_mid";
           }
           else {
               name << "_end";
           }
           push_back(TuneParam(i++,name.str(),phase ? VAL(0.4) : VAL(0.6),0,THREAT_RANGE,&Params::THREAT_BY_BISHOP[phase][j],scales[phase],1));
       }
   }
   ASSERT(i==THREAT_BY_ROOK);
   for (int phase = 0; phase < 2; phase++) {
       for (int j = 0; j < 5; j++) {
           stringstream name;
           name << "threat_by_rook" << j;
           if (phase == 0) {
               name << "_mid";
           }
           else {
               name << "_end";
           }
           push_back(TuneParam(i++,name.str(),phase ? VAL(0.4) : VAL(0.7),0,THREAT_RANGE,&Params::THREAT_BY_ROOK[phase][j],scales[phase],1));
       }
   }
   // add mobility
   ASSERT(i==KNIGHT_MOBILITY);
   for (int m = 0; m < 9; m++) {
      stringstream name;
      name << "knight_mobility" << m;
      const score_t val = KNIGHT_MOBILITY_INIT[m];
      push_back(TuneParam(i++,name.str(),val,-MOBILITY_RANGE,+MOBILITY_RANGE,&Params::KNIGHT_MOBILITY[m],TuneParam::Any,1));
   }
   ASSERT(i==BISHOP_MOBILITY);
   for (int m = 0; m < 15; m++) {
      stringstream name;
      name << "bishop_mobility" << m;
      push_back(TuneParam(i++,name.str(),BISHOP_MOBILITY_INIT[m],-MOBILITY_RANGE,MOBILITY_RANGE,&Params::BISHOP_MOBILITY[m],TuneParam::Any,1));
   }
   ASSERT(i==ROOK_MOBILITY_MIDGAME);
   for (int phase = 0; phase < 2; phase++) {
      for (int m = 0; m < 15; m++) {
         stringstream name;
         name << "rook_mobility";
         if (phase == 0) {
            name << "_mid";
         }
         else {
            name << "_end";
         }
         name << m;
         push_back(TuneParam(i++,name.str(),ROOK_MOBILITY_INIT[phase][m],-MOBILITY_RANGE,MOBILITY_RANGE,&Params::ROOK_MOBILITY[phase][m],scales[phase],1));
      }
   }
   ASSERT(i==QUEEN_MOBILITY_MIDGAME);
   for (int phase = 0; phase < 2; phase++) {
      for (int m = 0; m < 24; m++) {
         stringstream name;
         name << "queen_mobility";
         if (phase == 0) {
            name << "_mid";
         }
         else {
            name << "_end";
         }
         name << m;
         push_back(TuneParam(i++,name.str(),QUEEN_MOBILITY_INIT[phase][m],-MOBILITY_RANGE,MOBILITY_RANGE,&Params::QUEEN_MOBILITY[phase][m],scales[phase],1));
      }
   }
   ASSERT(i==KING_MOBILITY_ENDGAME);
   for (int m = 0; m < 5; m++) {
      stringstream name;
      name << "king_mobility_endgame" << m;
      push_back(TuneParam(i++,name.str(),KING_MOBILITY_ENDGAME_INIT[m],-MOBILITY_RANGE,MOBILITY_RANGE,&Params::KING_MOBILITY_ENDGAME[m],TuneParam::Endgame,1));
   }
   // outposts
   ASSERT(i==KNIGHT_OUTPOST_MIDGAME);
   for (int p = 0; p < 2; p++) {
      for (int d = 0; d < 2; d++) {
         stringstream name;
         name << "knight_outpost";
         if (p == 0) {
            name << "_mid";
         }
         else {
            name << "_end";
         }
         name << d;
         push_back(TuneParam(i++,name.str(),OUTPOST_RANGE/2,0,OUTPOST_RANGE,&Params::KNIGHT_OUTPOST[p][d],p == 0 ? TuneParam::Midgame : TuneParam::Endgame,1));
      }
   }
   ASSERT(i==BISHOP_OUTPOST_MIDGAME);
   for (int p = 0; p < 2; p++) {
      for (int d = 0; d < 2; d++) {
         stringstream name;
         name << "bishop_outpost";
         if (p == 0) {
            name << "_mid";
         }
         else {
            name << "_end";
         }
         name << d;
         push_back(TuneParam(i++,name.str(),OUTPOST_RANGE/2,0,OUTPOST_RANGE,&Params::BISHOP_OUTPOST[p][d],p == 0 ? TuneParam::Midgame : TuneParam::Endgame,1));
      }
   }
   ASSERT(i==PAWN_STORM);
   for (int b=0; b<2; b++) {
       for (int file=0; file<4; file++) {
           for (int dist=0; dist<5; dist++) {
               stringstream name;
               name << "pawn_storm";
               name << file;
               name << '_' << dist;
               if (b) name << "_blocked";
               push_back(TuneParam(i++,name.str(),PAWN_STORM_INIT[b][file][dist],-VAL(0.5),VAL(0.5),&Params::PAWN_STORM[b][file][dist],TuneParam::Midgame,1));
           }
       }
   }
}

void Tune::checkParams() const
{
   if (NUM_MISC_PARAMS != KING_OPP_PASSER_DISTANCE) {
      cerr << "warning: NUM_MISC_PARAMS incorrect, should be " << KING_OPP_PASSER_DISTANCE << endl;
   }
   int i = 0;
   for (const TuneParam &p : (*this)) {
      if (p.index != i) {
         cerr << "warning: index mismatch in Tune::(*this) at position " << i << ", param " << p.name << endl;
      }
      ++i;
      if (p.current < p.min_value) {
         cerr << "warning: param " << p.name << " has current < min" << endl;
      }
      if (p.current > p.max_value) {
         cerr << "warning: param " << p.name << " has current > max" << endl;
      }
      if (p.min_value > p.max_value) {
         cerr << "warning: param " << p.name << " has min>max" << endl;
      }
/*
      if (p.min_value == p.current) {
         cerr << "warning: param " << p.name << " tuned to min value (" << p.current << ")." << endl;
      }
      if (p.max_value == p.current) {
         cerr << "warning: param " << p.name << " tuned to max value (" << p.current << ")." << endl;
      }
*/
   }
}

void Tune::applyParams(bool check) const
{
   if (check) {
      checkParams();
   }

   memset(Params::PASSED_PAWN[0],'\0',sizeof(score_t)*8);
   memset(Params::PASSED_PAWN[1],'\0',sizeof(score_t)*8);
   memset(Params::CONNECTED_PASSER[0],'\0',sizeof(score_t)*8);
   memset(Params::CONNECTED_PASSER[1],'\0',sizeof(score_t)*8);
   memset(Params::ADJACENT_PASSER[0],'\0',sizeof(score_t)*8);
   memset(Params::ADJACENT_PASSER[1],'\0',sizeof(score_t)*8);
   memset(Params::DOUBLED_PAWNS[0],'\0',sizeof(score_t)*8);
   memset(Params::DOUBLED_PAWNS[1],'\0',sizeof(score_t)*8);
   memset(Params::TRIPLED_PAWNS[0],'\0',sizeof(score_t)*8);
   memset(Params::TRIPLED_PAWNS[1],'\0',sizeof(score_t)*8);
   memset(Params::ISOLATED_PAWN[0],'\0',sizeof(score_t)*8);
   memset(Params::ISOLATED_PAWN[1],'\0',sizeof(score_t)*8);

   // update parameter structure from Tune array (handles most parameteters)
   for (const auto &t : (*this)) {
       if (t.param) {
           *(t.param) = t.current;
       }
   }
   // Special cases:
   // 1) compute king cover scores
   for (int i = 0; i < 6; i++) {
      for (int k = 0; k < 4; k++) {
         Params::KING_COVER[i][k] = PARAM(Tune::KING_COVER1+i)*PARAM(Tune::KING_COVER_FILE_FACTOR0+k)/64;
      }
   }
   // 2) PSTs. These are symmetrical. Expand 32-bit tune params to 64-bit PSTs
   for (int phase = 0; phase < 2; phase++) {
      for (int j = 0; j < 32; j++) {
          int n = 8*(j/4) + (j%4);
          int k = 8*(j/4) + 7 - (j%4);
          Params::KNIGHT_PST[phase][k] = Params::KNIGHT_PST[phase][n];
          Params::BISHOP_PST[phase][k] = Params::BISHOP_PST[phase][n];
          Params::ROOK_PST[phase][k] = Params::ROOK_PST[phase][n];
          Params::QUEEN_PST[phase][k] = Params::QUEEN_PST[phase][n];
          Params::KING_PST[phase][k] = Params::KING_PST[phase][n];
      }
   }
   // 3) compute king attack scale array (sigmoid)
   for (int i = 0; i < Params::KING_ATTACK_SCALE_SIZE; i++) {
       int x = int(PARAM(KING_ATTACK_SCALE_BIAS) +
           std::round(PARAM(KING_ATTACK_SCALE_MAX)/(1+exp(-PARAM(KING_ATTACK_SCALE_FACTOR)*(i-PARAM(KING_ATTACK_SCALE_INFLECT))/1000.0))));
       Params::KING_ATTACK_SCALE[i] = x;
   }
}

void Tune::writeX0(ostream &o)
{
   for (const TuneParam &p : *this) {
      o << p.name << ' ' << p.current << endl;
   }
   o << endl;
}

void Tune::readX0(istream &is)
{
   while (is.good()) {
      string in;
      getline(is,in);
      size_t pos = in.find(' ');
      if (pos == string::npos) continue;
      const string name = in.substr(0,pos);
      auto it = std::find_if(begin(),end(),[&name] (const TuneParam &p) { return p.name == name; });
      if (it == end()) {
         cerr << "invalid param name found in input file: " << name << endl;
      } else {
         stringstream valstream(in.substr(pos+1,in.size()));
         score_t val;
         valstream >> val;
         if (!valstream.bad() && !valstream.fail()) {
            it->current = val;
         } else {
            cerr << "error parsing value for parameter " << name << endl;
         }
      }
   }
}

// return index for parameter given name, -1 if not not found
int Tune::findParamByName(const string &name) const {
   int i = 0;
   for (auto it : *this) {
      if (it.name == name) {
         return i;
      }
      ++i;
   }
   return -1;
}

double Tune::scale(score_t value,int index,int materialLevel) const
{
   ASSERT(materialLevel >= 0 && materialLevel<32);
   switch ((*this)[index].scaling) {
   case TuneParam::Any:
      return value;
   case TuneParam::Midgame:
      return (value*Params::MATERIAL_SCALE[materialLevel])/128.0;
   case TuneParam::Endgame:
      return (value*(128-Params::MATERIAL_SCALE[materialLevel]))/128.0;
   case TuneParam::None:
      return 0;
   }
   return 0;
}

score_t Tune::kingAttackSigmoid(score_t weight) const
{
    return PARAM(KING_ATTACK_SCALE_BIAS) +
        PARAM(KING_ATTACK_SCALE_MAX)/(1+exp(-PARAM(KING_ATTACK_SCALE_FACTOR)*(weight-PARAM(KING_ATTACK_SCALE_INFLECT))/1000.0));
}

