cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(arasan)

add_compile_definitions(ARASAN_VERSION=23.4)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set (CMAKE_CXX_STANDARD 17)

include(CheckIPOSupported)
check_ipo_supported()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# options
option(SYZYGY_TBS "Syzygy tablebase support" ON)
option(64BIT "64-bit target" ON)
option(OLD "Use no SIMD instructions" OFF)
option(MODERN "Use modern instructions" OFF)
# AVX2 and BMI2 can be specified separately or together and imply also "modern"
option(AVX2 "Use AVX2 instructions" OFF)
option(AVX512 "Use AVX512 instructions" OFF)
option(BMI2 "Use BMI2 instructions" OFF)
option(NUMA "Build for non-uniform memory access (NUMA) system" OFF)
option(UNIT_TESTS "unit tests run on startup" OFF)
option(NNUE "Neural network support" ON)

set(EXE_NAME arasanx)

add_compile_definitions(SMP SMP_STATS USE_INTRINSICS)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(_DEBUG)
endif()

if(CMAKE_SYSTEM_NAME MATCHES Darwin)
    add_compile_definitions(_MAC)
endif()

if(MSVC)
    add_compile_definitions(NOMINMAX _CRT_SECURE_NO_WARNINGS)
endif(MSVC)

if(NUMA)
    list(APPEND EXTRA_SRC topo.h topo.cpp)
    add_compile_definitions(NUMA)
endif(NUMA)

if(UNIT_TESTS)
    list(APPEND EXTRA_SRC unit.h unit.cpp)
endif(UNIT_TESTS)

if(SYZYGY_TBS)
    add_compile_definitions(SYZYGY_TBS)
    set(STB ${CMAKE_CURRENT_SOURCE_DIR}/syzygy/src)
    include_directories(${STB} .)
    list(APPEND EXTRA_SRC ${STB}/tbprobe.c ${STB}/tbprobe.h)
    SET_SOURCE_FILES_PROPERTIES(${STB}/tbprobe.c PROPERTIES LANGUAGE CXX )
endif(SYZYGY_TBS)

if(NNUE)
# SIMD only works on Intel/AMD architectures at present
    add_compile_definitions(NNUE)
    set(NNUE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/nnue)
    include_directories(${NNUE_DIR})
    list(APPEND EXTRA_SRC nnueintf.cpp)
endif(NNUE)

if(64BIT)
    set(EXE_NAME ${EXE_NAME}-64)
    add_compile_definitions(_64BIT)
else()
    set(EXE_NAME ${EXE_NAME}-32)
endif(64BIT)

if(UNIX AND NOT APPLE AND NOT WIN32)
    add_link_options("-fuse-ld=gold")
endif()

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64" OR CMAKE_SYSTEM_PROCESSOR MATCHES "86")
if(NOT OLD)
    add_compile_definitions(SIMD)
    add_compile_definitions(SSE2)
if(MODERN OR BMI2 OR AVX2 OR AVX512)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options( -msse4.1 -msse4.2 -mpopcnt)
    endif()
    add_compile_definitions(SSSE3)
    add_compile_definitions(SSE41)
    add_compile_definitions(USE_POPCNT)
endif()

if(AVX2 OR AVX512)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/arch:AVX2)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-mavx2 -mfma)
    endif()
    add_compile_definitions(AVX2)
endif(AVX2)

if(AVX512)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/arch:AVX512)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-mavx512bw)
    endif()
    add_compile_definitions(AVX512)
endif(AVX512)

if(BMI2)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-mbmi2 -msse4.2)
    endif()
    add_compile_definitions(BMI2)
endif()

# end of block starting with architecture test
endif()
endif()

if (AVX512)
     set(EXE_NAME ${EXE_NAME}-avx512)
elseif (AVX2)
     set(EXE_NAME ${EXE_NAME}-avx2)
if(BMI2)
    set(EXE_NAME ${EXE_NAME}-bmi2)
endif(BMI2)
elseif(MODERN)
    set(EXE_NAME ${EXE_NAME}-modern)
elseif (OLD)
    set(EXE_NAME ${EXE_NAME}-old)
endif()

if(NUMA)
    set(EXE_NAME ${EXE_NAME}-numa)
endif(NUMA)

add_executable (${EXE_NAME} arasanx.cpp attacks.cpp attacks.h bench.h
bench.cpp bhash.cpp bhash.h bitboard.cpp bitboard.h bitprobe.cpp
bitprobe.h board.cpp board.h boardio.cpp boardio.h bookdefs.h
bookread.cpp bookread.h bookwrit.cpp bookwrit.h calctime.cpp
calctime.h chess.cpp chess.h chessio.cpp chessio.h constant.h
eco.cpp ecodata.cpp ecodata.h eco.h ecoinfo.cpp ecoinfo.h epdrec.cpp
epdrec.h globals.cpp globals.h hash.cpp hash.h learn.cpp learn.h
legal.cpp legal.h log.cpp log.h material.cpp material.h movearr.cpp
movearr.h movegen.cpp movegen.h notation.cpp notation.h options.cpp
options.h params.cpp params.h protocol.cpp protocol.h scoring.cpp
scoring.h searchc.cpp searchc.h search.cpp search.h see.cpp see.h
stats.cpp stats.h stdendian.h syzygy.cpp syzygy.h tbconfig.h
tester.cpp tester.h threadc.cpp threadc.h threadp.cpp threadp.h
types.h bench.cpp input.cpp ${EXTRA_SRC})

add_executable (tuner EXCLUDE_FROM_ALL attacks.cpp attacks.h bhash.cpp bhash.h
bitboard.cpp bitboard.h bitprobe.cpp bitprobe.h board.cpp board.h
boardio.cpp boardio.h bookdefs.h bookread.cpp bookread.h bookwrit.cpp
bookwrit.h calctime.cpp calctime.h chess.cpp chess.h chessio.cpp
chessio.h constant.h eco.cpp ecodata.cpp ecodata.h eco.h
ecoinfo.cpp ecoinfo.h epdrec.cpp epdrec.h globals.cpp globals.h
hash.cpp hash.h learn.cpp learn.h legal.cpp legal.h log.cpp log.h
material.cpp material.h movearr.cpp movearr.h movegen.cpp movegen.h
notation.cpp notation.h options.cpp options.h params.h
protocol.cpp protocol.h scoring.cpp scoring.h searchc.cpp searchc.h
search.cpp search.h see.cpp see.h stats.cpp stats.h stdendian.h
syzygy.cpp syzygy.h tbconfig.h tester.cpp tester.h threadc.cpp
threadc.h threadp.cpp threadp.h tune.h tune.cpp tuner.cpp types.h ${EXTRA_SRC})

list(APPEND UTIL_SRC attacks.cpp attacks.h bhash.cpp bhash.h
bitboard.cpp bitboard.h bitprobe.cpp bitprobe.h board.cpp board.h
boardio.cpp boardio.h bookdefs.h bookread.cpp bookread.h bookwrit.cpp
bookwrit.h calctime.cpp calctime.h chess.cpp chess.h chessio.cpp
chessio.h constant.h eco.cpp ecodata.cpp ecodata.h eco.h
ecoinfo.cpp ecoinfo.h epdrec.cpp epdrec.h globals.cpp globals.h
hash.cpp hash.h learn.cpp learn.h legal.cpp legal.h log.cpp log.h
material.cpp material.h movearr.cpp movearr.h movegen.cpp movegen.h
notation.cpp notation.h options.cpp options.h params.h
scoring.cpp scoring.h searchc.cpp searchc.h
search.cpp search.h see.cpp see.h stats.cpp stats.h stdendian.h
syzygy.cpp syzygy.h tbconfig.h threadc.cpp
threadc.h threadp.cpp threadp.h tune.h tune.cpp tuner.cpp types.h ${EXTRA_SRC})

add_executable (pgnselect EXCLUDE_FROM_ALL util/pgnselect.cpp ${UTIL_SRC})

add_executable (playchess EXCLUDE_FROM_ALL util/playchess.cpp ${UTIL_SRC})

add_executable (makebook EXCLUDE_FROM_ALL util/makebook.cpp ${UTIL_SRC})

add_executable (ecocoder EXCLUDE_FROM_ALL util/ecocoder.cpp ${UTIL_SRC})

add_executable (makeeco EXCLUDE_FROM_ALL util/makeeco.cpp ${UTIL_SRC})

add_executable (selfplay EXCLUDE_FROM_ALL util/selfplay.cpp ${UTIL_SRC})

target_compile_options(tuner PUBLIC -DTUNE)

target_link_libraries(${EXE_NAME} PRIVATE Threads::Threads)
target_link_libraries(tuner PRIVATE Threads::Threads)
target_link_libraries(playchess PRIVATE Threads::Threads)
target_link_libraries(pgnselect PRIVATE Threads::Threads)
target_link_libraries(makeeco PRIVATE Threads::Threads)
target_link_libraries(ecocoder PRIVATE Threads::Threads)
target_link_libraries(makebook PRIVATE Threads::Threads)
target_link_libraries(selfplay PRIVATE Threads::Threads)

set_target_properties(${EXE_NAME} tuner playchess pgnselect makebook makeeco ecocoder selfplay PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)

add_custom_target(utils)

add_dependencies(utils playchess pgnselect makeeco ecocoder makebook selfplay)

