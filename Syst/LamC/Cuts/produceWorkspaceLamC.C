#if defined(__CLING__) and !defined(__ROOTCLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif
#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/Ana/Common.h"

#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

#include "TChain.h"
#include "TFile.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TTree.h"
#include "TSystem.h"

#include <memory>
#include <string>
#include <stdexcept>

#include "utils.C"

  using namespace ROOT;

  const int nbins = 90;
  const double mass_min = 2.1;
  const double mass_max = 2.46;


/**
   I need to think how to generate workspace
   1. I need one work space
   2. I need several RooDataSet hosted by one workspace
   3. I need to label each RooDataSet. They are labeled by several
      cuts.
      - pT
      - y
      - MVA
      - event filter
      - trigger
      - multiplicity
   4. I need to save serveral variables
      - mass
      - pT
      - y
      - MVA
   5. I need to think about how to deal with these variables. I temporarily
      do not save them because I label them in the name of dataset
      - Ntrkoffline
      - trigger bits
      - fitler bits
   6. I may save the following variables in the future
      - eta
      - phi
      - daughter related variables
      - other training variables
 */

using std::string;

void produceHistsLamC(std::string configName)
{
  // ROOT::EnableImplicitMT();
  FitParConfigs configs(configName.c_str());
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  bool applyNewWeight = false;
  if (mycuts._useWeight && abs(mycuts._pTMin - 3)<1E-3) applyNewWeight = true;
  if (!applyNewWeight) ROOT::EnableImplicitMT();

  const bool useWeight = mycuts._useWeight;

  const auto output = ::getOutNames(mycuts);
  const std::string ofileName = output.at("fileName");
  const std::string dsName = output.at("dsName");
  const std::string wsName = output.at("wsName");
  const std::string dsMCSignalName = output.at("dsMCSignalName");
  const std::string mvaName = mycuts._mvaName;

  TGraph* geff(0);
  EfficiencyTable<TGraph> effTab(geff);
  try {
    const auto effFileName = inputConfigs.getPaths("MultEff").front();
    TFile fEff(effFileName.c_str());
    const auto effTabName = inputConfigs.getName("MultEff");
    TGraph* geff = (TGraph*) fEff.Get(effTabName.c_str());
    effTab.setTable(geff);
    geff = nullptr;
  } catch (std::out_of_range& e) {
    std::cout << "Efficiency Table is not found. Do not use weight then.\n";
  }
  if (!geff && !useWeight)
    std::cout << "MultEff option will be ignored because useWeight is disable.\n";

  auto getWeight = [&effTab](unsigned short n) -> double { return effTab.getWeight(n); };

  // I do not know if it is safe or not to write using xroot 
  // I disable xrootd by creating an object instead of using pointer.
  // remember that the AccessPathName returns false it the path exits!
  const auto out_dir = gSystem->DirName(ofileName.c_str());
  if (gSystem->AccessPathName(out_dir)) {
    gSystem->mkdir(out_dir, kTRUE);
  }
  TFile ofile(ofileName.c_str(), "recreate");
  std::cout << "Will write to " << ofileName << ".\n";
  // MC
  std::string mccuts = Form("cand_pT>%f && cand_pT<%f "
      "&& abs(cand_y)<%f && cand_matchGEN > 0.5",
      mycuts._pTMin, mycuts._pTMax, mycuts._yAbsMax);

  std::string fileMC = inputConfigs.getPaths("MC").at(0);
  std::string treeMC = inputConfigs.getName("MC");

  RDataFrame dfMC(treeMC, fileMC);
  auto dfMCSkim = dfMC.Filter(mccuts);
  const double ntotal = *dfMCSkim.Count();

  // data
  const char* wgtVarName = useWeight ? "weight" : 0;

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
  TTree* t = &chain;

  t->SetBranchStatus("*", 0);
  t->SetBranchStatus("cand_pT", 1);
  t->SetBranchStatus("cand_y", 1);
  t->SetBranchStatus(mvaName.c_str(), 1);
  t->SetBranchStatus("cand_mass", 1);
  t->SetBranchStatus("trigBit_2", 1);
  t->SetBranchStatus("trigBit_4", 1);
  t->SetBranchStatus("filterBit_4", 1);
  t->SetBranchStatus("filterBit_5", 1);
  t->SetBranchStatus("Ntrkoffline", 1);

  const auto trigcuts = mycuts._useMB ? "trigBit_4 > 0.5" : "trigBit_2 > 0.5";
  const auto ntrkcuts = ::string_format("Ntrkoffline >= %d && Ntrkoffline < %d",
                                        mycuts._NtrkofflineMin, mycuts._NtrkofflineMax);
  const auto kinecuts = ::string_format("abs(cand_y) < %.2f && cand_pT>%.2f && cand_pT<%.2f",
                                        mycuts._yAbsMax, mycuts._pTMin, mycuts._pTMax);

  RDataFrame dfData (chain);
  // auto dfDataSkim = dfData.Filter("filterBit_4 > 0.5").Filter(ntrkcuts).Filter(kinecuts);
  auto dfDataSkim = applyNewWeight ? dfData.Define("eventWeight", getWeight, {"Ntrkoffline"}).Filter("filterBit_4 > 0.5").Filter(ntrkcuts).Filter(kinecuts)
    : dfData.Filter("filterBit_4 > 0.5").Filter(ntrkcuts).Filter(kinecuts);

  std::vector<ROOT::RDF::RResultPtr<::TH1D > > ptrs;
  std::vector<ROOT::RDF::RResultPtr<::TH1D > > ptrsMC;
  const std::string trig = mycuts._useMB ? "MB" : "HM";
  const std::string ptbin = ::string_format("pT%.0fto%.0f", mycuts._pTMin, mycuts._pTMax);
  const auto cuts = getCuts(trig, ptbin);
  std::vector<double> eff;
  for (const auto cut : cuts) {
    std::string mvacut = ::string_format("%s > %f", mvaName.c_str(), cut);
    std::string mvalabel = ::string_format("mva%g", cut);
    std::string title = mvalabel + ";Mass (GeV);";
    std::replace(mvalabel.begin(), mvalabel.end(), '.', 'p');
    std::string histName = dsName + "_" + mvalabel;
    std::string histNameData = histName;
    std::string histNameMC = histName;
    histNameData.replace(0, 2, "hMass");
    histNameMC.replace(0, 2, "hMassMC");
    auto hist = mycuts._useWeight ?
      dfDataSkim.Filter(mvacut).Histo1D({histNameData.c_str(), title.c_str(), nbins, mass_min, mass_max},
                     "cand_mass", "eventWeight") :
      dfDataSkim.Filter(mvacut).Histo1D({histNameData.c_str(), title.c_str(), nbins, mass_min, mass_max},
                     "cand_mass");
    auto dfMCSkimMVA = dfMCSkim.Filter(mvacut);
    auto histMC = dfMCSkimMVA.Histo1D({histNameMC.c_str(), title.c_str(), nbins, mass_min, mass_max}, "cand_mass");
    ptrs.push_back(hist);
    ptrsMC.push_back(histMC);
    const double npass = *dfMCSkimMVA.Count();
    std::cout << "Passed " << npass << "\n";
    eff.push_back(npass/ntotal);
  }
  ofile.cd();
  for (auto& ptr : ptrs) { ofile.cd(); ptr->Write(); }
  for (auto& ptr : ptrsMC) { ofile.cd(); ptr->Write(); }
  TGraph effGraph(eff.size(), cuts.data(), eff.data());
  effGraph.Write("eff");

  RooWorkspace ws(wsName.c_str(), "");
  for (auto& ptr : ptrs) {
    RooRealVar mass("cand_mass", "", mass_min, mass_max);
    auto p = ptr.GetPtr();
    std::string dhName = (p->GetName());
    dhName.replace(0, 1, "dh_");
    RooDataHist dh(dhName.c_str(), "", mass, p);
    ws.import(dh);
  }

  for (auto& ptr : ptrsMC) {
    RooRealVar mass("cand_mass", "", mass_min, mass_max);
    auto p = ptr.GetPtr();
    std::string dhName = (p->GetName());
    dhName.replace(0, 1, "dh_");
    RooDataHist dh(dhName.c_str(), "", mass, p);
    ws.import(dh);
  }
  auto wsFileName = output.at("outdir") + wsName + ".root";
  std::cout << "Will write workspace to " << wsFileName << "\n";
  ws.writeToFile(wsFileName.c_str());
}

void produceWorkspaceLamC() 
{
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  // for (const auto trig : {"MB", "HM"}) {
  // for (const auto trig : {"HM"}) {
  for (const auto trig : {"MB"}) {
    // for (int ipt=nPt-1; ipt<nPt; ++ipt) {
    // for (int ipt=1; ipt<2; ++ipt) {
    // for (int ipt=1; ipt<nPt; ++ipt) {
    // for (int ipt=nPt-1; ipt<nPt; ++ipt) {
    for (int ipt=0; ipt<1; ++ipt) {
      if (ipt == 0 && !strcmp(trig, "HM")) continue;
      produceHistsLamC(::string_format("configs/pT%.0fto%.0f_%s.conf", pts[ipt], pts[ipt+1], trig));
    }
  }
}
