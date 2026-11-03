// Copyright 2026 by Jon Dart. All Rights Reserved.
// Unit test code for NNUE layers

#include "nnue/nnue.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <map>
#include <array>

#include "board.h"
#include "boardio.h"
#include "notation.h"
#include "scoring.h"
#include "search.h"
#include "nnue.h"
namespace nnue {
#include "layers/crelu.h"
#include "layers/pairwisemult.h"
#include "layers/sparselinear.h"
#include "layers/linear.h"
}

// Helper to fill arrays with random values
template<typename T>
static void fill_random(T* arr, size_t n, int minValue, int maxValue) {
    static std::mt19937 rng(42);
    std::uniform_int_distribution<int32_t> dist(minValue, maxValue);
    for (size_t i = 0; i < n; ++i) {
        arr[i] = dist(rng);
    }
}

// Report a failed test: prints a message if there were errors. Returns the number of errors, so
// it can be used in the return statement of a test function (with __func__ as the name).
static int testResult(int errs, const char *name) {
    if (errs) {
        std::cerr << "NNUE test " << name << " failed" << std::endl;
    }
    return errs;
}

// Helper to compare outputs
template <typename OutputType, size_t outputSize>
static int compare_outputs(const OutputType* output, const OutputType* expected) {
    int errs = 0;
    for (size_t i = 0; i < outputSize; ++i) {
        if (output[i] != expected[i]) {
            std::cout << "diff at " << i << ' ' << int(output[i]) << ' ' << int(expected[i]) << std::endl;
            ++errs;
            if (i>10) break;
        }
    }
    return errs;
}

template<typename InputType, typename OutputType, size_t inputSize, unsigned clamp, unsigned shift>
static int testCReLU() {
    int err = 0;
    alignas(nnue::DEFAULT_ALIGN) InputType input[inputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType output[inputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType expected[inputSize];

    fill_random<InputType>(input, inputSize, -511, 511);

    nnue::CReLU<InputType, OutputType, inputSize, clamp, shift> layer;

    layer.forward(input,output);

    // compute expected result using non-SIMD code
    layer.cReLUGeneric(input,expected);

    err = compare_outputs<OutputType,inputSize>(output,expected);
    if (err) {
        std::cerr << "CReLU test with size " << inputSize << " and parameters " << clamp << ", " << shift << " failed" << std::endl;
    }

    return testResult(err, __func__);
}

template<typename InputType, typename OutputType, size_t inputSize>
static int testPairwiseMult() {
    int err = 0;
    alignas(nnue::DEFAULT_ALIGN) InputType input[inputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType output[inputSize/2];
    alignas(nnue::DEFAULT_ALIGN) OutputType expected[inputSize/2];

    fill_random<InputType>(input, inputSize, -362, 362);

    constexpr int32_t QUANT = 255;
    constexpr int32_t QUANT_BITS = 7;

    nnue::PairwiseMult<InputType, OutputType, inputSize, QUANT, QUANT_BITS> layer;

    layer.forward(input,output);

    // compute expected result using non-SIMD code
    layer.pairwiseMultGeneric(input,expected);

    // output is 1/2 input size
    err = compare_outputs<OutputType,inputSize/2>(output,expected);
    if (err) {
        std::cerr << "PairwiseMult test failed" << std::endl;
    }

    return testResult(err, __func__);
}

template<typename InputType, typename WeightType, typename BiasType, typename OutputType, size_t inputSize,
         size_t outputSize, unsigned inputShift, unsigned outputShift>
static int testLinearLayer() {

    int err = 0;
    alignas(nnue::DEFAULT_ALIGN) InputType input[inputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType output[outputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType expected[outputSize];

    fill_random<InputType>(input, inputSize, -362, 362);

    nnue::LinearLayer<InputType, WeightType, BiasType, OutputType, inputSize, outputSize, 1 /* buckets*/,inputShift, outputShift> layer;

    //initialize layer weights and biases
    BiasType biasdata[outputSize];
    for (size_t b = 0; b < layer.numBuckets(); ++b) {
        fill_random(biasdata, outputSize, -1024, 1024);
        for (size_t i = 0; i < outputSize; ++i) {
            WeightType weightdata[inputSize];
            fill_random(weightdata, inputSize, -127, 127);
            layer.setCol(b,i,weightdata);
        }
        layer.setBiases(b,biasdata);
    }

    layer.forward(0,input,output);

    // compute expected result using non-SIMD code
    layer.dotProductGeneric(0,input,expected);

    err = compare_outputs<OutputType,outputSize>(output,expected);
    if (err) {
        std::cerr << "LinearLayer " << inputSize << 'x' << outputSize << " with byte size "
                  << sizeof(InputType) << " failed." << std::endl;
    }

    return testResult(err, __func__);
}

// Exercises the output-layer SIMD path (int32 inputs/weights, int64
// accumulation: simd::dotProductnx1_i64). Uses realistic L2-activation
// magnitudes and high all-positive weights so the accumulated product sum
// exceeds the int32 range, verifying the int64 reduction matches the scalar
// reference (an int32-only reduction would overflow here). Inputs stay within
// the regime where the per-lane int32 partial sums do not overflow.
static int testOutputLayerInt64() {
    constexpr size_t inputSize = 32;
    constexpr size_t outputSize = 1;
    constexpr int32_t L2_OUT_CLAMP = 1044480; // QA*QB*QC
    int err = 0;
    alignas(nnue::DEFAULT_ALIGN) int32_t input[inputSize];
    alignas(nnue::DEFAULT_ALIGN) int64_t output[outputSize];
    alignas(nnue::DEFAULT_ALIGN) int64_t expected[outputSize];

    // L2 activations are CReLU-clamped to [0, L2_OUT_CLAMP]; weights to [-127, 127].
    fill_random<int32_t>(input, inputSize, L2_OUT_CLAMP * 9 / 10, L2_OUT_CLAMP);

    nnue::LinearLayer<int32_t, int32_t, int32_t, int64_t, inputSize, outputSize, 1 /* buckets */,
                      0, 0, false>
        layer;

    int32_t biasdata[outputSize];
    fill_random(biasdata, outputSize, -(1 << 24), 1 << 24);
    int32_t weightdata[inputSize];
    fill_random(weightdata, inputSize, 64, 127);
    layer.setCol(0, 0, weightdata);
    layer.setBiases(0, biasdata);

    layer.forward(0, input, output);
    layer.dotProductGeneric(0, input, expected);

    // confirm the test actually exercises int64 accumulation: the reference sum
    // must exceed the signed 32-bit range
    if (expected[0] <= 0x7fffffffLL && expected[0] >= -0x7fffffffLL) {
        std::cerr << "testOutputLayerInt64: sum " << expected[0]
                  << " did not exceed int32 range" << std::endl;
        ++err;
    }

    err += compare_outputs<int64_t, outputSize>(output, expected);
    if (err) {
        std::cerr << "OutputLayer int64 SIMD path failed." << std::endl;
    }
    return testResult(err, __func__);
}

template<typename InputType, typename WeightType, typename BiasType, typename OutputType, size_t inputSize,
         size_t outputSize, unsigned inputShift, unsigned outputShift>
static int testSparseLinear() {

    int err = 0;

    alignas(nnue::DEFAULT_ALIGN) InputType input[inputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType output[outputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType expected[outputSize];

    // Note: the input to the SparseLinear layer is unsigned, but the SIMD implementation
    // currently requires that the values not exceed 127 (max value for a signed char).
    fill_random<InputType>(input, inputSize, 0, 127);

    nnue::SparseLinear<InputType, WeightType, BiasType, OutputType, inputSize, outputSize, 1 /* buckets*/, inputShift, outputShift> layer;

    // initialize layer weights and biases
    // TBD: would like to move this inside the class, but there are compile issues with including
    // <random> into linear.h
    BiasType biasdata[outputSize];
    for (size_t b = 0; b < layer.numBuckets(); ++b) {
        fill_random(biasdata, outputSize, -1024, 1024);
        for (size_t i = 0; i < outputSize; ++i) {
            WeightType weightdata[inputSize];
            fill_random(weightdata, inputSize, -127, 127);
            layer.setCol(b,i,weightdata);
        }
        layer.setBiases(b,biasdata);
    }
    // build the optimized weight layout
    layer.postProcess();

    layer.forward(0,input,output);

    // compute expected result using non-SIMD code
    layer.dotProductGeneric(0,input,expected);

    err = compare_outputs<OutputType,outputSize>(output,expected);
    if (err) {
        std::cerr << "SparseLinear " << inputSize << 'x' << outputSize << " with byte size "
                  << sizeof(InputType) << " failed." << std::endl;
    }

    return testResult(err, __func__);
}

template<typename InputType, typename WeightType, typename BiasType, typename OutputType, size_t inputSize,
         size_t outputSize, unsigned inputShift, unsigned outputShift>
static int testOptimizedLinear() {

    int err = 0;
    alignas(nnue::DEFAULT_ALIGN) InputType input[inputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType output[outputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType expected[outputSize];

    nnue::SparseLinear<InputType, WeightType, BiasType, OutputType, inputSize, outputSize, 1 /* buckets*/, inputShift, outputShift> layer(1250, 127);;

    layer.forward(0,input,output);

    // compute expected result using non-SIMD code
    nnue::LinearLayer<InputType, WeightType, BiasType, OutputType, inputSize, outputSize, 1 /* buckets*/, inputShift, outputShift> layer2;
    layer2.fillRandom(127, 1250);
    layer.dotProductGeneric(0,input,expected);

    err = compare_outputs<OutputType,outputSize>(output,expected);
    if (err) {
        std::cerr << "LinearLayer " << inputSize << 'x' << outputSize << " with byte size "
                  << sizeof(InputType) << " failed." << std::endl;
    }

    return testResult(err, __func__);
}

template <typename arrayType>
static int indexCompare(const arrayType &indices,
                        size_t count,
                        std::set<nnue::IndexType> expected,
                        const std::string &text) {
    int errs = 0;
    for (auto it = indices.begin(); it != indices.begin() + count; it++) {
        if (expected.find(*it) == expected.end()) {
            ++errs;
            std::cerr << "error: " << text << " index " << *it << " not expected" << std::endl;
        } else
            expected.erase(*it);
    }
    if (!expected.empty()) {
        ++errs;
        std::cerr << "error: not all expected " << text << " indices found, missing " << std::endl;
        for (auto i : expected) {
            std::cerr << i << ' ';
        }
        std::cerr << std::endl;
    }
    return errs;
}

static int testPSQIndices(const Board &board, ColorType color, const std::set<nnue::IndexType> &expected) {
    nnue::PSQIndexArray indices;
    auto count = nnue::Evaluator::getIndices(color, board, indices);
    std::string text(ColorImage(color));
    text += " PSQ";
    return testResult(indexCompare<nnue::PSQIndexArray>(indices,count,expected,text), __func__);
}

static int testPawnPairIndices(const Board &board, ColorType color, const std::set<nnue::IndexType> &expected) {
    nnue::PPIndexArray indices;
    const size_t count = nnue::Network::FeatureXformer::PawnPair::getActivePairs(
            color, board.kingSquare(color), board.pawn_bits, indices);
    std::string text(ColorImage(color));
    text += " PawnPair";
    return testResult(indexCompare<nnue::PPIndexArray>(indices,count,expected,text), __func__);
}

// test the calcNnzData support function used by the SparseLinear class
template <size_t size>
static int testNnz() {
    int errs = 0;
    alignas(nnue::DEFAULT_ALIGN) uint8_t input[size];
    alignas(nnue::DEFAULT_ALIGN) uint16_t output[size/4]; // groups of 4
    size_t outputCount = 0;
    // input is unsigned
    fill_random(input, size, 0, 127);
    static std::mt19937 rng(89);
    std::uniform_int_distribution<int> dist1(0,100);

    // make array sparse
    for (size_t i = 0; i < size; ++i) {
        auto x = dist1(rng);
        if (x < 75) {
            input[i] = 0;
        }
    }

    // Calculate expected number of groups with non-zero elements
    size_t expectedGroupCount = 0;
    for (size_t groupIdx = 0; groupIdx < size/4; ++groupIdx) {
        bool hasNonZero = false;
        for (size_t k = 0; k < 4; ++k) {
            if (input[groupIdx * 4 + k] != 0) {
                hasNonZero = true;
                break;
            }
        }
        if (hasNonZero) {
            expectedGroupCount++;
        }
    }

    // Ensure we have at least one group with all zeros for testing
    if (expectedGroupCount == size/4) { // very unlikely - all groups have non-zero
        for (size_t k = 0; k < 4; ++k) {
            input[k] = 0; // zero out first group
        }
        expectedGroupCount--;
    }

    simd::calcNnzData<size>(input, output, outputCount);

    // Build expected set of group indices with non-zero elements
    std::set<uint16_t> expectedGroups;
    for (size_t groupIdx = 0; groupIdx < size/4; ++groupIdx) {
        for (size_t k = 0; k < 4; ++k) {
            if (input[groupIdx * 4 + k] != 0) {
                expectedGroups.insert(groupIdx);
                break;
            }
        }
    }

    // Build actual set of output group indices
    std::set<uint16_t> actualGroups(output, output + outputCount);

    // Compare the sets
    if (expectedGroups != actualGroups) {
        std::cerr << "testNnz failed - group sets don't match" << std::endl;
        std::cerr << " expected: ";
        for (auto val : expectedGroups) std::cerr << val << ' ';
        std::cerr << std::endl;
        std::cerr << " actual: ";
        for (auto val : actualGroups) std::cerr << val << ' ';
        std::cerr << std::endl;
        ++errs;
    }
    return testResult(errs, __func__);
}

static int testThreatIndices(const Board &board, ColorType color, const std::set<nnue::IndexType> &expected) {
    nnue::ThreatIndexArray indices;
    const size_t count = nnue::Network::FeatureXformer::Threat::getActiveThreats(color, board, indices);
    std::string text(ColorImage(color));
    text += " Threat";
    return testResult(indexCompare<nnue::ThreatIndexArray>(indices,count,expected,text), __func__);
}

static int testFeatureIndices() {
    int errs = 0;

    const char *fen = "r3k2r/1bq2pp1/p3p3/8/PN1B2n1/2PB1Nbp/1P2Q3/3R1RK1 b kq -";

    Board board;
    if (!BoardIO::readFEN(board, fen)) {
        std::cerr << "error in FEN: " << fen << std::endl;
        return testResult(1, __func__);
    }
    Board start(board);

    errs += testPSQIndices(board, White,
                           {964, 962,  1089, 782,  1035, 789,  916,  850,  1297, 1168, 799, 862,
                            924, 1241, 1199, 1195, 1334, 1461, 1202, 1201, 1407, 1531, 1400});
    errs += testPSQIndices(board, Black,
                           {2940, 2938, 3065, 2742, 2995, 2733, 2860, 2794, 2473, 2344, 2727, 2790,
                            2852, 2401, 2327, 2323, 2446, 2573, 2314, 2313, 2503, 2627, 2496});
    errs += testPawnPairIndices(board, White, {84, 259, 3747, 3764, 3972, 4088, 4094});
    errs += testPawnPairIndices(board, Black, {2, 57, 497, 3096, 4386, 4450, 4456});
    errs += testThreatIndices(board, White,
                            {24, 1087, 1168, 2179, 3941, 6107, 6186, 9108,
                             10032, 10922, 15587, 17045, 18503, 19947, 19949, 22878,
                             34650, 36334, 38016, 42450, 42551, 46473, 53764, 56664,
                             56679});
    errs += testThreatIndices(board, Black,
                            {3807, 5485, 6610, 8187, 8282, 15630, 18545, 18555,
                             22924, 30138, 30451, 32805, 32825, 33921, 34000, 36168,
                             43377, 44301, 45201, 46425, 53721, 55169, 56627, 58091,
                             58093});

   return testResult(errs, __func__);
}

struct Case {
    std::string fen;
    std::vector<std::string> moves;
    Case(const std::string &f, const std::vector<std::string> &m) : fen(f) {
        std::copy(m.begin(), m.end(), std::back_inserter(moves));
    }
    Case() = default;
};

static const std::array<Case,9> incrementalCases = {
    Case("r2q1rk1/pb1nbp1p/1pp1pp2/8/2BPN2P/5N2/PPP1QPP1/2KR3R w - -",{"Ne5","fxe5"}),
    Case("r3k2r/1bq2pp1/4p3/p7/3B2n1/1NPB1Nbp/1P2Q3/3R1RK1 b kq -",{"Bxf3","Rxf3"}),
    Case("r3k2r/1bq2pp1/4p3/p7/3B2n1/1NPB1Nbp/1P2Q3/3R1RK1 b kq -",{"Bxf3","Rxf3","O-O"}),
    Case("8/2Pk3q/3N4/5P2/2K5/6n1/8/8 w - -",{"c8=Q"}),
    Case("8/2Pk3q/3N4/5P2/2K5/6n1/8/8 w - -",{"c8=Q","Ke7","Qc7+"}),
    Case("r1b1k1nr/1pqp1p1p/p1n1p3/2b3pP/3NP1P1/2N5/PPP2P2/R1BQKB1R w KQkq g6",{"hxg6"}),
    Case("4k2r/p1r2p1p/4pp2/1b1p4/8/2P1B3/PP3PPP/R3K2R w KQk -",{"O-O-O"}),
    // several plies with pawn moves and captures
    Case("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -",
         {"e4","d5","exd5","Qxd5","Nc3","Qa5","d4","e6"}),
    // king moves that cross between files a-d and e-h, with pawn changes
    Case("4k3/pp3ppp/2p5/3p4/3P4/2P5/PP3PPP/4K3 w - -",
         {"Kd2","Kd7","Kd3","Ke6","c4","dxc4+","Kxc4"})
};

// Check that threat feature deltas between the start and each later position of a case
// equal the difference between the full feature sets
static int testThreatDeltas() {
    using TF = nnue::Network::FeatureXformer::Threat;
    using Snapshot = std::array<Bitboard, N_PIECES * N_COLORS>; // layout of Occupancies
    int errs = 0;
    auto snap = [](const Board &b) {
        Snapshot s;
        const Bitboard *src = &b.pawn_bits[0];
        std::copy(src, src + s.size(), s.begin());
        return s;
    };
    auto occ = [](const Snapshot &s) -> const Occupancies & {
        return *reinterpret_cast<const Occupancies *>(s.data());
    };
    auto fullSet = [&](ColorType side, const Board &b, const Snapshot &s) {
        nnue::ThreatIndexArray a;
        const size_t n = TF::getActiveThreats(side, b.kingSquare(side), occ(s), a);
        return std::set<nnue::IndexType>(a.begin(), a.begin() + n);
    };
    int i = 0;
    for (const Case &c : incrementalCases) {
        Board board;
        if (!BoardIO::readFEN(board, c.fen)) {
            std::cerr << "error in FEN: " << c.fen << std::endl;
            return testResult(1, __func__);
        }
        const Snapshot start = snap(board);
        const Square startKing[2] = {board.kingSquare(White), board.kingSquare(Black)};
        const Board startBoard(board);
        for (const auto &moveStr : c.moves) {
            const Move m = Notation::value(board, board.sideToMove(), Notation::InputFormat::SAN,
                                           moveStr, true);
            if (IsNull(m)) {
                std::cerr << "could not parse move " << moveStr << std::endl;
                return testResult(1, __func__);
            }
            board.doMove(m);
            const Snapshot cur = snap(board);
            for (ColorType side : colors) {
                const Square ksq = board.kingSquare(side);
                // a king move across the board changes all features: caller refreshes
                if ((File(startKing[side]) >= chess::EFILE) != (File(ksq) >= chess::EFILE)) {
                    continue;
                }
                nnue::ThreatIndexArray added, removed;
                size_t addedCount, removedCount;
                TF::getThreatDeltas(side, ksq, occ(start), occ(cur), added, addedCount, removed,
                                    removedCount);
                const auto oldSet = fullSet(side, startBoard, start);
                const auto curSet = fullSet(side, board, cur);
                std::set<nnue::IndexType> expAdded, expRemoved;
                std::set_difference(curSet.begin(), curSet.end(), oldSet.begin(), oldSet.end(),
                                    std::inserter(expAdded, expAdded.begin()));
                std::set_difference(oldSet.begin(), oldSet.end(), curSet.begin(), curSet.end(),
                                    std::inserter(expRemoved, expRemoved.begin()));
                const std::set<nnue::IndexType> gotAdded(added.begin(), added.begin() + addedCount);
                const std::set<nnue::IndexType> gotRemoved(removed.begin(),
                                                           removed.begin() + removedCount);
                // sets also catch duplicates, via the size comparison
                if (gotAdded != expAdded || gotRemoved != expRemoved ||
                    gotAdded.size() != addedCount || gotRemoved.size() != removedCount ||
                    added[addedCount] != nnue::LAST_INDEX ||
                    removed[removedCount] != nnue::LAST_INDEX) {
                    ++errs;
                    std::cerr << "testThreatDeltas: error in case " << i << ", move " << moveStr
                              << ", " << ColorImage(side) << ": added " << addedCount
                              << " (expected " << expAdded.size() << "), removed " << removedCount
                              << " (expected " << expRemoved.size() << ")" << std::endl;
                }
            }
        }
        ++i;
    }
    return testResult(errs, __func__);
}

static int testIncrementalUpdate() {
    int errs = 0;

    NodeInfo *node = new Search::NodeStack;
    int i = 0;
    Board board;
    for (const Case &c : incrementalCases) {
        if (!BoardIO::readFEN(board, c.fen)) {
            std::cout << "error in FEN: " << c.fen << std::endl;
            return testResult(1, __func__);
        }
        Scoring s;
        s.clear();
        NodeInfo *target = node;
        node->ply = 0;
        node->stm = board.sideToMove();
        node->dirty_num = 0;
        node->kingPos[White] = board.kingSquare(White);
        node->kingPos[Black] = board.kingSquare(Black);
        node->accum.setEmpty();
        // make sure starting position has an evaluated accumulator
        s.evalu8NNUE(board,node);
        int ply = 0;
        for (const auto &moveStr : c.moves) {
            Move m = Notation::value(board, board.sideToMove(), Notation::InputFormat::SAN,
                                     moveStr, true);
            if (IsNull(m)) {
                std::cout << "could not parse move " << moveStr << std::endl;
                return testResult(1, __func__);
            }
            target->last_move = m;
            board.doMove(m,target);
            ++target;
            target->ply = ++ply;
        }
        score_t score1 = s.evalu8NNUE(board,target); // incremental eval
        score_t score2 = s.evalu8NNUE(board); // force full eval of target position
        if (score1 != score2) {
            std::cerr << "test NNUE incremental: error in case " << i << std::endl;
            ++errs;
        }
        ++i;
    }
    board.reset();
    Scoring s;
    s.clear();
    // test after null move
    node->ply = 0;
    node->stm = board.sideToMove();
    node->dirty_num = 0;
    node->kingPos[White] = board.kingSquare(White);
    node->kingPos[Black] = board.kingSquare(Black);
    node->accum.setEmpty();
    node->last_move = NullMove;
    node->num_legal = 1; // needed for evaluate logic
    // ensure starting node has evaluated accumulator
    s.evalu8NNUE(board,node);
    board.doNull(node);
    (node+1)->ply = 1; // needed for evaluate logic
    // do incremental eval
    score_t endScore = s.evalu8NNUE(board,node+1);
    int tmp = errs;
    // compare with full eval
    errs += endScore != s.evalu8NNUE(board);
    if (errs-tmp) std::cerr << "error in testNNUE - null move" << std::endl;
    delete [] node;
    return testResult(errs, __func__);
}


// Validate the full network pipeline against an independent scalar reference.
// Uses internally generated data, so it does not depend on a loaded
// network file.
static int testWholeNetworkSIMDvsScalar() {
    int err = 0;
    constexpr size_t IS1 = nnue::NetworkParams::L1_SIZE; // L2 input width
    constexpr size_t IS2 = nnue::NetworkParams::L2_SIZE; // output input width
    constexpr int L1c = static_cast<int>(nnue::NetworkParams::L1_OUT_CLAMP);
    constexpr int L2c = static_cast<int>(nnue::NetworkParams::L2_OUT_CLAMP);

    nnue::CReLU<int32_t, int32_t, IS1, nnue::NetworkParams::L1_OUT_CLAMP, 0> creluL1;
    nnue::CReLU<int32_t, int32_t, IS2, nnue::NetworkParams::L2_OUT_CLAMP, 0> creluL2;
    nnue::LinearLayer<int32_t, int32_t, int32_t, int32_t, IS1, IS2, 1, 0, 0, false> l2;
    nnue::LinearLayer<int32_t, int32_t, int64_t, int64_t, IS2, 1, 1, 0, 0, false> outLayer;

    // synthetic L2 weights/biases (weights ~ QC scale, biases ~ S1*QC scale)
    int32_t w2[IS2][IS1];
    int32_t b2[IS2];
    for (size_t i = 0; i < IS2; ++i) {
        fill_random(w2[i], IS1, -100, 100);
        l2.setCol(0, i, w2[i]);
    }
    fill_random(b2, IS2, -L1c, L1c);
    l2.setBiases(0, b2);

    // synthetic output weights/biases (int64 bias)
    int32_t w3[IS2];
    fill_random(w3, IS2, -100, 100);
    int64_t b3[1];
    fill_random(b3, 1, -L2c, L2c);
    outLayer.setCol(0, 0, w3);
    outLayer.setBiases(0, b3);

    // synthetic L1 output spanning the clamp range (negatives -> 0, large -> L1c)
    alignas(nnue::DEFAULT_ALIGN) int32_t l1Out[IS1];
    fill_random(l1Out, IS1, -L1c, 2 * L1c);

    // run the head pipeline
    alignas(nnue::DEFAULT_ALIGN) int32_t l1Act[IS1];
    creluL1.forward(l1Out, l1Act);
    alignas(nnue::DEFAULT_ALIGN) int32_t l2Out[IS2];
    l2.forward(0, l1Act, l2Out);
    alignas(nnue::DEFAULT_ALIGN) int32_t l2Act[IS2];
    creluL2.forward(l2Out, l2Act);
    alignas(nnue::DEFAULT_ALIGN) int64_t out[1];
    outLayer.forward(0, l2Act, out);
    const int32_t score = static_cast<int32_t>(out[0] * nnue::NetworkParams::OUTPUT_SCALE /
                                               nnue::NetworkParams::OUTPUT_DIVISOR);

    // independent scalar reference
    int32_t refL1Act[IS1];
    for (size_t i = 0; i < IS1; ++i)
        refL1Act[i] = std::clamp<int32_t>(l1Out[i], 0, L1c);
    int32_t refL2Act[IS2];
    for (size_t j = 0; j < IS2; ++j) {
        int64_t s = 0;
        for (size_t i = 0; i < IS1; ++i)
            s += static_cast<int64_t>(refL1Act[i]) * w2[j][i];
        s += b2[j];
        refL2Act[j] = static_cast<int32_t>(std::clamp<int64_t>(s, 0, L2c));
    }
    int64_t refOut = b3[0];
    for (size_t j = 0; j < IS2; ++j)
        refOut += static_cast<int64_t>(refL2Act[j]) * w3[j];
    const int32_t refScore = static_cast<int32_t>(refOut * nnue::NetworkParams::OUTPUT_SCALE /
                                                  nnue::NetworkParams::OUTPUT_DIVISOR);

    if (score != refScore) {
        ++err;
        std::cerr << "testIntHead failed: got " << score << " expected " << refScore << std::endl;
    }
    return testResult(err, __func__);
}


#ifdef SIMD
// Test the incremental accumulator update with 8-bit weights widened to 16 bits against a
// scalar calculation
template <size_t outputSize>
static int testWideningUpdate() {
    constexpr size_t rows = 200;
    constexpr size_t maxIndices = 40;
    alignas(nnue::DEFAULT_ALIGN) static int8_t weights[rows][outputSize];
    alignas(nnue::DEFAULT_ALIGN) int16_t source[outputSize];
    alignas(nnue::DEFAULT_ALIGN) int16_t target[outputSize];
    alignas(nnue::DEFAULT_ALIGN) int16_t inPlace[outputSize];
    static std::mt19937 rng(7);
    int errs = 0;
    fill_random(&weights[0][0], rows * outputSize, -127, 127);
    for (int iter = 0; iter < 50; ++iter) {
        fill_random(source, outputSize, -5000, 5000);
        unsigned added[maxIndices], removed[maxIndices];
        std::uniform_int_distribution<unsigned> countDist(0, maxIndices);
        std::uniform_int_distribution<unsigned> rowDist(0, rows - 1);
        const size_t addedCount = countDist(rng), removedCount = countDist(rng);
        for (size_t i = 0; i < addedCount; ++i) added[i] = rowDist(rng);
        for (size_t i = 0; i < removedCount; ++i) removed[i] = rowDist(rng);
        simd::update<int16_t, int8_t, rows, outputSize>(source, target, weights, added, addedCount,
                                                         removed, removedCount);
        std::memcpy(inPlace, source, sizeof(source));
        simd::update<int16_t, int8_t, rows, outputSize>(inPlace, inPlace, weights, added, addedCount,
                                                         removed, removedCount);
        for (size_t j = 0; j < outputSize; ++j) {
            int expected = source[j];
            for (size_t i = 0; i < addedCount; ++i) expected += weights[added[i]][j];
            for (size_t i = 0; i < removedCount; ++i) expected -= weights[removed[i]][j];
            if (target[j] != expected || inPlace[j] != expected) {
                ++errs;
                std::cerr << "testWideningUpdate failed (size " << outputSize << ") at " << j << ": got "
                          << target[j] << " " << inPlace[j] << " expected " << expected << std::endl;
                return testResult(errs, __func__);
            }
        }
    }
    return testResult(errs, __func__);
}

// Test updating an accumulator in one pass with three sets of rows (16-bit weights and two
// sets of 8-bit weights), as for the piece-square, pair and threat features
template <size_t outputSize>
static int testFusedUpdate() {
    constexpr size_t rows1 = 100, rows2 = 120, rows3 = 150;
    constexpr size_t maxIndices = 30;
    alignas(nnue::DEFAULT_ALIGN) static int16_t weights1[rows1][outputSize];
    alignas(nnue::DEFAULT_ALIGN) static int8_t weights2[rows2][outputSize];
    alignas(nnue::DEFAULT_ALIGN) static int8_t weights3[rows3][outputSize];
    alignas(nnue::DEFAULT_ALIGN) int16_t source[outputSize];
    alignas(nnue::DEFAULT_ALIGN) int16_t target[outputSize];
    alignas(nnue::DEFAULT_ALIGN) int16_t inPlace[outputSize];
    static std::mt19937 rng(11);
    int errs = 0;
    fill_random(&weights1[0][0], rows1 * outputSize, -3000, 3000);
    fill_random(&weights2[0][0], rows2 * outputSize, -127, 127);
    fill_random(&weights3[0][0], rows3 * outputSize, -127, 127);
    struct Indices {
        unsigned added[maxIndices], removed[maxIndices];
        size_t addedCount, removedCount;
        void fill(size_t rows) {
            std::uniform_int_distribution<unsigned> countDist(0, maxIndices);
            std::uniform_int_distribution<unsigned> rowDist(0, static_cast<unsigned>(rows - 1));
            addedCount = countDist(rng);
            removedCount = countDist(rng);
            for (size_t i = 0; i < addedCount; ++i) added[i] = rowDist(rng);
            for (size_t i = 0; i < removedCount; ++i) removed[i] = rowDist(rng);
        }
    };
    for (int iter = 0; iter < 50; ++iter) {
        fill_random(source, outputSize, -5000, 5000);
        Indices i1, i2, i3;
        i1.fill(rows1);
        i2.fill(rows2);
        i3.fill(rows3);
        // include cases with empty sets
        if (iter % 5 == 1) i2.addedCount = i2.removedCount = 0;
        if (iter % 5 == 2) i3.addedCount = i3.removedCount = 0;
        auto u1 = simd::makeRowUpdate(weights1, i1.added, i1.addedCount, i1.removed, i1.removedCount);
        auto u2 = simd::makeRowUpdate(weights2, i2.added, i2.addedCount, i2.removed, i2.removedCount);
        auto u3 = simd::makeRowUpdate(weights3, i3.added, i3.addedCount, i3.removed, i3.removedCount);
        simd::updateRows<int16_t, outputSize>(source, target, u1, u2, u3);
        std::memcpy(inPlace, source, sizeof(source));
        simd::updateRows<int16_t, outputSize>(inPlace, inPlace, u1, u2, u3);
        for (size_t j = 0; j < outputSize; ++j) {
            int expected = source[j];
            for (size_t i = 0; i < i1.addedCount; ++i) expected += weights1[i1.added[i]][j];
            for (size_t i = 0; i < i1.removedCount; ++i) expected -= weights1[i1.removed[i]][j];
            for (size_t i = 0; i < i2.addedCount; ++i) expected += weights2[i2.added[i]][j];
            for (size_t i = 0; i < i2.removedCount; ++i) expected -= weights2[i2.removed[i]][j];
            for (size_t i = 0; i < i3.addedCount; ++i) expected += weights3[i3.added[i]][j];
            for (size_t i = 0; i < i3.removedCount; ++i) expected -= weights3[i3.removed[i]][j];
            if (target[j] != static_cast<int16_t>(expected) ||
                inPlace[j] != static_cast<int16_t>(expected)) {
                ++errs;
                std::cerr << "testFusedUpdate failed (size " << outputSize << ") at " << j << ": got "
                          << target[j] << " " << inPlace[j] << " expected " << expected << std::endl;
                return testResult(errs, __func__);
            }
        }
    }
    return testResult(errs, __func__);
}
#endif


// Attacks of a piece on an empty board, from the Attacks tables
static Bitboard emptyBoardAttacks(Piece p, Square sq) {
    const Bitboard empty;
    switch (TypeOfPiece(p)) {
    case Pawn:
        // pawn_attacks[sq][c] is the squares from which a pawn of color c attacks sq,
        // so by symmetry a White pawn on sq attacks pawn_attacks[sq][Black]
        return Attacks::pawn_attacks[sq][ColorOfPiece(p) == White ? Black : White];
    case Knight:
        return Attacks::knight_attacks[sq];
    case Bishop:
        return Attacks::bishopAttacks(sq, empty);
    case Rook:
        return Attacks::rookAttacks(sq, empty);
    case Queen:
        return Attacks::bishopAttacks(sq, empty) | Attacks::rookAttacks(sq, empty);
    default:
        return Attacks::king_attacks[sq];
    }
}

// Expected rule for which (attacker, victim) piece type pairs are valid threat features
static bool threatPairAllowed(PieceType attacker, PieceType victim) {
    switch (attacker) {
    case Pawn:
        return victim == Knight || victim == Rook;
    case Knight:
    case Queen:
        return victim != King && victim != Empty;
    case Bishop:
    case Rook:
        return victim == Pawn || victim == Knight || victim == Bishop || victim == Rook;
    default:
        return false;
    }
}

static int testThreatFeature() {
    using TF = nnue::Network::FeatureXformer::Threat;
    int errs = 0;
    const Piece pieces[12] = {WhitePawn, WhiteKnight, WhiteBishop, WhiteRook, WhiteQueen, WhiteKing,
                              BlackPawn, BlackKnight, BlackBishop, BlackRook, BlackQueen, BlackKing};
    // king squares with different file mirroring and bucket
    const Square kings[4] = {4 /* e1 */, 0 /* a1 */, 60 /* e8 */, 56 /* a8 */};
    for (ColorType side : {White, Black}) {
        for (Square ksq : kings) {
            std::map<nnue::IndexType, std::array<int, 4>> seen;
            for (Piece a : pieces) {
                for (Square from = 0; from < 64; ++from) {
                    if (TypeOfPiece(a) == Pawn && (Rank<White>(from) == 1 || Rank<White>(from) == 8)) continue;
                    Bitboard targets(emptyBoardAttacks(a, from));
                    Square to;
                    while (targets.iterate(to)) {
                        for (Piece v : pieces) {
                            const nnue::IndexType idx = TF::getIndex(side, a, from, to, v, ksq);
                            const bool expectValid = threatPairAllowed(TypeOfPiece(a), TypeOfPiece(v));
                            if (idx == nnue::LAST_INDEX) {
                                // sentinel is only allowed for excluded pairs and semi-excluded
                                // (same type) pairs
                                if (expectValid && TypeOfPiece(a) != TypeOfPiece(v)) {
                                    ++errs;
                                    std::cerr << "testThreatFeature: unexpected sentinel" << std::endl;
                                }
                                continue;
                            }
                            if (!expectValid || idx >= TF::DIMENSIONS) {
                                ++errs;
                                std::cerr << "testThreatFeature: bad index " << idx << std::endl;
                                continue;
                            }
                            // different features must have different indices, for the same
                            // perspective and king square (after mirroring)
                            const std::array<int, 4> key = {static_cast<int>(a), from, to, static_cast<int>(v)};
                            if (!seen.insert({idx, key}).second) {
                                ++errs;
                                std::cerr << "testThreatFeature: duplicate index " << idx << std::endl;
                            }
                            // perspective symmetry: flipping colors and ranks swaps the perspective
                            const nnue::IndexType idx2 = TF::getIndex(side == White ? Black : White,
                                                                static_cast<Piece>(a ^ 8), from ^ 56, to ^ 56,
                                                                static_cast<Piece>(v ^ 8), ksq ^ 56);
                            if (idx2 != idx) {
                                ++errs;
                                std::cerr << "testThreatFeature: perspective mismatch " << idx << " " << idx2 << std::endl;
                            }
                        }
                    }
                }
            }
            // same-type mutual attacks: exactly one of the two directions is a feature
            for (Piece a : pieces) {
                const PieceType t = TypeOfPiece(a);
                if (t == Pawn || t == King) continue;
                for (Piece v : pieces) {
                    if (TypeOfPiece(v) != t) continue;
                    for (Square x = 0; x < 64; ++x) {
                        Bitboard targets(emptyBoardAttacks(a, x));
                        Square y;
                        while (targets.iterate(y)) {
                            const bool ab = TF::getIndex(side, a, x, y, v, ksq) != nnue::LAST_INDEX;
                            const bool ba = TF::getIndex(side, v, y, x, a, ksq) != nnue::LAST_INDEX;
                            if (ab == ba) {
                                ++errs;
                                std::cerr << "testThreatFeature: mutual attack not exclusive" << std::endl;
                            }
                        }
                    }
                }
            }
        }
    }
    return errs;
}

int testNNUE() {

    int errs = 0;

    errs += testCReLU<int32_t,int32_t,1024,255,6>();
    errs += testCReLU<int32_t,int32_t,1024,255,0>();
    errs += testCReLU<int32_t,int32_t,16, 255, 6>(); // should not use SIMD
    errs += testCReLU<int32_t,int32_t,16, 255, 0>(); // should not use SIMD
    errs += testPairwiseMult<int16_t,int8_t,1024>();
    // v8 sparse head: pairwise-mul of one perspective (2*FT_OUTPUT_SIZE/2 -> u8)
    errs += testPairwiseMult<int16_t,uint8_t,nnue::NetworkParams::FT_OUTPUT_SIZE>();
    errs += testLinearLayer<int8_t, int8_t, int32_t, int32_t,16,1024,2,2>();
    errs += testLinearLayer<int32_t,int32_t,int32_t,int32_t,16,32,0,6>();
    errs += testLinearLayer<int32_t,int32_t,int32_t,int32_t,32,32,6,0>();
    errs += testLinearLayer<int32_t,int32_t,int32_t,int32_t,32,1,0,0>();
    errs += testOutputLayerInt64();
    int tmp = errs;
    errs += testNnz<1024>();
    if (errs - tmp > 0) {
        std::cerr << "testNnz failed: skipping SparseLinear test" << std::endl;
    }
    else {
        errs += testSparseLinear<uint8_t,int8_t,int32_t,int32_t,1024,16,7,0>();
        // v8 sparse L1: pairwise-mul u8 input (FT_OUTPUT_SIZE) -> L1_SIZE, no shift
        errs += testSparseLinear<uint8_t,int8_t,int32_t,int32_t,
                                 nnue::NetworkParams::FT_OUTPUT_SIZE,
                                 nnue::NetworkParams::L1_SIZE,0,0>();
    }

#ifdef SIMD
    errs += testWideningUpdate<nnue::NetworkParams::FT_OUTPUT_SIZE>();
    errs += testFusedUpdate<nnue::NetworkParams::FT_OUTPUT_SIZE>();
    errs += testWideningUpdate<256>();
#endif

    tmp = errs;
    errs += testFeatureIndices();
    errs += testThreatFeature();

    errs += testThreatDeltas();

    if (errs - tmp > 0) {
        std::cerr << "feature tests failed, skipping whole network test" << std::endl;
        return errs;
    }

    errs += testWholeNetworkSIMDvsScalar();
    errs += testIncrementalUpdate();

    return errs;
}
