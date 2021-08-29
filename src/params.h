// Copyright 2015-2020 by Jon Dart. All Rights Reserved.
//
#ifndef _PARAMS_H
#define _PARAMS_H

#include "chess.h"

#ifdef TUNE
// scoring parameters that are not const, so can be
// modified during tuning
#define PARAM_MOD score_t
#else
// parameters are const so can be optimized better
#define PARAM_MOD const score_t
#endif

struct Params  {
    
    static const int KING_ATTACK_SCALE_SIZE = 150;
    static const int KING_ATTACK_FACTOR_RESOLUTION = 4;

    static constexpr score_t PAWN_VALUE = (score_t)128; // midgame pawn value
    static constexpr score_t KING_VALUE = (score_t)32*PAWN_VALUE;

    static const score_t SEE_PIECE_VALUES[7];

    static FORCEINLINE score_t Gain(Move move) {
        return (TypeOfMove(move) == Promotion) ?
                SEE_PIECE_VALUES[Capture(move)] + SEE_PIECE_VALUES[PromoteTo(move)] - SEE_PIECE_VALUES[Pawn] :
                SEE_PIECE_VALUES[Capture(move)];
    }

    static FORCEINLINE score_t MVV_LVA(Move move) {
        return 8*Gain(move) - SEE_PIECE_VALUES[PieceMoved(move)];
    }

    static PARAM_MOD PAWN_VALUE_MIDGAME;
    static PARAM_MOD PAWN_VALUE_ENDGAME;
    static PARAM_MOD KNIGHT_VALUE_MIDGAME;
    static PARAM_MOD KNIGHT_VALUE_ENDGAME;
    static PARAM_MOD BISHOP_VALUE_MIDGAME;
    static PARAM_MOD BISHOP_VALUE_ENDGAME;
    static PARAM_MOD ROOK_VALUE_MIDGAME;
    static PARAM_MOD ROOK_VALUE_ENDGAME;
    static PARAM_MOD QUEEN_VALUE_MIDGAME;
    static PARAM_MOD QUEEN_VALUE_ENDGAME;
    static PARAM_MOD KN_VS_PAWN_ADJUST[3];
    static PARAM_MOD MINOR_FOR_PAWNS_MIDGAME;
    static PARAM_MOD MINOR_FOR_PAWNS_ENDGAME;
    static PARAM_MOD RB_ADJUST_MIDGAME;
    static PARAM_MOD RB_ADJUST_ENDGAME;
    static PARAM_MOD RBN_ADJUST_MIDGAME;
    static PARAM_MOD RBN_ADJUST_ENDGAME;
    static PARAM_MOD QR_ADJUST_MIDGAME;
    static PARAM_MOD QR_ADJUST_ENDGAME;
    static PARAM_MOD Q_VS_3MINORS_MIDGAME;
    static PARAM_MOD Q_VS_3MINORS_ENDGAME;
    static PARAM_MOD CASTLING[6];
#ifdef TUNE
    static PARAM_MOD KING_ATTACK_SCALE_MAX;
    static PARAM_MOD KING_ATTACK_SCALE_INFLECT;
    static PARAM_MOD KING_ATTACK_SCALE_FACTOR;
    static PARAM_MOD KING_ATTACK_SCALE_BIAS;
#endif
    static PARAM_MOD KING_COVER[6][4];
    static PARAM_MOD KING_COVER_BASE;
    static PARAM_MOD KING_DISTANCE_BASIS;
    static PARAM_MOD KING_DISTANCE_MULT;
    static PARAM_MOD PIN_MULTIPLIER_MID;
    static PARAM_MOD PIN_MULTIPLIER_END;
    static PARAM_MOD KRMINOR_VS_R_NO_PAWNS;
    static PARAM_MOD KQMINOR_VS_Q_NO_PAWNS;
    static PARAM_MOD TRADE_DOWN1;
    static PARAM_MOD TRADE_DOWN2;
    static PARAM_MOD TRADE_DOWN3;
    static PARAM_MOD PAWN_ATTACK_FACTOR;
    static PARAM_MOD MINOR_ATTACK_FACTOR;
    static PARAM_MOD MINOR_ATTACK_BOOST;
    static PARAM_MOD ROOK_ATTACK_FACTOR;
    static PARAM_MOD ROOK_ATTACK_BOOST;
    static PARAM_MOD QUEEN_ATTACK_FACTOR;
    static PARAM_MOD QUEEN_ATTACK_BOOST;
    static PARAM_MOD OWN_PIECE_KING_PROXIMITY_MIN;
    static PARAM_MOD OWN_MINOR_KING_PROXIMITY;
    static PARAM_MOD OWN_ROOK_KING_PROXIMITY;
    static PARAM_MOD OWN_QUEEN_KING_PROXIMITY;

    static PARAM_MOD KING_ATTACK_COVER_BOOST_BASE;
    static PARAM_MOD KING_ATTACK_COVER_BOOST_SLOPE;
    static PARAM_MOD KING_ATTACK_COUNT;
    static PARAM_MOD KING_ATTACK_SQUARES;
    static PARAM_MOD PAWN_PUSH_THREAT_MID;
    static PARAM_MOD PAWN_PUSH_THREAT_END;
    static PARAM_MOD ENDGAME_KING_THREAT;
    static PARAM_MOD BISHOP_TRAPPED;
    static PARAM_MOD BISHOP_PAIR_MID;
    static PARAM_MOD BISHOP_PAIR_END;
    static PARAM_MOD BISHOP_PAWN_PLACEMENT_END;
    static PARAM_MOD BAD_BISHOP_MID;
    static PARAM_MOD BAD_BISHOP_END;
    static PARAM_MOD CENTER_PAWN_BLOCK;
    static PARAM_MOD OUTSIDE_PASSER_MID;
    static PARAM_MOD OUTSIDE_PASSER_END;
    static PARAM_MOD WEAK_PAWN_MID;
    static PARAM_MOD WEAK_PAWN_END;
    static PARAM_MOD WEAK_ON_OPEN_FILE_MID;
    static PARAM_MOD WEAK_ON_OPEN_FILE_END;
    static PARAM_MOD SPACE;
    static PARAM_MOD PAWN_CENTER_SCORE_MID;
    static PARAM_MOD ROOK_ON_7TH_MID;
    static PARAM_MOD ROOK_ON_7TH_END;
    static PARAM_MOD TWO_ROOKS_ON_7TH_MID;
    static PARAM_MOD TWO_ROOKS_ON_7TH_END;
    static PARAM_MOD TRAPPED_ROOK;
    static PARAM_MOD TRAPPED_ROOK_NO_CASTLE;
    static PARAM_MOD ROOK_ON_OPEN_FILE_MID;
    static PARAM_MOD ROOK_ON_OPEN_FILE_END;
    static PARAM_MOD ROOK_BEHIND_PP_MID;
    static PARAM_MOD ROOK_BEHIND_PP_END;
    static PARAM_MOD QUEEN_OUT;
    static PARAM_MOD PAWN_SIDE_BONUS;
    static PARAM_MOD KING_OWN_PAWN_DISTANCE;
    static PARAM_MOD KING_OPP_PAWN_DISTANCE;
    static PARAM_MOD SIDE_PROTECTED_PAWN;

    // The following tables are computed from tuning parameters.
    static PARAM_MOD KING_OPP_PASSER_DISTANCE[6];
    static PARAM_MOD KING_POSITION_LOW_MATERIAL[3];
    static PARAM_MOD KING_ATTACK_SCALE[KING_ATTACK_SCALE_SIZE];
    static PARAM_MOD OWN_PIECE_KING_PROXIMITY_MULT[16];
    static PARAM_MOD PASSED_PAWN[2][8];
    static PARAM_MOD PASSED_PAWN_FILE_ADJUST[4];
    static PARAM_MOD CONNECTED_PASSER[2][8];
    static PARAM_MOD ADJACENT_PASSER[2][8];
    static PARAM_MOD QUEENING_PATH_CLEAR[2][6];
    static PARAM_MOD PP_OWN_PIECE_BLOCK[2][3];
    static PARAM_MOD PP_OPP_PIECE_BLOCK[2][3];
    static PARAM_MOD QUEENING_PATH_CONTROL[2][3];
    static PARAM_MOD QUEENING_PATH_OPP_CONTROL[2][3];
    static PARAM_MOD DOUBLED_PAWNS[2][8];
    static PARAM_MOD TRIPLED_PAWNS[2][8];
    static PARAM_MOD ISOLATED_PAWN[2][8];

    // Threat scoring
    static PARAM_MOD THREAT_BY_PAWN[2][5];
    static PARAM_MOD THREAT_BY_KNIGHT[2][5];
    static PARAM_MOD THREAT_BY_BISHOP[2][5];
    static PARAM_MOD THREAT_BY_ROOK[2][5];

    // Piece/square tables
    static PARAM_MOD KNIGHT_PST[2][64];
    static PARAM_MOD BISHOP_PST[2][64];
    static PARAM_MOD ROOK_PST[2][64];
    static PARAM_MOD QUEEN_PST[2][64];
    static PARAM_MOD KING_PST[2][64];

    // mobility tables
    static PARAM_MOD KNIGHT_MOBILITY[9];
    static PARAM_MOD BISHOP_MOBILITY[15];
    static PARAM_MOD ROOK_MOBILITY[2][15];
    static PARAM_MOD QUEEN_MOBILITY[2][24];
    static PARAM_MOD KING_MOBILITY_ENDGAME[5];

    // outpost scores [phase][defended]
    static PARAM_MOD KNIGHT_OUTPOST[2][2];
    static PARAM_MOD BISHOP_OUTPOST[2][2];

    // pawn storm [blocked][file][distance]
    static PARAM_MOD PAWN_STORM[2][4][5];

    // not tuned presently (fixed)
    static const int MATERIAL_SCALE[32];
    static const int MIDGAME_THRESHOLD;
    static const int ENDGAME_THRESHOLD;

    static inline int foldFile(int f) {
        return f > 4 ? 8-f : f-1;
    }

#ifdef TUNE
    static void write(std::ostream &, const std::string &comment);
#endif
};

#endif
