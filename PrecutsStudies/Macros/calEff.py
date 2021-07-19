#!/usr/bin/env python3
import argparse

parser = argparse.ArgumentParser(description=
'''
This is a script in step 2.
It makes use of histograms created in step 1 and
generate efficiency histograms in used step 3.
Requirements: RDataFrame and PyROOT on Python3
Test platform: CentOS7
               Python 3.6.8
               GCC 4.8.5 20150623 (Red Hat 4.8.5-44)
               ROOT 6.22/06
''', formatter_class=argparse.RawTextHelpFormatter)

args = parser.parse_args()

import ROOT as r
r.gROOT.LoadMacro("helpers.cc+")

f = r.TFile.Open("outputHists.root")
#f.ls()

varLabels = {
    "piplus_dxySig" : r.Direction.larger,
    "piminus_dxySig" : r.Direction.larger,
    "piplus_dzSig" : r.Direction.larger,
    "piminus_dzSig" : r.Direction.larger,
    "dca_pipi" : r.Direction.less,
    "vtxChi2_Ks" : r.Direction.less,
    "dlSig_Ks" : r.Direction.larger,
    "cosAlpha3D_Ks" : r.Direction.larger,
    "massDiff_Ks" : r.Direction.less
  }

ptLabels = range(0,2)

gEffs = {}

for ipt in ptLabels:
  for k,v in varLabels.items():
    hS =  f.Get("hSPt%d_%s" % (ipt, k))
    hB =  f.Get("hBPt%d_%s" % (ipt, k))
    gEffs["pT"+str(ipt)+k] = (r.calEff(hS, hB, v))

ofile = r.TFile.Open("outputEff.root", "recreate")
for (k,v) in gEffs.items():
  for i in v:
    i.Write()
