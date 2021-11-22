#!/usr/bin/env python
import argparse
parser = argparse.ArgumentParser(description='Merge RooWorkspace')
parser.add_argument('-i', dest='inputList', help='input file list', type=str)
parser.add_argument('--dsName', dest='dsName', help='dataset name', type=str, default='')
parser.add_argument('--wsName', dest='wsName', help='workspace name', type=str, default='')
parser.add_argument('--dsTitle', dest='dsTitle', help='dataset title', type=str, default='')
parser.add_argument('--wsTitle', dest='wsTitle', help='workspace title', type=str, default='')
parser.add_argument('--outdir', dest='outdir', help='output directory', type=str, default='./')
parser.add_argument('--outfile', dest='outfile', help='output file, not directory', type=str, default='merged.root')
parser.add_argument('--reweight', dest='reweight', help='weighted dataset', action='store_const', const=True)
args = parser.parse_args()

import ROOT
ROOT.gSystem.Load("libRooFitCore.so")
ROOT.gSystem.Load("libRooFit.so")
ROOT.gROOT.LoadMacro("MergeWS.cc+")
conf = ROOT.Conf()
conf._inFileList = args.inputList
conf._wsName =  args.wsName
conf._wsTitle = args.wsName if len(args.wsTitle) else args.wsTitle
conf._dsName = args.dsName
conf._dsTitle = args.dsName if len(args.dsTitle) else args.dsTitle
conf._ofile = args.outfile
conf._reweight = args.reweight
ROOT.MergeWS(conf)
