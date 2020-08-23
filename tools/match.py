#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright 2020 by Jon Dart. All Rights Reserved.
# Released under the MIT license: see doc/license.txt
#
import json, os, sys
from executor.ssh.client import RemoteCommand

# Sets up a match using multiple machine resources

# NPS value (in Mnps) that results in actual time control
# equalling nominal time control. Equivalent to 1-core performance
# on a Xeon 2690v3.
NPS_BASE = 1.33
# command to run matches on a single machine, local or remote
REMOTE_MATCH_DIR = "/home/jdart/chess/cutechess-cli"
REMOTE_MATCH_CMD = "/home/jdart/chess/cutechess-cli/match-sprt"

def scaleTime(timeStr,factor):
    if (timeStr.find(':')==-1):
        # only seconds are given
        time = round(float(timeStr)*factor,2)
    else:
        # minutes & seconds given
        parts = timeStr.split(':')
        minutes = float(parts[0])*factor
        seconds = float(parts[1])*factor
        time = minutes*60+seconds
    # time is in seoonds
    minutes = int(time)/60
    seconds = str(round(time-60*int(minutes),2))
    if minutes==0:
        return str(round(time-60*int(minutes),2))
    else:
        return str(int(minutes)) + ':' + str(round(time-60*int(minutes),2))

def scaleTC(tc,factor):
    start, inc = tc.split('+')
    return scaleTime(start,factor) + '+' + scaleTime(inc,factor)

def main(argv = None):
    
    global NPS_BASE, REMOTE_MATCH_CMD, REMOTE_MATCH_DIR

    if argv is None:
        argv = sys.argv[1:]

    if (len(argv) < 2):
        print("syntax: match <time control> <games>",file=sys.stderr)
        exit(1)

    try:
        with open('machines.json', 'r') as machineFile:
            data=machineFile.read()
    except:
        print("machine file not found or could not be opened",file=sys.stderr)
        exit(1)

    # machine list
    machineList = []

    try:
        machineList = json.loads(data)
    except:
        print("failed to parse machine file",file=sys.stderr)
        exit(1)

    games = argv[0]
    tc = argv[1]

    # start the matches
    for machine in machineList:
        try:
            host = machine['hostname']
            nps = float(machine['nps'])
            cores = int(machine['cores'])
        except KeyError:
            print("warning: expected hostname, nps and cores for machine, not found",file=sys.stderr)
            continue
        # Parse and scale time control
        factor = float(NPS_BASE)/nps
        new_tc = scaleTC(tc,factor)
        # execute the remote or local match script
        try:
            cmd = REMOTE_MATCH_CMD + ' ' + str(games) + ' ' + new_tc + ' ' + str(cores)
            print("starting : host=%s tc=%s" % (host,new_tc))
            cmd = RemoteCommand(host,cmd,capture=True,directory=REMOTE_MATCH_DIR)
            cmd.start()
        except:
            print("error starting command " + cmd,file=sys.stderr)
            traceback.print_tb(tb, limit=None, file=None)
            return 2

if __name__ == "__main__":
    sys.exit(main())
