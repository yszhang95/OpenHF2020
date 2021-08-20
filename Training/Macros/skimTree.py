#!/usr/bin/env python
import argparse
parser = argparse.ArgumentParser(description='Convert ParticleTree to simplified ParticleNTuple.', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-i', dest='inputList', help='input file list', type=str)
parser.add_argument('--pTMin', dest='pTMin', help='minimum pT', type=float, default=0.9)
parser.add_argument('--pTMax', dest='pTMax', help='maximum pT', type=float, default=20)
parser.add_argument('--absMassLw', dest='absMassLw', help='minimum distance to PDG mass', type=float, default=-1.)
parser.add_argument('--absMassUp', dest='absMassUp', help='maximum distance to PDG mass', type=float, default=20)
parser.add_argument('--absYMin', dest='absYMin', help='minimum abs(y)', type=float, default=-1.)
parser.add_argument('--absYMax', dest='absYMax', help='maximum abs(y)', type=float, default=1.)
parser.add_argument('--maxEvents', dest='maxEvents', help='number of events', type=int, default=-1)
parser.add_argument('--treeDir', dest='treeDir', help='TDirectory for ParticleTree', type=str, default='lambdacAna')
parser.add_argument('--outdir', dest='outdir', help='output directory', type=str, default='.')
parser.add_argument('--suffix', dest='suffix', help='suffix', type=str, default='LamCKsP')
parser.add_argument('--mc', dest='isMC', help='intput is MC (default: input is data)', action='store_const', const=True, default=False)
parser.add_argument('--flipEta', dest='flipEta', help='flip the (pseudo-) rapidities of particles', action='store_const', const=True, default=False)
parser.add_argument('--saveAll', dest='saveAll', help='Save all candidates when intput is MC (default: not)', action='store_const', const=True, default=False)
parser.add_argument('--selectDeDx', dest='selectDeDx', help='Select candidates using dE/dx (default: not)', action='store_const', const=True, default=False)
args = parser.parse_args()
print ('The input file list is', args.inputList)

if args.isMC and args.treeDir == "lambdacAna":
    args.treeDir = "lambdacAna_mc"

import ROOT as r
r.gROOT.LoadMacro('skimTree.cc+')
r.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')
conf = r.Config(args.isMC)
conf.SetInputList(args.inputList)
conf.SetNEntries(args.maxEvents)
conf.SetTreeDir(args.treeDir)
conf.SetOutDir(args.outdir)
conf.SetPostfix(args.suffix)
conf.SetSaveMatchedOnly(not args.saveAll)
conf.SetSelectDeDx(args.selectDeDx)
conf.SetDeDxSelection((0.7, 1.5, 0.75, 1.25))
conf.SetFlipEta(args.flipEta)

Ks = r.Particle(310)
Ks.selfConj(True);
Ks.longLived(True)
LambdaC = r.Particle(4122)
LambdaC.addDaughter(Ks)
LambdaC.addDaughter(r.Particle(+2212))

MassPDG = 2.2865 # GeV
kinsCut = r.KineCut(args.pTMin, args.pTMax, 0., 1000, args.absYMin, args.absYMax, MassPDG, args.absMassLw, args.absMassUp)

ts = r.TStopwatch()
ts.Start()
print (conf.isMC())
r.skimTree(conf, LambdaC, kinsCut)
ts.Stop()
ts.Print()
