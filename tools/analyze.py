#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Tool to analyze an EPD file with test positions and produce PVs and
# solution times/counts.
# Copyright 2016 by Jon Dart. All Rights Reserved.
# This code is under the MIT license: see license directory.

import re, sys, subprocess, time, chess, chess.uci

class Options:
   engine_name = 'stockfish'
   multi_pv_value = 3
   search_time = 60
   cores = 1
   hash_size = 4000

class Results:
   # the current position's results indexed by multipv index
   infos = {}
   solution_time = 0
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

class MyInfoHandler(chess.uci.InfoHandler):
    pat = re.compile("multipv [0-9]+")
    pat2 = re.compile(" pv ")

    def pre_info(self,line):
        super(MyInfoHandler, self).pre_info(line)
        global results
        global position
        global options
        match = MyInfoHandler.pat.search(line)
        if match != None:
            parts = match.group().split()
            if len(parts)>1:
                results.infos[int(parts[1])] = line
        elif options.multi_pv_value == 1:
            match = MyInfoHandler.pat2.search(line)
            if match != None:
                results.infos[1] = line
        i = results.infos[1].find(" time ")
        if i != -1:
            rest = results.infos[1][i+6:]
            j = rest.find(" ")
            if (j == -1):
                time_str = rest[0:]
            else:
                time_str = rest[0:j]
        # obtain the current bm from the pv
        i = results.infos[1].find(" pv ")
        if i != -1:
            rest = results.infos[1][i+4:]
            j = rest.find(" ")
            if (j == -1):
                move = rest[0:]
            else:
                move = rest[0:j]
            m = chess.Move.from_uci(move)
            if correct(m,position):
               if (results.solution_time == 0):
                  results.solution_time = int(time_str)
            else:
               results.solution_time = 0

    def on_bestmove(self,bestm,ponder):
        global results
        global done
        san_move = position.board.san(bestm)
        #print("result: " + san_move)
        results.bestmove = bestm
        done = True

def init():
    engine.uci()
    global options
    engine.setoption({'Hash': options.hash_size, 'Threads': options.cores, 'MultiPV': options.multi_pv_value})
    engine.isready()                       
    info_handler = MyInfoHandler()
    engine.info_handlers.append(info_handler)
    print("engine ready")            

def search(engine,board,ops,search_time):
    global results
    engine.position(board)
    engine.go(movetime=search_time)
    # print last group of results
    for i in range(1,options.multi_pv_value+1):
        print(results.infos[i])

def main(argv = None):
    global options
    global results
    global position
    global solved
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
            arg = arg + 1
        if (argv[arg][1] == 't'):
            arg = arg + 1
            if (arg < len(argv)):
                try:
                    options.search_time = int(argv[arg])
                except exceptions.ValueError:
                    print(('Invalid value for parameter %s: %s' % (argv[i], argv[i + 1])),file=sys.stderr)
                    return 2
            arg = arg + 1
        elif (argv[arg][1] == 'e'):
            arg = arg + 1
            if (arg < len(argv)):
                options.engine_name = argv[arg]
                arg = arg + 1
        elif (argv[arg][1] == 'H'):
            arg = arg + 1
            if (arg < len(argv)):
                try:
                    options.hash_size = int(argv[arg])
                except exceptions.ValueError:
                    print(('Invalid value for parameter %s: %s' % (argv[i], argv[i + 1])),file=sys.stderr)
                    return 2
            arg = arg+1
        elif (argv[arg][1] == 'm'):
            arg = arg + 1
            if (arg < len(argv)):
                try:
                    options.multi_pv_value = int(argv[arg])
                except exceptions.ValueError:
                    print(('Invalid value for parameter %s: %s' % (argv[i], argv[i + 1])),file=sys.stderr)
                    return 2
            arg = arg+1
        else:
            print("Unrecognized switch: " + argv[arg], file=sys.stderr)
            return
    time = options.search_time*1000

    if (arg >= len(argv)):
        print("Expected a filename to analyze.", file=sys.stderr)
        return

    try:
        engine = chess.uci.popen_engine(options.engine_name)
    except FileNotFoundError:
       print("engine executable " + options.engine_name + " not found", file=sys.stderr)
       return
    except:
       print("failed to start child process " + options.engine_name, file=sys.stderr)
       return

    init()

    pat = re.compile('^(([pnbrqkPNBRQK1-8])+\/)+([pnbrqkPNBRQK1-8])+ [wb]+ [\-kqKQ]+ [\-a-h1-8]+')

    with open(argv[arg]) as f:
        for line in f:
           # skip blank lines
           if len(line.strip())==0:
               continue
           m = pat.search(line)
           if m == None:
               print("error: invalid FEN in line: %s" % line, file=sys.stderr)
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
                           print(key + " solution move " + str(i) + " could not be parsed",file=sys.stderr)
                     i = i + 1                                 
                     position.ops[key] = san_moves

               results.solution_time = 0
               results.bestmove = None
               results.infos = {}
               search(engine,position.board,position.ops,time)
               if (done and results.bestmove != None):
                  if correct(results.bestmove,position):
                     print("++ solved in " + str(results.solution_time/1000.0) + " seconds")
                     solved = solved + 1
                  else:
                     print("-- not solved")
    engine.quit()
    print()
    print("solved: " + str(solved))

if __name__ == "__main__":
    sys.exit(main())
