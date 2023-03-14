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

#include "utils.C"

using namespace RooFit;

using std::cout;
using std::endl;

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

  const auto mvaStr = mycuts.getMva();
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
  cand_mass->setRange("full", 2.1, 2.46);
  cand_mass->setRange("signal", 2.165, 2.375);
  cand_mass->setRange("peak", 2.25, 2.32);
  cand_mass->setRange("left", 2.15, 2.22);
  cand_mass->setRange("right", 2.37, 2.45);

  std::cout << ">>>>>>>> Fit true LambdaC\n" << "\n";

  const std::string wsName = myws.GetName();

  const auto dsMCSignalName = strs.at("dsMCSignalName");
  std::cout << dsMCSignalName << "\n";
  auto dsMCSignal = myws.data(dsMCSignalName.c_str());
  fitSignal(*cand_mass, *dsMCSignal, par, strs);

  std::cout << "\n";

  std::cout << ">>>>>>>> Fit LambdaC->KsProton in data\n" << "\n";
  // special for data
  const auto NtrkMin = mycuts._NtrkofflineMin;
  const auto NtrkMax = mycuts._NtrkofflineMax;
  if (NtrkMin > 184) {
    strs["mult"] = "HM";
  } else {
    strs["mult"] = "MB";
  }
  const std::string dsName = strs.at("dsName");
  auto dsData = myws.data(dsName.c_str());
  auto result = fitLamC(*cand_mass, *dsData, myws, par, opts, strs);

  // get the stream back
  gSystem->RedirectOutput(nullptr);
  std::cout << "Redirecting output to shell...\n";
  std::cout << "status is " << result.status() << "\n";

  std::string ofileName = gSystem->BaseName(strs.at("fileName").c_str());
  auto pos = ofileName.find(".root");
  ofileName.replace(pos, 5, "_" + mvaStr + "_postfit.root");
  std::string outdir = strs.at("outdir");
  ofileName = outdir + ofileName;
  cout << ofileName << "\n\n";
  if (gSystem->AccessPathName(outdir.c_str())) {
    gSystem->mkdir(outdir.c_str(), kTRUE);
  }
  TFile ofile(ofileName.c_str(), "recreate");
  ofile.cd();
  myws.Write();
}

void fitEachBin(const std::string& configName, const double cut)
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
  mycuts._mvaCut = cut;

  const auto NtrkMin = mycuts._NtrkofflineMin;
  const auto NtrkMax = mycuts._NtrkofflineMax;
  const float pTMin = mycuts._pTMin;
  const float pTMax = mycuts._pTMax;

  const std::string mvaName = mycuts._mvaName;
  const std::string mvaStr = mycuts.getMva();
  const std::string kinStr = mycuts.getKinematics();
  const std::string eventStr = mycuts.getEventFilter();
  const std::string ntrkStr = mycuts.getNtrkoffline();

  const auto names = ::getOutNames(mycuts);

  const std::string wsName = names.at("wsName");
  // std::cout << wsName << "\n";
  strs["dsName"] = names.at("dsName") + "_" + mvaStr;
  strs["dsMCSignalName"] = names.at("dsMCSignalName") + "_" + mvaStr;
  strs.at("dsName").replace(0, 2, "dh_Mass");
  strs.at("dsMCSignalName").replace(0, 2, "dh_Mass");
  std::cout << strs.at("dsName") << "\n";

  strs["fileName"] = names.at("fileName");

  strs["outdir"] = "./postfit/";

  const std::string trig = mycuts._useMB ? "MB" : "HM";
  strs["path_name_log"] = Form("dz1p0_output/LamCFit_pT%.1fto%.1f_%s_%s.log",
      pTMin, pTMax, trig.c_str(), mvaStr.c_str());
  strs["fig_path"] = Form("dz1p0_output/pT%.1fto%.1f_%s_%s",
      pTMin, pTMax, trig.c_str(), mvaStr.c_str());

  std::string fileName = strs.at("fileName");
  auto pos = fileName.find("ofile");
  // fileName.replace(pos, 5, "ws");
  cout << fileName << "\n";
  TFile infile(fileName.c_str());
  auto myws = (RooWorkspace*) infile.Get(wsName.c_str());

  FitOptsHF opts;
  // opts.numCPU = 4;
  opts.useWeight = mycuts._useWeight;
  opts.useHist = true;
  opts.useHistOnly = true;
  // opts.fixScale = true;
  if (trig != "HM" && std::abs(pTMin-3)>1E-3) {
    opts.fixScale = false;
  } else {
    opts.fixScale = true;
  }
  opts.adjustRanges = true;
  // if (abs(mycuts._pTMin - 8)<1E-3 && mycuts._useWeight) opts.fixMean = true;
  fitEachBin(*myws, par, mycuts, opts, strs);
}

void fit_cuts()
{
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  for (const auto trig : {"MB", "HM"}) {
  // for (const auto trig : {"HM"}) {
  // for (const auto trig : {"MB"}) {
    for (int ipt=0; ipt<nPt; ++ipt) {
    // for (int ipt=nPt-1; ipt<nPt; ++ipt) {
    // for (int ipt=0; ipt<1; ++ipt) {
    // for (int ipt=1; ipt<2; ++ipt) {
      if (std::abs(pts[ipt]-2)<1E-3 && !strcmp(trig, "HM")) continue;
      std::string ptbin = ::string_format("pT%.0fto%.0f", pts[ipt], pts[ipt+1]);
      std::string infile_name = ::string_format("output_workspace/ofile_dz1p0_%s_%s_yAbs1.root", trig, ptbin.c_str());
      TFile infile(infile_name.c_str());
      TGraphAsymmErrors* gEff{nullptr};
      if (infile.IsOpen()) {
        std::cout << "Opened " << infile_name << "\n";
        infile.GetObject("efficiency", gEff);
      }

      std::vector<double> cuts{gEff->GetX(), gEff->GetX()+gEff->GetN()};

      VarSetup setup(trig, pts[ipt], pts[ipt+1]);
      for (const auto cut : cuts) {
        // std::cout << cut << "\n";
        fitEachBin(::string_format("configs/pT%.0fto%.0f_%s.conf", pts[ipt], pts[ipt+1], trig), cut);
      }
    }
  }
}
