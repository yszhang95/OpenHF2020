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

std::vector<double> get_mva_binning(
    const std::string trig,
    const double ptmin, const double ptmax)
{
  return {};
}


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

void produce2DHistsLamC(std::string configName)
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
  std::string ofileName = output.at("fileName");
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
  // where is daughter kinematics?
  std::string mccuts = Form("cand_pT>%f && cand_pT<%f "
      "&& abs(cand_y)<%f && cand_matchGEN > 0.5",
      mycuts._pTMin, mycuts._pTMax, mycuts._yAbsMax);

  std::string fileMC = inputConfigs.getPaths("MC").at(0);
  std::string treeMC = inputConfigs.getName("MC");

  RDataFrame dfMC(treeMC, fileMC);
  auto dfMCSkim = dfMC.Filter(mccuts);

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

  const std::string trig_str = mycuts._useMB ? "MB" : "HM";
  VarSetup setup(trig_str, mycuts._pTMin, mycuts._pTMax);

  const auto trigcuts = mycuts._useMB ? "trigBit_4 > 0.5" : "trigBit_2 > 0.5";
  const auto ntrkcuts = ::string_format("Ntrkoffline >= %d && Ntrkoffline < %d",
                                        mycuts._NtrkofflineMin, mycuts._NtrkofflineMax);
  const auto kinecuts = ::string_format("abs(cand_y) < %.2f && cand_pT>%.2f && cand_pT<%.2f",
                                        mycuts._yAbsMax, mycuts._pTMin, mycuts._pTMax);

  RDataFrame dfData (chain);
  // auto dfDataSkim = dfData.Filter("filterBit_4 > 0.5").Filter(ntrkcuts).Filter(kinecuts);
  auto dfDataSkim = applyNewWeight ? dfData.Define("eventWeight", getWeight, {"Ntrkoffline"}).Filter("filterBit_4 > 0.5").Filter(ntrkcuts).Filter(kinecuts)
    : dfData.Filter("filterBit_4 > 0.5").Filter(ntrkcuts).Filter(kinecuts);

  std::vector<ROOT::RDF::RResultPtr<::TH2D > > ptrs;
  std::vector<ROOT::RDF::RResultPtr<::TH2D > > ptrsMC;
  const std::string trig = mycuts._useMB ? "MB" : "HM";
  const std::string ptbin = ::string_format("pT%.0fto%.0f", mycuts._pTMin, mycuts._pTMax);
  std::string title = ";Mass (GeV);MVA;";
  std::string histName = dsName;
  std::string histNameData = histName;
  std::string histNameMC = histName;
  histNameData.replace(0, 2, "hMassAndMVA");
  histNameMC.replace(0, 2, "hMassAndMVAMC");
  auto hist = mycuts._useWeight ?
    dfDataSkim.Histo2D({histNameData.c_str(), title.c_str(), nbins, mass_min, mass_max, setup.nbins_mva, setup.mva_min, setup.mva_max},
        "cand_mass", mvaName, "eventWeight") :
    dfDataSkim.Histo2D({histNameData.c_str(), title.c_str(), nbins, mass_min, mass_max, setup.nbins_mva, setup.mva_min, setup.mva_max},
        "cand_mass", mvaName);
  auto histMC = dfMCSkim.Histo2D({histNameMC.c_str(), title.c_str(), nbins, mass_min, mass_max, setup.nbins_mva, setup.mva_min, setup.mva_max},
      "cand_mass", mvaName);

  auto histEffMC = dfMCSkim.Histo1D({"hMVAMC", "MVA;MVA;Counts", setup.nbins_mva, setup.mva_min, setup.mva_max}, mvaName);

  ofile.cd();
  hist->Write();
  histMC->Write();

  // efficiency
  TGraphAsymmErrors gEffificiency;
  const double total = histMC->GetEntries();

  RooWorkspace myws(wsName.c_str(), "myws");
  RooRealVar cand_mass("cand_mass", "Mass of candidates", mass_min, mass_max);
  myws.import(cand_mass);

  float mva_cut=setup.cut_min;
  std::vector<double> mva_cuts{};
  std::vector<int > mva_ibins{};
  for (int start_bin = hist->GetYaxis()->FindBin(setup.cut_min);
      mva_cut<setup.cut_max; start_bin += setup.step){
    mva_cut = hist->GetYaxis()->GetBinLowEdge(start_bin);
    mva_ibins.push_back(start_bin);
    mva_cuts.push_back(mva_cut);
  }
  {
    auto ibin = hist->GetYaxis()->FindBin(mycuts._mvaCut);
    auto lowEdge = hist->GetYaxis()->GetBinLowEdge(ibin);
    if (std::abs((lowEdge-mycuts._mvaCut)/mycuts._mvaCut)) {
      std::cout << "WRONG bin edge for MVA optimal cut\n";
    }
    auto it = std::find_if(mva_cuts.begin(), mva_cuts.end(),
        [&mycuts](const double cut) { return std::abs(mycuts._mvaCut-cut)/mycuts._mvaCut <1E-3; });
    if (it == mva_cuts.end()) {
      mva_cuts.push_back(mycuts._mvaCut);
      mva_ibins.push_back(ibin);
      std::sort(mva_cuts.begin(), mva_cuts.end());
    }
  }

  //for (const auto mva_cut : mva_cuts) {
  for (int ip=0; ip<mva_cuts.size(); ip++) {
    // std::cout << mva_cut << "\n";
    const auto mva_cut = mva_cuts.at(ip);
    const auto start_bin = mva_ibins.at(ip);
    auto mva_label = "_mva" + ::string_format("%g", mva_cut);
    std::replace(mva_label.begin(), mva_label.end(), '.', 'p');

    // data
    std::string name = "h_Mass" + mva_label;
    auto h = hist->ProjectionX(name.c_str(), start_bin, 100000);
    // MC
    name.replace(0, 1, "hMC");
    auto hMC = histMC->ProjectionX(name.c_str(), start_bin, 100000);

    // data
    std::string ds_name = dsName + mva_label;
    ds_name.replace(1, 1, "h_Mass");
    std::string ds_title = "#Lambda_{c}^{+} MVA > " + std::to_string(mva_cut);
    RooDataHist dh(ds_name.c_str(), ds_title.c_str(), cand_mass, h);
    // MC
    ds_name.insert(7, "MC");
    ds_title = "MC " + ds_title;
    RooDataHist dhMC(ds_name.c_str(), ds_title.c_str(), cand_mass, hMC);

    myws.import(dh);
    myws.import(dhMC);

    // efficiency
    const double pass = hMC->GetEntries();
    const double eff_val = pass / total;
    const double eff_upper = TEfficiency::ClopperPearson(total, pass, 0.68, true);
    const double eff_lower = TEfficiency::ClopperPearson(total, pass, 0.68, false);
    const double err_upper = eff_upper - eff_val;
    const double err_lower = eff_val - eff_lower;
    gEffificiency.AddPoint(mva_cut, eff_val);
    gEffificiency.SetPointError(gEffificiency.GetN()-1, 0, 0, err_lower, err_upper);
  }

  // calculate efficiency
  TGraphAsymmErrors gEffFull(setup.nbins_mva);
  for (int i=0; i<setup.nbins_mva; ++i) {
    auto pass = histEffMC->Integral(i+1, setup.nbins_mva+1);
    auto mva_cut = histEffMC->GetBinLowEdge(i+1);
    const double eff_val = pass / total;
    const double eff_upper = TEfficiency::ClopperPearson(total, pass, 0.68, true);
    const double eff_lower = TEfficiency::ClopperPearson(total, pass, 0.68, false);
    const double err_upper = eff_upper - eff_val;
    const double err_lower = eff_val - eff_lower;
    gEffFull.SetPoint(i, mva_cut, eff_val);
    gEffFull.SetPointError(i, 0, 0, err_lower, err_upper);
  }
  
  ofile.cd();
  myws.Write();
  gEffificiency.Write("efficiency");
  gEffFull.Write("effFull");

}

void produceWorkspaceLamC_cuts() 
{
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  for (const auto trig : {"MB", "HM"}) {
  // for (const auto trig : {"HM"}) {
  // for (const auto trig : {"MB"}) {
    // for (int ipt=nPt-1; ipt<nPt; ++ipt) {
    for (int ipt=0; ipt<nPt; ++ipt) {
    // for (int ipt=1; ipt<2; ++ipt) {
    // for (int ipt=1; ipt<nPt; ++ipt) {
    // for (int ipt=nPt-1; ipt<nPt; ++ipt) {
    // for (int ipt=0; ipt<1; ++ipt) {
      if (std::abs(pts[ipt]-2)<1E-3 && !strcmp(trig, "HM")) continue;
      produce2DHistsLamC(::string_format("configs/pT%.0fto%.0f_%s.conf", pts[ipt], pts[ipt+1], trig));
    }
  }
}
