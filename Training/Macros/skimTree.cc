#include <vector>
#include <list>
#include <map>
#include <set>
#include <tuple>

#include <memory>

#include <algorithm>
#include <climits>
#include <numeric>

#include "TChain.h"
#include "TFileCollection.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "THashList.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TMath.h"
#include "TString.h"
#include "TSystem.h"

#include "Utilities/TreeReader/ParticleTreeData.hxx"
#include "Utilities/Ana/Common.h"
#include "Utilities/Ana/TreeHelpers.h"
#include "Utilities/Ana/MyNTuple.h"

using std::cerr;
using std::cout;
using std::endl;

using std::vector;
using std::string;
using std::list;
using std::map;
using std::tuple;

using std::shared_ptr;

using PtEtaPhiM_t = ROOT::Math::PtEtaPhiMVector;

using MatchPairInfo =  tuple<size_t, size_t, double, double>;

struct Config
{
public:
  Config(const bool isMC): Config(-1, isMC) {}
  Config(const Long64_t n, const bool isMC): Config(n, isMC, 0.03, 0.1) {}
  Config(const Long64_t n, const bool isMC, const float dR, const float dPt):
    _matchCriterion(dR, dPt),
    _ntrkLow(0), _ntrkHigh(UShort_t(-1)),
    _nentries(n), _isMC(isMC),
    _saveMatchedOnly(1), _selectDeDx(0) {}
  TString GetInputList() const { return _inputList; }
  void SetInputList(const TString s) { _inputList = s; }
  TString GetTreeDir() const { return _treeDir; }
  void SetTreeDir(const TString s) { _treeDir = s; }
  TString GetPostfix() const { return _postfix; }
  void SetPostfix(const TString s) { _postfix = s; }
  TString GetOutDir() const { return _outDir; }
  void SetOutDir(const TString s) { _outDir = s; }
  Long64_t GetNEntries() const { return _nentries; }
  void SetNEntries(Long64_t n) { _nentries = n; }
  bool isMC() const { return _isMC; }
  bool flipEta() const { return _flipEta; }
  void SetFlipEta(const bool flip) { _flipEta = flip; }
  bool SaveMatchedOnly() const { return _saveMatchedOnly; }
  void SetSaveMatchedOnly(const bool save) { _saveMatchedOnly = save; }
  const MatchCriterion& GetMatchCriterion() const { return _matchCriterion; }
  const DeDxSelection&  GetDeDxSelection() const { return _dEdxSelection; }
  void SetDeDxSelection(const DeDxSelection& sel){ _dEdxSelection = sel; }
  bool SelectDeDx() const { return _selectDeDx; }
  void SetSelectDeDx (const bool sel) { _selectDeDx = sel; }
  int  GetTriggerIndex() const { return _triggerIndex; }
  int  GetFilterIndex()  const { return _filterIndex;  }
  void SetTriggerIndex(const int idx) { _triggerIndex = idx; }
  void SetFilterIndex(const int idx)  { _filterIndex  = idx; }
  void SetKeptBranchNames(const std::vector<TString> names)
  {
    _keptBranchNames = names;
  }
  std::vector<TString> GetKeptBranchNames() const
  { return _keptBranchNames; }

  UShort_t NtrkLow() const { return _ntrkLow; }
  UShort_t NtrkHigh() const { return _ntrkHigh; }
  void NtrkLow(unsigned short n) { _ntrkLow = n; }
  void NtrkHigh(unsigned short n) { _ntrkHigh = n; }

  void SetEffFileName(const std::string s) { _effFileName = s; }
  void SetEffGraphName(const std::string s) { _effGraphName = s; }
  void SetEffGraphType(const std::string s) { _effGraphType = s; }
  std::string GetEffFileName() const { return _effFileName; }
  std::string GetEffGraphName() const { return _effGraphName; }
  std::string GetEffGraphType() const { return _effGraphType; }

private:
  MatchCriterion _matchCriterion;
  DeDxSelection  _dEdxSelection;
  std::vector<TString> _keptBranchNames;
  TString _inputList;
  TString _treeDir;
  TString _postfix;
  TString _outDir;
  std::string _effFileName;
  std::string _effGraphName;
  std::string _effGraphType;
  Long64_t _nentries;
  int      _triggerIndex;
  int      _filterIndex;
  UShort_t _ntrkLow;
  UShort_t _ntrkHigh;
  bool     _isMC;
  bool     _flipEta;
  bool     _saveMatchedOnly;
  bool     _selectDeDx;
};

TString getPhaseSpaceString(const KineCut& kins)
{
  auto phase = TString::Format("pT%.1fto%.1f_", kins.pTMin, kins.pTMax);
  phase += kins.absYMin>0 ?
    TString::Format("yAbs%.1fto%.1f", kins.absYMin, kins.absYMax) :
    TString::Format("yAbs%.1f", kins.absYMax) ;
  phase += "_";
  phase += kins.absMassLw>0 ?
    TString::Format("absMassDiff%.3fto%.3f", kins.absMassLw, kins.absMassUp) :
    TString::Format("absMassDiff%.3f", kins.absMassUp) ;
  int point = phase.Index(".");
  while(point > 0) {
    phase.Replace(point, 1, "p");
    point = phase.Index(".");
  }
  return phase;
}

int skimTree(const Config& conf,
             Particle particle, KineCut kins)
{
  const auto inputList = conf.GetInputList();
  const auto treeDir = conf.GetTreeDir();
  const auto postfix = conf.GetPostfix();
  const auto outDir = conf.GetOutDir();
  auto nentries = conf.GetNEntries();
  const auto triggerIndex = conf.GetTriggerIndex();
  const auto filterIndex  = conf.GetFilterIndex();
  const auto isMC = conf.isMC();
  const auto flipEta = conf.flipEta();
  const auto saveMatchedOnly = conf.SaveMatchedOnly();
  const auto selectDeDx = conf.SelectDeDx();
  const auto keptBranches = conf.GetKeptBranchNames();
  const auto ntrkLow = conf.NtrkLow();
  const auto ntrkHigh = conf.NtrkHigh();
  DeDxSelection   dEdxSelection  = conf.GetDeDxSelection();
  MatchCriterion  matchCriterion = conf.GetMatchCriterion();

  TString basename(gSystem->BaseName(inputList));
  auto firstPos = basename.Index(".list");
  if (firstPos>0) {
    basename.Replace(firstPos, 5, "_");
  }
  if (basename[basename.Length()-1] != '_')
    basename.Append('_');
  firstPos = basename.Index(".");
  while (firstPos>0) {
    basename.Replace(firstPos, 1, "_");
    firstPos = basename.Index("__");
    while (firstPos>0) {
      basename.Replace(firstPos, 2, "_");
      firstPos = basename.Index("__");
    }
    firstPos = basename.Index(".");
  }

  basename += nentries > 0 ? Form("%s%lld_", treeDir.Data(), nentries) : (treeDir + "_AllEntries_");
  basename += getPhaseSpaceString(kins);
  if (postfix.Length()) basename += "_";
  basename += postfix + ".root";
  TString outName;
  if (outDir == "") outName = basename;
  else outName = outDir + "/" + basename;
  if (flipEta) {
    auto index = outName.Index(".root");
    if (index >0) {
      outName.Replace(index, 5, "_etaFlipped.root");
    }
  }

  TFile effFile(conf.GetEffFileName().c_str());
  TGraph* g(nullptr);
  if (effFile.IsOpen()) {
    if (conf.GetEffGraphType() == "TGraphAsymmErrors") {
      const auto effname = conf.GetEffGraphName();
      g = (TGraphAsymmErrors*) effFile.Get(effname.c_str());
    }
    else if (conf.GetEffGraphType() == "TGraphErrors") {
      const auto effname = conf.GetEffGraphName();
      g = (TGraphErrors*) effFile.Get(effname.c_str());
    }
  }

  const bool reweightEvent = g;

  EfficiencyTable<TGraph> effTab(g);

  TFile ofile(outName, "recreate");
  cout << "Created " << ofile.GetName() << endl;

  // new TTree
  ofile.mkdir(treeDir);
  ofile.cd(treeDir);

  TH1I hNtrkoffline("hNtrkoffline", "N_{trk}^{offline} for PV with highest N;N_{trk}^{offline};", 300, 0, 300);

  TFileCollection tf("tf", "", inputList);
  TChain t(treeDir+"/ParticleTree");
  t.AddFileInfoList(tf.GetList());
  ParticleTree* pp;
  if (isMC) pp = new ParticleTreeMC(&t);
  else pp = new ParticleTreeData(&t);
  ParticleTree& p = *pp;

  TTree tt("ParticleNTuple", "ParticleNTuple");
  MyNTuple ntp(&tt);
  ntp.isMC = isMC;
  ntp.flipEta = flipEta;
  unsigned short dauNGDau[] = {2, 0};
  ntp.setNDau(2, 2, dauNGDau);
  ntp.initNTuple();
  if (reweightEvent) { ntp.initWeightBranch(); }
  if (!keptBranches.empty()) ntp.pruneNTuple(keptBranches);
  Int_t ievent=0;
  ntp.t->Branch("eventID", &ievent);

  // if(nentries < 0) nentries = p.GetEntries();
  if(nentries < 0) nentries = p.GetEntriesFast();
  // cout << "Tree " << treeDir << "/ParticleTree in " << inputList
  //   << " has " << nentries << " entries." << endl;

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    auto jentry =  p.LoadTree(ientry);
    if (jentry < 0) break;
    p.GetEntry(ientry);

    // check pileup filter
    // if (!p.evtSel().at(4)) continue;
    if (!isMC) {
      const bool passEventSel = passEvent(p, filterIndex, triggerIndex);
      if (!passEventSel) continue;
    }

    if (!isMC) hNtrkoffline.Fill(p.Ntrkoffline());

    const auto recosize = p.cand_mass().size();

    const auto& pdgId = p.cand_pdgId();

    // reco-gen matching
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

    // loop over particles
    bool passNtrk = true;
    if (recosize > 0) {
      const auto ntrk = p.cand_Ntrkoffline().front();
      passNtrk =  ntrk < ntrkHigh && ntrk >= ntrkLow;
      if (!passNtrk && !isMC) continue;
      if (reweightEvent && !isMC) {
        double eventWeight = 1;
        eventWeight = effTab.getWeight(ntrk);
        ntp.setEventWeight(eventWeight);
      }
    }
    for (size_t ireco=0; ireco<recosize; ireco++) {
      // begin LambdaC
      if (pdgId[ireco] == std::abs(particle.id())) {
        // pass kinematic cuts
        if(!passKinematic(p.cand_pT().at(ireco),
                          p.cand_eta().at(ireco),
                          p.cand_y().at(ireco),
                          p.cand_mass().at(ireco),
                          kins)) continue;
        const auto dauIdx = p.cand_dauIdx().at(ireco);
        // 0 for Ks, 1 for proton
        const auto gDauIdx = p.cand_dauIdx().at(dauIdx.at(0));

        // check reco-gen match
        if (isMC) {
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

        // check dE/dx
        const auto trkIdxProton = p.cand_trkIdx().at(dauIdx.at(1));  // 1 for proton
        const auto dEdx = p.trk_dEdx_dedxHarmonic2().at(trkIdxProton);
        PtEtaPhi_t dau1P = getRecoDauP3(ireco, 1, p);
        const auto pProton = dau1P.R();
        if (selectDeDx && !dEdxSelection(pProton, dEdx)) { continue; }

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

        // retrieve other information
        if (!isMC) ntp.hltRecordLumi = p.hltRecordLumi().at(0); // 0 means high multiplicity 185, temporary
        ntp.retrieveTreeInfo(p, ireco);
        // fill this entry
        ntp.fillNTuple();
        ievent = (++ievent) % 8096;
      } // end LambdaC
    } // end looping over particles
  } // end loop
  delete pp;
  ofile.Write();

  return 0;
}
