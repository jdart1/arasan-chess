// Copyright 1993-1999, 2005, 2009, 2012-2014, 2017-2019, 2021-2024 by Jon Dart.
// All Rights Reserved.

#include "bookread.h"
#include "bookdefs.h"
#include "constant.h"
#include "globals.h"
#include "movegen.h"
#include "notation.h"
#include "params.h"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "notation.h"
#include <iostream> // for debugging

BookReader::BookReader() {
    // seed the random number generator
    engine.seed(getRandomSeed());
}

BookReader::~BookReader() { close(); }

int BookReader::open(const char *pathName) {
    if (book_file.is_open())
        return 0;
    book_file.open(pathName, std::ios_base::in | std::ios_base::binary);
    if (!book_file.is_open()) {
        return -1;
    } else {
        // read the header
        book_file.read((char *)&hdr, sizeof(book::BookHeader));
        if (book_file.bad()) {
            return -1;
        }
        // correct header for endian-ness
        hdr.num_index_pages = swapEndian16((uint8_t *)&hdr.num_index_pages);
        // verify book version is correct
        if (hdr.version != book::BOOK_VERSION) {
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

Move BookReader::pick(const Board &b, bool trace) {
    if (trace) {
        std::cout << globals::debugPrefix << "BookReader::pick - hash=" << (std::hex)
                  << b.hashCode() << (std::dec) << std::endl;
    }

    std::vector<book::DataEntry> rawMoves;
    std::vector<std::pair<Move, int>> results;
    if (b.repCount() > 0) {
        return NullMove;
    } else if (lookup(b, rawMoves) <= 0) {
        return NullMove;
    }
    Move move_list[Constants::MaxMoves];
    RootMoveGenerator mg(b);
    (void)mg.generateAllMoves(move_list, 1 /* repeatable */);

    // Remove infrequent moves and zero-weight moves.
    filterByFreq(rawMoves);
    //
    // Compute the best move. We use a modified version of Thompson
    // sampling. Rewards for each move are based on a random (Dirichlet
    // distribution) sample drawn from the win/loss/draw statistics.
    // This is modified by any explicit weight, a frequency adjustment,
    // and an additional random factor.
    //
    double maxReward = -1e9;
    Move bestMove = NullMove;
    for (const book::DataEntry &info : rawMoves) {
        if (trace) {
            std::cout << globals::debugPrefix;
            Notation::image(b, move_list[info.index], Notation::OutputFormat::SAN, std::cout);
        }

        std::array<double, OUTCOMES> counts;
        counts[0] = info.win;
        counts[1] = info.loss;
        counts[2] = info.draw;
        // compute a sample based on the count distribution and
        // calculate its reward
        auto reward = (sample_dirichlet(counts) * bookSelectionOptions.scoring) / 50;
        if (trace) {

            std::cout << " WLD=[" << int(info.win) << "," << int(info.loss) << "," << int(info.draw)
                      << "]";
            std::cout << " weight: " << static_cast<int>(info.weight);
            std::cout << " reward: " << reward;
        }
        if (info.weight != book::NO_RECOMMEND) {
            // boost or reduce reward according to explicit weight
            const double adjustedWeight = static_cast<double>(effectiveWeight(info.weight));
            reward *= (1.0 + 2 * (adjustedWeight - 0.5)) / book::MAX_WEIGHT;
        }
        if (trace)
            std::cout << " adjusted reward: " << reward;
        // penalize infrequent moves
        const double f = bookSelectionOptions.frequency / 100.0;
        double freqAdjust = log10(static_cast<double>(info.win + info.loss + info.draw)) * 0.2 * f;
        if (trace)
            std::cout << " frequency: " << freqAdjust;
        reward += freqAdjust;
        // add a random amount based on randomness parameter
        std::normal_distribution<double> dist(0, 0.0019 * bookSelectionOptions.random);
        double rand = dist(engine);
        if (trace)
            std::cout << " random: " << rand;
        reward += rand;
        if (trace)
            std::cout << " total: " << reward << std::endl;
        if (reward > maxReward) {
            maxReward = reward;
            bestMove = move_list[info.index];
        }
    }
    return bestMove;
}

unsigned BookReader::book_moves(const Board &b, std::vector<Move> &moves) {
    std::vector<book::DataEntry> results;
    // Don't return a book move if we have repeated this position
    // before .. make the program to search to see if the repetition
    // is desirable or not.
    int n;
    if (b.repCount() > 0) {
        return 0;
    } else if ((n = lookup(b, results)) <= 0) {
        return 0;
    } else {
        Move move_list[Constants::MaxMoves];
        RootMoveGenerator mg(b);
        (void)mg.generateAllMoves(move_list, 1 /* repeatable */);
        filterByFreq(results);
        moves.resize(results.size());
        std::sort(results.begin(), results.end());
        std::transform(results.begin(), results.end(), moves.begin(),
                       [&](const book::DataEntry &entr) -> Move { return move_list[entr.index]; });
    }
    return static_cast<unsigned>(moves.size());
}

int BookReader::lookup(const Board &board, std::vector<book::DataEntry> &results) {
    // fetch the index page
    if (!is_open())
        return -1;
    int probe = (int)(board.hashCode() % hdr.num_index_pages);
    // seek to index
    book_file.seekg(
        (std::ios::off_type)(sizeof(book::BookHeader) + probe * sizeof(book::IndexPage)),
        std::ios_base::beg);
    if (book_file.fail())
        return -1;
    book::IndexPage index;
    book_file.read((char *)&index, sizeof(book::IndexPage));
    if (book_file.fail())
        return -1;
    // correct for endianness
    index.next_free = swapEndian32((uint8_t *)&index.next_free);
    book::BookLocation loc(0, book::INVALID_INDEX);
    for (unsigned i = 0; i < index.next_free; i++) {
        // correct for endianness
        uint64_t indexHashCode = (uint64_t)(swapEndian64((uint8_t *)&index.index[i].hashCode));
        if (indexHashCode == board.hashCode()) {
            // correct for endianness
            index.index[i].page = (uint16_t)(swapEndian16((uint8_t *)&index.index[i].page));
            index.index[i].index = (uint16_t)(swapEndian16((uint8_t *)&index.index[i].index));
            loc = index.index[i];
            break;
        }
    }
    if (!loc.isValid()) {
        // no book moves found
        return 0;
    }
    book_file.seekg(sizeof(book::BookHeader) + hdr.num_index_pages * sizeof(book::IndexPage) +
                    loc.page * sizeof(book::DataPage));
    if (book_file.fail())
        return -1;
    book::DataPage data;
    book_file.read((char *)&data, sizeof(book::DataPage));
    if (book_file.fail())
        return -1;
    while (loc.index != book::NO_NEXT) {
        assert(loc.index < book::DATA_PAGE_SIZE);
        book::DataEntry &bookEntry = data.data[loc.index];
        // correct multi-uint8_t values for endianess:
        bookEntry.next = swapEndian16((uint8_t *)&bookEntry.next);
        bookEntry.win = swapEndian32((uint8_t *)&bookEntry.win);
        bookEntry.loss = swapEndian32((uint8_t *)&bookEntry.loss);
        bookEntry.draw = swapEndian32((uint8_t *)&bookEntry.draw);
        results.push_back(bookEntry);
        loc.index = bookEntry.next;
    }
    return (int)results.size();
}

double BookReader::calcReward(const std::array<double, OUTCOMES> &sample,
                              score_t contempt) const noexcept {
    // calculate reward for a single sample (note outcomes are doubles)
    double win = sample[0];
    double loss = sample[1];
    double draw = sample[2];
    double val = 1.0 * win + contemptFactor(contempt) * draw;
    double sum = win + loss + draw;
    if (sum == 0.0) {
        return 0.0;
    } else {
        return val / sum;
    }
}

double BookReader::sample_dirichlet(const std::array<double, OUTCOMES> &counts, score_t contempt) {
    std::array<double, OUTCOMES> sample;
    double sum = 0.0;
    unsigned i = 0;
    for (double a : counts) {
        double s = 0.0;
        if (a != 0.0) {
            std::gamma_distribution<double> dist(a, 1.0);
            s = dist(engine);
        }
        sample[i++] = s;
        sum += s;
    }
    std::transform(sample.begin(), sample.end(), sample.begin(),
                   [&](const double x) -> double { return x / sum; });
    return calcReward(sample, contempt);
}

void BookReader::filterByFreq(std::vector<book::DataEntry> &results) {
    double x = bookSelectionOptions.frequency - 100.0;
    double y = std::min<double>(0.0, bookSelectionOptions.frequency - 50.0);
    const double freqThreshold = pow(10.0, (x + y) / 37.0);
    unsigned minCount = 0, maxCount = 0;
    if (globals::options.search.strength < 100) {
        minCount = (uint32_t)1 << ((100 - globals::options.search.strength) / 10);
    }
    for (const book::DataEntry &info : results) {
        unsigned count = info.count();
        if (count > maxCount)
            maxCount = count;
    }
    // In reduced-strength mode, "dumb down" the opening book by
    // omitting moves with low counts. Also apply a relative
    // frequency test based on the book frequency option. Don't remove
    // moves from the "steering" book unless they have zero weight
    // ("don't play" moves).
    auto new_end =
        std::remove_if(results.begin(), results.end(), [&](const book::DataEntry &info) -> bool {
            return effectiveWeight(info.weight) == 0 ||
                   (info.weight == book::NO_RECOMMEND &&
                    (info.count() < minCount || double(info.count()) / maxCount <= freqThreshold));
        });
    if (results.end() != new_end) {
        results.erase(new_end, results.end());
    }
}

void BookReader::setVariety(unsigned variety) {
    assert(variety <= 100);
    bookSelectionOptions.frequency = 100-variety;
    bookSelectionOptions.weighting = std::min<unsigned>(100,200-2*variety);
    bookSelectionOptions.scoring = 100-variety;
    bookSelectionOptions.random = variety;
}

// compute the effective weight, taking into account the book selection options
unsigned BookReader::effectiveWeight(unsigned weight) {
    unsigned x = 2*static_cast<unsigned>(std::max<int>(0,50-bookSelectionOptions.weighting));
    assert(x<=100);
    unsigned newWeight = x + ((100-x)*weight)/100;
    return (weight == 0) ? newWeight/2 : weight;
}
