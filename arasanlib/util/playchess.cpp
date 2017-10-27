// Copyright 2010, 2011, 2012, 2017 by Jon Dart. All Rights Reserved.
#include "board.h"
#include "notation.h"
#include "legal.h"
#include "hash.h"
#include "globals.h"
#include "chessio.h"
#include "search.h"
#include <iostream>
#include <fstream>
#include <ctype.h>

// Filters PGN games and removes those for which the terminal
// position's eval differs significantly from the game result.

static void usage() 
{
   cerr << "playchess -t <time limit (sec.)> -min <min ply> -e <min ELO> pgn_file(s)" << endl;
}

int CDECL main(int argc, char **argv)
{
   Bitboard::init();
   initOptions(argv[0]);
   Attacks::init();
   Scoring::init();
   options.search.hash_table_size = 64*1024*1024;
   if (!initGlobals(argv[0], false)) {
      cleanupGlobals();
      exit(-1);
   }
   atexit(cleanupGlobals);
   delayedInit();
   if (EGTBMenCount) {
      cerr << "Initialized tablebases" << endl;
   }
   options.book.book_enabled = options.log_enabled = 0;

   int minMoves = 30;
   int minELO = 0;
   // time limit in ms.
   int timeLimit = 5000;

   if (argc ==1) {
      cerr << "usage: playchess.exe -t <search time> -m <min ELO> <input file>" << endl;
      return -1;
   }
   else {
      int arg = 1;
      auto processInt = [&arg,&argc,&argv] (int &opt, const string &name) {
         if (++arg < argc) {
            stringstream s(argv[arg]);
            s >> opt;
            if (s.bad() || s.fail()) {
               cerr << "expected integer after -" << name  << endl;
               exit(-1);
            }
         } else {
            cerr << "expected integer after -" << name << endl;
            exit(-1);
         }
      };
      for (;arg < argc && *(argv[arg]) == '-';++arg) {
         if (strcmp(argv[arg],"-min")==0) {
            processInt(minMoves,"m");
         }
         else if (strcmp(argv[arg],"-t")==0) {
            processInt(timeLimit,"t");
            timeLimit *= 1000; // convert to milliseconds
         }
         else if (strcmp(argv[arg],"-e")==0) {
            processInt(minELO,"e");
         }
         else {
            usage();
            exit(-1);
         }
      }
      if (arg >= argc) {
         usage();
         exit(-1);
      }
      SearchController *searcher = new SearchController();
      Statistics stats;
      for (;arg < argc;arg++) {
         ifstream pgn_file( argv[arg], ios::in);
         int c;
         ColorType side;
         string result, white, black;
         if (!pgn_file.good()) {
            cerr << "could not open file " << argv[arg] << endl;
            exit(-1);
         }
         else {
            vector<ChessIO::Header> hdrs;

            Board board;
            while (!pgn_file.eof()) {

               long first;
               MoveArray moves;
               board.reset();
               side = White;
               while (pgn_file.good() && (c = pgn_file.get()) != EOF) {
                  if (c=='[') {
                     pgn_file.putback(c);
                     break;
                  }
               }
               if (pgn_file.eof()) break;
               hdrs.clear();
               ChessIO::collect_headers(pgn_file,hdrs,first);

               float last_score = -1;
               int valid = 1;
               int var = 0;
               bool done = false;
               while (!done) {
                  ChessIO::Token tok = ChessIO::get_next_token(pgn_file);
                  switch(tok.type) {
                  case ChessIO::Eof:
                     done = true;
                     break;
                  case ChessIO::Number:
                     continue;
                  case ChessIO::GameMove: {
                     if (var) continue;  
                     // parse the move
                     Move m;
                     m = Notation::value(board,board.sideToMove(),
                                         Notation::InputFormat::SAN,tok.val);
                     if (IsNull(m) ||
                         !legalMove(board,StartSquare(m), DestSquare(m))) {
                        // echo to both stdout and stderr
                        cerr << "Illegal move: " << tok.val << endl;
                        cout << "Illegal move: " << tok.val << endl;
                        valid = 0;

                     }
                     else if (valid) {
                        string result;
                        Notation::image(board,m,Notation::OutputFormat::SAN,result);
                        BoardState bs = board.state;
                        moves.add_move(board,bs,m,result,false);

                        board.doMove(m);
                     }
                     side = OppositeColor(side);
                     break;
                  }
                  case ChessIO::Unknown:
                     if (strcmp(tok.val.c_str(),"(") == 0)
                        ++var;
                     else if (strcmp(tok.val.c_str(),")")==0) {
                        --var;   
                     } else {
                        cerr << "Unrecognized text: " << tok.val << endl;
                        valid = 0;
                     }
                     break;
                  case ChessIO::Comment:
                     break;
                  case ChessIO::Result: {
                     result = tok.val;
                     if (result == "#") {
                        last_score = 10000.0F;
                     }
                     else if (result == "1/2-1/2" &&
                              Scoring::isDraw(board)) {
                        last_score = 0.0;
                     } else {
                        stats.clear();
                        searcher->findBestMove(board,
                                               FixedTime,
                                               timeLimit,
                                               0,            /* extra time allowed */
                                               Constants::MaxPly,           /* ply limit */
                                               false,         /* background */
                                               false, /* UCI */
                                               stats,
                                               Silent);
                     
                        last_score = float(stats.value)/Params::PAWN_VALUE;
                     }
                     if (board.sideToMove() != White) last_score = -last_score;
                     done = true;
                  }
                  default:   
                     break;
                  } // end switch
               }
               // done with a game
               if (moves.num_moves() < (unsigned)minMoves || !valid) {
                  continue;
               }
               if (strcmp(result.c_str(),"1/2-1/2")==0) {
                  if (last_score >=1.0 || last_score <=-1.0) {
                     continue;
                  }
               }
               else if (strcmp(result.c_str(),"1-0")==0) {
                  if (last_score <= 1.5) continue;
               }
               else if (strcmp(result.c_str(),"0-1")==0) {
                  if (last_score >= -1.5) continue;
               }

               string eco;
               ChessIO::get_header(hdrs,"ECO",eco);

               // output header
               if (valid) {
                  string whiteELOStr,blackELOStr;
                  if (minELO > 0) {
                     if (ChessIO::get_header(hdrs,"WhiteElo",whiteELOStr) &&
                         ChessIO::get_header(hdrs,"BlackElo",blackELOStr)) {
                        int whiteElo=0, blackElo=0;
                        if (sscanf(whiteELOStr.c_str(),"%d",&whiteElo) &&
                            sscanf(blackELOStr.c_str(),"%d",&blackElo)) {
                           if (whiteElo < minELO || blackElo < minELO)
                              continue;
                        }
                     }
                     else                            // no ELO info available
                        continue;
                  }
                  ChessIO::store_pgn(cout, moves, result, hdrs);
               }
            }
            pgn_file.close();
         }
      }
   }

   return 0;
}
