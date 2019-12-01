#!/usr/bin/python3
# -*- coding: utf-8 -*-

# This script reads a PGN file and produces as output the
# PGN filtered to remove games whose final position scores
# differently from the reported result (for example,
# White significantly ahead or position even, but White
# recorded as losing). Usually these are losses on time or
# by disconnection (f playing on a server).
#
# Usage:
# python3 playchess.py -e <engine path> -t <time> -c <engine cores> -H <engine hash> pgn_file
# output is to stdout.
#
# Copyright 2019 by Jon Dart. All Rights Reserved.
#
import asyncio, sys, subprocess, time, chess, chess.engine, chess.pgn

class Options:
   engine_name = 'stockfish'
   search_time = 5
   cores = 1
   hash_size = 4000
   win_threshold = 150
   draw_threshold = 100

options = Options()

async def main(argv = None):
    global options
    global engine

    if argv is None:
        argv = sys.argv[1:]

    arg = 0
    while ((arg < len(argv)) and (argv[arg][0:1] == '-')):
        if (argv[arg][1] == 'c'):
            arg = arg + 1
            if (arg < len(argv)):
                try:
                    options.cores = int(argv[arg])
                except exceptions.ValueError:
                    print(('Invalid value for parameter %s: %s' % (argv[i], argv[i + 1])),file=sys.stderr)
                    return 2
        elif (argv[arg][1] == 't'):
            arg = arg + 1
            if (arg < len(argv)):
                try:
                    options.search_time = int(argv[arg])
                except exceptions.ValueError:
                    print(('Invalid value for parameter %s: %s' % (argv[i], argv[i + 1])),file=sys.stderr)
                    return 2
        elif (argv[arg][1] == 'e'):
            arg = arg + 1
            if (arg < len(argv)):
                options.engine_name = argv[arg]
        elif (argv[arg][1] == 'H'):
            arg = arg + 1
            if (arg < len(argv)):
                try:
                    options.hash_size = int(argv[arg])
                except exceptions.ValueError:
                    print(('Invalid value for parameter %s: %s' % (argv[i], argv[i + 1])),file=sys.stderr)
                    return 2
        else:
            print("Unrecognized switch: " + argv[arg], file=sys.stderr)
            return
        arg = arg + 1

    time = options.search_time*1000

    if (arg >= len(argv)):
        print("Expected a filename.", file=sys.stderr)
        return

    try:
       transport, engine = await chess.engine.popen_uci(options.engine_name)
    except FileNotFoundError:
       print("engine executable " + options.engine_name + " not found", file=sys.stderr)
       return
    except Exception as ex:
       print("failed to start child process " + options.engine_name, file=sys.stderr)
       print(ex) 
       return

    try:
       f = open(argv[arg],encoding='utf-8-sig')
       while (True):
          g = chess.pgn.read_game(f)
          if (g == None):
             break
          visitor = chess.pgn.BoardBuilder()
          g.accept(visitor)
          # search the final position
          info = await engine.analyse(visitor.result(), limit=chess.engine.Limit(time=options.search_time), options={'Hash': options.hash_size, 'Threads': options.cores})
          # score in centipawns, White POV
          score = info['score'].white().score()
          # game result
          result = g.headers['Result']
          if (result == None or result == "*"):
             ok = False
          else:
             if (result == "0-1"):
                 ok = (score <= -options.win_threshold)
             elif (result == "1-0"):
                 ok = (score >= options.win_threshold)
             elif (result == "1/2-1/2"):
                 ok = (abs(score) <= options.draw_threshold)
             else:
                 print("Invalid game result %s" % result,file=sys.stderr)
                 ok = False
          if ok:
             g.accept(chess.pgn.FileExporter(sys.stdout))     
    finally:    
        f.close()
        await engine.quit()

if __name__ == "__main__":
    asyncio.set_event_loop_policy(chess.engine.EventLoopPolicy())
#    asyncio.run(main())
    loop = asyncio.get_event_loop()
    sys.exit(loop.run_until_complete(main()))
