// Copyright 1996-2004, 2012-2017 by Jon Dart.  All Rights Reserved.

// Stand-alone executable to build the binary opening book from
// a text file.

// The book file is a binary file consisting of a header followed
// by one or more "index pages" and then by one or more "data pages."
// These data structures are defined in bookdefs.h.

#include "board.h"
#include "bookdefs.h"
#include "bookwrit.h"
#include "bhash.h"
#include "chessio.h"
#include "movegen.h"
#include "notation.h"
#include "attacks.h"
#include "globals.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <stack>
#include <vector>
using namespace std;

extern "C"{
#include <ctype.h>
#include <math.h>
};

enum ResultType {White_Win, Black_Win, DrawResult, UnknownResult};
ResultType tmp_result;

// number of pages in the book.
static unsigned indexPages = 1;                    // default
// max ply depth processed for PGN games
static int maxPly = 70;
static bool verbose = false;

enum MoveEval {NO_MOVE_EVAL,
               BLUNDER_MOVE,POOR_MOVE,QUESTIONABLE_MOVE,NEUTRAL_EVAL,
               INTERESTING_MOVE,GOOD_MOVE,VERY_GOOD_MOVE};
enum PositionEval {NO_POSITION_EVAL,
                   BLACK_WINNING_ADVANTAGE,
                   BLACK_STRONG_ADVANTAGE,
                   BLACK_MODERATE_ADVANTAGE,
                   BLACK_ADVANTAGE,
                   EQUAL_POSITION,
                   WHITE_ADVANTAGE,
                   WHITE_MODERATE_ADVANTAGE,
                   WHITE_STRONG_ADVANTAGE,
                   WHITE_WINNING_ADVANTAGE};
static unordered_map<string, MoveEval> moveEvals;
static unordered_map<string, PositionEval> positionEvals;

static string output_name;

class BookEntry;

struct MoveListEntry {
    Move move;
    int index;
    unsigned rec;
    MoveEval moveEval;
    // store state so moves are undoable
    BoardState state;
    MoveListEntry(): move(NullMove),
                     index(0),
                     rec(book::NO_RECOMMEND),
                     moveEval(NO_MOVE_EVAL) {
    }
};

struct Variation {
    Board save;
    vector<MoveListEntry> moves;
    ResultType result;
    PositionEval eval;
    int move_num; // at start of variation
    Variation() {
       eval = NO_POSITION_EVAL;
       result = UnknownResult;
       move_num = 0;
    }
    Variation(const Board &b, int n) : save(b), move_num(n) {
       eval = NO_POSITION_EVAL;
       result = UnknownResult;
    }
    Move lastMove() const {
        return moves.size() == 0 ? NullMove : moves[moves.size()-1].move;
    }
};

static unsigned minFrequency = 0;

#ifdef __INTEL_COMPILER
#pragma pack(push,1)
#endif

class BookEntry
BEGIN_PACKED_STRUCT
   public:
   BookEntry( unsigned rec, PositionEval ev,
               MoveEval mev, ResultType result,
               byte mv_indx,
               BookEntry *nxt, bool first);

    BookEntry *next;
    bool first;
    int white_win_loss;  // wins - losses for White
    PositionEval eval;
    MoveEval moveEval;
    unsigned rec; // explicit weight if any
    int relativeFreq, winWeight;
    uint32_t count;
    byte move_index;
    uint16_t weight;
END_PACKED_STRUCT

#ifdef __INTEL_COMPILER
#pragma pack(pop)
#endif

BookEntry::BookEntry( unsigned r, PositionEval ev,
                       MoveEval mev,
                       ResultType result, byte mv_indx,
                       BookEntry *nxt, bool first_file)
    :next(nxt), first(first_file), white_win_loss(0),
     eval(ev), moveEval(mev), rec(r),
     relativeFreq(0),winWeight(0),
     count(1),
     move_index(mv_indx), weight(0)
{
   if (result == White_Win) {
      white_win_loss++;
   }
   else if (result == Black_Win) {
      white_win_loss--;
   }
}


static unordered_map <uint64_t, BookEntry *>* hashTable = NULL;

// Compute a recommended relative weight for a set of book
// moves from a given position
static void computeWeights(const hash_t hashCode, BookEntry *be)
{
   int total = 0;

   // compute total frequency for eligible moves
   BookEntry *p;
   for (p = be; p; p = p->next) {
      if ((p->count >= minFrequency) || p->first) {
          total += p->count;
      }
   }
   if (total == 0) return;
   int bestWinWeight = -1;
   const int black_to_move = BoardHash::sideToMove(hashCode) == Black;
   for (p = be; p; p = p->next) {
       p->relativeFreq = book::MAX_WEIGHT*(p->count)/total;
       int winloss;
       if (black_to_move) {
           winloss = (-p->white_win_loss*100)/(int)(p->count);
       }
       else {
           // white is on move
           winloss = (p->white_win_loss*100)/(int)(p->count);
       }
       p->winWeight = (winloss+100)/2;
       if (p->winWeight > bestWinWeight) bestWinWeight = p->winWeight;
#ifdef _TRACE
       cout << "index " << (int)p->move_index << " win weight=" << p->winWeight << endl;
#endif
   }
   // Now compute the weights
   int totalWeight = 0;
   for (p = be; p; p = p->next) {
      int w;
#ifdef _TRACE
      cout << "index: " << (int)p->move_index << " ";
#endif
      if (p->rec != book::NO_RECOMMEND) {
         w = p->rec*book::MAX_WEIGHT/100;
#ifdef _TRACE
         cout << "explicit weight: " << w << endl;
#endif
      }
      else if (p->moveEval == BLUNDER_MOVE ||
               p->moveEval == POOR_MOVE) {
         // don't play these moves
         w = 0;
#ifdef _TRACE
         cout << "avoid move: " << w << endl;
#endif
      }
      else if (p->moveEval == VERY_GOOD_MOVE) {
         w = book::MAX_WEIGHT;
#ifdef _TRACE
         cout << "best move: " << w << endl;
#endif
      }
      else {
#ifdef _TRACE
          cout << "count=" << p->count << " moveEval=" <<
              (int)p->moveEval << " posEval=" << (int)p->eval << endl;
#endif
          if (p->first && p->count < (unsigned)std::max<int>(2,minFrequency) &&
              (p->moveEval != NO_MOVE_EVAL || p->eval != NO_POSITION_EVAL)) {
             // This move is in the first annotated book file but
             // occurs elsewhere with very low frequency. If we have
             // information about it from an annotation, start
             // it with a neutral eval and modify that based on
             // the annotation (even if it has 0 winning results).
             // If however there is no NAG associated with the move,
             // use the win/loss frequency to set the weight as usual.
             w = book::MAX_WEIGHT/2;
#ifdef _TRACE
             cout << "using neutral eval, w=" << w << endl;
#endif
         }
         else if (bestWinWeight == 0) {
#ifdef _TRACE
             cout << "bestWinWeight = 0, so w=0" << endl;
#endif
             w = 0;
         } else {
             int winWeight = p->winWeight;
             if (winWeight <= 9*bestWinWeight/10) {
                // moves that are relatively worse than the best move
                // get bumped lower in weight
                winWeight = winWeight*winWeight/bestWinWeight;
             }
             w = (p->relativeFreq*winWeight)/50;
         }
#ifdef _TRACE
         cout << " computed weight=" << w << endl;
         int w1 = w;
#endif
         if (p->moveEval != NO_MOVE_EVAL) {
            // if there is an eval (from a NAG) for the move, use that
            // to modify the weight
            const int ev = (int)p->moveEval-(int)NEUTRAL_EVAL-1;
            // move evals are -1..2
            w = w*(100-25*ev)/100;
         }
         else if (p->eval != NO_POSITION_EVAL) {
            // if there is an eval (from a NAG) for the position, use
            // that to modify the weight. Note though position evals
            // are generally at the end of a line, and we do not want
            // to modify high-frequency moves, because the basic book
            // is incomplete and there will be branches between those
            // moves and the eval point.
            // Note: ev is range -4..4
            const int ev = (int)p->eval-(int)EQUAL_POSITION;
            const int div = 1 + p->count/10;
            if (black_to_move) {
                w = w*(100-25*ev/div)/100;
            } else {
                w = w*(100+25*ev/div)/100;
            }
         }
#ifdef _TRACE
         if (w1 != w) cout << "adjusted weight: " << w << endl;
#endif
      }
      // This weight is not normalized yet, so allow it to exceed max weight
      w = std::min<int>(w,2*book::MAX_WEIGHT);
      p->weight = w;
      totalWeight += w;
   }
   // last pass: normalize the weights
   for (p = be; p; p = p->next) {
      if (totalWeight) {
         p->weight = p->weight*book::MAX_WEIGHT/totalWeight;
      }
      else {
         // no moves have non-zero weight
         p->weight = 0;
      }
#ifdef _TRACE
      cout << "index " << (int)p->move_index << " normalized weight: " << p->weight << endl;
#endif
   }
}

// Add move to our in-memory data structure
static void
add_move(const Board & board, const MoveListEntry &m, bool is_first_file,
         const Variation &var)
{
#ifdef _TRACE
   cout << "adding move ";
   Notation::image(board,m.move,Notation::SAN_OUT,cout);
   cout << endl;
#endif
   const int move_index = m.index;
   const int recommend = m.rec;
   auto it = hashTable->find(board.hashCode());
   BookEntry *be;
   if (it == hashTable->end())
      be = NULL;
   else
      be = (*it).second;
   if (be == NULL) {
       // position not found in hashtable
       BookEntry *new_entry;
       try {
          new_entry = new BookEntry(recommend,
                                    var.eval, m.moveEval, var.result,
                                    move_index, NULL, is_first_file);
       } catch(std::bad_alloc) {
          cerr << "out of memory!" << endl;
          exit(-1);
       }
      (*hashTable)[board.hashCode()] = new_entry;
#ifdef _TRACE
      cout << "inserting position: " <<
         " h:" << (hex) << Bitboard(board.hashCode()).hivalue() <<
         Bitboard(board.hashCode()).lovalue() << (dec) <<
         " index = " << (int)move_index << " from bk = " << (int)is_first_file <<
          " pos. eval=" << (int)var.eval << " moveEval=" << (int)m.moveEval << " recommend=" << (int)recommend << endl;
#endif
   }
   else {
      // update existing entry
      BookEntry *p = be;
      int found = 0;
      while (p && !found) {
         if (p->move_index == move_index) {
            ++found; break;
         }
         p = p->next;
      }
      if (found) {
         // Move already in hash table
         p->count++;
         if (var.result == White_Win) {
            p->white_win_loss++;
         }
         else if (var.result == Black_Win) {
            p->white_win_loss--;
         }
         if (m.rec != book::NO_RECOMMEND &&
             p->rec == book::NO_RECOMMEND) {
            // set explicit weight if not set already
            p->rec = m.rec;
         }
         if (m.moveEval != NO_MOVE_EVAL && p->moveEval == NO_MOVE_EVAL) {
            // set move eval if not set already
            p->moveEval = m.moveEval;
         }
#ifdef _TRACE
         cout << "updating: " <<
            " h:" << (hex) << Bitboard(board.hashCode()).hivalue() <<
            Bitboard(board.hashCode()).lovalue() << (dec) <<
            " index = " << (int)move_index <<
            " first = " << (int)is_first_file <<
            " winloss=" << p->white_win_loss <<
            " rec=" << p->rec <<
            " pos. eval=" << (int)p->eval <<
            " moveEval=" << (int)p->moveEval <<
            " count=" << p->count << endl;
#endif
      }
      else {
         // Position is in hashtable, but not with this move.
#ifdef _TRACE
         cout << "inserting move: " <<
            " h:" << (hex) << Bitboard(board.hashCode()).hivalue() <<
            Bitboard(board.hashCode()).lovalue() << (dec) <<
            " index = " << (int)move_index << " first = " << (int)is_first_file <<
             " pos. eval =" << (int)var.eval << " move eval=" << (int)m.moveEval << " recommend=" << (int)recommend << endl;
#endif
         BookEntry *new_entry;
         try {
            new_entry = new BookEntry(recommend,
                                      var.eval, m.moveEval, var.result,
                                      move_index, be, is_first_file);
         } catch(std::bad_alloc) {
            cerr << "out of memory!" << endl;
            exit(-1);
         }
         (*hashTable)[board.hashCode()] = new_entry;
      }
   }
}


// convert a move to an index based on the order the move generator
// provides. Return -1 if move is not in generated list, hence is illegal.
static int get_move_indx(const Board &board, Move move) {
    // check (pseudo) legality:
    MoveGenerator mg(board);
    int move_indx = -1;
    Move moves[Constants::MaxMoves];
    int n = mg.generateAllMoves(moves, 1 /* repeatable */);
    for (int i = 0; i < n; i++) {
      if (MovesEqual(moves[i],move)) {
          move_indx = i;
          break;
       }
    }
    return move_indx;
}

static void processVar(const Variation &var, bool first) {
    // Game has been processed, now add moves to book (we do this
    // only after seeing the whole variation, because we want the
    // end of line eval or result, if any).
#ifdef _TRACE
    cout << "process var: board=" << var.save << " hash=" << (hex) << var.save.hashCode() << (dec) << " eval=";
    if (var.eval == NO_POSITION_EVAL) {
        cout << "none";
    } else {
        cout << (int)var.eval-(int)EQUAL_POSITION;
    }
    cout << endl;
#endif
    if (var.moves.size()) {
        Board board(var.save);
        vector<MoveListEntry>::const_iterator it = var.moves.begin();
        int ply = var.move_num*2;
        while (it != var.moves.end()) {
            const MoveListEntry &m = *it++;
            if (ply < maxPly || first) {
                add_move(board, m, first, var);
            }
            ++ply;
            board.doMove(m.move);
        }
    }
}

static int do_pgn(ifstream &infile, const string &book_name, bool firstFile)
{
   vector<ChessIO::Header> hdrs;
   long games = 0L;
   ColorType side = White;
   while (!infile.eof() && infile.good()) {
      long first;
      side = White;
      ResultType last_result = UnknownResult;
      hdrs.clear();
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
      ++games;
#ifdef _TRACE
      cout << "game " << games << endl;
#endif
      int move_num = 0;
      // process a game
      Board board;
      int var = 0;
      const int MAX_VAR = 20;
      Variation varStack[MAX_VAR];
      varStack[var++] = Variation(board,0);
      Variation &topVar = varStack[0];


      Board p1,p2;
      for (;;) {
         string num;
         ChessIO::Token tok = ChessIO::get_next_token(infile);
         if (tok.type == ChessIO::Eof)
            break;
         else if (tok.type == ChessIO::OpenVar) {
             if (var >= MAX_VAR-1) {
                 cerr << "error: variation nesting limit reached" << endl;
                 continue;
             }
             // New variation is starting. Its start position is the current
             // board, -1 halfmove
             Move lastMove = varStack[var-1].lastMove();
             board.undoMove(lastMove,varStack[var-1].moves[
                                                           varStack[var-1].moves.size()-1].state);

             Variation newVar(board,move_num); // start of variation
             varStack[var++] = newVar;
             varStack[var-1] = varStack[var-1]; // new top of stack
             side = OppositeColor(side);
         }
         else if (var && tok.type == ChessIO::CloseVar) {
             const Variation &branchPoint = varStack[var-1];
             processVar(branchPoint,firstFile);
             if (var >= 2) {
                 Variation &parent = varStack[var-2];
                 // minimax child variation evals back to parent
                 if (parent.save.sideToMove() == branchPoint.save.sideToMove()) {
                     // maximize
                     parent.eval = (PositionEval)std::max<int>(
                                                           (int)parent.eval,
                                                           (int)branchPoint.eval);
                 } else {
                     // minimize
                     parent.eval = (PositionEval)std::min<int>(
                                                           (int)parent.eval,
                                                           (int)branchPoint.eval);
                 }
             }
             // reset board to start of variation
             board = branchPoint.save;
             --var;
             // now reapply last move from the parent variation
             // (main line move)
             ASSERT(var);
             Move mainLine = varStack[var-1].moves[varStack[var-1].moves.size()\
-1].move;
             board.doMove(mainLine);
             side = board.sideToMove();
         }
         else if (tok.type == ChessIO::NAG) {
             // applies to the previous move or line
             auto it = moveEvals.find(tok.val);
             if (it != moveEvals.end()) {
                 // get last move and set its eval
                 varStack[var-1].moves[varStack[var-1].moves.size()-1].moveEval = (*it).second;
             }
             //check for position eval
             auto it2 = positionEvals.find(tok.val);
             if (it2 != positionEvals.end()) {
                 // associate it with the current variation
                 varStack[var-1].eval = (PositionEval)((*it2).second);
             }
         }
         else if (tok.type == ChessIO::Comment) {
            string comment(tok.val);
            if (comment.length() > 0 && comment[0] == '{') {
                string::iterator it = comment.begin()+1;
                // remove initial brace & leading spaces after it
                while (it != comment.end() &&
#ifdef __CYGWIN__
		       (((unsigned char)(*it))<=0x7f) &&
#else
		       isascii(*it) &&
#endif
		       isspace(*it)) it++;
                comment = string(it,comment.end());
            }
            if (comment.length() >= 8 && comment.substr(0,7) == "weight:") {
               stringstream s(tok.val.substr(8));
               int num;
               s >> num;
               if (s.good()) {
                  // get last move and set its recommendation
                  if (num >=0 && num <=100) {
                      if (varStack[var-1].moves.size()) {
                          varStack[var-1].moves[varStack[var-1].moves.size()-1].rec = num;
                      }
                      else {
                          cerr << "warning: misplaced weight comment, ignored" << endl;
                      }
                  }
                  else {
                     cerr << "Warning: invalid move weight: " << num << ", ignored" << endl;
                  }
               }
            }
            else if (comment.length()>=3) {
                if (comment.substr(0,7) == "1/2-1/2" ||
                     comment.substr(0,3) == "\xBD-\xBD") {
                    varStack[var-1].result = DrawResult;
                }
                else if (comment.substr(0,3) == "1-0") {
                    varStack[var-1].result = White_Win;
                }
                else if (comment.substr(0,3) == "0-1") {
                    varStack[var-1].result = Black_Win;
                }
            }
         }
         else if (tok.type == ChessIO::Number) {
             num = tok.val;
             stringstream s(tok.val);
             s >> move_num;
         }
         else if (tok.type == ChessIO::GameMove) {
            // parse the move
             Move move = Notation::value(board,side,Notation::InputFormat::SAN,tok.val);
            if (IsNull(move)) {
                cerr << "Illegal move: " << tok.val <<
                   " in game " << games << ", file " <<
                   book_name << endl;
                return -1;
            }
            else {
                int move_indx = get_move_indx(board,move);
                if (move_indx == -1) {
                    cerr << "Illegal move: " << tok.val <<
                        " in game " << games << ", file " <<
                        book_name << endl;
                    return -1;
                } else {
                    MoveListEntry m;
                    m.move = move;
                    m.index = move_indx;
                    m.state = board.state;
#ifdef _TRACE
                    cout << "adding to stack " << var-1 << " ";
                    MoveImage(move,cout);
                    cout << endl;
#endif
                    varStack[var-1].moves.push_back(m);
#ifdef _TRACE
                    cout << " size=" << varStack[var-1].moves.size() << endl;

#endif
                }
            }
            p2 = p1;
            p1 = board;
            board.doMove(move);
            side = OppositeColor(side);
         }
         else if (tok.type == ChessIO::Unknown) {
            cerr << "Unrecognized text: " << tok.val <<
                         " in game " << games << ", file " <<
                          book_name << endl;
         }
         else if (tok.type == ChessIO::Result) {
            if (tok.val == "1/2-1/2")
               last_result = DrawResult;
            else if (tok.val=="0-1" || tok.val=="0:1")
               last_result = Black_Win;
            else if (tok.val == "1-0" || tok.val == "1:0")
               last_result = White_Win;
            else
                last_result = UnknownResult;
            topVar.result = last_result;
            break;
         }
      }
      processVar(topVar,firstFile);
      --var;
      ASSERT(var == 0);
   }
   return 0;
}

static void usage() {
    cerr << "Usage:" << endl;
    cerr << "makebook -n <pages> -p <max play> -h <hash size>" << endl;
    cerr << "         -m <min frequency> -o <output file> <input file(s)>" << endl;
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

   hashTable =  new unordered_map< uint64_t, BookEntry*>;
   moveEvals.insert(std::pair<string,MoveEval>("$1",GOOD_MOVE));
   moveEvals.insert(std::pair<string,MoveEval>("$2",POOR_MOVE));
   moveEvals.insert(std::pair<string,MoveEval>("$3",VERY_GOOD_MOVE));
   moveEvals.insert(std::pair<string,MoveEval>("$4",BLUNDER_MOVE));
   moveEvals.insert(std::pair<string,MoveEval>("$5",INTERESTING_MOVE));
   moveEvals.insert(std::pair<string,MoveEval>("$6",QUESTIONABLE_MOVE));

   positionEvals.insert(std::pair<string,PositionEval>("$21",BLACK_WINNING_ADVANTAGE));
   positionEvals.insert(std::pair<string,PositionEval>("$19",BLACK_STRONG_ADVANTAGE));
   positionEvals.insert(std::pair<string,PositionEval>("$17",BLACK_MODERATE_ADVANTAGE));
   positionEvals.insert(std::pair<string,PositionEval>("$15",BLACK_ADVANTAGE));
   positionEvals.insert(std::pair<string,PositionEval>("$10",EQUAL_POSITION));
   positionEvals.insert(std::pair<string,PositionEval>("$11",EQUAL_POSITION));
   positionEvals.insert(std::pair<string,PositionEval>("$12",EQUAL_POSITION));
   positionEvals.insert(std::pair<string,PositionEval>("$14",WHITE_ADVANTAGE));
   positionEvals.insert(std::pair<string,PositionEval>("$16",WHITE_MODERATE_ADVANTAGE));
   positionEvals.insert(std::pair<string,PositionEval>("$18",WHITE_STRONG_ADVANTAGE));
   positionEvals.insert(std::pair<string,PositionEval>("$20",WHITE_WINNING_ADVANTAGE));

   output_name = "";
   int arg = 1;
   while (arg < argc) {
      if (*argv[arg] == '-') {
         char c = argv[arg][1];
         switch(c) {
            case 'p': /* max ply */
               ++arg;
               maxPly = atoi(argv[arg]);
               if (maxPly == 0) {
                  cerr << "Illegal max ply (-p) value" << endl;
                  exit(-1);
               }
               break;
            case 'o':                             /* output file name */
               ++arg;
               output_name = argv[arg];
               break;
            case 'n':                             /* number of pages */
               ++arg;
               indexPages = atoi(argv[arg]);
               if (indexPages == 0) {
                  cerr << "Illegal size specified for -n" << endl;
                  exit(-1);
               }
               break;
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

   if (output_name == "") {
      output_name = "book.bin";
   }
   if (arg >= argc) {
       cerr << "No book input files specified." << endl;
       usage();
       return -1;
   }

   bool first = true;
   while (arg < argc) {
      book_name = argv[arg++];
      ifstream infile;

      infile.open(book_name.c_str(), ios::in);
      if (!infile.good()) {
         cerr << "Can't open book file: " << book_name << endl;
         return -1;
      }
      if (verbose) cout << "processing " << book_name << endl;
      int result = do_pgn(infile, book_name, first);
      first = false;
      infile.close();
      if (result == -1) break;
   }

   // Iterate through the hash table, picking out moves that meet
   // the "minFrequency" test. Also at this stage we compute move
   // weights.
   if (verbose) cout << "PGN processing complete." << endl;
   auto it = hashTable->begin();
   BookWriter writer(indexPages);
   uint32_t total_moves = 0;
   unsigned long positions = 0;
   while (it != hashTable->end()) {
       BookEntry* be = (*it).second;
       // Note: (*it).first is the hash code
#ifdef _TRACE
       cout << "h:" << (hex) << (*it).first << (dec) << endl;
#endif
       computeWeights((*it).first,be);
       int added = 0;
       while (be) {
           if ((be->count >= minFrequency) || be->first) {
               ++added;
               try {
                   writer.add((*it).first,be->move_index,
                              be->weight,be->count);
               } catch(BookFullException &ex) {
                   cerr << ex.what() << endl;
                   return -1;
               }
               total_moves++;
           }
           be = be->next;
       }
       if (added) ++positions;
       ++it;
   }
   if (verbose) cout << "writing .." << endl;
   if (writer.write(output_name.c_str())) {
       cerr << "error writing book" << endl;
       return -1;
   }
   else {
       cout << positions << " positions, " << total_moves << " total moves in book." << endl;
       cout << "book capacity is about " << indexPages*book::INDEX_PAGE_SIZE << " positions." << endl;
       return 0;
   }
}
