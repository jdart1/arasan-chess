#
# Gnu Makefile for Arasan
# Copyright 2004-2020 by Jon Dart. All Rights Reserved.
#
VERSION = 22.1

# paths, modify as needed:

# location of source files
SRC = ../src
# location of .o files
BUILD	= ../build
# location of .o files for tuning program
TUNE_BUILD	= ../tune_build
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

# Path to git_archive_all (https://github.com/Kentzo/git-archive-all)
GIT_ARCHIVE_ALL_PATH ?= /home/jdart/bin/git-archive-all

# set to enable compliation with Syzygy tablebase support
SYZYGY_TBS := 1

CFLAGS = -I.
ifdef NUMA
CFLAGS := $(CFLAGS) -DNUMA -I$(HWLOC)/include
endif
ifdef SYZYGY_TBS
CFLAGS := $(CFLAGS) -DSYZYGY_TBS
endif

# SMP flags (note: we do not support a non-SMP build anymore)
SMPFLAGS = -DSMP -DSMP_STATS
SMPLIB   = -lpthread

POPCNT_FLAGS := -DUSE_POPCNT -msse4.1 -msse4.2 -mpopcnt
BMI2_FLAGS := $(POPCNT_FLAGS) -DBMI2 -mbmi2
AVX2_FLAGS := $(BMI2_FLAGS) -mavx2

PGO_RUN_FLAGS = -H 64M

TUNE_FLAGS := -DTUNE

ifdef NUMA
NUMA_OBJS=$(BUILD)/topo.o
NUMA_PROFILE_OBJS=$(PROFILE)/topo.o
NUMA_TUNE_OBJS=$(TUNE_BUILD)/topo.o
endif

#PROF     = -pg
#DEBUG   = -g -D_DEBUG=1 -DUNIT_TESTS
#TRACE    = -D_TRACE=1 -D_THREAD_TRACE

shell        := $(SHELL)
UNAME        := $(shell uname -s|awk '{print tolower($$0)}')
ifndef OSTYPE
OSTYPE       := $(UNAME)
endif
ARCH         := $(shell uname -m)

IN_GIT := $(shell git rev-parse --is-inside-work-tree)
ifeq ("$(IN_GIT)","true")
ARASAN_VERSION ?= $(shell git describe --tags --always)
else
ARASAN_VERSION ?= $(VERSION)
endif
CFLAGS := $(CFLAGS) -DARASAN_VERSION=$(ARASAN_VERSION)

ifeq ("$(ARCH)","x86_64")
CFLAGS := $(CFLAGS) -msse2 -msse3
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
   CFLAGS := $(CFLAGS) -D_64BIT
endif
ARASANX := arasanx-$(LBITS)
ifdef NUMA
ARASANX := $(ARASANX)-numa
endif
ifdef BUILD_TYPE
ARASANX := $(ARASANX)-$(BUILD_TYPE)
endif
endif

TUNER := tuner

ifeq ("$(CC)","icc")
# Intel C++ compiler
CPP     = icc
LD      = icc
AR      = xiar
CFLAGS  := $(CFLAGS) -std=c++11 -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM
ifndef DEBUG
OPT     = -O3 -fno-rtti -DNDEBUG
GTB_OPT := -O3 -fno-rtti -DNDEBUG
endif

PROF_GEN = -prof-gen -prof-dir $(PROF_DATA)
PROF_USE = -prof-use -ip -prof-dir $(PROF_DATA)

else

ifeq ("$(findstring clang,$(CC))","clang")

CPP     := $(CC)
LD      := $(CC)
AR      = ar

LLVM_PROFDATA = llvm-profdata

PROF_GEN = -fprofile-generate=$(PROF_DATA)
PROF_USE = -fprofile-use=$(PROF_DATA)

CFLAGS  := -std=c++11 -Wall -Wextra -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(CFLAGS)
ifndef DEBUG
# use LTO only if no PGO
ifdef PGO
LTO :=
else
LTO := -flto
endif
OPT     = -O3 $(LTO) -fno-rtti -DNDEBUG
LDFLAGS := $(LDFLAGS) $(OPT)
ifneq ("darwin","$(findstring darwin,$(OSTYPE))")
# "Gold" not supported on darwin
LDFLAGS := $(LDFLAGS) -fuse-ld=gold
endif
endif

else

CC ?= g++
CPP     := $(CC)
LD      := $(CC)
AR      = ar

PROF_GEN = -fprofile-generate -fprofile-dir=$(PROF_DATA)
PROF_USE = -fprofile-use -fprofile-dir=$(PROF_DATA)

GCC_MIN_VERSION = 4.9
GCCVER := $(shell expr `$(CC) -dumpversion`)
GCCNEW := $(shell awk 'BEGIN{print $(GCCVER) >= 4.9}')

ifeq "$(GCCNEW)" "1"
# options for GCC
CFLAGS := -std=c++11 -Wall -Wextra -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(CFLAGS)
SMPFLAGS := $(SMPFLAGS) -pthread
LDFLAGS := $(LDFLAGS) -fuse-ld=gold -L$(HWLOC)/lib
ifndef DEBUG
LTO	= -flto
OPT	= -Ofast $(LTO) -fno-rtti -fno-enforce-eh-specs -DNDEBUG
endif
else
$(error GCC version is $(GCCVER): versions < $(GCC_MIN_VERSION) not supported)
endif

endif
endif

ifeq ("darwin","$(findstring darwin,$(OSTYPE))")
CFLAGS := -D_MAC $(CFLAGS)
LDFLAGS := $(LDFLAGS)
LLVM_PROFDATA := xcrun $(LLVM_PROFDATA)
else
LIBS := $(LIBS) -lrt
LLVM_PROFDATA := $(LLVM_PROFDATA)
endif

default: dirs $(EXPORT)/$(ARASANX)

tuning: dirs $(EXPORT)/$(TUNER)

tuning-popcnt: dirs
	@$(MAKE) TUNER=$(TUNER)-popcnt CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)' tuning

utils: dirs $(EXPORT)/pgnselect $(EXPORT)/playchess $(EXPORT)/makebook $(EXPORT)/makeeco $(EXPORT)/ecocoder

clean: dirs
	rm -f $(BUILD)/*.o
	rm -f $(TUNE_BUILD)/*.o
	rm -f $(PROFILE)/*.o
	rm -f $(PROFILE)/*.gcda
	rm -f $(PROFILE)/*.gcno
	rm -f $(PROF_DATA)/*.dyn $(PROF_DATA)/*.profraw $(PROF_DATA)/*.profdata
	cd $(EXPORT) && rm -f arasanx* tuner* makeeco makebook playchess pgnselect ecocoder

dirs:
	mkdir -p $(BUILD)
	mkdir -p $(TUNE_BUILD)
	mkdir -p $(EXPORT)
	mkdir -p $(PROFILE)
	mkdir -p $(PROF_DATA)

release:
	mkdir -p ../release
	cd .. && $(GIT_ARCHIVE_ALL_PATH)/git_archive_all.py --prefix arasan-$(VERSION)/ release/arasan-$(VERSION).tar
	gzip ../release/arasan-$(VERSION).tar

install: all
	mkdir -p $(INSTALL)/arasan-$(VERSION)
	cp $(EXPORT)/$(ARASANX) $(INSTALL)/arasan-$(VERSION)
	chmod 755 $(INSTALL)/arasan-$(VERSION)/$(ARASANX)
	ln -s $(INSTALL)/arasan-$(VERSION)/$(ARASANX) $(INSTALL)/arasan-$(VERSION)/arasanx
	cp $(SRC)/arasan.rc $(INSTALL)/arasan-$(VERSION)
	chmod u+s $(INSTALL)/arasan-$(VERSION)/$(ARASANX)

$(BUILD)/%.o: %.cpp
	$(CPP) $(OPT) $(TRACE) $(CFLAGS) $(SMPFLAGS) $(DEBUG) -c -o $@ $<

$(BUILD)/%.o: $(UTIL)/%.cpp
	$(CPP) $(OPT) $(TRACE) $(CFLAGS) $(SMPFLAGS) $(DEBUG) -c -o $@ $<

$(TUNE_BUILD)/%.o: %.cpp
	$(CPP) $(OPT) $(TRACE) $(CFLAGS) $(SMPFLAGS) $(TUNE_FLAGS) $(DEBUG) -c -o $@ $<

$(PROFILE)/%.o: %.cpp
ifeq ($(PASS),1)
	$(CPP) $(PROF_GEN) $(OPT) $(TRACE) $(CFLAGS) $(SMPFLAGS) $(DEBUG) -c -o $@ $<
else
	$(CPP) $(PROF_USE) $(OPT) $(LTO) $(TRACE) $(CFLAGS) $(SMPFLAGS) $(DEBUG) -c -o $@ $<
endif

profiled: dirs
	@$(MAKE) PASS=1 profile
	@$(MAKE) PASS=1 profile-run
ifeq ("$(findstring clang,$(CC))","clang")
	$(LLVM_PROFDATA) merge -output=$(PROF_DATA)/default.profdata $(PROF_DATA)/default_*profraw
endif
	@$(MAKE) PASS=2 profile

popcnt-profiled: dirs
	@$(MAKE) PASS=1 BUILD_TYPE=popcnt PGO=1 CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)' profile
	@$(MAKE) PASS=1 BUILD_TYPE=popcnt PGO=1 CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)' profile-run
ifeq ("$(findstring clang,$(CC))","clang")
	$(LLVM_PROFDATA) merge -output=$(PROF_DATA)/default.profdata $(PROF_DATA)/default_*profraw
endif
	@$(MAKE) PASS=2 BUILD_TYPE=popcnt PGO=1 CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)' profile

popcnt: dirs
	@$(MAKE) BUILD_TYPE=popcnt CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)'

bmi2: dirs
	@$(MAKE) ARASANX=$(ARASANX)-bmi2 CFLAGS='$(CFLAGS) $(BMI2_FLAGS)'

avx2: dirs
	@$(MAKE) ARASANX=$(ARASANX)-avx2 CFLAGS='$(CFLAGS) $(AVX2_FLAGS)'

bmi2-profiled: dirs
	@$(MAKE) PASS=1 BUILD_TYPE=bmi2 PGO=1 CFLAGS='$(CFLAGS) $(BMI2_FLAGS)' profile
	@$(MAKE) PASS=1 BUILD_TYPE=bmi2 PGO=1 CFLAGS='$(CFLAGS) $(BMI2_FLAGS)' profile-run
ifeq ("$(findstring clang,$(CC))","clang")
	$(LLVM_PROFDATA) merge -output=$(PROF_DATA)/default.profdata $(PROF_DATA)/default_*profraw
endif
	@$(MAKE) PASS=2 BUILD_TYPE=bmi2 PGO=1 CFLAGS='$(CFLAGS) $(BMI2_FLAGS)' profile

avx2-profiled: dirs
	@$(MAKE) PASS=1 BUILD_TYPE=avx2 PGO=1 CFLAGS='$(CFLAGS) $(AVX2_FLAGS)' profile
	@$(MAKE) PASS=1 BUILD_TYPE=avx2 PGO=1 CFLAGS='$(CFLAGS) $(AVX2_FLAGS)' profile-run
ifeq ("$(findstring clang,$(CC))","clang")
	$(LLVM_PROFDATA) merge -output=$(PROF_DATA)/default.profdata $(PROF_DATA)/default_*profraw
endif
	@$(MAKE) PASS=2 BUILD_TYPE=avx2 PGO=1 CFLAGS='$(CFLAGS) $(AVX2_FLAGS)' profile

TB_SOURCES=
TB_OBJS=
TB_LIBS=

ifdef SYZYGY_TBS
TB_SOURCES := $(TB_SOURCES) syzygy.cpp $(STB)/tbprobe.c
TB_TUNE_OBJS := $(TB_OBJS) $(TUNE_BUILD)/syzygy.o $(TUNE_BUILD)/tbprobe.o
TB_OBJS := $(TB_OBJS) $(BUILD)/syzygy.o $(BUILD)/tbprobe.o
STB_FLAGS := -x c++ $(CFLAGS)
$(BUILD)/%.o: $(STB)/%.c
	$(CC) $(STB_FLAGS) $(OPT) $(DEBUG) -c $< -o $@


$(TUNE_BUILD)/%.o: $(STB)/%.c
	$(CC) $(STB_FLAGS) $(TUNE_FLAGS) $(OPT) $(DEBUG) -c $< -o $@
endif

ifeq ("$(findstring UNIT_TESTS,$(DEBUG))","UNIT_TESTS")
UNIT_TEST_SRC:=unit.cpp
endif
ifeq ("$(findstring UNIT_TESTS,$(CFLAGS))","UNIT_TESTS")
UNIT_TEST_SRC:=unit.cpp
endif

ARASANX_SOURCES = arasanx.cpp tester.cpp bench.cpp protocol.cpp \
globals.cpp board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp calctime.cpp eco.cpp ecodata.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp $(UNIT_TEST_SRC)

MAKEBOOK_SOURCES = makebook.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp calctime.cpp eco.cpp ecodata.cpp \
legal.cpp stats.cpp threadp.cpp threadc.cpp protocol.cpp \
tester.cpp bench.cpp

MAKEECO_SOURCES = makeeco.cpp globals.cpp board.cpp boardio.cpp \
material.cpp chess.cpp attacks.cpp bitboard.cpp chessio.cpp \
epdrec.cpp bhash.cpp params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp log.cpp search.cpp searchc.cpp \
learn.cpp movegen.cpp hash.cpp legal.cpp stats.cpp \
calctime.cpp eco.cpp ecodata.cpp threadp.cpp threadc.cpp \
protocol.cpp tester.cpp bench.cpp

ECOCODER_SOURCES = ecocoder.cpp globals.cpp board.cpp \
boardio.cpp material.cpp chess.cpp attacks.cpp bitboard.cpp \
chessio.cpp epdrec.cpp bhash.cpp params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp bookread.cpp \
bookwrit.cpp log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp legal.cpp eco.cpp ecodata.cpp \
stats.cpp calctime.cpp threadp.cpp threadc.cpp protocol.cpp \
tester.cpp bench.cpp

TUNER_SOURCES = tuner.cpp tune.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
vparams.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp log.cpp search.cpp \
searchc.cpp learn.cpp movegen.cpp \
hash.cpp calctime.cpp eco.cpp ecodata.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp protocol.cpp \
tester.cpp bench.cpp

PGNSELECT_SOURCES = pgnselect.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp calctime.cpp eco.cpp ecodata.cpp \
legal.cpp stats.cpp threadp.cpp threadc.cpp protocol.cpp \
tester.cpp bench.cpp

PLAYCHESS_SOURCES = playchess.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp calctime.cpp eco.cpp ecodata.cpp \
legal.cpp stats.cpp threadp.cpp threadc.cpp protocol.cpp \
tester.cpp bench.cpp

ARASANX_PROFILE_OBJS = $(patsubst %.cpp, $(PROFILE)/%.o, $(ARASANX_SOURCES)) $(ASM_PROFILE_OBJS) $(TB_OBJS) $(NUMA_PROFILE_OBJS) $(TB_LIBS)
ARASANX_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(ARASANX_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
TUNER_OBJS    = $(patsubst %.cpp, $(TUNE_BUILD)/%.o, $(TUNER_SOURCES)) $(TB_TUNE_OBJS) $(NUMA_TUNE_OBJS) $(TB_LIBS)
MAKEBOOK_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(MAKEBOOK_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
MAKEECO_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(MAKEECO_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
ECOCODER_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(ECOCODER_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
PGNSELECT_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(PGNSELECT_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
PLAYCHESS_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(PLAYCHESS_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)

$(EXPORT)/makebook:  $(MAKEBOOK_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(MAKEBOOK_OBJS) $(DEBUG) -o $(EXPORT)/makebook -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/makeeco:  $(MAKEECO_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(MAKEECO_OBJS) $(DEBUG) -o $(EXPORT)/makeeco -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/ecocoder:  $(ECOCODER_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(ECOCODER_OBJS) $(DEBUG) -o $(EXPORT)/ecocoder -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/pgnselect:  $(PGNSELECT_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(PGNSELECT_OBJS) $(DEBUG) -o $(EXPORT)/pgnselect -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/playchess:  $(PLAYCHESS_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(OPT) $(PLAYCHESS_OBJS) $(DEBUG) -o $(EXPORT)/playchess -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/$(TUNER):  $(TUNER_OBJS)
	cd $(TUNE_BUILD) && $(LD) $(LDFLAGS) $(OPT) $(TUNER_OBJS) $(DEBUG) -o $(EXPORT)/$(TUNER) -lstdc++ $(LIBS) $(SMPLIB)

ifeq ($(PASS),1)
profile: dirs $(PROFILE)/arasanx
else
profile: dirs $(EXPORT)/$(ARASANX)
endif

ifeq ($(PASS),1)
$(PROFILE)/arasanx:  $(ARASANX_PROFILE_OBJS)
	cd $(PROFILE) && $(LD) $(PROF_GEN) $(ARASANX_PROFILE_OBJS) $(DEBUG) -o $(PROFILE)/arasanx -lstdc++ $(LIBS) $(SMPLIB)
else
ifeq ($(PASS),2)
$(EXPORT)/$(ARASANX):  $(ARASANX_PROFILE_OBJS)
	cd $(PROFILE) && $(LD) $(PROF_USE) $(OPT) $(LTO) $(ARASANX_PROFILE_OBJS) $(DEBUG) -o $(PROFILE)/arasanx -lstdc++ $(LIBS) $(SMPLIB)
	cp $(PROFILE)/arasanx $(EXPORT)/$(ARASANX)
else
# non-PGO build
$(EXPORT)/$(ARASANX):  $(ARASANX_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(LTO) $(ARASANX_OBJS) $(DEBUG) -o $(EXPORT)/$(ARASANX) -lstdc++ $(LIBS) $(SMPLIB)
endif
endif

profile-run:
	$(PROFILE)/arasanx $(PGO_RUN_FLAGS) bench
	rm $(PROFILE)/*.o
	rm -f $(PROFILE)/arasanx $(EXPORT)/arasanx

.PHONY: all clean dirs profile bmi2 profile-run install release

.EXPORT_ALL_VARIABLES:

