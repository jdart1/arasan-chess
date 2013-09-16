# Arasan Makefile for use with NMAKE and Intel C++ on the Windows platform
# Copyright 2004-2012 by Jon Dart. All Rights Reserved.
#
TARGET = win32
#TARGET = win64
VERSION = 16.2

# directory defines - objects
PROFILE = ..\$(TARGET)\profile
PROFDATA = ..\$(TARGET)\profdata

#TRACE=/D_TRACE 
#DEBUG=/Zi -D_DEBUG 
#LDDEBUG=/DEBUG /PDB:arasanx.pdb

#define the appropriate macro for the type(s) of tablebase supported
#GAVIOTA_TBS=1
#NALIMOV_TBS=1

# location of the Gaviota tablebase source code
GTB=michiguel-Gaviota-Tablebases-161d6cb

# location of the Nalimov tablebase source code
TB=tb

!If "$(TARGET)" == "win64"
ARCH = /D_WIN64
ARASANX = arasanx-64
!Else
ARASANX = arasanx
!Endif

!Ifdef DEBUG
BUILD_TYPE=debug
!Else
BUILD_TYPE=release
!Endif
BUILD = ..\$(TARGET)\$(BUILD_TYPE)
PGO_BUILD = ..\$(TARGET)\pgo_build
RELEASE = $(BUILD_ROOT)\release
# relative to root:
SOURCE_ARCHIVE = release\arasan_source$(VERSION).zip

BUILD_PROFILE = $(PROFILE)

TESTS = ..\tests

# SMP flags. Note: we don't support a non-SMP build any more.
!Ifdef DEBUG
SMP=/MTd /DSMP /DSMP_STATS
!Else
SMP=/MT /DSMP /DSMP_STATS
!Endif
PROF_RUN_SMP=-c 2

LD=link
CL=cl

# Intel C++ 12.0 defs, release build (intel64)
!If "$(TARGET)" == "win64"
INTEL64 = /Wp64
INTEL64LIB = bufferoverflowU.lib
CL       = icl
LD       = xilink
CFLAGS = /D_CONSOLE /D_CRT_SECURE_NO_WARNINGS $(TRACE) $(SMP) $(DEBUG) $(INTEL64)
OPT = /O3 /Ob2 /GR- /Qinline-max-size- /DUSE_INTRINSICS /DUSE_ASM
!Else
# Intel C++ 12.0 defs, release build (IA32)
CL       = icl
LD       = xilink
CFLAGS = /D_CONSOLE /D_CRT_SECURE_NO_WARNINGS $(TRACE) $(SMP) $(DEBUG) $(INTEL64)
OPT = /O3 /Ob2 /Oy- /Gr /GR- /Qinline-max-size- /DUSE_INTRINSICS /DUSE_ASM
!Endif

# Intel C++ defs (profile build)
IPO = /Qipo
PROF_GEN_LD = xilink
PROF_GEN_FLAGS = /Qprof_gen /Qprof_dir$(PROFDATA)
PROF_RUN_FLAGS = -H 64M $(PROF_RUN_SMP)
PROF_USE_FLAGS = /Qprof_use $(IPO) /Qprof_dir$(PROFDATA)

TB_FLAGS = $(CFLAGS) $(SMP) /Gr /O1 /GS- /I$(TB) /I.
TB_PROFILE_FLAGS = $(TB_FLAGS) $(PROF_USE_FLAGS)

default: dirs $(BUILD)\arasanx.exe $(BUILD)\makebook.exe $(BUILD)\makeeco.exe

!IfDef NALIMOV_TBS
TB_OBJS = $(BUILD)\tbprobe.obj
TB_PROFILE_OBJS = $(PROFILE)\tbprobe.obj
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
TB_PROFILE_OBJS = $(TB_PROFILE_OBJS) $(GTB_PROFILE_OBJS) $(PROFILE)\gtb.obj

GTB_LIBDIR=..\lib\$(TARGET)\$(BUILD_TYPE)
GTB_LIBNAME=gtb.lib

# A library version of the GTB code, mostly for use with the Visual C++ IDE.
# Note: PGO in Visual C++ is not compatible with static libraries.
gtb-lib: $(GTB_OBJS) $(GTB_LIBDIR)
	lib /LTCG /out:$(GTB_LIBDIR)\$(GTB_LIBNAME) $(GTB_OBJS)

$(GTB_LIBDIR):
	md $(GTB_LIBDIR)
!Endif

# Linker flags
LDFLAGS  = kernel32.lib user32.lib $(LD_FLAGS) /nologo /subsystem:console /incremental:no /opt:ref /stack:4000000 /version:$(VERSION)
 
ARASANX_OBJS = $(BUILD)\arasanx.obj \
$(BUILD)\attacks.obj $(BUILD)\bhash.obj $(BUILD)\bitboard.obj \
$(BUILD)\board.obj $(BUILD)\boardio.obj $(BUILD)\options.obj \
$(BUILD)\chess.obj $(BUILD)\material.obj $(BUILD)\movegen.obj \
$(BUILD)\scoring.obj $(BUILD)\searchc.obj \
$(BUILD)\see.obj $(BUILD)\globals.obj $(BUILD)\search.obj \
$(BUILD)\notation.obj $(BUILD)\hash.obj $(BUILD)\stats.obj \
$(BUILD)\bitprobe.obj $(BUILD)\epdrec.obj $(BUILD)\chessio.obj \
$(BUILD)\movearr.obj $(BUILD)\log.obj $(BUILD)\book.obj \
$(BUILD)\bookread.obj $(BUILD)\bookutil.obj $(BUILD)\bookentr.obj \
$(BUILD)\calctime.obj $(BUILD)\legal.obj $(BUILD)\eco.obj \
$(BUILD)\learn.obj $(BUILD)\history.obj \
$(BUILD)\ecodata.obj $(BUILD)\threadp.obj $(BUILD)\threadc.obj $(TB_OBJS) 

ARASANX_PGO_OBJS = $(PGO_BUILD)\arasanx.obj \
$(PGO_BUILD)\attacks.obj $(PGO_BUILD)\bhash.obj $(PGO_BUILD)\bitboard.obj \
$(PGO_BUILD)\board.obj $(PGO_BUILD)\boardio.obj $(PGO_BUILD)\options.obj \
$(PGO_BUILD)\chess.obj $(PGO_BUILD)\material.obj $(PGO_BUILD)\movegen.obj \
$(PGO_BUILD)\scoring.obj $(PGO_BUILD)\searchc.obj \
$(PGO_BUILD)\see.obj $(PGO_BUILD)\globals.obj $(PGO_BUILD)\search.obj \
$(PGO_BUILD)\notation.obj $(PGO_BUILD)\hash.obj $(PGO_BUILD)\stats.obj \
$(PGO_BUILD)\bitprobe.obj $(PGO_BUILD)\epdrec.obj $(PGO_BUILD)\chessio.obj \
$(PGO_BUILD)\movearr.obj $(PGO_BUILD)\log.obj $(PGO_BUILD)\book.obj \
$(PGO_BUILD)\bookread.obj $(PGO_BUILD)\bookutil.obj $(PGO_BUILD)\bookentr.obj \
$(PGO_BUILD)\calctime.obj $(PGO_BUILD)\legal.obj $(PGO_BUILD)\eco.obj \
$(PGO_BUILD)\learn.obj $(PGO_BUILD)\history.obj \
$(PGO_BUILD)\ecodata.obj $(PGO_BUILD)\threadp.obj $(PGO_BUILD)\threadc.obj \
$(TB_OBJS) 

ARASANX_PROFILE_OBJS = $(PROFILE)\arasanx.obj \
$(PROFILE)\attacks.obj $(PROFILE)\bhash.obj $(PROFILE)\bitboard.obj \
$(PROFILE)\board.obj $(PROFILE)\boardio.obj $(PROFILE)\options.obj \
$(PROFILE)\chess.obj $(PROFILE)\material.obj $(PROFILE)\movegen.obj \
$(PROFILE)\scoring.obj $(PROFILE)\searchc.obj \
$(PROFILE)\see.obj $(PROFILE)\globals.obj $(PROFILE)\search.obj \
$(PROFILE)\notation.obj $(PROFILE)\hash.obj $(PROFILE)\stats.obj \
$(PROFILE)\bitprobe.obj $(PROFILE)\epdrec.obj $(PROFILE)\chessio.obj \
$(PROFILE)\movearr.obj $(PROFILE)\log.obj $(PROFILE)\book.obj \
$(PROFILE)\bookread.obj $(PROFILE)\bookutil.obj $(PROFILE)\bookentr.obj \
$(PROFILE)\calctime.obj $(PROFILE)\legal.obj $(PROFILE)\eco.obj \
$(PROFILE)\ecodata.obj $(PROFILE)\learn.obj $(BUILD)\history.obj \
$(PROFILE)\threadp.obj $(PROFILE)\threadc.obj $(TB_PROFILE_OBJS) 

MAKEBOOK_OBJS = $(BUILD)\makebook.obj \
$(BUILD)\attacks.obj $(BUILD)\bhash.obj $(BUILD)\bitboard.obj \
$(BUILD)\board.obj $(BUILD)\boardio.obj $(BUILD)\options.obj \
$(BUILD)\chess.obj $(BUILD)\material.obj $(BUILD)\movegen.obj \
$(BUILD)\scoring.obj $(BUILD)\searchc.obj \
$(BUILD)\see.obj $(BUILD)\globals.obj $(BUILD)\search.obj \
$(BUILD)\notation.obj $(BUILD)\hash.obj $(BUILD)\stats.obj \
$(BUILD)\bitprobe.obj $(BUILD)\epdrec.obj $(BUILD)\chessio.obj \
$(BUILD)\movearr.obj $(BUILD)\log.obj $(BUILD)\book.obj \
$(BUILD)\bookread.obj $(BUILD)\bookutil.obj $(BUILD)\bookentr.obj \
$(BUILD)\learn.obj $(BUILD)\history.obj $(BUILD)\legal.obj \
$(BUILD)\threadp.obj $(BUILD)\threadc.obj $(TB_OBJS)

MAKEECO_OBJS = $(BUILD)\makeeco.obj \
$(BUILD)\attacks.obj $(BUILD)\bhash.obj $(BUILD)\bitboard.obj \
$(BUILD)\board.obj $(BUILD)\boardio.obj $(BUILD)\options.obj \
$(BUILD)\chess.obj $(BUILD)\material.obj $(BUILD)\movegen.obj \
$(BUILD)\scoring.obj $(BUILD)\searchc.obj \
$(BUILD)\see.obj $(BUILD)\globals.obj $(BUILD)\search.obj \
$(BUILD)\notation.obj $(BUILD)\hash.obj $(BUILD)\stats.obj \
$(BUILD)\bitprobe.obj $(BUILD)\epdrec.obj $(BUILD)\chessio.obj \
$(BUILD)\movearr.obj $(BUILD)\log.obj $(BUILD)\book.obj \
$(BUILD)\bookread.obj $(BUILD)\bookutil.obj $(BUILD)\bookentr.obj \
$(BUILD)\legal.obj  $(BUILD)\history.obj $(BUILD)\learn.obj \
$(BUILD)\threadp.obj $(BUILD)\threadc.obj $(TB_OBJS)

{}.cpp{$(BUILD)}.obj:
    $(CL) $(OPT) $(DEBUG) $(CFLAGS) /c /Fo$@ $<

{}.cpp{$(PROFILE)}.obj:
    $(CL) $(CFLAGS) $(OPT) $(SSE) $(PROF_GEN_FLAGS) /c /Fo$@ $<

{}.cpp{$(PGO_BUILD)}.obj:
    $(CL) $(CFLAGS) $(PROF_USE_FLAGS) $(OPT) $(SSE) /c /Fo$@ $<

default: dirs $(BUILD)\$(ARASANX).exe $(BUILD)\makebook.exe $(BUILD)\makeeco.exe

profile: dirs $(PROFILE)\arasanx.exe

$(BUILD)\makebook.exe:  $(MAKEBOOK_OBJS)
        $(LD) $(MAKEBOOK_OBJS) $(LDFLAGS) /out:$(BUILD)\makebook.exe

$(BUILD)\makeeco.exe:  $(MAKEECO_OBJS)
        $(LD) $(MAKEECO_OBJS) $(LDFLAGS) /out:$(BUILD)\makeeco.exe

$(BUILD)\$(ARASANX).exe: dirs $(ARASANX_OBJS)
        $(LD) $(ARASANX_OBJS) $(LDFLAGS) $(LDDEBUG) /out:$(BUILD)\$(ARASANX).exe

$(BUILD)\tbprobe.obj: tbprobe.cpp
    $(CL) $(TB_FLAGS) /c /Fo$@ tbprobe.cpp

$(PROFILE)\tbprobe.obj: tbprobe.cpp
    $(CL) $(TB_PROFILE_FLAGS) /c /Fo$@ tbprobe.cpp

$(PROFILE)\arasanx.exe:  $(ARASANX_PROFILE_OBJS)
    $(PROF_GEN_LD) $(PROF_LINK_FLAGS) $(LDFLAGS) $(ARASANX_PROFILE_OBJS) /out:$(PROFILE)\arasanx.exe

profile-run: dirs
    del $(PROFDATA)\*.pgc
    $(PROFILE)\arasanx $(PROF_RUN_FLAGS) <..\tests\prof

profile-optimized: dirs $(ARASANX_PGO_OBJS)
    $(LD) $(LDFLAGS) $(ARASANX_PGO_OBJS) /out:$(PGO_BUILD)\arasanx.exe

run-tests:
    $(BUILD)\$(ARASANX) -H 256M -c 2 <$(TESTS)\tests >$(TESTS)\test-results.txt

score-tests:
    date /t >$(TESTS)\test-summary.txt
    perl $(TESTS)\tests.pl >>$(TESTS)\test-summary.txt

release: $(RELEASE)
	call release.bat $(SOURCE_ARCHIVE)

$(PROFILE):
	md $(PROFILE)

$(PROFDATA):
        md $(PROFDATA)

$(BUILD):
	md $(BUILD)

$(PGO_BUILD):
	md $(PGO_BUILD)

$(RELEASE):
	md $(RELEASE)

clean: dirs
	del $(BUILD)\*.obj
	del $(BUILD)\*.exe
	del $(BUILD)\*.asm
	del $(PROFILE)\*.asm
	del $(PROFILE)\*.pgc
	del $(PROFILE)\*.pgd
	del $(PROFILE)\*.obj
	del $(PROFILE)\*.exe
	del /q $(RELEASE)\*.*
	del /q $(PGO_BUILD)\*.*

dirs: $(BUILD) $(PROFILE) $(PROFDATA) $(PGO_BUILD) $(RELEASE)
