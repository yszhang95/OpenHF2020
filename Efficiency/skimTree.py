#!/usr/bin/env python
import argparse
parser = argparse.ArgumentParser(description='Convert ParticleTree to simplified ParticleNTuple.', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-i', dest='inputList', help='input file list', type=str)
parser.add_argument('--maxEvents', dest='maxEvents', help='number of events', type=int, default=-1)
parser.add_argument('--treeDir', dest='treeDir', help='TDirectory for ParticleTree', type=str, default='lambdacAna_mc')
parser.add_argument('--outdir', dest='outdir', help='output directory', type=str, default='.')
parser.add_argument('--suffix', dest='suffix', help='suffix', type=str, default='Gen')
parser.add_argument('--flipEta', dest='flipEta', help='flip the (pseudo-) rapidities of particles', action='store_const', const=True, default=False)
args = parser.parse_args()
print ('The input file list is', args.inputList)

args.treeDir = "lambdacAna_mc"

import ROOT as r
r.gROOT.LoadMacro('saveGenTuple.cc+')
r.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')
conf = r.Config()
conf.SetInputList(args.inputList)
conf.SetNEntries(args.maxEvents)
conf.SetTreeDir(args.treeDir)
conf.SetOutDir(args.outdir)
conf.SetPostfix(args.suffix)
conf.SetFlipEta(args.flipEta)

Ks = r.Particle(310)
Ks.selfConj(True);
#Ks.longLived(True)
Ks.addDaughter(r.Particle(-211));
Ks.addDaughter(r.Particle(+211));
LambdaC = r.Particle(4122)
LambdaC.addDaughter(Ks)
LambdaC.addDaughter(r.Particle(+2212))

ts = r.TStopwatch()
ts.Start()
r.saveGenTuple(conf, LambdaC)
ts.Stop()
ts.Print()
