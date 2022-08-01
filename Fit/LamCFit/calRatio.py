#!/usr/bin/env python3
import numpy as np
import pandas as pd
Lc = pd.read_csv("LcSpectra.txt", delimiter=" ")
print(Lc)
D0 = pd.read_csv("D0Spectra.txt", delimiter=" ")
print(D0)

D0Rebin = pd.DataFrame(columns = list(D0.columns))

nBins = [2, 2, 2, 2, 4, 2]
widths = [0.5, 0.5, 0.5, 0.5, 0.5, 1]
ibin = 0
i = 0
for nBin, width in zip(nBins, widths):
  #print(D0["dSigma/dPt/dY"][ibin:ibin+nBin])
  dPt = D0["pTMax"][ibin+nBin-1] - D0["pTMin"][ibin]
  #print(dPt)
  #print(np.sum(D0["dSigma/dPt/dY"][ibin:ibin+nBin]) * width / dPt)
  #print(np.sqrt(np.sum(D0["dSigma/dPt/dYErr"][ibin:ibin+nBin]** 2 )) * width / dPt)
  d0sigma = np.sum(D0["dSigma/dPt/dY"][ibin:ibin+nBin]) * width / dPt
  d0sigmaErr = np.sqrt(np.sum(D0["dSigma/dPt/dYErr"][ibin:ibin+nBin]** 2 )) * width / dPt
  D0Rebin.loc[i] = [D0["pTMin"][ibin], D0["pTMax"][ibin+nBin-1], d0sigma, d0sigmaErr]

  ibin = ibin + nBin
  i = i+1

#print(D0Rebin)
#print(Lc["dSigma/dPt/dY"]/D0Rebin["dSigma/dPt/dY"])
CMS = (Lc["dSigma/dPt/dY"]/D0Rebin["dSigma/dPt/dY"]).to_numpy()
alice_ratio = pd.read_csv("alice_ratio.csv")
alice = alice_ratio["ratio"][1:-1].to_numpy()

CMSErr = CMS * Lc["dSigma/dPt/dYErr"]/Lc["dSigma/dPt/dY"]
CMSErr = CMSErr.to_numpy()

aliceErr = np.sqrt(alice_ratio["stat +"][1:-1].to_numpy() ** 2 + alice_ratio["sys +"][1:-1].to_numpy() ** 2)

totErr = np.sqrt(aliceErr ** 2 + CMSErr ** 2)
print(aliceErr)

ratioComp = CMS/alice
sig = (CMS-alice)/totErr
print(sig)

import ROOT
ROOT.gROOT.SetBatch(True)
cr = ROOT.TCanvas("cr", "", 600, 900)
cr.Divide(1, 2)
cr.cd(1)
pTs = (Lc["pTMax"] + Lc["pTMin"]).to_numpy() / 2.
pTErrs = (Lc["pTMax"] - Lc["pTMin"]).to_numpy() / 2.
gr = ROOT.TGraphErrors(len(CMS), pTs, CMS, pTErrs, CMSErr)
frame = gr.GetHistogram()
frame.SetAxisRange(0, 0.8, "Y")
frame.SetTitle(";p_{T} (GeV);#Lambda_{c}^{+}/D^{0}")
gr.SetMarkerStyle(20)
gr.SetMarkerColor(ROOT.kRed)
gr.Draw("APZ")

pT_alice = alice_ratio["PT [GEV/C]"][1:-1].to_numpy()
pTErrs_alice = (alice_ratio["PT [GEV/C] HIGH"][1:-1] - alice_ratio["PT [GEV/C] LOW"][1:-1]).to_numpy()/2.
gr_alice = ROOT.TGraphMultiErrors(len(alice), pT_alice, alice, pTErrs_alice, pTErrs_alice, aliceErr, aliceErr)
#gr_alice.AddYError(len(alice))
gr_alice.SetMarkerStyle(ROOT.kFullSquare)
gr_alice.SetMarkerColor(ROOT.kBlack)
#gr_alice.SetMarkerSize(0.5)
gr_alice.SetLineWidth(0)
#gr_alice.SetLineWidth(0)
gr_alice.SetFillColorAlpha(ROOT.kBlue, 0.3)
gr_alice.Draw("2")
gr_alice.Draw("P")

leg = ROOT.TLegend(0.6, 0.75, 0.89, 0.89)
leg.SetFillStyle(0)
leg.AddEntry(gr, "CMS 8.16 TeV, stat", "P")
leg.AddEntry(gr_alice, "ALICE 5.02 TeV, stat+syst", "P")
leg.Draw()

cr.cd(2)
grdiff = ROOT.TGraph(len(sig), pTs, sig)
frame2 = grdiff.GetHistogram()
#frame2.SetBinContent(1, 0)
frame2.SetTitle(";p_{T} (GeV);(CMS-ALICE)/Error")
frame2.SetAxisRange(-3, 2, "Y")
frame2.Draw()
grdiff.SetMarkerStyle(20)
grdiff.SetMarkerSize(1.5)
grdiff.Draw("P")

cr.Print("comp.png")
