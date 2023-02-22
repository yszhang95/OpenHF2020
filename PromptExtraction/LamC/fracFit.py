#!/usr/bin/env python3
import ROOT
from ROOT import TFile, TH1, TCanvas, TLegend
from ROOT import RooDataHist, RooAddPdf, RooHistPdf, RooRealVar
import ROOT.RooFit

import sys

class dca_fitter():
  def __init__(self, h_data, h_prompt, h_nonprompt):
    dca_min = h_data.GetBinLowEdge(1)
    dca_max = h_data.GetBinLowEdge(h_data.GetNbinsX()+1)
    self.__dca = RooRealVar("DCA", "DCA (cm) of #Lambda_{c}^{+}", dca_min, dca_max)
    self.__dh_prompt = RooDataHist("dhPromptMC", "MC prompt", self.__dca, h_prompt)
    self.__dh_nonprompt = RooDataHist("dhNonPromptMC", "MC nonprompt", self.__dca, h_nonprompt)
    self.__pdf_prompt = RooHistPdf("pdfPromptMC", "Template of MC prompt", self.__dca, self.__dh_prompt)
    self.__pdf_nonprompt = RooHistPdf("pdfNonPromptMC", "Template of MC nonprompt", self.__dca, self.__dh_nonprompt)

    self.__frac = RooRealVar("frac", "Fraction of prompt", 0.9, 0, 1)

    #self.__pdf_sum = RooAddPdf("pdf_sum", "Sum of MC prompt and MC nonprompt", ROOT.RooArgList(self.__pdf_prompt, self.__pdf_nonprompt), self.__frac)
    self.__pdf_sum = RooAddPdf("pdf_sum", "Sum of MC prompt and MC nonprompt", self.__pdf_prompt, self.__pdf_nonprompt, self.__frac)

    self.__dh_data = RooDataHist("dhData", "Data", self.__dca, h_data)

  def set_frac_range(self, *args):
    if len(args) == 3:
      name = args[0]
      val_min = args[1]
      val_max = args[2]
      self.__frac.setRange(name, val_min, val_max)
    elif len(args) == 2:
      val_min = args[0]
      val_max = args[1]
      self.__frac.setRange(val_min, val_max)
    else:
      raise NotImplementedError("Parameters can only be (name, val_min, val_max), or (val_min, val_max)")

  def set_dca_range(self, *args):
    if len(args) == 3:
      name = args[0]
      val_min = args[1]
      val_max = args[2]
      self.__dca.setRange(name, val_min, val_max)
    elif len(args) == 2:
      val_min = args[0]
      val_max = args[1]
      self.__dca.setRange(val_min, val_max)
    else:
      raise NotImplementedError("Parameters can only be (name, val_min, val_max), or (val_min, val_max)")

  def fit(self):
    #self.__pdf_sum.chi2FitTo(self.__dh_data, Range("full"))
    self.__pdf_sum.fitTo(self.__dh_data, ROOT.RooFit.SumW2Error(True))

  def plot(self, output, **kw):
    c = TCanvas("c", "dh", 800, 600)
    xframe = self.__dca.frame(Name="dca_lamc", Title="DCA (cm) of #Lambda_{c}^{+}")
    self.__dh_data.plotOn(xframe, MarkerStyle=20, DataError='SumW2', Name="g_data")
    self.__pdf_sum.plotOn(xframe, LineStyle=1, Name="g_pdf_sum")
    self.__pdf_sum.plotOn(xframe, Components=(self.__pdf_prompt), LineStyle=1, Name="g_pdf_p",
        LineColor='kRed', DrawOption="F", FillColor=ROOT.TColor.GetColorTransparent(ROOT.kRed, 0.3))
    self.__pdf_sum.plotOn(xframe, Components=(self.__pdf_nonprompt), LineStyle=1, Name="g_pdf_np",
        LineColor='kBlue', DrawOption="F", FillColor=ROOT.TColor.GetColorTransparent(ROOT.kBlue, 0.3))

    g_data = xframe.getHist("g_data")
    g_pdf_sum = xframe.getCurve("g_pdf_sum")
    g_pdf_p = xframe.getCurve("g_pdf_p")
    g_pdf_np = xframe.getCurve("g_pdf_np")

    leg = TLegend(0.65, 0.7, 0.9, 0.9)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.AddEntry(g_data, "Data", "p")
    leg.AddEntry(g_pdf_sum, "Data", "l")
    leg.AddEntry(g_pdf_p, "Pormpt", "f")
    leg.AddEntry(g_pdf_np, "b decay", "f")

    xframe.Draw();
    leg.Draw()

    tex = ROOT.TLatex()
    tex.SetTextFont(42)
    err = max(abs(self.__frac.getErrorHi()), abs(self.__frac.getErrorLo()))
    tex.DrawLatexNDC(0.65, 0.55, "frac.={frac:.2f}+/-{err:.2f}".format(frac=self.__frac.getVal(), err=err))

    ptstr = kw.get("pt", None)
    if ptstr:
      tex.DrawLatexNDC(0.65, 0.5, ptstr)
    c.Print(output)
    print(self.__frac.getVal(), err)

if __name__ == '__main__':
  configs = ROOT.FitParConfigs(sys.argv[1])
  input_configs = configs.getInputConfigs()
  cut_configs = configs.getCutConfigs()
  mycuts = ROOT.VarCuts(cut_configs)

  output = ROOT.getNames(configs, mycuts);
  fname = output["fileName"]
  fname = ROOT.gSystem.BaseName(fname)
  fname = fname.replace(".root", "_splot.root")
  fname = "output/" + fname

  f = TFile(fname)
  h_data = f.Get("DCA")
  h_prompt = f.Get("DCA_PromptMC_rebin")
  h_nonprompt = f.Get("DCA_NonPromptMC_rebin")

  fitter = dca_fitter(h_data, h_prompt, h_nonprompt)

  fitter.set_frac_range(cut_configs.getFloatMin("p_frac"), cut_configs.getFloatMax("p_frac"))

  fitter.fit()

  output = fname[0:-5]
  output = output+".png"

  fitter.plot(output, pt="pT %.1f -- %.1f" % (mycuts._pTMin, mycuts._pTMax))
