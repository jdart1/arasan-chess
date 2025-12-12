# Arasan

Arasan is a chess engine, that is, a console-based program that plays the game of chess.

The chess engine by itself has no graphical interface, but can be used together
with external interface programs such as [Winboard and
xboard](https://www.gnu.org/software/xboard/). This repository also contains source for a
Windows-only GUI for Arasan, in the "gui" subdirectory.

For communicating with a chess interface, Arasan supports either the
standard [CECP](http://home.hccnet.nl/h.g.muller/engine-intf.html)
protocol (version 2) or the
[UCI](https://www.chessprogramming.org/Chess_Engine_Communication_Protocol)
protocol. CECP is the native protocol used by Winboard and
xboard. UCI-compatible chess interfaces include ChessBase and
Fritz. [Arena](http://www.playwitharena.de/), a free interface,
supports both protocols.

Arasan is multi-platform (Windows, Linux, Mac OS, Unix) and supports multi-threading for higher performance.

## Copyright, license

Copyright 1994-2025 by Jon Dart. All Rights Reserved.

Arasan is licensed under the MIT License. See the LICENSE file. Note: this includes some special terms related to the GUI.

## Program variants

Several different binaries for Arasan can be built. The default
executable (arasanx-32 for 32-bit operating systems, or arasanx-64 for
64-bit ones) is designed to be runnable on most systems. Default x86 or x86_64 builds
assume SSE2 instructions are available (these were introduced in 2000
with the Pentium IV).

Other program variants include:

1. arasanx-64-old - assumes no SIMD instructions, not even SSE2. Will run on very old x86 hardware.

2. arasanx-64-modern - requires a x86 processor with POPCNT, SSSE3 and SSE4.1 instructions (Intel Nehalem or later,
i.e. 2008 era or more recent).

3. arasanx-64-avx2 - requires a more modern x86 processor with AVX2 instructions,
as well as those required by the "modern" build. Recommended for AMD
Excavator through Zen2 processors.

4. arasanx-64-avx2-bmi2 - requires a more modern x86 processor with AVX2 and BMI2 instructions,
as well as those required by the "modern" build. Recommended for Intel Haswell or later processors
and AMD processors on the Zen3 architecture (Nov. 2020) or later.

5. arasanx-64-avx512 - requires a modern x86 processor with AVX512
instructions (Intel Xeon Skylake X and Cannon Lake, or 11th generation
or later Intel Core processors (Tiger Lake), or Ryzen Zen 5 or later), as
well as those required by the "avx2" build.

6. arasanx-64-neon - ARM executable with support for NEON instruction set.

In addition it is possible to build a version with support for NUMA (Non-
Uniform Memory Access) systems - generally these are large multi-CPU systems.
NUMA-enabled versions of Arasan require the HWLOC library version 2.0
or higher: see https://www.open-mpi.org/software/hwloc/v2.0/.

## Command-line switches

Arasanx recognizes the following command-line options:

- -a auto-loads the options from an arasan.rc file in the program directory (if present),
and also (if present) from an arasan.rc in the user's home directory.

- -H: specifies the hash table size in bytes ('K', 'M', or 'G' can be used to indicate
kilobytes, megabyates or gigabytes). 

- -c: specifies the number of threads to use (default is 1).

- -r: specifies an option file (arasan.rc) to load on startup.

- -t: shows some debugging output in the UI window or log file. To
enable debug output, you also need to use the "-debugMode true" switch
if using xboard/Winboard, or the "debug=true" option if using Arasan
as a UCI engine. You normally shouldn't need to turn this on.

- -ics: outputs additional info when playing on a chess server.

Note: when using Arasan with a GUI or other interface program, usually
hash size and thread count are set in the interface. But the -H and -c
switches, if specified on the arasanx command line, take precedence
over those passed to Arasan via a GUI, or via the arasan.rc file.

If the word "bench" is specified on the command line, then Arasan will
run the bench command (for performance reporting) and then exit.

## Option file (arasan.rc)

Formerly Arasan would auto-load options from an arasan.rc file.
The engine would look for this file in the program's directory,
and if present, Arasan will also read a file from the user's home
directory:

- on Windows, from arasan.rc
- on MacOS or Linux, from .arasan.rc

The contents of these files will overwrite the one that is in the
program directory.

This behavior has now changed. Now, by default, no options file is read
and the program's options are entirely controlled by UCI or CECP commands.

To get the older behavior of auto-loading the .rc file, you can specify
the -a flag on the command line. Alternatively the -r flag with a
filename will read the options from the specified filename.

Option files are primarily useful for configuring Arasan when running
it from the command line or from command-line tools. Generally, chess
engine interfaces (especially GUIs such as Fritz or xboard) will have
their own means of setting options, and they may set default options
that differ from anything in an options file. Their settings override
anything in an arasan.rc file. Therefore, using an option file with an
engine GUI is not recommended, and that is why it is now disabled by
default in the chess engine.

At present, Arasan utility programs still use the older method of auto-loading
.rc files.

## UCI Options

When used as a UCI engine, Arasan supports the following options:

- Hash - hash size in kilobytes
- Threads - number of threads to use
- Ponder - true to enable pondering (thinking on opponent's time)
- Contempt - followed by a number from -200 to 200. This is the inverse of
the value of a draw in centipawns (1/100 pawn value). Negative values
mean the opponent is higher rated, so a draw is desirable. Positive values
mean the opponent is lower rated, so a draw should be avoided.
- Use tablebases - true to use tablebases, false to disable
- SyzygyUses50MoveRule - true to observe the 50 move draw rule when probing
tablebases; false to have tablebase probes ignore the rule. Default is true.
- SyzygyProbeDepth - maximum depth to probe tablebases. Default is 4.
- SyzygyPath - path to the Syzygy tablebases. Multiple directories can
be specified, separated by ':'.
- MultiPV - followed by a number. Number of distinct best lines to display.
Default is 1.
- OwnBook - true or false. True to enable use of the Arasan book (book.bin)
if installed. Default is true.
- BookPath - path to the Arasan book. Default is book.bin in the same
directory as the Arasan engine executable.
- Book variety - value from 0 to 100, default 50. Higher values
produce more variety in book move selection.
- Position learning - true to enable position learning (storing
key position results in a file and later retrieving them).
- Learning file - name & location of the file to store position learning data
- UCI_LimitStrength - true or false, default false. True to limit the engine
playing strength.
- UCI_Elo - desired playing strength, settable from Elo 1000 to Elo 3450. This is
only effective if UCI_LimitStrength is set true.
- Set processor affinity - for NUMA builds only. If set true, binds threads to
cores.
- Move overhead - value settable from 0 to 1000. This is a value in milliseconds
that will be subtraced from the time available to make a move. It helps Arasan
account for network or interface delays in calculating its time usage.
- NNUE File - the name of the neural network file to load. Currently assumed to be
in the same directory as the Arasan engine executable.

The defaults for many of these options are the values set in the arasan.rc file.

## CECP options

Besides the standard option-setting commands such as "memory" and "egtpath,"
the following additional option settings are available in CECP mode:

- SyzygyUses50MoveRule - true to observe the 50 move draw rule when probing
tablebases; false to have tablebase probes ignore the rule. Default is true.
- SyzygyProbeDepth - maximum depth to probe tablebases. Default is 4.
- Can resign - sets whether or not the engine can resign a game. Note: under UCI, the
interface program is always in charge of resignation.
- Resign threshold - sets how bad the score must be before resignation.
Only effective if "Can resign" is set true. Value is in centipawns
(1/100 pawn) and is negative. So for example -500 means resign when
down 5 pawns in score value.
- OwnBook - true to enable Arasan's native opening book, false to disable.
- BookPath - path to the Arasan book. Default is book.bin in the same
directory as the Arasan engine executable.
- Book variety - value from 0 to 100, default 50. Higher values
produce more variety in book move selection.
- Store games - true to store games that are played.
- Game pathname - specifies the path + filename for game storage. Default is "games.pgn"
in the Arasan executable directory (must be writeable).
- Position learning - true to enable position learning (storing
key position results in a file and later retrieving them).
- Learning file - name & location of the file to store position learning data
- Strength - search strength, settable on a scale of 0 (weakest) to 100 (strongest)
- Set processor affinity - for NUMA builds only. If set true, binds threads to
cores.
- Move overhead - value settable from 0 to 1000. This is a value in milliseconds
that will be subtraced from the time available to make a move. It helps Arasan
account for network or interface delays in calculating its time usage.
- NNUE File - the name of the neural network file to load. Currently assumed to be
in the same directory as the Arasan engine executable.

Note: the defaults for all these options are taken from the values in arasan.rc,
if that file is present.

## Neural Network

Arasan now supports chess evaluation utilizing a neural network. The
neural network file is external to the program and is loaded at
runtime. Arasan builds embed the name of a compatible network
file, assumed to be in the same location as the executable.

A different network file name can be specified using UCI or CECP
options, or by setting the `search.nnueFile` option in the arasan.rc
file, but normally you should not need these settings. In the options,
if no directory path is part of the option or a relative path is used,
the file is assumed to be in or relative to the same directory as the
Arasan executable.

## Tablebases

Arasan supports Syzygy format compressed endgame tablebases. You can configure
Arasan to use tablebases by editing the arasan.rc file, or by using a
GUI that supports UCI or CECP option commands. Note that many
chess GUIs will override the arasan.rc settings, and set their own defaults.
So if using a GUI you should if possible set the tablebase path and related
options in the GUI, not in the file.

The Arasan distribution does not come with any tablebase files. Syzygy
tablebases can be downloaded from https://syzygy-tables.info/.

## Programming and build information

See [BUILD.md](https://github.com/jdart1/arasan-chess/blob/master/doc/BUILD.md) for compilation instructions. See
the [Programmer's Guide](https://arasanchess.org/programr.shtml) for additional technical information.

## Related projects

Arasan relies on the following other projects:

- [Fathom](https://github.com/jdart1/Fathom), Syzygy tablebase probing code (fork of [basil00/Fathom](https://github.com/basil00/Fathom))
- [stats](https://www.github.com/jdart1/stats), Python module for SPRT computation (derived from [glinscott/fishtest](https://github.com/glinscott/fishtest))
- [chess-openings](https://github.com/lichess-org/chess-openings), chess openings classification database
- [bullet](https://github.com/jdart1/bullet), NNUE network trainer

## Additional information

Arasan's [website](http://www.arasanchess.org) hosts binaries and additional information
related to Arasan.
