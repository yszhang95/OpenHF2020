#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooVoigtian.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooPlot.h"
#include "TFile.h"
#include "RooWorkspace.h"
#include "RooAddPdf.h"
#include "TAxis.h"
#include "TCanvas.h"

#include "RooCBShapeDS.cxx"

#pragma link C++ class RooExtCBShape+;

using namespace RooFit;

void drawFit(RooAbsPdf& pdf, RooAbsData& data, RooRealVar& mass, const char* name)
{
  auto mass_frame = mass.frame(Title(Form("%s", pdf.GetTitle())));
  data.plotOn(mass_frame);
  pdf.plotOn(mass_frame);
  mass_frame->GetXaxis()->SetTitleOffset(1.3);
  mass_frame->GetYaxis()->SetTitleOffset(1.3);
  mass_frame->GetXaxis()->SetTitleSize(0.04);
  mass_frame->GetYaxis()->SetTitleSize(0.04);

  std::cout << "start to print" << std::endl;
  auto pars =  pdf.getParameters(mass);
  //pars->printArgs(std::cout);
  pdf.paramOn(mass_frame, Parameters(*pars), Layout(0.6, 0.99, 0.95));

  TCanvas c("c", "", 450*2, 450);
  c.Divide(2, 1);
  auto pad1 = c.cd(1);
  pad1->SetTopMargin(0.15);
  pad1->SetLeftMargin(0.15);
  pad1->SetRightMargin(0.05);
  pad1->SetBottomMargin(0.15);
  mass_frame->Draw();

  auto pad2 = c.cd(2);
  pad2->SetLogy();
  pad2->SetTopMargin(0.15);
  pad2->SetLeftMargin(0.15);
  pad2->SetRightMargin(0.05);
  pad2->SetBottomMargin(0.15);
  mass_frame->Draw();

  c.Print(name);
}

//void fitSingleGaussian(RooAbsData& data, RooRealVar& Cand_Mass)
void fitSingleGaussian(RooAbsData& data, RooRealVar& Cand_Mass)
{
  //RooRealVar Cand_Mass("Cand_Mass", "m_{K_{s}^{0}p} (GeV)", 2.12, 2.43);
  Cand_Mass.setRange("singleGaus", 2.25, 2.33);
  RooRealVar mean("mean", "mean (GeV)", 2.29, 2.2, 2.36);
  RooRealVar sigma("sigma", "sigma (GeV)", 0.04, 0.0, 0.1);
  RooGaussian gaus("gaus", "single gaussian", Cand_Mass, mean, sigma);
  gaus.fitTo(data, Range("singleGaus"));

  drawFit(gaus, data, Cand_Mass, "singleGaussian.png");
}

void testFitSingleGaussian()
{
  RooRealVar Cand_Mass("Cand_Mass", "m_{K_{s}^{0}p} (GeV)", 2.12, 2.43);
  RooRealVar mean("mean", "mean (GeV)", 2.29, 2.2, 2.36);
  RooRealVar sigma("sigma", "sigma (GeV)", 0.05, 0.0, 0.3);
  RooGaussian gaus("gaus", "single gaussian", Cand_Mass, mean, sigma);
  auto data = gaus.generate(Cand_Mass, 1000);
  fitSingleGaussian(*data, Cand_Mass);
}

void fitDoubleGaussian(RooAbsData& data, RooRealVar& Cand_Mass)
{
  //RooRealVar Cand_Mass("Cand_Mass", "m_{K_{s}^{0}p} (GeV)", 2.12, 2.43);
  RooRealVar mean("mean", "mean (GeV)", 2.29, 2.2, 2.36);
  RooRealVar sigma1("sigma1", "sigma of the 1st gaussian (GeV)", 0.01, 0.0, 0.1);
  RooRealVar sigma2("sigma2", "sigma of the 2nd gaussian (GeV)", 0.001, 0.0, 0.3);
  RooGaussian gaus1("gaus1", "1st gaussian", Cand_Mass, mean, sigma1);
  RooGaussian gaus2("gaus2", "2nd gaussian", Cand_Mass, mean, sigma2);

  RooRealVar sigFrac1("sigFrac1", "signal fraction of the 1st gaussian", 0.5, 0.0, 1.0);
  RooAddPdf doubGaus("doubGaus", "double gaussian of the signal shape", RooArgList(gaus1, gaus2), sigFrac1);

  Cand_Mass.setRange("doubleGaussian", 2.2, 2.38);

  doubGaus.fitTo(data, Range("doubleGaussian"));

  drawFit(doubGaus, data, Cand_Mass, "doubleGaussian.png");
}

void testFitDoubleGaussian()
{
  RooRealVar Cand_Mass("Cand_Mass", "m_{K_{s}^{0}p} (GeV)", 2.12, 2.43);
  RooRealVar mean("mean", "mean (GeV)", 2.29, 2.2, 2.36);
  RooRealVar sigma1("sigma1", "sigma of the 1st gaussian (GeV)", 0.01, 0.0, 0.1);
  RooRealVar sigma2("sigma2", "sigma of the 2nd gaussian (GeV)", 0.001, 0.0, 0.1);
  RooGaussian gaus1("gaus1", "1st gaussian", Cand_Mass, mean, sigma1);
  RooGaussian gaus2("gaus2", "2nd gaussian", Cand_Mass, mean, sigma2);

  RooRealVar sigFrac1("sigFrac1", "signal fraction of the 1st gaussian", 0.5, 0.0, 1.0);
  RooAddPdf doubGaus("doubGaus", "double gaussian of the signal shape", RooArgList(gaus1, gaus2), sigFrac1);

  auto data = doubGaus.generate(Cand_Mass, 1000);
  fitDoubleGaussian(*data, Cand_Mass);
}

void fitVoigtian(RooAbsData& data, RooRealVar& Cand_Mass)
{
  //RooVoigtian(const char *name, const char *title, 
  //RooAbsReal& _x, RooAbsReal& _mean, RooAbsReal& _width, RooAbsReal& _sigma, Bool_t doFast) 
  //: RooAbsPdf(name,title), x("x","Dependent",this,_x), mean("mean","Mean",this,_mean), 
  //width("width","Breit-Wigner Width",this,_width), 
  //sigma("sigma","Gauss Width",this,_sigma), _doFast(doFast) 

  //RooRealVar Cand_Mass("Cand_Mass", "m_{K_{s}^{0}p} (GeV)", 2.12, 2.43);
  RooRealVar mean("mean", "mean (GeV)", 2.29, 2.2, 2.36);
  RooRealVar width("width", "Breit-Wigner width (GeV)", 0.01, 0.0, 0.1);
  RooRealVar sigma("sigma", "sigma (GeV)", 0.04, 0.0, 0.1);
  RooVoigtian voig("voig", "The convolution of a Breit-Wigner with a Gaussian", Cand_Mass, mean, width, sigma, false);

  Cand_Mass.setRange("voigtian", 2.2, 2.38);

  voig.fitTo(data, Range("voigtian"));

  drawFit(voig, data, Cand_Mass, "voigtian.png");
}

void testFitVoigtian()
{
  RooRealVar Cand_Mass("Cand_Mass", "m_{K_{s}^{0}p} (GeV)", 2.12, 2.43);
  RooRealVar mean("mean", "mean (GeV)", 2.29, 2.2, 2.36);
  RooRealVar width("width", "Breit-Wigner width (GeV)", 0.01, 0.0, 0.1);
  RooRealVar sigma("sigma", "sigma (GeV)", 0.04, 0.0, 0.1);
  RooVoigtian voig("voig", "The convolution of a Breit-Wigner with a Gaussian", Cand_Mass, mean, width, sigma, false);
  auto data = voig.generate(Cand_Mass, 10000);
  fitVoigtian(*data, Cand_Mass);
}

void fitRooCBShapeDS(RooAbsData& data, RooRealVar& Cand_Mass, RooWorkspace& ws)
{
  //RooRealVar Cand_Mass("Cand_Mass", "m_{K_{s}^{0}p} (GeV)", 2.12, 2.43);
  Cand_Mass.setRange("cbDS", 2.15, 2.45);
  RooRealVar mean("mean", "mean (GeV)", 2.29, 2.2, 2.36);
  RooRealVar sigma("sigma", "sigma (GeV)", 0.04, 0.0, 0.1);
  RooRealVar n1("n1", "n1 on left side", 3, 1,  100);
  RooRealVar n2("n2", "n2 on left side", 3, 1, 100);
  RooRealVar alpha1("alpha1", "alpha1 on left side", 1.2, 0, 5);
  RooRealVar alpha2("alpha2", "alpha2 on left side", 1.2, 0, 5);
  RooExtCBShape cbDS("cbDS", "Double-side Crystal Ball function", Cand_Mass, mean, sigma, alpha1, n1, alpha2, n2);
  cbDS.fitTo(data, Range("cbDS"));
  cbDS.fitTo(data);

  ws.importClassCode(cbDS.IsA());
  ws.import(cbDS);

  drawFit(cbDS, data, Cand_Mass, "CBShapeDS.png");
}

void MCLamC(const char* input_file="")
{
  TFile* f = TFile::Open(input_file);
  RooWorkspace* workspace;
  f->GetObject("lamc", workspace);
  f->Close();
  auto dataset = workspace->data("lamc");
  RooRealVar mass("Cand_Mass", "M_{pK^{0}_{s}} (GeV)", 2.13, 2.45);
  fitSingleGaussian(*dataset, mass);
  fitDoubleGaussian(*dataset, mass);
  fitVoigtian(*dataset, mass);
  fitRooCBShapeDS(*dataset, mass, *workspace);

  workspace->writeToFile("afterfit.root");
}
