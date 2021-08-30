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
#include "TH1D.h"
#include "TH3D.h"
#include "TTree.h"
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

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
int TMVACrossValidationApp(const tmvaConfigs&);
#endif

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
    configs.setOutFileName("TMVA_CV" + tempOutFileName +".root");
  } else {
    string tempOutFileName = configs.getOutFileName();
    if (tempOutFileName.find(".root") == string::npos) {
      tempOutFileName += ".root";
    }
    configs.setOutFileName("TMVA_CV_" + tempOutFileName);
  }

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  int code = TMVACrossValidationApp(configs);
#else
  cerr << "ROOT VERSION is too low. Please upgrade to the latest version" << endl;
  int code = -1;
#endif
  return code;
}

#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
int TMVACrossValidationApp(const tmvaConfigs& configs)
{
  gROOT->SetBatch(true);
  const bool saveTree = configs.saveTree();
  const bool saveDau = configs.saveDau();
  const bool selectMVA = configs.selectMVA();
  const bool isMC = configs.isMC();
  const bool saveMatchedOnly = configs.saveMatchedOnly();
  const bool flipEta = configs.flipEta();
  const bool selectDeDx = configs.selectDeDx();

  DeDxSelection dedxSel{0.7, 1.5, 0.75, 1.25};
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
  TString outfileName = configs.getOutFileName();
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
    // hard code, temporary use
    if (pars.at("vars") != "eventID := eventID % 8096")
      reader->AddSpectator(pars.at("vars"), &helper.specs[ivar]);
  }

  // hard code here, temporary use
  int eventID = 0;
  reader->AddSpectator("eventID := eventID % 8096", &eventID);

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
  TString prefix = "TMVACrossValidation";
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
  vector<unique_ptr<TH3D>> hMassPtMVA[2];
  for (size_t iy=0; iy<2; ++iy) {
    auto& vec = hMassPtMVA[iy];
    vec.resize(methodNames_copy.size());
    for (size_t i=0; i<methodNames.size(); i++) {
      auto& ptr = vec[i];
      ptr = std::make_unique<TH3D>(Form("hMassPtMVA_%s_y%d", methodNames_copy[i].Data(), iy), ";mass;pT;MVA", 120, 2.15, 2.45, 10, 0., 10., 1000, 0., 1.);
    }
  }

  // Prepare input tree (this must be replaced by your data source)
  const auto inputFileNames = configs.getInputFileNames();
  const auto treeInfo = configs.getTreeInfo();

  const auto treeDir = treeInfo.at("treeDir");
  const auto particle_id = treeInfo.at("pdgId").Atoi();
  Long64_t nentries = treeInfo.at("nentries").Atoll();

  cout << "--- TMVAClassificationApp    : Using inputs: " << endl;

  TChain t(treeDir+"/ParticleTree");
  addFilesToChain(&t, inputFileNames);
  const auto files = t.GetListOfFiles();
  for (int i=0; i<files->GetEntries(); ++i) {
    cout << "---\t\tInput file " << i << " "
         << files->At(i)->GetTitle() << ":"
         << files->At(i)->GetName() << endl;
  }

  ParticleTree* pp;
  if (configs.isMC()) pp = new ParticleTreeMC(&t);
  else pp = new ParticleTreeData(&t);
  ParticleTree& p = *pp;

  outputFile.mkdir(treeDir);
  outputFile.cd(treeDir);
  TTree tt("ParticleNTuple", "ParticleNTuple");
  MyNTuple ntp(&tt);
  ntp.dropDau = !saveDau;
  ntp.isMC = isMC;
  ntp.flipEta = flipEta;
  unsigned short dauNGDau[] = {2, 0};
  ntp.setNDau(2, 2, dauNGDau);
  ntp.initMVABranches(methodNames_copy);
  ntp.initNTuple();

  TH1I hNtrkoffline("hNtrkoffline", "N_{trk}^{offline} for PV with highest N;N_{trk}^{offline};", 300, 0., 300.);

  // hard code for LambdaC begin
  MatchCriterion  matchCriterion(0.03, 0.1);
  Particle particle(4122);
  if (isMC) {
    Particle Ks(310);
    Ks.selfConj(true);
    Ks.longLived(true);
    particle.addDaughter(Ks);
    Particle proton(2212);
    particle.addDaughter(proton);
  }
  // hard code for LambdaC end

  if(nentries < 0) nentries = p.GetEntriesFast();
  // Event loop
  Long64_t num = 0;
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    auto jentry =  p.LoadTree(ientry);
    if (jentry < 0) break;
    p.GetEntry(ientry);

    // check pileup filter
    if (!p.evtSel().at(4)) continue;
    if (!isMC) hNtrkoffline.Fill(p.Ntrkoffline());

    const auto recosize = p.cand_mass().size();
    const auto& pdgId = p.cand_pdgId();

    // reco gen matching begin
    map<size_t, size_t> matchPairs;
    if (isMC) {
      const auto gensize = p.gen_mass().size();
      const auto& gen_pdgId = p.gen_pdgId();

      map<size_t, PtEtaPhiM_t> p4GenFS;
      for (size_t igen=0; igen<gensize; igen++) {
        if (abs(gen_pdgId[igen]) != std::abs(particle.id())) continue;
        const auto gen_dauIdx = p.gen_dauIdx().at(igen);
        Particle particle_copy(particle);
        bool sameChain = checkDecayChain(particle_copy, igen, p);
        if (!sameChain) continue;
        p4GenFS[particle_copy.daughter(0)->treeIdx()] = getGenP4(particle_copy.daughter(0)->treeIdx(), p); // Ks from LambdaC
        p4GenFS[particle_copy.daughter(1)->treeIdx()] = getGenP4(particle_copy.daughter(1)->treeIdx(), p); // proton from LambdaC
      }
      map<size_t, PtEtaPhiM_t> p4RecoFS;
      for (size_t ireco=0; ireco<recosize; ireco++) {
        if (p.cand_pdgId().at(ireco) == PdgId::Ks ||
            p.cand_pdgId().at(ireco) == PdgId::Proton)
          p4RecoFS[ireco] = getRecoP4(ireco, p); // Ks or proton
      }
      vector<bool> recoLock(recosize, 0);
      vector<bool> genLock (gensize,  0);
      vector<MatchPairInfo> matchGenInfo;
      for (const auto& gen : p4GenFS) {
        for (const auto& reco : p4RecoFS) {
          if (matchCriterion.match(reco.second, gen.second)) {
            const auto dR = ROOT::Math::VectorUtil::DeltaR(reco.second, gen.second);
            const auto dRelPt = TMath::Abs(gen.second.Pt() - reco.second.Pt())/gen.second.Pt();
            matchGenInfo.push_back( std::make_tuple(reco.first, gen.first, dR, dRelPt) );
          }
        }
      }
      // sort by pT
      std::sort(matchGenInfo.begin(), matchGenInfo.end(), [](MatchPairInfo i, MatchPairInfo j) { return std::get<3>(i) < std::get<3>(j); } );

      for (auto e : matchGenInfo) {
        const auto ireco = std::get<0>(e);
        const auto igen  = std::get<1>(e);
        if (!recoLock.at(ireco) && !genLock.at(igen)) {
          recoLock.at(ireco) = 1;
          genLock.at(igen)   = 1;
          matchPairs.insert(map<size_t, size_t>::value_type(ireco, igen));
        }
      }
    }
    // reco gen matching end

  // Prepare the event tree
  // - Here the variable names have to corresponds to your tree
  // - You can use the same variables as above which is slightly faster,
  //   but of course you can use different ones and copy the values inside the event loop
  //
    vector<float> mvaValues(methodNames.size(), -1);
    for (size_t ireco=0; ireco<recosize; ireco++) {
      // begin LambdaC
      if (pdgId[ireco] == std::abs(particle_id)) {
        eventID = (num++) % 8192;

        // hard code begin
        const auto dauIdx = p.cand_dauIdx().at(ireco);
        // 0 for Ks, 1 for proton
        const auto gDauIdx = p.cand_dauIdx().at(dauIdx.at(0));
        if (isMC) {
          // check reco-gen match
          bool matchGEN = true;
          bool isSwap = false;

          ntp.cand_dau_matchGEN[0] = matchPairs.find( dauIdx.at(0) ) != matchPairs.end();
          ntp.cand_dau_matchGEN[1] = matchPairs.find( dauIdx.at(1) ) != matchPairs.end();
          matchGEN = matchGEN && ( matchPairs.find( dauIdx.at(0) ) != matchPairs.end() );
          matchGEN = matchGEN && ( matchPairs.find( dauIdx.at(1) ) != matchPairs.end() );

          if (ntp.cand_dau_matchGEN[0]) {
            ntp.cand_dau_isSwap[0] = std::abs(0.497611 - p.gen_mass().at(matchPairs.at(dauIdx.at(0)))) > 0.01;
            isSwap = isSwap || ntp.cand_dau_isSwap[0];
          }
          if (ntp.cand_dau_matchGEN[1]) {
            ntp.cand_dau_isSwap[1] = std::abs(0.938272081 - p.gen_mass().at((matchPairs.at(dauIdx.at(1))))) > 0.01;
            isSwap = isSwap || ntp.cand_dau_isSwap[1];
          }

          ntp.cand_matchGEN = matchGEN;
          ntp.cand_isSwap = isSwap;
          if (saveMatchedOnly && !matchGEN) continue;
        }
        // hard code end

        // special for Ks begin
        // check it is not a Lambda/Anti-Lambda
        PtEtaPhiM_t gDau0P4 = getRecoDauP4(dauIdx.at(0), 0, p); // pi- from Ks
        PtEtaPhiM_t gDau1P4 = getRecoDauP4(dauIdx.at(0), 1, p); // pi+ from Ks
        if (gDau0P4.P() > gDau1P4.P()) {
          gDau0P4.SetM(0.938272081);
        } else {
          gDau1P4.SetM(0.938272081);
        }
        PtEtaPhiM_t total = gDau0P4 + gDau1P4;
        if ( std::abs(total.M() - 1.11568) < 0.02) { continue; }
        // dielectron cross check
        gDau0P4.SetM(0.000511);
        gDau1P4.SetM(0.000511);
        PtEtaPhiM_t ee = gDau0P4 + gDau1P4;
        if ( ee.M() <0.2 ) { continue; }
        // special for Ks end

        ntp.retrieveTreeInfo(p, ireco);
        helper.GetValues(ntp, allTrainVars, allSpectatorVars, allCommonCuts);
        bool passCuts = true;
        for (size_t icut=0; icut<helper.nCuts; ++icut) {
          passCuts = passCuts && static_cast<int>(std::round(helper.cuts[icut]));
          if (!passCuts) break;
        }
        if (!passCuts) continue;
        bool passMVA = false;
        for (size_t i=0; i<methodNames.size(); i++) {
          const auto& methodName = methodNames.at(i);
          mvaValues[i] = reader->EvaluateMVA(methodName);
          passMVA = passMVA || (selectMVA && mvaValues[i] > std::stod(configs.getMVACutMins().at(i)));
          if (abs(ntp.cand_y)<1.) {
            hMassPtMVA[0][i]->Fill(ntp.cand_mass, ntp.cand_pT, mvaValues[i]);
          } else if (abs(ntp.cand_y)<2.) {
            hMassPtMVA[1][i]->Fill(ntp.cand_mass, ntp.cand_pT, mvaValues[i]);
          }
        }
        ntp.setMVAValues(mvaValues);
        if (selectMVA &&  !passMVA) continue;
        if (selectDeDx && !dedxSel(ntp.cand_dau_pT[1] * std::cosh(ntp.cand_dau_eta[1]),
                                   ntp.trk_dau_dEdx_dedxHarmonic2[1])) continue; // select proton
        if (saveTree) ntp.fillNTuple();
      }
    }
  }
  // Save the output
  // outputFile.Write();
  hNtrkoffline.Write();
  if (saveTree) ntp.t->Write();
  for (const auto& vec : hMassPtMVA) {
    for (const auto& h : vec) h->Write();
  }

  std::cout << "==> Wrote root file: " << outputFile.GetName() << std::endl;
  std::cout << "==> TMVAClassificationApplication is done!" << std::endl;

  delete reader;
  delete pp;
  return 0;
}
#endif
