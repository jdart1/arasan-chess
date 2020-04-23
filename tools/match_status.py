#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright 2017, 2020 by Jon Dart. All Rights Reserved.
#
import re, os, os.path, sys, time

# checks status of cutechess match on a host

# Cutechess bin directory
cutechess_cli_path = '/home/jdart/chess/cutechess-cli'

# Log file
log_file_name = 'match.log'

MAX_RESULTS = 100000

def read_last(filename, BUFSIZE=4096):
    f = open(filename, "r")
    f.seek(0, 2)
    p = f.tell()
    remainder = ""
    sz = min(BUFSIZE, p)
    p -= sz
    f.seek(p)
    buf = f.read(sz)
    if ('\n' in buf):
        i = buf.index('\n')
    else:
        return None
    lines = buf[i+1:].split("\n")
    lines.reverse()
    if (lines == None or len(lines) == 0):
       return None
    else:
       return lines

def main(argv = None):
    global cutechess_cli_path, log_file_name, MAX_RESULTS

    n = [0,0,0,0,0]
    results = {}
    index = 0
    path = cutechess_cli_path + '/' + log_file_name

    INVALID = -1

    if (os.path.isfile(path) == None):
        print(str(n))
        exit(-1)

    #Finished game 14 (Arasan-Base vs XboardEngine): 1-0 {Black resigns}
    pat=re.compile("^Finished game ([0-9]+) \(.+\): ([0-9-\/]+) {(.*)}")

    results = []
    for i in range(MAX_RESULTS):
        results.append(INVALID)

    with open(path) as f:
        maxgame = 0
        for line in f:
            m = pat.match(line)
            if m != None:
                game = int(m.group(1))
                result = m.group(2)
                result_kind = m.group(3)
                if (result_kind.find('disconnect') != -1 or result_kind.find('stall') != -1):
                    continue
                r = 0.5
                if (result == "0-1"):
                   r = 0
                elif (result == "1-0"):
                   r = 1
                results[game] = int(2*r)
                if (game>maxgame):
                    maxgame = game
        for game in range(0,maxgame,2):
            p = [results[game+1],results[game+2]]
            if p[0] != INVALID and p[1] != INVALID:
                idx = p[0] + (2-p[1])
                n[idx] = n[idx] + 1
    for i in range(0,4):
        print(n[i],end=" ")
    print(n[4])

if __name__ == "__main__":
    sys.exit(main())
