// Copyright 2015, 2016 by Jon Dart. All Rights Reserved.
//
#ifndef _PARAMS_H
#define _PARAMS_H

#include "types.h"

#ifdef TUNE
// scoring parameters that are not const, so can be
// modified during tuning
#define PARAM_MOD
#else
// parameters are const so can be optimized better
#define PARAM_MOD const
#endif

#ifdef __INTEL_COMPILER
#pragma pack(push,1)
#endif

struct Params 
BEGIN_PACKED_STRUCT
    
    static const int KING_ATTACK_SCALE_SIZE = 90;
    static const int KING_ATTACK_FACTOR_RESOLUTION = 4;

    static PARAM_MOD int RB_ADJUST[4];
    static PARAM_MOD int RBN_ADJUST[4];
    static PARAM_MOD int QR_ADJUST[4];
    static PARAM_MOD int KN_VS_PAWN_ADJUST[3];
    static PARAM_MOD int CASTLING[6];
    static PARAM_MOD int KING_COVER[5];
    static PARAM_MOD int KING_COVER_BASE;
    static PARAM_MOD int KING_FILE_OPEN;
    static PARAM_MOD int KING_DISTANCE_BASIS;
    static PARAM_MOD int KING_DISTANCE_MULT;
    static PARAM_MOD int PIN_MULTIPLIER_MID;
    static PARAM_MOD int PIN_MULTIPLIER_END;
    static PARAM_MOD int ROOK_VS_PAWNS;
    static PARAM_MOD int KRMINOR_VS_R;
    static PARAM_MOD int KRMINOR_VS_R_NO_PAWNS;
    static PARAM_MOD int KQMINOR_VS_Q;
    static PARAM_MOD int KQMINOR_VS_Q_NO_PAWNS;
    static PARAM_MOD int MINOR_FOR_PAWNS;
    static PARAM_MOD int ENDGAME_PAWN_ADVANTAGE;
    static PARAM_MOD int PAWN_ENDGAME1;
    static PARAM_MOD int PAWN_ENDGAME2;
    static PARAM_MOD int PAWN_ATTACK_FACTOR1;
    static PARAM_MOD int PAWN_ATTACK_FACTOR2;
    static PARAM_MOD int MINOR_ATTACK_FACTOR;
    static PARAM_MOD int MINOR_ATTACK_BOOST;
    static PARAM_MOD int ROOK_ATTACK_FACTOR;
    static PARAM_MOD int QUEEN_ATTACK_FACTOR;
    static PARAM_MOD int ROOK_ATTACK_BOOST;
    static PARAM_MOD int QUEEN_ATTACK_BOOST;
    static PARAM_MOD int QUEEN_ATTACK_BOOST2;
    static PARAM_MOD int PAWN_THREAT_ON_PIECE_MID;
    static PARAM_MOD int PAWN_THREAT_ON_PIECE_END;
    static PARAM_MOD int PIECE_THREAT_MM_MID;
    static PARAM_MOD int PIECE_THREAT_MR_MID;
    static PARAM_MOD int PIECE_THREAT_MQ_MID;
    static PARAM_MOD int PIECE_THREAT_MM_END;
    static PARAM_MOD int PIECE_THREAT_MR_END;
    static PARAM_MOD int PIECE_THREAT_MQ_END;
    static PARAM_MOD int MINOR_PAWN_THREAT_END;
    static PARAM_MOD int MINOR_PAWN_THREAT_MID;
    static PARAM_MOD int PIECE_THREAT_RM_MID;
    static PARAM_MOD int PIECE_THREAT_RR_MID;
    static PARAM_MOD int PIECE_THREAT_RQ_MID;
    static PARAM_MOD int PIECE_THREAT_RM_END;
    static PARAM_MOD int PIECE_THREAT_RR_END;
    static PARAM_MOD int PIECE_THREAT_RQ_END;
    static PARAM_MOD int ROOK_PAWN_THREAT_MID;
    static PARAM_MOD int ROOK_PAWN_THREAT_END;
    static PARAM_MOD int ENDGAME_KING_THREAT;
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
    static PARAM_MOD int ROOK_BEHIND_PP_MID;
    static PARAM_MOD int ROOK_BEHIND_PP_END;
    static PARAM_MOD int QUEEN_OUT;
    static PARAM_MOD int PAWN_SIDE_BONUS;
    static PARAM_MOD int KING_OWN_PAWN_DISTANCE;
    static PARAM_MOD int KING_OPP_PAWN_DISTANCE;
    static PARAM_MOD int SUPPORTED_PASSER6;
    static PARAM_MOD int SUPPORTED_PASSER7;
    static PARAM_MOD int SIDE_PROTECTED_PAWN;

    // The following tables are computed from tuning parameters.
    static PARAM_MOD int KING_ATTACK_COUNT_BOOST[3];
    static PARAM_MOD int KING_OPP_PASSER_DISTANCE[6];
    static PARAM_MOD int KING_POSITION_LOW_MATERIAL[3];
    static PARAM_MOD int TRADE_DOWN[16];
    static PARAM_MOD int KING_ATTACK_SCALE[KING_ATTACK_SCALE_SIZE];
    static PARAM_MOD int PASSED_PAWN[2][8];
    static PARAM_MOD int POTENTIAL_PASSER[2][8];
    static PARAM_MOD int CONNECTED_PASSER[2][8];
    static PARAM_MOD int ADJACENT_PASSER[2][8];
    static PARAM_MOD int PP_OWN_PIECE_BLOCK[2][21];
    static PARAM_MOD int PP_OPP_PIECE_BLOCK[2][21];
    static PARAM_MOD int DOUBLED_PAWNS[2][8];
    static PARAM_MOD int TRIPLED_PAWNS[2][8];
    static PARAM_MOD int ISOLATED_PAWN[2][8];

    // Piece/square tables
    static PARAM_MOD int KNIGHT_PST[2][64];
    static PARAM_MOD int BISHOP_PST[2][64];
    static PARAM_MOD int ROOK_PST[2][64];
    static PARAM_MOD int QUEEN_PST[2][64];
    static PARAM_MOD int KING_PST[2][64];

    // mobility tables
    static PARAM_MOD int KNIGHT_MOBILITY[9];
    static PARAM_MOD int BISHOP_MOBILITY[15];
    static PARAM_MOD int ROOK_MOBILITY[2][15];
    static PARAM_MOD int QUEEN_MOBILITY[2][29];
    static PARAM_MOD int KING_MOBILITY_ENDGAME[5];

    // outpost scores
    static PARAM_MOD int KNIGHT_OUTPOST[3][64];
    static PARAM_MOD int BISHOP_OUTPOST[3][64];

    // not tuned presently (fixed)
    static const int MATERIAL_SCALE[32];
    static const int MIDGAME_THRESHOLD;
    static const int ENDGAME_THRESHOLD;

#ifdef TUNE
    static void write(ostream &);
#endif
END_PACKED_STRUCT

#ifdef __INTEL_COMPILER
#pragma pack(pop)
#endif

#endif
