#if defined(__CLING__) and !defined(__ROOTCLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif
#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/Ana/Common.h"

#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

#include "TH1D.h"
#include "TChain.h"
#include "TFile.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TTree.h"
#include "TSystem.h"


#include <memory>
#include <string>
#include <stdexcept>

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

struct NTupleVars
{
  Float_t _cand_pT;
  Float_t _cand_mass;
  Float_t _cand_mva;
  Float_t _cand_y;
  Float_t _eventWeight;
  Float_t _eventWeightUp;
  Float_t _eventWeightLo;
  Int_t _trigBit_2;
  Int_t _trigBit_4;
  Int_t _filterBit_4;
  Int_t _filterBit_5;
  UShort_t _Ntrkoffline;
};

void produceWorkspaceLamC(std::string configName)
{
  // I do not use RooDataHist::add
  // because it seems that RooDataHist
  // merge underflow/overflow to lowest/highest bin
  // under rootv6.26.
  // Instead, I use TH1D.
  TH1D* hMass = new TH1D("hMass", ";Mass;Events", 100, 2.1, 2.47);
  const double massLo = 2.05;
  const double massHi = 2.55;

  FitParConfigs configs(configName.c_str());
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  bool useHistOnly = true;

  const bool useWeight = mycuts._useWeight;
  TGraph* g(0);
  EfficiencyTable<TGraph> effTab(g);
  try {
    const auto effFileName = inputConfigs.getPaths("MultEff").front();
    TFile fEff(effFileName.c_str());
    const auto effTabName = inputConfigs.getName("MultEff");
    TGraph* g = (TGraph*) fEff.Get(effTabName.c_str());
    effTab.setTable(g);
    g = nullptr;
    // useWeight = true;
  } catch (std::out_of_range& e) {
    std::cout << "Efficiency Table is not found. Do not use weight then.\n";
  }
  if (!g && !useWeight)
    std::cout << "MultEff option will be ignored because useWeight is disable.\n";

  const auto output = ::getNames(configs, mycuts);
  int effTypeCode = 0;
  try {
    effTypeCode = std::stoi(output.at("effType"));
  } catch (std::out_of_range& e) {}
  const auto effType = EfficiencyTable<TGraph>::Value(effTypeCode);
  std::string ofileName = output.at("fileName");
  const std::string dsName = output.at("dsName");
  const std::string wsName = output.at("wsName");
  const std::string dsMCSignalName = output.at("dsMCSignalName");
  const std::string mvaName = mycuts._mvaName;

  RooRealVar cand_mass("cand_mass",
                       "mass of #Lambda_{c}^{+} #rightarrow K_{S}^{0}p",
                       massLo, massHi);
  // think about what to do here, regarding MVA name
  RooRealVar cand_mva("cand_mva",
                      "MVA of #Lambda_{c}^{+} #rightarrow K_{S}^{0}p",
                      0, 1);
  RooRealVar cand_MLP(mvaName.c_str(),
                      "MVA of #Lambda_{c}^{+} #rightarrow K_{S}^{0}p",
                      0, 1);
  RooRealVar cand_pT("cand_pT",
                     "p_{T} of #Lambda_{c}^{+} #rightarrow K_{S}^{0}p",
                     0, 50);
  RooRealVar cand_y("cand_y",
                     "rapidity of #Lambda_{c}^{+} #rightarrow K_{S}^{0}p",
                     -50, 50);
  RooRealVar cand_matchGEN("cand_matchGEN", "Match GEN", -0.5, 1.5);
  RooRealVar Ntrkoffline("Ntrkoffline", "# of offline tracks", -0.5, 800.);
  RooRealVar dz1p0("dz1p0", "pile-up filter dz1p0", -0.5, 1.5);
  RooRealVar gplus("gplus", "pile-up filter gplus", -0.5, 1.5);
  RooRealVar trigHM("trigHM", "High multiplicity trigger", -0.5, 1.5);
  RooRealVar trigMB("trigMB", "Minimum bias trigger", -0.5, 1.5);

  RooRealVar wgtVar("weight", "weight from HM trigger", -0.1, 1.5);

  // not owned by default
  // either argSets.setRealValue("cand_mass", 2.29)
  // or cand_mass.setVal(2.29)
  auto argSets = RooArgSet(cand_mass, cand_mva, cand_pT, cand_y);
  if (useWeight) argSets.add(wgtVar);

  // I do not know to write using xroot is safe or not
  // I disable xrootd by creating an object instead of using pointer.
  // remember that the AccessPathName returns false it the path exits!
  ofileName = gSystem->BaseName(ofileName.c_str());
  ofileName = "./workspaces/" + ofileName;
  const auto out_dir = gSystem->DirName(ofileName.c_str());
  if (gSystem->AccessPathName(out_dir)) {
    gSystem->mkdir(out_dir, kTRUE);
  }
  TFile ofile(ofileName.c_str(), "recreate");
  std::cout << "Will write to " << ofileName << ".\n";
  RooWorkspace myws(wsName.c_str(), "");
  RooAbsData::setDefaultStorageType(RooAbsData::Tree);
  // MC
  std::string mccuts = Form("cand_pT>%f && cand_pT<%f "
      "&& abs(cand_y)<%f && %s > %f && cand_matchGEN > 0.5",
      mycuts._pTMin, mycuts._pTMax, mycuts._yAbsMax, mvaName.c_str(), mycuts._mvaCut);

  std::string fileMC = inputConfigs.getPaths("MC").at(0);
  std::string treeMC = inputConfigs.getName("MC");

  RooDataSet dsLamCMC(dsMCSignalName.c_str(), "matched LambdaCKsP",
      RooArgList(cand_mass, cand_pT, cand_y, cand_MLP, cand_matchGEN),
      RooFit::ImportFromFile(fileMC.c_str(), treeMC.c_str()),
      RooFit::Cut(mccuts.c_str()));
  myws.import(dsLamCMC);

  // data
  const char* wgtVarName = useWeight ? "weight" : 0;
  RooDataSet mydata(dsName.c_str(), "", argSets, wgtVarName);

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

  NTupleVars ntupleVars;
  t->SetBranchAddress("cand_pT", &ntupleVars._cand_pT);
  t->SetBranchAddress("cand_y", &ntupleVars._cand_y);
  t->SetBranchAddress(mvaName.c_str(), &ntupleVars._cand_mva);
  t->SetBranchAddress("cand_mass", &ntupleVars._cand_mass);
  // t->SetBranchAddress("eventWeight", &ntupleVars._eventWeight);
  // t->SetBranchAddress("eventWeightUp", &ntupleVars._eventWeightUp);
  // t->SetBranchAddress("eventWeightLo", &ntupleVars._eventWeightLo);
  t->SetBranchAddress("trigBit_2", &ntupleVars._trigBit_2);
  t->SetBranchAddress("trigBit_4", &ntupleVars._trigBit_4);
  t->SetBranchAddress("filterBit_4", &ntupleVars._filterBit_4);
  t->SetBranchAddress("filterBit_5", &ntupleVars._filterBit_5);
  t->SetBranchAddress("Ntrkoffline", &ntupleVars._Ntrkoffline);

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
  // if (useWeight) {
  //   switch (effType)  {
  //     case EfficiencyTable<TGraph>::Value::effCent :
  //       t->SetBranchStatus("eventWeight", 1); break;
  //     case EfficiencyTable<TGraph>::Value::effLow :
  //       t->SetBranchStatus("eventWeightUp", 1); break;
  //     case EfficiencyTable<TGraph>::Value::effUp :
  //       t->SetBranchStatus("eventWeightLo", 1); break;
  //     default:
  //       throw std::logic_error("The weight type is wrong.\n");
  //   }
  // }

  Long64_t nentries = t->GetEntries();
  for (Long64_t ientry=0; ientry!=nentries; ++ientry) {
    t->GetEntry(ientry);
    // event level
    // pileup filter
    const bool pass_dz1p0 = ntupleVars._filterBit_4 > 0.5;
    // pileup filter
    const bool pass_gplus = ntupleVars._filterBit_5 > 0.5;
    // trigger bits, MB
    const bool pass_MB = ntupleVars._trigBit_4 > 0.5;
    // trigger bits, MB
    const bool pass_HM = ntupleVars._trigBit_2 > 0.5;

    // multiplicity cuts
    const bool pass_ntrkoffline =
      ntupleVars._Ntrkoffline >= mycuts._NtrkofflineMin &&
      ntupleVars._Ntrkoffline < mycuts._NtrkofflineMax;

    // candidate level
    // rapidity cuts
    const bool pass_y = std::abs(ntupleVars._cand_y) < mycuts._yAbsMax;
    const bool pass_pT = ntupleVars._cand_pT > mycuts._pTMin
      && ntupleVars._cand_pT < mycuts._pTMax;
    // MVA cuts;
    const bool pass_mva = ntupleVars._cand_mva > mycuts._mvaCut;

    // in all
    bool pass_cuts = pass_ntrkoffline && pass_y && pass_pT && pass_mva;
    if (mycuts._useHM) pass_cuts = pass_cuts && pass_HM;
    if (mycuts._useMB) pass_cuts = pass_cuts && pass_MB;
    if (mycuts._usedz1p0) pass_cuts = pass_cuts && pass_dz1p0;
    if (mycuts._usegplus) pass_cuts = pass_cuts && pass_gplus;

    if (!pass_cuts) continue;

    // let us set up values
    cand_mass.setVal(ntupleVars._cand_mass);
    cand_mva.setVal(ntupleVars._cand_mva);
    cand_pT.setVal(ntupleVars._cand_pT);
    cand_y.setVal(ntupleVars._cand_y);

    // add them to dataset
    // forget the weight temporarily
    // I do not know how to handle this
    // it seems I need to add weight in RooArgSet, and
    // I also need to add the name of weight in RooDataSet constructor
    // here, should I do
    // weight.setVal(1.1); mydata.add(argSets, weight.getVal());
    // or I only need to do
    // mydata.add(argSets, 1.1)
    // ?

    if (useWeight) {
      // double eventWeight = 1;
      // switch (effType)  {
      //   case EfficiencyTable<TGraph>::Value::effCent :
      //     eventWeight = ntupleVars._eventWeight; break;
      //   case EfficiencyTable<TGraph>::Value::effLow :
      //     eventWeight = ntupleVars._eventWeightUp; break;
      //   case EfficiencyTable<TGraph>::Value::effUp :
      //     eventWeight = ntupleVars._eventWeightLo; break;
      //   default:
      //     throw std::logic_error("The weight type is wrong.\n");
      // }
      const double eventWeight = effTab.getWeight(ntupleVars._Ntrkoffline, effType);
      wgtVar.setVal(eventWeight);
      if (!useHistOnly) {
        mydata.add(argSets, eventWeight);
      }
      hMass->Fill(ntupleVars._cand_mass, eventWeight);
    } else {
      if (!useHistOnly) {
        mydata.add(argSets);
      }
      hMass->Fill(ntupleVars._cand_mass);
    }
  }
  // or you can myws.writeToFile("output.root", kTRUE)
  // the second arguments is for `recreate`
  // if you do
  // TFile ofile("name.root", "recreate"); TTree t("t", ""); t.Write();
  // t will flush parts of data to ofile once buffers are full
  // I do not know if writeToFile have similar results
  ofile.cd();
  if (!useHistOnly) {
    myws.import(mydata);
  } else {
    mydata.Delete();
    RooDataSet mydata(dsName.c_str(), "", argSets, wgtVarName);
    auto dh = RooDataHist(dsName.c_str(), "", cand_mass, hMass);
    myws.import(dh);
  }
  myws.Write();
  delete hMass;
}
