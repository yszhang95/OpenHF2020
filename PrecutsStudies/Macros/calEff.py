#!/usr/bin/env python3
import ROOT as r
f = r.TFile.Open("outputHists.root")
#f.ls()

varLabels = (
  "piplus_dxySig",
  "piminus_dxySig",
  "piplus_dzSig",
  "piminus_dzSig",
  "dca_pipi",
  "vtxChi2_Ks",
  "dlSig_Ks",
  "cosAlpha3D_Ks",
  "massDiff_Ks"
)

ptLabels = range(0,2)

r.gROOT.LoadMacro("helpers.cc+")

gEffs = {}

for ipt in ptLabels:
  for ivar in varLabels:
    hS =  f.Get("hSPt%d_%s" % (ipt, ivar))
    hB =  f.Get("hBPt%d_%s" % (ipt, ivar))
    gEffs[ivar] = (r.calEff(hS, hB))

ofile = r.TFile.Open("outputEff.root", "recreate")
for (k,v) in gEffs.items():
  for i in v:
    i.Write()
