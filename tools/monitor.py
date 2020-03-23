#!/usr/bin/python3
# -*- coding: utf-8 -*-

# Copyright 2019, 2020 by Jon Dart. All Rights Reserved.
#
import json, sys, subprocess, math, time, threading
from subprocess import Popen, PIPE, call
# copy of Fishtest stats module fromm http://www.github.com/jdart1/stats
# copy because it is a nested package in Fishtest
from stats import stat_util

# Script to monitor cutechess logs on multiple machines and
# compute SPRT.
# Normally terminates all matches when significance reacheed.
# with -n argument, will run for a fix number of matches and
# then terminate, whether or not result is sigificant.

SHELL='/bin/bash'

# command to execute ssh
SSH_CMD = 'ssh -i /home/jdart/.ssh/id_rsa'

# script to call to get status
REMOTE_EXEC_SCRIPT = 'python3 /home/jdart/tools/match_status.py'

# script to stop matches
STOP_SCRIPT = '/home/jdart/tools/stop-all'

POLL_INTERVAL = 180

scores = [0,0,0]

ELO_MODEL='logistic'

class Monitor:

   def get_status(self,host,scores):
      global SSH_CMD, REMOTE_EXEC_SCRIPT
      if (host == 'localhost'):
         cmd = ""
      else:
         cmd = SSH_CMD + ' -f ' + host + ' '

      proc = Popen(cmd + REMOTE_EXEC_SCRIPT, stdout=PIPE, shell=True)
      status_stdout = proc.communicate()[0]
#      print("host="+host)
#      tmp = [scores[0], scores[1], scores[2]]
      for line in status_stdout.splitlines():
#         print("received " + line.decode())
         i = 0
         for chunk in line.decode().split(" "):
            scores[i] = scores[i] + int(chunk)
            i = i + 1
#      print("scores=      " + str(scores[0]-tmp[0]) + " " + str(scores[1]-tmp[1]) + " " + str(scores[2]-tmp[2]))
      proc.wait()

   def run(self,machines,limit):
      global scores, STOP_SCRIPT, ELO_MODEL, POLL_INTERVAL
      alpha = 0.05
      beta = 0.05
      LA=math.log(beta/(1-alpha))
      LB=math.log((1-beta)/alpha)
      result = ""
      while(limit > 0 or len(result)==0):
         games = 0
         time.sleep(POLL_INTERVAL)
         for i in range(0,3):
            scores[i] = 0
         for host in machines:
            self.get_status(host['hostname'],scores)
         for i in range(0,3):
            games = games + scores[i]
         if (games == 0):
            continue
         R = {}
         R['wins'] = scores[0]
         R['losses'] = scores[1]
         R['draws'] = scores[2]
         print("W/L/D=" + str(R['wins']) + '/' + str(R['losses'])+ '/' + str(R['draws']))
         stat_result = stat_util.SPRT_elo(R,alpha,beta,0.05,0,5.0,ELO_MODEL)
         LLR = float(stat_result['LLR'])
         print("LLR=" + "{0:.2f}".format(round(LLR,2)) + " [" + \
               "{0:.2f}".format(round(LA,2)) + "," + \
               "{0:.2f}".format(round(LB,2)) + "] (" + str(games) + " games)")
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

    limit = 1000000
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
    t = threading.Thread(target=mon.run,args=(machines,limit,))
    t.start()

if __name__ == "__main__":
    sys.exit(main())
