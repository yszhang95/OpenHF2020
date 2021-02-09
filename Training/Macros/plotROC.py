#!/usr/bin/env python3
import ROOT as r

infiles = [
'../test/TMVA_topo4_dau3trans_BDTG.root',
'../test/TMVA_topo4_dau4_BDTG.root',
'../test/TMVA_topo8_BDTG.root',
'../test/TMVA_topo8_dau3trans_BDTG.root',
'../test/TMVA_topo4_CutsOnKs_BDTG.root'
        ]

fs = [ r.TFile.Open(f) for f in infiles ]
hists = [f.Get('Method_BDT/BDTG/MVA_BDTG_trainingRejBvsS')\
            .Clone(f.GetName()\
            .replace("../test/TMVA_", "").replace(".root", "")) \
            for f in fs]
c = r.TCanvas("cROC", "", 800, 600)
hFrame = c.DrawFrame(0, 0, 1, 1);
hFrame.SetTitle("Background rejection versus Signal efficiency;Background rejection;Signal efficiency")
for h in hists:
    h.Draw("SAME")

c.Print("ROC.pdf")
