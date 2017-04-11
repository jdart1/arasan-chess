#!/usr/bin/python3
# -*- coding: utf-8 -*-

import sys, subprocess, math, time, threading
from subprocess import Popen, PIPE, call

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

# list of hosts, adjust as needed
hosts = ['sassy','puppy','wombat','localhost']

scores = [0,0,0]

limit = 0

class Monitor:

# following code is from Michel Van Den Bergh
   def LL(self,x):
      return 1/(1+10**(-x/400))
   
   def LLR(self,W,D,L,elo0,elo1):
# avoid division by zero
      """
      This function computes the log likelihood ratio of H0:elo_diff=elo0 versus
      H1:elo_diff=elo1 under the logistic elo model
      
      expected_score=1/(1+10**(-elo_diff/400)).
      
      W/D/L are respectively the Win/Draw/Loss count. It is assumed that the outcomes of
      the games follow a trinomial distribution with probabilities (w,d,l). Technically
      this is not quite an SPRT but a so-called GSPRT as the full set of parameters (w,d,l)
      cannot be derived from elo_diff, only w+(1/2)d. For a description and properties of
      the GSPRT (which are very similar to those of the SPRT) see
      
      http://stat.columbia.edu/~jcliu/paper/GSPRT_SQA3.pdf
      
      This function uses the convenient approximation for log likelihood
      ratios derived here:
      
      http://hardy.uhasselt.be/Toga/GSPRT_approximation.pdf
      
      The previous link also discusses how to adapt the code to the 5-nomial model
      discussed above.
      """
      if W==0 or D==0 or  L==0:
         return 0.0
      N=W+D+L
      w,d,l=W/N,D/N,L/N
      s=w+d/2
      m2=w+d/4
      var=m2-s**2
      var_s=var/N
      s0=self.LL(elo0)
      s1=self.LL(elo1)
      return (s1-s0)*(2*s-s0-s1)/var_s/2.0
 
   def SPRT(self,W,D,L,elo0,elo1,alpha,beta,games):
      """
      This function sequentially tests the hypothesis H0:elo_diff=elo0 versus
      the hypothesis H1:elo_diff=elo1 for elo0<elo1. It should be called after
      each game until it returns either 'H0' or 'H1' in which case the test stops
      and the returned hypothesis is accepted.

      alpha is the probability that H1 is accepted while H0 is true
      (a false positive) and beta is the probability that H0 is accepted
      while H1 is true (a false negative). W/D/L are the current win/draw/loss
      counts, as before.
      """
      LLR_=self.LLR(W,D,L,elo0,elo1)
      LA=math.log(beta/(1-alpha))
      LB=math.log((1-beta)/alpha)
      print("LLR=" + "{0:.2f}".format(round(LLR_,2)) + " [" + \
          "{0:.2f}".format(round(LA,2)) + "," + \
          "{0:.2f}".format(round(LB,2)) + "] (" + str(games) + " games)")
      if LLR_>LB:
          return 'H1'
      elif LLR_<LA:
          return 'H0'
      else:
          return ''

   def get_status(self,host,scores):
      global SSH_CMD, REMOTE_EXEC_SCRIPT
      if (host == 'localhost'):
         cmd = ""
      else:
         cmd = SSH_CMD + ' -f ' + host + ' '
         
      proc = Popen(cmd + REMOTE_EXEC_SCRIPT, stdout=PIPE, shell=True)
      status_stdout = proc.communicate()[0]
      for line in status_stdout.splitlines():
#         print("received " + line.decode())
         i = 0
         for chunk in line.decode().split(" "):
            scores[i] = scores[i] + int(chunk)
            i = i + 1
      proc.wait()    

   def run(self):
      global scores, STOP_SCRIPT, limit
      result = ""
      while(limit > 0 or len(result)==0):
         games = 0   
         time.sleep(180)
         for i in range(0,3):
            scores[i] = 0
         for host in hosts:
#            print("querying " + host)
            self.get_status(host,scores)
         for i in range(0,3):
            games = games + scores[i]
#         print("games= " + str(games))   
         if (games == 0):
            continue
         # Note: scores are W L D but from the perspective of the
         # base version. So L W D from the perspective of the version
         # being tested
         W = scores[1]
         L = scores[0]
         D = scores[2]
         result = self.SPRT(W,D,L,-1.5,4.5,0.05,0.05,games)
         print("result = %s" % result)
         if (limit > 0 and games >= limit):
            break
      subprocess.call(STOP_SCRIPT,shell=True)

def main(argv = None):
    if argv is None:
        argv = sys.argv[1:]

    global limit
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

    # start thread
    mon = Monitor()
    t = threading.Thread(target=mon.run)
    t.start()

if __name__ == "__main__":
    sys.exit(main())
