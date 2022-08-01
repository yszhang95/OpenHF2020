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

RooFitResult D0Fitter::fitSwap(RooRealVar& mass, RooAbsData& ds, map<string, string> strs)
{
  /**
     Model setup
   */
  RooRealVar meanSwap("meanSwap", "mean of swap",
                      MassFitter::_par.getInit("meanSwap"),
                      MassFitter::_par.getMin("meanSwap"),
                      MassFitter::_par.getMax("meanSwap"));
  RooRealVar sigmaSwap("sigmaSwap", "sigma of the swap component",
                       MassFitter::_par.getInit("sigmaSwap"),
                       MassFitter::_par.getMin("sigmaSwap"),
                       MassFitter::_par.getMax("sigmaSwap"));
  RooGaussian swapGaus("swapGaus", "swap gaussian",
                       mass, meanSwap, sigmaSwap);

  auto result = swapGaus.fitTo(ds, Range("signal"), Save());

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

  MassFitter::_par.setInit("meanSwap", meanSwap.getVal());
  MassFitter::_par.setInit("sigmaSwap", sigmaSwap.getVal());

  return *result;
}

RooFitResult D0Fitter::fitDoubGausAndSwap(RooRealVar& mass, RooAbsData& ds, map<string, string> strs)
{
  /**
     Model setup
  */
  // construct double gaussian
  // parameters
  RooRealVar mean("mean", "mean (GeV)", MassFitter::_par.getInit("mean"),
                  MassFitter::_par.getMin("mean"), MassFitter::_par.getMax("mean"));
  RooRealVar sigma1("sigma1", "sigma of 1st gaussian",
                    MassFitter::_par.getInit("sigma1"), MassFitter::_par.getMin("sigma1"),
                    MassFitter::_par.getMax("sigma1"));
  RooRealVar sigma1N("sigma1N", "sigma ratio, 2nd/1st gaussian",
                     MassFitter::_par.getInit("sigma1N"), MassFitter::_par.getMin("sigma1N"),
                     MassFitter::_par.getMax("sigma1N"));
  RooFormulaVar sigma2("sigma2", "@0*@1", RooArgSet(sigma1, sigma1N));
  RooRealVar frac1("frac1", "fraction of 1st gaussian",
                   MassFitter::_par.getInit("frac1"), MassFitter::_par.getMin("frac1"),
                   MassFitter::_par.getMax("frac1"));

  RooGaussian gaus1("gaus1", "1st gaussian for signal",
                    mass, mean, sigma1);
  RooGaussian gaus2("gaus2", "2nd gaussian for signal",
                    mass, mean, sigma2);
  RooAddPdf doubGaus("doubGaus", "double gaussian of the signal",
                     RooArgList(gaus1, gaus2), frac1);

  // construct double gaussian + swap
  RooRealVar meanSwap("meanSwap", "mean of swap",
                      MassFitter::_par.getInit("meanSwap"),
                      MassFitter::_par.getMin("meanSwap"),
                      MassFitter::_par.getMax("meanSwap"));
  RooRealVar sigmaSwap("sigmaSwap", "sigma of the swap component",
                       MassFitter::_par.getInit("sigmaSwap"),
                       MassFitter::_par.getMin("sigmaSwap"),
                       MassFitter::_par.getMax("sigmaSwap"));
  RooRealVar doubGausFrac("doubGausFrac",
                          "fraction of the double gaussian",
                          MassFitter::_par.getInit("doubGausFrac"),
                          MassFitter::_par.getMin("doubGausFrac"),
                          MassFitter::_par.getMax("doubGausFrac"));
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

  auto result = signal.fitTo(ds, Range("signal"), Save());

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
  MassFitter::_par.setInit("doubGausFrac", doubGausFrac.getVal());

  return *result;
}

RooFitResult D0Fitter::fitCBShape(RooRealVar& mass, RooAbsData& ds, map<string, string> strs)
{
  /**
     Model setup
  */
  // Crystal Ball function
  const std::string process_label = strs.at("process_label");
  const std::string process_name  = strs.at("process_name");

  const std::string m0_name = ::Form("m0%s", process_label.c_str());
  const std::string sigma_name = ::Form("sigma%s", process_label.c_str());
  const std::string alpha_name = ::Form("alpha%s", process_label.c_str());
  const std::string nCB_name = ::Form("nCB%s", process_label.c_str());

  RooRealVar m0(m0_name.c_str(), ::Form("Guassian mean of %s component", process_name.c_str()),
                MassFitter::_par.getInit(m0_name.c_str()),
                MassFitter::_par.getMin(m0_name.c_str()), MassFitter::_par.getMax(m0_name.c_str()));
  RooRealVar sigma(sigma_name.c_str(), ::Form("Guassian sigma of %s component", process_name.c_str()),
                   MassFitter::_par.getInit(sigma_name.c_str()),
                   MassFitter::_par.getMin(sigma_name.c_str()), MassFitter::_par.getMax(sigma_name.c_str()));
  RooRealVar alpha(alpha_name.c_str(), ::Form("alpha of %s component", process_name.c_str()),
                   MassFitter::_par.getInit(alpha_name.c_str()),
                   MassFitter::_par.getMin(alpha_name.c_str()), MassFitter::_par.getMax(alpha_name.c_str()));
  RooRealVar nCB(nCB_name.c_str(), ::Form("n of %s component", process_name.c_str()),
                 MassFitter::_par.getInit(nCB_name.c_str()),
                 MassFitter::_par.getMin(nCB_name.c_str()), MassFitter::_par.getMax(nCB_name.c_str()));

  RooCBShape cbShape("cbShape", ::Form("Crystal Ball function for %s", process_name.c_str()),
                     mass, m0, sigma, alpha, nCB);

  /**
     Fit
  */
  auto result = cbShape.fitTo(ds, Range(process_label.c_str()), Save());

  /**
     Draw
  */
  auto mass_frame = mass.frame(Range("full"));
  mass_frame->SetTitle("");
  // mass_frame->GetXaxis()->SetTitle(::Form("M_{%s} (GeV)", process_label.name()));
  mass_frame->GetXaxis()->SetTitle(strs.at("xtitle").c_str());

  TCanvas c("c", "", 600, 900);
  setCanvas(c);

  c.cd(1);
  ds.plotOn(mass_frame);
  cbShape.plotOn(mass_frame, NormRange(process_label.c_str()), Range("full"));
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

  const auto fig_path = strs.at("fig_path") + "_MC" + process_label + ".png";
  c.Print(fig_path.c_str());

  /**
     Update the parameter set
  */
  MassFitter::_par.setInit(m0_name, m0.getVal());
  MassFitter::_par.setInit(sigma_name, sigma.getVal());
  MassFitter::_par.setInit(alpha_name, alpha.getVal());
  MassFitter::_par.setInit(nCB_name, nCB.getVal());

  return *result;
}


RooFitResult  D0Fitter::fitData(RooRealVar& mass, RooAbsData& ds, RooWorkspace& ws,
                                const FitOptsHF& fitOpts,
                                std::map<std::string, std::string> strs)
{
  bool useRareProcesses = fitOpts.useRareProcesses ? useRareProcesses = MassFitter::_par.hasVariable("nKK") && MassFitter::_par.hasVariable("nPiPi") : false;
  if (!useRareProcesses && fitOpts.useRareProcesses)
    std::cout << "Conflict setup for useRareProcesses. KKPiPi parameters are not set.\n";

  std::cout << "useRareProcesses is " << std::boolalpha << useRareProcesses << "\n";

  /**
     Model setup
  */
  // common scale factor
  RooRealVar scale("scale", "width ratio for data/MC",
                   MassFitter::_par.getInit("scale"),
                   MassFitter::_par.getMin("scale"), MassFitter::_par.getMax("scale"));

  // construct double gaussian
  // parameters
  RooRealVar mean("mean", "mean (GeV)", MassFitter::_par.getInit("mean"),
                  MassFitter::_par.getMin("mean"), MassFitter::_par.getMax("mean"));
  // sigma1 in par was obtained from MC
  RooRealVar sigma1MC("sigma1MC", "sigma of 1st gaussian in MC",
                      MassFitter::_par.getInit("sigma1"), MassFitter::_par.getMin("sigma1"),
                      MassFitter::_par.getMax("sigma1"));
  RooFormulaVar sigma1("sigma1", "sigma of 1st gaussian", "@0*@1",
                       RooArgList(sigma1MC, scale));
  RooRealVar sigma1N("sigma1N", "sigma ratio, 2nd/1st gaussian",
                     MassFitter::_par.getInit("sigma1N"), MassFitter::_par.getMin("sigma1N"),
                     MassFitter::_par.getMax("sigma1N"));
  RooFormulaVar sigma2("sigma2", "@0*@1", RooArgSet(sigma1, sigma1N));
  RooRealVar frac1("frac1", "fraction of 1st gaussian",
                   MassFitter::_par.getInit("frac1"), MassFitter::_par.getMin("frac1"),
                   MassFitter::_par.getMax("frac1"));

  RooGaussian gaus1("gaus1", "1st gaussian for signal",
                    mass, mean, sigma1);
  RooGaussian gaus2("gaus2", "2nd gaussian for signal",
                    mass, mean, sigma2);
  RooAddPdf doubGaus("doubGaus", "double gaussian of the signal",
                     RooArgList(gaus1, gaus2), frac1);

  // construct double gaussian + swap
  RooRealVar meanSwap("meanSwap", "mean of swap",
                      MassFitter::_par.getInit("meanSwap"),
                      MassFitter::_par.getMin("meanSwap"),
                      MassFitter::_par.getMax("meanSwap"));
  // sigma for swap in par was obtained in MC
  RooRealVar sigmaSwapMC("sigmaSwapMC", "sigma of the swap component in MC",
                         MassFitter::_par.getInit("sigmaSwap"),
                         MassFitter::_par.getMin("sigmaSwap"),
                         MassFitter::_par.getMax("sigmaSwap"));
  RooFormulaVar sigmaSwap("sigmaSwap", "sigma of the swap component",
                          "@0*@1", RooArgList(sigmaSwapMC, scale));
  RooGaussian swapGaus("swapGaus", "swap gaussian",
                       mass, meanSwap, sigmaSwap);

  //  RooAddPdf signal("signal", "true and swap D",
  //               RooArgList(doubGaus, swapGaus), doubGausFrac);

  // Crystal Ball function for KK
  RooRealVar m0KK("m0KK", "Guassian mean of KK component",
                  MassFitter::_par.getInit("m0KK", 0),
                  MassFitter::_par.getMin("m0KK", 0), MassFitter::_par.getMax("m0KK", 0));
  // sigma for KK in par was obtained in MC
  RooRealVar sigmaKKMC("sigmaKKMC", "Guassian sigma of KK component in MC",
                     MassFitter::_par.getInit("sigmaKK", 1),
                     MassFitter::_par.getMin("sigmaKK", 1), MassFitter::_par.getMax("sigmaKK", 1));
  RooFormulaVar sigmaKK("sigmaKK", "sigma for D0->K+K-", "@0*@1",
                        RooArgList(sigmaKKMC, scale));
  RooRealVar alphaKK("alphaKK", "alpha of KK component",
                     MassFitter::_par.getInit("alphaKK", 1),
                     MassFitter::_par.getMin("alphaKK", 1), MassFitter::_par.getMax("alphaKK", 1));
  RooRealVar nCBKK("nCBKK", "n of KK component",
                 MassFitter::_par.getInit("nCBKK", 1),
                 MassFitter::_par.getMin("nCBKK", 1), MassFitter::_par.getMax("nCBKK", 1));

  RooCBShape cbShapeKK("cbShapeKK","Crystal Ball function for KK",
                       mass, m0KK, sigmaKK, alphaKK, nCBKK);

  // Crystal Ball function for PiPi
  RooRealVar m0PiPi("m0PiPi", "Guassian mean of PiPi component",
                    MassFitter::_par.getInit("m0PiPi", 0),
                    MassFitter::_par.getMin("m0PiPi", 0), MassFitter::_par.getMax("m0PiPi", 0));
  // sigma for PiPi in par was obtained in MC
  RooRealVar sigmaPiPiMC("sigmaPiPiMC",
                         "Guassian sigma of PiPi component in MC",
                         MassFitter::_par.getInit("sigmaPiPi", 1),
                         MassFitter::_par.getMin("sigmaPiPi", 1), MassFitter::_par.getMax("sigmaPiPi", 1));
  RooFormulaVar sigmaPiPi("sigmaPiPi", "sigma for D0->Pi+Pi-", "@0*@1",
                        RooArgList(sigmaPiPiMC, scale));
  RooRealVar alphaPiPi("alphaPiPi", "alpha of PiPi component",
                       MassFitter::_par.getInit("alphaPiPi", 1),
                       MassFitter::_par.getMin("alphaPiPi", 1), MassFitter::_par.getMax("alphaPiPi", 1));
  RooRealVar nCBPiPi("nCBPiPi", "n of PiPi component",
                   MassFitter::_par.getInit("nCBPiPi", 1),
                   MassFitter::_par.getMin("nCBPiPi", 1), MassFitter::_par.getMax("nCBPiPi", 1));

  RooCBShape cbShapePiPi("cbShapePiPi","Crystal Ball function for PiPi",
                         mass, m0PiPi, sigmaPiPi, alphaPiPi, nCBPiPi);

  // 3rd order Chebyshev polynomial for background
  RooRealVar a1("a1", "a1 for Chebyshev(1)", MassFitter::_par.getInit("a1"),
                MassFitter::_par.getMin("a1"), MassFitter::_par.getMax("a1"));
  RooRealVar a2("a2", "a2 for Chebyshev(2)", MassFitter::_par.getInit("a2"),
                MassFitter::_par.getMin("a2"), MassFitter::_par.getMax("a2"));
  RooRealVar a3("a3", "a3 for Chebyshev(3)", MassFitter::_par.getInit("a3"),
                MassFitter::_par.getMin("a3"), MassFitter::_par.getMax("a3"));
  RooRealVar a4("a4", "a4 for Chebyshev(4)", 0, -1, 1);
  if (fitOpts.order == 4) {
    try {
      a4.setMin(MassFitter::_par.getMin("a4"));
      a4.setMax(MassFitter::_par.getMax("a4"));
      a4.setVal(MassFitter::_par.getInit("a4"));
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
                          MassFitter::_par.getInit("doubGausFrac"),
                          MassFitter::_par.getMin("doubGausFrac"),
                          MassFitter::_par.getMax("doubGausFrac"));
  RooRealVar nsig("nsig", "yields for true component",
                  MassFitter::_par.getInit("nsig"),
                  MassFitter::_par.getMin("nsig"), MassFitter::_par.getMax("nsig"));
  RooFormulaVar nswap("nswap", "yields for swap component",
                      "@0/@1*(1-@1)", RooArgList(nsig, doubGausFrac));
  RooRealVar nKK("nKK", "yields for KK component",
                 MassFitter::_par.getInit("nKK", 0),
                 MassFitter::_par.getMin("nKK", 0), MassFitter::_par.getMax("nKK", 0));
  RooRealVar nPiPi("nPiPi", "yields for PiPi component",
                 MassFitter::_par.getInit("nPiPi", 0),
                 MassFitter::_par.getMin("nPiPi", 0), MassFitter::_par.getMax("nPiPi", 0));
  RooRealVar nbkg("nbkg", "yields for background component",
                  MassFitter::_par.getInit("nbkg"),
                  MassFitter::_par.getMin("nbkg"), MassFitter::_par.getMax("nbkg"));

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

  RooDataHist* dhptr = nullptr;
  RooFitResult* result = nullptr;
  TH1* h = ds.createHistogram(::Form("%s_binned", ds.GetName()), mass,
                              Binning(fitOpts.nBins));
  // hard coded
  if (fitOpts.adjustRanges) {
    std::pair<double, double> fullEdges = mass.getRange("full");
    std::pair<double, double> leftEdges = mass.getRange("left");
    std::pair<double, double> rightEdges = mass.getRange("right");
    std::pair<double, double> peakEdges = mass.getRange("peak");
    auto init_bkg = h->Integral(h->FindBin(leftEdges.first), h->FindBin(leftEdges.second))
                    + h->Integral(h->FindBin(rightEdges.first), h->FindBin(rightEdges.second));
    auto init_sig = h->Integral(h->FindBin(peakEdges.first), h->FindBin(peakEdges.second));
    init_bkg = init_bkg * (fullEdges.second - fullEdges.first)
               / (leftEdges.second - leftEdges.first + rightEdges.second - rightEdges.first);
    init_sig = init_sig - init_bkg * (peakEdges.second - peakEdges.first) / (fullEdges.second - fullEdges.first);

    nsig.setVal(init_sig);
    nsig.setMin(0.1 * init_sig);
    nsig.setMax(5.0 * init_sig);
    nbkg.setVal(init_bkg);
    nbkg.setMin(0.1 * init_bkg);
    nbkg.setMax(2.0 * init_bkg);

    std::cout << "Initial guess for auto-adjusting\n"
      << "\tnsig init: " << nsig.getVal() << ", min: " << nsig.getMin() << ", max: " << nsig.getMax() << "\n"
      << "\tnbkg init: " << nbkg.getVal() << ", min: " << nbkg.getMin() << ", max: " << nbkg.getMax() << "\n";
    
  }

  if (auto dsPtr = dynamic_cast<RooDataSet*>(&ds) ) {
    std::string name = ds.GetName();
    // name starts with ds
    auto pos = name.find("ds");
    name.replace(pos, 2, "dh");
    dhptr = new RooDataHist(name.c_str(), "", mass, h);
    std::cout << "Created RooDataHist " << name << "\n";
  } else {
    dhptr = dynamic_cast<RooDataHist*>(&ds); 
  }

  if (fitOpts.useHist) {
    auto dh = *dhptr;
    result = sum.fitTo(dh, Range("full"), Save(), AsymptoticError(fitOpts.useWeight));
  }

  if (!fitOpts.useHistOnly) {
    result = sum.fitTo(ds, Range("full"), Save(), NumCPU(fitOpts.numCPU),
                       AsymptoticError(fitOpts.useWeight));
  }

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
  MassFitter::_par.setInit("a1", a1.getVal());
  MassFitter::_par.setInit("a2", a2.getVal());
  MassFitter::_par.setInit("a3", a3.getVal());
  MassFitter::_par.setInit("scale", scale.getVal());
  MassFitter::_par.setInit("nsig", nsig.getVal());
  if (useRareProcesses) {
    MassFitter::_par.setInit("nKK", nKK.getVal());
    MassFitter::_par.setInit("nPiPi", nPiPi.getVal());
  }
  MassFitter::_par.setInit("nbkg", nbkg.getVal());
  MassFitter::_par.setInit("doubGausFrac", doubGausFrac.getVal());

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
  if (dhptr) {
    if (fitOpts.saveToWS) ws.import(*dhptr);
    if (dynamic_cast<RooDataSet*>(&ds)) delete dhptr;
  }
  if (h) delete h;
  return *result;
}

RooFitResult D0Fitter::fitD0(RooRealVar& mass, RooWorkspace& myws,
                             const FitOptsHF& fitOpts,
                             std::map<std::string, std::string> strs)
{
  std::cout << ">>>>>>>> Fit true D\n" << "\n";
  const auto strs_doubGaus = strs;
  auto dsMCSignal = myws.data(strs_doubGaus.at("dsMCSignal").c_str());
  fitDoubGaus(mass, *dsMCSignal, strs_doubGaus);
  std::cout << "\n";

  std::cout << ">>>>>>>> Fit swap\n" << "\n";
  const auto strs_swap = strs;
  auto dsMCSwap = myws.data(strs.at("dsMCSwap").c_str());
  fitSwap(mass, *dsMCSwap, strs_swap);
  std::cout << "\n";

  std::cout << ">>>>>>>> Fit true and swap D\n" << "\n";
  const auto strs_mc = strs;
  auto dsMCAll = myws.data(strs.at("dsMCAll").c_str());
  fitDoubGausAndSwap(mass, *dsMCAll, strs_mc);
  std::cout << "\n";

  if (fitOpts.useRareProcesses) {
    std::cout << ">>>>>>>> Fit D0->KK\n" << "\n";
    auto strs_KK = strs;
    strs_KK["process_label"] = "KK";
    strs_KK["process_name"] = "D^{0} #rightarrow K^{+}K^{-}";
    strs_KK["xtitle"] = "M_{K^{+}K^{-}}";
    auto dsMCKK = myws.data(strs.at("dsMCKK").c_str());
    fitCBShape(mass, *dsMCKK, strs_KK);
    std::cout << "\n";

    std::cout << ">>>>>>>> Fit D0->PiPi\n" << "\n";
    auto strs_PiPi = strs;
    strs_PiPi["process_label"] = "PiPi";
    strs_PiPi["process_name"] = "D^{0} #rightarrow #pi^{+}#pi^{-}";
    strs_PiPi["xtitle"] = "M_{#pi^{+}#pi^{-}}";
    auto dsMCPiPi = myws.data(strs.at("dsMCPiPi").c_str());
    fitCBShape(mass, *dsMCPiPi, strs_PiPi);
    std::cout << "\n";
  }

  std::cout << ">>>>>>>> Fit D0->KPi in data\n" << "\n";
  const auto strs_data = strs;
  auto dsData = myws.data(strs.at("dsData").c_str());
  auto result = fitData(mass, *dsData, myws, fitOpts, strs_data);
  std::cout << "\n";

  return result;
}

#endif
