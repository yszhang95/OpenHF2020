// see https://root-forum.cern.ch/t/problem-loading-customized-style-while-launching-root/18664/11

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
#include "TFile.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TClassTable.h"
#include "TStopwatch.h"

using namespace RooFit;

using std::cout;
using std::endl;

void fit(std::string configName)
{
  TStopwatch ts;
  ts.Start();
  gROOT->SetBatch(kTRUE);

  FitParConfigs configs(configName.c_str());
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  auto par = configs.getParConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  const float pTMin = mycuts._pTMin;
  const float pTMax = mycuts._pTMax;
  const float yAbsMax = mycuts._yAbsMax;
  const float mvaCut = mycuts._mvaCut;

  const std::string mvaName = mycuts._mvaName;
  const std::string mvaStr = mycuts.getMva();
  const std::string kinStr = mycuts.getKinematics();
  const std::string eventStr = mycuts.getEventFilter();
  const std::string ntrkStr = mycuts.getNtrkoffline();

  const std::string label = kinStr + "_" + mvaStr + "_" + eventStr + "_" + ntrkStr;
  const std::string wsName = "ws_" + label;
  const std::string dsName = "ds_" + label;
  const std::string fileName = "ofile_" + wsName + ".root";

  std::string fileMC = inputConfigs.getPaths("MC").at(0);
  std::string treeMC = inputConfigs.getName("MC");
  RooRealVar cand_mass("cand_mass", "cand_mass", 2.1, 2.47);
  RooRealVar cand_pT("cand_pT", "cand_pT", 0, 100);
  RooRealVar cand_y("cand_y", "rapidity", -2.4, 2.4);
  RooRealVar cand_matchGEN("matchGEN", "matchGEN", -1, 2);
  RooRealVar mva(mvaName.c_str(), "MVA", 0, 1);

  cand_mass.setRange("full", 2.1, 2.47);
  cand_mass.setRange("signal", 2.165, 2.375);

  std::string mccuts = Form("cand_pT>%f && cand_pT<%f "
                            "&& abs(cand_y)<%f && %s > %f",
                            pTMin, pTMax, yAbsMax, mvaName.c_str(), mvaCut);
  RooDataSet dsLamCMC("dsLamCMC", "",
                      RooArgList(cand_mass, cand_pT, cand_y, mva),
                      ImportFromFile(fileMC.c_str(), treeMC.c_str()),
                      Cut(mccuts.c_str()));

  auto ofileName = fileName;
  ofileName.replace(fileName.size()-5, 5, "_postfit.root");
  TFile ofile(ofileName.c_str(), "recreate");

  auto f = TFile::Open(fileName.c_str());
  RooWorkspace* myws;
  f->GetObject(wsName.c_str(), myws);
  auto dsLamCSkim =
    dynamic_cast<RooDataSet*>( myws->data(dsName.c_str()));
  //dsLamCSkim->convertToVectorStore();

  std::map<std::string, std::string> strs;
  strs["fig_path"] = outputConfigs.getPath("fig_path");
  strs["xtitle"] = "M_{K_{S}^{0}p}";
  strs["pT"] = ::string_format("%.1f<p_{T}<%.1f", mycuts._pTMin, mycuts._pTMax);
  strs["y"] = ::string_format("|y|<%g", mycuts._yAbsMax);
  strs["log_path"] = outputConfigs.getPath("log_path");

  // remember that the AccessPathName returns false it the path exits!
  const auto fig_out_dir = gSystem->DirName(strs.at("fig_path").c_str());
  if (gSystem->AccessPathName(fig_out_dir)) {
    gSystem->mkdir(fig_out_dir, kTRUE);
  }
  const auto log_out_dir = gSystem->DirName(strs.at("log_path").c_str());
  if (gSystem->AccessPathName(log_out_dir)) {
    gSystem->mkdir(log_out_dir, kTRUE);
  }

  // redirect, do not forget to get it back
  const auto path_name_log = strs.at("log_path");
  if (!gSystem->AccessPathName(path_name_log.c_str())) {
    gSystem->Unlink(path_name_log.c_str());
  }
  gSystem->RedirectOutput(path_name_log.c_str(), "a", nullptr);

  // fit to MC
  fitSignal(cand_mass, dsLamCMC, par, strs);

  // fit to data
  if (ntrkStr == "NtrkAll") {
    strs["mult"] = "MB";
  } else {
    if (mycuts._NtrkofflineMax == UShort_t(-1)) {
      strs["mult"] = string_format("N_{trk}^{offline} #geq %zu",
                                   mycuts._NtrkofflineMin);
    } else if (mycuts._NtrkofflineMin == 0) {
      strs["mult"] = string_format("N_{trk}^{offline} < %zu",
                                   mycuts._NtrkofflineMax);
    } else {
      strs["mult"] = string_format("%zu #leq N_{trk}^{offline} < %zu",
                                   mycuts._NtrkofflineMin,
                                   mycuts._NtrkofflineMax);
    }
  }
  fitLamC(cand_mass, *dsLamCSkim, *myws, par, strs, false);
  // fitLamC(cand_mass, *dsLamCSkim, *myws, par, strs, true);
  std::cout << "MC entries " << dsLamCMC.sumEntries() << std::endl;

  // get the stream back
  ts.Print();
  gSystem->RedirectOutput(nullptr);

  ofile.cd();
  myws->Write();

  f->Close();
  ts.Stop();
  ts.Print();
}

void fitAlice()
{
  const float pT[] = {2, 3};
  const size_t nPt = sizeof(pT)/sizeof(float) - 1;
  for (size_t ipt=0; ipt<nPt; ++ipt) {
    auto configName = string_format("configForAlice/pT%gto%g.conf",
                                    pT[ipt], pT[ipt+1]);
    fit(configName);
  }
}

void fitCMS()
{
  // const float pT[] = {2, 3, 4, 5, 6, 8, 10};
  //const float pT[] = {8, 10};
  const float pT[] = {3, 4};
  const size_t nPt = sizeof(pT)/sizeof(float) - 1;
  //const int Ntrk[] = {0, 35, 60, 90, 120, 185};
  const int Ntrk[] = {185, 250};
  //const int Ntrk[] = {90, 120};
  const size_t nMult = sizeof(Ntrk)/sizeof(int) - 1;
  for (size_t ipt=0; ipt<nPt; ++ipt) {
    for (size_t imult=0; imult<nMult; ++imult) {
      auto configName = string_format("configs/pT%gto%g_Ntrk%dto%d.conf",
                                    pT[ipt], pT[ipt+1], Ntrk[imult], Ntrk[imult+1]);
      fit(configName);
    }
  }
}

void fitDP()
{
  const float pT[] = {3, 4};
  const size_t nPt = sizeof(pT)/sizeof(float) - 1;
  //const int NtrkMin[] = {0, 185};
  //const int NtrkMax[] = {35, 250};
  const int NtrkMin[] = {185};
  const int NtrkMax[] = {250};
  const size_t nMult = sizeof(NtrkMin)/sizeof(int);
  for (size_t ipt=0; ipt<nPt; ++ipt) {
    for (size_t imult=0; imult<nMult; ++imult) {
      auto configName = string_format("configs/pT%gto%g_Ntrk%dto%d.conf",
                                    pT[ipt], pT[ipt+1], NtrkMin[imult], NtrkMax[imult]);
      fit(configName);
    }
  }
}
