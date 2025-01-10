# Arasan

Arasan is a chess engine, that is, a console-based program that plays the game of chess.

The chess engine by itself has no graphical interface, but can be used together
with external interface programs such as [Winboard and xboard](https://www.gnu.org/software/xboard/). 

For communicating with a chess interface, Arasan supports either the
standard [CECP](http://home.hccnet.nl/h.g.muller/engine-intf.html)
protocol (version 2) or the
[UCI](https://www.chessprogramming.org/Chess_Engine_Communication_Protocol)
protocol. CECP is the native protocol used by Winboard and
xboard. UCI-compatible chess interfaces include ChessBase and
Fritz. [Arena](http://www.playwitharena.de/), a free interface,
supports both protocols.

Arasan is multi-platform (Windows, Linux, Mac OS, Unix) and supports multi-threading for higher performance.

## Program variant

This is a variant on Arasan chess intended to run on mobile apple devices using [async-chesskit-engine](https://github.com/Amir-Zucker/async-chesskit-engine)
for the original arasan chess engine, see: [Arasan](https://github.com/jdart1/arasan-chess) 

## Option file (arasan.rc)

Formerly Arasan would auto-load options from an arasan.rc file.
The engine would look for this file in the program's directory,
and if present, Arasan will also read a file from the user's home
directory:

- on Windows or MacOS, from arasan.rc
- on Linux, from .arasan.rc

The contents of these files will overwrite the one that is in the
program directory.

Option files are primarily useful for configuring Arasan when running
it from the command line or from command-line tools. Generally, chess
engine interfaces (especially GUIs such as Fritz or xboard) will have
their own means of setting options, and they may set default options
that differ from anything in an options file. Their settings override
anything in an arasan.rc file.

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
<!--- SyzygyUses50MoveRule - true to observe the 50 move draw rule when probing-->
<!--tablebases; false to have tablebase probes ignore the rule. Default is true.-->
<!--- SyzygyProbeDepth - maximum depth to probe tablebases. Default is 4.-->
<!--- SyzygyPath - path to the Syzygy tablebases. Multiple directories can-->
<!--be specified, separated by ':'.-->
- MultiPV - followed by a number. Number of distinct best lines to display.
Default is 1.
- OwnBook - true or false. True to enable use of the Arasan book (book.bin)
if installed. Default is true.
- BookPath - path to the Arasan book. Default is book.bin in the same
directory as the Arasan engine executable HOME environment variable directory.
- Book variety - value from 0 to 100, default 50. Higher values
produce more variety in book move selection.
- Position learning - true to enable position learning (storing
key position results in a file and later retrieving them).
- Learn file name - name & location of the file to store position learning data
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

## Neural Network

Arasan now supports chess evaluation utilizing a neural network. The
neural network file is external to the program and is loaded at
runtime. The file name for the network can be specified by setting 
the `search.nnueFile` option in the arasan.rc file. 
There is a default network filename set there. In the options, 
if no directory path is part of the option or a relative path
is used, the file is assumed to be in or relative to the same
directory as the HOME environment variable.

## Tablebases

Table bases are currently not supported.  
There is an open issue on the matter: [issue](https://github.com/Amir-Zucker/arasan-chess-monochrome/issues/2)   
feel free to contribute to the project by fixing this. 
<!--Arasan supports Syzygy format compressed endgame tablebases. You can configure-->
<!--Arasan to use tablebases by editing the arasan.rc file, or by using a-->
<!--GUI that supports UCI or CECP option commands. Note that many-->
<!--chess GUIs will override the arasan.rc settings, and set their own defaults.-->
<!--So if using a GUI you should if possible set the tablebase path and related-->
<!--options in the GUI, not in the file.-->
<!---->
<!--The Arasan distribution does not come with any tablebase files. Syzygy-->
<!--tablebases can be downloaded from https://syzygy-tables.info/. -->

## Related projects

Arasan relies on the following other projects, also authored by or modified by Jon Dart:

- [Fathom](https://github.com/jdart1/Fathom), Syzygy tablebase probing code (fork of [basil00/Fathom](https://github.com/basil00/Fathom))
- [nnue](https://github.com/jdart1/nnue), Chess neural network reading code
- [stats](https://www.github.com/jdart1/stats), Python module for SPRT computation (derived from [glinscott/fishtest](https://github.com/glinscott/fishtest))

## Additional information

Arasan's [website](http://www.arasanchess.org) hosts binaries and additional information
related to Arasan.


## Copyright, license

Arasan is licensed under the MIT License. See the LICENSE file.
