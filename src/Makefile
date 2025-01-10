#
# Gnu Makefile for Arasan
# Copyright 2004-2024 by Jon Dart. All Rights Reserved.
#
VERSION = 25.0

# paths, modify as needed:

# location of source files
SRC = ../src
# location of .o files
BUILD	= ../build
# location for executables and binary book files
EXPORT  = ../bin
# location for profile-generating executables
PROFILE = ../profile
# location for profile information
PROF_DATA = ../prof_data
# location of release package
RELEASE = ../release
# location of book sources
BOOK    = ../book
# location of test files
TESTS   = ../tests
# installation directory (default)
INSTALL = /usr/local/bin
# location of the Syzygy tablebase source code (Fathom) (if used)
STB=syzygy/src
# compile in NUMA support
#NUMA=1
# location of HWLOC library (for NUMA support)
HWLOC=/usr/local
#
# directory for utility programs
UTIL = util

# default network
NETWORK ?= arasanv3-20241109.nnue

# Path to git_archive_all (https://github.com/Kentzo/git-archive-all)
GIT_ARCHIVE_ALL_PATH ?= /home/jdart/bin/git-archive-all

# set to enable compliation with Syzygy tablebase support
# SYZYGY_TBS := 1

FLAGS := -I .
ifdef NUMA
FLAGS := $(FLAGS) -DNUMA -I$(HWLOC)/include
NUMA_OBJS=$(BUILD)/topo.o
NUMA_PROFILE_OBJS=$(PROFILE)/topo.o
endif
#
#ifdef SYZYGY_TBS
#FLAGS := $(FLAGS) -DSYZYGY_TBS
#endif
ifdef TUNE
FLAGS := $(FLAGS) -DTUNE
endif
NNUE_FLAGS := -Innue -DNETWORK=$(NETWORK)
NNUE_SRC := nnueintf.cpp

#PROF     = -pg
#DEBUG    = -g -D_DEBUG=1 -DUNIT_TESTS
#TRACE    = -D_TRACE=1 -D_THREAD_TRACE

shell        := $(SHELL)
UNAME        := $(shell uname -s|awk '{print tolower($$0)}')
ifndef OSTYPE
OSTYPE       := $(UNAME)
endif
ARCH         := $(shell uname -m)

# SMP flags
SMPFLAGS =  -DSMP_STATS
SMPLIB   = -lpthread

POPCNT_FLAGS := -DUSE_POPCNT
ifeq ("$(ARCH)","x86_64")
BASIC_SIMD_FLAGS = -DSIMD -DSSE2 -msse2
POPCNT_FLAGS := $(POPCNT_FLAGS) -DSSSE3 -DSSE41 -mssse3 -msse4.1 -msse4.2 -mpopcnt
BMI2_FLAGS := $(POPCNT_FLAGS) -DBMI2 -mbmi2
AVX2_FLAGS := -DAVX2 -mavx2 -mfma
AVX512_FLAGS := -DAVX512 -mavx512bw
else ifeq ("$(ARCH)","arm64")
NEON_FLAGS := -DSIMD -DNEON
else ifeq ("$(ARCH)","aarch64")
NEON_FLAGS := -DSIMD -DNEON
endif

ifdef BUILD_TYPE
ifeq ("$(BUILD_TYPE)","old")
else
FLAGS := $(FLAGS) $(BASIC_SIMD_FLAGS)
ifeq ("$(BUILD_TYPE)","modern")
FLAGS := $(FLAGS) $(POPCNT_FLAGS)
else ifeq ("$(BUILD_TYPE)","avx2")
FLAGS := $(FLAGS)  $(POPCNT_FLAGS) $(AVX2_FLAGS)
else ifeq ("$(BUILD_TYPE)","avx2-bmi2")
FLAGS := $(FLAGS)  $(POPCNT_FLAGS) $(AVX2_FLAGS) $(BMI2_FLAGS)
else ifeq ("$(BUILD_TYPE)","avx512")
FLAGS := $(FLAGS)  $(POPCNT_FLAGS) $(AVX512_FLAGS) $(AVX2_FLAGS) $(BMI2_FLAGS)
else ifeq ("$(BUILD_TYPE)","neon")
FLAGS := $(FLAGS) $(NEON_FLAGS)
else
$(error unrecognized BUILD_TYPE: should be one of: old modern avx2 avx2-bmi2 avx512 neon)
endif
endif
else
# no BUILD_TYPE specified
FLAGS := $(FLAGS) $(BASIC_SIMD_FLAGS)
endif

PGO_RUN_FLAGS = -H 64M -a

IN_GIT := $(shell git rev-parse --is-inside-work-tree)
ifeq ("$(IN_GIT)","true")
ARASAN_VERSION ?= $(shell git describe --tags --always)
else
ARASAN_VERSION ?= $(VERSION)
endif
FLAGS := $(FLAGS) -DARASAN_VERSION=$(ARASAN_VERSION)

ifeq ("$(ARCH)","x86_64")
FLAGS := $(FLAGS) -msse2 -msse3
endif

LBITS := $(shell getconf LONG_BIT)
LIBS := -lc -lm

ifdef NUMA
LIBS := -lhwloc $(LIBS)
endif

ifdef EXE
ARASANX := $(EXE)
else
ifeq ($(LBITS),64)
   FLAGS := $(FLAGS) -D_64BIT
endif
ARASANX := arasanx-$(LBITS)
ifdef NUMA
ARASANX := $(ARASANX)-numa
endif
ifdef BUILD_TYPE
ARASANX := $(ARASANX)-$(BUILD_TYPE)
endif
endif

ifeq ("$(CC)","icc")
# Intel C++ compiler
INTEL := 1
else
ifeq ("$(CC)","icpx")
CLANG := 1
ONEAPI := 1
endif
endif

ifdef INTEL
CPP     := $(CC)
LD      := $(CC)
AR      := xiar
FLAGS  := $(FLAGS) -std=c++17 -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM
PROF_GEN = -prof-gen -prof-dir $(PROF_DATA)
PROF_USE = -prof-use -ip -prof-dir $(PROF_DATA)
ifndef DEBUG
OPT     = -O3 -fno-rtti -DNDEBUG
endif

else
# not Intel

ifneq (,$(findstring clang,$(CC)))
CLANG := 1
else
# detect 'cc' aliased to 'clang'
CC_VERSION := $(shell $(CC) -v  2>&1 >/dev/null | cat)
ifneq (,$(findstring clang,$(CC_VERSION)))
CLANG := 1
endif
endif

ifdef CLANG

CPP     := $(CC)
LD      := $(CC)
AR      = ar

LLVM_PROFDATA ?= llvm-profdata
LLVM_PROFGEN ?= llvm-profgen

ifdef CSSPGO
ifdef ONEAPI
PROF_GEN = -fprofile-sample-generate
PROF_USE = -fprofile-sample-use=$(PROF_DATA)/arasanx.freq.prof
else
PROF_GEN = -fpseudo-probe-for-profiling
PROF_USE = -fprofile-sample-use=$(PROF_DATA)/arasanx.freq.prof
endif
else
PROF_GEN = -fprofile-generate=$(PROF_DATA)
# extra flags for compile only (not link):
PROF_GEN_EXTRA = -mllvm -enable-name-compression=false
PROF_USE = -fprofile-use=$(PROF_DATA)
endif

FLAGS := -std=c++17 -Wall -Wextra -Wshadow -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(FLAGS)
ifndef DEBUG
LTO := -flto
OPT     = -O3 -fno-rtti -DNDEBUG
LDFLAGS := $(LDFLAGS) $(OPT)
ifeq ($(findstring darwin,$(OSTYPE)),)
ifeq ($(findstring cygwin,$(OSTYPE)),)
ifeq ($(findstring msys,$(OSTYPE)),)
# "Gold" not supported on darwin or cygwin
LDFLAGS := $(LDFLAGS) -fuse-ld=gold
endif
endif
endif
endif

else

CC ?= g++
CPP     := $(CC)
LD      := $(CC)
AR      = ar

PROF_GEN = -fprofile-generate -fprofile-dir=$(PROF_DATA)
PROF_USE = -fprofile-use -fprofile-dir=$(PROF_DATA)

GCCMINVERSION := 7
# check for bc
ifeq (, $(shell which bc))
$(error "No 'bc' found in PATH, please install it")
endif
# Get 1st two digits of full GCC version
GCCVER := $(shell expr `$(CC) -dumpfullversion | awk -F. '{printf "%s.%s\n", $$1, $$2}'`)
GCC11 := $(shell echo '$(GCCVER) >= 11.0' | bc)
GCCNEW := $(shell echo '$(GCCVER) >= $(GCCMINVERSION)' | bc)

ifeq "$(GCCNEW)" "1"
# options for GCC
FLAGS := -Wall -Wextra -Wshadow -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(FLAGS)
ifneq "$(GCC11)" "1"
# c++17 support must be enabled explicitly
FLAGS := $(FLAGS) -std=c++17
endif
SMPFLAGS := $(SMPFLAGS) -pthread
ifeq ($(findstring darwin,$(OSTYPE)),)
ifeq ($(findstring cygwin,$(OSTYPE)),)
ifeq ($(findstring msys,$(OSTYPE)),)
LDFLAGS := $(LDFLAGS) -fuse-ld=gold
endif
endif
endif
ifndef DEBUG
OPT	:= -Ofast -fno-rtti -fno-enforce-eh-specs -DNDEBUG
LTO     := -flto
LDFLAGS := $(LDFLAGS) $(OPT)
endif
else
$(error GCC version is $(GCCVER): versions < $(GCCMINVERSION) not supported)
endif

endif
endif

ifeq ("darwin","$(findstring darwin,$(OSTYPE))")
FLAGS := -D_MAC $(FLAGS)
LDFLAGS := $(LDFLAGS)
LLVM_PROFDATA := xcrun $(LLVM_PROFDATA)
else
LIBS := $(LIBS) -lrt
LLVM_PROFDATA := $(LLVM_PROFDATA)
endif

ifdef NUMA
LDFLAGS := $(LDFLAGS) -L$(HWLOC)/lib
endif

default: dirs $(EXPORT)/$(ARASANX)

utils: dirs $(EXPORT)/playchess $(EXPORT)/makebook $(EXPORT)/makeeco $(EXPORT)/ecocoder \
$(EXPORT)/selfplay $(EXPORT)/checkbin $(EXPORT)/convertbin

clean: dirs
	rm -f $(BUILD)/*.o
	rm -f $(PROFILE)/*.o
	rm -f $(PROFILE)/*.gcda
	rm -f $(PROFILE)/*.gcno
	rm -f $(PROFILE)/*.dyn
	rm -f $(PROFILE)/*.nnue
	rm -f $(PROF_DATA)/*.*
	cd $(EXPORT) && rm -f arasanx* makeeco makebook playchess ecocoder selfplay checkbin

dirs:
	mkdir -p $(BUILD)
	mkdir -p $(EXPORT)
	mkdir -p $(PROFILE)
	mkdir -p $(PROF_DATA)

release:
	mkdir -p ../release
	cd .. && $(GIT_ARCHIVE_ALL_PATH)/git_archive_all.py --prefix arasan-$(VERSION)/ release/arasan-$(VERSION).tar
	gzip ../release/arasan-$(VERSION).tar

install: default
	mkdir -p $(INSTALL)/arasan-$(VERSION)
	cp $(EXPORT)/$(ARASANX) $(INSTALL)/arasan-$(VERSION)
	cp ../network/$(NETWORK) $(INSTALL)/arasan-$(VERSION)
	chmod 755 $(INSTALL)/arasan-$(VERSION)/$(ARASANX)
	ln -s $(INSTALL)/arasan-$(VERSION)/$(ARASANX) $(INSTALL)/arasan-$(VERSION)/arasanx
	cp $(SRC)/arasan.rc $(INSTALL)/arasan-$(VERSION)
	chmod u+s $(INSTALL)/arasan-$(VERSION)/$(ARASANX)

$(BUILD)/%.o: %.cpp
	$(CPP) $(FLAGS) $(OPT) $(TRACE) $(SMPFLAGS) $(NNUE_FLAGS) $(DEBUG) $(CXXFLAGS) -c -o $@ $<

$(BUILD)/%.o: $(UTIL)/%.cpp
	$(CPP) $(FLAGS) $(OPT) $(TRACE) $(SMPFLAGS) $(NNUE_FLAGS) $(DEBUG) $(CXXFLAGS) -c -o $@ $<

ifeq ($(PASS),1)
CPP_PROF_FLAGS := $(PROF_GEN) $(OPT) $(TRACE) $(FLAGS) $(SMPFLAGS) $(NNUE_FLAGS) $(DEBUG) $(CXXFLAGS)
else
CPP_PROF_FLAGS := $(PROF_USE) $(OPT) $(LTO) $(TRACE) $(FLAGS) $(SMPFLAGS) $(NNUE_FLAGS) $(DEBUG) $(CXXFLAGS)
endif

$(PROFILE)/%.o: %.cpp
	$(CPP) $(CPP_PROF_FLAGS) $(PROF_GEN_EXTRA) $(CXXFLAGS) -c -o $@ $<

profiled: dirs
	ln -s -f ../network/*.nnue $(PROFILE)
	@$(MAKE) PASS=1 BUILD_TYPE=$(BUILD_TYPE) PGO=1 profile
	@$(MAKE) PASS=1 BUILD_TYPE=$(BUILD_TYPE) PGO=1 profile-run
ifdef CLANG
ifdef CSSPGO
	$(LLVM_PROFGEN) --perfdata $(PROF_DATA)/arasanx.perf.data --binary $(PROFILE)/arasanx --output $(PROF_DATA)/arasanx.freq.prof
else
	$(LLVM_PROFDATA) merge -output=$(PROF_DATA)/default.profdata $(PROF_DATA)/default_*profraw
endif
endif
	@$(MAKE) PASS=2 BUILD_TYPE=$(BUILD_TYPE) PGO=1 profile

#ifdef SYZYGY_TBS
#TB_SOURCES := $(TB_SOURCES) syzygy.cpp $(STB)/tbprobe.c
#TB_OBJS := $(BUILD)/syzygy.o $(BUILD)/tbprobe.o
#TB_PROFILE_OBJS := $(PROFILE)/syzygy.o $(PROFILE)/tbprobe.o
#STB_FLAGS := -x c++
#$(BUILD)/%.o: $(STB)/%.c
#	$(CC)  $(FLAGS) $(STB_FLAGS) $(OPT) $(DEBUG) $(CXXFLAGS) -c $< -o $@
#
#$(PROFILE)/%.o: $(STB)/%.c
#	$(CC) $(STB_FLAGS) $(CPP_PROF_FLAGS) $(PROF_GEN_EXTRA) $(CXXFLAGS) -c $< -o $@
#
#endif

ifeq ("$(findstring UNIT_TESTS,$(DEBUG))","UNIT_TESTS")
UNIT_TEST_SRC:=unit.cpp binformat.cpp
endif
ifeq ("$(findstring UNIT_TESTS,$(CXXFLAGS))","UNIT_TESTS")
UNIT_TEST_SRC:=unit.cpp binformat.cpp
endif

ARASANX_SOURCES = arasanx.cpp tester.cpp bench.cpp protocol.cpp \
input.cpp globals.cpp board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp calctime.cpp eco.cpp ecodata.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp $(NNUE_SRC) $(UNIT_TEST_SRC)

UTIL_SOURCES = globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp calctime.cpp eco.cpp ecodata.cpp \
legal.cpp stats.cpp threadp.cpp threadc.cpp $(NNUE_SRC)

MAKEBOOK_SOURCES = makebook.cpp $(UTIL_SOURCES)

MAKEECO_SOURCES = makeeco.cpp $(UTIL_SOURCES)

ECOCODER_SOURCES = ecocoder.cpp $(UTIL_SOURCES)

PLAYCHESS_SOURCES = playchess.cpp $(UTIL_SOURCES)

SELFPLAY_SOURCES = selfplay.cpp $(UTIL_SOURCES) binformat.cpp

CHECKBIN_SOURCES = checkbin.cpp $(UTIL_SOURCES) binformat.cpp

CONVERTBIN_SOURCES = convertbin.cpp $(UTIL_SOURCES) binformat.cpp

ARASANX_PROFILE_OBJS = $(patsubst %.cpp, $(PROFILE)/%.o, $(ARASANX_SOURCES)) $(ASM_PROFILE_OBJS) $(TB_PROFILE_OBJS) $(NUMA_PROFILE_OBJS)
ARASANX_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(ARASANX_SOURCES)) $(TB_OBJS) $(NUMA_OBJS)
MAKEBOOK_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(MAKEBOOK_SOURCES)) $(TB_OBJS) $(NUMA_OBJS)
MAKEECO_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(MAKEECO_SOURCES)) $(TB_OBJS) $(NUMA_OBJS)
ECOCODER_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(ECOCODER_SOURCES)) $(TB_OBJS) $(NUMA_OBJS)
PLAYCHESS_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(PLAYCHESS_SOURCES)) $(TB_OBJS) $(NUMA_OBJS)
SELFPLAY_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(SELFPLAY_SOURCES)) $(TB_OBJS) $(NUMA_OBJS)
CHECKBIN_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(CHECKBIN_SOURCES)) $(TB_OBJS) $(NUMA_OBJS)
CONVERTBIN_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(CONVERTBIN_SOURCES)) $(TB_OBJS) $(NUMA_OBJS)

$(EXPORT)/makebook:  $(MAKEBOOK_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(MAKEBOOK_OBJS) $(DEBUG) $(CXXFLAGS) -o $(EXPORT)/makebook -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/makeeco:  $(MAKEECO_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(MAKEECO_OBJS) $(DEBUG) $(CXXFLAGS) -o $(EXPORT)/makeeco -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/ecocoder:  $(ECOCODER_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(ECOCODER_OBJS) $(DEBUG) $(CXXFLAGS) -o $(EXPORT)/ecocoder -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/playchess:  $(PLAYCHESS_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(PLAYCHESS_OBJS) $(DEBUG) $(CXXFLAGS) -o $(EXPORT)/playchess -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/selfplay:  $(SELFPLAY_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(SELFPLAY_OBJS) $(DEBUG) $(CXXFLAGS) -o $(EXPORT)/selfplay -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/checkbin:  $(CHECKBIN_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(CHECKBIN_OBJS) $(DEBUG) $(CXXFLAGS) -o $(EXPORT)/checkbin -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/convertbin:  $(CONVERTBIN_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(CONVERTBIN_OBJS) $(DEBUG) $(CXXFLAGS) -o $(EXPORT)/convertbin -lstdc++ $(LIBS) $(SMPLIB)

ifeq ($(PASS),1)
profile: dirs $(PROFILE)/arasanx
else
profile: dirs $(EXPORT)/$(ARASANX)
endif

ifeq ($(PASS),1)
$(PROFILE)/arasanx:  $(ARASANX_PROFILE_OBJS)
	cd $(PROFILE) && $(LD) $(PROF_GEN) $(ARASANX_PROFILE_OBJS) $(DEBUG) $(CXXFLAGS) -o $(PROFILE)/arasanx -lstdc++ $(LIBS) $(SMPLIB)
else
ifeq ($(PASS),2)
$(EXPORT)/$(ARASANX):  $(ARASANX_PROFILE_OBJS)
	cd $(PROFILE) && $(LD) $(PROF_USE) $(OPT) $(LTO) $(ARASANX_PROFILE_OBJS) $(DEBUG) $(CXXFLAGS) -o $(PROFILE)/arasanx -lstdc++ $(LIBS) $(SMPLIB)
	cp $(PROFILE)/arasanx $(EXPORT)/$(ARASANX)
else
# non-PGO build
$(EXPORT)/$(ARASANX):  $(ARASANX_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(ARASANX_OBJS) $(DEBUG) $(CXXFLAGS) -o $(EXPORT)/$(ARASANX) -lstdc++ $(LIBS) $(SMPLIB)
endif
endif

profile-run:
ifdef CSSPGO
	perf record -o $(PROF_DATA)/arasanx.perf.data -b -c 100003 -e br_inst_retired.near_taken:uppp -- $(PROFILE)/arasanx $(PROFILE_RUN_FLAGS) bench
	rm $(PROFILE)/*.o
	rm -f $(EXPORT)/arasanx
else
	$(PROFILE)/arasanx $(PGO_RUN_FLAGS) bench
	rm $(PROFILE)/*.o
	rm -f $(PROFILE)/arasanx $(EXPORT)/arasanx
endif

.PHONY: all clean dirs profile bmi2 profile-run install release

.EXPORT_ALL_VARIABLES:

