#!/usr/bin/env python3
import argparse
parser = argparse.ArgumentParser(description='generate condor configuration.')
parser.add_argument('-i', dest='inputCert', help='input certificate', type=str)
parser.add_argument('--exe', dest='userexe', help='executable', type=str)
parser.add_argument('--boost', dest='boost', help='pPb vs Pbp', type=str)
parser.add_argument('--dataset', dest='dataset', help='dataset, e.g. dataHM1, dataMB1', type=str)
parser.add_argument('--site', dest='site', help='destination: lxplus (default) or fnal', type=str, default="lxplus")
parser.add_argument('--pTMin', dest='pTMin', help='min pt', type=float)
parser.add_argument('--pTMax', dest='pTMax', help='max pt', type=float)
parser.add_argument('--transferCert', dest='transferCert', help='transfer cert', type=int, default=0)
args = parser.parse_args()
dest = ""
if args.site == "lxplus":
    dest = "root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/AppMVA/%s_%s/pT_%.1f_%.1f" % \
        (args.dataset, args.boost, args.pTMin, args.pTMax)
elif args.site == "fnal":
    dest = "root://cmseos.fnal.gov///store/user/yousen//RiceHIN/OpenHF2020_LamCKsP/AppMVA/%s_%s/pT_%.1f_%.1f" % \
        (args.dataset, args.boost, args.pTMin, args.pTMax)

import os
import subprocess

mylists = None

import XRootD.client

myclient = XRootD.client.FileSystem("root://eoscms.cern.ch")

if "HM" in args.dataset:
    mystat, mydir = myclient.dirlist("/store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV3/")
    mylists = [item.name for item in mydir]
elif "MB" in args.dataset:
    mystat, mydir = myclient.dirlist("/store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV4/")
    mylists = [item.name for item in mydir]

myexe = args.userexe

logdir = "log_%s_%s_%s" % (args.dataset, args.boost, myexe.replace(".sh", ""))
proxy_path = "/afs/cern.ch/user/y/yousen/private/$(Proxy_filename)" if args.transferCert != 0 else "$(Proxy_filename)"
cmd='''
# this is config for submitting condor jobs
Proxy_filename = %s
Proxy_path = %s
Universe   = vanilla
Executable = %s
Log        = %s/$(Cluster)_$(Process).log
Output     = %s/$(Cluster)_$(Process).out
Error      = %s/$(Cluster)_$(Process).err
should_transfer_files = YES
when_to_transfer_output = ON_EXIT
transfer_output_files = ""
#+JobFlavour = "workday"
''' % (os.path.basename(args.inputCert), proxy_path, args.userexe, logdir, logdir ,logdir)

num = 0
for l in mylists:
    if args.boost == "pPb" and not "pPb" in l:
        continue
    if args.boost == "Pbp" and not "Pbp" in l:
        continue
    if not args.dataset in l:
        continue
    cmd += "#Process %d" % num
    cmd += "#List %d" % num
    cmd += '''
Arguments = $(Proxy_filename) %s %s
''' % (l, dest)
    transfer_input_files = "transfer_input_files = dataset" if args.transferCert == 0 else "transfer_input_files = dataset,$(Proxy_path)"
    cmd += transfer_input_files + "\n"
    cmd +='queue\n'
    num = num+1

#print (cmd)
with open("sub_%s_%s_%s.jdl" % (args.dataset, args.boost, myexe.replace(".sh", "")), 'w') as f:
    print("Created", f.name)
    f.write(cmd)
    f.close
    subprocess.run(['mkdir', logdir])
    print("log dir is ", logdir)
    #subprocess.run(['condor_submit', f.name])
    #print (f.name)
