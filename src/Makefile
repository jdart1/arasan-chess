#
# Gnu Makefile for Arasan 18.0
#
VERSION = 18.0.0

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
# location of the Gaviota tablebase source code (if used)
GTB=michiguel-Gaviota-Tablebases-161d6cb
# location of the Nalimov tablebase source code (if used)
TB=tb
#
LIBCMAES_INCLUDE=/usr/local/include/libcmaes
LIBCMAES_PATH=/usr/local/lib
EIGEN3_INCLUDE=/home/jdart/eigen-eigen-10219c95fe65/Eigen
EIGEN3_UNSUPPORTED=/home/jdart/eigen-eigen-10219c95fe65/unsupported/Eigen

TUNE_INCLUDES := -I$(LIBCMAES_INCLUDE) -I$(EIGEN3_INCLUDE) -I$(EIGEN3_UNSUPPORTED)

TUNE_LDFLAGS := -L$(LIBCMAES_PATH)

TUNE_LIBS = -lcmaes

#define the appropriate macro for the type(s) of tablebase supported
GAVIOTA_TBS = 1
NALIMOV_TBS = 1

ifdef GAVIOTA_TBS
CFLAGS := $(CFLAGS) -DGAVIOTA_TBS -I$(GTB)
endif
ifdef NALIMOV_TBS 
CFLAGS := $(CFLAGS) -DNALIMOV_TBS -I. -I$(TB)
endif

# Build the GTB sources into a separate object dir.
GTB_BUILD	= ../build/gtb
# GTB is built as a separate library due to problems with PGO
GTB_LIBNAME     = libgtb.a

# SMP flags (note: we do not support a non-SMP build anymore)
SMPFLAGS = -DSMP -DSMP_STATS
SMPLIB   = -lpthread

PGO_RUN_FLAGS = -H 64M

TUNE_CFLAGS := -ansi -std=c++11 -DTUNE
TUNE_OPT = -DUSE_INTRINSICS -O2

#PROF     = -pg
#DEBUG   = -g -D_DEBUG=1 -DUNIT_TESTS
#DEBUG = -g
#TRACE    = -D_TRACE=1 -D_THREAD_TRACE

shell        := $(SHELL)
UNAME        := $(shell uname -s|awk '{print tolower($$0)}')
ifndef OSTYPE
OSTYPE       := $(UNAME)
endif
ARCH         := $(shell uname -m)

ifeq ($(ARCH),"x86_64")
SSE=-msse3
endif

ifeq ("mingw","$(findstring mingw,$(UNAME))")
# MingW
LIBS = -lmsvcrt -lwinmm
SMPLIB := -mthreads $(SMPLIB)
#set MINGW for gtb code
GTB_FLAGS := $(GTB_FLAGS) -DMINGW
# do not rely on getconf
ifeq ("mingw32","$(findstring mingw32,$(UNAME))")
LBITS = 32
else
LBITS = 64
endif
else
LBITS := $(shell getconf LONG_BIT)
LIBS = -lc -lm
endif

ifeq ($(LBITS),64)
   CFLAGS := $(CFLAGS) -D_64BIT
endif
ARASANX := arasanx-$(LBITS)

ifeq ("$(CC)","icc")
# Intel C++ compiler
CPP     = icc
LD      = icc
AR      = xiar
CFLAGS  := -no-vec -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(CFLAGS)
OPT     = -O3 -fno-rtti -finline-functions $(SSE)
GTB_OPT := -O3 -finline-functions $(SSE) -ipo

PROF_GEN = -prof-gen -prof-dir $(PROF_DATA)
PROF_USE = -prof-use -ip -prof-dir $(PROF_DATA)

else 

ifeq ("$(CC)","clang")

CPP     := $(CC)
LD      := $(CC)
AR      = ar

PROF_GEN = -fprofile-generate -fprofile-dir=$(PROF_DATA)
PROF_USE = -fprofile-use -fprofile-dir=$(PROF_DATA)

CFLAGS  := -Wall -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(CFLAGS) $(SSE)
OPT     = -O3 -fno-rtti -finline-functions $(SSE)
GTB_OPT := -O3 -finline-functions $(SSE)

else

CC ?= g++
CPP     := $(CC)
LD      := $(CC)
AR      = ar

PROF_GEN = -fprofile-generate -fprofile-dir=$(PROF_DATA)
PROF_USE = -fprofile-use -fprofile-dir=$(PROF_DATA)

GCCOLD  := $(shell expr `gcc -dumpversion | sed -e "s/\.//"` \< 40)
GCCNEW  := $(shell expr `gcc -dumpversion | sed -e "s/\.//"` \>= 46)

ifeq "$(GCCNEW)" "1"
# options for GCC 4.6 or higher:
CFLAGS  := -Wall -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(CFLAGS)
OPT	 = -Ofast -fno-rtti -fno-enforce-eh-specs -finline-functions $(SSE)
GTB_OPT := -Ofast -finline-functions $(SSE)
else
ifeq "$(GCCOLD)" "0"
# pre-4.6 gcc options:
CFLAGS  := -Wall -fno-strict-aliasing -DUSE_INTRINSICS -DUSE_ASM $(CFLAGS)
OPT	 = -O3 -fno-rtti -fno-enforce-eh-specs -finline-functions $(SSE)
GTB_OPT := -O3 -finline-functions $(SSE)
else
# really old gcc
CFLAGS  := -DUSE_INTRINSICS -DUSE_ASM $(CFLAGS)
OPT	 = -O2
endif
endif

endif
endif

ifeq ("darwin","$(findstring darwin,$(OSTYPE))")
CFLAGS := -D_MAC $(CFLAGS) 
LDFLAGS := $(LDFLAGS) 
else
LIBS := $(LIBS) -lrt
endif

all: dirs $(EXPORT)/$(ARASANX) $(EXPORT)/makebook $(EXPORT)/makeeco $(EXPORT)/tuner $(EXPORT)/testpos $(EXPORT)/$(ARASANX)-tune

popcnt: SSE = -msse4.2
popcnt: CFLAGS := $(CFLAGS) -DUSE_POPCNT
popcnt: ARASANX := $(ARASANX)-popcnt
popcnt: all

# Solaris target: note only GCC is supported
sparc-solaris: 
	@$(MAKE) CC=g++ OPT='$(OPT)' SSE= LIBS='$(LIBS)' CFLAGS='$(CFLAGS)' all

# Arm target on Arm host with GCC
arm-linux:
	@$(MAKE) CC=g++ OPT='$(OPT)' SSE= CFLAGS='$(CFLAGS)' all
 
clean: dirs
	rm -f $(BUILD)/*.o
	rm -f $(TUNE_BUILD)/*.o
ifdef GAVIOTA_TBS
	rm -f $(GTB_BUILD)/*.o
	rm -f $(GTB_BUILD)/*.a
endif
	rm -f $(PROFILE)/*.o
	rm -f $(PROFILE)/*.gcda
	rm -f $(PROFILE)/*.gcno
	rm -f $(PROF_DATA)/*.dyn
	cd $(EXPORT) && rm -f arasanx* makeeco makebook

dirs:
	mkdir -p $(BUILD)
	mkdir -p $(TUNE_BUILD)
ifdef GAVIOTA_TBS
	mkdir -p $(GTB_BUILD)
endif
	mkdir -p $(EXPORT)
	mkdir -p $(PROFILE)
	mkdir -p $(PROF_DATA)

#book:	$(EXPORT)/makebook
#	$(EXPORT)/makebook -n 10 -o $(EXPORT)/book.bin $(BOOK)/basic.bk

release:
	mkdir -p ../release
	cd .. && git archive --format tar --prefix arasan-$(VERSION)/ -o release/arasan-$(VERSION).tar v$(VERSION)
	gzip ../release/arasan-$(VERSION).tar

install: all
	mkdir -p $(INSTALL)/arasan-$(VERSION) 
	cp $(EXPORT)/$(ARASANX) $(INSTALL)/arasan-$(VERSION)
	chmod 755 $(INSTALL)/arasan-$(VERSION)/$(ARASANX)
	ln -s $(INSTALL)/arasan-$(VERSION)/$(ARASANX) $(INSTALL)/arasan-$(VERSION)/arasanx
	cp $(SRC)/arasan.rc $(INSTALL)/arasan-$(VERSION)
	cp $(BOOK)/book.bin $(INSTALL)/arasan-$(VERSION)
	chmod 644 $(INSTALL)/arasan-$(VERSION)/book.bin
	chmod u+s $(INSTALL)/arasan-$(VERSION)/$(ARASANX)

$(BUILD)/%.o: %.cpp
	$(CPP) $(OPT) $(PROF) $(TRACE) $(CFLAGS) $(SMPFLAGS) $(DEBUG) -c -o $@ $<

$(TUNE_BUILD)/%.o: %.cpp
	$(CPP) $(OPT) $(PROF) $(TRACE) $(TUNE_CFLAGS) $(TUNE_OPT) $(SMP_FLAGS) $(TUNE_INCLUDES) $(DEBUG) -c -o $@ $<

$(PROFILE)/%.o: %.cpp
ifeq ($(PASS),1)
	$(CPP) $(PROF_GEN) $(OPT) $(TRACE) $(CFLAGS) $(SMPFLAGS) $(DEBUG) -c -o $@ $<
else
	$(CPP) $(PROF_USE) $(OPT) $(TRACE) $(CFLAGS) $(SMPFLAGS) $(DEBUG) -c -o $@ $<
endif

profiled:
	@$(MAKE) PASS=1 profile profile-run
	@$(MAKE) PASS=2 profile

popcnt-profiled:
	@$(MAKE) PASS=1 ARASANX=$(ARASANX)-popcnt CFLAGS='$(CFLAGS) -DUSE_POPCNT' SSE=-msse4.2 profile profile-run
	@$(MAKE) PASS=2 ARASANX=$(ARASANX)-popcnt CFLAGS='$(CFLAGS) -DUSE_POPCNT' SSE=-msse4.2 profile

# sources for tablebase support.
ifdef GAVIOTA_TBS
# all sources are under $(GTB), we will use vpath (see below) to find them
GTB_SOURCES := gtb-probe.c gtb-dec.c gtb-att.c sysport.c \
	wrap.c hzip.c LzmaEnc.c LzmaDec.c Alloc.c LzFind.c \
	Lzma86Enc.c Lzma86Dec.c Bra86.c zcompress.c \
	uncompr.c inflate.c deflate.c adler32.c \
	crc32.c infback.c inffast.c inftrees.c \
	trees.c zutil.c lzf_c.c lzf_d.c

GTB_OBJS:=$(GTB_SOURCES:%.c=$(GTB_BUILD)/%.o)

GTB_FLAGS := -x c -DZ_PREFIX -DNDEBUG $(SMP) -I$(GTB)/sysport -I$(GTB)/compression -I$(GTB)/compression/huffman -I$(GTB)/compression/lzma -I$(GTB)/compression/zlib -I$(GTB)/compression/liblzf $(GTB_FLAGS)

TB_SOURCES := gtb.cpp
TB_OBJS := $(BUILD)/gtb.o
TB_LIBS := $(GTB_BUILD)/$(GTB_LIBNAME)

vpath %.c $(GTB) $(GTB)/sysport $(GTB)/compression $(GTB)/compression/zlib $(GTB)/compression/huffman $(GTB)/compression/lzma $(GTB)/compression/liblzf

#rule for building GTB (c code)
$(GTB_BUILD)/%.o: %.c
	$(CC) $(GTB_FLAGS) $(GTB_OPT) $(DEBUG) -c $< -o $@

$(TB_LIBS): $(GTB_OBJS)
	$(AR) $(GTB_AR_OPT) rcs $(GTB_BUILD)/$(GTB_LIBNAME) $(GTB_OBJS)

endif

ifdef NALIMOV_TBS 
TB_SOURCES := $(TB_SOURCES) $(TB)/tbprobe.cpp
TB_OBJS := $(TB_OBJS) $(BUILD)/tbprobe.o
endif

ARASANX_SOURCES = arasanx.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp history.cpp \
hash.cpp calctime.cpp eco.cpp ecodata.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp unit.cpp

MAKEBOOK_SOURCES = makebook.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp \
scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp history.cpp \
movegen.cpp hash.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp

MAKEECO_SOURCES = makeeco.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp \
scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp history.cpp \
movegen.cpp hash.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp

TUNER_SOURCES = tune.cpp tuner.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp history.cpp \
hash.cpp calctime.cpp eco.cpp ecodata.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp unit.cpp

ARASANX_TUNE_SOURCES = arasanx.cpp tune.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp history.cpp \
hash.cpp calctime.cpp eco.cpp ecodata.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp unit.cpp

TESTPOS_SOURCES = testpos.cpp globals.cpp  \
board.cpp boardio.cpp material.cpp \
chess.cpp attacks.cpp \
bitboard.cpp chessio.cpp epdrec.cpp bhash.cpp  \
scoring.cpp see.cpp \
movearr.cpp notation.cpp options.cpp bitprobe.cpp \
bookread.cpp bookwrit.cpp \
log.cpp search.cpp searchc.cpp learn.cpp \
movegen.cpp history.cpp \
hash.cpp calctime.cpp eco.cpp ecodata.cpp legal.cpp \
stats.cpp threadp.cpp threadc.cpp unit.cpp

ARASANX_PROFILE_OBJS = $(patsubst %.cpp, $(PROFILE)/%.o, $(ARASANX_SOURCES)) $(ASM_PROFILE_OBJS) $(TB_OBJS) $(TB_LIBS)
ARASANX_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(ARASANX_SOURCES)) $(TB_OBJS) $(TB_LIBS)
TUNER_OBJS    = $(patsubst %.cpp, $(TUNE_BUILD)/%.o, $(TUNER_SOURCES))
ARASANX_TUNE_OBJS    = $(patsubst %.cpp, $(TUNE_BUILD)/%.o, $(ARASANX_TUNE_SOURCES))
TESTPOS_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(TESTPOS_SOURCES)) $(TB_OBJS) $(TB_LIBS)
MAKEBOOK_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(MAKEBOOK_SOURCES)) $(TB_OBJS) $(TB_LIBS)
MAKEECO_OBJS    = $(patsubst %.cpp, $(BUILD)/%.o, $(MAKEECO_SOURCES)) $(TB_OBJS) $(TB_LIBS)

$(EXPORT)/makebook:  $(MAKEBOOK_OBJS) 
	cd $(BUILD) && $(LD) $(PROF) $(LDFLAGS) $(MAKEBOOK_OBJS) $(DEBUG) -o $(EXPORT)/makebook -lstdc++ $(LIBS) $(SMPLIB)	

$(EXPORT)/makeeco:  $(MAKEECO_OBJS) 
	cd $(BUILD) && $(LD) $(PROF) $(LDFLAGS) $(MAKEECO_OBJS) $(DEBUG) -o $(EXPORT)/makeeco -lstdc++ $(LIBS) $(SMPLIB)	

$(EXPORT)/tuner:  $(TUNER_OBJS) 
	cd $(TUNE_BUILD) && $(LD) $(PROF) $(LDFLAGS) $(TUNE_LDFLAGS) $(TUNER_OBJS) $(DEBUG) -o $(EXPORT)/tuner -lstdc++ $(LIBS) $(TUNE_LIBS) $(SMPLIB)

$(EXPORT)/$(ARASANX)-tune:  $(ARASANX_TUNE_OBJS) 
	cd $(TUNE_BUILD) && $(LD) $(PROF) $(LDFLAGS) $(TUNE_LDFLAGS) $(ARASANX_TUNE_OBJS) $(DEBUG) -o $(EXPORT)/$(ARASANX)-tune -lstdc++ $(LIBS) $(TUNE_LIBS) $(SMPLIB)

$(EXPORT)/testpos:  $(TESTPOS_OBJS) 
	cd $(BUILD) && $(LD) $(PROF) $(LDFLAGS) $(TESTPOS_OBJS) $(DEBUG) -o $(EXPORT)/testpos -lstdc++ $(LIBS) $(SMPLIB)	

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
	cd $(PROFILE) && $(LD) $(PROF_USE) $(ARASANX_PROFILE_OBJS) $(DEBUG) -o $(PROFILE)/arasanx -lstdc++ $(LIBS) $(SMPLIB)
	cp $(PROFILE)/arasanx $(EXPORT)/$(ARASANX)
else
# non-PGO build
$(EXPORT)/$(ARASANX):  $(ARASANX_OBJS)
	cd $(BUILD) && $(LD) $(PROF) $(LDFLAGS) $(ARASANX_OBJS) $(DEBUG) -o $(EXPORT)/$(ARASANX) -lstdc++ $(LIBS) $(SMPLIB)
endif
endif

profile-run:
	$(PROFILE)/arasanx $(PGO_RUN_FLAGS) <../tests/prof
	rm $(PROFILE)/*.o
	rm -f $(PROFILE)/arasanx $(EXPORT)/arasanx

run-tests:
	$(EXPORT)/$(ARASANX) -H 256m -c 2 <$(TESTS)/tests >$(TESTS)/test-results

score-tests:
	date >$(TESTS)/test-summary
	perl $(TESTS)/tests.pl $(TESTS)/test-results >>$(TESTS)/test-summary

.PHONY: all clean dirs profile profile-run install release run-tests score-tests
