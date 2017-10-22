# Arasan Makefile for use with NMAKE and Intel C++ on the Windows platform
# Copyright 2004-2017 by Jon Dart. All Rights Reserved.
#
VERSION = 20.2
#
#TARGET = win32
TARGET = win64

# directory defines - objects
PROFILE = ..\$(TARGET)\profile
PROFDATA = ..\$(TARGET)\profdata

#TRACE=/D_TRACE 
#DEBUG=/Zi -D_DEBUG 
#LDDEBUG=/DEBUG /PDB:arasanx.pdb
BUILD_ROOT=..
UTIL=util

#define the appropriate macro for the type(s) of tablebase supported
#GAVIOTA_TBS=1
#NALIMOV_TBS=1
SYZYGY_TBS=1

# location of the Gaviota tablebase source code
GTB=michiguel-Gaviota-Tablebases-161d6cb

# location of the Nalimov tablebase source code
TB=tb

# location of SYZYGY tablebase code
STB=syzygy

# enable NUMA support
#NUMA=1

# location of hwloc library
HWLOC64=c:\chess\hwloc-win64-build-1.11.3
HWLOC32=c:\chess\hwloc-win32-build-1.11.3

!If "$(TARGET)" == "win64"
!Ifndef ARASANX
ARASANX = arasanx-64
!Endif
ARCH=/D_WIN64
HWLOC=$(HWLOC64)
!Else
!Ifndef ARASANX
ARASANX = arasanx-32
!Endif
HWLOC=$(HWLOC32)
!Endif

!Ifdef DEBUG
BUILD_TYPE=debug
!Else
BUILD_TYPE=release
!Endif

BUILD = $(BUILD_ROOT)\$(TARGET)\$(BUILD_TYPE)
POPCNT_BUILD = $(BUILD_ROOT)\$(TARGET)-popcnt\$(BUILD_TYPE)
BMI2_BUILD = $(BUILD_ROOT)\$(TARGET)-bmi2\$(BUILD_TYPE)
TUNE_BUILD = $(BUILD_ROOT)\$(TARGET)\$(BUILD_TYPE)-tune
PROFILE = $(BUILD_ROOT)\$(TARGET)\profile
PROFDATA = $(BUILD_ROOT)\$(TARGET)\profdata
PGO_BUILD = $(BUILD_ROOT)\$(TARGET)\pgo_build
RELEASE = $(BUILD_ROOT)\release
# relative to root:
SOURCE_ARCHIVE = release\arasan_source$(VERSION).zip

!If "$(PLATFORM)"=="XP"
CFLAGS=$(CFLAGS) /D_USING_V110_SDK71_
!Endif
!IfDef NUMA
CFLAGS=$(CFLAGS) /DNUMA /I$(HWLOC)/include
NUMA_LIBS=$(HWLOC)/lib/libhwloc.lib
NUMA_OBJS=$(BUILD)/topo.obj
NUMA_PROFILE_OBJS=$(PROFILE_BUILD)/topo.obj
NUMA_TUNE_OBJS=$(TUNE_BUILD)/topo.obj
!Endif

TESTS = ..\tests

# SMP flags. Note: we don't support a non-SMP build any more.
!Ifdef DEBUG
SMP=/MTd /DSMP /DSMP_STATS
!Else
SMP=/MT /DSMP /DSMP_STATS
!Endif
PROF_RUN_SMP=-c 2

!Ifdef ARASAN_VERSION
CFLAGS=$(CFLAGS) -DARASAN_VERSION=$(ARASAN_VERSION)
!Else
CFLAGS=$(CFLAGS) -DARASAN_VERSION=$(VERSION)
!Endif

TUNE_FLAGS=-DTUNE

# Intel C++ defs, release build (intel64)
!If "$(TARGET)" == "win64"
INTEL64 = /Wp64
INTEL64LIB = bufferoverflowU.lib
CL       = icl
LD       = xilink
CFLAGS = /Qstd=c++11 /D_CONSOLE /D_CRT_SECURE_NO_WARNINGS $(TRACE) $(SMP) $(DEBUG) $(INTEL64) /EHsc $(CFLAGS)
OPT = /O3 /Ob2 /GR- /DUSE_INTRINSICS /DUSE_ASM /DNDEBUG
!Else
# Intel C++ defs, release build (IA32)
CL       = icl
LD       = xilink
CFLAGS = /Qstd=c++11 /D_CONSOLE /D_CRT_SECURE_NO_WARNINGS $(TRACE) $(SMP) $(DEBUG) $(INTEL64) /EHsc $(CFLAGS)
OPT = /O3 /Ob2 /Oy- /Gr /GR- /DUSE_INTRINSICS /DUSE_ASM /DNDEBUG
!Endif

# Intel C++ defs (profile build)
IPO = /Qipo
PROF_GEN_LD = xilink
PROF_GEN_FLAGS = /Qprof_gen /Qprof_dir$(PROFDATA)
PROF_RUN_FLAGS = -H 64M $(PROF_RUN_SMP)
PROF_USE_FLAGS = /Qprof_use $(IPO) /Qprof_dir$(PROFDATA)

TB_FLAGS = $(CFLAGS) $(SMP) /Gr /O1 /GS- /I$(TB) /I.
TB_PROFILE_FLAGS = $(TB_FLAGS) $(PROF_USE_FLAGS)

POPCNT_FLAGS = -DUSE_POPCNT /QxSSE4.2

BMI2_FLAGS = -DUSE_POPCNT -DBMI2 /QxCORE-AVX2

default: dirs $(BUILD)\$(ARASANX).exe

tuning: dirs $(BUILD)\tuner.exe

utils: $(BUILD)\pgnselect.exe $(BUILD)\playchess.exe $(BUILD)\makebook.exe $(BUILD)\makeeco.exe $(BUILD)\ecocoder.exe

!IfDef NALIMOV_TBS
TB_OBJS = $(BUILD)\nalimov.obj
TB_TUNE_OBJS = $(TUNE_BUILD)\nalimov.obj
TB_PROFILE_OBJS = $(PROFILE)\nalimov.obj
TB_PGO_OBJS = $(PGO_BUILD)\nalimov.obj
CFLAGS = $(CFLAGS) -DNALIMOV_TBS
!Endif
!IfDef GAVIOTA_TBS

GTB_SRCS = $(GTB)\gtb-probe.c $(GTB)\gtb-dec.c $(GTB)\gtb-att.c \
	$(GTB)\sysport\sysport.c \
	$(GTB)\compression\wrap.c $(GTB)\compression\huffman\hzip.c \
	$(GTB)\compression\lzma\LzmaEnc.c $(GTB)\compression\lzma\LzmaDec.c \
	$(GTB)\compression\lzma\Alloc.c $(GTB)\compression\lzma\LzFind.c \
	$(GTB)\compression\lzma\Lzma86Enc.c \
	$(GTB)\compression\lzma\Lzma86Dec.c \
	$(GTB)\compression\lzma\Bra86.c \
	$(GTB)\compression\zlib\zcompress.c \
	$(GTB)\compression\zlib\uncompr.c $(GTB)\compression\zlib\inflate.c \
	$(GTB)\compression\zlib\deflate.c $(GTB)\compression\zlib\adler32.c \
	$(GTB)\compression\zlib\crc32.c $(GTB)\compression\zlib\infback.c \
	$(GTB)\compression\zlib\inffast.c $(GTB)\compression\zlib\inftrees.c \
	$(GTB)\compression\zlib\trees.c $(GTB)\compression\zlib\zutil.c \
	$(GTB)\compression\liblzf\lzf_c.c $(GTB)\compression\liblzf\lzf_d.c

GTB_OBJS = $(BUILD)\gtb-probe.obj $(BUILD)\gtb-dec.obj \
	$(BUILD)\gtb-att.obj \
	$(BUILD)\sysport.obj \
	$(BUILD)\wrap.obj $(BUILD)\hzip.obj \
	$(BUILD)\LzmaEnc.obj $(BUILD)\LzmaDec.obj \
	$(BUILD)\Alloc.obj $(BUILD)\LzFind.obj \
	$(BUILD)\Lzma86Enc.obj \
	$(BUILD)\Lzma86Dec.obj \
	$(BUILD)\Bra86.obj \
	$(BUILD)\zcompress.obj \
	$(BUILD)\uncompr.obj $(BUILD)\inflate.obj \
	$(BUILD)\deflate.obj $(BUILD)\adler32.obj \
	$(BUILD)\crc32.obj $(BUILD)\infback.obj \
	$(BUILD)\inffast.obj $(BUILD)\inftrees.obj \
	$(BUILD)\trees.obj $(BUILD)\zutil.obj \
	$(BUILD)\lzf_c.obj $(BUILD)\lzf_d.obj

GTB_TUNE_OBJS = $(TUNE_BUILD)\gtb-probe.obj $(TUNE_BUILD)\gtb-dec.obj \
	$(TUNE_BUILD)\gtb-att.obj \
	$(TUNE_BUILD)\sysport.obj \
	$(TUNE_BUILD)\wrap.obj $(TUNE_BUILD)\hzip.obj \
	$(TUNE_BUILD)\LzmaEnc.obj $(TUNE_BUILD)\LzmaDec.obj \
	$(TUNE_BUILD)\Alloc.obj $(TUNE_BUILD)\LzFind.obj \
	$(TUNE_BUILD)\Lzma86Enc.obj \
	$(TUNE_BUILD)\Lzma86Dec.obj \
	$(TUNE_BUILD)\Bra86.obj \
	$(TUNE_BUILD)\zcompress.obj \
	$(TUNE_BUILD)\uncompr.obj $(TUNE_BUILD)\inflate.obj \
	$(TUNE_BUILD)\deflate.obj $(TUNE_BUILD)\adler32.obj \
	$(TUNE_BUILD)\crc32.obj $(TUNE_BUILD)\infback.obj \
	$(TUNE_BUILD)\inffast.obj $(TUNE_BUILD)\inftrees.obj \
	$(TUNE_BUILD)\trees.obj $(TUNE_BUILD)\zutil.obj \
	$(TUNE_BUILD)\lzf_c.obj $(TUNE_BUILD)\lzf_d.obj

GTB_PROFILE_OBJS = $(PROFILE)\gtb-probe.obj $(PROFILE)\gtb-dec.obj \
	$(PROFILE)\gtb-att.obj \
	$(PROFILE)\sysport.obj \
	$(PROFILE)\wrap.obj $(PROFILE)\hzip.obj \
	$(PROFILE)\LzmaEnc.obj $(PROFILE)\LzmaDec.obj \
	$(PROFILE)\Alloc.obj $(PROFILE)\LzFind.obj \
	$(PROFILE)\Lzma86Enc.obj \
	$(PROFILE)\Lzma86Dec.obj \
	$(PROFILE)\Bra86.obj \
	$(PROFILE)\zcompress.obj \
	$(PROFILE)\uncompr.obj $(PROFILE)\inflate.obj \
	$(PROFILE)\deflate.obj $(PROFILE)\adler32.obj \
	$(PROFILE)\crc32.obj $(PROFILE)\infback.obj \
	$(PROFILE)\inffast.obj $(PROFILE)\inftrees.obj \
	$(PROFILE)\trees.obj $(PROFILE)\zutil.obj \
	$(PROFILE)\lzf_c.obj $(PROFILE)\lzf_d.obj

GTB_PGO_OBJS = $(PGO_BUILD)\gtb-probe.obj $(PGO_BUILD)\gtb-dec.obj \
	$(PGO_BUILD)\gtb-att.obj \
	$(PGO_BUILD)\sysport.obj \
	$(PGO_BUILD)\wrap.obj $(PGO_BUILD)\hzip.obj \
	$(PGO_BUILD)\LzmaEnc.obj $(PGO_BUILD)\LzmaDec.obj \
	$(PGO_BUILD)\Alloc.obj $(PGO_BUILD)\LzFind.obj \
	$(PGO_BUILD)\Lzma86Enc.obj \
	$(PGO_BUILD)\Lzma86Dec.obj \
	$(PGO_BUILD)\Bra86.obj \
	$(PGO_BUILD)\zcompress.obj \
	$(PGO_BUILD)\uncompr.obj $(PGO_BUILD)\inflate.obj \
	$(PGO_BUILD)\deflate.obj $(PGO_BUILD)\adler32.obj \
	$(PGO_BUILD)\crc32.obj $(PGO_BUILD)\infback.obj \
	$(PGO_BUILD)\inffast.obj $(PGO_BUILD)\inftrees.obj \
	$(PGO_BUILD)\trees.obj $(PGO_BUILD)\zutil.obj \
	$(PGO_BUILD)\lzf_c.obj $(PGO_BUILD)\lzf_d.obj

GTB_FLAGS = $(OPT) /DZ_PREFIX $(SMP) /I$(GTB)\sysport /I$(GTB)\compression /I$(GTB)\compression/huffman /I$(GTB)\compression/lzma /I$(GTB)\compression/zlib /I$(GTB)\compression/liblzf

{$(GTB)}.c{$(BUILD)}.obj:
	$(CL) $(GTB_FLAGS) /Fo$@ -c $<

{$(GTB)\sysport}.c{$(BUILD)}.obj:
	$(CL) $(GTB_FLAGS) /Fo$@ -c $<

{$(GTB)\compression}.c{$(BUILD)}.obj:
	$(CL) $(GTB_FLAGS) /Fo$@ -c $<

{$(GTB)\compression\lzma}.c{$(BUILD)}.obj:
	$(CL) $(GTB_FLAGS) /Fo$@ -c $<

{$(GTB)\compression\zlib}.c{$(BUILD)}.obj:
	$(CL) $(GTB_FLAGS) /Fo$@ -c $<

{$(GTB)\compression\liblzf}.c{$(BUILD)}.obj:
	$(CL) $(GTB_FLAGS) /Fo$@ -c $<

{$(GTB)\compression\huffman}.c{$(BUILD)}.obj:
	$(CL) $(GTB_FLAGS) /Fo$@ -c $<

{$(GTB)}.c{$(PROFILE)}.obj:
	$(CL) $(GTB_FLAGS) $(PROF_GEN_FLAGS) /Fo$@ -c $<

{$(GTB)\sysport}.c{$(PROFILE)}.obj:
	$(CL) $(GTB_FLAGS) $(PROF_GEN_FLAGS) /Fo$@ -c $<

{$(GTB)\compression}.c{$(PROFILE)}.obj:
	$(CL) $(GTB_FLAGS) $(PROF_GEN_FLAGS) /Fo$@ -c $<

{$(GTB)\compression\lzma}.c{$(PROFILE)}.obj:
	$(CL) $(GTB_FLAGS) $(PROF_GEN_FLAGS) /Fo$@ -c $<

{$(GTB)\compression\zlib}.c{$(PROFILE)}.obj:
	$(CL) $(GTB_FLAGS) $(PROF_GEN_FLAGS) /Fo$@ -c $<

{$(GTB)\compression\liblzf}.c{$(PROFILE)}.obj:
	$(CL) $(GTB_FLAGS) $(PROF_GEN_FLAGS) /Fo$@ -c $<

{$(GTB)\compression\huffman}.c{$(PROFILE)}.obj:
	$(CL) $(GTB_FLAGS) $(PROF_GEN_FLAGS) /Fo$@ -c $<

CFLAGS = $(CFLAGS) -DGAVIOTA_TBS -I$(GTB)

TB_OBJS = $(TB_OBJS) $(GTB_OBJS) $(BUILD)\gtb.obj
TB_TUNE_OBJS = $(TB_TUNE_OBJS) $(GTB_TUNE_OBJS) $(TUNE_BUILD)\gtb.obj
TB_PROFILE_OBJS = $(TB_PROFILE_OBJS) $(GTB_PROFILE_OBJS) $(PROFILE)\gtb.obj
TB_PGO_OBJS = $(TB_PGO_OBJS) $(GTB_PGO_OBJS) $(PGO_BUILD)\gtb.obj

GTB_LIBDIR=..\lib\$(TARGET)\$(BUILD_TYPE)
GTB_LIBNAME=gtb.lib

# A library version of the GTB code, mostly for use with the Visual C++ IDE.
# Note: PGO in Visual C++ is not compatible with static libraries.
gtb-lib: $(GTB_OBJS) $(GTB_LIBDIR)
	lib /LTCG /out:$(GTB_LIBDIR)\$(GTB_LIBNAME) $(GTB_OBJS)

$(GTB_LIBDIR):
	md $(GTB_LIBDIR)
!Endif

!IfDef SYZYGY_TBS
CFLAGS = $(CFLAGS) -DSYZYGY_TBS
STB_FLAGS = /TP $(CFLAGS)
TB_OBJS = $(TB_OBJS) $(BUILD)\syzygy.obj $(BUILD)\tbprobe.obj
TB_TUNE_OBJS = $(TB_TUNE_OBJS) $(BUILD_TUNE)\syzygy.obj $(BUILD_TUNE)\tbprobe.obj
TB_PGO_OBJS = $(TB_PGO_OBJS) $(PGO_BUILD)\syzygy.obj $(PGO_BUILD)\tbprobe.obj
TB_PROFILE_OBJS = $(TB_PROFILE_OBJS) $(PROFILE)\syzygy.obj $(PROFILE)\tbprobe.obj
CFLAGS = $(CFLAGS) -DSYZYGY_TBS
STB_FLAGS = /TP $(CFLAGS)
STB_PROFILE_FLAGS = $(STB_FLAGS)
$(BUILD)\tbprobe.obj: $(STB)\tbprobe.c $(STB)\tbcore.c
	$(CL) $(STB_FLAGS) $(OPT) $(DEBUG) /c /Fo$@ $(STB)\tbprobe.c
$(PROFILE)\tbprobe.obj: $(STB)\tbprobe.c $(STB)\tbcore.c
	$(CL) $(STB_PROFILE_FLAGS) $(OPT) $(DEBUG) /c /Fo$@ $(STB)\tbprobe.c
$(PGO_BUILD)\tbprobe.obj: $(STB)\tbprobe.c $(STB)\tbcore.c
	$(CL) $(STB_PROFILE_FLAGS) $(PROF_USE_FLAGS) $(OPT) $(DEBUG) /c /Fo$@ $(STB)\tbprobe.c
!Endif

# Linker flags
LDFLAGS  = kernel32.lib user32.lib winmm.lib $(NUMA_LIBS) $(LD_FLAGS) /nologo /subsystem:console /incremental:no /opt:ref /stack:4000000 /version:$(VERSION)
 
ARASANX_OBJS = $(BUILD)\arasanx.obj \
$(BUILD)\attacks.obj $(BUILD)\bhash.obj $(BUILD)\bitboard.obj \
$(BUILD)\board.obj $(BUILD)\boardio.obj $(BUILD)\options.obj \
$(BUILD)\chess.obj $(BUILD)\material.obj $(BUILD)\movegen.obj \
$(BUILD)\params.obj $(BUILD)\scoring.obj $(BUILD)\searchc.obj \
$(BUILD)\see.obj $(BUILD)\globals.obj $(BUILD)\search.obj \
$(BUILD)\notation.obj $(BUILD)\hash.obj $(BUILD)\stats.obj \
$(BUILD)\bitprobe.obj $(BUILD)\epdrec.obj $(BUILD)\chessio.obj \
$(BUILD)\movearr.obj $(BUILD)\log.obj \
$(BUILD)\bookread.obj $(BUILD)\bookwrit.obj \
$(BUILD)\calctime.obj $(BUILD)\legal.obj $(BUILD)\eco.obj \
$(BUILD)\learn.obj \
$(BUILD)\ecodata.obj $(BUILD)\threadp.obj $(BUILD)\threadc.obj \
$(BUILD)\unit.obj $(TB_OBJS) $(NUMA_OBJS)

TUNER_OBJS = $(TUNE_BUILD)\tuner.obj \
$(TUNE_BUILD)\attacks.obj $(TUNE_BUILD)\bhash.obj $(TUNE_BUILD)\bitboard.obj \
$(TUNE_BUILD)\board.obj $(TUNE_BUILD)\boardio.obj $(TUNE_BUILD)\options.obj \
$(TUNE_BUILD)\chess.obj $(TUNE_BUILD)\material.obj $(TUNE_BUILD)\movegen.obj \
$(TUNE_BUILD)\vparams.obj $(TUNE_BUILD)\scoring.obj $(TUNE_BUILD)\searchc.obj \
$(TUNE_BUILD)\see.obj $(TUNE_BUILD)\globals.obj $(TUNE_BUILD)\search.obj \
$(TUNE_BUILD)\notation.obj $(TUNE_BUILD)\hash.obj $(TUNE_BUILD)\stats.obj \
$(TUNE_BUILD)\bitprobe.obj $(TUNE_BUILD)\epdrec.obj $(TUNE_BUILD)\chessio.obj \
$(TUNE_BUILD)\movearr.obj $(TUNE_BUILD)\log.obj \
$(TUNE_BUILD)\bookread.obj $(TUNE_BUILD)\bookwrit.obj \
$(TUNE_BUILD)\calctime.obj $(TUNE_BUILD)\legal.obj $(TUNE_BUILD)\eco.obj \
$(TUNE_BUILD)\learn.obj \
$(TUNE_BUILD)\ecodata.obj $(TUNE_BUILD)\threadp.obj $(TUNE_BUILD)\threadc.obj \
$(TUNE_BUILD)\tune.obj $(TB_TUNE_OBJS) $(NUMA_TUNE_OBJS)

ARASANX_PGO_OBJS = $(PGO_BUILD)\arasanx.obj \
$(PGO_BUILD)\attacks.obj $(PGO_BUILD)\bhash.obj $(PGO_BUILD)\bitboard.obj \
$(PGO_BUILD)\board.obj $(PGO_BUILD)\boardio.obj $(PGO_BUILD)\options.obj \
$(PGO_BUILD)\chess.obj $(PGO_BUILD)\material.obj $(PGO_BUILD)\movegen.obj \
$(PGO_BUILD)\params.obj $(PGO_BUILD)\scoring.obj $(PGO_BUILD)\searchc.obj \
$(PGO_BUILD)\see.obj $(PGO_BUILD)\globals.obj $(PGO_BUILD)\search.obj \
$(PGO_BUILD)\notation.obj $(PGO_BUILD)\hash.obj $(PGO_BUILD)\stats.obj \
$(PGO_BUILD)\bitprobe.obj $(PGO_BUILD)\epdrec.obj $(PGO_BUILD)\chessio.obj \
$(PGO_BUILD)\movearr.obj $(PGO_BUILD)\log.obj \
$(PGO_BUILD)\bookread.obj $(PGO_BUILD)\bookwrit.obj \
$(PGO_BUILD)\calctime.obj $(PGO_BUILD)\legal.obj $(PGO_BUILD)\eco.obj \
$(PGO_BUILD)\learn.obj \
$(PGO_BUILD)\ecodata.obj $(PGO_BUILD)\threadp.obj $(PGO_BUILD)\threadc.obj \
$(PGO_BUILD)\unit.obj $(TB_PGO_OBJS) $(NUMA_PGO_OBJS)

ARASANX_POPCNT_OBJS = $(POPCNT_BUILD)\arasanx.obj \
$(POPCNT_BUILD)\attacks.obj $(POPCNT_BUILD)\bhash.obj $(POPCNT_BUILD)\bitboard.obj \
$(POPCNT_BUILD)\board.obj $(POPCNT_BUILD)\boardio.obj $(POPCNT_BUILD)\options.obj \
$(POPCNT_BUILD)\chess.obj $(POPCNT_BUILD)\material.obj $(POPCNT_BUILD)\movegen.obj \
$(POPCNT_BUILD)\params.obj $(POPCNT_BUILD)\scoring.obj $(POPCNT_BUILD)\searchc.obj \
$(POPCNT_BUILD)\see.obj $(POPCNT_BUILD)\globals.obj $(POPCNT_BUILD)\search.obj \
$(POPCNT_BUILD)\notation.obj $(POPCNT_BUILD)\hash.obj $(POPCNT_BUILD)\stats.obj \
$(POPCNT_BUILD)\bitprobe.obj $(POPCNT_BUILD)\epdrec.obj $(POPCNT_BUILD)\chessio.obj \
$(POPCNT_BUILD)\movearr.obj $(POPCNT_BUILD)\log.obj \
$(POPCNT_BUILD)\bookread.obj $(POPCNT_BUILD)\bookwrit.obj \
$(POPCNT_BUILD)\calctime.obj $(POPCNT_BUILD)\legal.obj $(POPCNT_BUILD)\eco.obj \
$(POPCNT_BUILD)\learn.obj \
$(POPCNT_BUILD)\ecodata.obj $(POPCNT_BUILD)\threadp.obj $(POPCNT_BUILD)\threadc.obj \
$(POPCNT_BUILD)\unit.obj $(TB_OBJS) $(NUMA_OBJS)

ARASANX_BMI2_OBJS = $(BMI2_BUILD)\arasanx.obj \
$(BMI2_BUILD)\attacks.obj $(BMI2_BUILD)\bhash.obj $(BMI2_BUILD)\bitboard.obj \
$(BMI2_BUILD)\board.obj $(BMI2_BUILD)\boardio.obj $(BMI2_BUILD)\options.obj \
$(BMI2_BUILD)\chess.obj $(BMI2_BUILD)\material.obj $(BMI2_BUILD)\movegen.obj \
$(BMI2_BUILD)\params.obj $(BMI2_BUILD)\scoring.obj $(BMI2_BUILD)\searchc.obj \
$(BMI2_BUILD)\see.obj $(BMI2_BUILD)\globals.obj $(BMI2_BUILD)\search.obj \
$(BMI2_BUILD)\notation.obj $(BMI2_BUILD)\hash.obj $(BMI2_BUILD)\stats.obj \
$(BMI2_BUILD)\bitprobe.obj $(BMI2_BUILD)\epdrec.obj $(BMI2_BUILD)\chessio.obj \
$(BMI2_BUILD)\movearr.obj $(BMI2_BUILD)\log.obj \
$(BMI2_BUILD)\bookread.obj $(BMI2_BUILD)\bookwrit.obj \
$(BMI2_BUILD)\calctime.obj $(BMI2_BUILD)\legal.obj $(BMI2_BUILD)\eco.obj \
$(BMI2_BUILD)\learn.obj \
$(BMI2_BUILD)\ecodata.obj $(BMI2_BUILD)\threadp.obj $(BMI2_BUILD)\threadc.obj \
$(BMI2_BUILD)\unit.obj $(TB_OBJS) $(NUMA_OBJS)

ARASANX_PROFILE_OBJS = $(PROFILE)\arasanx.obj \
$(PROFILE)\attacks.obj $(PROFILE)\bhash.obj $(PROFILE)\bitboard.obj \
$(PROFILE)\board.obj $(PROFILE)\boardio.obj $(PROFILE)\options.obj \
$(PROFILE)\chess.obj $(PROFILE)\material.obj $(PROFILE)\movegen.obj \
$(PROFILE)\params.obj $(PROFILE)\scoring.obj $(PROFILE)\searchc.obj \
$(PROFILE)\see.obj $(PROFILE)\globals.obj $(PROFILE)\search.obj \
$(PROFILE)\notation.obj $(PROFILE)\hash.obj $(PROFILE)\stats.obj \
$(PROFILE)\bitprobe.obj $(PROFILE)\epdrec.obj $(PROFILE)\chessio.obj \
$(PROFILE)\movearr.obj $(PROFILE)\log.obj \
$(PROFILE)\bookread.obj $(PROFILE)\bookwrit.obj \
$(PROFILE)\calctime.obj $(PROFILE)\legal.obj $(PROFILE)\eco.obj \
$(PROFILE)\ecodata.obj $(PROFILE)\learn.obj \
$(PROFILE)\threadp.obj $(PROFILE)\threadc.obj $(TB_PROFILE_OBJS) \
$(NUMA_PROFILE_OBJS)

MAKEBOOK_OBJS = $(BUILD)\makebook.obj \
$(BUILD)\attacks.obj $(BUILD)\bhash.obj $(BUILD)\bitboard.obj \
$(BUILD)\board.obj $(BUILD)\boardio.obj $(BUILD)\options.obj \
$(BUILD)\chess.obj $(BUILD)\material.obj $(BUILD)\movegen.obj \
$(BUILD)\params.obj $(BUILD)\scoring.obj $(BUILD)\searchc.obj \
$(BUILD)\see.obj $(BUILD)\globals.obj $(BUILD)\search.obj \
$(BUILD)\notation.obj $(BUILD)\hash.obj $(BUILD)\stats.obj \
$(BUILD)\bitprobe.obj $(BUILD)\epdrec.obj $(BUILD)\chessio.obj \
$(BUILD)\movearr.obj $(BUILD)\log.obj \
$(BUILD)\bookread.obj $(BUILD)\bookwrit.obj \
$(BUILD)\learn.obj $(BUILD)\legal.obj \
$(BUILD)\threadp.obj $(BUILD)\threadc.obj $(TB_OBJS) $(NUMA_OBJS)

MAKEECO_OBJS = $(BUILD)\makeeco.obj \
$(BUILD)\attacks.obj $(BUILD)\bhash.obj $(BUILD)\bitboard.obj \
$(BUILD)\board.obj $(BUILD)\boardio.obj $(BUILD)\options.obj \
$(BUILD)\chess.obj $(BUILD)\material.obj $(BUILD)\movegen.obj \
$(BUILD)\params.obj $(BUILD)\scoring.obj $(BUILD)\searchc.obj \
$(BUILD)\see.obj $(BUILD)\globals.obj $(BUILD)\search.obj \
$(BUILD)\notation.obj $(BUILD)\hash.obj $(BUILD)\stats.obj \
$(BUILD)\bitprobe.obj $(BUILD)\epdrec.obj $(BUILD)\chessio.obj \
$(BUILD)\movearr.obj $(BUILD)\log.obj \
$(BUILD)\bookread.obj $(BUILD)\bookwrit.obj \
$(BUILD)\legal.obj $(BUILD)\learn.obj \
$(BUILD)\threadp.obj $(BUILD)\threadc.obj $(TB_OBJS) $(NUMA_OBJS)

ECOCODER_OBJS = $(BUILD)\ecocoder.obj \
$(BUILD)\attacks.obj $(BUILD)\bhash.obj $(BUILD)\bitboard.obj \
$(BUILD)\board.obj $(BUILD)\boardio.obj $(BUILD)\options.obj \
$(BUILD)\chess.obj $(BUILD)\material.obj $(BUILD)\movegen.obj \
$(BUILD)\params.obj $(BUILD)\scoring.obj $(BUILD)\searchc.obj \
$(BUILD)\see.obj $(BUILD)\globals.obj $(BUILD)\search.obj \
$(BUILD)\notation.obj $(BUILD)\hash.obj $(BUILD)\stats.obj \
$(BUILD)\bitprobe.obj $(BUILD)\epdrec.obj $(BUILD)\chessio.obj \
$(BUILD)\movearr.obj $(BUILD)\log.obj \
$(BUILD)\bookwrit.obj $(BUILD)\bookread.obj \
$(BUILD)\legal.obj \
$(BUILD)\learn.obj $(BUILD)\threadp.obj $(BUILD)\threadc.obj \
$(BUILD)\eco.obj $(BUILD)\ecodata.obj $(TB_OBJS) \
$(NUMA_OBJS)

PGNSELECT_OBJS = $(BUILD)\pgnselect.obj \
$(BUILD)\attacks.obj $(BUILD)\bhash.obj $(BUILD)\bitboard.obj \
$(BUILD)\board.obj $(BUILD)\boardio.obj $(BUILD)\options.obj \
$(BUILD)\chess.obj $(BUILD)\material.obj $(BUILD)\movegen.obj \
$(BUILD)\params.obj $(BUILD)\scoring.obj $(BUILD)\searchc.obj \
$(BUILD)\see.obj $(BUILD)\globals.obj $(BUILD)\search.obj \
$(BUILD)\notation.obj $(BUILD)\hash.obj $(BUILD)\stats.obj \
$(BUILD)\bitprobe.obj $(BUILD)\epdrec.obj $(BUILD)\chessio.obj \
$(BUILD)\movearr.obj $(BUILD)\log.obj \
$(BUILD)\bookwrit.obj $(BUILD)\bookread.obj \
$(BUILD)\legal.obj \
$(BUILD)\learn.obj $(BUILD)\threadp.obj $(BUILD)\threadc.obj $(TB_OBJS) \
$(NUMA_OBJS)

PLAYCHESS_OBJS = $(BUILD)\playchess.obj \
$(BUILD)\attacks.obj $(BUILD)\bhash.obj $(BUILD)\bitboard.obj \
$(BUILD)\board.obj $(BUILD)\boardio.obj $(BUILD)\options.obj \
$(BUILD)\chess.obj $(BUILD)\material.obj $(BUILD)\movegen.obj \
$(BUILD)\params.obj $(BUILD)\scoring.obj $(BUILD)\searchc.obj \
$(BUILD)\see.obj $(BUILD)\globals.obj $(BUILD)\search.obj \
$(BUILD)\notation.obj $(BUILD)\hash.obj $(BUILD)\stats.obj \
$(BUILD)\bitprobe.obj $(BUILD)\epdrec.obj $(BUILD)\chessio.obj \
$(BUILD)\movearr.obj $(BUILD)\log.obj \
$(BUILD)\bookwrit.obj $(BUILD)\bookread.obj \
$(BUILD)\legal.obj \
$(BUILD)\learn.obj $(BUILD)\threadp.obj $(BUILD)\threadc.obj $(TB_OBJS) \
$(NUMA_OBJS)

{}.cpp{$(BUILD)}.obj:
    $(CL) $(OPT) $(DEBUG) $(CFLAGS) /c /Fo$@ $<

{$(UTIL)}.cpp{$(BUILD)}.obj:
    $(CL) $(OPT) $(DEBUG) $(CFLAGS) -I. /c /Fo$@ $<

{}.cpp{$(TUNE_BUILD)}.obj:
    $(CL) $(OPT) $(DEBUG) $(CFLAGS) $(TUNE_FLAGS) /c /Fo$@ $<

{}.cpp{$(PROFILE)}.obj:
    $(CL) $(CFLAGS) $(OPT) $(PROF_GEN_FLAGS) /c /Fo$@ $<

{}.cpp{$(PGO_BUILD)}.obj:
    $(CL) $(CFLAGS) $(PROF_USE_FLAGS) $(OPT) /c /Fo$@ $<

{}.cpp{$(POPCNT_BUILD)}.obj:
    $(CL) $(OPT) $(DEBUG) $(CFLAGS) /c /Fo$@ $<

{}.cpp{$(BMI2_BUILD)}.obj:
    $(CL) $(OPT) $(DEBUG) $(CFLAGS) /c /Fo$@ $<

profile: dirs $(PROFILE)\arasanx.exe

$(BUILD)\makebook.exe:  $(MAKEBOOK_OBJS)
        $(LD) $(MAKEBOOK_OBJS) $(LDFLAGS) /out:$(BUILD)\makebook.exe

$(BUILD)\makeeco.exe:  $(MAKEECO_OBJS)
        $(LD) $(MAKEECO_OBJS) $(LDFLAGS) /out:$(BUILD)\makeeco.exe

$(BUILD)\ecocoder.exe:  $(ECOCODER_OBJS)
        $(LD) $(ECOCODER_OBJS) $(LINKOPT) $(LDFLAGS) /out:$(BUILD)\ecocoder.exe

$(BUILD)\$(ARASANX).exe: dirs $(ARASANX_OBJS)
        $(LD) $(ARASANX_OBJS) $(LDFLAGS) $(LDDEBUG) /out:$(BUILD)\$(ARASANX).exe

$(BUILD)\tuner.exe: dirs $(TUNER_OBJS)
        $(LD) $(TUNER_OBJS) $(LDFLAGS) $(LDDEBUG) /out:$(BUILD)\tuner.exe

$(BUILD)\pgnselect.exe: dirs $(PGNSELECT_OBJS)
        $(LD) $(PGNSELECT_OBJS) $(LINKOPT) $(LDFLAGS) $(LDDEBUG) /out:$(BUILD)\pgnselect.exe

$(BUILD)\playchess.exe: dirs $(PLAYCHESS_OBJS)
        $(LD) $(PLAYCHESS_OBJS) $(LINKOPT) $(LDFLAGS) $(LDDEBUG) /out:$(BUILD)\playchess.exe

$(BUILD)\nalimov.obj: nalimov.cpp
    $(CL) $(TB_FLAGS) /c /Fo$@ nalimov.cpp

$(PROFILE)\nalimov.obj: nalimov.cpp
    $(CL) $(TB_PROFILE_FLAGS) /c /Fo$@ nalimov.cpp

$(PGO_BUILD)\nalimov.obj: nalimov.cpp
    $(CL) $(PROF_USE_FLAGS) $(TB_PROFILE_FLAGS) /c /Fo$@ nalimov.cpp

$(PROFILE)\arasanx.exe:  $(ARASANX_PROFILE_OBJS)
    $(PROF_GEN_LD) $(PROF_LINK_FLAGS) $(LDFLAGS) $(ARASANX_PROFILE_OBJS) /out:$(PROFILE)\arasanx.exe

profile-run: dirs
    del $(PROFDATA)\*.pgc
    $(PROFILE)\arasanx $(PROF_RUN_FLAGS) <..\tests\prof

profile-optimized: dirs $(ARASANX_PGO_OBJS)
    $(LD) $(LDFLAGS) $(ARASANX_PGO_OBJS) /out:$(BUILD)\$(ARASANX).exe

popcnt: dirs
	SET CFLAGS=$(POPCNT_FLAGS)
	set ARASANX=$(ARASANX)-popcnt
	nmake -f Makefile.icl

bmi2: dirs
	SET CFLAGS=$(BMI2_FLAGS)
	set ARASANX=$(ARASANX)-bmi2
	nmake -f Makefile.icl

profile-build: dirs profile profile-run profile-optimized 

profiled: profile-build

popcnt-profiled:
	SET CFLAGS=$(POPCNT_FLAGS)
	SET ARASANX=$(ARASANX)-popcnt
	nmake -f Makefile.icl profile-build

bmi2-profiled:
	SET CFLAGS=$(BMI2_FLAGS)
	SET ARASANX=$(ARASANX)-bmi2
	nmake -f Makefile.icl profile-build

release: $(RELEASE) $(SOURCE_ARCHIVE)

$(SOURCE_ARCHIVE):
	call release.bat $(SOURCE_ARCHIVE)

$(PROFILE):
	md $(PROFILE)

$(PROFDATA):
        md $(PROFDATA)

$(BUILD):
	md $(BUILD)

$(TUNE_BUILD):
	md $(TUNE_BUILD)

$(PGO_BUILD):
	md $(PGO_BUILD)

$(POPCNT_BUILD):
	md $(POPCNT_BUILD)

$(BMI2_BUILD):
	md $(BMI2_BUILD)

$(RELEASE):
	md $(RELEASE)

clean: dirs
	del $(BUILD)\*.obj
	del $(TUNE_BUILD)\*.obj
	del $(TUNE_BUILD)\*.asm
	del $(BUILD)\*.exe
	del $(BUILD)\*.asm
	del $(PROFILE)\*.asm
	del $(PROFILE)\*.pgc
	del $(PROFILE)\*.pgd
	del $(PROFILE)\*.obj
	del $(PROFILE)\*.exe
	del /q $(RELEASE)\*.*
	del /q $(PGO_BUILD)\*.*
	del /q $(POPCNT_BUILD)\*.*
	del /q $(BMI2_BUILD)\*.*

dirs: $(BUILD) $(TUNE_BUILD) $(PROFILE) $(PROFDATA) $(PGO_BUILD) $(POPCNT_BUILD) $(BMI2_BUILD) $(RELEASE)
