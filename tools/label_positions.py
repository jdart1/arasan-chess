#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright 2016-2018, 2020 by Jon Dart. All Rights Reserved.
# This code is under the MIT license: see doc directory.
#
# Tool to run matches that add score records (with c2 tag) to an EPD
# input file. Uses cutechess-cli for matches.
# Usage:
# python3 label_positions.py [-c cores] [-e engine] [-t time control] [-p game file] [-o options] epd_file
# Note: this has only been tested on Linux. May need mods for other platforms.

import re, sys, subprocess, tempfile, time, threading
from subprocess import Popen, PIPE, call

# Path to the cutechess-cli executable.
cutechess_cli_path = '/home/jdart/chess/cutechess-cli'

SHELL='/bin/bash'

RESULT_KEY='c2'

class Options:
   engine_name = 'Stockfish-8'
   tc = "0.10+0.1"
   cores = 1
   extra_options = "option.SyzygyPath=/home/jdart/chess/syzygy option.Hash=128"
   save_game_file = None

file_lock = threading.RLock()

output_lock = threading.RLock()

class RunGames:

   def run(self, file_lock, output_lock, epd_file, pat):
      while(1):
         file_lock.acquire()
         line = epd_file.readline().strip()
         file_lock.release()
         # stop at blank line
         if len(line)==0:
             break
         m = pat.search(line)
         if m == None:
            print("error: invalid FEN in line: %s" % line, file=sys.stderr)
         else:
            self.run_game(m.group())

   def run_game(self,epd):
      # create temp file for EPD
      global output_lock
      temp_epd_file = tempfile.NamedTemporaryFile(delete=False)
      temp_file_name = temp_epd_file.name
      temp_epd_file.write(bytes(epd+"\n",'UTF-8'))
      temp_epd_file.flush()
      temp_epd_file.close()

      try:
          fcp = Options.engine_name
          options = "-openings file=%s format=epd -each tc=%s %s" % (temp_file_name,Options.tc,Options.extra_options)
          if (Options.save_game_file != None):
             options = options + " -pgnout " + Options.save_game_file
          cutechess_args = ' -engine conf=%s -engine conf=%s %s' % (fcp, fcp, options)
          command = '%s/%s %s' % (cutechess_cli_path, 'cutechess-cli', cutechess_args)

          # Run cutechess-cli and wait for it to finish
          try:
              #print("command="+command+'\n')
              process = Popen(command, shell=True, stdout=PIPE, cwd=cutechess_cli_path)
          except FileNotFoundError:
              print("cutechess-cli not found at %s" % cutechess_cli_path,file=sys.stderr)
              return 2

          try:
             output, errs = process.communicate(timeout=240)
          except subprocess.TimeoutExpired:
             process.kill()
             print("cutechess-cli timed out, was killed",file=sys.stderr)
             output, errs = process.communicate()
             return 2
          if process.returncode != 0:
              if errs != None:
                  for line in errs.splitlines():
                      print(line,file=sys.stderr)
              print('Could not execute command: %s' % command,file=sys.stderr)
              return 2
          # Convert Cutechess-cli's result into a numeric score.
          # Note that only one game should be played
          # output is like "Finished game 1 (stockfish-dev vs stockfish-dev): 0-1 {Black mates}"
          result_pat = re.compile("^Finished game \\d+ \\(.+\\): ([0-2\\-\\/\\*]+).+$")
          value = 0.5
          matched = False
          for lin in output.splitlines():
              line = lin.decode('utf-8')
              match = result_pat.match(line)
              if (match != None):
                 result = match.group(1)
                 matched = True
                 if (result == "*"):
                     print('The game did not terminate properly',file=sys.stderr)
                     break
                 elif (result == "0-1"):
                     value = 0.0
                 elif (result == "1-0"):
                     value = 1.0
          if (matched):
              output_lock.acquire()
              # output epd + result
              print(epd + ' ' + RESULT_KEY + ' "' + str(value) + '";')
              output_lock.release()
          else:
              print('No result found',file=sys.stderr)
      finally:
          call('rm ' + temp_file_name,shell=True)

def init():
    global options

def main(argv = None):
    global options

    if argv is None:
        argv = sys.argv[1:]

    arg = 0
    while ((arg < len(argv)) and (argv[arg][0:1] == '-')):
        if (argv[arg][1] == 'c'):
            arg = arg + 1
            if (arg < len(argv)):
                try:
                    Options.cores = int(argv[arg])
                except exceptions.ValueError:
                    print(('Invalid value for parameter %s: %s' % (argv[i], argv[i + 1])),file=sys.stderr)
                    return 2
            arg = arg + 1
        elif (argv[arg][1] == 'e'):
            arg = arg + 1
            if (arg < len(argv)):
                Options.engine_name = argv[arg]
                arg = arg + 1
        elif (argv[arg][1] == 'o'):
            arg = arg + 1
            if (arg < len(argv)):
                Options.extra_options = argv[arg]
                arg = arg + 1
        elif (argv[arg][1] == 'p'):
            arg = arg + 1
            if (arg < len(argv)):
                Options.save_game_file = argv[arg]
                arg = arg + 1
        elif (argv[arg][1] == 't'):
            arg = arg + 1
            if (arg < len(argv)):
                Options.tc = argv[arg]
                arg = arg + 1
        else:
            print("Unrecognized switch: " + argv[arg], file=sys.stderr)
            return

    if (arg >= len(argv)):
        print("Expected a filename to analyze.", file=sys.stderr)
        return

    init()

    global pat
    pat = re.compile('^(([pnbrqkPNBRQK1-8])+\/)+([pnbrqkPNBRQK1-8])+ [wb]+ [\-kqKQ]+ [\-a-h1-8]+')

    epd_file = open(argv[arg])


    # start threads
    for num_thread in range(Options.cores):
       rg = RunGames()
       t = threading.Thread(target=rg.run, args=(file_lock,output_lock,epd_file,pat))
       #print("starting " + str(num_thread))
       t.start()

if __name__ == "__main__":
    sys.exit(main())
