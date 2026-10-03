// Copyright 2026 by Jon Dart. All Rights Reserved.
#include "attacks.h"
#include "bitutil.h"
#include "board.h"
#include "globals.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "nnparams.h"

// Reports aggregate statistics for files in bullet ("ChessBoard") format.
// Each record is 32 bytes: occupancy (u64), 16 bytes of packed pieces, score
// (i16), result (u8, 0/1/2 = loss/draw/win), king square (u8), opponent king
// square (u8), 3 bytes padding. Positions are stored from the side to move's
// point of view (Black-to-move positions are flipped), so results are side to
// move relative, and the stored king squares are relative to each king's owner.
// The format holds no move, so there are no "non-quiet" statistics.

namespace {

constexpr size_t RECORD_SIZE = 32;
constexpr unsigned N_KING_BUCKETS = nnue::NetworkParams::KING_BUCKETS;
constexpr unsigned N_OUTPUT_BUCKETS = nnue::NetworkParams::OUTPUT_BUCKETS;

struct Stats {
    uint64_t positions = 0;
    uint64_t invalid = 0;
    std::array<uint64_t, 3> results{}; // loss, draw, win (side to move)
    std::array<uint64_t, N_KING_BUCKETS> stmKingBuckets{};
    std::array<uint64_t, N_KING_BUCKETS> oppKingBuckets{};
    std::array<uint64_t, N_OUTPUT_BUCKETS> outputBuckets{};
    uint64_t inCheck = 0;

    void merge(const Stats &o) {
        positions += o.positions;
        invalid += o.invalid;
        for (size_t i = 0; i < results.size(); i++) results[i] += o.results[i];
        for (size_t i = 0; i < stmKingBuckets.size(); i++) stmKingBuckets[i] += o.stmKingBuckets[i];
        for (size_t i = 0; i < oppKingBuckets.size(); i++) oppKingBuckets[i] += o.oppKingBuckets[i];
        for (size_t i = 0; i < outputBuckets.size(); i++) outputBuckets[i] += o.outputBuckets[i];
        inCheck += o.inCheck;
    }
};

// Hands out batches of records from a list of files to multiple threads.
class BatchReader {
  public:
    static constexpr size_t BATCH_RECORDS = 16384;

    BatchReader(char **f, int n) : files(f), nfiles(n) {}

    // Fills buf with up to BATCH_RECORDS whole records; returns the record count
    // (0 when all input is exhausted).
    size_t next(std::vector<uint8_t> &buf) {
        std::lock_guard<std::mutex> lock(mtx);
        buf.resize(BATCH_RECORDS * RECORD_SIZE);
        while (true) {
            if (!in.is_open()) {
                if (fileIndex >= nfiles) return 0;
                const char *name = files[fileIndex++];
                in.clear();
                in.open(name, std::ios::binary);
                if (!in) {
                    std::cerr << "cannot open " << name << std::endl;
                    status = 1;
                    continue;
                }
                current = name;
            }
            in.read(reinterpret_cast<char *>(buf.data()), buf.size());
            std::streamsize got = in.gcount();
            size_t recs = static_cast<size_t>(got) / RECORD_SIZE;
            if (in.eof() || in.fail()) {
                if (got % RECORD_SIZE != 0) {
                    std::cerr << current << ": trailing partial record ignored" << std::endl;
                }
                in.close();
            }
            if (recs) return recs;
        }
    }

    int getStatus() const { return status; }

  private:
    std::mutex mtx;
    char **files;
    int nfiles;
    int fileIndex = 0;
    std::ifstream in;
    std::string current;
    int status = 0;
};

uint64_t readLE64(const uint8_t *p) {
    uint64_t v = 0;
    for (int i = 7; i >= 0; --i) v = (v << 8) | p[i];
    return v;
}

double pct(uint64_t n, uint64_t total) {
    return total ? 100.0 * static_cast<double>(n) / static_cast<double>(total) : 0.0;
}

// Returns false if the record is malformed.
bool process(const uint8_t *rec, Stats &stats) {
    uint64_t occ = readLE64(rec);
    const uint8_t *pieces = rec + 8;
    unsigned res = rec[26];
    unsigned ksq = rec[27];
    unsigned oppKsq = rec[28];
    if (res > 2 || ksq > 63 || oppKsq > 63) return false;
    unsigned count = Bitboard(occ).bitCount();
    if (count < 2 || count > 32) return false;

    Board board;
    board.setSideToMove(White);
    for (Square s = 0; s < 64; s++) board.setContents(EmptyPiece, s);
    unsigned idx = 0;
    unsigned kings[2] = {0, 0};
    Bitboard bits(occ);
    Square sq;
    while ((sq = bits.firstOne()) != InvalidSquare) {
        bits.clear(sq);
        unsigned code = (pieces[idx / 2] >> (4 * (idx & 1))) & 0xf;
        ++idx;
        if ((code & 7) > 5) return false;
        if (code == 5) ++kings[0];
        if (code == 13) ++kings[1];
        board.setContents(static_cast<Piece>(code + 1), sq);
    }
    if (kings[0] != 1 || kings[1] != 1) return false;
    board.setSecondaryVars();

    ++stats.positions;
    ++stats.results[res];
    ++stats.stmKingBuckets[nnue::NetworkParams::KING_BUCKETS_MAP[ksq]];
    ++stats.oppKingBuckets[nnue::NetworkParams::KING_BUCKETS_MAP[oppKsq]];
    ++stats.outputBuckets[nnue::NetworkParams::getOutputBucket(count)];
    if (board.inCheck()) ++stats.inCheck;
    return true;
}

void report(const Stats &s) {
    const uint64_t n = s.positions;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "positions: " << n << std::endl;
    if (s.invalid) std::cout << "invalid records skipped: " << s.invalid << std::endl;
    if (!n) return;
    static const char *RESULT_NAMES[3] = {"loss", "draw", "win"};
    std::cout << "result (side to move relative):" << std::endl;
    for (unsigned i = 0; i < 3; i++) {
        std::cout << "  " << RESULT_NAMES[i] << ": " << s.results[i] << " ("
                  << pct(s.results[i], n) << "%)" << std::endl;
    }
    std::cout << "king buckets (side to move / opponent):" << std::endl;
    for (unsigned i = 0; i < N_KING_BUCKETS; i++) {
        std::cout << "  " << i << ": " << s.stmKingBuckets[i] << " ("
                  << pct(s.stmKingBuckets[i], n) << "%) / " << s.oppKingBuckets[i] << " ("
                  << pct(s.oppKingBuckets[i], n) << "%)" << std::endl;
    }
    std::cout << "output buckets:" << std::endl;
    for (unsigned i = 0; i < N_OUTPUT_BUCKETS; i++) {
        std::cout << "  " << i << ": " << s.outputBuckets[i] << " ("
                  << pct(s.outputBuckets[i], n) << "%)" << std::endl;
    }
    std::cout << "side to move in check: " << s.inCheck << " (" << pct(s.inCheck, n) << "%)"
              << std::endl;
}

void worker(BatchReader &reader, Stats &stats) {
    std::vector<uint8_t> buf;
    size_t n;
    while ((n = reader.next(buf)) != 0) {
        for (size_t i = 0; i < n; i++) {
            if (!process(buf.data() + i * RECORD_SIZE, stats)) ++stats.invalid;
        }
    }
}

} // namespace

int main(int argc, char **argv) {
    unsigned cores = 1;
    int first = 1;
    while (first < argc && argv[first][0] == '-' && argv[first][1] != '\0') {
        std::string opt = argv[first];
        if (opt == "-c" && first + 1 < argc) {
            char *end;
            long v = strtol(argv[first + 1], &end, 10);
            if (*end != '\0' || v < 1 || v > 1024) {
                std::cerr << "invalid core count: " << argv[first + 1] << std::endl;
                return 2;
            }
            cores = static_cast<unsigned>(v);
            first += 2;
        } else {
            break;
        }
    }
    if (first >= argc) {
        std::cerr << "Usage: bulletstats [-c <cores>] <file> [<file> ...]" << std::endl;
        return 2;
    }
    BitUtils::init();
    Board::init();
    Attacks::init();

    BatchReader reader(argv + first, argc - first);
    std::vector<Stats> perThread(cores);
    if (cores == 1) {
        worker(reader, perThread[0]);
    } else {
        std::vector<std::thread> threads;
        for (unsigned t = 0; t < cores; t++) {
            threads.emplace_back(worker, std::ref(reader), std::ref(perThread[t]));
        }
        for (auto &t : threads) t.join();
    }
    Stats stats;
    for (const Stats &s : perThread) stats.merge(s);
    report(stats);
    return reader.getStatus();
}
