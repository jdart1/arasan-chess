// Copyright 2013-2017 by Jon Dart.  All Rights Reserved.

// Unit tests for Arasan

#include "board.h"
#include "boardio.h"
#include "legal.h"
#include "movegen.h"
#include "options.h"
#include "movearr.h"
#include "notation.h"
#include "chessio.h"
#include "scoring.h"
#include "search.h"
#include "globals.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <utility>

using namespace std;
using namespace chess;

static int testIsPinned() {

    struct IsPinnedData {
       string fen;
       Square start, dest;
       int result;
       IsPinnedData(const char *fenStr, Square s, Square d, int res)
          : fen(fenStr), start(s), dest(d), result(res) {
          if (!BoardIO::readFEN(board, fenStr)) {
             cerr << "error in FEN: " << fenStr << endl;
             return;
          }
       }
       Board board;
    };

    const IsPinnedData cases[] = {
       IsPinnedData("1R2r2k/5bp1/3Q2p1/2p5/4P2P/1p6/2r1n1BK/8 b - -", E8, E7, 1),
       IsPinnedData("NR2r2k/5bp1/3Q2p1/2p5/4P2P/1p6/2r1n1BK/8 b - -", E8, E7, 1),
       IsPinnedData("1R2r2k/5bp1/3Q2p1/2p5/4P2P/1p6/2r1n1BK/8 b - -", E8, G8, 0),
       IsPinnedData("NR2r2k/5bp1/3Q2p1/2p5/4P2P/1p6/2r1n1BK/8 b - -", E8, G8, 0),
       IsPinnedData("k3r1RB/7R/3Q1np1/2p4n/4P2P/1p6/2r4K/8 b - -", E8, E6, 1),
       IsPinnedData("k3r1BR/8/3Q1bp1/2p5/4P2P/1p6/2r1n2K/8 b - -", E8, E7, 0),
       IsPinnedData("8/2R3bk/1P6/4p2p/1rP5/5BP1/6P1/K3RN1q w - -", E1, E2, 0),
       IsPinnedData("8/2R3bk/1P6/4p2p/1rP5/4NBP1/6P1/K3R2q w - -", E1, E2, 1),
       IsPinnedData("8/1R2k1p1/4b1p1/2p1Q3/4P2P/1p6/2r1n1BK/8 b - -", E6, C4, 1),
       IsPinnedData("4R3/1R4p1/4b1p1/2p1k3/4P2P/1p6/2r1n1BK/8 b - -", E6, C4, 1),
       IsPinnedData("4R3/1R2b1p1/4b1p1/2p1k3/4P2P/1p6/2r1n1BK/8 b - -", E7, D6, 0),
       IsPinnedData("1r5k/6p1/3Q1bp1/2p5/4P2P/1p6/2r1n1BK/8 w - -", G2, F3,0),
       IsPinnedData("r1bq1rk1/5ppp/2np4/p2Np1b1/1pP1P3/6P1/PPN2PBP/R2QK2R b KQ -",G5, E7,0),
       IsPinnedData("6k1/5p1p/3p1bp1/3P3P/1pP5/1P4P1/5PB1/r2Q2K1 w - -",D1, F1,0),
       IsPinnedData("4r2k/Q4b2/6p1/2p1b3/4P2P/1p4R1/2r1R1BK/8 w - -",G3,G4,1),
       IsPinnedData("3r3k/Q4b2/6p1/2p1K3/4P2P/1p4R1/2r1R1Bb/8 w - -",G3,G4,1),
       IsPinnedData("3r3k/Q4b2/6p1/2p1K3/4PB1P/1p4R1/2r1R2b/8 w - -",G3,G4,0),
       IsPinnedData("3r3k/Q4b2/6p1/2p3K1/4P2P/1p2N1R1/2r1R3/2b5 w - -",E3,E5,1),
       IsPinnedData("3r3k/Q4b2/6p1/2p3K1/4P2P/1p2N1R1/2rrR3/2b5 w - -",E3,E5,0),
       IsPinnedData("3r3k/Q4b2/6pb/2p5/4P2P/1p2B1R1/4R3/2K5 w - -",E3,F4,0),
       IsPinnedData("3r3k/Q4b2/6pb/2p5/4P2P/1p2B1R1/4R3/2K5 w - -",E3,F2,1),
       IsPinnedData("2k5/4r3/1P2b1r1/4p2p/2P5/6PQ/q4B2/3R3K b - -",E6,F7,1),
       IsPinnedData("3r3k/2b2bp1/1R4p1/K1p5/4P2P/1p6/6BQ/2n3R1 w - -",B6,C6,1),
       IsPinnedData("1R2r2k/6p1/6p1/2pQb3/4P2P/1p4B1/2r4K/4n3 w - -",G3,F2,1),
       IsPinnedData("1r5k/2b2bp1/3R2p1/2p1K3/4P2P/1p6/6BQ/2n3R1 w - -",D6,B6,1),
       IsPinnedData("1r5k/5bp1/3K2p1/8/4PR1P/1p4b1/6BN/2n4Q w - -",F4,F3,1),
       IsPinnedData("1r6/2b2bp1/3r2p1/2p1k3/4P2P/1pn4K/1B4BQ/N5R1 b - -",C3,A4,1),
       IsPinnedData("1r5k/2b2bp1/3r2p1/2p1K3/7P/1p2R3/4B2Q/2n1r3 w - -",E3,C3,0)
    };

    int errs = 0;
    for (int i = 0; i<28; i++) {
       const IsPinnedData & data = cases[i];
       if (data.board.isPinned(data.board.sideToMove(),data.start, data.dest) != data.result) {
          cerr << "isPinned: error in case " << i << endl;
          ++errs;
       }
    }
    return errs;
}

static int testGetPinned() {

    struct GetPinnedData {
       string fen;
       set<Square> sqs;
       GetPinnedData(const char *fenStr, const std::initializer_list<Square> &sqlist) : fen(fenStr) {
          if (!BoardIO::readFEN(board, fenStr)) {
             cerr << "error in FEN: " << fenStr << endl;
             return;
          }
          std::for_each(sqlist.begin(), sqlist.end(),
                        [this] (Square sq) { sqs.insert(sq); });
       }
       Board board;
    };

    const GetPinnedData cases[] = {
       GetPinnedData("1R2b2k/5rp1/3Q2p1/2p5/4P2P/1p6/2r1n1BK/8 w - -",{E8}),
       GetPinnedData("4r2k/5bp1/3Q2p1/2p1B2p/4P3/1p5R/2r1n2K/8 w - -", {H5, G7}),

       GetPinnedData("5R2/7b/1p3n1n/5k2/P1P1p3/2P1B2P/4P1BK/3r4 w - -", {F6}),
       GetPinnedData("r1k2r2/p6p/2pbn3/2p1p1pq/N2p4/1P1P1PPB/P1Q2RKP/R7 w - -", {E6}),
       GetPinnedData("4kn1Q/4b3/2q3p1/1n2p1P1/1pB5/1P2B1N1/1PK5/8 w - -",{F8}),
       GetPinnedData("4kn1Q/4b3/2q3p1/1n2p1P1/1pB5/1P2B1N1/1PK5/8 b - -",{C4}),
       GetPinnedData("4knbQ/8/2q3p1/1n2p1P1/1pB5/1P2B1N1/1PK5/8 w - -",{}),
       GetPinnedData("7B/p2q4/1p1p1p2/1PpPk1pQ/4P3/3n4/1P5P/5B1K w - -",{G5,F6}),
       GetPinnedData("B7/4Npp1/7p/2k1p3/p1pn2PP/P3n1K1/2r2B2/2R5 w - -",{}),
       GetPinnedData("8/2p3r1/1p4kR/1P1P4/3rP2R/2p5/4K2P/8 b - -",{})
    };

    int errs = 0;
    for (int i = 0; i<10; i++) {
       const GetPinnedData & data = cases[i];
       Board board(data.board);
       const ColorType side = board.sideToMove();
       const ColorType oside = board.oppositeSide();
       Bitboard b = board.getPinned(board.kingSquare(oside), side, oside);
       if (b.bitCount() != data.sqs.size()) {
          cout << i << ' ' << b.bitCount() << ' ' << data.sqs.size() << endl;
          cerr << "error in getPinned (count), case " << i << endl;
          ++errs;
          continue;
       }
       Square sq;
       while (b.iterate(sq)) {
          if (data.sqs.count(sq) != 1) {
             cerr << "error in getPinned, case " << i << ' ' <<
                SquareImage(sq) << " not found" << endl;
             ++errs;
          }
       }
    }
    return errs;
}

static int testSee() {

    struct SeeData {
       string fen;
       Move move;
       int result;
       SeeData(const char *fenStr, const char *moveStr, int res) :
          fen(fenStr), move(NullMove), result(res) {
          if (!BoardIO::readFEN(board, fenStr)) {
             cerr << "warning: testSee: error in FEN: " << fenStr << endl;
             return;
          }
          move = Notation::value(board,board.sideToMove(),
                                 Notation::InputFormat::SAN,moveStr);
          if (IsNull(move)) {
             cerr << "warning: testSee: error in move: " << moveStr << endl;
          }
       }
       Board board;
    };

    const SeeData seeData[] = {
       SeeData("4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - -","hxg4",0),
       SeeData("4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - -","hxg4",0),
       SeeData("4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - -","Bxf3",0),
       SeeData("2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - -","dxe5",Params::PAWN_VALUE),
       SeeData("7r/5qpk/p1Qp1b1p/3r3n/BB3p2/5p2/P1P2P2/4RK1R w - -","Re8",0),
       SeeData("6rr/6pk/p1Qp1b1p/2n5/1B3p2/5p2/P1P2P2/4RK1R w - -","Re8",-Params::ROOK_VALUE),
       SeeData("7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - -","Re8",-Params::ROOK_VALUE),
       SeeData("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -","f8=Q",Params::BISHOP_VALUE-Params::PAWN_VALUE),
       SeeData("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -","f8=N",Params::KNIGHT_VALUE-Params::PAWN_VALUE),
       SeeData("7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -","f8=Q",Params::QUEEN_VALUE-Params::PAWN_VALUE),
       SeeData("7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -","f8=B",Params::BISHOP_VALUE-Params::PAWN_VALUE),
       SeeData("7R/4bP2/8/8/1q6/3K4/5p2/4k3 w - -","f8=R",-Params::PAWN_VALUE),
       SeeData("8/4kp2/2npp3/1Nn5/1p2PQP1/7q/1PP1B3/4KR1r b - -","Rxf1+",0),
       SeeData("8/4kp2/2npp3/1Nn5/1p2P1P1/7q/1PP1B3/4KR1r b - -","Rxf1+", 0),
       SeeData("2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - -","Qxc1",2*Params::ROOK_VALUE-Params::QUEEN_VALUE),
       SeeData("r2qk1nr/pp2ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq -","Nxe5",Params::PAWN_VALUE),
       SeeData("6r1/4kq2/b2p1p2/p1pPb3/p1P2B1Q/2P4P/2B1R1P1/6K1 w - -","Bxe5",0),
       SeeData("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6","gxh6",0),
       SeeData("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R1B2B/PQ3P1P/3R2K1 w - h6","gxh6",Params::PAWN_VALUE),
       SeeData("2r4r/1P4pk/p2p1b1p/7n/BB3p2/2R2p2/P1P2P2/4RK2 w - -","Rxc8",Params::ROOK_VALUE),
       SeeData("2r5/1P4pk/p2p1b1p/5b1n/BB3p2/2R2p2/P1P2P2/4RK2 w - -","Rxc8",Params::ROOK_VALUE),
       SeeData("2r4k/2r4p/p7/2b2p1b/4pP2/1BR5/P1R3PP/2Q4K w - -","Rxc5",Params::BISHOP_VALUE),
       SeeData("8/pp6/2pkp3/4bp2/2R3b1/2P5/PP4B1/1K6 w - -","Bxc6",Params::PAWN_VALUE-Params::BISHOP_VALUE),
       SeeData("4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -","Rxe4",Params::PAWN_VALUE-Params::ROOK_VALUE),
       SeeData("4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -","Bxe4",Params::PAWN_VALUE)
    };

    int errs = 0;
    for (int i = 0; i < 25; i++) {
       const SeeData &data = seeData[i];
       int result = see(data.board,data.move);
       if (result != data.result) {
          cerr << "see: error in case " << i << endl;
          ++errs;
       }
    }
    return errs;
}

static int testNotation() {

    struct NotationData {
       string fen,moveStr;
       NotationData(const char *fenStr, const char *s) {
          fen = fenStr;
          if (!BoardIO::readFEN(board, fenStr)) {
             cerr << "error in FEN: " << fenStr << endl;
             return;
          }
          moveStr = s;
       }
       Board board;
    };

    const NotationData notationData[] = {
        NotationData("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1","e4"),
        NotationData("r1bq1rk1/pp1n1pbp/2pp1np1/4p3/P2PP3/2N2N1P/1PP1BPP1/R1BQ1RK1 w - -","dxe5"),
        NotationData("rnbqkb1r/pp2pppp/2p2n2/3p4/2PP4/2N2N2/PP2PPPP/R1BQKB1R b KQkq -","dc4"),
        NotationData("4Qnk1/pp1P1p2/6rp/8/3P4/r5BK/8/8 w - -","d8Q"),
        NotationData("4Qnk1/pp1P1p2/6rp/8/3P4/r5BK/8/8 w - -","d8=Q"),
        NotationData("2kr1b1r/p2nPp2/2q2P2/2p3B1/1pp5/2N3P1/PP3P1P/1R1Q1K2 w - -","exd8Q+"),
        NotationData("2kr1b1r/p2nPp2/2q2P2/2p3B1/1pp5/2N3P1/PP3P1P/1R1Q1K2 w - -","exd8=Q+"),
        NotationData("3rr3/k1qn2b1/1pp1p3/p2nPp1p/P1BPQ1pP/1N6/1P1B1PP1/2R1R1K1 w - f6","exf6"),
        NotationData("r1b2rk1/pp2q2p/2n3p1/2P3N1/8/P2p4/2PB1PPP/R2QK2R w KQ -","Be3"),
        NotationData("7r/4k1pp/1q2bp2/2p1p3/2P1P3/5N2/1pQ2PPP/r3RNK1 w - -","N1d2"),
        NotationData("r2q2k1/3bbppp/3pn3/1p2pN2/4P2n/2P2N1P/1PB1QPP1/2BR2K1 w - -","N3xh4"),
        NotationData("2r2rk1/6pp/2pqr3/p4pB1/1b1P4/1Q2QPP1/1P2P1BP/2R2RK1 w - -","Qexe6"),
        NotationData("r4k2/3b1p1p/3n1npN/4p3/1Q2P3/7P/1Pq2PP1/2BR1NK1 b - -","Nxe4"),
        NotationData("8/3b2kp/4p1p1/pr1n4/2N1N2P/1P4P1/1K3P2/3R4 w - -","Ned6"),
        NotationData("r1r3k1/2n1ppbp/npp5/p2pP2p/P2P1N2/1PP5/3B1PPN/2Rb1RK1 w - -","Rfxd1"),
        NotationData("1r6/k1q3pp/p2r1p2/Ppp1pP1B/1nPpP3/R2P2P1/2PQ2KP/R7 w - b6 0 1","axb6"),
        NotationData("8/3k2p1/5bK1/p7/P2P1PPp/4P3/7P/8 b - g3 0 1","hxg3")
    };

    int errs = 0;
    for (int i = 0; i < 17; i++) {
        Move m = Notation::value(notationData[i].board,
                                 notationData[i].board.sideToMove(),
                                 Notation::InputFormat::SAN,
                                 notationData[i].moveStr);
        if (m == NullMove || !legalMove(notationData[i].board,m)) {
           cout << "notation: error in case " << i << endl;
           ++errs;
        }
    }
    // Verify e.p. square is set correctly
    Board board;
    stringstream s(notationData[15].fen);
    s >> board;
    if (board.enPassantSq() != B5) {
        cerr << "notation: error in case 17" << endl;
        ++errs;
    }
    stringstream s2;
    s2 << board;
    if (s2.str() != notationData[15].fen) {
        cerr << "notation: error in case 18" << endl;
        ++errs;
    }
    stringstream s3(notationData[16].fen);
    s3 >> board;
    if (board.enPassantSq() != G4) {
        cerr << "notation: error in case 19" << endl;
        ++errs;
    }
    stringstream s4;
    s4 << board;
    if (s4.str() != notationData[16].fen) {
        cerr << "notation: error in case 20" << endl;
        ++errs;
    }
    // verify flip() sets e.p. square correctly
    board.flip();
    if (board.enPassantSq() != G5) {
        cout << "notation: error in case 21" << endl;
    }
    int casenum = 22;
    // Test WB and UCI formats
    for (int i = 0; i < 2; i++) {
        Notation::InputFormat fmt = (i == 0 ? Notation::InputFormat::WB :
                                     Notation::InputFormat::UCI);
        string fenStr = "4Qnk1/pp1P1p2/6rp/8/3P4/r5BK/8/8 w - -";
        if (!BoardIO::readFEN(board, fenStr.c_str())) {
            cerr << "error in FEN: " << fenStr << endl;
            return ++errs;
        }
        Move m = Notation::value(board,White,fmt,"d7d8q");
        if (m == NullMove || StartSquare(m) != D7 || DestSquare(m) != D8 || PromoteTo(m) != Queen) {
            cout << "notation: error in case " << casenum << endl;
            ++errs;
        }
        ++casenum;
        fenStr = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
        if (!BoardIO::readFEN(board, fenStr.c_str())) {
            cerr << "notation: error in FEN: " << fenStr << endl;
            return ++errs;
        }
        m = Notation::value(board,White,fmt,"e2e4");
        if (m == NullMove || StartSquare(m) != E2 || DestSquare(m) != E4 || PromoteTo(m) != Empty) {
            cout << "notation: error in case " << casenum << endl;
            ++errs;
        }
        ++casenum;
        fenStr = "r2qr1k1/p2n1pp1/1pb2b1p/3p4/8/1QNBPN2/PP3PPP/3RK2R w K -";
        if (!BoardIO::readFEN(board, fenStr.c_str())) {
            cerr << "notation: error in FEN: " << fenStr << endl;
            return ++errs;
        }
        m = Notation::value(board,White,fmt,fmt == Notation::InputFormat::WB ? "O-O" : "e1g1");
        if (TypeOfMove(m) != KCastle) {
            cout << "notation: error in case " << casenum << endl;
            ++errs;
        }
        ++casenum;
        fenStr = "r3k2r/ppqnbp1b/2n1p2p/2ppP1p1/8/P2P1NPP/1PP1QPB1/R1B1RNK1 b kq -";
        if (!BoardIO::readFEN(board, fenStr.c_str())) {
            cerr << "notation: error in FEN: " << fenStr << endl;
            return ++errs;
        }
        m = Notation::value(board,Black,fmt,fmt == Notation::InputFormat::WB ? "O-O" : "e8g8");
        if (TypeOfMove(m) != KCastle) {
            cout << "notation: error in case " << casenum << endl;
            ++errs;
        }
        ++casenum;
        fenStr = "r2qkb1r/1p3pp1/p1bppn1p/8/4P3/2NB1Q2/PPPB1PPP/R3K2R w KQkq -";
        if (!BoardIO::readFEN(board, fenStr.c_str())) {
            cerr << "notation: error in FEN: " << fenStr << endl;
            return ++errs;
        }
        m = Notation::value(board,White,fmt,fmt == Notation::InputFormat::WB ? "O-O-O" : "e1c1");
        if (TypeOfMove(m) != QCastle) {
            cout << "notation: error in case " << casenum << endl;
            ++errs;
        }
        ++casenum;
        fenStr = "r3k2r/ppqnbp1b/2n1p2p/2ppP1p1/8/P2P1NPP/1PP1QPB1/R1B1RNK1 b kq -";
        if (!BoardIO::readFEN(board, fenStr.c_str())) {
            cerr << "notation: error in FEN: " << fenStr << endl;
            return ++errs;
        }
        m = Notation::value(board,Black,fmt,fmt == Notation::InputFormat::WB ? "O-O-O" : "e8c8");
        if (TypeOfMove(m) != QCastle) {
            cout << "notation: error in case " << casenum << endl;
            ++errs;
        }
        ++casenum;
    }
    return errs;

}

static int testPGN() {
static const string pgn_test = "[Event \"?\"]"
"[Site \"chessclub.com\"]"
"[Date \"2013.12.16\"]"
"[Round \"?\"]"
"[White \"?\"]"
"[Black \"?\"]"
"[Result \"*\"]"
"[ECO \"B08\"]"
"[WhiteElo \"2527\"]"
"[BlackElo \"2558\"]"
""
"1. d4 g6 2. e4 Bg7 3. Nc3 c6 4. Nf3 d6 5. Be2 Nf6 6. O-O O-O 7. a4 a5 (7..."
"Nbd7 $1 8. h3 e5 9. dxe5 dxe5 10. Qd6 Re8 11. Bc4 Bf8 12. Qd3 h6 13. Rd1 Qc7"
"14. Qe3 Nc5 15. b3 b6 16. Ba3 Kg7 17. b4 Ne6 18. Rdb1 Bb7 19. a5 b5 20. Bxe6"
"Rxe6 21. Bc1 Kg8 22. a6 Bc8 23. Ne1 Re8 24. Qf3 Nd7 25. Nd3 Nb8 26. Nc5 Nd7 27."
"Nd3 Nb8 28. Nc5 Nd7 29. Nd3 {1/2-1/2 (29) Saglione,E (2547)-Ludgate,A (2515)"
"ICCF 2010}) 8. h3 Na6 9. Bf4 *";

      stringstream infile(pgn_test);
      long first;
      vector <ChessIO::Header>hdrs;
      ChessIO::collect_headers(infile,hdrs,first);
      int errs = 0;
      if (hdrs.size() != 10) {
         ++errs;
         cerr << "error in PGN test: header count" << endl;
      }
      ChessIO::Header firstHdr = hdrs[0];
      if (firstHdr.tag() != "Event") {
         ++errs;
         cerr << "error in PGN test: bad tag" << endl;
      }
      if (firstHdr.value() != "?") {
         ++errs;
         cerr << "error in PGN test: bad value" << endl;
      }

      int var = 0;
      int seen = 0;
      for (;;) {
         string num;
         ChessIO::Token tok = ChessIO::get_next_token(infile);
         if (tok.type == ChessIO::OpenVar) {
             seen |= 1;
             ++var;
         } else if (tok.type == ChessIO::CloseVar) {
             seen |= 2;
             --var;
         } else if (tok.type == ChessIO::NAG) {
             seen |= 4;
             if (tok.val != "$1") {
                 ++errs;
                 cout << "PGN test: NAG error" << endl;
             }
         } else if (tok.type == ChessIO::Comment) {
             seen |= 8;
             if (tok.val.substr(0,9) != "{1/2-1/2 ") {
                 ++errs;
                 cout << "PGN test: comment error" << endl;
             }
         }
         else if (tok.type == ChessIO::Result) {
             seen |= 16;
             if (tok.val != "*") {
                 ++errs;
                 cout << "PGN test: result error" << endl;
             }
         }
         if (tok.type == ChessIO::Eof)
            break;
      }
      if (var) {
          ++errs;
          cout << "PGN test: variation not closed" << endl;
      }
      if (seen != 0x1f) {
          ++errs;
          cout << "PGN test: missing tokens" << endl;
      }
      return errs;
}

static int testEval() {
    // verify eval results are symmetrical (White/Black)
    const int CASES = 41;
    static const string fens[CASES] = {
        "8/4K3/8/1NR5/8/4k1r1/8/8 w - -",
        "8/4K3/8/1N6/6p1/4k2p/8/8 w - -",
        "8/4K3/8/1r6/6B1/4k2N/8/8 w - -",
        "3b4/1n3n2/1pk3Np/p7/P4P1p/1P6/5BK1/3R4 b - -",
        "8/3r1ppk/8/P6P/3n4/2K5/R2B4/8 b - -",
        "1rb1r1k1/2q2pb1/pp1p4/2n1pPPQ/Pn1BP3/1NN4R/1PP4P/R5K1 b - -",
        "6k1/1b4p1/5p1p/pq3P2/1p1BP3/1P2QR1P/P1r3PK/8 w - -",
        "8/5pk1/7p/3p1R2/p1p3P1/2P2K1P/1P1r4/8 w - -",
        "6k1/p3pp2/6p1/7P/R7/b1q2P2/B1P1K2P/7R b - -",
        "r7/1b4k1/pp1np1p1/3pq1NN/7P/4P3/PP4P1/1Q3RK1 b - -",
        "4b3/2p4p/pp1bk3/2p3p1/2P5/PPB2PP1/7P/3K1N2 w - -",
        "r1bqr1k1/ppp2ppp/3p4/4n3/2PN4/P1Q1P3/1PB2PPP/R4RK1 b - -",
        "r4rk1/1ppqbppp/p2p1n2/8/1n1PP3/1Q3N2/PP1N1PPP/R1B1R1K1 b - -",
        "r6k/1p4bp/1p1n1pp1/1B6/8/P4NP1/1P3P1P/2R3K1 w - -",
        "r1b2r1k/pp3n1p/2p1p3/3Pnppq/3PP3/1P1N1PP1/P5BP/R1Q2RK1 w - -",
        "2kr3r/1bpnqp2/1p2p3/p2p3p/P1PPBPp1/2P1P1P1/2QN2P1/1R2K2R w K -",
        "8/1R6/3k4/2p5/2p1B3/5K2/8/8 w - -",
        "1BR2rk1/pP1nbpp1/B2P2p1/8/8/8/1P4P1/3n2K1 b - -",
        "r1b1k2r/1p1n1pp1/p6p/2p5/4Nb2/5NP1/PPP2P1P/1K1R1B1R b kq -",
        "r1b2rk1/1p1n1pp1/p6p/2p5/4Nb2/3R1NP1/PPP2P1P/1K1R1B2 b - -",
        "1kr5/1p1b2R1/p3p2Q/2bp3P/8/P1PB1P2/1P1K1P2/R6q b - -",
        "rb3rk1/1p1RRpp1/p6p/r1p5/4Nb2/5NP1/PPP2P1P/1K3B2 b - -",
        "5rk1/1pqn2pp/4pn2/p7/2P5/4PP2/1B2BP1P/3Q1RK1 w - -",
        "3k1q2/p3p1p1/1p1nQ3/3P4/P2P4/B2P4/6KP/8 b - -",
        "6k1/4R1P1/5P2/5K1p/7r/8/8/8 w - -",
        "1n1q1rk1/4ppbp/3p1np1/1PpP4/4P3/2N2N2/3B1PPP/Q3K2R b K -",
        "3q1rk1/4ppbp/1n1p1np1/1PpP4/2N1P3/5N2/3B1PPP/Q3K2R b K -",
        "3q1rk1/4ppbp/1n1p1np1/1P1P4/4P3/2p1BN2/2N2PPP/Q3K2R b K -",
        "N5r1/pQ6/3b1nkp/2q5/2Pp1p2/4nP2/PP1B2PP/1RR3K1 b - -",
        "8/2kn2q1/B1p2pP1/P1P2p1p/3P2bP/3P1B2/1K1P1Q2/8 b - -",
        "5nk1/3b1r2/2p1p3/1pPpP1qp/1P1Q4/6P1/4BN1P/R5K1 w - - 0 1",
        "r5rk/1R3Q2/2p5/p4K2/5N2/5q2/P7/1R6 b - - 0 43",
        "2r2bk1/7p/7p/2qPpPP1/pp1p4/P2Q4/1PP5/1K1R3R w - - 0 31",
        "1r6/2Qnk2p/3p4/5p2/2P5/1P2b1P1/P5KP/8 w - - 0 30",
        "8/3B2k1/4K1p1/8/8/4B2P/1b6/8 b - - 0 59",
        "2r5/8/1qr3pk/3p1p1p/R2PpP1P/2Pb2P1/3Q3K/2RB4 b - - 0 52",
        "5r2/3rq2k/p2Np1p1/1b1pP2p/3Q1P1P/1P4P1/2R3K1/2R5 b - - 0 42",
        "8/8/8/1K6/3N4/k5p1/6N1/8 b - - 0 92",
        "1r5k/3nbp2/1r2b1pP/3np3/2B1R3/1NN4q/1PPB2R1/1K3Q2 b - - 0 31",
        "8/6pk/6p1/4r3/1Qp5/4qPpP/6P1/6RK b - - 0 44",
        "2k5/1p1b1p2/5n2/p1p1pP2/2PnPq2/3P2r1/PP3QB1/3R1RK1 b - - 0 36"
    };

    int errs = 0;
    for (int i = 0; i < CASES; i++) {
        Board board;
        if (!BoardIO::readFEN(board, fens[i].c_str())) {
            cerr << "testEval case " << i << " error in FEN: " << fens[i] << endl;
            ++errs;
            continue;
        }
        Scoring *s = new Scoring();
        int eval1 = s->evalu8(board);
        board.flip();
        int eval2 = s->evalu8(board);
        if (eval1 != eval2) {
            ++errs;
            cerr << "testEval case " << i << " eval mismatch" << endl;
        }
        delete s;
    }
    return errs;
}

static int testBitbases() {
    // verify eval results are symmetrical (White/Black)
    const int CASES = 8;
    struct Case 
    {
        Case(const string &f, int r)
            :fen(f),result(r) 
            {
            }
        string fen;
        int result;
    };
    
    static const Case cases[CASES] = {
        Case("8/8/1p6/2k5/8/5K2/8/8 b - - 0 1",Constants::BITBASE_WIN),
        Case("8/8/5k2/8/2K5/1P6/8/8 w - - 0 1",Constants::BITBASE_WIN),
        Case("8/8/1p6/2k5/8/5K2/8/8 w - - 0 1",-Constants::BITBASE_WIN),
        Case("8/8/5k2/8/2K5/1P6/8/8 b - - 0 1",-Constants::BITBASE_WIN),
        Case("8/8/8/k7/p7/8/8/1K6 b - - 0 1",0),
        Case("8/8/8/k7/p7/8/8/1K6 w - - 0 1",0),
        Case("1k6/8/8/P7/K7/8/8/8 w - - 0 1",0),
        Case("1k6/8/8/P7/K7/8/8/8 b - - 0 1",0)
    };

    int errs = 0;
    Scoring *s = new Scoring();
    Board board;
    for (int i = 0; i<CASES; i++) {
        if (!BoardIO::readFEN(board, cases[i].fen.c_str())) {
            cerr << "testBitbases: case " << i << " invalid FEN" << endl;
            ++errs;
        } else {
            if (s->evalu8(board) != cases[i].result) {
                cerr << "testBitbases: case " << i << " invalid result" << endl;
                ++errs;
            }
        }
    }
    return errs;
}

static int testDrawEval() {
    // verify detection of KBP and other draw situations
    const int DRAW_CASES = 12;
    static const string draw_fens[DRAW_CASES] = {
        "k7/8/P7/B7/1K6/8/8/8 w - - 0 1", // KBP draw
        "8/8/8/1k6/b7/p7/8/K7 b - - 0 1", // KBP draw
        "8/8/8/8/7b/4k2p/8/6K1 b - - 0 1", // KBP draw
        "8/7k/4B3/8/6KP/8/8/8 b - - 0 3", // KBP draw
        "8/8/2KN3k/8/3N4/8/8/8 w - - 0 1", // KNN draw
        "8/8/2KN3k/8/3N4/8/8/8 w - - 0 1", // KNN draw
        "8/8/8/3n4/8/2kn3K/8/8 b - - 0 1", // KNN draw
        "8/8/2K4k/8/3N4/8/8/8 w - - 0 1", // KN draw
        "8/8/8/3n4/8/2k4K/8/8 b - - 0 1", // KN draw
        "8/8/8/8/8/2k4K/8/8 b - - 0 1", // KK draw
        "8/8/8/2B1b3/8/2k4K/8/8 b - - 0 1", // KB vs KB draw (same color)
        "8/6k1/8/7P/3K4/8/4B2P/8 w - - 0 1" // KBPP draw
        // technically these are draws but not recognized yet:
        //"8/8/8/1k6/b7/p7/8/2K5 b - - 0 1", // KBP draw
        // 8/5k2/8/5B2/8/6KP/8/8 w - - 0 1 // KBP draw
    };

    const int NON_DRAW_CASES = 4;
    static const string nondraw_fens[NON_DRAW_CASES] = {
        "8/7k/8/6B1/6KP/8/8/8 w - - 0 3", // KBP right-color pawn
        "8/3k3B/8/8/5K2/7P/8/8 b - - 0 1", // KBP opp king too far
        "6K1/8/6b1/6k1/8/6B1/8/8 w - - 0 1", // opp color bishops
        "8/8/8/5k1P/3K4/8/4B2P/8 w - - 0 1" // KBPP too far
    };
    int errs = 0;
#if defined(NALIMOV_TBS) || defined(GAVIOTA_TBS) || defined(SYZYGY_TBS)
    int tmp = options.search.use_tablebases;
    options.search.use_tablebases = 0;
#endif
    for (int i = 0; i < DRAW_CASES; i++) {
        Board board;
        if (!BoardIO::readFEN(board, draw_fens[i].c_str())) {
            cerr << "testDrawEval draw case " << i << " error in FEN: " << draw_fens[i] << endl;
            ++errs;
            continue;
        }
        Scoring *s = new Scoring();
        if (!s->isDraw(board)) {
            cerr << "testDrawEval: error in draw case " << i << " fen=" << draw_fens[i] << endl;
            ++errs;
        }
	delete s;
    }
    for (int i = 0; i < NON_DRAW_CASES; i++) {
        Board board;
        if (!BoardIO::readFEN(board, nondraw_fens[i].c_str())) {
            cerr << "testDrawEval non-draw case " << i << " error in FEN: " << nondraw_fens[i] << endl;
            ++errs;
            continue;
        }
        Scoring *s = new Scoring();
        if (s->isDraw(board)) {
            cerr << "testDrawEval: error in non-draw case " << i << " fen=" << nondraw_fens[i] << endl;
            ++errs;
        }
	delete s;
    }
#if defined(NALIMOV_TBS) || defined(GAVIOTA_TBS) || defined(SYZYGY_TBS)
    options.search.use_tablebases = tmp;
#endif
    return errs;
}

static int testWouldAttack() {
   static const struct TestCase
   {
      string fen;
      Square start, dest;
      Square target;
      int result;
      TestCase(const string &s, Square st, Square d, Square t,int r) :
         fen(s),start(st), dest(d), target(t), result(r)
         {
         }
   } cases[11] = {TestCase("2r1k2r/1p1b2p1/pBq2p2/3p1n1p/5P2/1Q4P1/PP5P/1K1R1BR1 w k -",F1,D3,F5,1),
                TestCase("2rqr1k1/1p1b1ppp/1b1n1n2/p2p4/P2N4/2P3PP/1P2NPBK/R2QBR2 b - -",D1,B3,F7,0),
                TestCase("2rqr1k1/1p1b1ppp/1b1n1n2/p2p4/P2N4/2P3PP/1P2NPBK/R2QBR2 b - -",D1,B3,D5,1),
                TestCase("2rqr1k1/1p1b1ppp/1b1n1n2/p2p4/P2N4/2P3PP/1P2NPBK/R2QBR2 b - -",D1,B3,B6,1),
                TestCase("2kr3r/ppqnn3/2p2b2/P2p2pp/1B1Pp3/1N6/1PP1BPPP/R2Q1RK1 w - -",C2,C4,D5,1),
                TestCase("2kr3r/ppqnn3/2p2b2/P2p2pp/1B1Pp3/1N6/1PP1BPPP/R2Q1RK1 w - -",B3,C5,D7,1),
                TestCase("k3r2r/p6q/Pp1Q1n2/3PP3/4p1p1/6pP/1P2BP2/2R1R1K1 b - -",G3,F2,G1,1),
                TestCase("2rr2k1/p4pp1/1p5p/7P/4PP2/1R2B1P1/P3R3/2b2K2 b - -",D8,D1,F1,1),
                TestCase("2r1r3/5Qpk/5p2/2B4p/3pqP2/P3n1P1/1P5P/K1R3R1 b - -",H7,H6,H5,1),
                TestCase("2r2rk1/1p1b2p1/pBq2p2/3p1n1p/5P2/1Q1B2P1/PP5P/1K1R2R1 w - -",D1,C1,C8,0),
                TestCase("5r2/2p2k2/4N1p1/2p1p1Pp/2P1Pp1P/5P2/3K4/8 b - -",F8,D8,D2,1)
   };
   int errs = 0;
   for (int i = 0; i < 11; i++) {
      const TestCase &acase = cases[i];
        Board board;
        if (!BoardIO::readFEN(board, acase.fen.c_str())) {
            cerr << "wouldAttack: error in test case " << i << " error in FEN: " << acase.fen << endl;
            ++errs;
            continue;
        }
        Move m = CreateMove(acase.start,acase.dest,TypeOfPiece(board[acase.start]),TypeOfPiece(board[acase.dest]));
        if ((board.wouldAttack(m,acase.target) != 0) != acase.result) {
           cerr << "wouldAttack: error in test case " << i << endl;
           ++errs;
        }
   }
   return errs;
}

static int testCheckStatus() {
   static const struct TestCase
   {
      string fen;
      string move;
      CheckStatusType result;
       TestCase(const string &str, const string &m, CheckStatusType r):
           fen(str),move(m),result(r)
         {
         }
   } cases[20] = {
       TestCase("5r1k/pp4pp/2p5/2b1P3/4P3/1PB1p3/P5PP/3N1QK1 b - -","e2+",InCheck),
       TestCase("8/1n3ppk/7p/3n1P1P/kP4K1/1r6/2N5/3B4 w - -","Ne3",NotInCheck),
       TestCase("8/5ppb/3k3p/1p3P1P/1PrN1PK1/3R4/8/8 w - -","Nf3+",InCheck),
       TestCase("8/5ppb/3k3p/1p1r1P1P/1P1N2K1/3R4/8/8 w - -","Nxb5+",InCheck),
       TestCase("8/5ppb/7p/5P1P/k2BR1K1/8/8/8 w - -","Bxg7+",InCheck),
       TestCase("7R/5kp1/4n1pp/2r1p3/4P1P1/3R3P/r4P2/5BK1 w - -","Rd7+",InCheck),
       TestCase("7R/4nkp1/6pp/2r1p3/4P1P1/3R3P/r4P2/5BK1 w - -","Rd7",NotInCheck),
       TestCase("r1bq1bkr/ppn2p2/2n4p/2p1p3/4B2p/2NP2P1/PP1NPP1P/R1B3QK w - -","gxh4+",InCheck),
	   TestCase("4B3/1n3ppb/2P4p/5P1P/kPrN2K1/3R4/8/8 w - -","cxb7+",InCheck),
       TestCase("8/4kp2/6pp/3P4/5P1n/P2R3P/7r/5K2 w - -","d6+",InCheck),
       TestCase("7k/1p4p1/pPp4p/P1P1b2q/4Q1n1/2N3P1/5BK1/7R b - -","Qh1+",InCheck),
       TestCase("7k/1p4p1/pPp4p/P1P1b2q/4Q1n1/2N2KP1/5BR1/8 b - -","Qh1",NotInCheck),
       TestCase("8/5ppb/7p/5P1P/3B1RK1/8/4k3/8 w - -","Re4+",InCheck),
       TestCase("5k2/5ppb/7p/7P/3BrP2/8/2K5/8 b - -","Rd4+",InCheck),
       TestCase("6k1/5ppb/7p/7P/4rP2/3B4/2K5/8 b - -","Rd4",NotInCheck),
       TestCase("8/1R3P1k/8/5r2/2P1p1pP/8/1p5K/8 w - -","f8=Q",InCheck),
       TestCase("2k2Nn1/2r5/q2p4/p2Np1P1/1pPpP1K1/1P1Pb2Q/P6R/8 w - -","Kh5+",InCheck),
       TestCase("8/1P3ppb/2k4p/1p3P1P/1Pr3K1/3RN3/8/8 w - -","b8=N",InCheck),
       TestCase("1r6/P4ppb/7p/1p2kP1P/1P1N2K1/3R4/8/8 w - -","axb8=Q",InCheck),
       TestCase("1r6/P1N2ppb/7p/1p2kP1P/1P4K1/3R4/8/8 w - -","axb8=Q",NotInCheck)
   };
   int errs = 0;
   for (int i = 0; i<20; i++) {
      const TestCase &acase = cases[i];
      Board board;
      if (!BoardIO::readFEN(board, acase.fen.c_str())) {
          cerr << "testCheckStatus: error in test case " << i << " error in FEN" << endl;
          ++errs;
          continue;
      }
      Move m = Notation::value(board,board.sideToMove(),Notation::InputFormat::SAN,acase.move);
	  if (IsNull(m)) {
		  cerr << "testCheckStatus: error in test case " << i << " bad move" << endl;
		  ++errs;
		  continue;
	  }
      CheckStatusType wouldCheckResult = board.wouldCheck(m);
      board.doMove(m);
      if (board.checkStatus(m) != acase.result) {
          cerr << "testCheckStatus: error in test case " << i << " bad result" << endl;
          ++errs;
      }
      if (board.checkStatus() != wouldCheckResult) {
          cerr << "testCheckStatus: error in test case " << i << " wouldCheck result" << endl;
          ++errs;
      }
      if (board.checkStatus() != acase.result) {
          cerr << "testCheckStatus: error in test case " << i << " result mismatch" << endl;
          ++errs;
      }
   }
   return errs;
}

static int testRec(const EPDRecord &rec, vector < pair<string,string> > correct)
{
   int errs = 0;
   if (rec.hasError()) {
       cerr << "EPD test: error reading EPD record: " << rec.getError() << endl;
      return ++errs;
   }
   if (rec.getSize() != correct.size()) {
      cerr << "EPD test: expected size " << correct.size() << ", got " << rec.getSize() << endl;
      return ++errs;
   }
   unsigned i = 0;
   for (auto it = correct.begin(); it != correct.end(); it++, i++) {
      const string &ckey = it->first;
      const string &cval = it->second;
      string key,val;
      if (rec.getData(i,key,val)) {
          if (key != ckey) {
              cerr << "EPD test: key mismatch" << endl;
              ++errs;
          }
          if (val != cval) {
              cerr << "EPD test: value mismatch" << endl;
              ++errs;
          }
      }
      else {
          cerr << "EPD test: failed to retrieve key " << i << endl;
          ++errs;
      }
   }
   return errs;
}


static int testEPD()
{
   static string epd1 = "r1b1k2r/ppq3b1/2p1pp2/P2pPpNp/1P1P1P2/2P4P/6P1/RN1QR1K1 w kq - c1 \"3 0\"; c2 \"1.000\";\n";
   static string epd2 = "8/6k1/5R2/K3p1P1/P3Pp2/8/nPb5/8 w - - bm Re6 Rf5; c0 \"Arasan-Crafty, test game 2017\";\n";
   typedef vector < pair<string, string> > pairvect;
   pairvect correct1,correct2;
   correct1.push_back(pair<string,string>("c1","\"3 0\""));
   correct1.push_back(pair<string,string>("c2","\"1.000\""));
   correct2.push_back(pair<string,string>("bm","Re6 Rf5"));
   correct2.push_back(pair<string,string>("c0","\"Arasan-Crafty, test game 2017\""));

   struct Case
   {
       string epd;
       pairvect correct;
       Case(string e, const pairvect &corr):
           epd(e),correct(corr)
           {
           }
   } cases[2] = { Case(epd1,correct1), Case(epd2,correct2) };

   EPDRecord rec;
   Board board;
   int errs = 0;
   for (int i = 0; i < 2; i++) {
       const Case &acase = cases[i];
       stringstream s(acase.epd);
       ChessIO::readEPDRecord(s,board,rec);
       errs += testRec(rec,acase.correct);
       if (errs) continue;
       ostringstream out;
       ChessIO::writeEPDRecord(out,board,rec);
       errs += testRec(rec,acase.correct);
       if (out.str() != acase.epd) {
           cerr << "EPD test: output string != input string" << endl;
           ++errs;
       }
       rec.clear();
       if (rec.getSize() != 0) {
           cerr << "EPD test: invalid size after clear" << endl;
           ++errs;
       }
   }
   return errs;
}

static int testPerft()
{
   // Perft tests for move generator - thanks to Martin Sedlak & Steve Maugham
   static const struct TestCase
   {
      string fen;
      int depth;
      uint64_t result;
      TestCase(const string &s, int d, uint64_t r) :
         fen(s),depth(d),result(r)
         {
         }
   } cases[28] = {
      // avoid illegal ep (thanks to Steve Maughan):
      TestCase("3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1",6,1134888),
      TestCase("8/8/8/8/k1p4R/8/3P4/3K4 w - - 0 1",6,1134888),
      // avoid illegal ep #2
      TestCase("8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1",6,1015133),
      TestCase("8/b2p2k1/8/2P5/8/4K3/8/8 b - - 0 1",6,1015133),
      //en passant capture checks opponent:
      TestCase("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1",6,1440467),
      TestCase("8/5k2/8/2Pp4/2B5/1K6/8/8 w - d6 0 1",6,1440467),
      // short castling gives check:
      TestCase("5k2/8/8/8/8/8/8/4K2R w K - 0 1",6,661072),
      TestCase("4k2r/8/8/8/8/8/8/5K2 b k - 0 1",6,661072),
      // long castling gives check:
      TestCase("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1",6,803711),
      TestCase("r3k3/8/8/8/8/8/8/3K4 b q - 0 1",6,803711),
      // castling (including losing cr due to rook capture):
      TestCase("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1",4,1274206),
      TestCase("r3k2r/7b/8/8/8/8/1B4BQ/R3K2R b KQkq - 0 1",4,1274206),
      // castling prevented:
      TestCase("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1",4,1720476),
      TestCase("r3k2r/8/5Q2/8/8/3q4/8/R3K2R w KQkq - 0 1",4,1720476),
      // promote out of check:
      TestCase("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1",6,3821001),
      TestCase("3K4/8/8/8/8/8/4p3/2k2R2 b - - 0 1",6,3821001),
      // discovered check:
      TestCase("8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1",5,1004658),
      TestCase("5K2/8/1Q6/2N5/8/1p2k3/8/8 w - - 0 1",5,1004658),
      // promote to give check:
      TestCase("4k3/1P6/8/8/8/8/K7/8 w - - 0 1",6,217342),
      TestCase("8/k7/8/8/8/8/1p6/4K3 b - - 0 1",6,217342),
      // underpromote to check:
      TestCase("8/P1k5/K7/8/8/8/8/8 w - - 0 1",6,92683),
      TestCase("8/8/8/8/8/k7/p1K5/8 b - - 0 1",6,92683),
      // self stalemate:
      TestCase("K1k5/8/P7/8/8/8/8/8 w - - 0 1",6,2217),
      TestCase("8/8/8/8/8/p7/8/k1K5 b - - 0 1",6,2217),
      // stalemate/checkmate:
      TestCase("8/k1P5/8/1K6/8/8/8/8 w - - 0 1",7,567584),
      TestCase("8/8/8/8/1k6/8/K1p5/8 b - - 0 1",7,567584),
      // double check:
      TestCase("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1",4,23527),
      TestCase("8/5k2/8/5N2/5Q2/2K5/8/8 w - - 0 1",4,23527)
   };
   int errs = 0;
   for (int i = 0; i<28; i++) {
      const TestCase &acase = cases[i];
      Board board;

      if (!BoardIO::readFEN(board, acase.fen.c_str())) {
         cerr << "testPerft: error in test case " << i << " error in FEN: " << acase.fen << endl;
         ++errs;
         continue;
      }
      uint64_t result;
      if ((result = RootMoveGenerator::perft(board,acase.depth)) != acase.result) {
         cerr << "testPerft: error in test case " << i << " wrong result: " << result << endl;
         ++errs;
      }
   }
   return errs;
}


int doUnit() {

   int errs = 0;
   errs += testWouldAttack();
   errs += testNotation();
   errs += testIsPinned();
   errs += testGetPinned();
   errs += testSee();
   errs += testPGN();
   errs += testEval();
   errs += testBitbases();
   errs += testDrawEval();
   errs += testCheckStatus();
   errs += testEPD();
   errs += testPerft();
   return errs;
}
