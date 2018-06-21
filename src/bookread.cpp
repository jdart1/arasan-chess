// Copyright 1993-1999, 2005, 2009, 2012-2014, 2017-2018 by Jon Dart.
// All Rights Reserved.

#include "bookread.h"
#include "constant.h"
#include "bookdefs.h"
#include "movegen.h"
#include "globals.h"
#include "debug.h"
#include "params.h"
#include <algorithm>
#include <cmath>
#include <iostream> // for debugging
#include <assert.h>
#ifdef _WIN32
  #include <windows.h>
#endif

//#define _TRACE

#ifdef _TRACE
#include "notation.h"
#endif

static constexpr unsigned NUM_SAMPLES = 50;

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

struct Stats
{
   Move move;
   std::array<double,NUM_SAMPLES> samples;
   double weight = 0.0; // computed weight
   double weightAdjust = 1.0; // adjustment factor from book PGN
#ifdef _TRACE
   unsigned w,l,d;
#endif
};

Move BookReader::pick(const Board &b, score_t contempt) {
#ifdef _TRACE
   cout << "BookReader::pick - hash=" << (hex) << b.hashCode() << (dec) << endl;
#endif
   vector < pair<Move,int> > results;
   vector <book::DataEntry> rawMoves;
   if (b.repCount() > 0) {
      return NullMove;
   }
   else if (lookup(b,rawMoves) <= 0) {
      return NullMove;
   }

   Move move_list[Constants::MaxMoves];
   RootMoveGenerator mg(b);
#ifdef _DEBUG
   const unsigned max = mg.generateAllMoves(move_list,1 /* repeatable */);
#else
   (void)mg.generateAllMoves(move_list,1 /* repeatable */);
#endif
   // compute total and max count
   uint64_t totalCount = 0;
   unsigned maxCount = 0;
   for (const book::DataEntry &info : rawMoves) {
      unsigned count = info.count();
      totalCount += count;
      if (count > maxCount) maxCount = count;
   }

   // Remove infrequent moves and zero-weight moves.
   filterByFreq(rawMoves);
   //
   // Prepare to compute weights
   //
   vector<Stats> stats;
   for (const book::DataEntry &info : rawMoves) {
      Stats stat;
      stat.move = move_list[info.index];
#ifdef _TRACE
      stat.w = info.win;
      stat.l = info.loss;
      stat.d = info.draw;
#endif

      if (info.weight != book::NO_RECOMMEND) {
         stat.weightAdjust = double(2*info.weight)/book::MAX_WEIGHT;
      }
      // compute a bunch of random samples for each move.
      for (unsigned i = 0; i < NUM_SAMPLES; i++) {
         stat.samples[i] = sample_dirichlet(info);
      }
      stats.push_back(stat);
   }

   if (stats.empty()) {
      return NullMove;
   }

   double tolerance = 0.25*(100.0-options.book.scoring)/100.0;

   // Now compute weights for each move. This is based on the move's
   // ranking relative to other moves over the whole sample set.
   for (unsigned i = 0; i < NUM_SAMPLES; i++) {
      unsigned best_index = 0, index = 0;
      double best = -1e4;
      for (auto &s : stats) {
         if (s.samples[i] > best) {
            best = s.samples[i];
            best_index = index;
         }
         ++index;
      }
      unsigned j = 0;
      for (auto &s : stats) {
         // A move gets a score of 1 when it is the best in a random
         // sample, a declining score if it is not best but in
         // tolerance, and a zero score if out of tolerance.
         if (j==best_index) {
            s.weight += 1.0;
         } else {
            double x = (best-s.samples[i])/tolerance;
            if (x <=1.0) {
               s.weight += 1.0-x;
            }
         }
         ++j;
      }
   }

   // Correct the weights based on any explicit move weighting in the
   // book, and find the sum of corrected weights.
   double weightSum = 0.0;
   for (auto & s : stats) {
#ifdef _TRACE
      Notation::image(b,s.move,Notation::OutputFormat::SAN,cout);
      array<double,OUTCOMES> outcomes = { double(s.w), double(s.l), double(s.d) };
      cout << " " << "w:" << s.w << " l:" << s.l <<
         " d:" << s.d << " score=" << calcReward(outcomes,contempt) <<
         " weight=" << s.weight << " adjust=" <<
         s.weightAdjust <<
         " adjusted=" << s.weight*s.weightAdjust << endl;
#endif
      weightSum += s.weight*s.weightAdjust;
   }

   // Randomly pick a move based on weights
   std::uniform_real_distribution<double> dist(0,weightSum);
   double rand = dist(engine);
   double sum = 0.0;
   for (auto & s : stats) {
      sum += s.weight*s.weightAdjust;
      if (rand <= sum) {
         return s.move;
      }
   }
   // Shouldn't get here
   return NullMove;
}


unsigned BookReader::book_moves(const Board &b, vector<Move> &moves) {
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
      Move move_list[Constants::MaxMoves];
      RootMoveGenerator mg(b);
      (void)mg.generateAllMoves(move_list,1 /* repeatable */);
      filterByFreq(results);
      moves.resize(results.size());
      std::sort(results.begin(),results.end());
      std::transform(results.begin(),results.end(),moves.begin(),
                     [&](const book::DataEntry &entr) -> Move {
                        return move_list[entr.index];});
   }
   return static_cast<unsigned>(moves.size());
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
       bookEntry.weight = bookEntry.weight;
       bookEntry.win = swapEndian32((byte*)&bookEntry.win);
       bookEntry.loss = swapEndian32((byte*)&bookEntry.loss);
       bookEntry.draw = swapEndian32((byte*)&bookEntry.draw);
       results.push_back(bookEntry);
       loc.index = bookEntry.next;
   }
   return (int)results.size();
}

double BookReader::calcReward(const std::array<double,OUTCOMES> &sample, score_t contempt) const noexcept
{
   // calculate reward for a single sample (note outcomes are doubles)
   double win = sample[0];
   double loss = sample[1];
   double draw = sample[2];
   double val = 1.0*win + contemptFactor(contempt)*draw;
   double sum = win + loss + draw;
   if (sum == 0.0) {
      return 0.0;
   }
   else {
      return val/sum;
   }
}

double BookReader::sample_dirichlet(const book::DataEntry &info, score_t contempt)
{
   std::array<double,OUTCOMES> sample;
   double sum = 0.0;
   unsigned i = 0;
   array<unsigned,OUTCOMES> counts = { info.win, info.loss, info.draw };
   for (unsigned a : counts) {
      std::gamma_distribution<double> dist(a,1.0);
      double s = dist(engine);
      sample[i++] = s;
      sum += s;
   }
   std::transform(sample.begin(), sample.end(), sample.begin(),[&](const double x) -> double { return x/sum;
      });
   return calcReward(sample,contempt);
}

void BookReader::filterByFreq(vector<book::DataEntry> &results)
{
   const double freqThreshold = pow(10.0,(options.book.frequency-100.0)/40.0);

   unsigned minCount = 0;
   if (options.search.strength < 100) {
      minCount = (uint32_t)1<<((100-options.search.strength)/10);
   }
   unsigned maxCount = 0;
   for (const book::DataEntry &info : results) {
      unsigned count = info.count();
      if (count > maxCount) maxCount = count;
   }
   // In reduced-strength mode, "dumb down" the opening book by
   // omitting moves with low counts. Also apply a relative
   // frequency test based on the book frequency option. Don't remove
   // moves from the "steering" book unless they have zero weight
   // ("don't play" moves).
   auto new_end = std::remove_if(results.begin(),results.end(),
                                 [&](const book::DataEntry &info) -> bool {
                                    return
                                       info.weight == 0 || (info.weight == book::NO_RECOMMEND &&
                                       (info.count() < minCount ||
                                        double(info.count())/maxCount < freqThreshold));
                                 });
   if (results.end() != new_end) {
      results.erase(new_end,results.end());
   }
}
