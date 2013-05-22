// Copyright 1996-2004, 2012, 2013 by Jon Dart.  All Rights Reserved.

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
// Each one is 18 bytes and contains the full hash code, the move
// index, win/loss info for the move, a weight indicating how
// often the computer should play the move, and some learning information.
// Hash entries for each chain in the hash table are stored contiguously
// in the book, and the weight byte for the last entry in each chain has
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
static int maxPly = 60; 
static bool verbose = false;

static string output_name;

class BookEntry2;

struct book_info
{
   unsigned next_free;
   unsigned *hash_table;
   BookEntry2 **book_moves;                       // array of BookEntry2 *
};

static unsigned int minFrequency = 0;

// During opening book construction, we add some extra members to
// each book entry type.
class BookEntry2  : public BookEntry
{
   public:
      BookEntry2( hash_t hc, byte rec, byte mv_indx,
         BookEntry2 *nxt, bool from_bk, bool last = false);

    BookEntry2 *next;
    bool from_bk;
    unsigned int count;                         // number of times position has occurred
    int white_win_loss;                         // wins - losses for White
    unsigned int next_index;
};


BookEntry2::BookEntry2( hash_t hc, byte rec, byte mv_indx,
BookEntry2 *nxt, bool bk, bool last)
:BookEntry(hc,rec,mv_indx,last),next(nxt),from_bk(bk)
{
   white_win_loss = 0;
   count = 1;
}


static map <uint64, BookEntry2 *>* hashTable = NULL;

static book_info *book_data;

static void
add_move(const Board & board, int move_index, int recommend, bool is_bk_file,
         ResultType last_result)
{
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
            move_index, NULL, is_bk_file, false);
       } catch(std::bad_alloc) {
          cerr << "out of memory!" << endl;
          exit(-1);
       }
      (*hashTable)[board.hashCode()] = new_entry;
#ifdef _TRACE
      cout << "inserting: " <<
         " h:" << (hex) << Bitboard(board.hashCode()).hivalue() <<
         Bitboard(board.hashCode()).lovalue() << (dec) <<
         " index = " << (int)move_index << " from bk = " << (int)is_bk_file <<
         " recommend=" << (int)recommend << endl;
#endif
   }
   else {
      BookEntry2 *p = be;
      int found = 0;
      while (p && !found) {
         if (p->move_index == move_index) {
            ++found; break;
         }
         p = p->next;
      }
      if (found) {
         p->count++; 
         if (last_result == White_Win) {
            p->white_win_loss++;
         }
         else if (last_result == Black_Win) {
            p->white_win_loss--;
         }
#ifdef _TRACE
         cout << "updating: " <<
            " h:" << (hex) << Bitboard(board.hashCode()).hivalue() <<
            Bitboard(board.hashCode()).lovalue() << (dec) <<
            " index = " << (int)move_index << " from bk = " << (int)is_bk_file <<
            " winloss=" << p->white_win_loss <<
            " count=" << p->count << endl;
#endif
      }
      else {
#ifdef _TRACE
         cout << "inserting: " <<
            " h:" << (hex) << Bitboard(board.hashCode()).hivalue() <<
            Bitboard(board.hashCode()).lovalue() << (dec) <<
            " index = " << (int)move_index << " from bk = " << (int)is_bk_file <<
            " recommend=" << (int)recommend << endl;
#endif
         BookEntry2 *new_entry;
         try {
            new_entry = new BookEntry2(board.hashCode(), recommend,
            move_index, p, is_bk_file, false);
         } catch(std::bad_alloc) {
            cerr << "out of memory!" << endl;
            exit(-1);
         }
         new_entry->next = (*hashTable)[board.hashCode()];
         (*hashTable)[board.hashCode()] = new_entry;
      }
   }
}


static void
add_move(BookEntry2 *new_entry)
{
   hash_t hc = new_entry->hash_code();
   unsigned page = (unsigned)(hc % bookPages);
   // use the high order bits of the hash code as the table probe
   unsigned probe = (unsigned) (hc>>32) % hashSize;
   unsigned hit = book_data[page].hash_table[probe];
   new_entry->next_index = hit;
   {
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
         " h:" << (hex) << Bitboard(new_entry->hash_code()).hivalue() <<
         Bitboard(new_entry->hash_code()).lovalue() <<
`         (dec) << " i:" <<
         (int)new_entry->get_move_index() << " slot:" << next <<
         " w:" << new_entry->get_recommend() << endl;
#endif
      book_data[page].next_free++;
   }
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
#ifdef _TRACE
         //         cout << "slot " << i << " writing hash entry " << running_count << endl;
#endif

         unsigned indx = ht_entry;
         BookEntry2 *entry = NULL;
         while (indx != INVALID) {
            entry = book_data[page].book_moves[indx];
            //cout  << "Hash = " << (hex) << Bitboard(entry->hash_code()).hivalue() <<
            //Bitboard(entry->hash_code()).lovalue() <<
            //" white winloss = " << entry->white_win_loss << endl;
            if (entry->hash_code() & (hash_t)1) {
                entry->set_winloss((-entry->white_win_loss*100)/(int)entry->count);
            }
            else {
               // white is on move
               entry->set_winloss((entry->white_win_loss*100)/(int)entry->count);
            }
            entry->set_frequency(entry->count);
            entry->learn_score = 0.0F;
            indx = entry->next_index;
            ++running_count;
         }
         if (entry)
            entry->set_last();
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
   long bookSize = book_file.tellp();
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

static int do_bk(ifstream &infile)
{
   unsigned line = 0;
   Board board;
   Board tmpboard;

   ResultType last_result = UnknownResult;

   string buf;
   while (!infile.eof() && infile.good()) {
      std::getline(infile,buf);
#ifdef _TRACE
      cout << buf << endl;
#endif
      line++;
      if (buf.size() == 0) continue;
      char c = buf[0];
      switch (c) {
         case ';':
         {
            // comment, ignore except for result, if any
            if (buf.find("1/2-1/2") != string::npos)
               last_result = DrawResult;
            else if (buf.find("0-1") != string::npos)
               last_result = Black_Win;
            else if (buf.find("1-0") != string::npos)
               last_result = White_Win;
            else
               last_result = UnknownResult;
            continue;
         }
         case 'm':
            tmpboard = board;
            tmp_result = last_result;
            continue;
         case 'r':
            last_result = tmp_result;
            board = tmpboard;
            continue;
         case '-':
            last_result = UnknownResult;
            board.reset();
            break;
         default:
         {
            stringstream s(buf);
            while (!s.eof()) {
               string movebuf;
               // read next space-delimited token
               s >> movebuf;
               // ignore blanks due to trailing whitespace:
               if (movebuf.find_first_not_of(' ') == string::npos) {
                   break;
               }
               // try to parse as a move 
               Move move = Notation::value(board, board.sideToMove(), movebuf.c_str());
               if (IsNull(move)) {
                  cerr << endl << "Illegal move in file, line " <<
                     line << " (" << movebuf << ")" << endl;
                  infile.close();
                  return -1;
               }
#ifdef _TRACE
               cout << movebuf << endl;
#endif
               int move_indx = get_move_indx(board,move);
               if (move_indx == -1) {
                  cerr << endl << "Illegal move in file, line " << line << " ("
                     << movebuf << ")" << endl;
                  infile.close();
                  return -1;
               }
               int recommend = BookEntry::NO_RECOMMEND;
               char c = '\0';
               if (!s.eof()) {
                  // move may be followed by a weight (0-100)
                  s >> c;
                  s.putback(c);
                  if (isdigit(c)) {
                     s >> recommend;
                  }
               }
               add_move(board, move_indx, recommend, true, last_result);
               board.doMove(move);
            }
         }
      }
   }
   return 0;
}

struct MoveListEntry {
    Move move;
    int index;
};

static int do_pgn(ifstream &infile, const string &book_name)
{
   ArasanVector<ChessIO::Header> hdrs;
   Board board;
   long games = 0L;
   ColorType side = White;
   vector<MoveListEntry> moves;
   while (!infile.eof() && infile.good()) {
      long first;
      board.reset();
      moves.clear();
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
      int ply = 0;
      // process a game
      for (;;) {
         static char buf[2048];
         static char num[20];
         ChessIO::Token tok = ChessIO::get_next_token(infile,buf,2048);
         if (tok == ChessIO::Eof)
            break;
         else if (tok == ChessIO::Number) {
            strncpy(num,buf,10);
         }
         else if (tok == ChessIO::GameMove) {
            // parse the move
            Move move = Notation::value(board,side,buf);
            if (IsNull(move)) {
                cerr << "Illegal move: " << buf << 
                   " in game " << games << ", file " <<
                   book_name << endl;
                return -1;
            }
            else {
                if (ply++ <= maxPly) {
                    int move_indx = get_move_indx(board,move);
                    if (move_indx == -1) {
                        cerr << "Illegal move: " << buf << 
                         " in game " << games << ", file " <<
                          book_name << endl;
                         return -1;
                    } else {
                        MoveListEntry m;
                        m.move = move;
                        m.index = move_indx;
                        moves.push_back(m);
                    }
                }
                board.doMove(move);
            }
            side = OppositeColor(side);
         }
         else if (tok == ChessIO::Unknown) {
 	    cerr << "Unrecognized text: " << buf << 
                         " in game " << games << ", file " <<
                          book_name << endl;
         }
         else if (tok == ChessIO::Comment) {
            // ignored for now
         }
         else if (tok == ChessIO::Result) {
            if (strstr(buf,"1/2-1/2"))
               last_result = DrawResult;
            else if (strstr(buf,"0-1") || strstr(buf,"0:1"))
               last_result = Black_Win;
            else if (strstr(buf,"1-0") || strstr(buf,"1:0"))
               last_result = White_Win;
            else
                last_result = UnknownResult;
            break;
         }
      }
      // game has been processed, now add moves to book (we do this
      // only now because we needed to see the result first)
      if (moves.size()) {
         board.reset();
         vector<MoveListEntry>::iterator it = moves.begin();
         while (it != moves.end()) {
             const MoveListEntry &m = *it++;
             add_move(board, m.index, BookEntry::NO_RECOMMEND, false, last_result);
             board.doMove(m.move);
         }
      }   
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

   while (arg < argc) {
      book_name = argv[arg++];
      ifstream infile;

      infile.open(book_name.c_str(), ios::in);
      if (!infile.good()) {
         cerr << "Can't open book file: " << book_name << endl;
         return -1;
      }
      size_t extpos = book_name.find_last_of('.');
      if (extpos == string::npos) {
         cerr << "book file " << book_name << " lacks extension - cannot determine file type" << endl;
         return -1;
      }
      string ext = book_name.substr(extpos+1);
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
      int result;
      if (ext == "pgn") {
         if (verbose) cout << "processing " << book_name << endl;
         result = do_pgn(infile, book_name);
      }
      else if (ext == "bk") {
         if (verbose) cout << "processing " << book_name << endl;
         result = do_bk(infile);
         if (result) return -1;
      }
      else {
         cerr << "unknown book file extension: " << ext << endl;
         return -1;
      }
      infile.close();
      if (result == -1) break;
   }

   // Iterate through the hash table, picking out moves that meet
   // the "minFrequency" test

   map< uint64, BookEntry2 *>::const_iterator it = hashTable->begin();
   while (it != hashTable->end()) {
      BookEntry2* be = (*it).second;
      while (be) {
         if ((be->count >= minFrequency) || be->from_bk) {
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
