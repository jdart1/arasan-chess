cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(arasan)

add_compile_definitions(ARASAN_VERSION=22.0)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set (CMAKE_CXX_STANDARD 11)

include(CheckIPOSupported)
check_ipo_supported()

# options
option(SYZYGY_TBS "SYZYGY_TBS" ON)
option(64BIT "64BIT" ON)
option(BMI2 "BMI2" OFF)
option(POPCNT "POPCNT" OFF)
option(NUMA "NUMA" OFF)

set(EXE_NAME arasanx)

add_compile_definitions(SMP SMP_STATS)

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
if(MSVC)
    set_source_files_properties(${STB}/tbprobe.c PROPERTIES COMPILE_FLAGS -Tp)
endif(MSVC)
endif(SYZYGY_TBS)

if(64BIT)
    set(EXE_NAME ${EXE_NAME}-64)
    add_compile_definitions(USE_64BIT)
else()
    set(EXE_NAME ${EXE_NAME}-32)
endif(64BIT)

if(BMI2)
    add_compile_definitions(USE_POPCNT BMI2)
    set(EXE_NAME ${EXE_NAME}-bmi2)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-mbmi2 -msse4.2)
    endif() 
elseif(POPCNT)
    add_compile_options(-DUSE_POPCNT)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
      add_compile_options(-msse4.2)
    endif()
    set(EXE_NAME ${EXE_NAME}-popcnt)
endif(BMI2)

if(NUMA)
    set(EXE_NAME ${EXE_NAME}-numa)
endif(NUMA)

add_executable (${EXE_NAME} arasanx.cpp attacks.cpp attacks.h bench.h
bench.cpp bhash.cpp bhash.h bitboard.cpp bitboard.h bitprobe.cpp
bitprobe.h board.cpp board.h boardio.cpp boardio.h bookdefs.h
bookread.cpp bookread.h bookwrit.cpp bookwrit.h calctime.cpp
calctime.h chess.cpp chess.h chessio.cpp chessio.h constant.h debug.h
eco.cpp ecodata.cpp ecodata.h eco.h ecoinfo.cpp ecoinfo.h epdrec.cpp
epdrec.h globals.cpp globals.h hash.cpp hash.h learn.cpp learn.h
legal.cpp legal.h log.cpp log.h material.cpp material.h movearr.cpp
movearr.h movegen.cpp movegen.h notation.cpp notation.h options.cpp
options.h params.cpp params.h protocol.cpp protocol.h scoring.cpp
scoring.h searchc.cpp searchc.h search.cpp search.h see.cpp see.h
stats.cpp stats.h stdendian.h syzygy.cpp syzygy.h tbconfig.h
tester.cpp tester.h threadc.cpp threadc.h threadp.cpp threadp.h
types.h ${EXTRA_SRC})

add_executable (tuner EXCLUDE_FROM_ALL attacks.cpp attacks.h bhash.cpp bhash.h
bitboard.cpp bitboard.h bitprobe.cpp bitprobe.h board.cpp board.h
boardio.cpp boardio.h bookdefs.h bookread.cpp bookread.h bookwrit.cpp
bookwrit.h calctime.cpp calctime.h chess.cpp chess.h chessio.cpp
chessio.h constant.h debug.h eco.cpp ecodata.cpp ecodata.h eco.h
ecoinfo.cpp ecoinfo.h epdrec.cpp epdrec.h globals.cpp globals.h
hash.cpp hash.h learn.cpp learn.h legal.cpp legal.h log.cpp log.h
material.cpp material.h movearr.cpp movearr.h movegen.cpp movegen.h
notation.cpp notation.h options.cpp options.h params.h
protocol.cpp protocol.h scoring.cpp scoring.h searchc.cpp searchc.h
search.cpp search.h see.cpp see.h stats.cpp stats.h stdendian.h
syzygy.cpp syzygy.h tbconfig.h tester.cpp tester.h threadc.cpp
threadc.h threadp.cpp threadp.h tune.h tune.cpp tuner.cpp types.h
vparams.cpp ${EXTRA_SRC})

add_executable (pgnselect EXCLUDE_FROM_ALL util/pgnselect.cpp attacks.cpp attacks.h bhash.cpp bhash.h
bitboard.cpp bitboard.h bitprobe.cpp bitprobe.h board.cpp board.h
boardio.cpp boardio.h bookdefs.h bookread.cpp bookread.h bookwrit.cpp
bookwrit.h calctime.cpp calctime.h chess.cpp chess.h chessio.cpp
chessio.h constant.h debug.h eco.cpp ecodata.cpp ecodata.h eco.h
ecoinfo.cpp ecoinfo.h epdrec.cpp epdrec.h globals.cpp globals.h
hash.cpp hash.h learn.cpp learn.h legal.cpp legal.h log.cpp log.h
material.cpp material.h movearr.cpp movearr.h movegen.cpp movegen.h
notation.cpp notation.h options.cpp options.h params.cpp params.h
protocol.cpp protocol.h scoring.cpp scoring.h searchc.cpp searchc.h
search.cpp search.h see.cpp see.h stats.cpp stats.h stdendian.h
syzygy.cpp syzygy.h tbconfig.h tester.cpp tester.h threadc.cpp
threadc.h threadp.cpp threadp.h types.h ${EXTRA_SRC})

add_executable (playchess EXCLUDE_FROM_ALL util/playchess.cpp attacks.cpp attacks.h bhash.cpp bhash.h
bitboard.cpp bitboard.h bitprobe.cpp bitprobe.h board.cpp board.h
boardio.cpp boardio.h bookdefs.h bookread.cpp bookread.h bookwrit.cpp
bookwrit.h calctime.cpp calctime.h chess.cpp chess.h chessio.cpp
chessio.h constant.h debug.h eco.cpp ecodata.cpp ecodata.h eco.h
ecoinfo.cpp ecoinfo.h epdrec.cpp epdrec.h globals.cpp globals.h
hash.cpp hash.h learn.cpp learn.h legal.cpp legal.h log.cpp log.h
material.cpp material.h movearr.cpp movearr.h movegen.cpp movegen.h
notation.cpp notation.h options.cpp options.h params.cpp params.h
protocol.cpp protocol.h scoring.cpp scoring.h searchc.cpp searchc.h
search.cpp search.h see.cpp see.h stats.cpp stats.h stdendian.h
syzygy.cpp syzygy.h tbconfig.h tester.cpp tester.h threadc.cpp
threadc.h threadp.cpp threadp.h types.h ${EXTRA_SRC})

add_executable (makebook EXCLUDE_FROM_ALL util/makebook.cpp attacks.h attacks.cpp bhash.cpp bhash.h
bitboard.cpp bitboard.h bitprobe.cpp bitprobe.h board.cpp board.h
boardio.cpp boardio.h bookdefs.h bookread.cpp bookread.h bookwrit.cpp
bookwrit.h calctime.cpp calctime.h chess.cpp chess.h chessio.cpp
chessio.h constant.h debug.h eco.cpp ecodata.cpp ecodata.h eco.h
ecoinfo.cpp ecoinfo.h epdrec.cpp epdrec.h globals.cpp globals.h
hash.cpp hash.h learn.cpp learn.h legal.cpp legal.h log.cpp log.h
material.cpp material.h movearr.cpp movearr.h movegen.cpp movegen.h
notation.cpp notation.h options.cpp options.h params.cpp params.h
protocol.cpp protocol.h scoring.cpp scoring.h searchc.cpp searchc.h
search.cpp search.h see.cpp see.h stats.cpp stats.h stdendian.h
syzygy.cpp syzygy.h tbconfig.h tester.cpp tester.h threadc.cpp
threadc.h threadp.cpp threadp.h types.h ${EXTRA_SRC})

add_executable (ecocoder EXCLUDE_FROM_ALL util/ecocoder.cpp attacks.cpp attacks.h bhash.cpp bhash.h
bitboard.cpp bitboard.h bitprobe.cpp bitprobe.h board.cpp board.h
boardio.cpp boardio.h bookdefs.h bookread.cpp bookread.h bookwrit.cpp
bookwrit.h calctime.cpp calctime.h chess.cpp chess.h chessio.cpp
chessio.h constant.h debug.h eco.cpp ecodata.cpp ecodata.h eco.h
ecoinfo.cpp ecoinfo.h epdrec.cpp epdrec.h globals.cpp globals.h
hash.cpp hash.h learn.cpp learn.h legal.cpp legal.h log.cpp log.h
material.cpp material.h movearr.cpp movearr.h movegen.cpp movegen.h
notation.cpp notation.h options.cpp options.h params.cpp params.h
protocol.cpp protocol.h scoring.cpp scoring.h searchc.cpp searchc.h
search.cpp search.h see.cpp see.h stats.cpp stats.h stdendian.h
syzygy.cpp syzygy.h tbconfig.h tester.cpp tester.h threadc.cpp
threadc.h threadp.cpp threadp.h types.h ${EXTRA_SRC})

add_executable (makeeco EXCLUDE_FROM_ALL util/makeeco.cpp attacks.cpp attacks.h bhash.cpp bhash.h
bitboard.cpp bitboard.h bitprobe.cpp bitprobe.h board.cpp board.h
boardio.cpp boardio.h bookdefs.h bookread.cpp bookread.h bookwrit.cpp
bookwrit.h calctime.cpp calctime.h chess.cpp chess.h chessio.cpp
chessio.h constant.h debug.h eco.cpp ecodata.cpp ecodata.h eco.h
ecoinfo.cpp ecoinfo.h epdrec.cpp epdrec.h globals.cpp globals.h
hash.cpp hash.h learn.cpp learn.h legal.cpp legal.h log.cpp log.h
material.cpp material.h movearr.cpp movearr.h movegen.cpp movegen.h
notation.cpp notation.h options.cpp options.h params.cpp params.h
protocol.cpp protocol.h scoring.cpp scoring.h searchc.cpp searchc.h
search.cpp search.h see.cpp see.h stats.cpp stats.h stdendian.h
syzygy.cpp syzygy.h tbconfig.h tester.cpp tester.h threadc.cpp
threadc.h threadp.cpp threadp.h types.h ${EXTRA_SRC})

target_compile_options(tuner PUBLIC -DTUNE)

target_link_libraries(${EXE_NAME} Threads::Threads)
target_link_libraries(tuner Threads::Threads)
target_link_libraries(playchess Threads::Threads)
target_link_libraries(pgnselect Threads::Threads)
target_link_libraries(makeeco Threads::Threads)
target_link_libraries(ecocoder Threads::Threads)
target_link_libraries(makebook Threads::Threads)

set_target_properties(${EXE_NAME} tuner playchess pgnselect makebook makeeco ecocoder PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)

add_custom_target(utils)

add_dependencies(utils playchess pgnselect makeeco ecocoder makebook)




