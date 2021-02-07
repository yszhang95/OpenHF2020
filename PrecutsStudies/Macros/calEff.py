#!/usr/bin/env python3
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
