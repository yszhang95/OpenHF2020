#!/usr/bin/env python3
import ROOT as r

infiles = [
'../test/TMVA_topo4_dau3trans_BDTG.root',
'../test/TMVA_topo4_dau4_BDTG.root',
'../test/TMVA_topo8_BDTG.root',
'../test/TMVA_topo8_dau3trans_BDTG.root',
'../test/TMVA_topo4_CutsOnKs_BDTG.root'
        ]
colors = [1, 2, 3, 4, 5, 6]

fs = [ r.TFile.Open(f) for f in infiles ]
hists = [f.Get('Method_BDT/BDTG/MVA_BDTG_trainingRejBvsS')\
            .Clone(f.GetName()\
            .replace("../test/TMVA_", "").replace(".root", "")) \
            for f in fs]
c = r.TCanvas("cROC", "", 800, 600)
hFrame = c.DrawFrame(0, 0, 1, 1);
hFrame.SetTitle("Background rejection versus Signal efficiency;Background rejection;Signal efficiency")
leg = r.TLegend(0.15, 0.15, 0.5, 0.4)
for (h, col, f) in zip(hists, colors, infiles):
    h.SetLineColor(col)
    h.Draw("SAME")
    leg.AddEntry(h, f.replace("../test/TMVA_", "").replace(".root", ""), "l")
leg.SetBorderSize(0)
leg.Draw()

c.Print("ROC.pdf")
