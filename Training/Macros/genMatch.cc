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
#include "THashList.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TMath.h"
#include "TString.h"
#include "TSystem.h"

#include "TreeReader/ParticleTreeMC2.hxx"
#include "Ana/Common.h"
#include "Ana/TreeHelpers.h"

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

int genMatchFS(const TString& inputList, const TString& treeDir,
               const TString& postfix, const TString& outDir,
               Particle particle, KineCut kins,
               Long64_t nentries=-1, const bool saveMatchedOnly=false,
               TString type="")
{
  type.ToLower();
  bool sortByPt(false), sortByR(false);
  if (type == "dpt") sortByPt = true;
  else if (type == "dr") sortByR = true;
  else if (type != "") { cout << "Wrong input variable for sort type" << endl; return -1; }
  TString basename(gSystem->BaseName(inputList));
  const auto firstPos = basename.Index(".list");
  basename.Replace(firstPos, 5, "_");
  basename += nentries > 0 ? Form("%s%lld_", treeDir.Data(), nentries) : (treeDir + "_AllEntries_");
  basename += sortByPt ? "sortBydPt_" : "";
  basename += sortByR  ? "sortBydR_"  : "";
  basename += postfix + ".root";
  TString outName;
  if (outDir == "") outName = basename;
  else outName = outDir + "/" + basename;
  TFile ofile(outName, "recreate");
  cout << "Created " << ofile.GetName() << endl;

  // new TTree
  ofile.mkdir(treeDir);
  ofile.cd(treeDir);
  TH1I hNtrkoffline("hNtrkoffline", "N_{trk}^{offline} for PV with highest N;N_{trk}^{offline};", 300, 0, 300);
  TTree tt("ParticleNTuple", "ParticleNTuple");
  MyNTuple ntp(&tt);
  unsigned short dauNGDau[] = {2, 0};
  ntp.isMC = true;
  ntp.setNDau(2, 2, dauNGDau);
  ntp.initNTuple();
  Int_t ievent=0;
  ntp.t->Branch("eventID", &ievent);

  TFileCollection tf("tf", "", inputList);
  TChain t(treeDir+"/ParticleTree");
  t.AddFileInfoList(tf.GetList());
  ParticleTreeMC p(&t);

  if(nentries < 0) nentries = p.GetEntries();
  cout << "Tree " << treeDir << "/ParticleTree in " << inputList
    << " has " << nentries << " entries." << endl;

  MatchCriterion  matchCriterion(0.03, 0.1);

  for (Long64_t ientry=0; ientry<nentries; ientry++) {
    if (ientry % 20000 == 0) cout << "pass " << ientry << endl;
    p.GetEntry(ientry);
    // check pileup filter
    if (!p.evtSel().at(4)) continue;
    // hNtrkoffline.Fill(p.Ntrkoffline());

    // reco-gen matching
    const auto gensize = p.gen_mass().size();
    const auto recosize = p.cand_mass().size();

    const auto& pdgId = p.cand_pdgId();
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

    map<size_t, size_t> matchPairs;

    for (auto e : matchGenInfo) {
      const auto ireco = std::get<0>(e);
      const auto igen  = std::get<1>(e);
      if (!recoLock.at(ireco) && !genLock.at(igen)) {
        recoLock.at(ireco) = 1;
        genLock.at(igen)   = 1;
        matchPairs.insert(map<size_t, size_t>::value_type(ireco, igen));
      }
    }
    // loop over particles
    for (size_t ireco=0; ireco<recosize; ireco++) {
      // begin LambdaC
      if (pdgId[ireco] == std::abs(particle.id())) {
        const auto dauIdx = p.cand_dauIdx().at(ireco);
        // 0 for Ks, 1 for proton
        const auto gDauIdx = p.cand_dauIdx().at(dauIdx.at(0));
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

        // check it is not a Lambda/Anti-Lambda
        PtEtaPhiM_t gDau0P4 = getRecoDauP4(dauIdx.at(0), 0, p); // pi- from Ks
        PtEtaPhiM_t gDau1P4 = getRecoDauP4(dauIdx.at(0), 1, p); // pi+ from Ks
        if (gDau0P4.P() > gDau1P4.P()) {
          gDau0P4.SetM(0.938272081);
        } else {
          gDau1P4.SetM(0.938272081);
        }
        PtEtaPhiM_t total = gDau0P4 + gDau1P4;
        if ( std::abs(total.M() - 1.11568) < 0.02) { matchGEN = false; }
        // dielectron cross check
        gDau0P4.SetM(0.000511);
        gDau1P4.SetM(0.000511);
        PtEtaPhiM_t ee = gDau0P4 + gDau1P4;
        if ( ee.M() <0.2 ) { matchGEN = false; }

        ntp.cand_matchGEN = matchGEN;
        ntp.cand_isSwap = isSwap;
        if (saveMatchedOnly && !matchGEN) continue;
        // retrieve other information
        ntp.retrieveTreeInfo(p, ireco);
        // fill this entry
        ntp.fillNTuple();
        ievent = (++ievent) % 8096;
      } // end LambdaC
    } // end looping over particles
  } // end loop
  ofile.Write();
  return 0;
}
