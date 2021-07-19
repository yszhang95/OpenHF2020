#!/usr/bin/env python
import ROOT as r
r.gROOT.SetBatch()
r.gStyle.SetOptStat(0)

def MultipleKsMatch_dRPi0p03_MatchDau():
  fileNames = [
    'ksInLc_pPb_effHists.root.old',
    'ks_pPb_effHists.root',
    'ksFromLc_pPb_effHists.root'
      ]
  hnames = [
      'ksInLc',
      'ksInMB',
      'ksFromLc'
      ]
  hKsRecos = []
  hKsGens = []
  gs = []
  for n,h in zip(fileNames, hnames):
    f = r.TFile.Open(n)
    hKsReco2D = f.Get("hKsReco")
    nSmall = hKsReco2D.GetYaxis().FindBin(-0.99)
    nLarge = hKsReco2D.GetYaxis().FindBin(0.99)
    hKsRecoTemp = hKsReco2D.ProjectionX('KsReco'+h, nSmall, nLarge)
    #print hKsRecoTemp.GetXaxis().GetNbins()
    hKsRecos.append(hKsRecoTemp)
    #print hKsRecoTemp.GetEntries()
    hKsGen2D = f.Get("hKsGen")
    hKsGenTemp = hKsGen2D.ProjectionX('KsGen'+h, nSmall, nLarge)
    #print hKsGenTemp.GetXaxis().GetNbins()
    hKsGens.append(hKsGenTemp)
    #print hKsGenTemp.GetEntries()
    g = r.TGraphAsymmErrors(hKsGenTemp)
    g.Divide(hKsRecoTemp, hKsGenTemp)
    gs.append(g)
    c = r.TCanvas("c"+h, "", 800, 600)
    hKsReco2D.Divide(hKsGen2D)
    hKsReco2D.Draw("LEGO2")
    c.Print("EffPlots/"+h+"_matchDau.pdf")
    c.Print("EffPlots/"+h+"_matchDau.png")
    f.Close()

  #print (gs)
  c = r.TCanvas("c", "", 800, 600)
  frame = c.DrawFrame(0., 0., 10., 1.0)
  tex = r.TLatex()
  tex.SetNDC()

  gs[0].SetLineColor(r.kRed)
  gs[0].SetMarkerColor(r.kRed)

  gs[1].SetLineColor(r.kBlue)
  gs[1].SetMarkerColor(r.kBlue)

  gs[2].SetLineColor(r.kBlack)
  gs[2].SetMarkerColor(r.kBlack)

  gs[0].Draw("p")
  gs[1].Draw("p")
  gs[2].Draw("P")
  tex.DrawLatex(0.7, 0.8, "|y|<1")

  leg = r.TLegend(0.15, 0.5, 0.5, 0.85)
  leg.AddEntry(gs[0], 'Ks in Lc samples', 'lp')
  leg.AddEntry(gs[1], 'Ks in EPOS MB', 'lp')
  leg.AddEntry(gs[2], 'Ks from Lc', 'lp')
  leg.SetBorderSize(0)
  leg.SetFillStyle(4000)
  leg.Draw()

  c.Print("EffPlots/KsEff_matchDau.pdf")
  c.Print("EffPlots/KsEff_matchDau.png")

def MultipleKsMatch_dRPi0p03():
  fileNames = [
    'pPb_dRKs0.030000_dRPi0.030000_multipleKsMatch_effHists.root',
    'ks_pPb_effHists.root',
    'ksFromLc_pPb_effHists.root'
      ]
  hnames = [
      'ksInLc',
      'ksInMB',
      'ksFromLc'
      ]
  hKsRecos = []
  hKsGens = []
  gs = []
  for n,h in zip(fileNames, hnames):
    f = r.TFile.Open(n)
    hKsReco2D = f.Get("hKsReco")
    nSmall = hKsReco2D.GetYaxis().FindBin(-0.99)
    nLarge = hKsReco2D.GetYaxis().FindBin(0.99)
    hKsRecoTemp = hKsReco2D.ProjectionX('KsReco'+h, nSmall, nLarge)
    #print hKsRecoTemp.GetXaxis().GetNbins()
    hKsRecos.append(hKsRecoTemp)
    #print hKsRecoTemp.GetEntries()
    hKsGen2D = f.Get("hKsGen")
    hKsGenTemp = hKsGen2D.ProjectionX('KsGen'+h, nSmall, nLarge)
    #print hKsGenTemp.GetXaxis().GetNbins()
    hKsGens.append(hKsGenTemp)
    #print hKsGenTemp.GetEntries()
    g = r.TGraphAsymmErrors(hKsGenTemp)
    g.Divide(hKsRecoTemp, hKsGenTemp)
    gs.append(g)
    c = r.TCanvas("c"+h, "", 800, 600)
    hKsReco2D.Divide(hKsGen2D)
    hKsReco2D.Draw("LEGO2")
    c.Print("EffPlots/"+h+".pdf")
    c.Print("EffPlots/"+h+".png")
    f.Close()

  #print (gs)
  c = r.TCanvas("c", "", 800, 600)
  frame = c.DrawFrame(0., 0., 10., 1.0)
  tex = r.TLatex()
  tex.SetNDC()

  gs[0].SetLineColor(r.kRed)
  gs[0].SetMarkerColor(r.kRed)

  gs[1].SetLineColor(r.kBlue)
  gs[1].SetMarkerColor(r.kBlue)

  gs[2].SetLineColor(r.kBlack)
  gs[2].SetMarkerColor(r.kBlack)

  gs[0].Draw("p")
  gs[1].Draw("p")
  gs[2].Draw("P")
  tex.DrawLatex(0.7, 0.8, "|y|<1")

  leg = r.TLegend(0.15, 0.5, 0.5, 0.85)
  leg.AddEntry(gs[0], 'Ks in Lc samples', 'lp')
  leg.AddEntry(gs[1], 'Ks in EPOS MB', 'lp')
  leg.AddEntry(gs[2], 'Ks from Lc', 'lp')
  leg.SetBorderSize(0)
  leg.SetFillStyle(4000)
  leg.Draw()

  c.Print("EffPlots/KsEff.pdf")
  c.Print("EffPlots/KsEff.png")

def KsMatch_PidR():
  fileMultipleNames = [
    'pPb_dRKs0.030000_dRPi0.030000_multipleKsMatch_effHists.root',
    'pPb_dRKs0.030000_dRPi0.100000_multipleKsMatch_effHists.root'
      ]
  fileSingleNames = [
    'pPb_dRKs0.030000_dRPi0.030000_singleKsMatch_effHists.root',
    'pPb_dRKs0.030000_dRPi0.100000_singleKsMatch_effHists.root'
      ]
  hnames = [
      'Pi0p03',
      'Pi0p10'
      ]
  for m, s, h in zip(fileMultipleNames, fileSingleNames, hnames):
    fm = r.TFile.Open(m)
    fs = r.TFile.Open(s)

    hKsReco2DM = fm.Get("hKsReco")
    hKsReco2DS = fs.Get("hKsReco")

    hKsReco0Pi2DM = fm.Get("hKsReco0Pi")
    hKsReco0Pi2DS = fs.Get("hKsReco0Pi")
    hKsReco1Pi2DM = fm.Get("hKsReco1Pi")
    hKsReco1Pi2DS = fs.Get("hKsReco1Pi")
    hKsReco2Pi2DM = fm.Get("hKsReco2Pi")
    hKsReco2Pi2DS = fs.Get("hKsReco2Pi")

    nSmall = hKsReco2DM.GetYaxis().FindBin(-0.99)
    nLarge = hKsReco2DM.GetYaxis().FindBin(0.99)

    hKsRecoTempM = hKsReco2DM.ProjectionX('KsRecoM'+h, nSmall, nLarge)
    hKsRecoTempS = hKsReco2DS.ProjectionX('KsRecoS'+h, nSmall, nLarge)
    hKsReco0PiTempM = hKsReco0Pi2DM.ProjectionX('KsReco0PiM'+h, nSmall, nLarge)
    hKsReco0PiTempS = hKsReco0Pi2DS.ProjectionX('KsReco0PiS'+h, nSmall, nLarge)
    hKsReco1PiTempM = hKsReco1Pi2DM.ProjectionX('KsReco1PiM'+h, nSmall, nLarge)
    hKsReco1PiTempS = hKsReco1Pi2DS.ProjectionX('KsReco1PiS'+h, nSmall, nLarge)
    hKsReco2PiTempM = hKsReco2Pi2DM.ProjectionX('KsReco2PiM'+h, nSmall, nLarge)
    hKsReco2PiTempS = hKsReco2Pi2DS.ProjectionX('KsReco2PiS'+h, nSmall, nLarge)

    hKsRecoTempM.SetMarkerStyle(20)
    hKsRecoTempM.SetMarkerSize(1.2)
    hKsReco2PiTempM.SetLineColor(r.kRed)
    hKsReco1PiTempM.SetLineColor(r.kBlue)
    hKsReco0PiTempM.SetLineColor(r.kGreen)
    hKsReco2PiTempM.SetFillColor(r.kRed)
    hKsReco1PiTempM.SetFillColor(r.kBlue)
    hKsReco0PiTempM.SetFillColor(r.kGreen)

    hKsRecoTempS.SetMarkerStyle(20)
    hKsRecoTempS.SetMarkerSize(1.2)
    hKsReco2PiTempS.SetLineColor(r.kRed)
    hKsReco1PiTempS.SetLineColor(r.kBlue)
    hKsReco0PiTempS.SetLineColor(r.kGreen)
    hKsReco2PiTempS.SetFillColor(r.kRed)
    hKsReco1PiTempS.SetFillColor(r.kBlue)
    hKsReco0PiTempS.SetFillColor(r.kGreen)

    hStackM = r.THStack('KsRecoMStack'+h, "")
    hStackM.Add(hKsReco0PiTempM)
    hStackM.Add(hKsReco1PiTempM)
    hStackM.Add(hKsReco2PiTempM)

    hStackS = r.THStack('KsRecoSStack'+h, "")
    hStackS.Add(hKsReco0PiTempS)
    hStackS.Add(hKsReco1PiTempS)
    hStackS.Add(hKsReco2PiTempS)

    cM = r.TCanvas("cM"+h, "Allow multiple match, " + h, 600, 1000)
    cM.Divide(1,2)
    cM.cd(1)
    hStackM.Draw()
    hKsRecoTempM.Draw("ESAME")
    cM.cd(1).BuildLegend()
    cM.cd(2)
    gM0Pi = r.TGraphAsymmErrors(hKsReco0PiTempM)
    gM1Pi = r.TGraphAsymmErrors(hKsReco1PiTempM)
    gM2Pi = r.TGraphAsymmErrors(hKsReco2PiTempM)
    gM0Pi.Divide(hKsReco0PiTempM, hKsRecoTempM)
    gM1Pi.Divide(hKsReco1PiTempM, hKsRecoTempM)
    gM2Pi.Divide(hKsReco2PiTempM, hKsRecoTempM)
    #hframe = cM.cd(2).DrawFrame(0.0, 0.0, 10.0, 1.0)
    hframe = gM0Pi.GetHistogram()
    hframe.SetTitle(";p_T (GeV);Ratio to matching Ks p3")
    hframe.GetYaxis().SetRangeUser(0, 1)
    hframe.GetXaxis().SetRangeUser(0, 10)
    hframe.Draw()
    gM0Pi.Draw("P")
    gM1Pi.Draw("P")
    gM2Pi.Draw("P")
    leg = cM.cd(2).BuildLegend()
    cM.Print("EffPlots/Stack012PiMultipleMatch"+h+".pdf")
    cM.Print("EffPlots/Stack012PiMultipleMatch"+h+".png")

    cS = r.TCanvas("cS"+h, "Allow single match, " + h, 600, 1000)
    cS.Divide(1,2)
    cS.cd(1)
    hStackS.Draw()
    hKsRecoTempS.Draw("ESAME")
    cS.cd(1).BuildLegend()
    cS.cd(2)
    gS0Pi = r.TGraphAsymmErrors(hKsReco0PiTempS)
    gS1Pi = r.TGraphAsymmErrors(hKsReco1PiTempS)
    gS2Pi = r.TGraphAsymmErrors(hKsReco2PiTempS)
    gS0Pi.Divide(hKsReco0PiTempS, hKsRecoTempS)
    gS1Pi.Divide(hKsReco1PiTempS, hKsRecoTempS)
    gS2Pi.Divide(hKsReco2PiTempS, hKsRecoTempS)
    hframe = gS0Pi.GetHistogram()
    hframe.SetTitle(";p_T (GeV);Ratio to matching Ks p3")
    hframe.GetYaxis().SetRangeUser(0, 1)
    hframe.GetXaxis().SetRangeUser(0, 10)
    hframe.Draw()
    gS0Pi.Draw("P")
    gS1Pi.Draw("P")
    gS2Pi.Draw("P")
    leg = cS.cd(2).BuildLegend()
    cS.Print("EffPlots/Stack012PiSingleMatch"+h+".pdf")
    cS.Print("EffPlots/Stack012PiSingleMatch"+h+".png")

def MultipleSingleKsMatch():
  fileMultipleNames = [
    'pPb_dRKs0.030000_dRPi0.030000_multipleKsMatch_effHists.root',
    'pPb_dRKs0.030000_dRPi0.100000_multipleKsMatch_effHists.root'
      ]
  fileSingleNames = [
    'pPb_dRKs0.030000_dRPi0.030000_singleKsMatch_effHists.root',
    'pPb_dRKs0.030000_dRPi0.100000_singleKsMatch_effHists.root'
      ]
  hnames = [
      'Pi0p03',
      'Pi0p10'
      ]

  for m, s, h in zip(fileMultipleNames, fileSingleNames, hnames):
    fm = r.TFile.Open(m)
    fs = r.TFile.Open(s)

    hKsReco2DM = fm.Get("hKsReco")
    hKsReco2DS = fs.Get("hKsReco")

    nSmall = hKsReco2DM.GetYaxis().FindBin(-0.99)
    nLarge = hKsReco2DM.GetYaxis().FindBin(0.99)

    hKsRecoTempM = hKsReco2DM.ProjectionX('KsRecoM'+h, nSmall, nLarge)
    hKsRecoTempS = hKsReco2DS.ProjectionX('KsRecoS'+h, nSmall, nLarge)

    hRatio = hKsRecoTempM.Clone("hKsRecoRatio"+h)
    hRatio.Divide(hKsRecoTempS)

    c = r.TCanvas("c"+h, "", 600, 1000)
    c.Divide(1,2)
    c.cd(1)
    hKsRecoTempM.SetLineColor(r.kRed)
    hKsRecoTempS.SetLineColor(r.kBlue)
    hKsRecoTempM.Draw();
    hKsRecoTempS.Draw("SAME");
    leg = r.TLegend(0.65, 0.7, 0.88, 0.88)
    leg.AddEntry(hKsRecoTempM, "Multiple match", "lp");
    leg.AddEntry(hKsRecoTempS, "Single match", "lp");
    leg.Draw()
    c.cd(2)
    hRatio.Draw()
    hRatio.SetTitle("Ks p4 match")
    hRatio.GetYaxis().SetTitle("allow multiple/ allow single")

    c.Print("EffPlots/KsRecoMultipleSingle"+h+".pdf")
    c.Print("EffPlots/KsRecoMultipleSingle"+h+".png")

    fm.Close()
    fs.Close()

  for m, s, h in zip(fileMultipleNames, fileSingleNames, hnames):
    fm = r.TFile.Open(m)
    fs = r.TFile.Open(s)

    hKsReco2DM = fm.Get("hKsReco2Pi")
    hKsReco2DS = fs.Get("hKsReco2Pi")

    nSmall = hKsReco2DM.GetYaxis().FindBin(-0.99)
    nLarge = hKsReco2DM.GetYaxis().FindBin(0.99)

    hKsRecoTempM = hKsReco2DM.ProjectionX('KsRecoM'+h, nSmall, nLarge)
    hKsRecoTempS = hKsReco2DS.ProjectionX('KsRecoS'+h, nSmall, nLarge)

    hRatio = hKsRecoTempM.Clone("hKsReco2PiRatio"+h)
    hRatio.Divide(hKsRecoTempS)

    c = r.TCanvas("c"+h, "", 600, 1000)
    c.Divide(1,2)
    c.cd(1)
    hKsRecoTempM.SetLineColor(r.kRed)
    hKsRecoTempS.SetLineColor(r.kBlue)
    hKsRecoTempM.Draw();
    hKsRecoTempS.Draw("SAME");
    leg = r.TLegend(0.65, 0.7, 0.88, 0.88)
    leg.AddEntry(hKsRecoTempM, "Multiple match", "lp");
    leg.AddEntry(hKsRecoTempS, "Single match", "lp");
    leg.Draw()
    c.cd(2)
    hRatio.Draw()
    hRatio.SetTitle("Both #pi^{+}#^pi{-} match, Ks p4 match is pre-required")
    hRatio.GetYaxis().SetTitle("allow multiple/ allow single")

    c.Print("EffPlots/KsReco2PiMultipleSingle"+h+".pdf")
    c.Print("EffPlots/KsReco2PiMultipleSingle"+h+".png")

    fm.Close()
    fs.Close()
def PiIndep():
  fileMultipleNames = [
    'pPb_dRKs0.030000_dRPi0.030000_multipleKsMatch_effHists.root',
    'pPb_dRKs0.030000_dRPi0.100000_multipleKsMatch_effHists.root'
      ]
  fileSingleNames = [
    'pPb_dRKs0.030000_dRPi0.030000_singleKsMatch_effHists.root',
    'pPb_dRKs0.030000_dRPi0.100000_singleKsMatch_effHists.root'
      ]
  hnames = [
      'Pi0p03',
      'Pi0p10'
      ]
  for m, s, h in zip(fileMultipleNames, fileSingleNames, hnames):
    fm = r.TFile.Open(m)
    fs = r.TFile.Open(s)

    hKsReco2Pi2D = fm.Get("hKsReco2Pi")
    hKsReco2PiIndep2D = fm.Get("hKsReco2PiIndep")

    nSmall = hKsReco2Pi2D.GetYaxis().FindBin(-0.99)
    nLarge = hKsReco2Pi2D.GetYaxis().FindBin(0.99)

    hKsReco2PiTemp = hKsReco2Pi2D.ProjectionX('KsReco2Pi'+h, nSmall, nLarge)
    hKsReco2PiIndepTemp = hKsReco2PiIndep2D.ProjectionX('KsReco2PiIndep'+h, nSmall, nLarge)
    hRatio = hKsReco2PiIndepTemp.Clone("hRatio"+h)
    hRatio.Divide(hKsReco2PiTemp)

    c = r.TCanvas("c"+h, "", 600, 1000)
    c.Divide(1,2)
    c.cd(1)
    hKsReco2PiTemp.SetLineColor(r.kRed)
    hKsReco2PiIndepTemp.SetLineColor(r.kBlue)
    hKsReco2PiTemp.Draw();
    hKsReco2PiIndepTemp.Draw("SAME");
    leg = r.TLegend(0.5, 0.7, 0.88, 0.88)
    leg.AddEntry(hKsReco2PiTemp, "Multiple match, 2Pi", "lp");
    leg.AddEntry(hKsReco2PiIndepTemp, "Multiple match, 2Pi Indep.", "lp");
    leg.Draw()
    c.cd(2)
    hRatio.Draw()
    hRatio.SetTitle("Both #pi^{+}#^pi{-} match")
    hRatio.GetYaxis().SetTitle("Indep. on Ks / Ks match")

    c.Print("EffPlots/Match2PiIndepOrNot_"+h+".pdf")
    c.Print("EffPlots/Match2PiIndepOrNot_"+h+".png")
    fm.Close()
    fs.Close()

PiIndep()
KsMatch_PidR()
MultipleSingleKsMatch()
MultipleKsMatch_dRPi0p03()
MultipleKsMatch_dRPi0p03_MatchDau()
