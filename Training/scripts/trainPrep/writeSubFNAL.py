#!/usr/bin/env python3
import argparse
parser = argparse.ArgumentParser(description='generate condor configuration.')
parser.add_argument('-i', dest='inputCert', help='input certificate', type=str)
parser.add_argument('--exe', dest='userexe', help='executable', type=str)

parser.add_argument('--boost', dest='boost', help='pPb vs Pbp', type=str)
parser.add_argument('--dataset', dest='dataset', help='dataset, e.g. dataHM1, dataMB1', type=str)
#parser.add_argument('--triggerIndex', dest='triggerIndex', help='trigger index', type=int, default=2)
parser.add_argument('--filterIndex', dest='filterIndex', help='filter index', type=int, default=4)
parser.add_argument('--site', dest='site', help='destination: lxplus or fnal (default)', type=str, default="fnal")

args = parser.parse_args()

#dest = ""
#if args.site == "lxplus":
    #dest = "root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/AppMVA/%s_%s" % (args.dataset, args.boost)
#elif args.site == "fnal":
    #dest = "root://cmseos.fnal.gov///store/user/yousen//RiceHIN/OpenHF2020_LamCKsP/AppMVA/%s_%s" % (args.dataset, args.boost)


import os
import subprocess
import XRootD.client

mylists = None
trigIndex = None

myclient = XRootD.client.FileSystem("root://eoscms.cern.ch")
mystat, mydir = myclient.dirlist("/store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV2/")

if "HM" in args.dataset:
    trigIndex = 2
    mystat, mydir = myclient.dirlist("/store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV3/")
    mylists = [item.name for item in mydir]
elif "MB" in args.dataset:
    trigIndex = 4
    mystat, mydir = myclient.dirlist("/store/group/phys_heavyions/yousen/OpenHF2020Storage/SplitFileListsV4/")
    mylists = [item.name for item in mydir]

myexe = args.userexe
logdir = "log_%s_%s_trig%d_filter%d_%s" % (args.dataset, args.boost, trigIndex, args.filterIndex, myexe.replace(".sh", ""))
proxy_path = "/afs/cern.ch/user/y/yousen/private/$(Proxy_filename)" if args.site == "lxplus" else "$(Proxy_filename)"

dest = "root://eoscms.cern.ch//store/group/phys_heavyions/yousen/OpenHF2020Storage/TrainPrep/%s" % (logdir.replace("log", "trainPrep"))

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
''' % (os.path.basename(args.inputCert), proxy_path, args.userexe, logdir, logdir ,logdir)

num = 0
for l in mylists:
    if args.boost == "pPb" and not "pPb" in l:
        continue
    if args.boost == "Pbp" and not "Pbp" in l:
        continue
    if not args.dataset in l:
        continue
    cmd += "#%d" % num
    cmd += '''
Arguments = $(Proxy_filename) %s %s
''' % (l, dest)
    cmd +='queue\n'
    num = num+1

#print (cmd)

with open("sub_%s_%s_trig%d_filter%d_%s.jdl" % (args.dataset, args.boost, trigIndex, args.filterIndex, myexe.replace(".sh", "")), 'w') as f:
    print("Opened", f.name)
    f.write(cmd)
    f.close
    subprocess.run(['mkdir', logdir])
