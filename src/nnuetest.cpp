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


// Validate the full int32 head pipeline (Raphael-style quantization): L1-output
// CReLU clamp -> int32 L2 affine (no shift) -> CReLU clamp -> int32/int64 output
// affine -> final OUTPUT_SCALE/OUTPUT_DIVISOR scaling. Self-contained: builds the
// head layers with synthetic weights and compares against an independent scalar
// reference, so it does not depend on a loaded network file.
static int testIntHead() {
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
    return err;
}

int testNNUE() {

    int errs = 0;

    errs += testCReLU<int32_t,int32_t,1024,255,6>();
    errs += testCReLU<int32_t,int32_t,1024,255,0>();
    errs += testCReLU<int32_t,int32_t,16, 255, 6>(); // should not use SIMD
    errs += testCReLU<int32_t,int32_t,16, 255, 0>(); // should not use SIMD
    errs += testPairwiseMult<int16_t,int8_t,1024>();
    // v8 sparse head: pairwise-mul of one perspective (2*HIDDEN_WIDTH/2 -> u8)
    errs += testPairwiseMult<int16_t,uint8_t,nnue::NetworkParams::HIDDEN_WIDTH>();
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
        // v8 sparse L1: pairwise-mul u8 input (HIDDEN_WIDTH) -> L1_SIZE, no shift
        errs += testSparseLinear<uint8_t,int8_t,int32_t,int32_t,
                                 nnue::NetworkParams::HIDDEN_WIDTH,
                                 nnue::NetworkParams::L1_SIZE,0,0>();
    }

    errs += testIntHead();

    errs += testWholeNetwork();

    return errs;
}
