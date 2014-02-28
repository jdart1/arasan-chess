// Copyright 2013, 2014 by Jon Dart.  All Rights Reserved.

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

#include <iostream>

#include <string>

using namespace std;

static int testIsPinned() {

    struct IsPinnedData {
       string fen;
       Square start, dest;
       int result;
       IsPinnedData(const char *fenStr, Square s, Square d, int res) {
          fen = fenStr; 
          if (!BoardIO::readFEN(board, string(fenStr))) {
             cerr << "error in FEN: " << fenStr << endl;
             return;
          }
          dest = d;
          start = s;
          result = res;
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

static int testSee() {

    struct SeeData {
       string fen;
       Move move;
       int result;
       SeeData(const char *fenStr, const char *moveStr, int res) {
          fen = fenStr; 
          if (!BoardIO::readFEN(board, fenStr)) {
             cerr << "error in FEN: " << fenStr << endl;
             return;
          }
          move = Notation::value(board,board.sideToMove(),
                                 Notation::SAN_IN,moveStr);
          if (IsNull(move)) {
             cerr << "error in move: " << moveStr << endl;
             return;
          }
          result = res;
       }
       Board board;
    };

    const SeeData seeData[] = {
       SeeData("4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - -","hxg4",0),
       SeeData("4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - -","hxg4",0),
       SeeData("4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - -","Bxf3",0),
       SeeData("2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - -","dxe5",PAWN_VALUE),
       SeeData("7r/5qpk/p1Qp1b1p/3r3n/BB3p2/5p2/P1P2P2/4RK1R w - -","Re8",0),
       SeeData("6rr/6pk/p1Qp1b1p/2n5/1B3p2/5p2/P1P2P2/4RK1R w - -","Re8",-ROOK_VALUE),
       SeeData("7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - -","Re8",-ROOK_VALUE),
       SeeData("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -","f8=Q",BISHOP_VALUE-PAWN_VALUE),
       SeeData("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - -","f8=N",KNIGHT_VALUE-PAWN_VALUE),
       SeeData("7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -","f8=Q",QUEEN_VALUE-PAWN_VALUE),
       SeeData("7R/5P2/8/8/8/3K2r1/5p2/4k3 w - -","f8=B",BISHOP_VALUE-PAWN_VALUE),
       SeeData("7R/4bP2/8/8/1q6/3K4/5p2/4k3 w - -","f8=R",-PAWN_VALUE),
       SeeData("8/4kp2/2npp3/1Nn5/1p2PQP1/7q/1PP1B3/4KR1r b - -","Rxf1+",0),
       SeeData("8/4kp2/2npp3/1Nn5/1p2P1P1/7q/1PP1B3/4KR1r b - -","Rxf1+", 0),
       SeeData("2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - -","Qxc1",2*ROOK_VALUE-QUEEN_VALUE),
       SeeData("r2qk1nr/pp2ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq -","Nxe5",PAWN_VALUE),
       SeeData("6r1/4kq2/b2p1p2/p1pPb3/p1P2B1Q/2P4P/2B1R1P1/6K1 w - -","Bxe5",0),
       SeeData("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6","gxh6",0),
       SeeData("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R1B2B/PQ3P1P/3R2K1 w - h6","gxh6",PAWN_VALUE),
       SeeData("2r4r/1P4pk/p2p1b1p/7n/BB3p2/2R2p2/P1P2P2/4RK2 w - -","Rxc8",ROOK_VALUE),
       SeeData("2r5/1P4pk/p2p1b1p/5b1n/BB3p2/2R2p2/P1P2P2/4RK2 w - -","Rxc8",ROOK_VALUE),
       SeeData("2r4k/2r4p/p7/2b2p1b/4pP2/1BR5/P1R3PP/2Q4K w - -","Rxc5",BISHOP_VALUE),
       SeeData("8/pp6/2pkp3/4bp2/2R3b1/2P5/PP4B1/1K6 w - -","Bxc6",PAWN_VALUE-BISHOP_VALUE),
       SeeData("4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -","Rxe4",PAWN_VALUE-ROOK_VALUE),
       SeeData("4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - -","Bxe4",PAWN_VALUE)
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
        NotationData("r1r3k1/2n1ppbp/npp5/p2pP2p/P2P1N2/1PP5/3B1PPN/2Rb1RK1 w - -","Rfxd1")
    };

    int errs = 0;
    for (int i = 0; i < 15; i++) {
        Move m = Notation::value(notationData[i].board,
                                 notationData[i].board.sideToMove(),
                                 Notation::SAN_IN,
                                 notationData[i].moveStr);
        if (m == NullMove) {
           cout << "notation: error in case " << i << endl;
           ++errs;
        }
    } 
    Board board;
    string fenStr = "4Qnk1/pp1P1p2/6rp/8/3P4/r5BK/8/8 w - -";
    if (!BoardIO::readFEN(board, fenStr.c_str())) {
        cerr << "error in FEN: " << fenStr << endl;
        return ++errs;
    }
    Move m = Notation::value(board,White,Notation::WB_IN,"d7d8q");
    if (m == NullMove || StartSquare(m) != D7 || DestSquare(m) != D8 || PromoteTo(m) != Queen) {
       cout << "notation: error in case 16" << endl;
       ++errs;
    }
    fenStr = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -";
    if (!BoardIO::readFEN(board, fenStr.c_str())) {
        cerr << "error in FEN: " << fenStr << endl;
        return ++errs;
    }
    m = Notation::value(board,White,Notation::WB_IN,"e2e4");
    if (m == NullMove || StartSquare(m) != E2 || DestSquare(m) != E4 || PromoteTo(m) != Empty) {
       cout << "notation: error in case 17" << endl;
       ++errs;
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
      int errs = 0;
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
    // verify eval results are symmetrical (White/Black, right/left)
    const int CASES = 8;
    static const string fens[CASES] = {
        "8/4K3/8/1NR5/8/4k1r1/8/8 w - -",
        "8/4K3/8/1N6/6p1/4k2p/8/8 w - -",
        "8/4K3/8/1r6/6B1/4k2N/8/8 w - -",
        "3b4/1n3n2/1pk3Np/p7/P4P1p/1P6/5BK1/3R4 b - -",
        "8/3r1ppk/8/P6P/3n4/2K5/R2B4/8 b - -",
        "1rb1r1k1/2q2pb1/pp1p4/2n1pPPQ/Pn1BP3/1NN4R/1PP4P/R5K1 b - -",
        "6k1/1b4p1/5p1p/pq3P2/1p1BP3/1P2QR1P/P1r3PK/8 w - -",
        "8/5pk1/7p/3p1R2/p1p3P1/2P2K1P/1P1r4/8 w - -"
    };
    int errs = 0;
    SearchController c;
    for (int i = 0; i < CASES; i++) {
        Board board;
        if (!BoardIO::readFEN(board, fens[i].c_str())) {
            cerr << "testEval case " << i << " error in FEN: " << fens[i] << endl;
            ++errs;
            continue;
        }
        Scoring *s = new Scoring(&c.hashTable);
        int eval1 = s->evalu8(board);
        board.flip();
        int eval2 = s->evalu8(board);
        if (eval1 != eval2) {
            ++errs;
            cerr << "testEval case " << i << " eval mismatch" << endl;
        }
        board.flip2();
        int eval3 = s->evalu8(board);
        if (eval1 != eval3) {
            ++errs;
            cerr << "testEval case " << i << " eval mismatch" << endl;
        }
		delete s;
    }
    return errs;
}

int doUnit() {

   int errs = 0;
   errs += testNotation();
   errs += testIsPinned();
   errs += testSee();
   errs += testPGN();
   errs += testEval();
   return errs;
}
