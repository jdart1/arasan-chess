// Copyright 2016, 2017, 2019 by Jon Dart.  All Rights Reserved.

// Utility to extract selected positions from PGN files into an EPD file.

#include "board.h"
#include "boardio.h"
#include "legal.h"
#include "movegen.h"
#include "options.h"
#include "movearr.h"
#include "notation.h"
#include "chessio.h"
#include "movearr.h"
#include "globals.h"
#include "chessio.h"
#include "search.h"
#include "see.h"
extern "C"
{
#include <string.h>
#include <ctype.h>
};
#include <cstddef>
#include <fstream>
#include <iostream>
#include <ctype.h>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;

static struct SelectOptions
{
   int minPly;
   int maxPly;
   int sampleInterval;
   int minSampleDistance;
   bool randomMoves;
   int maxScore;
   int quiesce;

   SelectOptions() :
      minPly(20),
      maxPly(150),
      sampleInterval(16),
      minSampleDistance(3),
      randomMoves(false),
      maxScore(30*Params::PAWN_VALUE),
      quiesce(0)
      {
      }
} selOptions;

static std::mt19937_64 random_engine;

static unordered_map<hash_t,double> *positions;

static const char *CASTLE_STATUS_KEY = "c1";

static const int SEARCH_DEPTH = 2;

static void show_usage()
{
   cerr << "Usage: pgnselect [options] pgn_file" << endl;
   cerr << "Options:" << endl;
   cerr << "-max <int> - max ply to sample" << endl;
   cerr << "-min <int> - min ply to sample" << endl;
   cerr << "-d <int> - min ply distance between samples" << endl;
   cerr << "-r - insert random moves" << endl;
   cerr << "-s <int> - set sample interval (plies)" << endl;
}

static int score(SearchController *searcher,const Board &board,
                 Statistics &stats)
{
   stats.clear();
   searcher->findBestMove(board,
                          FixedDepth,
                          999,
                          0,            /* extra time allowed */
                          SEARCH_DEPTH, /* ply limit */
                          false,        /* background */
                          false, /* UCI */
                          stats,
                          TalkLevel::Silent);

   return stats.value;
}

static int ok_to_insert(const Board &board, SearchController *searcher, Statistics &stats) {
   int ok = !((board.getMaterial(White).kingOnly() &&
                board.getMaterial(Black).infobits() == Material::KP) ||
               (board.getMaterial(Black).kingOnly() &&
                board.getMaterial(White).infobits() == Material::KP)) &&
              !Scoring::isLegalDraw(board) &&
              !Scoring::theoreticalDraw(board) &&
               positions->count(board.hashCode()) == 0 &&
               (board.getMaterial(White).men() + board.getMaterial(Black).men() > 5);
   if (ok) {
      int val = std::abs(score(searcher,board,stats));
      if (val > selOptions.maxScore) {
          ok = false;
      }
      else {
          std::normal_distribution<double> dist(0,double(5*Params::PAWN_VALUE));
          // favor positions with more moderate scores
          ok = val <= 5*Params::PAWN_VALUE ||
               val >= std::round(std::abs(dist(random_engine)));
      }
   }
   return ok;
}

int CDECL main(int argc, char **argv)
{
   Bitboard::init();
   Attacks::init();
   Scoring::init();
   if (!initGlobals(argv[0], false)) {
      cleanupGlobals();
      exit(-1);
   }
   atexit(cleanupGlobals);

   random_engine.seed(getRandomSeed());

   positions = new unordered_map<hash_t, double>();

   Board board;

   if (argc <= 1)
   {
      show_usage();
      return -1;
   }
   else
   {
      int arg = 1;
      auto processInt = [&arg, &argc, &argv](int &opt, const string &name) {
         if (++arg < argc) {
            stringstream s(argv[arg]);
            s >> opt;
            if (s.bad() || s.fail()) {
               cerr << "expected integer after -" << name << endl;
               exit(-1);
            }
         }
         else {
            cerr << "expected integer after -" << name << endl;
            exit(-1);
         }
      };
      for (; arg < argc && *(argv[arg]) == '-'; ++arg) {
         if (strcmp(argv[arg], "-r") == 0) {
            selOptions.randomMoves = true;
         }
         else if (strcmp(argv[arg], "-d") == 0) {
            processInt(selOptions.minSampleDistance, "d");
         }
         else if (strcmp(argv[arg], "-max") == 0) {
            processInt(selOptions.maxPly, "max");
         }
         else if (strcmp(argv[arg], "-min") == 0) {
            processInt(selOptions.minPly, "-min");
         }
         else if (strcmp(argv[arg], "-s") == 0) {
            processInt(selOptions.sampleInterval, "-s");
         }
         else if (strcmp(argv[arg], "-q") == 0) {
            selOptions.quiesce++;
         }
         else {
            show_usage();
            exit(-1);
         }
      }
      if (arg >= argc) {
         show_usage();
         exit(-1);
      }

      SearchController *searcher = new SearchController();
      Statistics stats;

      // This ensures PVs are not terminated by a hash hit
      options.search.hash_table_size = 0;

      ifstream pgn_file(argv[arg], ios::in);
      ColorType side;
      string result, white, black;
      if (!pgn_file.good()) {
         cerr << "could not open file " << argv[arg] << endl;
         exit(-1);
      }
      else
      {
         vector<ChessIO::Header> hdrs;
         bool quit = false;
         while (!quit && !pgn_file.eof()) {
            long first;
            MoveArray moves;
            board.reset();
            side = White;
            if (!pgn_file.good()) {
               if (pgn_file.bad() || pgn_file.fail()) {
                  cerr << "read error" << endl;
                  quit = true;
               }
            }
            else {
               pgn_file.ignore('[');
               if (!pgn_file.good()) {
                  if (!pgn_file.eof()) cerr << "read error" << endl;
                  quit = true;
               }
               else {
                  pgn_file.putback('[');
               }
            }
            if (quit) break;
            hdrs.clear();
            ChessIO::collect_headers(pgn_file, hdrs, first);
            if (!hdrs.size()) continue;
            bool ok = true;
            bool done = false;
            bool exit = false;
            int ply = 0;
            std::uniform_int_distribution<unsigned> dist(0, selOptions.sampleInterval - 1);
            int next = selOptions.minPly + dist(random_engine);
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
                        for (int i = (int)tok.val.size() - 1; i >= 0; --i)
                           pgn_file.putback(tok.val[0]);
                     }
                     exit = true;
                     break;
                  }
                  // parse the move
                  Move m = Notation::value(board, side, Notation::InputFormat::SAN, tok.val);
                  if (IsNull(m) ||
                      !legalMove(board, StartSquare(m),
                                 DestSquare(m))) {
                     cerr << "Illegal move: " << tok.val << endl;
                     ok = false;
                  }
                  else {
                     BoardState bs = board.state;
                     string img;
                     // convert to SAN
                     Notation::image(board, m, Notation::OutputFormat::SAN, img);
                     moves.add_move(board, bs, m, img, false);
                     board.doMove(m);
                     ++ply;
                     if (ply >= next && ply <= selOptions.maxPly) {
                        BoardState state(board.state);
                        Move randomMove = NullMove;
                        if (selOptions.randomMoves) {
                           RootMoveGenerator mg(board);
                           Move allmoves[Constants::MaxMoves];
                           Move move;
                           int count = 0;
                           while (!IsNull(move = mg.nextMove(count))) {
                              allmoves[count] = move;
                           }
                           if (count > 1) {
                              std::uniform_int_distribution<unsigned> move_dist(0,count-1);
                              randomMove = allmoves[move_dist(random_engine)];
                              board.doMove(randomMove);
                           }
                        }

                        // omit KPK and drawn positions
                        bool ok = ok_to_insert(board,searcher,stats);
                        if (ok) {
                           Board tmp(board);
                           if (selOptions.quiesce) {
                              // obtain the quiet position at the end of the
                              // PV
                              for (int len = 0; !IsNull(stats.best_line[len]) && len < Constants::MaxPly; len++) {
                                 board.doMove(stats.best_line[len]);
                              }
                              ok = ok_to_insert(board,searcher,stats);
                              // verify actually is quiet
                              RootMoveGenerator mg(board);
                              Move moves[Constants::MaxMoves];
                              unsigned n = mg::generateCaptures(board,moves,true,board.occupied[board.oppositeSide()]);
                              for (unsigned i = 0; i < n; i++) {
                                 if (see(board,moves[i])>0) {
                                    ok = false;
                                    break;
                                 }
                              }
                           }

                           if (ok) {
                              EPDRecord rec;
                              stringstream cs_string;
                              cs_string << "\"" << (int)board.castleStatus(White) << ' ' <<
                                 (int)board.castleStatus(Black) << "\"";
                              string key(CASTLE_STATUS_KEY);
                              rec.add(key,cs_string.str().c_str());
                              ChessIO::writeEPDRecord(cout,board,rec);
                              next += selOptions.minSampleDistance + (rand() % (selOptions.sampleInterval - selOptions.minSampleDistance));
                           }
                           board = tmp;
                        }
                        if (!IsNull(randomMove)) {
                           board.undoMove(randomMove,state);
                        }
                     }
                  }
                  side = OppositeColor(side);
                  break;
               }
               case ChessIO::Unknown: {
                  if (done) {
                     // ignore unknown text after result
                     // if it looks like a tag, push it back
                     if (tok.val.size() && tok.val[0] == '[') {
                        for (int i = (int)tok.val.size() - 1; i >= 0; --i)
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
               default:
                  break;
               } // end switch
            }
         }

         pgn_file.close();
      }
      delete searcher;
   }

   delete positions;
   return 0;
}
