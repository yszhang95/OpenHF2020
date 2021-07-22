#!/usr/bin/env python
import argparse
parser = argparse.ArgumentParser(description='Add a branch of weight to existing tree.')
parser.add_argument('-i', dest='inputFile', help='input file', type=str)
parser.add_argument('--treeDir', dest='treeDir', help='TDirectory for ParticleTree', type=str, default='lambdacAna')
parser.add_argument('--outdir', dest='outdir', help='output directory', type=str, default='.')
parser.add_argument('--weight', dest='weight', help='global weight to multiply', type=float, default=1)
parser.add_argument('--uselumi', dest='uselumi', help='divide weight by hltRecordLumi or not', type=bool, default=False)
args = parser.parse_args()
print ('The input file is', args.inputFile)

outputFile = args.inputFile.replace('.root', '_weighted.root')

import ROOT
df = ROOT.RDataFrame(args.treeDir+"/ParticleNTuple", args.inputFile)
#print(df.GetColumnNames())
eqn = "%f/hltRecordLumi" % args.weight if args.uselumi else "%f" % args.weight
newdf = df.Define("weight", eqn)
newdf.Snapshot(args.treeDir+"/ParticleNTuple", outputFile)
