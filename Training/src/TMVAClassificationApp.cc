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

#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooRealVar.h"

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
using std::cerr;
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
    configs.setOutFileName("TMVA" + tempOutFileName +".root");
  } else {
    string tempOutFileName = configs.getOutFileName();
    if (tempOutFileName.find(".root") == string::npos) {
      tempOutFileName += ".root";
    }
    configs.setOutFileName("TMVA_" + tempOutFileName);
  }
  int code = TMVAClassificationApp(configs);
  cerr << "\nThe code for TMVAClassificationApp is " << code << endl;
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
  const bool wantAbort = configs.wantAbort();

  if (wantAbort) { gErrorAbortLevel = kError; }

  DeDxSelection dedxSel{0.7, 1.5, 0.75, 1.25};

  TFile effFile(configs.getEffFileName().c_str());
  TGraph* g(nullptr);
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

  const bool reweightEvent = g;
  if (DEBUG) { cout << "Do reweighting: " << reweightEvent << endl;}

  EfficiencyTable<TGraph> effTab(g);

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

  TString outfileNameWS = outfileName;
  {
    auto index =  outfileNameWS.Index(".root");
    if (index > 0) {
      outfileNameWS.Replace(index, 5, "_ws.root");
    }
  }

  TFile outputFileWS( outfileNameWS, "RECREATE" );
  RooAbsData::setDefaultStorageType(RooAbsData::Tree);
  const auto wsStrs = configs.getWorkspaceStrs();
  const auto dsStrs = configs.getDataSetStrs();
  const bool useWS = !wsStrs.empty();
  RooRealVar NtrkPV("Ntrkoffline", "Ntrkoffline w.r.t. PV with highest N", 0, 400, "");
  // LambdaC kinematic info
  RooRealVar cand_mass("cand_mass", "mass of #Lambda_{c}^{+} candidate", 2.15, 2.45, "GeV");
  RooRealVar cand_pT  ("cand_pT", "p_{T} of #Lambda_{c}^{+} candidate",  1.9, 100., "GeV");
  RooRealVar cand_eta("cand_eta", "#eta of #Lambda_{c}^{+} candidate", -2.4, 2.4, "");
  RooRealVar cand_y("cand_y", "rapidity of #Lambda_{c}^{+} candidate", -1.0, 1.0, "");
  const double piVal = TMath::Pi();
  RooRealVar cand_phi("cand_phi", "#phi of #Lambda_{c}^{+} candidate", -piVal, piVal, "");
  // LambdaC decay info
  RooRealVar cand_decayLength3D("cand_decayLength3D", "3D decay length of #Lambda_{c}^{+} candidate", 0, 20, "cm");
  RooRealVar cand_decayLengthSig3D("cand_decayLength3DSig", "3D decay length significance of #Lambda_{c}^{+} candidate", 0, 50);
  RooRealVar cand_cosAngle3D("cand_cosAngle3D", "Cosine of 3D pointing angle of #Lambda_{c}^{+} candidate", -1., 1.);
  // Ks kinematic info
  RooRealVar cand_dau0_mass("cand_dau0_mass", "mass of K_{S}^{0}", 0.46, 0.54, "GeV");
  RooRealVar cand_dau0_pT("cand_dau0_pT", "p_{T} of K_{S}^{0}", 0, 100, "GeV");
  RooRealVar cand_dau0_eta("cand_dau0_eta", "#eta of K_{S}^{0}", -2.4, 2.4, "");
  RooRealVar cand_dau0_phi("cand_dau0_phi", "#phi of K_{S}^0", -piVal, piVal, "");
  // proton info
  RooRealVar cand_dau1_pT("cand_dau1_pT", "p_{T} of proton", 0, 100, "GeV");
  RooRealVar cand_dau1_eta("cand_dau1_eta", "#eta of proton", -2.4, 2.4, "");
  RooRealVar cand_dau1_phi("cand_dau1_phi", "#phi of proton", -piVal, piVal, "");
  RooRealVar trk_dau1_dEdxRes("trk_dau1_dEdxRes", "dE/dx residual for proton", -1., 1., "");
  // trigger bits
  RooRealVar passHM("trigBit_2", "pass HM185", -1, 2, "");
  RooRealVar passMB("trigBit_4", "pass MB", -1, 2, "");
  RooRealVar passDz1p0("filterBit_4", "pass dz1p0", -1, 2, "");
  RooRealVar passGplus("filterBit_5", "pass Gplus", -1, 2, "");
  // MVA
  RooRealVar MVA("cand_mva", "first method in MVA collection", -1, 1, "");
  // at most 9 variables in old ROOT
  RooArgSet cols(cand_mass, cand_pT, cand_eta, cand_y, cand_phi,
                 cand_decayLength3D, cand_decayLengthSig3D, cand_cosAngle3D);
  cols.add(cand_dau0_mass); cols.add(cand_dau0_pT);
  cols.add(cand_dau0_eta); cols.add(cand_dau0_phi);
  cols.add(cand_dau1_pT); cols.add(cand_dau1_eta);
  cols.add(cand_dau1_phi); cols.add(trk_dau1_dEdxRes);
  cols.add(NtrkPV); cols.add(passHM); cols.add(passMB);
  cols.add(passDz1p0); cols.add(passGplus);
  cols.add(MVA);
  // weight
  RooRealVar wgtVar("weight", "event weight", 0, 100);
  if (reweightEvent) cols.add(wgtVar);
  // workspace and dataset
  RooWorkspace ws(wsStrs.at(0), wsStrs.at(1));
  const char* wgtVarName = reweightEvent ? "weight" : 0;
  RooDataSet ds(dsStrs.at(0), dsStrs.at(1), cols, wgtVarName);

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
  if (reweightEvent) { ntp.initWeightBranch(); }
  ntp.initMVABranches(methodNames_copy);
  ntp.initNTuple();
  // std::vector<TString> keptBranches{"cand_mass", "cand_pTDau0", "cand_etaDau0"};
  const auto keptBranches = configs.getKeptBranchNames();
  if (pruneNTuple) ntp.pruneNTuple(keptBranches);
  cout << "NTuple prepared" << endl;

  TH1D hNtrkoffline("hNtrkoffline", "N_{trk}^{offline} for PV with highest N;N_{trk}^{offline};", 300, 0., 300.);
  TH1D hNtrkofflineDz1p0("hNtrkofflineDz1p0", "N_{trk}^{offline} for PV with highest N, dz1p0;N_{trk}^{offline};", 300, 0., 300.);
  TH1D hNtrkofflineGplus("hNtrkofflineGplus", "N_{trk}^{offline} for PV with highest N, Gplus;N_{trk}^{offline};", 300, 0., 300.);

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
  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    auto jentry =  p.LoadTree(ientry);
    if (jentry < 0 && jentry != -2) {
      outputFile.Write();
      outputFileWS.Write();
      delete reader;
      delete pp;
      return jentry;
    }
    if (jentry < 0) {
      cout << "Code for the last entry is " << ientry << endl;
      break;
    }
    auto bytes = p.GetEntry(ientry);
    if (bytes == 0) {
      std::cerr << "[ERROR] in TMVAClassificationApp: Cannot read "
                << ientry << "th entry properly" << std::endl;
      outputFile.Write();
      outputFileWS.Write();
      delete reader;
      delete pp;
      return -2;
    }

    // check trigger filter;
    if (!isMC && !p.passHLT().at(triggerIndex)) continue;
    // check pileup filter
    // if (!p.evtSel().at(4)) continue;
    // if (!isMC) {
    //   const bool passEventSel = passEvent(p, filterIndex, triggerIndex);
    //   if (!passEventSel) continue;
    // }
    double ntrkWeight = 1.;
    double NtrkInFloat = 0;
    if (!isMC) {
      const auto ntrk = p.Ntrkoffline();
      if (reweightEvent && ntrk < 400) {
        ntrkWeight = effTab.getWeight(ntrk);
      }
      hNtrkoffline.Fill(ntrk, ntrkWeight);
      if (p.evtSel().at(4)) {
        hNtrkofflineDz1p0.Fill(ntrk, ntrkWeight);
      }
      if (p.evtSel().at(5)) {
        hNtrkofflineGplus.Fill(ntrk, ntrkWeight);
      }

      // temporary usage
      // check it in the future if I have new trees
      // now use vertex.trackSize() with largest number
      if ( ntrk >= NtrkHigh || ntrk < NtrkLow) continue;
      NtrkInFloat = ntrk;
    }

    // event weight
    double eventWeight = 1;
    // check Ntrkoffline range
    // temporarily not useful
    if (!isMC) {
      const auto noCandidate = p.cand_mass().empty();
      if (noCandidate) continue;
      if (DEBUG && noCandidate) cout << "No candidate in this event" << endl;
      const auto Ntrk = p.cand_Ntrkoffline().at(0);
      // if ( Ntrk >= NtrkHigh || Ntrk < NtrkLow) continue;
      if (reweightEvent) eventWeight = effTab.getWeight(Ntrk);
      // cout << "Ntrk: " << Ntrk << ", efficiency: " << effTab.getEfficiency(Ntrk) << endl;
    }

    eventWeight = ntrkWeight;
    if (reweightEvent) ntp.setEventWeight(eventWeight);

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
        if (selectDeDx && !dedxSel(ntp.cand_dau_pT[1] * std::cosh(ntp.cand_dau_eta[1]),
                                   ntp.trk_dau_dEdx_dedxHarmonic2[1])) continue; // select proton
        bool passMVA = false;
        for (size_t i=0; i<methodNames.size(); i++) {
          const auto& methodName = methodNames.at(i);
          mvaValues[i] = reader->EvaluateMVA(methodName);
          passMVA = passMVA || (selectMVA && mvaValues[i] > std::stod(configs.getMVACutMins().at(i)));
          if (abs(ntp.cand_y)<1.) {
            hMassPtMVA[0][i]->Fill(ntp.cand_mass, ntp.cand_pT, mvaValues[i], eventWeight);
          } else if (abs(ntp.cand_y)<2.) {
            hMassPtMVA[1][i]->Fill(ntp.cand_mass, ntp.cand_pT, mvaValues[i], eventWeight);
          }
          hMassNtrkMVA[i]->Fill(ntp.cand_mass, ntp.cand_Ntrkoffline, mvaValues[i], eventWeight);
        }
        ntp.setMVAValues(mvaValues);
        if (abs(ntp.cand_y)<1.)
          hists.fillHists(ntp, eventWeight);
        if (selectMVA &&  !passMVA) continue;
        if (saveTree) ntp.fillNTuple();
        if (useWS) {
          NtrkPV.setVal( NtrkInFloat );
          // LambdaC kinematic info
          cand_mass.setVal( ntp.cand_mass );
          cand_pT  .setVal( ntp.cand_pT   );
          cand_phi .setVal( ntp.cand_phi  );
          cand_eta .setVal( ntp.cand_eta  );
          cand_y   .setVal( ntp.cand_y    );
          // LambdaC decay info
          cand_decayLength3D   .setVal( ntp.cand_decayLength3D );
          cand_decayLengthSig3D.setVal( ntp.cand_decayLength3D/ ntp.cand_decayLengthError3D );
          cand_cosAngle3D      .setVal( TMath::Cos(ntp.cand_angle3D) );
          // Ks info
          cand_dau0_mass.setVal( ntp.cand_dau_mass[0] );
          cand_dau0_pT  .setVal( ntp.cand_dau_pT[0]   );
          cand_dau0_eta .setVal( ntp.cand_dau_eta[0]  );
          cand_dau0_phi .setVal( ntp.cand_dau_phi[0]  );
          // proton info
          cand_dau1_pT  .setVal( ntp.cand_dau_pT[1]   );
          cand_dau1_eta .setVal( ntp.cand_dau_eta[1]  );
          cand_dau1_phi .setVal( ntp.cand_dau_phi[1]  );
          trk_dau1_dEdxRes.setVal( ntp.trk_dau_dEdxRes[1] );
          // trigger bits
          passHM.setVal( ntp.trigBit[2] );
          passMB.setVal( ntp.trigBit[4] );
          // filter bits
          passDz1p0.setVal( ntp.filterBit[4] );
          passGplus.setVal( ntp.filterBit[5] );
          // MVA
          MVA.setVal(ntp.cand_MVA[0]);
          if (reweightEvent) {
            wgtVar.setVal(eventWeight);
            ds.add(cols, eventWeight);
          } else
            ds.add(cols);
        }
      }
    }
  }
  // Save the output
  // outputFile.Write();
  hNtrkoffline.Write();
  hNtrkofflineDz1p0.Write();
  hNtrkofflineGplus.Write();
  if (saveTree) ntp.t->Write();
  for (const auto& vec : hMassPtMVA) {
    for (const auto& h : vec) h->Write();
  }
  for (const auto& h : hMassNtrkMVA) h->Write();
  hists.writeHists();

  std::cout << "==> Wrote root file: " << outputFile.GetName() << std::endl;

  outputFileWS.cd();
  if (useWS) {
    ws.import(ds);
    ws.Write();
    // ds.Write();
  }
  std::cout << "==> TMVAClassificationApplication is done!" << std::endl;

  delete reader;
  delete pp;
  return 0;
}
