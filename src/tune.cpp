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
   KING_ATTACK_PARAM4,
   KING_ATTACK_PARAM5,
   KING_ATTACK_PARAM6,
   KING_ATTACK_PARAM7,
   KING_ATTACK_PARAM8,
   KING_ATTACK_PARAM9,
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
   KING_ATTACK_SIGMOID_MID,
   KING_ATTACK_SIGMOID_DIV,
   KING_ATTACK_SIGMOID_MULT,
   TRADE_DOWN_LINEAR,
   TRADE_DOWN_SQ,
   PASSED_PAWN1,
   PASSED_PAWN2,
   PASSED_PAWN3,
   PASSED_PAWN4,
   PASSED_PAWN5,
   PASSED_PAWN6,
   PASSED_PAWN7,
   PASSED_PAWN_ENDGAME_FACTOR,
   POTENTIAL_PASSER1,
   POTENTIAL_PASSER2,
   POTENTIAL_PASSER3,
   POTENTIAL_PASSER4,
   POTENTIAL_PASSER5,
   POTENTIAL_PASSER6,
   POTENTIAL_PASSER_ENDGAME_FACTOR,
   CONNECTED_PASSERS1,
   CONNECTED_PASSERS2,
   CONNECTED_PASSERS3,
   CONNECTED_PASSERS4,
   CONNECTED_PASSERS5,
   CONNECTED_PASSERS6,
   CONNECTED_PASSERS7,
   CONNECTED_PASSERS_ENDGAME_FACTOR,
   ADJACENT_PASSERS1,
   ADJACENT_PASSERS2,
   ADJACENT_PASSERS3,
   ADJACENT_PASSERS4,
   ADJACENT_PASSERS5,
   ADJACENT_PASSERS6,
   ADJACENT_PASSERS7,
   ADJACENT_PASSERS_ENDGAME_FACTOR,
   DOUBLED_PAWNS1,
   DOUBLED_PAWNS2,
   DOUBLED_PAWNS3,
   DOUBLED_PAWNS4,
   DOUBLED_PAWNS_ENDGAME_FACTOR,
   ISOLATED_PAWN1,
   ISOLATED_PAWN2,
   ISOLATED_PAWN3,
   ISOLATED_PAWN4,
   ISOLATED_PAWN_ENDGAME_FACTOR
};
   
tune::TuneParam tune::tune_params[tune::NUM_TUNING_PARAMS] = {
   tune::TuneParam(RB_ADJUST1,"rb_adjust1",250,-100,400),
   tune::TuneParam(RB_ADJUST2,"rb_adjust2",75,-300,400),
   tune::TuneParam(RB_ADJUST3,"rb_adjust3",-75,-400,300),
   tune::TuneParam(RB_ADJUST4,"rb_adjust4",-250,-500,150),
   tune::TuneParam(RBN_ADJUST1,"rbn_adjust1",500,250,750),
   tune::TuneParam(RBN_ADJUST2,"rbn_adjust2",675,300,900),
   tune::TuneParam(RBN_ADJUST3,"rbn_adjust3",825,500,1200),
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
   tune::TuneParam(KING_COVER0,"king_cover0",230,100,320),
   tune::TuneParam(KING_COVER1,"king_cover1",293,100,450),
   tune::TuneParam(KING_COVER2,"king_cover2",116,50,200),
   tune::TuneParam(KING_COVER3,"king_cover3",27,0,100),
   tune::TuneParam(KING_COVER4,"king_cover4",25,0,100),
   tune::TuneParam(KING_FILE_OPEN,"king_file_open",-139,-300,0),
   tune::TuneParam(KING_DISTANCE_BASIS,"king_distance_basis",312,200,400),
   tune::TuneParam(KING_DISTANCE_MULT,"king_distance_mult",77,40,120),
   tune::TuneParam(PIN_MULTIPLIER_MID,"pin_multiplier_mid",227,0,500),
   tune::TuneParam(PIN_MULTIPLIER_END,"pin_multiplier_end",289,0,500),
   tune::TuneParam(KING_ATTACK_PARAM1,"king_attack_param1",539,-200,800),
   tune::TuneParam(KING_ATTACK_PARAM2,"king_attack_param2",0,0,250),
   tune::TuneParam(KING_ATTACK_PARAM3,"king_attack_param3",0,-200,500),
   tune::TuneParam(KING_ATTACK_PARAM4,"king_attack_param4",0,0,250),
   tune::TuneParam(KING_ATTACK_PARAM5,"king_attack_param5",404,-200,800),
   tune::TuneParam(KING_ATTACK_PARAM6,"king_attack_param6",0,0,2000),
   tune::TuneParam(KING_ATTACK_PARAM7,"king_attack_param7",283,0,640),
   tune::TuneParam(KING_ATTACK_PARAM8,"king_attack_param8",283,0,2500),
   tune::TuneParam(KING_ATTACK_PARAM9,"king_attack_param9",100,0,2500),
   tune::TuneParam(KING_ATTACK_BOOST_THRESHOLD,"king_attack_boost_threshold",524,100,960),
   tune::TuneParam(KING_ATTACK_BOOST_DIVISOR,"king_attack_boost_divisor",481,100,1000),
   tune::TuneParam(BISHOP_TRAPPED,"bishop_trapped",-1470,-2000,-400),
   tune::TuneParam(BISHOP_PAIR_MID,"bishop_pair_mid",447,100,600),
   tune::TuneParam(BISHOP_PAIR_END,"bishop_pair_end",577,125,750),
//   tune::TuneParam("bishop_pawn_placement_mid",-1,-200,0),
   tune::TuneParam(BISHOP_PAWN_PLACEMENT_END,"bishop_pawn_placement_end",-170,-250,0),
   tune::TuneParam(BAD_BISHOP_MID,"bad_bishop_mid",-44,-80,0),
   tune::TuneParam(BAD_BISHOP_END,"bad_bishop_end",-66,-120,0),
   tune::TuneParam(OUTPOST_NOT_DEFENDED,"outpost_not_defended",44,0,64),
   tune::TuneParam(CENTER_PAWN_BLOCK,"center_pawn_block",-127,-300,0),
   tune::TuneParam(OUTSIDE_PASSER_MID,"outside_passer_mid",110,0,250),
   tune::TuneParam(OUTSIDE_PASSER_END,"outside_passer_end",234,0,500),
   tune::TuneParam(WEAK_PAWN_MID,"weak_pawn_mid",-80,-250,0),
   tune::TuneParam(WEAK_PAWN_END,"weak_pawn_end",-80,-250,0),
   tune::TuneParam(WEAK_ON_OPEN_FILE_MID,"weak_on_open_file_mid",-112,-250,0),
   tune::TuneParam(WEAK_ON_OPEN_FILE_END,"weak_on_open_file_end",-112,-250,0),
   tune::TuneParam(SPACE,"space",16,0,80),
   tune::TuneParam(PAWN_CENTER_SCORE_MID,"pawn_center_score_mid",27,0,100),
   tune::TuneParam(ROOK_ON_7TH_MID,"rook_on_7th_mid",235,0,800),
   tune::TuneParam(ROOK_ON_7TH_END,"rook_on_7th_end",251,0,800),
   tune::TuneParam(TWO_ROOKS_ON_7TH_MID,"two_rooks_on_7th_mid",610,0,1200),
   tune::TuneParam(TWO_ROOKS_ON_7TH_END,"two_rooks_on_7th_end",610,0,1200),
   tune::TuneParam(ROOK_ON_OPEN_FILE_MID,"rook_on_open_file_mid",170,0,600),
   tune::TuneParam(ROOK_ON_OPEN_FILE_END,"rook_on_open_file_end",182,0,600),
   tune::TuneParam(ROOK_ATTACKS_WEAK_PAWN_MID,"rook_attacks_weak_pawn_mid",71,0,600),
   tune::TuneParam(ROOK_ATTACKS_WEAK_PAWN_END,"rook_attacks_weak_pawn_end",119,0,600),
   tune::TuneParam(ROOK_BEHIND_PP_MID,"rook_behind_pp_mid",25,0,600),
   tune::TuneParam(ROOK_BEHIND_PP_END,"rook_behind_pp_end",78,0,600),
   tune::TuneParam(QUEEN_OUT,"queen_out",-68,-200,0),
   tune::TuneParam(PASSER_OWN_PIECE_BLOCK_MID,"passer_own_piece_block_mid",-15,-200,0),
   tune::TuneParam(PASSER_OWN_PIECE_BLOCK_END,"passer_own_piece_block_end",-43,-200,0 ),
   tune::TuneParam(PP_BLOCK_BASE_MID,"pp_block_base_mid",149,0,280),
   tune::TuneParam(PP_BLOCK_BASE_END,"pp_block_base_end",132,0,280),
   tune::TuneParam(PP_BLOCK_MULT_MID,"pp_block_mult_mid",97,0,180),
   tune::TuneParam(PP_BLOCK_MULT_END,"pp_block_mult_end",44,0,80),
   tune::TuneParam(ENDGAME_PAWN_BONUS,"endgame_pawn_bonus",110,0,250),
   tune::TuneParam(KING_NEAR_PASSER,"king_near_passer",224,0,500),
   tune::TuneParam(OPP_KING_NEAR_PASSER,"opp_king_near_passer",-258,-500,0),
   tune::TuneParam(PAWN_SIDE_BONUS,"pawn_side_bonus",306,0,500),
   tune::TuneParam(SUPPORTED_PASSER6,"supported_passer6",401,0,750),
   tune::TuneParam(SUPPORTED_PASSER7,"supported_passer7",728,0,1500),
   tune::TuneParam(SIDE_PROTECTED_PAWN,"side_protected_pawn",-92,-500,0),
   tune::TuneParam(KING_ATTACK_SIGMOID_MID,"king_attack_sigmoid_mid",210,50,350),
   tune::TuneParam(KING_ATTACK_SIGMOID_DIV,"king_attack_sigmoid_div",180,100,500),
   tune::TuneParam(KING_ATTACK_SIGMOID_MULT,"king_attack_sigmoid_mult",156,80,300),
   tune::TuneParam(TRADE_DOWN_LINEAR,"trade_down_linear",43,0,150),
   tune::TuneParam(TRADE_DOWN_SQ,"trade_down_sq",0,0,150),
   tune::TuneParam(PASSED_PAWN1,"passed_pawn1",9,0,200),
   tune::TuneParam(PASSED_PAWN2,"passed_pawn2",51,0,500),
   tune::TuneParam(PASSED_PAWN3,"passed_pawn3",92,50,500),
   tune::TuneParam(PASSED_PAWN4,"passed_pawn4",198,70,500),
   tune::TuneParam(PASSED_PAWN5,"passed_pawn5",287,90,500),
   tune::TuneParam(PASSED_PAWN6,"passed_pawn6",534,200,1000),
   tune::TuneParam(PASSED_PAWN7,"passed_pawn7",1046,500,1500),
   tune::TuneParam(PASSED_PAWN_ENDGAME_FACTOR,"passed_pawn_endgame_factor",159,100,250),
   tune::TuneParam(POTENTIAL_PASSER1,"potential_passer1",10,0,200),
   tune::TuneParam(POTENTIAL_PASSER2,"potential_passer2",26,0,200),
   tune::TuneParam(POTENTIAL_PASSER3,"potential_passer3",37,0,200),
   tune::TuneParam(POTENTIAL_PASSER4,"potential_passer4",75,0,300),
   tune::TuneParam(POTENTIAL_PASSER5,"potential_passer5",75,0,500),
   tune::TuneParam(POTENTIAL_PASSER6,"potential_passer6",236,0,750),
   tune::TuneParam(POTENTIAL_PASSER_ENDGAME_FACTOR,"potential_passer_pawn_endgame_factor",197,100,300),
   tune::TuneParam(CONNECTED_PASSERS1,"connected_passers1",0,0,200),
   tune::TuneParam(CONNECTED_PASSERS2,"connected_passers2",0,0,250),
   tune::TuneParam(CONNECTED_PASSERS3,"connected_passers3",88,50,300),
   tune::TuneParam(CONNECTED_PASSERS4,"connected_passers4",206,70,400),
   tune::TuneParam(CONNECTED_PASSERS5,"connected_passers5",227,90,500),
   tune::TuneParam(CONNECTED_PASSERS6,"connected_passers6",531,200,1000),
   tune::TuneParam(CONNECTED_PASSERS7,"connected_passers7",771,70,1500),
   tune::TuneParam(CONNECTED_PASSERS_ENDGAME_FACTOR,"connected_passers_pawn_endgame_factor",134,70,200),
   tune::TuneParam(ADJACENT_PASSERS1,"adjacent_passers1",0,0,200),
   tune::TuneParam(ADJACENT_PASSERS2,"adjacent_passers2",0,0,300),
   tune::TuneParam(ADJACENT_PASSERS3,"adjacent_passers3",100,40,500),
   tune::TuneParam(ADJACENT_PASSERS4,"adjacent_passers4",150,70,500),
   tune::TuneParam(ADJACENT_PASSERS5,"adjacent_passers5",150,80,500),
   tune::TuneParam(ADJACENT_PASSERS6,"adjacent_passers6",306,170,1000),
   tune::TuneParam(ADJACENT_PASSERS7,"adjacent_passers7",771,350,1500),
   tune::TuneParam(ADJACENT_PASSERS_ENDGAME_FACTOR,"adjacent_passers_endgame_factor",133,70,250),
   tune::TuneParam(DOUBLED_PAWNS1,"double_pawns1",-47,-250,0),
   tune::TuneParam(DOUBLED_PAWNS2,"double_pawns2",-70,-250,0),
   tune::TuneParam(DOUBLED_PAWNS3,"double_pawns3",-108,-250,0),
   tune::TuneParam(DOUBLED_PAWNS4,"double_pawns4",-108,-250,0),
   tune::TuneParam(DOUBLED_PAWNS_ENDGAME_FACTOR,"double_pawns_endgame_factor",226,70,300),
   tune::TuneParam(ISOLATED_PAWN1,"isolated_pawn1",-70,-250,0),
   tune::TuneParam(ISOLATED_PAWN2,"isolated_pawn2",-70,-250,0),
   tune::TuneParam(ISOLATED_PAWN3,"isolated_pawn3",-70,-250,0),
   tune::TuneParam(ISOLATED_PAWN4,"isolated_pawn4",-95,-250,0),
   tune::TuneParam(ISOLATED_PAWN_ENDGAME_FACTOR,"isolated_pawn_endgame_factor",146,70,300)
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

static int potential_passer_score(int i, int base, int slope, int power, int rank6_bonus) 
{
   if (i == 0) return 0;

   int pp = base;
   if(i>1) {
      pp += int(slope*pow(double(i-1),double(power)/32.0))/4;
      if (i == 6) pp += rank6_bonus;
   }
   return pp;
}

static int passer_score2(int i, int base, int slope, int sq_slope,
                         int rank6_bonus, int rank7_bonus) 
{
   if (i == 0) return 0;
   int pp = base;
   if(i>1) {
      pp += (slope*(i-1)+(sq_slope*(i-1)*(i-1)))/4;
      if (i >= 6) pp += rank6_bonus;
      if (i == 7) pp += rank7_bonus;
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
         //cerr << "resetting to " << tune::tune_params[i].min_value << endl;
         //tune::tune_params[i].current = tune::tune_params[i].min_value;
      }
      if (tune::tune_params[i].current > tune::tune_params[i].max_value) {
         cerr << "warning: param " << tune_params[i].name << " has current > max" << endl;
         //cerr << "resetting to " << tune::tune_params[i].max_value << endl;
         //tune::tune_params[i].current = tune::tune_params[i].max_value;
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

   memset(Scoring::Params::PASSED_PAWN[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::PASSED_PAWN[1],'\0',sizeof(int)*8);
   for (int i = 1; i < 8; i++) {
      Scoring::Params::PASSED_PAWN[Scoring::Midgame][i] = PARAM(PASSED_PAWN1+i-1);
      Scoring::Params::PASSED_PAWN[Scoring::Endgame][i] = PARAM(PASSED_PAWN1+i-1)*PARAM(PASSED_PAWN_ENDGAME_FACTOR)/128;
   }
   memset(Scoring::Params::POTENTIAL_PASSER[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::POTENTIAL_PASSER[1],'\0',sizeof(int)*8);
   for (int i = 1; i < 7; i++) {
      Scoring::Params::POTENTIAL_PASSER[Scoring::Midgame][i] = PARAM(POTENTIAL_PASSER1+i-1);
      Scoring::Params::POTENTIAL_PASSER[Scoring::Endgame][i] = PARAM(POTENTIAL_PASSER1+i-1)*PARAM(POTENTIAL_PASSER_ENDGAME_FACTOR)/128;
   }
   memset(Scoring::Params::CONNECTED_PASSERS[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::CONNECTED_PASSERS[1],'\0',sizeof(int)*8);
   for (int i = 1; i < 8; i++) {
      Scoring::Params::CONNECTED_PASSERS[Scoring::Midgame][i] = PARAM(CONNECTED_PASSERS1+i-1);
      Scoring::Params::CONNECTED_PASSERS[Scoring::Endgame][i] = PARAM(CONNECTED_PASSERS1+i-1)*PARAM(CONNECTED_PASSERS_ENDGAME_FACTOR)/128;
   }
   memset(Scoring::Params::ADJACENT_PASSERS[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::ADJACENT_PASSERS[1],'\0',sizeof(int)*8);
   for (int i = 1; i < 8; i++) {
      Scoring::Params::ADJACENT_PASSERS[Scoring::Midgame][i] = PARAM(ADJACENT_PASSERS1+i-1);
      Scoring::Params::ADJACENT_PASSERS[Scoring::Endgame][i] = PARAM(ADJACENT_PASSERS1+i-1)*PARAM(ADJACENT_PASSERS_ENDGAME_FACTOR)/128;
   }
   memset(Scoring::Params::DOUBLED_PAWNS[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::DOUBLED_PAWNS[1],'\0',sizeof(int)*8);
   memset(Scoring::Params::ISOLATED_PAWN[0],'\0',sizeof(int)*8);
   memset(Scoring::Params::ISOLATED_PAWN[1],'\0',sizeof(int)*8);
   for (int i = 0; i < 4; i++) {
      Scoring::Params::DOUBLED_PAWNS[Scoring::Midgame][i] = 
         Scoring::Params::DOUBLED_PAWNS[Scoring::Midgame][7-i] =
         PARAM(DOUBLED_PAWNS1+i);
      Scoring::Params::DOUBLED_PAWNS[Scoring::Endgame][i] = 
         Scoring::Params::DOUBLED_PAWNS[Scoring::Endgame][7-i] = 
         PARAM(DOUBLED_PAWNS1+i)*PARAM(DOUBLED_PAWNS_ENDGAME_FACTOR)/128;
      Scoring::Params::ISOLATED_PAWN[Scoring::Midgame][i] = 
         Scoring::Params::ISOLATED_PAWN[Scoring::Midgame][7-i] =
         PARAM(ISOLATED_PAWN1+i);      
      Scoring::Params::ISOLATED_PAWN[Scoring::Endgame][i] = 
         Scoring::Params::ISOLATED_PAWN[Scoring::Endgame][7-i] = 
         PARAM(ISOLATED_PAWN1+i)*PARAM(ISOLATED_PAWN_ENDGAME_FACTOR)/128;
   }
   for (int i = 0; i < 512; i++) {
      Scoring::Params::KING_ATTACK_SCALE[i] =
         int(PARAM(KING_ATTACK_SIGMOID_MULT)*i/
             (1.0+sqrt(1.0+pow((i-PARAM(KING_ATTACK_SIGMOID_MID))/PARAM(KING_ATTACK_SIGMOID_DIV),2.0)))/100.0);
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



