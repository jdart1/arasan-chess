// Copyright 1996, 2013, 2014, 2017, 2021 by Jon Dart.  All Rights Reserved.

// Utility to add ECO codes to PGN game files.

#include "board.h"
#include "legal.h"
#include "movegen.h"
#include "options.h"
#include "movearr.h"
#include "notation.h"
#include "chessio.h"
#include "eco.h"
#include "movearr.h"
#include "globals.h"
#include "chessio.h"

extern "C"
{
#include <stddef.h>
#include <string.h>
#include <ctype.h>
};
#include <fstream>
#include <iostream>
#include <ctype.h>
#include <vector>

using namespace std;

static void show_usage()
{
   cerr << "Usage: ecocoder pgn_file" << endl;
}



int CDECL main(int argc, char **argv)
{
   Bitboard::init();
   Board::init();
   Attacks::init();
   Scoring::init();
   if (!initGlobals(argv[0], false)) {
      cleanupGlobals();
      exit(-1);
   }
   atexit(cleanupGlobals);

   Board board;
   ECO eco;
   int arg = 1;

   if (argc <=1)
   {
      show_usage();
      return -1;
   }
   else
   {
      ifstream pgn_file( argv[arg], ios::in | ios::binary);
      int c;
      ColorType side;
      string result, white, black;
      if (!pgn_file.good()) {
         cerr << "could not open file " << argv[arg] << endl;
         exit(-1);
      }
      else
      {
         vector<ChessIO::Header> hdrs;
         while (!pgn_file.eof())
         {
            long first;
            MoveArray moves;
            board.reset();
            side = White;
            while (pgn_file.good() && (c = pgn_file.get()) != EOF)
            {
               if (c=='[')
               {
                  int c1 = pgn_file.get();
                  if (c1 == 'E') {
                     int c2 = pgn_file.get();
                     if (c2 == 'v') {
                        pgn_file.putback(c2);
                        pgn_file.putback(c1);
                        pgn_file.putback(c);
                        break;
                     }
                  }
               }
            }
            hdrs.clear();
            ChessIO::collect_headers(pgn_file,hdrs,first);
            if (!hdrs.size()) continue;
            bool ok = true;
            bool done = false;
            bool exit = false;
            while (ok && !exit) {
               ChessIO::Token tok = ChessIO::get_next_token(pgn_file);
               string num;
               switch (tok.type) {
               case ChessIO::Eof: {
                  exit = true;
                  break;
               }
               case ChessIO::Number: {
                  num = tok.val;
                  break;
               }
               case ChessIO::GameMove: {
                  if (done) {
                     // we should not have moves after the result
                     // if it looks like a tag, push it back
                     if (tok.val.size() && tok.val[0] == '[') {
                        for (int i = (int)tok.val.size()-1; i >= 0; --i)
                           pgn_file.putback(tok.val[0]);
                     }
                     exit = true;
                     break;
                  }
                  // parse the move
                  Move m = Notation::value(board,side,Notation::InputFormat::SAN,tok.val);
                  if (IsNull(m) ||
                      !legalMove(board,StartSquare(m),
                                 DestSquare(m))) {
                     // echo to both stdout and stderr
                     cerr << "Illegal move: " << tok.val << endl;
                     cout << "Illegal move: " << tok.val << endl;
                     ok = false;
                  }
                  else {
                     BoardState bs = board.state;
                     string img;
                     // convert to SAN
                     Notation::image(board,m,Notation::OutputFormat::SAN,img);
                     moves.add_move(board,bs,m,img,false);
                     board.doMove(m);
                  }
                  side = OppositeColor(side);
                  break;
               }
               case ChessIO::Unknown: {
                  if (done) {
                     // ignore unknown text after result
                     // if it looks like a tag, push it back
                     if (tok.val.size() && tok.val[0] == '[') {
                        for (int i = (int)tok.val.size()-1; i >= 0; --i)
                           pgn_file.putback(tok.val[0]);
                     }
                     exit = true;
                     break;
                  }
                  cerr << "Unrecognized text: " << tok.val << endl;
                  break;
               }
               case ChessIO::Comment: {
                  // ignored for now
                  break;
               }
               case ChessIO::Result: {
                  result = tok.val;
                  done = true;
                  break;
               }
               case ChessIO::OpenVar:
                   cerr << "Warning: variations not supported" << endl;
                   done = true;
               default:
                   break;

               } // end switch
            }
            // output header
            string ecoC = "";
            int found = ChessIO::get_header(hdrs,"ECO",ecoC);
            if (!found) {
               string name;
               eco.classify(moves,ecoC,name);
               if (ecoC != "") {
                  ChessIO::Header ecoHeader("ECO",ecoC.c_str());
                  hdrs.push_back(ecoHeader);
               }
            }
            else if (ecoC == "") {
               string name;
               eco.classify(moves,ecoC,name);
               if (ecoC != "") {
                  ChessIO::Header ecoHeader("ECO",ecoC.c_str());
                  for (auto it = hdrs.begin(); it != hdrs.end(); it++) {
                     const ChessIO::Header &hdr = *it;
                     if (hdr.tag() == "ECO") { // replace old header
                        *it = ecoHeader;
                        break;
                     }
                  }
               }
            }
            if (moves.num_moves()>0) ChessIO::store_pgn(cout,moves,result,hdrs);
         }
         pgn_file.close();
      }
   }

   return 0;
}
