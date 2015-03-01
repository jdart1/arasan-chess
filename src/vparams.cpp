// Copyright 2015 by Jon Dart. All Rights Reserved.
//
// definitions of scoring parameters, non-const for tuning

// These have a 1-1 mapping to the tuning parameters
int Scoring::Params::RB_ADJUST[4];
int Scoring::Params::RBN_ADJUST[4];
int Scoring::Params::QR_ADJUST[4];
int Scoring::Params::KN_VS_PAWN_ADJUST[3];
int Scoring::Params::PAWN_TRADE[3];
int Scoring::Params::CASTLING[6];
int Scoring::Params::KING_COVER[5];
int Scoring::Params::KING_FILE_OPEN;
int Scoring::Params::KING_DISTANCE_BASIS;
int Scoring::Params::KING_DISTANCE_MULT;
int Scoring::Params::PIN_MULTIPLIER_MID;
int Scoring::Params::PIN_MULTIPLIER_END;
int Scoring::Params::KING_ATTACK_PARAM1;
int Scoring::Params::KING_ATTACK_PARAM2;
int Scoring::Params::KING_ATTACK_PARAM3;
int Scoring::Params::KING_ATTACK_BOOST_THRESHOLD;
int Scoring::Params::KING_ATTACK_BOOST_DIVISOR;
int Scoring::Params::BISHOP_TRAPPED;
int Scoring::Params::BISHOP_PAIR_MID;
int Scoring::Params::BISHOP_PAIR_END;
int Scoring::Params::BISHOP_PAWN_PLACEMENT_END;
int Scoring::Params::BAD_BISHOP_MID;
int Scoring::Params::BAD_BISHOP_END;
int Scoring::Params::OUTPOST_NOT_DEFENDED;
int Scoring::Params::CENTER_PAWN_BLOCK;
int Scoring::Params::OUTSIDE_PASSER_MID;
int Scoring::Params::OUTSIDE_PASSER_END;
int Scoring::Params::WEAK_PAWN_MID;
int Scoring::Params::WEAK_PAWN_END;
int Scoring::Params::WEAK_ON_OPEN_FILE_MID;
int Scoring::Params::WEAK_ON_OPEN_FILE_END;
int Scoring::Params::SPACE;
int Scoring::Params::PAWN_CENTER_SCORE_MID;
int Scoring::Params::ROOK_ON_7TH_MID;
int Scoring::Params::ROOK_ON_7TH_END;
int Scoring::Params::TWO_ROOKS_ON_7TH_MID;
int Scoring::Params::TWO_ROOKS_ON_7TH_END;
int Scoring::Params::ROOK_ON_OPEN_FILE_MID;
int Scoring::Params::ROOK_ON_OPEN_FILE_END;
int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_MID;
int Scoring::Params::ROOK_ATTACKS_WEAK_PAWN_END;
int Scoring::Params::ROOK_BEHIND_PP_MID;
int Scoring::Params::ROOK_BEHIND_PP_END;
int Scoring::Params::QUEEN_OUT;
int Scoring::Params::PASSER_OWN_PIECE_BLOCK_MID;
int Scoring::Params::PASSER_OWN_PIECE_BLOCK_END;
int Scoring::Params::PP_BLOCK_BASE_MID;
int Scoring::Params::PP_BLOCK_BASE_END;
int Scoring::Params::PP_BLOCK_MULT_MID;
int Scoring::Params::PP_BLOCK_MULT_END;
int Scoring::Params::ENDGAME_PAWN_BONUS;
int Scoring::Params::KING_NEAR_PASSER;
int Scoring::Params::OPP_KING_NEAR_PASSER;
int Scoring::Params::PAWN_SIDE_BONUS;
int Scoring::Params::SUPPORTED_PASSER6;
int Scoring::Params::SUPPORTED_PASSER7;
int Scoring::Params::SIDE_PROTECTED_PAWN;

// The following tables are computed from tuning parameters.
int Scoring::Params::MATERIAL_SCALE[32];
int Scoring::Params::MIDGAME_THRESHOLD = 22;
int Scoring::Params::ENDGAME_THRESHOLD;
int Scoring::Params::KNIGHT_PST[2][64];
int Scoring::Params::BISHOP_PST[2][64];
int Scoring::Params::KNIGHT_OUTPOST[64];
int Scoring::Params::BISHOP_OUTPOST[64];
// TBD: rook PST
int Scoring::Params::TRADE_DOWN[16];
int Scoring::Params::KING_PST[2][64];
int Scoring::Params::KNIGHT_MOBILITY[9];
int Scoring::Params::BISHOP_MOBILITY[15];
int Scoring::Params::ROOK_MOBILITY[15];
int Scoring::Params::QUEEN_MOBILITY[2][29];
int Scoring::Params::KING_MOBILITY_ENDGAME[9];
int Scoring::Params::PASSED_PAWN[2][8];
int Scoring::Params::POTENTIAL_PASSER[2][8];
int Scoring::Params::CONNECTED_PASSERS[2][8];
int Scoring::Params::ADJACENT_PASSERS[2][8];
int Scoring::Params::DOUBLED_PAWNS[2][8];
int Scoring::Params::ISOLATED_PAWN[2][8];

