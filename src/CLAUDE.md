# Project: Arasan

## Project Description
- This is a chess playing program

## Code Conventions
- clang-format for formatting
- indent is 4. Use spaces, not tabs. No trailing whitespace in files.
- static constants are all upper case
- class and struct names start with uppercase letter
- function and variable names start with lowercase letter
- the code uses both camel case and underscore case. Prefer camel case for class or struct names

## Project Structure
- /src - Main source code
  - /nnue - Efficiently Updated Neural Network (NNUE) code
  - /syzygy - Sygygy format tablebase reading code
  - /utils - source for utility programs

# Architecture
 - see doc/programr.hmtl for a description of the program's data structures and algorithms

## Building and testing
- Do not submit code for review that does not compile or triggers compiler warnings
- To build the program executable, "make clean && make -j BUILD_TYPE=avx2" in the src directory
- To build unit tests, "make clean && make BUILD_TYPE=avx2 -j unit" in the src directory
- The unit executable will be in the bin subdirectory. when run, it will produce output ending with an error count (the return value of the program is also the error count)
- BUILD_TYPE can be one of: modern, avx2, avx2-bmi2, avx512, neon, or omitted (implies SSE2 executable).
- By default, test changes using the avx2 build. But when making changes that use SIMD (notably the NNUE code), test changes on this platform for avx2, avx2-bmi2, modern, and default (no BUILD_TYPE specified) builds. But do not use neon on this platform: also avx512 executables can be built, but cannot be run.
- To run a SPRT test (actual games): 1) "make clean && make BUILD_TYPE=avx2 -j profiled" in the src directory; 2) execute "scatter ../bin/arasanx-64-avx2"; 3) execute ./sprt in the tools subdirectory 4) monitor results with "python3 -u monitor.py </dev/null >&monitor.log & tail -f monitor.log" in the tools subdirectory. Test will automatically stop when a sigifificant result is obtained. To terminate it early, execute the "stoo-all" script (in the path). Very bad results (<30% score) is cause for termination after a few hundred games.
- If adding a significant new feature, first write a unit test for it in unit.cpp or (for NNUE related code) nnuetest.cpp.

## Performance testing
- Runtime performance is very important. Do not introduce changes that cause regression in performance.
- To test performance, build the executable with "make clean && make BUILD_TYPE=avx2 CC=clang -j profiled", then run the executable with "bench" as an argument. The relevant output metric is NPS: higher is better.
- However, NPS is not the definitive measure of performance: SPRT testing is.

w## Instructions for Claude
- Include only the src directory and the nnue ubdirectory in scope, unless told otherwise. Code in the syzygy subdirectory belongs to a separate project, and should not be modified.
- Create a local branch for your work and follow the Git guidelines below.
- Do not make or suggest changes outside the scope of the assigned task.
- Do not make or suggest non-functional changes to the code.
- Be systematic. Focus on one specific aspect of a problem at a time.
- If there are multiple changes to be made, even if related, validate the plan before proceeding.
- Avoid making large changes within a file or changes that affect many files, unless strictly necessary.
- When large changes are made, where possible do them in stages. Each stage should make progress towards the end goal. Validate and test each stage before proceeding to the next.
- Use SIMD instructions intelligently to maximize runtime performance.
- Data structures used for SIMD operations must be aligned, usually the nnue::DEFAULT_ALIGN value is ok
- In src/nnue/simd.h, use the utility functions defined in simddefs.h in preference to direct calls to intrinsics.
- Use the functions in bitutil.h for bit-level operations.

## C++ guidelines
- Current code standard is C++17: do not use features of later versions
- Avoid duplicating code
- Do not shadow variables
- Follow the principle of encapsulation: class state is private, access and update methods are used to access and update state (however, existing code makes some use of the "friend" declaration, and also may use structures whose state is public).
- Methods that do not alter class state should be declared "const" ("const noexcept" if the function cannot throw an exception).
- Do not use C headers unless necessary. Prefer the equivalent C++ headers: for example <cassert>.
- Prefer inline functions over preprocessor macros.
- Use constexpr where possible when declaring variables.
- Pass small-sized values to functions by value, larger values as references (const references if only read-only access is needed)
- std::array is generally preferred over C-style arrays. But avoid structures with dynamically allocated contents (such as std::list) in peformance-senstive code.

## Git guidelines
- When completing a part of a task, or after significant progress has been made, commit your changes
- Use rollback when necessary to restore prior state
- Keep Git commit comments brief and concise
- When your task is complete, leave the branch unmerged: I will merge manually



