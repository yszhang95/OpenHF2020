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
df = ROOT.RDataFrame(args.treeDir+"/ParticleNTuple", args.inputFile)
df_skim = df.Filter(args.cuts)
df_skim.Snapshot(args.treeDir+"/ParticleNTuple", args.inputFile.replace(".root", "_slimmed.root"))

# you could also do with TTree::CopyTree(args.cuts)
# 1. retrieve the tree
# 2. create the output file
# 3. call CopyTree
# 4. write the tree
