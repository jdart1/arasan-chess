#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright 2019, 2020 by Jon Dart. All Rights Reserved.
#
import json, sys, subprocess, math, time, threading
from subprocess import Popen, PIPE, call
# copy of Fishtest stats module fromm http://www.github.com/jdart1/stats
# copy because it is a nested package in Fishtest
from stats import sprt, LLRcalc, stat_util

# Script to monitor cutechess logs on multiple machines and
# compute SPRT.
# Normally terminates all matches when significance reacheed.
# with -n argument, will run for a fix number of matches and
# then terminate, whether or not result is sigificant.

SHELL='/bin/bash'

# command to execute ssh
SSH_CMD = 'ssh -i /home/jdart/.ssh/id_rsa'

# script to call to get status
MATCH_STATUS_SCRIPT = 'python3 /home/jdart/tools/match_status.py'

# script to stop matches
STOP_SCRIPT = '/home/jdart/tools/stop-all'

class Monitor:

    scores = [0,0,0,0,0]

    def get_status(self,host,scores):
        global SSH_CMD, MATCH_STATUS_SCRIPT
        if (host == 'localhost'):
            cmd = ""
        else:
            cmd = SSH_CMD + ' -f ' + host + ' '

        proc = Popen(cmd + MATCH_STATUS_SCRIPT, stdout=PIPE, shell=True)
        status_stdout = proc.communicate()[0]
        for line in status_stdout.splitlines():
           i = 0
           for chunk in line.decode().split(" "):
               scores[i] = scores[i] + int(chunk)
               i = i + 1
        proc.wait()

    def run(self,machines,limit,poll_interval):
        global STOP_SCRIPT
        result = ""
        alpha = 0.05
        beta = 0.05
        elo0 = -0.5
        elo1 = 1.5
        while(limit > 0 or len(result)==0):
            games = 0
            time.sleep(poll_interval)
            for i in range(0,5):
                 self.scores[i] = 0
            for host in machines:
                 self.get_status(host['hostname'],self.scores)
            self.scores = [665,4413,8938,4490,680]
            for i in range(0,4):
                 print(str(self.scores[i]),end=" ")
            print(str(self.scores[4]))
            # get count of games - each score is for a pair so must mult x2
            score = 0.0;
            for i in range(0,5):
                games = games + 2*self.scores[i]
                score += i*self.scores[i]/2.0;
            if (games == 0):
                continue
            score = 100.0*score/games
            scores_reg=LLRcalc.regularize(self.scores)
            s = sprt.sprt()
            s.set_state(scores_reg)
            a5=s.analytics()
            LA=a5['a']
            LB=a5['b']
            LLR=LLRcalc.LLR_logistic(elo0,elo1,self.scores)
            stats = []
            for stat in (LLR,LA,LB,score):
                 stats.append("{0:.2f}".format(round(stat,2)))
            print('LLR=%s [%s, %s] %d games, score: %s%%' %(stats[0], stats[1], stats[2], games, stats[3]))
            if LLR>LB:
                result = 'H1'
            elif LLR<LA:
                result = 'H0'
            if len(result)>0:
                print("result=" + result)
            if ((len(result)>0) or (limit > 0 and games >= limit)):
                break
        subprocess.call(STOP_SCRIPT,shell=True)

def main(argv = None):
    if argv is None:
        argv = sys.argv[1:]

    limit = 25000
    poll_interval = 180
    arg = 0
    while ((arg < len(argv)) and (argv[arg][0:1] == '-')):
        if (argv[arg][1] == 'n'):
            arg = arg + 1
            if (arg < len(argv)):
                try:
                    limit = int(argv[arg])
                except exceptions.ValueError:
                    print(('Invalid value for parameter %s: %s' % (argv[i], argv[i + 1])),file=sys.stderr)
                    return
                arg = arg + 1
            else:
               print("expected number after -n")
               return
        elif (argv[arg][1] == 'p'):
            arg = arg + 1
            if (arg < len(argv)):
                try:
                    poll_interval = int(argv[arg])
                except exceptions.ValueError:
                    print(('Invalid value for parameter %s: %s' % (argv[i], argv[i + 1])),file=sys.stderr)
                    return
                arg = arg + 1
            else:
               print("expected number after -p")
               return
        else:
            print("Unrecognized switch: " + argv[arg], file=sys.stderr)
            return

    try:
        with open('machines.json', 'r') as machineFile:
            data=machineFile.read()
    except:
        print("machine file not found or could not be opened",file=sys.stderr)
        exit(1)

    machines = []

    try:
        machines = json.loads(data)
    except:
        print("failed to parse machine file",file=sys.stderr)
        exit(1)

    # start thread
    mon = Monitor()
    t = threading.Thread(target=mon.run,args=(machines,limit,poll_interval,))
    t.start()

if __name__ == "__main__":
    sys.exit(main())
