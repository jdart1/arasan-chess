// Copyright 2014-2016 by Jon Dart. All Rights Reserved.
#include "tune.h"
#include "chess.h"
#include "attacks.h"
#include "scoring.h"

extern "C" {
#include <math.h>
#include <string.h>
};

static const int MOBILITY_RANGE = PAWN_VALUE/3;
static const int OUTPOST_RANGE = PAWN_VALUE/3;
static const int PST_RANGE = PAWN_VALUE/2;
static const int PP_BLOCK_RANGE = PAWN_VALUE/3;
static const int TRADE_DOWN_RANGE = PAWN_VALUE/3;
static const int ENDGAME_KING_POS_RANGE = PAWN_VALUE/2;
static const int KING_ATTACK_SCALE_MAX = 5*PAWN_VALUE;
static const int KING_COVER_RANGE = int(0.3*PAWN_VALUE);
static const int KING_ATTACK_COUNT_BOOST_RANGE = Scoring::Params::KING_ATTACK_FACTOR_RESOLUTION*30;
static const int KING_ATTACK_COVER_BOOST_RANGE = Scoring::Params::KING_ATTACK_FACTOR_RESOLUTION*30;

int Tune::numTuningParams() const
{
   return (int)tune_params.size();
}

int Tune::paramArraySize() const 
{
   return (int)SIDE_PROTECTED_PAWN-(int)KING_COVER_BASE+1;
}

#define PARAM(x) tune_params[x].current

static int map_from_pst(int i) 
{
   int r = 1+(i/4);
   int f = 1+(i%4);
   ASSERT(OnBoard(MakeSquare(f,r,White)));
   return MakeSquare(f,r,White);
}

static void apply_to_pst(int i,score_t val,score_t arr[]) 
{
   int r = 1+(i/4);
   int f = 1+(i%4);
   ASSERT(OnBoard(MakeSquare(f,r,White)));
   arr[MakeSquare(f,r,White)] = arr[MakeSquare(9-f,r,White)] = val;
}

static int map_from_outpost(int i) 
{
   int r = 5+(i/4);
   int f = 1+(i%4);
   ASSERT(OnBoard(MakeSquare(f,r,White)));
   return MakeSquare(f,r,White);
}

static void apply_to_outpost(int i,score_t val,score_t arr[]) 
{
   int r = 5+(i/4);
   int f = 1+(i%4);
   ASSERT(OnBoard(MakeSquare(f,r,White)));
   arr[MakeSquare(f,r,White)] = arr[MakeSquare(9-f,r,White)] = val;
}

Tune::Tune() 
{
    // Tuning params for most parameters (except PSTs, mobility).
    // These are initialized to some reasonable but not optimal values.
    static Tune::TuneParam initial_params[Tune::NUM_MISC_PARAMS] = {
        Tune::TuneParam(Tune::RB_ADJUST1,"rb_adjust1",-350,-400,100,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::RB_ADJUST2,"rb_adjust2",-75,-400,300,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::RB_ADJUST3,"rb_adjust3",75,-300,400,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::RB_ADJUST4,"rb_adjust4",250,-150,500,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::RBN_ADJUST1,"rbn_adjust1",-500,-750,-150,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::RBN_ADJUST2,"rbn_adjust2",-675,-900,-300,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::RBN_ADJUST3,"rbn_adjust3",-825,-1200,-500,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::RBN_ADJUST4,"rbn_adjust4",-1000,-1500,-500,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::QR_ADJUST0,"qr_adjust0",-500,-750,-150,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::QR_ADJUST1,"qr_adjust1",0,-500,500,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::QR_ADJUST2,"qr_adjust2",500,250,1000,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::QR_ADJUST3,"qr_adjust3",500,250,1000,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::KN_VS_PAWN_ADJUST0,"kn_vs_pawn_adjust0",0,-250,250,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::KN_VS_PAWN_ADJUST1,"kn_vs_pawn_adjust1",-2400,-3600,-1200,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::KN_VS_PAWN_ADJUST2,"kn_vs_pawn_adjust2",-1500,-2000,-1000,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::CASTLING0,"castling0",0,-100,100,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CASTLING1,"castling1",-70,-300,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CASTLING2,"castling2",-100,-300,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CASTLING3,"castling3",280,0,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CASTLING4,"castling4",200,0,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CASTLING5,"castling5",-280,-500,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_COVER0,"king_cover0",250,50,KING_COVER_RANGE,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_COVER1,"king_cover1",225,50,KING_COVER_RANGE,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_COVER2,"king_cover2",100,50,KING_COVER_RANGE*3/4,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_COVER3,"king_cover3",29,-150,KING_COVER_RANGE/2,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_FILE_OPEN,"king_file_open",-285,-KING_COVER_RANGE,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_COVER_FILE_FACTOR0,"king_cover_file_factor0",64,48,96,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_COVER_FILE_FACTOR1,"king_cover_file_factor1",64,48,96,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_COVER_FILE_FACTOR2,"king_cover_file_factor2",64,48,96,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_COVER_FILE_FACTOR3,"king_cover_file_factor3",64,48,96,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::KING_COVER_BASE,"king_cover_base",-350,-500,0,Tune::TuneParam::Midgame,0),
        Tune::TuneParam(Tune::KING_DISTANCE_BASIS,"king_distance_basis",312,200,400),
        Tune::TuneParam(Tune::KING_DISTANCE_MULT,"king_distance_mult",77,40,120),
        Tune::TuneParam(Tune::PIN_MULTIPLIER_MID,"pin_multiplier_mid",227,0,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PIN_MULTIPLIER_END,"pin_multiplier_end",289,0,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ROOK_VS_PAWNS,"rook_vs_pawns",333,0,500,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::KRMINOR_VS_R,"krminor_vs_r",-100,-500,0,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::KRMINOR_VS_R_NO_PAWNS,"krminor_vs_r_no_pawns",-250,-1000,0,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::KQMINOR_VS_Q,"kqminor_vs_q",-100,-500,0,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::KQMINOR_VS_Q_NO_PAWNS,"kqminor_vs_q_no_pawns",-100,-3000,0,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::MINOR_FOR_PAWNS,"minor_for_pawns",500,0,750,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::ENDGAME_PAWN_ADVANTAGE,"endgame_pawn_advantage",31,0,250,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::PAWN_ENDGAME1,"pawn_endgame1",75,0,500,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::PAWN_ENDGAME2,"pawn_endgame2",125,0,500,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::PAWN_ATTACK_FACTOR1,"pawn_attack_factor1",8,0,25,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PAWN_ATTACK_FACTOR2,"pawn_attack_factor2",8,0,25,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::MINOR_ATTACK_FACTOR,"minor_attack_factor",25,10,50,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::MINOR_ATTACK_BOOST,"minor_attack_boost",35,5,70,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ROOK_ATTACK_FACTOR,"rook_attack_factor",31,12,75,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ROOK_ATTACK_BOOST,"rook_attack_boost",34,0,75,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ROOK_ATTACK_BOOST2,"rook_attack_boost2",34,0,75,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::QUEEN_ATTACK_FACTOR,"queen_attack_factor",33,20,75,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::QUEEN_ATTACK_BOOST,"queen_attack_boost",50,0,100,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::QUEEN_ATTACK_BOOST2,"queen_attack_boost2",50,0,100,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PAWN_THREAT_ON_PIECE_MID,"pawn_threat_on_piece_mid",50,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PAWN_THREAT_ON_PIECE_END,"pawn_threat_on_piece_end",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_MM_MID,"piece_threat_mm_mid",50,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_MR_MID,"piece_threat_mr_mid",50,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_MQ_MID,"piece_threat_mq_mid",50,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_MM_END,"piece_threat_mm_end",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_MR_END,"piece_threat_mr_end",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_MQ_END,"piece_threat_mq_end",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::MINOR_PAWN_THREAT_MID,"minor_pawn_threat_mid",50,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::MINOR_PAWN_THREAT_END,"minor_pawn_threat_end",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_RM_MID,"piece_threat_rm_mid",50,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_RR_MID,"piece_threat_rr_mid",50,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_RQ_MID,"piece_threat_rq_mid",50,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_RM_END,"piece_threat_rm_end",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_RR_END,"piece_threat_rr_end",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PIECE_THREAT_RQ_END,"piece_threat_rq_end",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ROOK_PAWN_THREAT_MID,"rook_pawn_threat_mid",50,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ROOK_PAWN_THREAT_END,"rook_pawn_threat_end",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ENDGAME_KING_THREAT,"endgame_king_threat",50,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::BISHOP_TRAPPED,"bishop_trapped",-1470,-2000,-400),
        Tune::TuneParam(Tune::BISHOP_PAIR_MID,"bishop_pair_mid",447,100,600,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::BISHOP_PAIR_END,"bishop_pair_end",577,125,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::BISHOP_PAWN_PLACEMENT_END,"bishop_pawn_placement_end",-170,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::BAD_BISHOP_MID,"bad_bishop_mid",-44,-80,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::BAD_BISHOP_END,"bad_bishop_end",-66,-120,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::CENTER_PAWN_BLOCK,"center_pawn_block",-127,-300,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::OUTSIDE_PASSER_MID,"outside_passer_mid",110,0,250,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::OUTSIDE_PASSER_END,"outside_passer_end",234,0,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::WEAK_PAWN_MID,"weak_pawn_mid",-80,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::WEAK_PAWN_END,"weak_pawn_end",-80,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::WEAK_ON_OPEN_FILE_MID,"weak_on_open_file_mid",-112,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::WEAK_ON_OPEN_FILE_END,"weak_on_open_file_end",-112,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::SPACE,"space",16,0,120,Tune::TuneParam::Any,1),
        Tune::TuneParam(Tune::PAWN_CENTER_SCORE_MID,"pawn_center_score_mid",27,0,100,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ROOK_ON_7TH_MID,"rook_on_7th_mid",235,0,800,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ROOK_ON_7TH_END,"rook_on_7th_end",251,0,800,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::TWO_ROOKS_ON_7TH_MID,"two_rooks_on_7th_mid",250,0,800,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::TWO_ROOKS_ON_7TH_END,"two_rooks_on_7th_end",250,0,800,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ROOK_ON_OPEN_FILE_MID,"rook_on_open_file_mid",170,0,600,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ROOK_ON_OPEN_FILE_END,"rook_on_open_file_end",182,0,600,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ROOK_BEHIND_PP_MID,"rook_behind_pp_mid",25,0,600,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ROOK_BEHIND_PP_END,"rook_behind_pp_end",78,0,600,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::QUEEN_OUT,"queen_out",-68,-200,0,Tune::TuneParam::Midgame),
        Tune::TuneParam(Tune::PAWN_SIDE_BONUS,"pawn_side_bonus",306,0,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::KING_OWN_PAWN_DISTANCE,"king_own_pawn_distance",50,0,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::KING_OPP_PAWN_DISTANCE,"king_opp_pawn_distance",50,0,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::SUPPORTED_PASSER6,"supported_passer6",401,0,1000,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::SUPPORTED_PASSER7,"supported_passer7",728,0,1500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::SIDE_PROTECTED_PAWN,"side_protected_pawn",-92,-500,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::KING_POSITION_LOW_MATERIAL0,"king_position_low_material0",150,128,200,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::KING_POSITION_LOW_MATERIAL1,"king_position_low_material1",140,128,200,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::KING_POSITION_LOW_MATERIAL2,"king_position_low_material2",130,128,200,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_MID2,"passed_pawn_mid2",60,0,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_MID3,"passed_pawn_mid3",110,50,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_MID4,"passed_pawn_mid4",180,70,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_MID5,"passed_pawn_mid5",270,90,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_MID6,"passed_pawn_mid6",700,500,1000,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_MID7,"passed_pawn_mid7",1110,500,1750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_END2,"passed_pawn_end2",90,0,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_END3,"passed_pawn_end3",160,50,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_END4,"passed_pawn_end4",280,70,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_END5,"passed_pawn_end5",420,90,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_END6,"passed_pawn_end6",840,500,1000,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::PASSED_PAWN_END7,"passed_pawn_end7",1410,500,1750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_MID2,"potential_passer_mid2",26,0,200,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_MID3,"potential_passer_mid3",37,0,200,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_MID4,"potential_passer_mid4",75,0,300,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_MID5,"potential_passer_mid5",75,0,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_MID6,"potential_passer_mid6",236,0,750,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_END2,"potential_passer_end2",40,0,200,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_END3,"potential_passer_end3",56,0,200,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_END4,"potential_passer_end4",115,0,300,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_END5,"potential_passer_end5",115,0,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::POTENTIAL_PASSER_END6,"potential_passer_end6",363,0,750,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_MID2,"connected_passer_mid2",0,0,250,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_MID3,"connected_passer_mid3",88,50,300,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_MID4,"connected_passer_mid4",206,70,400,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_MID5,"connected_passer_mid5",227,75,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_MID6,"connected_passer_mid6",531,100,1000,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_MID7,"connected_passer_mid7",771,150,1500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_END2,"connected_passer_end2",0,0,250,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_END3,"connected_passer_end3",92,50,300,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_END4,"connected_passer_end4",215,70,400,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_END5,"connected_passer_end5",237,75,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_END6,"connected_passer_end6",555,150,1000,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::CONNECTED_PASSER_END7,"connected_passer_end7",800,250,1500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_MID2,"adjacent_passer_mid2",0,0,250,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_MID3,"adjacent_passer_mid3",100,50,300,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_MID4,"adjacent_passer_mid4",150,70,400,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_MID5,"adjacent_passer_mid5",150,75,500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_MID6,"adjacent_passer_mid6",306,100,1000,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_MID7,"adjacent_passer_mid7",771,150,1500,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_END2,"adjacent_passer_end2",0,0,250,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_END3,"adjacent_passer_end3",103,50,300,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_END4,"adjacent_passer_end4",155,70,400,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_END5,"adjacent_passer_end5",155,75,500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_END6,"adjacent_passer_end6",317,150,1000,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ADJACENT_PASSER_END7,"adjacent_passer_end7",801,250,1500,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::DOUBLED_PAWNS_MID1,"doubled_pawns_mid1",-47,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::DOUBLED_PAWNS_MID2,"doubled_pawns_mid2",-70,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::DOUBLED_PAWNS_MID3,"doubled_pawns_mid3",-108,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::DOUBLED_PAWNS_MID4,"doubled_pawns_mid4",-108,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::DOUBLED_PAWNS_END1,"doubled_pawns_end1",-82,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::DOUBLED_PAWNS_END2,"doubled_pawns_end2",-123,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::DOUBLED_PAWNS_END3,"doubled_pawns_end3",-190,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::DOUBLED_PAWNS_END4,"doubled_pawns_end4",-190,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::TRIPLED_PAWNS_MID1,"tripled_pawns_mid1",-70,-500,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::TRIPLED_PAWNS_MID2,"tripled_pawns_mid2",-105,-500,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::TRIPLED_PAWNS_MID3,"tripled_pawns_mid3",-160,-500,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::TRIPLED_PAWNS_MID4,"tripled_pawns_mid4",-162,-500,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::TRIPLED_PAWNS_END1,"tripled_pawns_end1",-123,-500,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::TRIPLED_PAWNS_END2,"tripled_pawns_end2",-184,-500,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::TRIPLED_PAWNS_END3,"tripled_pawns_end3",-285,-500,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::TRIPLED_PAWNS_END4,"tripled_pawns_end4",-285,-500,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ISOLATED_PAWN_MID1,"isolated_pawn_mid1",-70,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ISOLATED_PAWN_MID2,"isolated_pawn_mid2",-70,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ISOLATED_PAWN_MID3,"isolated_pawn_mid3",-70,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ISOLATED_PAWN_MID4,"isolated_pawn_mid4",-95,-250,0,Tune::TuneParam::Midgame,1),
        Tune::TuneParam(Tune::ISOLATED_PAWN_END1,"isolated_pawn_end1",-79,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ISOLATED_PAWN_END2,"isolated_pawn_end2",-79,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ISOLATED_PAWN_END3,"isolated_pawn_end3",-79,-250,0,Tune::TuneParam::Endgame,1),
        Tune::TuneParam(Tune::ISOLATED_PAWN_END4,"isolated_pawn_end4",-108,-250,0,Tune::TuneParam::Endgame,1)

    };

   // boostrap values - maybe not optimal
   static const int KNIGHT_OUTPOST_INIT[64] = 
      {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
       0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
       0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
       10 ,30 ,60 ,60 ,60 ,60 ,30 ,10,
       10 ,40 ,90 ,140 ,140 ,90 ,40 ,10,
       10 ,40 ,90 ,140 ,140 ,90 ,40 ,10,
       10 ,10 ,60 ,60 ,60 ,60 ,10 ,10,
       10 ,10 ,10 ,10 ,10 ,10 ,10 ,10
      };
   static const int BISHOP_OUTPOST_INIT[64] = 
      {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
       0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
       0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
       10 ,20 ,50 ,50 ,50 ,50 ,20 ,10,
       10 ,30 ,70 ,110 ,110 ,70 ,30 ,10,
       10 ,30 ,70 ,110 ,110 ,70 ,30 ,10,
       10 ,10 ,50 ,50 ,50 ,50 ,10 ,10,
       10 ,10 ,10 ,10 ,10 ,10 ,10 ,10
      };

   static const int TRADE_DOWN_INIT[8] = {688, 602, 516, 430, 344, 258, 172, 86};
   static const int KNIGHT_MOBILITY_INIT[9] = {-180, -70, -20, 0, 20, 50, 70, 100, 120};
   static const int BISHOP_MOBILITY_INIT[15] = {-200, -110, -70, -30, 0, 30, 60, 90, 90, 90, 90, 90, 90, 90, 90};
   static const int ROOK_MOBILITY_INIT[2][15] = {{-220, -120, -80, -30, 0, 30, 70, 100, 120, 140, 170, 190, 210, 230, 240}, {-300, -170, -110, -50, 0, 50, 90, 140, 170, 200, 230, 260, 290, 310, 320}};
   static const int QUEEN_MOBILITY_INIT[2][29] = {{-100, -50, -10, -10, 40, 70, 90, 110, 130, 140, 160, 170, 190, 200, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210, 210}, {-120, -60, -10, 10, 50, 80, 110, 130, 160, 170, 200, 210, 230, 250, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260, 260}};
   static const int KING_MOBILITY_ENDGAME_INIT[5] = {-200, -120, -60, 0, 10};

static const int KNIGHT_PST_INIT[2][64] = 
{
   {
      -220 ,-140 ,-110 ,-100 ,-100 ,-110 ,-140 ,-220,
      -150 ,-60 ,-40 ,-30 ,-30 ,-40 ,-60 ,-150,
      -120 ,-40 ,-10 ,0 ,0 ,-10 ,-40 ,-120,
      -110 ,-30 ,0 ,30 ,30 ,0 ,-30 ,-110,
      -110 ,-30 ,0 ,30 ,30 ,0 ,-30 ,-110,
      -120 ,-40 ,-10 ,0 ,0 ,-10,-40 ,-120,
      -150 ,-60 ,-40 ,-30 ,-30 ,-40 ,-60 ,-150,
      -180 ,-90 ,-70 ,-60 ,-60 ,-70 ,-90 ,-180
   }
   ,
   {-230 ,-190 ,-160 ,-150 ,-150 ,-160 ,-190 ,-230,
    -130 ,-90 ,-50 ,-40 ,-40 , -50 ,-90,-130,
    -90 ,-50 ,-20 ,-10 ,-10 ,-20 ,-50 ,-90,
    -80 ,-40 ,-10 ,0 ,0 ,-10 ,-40 ,-80,
    -80 ,-30 ,0 ,10 ,10 ,0 ,-30,-80,
    -90 ,-40 ,0 ,0 ,0 ,0 ,-40 ,-90,
    -130 ,-70 ,-40 ,-30 ,-30 ,-40 ,-70 ,-130,
    -170 ,-130 ,-90 ,-80 ,-80 ,-90 ,-130 ,-170
   }
};

static const int BISHOP_PST_INIT[2][64] = 
{
   {
      -225 ,-120 ,-155 ,-155 ,-155 ,-155 ,-120 ,-225,
      -10 ,80 ,0 ,60 ,60 ,0 ,80 ,-10,
      -10 ,0 ,60 ,80 ,80 ,60 ,0 ,-10,
      0 ,0 ,60 ,100 ,100 ,60 ,0 ,0,
      0 ,60 ,60 ,60 ,60 ,60 ,60 ,0,
      100 ,100 ,100 ,100 ,100 ,100 ,100 ,100,
      -100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100,
      -100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100 ,-100
   }
   ,
   { -75,-45,-5,45,45,-5,-45,-75,
     -45,-15,25,75,75,25,-15,-45,
     -5,25,65,115,115,65,25,-5,
     45,75,115,165,165,115,75,45,
     45,75,115,165,165,115,75,45,
     -5,25,65,115,115,65,25,-5,
     -45,-15,25,75,75,25,-15,-45,
     -75,-45,-5,45,45,-5,-45,-75
   }
};

static const int ROOK_PST_INIT[2][64] = 
{
   {
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
   },
   {
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
   }
};

static const int QUEEN_PST_INIT[2][64] = 
{
   {
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
   },
   {
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0,
      0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
   }
};

   static const int KING_PST_INIT[2][64] = {
      {
         0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
         ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0
         ,-60 ,-60 ,-60 ,-60 ,-60 ,-60 ,-60 ,-60
         ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
         ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
         ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
         ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
         ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360 ,-360
      }
      , {-280 ,-230 ,-180 ,-130 ,-130 ,-180 ,-230 ,-280
         ,-220 ,-170 ,-120 ,-70 ,-70 ,-120 ,-170 ,-220
         ,-160 ,-110 ,-60 ,-10 ,-10 ,-60 ,-110 ,-160
         ,-100 ,-50 ,0 ,50 ,50 ,0 ,-50 ,-100
         ,-40 ,10 ,60 ,110 ,110 ,60 ,10 ,-40
         ,20 ,70 ,120 ,170 ,170 ,120 ,70 ,20
         ,80 ,130 ,180 ,230 ,230 ,180 ,130 ,80
         ,80 ,130 ,180 ,230 ,230 ,180 ,130 ,80
      }
   };
   
   const int PP_OWN_PIECE_BLOCK_INIT[2][21] = {{-15,-15,-15,-15,-15,-15,-30,-30,-30,-30,-30,-45,-45,-45,-45,-60,-60,-60,-75,-75,-90},{-43,-43,-43,-43,-43,-43,-86,-86,-86,-86,-86,-129,-129,-129,-129,-172,-172,-172,-215,-215,-258}};
   const int PP_OPP_PIECE_BLOCK_INIT[2][21] = {{-171,-85,-57,-42,-34,-28,-190,-95,-63,-47,-38,-217,-108,-72,-54,-251,-125,-83,-361,-180,-569},{-147,-147,-147,-147,-147,-147,-159,-159,-159,-159,-159,-180,-180,-180,-180,-204,-204,-204,-276,-276,-374}};

   int i = 0;
   for (;i < NUM_MISC_PARAMS; i++) {
      tune_params.push_back(initial_params[i]);
   }
   static const TuneParam::Scaling scales[2] = {Tune::TuneParam::Midgame,
                                                Tune::TuneParam::Endgame};
   for (int x = 0; x < 3; x++) {
      stringstream name;
      name << "king_attack_count_boost" << x+2;
      tune_params.push_back(TuneParam(i++,name.str(),6+4*x,0,KING_ATTACK_COUNT_BOOST_RANGE,Tune::TuneParam::Midgame,1));
   }
   ASSERT(i==KING_ATTACK_COVER_BOOST);
   for (int x = 0; x < 5; x++) {
      stringstream name;
      name << "king_attack_cover_boost" << x;
      tune_params.push_back(TuneParam(i++,name.str(),5*x,0,KING_ATTACK_COVER_BOOST_RANGE,Tune::TuneParam::Midgame,x));
   }
   ASSERT(i==KING_OPP_PASSER_DISTANCE);
   for (int x = 0; x < 6; x++) {
      stringstream name;
      name << "king_opp_passer_distance_rank" << x+2;
      tune_params.push_back(TuneParam(i++,name.str(),10+x*10,0,ENDGAME_KING_POS_RANGE,Tune::TuneParam::Endgame,1));
   }
   ASSERT(i==PP_OWN_PIECE_BLOCK_MID);
   // add passed pawn block tables
   for (int phase = 0; phase < 2; phase++) {
      for (int x = 0; x < 21; x++) {
         stringstream name;
         name << "pp_own_piece_block";
         if (phase == 0)
            name << "_mid";
         else
            name << "_end";
         name << x;
         int val = PP_OWN_PIECE_BLOCK_INIT[phase][x];
         tune_params.push_back(TuneParam(i++,name.str(),val,-PP_BLOCK_RANGE,0,scales[phase],1));
      }
   }
   for (int phase = 0; phase < 2; phase++) {
      for (int x = 0; x < 21; x++) {
         stringstream name;
         name << "pp_opp_piece_block";
         if (phase == 0)
            name << "_mid";
         else
            name << "_end";
         name << x;
         int val = PP_OPP_PIECE_BLOCK_INIT[phase][x];
         tune_params.push_back(TuneParam(i++,name.str(),val,-PP_BLOCK_RANGE,0,scales[phase],1));
      }
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
            int val = 0;
            ASSERT(map_from_pst(j) >= 0 && map_from_pst(j) < 64);
            switch(n) {
            case 0:
               val = KNIGHT_PST_INIT[phase][map_from_pst(j)]; break;
            case 1:
               val = BISHOP_PST_INIT[phase][map_from_pst(j)]; break;
            case 2:
               val = ROOK_PST_INIT[phase][map_from_pst(j)]; break;
            case 3:
               val = QUEEN_PST_INIT[phase][map_from_pst(j)]; break;
            case 4:
               val = KING_PST_INIT[phase][map_from_pst(j)]; break;
            default:
               break;
            }
            tune_params.push_back(TuneParam(i++,name.str(),val,-PST_RANGE,PST_RANGE,scales[phase],1));
         }
      }
   }
   // add mobility
   ASSERT(i==KNIGHT_MOBILITY);
   for (int m = 0; m < 9; m++) {
      stringstream name;
      name << "knight_mobility" << m;
      const int val = KNIGHT_MOBILITY_INIT[m];
      tune_params.push_back(TuneParam(i++,name.str(),val,val-MOBILITY_RANGE,val+MOBILITY_RANGE,Tune::TuneParam::Any,1));
   }
   ASSERT(i==BISHOP_MOBILITY);
   for (int m = 0; m < 15; m++) {
      stringstream name;
      name << "bishop_mobility" << m;
      tune_params.push_back(TuneParam(i++,name.str(),BISHOP_MOBILITY_INIT[m],-MOBILITY_RANGE,MOBILITY_RANGE,Tune::TuneParam::Any,1));
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
         tune_params.push_back(TuneParam(i++,name.str(),ROOK_MOBILITY_INIT[phase][m],-MOBILITY_RANGE,MOBILITY_RANGE,scales[phase],1));
      }
   }
   ASSERT(i==QUEEN_MOBILITY_MIDGAME);
   for (int phase = 0; phase < 2; phase++) {
      for (int m = 0; m < 29; m++) {
         stringstream name;
         name << "queen_mobility";
         if (phase == 0) {
            name << "_mid";
         }
         else {
            name << "_end";
         }
         name << m;
         tune_params.push_back(TuneParam(i++,name.str(),QUEEN_MOBILITY_INIT[phase][m],-MOBILITY_RANGE,MOBILITY_RANGE,scales[phase],1));
      }
   }
   ASSERT(i==KING_MOBILITY_ENDGAME);
   for (int m = 0; m < 5; m++) {
      stringstream name;
      name << "king_mobility_endgame" << m;
      tune_params.push_back(TuneParam(i++,name.str(),KING_MOBILITY_ENDGAME_INIT[m],-MOBILITY_RANGE,MOBILITY_RANGE,Tune::TuneParam::Endgame,1));
   }
   // outposts
   ASSERT(i==KNIGHT_OUTPOST);
   for (int p = 0; p < 3; p++) {
      for (int s = 0; s < 16; s++) {
         stringstream name;
         name << "knight_outpost" << p << '_' << s;
         int val = KNIGHT_OUTPOST_INIT[map_from_outpost(s)];
         if (p == 0) val /= 2;
         tune_params.push_back(TuneParam(i++,name.str(),val,0,OUTPOST_RANGE,Tune::TuneParam::Any,1));
      }
   }
   ASSERT(i==BISHOP_OUTPOST);
   for (int p = 0; p < 3; p++) {
      for (int s = 0; s < 16; s++) {
         stringstream name;
         name << "bishop_outpost" << p << '_' << s;
         int val = BISHOP_OUTPOST_INIT[map_from_outpost(s)];
         if (p == 0) val /= 2;
         tune_params.push_back(TuneParam(i++,name.str(),val,0,OUTPOST_RANGE,Tune::TuneParam::Any,1));
      }
   }
   // add trade down
   ASSERT(i==TRADE_DOWN);
   for (int p = 0; p < 8; p++) {
      stringstream name;
      name << "trade_down" << p;
      int val = TRADE_DOWN_INIT[p];
      tune_params.push_back(TuneParam(i++,name.str(),val,Util::Max(0,val-TRADE_DOWN_RANGE),val+TRADE_DOWN_RANGE,Tune::TuneParam::Any,1));
   }
   ASSERT(i==KING_ATTACK_SCALE);
   int slope = 0;
   int  val = 0;
   double f = KING_ATTACK_SCALE_MAX/(25.0*Scoring::Params::KING_ATTACK_SCALE_SIZE);

   for (int p = 0; p < Scoring::Params::KING_ATTACK_SCALE_SIZE; p++) {
      stringstream name;
      name << "king_attack_scale" << p;
      int val2 = Util::Min(KING_ATTACK_SCALE_MAX,p < 10 ? 0 : int(f*val/2));
      int range = Util::Max(PAWN_VALUE/4,val2/3);
      tune_params.push_back(TuneParam(i++,name.str(),val2,Util::Max(0,val2-range),Util::Min(KING_ATTACK_SCALE_MAX,val2+range),Tune::TuneParam::Midgame,p >= 10));
      slope = (p < Scoring::Params::KING_ATTACK_SCALE_SIZE/2 ? p : (Scoring::Params::KING_ATTACK_SCALE_SIZE-p));
      if (p>10) val += slope;
   }
}

void Tune::checkParams() const
{
#ifdef _DEBUG
   if (NUM_MISC_PARAMS != KING_ATTACK_COUNT_BOOST) {
      cerr << "warning: NUM_MISC_PARAMS incorrect, should be " << KING_ATTACK_COUNT_BOOST << endl;
   }
   for (int i = 0; i<NUM_MISC_PARAMS; i++) {
      if (tune_params[i].index != i) 
         cerr << "warning: index mismatch in Tune::tune_params at position " << i << ", param " << tune_params[i].name << endl;
      if (tune_params[i].current < tune_params[i].min_value) {
         cerr << "warning: param " << tune_params[i].name << " has current < min" << endl;
         //cerr << "resetting to " << tune_params[i].min_value << endl;
         //tune_params[i].current = tune_params[i].min_value;
      }
      if (tune_params[i].current > tune_params[i].max_value) {
         cerr << "warning: param " << tune_params[i].name << " has current > max" << endl;
         //cerr << "resetting to " << tune_params[i].max_value << endl;
         //tune_params[i].current = tune_params[i].max_value;
      }
      if (tune_params[i].min_value > tune_params[i].max_value) {
         cerr << "warning: param " << tune_params[i].name << " has min>max" << endl;
      }
   }
#endif
}

void Tune::applyParams() const
{
   checkParams();

   score_t *dest = Scoring::Params::RB_ADJUST;
   int i, j = 0;
   for (i = 0; i < 4; i++) {
      *dest++ = Tune::tune_params[j++].current;
   }
   dest = Scoring::Params::RBN_ADJUST;
   for (i = 0; i < 4; i++) {
      *dest++ = Tune::tune_params[j++].current;
   }
   dest = Scoring::Params::QR_ADJUST;
   for (i = 0; i < 4; i++) {
      *dest++ = Tune::tune_params[j++].current;
   }
   dest = Scoring::Params::KN_VS_PAWN_ADJUST;
   for (i = 0; i < 3; i++) {
      *dest++ = Tune::tune_params[j++].current;
   }
   dest = Scoring::Params::CASTLING;
   for (i = 0; i < 6; i++) {
      *dest++ = Tune::tune_params[j++].current;
   }
   // compute king cover scores
   for (i = 0; i < 5; i++) {
      for (int k = 0; k < 4; k++) {
         Scoring::Params::KING_COVER[i][k] =
            tune_params[Tune::KING_COVER0+i].current*tune_params[Tune::KING_COVER_FILE_FACTOR0+k].current/64;
      }
   }
   Scoring::Params::KING_COVER_BASE = tune_params[KING_COVER_BASE].current;
   Scoring::Params::KING_FILE_OPEN = tune_params[KING_FILE_OPEN].current;
   Scoring::Params::KING_DISTANCE_BASIS = tune_params[KING_DISTANCE_BASIS].current;
   Scoring::Params::KING_DISTANCE_MULT = tune_params[KING_DISTANCE_MULT].current;
   Scoring::Params::PIN_MULTIPLIER_MID = tune_params[PIN_MULTIPLIER_MID].current;
   Scoring::Params::PIN_MULTIPLIER_END = tune_params[PIN_MULTIPLIER_END].current;
   Scoring::Params::ROOK_VS_PAWNS = tune_params[ROOK_VS_PAWNS].current;
   Scoring::Params::KRMINOR_VS_R = tune_params[KRMINOR_VS_R].current;
   Scoring::Params::KRMINOR_VS_R_NO_PAWNS = tune_params[KRMINOR_VS_R_NO_PAWNS].current;
   Scoring::Params::KQMINOR_VS_Q = tune_params[KQMINOR_VS_Q].current;
   Scoring::Params::KQMINOR_VS_Q_NO_PAWNS = tune_params[KQMINOR_VS_Q_NO_PAWNS].current;
   Scoring::Params::MINOR_FOR_PAWNS = tune_params[MINOR_FOR_PAWNS].current;
   Scoring::Params::ENDGAME_PAWN_ADVANTAGE = tune_params[ENDGAME_PAWN_ADVANTAGE].current;
   Scoring::Params::PAWN_ENDGAME1 = tune_params[PAWN_ENDGAME1].current;
   Scoring::Params::PAWN_ENDGAME2 = tune_params[PAWN_ENDGAME2].current;
   Scoring::Params::PAWN_ATTACK_FACTOR1 = tune_params[PAWN_ATTACK_FACTOR1].current;
   Scoring::Params::PAWN_ATTACK_FACTOR2 = tune_params[PAWN_ATTACK_FACTOR2].current;
   Scoring::Params::MINOR_ATTACK_FACTOR = tune_params[MINOR_ATTACK_FACTOR].current;
   Scoring::Params::MINOR_ATTACK_BOOST = tune_params[MINOR_ATTACK_BOOST].current;
   Scoring::Params::ROOK_ATTACK_FACTOR = tune_params[ROOK_ATTACK_FACTOR].current;
   Scoring::Params::ROOK_ATTACK_BOOST = tune_params[ROOK_ATTACK_BOOST].current;
   Scoring::Params::ROOK_ATTACK_BOOST2 = tune_params[ROOK_ATTACK_BOOST2].current;
   Scoring::Params::QUEEN_ATTACK_FACTOR = tune_params[QUEEN_ATTACK_FACTOR].current;
   Scoring::Params::QUEEN_ATTACK_BOOST = tune_params[QUEEN_ATTACK_BOOST].current;
   Scoring::Params::QUEEN_ATTACK_BOOST2 = tune_params[QUEEN_ATTACK_BOOST2].current;
   Scoring::Params::PAWN_THREAT_ON_PIECE_MID = tune_params[PAWN_THREAT_ON_PIECE_MID].current;
   Scoring::Params::PAWN_THREAT_ON_PIECE_END = tune_params[PAWN_THREAT_ON_PIECE_END].current;
   Scoring::Params::PIECE_THREAT_MM_MID = tune_params[PIECE_THREAT_MM_MID].current;
   Scoring::Params::PIECE_THREAT_MR_MID = tune_params[PIECE_THREAT_MR_MID].current;
   Scoring::Params::PIECE_THREAT_MQ_MID = tune_params[PIECE_THREAT_MQ_MID].current;
   Scoring::Params::PIECE_THREAT_MM_END = tune_params[PIECE_THREAT_MM_END].current;
   Scoring::Params::PIECE_THREAT_MR_END = tune_params[PIECE_THREAT_MR_END].current;
   Scoring::Params::PIECE_THREAT_MQ_END = tune_params[PIECE_THREAT_MQ_END].current;
   Scoring::Params::MINOR_PAWN_THREAT_MID = tune_params[MINOR_PAWN_THREAT_MID].current;
   Scoring::Params::MINOR_PAWN_THREAT_END = tune_params[MINOR_PAWN_THREAT_END].current;
   Scoring::Params::PIECE_THREAT_RM_MID = tune_params[PIECE_THREAT_RM_MID].current;
   Scoring::Params::PIECE_THREAT_RR_MID = tune_params[PIECE_THREAT_RR_MID].current;
   Scoring::Params::PIECE_THREAT_RQ_MID = tune_params[PIECE_THREAT_RQ_MID].current;
   Scoring::Params::PIECE_THREAT_RM_END = tune_params[PIECE_THREAT_RM_END].current;
   Scoring::Params::PIECE_THREAT_RR_END = tune_params[PIECE_THREAT_RR_END].current;
   Scoring::Params::PIECE_THREAT_RQ_END = tune_params[PIECE_THREAT_RQ_END].current;
   Scoring::Params::ROOK_PAWN_THREAT_MID = tune_params[ROOK_PAWN_THREAT_MID].current;
   Scoring::Params::ROOK_PAWN_THREAT_END = tune_params[ROOK_PAWN_THREAT_END].current;
   Scoring::Params::ENDGAME_KING_THREAT = tune_params[ENDGAME_KING_THREAT].current;
   Scoring::Params::BISHOP_TRAPPED = tune_params[BISHOP_TRAPPED].current;
   Scoring::Params::BISHOP_PAIR_MID = tune_params[BISHOP_PAIR_MID].current;
   Scoring::Params::BISHOP_PAIR_END = tune_params[BISHOP_PAIR_END].current;
   Scoring::Params::BISHOP_PAWN_PLACEMENT_END = tune_params[BISHOP_PAWN_PLACEMENT_END].current;
   Scoring::Params::BAD_BISHOP_MID = tune_params[BAD_BISHOP_MID].current;
   Scoring::Params::BAD_BISHOP_END = tune_params[BAD_BISHOP_END].current;
   Scoring::Params::CENTER_PAWN_BLOCK = tune_params[CENTER_PAWN_BLOCK].current;
   Scoring::Params::OUTSIDE_PASSER_MID = tune_params[OUTSIDE_PASSER_MID].current;
   Scoring::Params::OUTSIDE_PASSER_END = tune_params[OUTSIDE_PASSER_END].current;
   Scoring::Params::WEAK_PAWN_MID = tune_params[WEAK_PAWN_MID].current;
   Scoring::Params::WEAK_PAWN_END = tune_params[WEAK_PAWN_END].current;
   Scoring::Params::WEAK_ON_OPEN_FILE_MID = tune_params[WEAK_ON_OPEN_FILE_MID].current;
   Scoring::Params::WEAK_ON_OPEN_FILE_END = tune_params[WEAK_ON_OPEN_FILE_END].current;
   Scoring::Params::SPACE = tune_params[SPACE].current;
   Scoring::Params::PAWN_CENTER_SCORE_MID = tune_params[PAWN_CENTER_SCORE_MID].current;
   Scoring::Params::ROOK_ON_7TH_MID = tune_params[ROOK_ON_7TH_MID].current;
   Scoring::Params::ROOK_ON_7TH_END = tune_params[ROOK_ON_7TH_END].current;
   Scoring::Params::TWO_ROOKS_ON_7TH_MID = tune_params[TWO_ROOKS_ON_7TH_MID].current;
   Scoring::Params::TWO_ROOKS_ON_7TH_END = tune_params[TWO_ROOKS_ON_7TH_END].current;
   Scoring::Params::ROOK_ON_OPEN_FILE_MID = tune_params[ROOK_ON_OPEN_FILE_MID].current;
   Scoring::Params::ROOK_ON_OPEN_FILE_END = tune_params[ROOK_ON_OPEN_FILE_END].current;
   Scoring::Params::ROOK_BEHIND_PP_MID = tune_params[ROOK_BEHIND_PP_MID].current;
   Scoring::Params::ROOK_BEHIND_PP_END = tune_params[ROOK_BEHIND_PP_END].current;
   Scoring::Params::QUEEN_OUT = tune_params[QUEEN_OUT].current;
   Scoring::Params::PAWN_SIDE_BONUS = tune_params[PAWN_SIDE_BONUS].current;
   Scoring::Params::KING_OWN_PAWN_DISTANCE = tune_params[KING_OWN_PAWN_DISTANCE].current;
   Scoring::Params::KING_OPP_PAWN_DISTANCE = tune_params[KING_OPP_PAWN_DISTANCE].current;
   Scoring::Params::SUPPORTED_PASSER6 = tune_params[SUPPORTED_PASSER6].current;
   Scoring::Params::SUPPORTED_PASSER7 = tune_params[SUPPORTED_PASSER7].current;
   Scoring::Params::SIDE_PROTECTED_PAWN = tune_params[SIDE_PROTECTED_PAWN].current;
   for (int i = 0; i < 3; i++) {
      Scoring::Params::KING_ATTACK_COUNT_BOOST[i] = PARAM(KING_ATTACK_COUNT_BOOST+i);
   }
   for (int i = 0; i < 5; i++) {
      Scoring::Params::KING_ATTACK_COVER_BOOST[i] = PARAM(KING_ATTACK_COVER_BOOST+i);
   }
   for (int i = 0; i < 6; i++) {
      Scoring::Params::KING_OPP_PASSER_DISTANCE[i] = PARAM(KING_OPP_PASSER_DISTANCE+i);
   }
   for (int i = 0; i < 3; i++) {
      Scoring::Params::KING_POSITION_LOW_MATERIAL[i] = PARAM(KING_POSITION_LOW_MATERIAL0+i);
   }
   for (int i = 0; i < 8; i++) {
      Scoring::Params::TRADE_DOWN[i] = PARAM(TRADE_DOWN+i);
   }
   for (int i = 0; i < Scoring::Params::KING_ATTACK_SCALE_SIZE; i++) {
      Scoring::Params::KING_ATTACK_SCALE[i] = PARAM(KING_ATTACK_SCALE+i);
   }
   memset(Scoring::Params::PASSED_PAWN[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::PASSED_PAWN[1],'\0',sizeof(int)*8);
   for (int i = 2; i < 8; i++) {
      Scoring::Params::PASSED_PAWN[Scoring::Midgame][i] = PARAM(PASSED_PAWN_MID2+i-2);
      Scoring::Params::PASSED_PAWN[Scoring::Endgame][i] = PARAM(PASSED_PAWN_END2+i-2);
   }
   memset(Scoring::Params::POTENTIAL_PASSER[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::POTENTIAL_PASSER[1],'\0',sizeof(int)*8);
   for (int i = 2; i < 7; i++) {
      Scoring::Params::POTENTIAL_PASSER[Scoring::Midgame][i] = PARAM(POTENTIAL_PASSER_MID2+i-2);
      Scoring::Params::POTENTIAL_PASSER[Scoring::Endgame][i] = PARAM(POTENTIAL_PASSER_END2+i-2);
   }
   memset(Scoring::Params::CONNECTED_PASSER[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::CONNECTED_PASSER[1],'\0',sizeof(int)*8);
   for (int i = 2; i < 8; i++) {
      Scoring::Params::CONNECTED_PASSER[Scoring::Midgame][i] = PARAM(CONNECTED_PASSER_MID2+i-2);
      Scoring::Params::CONNECTED_PASSER[Scoring::Endgame][i] = PARAM(CONNECTED_PASSER_END2+i-2);
   }
   memset(Scoring::Params::ADJACENT_PASSER[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::ADJACENT_PASSER[1],'\0',sizeof(int)*8);
   for (int i = 2; i < 8; i++) {
      Scoring::Params::ADJACENT_PASSER[Scoring::Midgame][i] = PARAM(ADJACENT_PASSER_MID2+i-2);
      Scoring::Params::ADJACENT_PASSER[Scoring::Endgame][i] = PARAM(ADJACENT_PASSER_END2+i-2);
   }
   memset(Scoring::Params::DOUBLED_PAWNS[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::DOUBLED_PAWNS[1],'\0',sizeof(int)*8);
   memset(Scoring::Params::TRIPLED_PAWNS[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::TRIPLED_PAWNS[1],'\0',sizeof(int)*8);
   memset(Scoring::Params::ISOLATED_PAWN[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::ISOLATED_PAWN[1],'\0',sizeof(int)*8);
   for (int i = 0; i < 4; i++) {
      Scoring::Params::DOUBLED_PAWNS[Scoring::Midgame][i] = 
         Scoring::Params::DOUBLED_PAWNS[Scoring::Midgame][7-i] =
         PARAM(DOUBLED_PAWNS_MID1+i);
      Scoring::Params::DOUBLED_PAWNS[Scoring::Endgame][i] = 
         Scoring::Params::DOUBLED_PAWNS[Scoring::Endgame][7-i] = 
         PARAM(DOUBLED_PAWNS_END1+i);
      Scoring::Params::TRIPLED_PAWNS[Scoring::Midgame][i] = 
         Scoring::Params::TRIPLED_PAWNS[Scoring::Midgame][7-i] =
         PARAM(TRIPLED_PAWNS_MID1+i);
      Scoring::Params::TRIPLED_PAWNS[Scoring::Endgame][i] = 
         Scoring::Params::TRIPLED_PAWNS[Scoring::Endgame][7-i] = 
         PARAM(TRIPLED_PAWNS_END1+i);
      Scoring::Params::ISOLATED_PAWN[Scoring::Midgame][i] = 
         Scoring::Params::ISOLATED_PAWN[Scoring::Midgame][7-i] =
         PARAM(ISOLATED_PAWN_MID1+i);      
      Scoring::Params::ISOLATED_PAWN[Scoring::Endgame][i] = 
         Scoring::Params::ISOLATED_PAWN[Scoring::Endgame][7-i] = 
         PARAM(ISOLATED_PAWN_END1+i);
   }
   for (int p = 0; p < 2; p++) {
      for (int i = 0; i < 21; i++) {
         Scoring::Params::PP_OWN_PIECE_BLOCK[p][i] =
            p == 0 ?
            PARAM(PP_OWN_PIECE_BLOCK_MID+i) :
            PARAM(PP_OWN_PIECE_BLOCK_END+i);
         Scoring::Params::PP_OPP_PIECE_BLOCK[p][i] =
            p == 0 ?
            PARAM(PP_OPP_PIECE_BLOCK_MID+i) :
            PARAM(PP_OPP_PIECE_BLOCK_END+i);
      }
   }

   for (int i = 0; i < 9; i++) {
      Scoring::Params::KNIGHT_MOBILITY[i] = PARAM(KNIGHT_MOBILITY+i);
   }
   for (int i = 0; i < 15; i++) {
      Scoring::Params::BISHOP_MOBILITY[i] = PARAM(BISHOP_MOBILITY+i);
   }
   for (int i = 0; i < 15; i++) {
      Scoring::Params::ROOK_MOBILITY[0][i] = PARAM(ROOK_MOBILITY_MIDGAME+i);
      Scoring::Params::ROOK_MOBILITY[1][i] = PARAM(ROOK_MOBILITY_ENDGAME+i);
   }
   for (int i = 0; i < 29; i++) {
      Scoring::Params::QUEEN_MOBILITY[0][i] = PARAM(QUEEN_MOBILITY_MIDGAME+i);
      Scoring::Params::QUEEN_MOBILITY[1][i] = PARAM(QUEEN_MOBILITY_ENDGAME+i);
   }
   for (int i = 0; i < 5; i++) {
      Scoring::Params::KING_MOBILITY_ENDGAME[i] = PARAM(KING_MOBILITY_ENDGAME+i);
   }

   for (int i = 0; i < 32; i++) {
      apply_to_pst(i,PARAM(KNIGHT_PST_MIDGAME+i),Scoring::Params::KNIGHT_PST[0]);
      apply_to_pst(i,PARAM(KNIGHT_PST_ENDGAME+i),Scoring::Params::KNIGHT_PST[1]);
      apply_to_pst(i,PARAM(BISHOP_PST_MIDGAME+i),Scoring::Params::BISHOP_PST[0]);
      apply_to_pst(i,PARAM(BISHOP_PST_ENDGAME+i),Scoring::Params::BISHOP_PST[1]);
      apply_to_pst(i,PARAM(ROOK_PST_MIDGAME+i),Scoring::Params::ROOK_PST[0]);
      apply_to_pst(i,PARAM(ROOK_PST_ENDGAME+i),Scoring::Params::ROOK_PST[1]);
      apply_to_pst(i,PARAM(QUEEN_PST_MIDGAME+i),Scoring::Params::QUEEN_PST[0]);
      apply_to_pst(i,PARAM(QUEEN_PST_ENDGAME+i),Scoring::Params::QUEEN_PST[1]);
      apply_to_pst(i,PARAM(KING_PST_MIDGAME+i),Scoring::Params::KING_PST[0]);
      apply_to_pst(i,PARAM(KING_PST_ENDGAME+i),Scoring::Params::KING_PST[1]);
   }
   memset(Scoring::Params::KNIGHT_OUTPOST,'\0',3*64*sizeof(int));
   int index = 0;
   for (int p = 0; p < 3; p++) {
      for (int i = 0; i < 16; i++,index++) {
         apply_to_outpost(i,PARAM(KNIGHT_OUTPOST+index),Scoring::Params::KNIGHT_OUTPOST[p]);
      }
   }
   memset(Scoring::Params::BISHOP_OUTPOST,'\0',3*64*sizeof(int));
   index = 0;
   for (int p = 0; p < 3; p++) {
      for (int i = 0; i < 16; i++,index++) {
         apply_to_outpost(i,PARAM(BISHOP_OUTPOST+index),Scoring::Params::BISHOP_OUTPOST[p]);
      }
   }
   
}

void Tune::writeX0(ostream &o) 
{
   for (int i=0; i < numTuningParams(); i++) {
      TuneParam p;
      getParam(i,p);
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
      string name = in.substr(0,pos);
      int index = -1;
      for (unsigned i = 0; i < tune_params.size(); i++) {
         if (tune_params[i].name == name) {
            index = (int)i;
            break;
         }
      }
      if (index == -1) {
         cerr << "invalid param name found in input file: " << name << endl;
      } else {
         stringstream valstream(in.substr(pos+1,in.size()));
         int val;
         valstream >> val;
         if (!valstream.bad() && !valstream.fail()) {
            updateParamValue(index,val);
         } else {
            cerr << "error parsing value for parameter " << name << endl;
         }
      }
   }
}

void Tune::getParam(int index, TuneParam &param) const
{
   param = tune_params[index];
}

score_t Tune::getParamValue(int index) const
{
   return tune_params[index].current;
}

void Tune::updateParamValue(int index, score_t value) 
{
   tune_params[index].current = value;
}

// return index for parameter given name, -1 if not not found
int Tune::findParamByName(const string &name) const {
   int i = 0;
   for (vector<TuneParam>::const_iterator it = tune_params.begin();
        it != tune_params.end();
        it++,i++) {
      if ((*it).name == name) {
         return i;
      }
   }
   return -1;
}

double Tune::scale(score_t value,int index,int materialLevel) const
{
   switch (tune_params[index].scaling) {
   case Tune::TuneParam::Any:
      return value;
   case Tune::TuneParam::Midgame:   
      return (value*Scoring::Params::MATERIAL_SCALE[materialLevel])/128.0;
   case Tune::TuneParam::Endgame:
      return (value*(128-Scoring::Params::MATERIAL_SCALE[materialLevel]))/128.0;
   case Tune::TuneParam::None:
      return 0;
   }
   return 0;
}

