#!/usr/bin/env python3
import numpy as np
import pandas as pd
import ROOT
ROOT.gROOT.SetBatch(True)

fonll8p16 = pd.read_csv("plot_fonll_8p16.txt", delimiter= " ")
fonll8 = pd.read_csv("plot_fonll_8.txt", delimiter = " ")
fonll5p03 = pd.read_csv("plot_fonll_5p03.txt", delimiter= " ")
print(fonll8p16)
print(fonll8)

c = ROOT.TCanvas("comp", "", 1500, 800)
c.Divide(2, 1)
pad1 = c.cd(1)
pad1.SetLogy()
frame1 = pad1.DrawFrame(0, 1E5, 10, 5E9)
frame1.SetTitle(";p_{T} GeV;d#sigma/dp_{T} (pb/GeV)")

pTs = (fonll8p16["pt"] + fonll8p16["dPt"]/2).to_numpy()
pTErrs = (fonll8p16["dPt"]/2).to_numpy()
fonll8p16_cent = fonll8p16["central"].to_numpy()
fonll8p16_errh = (fonll8p16["max"] - fonll8p16["central"]).to_numpy()
fonll8p16_errl = (fonll8p16["central"] - fonll8p16["min"]).to_numpy()
g8p16TeV = ROOT.TGraphAsymmErrors(len(pTs), pTs, fonll8p16_cent, pTErrs, pTErrs, fonll8p16_errh, fonll8p16_errl)

pTs = (fonll8["pt"] + fonll8["dPt"]/2).to_numpy()
pTErrs = (fonll8["dPt"]/2).to_numpy()
fonll8_cent = fonll8["central"].to_numpy()
fonll8_errh = (fonll8["max"] - fonll8["central"]).to_numpy()
fonll8_errl = (fonll8["central"] - fonll8["min"]).to_numpy()
g8TeV = ROOT.TGraphAsymmErrors(len(pTs), pTs, fonll8_cent, pTErrs, pTErrs, fonll8_errh, fonll8_errl)

pTs = (fonll5p03["pt"] + fonll5p03["dPt"]/2).to_numpy()
pTErrs = (fonll5p03["dPt"]/2).to_numpy()
fonll5p03_cent = fonll5p03["central"].to_numpy()
fonll5p03_errh = (fonll5p03["max"] - fonll5p03["central"]).to_numpy()
fonll5p03_errl = (fonll5p03["central"] - fonll5p03["min"]).to_numpy()
g5p03TeV = ROOT.TGraphAsymmErrors(len(pTs), pTs, fonll5p03_cent, pTErrs, pTErrs, fonll5p03_errh, fonll5p03_errl)

g8p16TeV.SetMarkerStyle(20)
g8p16TeV.SetMarkerColor(ROOT.kRed)
g8p16TeV.Draw("P")

g8TeV.SetMarkerStyle(ROOT.kFullSquare)
g8TeV.SetMarkerColor(ROOT.kBlue)
g8TeV.Draw("P")

g5p03TeV.SetMarkerStyle(ROOT.kOpenCircle)
g5p03TeV.SetMarkerColor(ROOT.kBlack)
g5p03TeV.Draw("P")

leg = ROOT.TLegend(0.6, 0.75, 0.89, 0.89)
leg.SetFillStyle(0)
leg.AddEntry(g8p16TeV, "FONLL 8.16TeV", "p")
leg.AddEntry(g8TeV, "FONLL 8TeV", "p")
leg.AddEntry(g5p03TeV, "FONLL 5.03TeV", "p")
leg.Draw()

tex = ROOT.TLatex()
tex.DrawLatexNDC(0.5, 0.65, "-0.96<|y_{cm}|<0.04")

pad2 = c.cd(2)
frame2 = pad2.DrawFrame(0, 0.5, 10, 2)
frame2.SetTitle(";p_{T} (GeV);D^{0} production over D^{0} at 5.03 TeV")
ratio_8p16_cent = fonll8p16_cent / fonll5p03_cent
ratio_8_cent = fonll8_cent / fonll5p03_cent
ratio_8p16 = ROOT.TGraph(len(pTs), pTs, ratio_8p16_cent)
ratio_8 = ROOT.TGraph(len(pTs), pTs, ratio_8_cent)

ratio_8p16.SetMarkerStyle(20)
ratio_8p16.SetMarkerColor(ROOT.kRed)
ratio_8p16.Draw("P")

ratio_8.SetMarkerStyle(ROOT.kFullSquare)
ratio_8.SetMarkerColor(ROOT.kBlue)
ratio_8.Draw("P")

ratio_8p16.Draw("P")
ratio_8.Draw("P")

c.Print("fonll.png")
