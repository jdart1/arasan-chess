// Copyright 2016, 2017, 2019-2021 by Jon Dart.  All Rights Reserved.

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
   unsigned minPly;
   unsigned maxPly;
   unsigned minGamePly;
   unsigned samplesPerGame;
   unsigned maxRetries;
   unsigned minSampleDistance;
   bool randomMoves;
   int maxScore;
   int quiesce;

   SelectOptions() :
      minPly(25),
      maxPly(150),
      minGamePly(36),
      samplesPerGame(2),
      maxRetries(6),
      minSampleDistance(8),
      randomMoves(false),
      maxScore(30*Params::PAWN_VALUE),
      quiesce(0)
      {
      }
} selOptions;

static std::mt19937_64 random_engine;

static unordered_map<hash_t,double> *positions;

static const int SEARCH_DEPTH = 12;

static void show_usage()
{
   cerr << "Usage: pgnselect [options] pgn_file" << endl;
   cerr << "Options:" << endl;
   cerr << "-d <int> - min ply distance between samples" << endl;
   cerr << "-max <int> - max ply to sample" << endl;
   cerr << "-min <int> - min ply to sample" << endl;
   cerr << "-n <int> - samples per game" << endl;
   cerr << "-r - insert random moves" << endl;
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

static bool exclude(const Board &board) {
    return ((board.getMaterial(White).kingOnly() &&
              board.getMaterial(Black).infobits() == Material::KP) ||
           (board.getMaterial(Black).kingOnly() &&
              board.getMaterial(White).infobits() == Material::KP)) ||
        board.isLegalDraw() ||
        Scoring::theoreticalDraw(board);
}

static bool ok_to_sample(const Board &board, SearchController *searcher, Statistics &stats, string &fen) 
{
    // omit KPK and drawn positions
    if (exclude(board)) {
        return false;
    }
    else {
        int val = std::abs(score(searcher,board,stats));
        if (val > selOptions.maxScore) {
            return false;
        }
        else if (selOptions.quiesce) {
            // obtain the quiet position at the end of the
            // PV
            Board tmp(board);
            for (int len = 0; !IsNull(stats.best_line[len]) && len < Constants::MaxPly; len++) {
                tmp.doMove(stats.best_line[len]);
            }
            if (exclude(tmp)) {
                return false;
            }
            else {
                // verify actually is quiet
                RootMoveGenerator mg(tmp);
                Move moves[Constants::MaxMoves];
                unsigned n = mg::generateCaptures(tmp,moves,true,board.occupied[board.oppositeSide()]);
                for (unsigned i = 0; i < n; i++) {
                    if (see(tmp,moves[i])>0) {
                        return false;
                    }
                }
                // update FEN to reflect quiet position
                stringstream s;
                BoardIO::writeFEN(tmp,s,0);
                fen = s.str();
            }
        }
    }
    return true;
}

static void sample(vector<string> &positions,SearchController *searcher, Statistics &stats) 
{
    const int count = int(positions.size());
    if (count <= 0) return;
    const unsigned sampleTarget = std::min<unsigned>(selOptions.samplesPerGame,count/selOptions.minSampleDistance);
    std::uniform_int_distribution<unsigned> dist(0,unsigned(count-1));
    vector<unsigned> sampled;
    for (unsigned i = 0; i < selOptions.maxRetries && sampled.size() < sampleTarget; i++) {
        unsigned idx = dist(random_engine);
        // verify idx is not wihin minSampleDistance of an existing
        // sample
        bool close = false;
        for (const unsigned &i : sampled) {
            if (static_cast<unsigned>(std::abs(static_cast<int>(i)-static_cast<int>(idx))) < selOptions.minSampleDistance) {
                close = true;
                continue;
            }
        }
        if (close) continue; // re-sample
        string fen(positions[idx]);
        Board board;
        (void)BoardIO::readFEN(board,fen);
        if (selOptions.randomMoves) {
            RootMoveGenerator mg(board);
            Move allmoves[Constants::MaxMoves];
            Move move;
            int count = 0;
            while (!IsNull(move = mg.nextMove(count))) {
                allmoves[count++] = move;
            }
            if (count > 1) {
                std::uniform_int_distribution<unsigned> move_dist(0,unsigned(count-1));
                Move randomMove = allmoves[move_dist(random_engine)];
                board.doMove(randomMove);
            }
        }
        if (ok_to_sample(board,searcher,stats,fen)) {
            cout << fen << endl;
            sampled.push_back(idx);
        }
    }
}

int CDECL main(int argc, char **argv)
{
   Bitboard::init();
   Board::init();
   Attacks::init();
   Scoring::init();
   Search::init();
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
      auto processInt = [&arg, &argc, &argv](unsigned &opt, const string &name) {
         if (++arg < argc) {
            stringstream s(argv[arg]);
            s >> opt;
            if (s.bad() || s.fail()) {
               cerr << "expected number after -" << name << endl;
               exit(-1);
            }
         }
         else {
            cerr << "expected number after -" << name << endl;
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
            processInt(selOptions.minPly, "min");
         }
         else if (strcmp(argv[arg], "-n") == 0) {
            processInt(selOptions.samplesPerGame, "n");
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
         vector<string> positions;
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
            unsigned ply = 0;
            positions.clear();
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
                     if (++ply>=selOptions.minPly) {
                         if (ply<selOptions.maxPly) {
                             stringstream s;
                             BoardIO::writeFEN(board,s,0);
                             positions.push_back(s.str());
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
            } // end of game
            if (ply >= selOptions.minGamePly) {
               sample(positions,searcher,stats);
            }
         }

         pgn_file.close();
      }
      delete searcher;
   }

   delete positions;
   return 0;
}
