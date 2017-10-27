// Copyright 1993-1999, 2005, 2009, 2012-2014, 2017 by Jon Dart.
// All Rights Reserved.

#include "bookread.h"
#include "constant.h"
#include "bookdefs.h"
#include "movegen.h"
#include "globals.h"
#include "debug.h"
extern "C" {
#include <string.h>
#include <math.h>
};
#include <algorithm>
#include <iostream> // for debugging
#include <assert.h>
#ifdef _WIN32
  #include <windows.h>
#endif

BookReader::BookReader()
{                 
   // seed the random number generator
   engine.seed(getRandomSeed());
}

BookReader::~BookReader()
{
    close();
}

int BookReader::open(const char *pathName) {
    if (book_file.is_open()) return 0;
    book_file.open(pathName, ios_base::in | ios_base::binary);
    if (!book_file.is_open()) {
        cerr <<"failed to open " << pathName << endl;
        return -1;
    } else {
        // read the header
        book_file.read((char*)&hdr,sizeof(book::BookHeader));
        if (book_file.bad()) {
            cerr <<"failed to read header" << endl;
            return -1;
        }
        // correct header for endian-ness
        hdr.num_index_pages = swapEndian16((byte*)&hdr.num_index_pages);
        // verify book version is correct
        if (hdr.version != book::BOOK_VERSION) {
            cerr << "expected book version " << book::BOOK_VERSION << ", got " << (unsigned)hdr.version << endl;
			close();
            return -1;
        } else {
            return 0;
        }
    }
}

void BookReader::close() {
    if (book_file.is_open()) {
       book_file.close();
    }
}

// Determine the weighting a book move will receive. Moves with higher weights
// will be played more often.
//
static int getWeight(const book::DataEntry &data) {
   // If strength reduction is enabled, "dumb down" the opening book
   // by pruning away infrequent moves.
   if (options.search.strength < 100 && data.count <
       (uint32_t)1<<((100-options.search.strength)/10)) {
       return 0;
   }
   return data.weight;
}

int BookReader::filterAndNormalize(const Board &board,
                                   vector<book::DataEntry> &rawMoves,
                                   vector< pair<Move,int> > &moves) {
   //
   // Build a list of candidate moves.
   //
   vector< pair<book::DataEntry,int> > candidates;
   for (unsigned i = 0; i < rawMoves.size(); i++) {
       int score = getWeight(rawMoves[i]);
       if (score > 0) {
           candidates.push_back(pair<book::DataEntry,int>(
                                                          rawMoves[i],score));
       }
   }
#ifdef _TRACE
   cout << "candidate_count = " << candidates.size() << endl;
#endif
   if (candidates.size() == 0) return 0;

   // Sort by descending weights
   for (unsigned i=1; i<candidates.size(); i++) {
      const int key = candidates[i].second;
      pair<book::DataEntry,int> tmp = candidates[i];
      int j = i-1;
      for (; j >= 0 && candidates[j].second < key; j--) {
         candidates[j+1] = candidates[j];
      }
      candidates[j+1] = tmp;
   }

   const int maxWeight = candidates[0].second;
   int total_weight = 0;
   for (unsigned i=0; i<candidates.size(); i++) {
      int weight = candidates[i].second;
      // apply selectivity value. If selectivity > 50, decrease
      // weights of moves besides the best move(s). If selectivity <
      // 50, increase weights of moves besides the best move(s).
#ifdef _TRACE
      cout << " orig weight=" << candidates[i].second;
#endif      
      if (maxWeight != weight) {
         if (options.book.selectivity > 50) {
            weight = int(weight*(1.0 - 2.0*double(options.book.selectivity-50)/100));
         } else if (options.book.selectivity < 50) {
            weight = std::min<int>(maxWeight,int(weight*(1.0 + double(50-options.book.selectivity)/12.0)));
         }
         // based on selectivity value, drop very low scoring moves
         if (weight < options.book.selectivity*maxWeight/1000) {
             weight = 0;
         }
      }
      candidates[i].second = weight;
      total_weight += candidates[i].second;
#ifdef _TRACE
      cout << " adjusted weight=" << candidates[i].second << endl;
#endif      
   }

   Move move_list[Constants::MaxMoves];
   MoveGenerator mg(board);
   const unsigned max = (unsigned)mg.generateAllMoves(move_list,1 /* repeatable */);

   if (total_weight) {
      // Convert move indexes to moves and normalize weight values.
      // Remove 0-weighted moves.
      for (unsigned i=0; i<candidates.size(); i++) {
         const unsigned index = (unsigned)candidates[i].first.index;
         ASSERT(index < max);
         const int normalizedWeight = (book::MAX_WEIGHT*candidates[i].second)/total_weight;
         if (normalizedWeight && index < max) {
            moves.push_back( pair<Move,int> (
                                move_list[index],
                                normalizedWeight));
         }
      }
   }
   return (int)moves.size();
}

Move BookReader::pick(const Board &b) {
#ifdef _TRACE
   cout << "BookReader::pick - hash=" << (hex) << b.hashCode() << (dec) << endl;
#endif
   vector < pair<Move,int> > results;
   if (book_moves(b, results) <= 0)
      return NullMove;
   else
     return pickRandom(b,results);
}

Move BookReader::pickRandom(const Board &b, 
                            const vector< pair<Move,int> > &moves)
{
   int total_weight = 0;
   const int n = (int)moves.size();
   for (int i = 0; i < n; i++) total_weight += moves[i].second;
   // If total_weight is 0, no moves have non-zero weights.
   if (total_weight == 0) return NullMove;
   std::uniform_int_distribution<unsigned> dist(0,total_weight);
   unsigned nRand = dist(engine);
   unsigned weight = 0;
   // Randomly pick from the available moves.  Prefer moves
   // with high weights.
   for (int i=0; i < n; i++) 
   {
      int w = moves[i].second;
      weight += w;
      if (nRand <= weight)
      {
#ifdef _TRACE
         cout << "selecting ";
         MoveImage(moves[i].first,cout);
         cout << endl;
#endif
         return moves[i].first;
      }
   }
   // should never get here
   assert(0);
   return NullMove;
}

int BookReader::book_moves(const Board &b, vector< pair<Move,int> > &moves) {
   vector <book::DataEntry> results;
   // Don't return a book move if we have repeated this position
   // before .. make the program to search to see if the repetition
   // is desirable or not.
   int n;
   if (b.repCount() > 0) {
      return 0;
   }
   else if ((n=lookup(b,results)) <= 0) {
      return 0;
   }
   else {
       return filterAndNormalize(b,results,moves);
   }
}


int BookReader::lookup(const Board &board, vector<book::DataEntry> &results) {
   // fetch the index page
   if (!is_open()) return -1;
   int probe = (int)(board.hashCode() % hdr.num_index_pages);
   // seek to index
   book_file.seekg((std::ios::off_type)(sizeof(book::BookHeader)+probe*sizeof(book::IndexPage)), std::ios_base::beg);
   if (book_file.fail()) return -1;
   book::IndexPage index;
   book_file.read((char*)&index,sizeof(book::IndexPage));
   if (book_file.fail()) return -1;
   // correct for endianness
   index.next_free = swapEndian32((byte*)&index.next_free);
   book::BookLocation loc(0,book::INVALID_INDEX);
   for (unsigned i = 0; i < index.next_free; i++) {
      // correct for endianness
      uint64_t indexHashCode = (uint64_t)(swapEndian64((byte*)&index.index[i].hashCode));
      if (indexHashCode == board.hashCode()) {
         // correct for endianness
         index.index[i].page = (uint16_t)(swapEndian16((byte*)&index.index[i].page));
         index.index[i].index = (uint16_t)(swapEndian16((byte*)&index.index[i].index));
         loc = index.index[i];
         break;
      }
   }
   if (!loc.isValid()) {
       // no book moves found
       return 0;
   }
   book_file.seekg(sizeof(book::BookHeader)+
                   hdr.num_index_pages*sizeof(book::IndexPage)+
                   loc.page*sizeof(book::DataPage));
   if (book_file.fail()) return -1;
   book::DataPage data;
   book_file.read((char*)&data,sizeof(book::DataPage));
   if (book_file.fail()) return -1;
   while(loc.index != book::NO_NEXT) {
       ASSERT(loc.index < book::DATA_PAGE_SIZE);
       book::DataEntry &bookEntry = data.data[loc.index];
       bookEntry.next = swapEndian16((byte*)&bookEntry.next);
       bookEntry.weight = swapEndian16((byte*)&bookEntry.weight);
       bookEntry.count = swapEndian32((byte*)&bookEntry.count);
       results.push_back(bookEntry);
       loc.index = bookEntry.next;
   }
   return (int)results.size();
}

