# Build & install instructions for Arasan

The source distribution for Arasan includes sources and makefiles for
building the chess program (arasanx), and also source for the
following other console programs:

- makebook - builds binary opening book files from text (PGN) files
- makeeco - builds ecodata.cpp from from "eco" text file
- ecococder - adds ECO codes to a PGN file
- playchess - filters PGN games, removing those where end eval differs from result (and short games)
- selfplay - generates positions for NNUE tuning
- post_process_nn - converts quantised network file from bullet to Arasan format

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

Note: the source codes for Windows and Linux are identical, although
some parts are only applicable to one platform (notably the GUI), and
there are different Makefiles for the two platforms, one for Windows
NMAKE (Makefile.win) and one for GNU make (Makefile).

Syzygy tablebase support is enabled by default.

Syzygy support, NNUE support,and some of the Python scripts in the tools
subdirectory rely on imported submodules. So if building Arasan from a
git respository, issue the following command within your git directory
to pull these dependencies into your your source tree:

`git submodule update --init --recursive`

Arasan now requires a modern compiler with at least C++-17
support. Clang is the recommended compiler (can use clang-cl on
Windows). Clang compiles are measurably faster, due to better code
generation, especially in the NNUE code.

The primary architectures supported and tested are x86, x86_64, and ARM.
However, the code is designed to be portable. In particular, there is support
for big-endian architectures.

# Building on Linux or Mac

32-bit Linux distros are not supported.

There is a Makefile in the src directory.

The Makefile requires the "gawk" utility, available on most Linux distros.
If using Cygwin, you should ensure you have installed this.

The following targets are defined in the makefile:

- default: builds just the chess engine
- profiled: PGO build of the chess engine
- unit: builds a program to execute unit tests
- utils: builds utility programs including "makebook"
- release: builds the release tarball
- install: installs the chess engine on the system (requires root or sudo)

By default, the Makefile will build a "generic" x86_64 executable that will be
relatively slow but run on practically all systems except very old
ones. This generic compile currently assumes SSE2 support at least is
present.

The Makefile supports compilation for other instruction sets by specifying
the BUILD_TYPE variable.

Supported instruction sets at present are:

- old (assumes no SIMD instructions, not even SSE2)
- modern (implies support for: POPCNT, SSSE3, SSE4.1)
- avx2 (assumes availabiity of AVX2 instruction set, plus "modern" instructions)
- avx2-bmi2 (assumes avaiability of AVX2 and BMI2 instruction sets, plus "modern" instructions)
- avx512 (assumes AVX512, plus avaiability of AVX2 and BMI2 instruction sets, plus "modern" instructions)
- neon (for ARM processors)

The BUILD_TYPE variable can be used to specify the desired instruction
set for the compilation: this works not just with the chess engine, but with
the utiliies, also. For example:

make BUILD_TYPE=avx2 utils

will build the utilities with AVX2 support.

You can specify the compiler by passing the CC variable
on the command line, and CXXFLAGS can also be used to pass additional
compliation flags. So, for example, to make an build of Arasan using gcc-10
with c++20 support, do:

make CC=gcc-10 CXXFLAGS=-std=c++20

As noted earlier, clang is recommended for best perfomance. To use clang,
you must ensure a compatible gcc toolchain is available so headers/libraries
can be accessed. Execute "clang -v" to see what toolchain clang is
trying to access. For PGO,
a compatible version of "llvm-profdata," from the clang bin directory,
must be present in the PATH: this may not be the case for a default
clang install. You can fix this by creating a symlink in /usr/bin to
the llvm-profdata utility (the target should be in the same directory as the
/usr/bin/clang symlink's target).

Program execution for PGO runs in the "profile" subdirectory, and
-a is passed to the program, allowing options such as tablebase
use to be set via an arasan.rc file in that directory, if desired.

Defining NUMA in the Makefile will build a version that has support
for NUMA (Non-Uniform Memory Access) machines. NUMA support relies
on the hwloc library version 2.0 or higher. Note: you must have a
compatible hwloc library in the library search path at runtime.

The Arasan engine binary is named "arasanx-64," followed
by the instruction set selected at build time (if specified), so for example:
"arasanx-64-avx2-bmi2."  "-numa" is added for a NUMA
build.

"make release" will build the release tarball and place it in the
release subdirectory. This target uses a Python tool git-archive-all
(https://github.com/Kentzo/git-archive-all), which needs to be
installed and its path set in the Makefile (or passed to it in the
variable GIT_ARCHIVE_ALL_PATH). And that tool calls "git archive," so
"make release" needs to be run from within a git repository.

"make install" will install Arasan binaries into /usr/local/bin/arasan-\<version>.
(You will need to be root, or use sudo, to execute "make install"). 

# Building on Windows

## Recommended build method

I recommend building the engine from the command line with NMAKE,
using the Makefile in the Windows source distribution (Makefile.win
in the repo).
This is the most reliable and tested method. Visual Studio Community
Edition is free for use on open-source projects and includes the
needed command-line tools. You need a recent version with C++ 17
support. As mentioned above, use of clang-cl (can be installed along
with MSVC) is recommended for best performance; this can be specified
by including `CC=clang-cl` on the command line.

The following targets are defined in the makefile:

- default: builds just the chess engine
- profiled: build the chess engine using PGO
- unit: builds a program to execute unit tests
- utils: builds utility programs including "makebook"
- release: builds the release zip file for Windows

Profiled builds will require that you have the network file copied into
the target profile directory (win64\profile or win32\profile).

As with the Linux Makefile, you can select the target instruction set
by setting the BUILD_TYPE variable. For example:
`make BUILD_TYPE=avx2` will build a version of the program with AVX2
support. Current build types supported are:

- old (assumes no SIMD instructions, not even SSE2)
- modern (implies support for: POPCNT, SSSE3, SSE4.1)
- avx2 (assumes availabiity of AVX2 instruction set, plus "modern" instructions)
- avx2-bmi2 (assumes avaiability of AVX2 and BMI2 instruction sets, plus "modern" instructions)
- avx512 (assumes AVX512, plus avaiability of AVX2 and BMI2 instruction sets, plus "modern" instructions)

The default with no BUILD_TYPE set is a very generic executable that
does not assume any advanced instruction set, but does assume SSE2,
available on all x86 processors since about 2000.

Release binaries from a build are placed in the
"\<target>\Release" directory, where <target> is "win32" or "win64",
while the debug objects and binaries are put into "\<target>\Debug".

The default target is 64-bit Windows. For a 32-bit build, edit the
Makefile to select TARGET=win32 and run the same build commands.

## Windows XP compatibility

To make a Windows XP-compatible build, edit the Makefile to set PLATFORM=XP.
This requires installation of the build tools version 141_xp, including the
Windows SDK version 7.1A. This is available as part of a Visual Studio 2017
install, or by selecting the approprate build tools as part of a later
Visual Studio install (but apparently Visual Studio 2026 no longer offers this).
Note I no longer have a functioning XP machine, and so compatibility of current
builds of Arasan is not tested.

## Building the engine with Visual Studio

The Windows source distribution includes Visual Studio solution
files for the chess engine in the "prj" directory.
But I use the engine "prj" solution file mostly for
debugging, not for building the final release. Project files are
less maintained/tested than the NMAKE Makefile.

## Building the Arasan Windows GUI

To build the GUI, use the Visual C++ solution file in the "gui"
subdirectory. (The command-line Makefile does not build the GUI). You
will need a version of Visual Studio that includes
the required MFC libraries. The Community Edition should work.
By default the GUI solution file produces a Windows XP-compatible build,
and so it requires the Windows SDK 7.1A to be installed. The Visual
Studio installer can add this, but it is not installed by default.

The Arasan GUI installer was built with InnoSetup: see the gui/install
directory for further details.

## Cygwin/MSYS

The Arasan engine can also be built using the Cygwin development tools, which
simulate a Linux environment under Windows. Use the Linux source
package for building with Cygwin or [MSYS2](https://www.msys2.org/). I
do not currently recommend or support [MingW](http://mingw.org/).

Cygwin builds with AVX2 do not work due to a <a href="https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54412">known GCC bug</a>.

# CMake

There is a CMakeLists.txt file in the source directory, for building
with [CMake](https://cmake.org/). It does not currently support PGO. It does support
[cross-compliation for
Android](https://developer.android.com/ndk/guides/cmake#android_platform).
