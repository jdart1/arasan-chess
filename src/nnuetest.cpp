// Copyright 2025 by Jon Dart. All Rights Reserved.
// Unit test code for NNUE layers

#include "nnue/nnue.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <random>
#include <set>

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
#include "layers/sqrcrelu.h"
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

    return err;
}

template<typename InputType, typename OutputType, size_t inputSize, unsigned clampMax, unsigned shift>
static int testSqrCReLU() {
    int err = 0;
    alignas(nnue::DEFAULT_ALIGN) InputType input[inputSize];
    alignas(nnue::DEFAULT_ALIGN) OutputType output[inputSize] = {0};
    alignas(nnue::DEFAULT_ALIGN) OutputType expected[inputSize] = {0};

    fill_random<InputType>(input, inputSize, -static_cast<int>(clampMax), clampMax);

    nnue::SqrCReLU<InputType, OutputType, inputSize, clampMax, shift> layer;

    layer.forward(input, output);

    // compute expected result using non-SIMD code
    layer.sqrCReLUGeneric(input, expected);

    err = compare_outputs<OutputType, inputSize>(output, expected);
    if (err) {
        std::cerr << "SqrCReLU test with size " << inputSize << " and parameters " 
                  << clampMax << ", " << shift << " failed" << std::endl;
    }

    return err;
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

    return err;
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

    return err;
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

    return err;
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

    return err;
}

static int testIndices(const Board &board, std::set<nnue::IndexType> &w_expected,
                       std::set<nnue::IndexType> &b_expected) {
    nnue::IndexArray wIndices, bIndices;
    auto wCount = nnue::Evaluator::getIndices(White, board, wIndices);
    auto bCount = nnue::Evaluator::getIndices(Black, board, bIndices);

    int errs = 0;
    for (auto it = wIndices.begin(); it != wIndices.begin() + wCount; it++) {
        if (w_expected.find(*it) == w_expected.end()) {
            ++errs;
            std::cerr << "error: white index " << *it << " not expected" << std::endl;
        } else
            w_expected.erase(*it);
    }
    for (auto it = bIndices.begin(); it != bIndices.begin() + bCount; it++) {
        if (b_expected.find(*it) == b_expected.end()) {
            ++errs;
            std::cerr << "error: black index " << *it << " not expected" << std::endl;
        } else
            b_expected.erase(*it);
    }
    if (!w_expected.empty()) {
        ++errs;
        std::cerr << "error: not all expected White indices found, missing " << std::endl;
        for (auto i : w_expected) {
            std::cerr << i << ' ';
        }
        std::cerr << std::endl;
    }
    if (!b_expected.empty()) {
        ++errs;
        std::cerr << "error: not all expected Black indices found, missing " << std::endl;
        for (auto i : b_expected) {
            std::cerr << i << ' ';
        }
        std::cerr << std::endl;
    }
    return errs;
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

    simd::calcNnzData(input, size, output, outputCount);

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
    return errs;
}

static int testWholeNetwork() {
    int errs = 0;

    const char *fen = "r3k2r/1bq2pp1/p3p3/8/PN1B2n1/2PB1Nbp/1P2Q3/3R1RK1 b kq -";

    Board board;
    if (!BoardIO::readFEN(board, fen)) {
        std::cerr << "error in FEN: " << fen << std::endl;
        return 1;
    }
    Board start(board);

    std::set<nnue::IndexType> w_expected{964,  962,  1089, 782,  1035, 789,  916,  850,
                                         1297, 1168, 799,  862,  924,  1241, 1199, 1195,
                                         1334, 1461, 1202, 1201, 1407, 1531, 1400};
    std::set<nnue::IndexType> b_expected{2940, 2938, 3065, 2742, 2995, 2733, 2860, 2794,
                                         2473, 2344, 2727, 2790, 2852, 2401, 2327, 2323,
                                         2446, 2573, 2314, 2313, 2503, 2627, 2496};

    errs += testIndices(board, w_expected, b_expected);

    if (errs) return errs;

    struct Case {
        std::string fen;
        std::vector<std::string> moves;
        Case(const std::string &f, const std::vector<std::string> &m) : fen(f) {
           std::copy(m.begin(), m.end(), std::back_inserter(moves));
        }
        Case() = default;
    };

    std::array<Case,7> cases = {
        Case("r2q1rk1/pb1nbp1p/1pp1pp2/8/2BPN2P/5N2/PPP1QPP1/2KR3R w - -",{"Ne5","fxe5"}),
        Case("r3k2r/1bq2pp1/4p3/p7/3B2n1/1NPB1Nbp/1P2Q3/3R1RK1 b kq -",{"Bxf3","Rxf3"}),
        Case("r3k2r/1bq2pp1/4p3/p7/3B2n1/1NPB1Nbp/1P2Q3/3R1RK1 b kq -",{"Bxf3","Rxf3","O-O"}),
        Case("8/2Pk3q/3N4/5P2/2K5/6n1/8/8 w - -",{"c8=Q"}),
        Case("8/2Pk3q/3N4/5P2/2K5/6n1/8/8 w - -",{"c8=Q","Ke7","Qc7+"}),
        Case("r1b1k1nr/1pqp1p1p/p1n1p3/2b3pP/3NP1P1/2N5/PPP2P2/R1BQKB1R w KQkq g6",{"hxg6"}),
        Case("4k2r/p1r2p1p/4pp2/1b1p4/8/2P1B3/PP3PPP/R3K2R w KQk -",{"O-O-O"})
    };

    NodeInfo *node = new Search::NodeStack;
    int i = 0;
    for (Case c : cases) {
        if (!BoardIO::readFEN(board, c.fen)) {
            std::cout << "error in FEN: " << fen << std::endl;
            return 1;
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
                return 1;
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
    board = start;
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
    return errs;
}

int testNNUE() {

    int errs = 0;

    errs += testCReLU<int32_t,int32_t,1024,255,6>();
    errs += testCReLU<int32_t,int32_t,1024,255,0>();
    errs += testCReLU<int32_t,int32_t,16, 255, 6>(); // should not use SIMD
    errs += testCReLU<int32_t,int32_t,16, 255, 0>(); // should not use SIMD
    errs += testSqrCReLU<int32_t,int32_t,1024,255,6>();
    errs += testSqrCReLU<int32_t,int32_t,32,64,0>();
    errs += testSqrCReLU<int32_t,int32_t,16,64,0>(); // should not use SIMD
    errs += testPairwiseMult<int16_t,int8_t,1024>();
    errs += testLinearLayer<int8_t, int8_t, int32_t, int32_t,16,1024,2,2>();
    errs += testLinearLayer<int32_t,int32_t,int32_t,int32_t,16,32,0,6>();
    errs += testLinearLayer<int32_t,int32_t,int32_t,int32_t,32,32,6,0>();
    errs += testLinearLayer<int32_t,int32_t,int32_t,int32_t,32,1,0,0>();
    int tmp = errs;
    errs += testNnz<1024>();
    if (errs - tmp > 0) {
        std::cerr << "testNnz failed: skipping SparseLinear test" << std::endl;
    }
    else {
        errs += testSparseLinear<uint8_t,int8_t,int32_t,int32_t,1024,16,7,0>();
    }

    errs += testWholeNetwork();

    return errs;
}
