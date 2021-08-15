#!/usr/bin/env python

import argparse
parser = argparse.ArgumentParser(description='Apply cuts on ParticleNTuple.')
parser.add_argument('-i', dest='inputFile', help='input file', type=str)
parser.add_argument('--treeDir', dest='treeDir', help='TDirectory for ParticleNTuple', type=str, default='lambdacAna')
parser.add_argument('--outdir', dest='outdir', help='output directory', type=str, default='.')
parser.add_argument('--cuts', dest='cuts', help='cuts on TTree', type=str, default="")
args = parser.parse_args()

print ('The input file list is', args.inputFile)

import ROOT
# one method
ROOT.gSystem.Load('${OPENHF2020TOP}/Utilities/lib/libMyTreeReader.so')
ROOT.gInterpreter.AddIncludePath("%s/Utilities/Ana/" % ROOT.gSystem.Getenv("OPENHF2020TOP"))
ROOT.gInterpreter.ProcessLine('#include "Common.h"')
ROOT.gInterpreter.Declare("DeDxSelection selectDeDx;")
# alternative way
#ROOT.gInterpreter.AddIncludePath("%s/Training/Macros/" % ROOT.gSystem.Getenv("OPENHF2020TOP"))
#ROOT.gInterpreter.ProcessLine('#include "SelectDeDx.cc"')

df = ROOT.RDataFrame(args.treeDir+"/ParticleNTuple", args.inputFile)
if args.cuts == "":
  args.cuts = "true"
df_preskim = df.Filter(args.cuts)
#df_skim = df_preskim.Define("cand_dau1_p", "cosh(cand_etaDau1) * cand_pTDau1").Filter("passDeDx(cand_dau1_p, trk_dau1_dEdx_dedxHarmonic2)")
df_skim = df_preskim.Define("cand_dau1_p", "cosh(cand_etaDau1) * cand_pTDau1").Filter("selectDeDx(cand_dau1_p, trk_dau1_dEdx_dedxHarmonic2)")
df_skim.Snapshot(args.treeDir+"/ParticleNTuple", args.inputFile.replace(".root", "_slimmed.root"))

# you could also do with TTree::CopyTree(args.cuts)
# 1. retrieve the tree
# 2. create the output file
# 3. call CopyTree
# 4. write the tree
