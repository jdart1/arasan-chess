# Build & install instructions for Arasan

The source distribution for Arasan includes sources and makefiles for
building the chess program (arasanx), and also source for the
following other console programs:

- makebook - builds binary opening book files from text (PGN) files
- makeeco - builds ecodata.cpp from from "eco" text file
- ecococder - adds ECO codes to a PGN file
- pgnfilter - samples PGN files, writes EPD records to stdout
- playchess - filters PGN games, removing those where end eval differs from result (and short games)
- tuner - automatically tunes scoring parameters
- selfplay - generates positions for NNUE tuning

Following is a sketch of the Arasan source directory tree:

```
arasan
  <version>
    src     - source code for the chess engine and related tools
    gui     - source code for the Arasan user interface (Windows only)
    book    - text files for opening book and ECO recognizer
    doc     - documentation
    network - network files
    tests   - test files and documentation
    tools   - misc. utility scripts
    prj     - Visual C++ project file and build directories (Windows)
```

Note: the source codes for Windows and Linux are identical, except
that there are different format Makefiles, one for Windows nmake and
one for GNU make.

Syzygy tablebase support is enabled by default.

Syzygy support, NNUE support,and some of the Python scripts in the tools
subdirectory rely on imported submodules. So if building Arasan from a
git respository, issue the following command within your git directory
to pull these dependencies into your your source tree:

`git submodule update --init --recursive`

Arasan now requires a modern compiler with at least C++-17 support.

Limited testing has been done on other OSs and architectures other
than x86 and amd64. However, the code is designed to be portable.
In particular, there is support for big-endian architectures.

# Building on Linux or Mac

There is a makefile in the src directory.

The Makefile requires the "bc" utility, available on most Linux distros.
If using Cygwin, you should ensure you have installed this.

In most cases, the Makefile should automatically select the correct
target architecture and OS (note: assumes target and host are the
same). By default it builds only the chess engine. Binaries are placed
in the "bin" subdirectory.

The following targets are defined in the makefile:

- default: builds just the chess engine
- profiled: PGO build of the chess engine
- tuning: builds the parameter tuning program
- utils: builds utility programs including "makebook"
- release: builds the release tarball
- install: installs the chess engine on the system (requires root or sudo)

By default the Makefile will build a "generic" executable that will be
relatively slow but run on practically all systems except very old
ones. This generic compile currently assumes SSE2 support at least is
present, for an x86 or x86_64 architecture.

The Makefile supports compilation for other instruction sets by specifying
the BUILD_TYPE variable.

Supported instruction sets at present are:

- old (assumes no SIMD instructions, not even SSE2)
- modern (implies support for: POPCNT, SSSE3, SSE4.1)
- avx2 (assumes availabiity of AVX2 instruction set, plus "modern" instructions)
- avx2-bmi2 (assumes avaiability of AVX2 and BMI2 instruction sets, plus "modern" instructions)
- avx512 (assumes AVX512, plus avaiability of AVX2 and BMI2 instruction sets, plus "modern" instructions)

The BUILD_TYPE variable can be used to specify the desired instruction
set for the compilation: this works not just with the chess engine, but with
the tuner and utiliies, also. For example:

make BUILD_TYPE=avx2 utils

will build the utilities with AVX2 support.

Note: the instruction sets supported via BUILD_TYPE will only work on
x86 or x86_64 CPUs that support these instructions, and the non-default settings
work best with a 64-bit build.

If necessary, you can specify the compiler by passing the CC variable
on the command line, and CXXFLAGS can also be used to pass additional
flags. So for example to make an build of Arasan using gcc-10
with c++20 support, do:

make CC=gcc-10 CXXFLAGS=-std=c++20

Defining NUMA in the Makefile will build a version that has support
for NUMA (Non-Uniform Memory Access) machines. NUMA support relies
on the hwloc library version 2.0 or higher. Note: you must have a
compatible hwloc library in the library search path at runtime.

The Arasan engine binary is named "arasanx-32" or "arasanx-64," followed
by the instruction set selected at build time (if specified), so for example:
"arasanx-64-avx2-bmi2."  "-numa" is added for a NUMA
build.

Note: "make release" will build the release tarball and place it in the
release subdirectory. This target uses a Python tool git-archive-all
(https://github.com/Kentzo/git-archive-all), which needs to be
installed and its path set in the Makefile (or passed to it in the
variable GIT_ARCHIVE_ALL_PATH). And that tool calls "git archive," so
"make release" needs to be run from within a git repository.

"make install" will install Arasan binaries into /usr/local/bin/arasan-\<version>.
(You will need to be root, or use sudo, to execute "make install"). 

# Building on Windows

## Recommended build method

I recommend building the engine with MSVC and NMAKE, using the supplied makefile
(named "Makefile" in the Windows source zip, or "Makefile.win" in the
github repo). This is the most reliable and tested method. Visual
Studio Community Edition is free for use on open-source projects and
includes the needed command-line tools. You need a recent version with C++ 17
support.

The following targets are defined in the makefile:

- default: builds just the chess engine
- profiled: build the chess engine using PGO
- tuning: builds the parameter tuning program
- utils: builds utility programs including "makebook"
- release: builds the release zip file for Windows

As with the Linux Makefile, you can select the target instruction set
by setting the BUILD_TYPE variable. For example:
`make BUILD_TYPE=avx2` will build a version of the program with AVX2
support. Current build types supported are:

- modern (implies: sse3, sse4, sse4.1, popcnt)
- avx2 (includes "modern" instruction sets)
- avx512 (includes "avx2" and "modern" instruction sets)

The default with no BUILD_TYPE set is a very generic executable that
does not assume any advanced instruction set, but does assume SSE2,
available on all x86 processors since about 2000.

Release binaries from a build are placed in the
"\<target>\Release" directory, where <target> is "win32" or "win64",
while the debug objects and binaries are put into "\<target>\Debug".

The default target is 64-bit Windows. For a 32-bit build, edit the
Makefile to select TARGET=win32 and run the same build commands.

## Windows XP compatibility

To make a XP-compatible build, edit the Makefile to set PLATFORM=XP.
This requires installation of the Windows SDK version 7.1A. You can
obtain this by running (or re-running, if needed) the Visual Studio
2019 installer and selecting "MSVC v140 - VS2015 C++ Build tools"
(not included by default). (Note Windows NT compilation support is
deprecated and will be removed by Microsoft eventually. Note also
I no longer have a functioning XP machine and so compatibility of
current builds of Arasan is not tested).

## Building the engine with Visual Studio

The Windows source distribution includes Visual Studio solution
files for the chess engine in the "prj" directory.
But I use the engine "prj" solution file mostly for
debugging, not for building the final release. Project files are
less maintained/tested than the NMAKE Makefile.

## Building the Arasan Windows GUI

Source for Arasan's native Windows GUI is included in the Windows source archives
downloadable from [arasanchess.org](https://www.arasanchess.org).

To build the GUI, use the Visual C++ solution file in the "gui"
subdirectory. (The command-line Makefile does not build the GUI). You
will need a version of Visual Studio that includes
the required MFC libraries. The Community Edition should work.
By default the GUI solution file produces a XP-compatible build,
so it requires the Windows SDK 7.1A to be installed. The Visual
Studio installer can add this, but it is not installed by default.

## Cygwin/MSYS

Arasan can also be built using the Cygwin development tools, which
simulate a Linux environment under Windows. Use the Linux source
package for building with Cygwin or [MSYS2](https://www.msys2.org/). I
do not currently recommend or support [MingW](http://mingw.org/).

# CMake

There is a CMakeLists.txt file in the source directory, for building
with [CMake](https://cmake.org/).  This should be considered somewhat
experimental. It does not currently support PGO. It does support
[cross-compliation for
Android](https://developer.android.com/ndk/guides/cmake#android_platform).
