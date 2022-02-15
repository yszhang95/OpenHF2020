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

using namespace RooFit;

using std::cout;
using std::endl;

void fitTest()
{
  gROOT->SetBatch(kTRUE);

  FitParConfigs configs("test.txt");
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  auto par = configs.getParConfigs();

  std::string fileMC = inputConfigs.getPaths("MC").at(0);
  std::string treeMC = inputConfigs.getName("MC");
  // I have to set the range of RooRealVar to the same as the plot range,
  // otherwise I cannot have proper plot using
  // Normalization(1.0, RooAbsReal::RealtiveExpected).
  // And it seems the extracted yields is also improper.
  // RooRealVar cand_mass("cand_mass", "cand_mass", 2.09, 2.55);
  RooRealVar cand_mass("cand_mass", "cand_mass", 2.1, 2.47);
  RooRealVar cand_pT("cand_pT", "cand_pT", 0, 100);
  RooRealVar cand_y("cand_y", "rapidity", -2.4, 2.4);
  RooRealVar cand_matchGEN("matchGEN", "matchGEN", -1, 2);
  RooRealVar dz1p0("filterBit_4", "dz1p0", -1, 2);
  RooRealVar gplus("filterBit_5", "gplus", -1, 2);
  RooRealVar mva("MLP8to10MBNp2N_noDR", "MVA", 0, 1);

  cand_mass.setRange("full", 2.1, 2.47);
  cand_mass.setRange("signal", 2.16, 2.32);

  TChain chain(inputConfigs.getName("Data").c_str(), "LambdaCKsP");
  const auto files = inputConfigs.getPaths("Data");
  for (const auto & file : files) {
    const auto it = file.find(".list");
    if (it != std::string::npos) {
      TFileCollection tf("tf", "", file.c_str());
      chain.AddFileInfoList(tf.GetList());
    } else {
      chain.Add(file.c_str());
    }
  }

  RooDataSet dsLamCMC("dsLamCMC", "",
                      RooArgList(cand_mass, cand_pT, cand_y, mva),
                    ImportFromFile(fileMC.c_str(), treeMC.c_str()),
                    Cut("abs(cand_y)<0.5 && MLP8to10MBNp2N_noDR > 0.016"));

  RooDataSet dsLamC("dsLamC", "", &chain, RooArgSet(cand_mass, cand_pT, cand_y,
                                                    dz1p0, gplus, mva));
  auto dsLamCSkim = dsLamC.reduce("abs(cand_y)<0.5 &&"
                                   "filterBit_4 > 0.5 && MLP8to10MBNp2N_noDR > 0.016");
  dsLamCSkim->convertToVectorStore();

  std::map<std::string, std::string> strs;
  strs["fig_path"] = "test";
  strs["xtitle"] = "M_{K_{S}^{0}p}";
  fitSignal(cand_mass, dsLamCMC, par, strs);
  fitLamC(cand_mass, *dsLamCSkim, par, strs);
  delete dsLamCSkim;
}


void fit(std::string configName)
{
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
  cand_mass.setRange("signal", 2.16, 2.32);

  std::string mccuts = Form("cand_pT>%f && cand_pT<%f "
                            "&& abs(cand_y)<%f && %s > %f",
                            pTMin, pTMax, yAbsMax, mvaName.c_str(), mvaCut);
  RooDataSet dsLamCMC("dsLamCMC", "",
                      RooArgList(cand_mass, cand_pT, cand_y, mva),
                      ImportFromFile(fileMC.c_str(), treeMC.c_str()),
                      Cut(mccuts.c_str()));

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

  // remember that the AccessPathName returns false it the path exits!
  const auto out_dir = gSystem->DirName(strs.at("fig_path").c_str());
  if (gSystem->AccessPathName(out_dir)) {
    gSystem->mkdir(out_dir, kTRUE);
  }

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
  fitLamC(cand_mass, *dsLamCSkim, par, strs);
  // std::cout << "MC entries " << dsLamCMC.sumEntries() << std::endl;

  f->Close();
}

void fitAlice()
{
  fit("testws_pT8to10.txt");
}
