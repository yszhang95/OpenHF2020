#if defined(__CLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/FitUtils/fitUtils.h"

#include "RooRealVar.h"
#include "RooDataSet.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"

using namespace RooFit;

using std::cout;
using std::endl;

void fitEachBin(RooWorkspace& myws, FitParConfigs::ParConfigs& par, const VarCuts& mycuts,
                FitOptsHF opts,
                std::map<std::string, std::string> strs)
{
  // redirect messages, do not forget to get the stream back
  std::string path_name_log = strs.at("path_name_log");
  std::cout << path_name_log << "\n";
  std::string fig_path = strs.at("fig_path");
  strs["alignment"] = "22";
  strs["pT"] = ::Form("%.1f < p_{T} < %.1f", mycuts._pTMin, mycuts._pTMax);
  strs["y"] = ::Form("|y| < %.1f", mycuts._yAbsMax);
  if (!gSystem->AccessPathName(path_name_log.c_str())) {
    gSystem->Unlink(path_name_log.c_str());
  }
  // remember that the AccessPathName returns false it the path exits!
  const auto out_dir = gSystem->DirName(path_name_log.c_str());
  if (gSystem->AccessPathName(out_dir)) {
    gSystem->mkdir(out_dir, kTRUE);
  }
  if (gSystem->AccessPathName(strs.at("outdir").c_str())) {
    gSystem->mkdir(strs.at("outdir").c_str(), kTRUE);
  }
  gSystem->RedirectOutput(path_name_log.c_str(), "a", nullptr);

  strs["xtitle"] = "M_{K#pi}";

  auto mass = dynamic_cast<RooRealVar*>(myws.var("mass"));

  std::cout << ">>>>>>>> Fit true D\n" << "\n";

  const std::string wsName = myws.GetName();

  const std::string kinStr = mycuts.getKinematics();
  const std::string mvaStr = mycuts.getMva();
  const auto MClabel = "_" + kinStr + "_" + mvaStr;

  const auto dsMCSignalName = "dsMCSignal" + MClabel;
  auto dsMCSignal = myws.data(dsMCSignalName.c_str());
  fitSignal(*mass, *dsMCSignal, par, strs);

  std::cout << "\n";

  std::cout << ">>>>>>>> Fit swap\n" << "\n";

  const auto dsMCSwapName = "dsMCSwap" + MClabel;
  auto dsMCSwap = myws.data(dsMCSwapName.c_str());
  fitSwap(*mass, *dsMCSwap, par, strs);

  std::cout << "\n";

  std::cout << ">>>>>>>> Fit true and swap D\n" << "\n";
  const auto dsMCAllName = "dsMCAll" + MClabel;
  auto dsMCAll = myws.data(dsMCAllName.c_str());
  fitSignalAndSwap(*mass, *dsMCAll, par, strs);
  std::cout << "\n";

  std::cout << ">>>>>>>> Fit D0->KK\n" << "\n";
  const auto dsMCKKName = "dsMCKK" + MClabel;
  auto dsMCKK = myws.data(dsMCKKName.c_str());
  fitCBShapeKK(*mass, *dsMCKK, par, strs);
  std::cout << "\n";

  std::cout << ">>>>>>>> Fit D0->PiPi\n" << "\n";
  const auto dsMCPiPiName = "dsMCPiPi" + MClabel;
  auto dsMCPiPi = myws.data(dsMCPiPiName.c_str());
  fitCBShapePiPi(*mass, *dsMCPiPi, par, strs);
  std::cout << "\n";

  std::cout << ">>>>>>>> Fit D0->KPi in data\n" << "\n";
  // special for data
  const auto NtrkMin = mycuts._NtrkofflineMin;
  const auto NtrkMax = mycuts._NtrkofflineMax;
  if (NtrkMin > 0) {
    strs["mult"] = ::string_format("%d #leq N_{trk} < %d", NtrkMin, NtrkMax);
  } else {
    strs["mult"] = ::string_format("N_{trk} < %d", NtrkMax);
  }
  const std::string dsName = "ds" + std::string(wsName.begin()+2, wsName.end());
  auto dsData = myws.data(dsName.c_str());
  int status = -1;
  if (opts.order == 4) {
    opts.order = 3;
    opts.saveToWS = 0;
    fitD0(*mass, *dsData, myws, par, opts, strs);
    opts.order = 4;
    opts.saveToWS = 1;
    opts.fixNames = {"nKK", "nPiPi"};
    for (const auto& varName: opts.fixNames) {std::cout << varName << "\n";}
    auto result = fitD0(*mass, *dsData, myws, par, opts, strs);
    status  = result.status();
  } else {
    auto result = fitD0(*mass, *dsData, myws, par, opts, strs);
    status  = result.status();
  }
  std::cout << "\n";

  // get the stream back
  gSystem->RedirectOutput(nullptr);

  if (status) {std::cout << "status " << status << std::endl; throw std::logic_error(::string_format("bad fit to Ntrk: %d-%d, pT: %.1f-%.1f", NtrkMin, NtrkMax, mycuts._pTMin, mycuts._pTMax));}
  if (myws.var("nKK")->getVal()<0) {std::cout << "nKK " << myws.var("nKK")->getVal() << std::endl; throw std::logic_error(::string_format("bad fit to Ntrk: %d-%d, pT: %.1f-%.1f", NtrkMin, NtrkMax, mycuts._pTMin, mycuts._pTMax));}
  if (myws.var("nPiPi")->getVal()<0) {std::cout << "nPiPi " << myws.var("nPiPi")->getVal() << std::endl; throw std::logic_error(::string_format("bad fit to Ntrk: %d-%d, pT: %.1f-%.1f", NtrkMin, NtrkMax, mycuts._pTMin, mycuts._pTMax));}

  auto ofileName = strs.at("fileName");
  auto pos = ofileName.find(".root");
  const std::string suffix = ::string_format("_order%d_postfit.root", opts.order);
  ofileName.replace(pos, 5, suffix);
  ofileName = strs.at("outdir") + ofileName;
  std::cout << ofileName << "\n";
  TFile ofile(ofileName.c_str(), "recreate");
  ofile.cd();
  myws.Write();
}

void fitEachMult(int NtrkMin, int NtrkMax, int order)
{
  // for loop
  std::map<std::string, std::string> strs;
  const int nPt = 8;
  const float pts[nPt+1] =   {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  const float treeMin[nPt] = {2.0, 3.0, 4.0, 5.0, 6.0, 6.0, 8.0, 8.0};
  const float treeMax[nPt] = {3.0, 4.0, 5.0, 6.0, 8.0, 8.0, 10., 10.};

  for (int ipt=0; ipt<nPt; ++ipt) {
    std::string configName = ::string_format("configs_order%d/D0wKKPiPi_pT%.0fto%.0f_Ntrk%dto%d_std.txt", order, pts[ipt], pts[ipt+1], NtrkMin, NtrkMax);
    std::cout << configName << "\n";

    FitParConfigs configs(configName);
    auto inputConfigs = configs.getInputConfigs();
    auto outputConfigs = configs.getOutputConfigs();
    auto par = configs.getParConfigs();
    const auto cutConfigs = configs.getCutConfigs();
    // modify it later
    VarCuts mycuts(cutConfigs);

    mycuts._pTMin = pts[ipt];
    mycuts._pTMax = pts[ipt+1];
    mycuts._NtrkofflineMin = NtrkMin;
    mycuts._NtrkofflineMax = NtrkMax;

    auto names = ::getNames(configs, mycuts);
    strs["outdir"] = "output_order" + std::to_string(order);
    if (*strs.at("outdir").rend() != '/') strs.at("outdir") += "/";
    strs["path_name_log"] = Form("output/D0Fit_pT%.1fto%.1f_Ntrk%dto%d_order%d.log",
                                     pts[ipt], pts[ipt+1], NtrkMin, NtrkMax, order);
    strs["fig_path"] = Form("output/pT%.1fto%.1f_Ntrk%dto%d_order%d",
                                pts[ipt], pts[ipt+1], NtrkMin, NtrkMax, order);
    const auto fileName = names.at("fileName");
    const auto wsName = names.at("wsName");
    strs["fileName"] = gSystem->BaseName(fileName.c_str());
    TFile infile(fileName.c_str());
    std::cout << fileName << "\n";
    auto ws = (RooWorkspace*) infile.Get(wsName.c_str());
    FitOptsHF opts;
    opts.numCPU = 4;
    opts.useWeight = mycuts._useWeight;
    opts.useHist = true;
    opts.useHistOnly = true;
    opts.fixScale = false;
    opts.order = order;
    fitEachBin(*ws, par, mycuts, opts, strs);
  }
}

void fit(int order=3)
{
  gErrorAbortLevel = kError;
  //fitEachMult(185, 250, "D0wKKPiPi_HM_std.txt", order);
  fitEachMult(185, 250, order);
  const int Ntrks[] = {0, 35, 60, 90, 120, 185};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;
  for (unsigned int ibin=0; ibin<nbins; ++ibin) {
    //fitEachMult(Ntrks[ibin], Ntrks[ibin+1], "D0wKKPiPi_MB_std.txt", order);
    fitEachMult(Ntrks[ibin], Ntrks[ibin+1], order);
  }
}
