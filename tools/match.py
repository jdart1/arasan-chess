#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright 2020, 2023 by Jon Dart. All Rights Reserved.
# Released under the MIT license: see doc/license.txt
#
import argparse, json, os, shlex, sys
from executor.ssh.client import RemoteCommand
from subprocess import Popen, PIPE, call

# Sets up a match using multiple machine resources

# NPS value (in Mnps) that results in actual time control
# equalling nominal time control. Equivalent to 1-core performance
# on a Xeon 2690v3.
NPS_BASE = 1.33
REMOTE_MATCH_DIR = "/home/jdart/chess/cutechess-cli"
# command to run matches on a single machine, local or remote
# arguments to match command: base engine (in cutechess config file), test engine, games, tc, cores
REMOTE_MATCH_CMD = "/home/jdart/chess/cutechess-cli/match-sprt"

DEFAULT_BASE_ENGINE="Arasan-Base"
DEFAULT_TEST_ENGINE="Arasan-Test"

class Options:
   base_engine = DEFAULT_BASE_ENGINE
   test_engine = DEFAULT_TEST_ENGINE
   games = 20000
   time_control = "1:0+0.8"

options = Options()

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
    
    global options, NPS_BASE, REMOTE_MATCH_CMD, REMOTE_MATCH_DIR

    if argv is None:
        argv = sys.argv[1:]

    parser = argparse.ArgumentParser(description="run matches across multiple machines")
    parser.add_argument("-b", "--base_engine", type=str,help="engine to compare against")
    parser.add_argument("-g", "--games", type=int,help="max number of games to play")
    parser.add_argument("-t", "--test_engine", type=str,help="engine to test")
    parser.add_argument("-tc", "--time_control", type=str, help="nominal time control")
    parser.parse_args(namespace=options)

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
        factor = float(NPS_BASE)/(nps/810000.0)
        new_tc = scaleTC(options.time_control, factor)
        # execute the remote or local match script
        cmd_string = ""
        try:
            cmd_string = REMOTE_MATCH_CMD + ' ' + options.base_engine + ' ' + options.test_engine + ' ' + str(options.games) + ' ' + new_tc + ' ' + str(cores)
            print("starting : host=%s tc=%s" % (host,new_tc))
            if host == 'localhost':
               process = Popen(shlex.split(cmd_string), shell=True, cwd=REMOTE_MATCH_DIR)
            else:
               cmd = RemoteCommand(host,cmd_string,capture=True,directory=REMOTE_MATCH_DIR)
               cmd.start()
        except:
            print("error starting command " + cmd_string,file=sys.stderr)
            traceback.print_tb(tb, limit=None, file=None)
            return 2

if __name__ == "__main__":
    sys.exit(main())
