#!/usr/bin/env python3
import argparse
parser = argparse.ArgumentParser(description='generate condor configuration.')
parser.add_argument('-i', dest='inputCert', help='input certificate', type=str)
parser.add_argument('--exe', dest='userexe', help='executable', type=str)
parser.add_argument('--dataset', dest='dataset', help='dataset, e.g. dataHM1, dataMB1', type=str)
parser.add_argument('--boost', dest='boost', help='pPb vs. Pbp', type=str)
args = parser.parse_args()

import os
import subprocess

mylists = ""
if "HM" in args.dataset:
    mylists = os.listdir('/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV3/')
elif "MB" in args.dataset:
    mylists = os.listdir('/eos/cms/store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV4/')

cmd='''
# this is config for submitting condor jobs
Proxy_filename = %s
Proxy_path = /afs/cern.ch/user/y/yousen/private/$(Proxy_filename)

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
+JobFlavour = "workday"
#max_transfer_output_mb = 2000
''' % (args.inputCert, args.userexe)

num = 0
for l in mylists:
    if not args.dataset in l:
        continue
    if not args.boost in l:
        continue
    cmd += "#%d" % num
    cmd += '''
Arguments = $(Proxy_filename) %s root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/RectCuts
''' % (l)
    cmd +='''transfer_input_files = $(Proxy_path)
queue
'''
    num = num+1

with open("sub_%s_%s.jdl" % (args.dataset, args.boost), 'w') as f:
    f.write(cmd)
    f.close
#subprocess.run(['ls'])
