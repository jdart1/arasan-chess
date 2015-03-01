// Copyright 2015 by Jon Dart. All Rights Reserved.
//
#ifndef _PARAMS_H
#define _PARAMS_H

#ifdef TUNE
// scoring parameters that are not const, so can be
// modified during tuning
#define PARAM_MOD
#else
// parameters are const so can be optimized better
#define PARAM_MOD const
#endif

  struct Params {
    
    // number of fixed parameters that map 1-1 to tuning paramaters
    static const int PARAM_ARRAY_SIZE = 79;

    static PARAM_MOD int RB_ADJUST1;
    static PARAM_MOD int RB_ADJUST2;
    static PARAM_MOD int RB_ADJUST3;
    static PARAM_MOD int RB_ADJUST4;
    static PARAM_MOD int RBN_ADJUST1;
    static PARAM_MOD int RBN_ADJUST2;
    static PARAM_MOD int RBN_ADJUST3;
    static PARAM_MOD int RBN_ADJUST4;
    static PARAM_MOD int QR_ADJUST0;
    static PARAM_MOD int QR_ADJUST1;
    static PARAM_MOD int QR_ADJUST2;
    static PARAM_MOD int QR_ADJUST3;
    static PARAM_MOD int KN_VS_PAWN_ADJUST0;
    static PARAM_MOD int KN_VS_PAWN_ADJUST1;
    static PARAM_MOD int KN_VS_PAWN_ADJUST2;
    static PARAM_MOD int PAWN_TRADE0;
    static PARAM_MOD int PAWN_TRADE1;
    static PARAM_MOD int PAWN_TRADE2;
    static PARAM_MOD int CASTLING0;
    static PARAM_MOD int CASTLING1;
    static PARAM_MOD int CASTLING2;
    static PARAM_MOD int CASTLING3;
    static PARAM_MOD int CASTLING4;
    static PARAM_MOD int CASTLING5;
    static PARAM_MOD int KING_COVER0;
    static PARAM_MOD int KING_COVER1;
    static PARAM_MOD int KING_COVER2;
    static PARAM_MOD int KING_COVER3;
    static PARAM_MOD int KING_COVER4;
    static PARAM_MOD int KING_FILE_OPEN;
    static PARAM_MOD int KING_DISTANCE_BASIS;
    static PARAM_MOD int KING_DISTANCE_MULT;
    static PARAM_MOD int PIN_MULTIPLIER_MID;
    static PARAM_MOD int PIN_MULTIPLIER_END;
    static PARAM_MOD int KING_ATTACK_PARAM1;
    static PARAM_MOD int KING_ATTACK_PARAM2;
    static PARAM_MOD int KING_ATTACK_PARAM3;
    static PARAM_MOD int KING_ATTACK_BOOST_THRESHOLD;
    static PARAM_MOD int KING_ATTACK_BOOST_DIVISOR;
    static PARAM_MOD int BISHOP_TRAPPED;
    static PARAM_MOD int BISHOP_PAIR_MID;
    static PARAM_MOD int BISHOP_PAIR_END;
    static PARAM_MOD int BISHOP_PAWN_PLACEMENT_END;
    static PARAM_MOD int BAD_BISHOP_MID;
    static PARAM_MOD int BAD_BISHOP_END;
    static PARAM_MOD int OUTPOST_NOT_DEFENDED;
    static PARAM_MOD int CENTER_PAWN_BLOCK;
    static PARAM_MOD int OUTSIDE_PASSER_MID;
    static PARAM_MOD int OUTSIDE_PASSER_END;
    static PARAM_MOD int WEAK_PAWN_MID;
    static PARAM_MOD int WEAK_PAWN_END;
    static PARAM_MOD int WEAK_ON_OPEN_FILE_MID;
    static PARAM_MOD int WEAK_ON_OPEN_FILE_END;
    static PARAM_MOD int SPACE;
    static PARAM_MOD int PAWN_CENTER_SCORE_MID;
    static PARAM_MOD int ROOK_ON_7TH_MID;
    static PARAM_MOD int ROOK_ON_7TH_END;
    static PARAM_MOD int TWO_ROOKS_ON_7TH_MID;
    static PARAM_MOD int TWO_ROOKS_ON_7TH_END;
    static PARAM_MOD int ROOK_ON_OPEN_FILE_MID;
    static PARAM_MOD int ROOK_ON_OPEN_FILE_END;
    static PARAM_MOD int ROOK_ATTACKS_WEAK_PAWN_MID;
    static PARAM_MOD int ROOK_ATTACKS_WEAK_PAWN_END;
    static PARAM_MOD int ROOK_BEHIND_PP_MID;
    static PARAM_MOD int ROOK_BEHIND_PP_END;
    static PARAM_MOD int QUEEN_OUT;
    static PARAM_MOD int PASSER_OWN_PIECE_BLOCK_MID;
    static PARAM_MOD int PASSER_OWN_PIECE_BLOCK_END;
    static PARAM_MOD int PP_BLOCK_BASE_MID;
    static PARAM_MOD int PP_BLOCK_BASE_END;
    static PARAM_MOD int PP_BLOCK_MULT_MID;
    static PARAM_MOD int PP_BLOCK_MULT_END;
    static PARAM_MOD int ENDGAME_PAWN_BONUS;
    static PARAM_MOD int KING_NEAR_PASSER;
    static PARAM_MOD int OPP_KING_NEAR_PASSER;
    static PARAM_MOD int PAWN_SIDE_BONUS;
    static PARAM_MOD int SUPPORTED_PASSER6;
    static PARAM_MOD int SUPPORTED_PASSER7;
    static PARAM_MOD int SIDE_PROTECTED_PAWN;

    // The following tables are computed from tuning parameters.
    static PARAM_MOD int MATERIAL_SCALE[32];
    static PARAM_MOD int MIDGAME_THRESHOLD;
    static PARAM_MOD int ENDGAME_THRESHOLD;
    static PARAM_MOD int KNIGHT_PST[2][64];
    static PARAM_MOD int BISHOP_PST[2][64];
    static PARAM_MOD int KNIGHT_OUTPOST[64];
    static PARAM_MOD int BISHOP_OUTPOST[64];
    // TBD: rook PST
    static PARAM_MOD int TRADE_DOWN[16];
    static PARAM_MOD int KING_PST[2][64];
    static PARAM_MOD int KNIGHT_MOBILITY[9];
    static PARAM_MOD int BISHOP_MOBILITY[15];
    static PARAM_MOD int ROOK_MOBILITY[15];
    static PARAM_MOD int QUEEN_MOBILITY[2][29];
    static PARAM_MOD int KING_MOBILITY_ENDGAME[9];
    static PARAM_MOD int PASSED_PAWN[2][8];
    static PARAM_MOD int POTENTIAL_PASSER[2][8];
    static PARAM_MOD int CONNECTED_PASSERS[2][8];
    static PARAM_MOD int ADJACENT_PASSERS[2][8];
    static PARAM_MOD int DOUBLED_PAWNS[2][8];
    static PARAM_MOD int ISOLATED_PAWN[2][8];

#ifdef TUNE
    static void write(ostream &);
#endif
  };

#endif
