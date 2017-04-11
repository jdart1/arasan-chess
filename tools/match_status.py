#!/usr/bin/python3
# -*- coding: utf-8 -*-

import re, os, sys, time

# checks status of cutechess match on a host

# Cutechess bin directory
cutechess_cli_path = '/home/jdart/chess/cutechess-cli'

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
    global cutechess_cli_path
    dir = cutechess_cli_path

    pat = re.compile('^.+\/match([0-9]+)\.log$')
    score_pat = re.compile('^Score.+: ([0-9]+) \- ([0-9]+) \- ([0-9]+).+$')

    filenames = next(os.walk(dir))[2]
    sums = [0,0,0]
    results = {}
    index = 0
    for fn in filenames:
       path = dir + '/' + fn 
       if (pat.match(path) != None): 
#           print (path)
           lines = read_last(path)
           if (lines != None):
               for aline in lines:
                   mat = score_pat.match(aline)
                   if (mat != None):
                       results[index] = mat.groups()
                       index = index + 1
                       break

    for key in results.keys():
        scores = results[key]
        for i in range(0,3):
            sums[i] = sums[i] + int(scores[i])

    print("%s %s %s" % (str(sums[0]), str(sums[1]), str(sums[2])))

if __name__ == "__main__":
    sys.exit(main())
