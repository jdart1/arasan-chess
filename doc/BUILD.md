# Build & install instructions for Arasan

The source distribution for Arasan includes sources and makefiles for
building the chess program (arasanx), and also source for the
following other console programs:

- makebook - builds binary opening book files from text (PGN) files
- makeeco - builds ecodata.cpp from from "eco" text file
- ecococder - adds ECO codes to a PGN file
- pgnfilter - samples PGN files, writes EPD records to stdout
- playchess - filters PGN games, removing those where end eval differs from result (and short games)
- tuner  - automatically tunes scoring parameters

Following is a sketch of the Arasan source directory tree:

```
arasan
  <version>
    src     - source code for the chess engine and related tools
    gui     - source code for the Arasan user interface (Windows only)
    book    - text files for opening book and ECO recognizer
    doc     - documentation
    tests   - test files and documentation
    tools   - misc. utility scripts
    prj     - Visual C++ project file and build directories (Windows)
```

Note: the source codes for Windows and Linux are identical, except
that there are different format Makefiles, one for Windows nmake and
one for GNU make.

Syzygy tablebase support is enabled by default.

The Syzygy probing code is now imported as a submodule from
jdart1/Fathom. So if building Arasan from a git respository, issue the
following command to pull the Syzygy code into your source tree:

git submodule update --init --recursive

in your repository directory.

Arasan now requires a modern compiler with C++11 features (although
not necessarily complete C++11 support).

Limited testing has been done on other OSs and architectures other
than x32 and amd64.  However, the code is designed to be portable.
In particular, there is support for big-endian architectures.

# Building on Linux or Mac

Gcc 4.9 or above is recommended for building Arasan (on Mac OS, use clang).

There is a makefile in the src directory. In most cases, just typing
"make" should automatically select the correct target architecture and
OS (note: assumes target and host are the same). By default it builds
only the chess engine. Binaries are placed in the "bin" subdirectory.

To select the Intel compiler instead of gcc, define the variable CC with
this command:

export CC=icc

"clang" is also supported (set CC=clang to use).

The following targets are defined in the makefile:

- default: builds just the chess engine, with no POPCNT or BMI2 support
- popcnt: builds just the chess engine, with POPCNT support
- bmi2: builds just the chess engine, with BMI2 support (includes POPCNT)
- profiled: PGO build with no POPCNT or BMI2 support
- popcnt-profiled: PGO build of the chess engine, with POPCNT support
- bmi2-profiled: PGO build of the chess engine, with BMI2 support (includes POPCNT)
- tuning: builds the parameter tuning program
- tuning-popcnt: builds the parameter tuning program with POPCNT support
- utils: builds utility programs including "makebook"
- release: builds the release tarball
- install: installs the chess engine on the system (requires root or sudo)

Note: POPCNT and BMI2 builds will only work on recent x64 CPUs that support these
instructions, and require a 64-bit compile.

Defining NUMA in the Makefile will build a version that has support
for NUMA (Non-Uniform Memory Access) machines. NUMA support relies
on the hwloc library (version 2.0 or higher).

The Arasan engine binary is named "arasanx-32", "arasanx-64",
"arasanx-64-popcnt", or "arasanx-64-bmi2," depending on the
architecture and build flags used. "-numa" is added for a NUMA
build.

Note: "make release" will build the release tarball and place it in the
release subdirectory. This target uses a Python tool git-archive-all
(https://github.com/Kentzo/git-archive-all), which needs to be
installed and its path set in the Makefile (or passed to it in the
variable GIT_ARCHIVE_ALL_PATH). And that tool calls "git archive" so
"make release" needs to be run from within a git repository.

"make install" will install Arasan binaries into /usr/local/bin/arasan-\<version>.
(You will need to be root, or use sudo, to execute "make install"). 

# Building on Windows

The release engine binaries for Arasan were built built from
the command-line using NMAKE, and the Arasan GUI was built with the
Visual Studio Community Edition, using the project file.

## Recommended build method

I recommend building the engine with MSVC using the supplied makefile
(named "Makefile" in the Windows source zip, or "Makefile.win" in the
github repo). This is the most reliable and tested method. Visual
Studio Community Edition is free for use on open-source projects and
has all the features you need. You need a recent version with C++ 11
support. Visual Studio 2012 and later should work.</p>

The following targets are defined in the makefile:

- default: builds just the chess engine, with no POPCNT or BMI2 support
- popcnt: builds just the chess engine, with POPCNT support
- bmi2: builds just the chess engine, with BMI2 support (includes POPCNT)
- profiled: PGO build with no POPCNT or BMI2 support
- popcnt-profiled: PGO build of the chess engine, with POPCNT support
- bmi2-profiled: PGO build of the chess engine, with BMI2 support (includes POPCNT)
- tuning: builds the parameter tuning program
- tuning-popcnt: builds the parameter tuning program with POPCNT support
- utils: builds utility programs including "makebook".
- release: builds the release zip file for Windows

Release binaries from a build are placed in the
"\<target>\Release" directory, where <target> is "win32" or "win64",
while the debug objects and binaries are put into "\<target>\Debug".

The default target is 64-bit Windows. For a 32-bit build, edit the
Makefile to select TARGET=win32 and run the same build commands.

## Windows XP compatibility

To make a XP-compatible build, edit the Makefile to set PLATFORM=XP.
This requires installation of the Windows SDK version
7.1A. Unfortunately, if you don't have the SDK installed already,
there are known issues installing it under Windows 10. The most
reliable way to get it that I have found is to uninstall all recent
Visual Studio versions, install Visual C++ 2012 Professional
(available from my.visualstudio.com), and then re-install any more
recent Visual Studio release you may have.

## Building the Arasan Windows GUI

Source for Arasan's native Windows GUI is included in the Windows source archives
downloadable from [arasanchess.org](https://www.arasanchess.org).

To build the GUI, use the Visual C++ solution file in the "gui"
subdirectory. (The command-line Makefile does not build the GUI). You
cannot build the GUI with the Express Edition of Visual C++, because
it lacks the required MFC libraries. The Community Edition is ok,
though. By default the GUI solution file builds a XP-compatible build
so it requires the Windows SDK 7.1A to be installed.

## Other build methods

The Windows source distribution includes Visual C++ 2019 solution
files for the chess engine and tools and GUI in
directories "prj" and "gui", respectively. But I use the "prj" solution
file mostly for debugging, not for building the final release.

Arasan can also be built using the Cygwin development tools, which simulate
a Linux environment under Windows. Use
the Linux source package for building with Cygwin. I do not currently recommend
or support [MingW](http://mingw.org/).

# CMake

There is a CMakeLists.txt file in the source directory, for building with [CMake](https://cmake.org/).
This should be considered somewhat experimental. It does not currently support PGO.
It does support [cross-compliation for Android](https://developer.android.com/ndk/guides/cmake#android_platform).
