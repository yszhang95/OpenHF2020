#!/usr/bin/env python3
import argparse
parser = argparse.ArgumentParser(description='generate condor configuration.')
parser.add_argument('-i', dest='inputCert', help='input certificate', type=str)
parser.add_argument('--exe', dest='userexe', help='executable', type=str)
args = parser.parse_args()

import os
import subprocess
import XRootD.client


myclient = XRootD.client.FileSystem("root://eoscms.cern.ch")
mystat, mydir = myclient.dirlist("/store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV2/")

mylists = [item.name for item in mydir]


cmd='''
# this is config for submitting condor jobs
Proxy_filename = %s

Universe   = vanilla
Executable = %s
Log        = log/$(Cluster)_$(Process).log
Output     = log/$(Cluster)_$(Process).out
Error      = log/$(Cluster)_$(Process).err
should_transfer_files = YES
when_to_transfer_output = ON_EXIT
transfer_output_files = ""
#requirements = (OpSysAndVer =?= "CntOS7")
#+JobFlavour = "espresso"
#+MaxRunTime = 360
#max_transfer_output_mb = 2000
''' % (args.inputCert, args.userexe)

num = 0
for l in mylists:
    #if "dataHM1" in l:
    #    continue
    #if "dataHM2" in l:
    #    continue
    cmd += "#%d" % num
    cmd += '''
Arguments = $(Proxy_filename) %s root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep
''' % (l)
    cmd +='''queue\n'''
    num = num+1

#print (cmd)
with open("sub.jdl", 'w') as f:
  f.write(cmd)
  f.close
#subprocess.run(['ls'])
