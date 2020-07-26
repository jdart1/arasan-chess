#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright 2017-2020 by Jon Dart. All Rights Reserved.
#
import argparse, re, sys, subprocess, time, chess, chess.engine, traceback

class Options:
   engine = 'stockfish'
   multipv = 3
   time = 60
   cores = 1
   hash = 4000
   syzygy = '/syzygypath'

class Results:
   # the current position's results indexed by multipv index
   infos = {}
   solution_time = 0
   solution_nodes = 0
   bestmove = None

class Position:
   # the current position's EPD operators
   ops = {}
   board = chess.Board()

options = Options()

results = Results()

position = Position()

done = False

solved = 0
tried = 0

def correct(bestmove,position):
    if not ('bm' in position.ops) and not ('am' in position.ops):
       print("error: missing target move(s) in line: " + line,file=sys.stderr)
       return False
    else:
       san = position.board.san(bestmove)
       if 'bm' in position.ops and san in position.ops['bm']:
          return True
       elif 'am' in position.ops and not (san in position.ops['am']):
          return True
       return False

def process_info(info,results):
   multipv = None
   if "multipv" in info:
      multipv = int(info["multipv"])
      results.infos[multipv] = info
   elif options.multipv == 1:
      results.infos[1] = info
   try:
      result = results.infos[1]
   except KeyError:
       return
   if "time" in result:
      time = result["time"]
   if "nodes" in result:
      node_str = result["nodes"]
   if "pv" in result:
      pv_str = result["pv"]
      # get current bm from the pv
      move = pv_str[0]
      if correct(move,position):
          if (time != None and results.solution_time == 0):
              # convert to milliseconds
              results.solution_time = int(1000*time)
          if (node_str != None and results.solution_nodes == 0):
              results.solution_nodes = int(node_str)
      else:
          # solution was found but now a different move is selected
          results.solution_time = 0
          results.solution_nodes = 0

def init(engine,options):
    # note: multipv not set here
    engine.configure({'Hash': options.hash, 'Syzygypath': options.syzygy, 'Threads': options.cores})
    print("engine ready")

def main(argv = None):
    global options
    global results
    global position
    global solved
    global tried

    engine = None

    if argv is None:
        argv = sys.argv[1:]

    parser = argparse.ArgumentParser(description="run test suite using UCI engine")
    parser.add_argument("-e", "--engine", type=str,help="engine path")
    parser.add_argument("-c", "--cores", type=int, help="number of cores")
    parser.add_argument("-m", "--multipv", type=int, help="number of lines to search/display")
    parser.add_argument("-s", "--syzygy", type=str,help="Syzygy tablebase path")
    parser.add_argument("-t", "--time", type=int,help="search time in seconds")
    parser.add_argument("-H", "--hash", type=int,help="hash size in kilobytes")
    parser.add_argument('filename',help="filename to analyze")
    parser.parse_args(namespace=options)

    try:
        engine = chess.engine.SimpleEngine.popen_uci(options.engine)
    except FileNotFoundError:
       print("engine executable " + options.engine + " not found", file=sys.stderr)
       return
    except:
       print(traceback.format_exc(), file=sys.stderr)
       print("failed to start child process " + options.engine, file=sys.stderr)
       return

    init(engine,options)

    pat = re.compile('^(([pnbrqkPNBRQK1-8])+\/)+([pnbrqkPNBRQK1-8])+ [wb]+ [\-kqKQ]+ [\-a-h1-8]+')

    with open(options.filename) as f:
        for line in f:
           # skip blank lines
           if len(line.strip())==0:
               continue
           m = pat.search(line)
           if m == None:
               print("error: invalid FEN in line: %s" % line, file=sys.stderr, flush=True)
           else:
               print()
               print(line)
               position.board = chess.Board(fen=m.group()+' 0 1')
               position.ops = position.board.set_epd(line)
               # set_epd returns moves as Move objects, convert to SAN:
               for key in position.ops:
                  i = 1
                  if (key == 'bm' or key == 'am'):
                     san_moves = []
                     for move in position.ops[key]:
                        try:
                           san_moves.append(position.board.san(move))
                        except:
                           print(key + " solution move " + str(i) + " could not be parsed",file=sys.stderr, flush=True)
                     i = i + 1
                     position.ops[key] = san_moves

               results.solution_time = 0
               results.solution_nodes = 0
               results.bestmove = None
               results.infos = {}
               with engine.analysis(board=position.board,
                                    limit=chess.engine.Limit(time=options.time),multipv=options.multipv) as analysis:
                  for info in analysis:
                     process_info(info,results)
               # print last group of results
               for i in range(1,options.multipv+1):
                   group = 0
                   infos = results.infos[i]
                   for key in ["depth","seldepth","multipv","score","nodes",
                               "nps","hashfull","tbhits","time","pv"]:
                       if key in infos:
                           if (group != 0):
                              print(" ",end="")
                           print(key + " ",end="")
                           if (key == "pv"):
                              fen = position.board.fen()
                              for m in infos[key]:
                                  san = position.board.san(m)
                                  print(san,end="")
                                  print(" ",end="")
                                  position.board.push(m)
                              # restore board
                              position.board.set_fen(fen)
                           else:
                              print(infos[key],end="")
                       group = group + 1
                   print()

               pv = results.infos[1].get("pv")
               if (pv != None):
                  results.bestmove = pv[0]
               tried = tried + 1
               if (results.bestmove != None):
                  if correct(results.bestmove,position):
                     solved = solved + 1
                     print("++ solved in " + str(results.solution_time/1000.0) + " seconds ("
                            + str(results.solution_nodes)
                            + " nodes)" + " (" + str(solved) + " out of " + str(tried) + " solved, "
                            + str(round(solved * 100 /tried,1)) + "%)", flush=True)

                  else:
                     print("-- not solved" + " (" + str(solved) + " out of " + str(tried) + " solved, "
                            + str(round(solved * 100 /tried,1)) + "%)", flush=True)
               else:
                   print("warning: best move == None for position " + str(tried),file=sys.stderr)
    engine.quit()
    print()
    print("RUN COMPLETED - " + str(solved) + " out of " + str(tried) + " solved (" + str(round(solved * 100 /tried,1)) + "%)")

if __name__ == "__main__":
    sys.exit(main())
