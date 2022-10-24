#!/usr/bin/env python3
import numpy as np
import pandas as pd
d0 = pd.read_csv("yields.txt", delimiter=" ")
eff = pd.read_csv("eff_alice.txt", delimiter=" ")
# Y/eff *frac / B.R. / dPt /dY / Lumi / 2, 2 is for (D+Dbar)/2
BR = 0.0395
Nevt=1.2029e+09
dY = 1
d0["Lumi"] = Nevt/ 8.1E9 * 4217 #ub^-1
spectra = d0["Y"] / eff["eff"] * d0["frac"] / BR /(d0["pTMax"]-d0["pTMin"]) / dY / d0["Lumi"] / 2
spectraErr = d0["YErr"] / eff["eff"] * d0["frac"] / BR /(d0["pTMax"]-d0["pTMin"]) / dY / d0["Lumi"] / 2
#print(d0)
#print(spectra)

alice = pd.read_csv("alice.txt", delimiter=" ")


fonll_D0_8TeV = pd.read_csv("fonll_D0_8TeV.txt", delimiter=" ")
fonll_D0_5TeV = pd.read_csv("fonll_D0_5TeV.txt", delimiter=" ")

d0copy = d0.copy()
d0copy["eff"] = eff["eff"]
d0copy["dSigma/dPt/dY"] = spectra
d0copy["delta_dSigma/dPt/dY"] = spectraErr
d0copy["fonll_ratios"] = fonll_D0_8TeV["dsigma/dPt/dY"]/fonll_D0_5TeV["dsigma/dPt/dY"]
d0copy["ratio"] = spectra / alice["xsection"]
#d0copy["ratio_err"] = d0copy["ratio"]* np.sqrt(spectraErr**2/spectra**2 + alice["err"]**2/alice["xsection"]**2)
d0copy["ratio_err"] = d0copy["ratio"]* np.sqrt(spectraErr**2/spectra**2 )

print(d0copy)

import ROOT
ROOT.gROOT.SetBatch(ROOT.kTRUE)
c = ROOT.TCanvas()
pts = (d0["pTMin"]+d0["pTMin"])/2
pterrs = (d0["pTMax"]-d0["pTMin"])/2
gr = ROOT.TGraphErrors(len(d0copy["ratio"]), pts.to_numpy(), d0copy["ratio"].to_numpy(), pterrs.to_numpy(), d0copy["ratio_err"].to_numpy())
gr.SetTitle(";p_{T} (GeV);D spectra ratio 8TeV/5TeV")
gr.SetMarkerStyle(20)
gr.Draw("AP")
gr_fonll = ROOT.TGraph(len(d0copy["ratio"]), pts.to_numpy(), d0copy["fonll_ratios"].to_numpy())
gr_fonll.SetMarkerStyle(ROOT.kOpenCircle)
gr_fonll.Draw("P")
leg=ROOT.TLegend(0.2, 0.75, 0.3, 0.89)
leg.SetFillStyle(0)
leg.AddEntry(gr, "data", "p")
leg.AddEntry(gr_fonll, "fonll", "p")
leg.Draw()
c.Print("ratioToAlice.png")
