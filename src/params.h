// Copyright 2015-2020, 2024 by Jon Dart. All Rights Reserved.
//
#ifndef _PARAMS_H
#define _PARAMS_H

#include "chess.h"

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

    static FORCEINLINE score_t maxValue(Move move) {
        return (TypeOfMove(move) == Promotion) ?
            SEE_PIECE_VALUES[Capture(move)] + SEE_PIECE_VALUES[PromoteTo(move)] - SEE_PIECE_VALUES[Pawn] :
            SEE_PIECE_VALUES[Capture(move)];
    }

    static const score_t PAWN_VALUE_MIDGAME;
    static const score_t PAWN_VALUE_ENDGAME;
    static const score_t KNIGHT_VALUE_MIDGAME;
    static const score_t KNIGHT_VALUE_ENDGAME;
    static const score_t BISHOP_VALUE_MIDGAME;
    static const score_t BISHOP_VALUE_ENDGAME;
    static const score_t ROOK_VALUE_MIDGAME;
    static const score_t ROOK_VALUE_ENDGAME;
    static const score_t QUEEN_VALUE_MIDGAME;
    static const score_t QUEEN_VALUE_ENDGAME;
    static const score_t KN_VS_PAWN_ADJUST[3];
    static const score_t MINOR_FOR_PAWNS_MIDGAME;
    static const score_t MINOR_FOR_PAWNS_ENDGAME;
    static const score_t RB_ADJUST_MIDGAME;
    static const score_t RB_ADJUST_ENDGAME;
    static const score_t RBN_ADJUST_MIDGAME;
    static const score_t RBN_ADJUST_ENDGAME;
    static const score_t QR_ADJUST_MIDGAME;
    static const score_t QR_ADJUST_ENDGAME;
    static const score_t Q_VS_3MINORS_MIDGAME;
    static const score_t Q_VS_3MINORS_ENDGAME;
    static const score_t CASTLING[6];
    static const score_t KING_COVER[6][4];
    static const score_t KING_COVER_BASE;
    static const score_t KING_DISTANCE_BASIS;
    static const score_t KING_DISTANCE_MULT;
    static const score_t PIN_MULTIPLIER_MID;
    static const score_t PIN_MULTIPLIER_END;
    static const score_t KRMINOR_VS_R_NO_PAWNS;
    static const score_t KQMINOR_VS_Q_NO_PAWNS;
    static const score_t TRADE_DOWN1;
    static const score_t TRADE_DOWN2;
    static const score_t TRADE_DOWN3;
    static const score_t PAWN_ATTACK_FACTOR;
    static const score_t MINOR_ATTACK_FACTOR;
    static const score_t MINOR_ATTACK_BOOST;
    static const score_t ROOK_ATTACK_FACTOR;
    static const score_t ROOK_ATTACK_BOOST;
    static const score_t QUEEN_ATTACK_FACTOR;
    static const score_t QUEEN_ATTACK_BOOST;
    static const score_t OWN_PIECE_KING_PROXIMITY_MIN;
    static const score_t OWN_MINOR_KING_PROXIMITY;
    static const score_t OWN_ROOK_KING_PROXIMITY;
    static const score_t OWN_QUEEN_KING_PROXIMITY;

    static const score_t KING_ATTACK_COVER_BOOST_BASE;
    static const score_t KING_ATTACK_COVER_BOOST_SLOPE;
    static const score_t KING_ATTACK_COUNT;
    static const score_t KING_ATTACK_SQUARES;
    static const score_t PAWN_PUSH_THREAT_MID;
    static const score_t PAWN_PUSH_THREAT_END;
    static const score_t ENDGAME_KING_THREAT;
    static const score_t BISHOP_TRAPPED;
    static const score_t BISHOP_PAIR_MID;
    static const score_t BISHOP_PAIR_END;
    static const score_t BISHOP_PAWN_PLACEMENT_END;
    static const score_t BAD_BISHOP_MID;
    static const score_t BAD_BISHOP_END;
    static const score_t CENTER_PAWN_BLOCK;
    static const score_t OUTSIDE_PASSER_MID;
    static const score_t OUTSIDE_PASSER_END;
    static const score_t WEAK_PAWN_MID;
    static const score_t WEAK_PAWN_END;
    static const score_t WEAK_ON_OPEN_FILE_MID;
    static const score_t WEAK_ON_OPEN_FILE_END;
    static const score_t SPACE;
    static const score_t PAWN_CENTER_SCORE_MID;
    static const score_t ROOK_ON_7TH_MID;
    static const score_t ROOK_ON_7TH_END;
    static const score_t TWO_ROOKS_ON_7TH_MID;
    static const score_t TWO_ROOKS_ON_7TH_END;
    static const score_t TRAPPED_ROOK;
    static const score_t TRAPPED_ROOK_NO_CASTLE;
    static const score_t ROOK_ON_OPEN_FILE_MID;
    static const score_t ROOK_ON_OPEN_FILE_END;
    static const score_t ROOK_BEHIND_PP_MID;
    static const score_t ROOK_BEHIND_PP_END;
    static const score_t QUEEN_OUT;
    static const score_t PAWN_SIDE_BONUS;
    static const score_t KING_OWN_PAWN_DISTANCE;
    static const score_t KING_OPP_PAWN_DISTANCE;
    static const score_t SIDE_PROTECTED_PAWN;

    // The following tables are computed from tuning parameters.
    static const score_t KING_OPP_PASSER_DISTANCE[6];
    static const score_t KING_POSITION_LOW_MATERIAL[3];
    static const score_t KING_ATTACK_SCALE[KING_ATTACK_SCALE_SIZE];
    static const score_t OWN_PIECE_KING_PROXIMITY_MULT[16];
    static const score_t PASSED_PAWN[2][8];
    static const score_t PASSED_PAWN_FILE_ADJUST[4];
    static const score_t CONNECTED_PASSER[2][8];
    static const score_t ADJACENT_PASSER[2][8];
    static const score_t QUEENING_PATH_CLEAR[2][6];
    static const score_t PP_OWN_PIECE_BLOCK[2][3];
    static const score_t PP_OPP_PIECE_BLOCK[2][3];
    static const score_t QUEENING_PATH_CONTROL[2][3];
    static const score_t QUEENING_PATH_OPP_CONTROL[2][3];
    static const score_t DOUBLED_PAWNS[2][8];
    static const score_t TRIPLED_PAWNS[2][8];
    static const score_t ISOLATED_PAWN[2][8];

    // Threat scoring
    static const score_t THREAT_BY_PAWN[2][5];
    static const score_t THREAT_BY_KNIGHT[2][5];
    static const score_t THREAT_BY_BISHOP[2][5];
    static const score_t THREAT_BY_ROOK[2][5];

    // Piece/square tables
    static const score_t KNIGHT_PST[2][64];
    static const score_t BISHOP_PST[2][64];
    static const score_t ROOK_PST[2][64];
    static const score_t QUEEN_PST[2][64];
    static const score_t KING_PST[2][64];

    // mobility tables
    static const score_t KNIGHT_MOBILITY[9];
    static const score_t BISHOP_MOBILITY[15];
    static const score_t ROOK_MOBILITY[2][15];
    static const score_t QUEEN_MOBILITY[2][24];
    static const score_t KING_MOBILITY_ENDGAME[5];

    // outpost scores [phase][defended]
    static const score_t KNIGHT_OUTPOST[2][2];
    static const score_t BISHOP_OUTPOST[2][2];

    // pawn storm [blocked][file][distance]
    static const score_t PAWN_STORM[2][4][5];

    // not tuned presently (fixed)
    static const int MATERIAL_SCALE[32];
    static const int MIDGAME_THRESHOLD;
    static const int ENDGAME_THRESHOLD;

    static inline int foldFile(int f) {
        return f > 4 ? 8-f : f-1;
    }
};

#endif
