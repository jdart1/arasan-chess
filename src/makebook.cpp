// Copyright 1996-2004, 2012-2014 by Jon Dart.  All Rights Reserved.

// Stand-alone executable to build the binary opening book from
// a text file.

// The book file is a binary file consisting of one or more "pages".
// Each page is a self-contained data structure consisting of
// three portions: a header, a hash table, and an array of book
// entries.  The page size and hash table size are configurable.

// By default each page is 65536 bytes, with a hash table of 2043
// entries.  Each page can hold around 4,000 half-moves.

// The header is defined in bookdefs.h.  It currently contains 4
// fields:
//   version number
//   number of pages
//   page size (bytes)
//   hash table size (number of entries)

// The hash table is an array of unsigned indexes into the book
// entry array.  The high-order 32 bits of the hash code for the
// position modulo the hash table size is used to look up in this
// array the first book entry for the position.

// The book entries themselves are structures defined in bookentr.h.
// Hash entries for each chain in the hash table are stored contiguously
// in the book, and the weight for the last entry in each chain has
// its hi bit set.

#include "board.h"
#include "bookentr.h"
#include "bookdefs.h"
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

static const int Buffer_Size = 4096;
// max. moves per page
static unsigned max_moves;

enum ResultType {White_Win, Black_Win, DrawResult, UnknownResult};
ResultType tmp_result;

// number of pages in the book.
static unsigned bookPages = 1;                    // default
// size of a single page
static unsigned pageSize = 65536;
// hash table size (number of entries).  Each entry is 2 bytes
// Default is 1/15th the number of entries in a page
static unsigned int hashSize;
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
static map<string, MoveEval> moveEvals;
static map<string, PositionEval> positionEvals;

static string output_name;

class BookEntry2;

struct MoveListEntry {
    Move move;
    int index;
    unsigned rec;
    MoveEval moveEval;
    // store state so moves are undoable
    BoardState state;
    MoveListEntry(): move(NullMove),
                     index(0),
                     rec(BookEntry::NO_RECOMMEND),
                     moveEval(NO_MOVE_EVAL) {
    }
};

struct Variation {
    Board save;
    vector<MoveListEntry> moves;
    ResultType result;
    PositionEval eval;
    Variation() {
       eval = NO_POSITION_EVAL;
       result = UnknownResult;
    }
    Variation(const Board &b) : save(b) {
       eval = NO_POSITION_EVAL;
       result = UnknownResult;
    }
    Move lastMove() const {
        return moves.size() == 0 ? NullMove : moves[moves.size()-1].move;
    }
};

struct book_info
{
   unsigned next_free;
   unsigned *hash_table;
   BookEntry2 **book_moves;                       // array of BookEntry2 *
};

static int minFrequency = 0;

// During opening book construction, we add some extra members to
// each book entry type.
class BookEntry2  : public BookEntry
{
   public:
   BookEntry2( hash_t hc, unsigned rec, PositionEval ev, 
               MoveEval mev, ResultType result,
               byte mv_indx,
               BookEntry2 *nxt, bool first, bool last = false);

    BookEntry2 *next;
    bool first;
    int count;  // number of times position has occurred
    int white_win_loss;  // wins - losses for White
    PositionEval eval;
    MoveEval moveEval;
    unsigned rec; // explicit weight if any
    int relativeFreq;
    unsigned int next_index;
};


BookEntry2::BookEntry2( hash_t hc, unsigned r, PositionEval ev,
                        MoveEval mev,
                        ResultType result, byte mv_indx,
                        BookEntry2 *nxt, bool bk, bool last)
:BookEntry(hc,r,mv_indx,last),next(nxt),first(bk)
{
   rec = r;
   eval = ev;
   moveEval = mev;
   count = 1;
   weight = 0;
   white_win_loss = 0;
   if (result == White_Win) {
      white_win_loss++;
   }
   else if (result == Black_Win) {
      white_win_loss--;
   }
}


static map <uint64, BookEntry2 *>* hashTable = NULL;

static book_info *book_data;

// Compute a recommended relative weight for a set of book
// moves from a given position
static void computeWeights(BookEntry2 &be) 
{

   vector<BookEntry2*> moves;
   BookEntry2 *p = &be;
   // collect all related moves into a vector and compute their
   // relative frequencies
   int total = 0;
   while (p) {
      if ((p->count >= minFrequency) || p->first) {
          total += p->count;
          moves.push_back(p);
      }
      p=p->next;
   }
   ASSERT(total);
   for (vector<BookEntry2*>::iterator it = moves.begin();
        it != moves.end();
        it++) {
        (*it)->relativeFreq = BookEntry::MAX_WEIGHT*((*it)->count)/total;
   }
   const int black_to_move = (be.hashCode() & (hash_t)1);
   // Now compute the weights
   int totalWeight = 0;
   for (vector<BookEntry2*>::iterator it = moves.begin();
        it != moves.end();
        it++) {
      int w;
      if ((*it)->rec != BookEntry::NO_RECOMMEND) {
         w = (*it)->rec*BookEntry::MAX_WEIGHT/100;
#ifdef _TRACE
         cout << "explicit weight: " << w << endl;
#endif
      }
      else if ((*it)->moveEval == BLUNDER_MOVE ||
               (*it)->moveEval == POOR_MOVE) {
         // don't play these moves
         w = 0;
#ifdef _TRACE
         cout << "avoid move: " << w << endl;
#endif
      }
      else if ((*it)->moveEval == VERY_GOOD_MOVE) {
         w = BookEntry::MAX_WEIGHT;
#ifdef _TRACE
         cout << "best move: " << w << endl;
#endif
      }
      else {
         int winloss;
         if (black_to_move) {
            winloss = (-(*it)->white_win_loss*100)/(*it)->count;
         }
         else {
            // white is on move
            winloss = ((*it)->white_win_loss*100)/(*it)->count;
         }
         int winWeight = (winloss+100)/2;

         w = ((*it)->relativeFreq*winWeight)/50;
         if ((*it)->first && (*it)->count < minFrequency) {
             // This move is in the first annotated book file but
             // occurs elsewhere with very low frequency. If we have
             // information about it from an annotation, start
             // it with a neutral eval and modify that based on
             // the annotation (even if it has 0 winning results).
             // If however there is no NAG associated with the move,
             // use the win/loss frequency to set the weight as usual.
             if ((*it)->moveEval != NO_MOVE_EVAL ||
                 (*it)->eval != NO_POSITION_EVAL) {
                 w = BookEntry::MAX_WEIGHT/2;
             }
         }
#ifdef _TRACE
         cout << "winWeight = " << winWeight << " relativeFreq=" <<
            (*it)->relativeFreq << " computed weight=" << w << endl;
         int w1 = w;
#endif 
         if ((*it)->moveEval != NO_MOVE_EVAL) {
            // if there is an eval (from a NAG) for the move, use that
            // to modify the weight
            const int ev = (int)(*it)->moveEval-(int)NEUTRAL_EVAL-1;
            // move evals are -1..2
            w = w*(100-25*ev)/100;
         }
         else if ((*it)->eval != NO_POSITION_EVAL) {
            // if there is an eval (from a NAG) for the position, use
            // that to modify the weight. Note though position evals
            // are generally at the end of a line, and we do not want
            // to modify high-frequency moves, because the basic book
            // is incomplete and there will be branches between those
            // moves and the eval point.
            // Note: ev is range -4..4
            const int ev = (int)(*it)->eval-(int)EQUAL_POSITION-1;
            const int div = 1 + (*it)->count/10;
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
      w = Util::Min(w,2*BookEntry::MAX_WEIGHT);
      (*it)->setWeight(w);
      totalWeight += w;
   }
   // last pass: normalize the weights
   for (vector<BookEntry2*>::iterator it = moves.begin();
        it != moves.end();
        it++) {
      if (totalWeight) {
         (*it)->setWeight((*it)->getWeight()*BookEntry::MAX_WEIGHT/totalWeight);
      }
      else {
         // no moves have non-zero weight
         (*it)->setWeight(0);
      }
   }
}


static void
add_move(const Board & board, const MoveListEntry &m, bool is_first_file,
         const Variation &var)
{
   const int move_index = m.index;
   const int recommend = m.rec;
   map<uint64,BookEntry2 *>::const_iterator it =
      hashTable->find(board.hashCode());
   BookEntry2 *be;
   if (it == hashTable->end())
      be = NULL;
   else
      be = (*it).second;
   if (be == NULL) {
       BookEntry2 *new_entry;
       try {
          new_entry = new BookEntry2(board.hashCode(), recommend,
                                     var.eval, m.moveEval, var.result,
                                     move_index, NULL, is_first_file, false);
       } catch(std::bad_alloc) {
          cerr << "out of memory!" << endl;
          exit(-1);
       }
      (*hashTable)[board.hashCode()] = new_entry;
#ifdef _TRACE
      cout << "inserting: " <<
         " h:" << (hex) << Bitboard(board.hashCode()).hivalue() <<
         Bitboard(board.hashCode()).lovalue() << (dec) <<
         " index = " << (int)move_index << " from bk = " << (int)is_first_file <<
         " recommend=" << (int)recommend << endl;
#endif
   }
   else {
      // update existing entry
      BookEntry2 *p = be;
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
         if (m.rec != BookEntry::NO_RECOMMEND &&
             p->rec == BookEntry::NO_RECOMMEND) {
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
            " count=" << p->count << endl;
#endif
      }
      else {
         // Position is in hashtable, but not with this move.
#ifdef _TRACE
         cout << "inserting: " <<
            " h:" << (hex) << Bitboard(board.hashCode()).hivalue() <<
            Bitboard(board.hashCode()).lovalue() << (dec) <<
            " index = " << (int)move_index << " from bk = " << (int)is_first_file <<
            " recommend=" << (int)recommend << endl;
#endif
         BookEntry2 *new_entry;
         try {
            new_entry = new BookEntry2(board.hashCode(), recommend,
                                       var.eval, m.moveEval, var.result,
                                       move_index, be, is_first_file, false);
         } catch(std::bad_alloc) {
            cerr << "out of memory!" << endl;
            exit(-1);
         }
         (*hashTable)[board.hashCode()] = new_entry;
      }
   }
}


static void
add_move(BookEntry2 *new_entry)
{
   hash_t hc = new_entry->hashCode();
   unsigned page = (unsigned)(hc % bookPages);
   // use the high order bits of the hash code as the table probe
   unsigned probe = (unsigned) (hc>>32) % hashSize;
   unsigned hit = book_data[page].hash_table[probe];
   new_entry->next_index = hit;
   if (book_data[page].next_free >= max_moves) {
      cerr << "\nError - Too many moves in book " <<
         "(page " << page << ")" << endl;
      exit(-1);
   }
   unsigned next = book_data[page].next_free;
   book_data[page].book_moves[next] = new_entry;
   book_data[page].hash_table[probe] = next;

#ifdef _TRACE
   cout << "inserting pg:" << page << " probe:" << probe <<
      " h:" << (hex) << Bitboard(new_entry->hashCode()).hivalue() <<
      Bitboard(new_entry->hashCode()).lovalue() <<
      (dec) << " i:" <<
      (int)new_entry->get_move_index() << " slot:" << next <<
      " w:" << new_entry->getWeight() << endl;
#endif
   book_data[page].next_free++;
}

static void write_page(int page,ofstream &book_file)
{
   byte *write_buffer = new byte[Buffer_Size];
   assert(write_buffer);

   // write the header
   struct BookHeader hdr;

   hdr.version = BookVersion;
   hdr.num_pages = bookPages;
   hdr.page_capacity = pageSize;
   hdr.hash_table_size = hashSize;
   char buf[Header_Size];
   buf[0] = hdr.version;
   buf[1] = hdr.num_pages;
   // fix endian-ness for multi-byte values:
   uint32 pg = swapEndian32((byte*)&hdr.page_capacity);
   memcpy(buf+2,&pg,4);
   uint32 ht = swapEndian32((byte*)&hdr.hash_table_size);
   memcpy(buf+6,&ht,4);
   book_file.write(buf,Header_Size);

   // Re-work the hash table.  Right now it holds pointers to the
   // head of each hash chain, linked together with "next" pointers.
   // We are going to rearrange the chains so they are contiguous
   // in the book.  Then the hash table will hold indexes to the start
   // of each block of hash entries - these indexes are counts of
   // how many BookEntry objects lie before the start of the desired
   // block.

#ifdef _TRACE
   cout << "write page " << page << endl;
#endif
   int running_count = 0;
   unsigned i;
   for (i = 0; i < hashSize; i++) {
      unsigned ht_entry = book_data[page].hash_table[i];
      if (ht_entry == INVALID) {
         book_file.put((char)(INVALID % 256));
         book_file.put((char)(INVALID / 256));
      }
      else {
         book_file.put((char)(running_count % 256));
         book_file.put((char)(running_count / 256));
         unsigned indx = ht_entry;
         BookEntry2 *entry = NULL;
         while (indx != INVALID) {
            entry = book_data[page].book_moves[indx];
            entry->learn_score = 0.0F;
            indx = entry->next_index;
            ++running_count;
         }
         if (entry) {
            entry->set_last();
         }
         
      }
   }

   // write the book moves

   unsigned index = 0;
   for (i = 0; i < hashSize; i++) {
      unsigned ht_entry = book_data[page].hash_table[i];
      while (ht_entry != INVALID) {
         BookEntry2 *book_entr =
            book_data[page].book_moves[ht_entry];
         BookEntry2 new_entry = *book_entr;
         // fix multi-byte values for endian-ness:
         new_entry.my_hash_code = swapEndian64((byte*)&(book_entr->my_hash_code));
         new_entry.learn_score = (float)swapEndian32((byte*)&(book_entr->learn_score));
         new_entry.weight = (uint16)swapEndian16((byte*)&(book_entr->weight));
         if (index + (unsigned)Entry_Size <= (unsigned)Buffer_Size) {
            memcpy(write_buffer + index,
               &new_entry, Entry_Size);
            index += Entry_Size;
         }
         else {
            unsigned to_go = Buffer_Size - index;
            byte *buf = (byte*)&new_entry;
            if (to_go) {
               memcpy(write_buffer + index,buf,to_go);
            }
            book_file.write((const char*)write_buffer, Buffer_Size);
            memcpy(write_buffer, buf + to_go, Entry_Size - to_go);
            index = Entry_Size - to_go;
         }
         BookEntry2 *old_entry = book_entr;
         ht_entry = book_entr->next_index;
         delete old_entry;
      }
   }
   if (index)
      book_file.write((const char*)write_buffer, index);
   std::streamoff bookSize = book_file.tellp();
   while (bookSize % pageSize != 0L) {
      book_file.put('\0');
      bookSize++;
   }
   delete[] write_buffer;
}


static void write_book()
{
   ofstream book_file(output_name.c_str(),
      ios::out | ios::trunc | ios::binary);
   long total_moves = 0L;
   for (unsigned i = 0; i < bookPages && book_file.good(); i++) {
      write_page(i,book_file);
      total_moves += book_data[i].next_free;
   }
   cout << total_moves << " total moves." << endl;
   book_file.close();
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
    cout << "process var: board=" << var.save << " hash=" << (hex) << var.save.hashCode() << (dec) << " eval=" << 
        (var.eval == NO_MOVE_EVAL ? "none" : 
         (int)(var.eval)-(int)NEUTRAL_EVAL-1) << endl;
#endif
    if (var.moves.size()) {
        Board board(var.save);
        vector<MoveListEntry>::const_iterator it = var.moves.begin();
        while (it != var.moves.end()) {
            const MoveListEntry &m = *it++;
            add_move(board, m, first, var);
            board.doMove(m.move);
        }
    }   
}

static int do_pgn(ifstream &infile, const string &book_name, bool firstFile)
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
      varStack[var++] = Variation(board);
      Variation &topVar = varStack[0];
      

      Board p1,p2;
      for (;;) {
         string num;
         ChessIO::Token tok = ChessIO::get_next_token(infile);
//         cout << "token: " << tok.val << endl;
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

             Variation newVar(board); // start of variation
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
                     parent.eval = (PositionEval)Util::Max(
                                                           (int)parent.eval,
                                                           (int)branchPoint.eval);
                 } else {
                     // minimize
                     parent.eval = (PositionEval)Util::Min(
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
         else if (tok.type == ChessIO::NAG &&
                  move_num*2 <= maxPly) {
             // applies to the previous move or line
             map<string,MoveEval>::const_iterator it =
                 moveEvals.find(tok.val);
             if (it != moveEvals.end()) {
                 // get last move and set its eval
                 varStack[var-1].moves[varStack[var-1].moves.size()-1].moveEval = (*it).second;
             }
             //check for position eval 
             map<string,PositionEval>::const_iterator it2 =
                 positionEvals.find(tok.val);
             if (it2 != positionEvals.end()) {
                 // associate it with the current variation
                 varStack[var-1].eval = (PositionEval)((*it2).second);
             }
         }
         else if (tok.type == ChessIO::Comment) {
            if (tok.val.substr(0,8) == "{weight:") {
               stringstream s(tok.val.substr(8));
               int num;
               s >> num;
               if (s.good()) {
                  // get last move and set its recommendation
                  if (num >=0 && num <=100) {
                      if (varStack[var-1].moves.size()) {
                          varStack[var-1].moves[varStack[var-1].moves.size()-1].rec = num;
#ifdef _TRACE
                          cout << "got move weight: " << num << endl;
#endif
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
            else if (tok.val.substr(0,8) == "{1/2-1/2") {
               varStack[var-1].result = DrawResult;
            }
            else if (tok.val.substr(0,4) == "{1-0") {
               varStack[var-1].result = White_Win;
            }
            else if (tok.val.substr(0,4) == "{0-1") {
               varStack[var-1].result = Black_Win;
            }
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
                return -1;
            }
            else {
                if (move_num*2 <= maxPly) {
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
            }
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

   hashTable =  new map< uint64, BookEntry2*>;
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
   hashSize = 0;
   while (arg < argc) {
      if (*argv[arg] == '-') {
         char c = argv[arg][1];
         switch(c) {
            case 'P':                             /* page size */
               ++arg;
               pageSize = atoi(argv[arg]);
               if (pageSize == 0) {
                  cerr << "Illegal page size" << endl;
                  exit(-1);
               }
               break;
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
            case 'n':                             /* number of pages (power of 2)*/
               ++arg;
               bookPages = atoi(argv[arg]);
               if (bookPages == 0) {
                  cerr << "Illegal size specified for -n" << endl;
                  exit(-1);
               }
               else if (bookPages > 255) {
                  cerr << "Error: max book pages is 255" << endl;
                  exit(-1);
               }
               break;
            case 'm':
               ++arg;
               minFrequency = atoi(argv[arg]);
               break;
            case 'h':                             /* hash size */
               ++arg;
               hashSize = atoi(argv[arg]);
               if (hashSize == 0) {
                  cerr << "Illegal hash size" << endl;
                  exit(-1);
               }
               break;
            case 'v': 
                verbose = true;
                break;
            default:
               cerr << "Illegal switch: " << c << endl;
               cerr << "Usage:" << endl;
               cerr << "makebook -n <bits> -p <page size> -h <hash size>" << endl;
               cerr << "         -m <min frequency> -o <output file> <input file>" << endl;
               exit(-1);
         }
         ++arg;
      }
      else
         break;
   }
   if (hashSize == 0)                             // no hash size specified
      hashSize = Util::Max((pageSize/Entry_Size)/15,1);
   max_moves = (pageSize - hashSize*2 - Header_Size)/Entry_Size;
   if (max_moves < 5) {
      cerr << "Insufficient page size.  Use a larger value for -p"
         << endl;
      exit(-1);
   }
   else if (max_moves*Entry_Size < pageSize/4) {
      cerr << "Warning: hash table will consume > 1/4 of page." << endl;
      cerr << "Suggest you use a larger page size or smaller hash table size." << endl;
   }

   if (output_name == "") {
      output_name = "book.bin";
   }
   // Initialize the "book_data" structures that will hold
   // the opening book info during book construction.

   book_data = new book_info[bookPages];
   unsigned i;
   for (i = 0; i < bookPages; i++) {
      book_data[i].book_moves = new BookEntry2 * [max_moves];
      book_data[i].hash_table = new unsigned[hashSize];
      book_data[i].next_free = 0;
   }

   for (unsigned j = 0; j < bookPages; j++) {
      for (i = 0; i < hashSize; i++) {
         book_data[j].hash_table[i] = INVALID;
      }
      for (i = 0; i < max_moves; i++) {
         book_data[j].book_moves[i] = NULL;
      }
   }
   if (arg >= argc) {
       cerr << "No book input files specified." << endl;
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

   map< uint64, BookEntry2 *>::const_iterator it = hashTable->begin();
   while (it != hashTable->end()) {
      BookEntry2* be = (*it).second;
      computeWeights(*be);
      while (be) {
         if ((be->count >= minFrequency) || be->first) {
            add_move(be);
         }
         be = be->next;
      }
      ++it;
   }
   write_book();
   cout << "Total book size: " <<
      pageSize*bookPages/1024 << "K. ";
   cout << "Capacity is about " <<
      max_moves*bookPages << " moves." << endl;

   return 0;
}
