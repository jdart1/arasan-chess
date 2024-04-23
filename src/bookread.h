// Copyright 1992, 1995, 2013, 2014, 2017-2019, 2021, 2024 by Jon Dart.
// All Rights Reserved.

#ifndef _BOOK_READER_H
#define _BOOK_READER_H

#include "board.h"
#include "bookdefs.h"
#include "hash.h"
#include <array>
#include <fstream>
#include <random>
#include <vector>

class BookReader {
    // provides read access to the opening book.

  public:
    BookReader();

    ~BookReader();

    // opens the book. Returns 0 if success
    int open(const char *pathName);

    // closes the book file.
    void close();

    bool is_open() const { return book_file.is_open(); }

    // Randomly pick a move for board position "b".
    Move pick(const Board &b, bool trace);

    // Return a vector of all book moves for a given position.
    // Returns number of moves found.
    unsigned book_moves(const Board &b, std::vector<Move> &results);

    // set the desired variety in move selection (0 - less variety, 100 (max) - more variety)
    void setVariety(unsigned variety);

  protected:
    static constexpr unsigned OUTCOMES = 3;

    // Return the move data structures for a given board position.
    // Return value is # of entries retrieved, -1 if error.
    int lookup(const Board &board, std::vector<book::DataEntry> &results);

    double calcReward(const std::array<double, OUTCOMES> &sample,
                      score_t contempt = 0) const noexcept;

    double sample_dirichlet(const std::array<double, OUTCOMES> &counts, score_t contempt = 0);

    void filterByFreq(std::vector<book::DataEntry> &);

    double contemptFactor(score_t contempt) const noexcept {
        return 1.0 / (1.0 + exp(-0.75 * contempt / Params::PAWN_VALUE));
    }

    unsigned effectiveWeight(unsigned weight);

    // These options are used internally and are set according to the "variety"
    // setting. They are not directly exposed to interfaces.
    struct BookSelectionOptions {
        BookSelectionOptions() :
            frequency(50), weighting(100), scoring(50), random(50) {
        }
        unsigned frequency, weighting, scoring, random;
    } bookSelectionOptions;

    std::ifstream book_file;
    book::BookHeader hdr;
    std::mt19937_64 engine;
};

#endif
