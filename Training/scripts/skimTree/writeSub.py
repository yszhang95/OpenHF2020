#!/usr/bin/env python3
import argparse
parser = argparse.ArgumentParser(description='generate condor configuration.')
parser.add_argument('-i', dest='inputCert', help='input certificate', type=str)
args = parser.parse_args()

import os
import subprocess

if not os.path.exists('lists'):
  subprocess.run(['mkdir', 'lists'])
if not os.path.exists('log'):
  subprocess.run(['mkdir', 'log'])

sample = ['MC', 'data']
for s in sample:
  for boost in ["pPb", "Pbp"]:
    n = 5 if s == "data" else 15
    runargs = ['split', '-d', '-l%d' % n, '%s_%s.list' % (s, boost), 'lists/%s_%s.list' % (s, boost)]
    subprocess.run(runargs)
mylists = os.listdir('lists')
#print (mylists)

cmd='''
# this is config for submitting condor jobs
Proxy_filename = %s
Proxy_path = /afs/cern.ch/user/y/yousen/private/$(Proxy_filename)

Universe   = vanilla
Executable = run.sh
Log        = log/$(Cluster)_$(Process).log
Output     = log/$(Cluster)_$(Process).out
Error      = log/$(Cluster)_$(Process).err
should_transfer_files = YES
when_to_transfer_output = ON_EXIT
transfer_output_files = ""
#requirements = (OpSysAndVer =?= "CntOS7")
+JobFlavour = "espresso"
max_transfer_output_mb = 2000
''' % args.inputCert

num = 0
for s in sample:
  for boost in ["pPb", "Pbp"]:
    n = 0
    for l in mylists:
      if "%s_%s" % (s, boost) in l:
        n = n+1
    for i in range(n):
      cmd += "#%d" % num
      cmd += '''
Arguments = $(Proxy_filename) %s %s_%s.list%02d root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage
''' % (s, s, boost, i)
      cmd +='''transfer_input_files = $(Proxy_path),lists/%s_%s.list%02d
queue
''' % (s, boost, i)

      num = num+1

#print (cmd)
with open("sub.jdl", 'w') as f:
  f.write(cmd)
  f.close
#subprocess.run(['ls'])
