#!/usr/bin/env python3
import ROOT as r
r.gROOT.SetBatch()

infiles = [
'../test/VarsBDTA/TMVA_topo4_CutsOnKs_BDTA_100_Depth3_Beta0.8.root',
'../test/VarsBDTA/TMVA_topo4_dau4_BDTA_100_Depth3_Beta0.8.root',
'../test/VarsBDTA/TMVA_topo4_dauEtaPtTrans_BDTA_100_Depth3_Beta0.8.root',
'../test/VarsBDTA/TMVA_topo4_dauEta_dauPtTrans_BDTA_100_Depth3_Beta0.8.root',
'../test/VarsBDTA/TMVA_topo8_BDTA_100_Depth3_Beta0.8.root',
'../test/VarsBDTA/TMVA_topo8_dau4_BDTA_100_Depth3_Beta0.8.root',
'../test/VarsBDTA/TMVA_topo8_dauEtaPtTrans_BDTA_100_Depth3_Beta0.8.root',
        ]
colors = [1, 2, 3, 4, 5, 6, 7]

fs = [ r.TFile.Open(f) for f in infiles ]
hists = [f.Get('Method_BDT/BDTA/MVA_BDTA_trainingRejBvsS')\
            .Clone(f.GetName()\
            .replace("../test/VarsBDTA/TMVA_", "").replace(".root", "")) \
            for f in fs]
c = r.TCanvas("cROC", "", 800, 600)
hFrame = c.DrawFrame(0, 0, 1, 1);
hFrame.SetTitle("Background rejection versus Signal efficiency;Signal efficiency;Background rejection")
leg = r.TLegend(0.12, 0.15, 0.57, 0.55)
for (h, col, f) in zip(hists, colors, infiles):
    h.SetLineColor(col)
    h.Draw("SAME")
    leg.AddEntry(h, f.replace("../test/VarsBDTA/TMVA_", "").replace(".root", ""), "l")
leg.SetBorderSize(0)
leg.Draw()

c.Print("ROC.pdf")
