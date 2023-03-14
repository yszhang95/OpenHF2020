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

int lowest = 1;
int highest = 5;

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

  std::cout << "Redirecting output to " << path_name_log << "...\n";

  gSystem->RedirectOutput(path_name_log.c_str(), "a", nullptr);

  strs["xtitle"] = "M_{K#pi}";

  auto cand_mass = dynamic_cast<RooRealVar*>(myws.var("cand_mass"));
  cand_mass->setRange(2.1, 2.47);
  cand_mass->setRange("full", 2.1, 2.47);
  cand_mass->setRange("signal", 2.165, 2.375);
  cand_mass->setRange("left", 2.1, 2.1+0.0037*40);
  cand_mass->setRange("right", 2.47-0.0037*40, 2.47);

  const std::string wsName = myws.GetName();

  const std::string kinStr = mycuts.getKinematics();
  const std::string mvaStr = mycuts.getMva();
  const auto MClabel = "_" + kinStr + "_" + mvaStr;

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
  // fitLamC(*cand_mass, *dsData, myws, par, opts, strs);
  fitSideband(*cand_mass, *dsData, myws, par, opts, strs);
  std::cout << "\n";

  // get the stream back
  gSystem->RedirectOutput(nullptr);
  std::cout << "Redirecting output to shell...\n";

  // std::string ofileName = gSystem->BaseName(c_str());
  std::string ofileName = strs.at("fileName");
  auto pos = ofileName.find(".root");
  ofileName.replace(pos, 5, ::Form("_order%d_postfit.root", opts.order));
  ofileName = gSystem->BaseName(ofileName.c_str());
  cout << ofileName << "\n";
  ofileName = strs.at("outdir") + ofileName;
  TFile ofile(ofileName.c_str(), "recreate");
  ofile.cd();
  myws.Write();
}

void fitEachBin(const std::string& configName)
{
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

  const std::string wsName = names.at("wsName");
  strs["dsName"] = names.at("dsName");
  strs["fileName"] = names.at("fileName");

  strs["outdir"] = "./postfit/";

  std::string fileName = strs.at("fileName");
  fileName = gSystem->BaseName(fileName.c_str());
  fileName = "workspaces/" + fileName;
  cout << fileName << "\n";

  FitOptsHF opts;
  // opts.numCPU = 4;
  opts.useWeight = mycuts._useWeight;
  // opts.useHist = false;
  opts.useHist = true;
  // opts.useHistOnly = true;
  opts.useHistOnly = true;
  opts.fixScale = false;

  for (int i=lowest; i<highest+1; ++i) {
    TFile infile(fileName.c_str());
    auto myws = (RooWorkspace*) infile.Get(wsName.c_str());
    opts.order = i;
    const char* weightstr = mycuts._useWeight ? "_weighted" : "";
    strs["path_name_log"] = Form("dz1p0_output/LamCFit_pT%.1fto%.1f_Ntrk%dto%d%s_order%d.log",
        pTMin, pTMax, NtrkMin, NtrkMax, weightstr, opts.order);
    strs["fig_path"] = Form("dz1p0_output/pT%.1fto%.1f_Ntrk%dto%d%s_order%d",
        pTMin, pTMax, NtrkMin, NtrkMax, weightstr, opts.order);
    fitEachBin(*myws, par, mycuts, opts, strs);
  }
}

void fit()
{
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;
  for (unsigned int ibin=0; ibin<nbins; ++ibin) {
    for (unsigned int ipt=0; ipt<nPt; ++ipt) {
      if (Ntrks[ibin+1] == 250 && Ntrks[ibin] == 185 && std::abs(pts[ipt]-2) <1E-5) continue;
      const char* useWeight = Ntrks[ibin+1] == 250 && Ntrks[ibin] == 185 ? "weighted" : "unweighted";
      auto configName = ::string_format("${OPENHF2020TOP}/Spectra/ProduceWorkspace/LamC/dz1p0_%s/pT%.0fto%.0f_Ntrk%dto%d.conf", useWeight, pts[ipt], pts[ipt+1], Ntrks[ibin], Ntrks[ibin+1]);
      configName = gSystem->ExpandPathName(configName.c_str());
      fitEachBin(configName);
    }
  }
}
