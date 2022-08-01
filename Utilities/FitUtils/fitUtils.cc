#include "Utilities/FitUtils/fitUtils.h"

#include "TStyle.h"
#include "TGaxis.h"
#include "TLatex.h"
#include "RooAddPdf.h"
#include "RooRealVar.h"
#include "RooCBShape.h"
#include "RooChebychev.h"
#include "RooGaussian.h"
#include "RooFormulaVar.h"
#include "RooUniformBinning.h"
#include "RooPlot.h"
#include "RooHist.h"
#include "RooFitResult.h"
#include "TH1.h"
#include "TLegend.h"

using namespace RooFit;
using std::map;
using std::string;


#ifdef __FIT_UTILS__

void setVLines(RooCurve* curve)
{
  if (!curve) return;
  double xlo = curve->GetPointX(0);
  double xhi = curve->GetPointX(curve->GetN()-1);
  curve->addPoint(xhi, 0);
  curve->addPoint(xlo, 0);
}

void setCanvas(TCanvas& c)
{
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  c.SetLeftMargin(0);
  c.SetRightMargin(0);
  c.SetTopMargin(0);
  c.SetBottomMargin(0);

  c.Divide(1, 2, 0, 0);
  // c.Divide(1, 2);

  auto upperPad = c.cd(1);
  upperPad->SetTopMargin(0.08);
  upperPad->SetLeftMargin(0.14);
  upperPad->SetRightMargin(0.05);
  auto lowerPad = c.cd(2);
  lowerPad->SetLeftMargin(0.14);
  lowerPad->SetBottomMargin(0.14);
  lowerPad->SetRightMargin(0.05);

  upperPad->SetFrameBorderMode(0);
  upperPad->SetFrameBorderSize(0);
  upperPad->SetFrameFillStyle(0);
  // upperPad->SetFrameFillColor(TColor::GetColorTransparent(0, 0.5));
  upperPad->SetFillStyle(0);

  lowerPad->SetFrameBorderMode(0);
  lowerPad->SetFrameBorderSize(0);
  lowerPad->SetFrameFillStyle(0);
  // lowerPad->SetFrameFillColor(TColor::GetColorTransparent(0, 0.5));
  lowerPad->SetFillStyle(0);
}

void setKinematics(TVirtualPad* pad, map<string, string> strs,
                   const double x, const double y)
{
  pad->cd();
  TLatex tex;
  if (strs.count("alignment")) {
    // integer
    auto _code = std::stoi(strs.at("alignment"));
    tex.SetTextAlign(_code);
  } else tex.SetTextAlign(21);
  if (strs.count("font")) {
    // integer
    auto _code = std::stoi(strs.at("font"));
    tex.SetTextFont(_code);
  } else tex.SetTextFont(42);
  if (strs.count("fontsize")) {
    // float number
    auto _code = std::stof(strs.at("fontsize"));
    tex.SetTextAlign(_code);
  }
  const auto fontSize = tex.GetTextSize();
  const auto moveUpFactor = 1.2 * fontSize;
  int n = 0;
  float ypos = y;
  if (strs.count("pT")) {
    ypos = y + (n++) * moveUpFactor;
    tex.DrawLatexNDC(x, ypos, strs.at("pT").c_str());
  }
  if (strs.count("y")) {
    ypos = y + (n++) * moveUpFactor;
    tex.DrawLatexNDC(x, ypos, strs.at("y").c_str());
  }
  //if (strs.count("mult")) {
  //  ypos = y + (n++) * moveUpFactor;
  //  tex.DrawLatexNDC(x, ypos, strs.at("mult").c_str());
  //}
  //
  if (strs.count("DCA")) {
    ypos = y + (n++) * moveUpFactor;
    tex.DrawLatexNDC(x, ypos, strs.at("DCA").c_str());
  }

  if (strs.count("mult")) {
    //ypos = y + (n++) * moveUpFactor;
    tex.DrawLatexNDC(0.72, 0.2, strs.at("mult").c_str());
  }
}

void setCMS(TVirtualPad* pad)
{
  pad->cd();
  TLatex texCMS;
  texCMS.SetTextFont(42);
  texCMS.SetTextSize(0.05);
  texCMS.SetTextAlign(11);
  texCMS.DrawLatexNDC(0.14, 0.93, "#bf{CMS} #it{preliminary}");
}

void setCollision(TVirtualPad* pad)
{
  pad->cd();
  TLatex tex;
  tex.SetTextAlign(31);
  tex.SetTextSize(0.045);
  tex.SetTextFont(42);
  tex.DrawLatexNDC(0.95, 0.93, "pPb 8.16 TeV");
}

void setCollisionAndLumi(TVirtualPad* pad)
{
  pad->cd();
  TLatex tex;
  tex.SetTextAlign(31);
  tex.SetTextSize(0.045);
  tex.SetTextFont(42);
  tex.DrawLatexNDC(0.95, 0.93, "186 nb^{-1} pPb 8.16 TeV");
}

void fitGausCB(RooRealVar& mass, RooAbsData& ds,
               FitParConfigs::ParConfigs& par,
               std::map<std::string, std::string> strs)
{

  /**
     Model setup
  */
  // construct double  gaussian
  // parameters
  RooRealVar mean("mean", "mean (GeV)", par.getInit("mean"),
                  par.getMin("mean"), par.getMax("mean"));
  RooRealVar sigma1("sigma1", "sigma of 1st gaussian",
                    par.getInit("sigma1"), par.getMin("sigma1"),
                    par.getMax("sigma1"));
  RooRealVar sigma1N("sigma1N", "sigma ratio, 2nd/1st gaussian",
                     par.getInit("sigma1N"), par.getMin("sigma1N"),
                     par.getMax("sigma1N"));
  RooFormulaVar sigma2("sigma2", "@0*@1", RooArgSet(sigma1, sigma1N));
  RooRealVar alpha("alpha", "alpha of CB component",
                     par.getInit("alpha"),
                     par.getMin("alpha"), par.getMax("alpha"));
  RooRealVar nCB("nCB", "n of CB component",
                 par.getInit("nCB"),
                 par.getMin("nCB"), par.getMax("nCB"));

  RooRealVar frac1("frac1", "fraction of 1st gaussian",
                   par.getInit("frac1"), par.getMin("frac1"),
                   par.getMax("frac1"));

  RooGaussian gaus1("gaus1", "1st gaussian for signal",
                    mass, mean, sigma1);
  RooCBShape cbShape("cbShape","Crystal Ball function for signal",
                       mass, mean, sigma2, alpha, nCB);

  RooAddPdf gausCB("gausCB", "gaussian + CB of the signal",
                     RooArgList(gaus1, cbShape), frac1);

  /**
     Fit.
  */

  gausCB.fitTo(ds, Range("signal"));

  /**
     Draw
  */
  auto mass_frame = mass.frame();
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle(strs.at("xtitle").c_str());

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  c.cd(1);
  ds.plotOn(mass_frame);
  gausCB.plotOn(mass_frame, NormRange("signal"));
  mass_frame->Draw();

  c.cd(2);
  auto pull = mass_frame->pullHist();
  pull->SetTitle("");
  pull->GetXaxis()->SetTitle(mass_frame->GetXaxis()->GetTitle());
  pull->GetYaxis()->SetTitle("pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();

  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);

  const auto fig_path = strs.at("fig_path")+"_signal.png";
  c.Print(fig_path.c_str());

  /**
     Update the parameter set.
  */
  par.setInit("mean", mean.getVal());
  par.setInit("sigma1", sigma1.getVal());
  par.setInit("sigma1N", sigma1N.getVal());
  par.setInit("frac1", frac1.getVal());
}


void fitSignal(RooRealVar& mass, RooAbsData& ds,
               FitParConfigs::ParConfigs& par,
               std::map<std::string, std::string> strs)
{

  /**
     Model setup
  */
  // construct double  gaussian
  // parameters
  RooRealVar mean("mean", "mean (GeV)", par.getInit("mean"),
                  par.getMin("mean"), par.getMax("mean"));
  RooRealVar sigma1("sigma1", "sigma of 1st gaussian",
                    par.getInit("sigma1"), par.getMin("sigma1"),
                    par.getMax("sigma1"));
  RooRealVar sigma1N("sigma1N", "sigma ratio, 2nd/1st gaussian",
                     par.getInit("sigma1N"), par.getMin("sigma1N"),
                     par.getMax("sigma1N"));
  RooFormulaVar sigma2("sigma2", "@0*@1", RooArgSet(sigma1, sigma1N));
  RooRealVar frac1("frac1", "fraction of 1st gaussian",
                   par.getInit("frac1"), par.getMin("frac1"),
                   par.getMax("frac1"));

  RooGaussian gaus1("gaus1", "1st gaussian for signal",
                    mass, mean, sigma1);
  RooGaussian gaus2("gaus2", "2nd gaussian for signal",
                    mass, mean, sigma2);
  RooAddPdf doubGaus("doubGaus", "double gaussian of the signal",
                     RooArgList(gaus1, gaus2), frac1);

  /**
     Fit.
  */

  doubGaus.fitTo(ds, Range("signal"));

  /**
     Draw
  */
  auto mass_frame = mass.frame();
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle(strs.at("xtitle").c_str());

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  c.cd(1);
  ds.plotOn(mass_frame);
  doubGaus.plotOn(mass_frame, NormRange("signal"));
  mass_frame->Draw();

  c.cd(2);
  auto pull = mass_frame->pullHist();
  pull->SetTitle("");
  pull->GetXaxis()->SetTitle(mass_frame->GetXaxis()->GetTitle());
  pull->GetYaxis()->SetTitle("pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();

  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);

  const auto fig_path = strs.at("fig_path")+"_signal.png";
  c.Print(fig_path.c_str());

  /**
     Update the parameter set.
  */
  par.setInit("mean", mean.getVal());
  par.setInit("sigma1", sigma1.getVal());
  par.setInit("sigma1N", sigma1N.getVal());
  par.setInit("frac1", frac1.getVal());
}

void fitSwap(RooRealVar& mass, RooAbsData& ds,
             FitParConfigs::ParConfigs& par,
             map<string, string> strs)
{
  /**
     Model setup
   */
  RooRealVar meanSwap("meanSwap", "mean of swap",
                      par.getInit("meanSwap"),
                      par.getMin("meanSwap"),
                      par.getMax("meanSwap"));
  RooRealVar sigmaSwap("sigmaSwap", "sigma of the swap component",
                       par.getInit("sigmaSwap"),
                       par.getMin("sigmaSwap"),
                       par.getMax("sigmaSwap"));
  RooGaussian swapGaus("swapGaus", "swap gaussian",
                       mass, meanSwap, sigmaSwap);

  swapGaus.fitTo(ds, Range("signal"));

  /**
     Draw
  */

  auto mass_frame = mass.frame(Range("full"));
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle("M_{K#pi} (GeV)");

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  c.cd(1);
  ds.plotOn(mass_frame);
  swapGaus.plotOn(mass_frame, NormRange("signal"));
  mass_frame->Draw();

  c.cd(2);
  auto pull = mass_frame->pullHist();
  pull->SetTitle("");
  pull->GetXaxis()->SetTitle(mass_frame->GetXaxis()->GetTitle());
  pull->GetYaxis()->SetTitle("pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();


  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);

  const auto fig_path = strs.at("fig_path")+"_swap.png";
  c.Print(fig_path.c_str());

  /**
     Update the parameter set.
  */

  par.setInit("meanSwap", meanSwap.getVal());
  par.setInit("sigmaSwap", sigmaSwap.getVal());
}


void fitSignalAndSwap(RooRealVar& mass, RooAbsData& ds,
                      FitParConfigs::ParConfigs& par,
                      map<string, string> strs)
{
  /**
     Model setup
  */
  // construct double gaussian
  // parameters
  RooRealVar mean("mean", "mean (GeV)", par.getInit("mean"),
                  par.getMin("mean"), par.getMax("mean"));
  RooRealVar sigma1("sigma1", "sigma of 1st gaussian",
                    par.getInit("sigma1"), par.getMin("sigma1"),
                    par.getMax("sigma1"));
  RooRealVar sigma1N("sigma1N", "sigma ratio, 2nd/1st gaussian",
                     par.getInit("sigma1N"), par.getMin("sigma1N"),
                     par.getMax("sigma1N"));
  RooFormulaVar sigma2("sigma2", "@0*@1", RooArgSet(sigma1, sigma1N));
  RooRealVar frac1("frac1", "fraction of 1st gaussian",
                   par.getInit("frac1"), par.getMin("frac1"),
                   par.getMax("frac1"));

  RooGaussian gaus1("gaus1", "1st gaussian for signal",
                    mass, mean, sigma1);
  RooGaussian gaus2("gaus2", "2nd gaussian for signal",
                    mass, mean, sigma2);
  RooAddPdf doubGaus("doubGaus", "double gaussian of the signal",
                     RooArgList(gaus1, gaus2), frac1);

  // construct double gaussian + swap
  RooRealVar meanSwap("meanSwap", "mean of swap",
                      par.getInit("meanSwap"),
                      par.getMin("meanSwap"),
                      par.getMax("meanSwap"));
  RooRealVar sigmaSwap("sigmaSwap", "sigma of the swap component",
                       par.getInit("sigmaSwap"),
                       par.getMin("sigmaSwap"),
                       par.getMax("sigmaSwap"));
  RooRealVar doubGausFrac("doubGausFrac",
                          "fraction of the double gaussian",
                          par.getInit("doubGausFrac"),
                          par.getMin("doubGausFrac"),
                          par.getMax("doubGausFrac"));
  RooGaussian swapGaus("swapGaus", "swap gaussian",
                       mass, meanSwap, sigmaSwap);

  RooAddPdf signal("signal", "true and swap D",
                   RooArgList(doubGaus, swapGaus), doubGausFrac);

  /**
     Fit
  */
  mean.setConstant(kTRUE);
  sigma1.setConstant(kTRUE);
  sigma1N.setConstant(kTRUE);
  frac1.setConstant(kTRUE);

  meanSwap.setConstant(kTRUE);
  sigmaSwap.setConstant(kTRUE);

  signal.fitTo(ds, Range("signal"));

  /**
     Draw
  */
  auto mass_frame = mass.frame(Range("full"));
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle("M_{K#pi} (GeV)");

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  c.cd(1);
  ds.plotOn(mass_frame);
  signal.plotOn(mass_frame, Components("doubGaus"),
                LineStyle(kDashed), LineColor(kRed));
  signal.plotOn(mass_frame, Components("swapGaus"),
                LineStyle(kDashed), LineColor(kYellow));
  signal.plotOn(mass_frame, NormRange("signal"));
  mass_frame->Draw();

  c.cd(2);
  auto pull = mass_frame->pullHist();
  pull->SetTitle("");
  pull->GetXaxis()->SetTitle(mass_frame->GetXaxis()->GetTitle());
  pull->GetYaxis()->SetTitle("pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();


  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);

  const auto fig_path = strs.at("fig_path")+"_MCD0all.png";
  c.Print(fig_path.c_str());

  /**
     Update the Parameter set
  */
  par.setInit("doubGausFrac", doubGausFrac.getVal());
}

void fitCBShapeKK(RooRealVar& mass, RooAbsData& ds,
                  FitParConfigs::ParConfigs& par,
                  std::map<std::string, std::string> strs)
{
  /**
     Model setup
  */
  // Crystal Ball function
  RooRealVar m0KK("m0KK", "Guassian mean of KK component",
                  par.getInit("m0KK"),
                  par.getMin("m0KK"), par.getMax("m0KK"));
  RooRealVar sigmaKK("sigmaKK", "Guassian sigma of KK component",
                     par.getInit("sigmaKK"),
                     par.getMin("sigmaKK"), par.getMax("sigmaKK"));
  RooRealVar alphaKK("alphaKK", "alpha of KK component",
                     par.getInit("alphaKK"),
                     par.getMin("alphaKK"), par.getMax("alphaKK"));
  RooRealVar nCBKK("nCBKK", "n of D0->K+K- component",
                 par.getInit("nCBKK"),
                 par.getMin("nCBKK"), par.getMax("nCBKK"));

  RooCBShape cbShapeKK("cbShapeKK","Crystal Ball function for KK",
                       mass, m0KK, sigmaKK, alphaKK, nCBKK);

  /**
     Fit
  */
  cbShapeKK.fitTo(ds, Range("KK"));
  cbShapeKK.fitTo(ds);

  /**
     Draw
  */
  auto mass_frame = mass.frame(Range("full"));
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle("M_{KK} (GeV)");

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  c.cd(1);
  ds.plotOn(mass_frame);
  cbShapeKK.plotOn(mass_frame, NormRange("KK"), Range("full"));
  mass_frame->Draw();

  c.cd(2);
  auto pull = mass_frame->pullHist();
  pull->SetTitle("");
  pull->GetXaxis()->SetTitle(mass_frame->GetXaxis()->GetTitle());
  pull->GetYaxis()->SetTitle("pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();


  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);

  const auto fig_path = strs.at("fig_path")+"_MCKK.png";
  c.Print(fig_path.c_str());

  /**
     Update the parameter set
  */
  par.setInit("m0KK", m0KK.getVal());
  par.setInit("sigmaKK", sigmaKK.getVal());
  par.setInit("alphaKK", alphaKK.getVal());
  par.setInit("nCBKK", nCBKK.getVal());
}

void fitCBShapePiPi(RooRealVar& mass, RooAbsData& ds,
                    FitParConfigs::ParConfigs& par,
                    std::map<std::string, std::string> strs)
{
  /**
     Model setup
  */
  // Crystal Ball function
  RooRealVar m0PiPi("m0PiPi", "Guassian mean of PiPi component",
                    par.getInit("m0PiPi"),
                    par.getMin("m0PiPi"), par.getMax("m0PiPi"));
  RooRealVar sigmaPiPi("sigmaPiPi", "Guassian sigma of PiPi component",
                       par.getInit("sigmaPiPi"),
                       par.getMin("sigmaPiPi"), par.getMax("sigmaPiPi"));
  RooRealVar alphaPiPi("alphaPiPi", "alpha of PiPi component",
                       par.getInit("alphaPiPi"),
                       par.getMin("alphaPiPi"), par.getMax("alphaPiPi"));
  RooRealVar nCBPiPi("nCBPiPi", "n of PiPi component",
                     par.getInit("nCBPiPi"),
                     par.getMin("nCBPiPi"), par.getMax("nCBPiPi"));

  RooCBShape cbShapePiPi("cbShapePiPi",
                         "Crystal Ball function for PiPi",
                         mass, m0PiPi, sigmaPiPi, alphaPiPi, nCBPiPi);

  /**
     Fit
  */
  cbShapePiPi.fitTo(ds, Range("PiPi"));
  cbShapePiPi.fitTo(ds);

  /**
     Draw
  */
  auto mass_frame = mass.frame(Range("full"));
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle("M_{#pi#pi} (GeV)");

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  c.cd(1);
  ds.plotOn(mass_frame);
  cbShapePiPi.plotOn(mass_frame, NormRange("PiPi"), Range("full"));
  mass_frame->Draw();

  c.cd(2);
  auto pull = mass_frame->pullHist();
  pull->SetTitle("");
  pull->GetXaxis()->SetTitle(mass_frame->GetXaxis()->GetTitle());
  pull->GetYaxis()->SetTitle("pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();


  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);

  const auto fig_path = strs.at("fig_path")+"_MCPiPi.png";
  c.Print(fig_path.c_str());

  /**
     Update the parameter set
  */
  par.setInit("m0PiPi", m0PiPi.getVal());
  par.setInit("sigmaPiPi", sigmaPiPi.getVal());
  par.setInit("alphaPiPi", alphaPiPi.getVal());
  par.setInit("nCBPiPi", nCBPiPi.getVal());
}


RooFitResult fitD0(RooRealVar& mass, RooAbsData& ds, RooWorkspace& ws,
           FitParConfigs::ParConfigs& par, const FitOptsHF& fitOpts,
           std::map<std::string, std::string> strs)
{
  bool useRareProcesses = par.hasVariable("nKK") && par.hasVariable("nPiPi");
  std::cout << "useRareProcesses is " << std::boolalpha << useRareProcesses << "\n";
  /**
     Model setup
  */
  // common scale factor
  RooRealVar scale("scale", "width ratio for data/MC",
                   par.getInit("scale"),
                   par.getMin("scale"), par.getMax("scale"));

  // construct double gaussian
  // parameters
  RooRealVar mean("mean", "mean (GeV)", par.getInit("mean"),
                  par.getMin("mean"), par.getMax("mean"));
  // sigma1 in par was obtained from MC
  RooRealVar sigma1MC("sigma1MC", "sigma of 1st gaussian in MC",
                    par.getInit("sigma1"), par.getMin("sigma1"),
                    par.getMax("sigma1"));
  RooFormulaVar sigma1("sigma1", "sigma of 1st gaussian", "@0*@1",
                       RooArgList(sigma1MC, scale));
  RooRealVar sigma1N("sigma1N", "sigma ratio, 2nd/1st gaussian",
                     par.getInit("sigma1N"), par.getMin("sigma1N"),
                     par.getMax("sigma1N"));
  RooFormulaVar sigma2("sigma2", "@0*@1", RooArgSet(sigma1, sigma1N));
  RooRealVar frac1("frac1", "fraction of 1st gaussian",
                   par.getInit("frac1"), par.getMin("frac1"),
                   par.getMax("frac1"));

  RooGaussian gaus1("gaus1", "1st gaussian for signal",
                    mass, mean, sigma1);
  RooGaussian gaus2("gaus2", "2nd gaussian for signal",
                    mass, mean, sigma2);
  RooAddPdf doubGaus("doubGaus", "double gaussian of the signal",
                     RooArgList(gaus1, gaus2), frac1);

  // construct double gaussian + swap
  RooRealVar meanSwap("meanSwap", "mean of swap",
                      par.getInit("meanSwap"),
                      par.getMin("meanSwap"),
                      par.getMax("meanSwap"));
  // sigma for swap in par was obtained in MC
  RooRealVar sigmaSwapMC("sigmaSwapMC", "sigma of the swap component in MC",
                       par.getInit("sigmaSwap"),
                       par.getMin("sigmaSwap"),
                       par.getMax("sigmaSwap"));
  RooFormulaVar sigmaSwap("sigmaSwap", "sigma of the swap component",
                          "@0*@1", RooArgList(sigmaSwapMC, scale));
  RooGaussian swapGaus("swapGaus", "swap gaussian",
                       mass, meanSwap, sigmaSwap);

  //  RooAddPdf signal("signal", "true and swap D",
  //               RooArgList(doubGaus, swapGaus), doubGausFrac);

  // Crystal Ball function for KK
  RooRealVar m0KK("m0KK", "Guassian mean of KK component",
                  par.getInit("m0KK", 0),
                  par.getMin("m0KK", 0), par.getMax("m0KK", 0));
  // sigma for KK in par was obtained in MC
  RooRealVar sigmaKKMC("sigmaKKMC", "Guassian sigma of KK component in MC",
                     par.getInit("sigmaKK", 1),
                     par.getMin("sigmaKK", 1), par.getMax("sigmaKK", 1));
  RooFormulaVar sigmaKK("sigmaKK", "sigma for D0->K+K-", "@0*@1",
                        RooArgList(sigmaKKMC, scale));
  RooRealVar alphaKK("alphaKK", "alpha of KK component",
                     par.getInit("alphaKK", 1),
                     par.getMin("alphaKK", 1), par.getMax("alphaKK", 1));
  RooRealVar nCBKK("nCBKK", "n of KK component",
                 par.getInit("nCBKK", 1),
                 par.getMin("nCBKK", 1), par.getMax("nCBKK", 1));

  RooCBShape cbShapeKK("cbShapeKK","Crystal Ball function for KK",
                       mass, m0KK, sigmaKK, alphaKK, nCBKK);

  // Crystal Ball function for PiPi
  RooRealVar m0PiPi("m0PiPi", "Guassian mean of PiPi component",
                    par.getInit("m0PiPi", 0),
                    par.getMin("m0PiPi", 0), par.getMax("m0PiPi", 0));
  // sigma for PiPi in par was obtained in MC
  RooRealVar sigmaPiPiMC("sigmaPiPiMC",
                         "Guassian sigma of PiPi component in MC",
                         par.getInit("sigmaPiPi", 1),
                         par.getMin("sigmaPiPi", 1), par.getMax("sigmaPiPi", 1));
  RooFormulaVar sigmaPiPi("sigmaPiPi", "sigma for D0->Pi+Pi-", "@0*@1",
                        RooArgList(sigmaPiPiMC, scale));
  RooRealVar alphaPiPi("alphaPiPi", "alpha of PiPi component",
                       par.getInit("alphaPiPi", 1),
                       par.getMin("alphaPiPi", 1), par.getMax("alphaPiPi", 1));
  RooRealVar nCBPiPi("nCBPiPi", "n of PiPi component",
                   par.getInit("nCBPiPi", 1),
                   par.getMin("nCBPiPi", 1), par.getMax("nCBPiPi", 1));

  RooCBShape cbShapePiPi("cbShapePiPi","Crystal Ball function for PiPi",
                         mass, m0PiPi, sigmaPiPi, alphaPiPi, nCBPiPi);

  // 3rd order Chebyshev polynomial for background
  RooRealVar a1("a1", "a1 for Chebyshev(1)", par.getInit("a1"),
                par.getMin("a1"), par.getMax("a1"));
  RooRealVar a2("a2", "a2 for Chebyshev(2)", par.getInit("a2"),
                par.getMin("a2"), par.getMax("a2"));
  RooRealVar a3("a3", "a3 for Chebyshev(3)", par.getInit("a3"),
                par.getMin("a3"), par.getMax("a3"));
  RooRealVar a4("a4", "a4 for Chebyshev(4)", 0, -1, 1);
  if (fitOpts.order == 4) {
    try {
      a4.setMin(par.getMin("a4"));
      a4.setMax(par.getMax("a4"));
      a4.setVal(par.getInit("a4"));
    } catch(std::out_of_range& e) {
      throw std::out_of_range("a4 is not set in configuration");
    }
  }
  RooArgList bkg_args(a1, a2);
  if (fitOpts.order <= 4) bkg_args.add(a3);
  if (fitOpts.order == 4) bkg_args.add(a4);
  RooChebychev bkg("bkg", "3rd Chebyshev poly.", mass, bkg_args);

  // yields for different components
  RooRealVar doubGausFrac("doubGausFrac",
                          "fraction of the double gaussian",
                          par.getInit("doubGausFrac"),
                          par.getMin("doubGausFrac"),
                          par.getMax("doubGausFrac"));
  RooRealVar nsig("nsig", "yields for true component",
                  par.getInit("nsig"),
                  par.getMin("nsig"), par.getMax("nsig"));
  RooFormulaVar nswap("nswap", "yields for swap component",
                      "@0/@1*(1-@1)", RooArgList(nsig, doubGausFrac));
  RooRealVar nKK("nKK", "yields for KK component",
                 par.getInit("nKK", 0),
                 par.getMin("nKK", 0), par.getMax("nKK", 0));
  RooRealVar nPiPi("nPiPi", "yields for PiPi component",
                 par.getInit("nPiPi", 0),
                 par.getMin("nPiPi", 0), par.getMax("nPiPi", 0));
  RooRealVar nbkg("nbkg", "yields for background component",
                  par.getInit("nbkg"),
                  par.getMin("nbkg"), par.getMax("nbkg"));

  // add components together
  auto sum = useRareProcesses ?
    RooAddPdf("sum", "sig+swap+KK+PiPi+bkg",
                RooArgList(bkg, cbShapePiPi, cbShapeKK, swapGaus, doubGaus),
                RooArgList(nbkg, nPiPi, nKK, nswap, nsig)) :
    RooAddPdf("sum", "sig+swap+bkg",
                RooArgList(bkg, swapGaus, doubGaus),
                RooArgList(nbkg, nswap, nsig));
  /**
     Fit
   */
  // remember to fix the all sigma in MC. they are labeled by MC
  // true D0
  sigma1MC.setConstant(kTRUE);
  sigma1N.setConstant(kTRUE);
  frac1.setConstant(kTRUE);
  // swap D0
  meanSwap.setConstant(kTRUE);
  sigmaSwapMC.setConstant(kTRUE);
  // true D0 / (true + swap D0)
  // need to study its effects
  doubGausFrac.setConstant(kTRUE);
  const double doubGausFracInit = doubGausFrac.getVal();
  const double doubGausFracErrorHiInit = doubGausFrac.getErrorHi();
  const double doubGausFracErrorLoInit = doubGausFrac.getErrorLo();
  if (useRareProcesses) {
    // CB for KK
    m0KK.setConstant(kTRUE);
    sigmaKKMC.setConstant(kTRUE);
    alphaKK.setConstant(kTRUE);
    nCBKK.setConstant(kTRUE);
    // CB for PiPi
    m0PiPi.setConstant(kTRUE);
    sigmaPiPiMC.setConstant(kTRUE);
    alphaPiPi.setConstant(kTRUE);
    nCBPiPi.setConstant(kTRUE);
  }
  scale.setConstant(fitOpts.fixScale);
  mean.setConstant(fitOpts.fixMean);

  // auto& parameters = *sum.getParameters(mass, false);
  auto& parameters = *sum.getParameters(mass);
  for(const auto& varName : fitOpts.fixNames) {
    auto& var = dynamic_cast<RooRealVar&>(parameters[varName.c_str()]);
    var.setConstant(kTRUE);
    std::cout << "Setting " << varName << " to constant\n";
  }
  if (nKK.isConstant()) std::cout << "nKK is constant\n";
  if (nPiPi.isConstant()) std::cout << "nPiPi is constant\n";


  TH1* h = nullptr;
  RooDataHist* dhptr = nullptr;
  RooFitResult* result = nullptr;
  std::pair<double, double> binEdges = mass.getRange("full");
  RooAbsData* dsAbsPtr = &ds;
  if (auto dsPtr = dynamic_cast<RooDataSet*>(dsAbsPtr) ) {
    h = dsPtr->createHistogram("binned", mass,
                                    Binning(fitOpts.nBins));
    if (fitOpts.useHist) {
      auto dh = RooDataHist("dh", "", mass, h);
      result = sum.fitTo(dh, Range("full"), Save(), AsymptoticError(fitOpts.useWeight));
    }
  } else {
    result = sum.fitTo(ds, Range("full"), Save(), NumCPU(fitOpts.numCPU),
                       AsymptoticError(fitOpts.useWeight));
  }

  if (!fitOpts.useHistOnly) {
    result = sum.fitTo(ds, Range("full"), Save(), NumCPU(fitOpts.numCPU),
                       AsymptoticError(fitOpts.useWeight));
  }

  if (h) { 
    h->SetName(::Form("%s_binned", ds.GetName()));
    std::string name = ds.GetName();
    // name starts with ds
    auto pos = name.find("ds");
    name.replace(pos, 2, "dh");
    dhptr = new RooDataHist(name.c_str(), "", mass, h);
    std::cout << "Created RooDataHist " << name << "\n";
  } else { dhptr = dynamic_cast<RooDataHist*>(&ds); };

  /**
     Draw
   */
  gStyle->SetCanvasPreferGL(1);
  auto mass_frame = mass.frame(Range("full"));
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle("M_{K#pi} (GeV)");

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  // c.cd(1);
  ds.plotOn(mass_frame, Name("curve_dataset"), Title(""), Binning(fitOpts.nBins));
  sum.plotOn(mass_frame, Components(bkg), LineColor(kRed),
             Normalization(1.0, RooAbsReal::RelativeExpected),
             LineStyle(kDashed), MarkerStyle(20),
             Range("full"), NormRange("full"),
             Name("curve_bkg")
             );
  sum.plotOn(mass_frame, Components(doubGaus), DrawOption("F"),
             FillColor(TColor::GetColorTransparent(kOrange-3, 0.3)),
             LineWidth(0),
             Normalization(1.0, RooAbsReal::RelativeExpected),
             Range("full"), NormRange("full"),
             Name("curve_doubGaus")
             );
  sum.plotOn(mass_frame, Components(swapGaus), DrawOption("F"),
             FillColor(TColor::GetColorTransparent(kGreen+4, 0.3)),
             LineWidth(0),
             Normalization(1.0, RooAbsReal::RelativeExpected),
             Range("full"), NormRange("full"),
             Name("curve_swapGaus")
             );
  if (useRareProcesses) {
    sum.plotOn(mass_frame, Components(cbShapeKK), DrawOption("F"),
        FillColor(TColor::GetColorTransparent(kBlue-6, 0.3)),
        LineWidth(0),
        Normalization(1.0, RooAbsReal::RelativeExpected),
        Range("full"), NormRange("full"),
        Name("curve_cbShapeKK")
        );
    sum.plotOn(mass_frame, Components(cbShapePiPi), DrawOption("F"),
        FillColor(TColor::GetColorTransparent(kRed-3, 0.3)),
        LineWidth(0),
        Normalization(1.0, RooAbsReal::RelativeExpected),
        Range("full"), NormRange("full"),
        Name("curve_cbShapePiPi")
        );
  }
  sum.plotOn(mass_frame,
             Normalization(1.0, RooAbsReal::RelativeExpected),
             Range("full"), NormRange("full"));
  mass_frame->SetMaximum(mass_frame->GetMaximum() * 1.1);
  // mass_frame->Draw();

  /// get `RooCurve`s
  auto curve_data = mass_frame->getHist("curve_dataset");
  auto curve_bkg = mass_frame->getCurve("curve_bkg");
  auto curve_doubGaus = mass_frame->getCurve("curve_doubGaus");
  auto curve_swapGaus = mass_frame->getCurve("curve_swapGaus");
  auto curve_cbShapeKK = useRareProcesses ? mass_frame->getCurve("curve_cbShapeKK") : nullptr;
  auto curve_cbShapePiPi = useRareProcesses ? mass_frame->getCurve("curve_cbShapePiPi") : nullptr;

  // set VLines
  setVLines(curve_doubGaus);
  setVLines(curve_swapGaus);
  setVLines(curve_cbShapeKK);
  setVLines(curve_cbShapePiPi);

  // build legend
  TLegend leg(0.62, 0.5, 0.9, 0.91);
  leg.SetFillStyle(0);
  leg.SetBorderSize(0);
  leg.AddEntry(curve_data, "Data", "p");
  leg.AddEntry(curve_doubGaus, "Signal", "f");
  leg.AddEntry(curve_swapGaus, "Swap", "f");
  if (useRareProcesses) {
    leg.AddEntry(curve_cbShapeKK,
        "D^{0}#rightarrow K^{+}K^{-}", "f");
    leg.AddEntry(curve_cbShapePiPi,
        "D^{0}#rightarrow#pi^{+}#pi^{-}", "f");
  }
  leg.AddEntry(curve_bkg, "Background", "l");
  // leg.Draw();

  c.cd(2);
  auto pull = mass_frame->pullHist();
  pull->SetTitle("");
  pull->GetXaxis()->SetTitle(mass_frame->GetXaxis()->GetTitle());
  pull->GetYaxis()->SetTitle("pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();

  c.cd(1);
  mass_frame->Draw();
  leg.Draw();

  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);
  TLatex tex;
  tex.SetTextFont(42);
  tex.SetTextAlign(11);
  tex.DrawLatexNDC(0.2, 0.2,
                   Form("N_{sig}=%.1f+/-%.1f", nsig.getVal(),
                        nsig.getErrorHi()));
  auto errorType = fitOpts.useWeight ? RooAbsData::SumW2 : RooAbsData::Poisson;
  auto chi2Var = dhptr ? sum.createChi2(*dhptr, Extended(kTRUE), Range("full"), DataError(errorType)) : nullptr;
  double chi2Val = chi2Var ?  chi2Var->getVal() : 0.;
  int ndf = 0;
  if (h) {
    ndf = fitOpts.nBins - result->floatParsFinal().getSize();
  }
  else {
    // works after 6.26
    // ndf = dhptr->getBinnings()->numBins() - result->floatParsFinal().getSize();
    ndf = ((RooUniformBinning &)mass.getBinning(0)).numBins() - result->floatParsFinal().getSize();
  }
  tex.DrawLatexNDC(0.2, 0.02, Form("#chi^{2}/N.D.F = %.1f/%d", chi2Val, ndf));

  const auto fig_path = strs.at("fig_path")+"_data.png";
  const auto fig_path_pdf = strs.at("fig_path")+"_data.pdf";
  c.Print(fig_path.c_str());
  c.Print(fig_path_pdf.c_str());

  /**
     Update the parameter set
   */
  par.setInit("a1", a1.getVal());
  par.setInit("a2", a2.getVal());
  par.setInit("a3", a3.getVal());
  par.setInit("scale", scale.getVal());
  par.setInit("nsig", nsig.getVal());
  if (useRareProcesses) {
    par.setInit("nKK", nKK.getVal());
    par.setInit("nPiPi", nPiPi.getVal());
  }
  par.setInit("nbkg", nbkg.getVal());
  par.setInit("doubGausFrac", doubGausFrac.getVal());

  std::cout << "\n";
  std::cout << "Final nsig is " << nsig.getVal()
            << " + " << nsig.getAsymErrorHi()
            << " - " << nsig.getAsymErrorLo()
            << "\n";
  std::cout << "double gaus frac in MC is " << doubGausFracInit
            << " + " << doubGausFracErrorHiInit
            << " - " << doubGausFracErrorLoInit
            << "\n";
  std::cout << "double gaus frac in data is " << doubGausFrac.getVal()
            << " + " << doubGausFrac.getAsymErrorHi()
            << " - " << doubGausFrac.getAsymErrorLo()
            << std::endl;
  std::cout << "status is " << result->status() << std::endl;
  if (fitOpts.saveToWS) ws.import(sum);
  if (h && dhptr) {
    if (fitOpts.saveToWS) ws.import(*dhptr);
    delete dhptr;
  }
  if (h) delete h;
  return *result;
}


RooFitResult fitLamC(RooRealVar& mass, RooAbsData& ds, RooWorkspace& ws,
             FitParConfigs::ParConfigs& par, const FitOptsHF& fitOpts,
             std::map<std::string, std::string> strs)
{
  /**
     Model setup
  */
  // common scale factor
  RooRealVar scale("scale", "width ratio for data/MC",
                   par.getInit("scale"),
                   par.getMin("scale"), par.getMax("scale"));

  if (fitOpts.fixScale) {
    scale.setVal(1.);
    scale.setConstant(kTRUE);
  }

  // construct double gaussian
  // parameters
  RooRealVar mean("mean", "mean (GeV)", par.getInit("mean"),
                  par.getMin("mean"), par.getMax("mean"));
  if (fitOpts.fixMean) {
    mean.setConstant(kTRUE);
  }
  // sigma1 in par was obtained from MC
  RooRealVar sigma1MC("sigma1MC", "sigma of 1st gaussian in MC",
                    par.getInit("sigma1"), par.getMin("sigma1"),
                    par.getMax("sigma1"));
  RooFormulaVar sigma1("sigma1", "sigma of 1st gaussian", "@0*@1",
                       RooArgList(sigma1MC, scale));
  RooRealVar sigma1N("sigma1N", "sigma ratio, 2nd/1st gaussian",
                     par.getInit("sigma1N"), par.getMin("sigma1N"),
                     par.getMax("sigma1N"));
  RooFormulaVar sigma2("sigma2", "@0*@1", RooArgSet(sigma1, sigma1N));
  RooRealVar frac1("frac1", "fraction of 1st gaussian",
                   par.getInit("frac1"), par.getMin("frac1"),
                   par.getMax("frac1"));

  RooGaussian gaus1("gaus1", "1st gaussian for signal",
                    mass, mean, sigma1);
  RooGaussian gaus2("gaus2", "2nd gaussian for signal",
                    mass, mean, sigma2);
  RooAddPdf doubGaus("doubGaus", "double gaussian of the signal",
                     RooArgList(gaus1, gaus2), frac1);

  // 3rd order Chebyshev polynomial for background
  RooRealVar a1("a1", "a1 for Chebyshev(1)", par.getInit("a1"),
                par.getMin("a1"), par.getMax("a1"));
  RooRealVar a2("a2", "a2 for Chebyshev(2)", par.getInit("a2"),
                par.getMin("a2"), par.getMax("a2"));
  RooRealVar a3("a3", "a3 for Chebyshev(3)", par.getInit("a3"),
                par.getMin("a3"), par.getMax("a3"));
  RooRealVar* a4 = nullptr;
  // RooChebychev bkg("bkg", "3rd Chebyshev poly.", mass,
  //                 RooArgList(a1, a2, a3));
  
  // you can also use RooWorkspace::factory
  RooChebychev* bkg;
  switch (fitOpts.order) {
    case 2: bkg = new RooChebychev("bkg", "2nd Chebyshev poly.",
                                   mass, RooArgList(a1, a2));
            break;
    case 3: bkg = new RooChebychev("bkg", "3rd Chebyshev poly.",
                                   mass, RooArgList(a1, a2, a3));
            break;
    case 4: {
            a4 = new RooRealVar("a4", "a4 for Chebyshev(4)", par.getInit("a4"),
                          par.getMin("a4"), par.getMax("a4"));
            bkg = new RooChebychev("bkg", "4th Chebyshev poly.",
                                   mass, RooArgList(a1, a2, a3, *a4));
            std::cout << "Done a4 " << a4->getVal() << "\n";
            break;
            }
    default:
            throw std::logic_error("Cannot construct background pdf");
  }
            std::cout << "bkg done \n";

  // yields for different components
  RooRealVar nsig("nsig", "yields for true component",
                  par.getInit("nsig"),
                  par.getMin("nsig"), par.getMax("nsig"));
  RooRealVar nbkg("nbkg", "yields for background component",
                  par.getInit("nbkg"),
                  par.getMin("nbkg"), par.getMax("nbkg"));

  // add components together
  RooAddPdf sum("sum", "sig+swap+KK+PiPi+bkg",
                RooArgList(*bkg, doubGaus),
                RooArgList(nbkg, nsig));

  /**
     Fit
   */
  // remember to fix the all sigma in MC. they are labeled by MC
  // true D0
  sigma1MC.setConstant(kTRUE);
  sigma1N.setConstant(kTRUE);
  frac1.setConstant(kTRUE);

  RooAbsData* dsAbsPtr = &ds;

  TH1* h = nullptr;
  RooDataHist* dhptr = nullptr;
  RooFitResult* result;
  std::pair<double, double> binEdges = mass.getRange("full");
  if (auto dsPtr = dynamic_cast<RooDataSet*>(dsAbsPtr) ) {
    h = dsPtr->createHistogram("binned", mass,
                               Binning(fitOpts.nBins, binEdges.first, binEdges.second));
    auto bkgYields = h->Integral(1, fitOpts.nBins);
    // fix later
    nbkg.setVal(bkgYields);
    nbkg.setMin(bkgYields * 0.9);
    nbkg.setMax(bkgYields * 1.1);
    if(fitOpts.useHist) {
      auto dh = RooDataHist("dh", "", mass, h);
      result = sum.fitTo(dh, Range("full"), Save(), AsymptoticError(fitOpts.useWeight));
      // if (fitOpts.useWeight)
      //  result = sum.chi2FitTo(dh, Range(binEdges.first, binEdges.second), NumCPU(fitOpts.numCPU), Save(), Minos(kFALSE));
    }
  } else {
    result = sum.fitTo(ds, Range("full"), NumCPU(fitOpts.numCPU), Save(), AsymptoticError(fitOpts.useWeight));
    // if (fitOpts.useWeight) {
    //  result = sum.chi2FitTo(dynamic_cast<RooDataHist&>(ds), Range(binEdges.first, binEdges.second), NumCPU(fitOpts.numCPU), Save(), Minos(kFALSE));
    // }
  }
  if (!fitOpts.useHistOnly) {
    result = sum.fitTo(ds, Range("full"), NumCPU(fitOpts.numCPU), Save(), AsymptoticError(fitOpts.useWeight));
  }

  if (h) { 
    h->SetName(::Form("%s_binned", ds.GetName()));
    std::string name = ds.GetName();
    // name starts with ds
    auto pos = name.find("ds");
    name.replace(pos, 2, "dh");
    dhptr = new RooDataHist(name.c_str(), "", mass, h);
    std::cout << "Created RooDataHist " << name << "\n";
  } else { dhptr = dynamic_cast<RooDataHist*>(&ds); };

  std::cout << "Finished fit\n";

  /**
     Draw
   */
  // gStyle->SetCanvasPreferGL(1);
  //
  TGaxis::SetExponentOffset(-0.05, 0, "y");
  auto mass_frame = mass.frame(Range("full"));
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle("M_{K_{S}^{0}p} (GeV)");

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  // c.cd(1);
  // dataset plotOn does not support Range option
  // Name option is for the RooHist or RooCurve objects
  // I do not know how to handle Normalization(scaleType)
  RooPlot* dsplot;
  if (h) {
    dsplot = ds.plotOn(mass_frame, Name("curve_mydataset"),
                       Binning(fitOpts.nBins, binEdges.first, binEdges.second),
                       MarkerStyle(20));
  } else {
    dsplot = ds.plotOn(mass_frame, Name("curve_mydataset"), MarkerStyle(20));
  }
  sum.plotOn(mass_frame, Components(*bkg), LineColor(kRed),
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             LineStyle(kDashed), MarkerStyle(20),
             Range("full"), NormRange("full"),
             Name("curve_bkg")
             );
  sum.plotOn(mass_frame, Components(doubGaus), DrawOption("F"),
             FillColor(TColor::GetColorTransparent(kOrange-3, 0.3)),
             LineWidth(0),
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             Range("full"), NormRange("full"),
             Name("curve_doubGaus")
             );
  sum.plotOn(mass_frame,
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             Range("full"), NormRange("full"),
             Name("curve_sum")
             );
  // mass_frame->SetMaximum(mass_frame->GetMaximum() * 1.1);

  /// get `RooCurve`s

  // curve_data is RooHist, instead of RooCurve
  auto curve_data = mass_frame->getHist("curve_mydataset");
  // pdf plots are RooCurve
  auto curve_sum = mass_frame->getCurve("curve_sum");
  auto curve_bkg = mass_frame->getCurve("curve_bkg");
  auto curve_doubGaus = mass_frame->getCurve("curve_doubGaus");

  // set VLines
  setVLines(curve_doubGaus);

  // build legend
  //TLegend leg(0.62, 0.5, 0.9, 0.91);
  TLegend leg(0.62, 0.45, 0.9, 0.82);
  leg.SetFillStyle(0);
  leg.SetBorderSize(0);
  // this line works
  // leg.AddEntry(dsplot->getHist("curve_mydataset"), "Data", "p");
  // this line works as well
  leg.AddEntry(curve_data, "Data", "p");
  // I do not know why the line below does not work.
  // leg.AddEntry("curve_mydata", "Data", "p");
  // this line does not work because ds is a RooPlot
  // leg.AddEntry(dsplot, "Data", "p");
  leg.AddEntry(curve_doubGaus, "Signal", "f");
  leg.AddEntry(curve_bkg, "Background", "l");

  c.cd();
  auto smallPad =  TPad("smallPad", "", 0.15, 0.57, 0.55, 0.82, 0, -1, 1);
  smallPad.Draw();
  smallPad.cd();
  mass.setRange("plotable", 2.22, 2.34);
  auto small_frame = mass.frame(Name("small_frame"), Range("plotable"), Title("zoomin"));
  if (h) {
    ds.plotOn(small_frame, Name(""),
              Binning(fitOpts.nBins, binEdges.first, binEdges.second),
              MarkerStyle(20));
  } else {
    ds.plotOn(small_frame, Name(""), MarkerStyle(20));
  }
  sum.plotOn(small_frame, Name("zoomin_bkg"),
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             Components(*bkg), LineColor(kRed), LineStyle(kDashed),
             MarkerStyle(20), Range("plotable"), NormRange("full"));
  sum.plotOn(small_frame, Name("zoomin_sum"),
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             MarkerStyle(20), Range("plotable"), NormRange("full"));
  auto curve_zoomin = small_frame->getCurve("zoomin_sum");
  double y_max = *std::max_element(curve_zoomin->GetY(),
                                   curve_zoomin->GetY() + curve_zoomin->GetN());
  double y_min = *std::min_element(curve_zoomin->GetY(),
                                   curve_zoomin->GetY() + curve_zoomin->GetN());
  small_frame->SetMaximum(y_max + 0.5 * (y_max - y_min));
  small_frame->SetMinimum(y_min - 0.5 * (y_max - y_min));
  small_frame->GetXaxis()->SetTitle();
  small_frame->GetYaxis()->SetTitle();
  small_frame->Draw();
  smallPad.Draw();

  c.cd(2);

  auto pull = mass_frame->pullHist();
  pull->SetTitle(";M_{K_{S}^{0}p};pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()),
                                    Range("full"));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();

  c.cd(1);
  mass_frame->Draw();
  leg.Draw();

  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);
  TLatex tex;
  tex.SetTextFont(42);
  tex.SetTextAlign(11);
  //tex.DrawLatexNDC(0.2, 0.25,
  //                 Form("scale=%.1f+/-%.1f", scale.getVal(),
  //                      scale.getErrorHi()));
  tex.DrawLatexNDC(0.2, 0.08,
                   Form("N_{sig}=%.1f+/-%.1f", nsig.getVal(),
                        nsig.getErrorHi()));
  auto errorType = fitOpts.useWeight ? RooAbsData::SumW2 : RooAbsData::Poisson;
  auto chi2Var = sum.createChi2(*dhptr, Extended(kTRUE), Range("full"), DataError(errorType));
  auto chi2Val = chi2Var->getVal();
  int ndf = 0;
  if (h) {
    ndf = fitOpts.nBins - result->floatParsFinal().getSize();
  }
  else {
    // works after 6.26
    // ndf = dhptr->getBinnings()->numBins() - result->floatParsFinal().getSize();
    ndf = ((RooUniformBinning &)mass.getBinning(0)).numBins() - result->floatParsFinal().getSize();
  }
  tex.DrawLatexNDC(0.2, 0.02, Form("#chi^{2}/N.D.F = %.1f/%d", chi2Val, ndf));

  const auto fig_path = strs.at("fig_path")+"_data.png";
  const auto fig_path_pdf = strs.at("fig_path")+"_data.pdf";
  c.Print(fig_path.c_str());
  c.Print(fig_path_pdf.c_str());

  /**
     Update the parameter set
   */
  par.setInit("a1", a1.getVal());
  par.setInit("a2", a2.getVal());
  par.setInit("a3", a3.getVal());
  par.setInit("scale", scale.getVal());
  par.setInit("nsig", nsig.getVal());
  par.setInit("nbkg", nbkg.getVal());

  std::cout << "\n";
  std::cout << "Final nsig is " << nsig.getVal()
            << " + " << nsig.getAsymErrorHi()
            << " - " << nsig.getAsymErrorLo()
            << std::endl;
  /**
   * Update the workspace
   */
  // I do not do this for D0 because
  // 1. D0 is fast
  // 2. D0 is too complex
  if (fitOpts.saveToWS) ws.import(sum);
  delete bkg;
  if (h && dhptr) {
    if (fitOpts.saveToWS)
      ws.import(*dhptr);
    delete dhptr;
  }
  delete a4;
  delete h;
  return *result;
}

RooFitResult fitLamCGausCB(RooRealVar& mass, RooAbsData& ds, RooWorkspace& ws,
             FitParConfigs::ParConfigs& par, const FitOptsHF& fitOpts,
             std::map<std::string, std::string> strs)
{
  /**
     Model setup
  */
  // common scale factor
  RooRealVar scale("scale", "width ratio for data/MC",
                   par.getInit("scale"),
                   par.getMin("scale"), par.getMax("scale"));

  if (fitOpts.fixScale) {
    scale.setVal(1.);
    scale.setConstant(kTRUE);
  }

  // construct double gaussian
  // parameters
  RooRealVar mean("mean", "mean (GeV)", par.getInit("mean"),
                  par.getMin("mean"), par.getMax("mean"));
  if (fitOpts.fixMean) {
    mean.setConstant(kTRUE);
  }
  // sigma1 in par was obtained from MC
  RooRealVar sigma1MC("sigma1MC", "sigma of 1st gaussian in MC",
                    par.getInit("sigma1"), par.getMin("sigma1"),
                    par.getMax("sigma1"));
  RooFormulaVar sigma1("sigma1", "sigma of 1st gaussian", "@0*@1",
                       RooArgList(sigma1MC, scale));
  RooRealVar sigma1N("sigma1N", "sigma ratio, 2nd/1st gaussian",
                     par.getInit("sigma1N"), par.getMin("sigma1N"),
                     par.getMax("sigma1N"));
  RooFormulaVar sigma2("sigma2", "@0*@1", RooArgSet(sigma1, sigma1N));

  RooRealVar alpha("alpha", "alpha of CB component",
                     par.getInit("alpha"),
                     par.getMin("alpha"), par.getMax("alpha"));
  RooRealVar nCB("nCB", "n of CB component",
                 par.getInit("nCB"),
                 par.getMin("nCB"), par.getMax("nCB"));
  alpha.setConstant(kTRUE);
  nCB.setConstant(kTRUE);

  RooRealVar frac1("frac1", "fraction of 1st gaussian",
                   par.getInit("frac1"), par.getMin("frac1"),
                   par.getMax("frac1"));

  RooGaussian gaus1("gaus1", "1st gaussian for signal",
                    mass, mean, sigma1);
  RooCBShape cbShape("cbShape","Crystal Ball function for signal",
                       mass, mean, sigma2, alpha, nCB);
  RooAddPdf gausCB("gausCB", "gaussian+CB of the signal",
                     RooArgList(gaus1, cbShape), frac1);

  // 3rd order Chebyshev polynomial for background
  RooRealVar a1("a1", "a1 for Chebyshev(1)", par.getInit("a1"),
                par.getMin("a1"), par.getMax("a1"));
  RooRealVar a2("a2", "a2 for Chebyshev(2)", par.getInit("a2"),
                par.getMin("a2"), par.getMax("a2"));
  RooRealVar a3("a3", "a3 for Chebyshev(3)", par.getInit("a3"),
                par.getMin("a3"), par.getMax("a3"));
  RooRealVar* a4 = nullptr;
  // RooChebychev bkg("bkg", "3rd Chebyshev poly.", mass,
  //                 RooArgList(a1, a2, a3));
  
  // you can also use RooWorkspace::factory
  RooChebychev* bkg;
  switch (fitOpts.order) {
    case 2: bkg = new RooChebychev("bkg", "2nd Chebyshev poly.",
                                   mass, RooArgList(a1, a2));
            break;
    case 3: bkg = new RooChebychev("bkg", "3rd Chebyshev poly.",
                                   mass, RooArgList(a1, a2, a3));
            break;
    case 4: {
            a4 = new RooRealVar("a4", "a4 for Chebyshev(4)", par.getInit("a4"),
                          par.getMin("a4"), par.getMax("a4"));
            bkg = new RooChebychev("bkg", "4th Chebyshev poly.",
                                   mass, RooArgList(a1, a2, a3, *a4));
            std::cout << "Done a4 " << a4->getVal() << "\n";
            break;
            }
    default:
            throw std::logic_error("Cannot construct background pdf");
  }
            std::cout << "bkg done \n";

  // yields for different components
  RooRealVar nsig("nsig", "yields for true component",
                  par.getInit("nsig"),
                  par.getMin("nsig"), par.getMax("nsig"));
  RooRealVar nbkg("nbkg", "yields for background component",
                  par.getInit("nbkg"),
                  par.getMin("nbkg"), par.getMax("nbkg"));

  // add components together
  RooAddPdf sum("sum", "sig+bkg",
                RooArgList(*bkg, gausCB),
                RooArgList(nbkg, nsig));

  /**
     Fit
   */
  // remember to fix the all sigma in MC. they are labeled by MC
  // true D0
  sigma1MC.setConstant(kTRUE);
  sigma1N.setConstant(kTRUE);
  frac1.setConstant(kTRUE);

  RooAbsData* dsAbsPtr = &ds;

  TH1* h = nullptr;
  RooDataHist* dhptr = nullptr;
  RooFitResult* result;
  std::pair<double, double> binEdges = mass.getRange("full");
  if (auto dsPtr = dynamic_cast<RooDataSet*>(dsAbsPtr) ) {
    h = dsPtr->createHistogram("binned", mass,
                               Binning(fitOpts.nBins, binEdges.first, binEdges.second));
    if(fitOpts.useHist) {
      auto dh = RooDataHist("dh", "", mass, h);
      result = sum.fitTo(dh, Range("full"), Save(), AsymptoticError(fitOpts.useWeight));
      // if (fitOpts.useWeight)
      //  result = sum.chi2FitTo(dh, Range(binEdges.first, binEdges.second), NumCPU(fitOpts.numCPU), Save(), Minos(kFALSE));
    }
  } else {
    result = sum.fitTo(ds, Range("full"), NumCPU(fitOpts.numCPU), Save(), AsymptoticError(fitOpts.useWeight));
    // if (fitOpts.useWeight) {
    //  result = sum.chi2FitTo(dynamic_cast<RooDataHist&>(ds), Range(binEdges.first, binEdges.second), NumCPU(fitOpts.numCPU), Save(), Minos(kFALSE));
    // }
  }
  if (!fitOpts.useHistOnly) {
    result = sum.fitTo(ds, Range("full"), NumCPU(fitOpts.numCPU), Save(), AsymptoticError(fitOpts.useWeight));
  }

  if (h) { 
    h->SetName(::Form("%s_binned", ds.GetName()));
    std::string name = ds.GetName();
    // name starts with ds
    auto pos = name.find("ds");
    name.replace(pos, 2, "dh");
    dhptr = new RooDataHist(name.c_str(), "", mass, h);
    std::cout << "Created RooDataHist " << name << "\n";
  } else { dhptr = dynamic_cast<RooDataHist*>(&ds); };

  std::cout << "Finished fit\n";

  /**
     Draw
   */
  // gStyle->SetCanvasPreferGL(1);
  //
  TGaxis::SetExponentOffset(-0.05, 0, "y");
  auto mass_frame = mass.frame(Range("full"));
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle("M_{K_{S}^{0}p} (GeV)");

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  // c.cd(1);
  // dataset plotOn does not support Range option
  // Name option is for the RooHist or RooCurve objects
  // I do not know how to handle Normalization(scaleType)
  RooPlot* dsplot;
  if (h) {
    dsplot = ds.plotOn(mass_frame, Name("curve_mydataset"),
                       Binning(fitOpts.nBins, binEdges.first, binEdges.second),
                       MarkerStyle(20));
  } else {
    dsplot = ds.plotOn(mass_frame, Name("curve_mydataset"), MarkerStyle(20));
  }
  sum.plotOn(mass_frame, Components(*bkg), LineColor(kRed),
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             LineStyle(kDashed), MarkerStyle(20),
             Range("full"), NormRange("full"),
             Name("curve_bkg")
             );
  sum.plotOn(mass_frame, Components(gausCB), DrawOption("F"),
             FillColor(TColor::GetColorTransparent(kOrange-3, 0.3)),
             LineWidth(0),
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             Range("full"), NormRange("full"),
             Name("curve_gausCB")
             );
  sum.plotOn(mass_frame,
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             Range("full"), NormRange("full"),
             Name("curve_sum")
             );
  // mass_frame->SetMaximum(mass_frame->GetMaximum() * 1.1);

  /// get `RooCurve`s

  // curve_data is RooHist, instead of RooCurve
  auto curve_data = mass_frame->getHist("curve_mydataset");
  // pdf plots are RooCurve
  auto curve_sum = mass_frame->getCurve("curve_sum");
  auto curve_bkg = mass_frame->getCurve("curve_bkg");
  auto curve_gausCB = mass_frame->getCurve("curve_gausCB");

  // set VLines
  setVLines(curve_gausCB);

  // build legend
  //TLegend leg(0.62, 0.5, 0.9, 0.91);
  TLegend leg(0.62, 0.45, 0.9, 0.82);
  leg.SetFillStyle(0);
  leg.SetBorderSize(0);
  // this line works
  // leg.AddEntry(dsplot->getHist("curve_mydataset"), "Data", "p");
  // this line works as well
  leg.AddEntry(curve_data, "Data", "p");
  // I do not know why the line below does not work.
  // leg.AddEntry("curve_mydata", "Data", "p");
  // this line does not work because ds is a RooPlot
  // leg.AddEntry(dsplot, "Data", "p");
  leg.AddEntry(curve_gausCB, "Signal", "f");
  leg.AddEntry(curve_bkg, "Background", "l");

  c.cd();
  auto smallPad =  TPad("smallPad", "", 0.15, 0.57, 0.55, 0.82, 0, -1, 1);
  smallPad.Draw();
  smallPad.cd();
  mass.setRange("plotable", 2.22, 2.34);
  auto small_frame = mass.frame(Name("small_frame"), Range("plotable"), Title("zoomin"));
  if (h) {
    ds.plotOn(small_frame, Name(""),
              Binning(fitOpts.nBins, binEdges.first, binEdges.second),
              MarkerStyle(20));
  } else {
    ds.plotOn(small_frame, Name(""), MarkerStyle(20));
  }
  sum.plotOn(small_frame, Name("zoomin_bkg"),
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             Components(*bkg), LineColor(kRed), LineStyle(kDashed),
             MarkerStyle(20), Range("plotable"), NormRange("full"));
  sum.plotOn(small_frame, Name("zoomin_sum"),
             //Normalization(1.0, RooAbsReal::RelativeExpected),
             MarkerStyle(20), Range("plotable"), NormRange("full"));
  auto curve_zoomin = small_frame->getCurve("zoomin_sum");
  double y_max = *std::max_element(curve_zoomin->GetY(),
                                   curve_zoomin->GetY() + curve_zoomin->GetN());
  double y_min = *std::min_element(curve_zoomin->GetY(),
                                   curve_zoomin->GetY() + curve_zoomin->GetN());
  small_frame->SetMaximum(y_max + 0.5 * (y_max - y_min));
  small_frame->SetMinimum(y_min - 0.5 * (y_max - y_min));
  small_frame->GetXaxis()->SetTitle();
  small_frame->GetYaxis()->SetTitle();
  small_frame->Draw();
  smallPad.Draw();

  c.cd(2);

  auto pull = mass_frame->pullHist();
  pull->SetTitle(";M_{K_{S}^{0}p};pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()),
                                    Range("full"));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();

  c.cd(1);
  mass_frame->Draw();
  leg.Draw();

  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);
  TLatex tex;
  tex.SetTextFont(42);
  tex.SetTextAlign(11);
  //tex.DrawLatexNDC(0.2, 0.25,
  //                 Form("scale=%.1f+/-%.1f", scale.getVal(),
  //                      scale.getErrorHi()));
  tex.DrawLatexNDC(0.2, 0.08,
                   Form("N_{sig}=%.1f+/-%.1f", nsig.getVal(),
                        nsig.getErrorHi()));
  auto errorType = fitOpts.useWeight ? RooAbsData::SumW2 : RooAbsData::Poisson;
  auto chi2Var = sum.createChi2(*dhptr, Extended(kTRUE), Range("full"), DataError(errorType));
  auto chi2Val = chi2Var->getVal();
  int ndf = 0;
  if (h) {
    ndf = fitOpts.nBins - result->floatParsFinal().getSize();
  }
  else {
    // works after 6.26
    // ndf = dhptr->getBinnings()->numBins() - result->floatParsFinal().getSize();
    ndf = ((RooUniformBinning &)mass.getBinning(0)).numBins() - result->floatParsFinal().getSize();
  }
  tex.DrawLatexNDC(0.2, 0.02, Form("#chi^{2}/N.D.F = %.1f/%d", chi2Val, ndf));

  const auto fig_path = strs.at("fig_path")+"_data.png";
  const auto fig_path_pdf = strs.at("fig_path")+"_data.pdf";
  c.Print(fig_path.c_str());
  c.Print(fig_path_pdf.c_str());

  /**
     Update the parameter set
   */
  par.setInit("a1", a1.getVal());
  par.setInit("a2", a2.getVal());
  par.setInit("a3", a3.getVal());
  par.setInit("scale", scale.getVal());
  par.setInit("nsig", nsig.getVal());
  par.setInit("nbkg", nbkg.getVal());

  std::cout << "\n";
  std::cout << "Final nsig is " << nsig.getVal()
            << " + " << nsig.getAsymErrorHi()
            << " - " << nsig.getAsymErrorLo()
            << std::endl;
  /**
   * Update the workspace
   */
  // I do not do this for D0 because
  // 1. D0 is fast
  // 2. D0 is too complex
  if (fitOpts.saveToWS) ws.import(sum);
  delete bkg;
  if (h && dhptr) {
    if (fitOpts.saveToWS)
      ws.import(*dhptr);
    delete dhptr;
  }
  delete a4;
  delete h;
  return *result;
}

RooFitResult fitSideband(RooRealVar& mass, RooAbsData& ds, RooWorkspace& ws,
             FitParConfigs::ParConfigs& par, const FitOptsHF& fitOpts,
             std::map<std::string, std::string> strs)
{
  /**
     Model setup
  */
  // common scale factor
  RooRealVar scale("scale", "width ratio for data/MC",
                   par.getInit("scale"),
                   par.getMin("scale"), par.getMax("scale"));

  if (fitOpts.fixScale) {
    scale.setVal(1.);
    scale.setConstant(kTRUE);
  }

  // nth order Chebyshev polynomial for background
  RooRealVar a1("a1", "a1 for Chebyshev(1)", par.getInit("a1"),
                par.getMin("a1"), par.getMax("a1"));
  RooRealVar a2("a2", "a2 for Chebyshev(2)", par.getInit("a2"),
                par.getMin("a2"), par.getMax("a2"));
  RooRealVar a3("a3", "a3 for Chebyshev(3)", par.getInit("a3"),
                par.getMin("a3"), par.getMax("a3"));
  RooRealVar* a4 = nullptr;
  // RooChebychev bkg("bkg", "3rd Chebyshev poly.", mass,
  //                 RooArgList(a1, a2, a3));
  
  // you can also use RooWorkspace::factory
  RooChebychev* bkg;
  switch (fitOpts.order) {
    case 2: bkg = new RooChebychev("bkg", "2nd Chebyshev poly.",
                                   mass, RooArgList(a1, a2));
            break;
    case 3: bkg = new RooChebychev("bkg", "3rd Chebyshev poly.",
                                   mass, RooArgList(a1, a2, a3));
            break;
    case 4: {
            a4 = new RooRealVar("a4", "a4 for Chebyshev(4)", par.getInit("a4"),
                          par.getMin("a4"), par.getMax("a4"));
            bkg = new RooChebychev("bkg", "4th Chebyshev poly.",
                                   mass, RooArgList(a1, a2, a3, *a4));
            std::cout << "Done a4 " << a4->getVal() << "\n";
            break;
            }
    default:
            throw std::logic_error("Cannot construct background pdf");
  }
            std::cout << "bkg done \n";

  /**
     Fit
   */
  // remember to fix the all sigma in MC. they are labeled by MC
  // true D0
  RooAbsData* dsAbsPtr = &ds;

  TH1* h = nullptr;
  RooDataHist* dhptr = nullptr;
  RooFitResult* result;
  std::pair<double, double> binEdges = mass.getRange("full");
  if (auto dsPtr = dynamic_cast<RooDataSet*>(dsAbsPtr) ) {
    h = dsPtr->createHistogram("binned", mass,
                               Binning(fitOpts.nBins, binEdges.first, binEdges.second));
    if(fitOpts.useHist) {
      auto dh = RooDataHist("dh", "", mass, h);
      if (fitOpts.prefitDataFraction < 0) {
        result = bkg->fitTo(dh, Range("left,right"), Save(), AsymptoticError(fitOpts.useWeight));
      } else {
        result = bkg->fitTo(dh, Range("left,right"), Save(), AsymptoticError(fitOpts.useWeight),
                            PrefitDataFraction(fitOpts.prefitDataFraction));
      }
    }
  } else {
    if (fitOpts.prefitDataFraction < 0) {
      result = bkg->fitTo(ds, Range("left,right"), NumCPU(fitOpts.numCPU), Save(), AsymptoticError(fitOpts.useWeight));
    } else {
      result = bkg->fitTo(ds, Range("left,right"), NumCPU(fitOpts.numCPU), Save(), AsymptoticError(fitOpts.useWeight),
          PrefitDataFraction(fitOpts.prefitDataFraction));
    }
  }
  if (!fitOpts.useHistOnly) {
    if (fitOpts.prefitDataFraction < 0) {
      result = bkg->fitTo(ds, Range("left,right"), NumCPU(fitOpts.numCPU), Save(), AsymptoticError(fitOpts.useWeight));
    } else {
      result = bkg->fitTo(ds, Range("left,right"), NumCPU(fitOpts.numCPU), Save(), AsymptoticError(fitOpts.useWeight),
          PrefitDataFraction(fitOpts.prefitDataFraction));
    }
  }

  if (h) { 
    h->SetName(::Form("%s_binned", ds.GetName()));
    std::string name = ds.GetName();
    // name starts with ds
    auto pos = name.find("ds");
    name.replace(pos, 2, "dh");
    dhptr = new RooDataHist(name.c_str(), "", mass, h);
    std::cout << "Created RooDataHist " << name << "\n";
  } else { dhptr = dynamic_cast<RooDataHist*>(&ds); };

  std::cout << "Finished fit\n";

  /**
     Draw
   */
  // gStyle->SetCanvasPreferGL(1);
  //
  TGaxis::SetExponentOffset(-0.05, 0, "y");
  auto mass_frame = mass.frame(Range("full"));
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle("M_{K_{S}^{0}p} (GeV)");

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  c.cd(1);
  // dataset plotOn does not support Range option
  // Name option is for the RooHist or RooCurve objects
  // I do not know how to handle Normalization(scaleType)
  RooPlot* dsplot;
  if (h) {
    dsplot = ds.plotOn(mass_frame, Name("curve_mydataset"),
                       Binning(fitOpts.nBins, binEdges.first, binEdges.second),
                       MarkerStyle(20));
  } else {
    dsplot = ds.plotOn(mass_frame, Name("curve_mydataset"), MarkerStyle(20));
  }
  bkg->plotOn(mass_frame, LineColor(kRed),
             // Normalization(1.0, RooAbsReal::RelativeExpected),
             LineStyle(kDashed), MarkerStyle(20),
             Range("full"), NormRange("left,right"),
             Name("curve_bkg")
             );
  bkg->plotOn(mass_frame, LineColor(kRed),
             // Normalization(1.0, RooAbsReal::RelativeExpected),
             LineStyle(kDashed), MarkerStyle(20),
             //Range("full"), NormRange("left,right"),
             Range("left"), NormRange("left,right"),
             Name("curve_bkg_left")
             );
  bkg->plotOn(mass_frame, LineColor(kRed),
             // Normalization(1.0, RooAbsReal::RelativeExpected),
             LineStyle(kDashed), MarkerStyle(20),
             //Range("full"), NormRange("left,right"),
             Range("right"), NormRange("left,right"),
             Name("curve_bkg_right")
             );

  /// get `RooCurve`s

  // curve_data is RooHist, instead of RooCurve
  auto curve_data = mass_frame->getHist("curve_mydataset");
  // pdf plots are RooCurve
  auto curve_bkg = mass_frame->getCurve("curve_bkg");
  auto curve_bkg_left = mass_frame->getCurve("curve_bkg_left");
  auto curve_bkg_right = mass_frame->getCurve("curve_bkg_right");

  // adjust range
  auto y_max_pos = std::max_element(curve_bkg->GetY() + 1,
                                   curve_bkg->GetY() + curve_bkg->GetN() - 1);
  auto y_min_pos = std::min_element(curve_bkg->GetY() + 1,
                                    curve_bkg->GetY() + curve_bkg->GetN() - 1);
  const double y_max = *y_max_pos;
  const double y_min = *y_min_pos;
  std::cout << "y max pos: " << y_max_pos - curve_bkg->GetY() - 1 << "\n";
  std::cout << "y min pos: " << y_min_pos - curve_bkg->GetY() - 1 << "\n";

  mass_frame->remove("curve_bkg");
  mass_frame->SetMinimum(y_min - 0.3 * (y_max - y_min));
  mass_frame->SetMaximum(y_max + 0.3 * (y_max - y_min));

  c.cd(1);
  mass_frame->Draw();

  c.cd(2);

  auto pull_left = mass_frame->pullHist("curve_mydataset", "curve_bkg_left");
  auto pull_right = mass_frame->pullHist("curve_mydataset", "curve_bkg_right");
  auto mass_frame_pull = mass.frame(Title(";M_{K_{S}^{0}p};pull"),
                                    Range("full"));
  mass_frame_pull->addPlotable(pull_left, "P");
  mass_frame_pull->addPlotable(pull_right, "P");
  mass_frame_pull->Draw();

  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);
  TLatex tex;
  tex.SetTextFont(42);
  tex.SetTextAlign(11);
  auto errorType = fitOpts.useWeight ? RooAbsData::SumW2 : RooAbsData::Poisson;
  auto chi2Var = bkg->createChi2(*dhptr, Extended(kFALSE), Range("left,right"), DataError(errorType));
  auto chi2Val = chi2Var->getVal();
  int ndf = 0;
  // fix later
  std::pair<double, double> leftBinEdges = mass.getRange("left");
  std::pair<double, double> rightBinEdges = mass.getRange("right");
  double frac = (leftBinEdges.second - leftBinEdges.first + rightBinEdges.second - rightBinEdges.first)/(binEdges.second - binEdges.first);
  if (h) {
    ndf = frac * fitOpts.nBins - result->floatParsFinal().getSize();
  }
  else {
    // works after 6.26
    // ndf = dhptr->getBinnings()->numBins() - result->floatParsFinal().getSize();
    ndf = frac * ((RooUniformBinning &)mass.getBinning(0)).numBins() - result->floatParsFinal().getSize();
  }
  tex.DrawLatexNDC(0.2, 0.02, Form("#chi^{2}/N.D.F = %.1f/%d", chi2Val, ndf));

  const auto fig_path = strs.at("fig_path")+"_data.png";
  const auto fig_path_pdf = strs.at("fig_path")+"_data.pdf";
  c.Print(fig_path.c_str());
  c.Print(fig_path_pdf.c_str());

  /**
     Update the parameter set
   */
  par.setInit("a1", a1.getVal());
  par.setInit("a2", a2.getVal());
  par.setInit("a3", a3.getVal());
  par.setInit("scale", scale.getVal());

  std::cout << "\n";
  /**
   * Update the workspace
   */
  // I do not do this for D0 because
  // 1. D0 is fast
  // 2. D0 is too complex
  ws.import(*bkg);
  delete bkg;
  if (h && dhptr) {
    delete dhptr;
  }
  // ws.import(*chi2Var, Name(::Form("%s_chi2Var", ds.GetName())));
  ws.import(*result, ::Form("%s_result", ds.GetName()));
  delete a4;
  delete h;
  return *result;
}

// MassFitter
RooFitResult MassFitter::fitDoubGaus(RooRealVar& mass, RooAbsData& ds,
                                     std::map<std::string, std::string> strs)
{
  /**
     Model setup
  */
  // construct double  gaussian
  // parameters
  RooRealVar mean("mean", "mean (GeV)", _par.getInit("mean"),
                  _par.getMin("mean"), _par.getMax("mean"));
  RooRealVar sigma1("sigma1", "sigma of 1st gaussian",
                    _par.getInit("sigma1"), _par.getMin("sigma1"),
                    _par.getMax("sigma1"));
  RooRealVar sigma1N("sigma1N", "sigma ratio, 2nd/1st gaussian",
                     _par.getInit("sigma1N"), _par.getMin("sigma1N"),
                     _par.getMax("sigma1N"));
  RooFormulaVar sigma2("sigma2", "@0*@1", RooArgSet(sigma1, sigma1N));
  RooRealVar frac1("frac1", "fraction of 1st gaussian",
                   _par.getInit("frac1"), _par.getMin("frac1"),
                   _par.getMax("frac1"));

  RooGaussian gaus1("gaus1", "1st gaussian for signal",
                    mass, mean, sigma1);
  RooGaussian gaus2("gaus2", "2nd gaussian for signal",
                    mass, mean, sigma2);
  RooAddPdf doubGaus("doubGaus", "double gaussian of the signal",
                     RooArgList(gaus1, gaus2), frac1);

  /**
     Fit.
  */

  auto result = doubGaus.fitTo(ds, Range("signal"), Save());

  /**
     Draw
  */
  auto mass_frame = mass.frame();
  mass_frame->SetTitle("");
  mass_frame->GetXaxis()->SetTitle(strs.at("xtitle").c_str());

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  c.cd(1);
  ds.plotOn(mass_frame);
  doubGaus.plotOn(mass_frame, NormRange("signal"));
  mass_frame->Draw();

  c.cd(2);
  auto pull = mass_frame->pullHist();
  pull->SetTitle("");
  pull->GetXaxis()->SetTitle(mass_frame->GetXaxis()->GetTitle());
  pull->GetYaxis()->SetTitle("pull");
  auto mass_frame_pull = mass.frame(Title(pull->GetTitle()));
  mass_frame_pull->addPlotable(pull, "P");
  mass_frame_pull->Draw();

  // set CMS labels
  auto pad = c.cd(1);
  setCMS(pad);
  setCollision(pad);
  setKinematics(pad, strs);

  const auto fig_path = strs.at("fig_path")+"_signal.png";
  c.Print(fig_path.c_str());

  /**
     Update the parameter set.
  */
  _par.setInit("mean", mean.getVal());
  _par.setInit("sigma1", sigma1.getVal());
  _par.setInit("sigma1N", sigma1N.getVal());
  _par.setInit("frac1", frac1.getVal());

  return *result;
}

#endif
