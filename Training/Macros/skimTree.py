#!/usr/bin/env python3
import argparse
parser = argparse.ArgumentParser(description='Convert ParticleTree to simplified ParticleNTuple.', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-i', '--inputList', help='input file list', type=str)
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
parser.add_argument('--triggerIndex', dest='triggerIndex', help='trigger index', type=int, default=2)
parser.add_argument('--filterIndex', dest='filterIndex', help='filter index', type=int, default=4)
parser.add_argument('--effFile', dest='effFile', help='file saving efficiency', type=str, default="")
parser.add_argument('--NtrkLow', dest='NtrkLow', help='lowest multiplicity', type=int, default=0)
parser.add_argument('--NtrkHigh', dest='NtrkHigh', help='highest multiplicity', type=int, default=1000)
parser.add_argument('--wantAbort', help='want abort', action='store_const', const=True, default=False)
parser.add_argument('--branches', help='kept branches, either in the file branches.list like input list or in format of pT,eta,mass separated in comma', type=str, default="")

args = parser.parse_args()
print ('The input file list is', args.inputList)

if args.isMC and args.treeDir == "lambdacAna":
    args.treeDir = "lambdacAna_mc"

import ROOT as r
failed = r.gROOT.LoadMacro('${OPENHF2020TOP}/Training/Macros/skimTree.cc+')
if not failed:
    print("Loaded and compiled macro skimTree.cc")
else:
    print("Failed loading macro skimTree.cc")
r.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')

MassPDG = 2.2865 # GeV
kinsCut = r.KineCut(args.pTMin, args.pTMax, 0., 1000, args.absYMin, args.absYMax, MassPDG, args.absMassLw, args.absMassUp)

conf = r.Config(args.isMC, kinsCut)
conf.SetInputList(args.inputList)
conf.SetNEntries(args.maxEvents)
conf.SetTreeDir(args.treeDir)
conf.SetOutDir(args.outdir)
conf.SetPostfix(args.suffix)
conf.SetSaveMatchedOnly(not args.saveAll)
conf.SetSelectDeDx(args.selectDeDx)
conf.SetDeDxSelection((0.7, 1.5, 0.75, 1.25))
conf.SetFlipEta(args.flipEta)
conf.SetTriggerIndex(args.triggerIndex)
conf.SetFilterIndex(args.filterIndex)
conf.SetWantAbort(args.wantAbort)

kept = []
if args.branches:
    if args.branches.find(".list") <0:
        kept = [s.strip() for s in args.branches.split(',')]
    else:
        # this means it is a file contianing the list of branches, separated in "\n"
        with open(args.branches) as f:
            kept = [e.strip() for e in f.readlines()]

else:
    kept = ["cand_mass", "cand_pT", "cand_y", "cand_eta", "cand_phi",
     "cand_massDau0", "cand_etaDau0", "cand_pTDau0", "cand_phiDau0",
     "cand_massDau1", "cand_etaDau1", "cand_pTDau1", "cand_phiDau1",
     "trk_dau1_dEdxRes", "trk_dau1_dEdx_dedxHarmonic2",
     "cand_angle3D", "cand_dauCosOpenAngle3D",
     "cand_dau_dR", "cand_dau0_angle3D",
     "cand_dau0_decayLength3D", "cand_dau0_decayLengthError3D", "cand_dau0_dca",
     "cand_Ntrkoffline",
     "trigBit_2", "trigBit_4", "filterBit_4", "filterBit_5"]
    print("No branches given; will use default information", kept)

if args.effFile:
    branches.append("eventWeight")
conf.SetKeptBranchNames(
    kept
)
conf.NtrkLow(args.NtrkLow)
conf.NtrkHigh(args.NtrkHigh)
conf.SetEffGraphName("Ntrk185")
conf.SetEffGraphType("TGraphAsymmErrors")
conf.SetEffFileName(args.effFile)

Ks = r.Particle(310)
Ks.selfConj(True);
Ks.longLived(True)
LambdaC = r.Particle(4122)
LambdaC.addDaughter(Ks)
LambdaC.addDaughter(r.Particle(+2212))


ts = r.TStopwatch()
ts.Start()
print (conf.isMC())
r.skimTree(conf, LambdaC)
ts.Stop()
ts.Print()
