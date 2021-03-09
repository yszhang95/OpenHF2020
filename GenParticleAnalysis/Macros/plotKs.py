#!/usr/bin/env python
import ROOT as r
r.gROOT.SetBatch()
r.gStyle.SetOptStat(0)

f = r.TFile.Open('PA8TeV_pPb_LamC_MC_20210302_KsMatch.root')
Regions = ['Peak', 'Side']
Charges = ['P', 'M']
h3D = {}
for t in Regions:
  for e in Charges:
    h3D[t+e] = f.Get('h%sChi2vsdPtvsdR_Pi%s' % (t, e))

LineStyle = [1, 2]
Colors = [2, 9]
Markers = [r.kFullCircle, r.kOpenCircle]
# dR: Peak and Side
hdR = {}
cdR = r.TCanvas("cdR", '', 500, 400)
cdR.SetLogy()
for t, ls, ms in zip(Regions, LineStyle, Markers):
  for e, col in zip(Charges, Colors):
    hdR[t+e] = h3D[t+e].ProjectionX()
    hdR[t+e].SetTitle('')
    hdR[t+e].Sumw2()
    hdR[t+e].Scale(1./hdR[t+e].Integral(0, 10000000))
    print hdR[t+e].Integral()
    hdR[t+e].SetLineStyle(ls)
    hdR[t+e].SetLineColor(col)
    hdR[t+e].SetMarkerStyle(ms)
    hdR[t+e].SetMarkerColor(col)
    hdR[t+e].Draw("ESAME");
cdR.BuildLegend()
cdR.Print('plots/KsdR.png')
cdR.Print('plots/KsdR.pdf')

hdPt = {}
cdPt = r.TCanvas("cdPt", '', 500, 400)
cdPt.SetLogy()
for t, ls, ms in zip(Regions, LineStyle, Markers):
  for e, col in zip(Charges, Colors):
    hdPt[t+e] = h3D[t+e].ProjectionY()
    hdPt[t+e].SetTitle('')
    hdPt[t+e].Sumw2()
    hdPt[t+e].Scale(1./hdPt[t+e].Integral(0, 10000000))
    print hdPt[t+e].Integral()
    hdPt[t+e].SetLineStyle(ls)
    hdPt[t+e].SetLineColor(col)
    hdPt[t+e].SetMarkerStyle(ms)
    hdPt[t+e].SetMarkerColor(col)
    hdPt[t+e].Draw("ESAME");
cdPt.BuildLegend()
cdPt.Print('plots/KsdPt.png')
cdPt.Print('plots/KsdPt.pdf')

hChi2 = {}
cChi2 = r.TCanvas("cChi2", '', 500, 400)
cChi2.SetLogy()
for t, ls, ms in zip(Regions, LineStyle, Markers):
  for e, col in zip(Charges, Colors):
    hChi2[t+e] = h3D[t+e].ProjectionZ()
    hChi2[t+e].SetTitle('')
    hChi2[t+e].Sumw2()
    hChi2[t+e].Scale(1./hChi2[t+e].Integral(0, 10000000))
    print hChi2[t+e].Integral()
    hChi2[t+e].SetLineStyle(ls)
    hChi2[t+e].SetLineColor(col)
    hChi2[t+e].SetMarkerStyle(ms)
    hChi2[t+e].SetMarkerColor(col)
    hChi2[t+e].Draw("ESAME");
cChi2.BuildLegend()
cChi2.Print('plots/KsChi2.png')
cChi2.Print('plots/KsChi2.pdf')

hdPtdRCut = {}
dRCut = [0.03, 0.1, 10000.0]
ThreeColors = [r.kRed, r.kBlue, r.kGreen]
cdRCut = r.TCanvas("dRCut", '', 500, 400)
cdRCut.SetLogy()
for t, ls, ms in zip(Regions, LineStyle, Markers):
  for cut, col in zip(dRCut, ThreeColors):
    end = h3D[t+'P'].GetXaxis().FindBin(cut-1e4)
    cutstr = str(cut).replace('.', 'p')
    hdPtdRCut[t+'dPtdRCut'+cutstr] = h3D[t+'P'].ProjectionY(h3D[t+'P'].GetName()+'dPtdRCut'+cutstr, 0, end)
    hdPtdRCut[t+'dPtdRCut'+cutstr].Sumw2()
    hdPtdRCut[t+'dPtdRCut'+cutstr].Scale(1./hdPtdRCut[t+'dPtdRCut'+cutstr].Integral(0, 100000))
    hdPtdRCut[t+'dPtdRCut'+cutstr].SetTitle('%s, #pi^{+}, dR<%.2f'% (t, cut))
    hdPtdRCut[t+'dPtdRCut'+cutstr].SetLineStyle(ls)
    hdPtdRCut[t+'dPtdRCut'+cutstr].SetLineColor(col)
    hdPtdRCut[t+'dPtdRCut'+cutstr].SetMarkerColor(col)
    hdPtdRCut[t+'dPtdRCut'+cutstr].SetMarkerStyle(ms)
    hdPtdRCut[t+'dPtdRCut'+cutstr].GetYaxis().SetRangeUser(0.001, 1.)
    hdPtdRCut[t+'dPtdRCut'+cutstr].Draw("ESAME")
cdRCut.BuildLegend()
cdRCut.Print('plots/KsdPtdRCut.pdf')
cdRCut.Print('plots/KsdPtdRCut.png')

hdRdPtCut = {}
#dPtCut = [1.0, 0.5, 0.2]
dPtCut = [0.2, 0.5, 1.0]
ThreeColors = [r.kRed, r.kBlue, r.kGreen]
cdPtCut = r.TCanvas("dPtCut", '', 500, 400)
cdPtCut.SetLogy()
for t, ls, ms in zip(Regions, LineStyle, Markers):
  for cut, col in zip(dPtCut, ThreeColors):
    end = h3D[t+'P'].GetYaxis().FindBin(cut-1e4)
    cutstr = str(cut).replace('.', 'p')
    hdRdPtCut[t+'dRdPtCut'+cutstr] = h3D[t+'P'].ProjectionX(h3D[t+'P'].GetName()+'dRdPtCut'+cutstr, 0, end)
    hdRdPtCut[t+'dRdPtCut'+cutstr].Sumw2()
    hdRdPtCut[t+'dRdPtCut'+cutstr].Scale(1./hdRdPtCut[t+'dRdPtCut'+cutstr].Integral(0, 100000))
    hdRdPtCut[t+'dRdPtCut'+cutstr].SetTitle('%s, #pi^{+}, dPt<%.2f'% (t, cut))
    hdRdPtCut[t+'dRdPtCut'+cutstr].SetLineStyle(ls)
    hdRdPtCut[t+'dRdPtCut'+cutstr].SetLineColor(col)
    hdRdPtCut[t+'dRdPtCut'+cutstr].SetMarkerColor(col)
    hdRdPtCut[t+'dRdPtCut'+cutstr].SetMarkerStyle(ms)
    #hdRdPtCut[t+'dRdPtCut'+cutstr].GetYaxis().SetRangeUser(0.001, 1.)
    hdRdPtCut[t+'dRdPtCut'+cutstr].Draw("ESAME")
cdPtCut.BuildLegend()
cdPtCut.Print('plots/KsdRdPtCut.pdf')
cdPtCut.Print('plots/KsdRdPtCut.png')

cMass = r.TCanvas("cmass", '', 500, 400)
hmass = f.Get("hKsMass")
hmass.Draw()
cMass.Print('KsMass.pdf')
cMass.Print('KsMass.png')
