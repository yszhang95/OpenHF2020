// Original code can be found on
// https://github.com/root-project/root/blob/v6-06-00-patches/tutorials/tmva/TMVAClassificationApplication.C
// Modified by Yousen Zhang, Rice University, US
// Data: 2020 August 27
//
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <tuple>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>

#include "TChain.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TH3D.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "THashList.h"
#include "TFileCollection.h"

#include "Riostream.h"
#include "TString.h"
#include "TPRegexp.h"
#include "TClonesArray.h"
#include "TObjString.h"


#include "TMVA/Factory.h"
#include "TMVA/Tools.h"
#include "TMVA/Config.h"

#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
#include "TMVA/DataLoader.h"
#endif

#include "functions.h"
#include "TreeReader/ParticleTreeMC.hxx"
#include "TreeReader/ParticleTreeData.hxx"

#include "Ana/Common.h"
#include "Ana/TreeHelpers.h"

using std::map;
using std::tuple;
using std::string;
using std::vector;
using std::ifstream;
using std::getline;
using std::cout;
using std::endl;
using std::istringstream;
using std::unique_ptr;

using PtEtaPhiM_t = ROOT::Math::PtEtaPhiMVector;

using MatchPairInfo =  tuple<size_t, size_t, double, double>;

int TMVAClassificationApp(const tmvaConfigs& configs);

int main( int argc, char** argv )
{
  // Select methods (don't look at this code - not of interest)
  gROOT->SetBatch(true);

  if( argc<2 || argc>3 ) {
    std::cout << "Usage: TMVAClassificationApp configName [ DEBUG ]" << std::endl;
    return -1;
  }

  if (argc==3 && string(argv[2]).size()) { DEBUG = 1; }
  else DEBUG = 0;

  tmvaConfigs configs(argv[1], DEBUG);
  if (configs.getOutFileName().empty()) {
    string tempOutFileName(gSystem->BaseName(argv[1]));
    {
      auto pos = tempOutFileName.find(".xml");
      if (pos!=string::npos) { tempOutFileName.erase(pos, 4); }
      if (tempOutFileName.size())
        tempOutFileName.insert(0, "_");
    }
    configs.setOutFileName("TMVA" + tempOutFileName +"_Ntuple.root");
  } else {
    string tempOutFileName = configs.getOutFileName();
    if (tempOutFileName.find(".root") == string::npos) {
      tempOutFileName += "_Ntuple.root";
    }
    configs.setOutFileName("TMVA_" + tempOutFileName);
  }
  int code = TMVAClassificationApp(configs);
  return code;
}


int TMVAClassificationApp(const tmvaConfigs& configs)
{
  gROOT->SetBatch(true);
  const UShort_t NtrkHigh = configs.NtrkHigh();
  const UShort_t NtrkLow = configs.NtrkLow();
  const int  triggerIndex = configs.triggerIndex();
  const int  filterIndex  = configs.filterIndex();
  const bool saveTree = configs.saveTree();
  const bool saveDau = configs.saveDau();
  const bool selectMVA = configs.selectMVA();
  const bool isMC = configs.isMC();
  const bool saveMatchedOnly = configs.saveMatchedOnly();
  const bool flipEta = configs.flipEta();
  const bool selectDeDx = configs.selectDeDx();
  const bool pruneNTuple = configs.pruneNTuple();

  DeDxSelection dedxSel{0.7, 1.5, 0.75, 1.25};

  // reweight done in skimTree.cc
  //TFile effFile(configs.getEffFileName().c_str());
  TGraph* g(nullptr);
  /*
  if (effFile.IsOpen()) {
    if (configs.getEffGraphType() == "TGraphAsymmErrors") {
      const auto effname = configs.getEffGraphName();
      g = (TGraphAsymmErrors*) effFile.Get(effname.c_str());
    }
    else if (configs.getEffGraphType() == "TGraphErrors") {
      const auto effname = configs.getEffGraphName();
      g = (TGraphErrors*) effFile.Get(effname.c_str());
    }
  }
  */

  const bool reweightEvent = g;
  if (DEBUG) { cout << "Do reweighting: " << reweightEvent << endl;}

  //EfficiencyTable<TGraph> effTab(g);

  // The explicit loading of the shared libTMVA is done in TMVAlogon.C, defined in .rootrc
  // if you use your private .rootrc, or run from a different directory, please copy the
  // corresponding lines from .rootrc

  //---------------------------------------------------------------
  // This loads the library
  TMVA::Tools::Instance();

  std::cout << std::endl;
  std::cout << "==> Start TMVAClassification" << std::endl;

  // ----------------------------------------------------------------------------------------------

  // --- Here the preparation phase begins

  // Create a ROOT output file where TMVA will store ntuples, histograms, etc.
  TString outfileName = configs.getOutFileName().c_str();
  if (flipEta) {
    auto index = outfileName.Index(".root");
    if (index >0) {
      outfileName.Replace(index, 5, "_etaFlipped.root");
    }
  }
  TFile outputFile( outfileName, "RECREATE" );

  if(DEBUG) { cout << "Output file name: " << outfileName << endl; }

  // Create the reader object.
  TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );

  // If you wish to modify default settings
  // (please check "src/Config.h" to see all available global options)
  //    (TMVA::gConfig().GetVariablePlotting()).fTimesRMS = 8.0;
  //    (TMVA::gConfig().GetIONames()).fWeightFileDir = "myWeightDirectory";

  // Define the input variables that shall be used for the MVA training
  // note that you may also use variable expressions, such as: "3*var1/var2*abs(var3)"
  // [all types of expressions that can also be parsed by TTree::Draw( "expression" )]

  // variables in training

  vector<vector<TString>> allTrainVars;
  vector<map<string, TString>> trainingVars = configs.getTrainVarExps();
  for (const auto& var : trainingVars) {
    auto v = splitTString(var.at("eqn_vars"), ":");
    allTrainVars.push_back(v);
  }

  vector<vector<TString>> allSpectatorVars;
  vector<map<string, TString>> spectatorVars = configs.getSpecVarExps();
  for (const auto& var : spectatorVars) {
    auto v = splitTString(var.at("eqn_vars"), ":");
    allSpectatorVars.push_back(v);
  }

  vector<vector<TString>> allCommonCuts;
  vector<map<string, TString>> commonCuts = configs.getCutsVarExps();
  for (const auto& var : commonCuts) {
    auto v = splitTString(var.at("eqn_vars"), ":");
    allCommonCuts.push_back(v);
  }


  MVAHelper helper(trainingVars, spectatorVars, commonCuts);

  for (size_t ivar=0; ivar!=trainingVars.size(); ivar++) {
    const auto& pars = trainingVars.at(ivar);
    reader->AddVariable(pars.at("vars"), &helper.vars[ivar]);
  }
  for (size_t ivar=0; ivar!=spectatorVars.size(); ivar++) {
    const auto& pars = spectatorVars.at(ivar);
    reader->AddSpectator(pars.at("vars"), &helper.specs[ivar]);
  }

  // ---- Book MVA methods
  //
  // Please lookup the various method configuration options in the corresponding cxx files, eg:
  // src/MethoCuts.cxx, etc, or here: http://tmva.sourceforge.net/optionRef.html
  // it is possible to preset ranges in the option string in which the cut optimisation should be done:
  // "...:CutRangeMin[2]=-1:CutRangeMax[2]=1"...", where [2] is the third input variable
  TString dir;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  dir = "dataset/weights";
#else
  dir = "weights";
#endif
   TString prefix = "TMVAClassification";
   // Book method(s)
  auto MethodCollection = setupMethodCollection();
  vector<TString> methodNames;
  vector<TString> methodNames_copy;
  const auto methods = configs.getMethods();
  for (size_t im=0; im<methods.size(); ++im) {
    const auto& method = methods.at(im);
    // if (DEBUG) { for (const auto& p : method) { cout << "key: " << p.first << ", value: " << p.second << endl; } }
    TString methodName = method.at("name");
    TString postfix = configs.getTrainXMLs().at(im);
    auto pos = postfix.Index(".xml");
    postfix.Remove(pos, 4);
    TString weightfile = dir + "_" + postfix + "/" + prefix + TString("_") + methodName + TString(".weights.xml");
    methodNames_copy.push_back(methodName);
    methodName += " method";
    reader->BookMVA( methodName, weightfile );
    methodNames.push_back(methodName);
  }

  // Read data
  // Prepare output histogram
  const auto& binnings = configs.getHistoBinning();
  vector<unique_ptr<TH3D>> hMassNtrkMVA;
  vector<unique_ptr<TH3D>> hMassPtMVA[2];
  for (size_t iy=0; iy<2; ++iy) {
    auto& vec = hMassPtMVA[iy];
    vec.resize(methodNames_copy.size());
    for (size_t i=0; i<methodNames.size(); i++) {
      auto& ptr = vec[i];
      const auto& binning = binnings.at(i);
      if (binning.size() == 9) {
        ptr = std::make_unique<TH3D>
          (Form("hMassPtMVA_%s_y%d", methodNames_copy[i].Data(), iy),
           ";mass;pT;MVA",
           binning.at(0).Atoi(), binning.at(1).Atof(), binning.at(2).Atof(),
           binning.at(3).Atoi(), binning.at(4).Atof(), binning.at(5).Atof(),
           binning.at(6).Atoi(), binning.at(7).Atof(), binning.at(8).Atof());
      } else {
        throw std::runtime_error("Need to pass in TH3D binning!");
      }

    }
  }

  hMassNtrkMVA.resize(methodNames_copy.size());
  for (size_t i=0; i!=methodNames.size(); i++) {
    auto& ptr = hMassNtrkMVA[i];
    const auto& binning = binnings.at(i);
    if (binning.size() == 9) {
      ptr = std::make_unique<TH3D>
        (Form("hMassNtrkMVA_%s", methodNames_copy[i].Data()),
         ";mass;pT;MVA",
         binning.at(0).Atoi(), binning.at(1).Atof(), binning.at(2).Atof(),
         250, 0., 250.,
         binning.at(6).Atoi(), binning.at(7).Atof(), binning.at(8).Atof());
    } else {
      throw std::runtime_error("Need to pass in TH3D binning!");
    }

  }

  // histograms
  varHists hists(configs);

  // Prepare input tree (this must be replaced by your data source)
  const auto inputFileNames = configs.getInputFileNames();
  const auto treeInfo = configs.getTreeInfo();

  const auto treeDir = treeInfo.at("treeDir");
  const auto particle_id = treeInfo.at("pdgId").Atoi();
  Long64_t nentries = treeInfo.at("nentries").Atoll();

  cout << "--- TMVAClassificationApp    : Using inputs:" <<  endl;

  TChain t(treeDir+"/ParticleNTuple");
  addFilesToChain(&t, inputFileNames);
  const auto files = t.GetListOfFiles();
  for (int i=0; i<files->GetEntries(); ++i) {
    cout << "---\t\tInput file " << i << " "
         << files->At(i)->GetTitle() << ":"
         << files->At(i)->GetName() << endl;
  }
  // hard code to read tree/chain begin
  float cand_eta,
        cand_y,
        cand_mass,
        cand_pT,
        cand_phi,
        cand_angle3D,
        cand_dauCosOpenAngle3D,
        cand_dau_dR,
        cand_etaDau0,
        cand_massDau0,
        cand_pTDau0,
        cand_phiDau0,
        cand_etaDau1,
        cand_massDau1,
        cand_pTDau1,
        cand_phiDau1,
        cand_dau0_angle3D,
        cand_dau0_dca,
        cand_dau0_decayLength3D,
        cand_dau0_decayLengthError3D,
        trk_dau1_dEdx_dedxHarmonic2,
        trk_dau1_dEdxRes;
  unsigned short cand_Ntrkoffline;

  t.SetBranchAddress("cand_Ntrkoffline", &cand_Ntrkoffline);
  t.SetBranchAddress("cand_eta", &cand_eta);
  t.SetBranchAddress("cand_y", &cand_y);
  t.SetBranchAddress("cand_mass", &cand_mass);
  t.SetBranchAddress("cand_phi", &cand_phi);
  t.SetBranchAddress("cand_pT", &cand_pT);
  t.SetBranchAddress("cand_angle3D", &cand_angle3D);
  t.SetBranchAddress("cand_dauCosOpenAngle3D", &cand_dauCosOpenAngle3D);
  t.SetBranchAddress("cand_dau_dR", &cand_dau_dR);
  t.SetBranchAddress("cand_pTDau1", &cand_pTDau1);
  t.SetBranchAddress("cand_etaDau1", &cand_etaDau1);
  t.SetBranchAddress("cand_massDau1", &cand_massDau1);
  t.SetBranchAddress("cand_phiDau1", &cand_phiDau1);
  t.SetBranchAddress("cand_pTDau0", &cand_pTDau0);
  t.SetBranchAddress("cand_etaDau0", &cand_etaDau0);
  t.SetBranchAddress("cand_massDau0", &cand_massDau0);
  t.SetBranchAddress("cand_phiDau0", &cand_phiDau0);
  t.SetBranchAddress("trk_dau1_dEdxRes", &trk_dau1_dEdxRes);
  t.SetBranchAddress("cand_dau0_angle3D", &cand_dau0_angle3D);
  t.SetBranchAddress("cand_dau0_dca", &cand_dau0_dca);
  t.SetBranchAddress("cand_dau0_decayLength3D", &cand_dau0_decayLength3D);
  t.SetBranchAddress("cand_dau0_decayLengthError3D", &cand_dau0_decayLengthError3D);
  t.SetBranchAddress("trk_dau1_dEdx_dedxHarmonic2", &trk_dau1_dEdx_dedxHarmonic2);
  t.SetBranchAddress("trk_dau1_dEdxRes", &trk_dau1_dEdxRes);

  // hard code to read tree/chain end

  TString mycuts = "";
  for (const auto& var : commonCuts) {
    auto v = var.at("vars");
    mycuts += "&& ( " + v + " )";
  }
  if (mycuts.Length() == 0) mycuts = "1";
  else if (mycuts(0, 2) == "&&") mycuts = mycuts(2, mycuts.Length()-1);
  TTreeFormula fCuts("fCuts", mycuts, &t);

  outputFile.mkdir(treeDir);
  outputFile.cd(treeDir);

  TTree tt("ParticleNTuple", "ParticleNTuple");
  MyNTuple ntp(&tt);
  unsigned short dauNGDau[] = {2, 0};
  ntp.setNDau(2, 2, dauNGDau);
  if (reweightEvent) { ntp.initWeightBranch(); }
  ntp.initMVABranches(methodNames_copy);
  if (pruneNTuple) ntp.pruneNTuple(methodNames_copy);
  cout << "NTuple prepared" << endl;

  TH1D hNtrkoffline("hNtrkoffline", "N_{trk}^{offline} for PV with highest N;N_{trk}^{offline};", 300, 0., 300.);

  // hard code for LambdaC begin
  // hard code for LambdaC end
  // done in skimTree.cc

  if(nentries < 0) nentries = t.GetEntriesFast();

  // for TTreeFormula::Notify() or UpdateFormulaLeaves()
  Int_t treenumber = 0;
  // Event loop
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    auto jentry =  t.LoadTree(ientry);
    if (jentry < 0) break;
    t.GetEntry(ientry);

    // not sure if TTreeFormula::EvalInstance() will automatically iterate to next entry
    // or I have to load GetEntry manually
    // Let us use GetEntry manually, as we did above
    // The following code will make the length of the input tree and output tree different
    // only apply passCuts to histograms!
    if (treenumber != t.GetTreeNumber()) {
      treenumber = t.GetTreeNumber();
      fCuts.Notify(); // or UpdateFormulaLeaves()
    }
    const double passCutsVal = fCuts.EvalInstance();
    const int passCuts = static_cast<int>(std::round(passCutsVal));

    // event weight
    double eventWeight = 1;
    // check Ntrkoffline range
    // efficiency correction and Ntrk selections should be done in skimTree.cc

    //if (reweightEvent) ntp.setEventWeight(eventWeight);

    // check pileup filter
    // done in skimTree.cc

  // Prepare the event tree
  // - Here the variable names have to corresponds to your tree
  // - You can use the same variables as above which is slightly faster,
  //   but of course you can use different ones and copy the values inside the event loop
  //
    vector<float> mvaValues(methodNames.size(), -1);

    // begin LambdaC
    // hard code begin

        // special for Ks begin
        // check it is not a Lambda/Anti-Lambda
        // dielectron cross check
        // special for Ks end
        // done in skimTree.cc

    // manully assign values
    //ntp.cand_dau_p[1] = cand_pTDau1 * std::cosh(cand_etaDau1);
    ntp.cand_pTDau[1] = cand_pTDau1;
    ntp.trk_dau_dEdxRes[1] = trk_dau1_dEdxRes;
    ntp.cand_etaDau[1] = cand_etaDau1;
    ntp.cand_angle3D = cand_angle3D;
    ntp.cand_eta = cand_eta;
    ntp.cand_y = cand_y;
    ntp.cand_mass = cand_mass;
    ntp.cand_pT = cand_pT;
    ntp.cand_phi = cand_phi;
    ntp.cand_angle3D = cand_angle3D;
    ntp.cand_dauCosOpenAngle3D = cand_dauCosOpenAngle3D;
    ntp.cand_dau_dR = cand_dau_dR;
    ntp.cand_etaDau[0] = cand_etaDau0;
    ntp.cand_massDau[0] = cand_massDau0;
    ntp.cand_pTDau[0] = cand_pTDau0;
    ntp.cand_phiDau[0] = cand_phiDau0;
    ntp.cand_etaDau[1] = cand_etaDau1;
    ntp.cand_massDau[1] = cand_massDau1;
    ntp.cand_pTDau[1] = cand_pTDau1;
    ntp.cand_phiDau[1] = cand_phiDau1;
    ntp.cand_dau_angle3D[0] = cand_dau0_angle3D;
    ntp.cand_dau_dca[0] = cand_dau0_dca;
    ntp.cand_dau_decayLength3D[0] = cand_dau0_decayLength3D;
    ntp.cand_dau_decayLengthError3D[0] = cand_dau0_decayLengthError3D;
    ntp.trk_dau_dEdx_dedxHarmonic2[1] = trk_dau1_dEdx_dedxHarmonic2,
    ntp.trk_dau_dEdxRes[1] = trk_dau1_dEdxRes;
    ntp.cand_Ntrkoffline = cand_Ntrkoffline;

    // replace the following with TTreeFormula
    helper.GetValues(ntp, allTrainVars, allSpectatorVars, allCommonCuts);
    /*
    bool passCuts = true;
    for (size_t icut=0; icut<helper.nCuts; ++icut) {
      passCuts = passCuts && static_cast<int>(std::round(helper.cuts[icut]));
      if (!passCuts) break;
    }
    if (!passCuts) continue;
    */

    // does not select dE/dx anymore, done in skimTree.cc
    /*
        if (selectDeDx && !dedxSel(ntp.cand_dau_pT[1] * std::cosh(ntp.cand_dau_eta[1]),
                                   ntp.trk_dau_dEdx_dedxHarmonic2[1])) continue; // select proton
                                   */
    // does not selectMVA anymore
    bool passMVA = false;
    for (size_t i=0; i<methodNames.size(); i++) {
      const auto& methodName = methodNames.at(i);
      mvaValues[i] = reader->EvaluateMVA(methodName);
      //passMVA = passMVA || (selectMVA && mvaValues[i] > std::stod(configs.getMVACutMins().at(i)));
      if (abs(ntp.cand_y)<1.) {
        if (passCuts)
          hMassPtMVA[0][i]->Fill(ntp.cand_mass, ntp.cand_pT, mvaValues[i], eventWeight);
        if (passCuts)
          hMassNtrkMVA[i]->Fill(ntp.cand_mass, ntp.cand_Ntrkoffline, mvaValues[i], eventWeight);
      } else if (abs(ntp.cand_y)<2.) {
        if (passCuts)
          hMassPtMVA[1][i]->Fill(ntp.cand_mass, ntp.cand_pT, mvaValues[i], eventWeight);
      }
    }
    ntp.setMVAValues(mvaValues);
    if (abs(ntp.cand_y)<1. && passCuts) {
          hists.fillHists(ntp, eventWeight);
    }
    /*
       if (selectMVA &&  !passMVA) continue;
       */
    if (saveTree) ntp.fillNTuple();
    // hard code end
  }
  // Save the output
  // outputFile.Write();
  hNtrkoffline.Write();
  if (saveTree) ntp.t->Write();
  for (const auto& vec : hMassPtMVA) {
    for (const auto& h : vec) h->Write();
  }
  for (const auto& h : hMassNtrkMVA) h->Write();
  hists.writeHists();

  std::cout << "==> Wrote root file: " << outputFile.GetName() << std::endl;
  std::cout << "==> TMVAClassificationApplication is done!" << std::endl;

  delete reader;
  return 0;
}
