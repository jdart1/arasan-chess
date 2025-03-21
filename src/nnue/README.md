# nnue

This code provides an implementation of a [Efficiently Updatable
Neural Network (NNUE)](https://www.chessprogramming.org/NNUE) for
chess. Originally this was compatible with the "[version
4](https://github.com/official-stockfish/Stockfish/commit/cb9c2594fcedc881ae8f8bfbfdf130cf89840e4c)"
implementation in
[Stockfish](https://github.com/official-stockfish/Stockfish), aka the
"SFv4" architecture. The original Stockfish NNUE was based on
contributions from Hisayori Noda aka Nodchip. Later versions have been
implemented primarily by Tomasz Sobczyk.

A Stockfish-compatible implementation is still available, in the
"SFv4" branch. However, this branch implements a different NNUE
architecture, used in Arasan 25.0. That architecture consists of a
horizonally mirrored feature transformer layer, with king buckets,
followed by a squared CRelU activation layer and then a single affine
layer with buckets based on material count. All weights are
16-bit integers. The key parameters of the network are specified in the file
`nnparams.h`. This architecture is similar to that of several other
strong chess engines including Obsidian, PlentyChess, Caissa, Virdithas, etc.

The Arasan network was tuned with [bullet](https://github.com/jw1912/bullet).

The quantized output from the bullet trainer is further processed by the program
`add_nn_version`, available in the util subdirectory, which adds a 4-byte version
number to the start of the file. That version is validated by the network
reading code at runtime.

## Copyright, license

Copyright 2021-2024 by Jon Dart. All Rights Reserved.

MIT Licensed.

## Implemented Features

- can read Stockfish network files using insertion operator on the Network class
- templated to support different network sizes, weight types, etc.
- full and incremental update supported
- SIMD support for x86_64 and ARM NEON architectures
- unit test code

## Missing Features

- read only, no support for writing network files
- does not validate hash codes from existing network files
- lacking certain optimizations that are in Stockfish

## Compilation


Requires C++-17. The Makefile (Gnu Make) builds a test executable. -DSIMD must be specificed to select SIMD optimizations. If SIMD is set then
also one or more flags must be set to select the desired instruction set(s).

Add -DNEON to use the ARMv8 NEON instruction set.

For x86_64 processors, the following flags can be set. They can be combined and are utilized in the following order of precedence.

1. AVX512
2. VNNI (in addition to AVX2)
3. AVX2
4. SSE41 (in addition to SSSE3)
5. SSSE3 (assumes SSE2 also present)
6. SSE2

If -DSIMD is enabled, at least one of: AVX2, SSSE3 or SSE2 must also be selected.

## Interface

The Evaluator class is templated and assumes use of a class or typedef that provides a basic interface to a chess position.

The interface subdirectory contains an implementation named ChessInterface that provides the necessary methods and is used by the test code. Users are likely to want to replace this with a class that wraps whatever native position representation they have.

## Test code

The nnue_test program in the test subdirectory tests the first layer of the network and the code for the interior linear transformation layers. It should execute and produce "0 errors" on output.

This program can also be used with the -f switch to read an existing network file, although its contents are not validated.
