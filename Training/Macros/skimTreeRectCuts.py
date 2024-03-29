#!/usr/bin/env python
import argparse
parser = argparse.ArgumentParser(description='Convert ParticleTree to simplified ParticleNTuple.',\
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-i', dest='inputList', help='input file list', type=str)
parser.add_argument('--maxEvents', dest='maxEvents', help='number of events', type=int, default=-1)
parser.add_argument('--treeDir', dest='treeDir', help='TDirectory for ParticleTree', type=str, default='lambdacAna')
parser.add_argument('--outdir', dest='outdir', help='output directory', type=str, default='.')
parser.add_argument('--suffix', dest='suffix', help='suffix', type=str, default='LamCKsP')
parser.add_argument('--mc', dest='isMC', help='intput is MC (default: input is data)', action='store_const', const=True, default=False)
parser.add_argument('--saveAll', dest='saveAll', help='Save all candidates when intput is MC (default: not)', action='store_const', const=True, default=False)
parser.add_argument('--filterIndex', dest='filterIndex', help='filter index', type=int, default=4)
parser.add_argument('--triggerIndex', dest='triggerIndex', help='trigger index', type=int, default=2)
args = parser.parse_args()
print ('The input file list is', args.inputList)
saveMatchedOnly = not args.saveAll

import ROOT as r
#r.gROOT.LoadMacro('${OPENHF2020TOP}/Training/Macros/skimTreeRectCuts.cc+')
found = r.gSystem.FindDynamicLibrary('skimTreeRectCuts_cc.so', True)
if found == "":
    r.gROOT.LoadMacro('${OPENHF2020TOP}/Training/Macros/skimTreeRectCuts.cc+')
else:
    status = r.gSystem.Load(found)
    if status < 0:
        print ("Not found ", found)
r.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')

Ks = r.Particle(310)
Ks.selfConj(True);
Ks.longLived(True)
LambdaC = r.Particle(4122)
LambdaC.addDaughter(Ks)
LambdaC.addDaughter(r.Particle(+2212))

#kinsCut = r.KineCut(0., 1000., 0., 1000)

ts = r.TStopwatch()
ts.Start()
r.skimTreeRectCuts(args.inputList, args.treeDir, args.suffix, args.outdir, LambdaC,\
                   args.maxEvents, args.isMC, saveMatchedOnly, args.filterIndex, args.triggerIndex)
ts.Stop()
ts.Print()
