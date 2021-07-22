#!/usr/bin/env python
import argparse
parser = argparse.ArgumentParser(description='Convert ParticleTree to simplified ParticleNTuple.')
parser.add_argument('-i', dest='inputList', help='input file list', type=str)
parser.add_argument('--maxEvents', dest='maxEvents', help='number of events', type=int, default=-1)
parser.add_argument('--treeDir', dest='treeDir', help='TDirectory for ParticleTree', type=str, default='lambdacAna_mc')
parser.add_argument('--outdir', dest='outdir', help='output directory', type=str, default='.')
parser.add_argument('--suffix', dest='suffix', help='suffix', type=str, default='LamCKsP')
parser.add_argument('--saveMatchedOnly', dest='saveMatchedOnly', help='only save matched if true', type=bool, default=True)
args = parser.parse_args()

print ('The input file list is', args.inputList)

import ROOT as r
r.gROOT.LoadMacro('genMatch.cc+')
r.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')

Ks = r.Particle(310)
Ks.selfConj(True);
Ks.longLived(True)
LambdaC = r.Particle(4122)
LambdaC.addDaughter(Ks)
LambdaC.addDaughter(r.Particle(+2212))

kinsCut = r.KineCut(0., 1000., 0., 1000)

ts = r.TStopwatch()
ts.Start()
r.genMatchFS(args.inputList, args.treeDir, args.suffix, args.outdir, LambdaC, kinsCut, args.maxEvents, args.saveMatchedOnly)
ts.Stop()
ts.Print()
