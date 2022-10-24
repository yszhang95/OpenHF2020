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

const double binLow = 2.13;
const double binHigh = 2.48;
const double nBins = 100;

void fitEachBin(RooWorkspace& myws, FitParConfigs::ParConfigs& par, const VarCuts& mycuts,
                const FitOptsHF& opts,
                std::map<std::string, std::string> strs)
{
  // redirect messages, do not forget to get the stream back
  std::string path_name_log = strs.at("path_name_log");
  std::string fig_path = strs.at("fig_path");
  strs["alignment"] = "22";
  strs["pT"] = ::Form("%.1f < p_{T} < %.1f", mycuts._pTMin, mycuts._pTMax);
  strs["y"] = ::Form("|y| < %.1f", mycuts._yAbsMax);
  // remember that the AccessPathName returns false it the path exits!
  if (!gSystem->AccessPathName(path_name_log.c_str())) {
    gSystem->Unlink(path_name_log.c_str());
  }
  const auto out_dir = gSystem->DirName(path_name_log.c_str());
  if (gSystem->AccessPathName(out_dir)) {
    gSystem->mkdir(out_dir, kTRUE);
  }

  std::cout << "Redirecting output to " << path_name_log << " ...\n";

  gSystem->RedirectOutput(path_name_log.c_str(), "a", nullptr);

  strs["xtitle"] = "M_{K#pi}";

  auto cand_mass = dynamic_cast<RooRealVar*>(myws.var("cand_mass"));
  cand_mass->setMin(binLow);
  cand_mass->setMax(binHigh);
  // cand_mass->setRange("full", 2.1, 2.47);
  cand_mass->setRange("full", binLow, binHigh);
  cand_mass->setRange("signal", 2.165, 2.375);

  std::cout << ">>>>>>>> Fit true LambdaC\n" << "\n";

  const std::string wsName = myws.GetName();

  const std::string kinStr = mycuts.getKinematics();
  const std::string mvaStr = mycuts.getMva();
  const auto MClabel = "_" + kinStr + "_" + mvaStr;

  const auto dsMCSignalName = "dsMCSignal" + MClabel;
  auto dsMCSignal = myws.data(dsMCSignalName.c_str());
  fitSignal(*cand_mass, *dsMCSignal, par, strs);

  std::cout << "\n";

  std::cout << ">>>>>>>> Fit LambdaC->KsProton in data\n" << "\n";
  // special for data
  const auto NtrkMin = mycuts._NtrkofflineMin;
  const auto NtrkMax = mycuts._NtrkofflineMax;
  if (NtrkMin > 0) {
    strs["mult"] = ::string_format("%d #leq N_{trk} < %d", NtrkMin, NtrkMax);
  } else {
    strs["mult"] = ::string_format("N_{trk} < %d", NtrkMax);
  }
  const std::string dsName = strs.at("dsName");
  auto dsData = myws.data(dsName.c_str());
  auto result = fitLamC(*cand_mass, *dsData, myws, par, opts, strs);
  std::cout << "\n";

  // get the stream back
  gSystem->RedirectOutput(nullptr);
  std::cout << "Redirecting output to shell...\n";
  std::cout << "The status is " << result.status() << std::endl;

  std::string ofileName = gSystem->BaseName(strs.at("fileName").c_str());
  auto pos = ofileName.find(".root");
  ofileName.replace(pos, 5, "_" + strs.at("order") + "_postfit.root");
  std::string outdir = strs.at("outdir");
  ofileName = outdir + ofileName;
  cout << ofileName << "\n" << "\n";
  if (gSystem->AccessPathName(outdir.c_str())) {
    gSystem->mkdir(outdir.c_str(), kTRUE);
  }
  TFile ofile(ofileName.c_str(), "recreate");
  ofile.cd();
  myws.Write();
}

void fitEachBin(const std::string& configName, const int order)
{
  // for loop
  std::map<std::string, std::string> strs;

  std::cout << configName << "\n";

  FitParConfigs configs(configName);
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  auto par = configs.getParConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  // modify it later
  VarCuts mycuts(cutConfigs);

  const auto NtrkMin = mycuts._NtrkofflineMin;
  const auto NtrkMax = mycuts._NtrkofflineMax;
  const float pTMin = mycuts._pTMin;
  const float pTMax = mycuts._pTMax;

  const std::string mvaName = mycuts._mvaName;
  const std::string mvaStr = mycuts.getMva();
  const std::string kinStr = mycuts.getKinematics();
  const std::string eventStr = mycuts.getEventFilter();
  const std::string ntrkStr = mycuts.getNtrkoffline();

  const auto names = ::getNames(configs, mycuts);

  const std::string label = kinStr + "_" + mvaStr + "_" + eventStr + "_" + ntrkStr;
  const std::string wsName = names.at("wsName");
  strs["dsName"] = names.at("dsName");
  strs.at("dsName").replace(1, 1, "h");
  // strs["fileName"] = names.at("fileName");
  strs["fileName"] = gSystem->BaseName(names.at("fileName").c_str());
  strs["fileName"] = "./ws_hists/" + strs.at("fileName");

  strs["outdir"] = "./postfit/";
  strs["order"] = "order" + std::to_string(order);

  const char* weightstr = mycuts._useWeight ? "_weighted" : "";
  strs["path_name_log"] = Form("dz1p0_output/LamCFit_pT%.1fto%.1f_Ntrk%dto%d_order%d%s.log",
      pTMin, pTMax, NtrkMin, NtrkMax, order, weightstr);
  strs["fig_path"] = Form("dz1p0_output/pT%.1fto%.1f_Ntrk%dto%d_order%d%s",
      pTMin, pTMax, NtrkMin, NtrkMax, order, weightstr);

  std::string fileName = strs.at("fileName");
  cout << fileName << "\n";
  TFile infile(fileName.c_str());
  auto myws = (RooWorkspace*) infile.Get(wsName.c_str());

  FitOptsHF opts;
  opts.numCPU = 4;
  opts.order = order;
  opts.useWeight = mycuts._useWeight;
  // opts.useHist = true;
  // opts.useHistOnly = true;
  opts.fixScale = true;
  opts.nBins = nBins;
  fitEachBin(*myws, par, mycuts, opts, strs);
}

void fit(const int order = 3)
{
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;
  // for (unsigned int ibin=0; ibin<nbins; ++ibin) {
  for (unsigned int ibin=5; ibin<6; ++ibin) {
  // for (unsigned int ibin=nbins-1; ibin<nbins; ++ibin) {
    // for (unsigned int ipt=0; ipt<nPt; ++ipt) {
    for (unsigned int ipt=4; ipt<5; ++ipt) {
    // for (unsigned int ipt=nPt-1; ipt<nPt; ++ipt) {
      if (Ntrks[ibin+1] == 250 && Ntrks[ibin] == 185 && ipt == 0) continue;
      std::string dir_order;
      switch(order) {
        case 2: dir_order = "_2nd"; break;
        case 4: dir_order = "_4th"; break;
      }
      const char* useWeight = Ntrks[ibin+1] == 250 && Ntrks[ibin] == 185 ? "weighted" : "unweighted";
      auto configName = ::string_format("dz1p0_%s%s/pT%.0fto%.0f_Ntrk%dto%d.conf",
                                        useWeight, dir_order.c_str(), pts[ipt], pts[ipt+1], Ntrks[ibin], Ntrks[ibin+1]);
      configName = gSystem->ExpandPathName(configName.c_str());
      fitEachBin(configName, order);
    }
  }
}
