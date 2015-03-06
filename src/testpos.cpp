// Copyright 2014-2015 by Jon Dart.  All Rights Reserved.

// Creates a file of FEN test positions + game results
// from a collection of PGNs.
//
// By default: uses all positions from a minimum ply out to tablebase
// range. Can also sample randomly from the PGNs (-s option).

#include "board.h"
#include "boardio.h"
#include "bhash.h"
#include "chessio.h"
#include "movegen.h"
#include "notation.h"
#include "types.h"
#include "debug.h"
#include "attacks.h"
#include "globals.h"
#include "util.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <stack>
#include <vector>
using namespace std;

extern "C"{
#include <math.h>
};

enum ResultType {White_Win, Black_Win, DrawResult, UnknownResult};
ResultType tmp_result;

static int min_ply = 16;

static int sample = 0;

static int max_ply_first_sample = 64;

static int min_men = 6;

static int samples_per_game = 4;

static int plies_between_samples = 25;

static int do_pgn(ifstream &infile, const string &in_name)
{
   ArasanVector<ChessIO::Header> hdrs;
   long games = 0L;
   ColorType side = White;
   while (!infile.eof() && infile.good()) {
      long first;
      side = White;
      hdrs.removeAll();
      int c;
      // skip to start of next header (handles cases where
      // comment follows end of previous game).
      while (infile.good() && (c = infile.get()) != EOF) {
         if (c=='[') {
            infile.putback(c);
            break;
         }
      }
      ChessIO::collect_headers(infile,hdrs,first);
      string result;
      if (!ChessIO::get_header(hdrs,"Result",result) || result == "*") {
         // game has no result
         continue;
      }
      ++games;
#ifdef _TRACE
      cout << "game " << games << endl;
#endif
      int move_num = 0;
      // process a game
      Board board;

      Board p1,p2;

      int next_sample = rand() % max_ply_first_sample;

      int ply = 0;

      int samples = 0;
      
      for (;;) {
         string num;
         ChessIO::Token tok = ChessIO::get_next_token(infile);
         if (tok.type == ChessIO::Eof)
            break;
         else if (tok.type == ChessIO::Comment) {
            // ignore
         }
         else if (tok.type == ChessIO::Number) {
             num = tok.val;
             stringstream s(tok.val);
             s >> move_num;
         }
         else if (tok.type == ChessIO::GameMove) {
            // parse the move
            Move move = Notation::value(board,side,Notation::SAN_IN,tok.val);
            if (IsNull(move)) {
                cerr << "Illegal move: " << tok.val << 
                   " in game " << games << ", file " <<
                   in_name << endl;
                break;
            }
            else {
               if ((board.getMaterial(White).men() +
                    board.getMaterial(Black).men() >= min_men) &&
                   board.repCount() == 0 && ply >= min_ply) {
                  if (!sample || (
                         ply >= next_sample && samples < samples_per_game)) {
                     BoardIO::writeFEN(board,cout,0);
                     cout << ' ' << result << endl;
                     next_sample = ply + plies_between_samples;
                     ++samples;
                  }
               }
            }
            board.doMove(move);
            ++ply;
            side = OppositeColor(side);
         }
         else if (tok.type == ChessIO::Unknown) {
            cerr << "Unrecognized text: " << tok.val << 
                         " in game " << games << ", file " <<
                          in_name << endl;
         }
         else if (tok.type == ChessIO::Result) {
            break;
         }
      }
   }
   return 0;
}

static void usage() {
    cerr << "Usage:" << endl;
    cerr << "testpos <input file(s)>" << endl;
}

int CDECL main(int argc, char **argv)
{
   Bitboard::init();
   initOptions(argv[0]);
   Attacks::init();
   Scoring::init();
   if (!initGlobals(argv[0], false)) {
      cleanupGlobals();
      exit(-1);
   }
   atexit(cleanupGlobals);

   srand((unsigned int)getCurrentTime());

   int arg = 1;
   while (arg < argc) {
      if (*argv[arg] == '-') {
         char c = argv[arg][1];
         if (c == 's') {
            ++sample;
            ++arg;
         } else {
            cerr << "unknown option: " << argv[arg] << endl;
            usage();
            exit(-1);
         }
      } else {
         break;
      }
   }
   
   if (arg >= argc) {
       cerr << "No input files specified." << endl;
       usage();
       return -1;
   }

   while (arg < argc) {
      string in_name = argv[arg++];
      ifstream infile;

      infile.open(in_name.c_str(), ios::in);
      if (!infile.good()) {
         cerr << "Can't open file: " << in_name << endl;
         return -1;
      }
      int result = do_pgn(infile, in_name);
      infile.close();
      if (result == -1) break;
   }

   return 0;
}
