#
# Gnu Makefile for Arasan 21.2
# Copyright 2004-2018 by Jon Dart. All Rights Reserved.
#
VERSION = 21.2

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
# location of hwloc library (required if compiled with NUMA support)
HWLOC64=/home/jdart/chess/hwloc-2.0.2
# directory for utility programs
UTIL = util

# Path to git_archive_all (https://github.com/Kentzo/git-archive-all)
GIT_ARCHIVE_ALL_PATH ?= /home/jdart/bin/git-archive-all

# set to enable compliation with Syzygy tablebase support
SYZYGY_TBS = 1

CFLAGS = -I.
ifdef NUMA
CFLAGS := $(CFLAGS) -DNUMA
endif
ifdef SYZYGY_TBS
CFLAGS := $(CFLAGS) -DSYZYGY_TBS
endif

# SMP flags (note: we do not support a non-SMP build anymore)
SMPFLAGS = -DSMP -DSMP_STATS
SMPLIB   = -lpthread

POPCNT_FLAGS := -DUSE_POPCNT -msse4.2
BMI2_FLAGS := $(POPCNT_FLAGS) -DBMI2

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
SSE=-msse3
endif

LBITS := $(shell getconf LONG_BIT)
LIBS := -lc -lm

ifdef NUMA
CFLAGS := $(CFLAGS) -I$(HWLOC64)/include
LIBS := -lhwloc $(LIBS)
endif

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

TUNER := tuner

ifeq ("$(CC)","icc")
# Intel C++ compiler
CPP     = icc
LD      = icc
AR      = xiar
CFLAGS  := $(CFLAGS) -std=c++11 -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM
ifndef DEBUG
OPT     = -O3 -fno-rtti $(SSE) -DNDEBUG
GTB_OPT := -O3 -fno-rtti $(SSE) -DNDEBUG
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

CFLAGS  := -std=c++11 -Wall -Wextra -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(SSE) $(CFLAGS)
BMI2_FLAGS := $(BMI2_FLAGS) -mbmi2
ifndef DEBUG
OPT     = -O3 -fno-rtti  $(SSE) -DNDEBUG
GTB_OPT := -O3 $(SSE) -DNDEBUG
endif

else

CC ?= g++
CPP     := $(CC)
LD      := $(CC)
AR      = ar

PROF_GEN = -fprofile-generate -fprofile-dir=$(PROF_DATA)
PROF_USE = -fprofile-use -fprofile-dir=$(PROF_DATA)

GCC_MIN_VERSION = 4.9
GCCNEW := $(shell expr `$(CC) -dumpversion` \>= $(GCC_MIN_VERSION))

ifeq "$(GCCNEW)" "1"
# options for GCC
CFLAGS := -std=c++11 -Wall -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(CFLAGS)
BMI2_FLAGS := $(BMI2_FLAGS) -mbmi2
ifndef DEBUG
OPT	= -Ofast -fno-rtti -fno-enforce-eh-specs $(SSE) -DNDEBUG
LTO	= -flto
GTB_OPT := -Ofast $(SSE) -DNDEBUG
endif
else
$(error GCC version < $(GCC_MIN_VERSION) not supported)
endif

endif
endif

ifeq ("darwin","$(findstring darwin,$(OSTYPE))")
CFLAGS := -D_MAC $(CFLAGS)
LDFLAGS := $(LDFLAGS)
LLVM-PROFDATA := xcrun $(LLVM_PROFDATA)
else
LIBS := $(LIBS) -lrt
endif

default: dirs $(EXPORT)/$(ARASANX)

tuning: dirs $(EXPORT)/$(TUNER)

tuning-popcnt: dirs
	@$(MAKE) TUNER=$(TUNER)-popcnt CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)' SSE=-msse4.2 tuning

utils: dirs $(EXPORT)/pgnselect $(EXPORT)/playchess $(EXPORT)/makebook $(EXPORT)/makeeco $(EXPORT)/ecocoder

# Solaris target: note only GCC is supported
sparc-solaris:
	@$(MAKE) CC=g++ OPT='$(OPT)' SSE= LIBS='$(LIBS)' CFLAGS='$(CFLAGS)' all

# Arm target on Arm host with GCC
arm-linux:
	@$(MAKE) CC=g++ OPT='$(OPT)' SSE= CFLAGS='$(CFLAGS)' all

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
ifeq ($(CC),"clang")
	$(LLVM_PROFDATA) merge -output=$(PROF_DIR)/default.profdata $(PROF_DIR)/default_*profraw
endif
else
	$(CPP) $(PROF_USE) $(OPT) $(LTO) $(TRACE) $(CFLAGS) $(SMPFLAGS) $(DEBUG) -c -o $@ $<
endif

profiled: dirs
	@$(MAKE) PASS=1 profile
	@$(MAKE) PASS=1 profile-run
	@$(MAKE) PASS=2 profile

popcnt-profiled: dirs
	@$(MAKE) PASS=1 BUILD_TYPE=popcnt CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)' SSE=-msse4.2 profile
	@$(MAKE) PASS=1 BUILD_TYPE=popcnt CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)' SSE=-msse4.2 profile-run
	@$(MAKE) PASS=2 BUILD_TYPE=popcnt CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)' SSE=-msse4.2 profile

popcnt: dirs
	@$(MAKE) BUILD_TYPE=popcnt CFLAGS='$(CFLAGS) $(POPCNT_FLAGS)' SSE=-msse4.2

bmi2: dirs
	@$(MAKE) ARASANX=$(ARASANX)-bmi2 CFLAGS='$(CFLAGS) $(BMI2_FLAGS)' SSE=-msse4.2

bmi2-profiled: dirs
	@$(MAKE) PASS=1 BUILD_TYPE=bmi2 CFLAGS='$(CFLAGS) $(BMI2_FLAGS)' SSE=-msse4.2 profile
	@$(MAKE) PASS=1 BUILD_TYPE=bmi2 CFLAGS='$(CFLAGS) $(BMI2_FLAGS)' SSE=-msse4.2 profile-run
	@$(MAKE) PASS=2 BUILD_TYPE=bmi2 CFLAGS='$(CFLAGS) $(BMI2_FLAGS)' SSE=-msse4.2 profile

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

ARASANX_SOURCES = arasanx.cpp tester.cpp protocol.cpp \
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
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp

MAKEECO_SOURCES = makeeco.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp

ECOCODER_SOURCES = ecocoder.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp legal.cpp \
eco.cpp ecodata.cpp \
stats.cpp threadp.cpp threadc.cpp

TUNER_SOURCES = tuner.cpp tune.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
vparams.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp log.cpp search.cpp \
searchc.cpp learn.cpp movegen.cpp \
hash.cpp calctime.cpp eco.cpp ecodata.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp $(UNIT_TEST_SRC)

PGNSELECT_SOURCES = pgnselect.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp calctime.cpp eco.cpp ecodata.cpp \
legal.cpp stats.cpp threadp.cpp threadc.cpp $(UNIT_TEST_SRC)

PLAYCHESS_SOURCES = playchess.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
params.cpp scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp hash.cpp calctime.cpp eco.cpp ecodata.cpp \
legal.cpp stats.cpp threadp.cpp threadc.cpp $(UNIT_TEST_SRC)

ARASANX_PROFILE_OBJS = $(patsubst %.cpp, $(PROFILE)/%.o, $(ARASANX_SOURCES)) $(ASM_PROFILE_OBJS) $(TB_OBJS) $(NUMA_PROFILE_OBJS) $(TB_LIBS)
ARASANX_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(ARASANX_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
TUNER_OBJS    = $(patsubst %.cpp, $(TUNE_BUILD)/%.o, $(TUNER_SOURCES)) $(TB_TUNE_OBJS) $(NUMA_TUNE_OBJS) $(TB_LIBS)
MAKEBOOK_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(MAKEBOOK_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
MAKEECO_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(MAKEECO_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
ECOCODER_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(ECOCODER_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
PGNSELECT_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(PGNSELECT_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)
PLAYCHESS_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(PLAYCHESS_SOURCES)) $(TB_OBJS) $(NUMA_OBJS) $(TB_LIBS)

$(EXPORT)/makebook:  $(MAKEBOOK_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(MAKEBOOK_OBJS) $(DEBUG) -o $(EXPORT)/makebook -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/makeeco:  $(MAKEECO_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(MAKEECO_OBJS) $(DEBUG) -o $(EXPORT)/makeeco -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/ecocoder:  $(ECOCODER_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(ECOCODER_OBJS) $(DEBUG) -o $(EXPORT)/ecocoder -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/pgnselect:  $(PGNSELECT_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(PGNSELECT_OBJS) $(DEBUG) -o $(EXPORT)/pgnselect -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/playchess:  $(PLAYCHESS_OBJS)
	cd $(BUILD) && $(LD) $(LDFLAGS) $(PLAYCHESS_OBJS) $(DEBUG) -o $(EXPORT)/playchess -lstdc++ $(LIBS) $(SMPLIB)

$(EXPORT)/$(TUNER):  $(TUNER_OBJS)
	cd $(TUNE_BUILD) && $(LD) $(LDFLAGS) $(TUNER_OBJS) $(DEBUG) -o $(EXPORT)/$(TUNER) -lstdc++ $(LIBS) $(SMPLIB)

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
	cd $(BUILD) && $(LD) $(LDFLAGS) $(ARASANX_OBJS) $(DEBUG) -o $(EXPORT)/$(ARASANX) -lstdc++ $(LIBS) $(SMPLIB)
endif
endif

profile-run:
	$(PROFILE)/arasanx $(PGO_RUN_FLAGS) <../tests/prof
	rm $(PROFILE)/*.o
	rm -f $(PROFILE)/arasanx $(EXPORT)/arasanx

.PHONY: all clean dirs profile bmi2 profile-run install release

.EXPORT_ALL_VARIABLES:

