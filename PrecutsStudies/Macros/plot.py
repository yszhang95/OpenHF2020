#!/usr/bin/env python3

import argparse

parser = argparse.ArgumentParser(description=
'''
This is a script in step 3.
It aims to plot efficiency histograms created in step 2.
A directory called plots wil be created to save plots.
Requirements: RDataFrame and PyROOT on Python3
Test platform: CentOS7
               Python 3.6.8
               GCC 4.8.5 20150623 (Red Hat 4.8.5-44)
               ROOT 6.22/06
''', formatter_class=argparse.RawTextHelpFormatter)

args = parser.parse_args()

import ROOT as r
r.gROOT.LoadMacro("helpers.cc+")

f = r.TFile.Open("outputEff.root")

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

gEffS = []
gEffB = []

for ipt in ptLabels:
  for k,v in varLabels.items():
    gEffS.append ( f.Get("gSPt%d_%s" % (ipt, k)) )
    gEffB.append ( f.Get("gBPt%d_%s" % (ipt, k)) )

if r.gSystem.cd("plots"):
  pass
else:
  r.gSystem.mkdir("plots")
  r.gSystem.cd("plots")

r.gROOT.SetBatch()
c = r.TCanvas("cEff", "", 800, 600)
for (s, b) in zip(gEffS, gEffB):
  s.SetLineColor(r.kRed)
  b.SetLineColor(r.kBlue)
  frame = s.GetHistogram()
  frame.GetYaxis().SetRangeUser(0., 1.1)
  frame.GetYaxis().SetTitle("Efficiency")
  direction = r.Direction.larger
  for (k, v) in varLabels.items():
    if k in s.GetName():
      direction = v
      break
  s.Draw("AL")
  b.Draw("L")
  leg = None
  if direction is r.Direction.larger:
    leg = r.TLegend(0.7, 0.7, 0.89, 0.89)
  elif direction is r.Direction.less:
    leg = r.TLegend(0.15, 0.15, 0.45, 0.45)
  leg.SetFillStyle(0)
  leg.SetBorderSize(0)
  leg.AddEntry(s, "Signal", "l")
  leg.AddEntry(b, "background", "l")
  leg.Draw()
  outputName = str(s.GetName()).replace("gS", "Eff_") + ".png"
  c.Print(outputName)
  c.Print(outputName.replace(".png", ".pdf"))
