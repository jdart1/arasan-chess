// Copyright 1996-2004, 2012-2014 by Jon Dart.  All Rights Reserved.

// Creates a EPD file of test positions from a collection of PGNs

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

// number of pages in the book.
static unsigned indexPages = 1;                    // default
// max ply depth processed for PGN games
static int maxPly = 70; 
static bool verbose = false;


static string output_name;

static unsigned minFrequency = 0;

class HashEntryBase 
{
   

   public:
    HashEntryBase( 
             const string &result,
              Move m,
              HashEntryBase *nxt);

    HashEntryBase *next;
    int white_win_loss;  // wins - losses for White
    uint32 count;
    int winloss;
    int score;
    Move move;
};


class HashEntryHead : public HashEntryBase 
{
   
   public:
   HashEntryHead( const string &f,
              const string &result,
              Move m,
              HashEntryHead *nxt) : fen(f),
              HashEntryBase(result,m,nxt)
   {
   }

    string fen;
};

HashEntryBase::HashEntryBase(
                     const string &result, Move m,
                       HashEntryBase *nxt)
   :next(nxt), white_win_loss(0),
     count(1),
     move(m)
{
   if (result == "1-0") {
      white_win_loss++;
   }
   else if (result == "0-1") {
      white_win_loss--;
   }
}

static std::map <uint64, HashEntryHead *>* hashTable = NULL;

// Add move to our in-memory data structure
static void
add_move(const Board & board, const Move &m, const string &result)
{
#ifdef _TRACE
   cout << "adding move ";
   Notation::image(board,m,Notation::SAN_OUT,cout);
   cout << endl;
#endif
   std::map<uint64,HashEntryHead *>::const_iterator it =
      hashTable->find(board.hashCode());
   HashEntryHead *be;
   if (it == hashTable->end())
      be = NULL;
   else
      be = (*it).second;
   if (be == NULL) {
       // position not found in hashtable
       HashEntryHead *new_entry;
       stringstream out;
       out << board;
       try {
          new_entry = new HashEntryHead(out.str(),result,
                                    m, NULL);
       } catch(std::bad_alloc) {
          cerr << "out of memory!" << endl;
          exit(-1);
       }
       (*hashTable)[board.hashCode()] = new_entry;
   }
   else {
      // update existing entry
      HashEntryBase *p = (HashEntryBase*)be;
      int found = 0;
      HashEntryBase *end = NULL;
      while (p && !found) {
         end = p;
         if (MovesEqual(p->move,m)) {
            ++found; break;
         }
         p = p->next;
      }
      if (found) {
         // Move already in hash table
         p->count++; 
         if (result == "1-0") {
            p->white_win_loss++;
         }
         else if (result == "0-1") {
            p->white_win_loss--;
         }
      }
      else {
         // Position is in hashtable, but not with this move.
         HashEntryBase *new_entry;
         try {
            new_entry = new HashEntryBase(result,
                                      m, NULL);
         } catch(std::bad_alloc) {
            cerr << "out of memory!" << endl;
            exit(-1);
         }
         end->next = new_entry;
      }
   }
}

static int do_pgn(ifstream &infile, const string &book_name)
{
   ArasanVector<ChessIO::Header> hdrs;
   long games = 0L;
   ColorType side = White;
   while (!infile.eof() && infile.good()) {
      long first;
      side = White;
      ResultType last_result = UnknownResult;
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
      if (!ChessIO::get_header(hdrs,"Result",result)) {
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
      int MIN_PLY = 16;
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
                   book_name << endl;
                break;
            }
            else {
               if ((board.getMaterial(White).men() +
                    board.getMaterial(Black).men() > 5) &&
                   board.repCount() == 0 && move_num*2 > MIN_PLY) {
                  add_move(board,move,result);
               }
            }
            board.doMove(move);
            side = OppositeColor(side);
         }
         else if (tok.type == ChessIO::Unknown) {
            cerr << "Unrecognized text: " << tok.val << 
                         " in game " << games << ", file " <<
                          book_name << endl;
         }
         else if (tok.type == ChessIO::Result) {
            break;
         }
         else {
            cerr << "warning: unknown token type: " << (int) tok.type << endl;
         }
      }
   }
   return 0;
}

static void usage() {
    cerr << "Usage:" << endl;
    cerr << "tunefile -m <min frequency> <input file(s)>" << endl;
}

bool cmp (const pair<Move,int> &x, const pair<Move,int>&y)
{
   return (x.second > y.second);
}


int CDECL main(int argc, char **argv)
{
   string book_name;

   Bitboard::init();
   initOptions(argv[0]);
   Attacks::init();
   Scoring::init();
   if (!initGlobals(argv[0], false)) {
      cleanupGlobals();
      exit(-1);
   }
   atexit(cleanupGlobals);

   hashTable =  new std::map< uint64, HashEntryHead*>;

   output_name = "";
   int arg = 1;
   while (arg < argc) {
      if (*argv[arg] == '-') {
         char c = argv[arg][1];
         switch(c) {
            case 'm':
               ++arg;
               minFrequency = (unsigned)atoi(argv[arg]);
               break;
            case 'v': 
                verbose = true;
                break;
            default:
               cerr << "Illegal switch: " << c << endl;
               usage();
               exit(-1);
         }
         ++arg;
      }
      else
         break;
   }

   if (arg >= argc) {
       cerr << "No input files specified." << endl;
       usage();
       return -1;
   }

   while (arg < argc) {
      book_name = argv[arg++];
      ifstream infile;

      infile.open(book_name.c_str(), ios::in);
      if (!infile.good()) {
         cerr << "Can't open file: " << book_name << endl;
         return -1;
      }
      if (verbose) cerr << "processing " << book_name << endl;
      int result = do_pgn(infile, book_name);
      infile.close();
      if (result == -1) break;
   }

   // Iterate through the hash table and output records
   if (verbose) cerr << "PGN processing complete." << endl;
   std::map< uint64, HashEntryHead *>::const_iterator it = hashTable->begin();
   uint32 total_moves = 0;
   unsigned long positions = 0;
   while (it != hashTable->end()) {
       HashEntryBase* p = (*it).second;
       HashEntryHead *first = (*it).second;
       // Note: (*it).first is the hash code
       const int black_to_move = BoardHash::sideToMove((*it).first) == Black;
       vector < pair<Move,int> > moves;
       int total_count = 0;
       while (p) {
          if (black_to_move) {
             p->winloss = (-p->white_win_loss*100)/(int)(p->count);
          }
          else {
             // white is on move
             p->winloss = (p->white_win_loss*100)/(int)(p->count);
          }
          total_count += p->count;
          p = p->next;
       }
       p = first;
       while (p) {
          int winWeight = (p->winloss+100)/2;
          p->score = winWeight*p->count/total_count;
          moves.push_back(pair<Move,int>(p->move,p->score));
          p = p->next;
       }
       if (moves.size()) {
          if (moves.size() >1) {
             std::sort(moves.begin(),moves.end(),cmp);
          }
          Board board;
          BoardIO::readFEN(board,first->fen);
          // output w/o move #
          BoardIO::writeFEN(board,cout,0);
          int count = 0;
          cout << " bm";
          int max_score = -1;
          for (vector< pair<Move,int> >::const_iterator mit = moves.begin();
               mit != moves.end() && count < 3;
               mit++) {
             if (count == 0) max_score = (*mit).second;
             // if multiple moves drop those with 0 score or those
             // scoring much less than max score
             if (count > 0) {
                if ((*mit).second == 0 ||
                    (*mit).second < max_score/2) {
                   break;
                }
             }
             cout << ' ';
             Notation::image(board,(*mit).first,Notation::SAN_OUT,cout);
             count++;
          }
          cout << ';' << endl;
       }
       ++it;
   }
}
