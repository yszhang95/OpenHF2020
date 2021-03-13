#include <vector>
#include <list>
#include <set>
#include <map>
#include <memory>

#include "Math/GenVector/LorentzVector.h"
#include "Math/GenVector/PtEtaPhiM4D.h"
#include "Math/GenVector/VectorUtil.h"
#include "TMath.h"
#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TString.h"
#include "TSystem.h"

#include "TreeReader/ParticleTreeMC.hxx"

namespace MCMatch{
  using std::cout;
  using std::endl;
  using std::vector;
  using std::string;
  using std::map;
  using PtEtaPhiM_t = ROOT::Math::PtEtaPhiM4D<double>;
  using Hist1DMaps = std::map<std::string, std::unique_ptr<TH1D>>;
  using Hist2DMaps = std::map<std::string, std::unique_ptr<TH2D>>;
  using Hist3DMaps = std::map<std::string, std::unique_ptr<TH3D>>;

  class MatchCriterion {
    public:
      template <typename T>
        bool match (const T& reco, const T& gen);

      MatchCriterion (const float dR, const float dRelPt) :
        _deltaR(dR), _deltaRelPt(dRelPt) { };
      ~MatchCriterion() {}
      void SetDeltaRelPt(const float dRelPt) { _deltaRelPt = dRelPt; }
      void SetDeltaR(const float dR) { _deltaR = dR; }

    private:
      float _deltaR;
      float _deltaRelPt;
  };
  template <typename T>
    bool MatchCriterion::match (const T& reco, const T& gen)
    {
      const auto dR = ROOT::Math::VectorUtil::DeltaR(reco, gen);
      const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/gen.Pt();
      //const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/reco.Pt();
      return dR < _deltaR && dRelPt < _deltaRelPt;
    }

  int genLamInXiMass(const TString& inputList, const TString& treeDir, Long64_t nentries=-1, TString type="")
  {
    type.ToLower();
    bool sortByPt(false), sortByR(false);
    if (type == "dpt") sortByPt = true;
    else if (type == "dr") sortByR = true;
    else if (type != "") { cout << "Wrong input variable for sort type" << endl; return -1; }
    TString basename(gSystem->BaseName(inputList));
    const auto firstPos = basename.Index(".list");
    basename.Replace(firstPos, 5, "_");
    basename += Form("%s%lld_", treeDir.Data(), nentries);

    basename += sortByPt ? "sortBydPt_" : "";
    basename += sortByR  ? "sortBydR_"  : "";
    basename += "LamInXiMassHists.root";
    TFile ofile(basename, "recreate");
    cout << "Created " << ofile.GetName() << endl;

    std::unique_ptr<TH1D> hLamMass(new TH1D("hLamMass", "#Lambda/#bar{#Lambda},  S+B;M_{#pi^{+}#pi^{-}} (GeV);Events", 200, 1.06, 1.16));
    Hist1DMaps hLamMassMatch;
    hLamMassMatch["dR0.03"]= std::move(std::unique_ptr<TH1D>(
          new TH1D("hLamMassMatch0p03", "#Lambda/#bar{#Lambda}, matching p3, dR<0.03;M_{#pi^{+}#pi^{-}} (GeV);Events", 200, 1.06, 1.16)));
    hLamMassMatch["dR0.10"]= std::move(std::unique_ptr<TH1D>(
          new TH1D("hLamMassMatch0p10", "#Lambda/#bar{#Lambda}, matching p3, dR<0.10;M_{#pi^{+}#pi^{-}} (GeV);Events", 200, 1.06, 1.16)));
    TFileCollection tf("tf", "", inputList);
    TChain t(treeDir+"/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC p(&t);

    if(nentries < 0) nentries = p.GetEntries();
    cout << "Tree lambdacAna_mc/ParticleTree in " << inputList
      << " has " << nentries << " entries." << endl;;

    const float width = 0.005;
    //const float pdgMass = 0.497611;
    const float pdgMass = 1.115683;

    map<string, MatchCriterion>  matchCriterion;
    matchCriterion.insert(map<string, MatchCriterion>::value_type("dR0.03", MatchCriterion(0.03, 0.5)));
    matchCriterion.insert(map<string, MatchCriterion>::value_type("dR0.10", MatchCriterion(0.10, 0.5)));

    for (Long64_t ientry=0; ientry<nentries; ientry++) {
      if (ientry % 50000 == 0) cout << "pass " << ientry << endl;
      p.GetEntry(ientry);
      auto gensize = p.gen_mass().size();
      auto recosize = p.cand_mass().size();

      auto pdgId = p.cand_pdgId();
      auto gen_pdgId = p.gen_pdgId();

      for (size_t ireco=0; ireco<recosize; ireco++) {
        // begin Lam
        if (pdgId[ireco] == 3122) {
          // reco daughters
          map<string, bool> matchGEN;
          matchGEN.insert(map<string, bool>::value_type("dR0.03", false));
          matchGEN.insert(map<string, bool>::value_type("dR0.10", false));

          PtEtaPhiM_t recoLam (
                p.cand_pT()[ireco],
                p.cand_eta()[ireco],
                p.cand_phi()[ireco],
                p.cand_mass()[ireco]
                );
          // topo cut
          if (p.cand_decayLength3D()[ireco]/p.cand_decayLengthError3D()[ireco]<12) continue;

          // Lambda cross check
          ROOT::Math::PtEtaPhiM4D<double> dau0(
              p.cand_pTDau().at(ireco).at(0),
              p.cand_etaDau().at(ireco).at(0),
              p.cand_phiDau().at(ireco).at(0),
              p.cand_massDau().at(ireco).at(0)
              );
          ROOT::Math::PtEtaPhiM4D<double> dau1(
              p.cand_pTDau().at(ireco).at(1),
              p.cand_etaDau().at(ireco).at(1),
              p.cand_phiDau().at(ireco).at(1),
              p.cand_massDau().at(ireco).at(1)
              );
          // proton must have larger momentum
          if (dau0.P() > dau1.P()) continue;
          dau1.SetM(0.13957018);
          ROOT::Math::PxPyPzE4D<double> total(dau0.x()+dau1.x(),
              dau0.y()+dau1.y(), dau0.z()+dau1.z(), dau0.E() + dau1.E());
          if ( std::abs(total.M() - 0.497614)< 0.02) { continue; }
          // dielectron cross check
          dau0.SetM(0.000511);
          dau1.SetM(0.000511);
          ROOT::Math::PxPyPzE4D<double> ee(dau0.x()+dau1.x(),
              dau0.y()+dau1.y(), dau0.z()+dau1.z(), dau0.E() + dau1.E());
          if ( ee.M() <0.2 ) { /* cout << "reject one" << endl */; continue; }

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {
            if (abs(gen_pdgId[igen]) != 3122) continue;
            PtEtaPhiM_t genLam (
                p.gen_pT()[igen],
                p.gen_eta()[igen],
                p.gen_phi()[igen],
                p.gen_mass()[igen]
                );
            for (auto& e : matchCriterion) {
              if(!matchGEN.at(e.first)) matchGEN[e.first] = e.second.match(recoLam, genLam);
            }
            bool done = true;
            for (const auto& e : matchGEN) done = done && e.second;
            if(done) break;
          }
          for (const auto& e: matchGEN) {
            if (e.second) hLamMassMatch[e.first]->Fill(recoLam.M());
          }
          hLamMass->Fill(recoLam.M());
        } // end Lam
      }
    }
    ofile.cd();
    ofile.Write();
    return 0;
  }


  int genLamMass(const TString& inputList, const TString& treeDir, Long64_t nentries=-1, TString type="")
  {
    type.ToLower();
    bool sortByPt(false), sortByR(false);
    if (type == "dpt") sortByPt = true;
    else if (type == "dr") sortByR = true;
    else if (type != "") { cout << "Wrong input variable for sort type" << endl; return -1; }
    TString basename(gSystem->BaseName(inputList));
    const auto firstPos = basename.Index(".list");
    basename.Replace(firstPos, 5, "_");
    basename += Form("%s%lld_", treeDir.Data(), nentries);

    basename += sortByPt ? "sortBydPt_" : "";
    basename += sortByR  ? "sortBydR_"  : "";
    basename += "LamMassHists.root";
    TFile ofile(basename, "recreate");
    cout << "Created " << ofile.GetName() << endl;

    std::unique_ptr<TH1D> hLamMass(new TH1D("hLamMass", "#Lambda/#bar{#Lambda},  S+B;M_{#pi^{+}#pi^{-}} (GeV);Events", 200, 1.06, 1.16));
    Hist1DMaps hLamMassMatch;
    hLamMassMatch["dR0.03"]= std::move(std::unique_ptr<TH1D>(
          new TH1D("hLamMassMatch0p03", "#Lambda/#bar{#Lambda}, matching p3, dR<0.03;M_{#pi^{+}#pi^{-}} (GeV);Events", 200, 1.06, 1.16)));
    hLamMassMatch["dR0.10"]= std::move(std::unique_ptr<TH1D>(
          new TH1D("hLamMassMatch0p10", "#Lambda/#bar{#Lambda}, matching p3, dR<0.10;M_{#pi^{+}#pi^{-}} (GeV);Events", 200, 1.06, 1.16)));
    TFileCollection tf("tf", "", inputList);
    TChain t(treeDir+"/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC p(&t);

    if(nentries < 0) nentries = p.GetEntries();
    cout << "Tree lambdacAna_mc/ParticleTree in " << inputList
      << " has " << nentries << " entries." << endl;;

    const float width = 0.005;
    //const float pdgMass = 0.497611;
    const float pdgMass = 1.115683;

    map<string, MatchCriterion>  matchCriterion;
    matchCriterion.insert(map<string, MatchCriterion>::value_type("dR0.03", MatchCriterion(0.03, 0.5)));
    matchCriterion.insert(map<string, MatchCriterion>::value_type("dR0.10", MatchCriterion(0.10, 0.5)));

    for (Long64_t ientry=0; ientry<nentries; ientry++) {
      if (ientry % 50000 == 0) cout << "pass " << ientry << endl;
      p.GetEntry(ientry);
      auto gensize = p.gen_mass().size();
      auto recosize = p.cand_mass().size();

      auto pdgId = p.cand_pdgId();
      auto gen_pdgId = p.gen_pdgId();

      for (size_t ireco=0; ireco<recosize; ireco++) {
        // begin Lam
        if (pdgId[ireco] == 3122) {
          // reco daughters
          map<string, bool> matchGEN;
          matchGEN.insert(map<string, bool>::value_type("dR0.03", false));
          matchGEN.insert(map<string, bool>::value_type("dR0.10", false));

          PtEtaPhiM_t recoLam (
                p.cand_pT()[ireco],
                p.cand_eta()[ireco],
                p.cand_phi()[ireco],
                p.cand_mass()[ireco]
                );

          // Lambda cross check
          ROOT::Math::PtEtaPhiM4D<double> dau0(
              p.cand_pTDau().at(ireco).at(0),
              p.cand_etaDau().at(ireco).at(0),
              p.cand_phiDau().at(ireco).at(0),
              p.cand_massDau().at(ireco).at(0)
              );
          ROOT::Math::PtEtaPhiM4D<double> dau1(
              p.cand_pTDau().at(ireco).at(1),
              p.cand_etaDau().at(ireco).at(1),
              p.cand_phiDau().at(ireco).at(1),
              p.cand_massDau().at(ireco).at(1)
              );
          // proton must have larger momentum
          if (dau0.P() > dau1.P()) continue;
          dau1.SetM(0.13957018);
          ROOT::Math::PxPyPzE4D<double> total(dau0.x()+dau1.x(),
              dau0.y()+dau1.y(), dau0.z()+dau1.z(), dau0.E() + dau1.E());
          if ( std::abs(total.M() - 0.497614)< 0.02) { continue; }
          // dielectron cross check
          dau0.SetM(0.000511);
          dau1.SetM(0.000511);
          ROOT::Math::PxPyPzE4D<double> ee(dau0.x()+dau1.x(),
              dau0.y()+dau1.y(), dau0.z()+dau1.z(), dau0.E() + dau1.E());
          if ( ee.M() <0.2 ) { /* cout << "reject one" << endl */; continue; }

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {
            if (abs(gen_pdgId[igen]) != 3122) continue;
            PtEtaPhiM_t genLam (
                p.gen_pT()[igen],
                p.gen_eta()[igen],
                p.gen_phi()[igen],
                p.gen_mass()[igen]
                );
            for (auto& e : matchCriterion) {
              if(!matchGEN.at(e.first)) matchGEN[e.first] = e.second.match(recoLam, genLam);
            }
            bool done = true;
            for (const auto& e : matchGEN) done = done && e.second;
            if(done) break;
          }
          for (const auto& e: matchGEN) {
            if (e.second) hLamMassMatch[e.first]->Fill(recoLam.M());
          }
          hLamMass->Fill(recoLam.M());
        } // end Lam
      }
    }
    ofile.cd();
    ofile.Write();
    return 0;
  }

  int genKsMass(const TString& inputList, const TString& treeDir, Long64_t nentries=-1, TString type="")
  {
    type.ToLower();
    bool sortByPt(false), sortByR(false);
    if (type == "dpt") sortByPt = true;
    else if (type == "dr") sortByR = true;
    else if (type != "") { cout << "Wrong input variable for sort type" << endl; return -1; }
    TString basename(gSystem->BaseName(inputList));
    const auto firstPos = basename.Index(".list");
    basename.Replace(firstPos, 5, "_");
    basename += Form("%s%lld_", treeDir.Data(), nentries);

    basename += sortByPt ? "sortBydPt_" : "";
    basename += sortByR  ? "sortBydR_"  : "";
    basename += "KsMassHists.root";
    TFile ofile(basename, "recreate");
    cout << "Created " << ofile.GetName() << endl;

    std::unique_ptr<TH1D> hKsMass(new TH1D("hKsMass", "K_{S}^{0},  S+B;M_{#pi^{+}#pi^{-}} (GeV);Events", 200, 0.4, 0.6));
    Hist1DMaps hKsMassMatch;
    hKsMassMatch["dR0.03"]= std::move(std::unique_ptr<TH1D>(
          new TH1D("hKsMassMatch0p03", "K_{S}^{0}, matching p3, dR<0.03;M_{#pi^{+}#pi^{-}} (GeV);Events", 200, 0.4, 0.6)));
    hKsMassMatch["dR0.10"]= std::move(std::unique_ptr<TH1D>(
          new TH1D("hKsMassMatch0p10", "K_{S}^{0}, matching p3, dR<0.10;M_{#pi^{+}#pi^{-}} (GeV);Events", 200, 0.4, 0.6)));
    TFileCollection tf("tf", "", inputList);
    TChain t(treeDir+"/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC p(&t);

    if(nentries < 0) nentries = p.GetEntries();
    cout << "Tree lambdacAna_mc/ParticleTree in " << inputList
      << " has " << nentries << " entries." << endl;;

    const float width = 0.005;
    const float pdgMass = 0.497611;

    map<string, MatchCriterion>  matchCriterion;
    matchCriterion.insert(map<string, MatchCriterion>::value_type("dR0.03", MatchCriterion(0.03, 0.5)));
    matchCriterion.insert(map<string, MatchCriterion>::value_type("dR0.10", MatchCriterion(0.10, 0.5)));

    for (Long64_t ientry=0; ientry<nentries; ientry++) {
      if (ientry % 50000 == 0) cout << "pass " << ientry << endl;
      p.GetEntry(ientry);
      auto gensize = p.gen_mass().size();
      auto recosize = p.cand_mass().size();

      auto pdgId = p.cand_pdgId();
      auto gen_pdgId = p.gen_pdgId();

      for (size_t ireco=0; ireco<recosize; ireco++) {
        // begin Ks
        if (pdgId[ireco] == 310) {
          // reco daughters
          map<string, bool> matchGEN;
          matchGEN.insert(map<string, bool>::value_type("dR0.03", false));
          matchGEN.insert(map<string, bool>::value_type("dR0.10", false));

          PtEtaPhiM_t recoKs (
                p.cand_pT()[ireco],
                p.cand_eta()[ireco],
                p.cand_phi()[ireco],
                p.cand_mass()[ireco]
                );

          // Lambda cross check
          ROOT::Math::PtEtaPhiM4D<double> pi1(
              p.cand_pTDau().at(ireco).at(0),
              p.cand_etaDau().at(ireco).at(0),
              p.cand_phiDau().at(ireco).at(0),
              p.cand_massDau().at(ireco).at(0)
              );
          ROOT::Math::PtEtaPhiM4D<double> pi2(
              p.cand_pTDau().at(ireco).at(1),
              p.cand_etaDau().at(ireco).at(1),
              p.cand_phiDau().at(ireco).at(1),
              p.cand_massDau().at(ireco).at(1)
              );
          if (pi1.P() > pi2.P()) pi1.SetM(0.938272013);
          else pi2.SetM(0.938272013);
          ROOT::Math::PxPyPzE4D<double> total(pi1.x()+pi2.x(),
              pi1.y()+pi2.y(), pi1.z()+pi2.z(), pi1.E() + pi2.E());
          if ( std::abs(total.M() - 1.115683)< 0.01) { continue; }
          // dielectron cross check
          pi1.SetM(0.000511);
          pi2.SetM(0.000511);
          ROOT::Math::PxPyPzE4D<double> ee(pi1.x()+pi2.x(),
              pi1.y()+pi2.y(), pi1.z()+pi2.z(), pi1.E() + pi2.E());
          if ( ee.M() <0.2 ) { /* cout << "reject one" << endl */; continue; }

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {
            if (abs(gen_pdgId[igen]) != 310) continue;
            PtEtaPhiM_t genKs (
                p.gen_pT()[igen],
                p.gen_eta()[igen],
                p.gen_phi()[igen],
                p.gen_mass()[igen]
                );
            for (auto& e : matchCriterion) {
              if(!matchGEN.at(e.first)) matchGEN[e.first] = e.second.match(recoKs, genKs);
            }
            bool done = true;
            for (const auto& e : matchGEN) done = done && e.second;
            if(done) break;
          }
          for (const auto& e: matchGEN) {
            if (e.second) hKsMassMatch[e.first]->Fill(recoKs.M());
          }
          hKsMass->Fill(recoKs.M());
        } // end Ks
      }
    }
    ofile.cd();
    ofile.Write();
    return 0;
  }

  void genLcMultipleMatch(const TString& inputList)
  {
    using std::cout;
    using std::endl;
    using std::vector;
    using PtEtaPhiM_t = ROOT::Math::PtEtaPhiM4D<float>;

    MatchCriterion  matchCriterion(0.03, 0.5);

    TString basename(gSystem->BaseName(inputList));
    const auto firstPos = basename.Index(".list");
    basename.Replace(firstPos, 5, "_");

    TFile ofile(basename+"effHists.root", "recreate");
    std::unique_ptr<TH2D> hLcReco(new TH2D("hLcReco", ";pT (GeV);y;", 50, 2, 12, 4, -1, 1));
    std::unique_ptr<TH2D> hLcGen(new TH2D("hLcGen", ";pT (GeV);y;", 50, 2, 12, 4, -1, 1));

    std::unique_ptr<TH2D> hPReco(new TH2D("hPReco", ";pT (GeV);y;", 20, 0, 5, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hPGen(new TH2D("hPGen", ";pT (GeV);y;", 20, 0, 5, 10, -2.5, 2.5));

    std::unique_ptr<TH2D> hKsReco(new TH2D("hKsReco", ";pT (GeV);y;", 40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsGen(new TH2D("hKsGen", ";pT (GeV);y;", 40, 0, 8, 10, -2.5, 2.5));

    TFileCollection tf("tf", "", inputList);
    TChain t("lambdacAna_mc/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC p(&t);

    auto nentries = p.GetEntries();
    //auto nentries = 1000L;

    cout << "Tree lambdacAna_mc/ParticleTree in " << inputList
      << " has " << nentries << " entries." << endl;;

    for (Long64_t ientry=0; ientry<nentries; ientry++) {
      p.GetEntry(ientry);
      auto gensize = p.gen_mass().size();
      auto recosize = p.cand_mass().size();

      auto pdgId = p.cand_pdgId();
      auto charge = p.cand_charge();
      auto gen_pdgId = p.gen_pdgId();
      auto gen_charge = p.gen_charge();

      auto dauIdxEvt = p.cand_dauIdx();
      auto gen_dauIdxEvt = p.gen_dauIdx();

      for (size_t ireco=0; ireco<recosize; ireco++) {
        // begin Lc
        if (pdgId[ireco] == 4122) {
          // reco daughters
          bool matchGEN = false;
          auto dauIdx = dauIdxEvt.at(ireco);
          PtEtaPhiM_t recoKs(
              p.cand_pT()[dauIdx[0]],
              p.cand_eta()[dauIdx[0]],
              p.cand_phi()[dauIdx[0]],
              p.cand_mass()[dauIdx[0]]
              );
          PtEtaPhiM_t recoProton(
              p.cand_pT()[dauIdx[1]],
              p.cand_eta()[dauIdx[1]],
              p.cand_phi()[dauIdx[1]],
              p.cand_mass()[dauIdx[1]]
              );
          auto chargeKs = charge[dauIdx[0]];
          auto chargeProton = charge[dauIdx[1]];
          auto pdgIdKs = pdgId[dauIdx[0]];
          auto pdgIdProton = pdgId[dauIdx[1]];

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {
            auto gen_dauIdx = gen_dauIdxEvt.at(igen);
            if (abs(gen_pdgId[igen]) != 4122) continue;
            auto gen_chargeKs = gen_charge[gen_dauIdx[0]];
            auto gen_chargeProton = gen_charge[gen_dauIdx[1]];
            auto gen_pdgIdKs = abs(gen_pdgId[gen_dauIdx[0]]);
            auto gen_pdgIdProton = abs(gen_pdgId[gen_dauIdx[1]]);

            if (gen_pdgIdProton != pdgIdProton
                || gen_pdgIdKs != pdgIdKs
                || gen_chargeProton != chargeProton
                || gen_chargeKs != chargeKs
               ) continue;

            PtEtaPhiM_t genKs(
                p.gen_pT()[gen_dauIdx[0]],
                p.gen_eta()[gen_dauIdx[0]],
                p.gen_phi()[gen_dauIdx[0]],
                p.gen_mass()[gen_dauIdx[0]]
                );
            PtEtaPhiM_t genProton(
                p.gen_pT()[gen_dauIdx[1]],
                p.gen_eta()[gen_dauIdx[1]],
                p.gen_phi()[gen_dauIdx[1]],
                p.gen_mass()[gen_dauIdx[1]]
                );
            auto matchKs = matchCriterion.match(recoKs, genKs);
            auto matchProton = matchCriterion.match(recoProton, genProton);

            matchGEN = matchKs && matchProton;
            if (matchGEN) {
              hLcReco->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
              break;
            }
          }
        } // end Lc
        // begin Ks
        if (pdgId[ireco] == 310) {
          // reco daughters
          bool matchGEN = false;
          auto dauIdx = dauIdxEvt.at(ireco);
          PtEtaPhiM_t recoPi0(
              p.cand_pT()[dauIdx[0]],
              p.cand_eta()[dauIdx[0]],
              p.cand_phi()[dauIdx[0]],
              p.cand_mass()[dauIdx[0]]
              );
          PtEtaPhiM_t recoPi1(
              p.cand_pT()[dauIdx[1]],
              p.cand_eta()[dauIdx[1]],
              p.cand_phi()[dauIdx[1]],
              p.cand_mass()[dauIdx[1]]
              );
          auto chargePi0 = charge[dauIdx[0]];
          auto chargePi1 = charge[dauIdx[1]];
          auto pdgIdPi0 = pdgId[dauIdx[0]];
          auto pdgIdPi1 = pdgId[dauIdx[1]];

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {
            auto gen_dauIdx = gen_dauIdxEvt.at(igen);
            if (abs(gen_pdgId[igen]) != 310) continue;
            auto gen_chargePi0 = gen_charge[gen_dauIdx[0]];
            auto gen_chargePi1 = gen_charge[gen_dauIdx[1]];
            auto gen_pdgIdPi0 = abs(gen_pdgId[gen_dauIdx[0]]);
            auto gen_pdgIdPi1 = abs(gen_pdgId[gen_dauIdx[1]]);

            if (gen_pdgIdPi0 != 211
                || gen_pdgIdPi1 != 211
               ) continue;

            PtEtaPhiM_t genPi0(
                p.gen_pT()[gen_dauIdx[0]],
                p.gen_eta()[gen_dauIdx[0]],
                p.gen_phi()[gen_dauIdx[0]],
                p.gen_mass()[gen_dauIdx[0]]
                );
            PtEtaPhiM_t genPi1(
                p.gen_pT()[gen_dauIdx[1]],
                p.gen_eta()[gen_dauIdx[1]],
                p.gen_phi()[gen_dauIdx[1]],
                p.gen_mass()[gen_dauIdx[1]]
                );
            bool matchPi0(false); bool matchPi1(false);
            if (gen_chargePi0 == chargePi0) {
              matchPi0 = matchCriterion.match(recoPi0, genPi0);
              matchPi1 = matchCriterion.match(recoPi1, genPi1);
            } else {
              matchPi0 = matchCriterion.match(recoPi0, genPi1);
              matchPi1 = matchCriterion.match(recoPi1, genPi0);
            }
            matchGEN = matchPi0 && matchPi1;
            if (matchGEN) {
              hKsReco->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
              break;
            }
          }
        } // end Ks
        // begin Proton
        if (pdgId[ireco] == 2212) {
          bool matchGEN = false;
          PtEtaPhiM_t recoP(
              p.cand_pT()[ireco],
              p.cand_eta()[ireco],
              p.cand_phi()[ireco],
              p.cand_mass()[ireco]
              );
          auto chargeP = charge[ireco];
          //auto pdgIdP = pdgId[ireco];

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {
            if (abs(gen_pdgId[igen]) != 2212) continue;
            auto gen_chargeP = gen_charge[igen];
            //auto gen_pdgIdP = gen_pdgId[igen];

            if (gen_chargeP != chargeP) continue;

            PtEtaPhiM_t genP(
              p.gen_pT()[igen],
              p.gen_eta()[igen],
              p.gen_phi()[igen],
              p.gen_mass()[igen]
                );
            matchGEN = matchCriterion.match(recoP, genP);
            if (matchGEN) {
              hPReco->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
              break;
            }
          }
        } // end Proton
      }

      for (size_t igen=0; igen<gensize; igen++) {
        auto gen_dauIdx = gen_dauIdxEvt.at(igen);
        // begin Lc
        if (abs(gen_pdgId[igen]) == 4122) {
          //auto gen_chargeKs = gen_charge[gen_dauIdx[0]];
          //auto gen_chargeProton = gen_charge[gen_dauIdx[1]];
          auto gen_pdgIdKs = abs(gen_pdgId[gen_dauIdx[0]]);
          auto gen_pdgIdProton = abs(gen_pdgId[gen_dauIdx[1]]);
          auto gen_gdauIdx = gen_dauIdxEvt.at(gen_dauIdx[0]);
          auto gen_pdgIdxPi0 = abs(gen_pdgId[gen_gdauIdx[0]]);
          auto gen_pdgIdxPi1 = abs(gen_pdgId[gen_gdauIdx[1]]);
          if (gen_pdgIdKs == 310 && gen_pdgIdProton == 2212
              && gen_pdgIdxPi0 == 211 && gen_pdgIdxPi1 == 211) 
            hLcGen->Fill(p.gen_pT()[igen], p.gen_y()[igen]);
        } // end Lc
        // begin Ks
        if (abs(gen_pdgId[igen]) == 310) {
          //auto gen_chargeKs = gen_charge[gen_dauIdx[0]];
          //auto gen_chargeProton = gen_charge[gen_dauIdx[1]];
          auto gen_pdgIdPi0 = abs(gen_pdgId[gen_dauIdx[0]]);
          auto gen_pdgIdPi1 = abs(gen_pdgId[gen_dauIdx[1]]);
          auto gen_momIdx = p.gen_momIdx()[igen][0];
          //bool passLc = abs(p.gen_pdgId().at(gen_momIdx)) == 4122 &&
            //p.gen_pT()[gen_momIdx] > 0.9 && p.gen_pT()[gen_momIdx] < 6.1
            //&& abs(p.gen_y()[gen_momIdx]) < 1.1;
          bool passLc = abs(p.gen_pdgId().at(gen_momIdx)) == 4122;
          if (gen_pdgIdPi0 == 211 && gen_pdgIdPi1 == 211 && passLc)
            hKsGen->Fill(p.gen_pT()[igen], p.gen_y()[igen]);
        } // end Ks
        // begin Proton
        if (abs(gen_pdgId[igen]) == 2212) {
          auto gen_momIdx = p.gen_momIdx()[igen][0];
          //bool passLc = abs(p.gen_pdgId().at(gen_momIdx)) == 4122 &&
            //p.gen_pT()[gen_momIdx] > 0.9 && p.gen_pT()[gen_momIdx] < 6.1
            //&& abs(p.gen_y()[gen_momIdx]) < 1.1;
          bool passLc = abs(p.gen_pdgId().at(gen_momIdx)) == 4122;
          //auto gen_chargeP = gen_charge[igen];
          if (passLc) hPGen->Fill(p.gen_pT()[igen], p.gen_y()[igen]);
        } // end Proton
      }
    }
    ofile.cd();
    hLcGen->Write();
    hLcReco->Write();
    hKsGen->Write();
    hKsReco->Write();
    hPGen->Write();
    hPReco->Write();
    return;
  }

  void genKsMultipleMatch(const TString& inputList, const float dRKs=0.03, const float dRPi=0.03)
  {
    using std::cout;
    using std::endl;
    using std::vector;
    using PtEtaPhiM_t = ROOT::Math::PtEtaPhiM4D<float>;

    MatchCriterion  matchCriterion(dRKs, 0.5);
    MatchCriterion  matchDau0Criterion(dRPi, 0.5);
    MatchCriterion  matchDau1Criterion(dRPi, 0.5);

    TString basename(gSystem->BaseName(inputList));
    const auto firstPos = basename.Index(".list");
    basename.Replace(firstPos, 5, Form("_dRKs%f_dRPi%f_multipleKsMatch_", dRKs, dRPi));

    TFile ofile(basename+"effHists.root", "recreate");
    std::unique_ptr<TH2D> hKsRecoAll(new TH2D("hKsRecoAll", "All channels, Ks p3;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco(new TH2D("hKsReco", "#pi^{+}#pi^{-} channel, Ks p3;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsGen(new TH2D("hKsGen", "All channels;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco0Pi(new TH2D("hKsReco0Pi", "#pi^{+}#pi^{-} channel, 0Pi;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco1Pi(new TH2D("hKsReco1Pi", "#pi^{+}#pi^{-} channel, 1Pi;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco2Pi(new TH2D("hKsReco2Pi", "#pi^{+}#pi^{-} channel, 2Pi;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco0PiIndep(new TH2D("hKsReco0PiIndep",
          "#pi^{+}#pi^{-} channel, 0Pi Indep.;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco1PiIndep(new TH2D("hKsReco1PiIndep",
          "#pi^{+}#pi^{-} channel, 1Pi Indep.;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco2PiIndep(new TH2D("hKsReco2PiIndep",
          "#pi^{+}#pi^{-} channel, 2Pi Indep.;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));

    TFileCollection tf("tf", "", inputList);
    TChain t("ksAna_mc/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC p(&t);

    auto nentries = p.GetEntries();
    //auto nentries = 1000L;

    cout << "Tree ksAna_mc/ParticleTree in " << inputList
      << " has " << nentries << " entries." << endl;;

    for (Long64_t ientry=0; ientry<nentries; ientry++) {
      p.GetEntry(ientry);
      auto gensize = p.gen_mass().size();
      auto recosize = p.cand_mass().size();
      //if(gensize == 0 || recosize == 0) continue;

      auto pdgId = p.cand_pdgId();
      auto charge = p.cand_charge();
      auto gen_pdgId = p.gen_pdgId();
      auto gen_charge = p.gen_charge();

      auto dauIdxEvt = p.cand_dauIdx();
      auto gen_dauIdxEvt = p.gen_dauIdx();

      for (size_t ireco=0; ireco<recosize; ireco++) {
        // begin Ks
        if (pdgId[ireco] == 310) {
          // reco daughters
          bool matchGEN = false;
          bool breakMatchGEN = false;
          bool breakMatch0Pi = false;
          bool breakMatch1Pi = false;
          bool breakMatch2Pi = false;

          auto dauIdx = dauIdxEvt.at(ireco);
          PtEtaPhiM_t recoPi0(
              p.cand_pT()[dauIdx[0]],
              p.cand_eta()[dauIdx[0]],
              p.cand_phi()[dauIdx[0]],
              p.cand_mass()[dauIdx[0]]
              );
          PtEtaPhiM_t recoPi1(
              p.cand_pT()[dauIdx[1]],
              p.cand_eta()[dauIdx[1]],
              p.cand_phi()[dauIdx[1]],
              p.cand_mass()[dauIdx[1]]
              );
          auto chargePi0 = charge[dauIdx[0]];
          auto chargePi1 = charge[dauIdx[1]];
          auto pdgIdPi0 = pdgId[dauIdx[0]];
          auto pdgIdPi1 = pdgId[dauIdx[1]];
          //cout << "pdgIdPi0: " << pdgIdPi0 << endl;
          //cout << "pdgIdPi1: " << pdgIdPi1 << endl;
          //cout << "pdgId Pi0 " << chargePi0 * pdgIdPi0 << endl;
          //cout << "pdgId Pi1 " << chargePi1 * pdgIdPi1 << endl;
          PtEtaPhiM_t recoKs (
                p.cand_pT()[ireco],
                p.cand_eta()[ireco],
                p.cand_phi()[ireco],
                p.cand_mass()[ireco]
                );

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {
            auto gen_dauIdx = gen_dauIdxEvt.at(igen);
            if (abs(gen_pdgId[igen]) != 310) continue;
            auto gen_chargePi0 = gen_charge[gen_dauIdx[0]];
            auto gen_chargePi1 = gen_charge[gen_dauIdx[1]];
            auto gen_pdgIdPi0 = abs(gen_pdgId[gen_dauIdx[0]]);
            auto gen_pdgIdPi1 = abs(gen_pdgId[gen_dauIdx[1]]);

            /*
            if (gen_pdgIdPi0 != pdgIdPi0
                || gen_pdgIdPi1 != pdgIdPi1
               ) continue;
               */
            //cout << gen_pdgId[gen_dauIdx[0]] << endl;
            //cout << gen_pdgId[gen_dauIdx[1]] << endl;

            PtEtaPhiM_t genPi0(
                p.gen_pT()[gen_dauIdx[0]],
                p.gen_eta()[gen_dauIdx[0]],
                p.gen_phi()[gen_dauIdx[0]],
                p.gen_mass()[gen_dauIdx[0]]
                );
            PtEtaPhiM_t genPi1(
                p.gen_pT()[gen_dauIdx[1]],
                p.gen_eta()[gen_dauIdx[1]],
                p.gen_phi()[gen_dauIdx[1]],
                p.gen_mass()[gen_dauIdx[1]]
                );
            bool matchPi0(false); bool matchPi1(false);
            if (gen_chargePi0 == chargePi0) {
              matchPi0 = matchDau0Criterion.match(recoPi0, genPi0);
              matchPi1 = matchDau1Criterion.match(recoPi1, genPi1);
            } else {
              matchPi0 = matchDau0Criterion.match(recoPi0, genPi1);
              matchPi1 = matchDau1Criterion.match(recoPi1, genPi0);
            }
            //matchGEN = matchPi0 && matchPi1;
            PtEtaPhiM_t genKs (
                p.gen_pT()[igen],
                p.gen_eta()[igen],
                p.gen_phi()[igen],
                p.gen_mass()[igen]
                );
            matchGEN = matchCriterion.match(recoKs, genKs);
            
            if (gen_pdgIdPi0 == 211 && gen_pdgIdPi1 == 211) {
              if (matchPi0 && matchPi1 && !breakMatch2Pi) {
                hKsReco2PiIndep->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                breakMatch2Pi = true;
              } else if ( (matchPi0 || matchPi1) && !breakMatch1Pi) {
                hKsReco1PiIndep->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                breakMatch1Pi = true;
              } else if (!breakMatch0Pi){
                hKsReco0PiIndep->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                breakMatch0Pi = true;
              }
            }
            if (matchGEN && !breakMatchGEN) {
              hKsRecoAll->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
              if (gen_pdgIdPi0 == 211 && gen_pdgIdPi1 == 211) {
                hKsReco->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                if (matchPi0 && matchPi1) {
                  hKsReco2Pi->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                } else if ( matchPi0 || matchPi1) {
                  hKsReco1Pi->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                } else {
                  hKsReco0Pi->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                }
              }
              //break;
              breakMatchGEN = true;
            }
          }
        } // end Ks
      }

      for (size_t igen=0; igen<gensize; igen++) {
        auto gen_dauIdx = gen_dauIdxEvt.at(igen);
        // begin Ks
        if (abs(gen_pdgId[igen]) == 310) {
          //auto gen_chargeKs = gen_charge[gen_dauIdx[0]];
          //auto gen_chargeProton = gen_charge[gen_dauIdx[1]];
          auto gen_pdgIdPi0 = abs(gen_pdgId[gen_dauIdx[0]]);
          auto gen_pdgIdPi1 = abs(gen_pdgId[gen_dauIdx[1]]);
          //cout << gen_pdgId[igen] << endl;
          auto gen_momIdx = p.gen_momIdx()[igen][0];
          //cout << gen_momIdx << endl;
          //if (gen_pdgIdPi0 == 211 && gen_pdgIdPi1 == 211) {
            //hKsGen->Fill(p.gen_pT()[igen], p.gen_y()[igen]);
          //}
          hKsGen->Fill(p.gen_pT()[igen], p.gen_y()[igen]);
        } // end Ks
      }
    }
    ofile.cd();
    hKsGen->Write();
    hKsReco0Pi->Write();
    hKsReco1Pi->Write();
    hKsReco2Pi->Write();
    hKsReco->Write();
    hKsRecoAll->Write();
    hKsReco0PiIndep->Write();
    hKsReco1PiIndep->Write();
    hKsReco2PiIndep->Write();
    return;
  }

  void genKsSingleMatch(const TString& inputList, const float dRKs=0.03, const float dRPi=0.03)
  {
    using std::cout;
    using std::endl;
    using std::vector;
    using PtEtaPhiM_t = ROOT::Math::PtEtaPhiM4D<float>;

    MatchCriterion  matchCriterion(dRKs, 0.5);
    MatchCriterion  matchDau0Criterion(dRPi, 0.5);
    MatchCriterion  matchDau1Criterion(dRPi, 0.5);

    TString basename(gSystem->BaseName(inputList));
    const auto firstPos = basename.Index(".list");
    basename.Replace(firstPos, 5, Form("_dRKs%f_dRPi%f_singleKsMatch_", dRKs, dRPi));

    TFile ofile(basename+"effHists.root", "recreate");
    std::unique_ptr<TH2D> hKsRecoAll(new TH2D("hKsRecoAll", "All channels, Ks p3;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco(new TH2D("hKsReco", "#pi^{+}#pi^{-} channel, Ks p3;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsGen(new TH2D("hKsGen", "All channels;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco0Pi(new TH2D("hKsReco0Pi", "#pi^{+}#pi^{-} channel, 0Pi;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco1Pi(new TH2D("hKsReco1Pi", "#pi^{+}#pi^{-} channel, 1Pi;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco2Pi(new TH2D("hKsReco2Pi", "#pi^{+}#pi^{-} channel, 2Pi;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco0PiIndep(new TH2D("hKsReco0PiIndep",
          "#pi^{+}#pi^{-} channel, 0Pi Indep.;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco1PiIndep(new TH2D("hKsReco1PiIndep",
          "#pi^{+}#pi^{-} channel, 1Pi Indep.;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsReco2PiIndep(new TH2D("hKsReco2PiIndep",
          "#pi^{+}#pi^{-} channel, 2Pi Indep.;pT (GeV);y;",
          40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH1D> hKsReco2PiMass(new TH1D("hKsReco2PiMass", "#pi^{+}#pi^{-}, 2Pi;Mass;Events",
          90, 0.46, 0.52));
    std::unique_ptr<TH1D> hKsRecoMass(new TH1D("hKsRecoMass", "#pi^{+}#pi^{-}, Ks p4;Mass;Events",
          90, 0.46, 0.52));

    TFileCollection tf("tf", "", inputList);
    TChain t("ksAna_mc/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC p(&t);

    auto nentries = p.GetEntries();
    //auto nentries = 1000L;

    cout << "Tree ksAna_mc/ParticleTree in " << inputList
      << " has " << nentries << " entries." << endl;;

    for (Long64_t ientry=0; ientry<nentries; ientry++) {
      p.GetEntry(ientry);
      auto gensize = p.gen_mass().size();
      auto recosize = p.cand_mass().size();
      //if(gensize == 0 || recosize == 0) continue;

      auto pdgId = p.cand_pdgId();
      auto charge = p.cand_charge();
      auto gen_pdgId = p.gen_pdgId();
      auto gen_charge = p.gen_charge();

      auto dauIdxEvt = p.cand_dauIdx();
      auto gen_dauIdxEvt = p.gen_dauIdx();

      std::set<unsigned short> matched_Pi_indices;
      std::set<unsigned short> matched_Ks_indices;

      for (size_t ireco=0; ireco<recosize; ireco++) {
        // begin Ks
        if (pdgId[ireco] == 310) {
          // reco daughters
          bool matchGEN = false;
          bool breakMatchGEN = false;
          bool breakMatch0Pi = false;
          bool breakMatch1Pi = false;
          bool breakMatch2Pi = false;
          auto dauIdx = dauIdxEvt.at(ireco);
          PtEtaPhiM_t recoPi0(
              p.cand_pT()[dauIdx[0]],
              p.cand_eta()[dauIdx[0]],
              p.cand_phi()[dauIdx[0]],
              p.cand_mass()[dauIdx[0]]
              );
          PtEtaPhiM_t recoPi1(
              p.cand_pT()[dauIdx[1]],
              p.cand_eta()[dauIdx[1]],
              p.cand_phi()[dauIdx[1]],
              p.cand_mass()[dauIdx[1]]
              );
          auto chargePi0 = charge[dauIdx[0]];
          auto chargePi1 = charge[dauIdx[1]];
          auto pdgIdPi0 = pdgId[dauIdx[0]];
          auto pdgIdPi1 = pdgId[dauIdx[1]];
          //cout << "pdgIdPi0: " << pdgIdPi0 << endl;
          //cout << "pdgIdPi1: " << pdgIdPi1 << endl;
          //cout << "pdgId Pi0 " << chargePi0 * pdgIdPi0 << endl;
          //cout << "pdgId Pi1 " << chargePi1 * pdgIdPi1 << endl;
          PtEtaPhiM_t recoKs (
                p.cand_pT()[ireco],
                p.cand_eta()[ireco],
                p.cand_phi()[ireco],
                p.cand_mass()[ireco]
                );

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {
            auto gen_dauIdx = gen_dauIdxEvt.at(igen);
            if (abs(gen_pdgId[igen]) != 310) continue;
            auto gen_chargePi0 = gen_charge[gen_dauIdx[0]];
            auto gen_chargePi1 = gen_charge[gen_dauIdx[1]];
            auto gen_pdgIdPi0 = abs(gen_pdgId[gen_dauIdx[0]]);
            auto gen_pdgIdPi1 = abs(gen_pdgId[gen_dauIdx[1]]);

            /*
            if (gen_pdgIdPi0 != pdgIdPi0
                || gen_pdgIdPi1 != pdgIdPi1
               ) continue;
               */
            //cout << gen_pdgId[gen_dauIdx[0]] << endl;
            //cout << gen_pdgId[gen_dauIdx[1]] << endl;

            PtEtaPhiM_t genPi0(
                p.gen_pT()[gen_dauIdx[0]],
                p.gen_eta()[gen_dauIdx[0]],
                p.gen_phi()[gen_dauIdx[0]],
                p.gen_mass()[gen_dauIdx[0]]
                );
            PtEtaPhiM_t genPi1(
                p.gen_pT()[gen_dauIdx[1]],
                p.gen_eta()[gen_dauIdx[1]],
                p.gen_phi()[gen_dauIdx[1]],
                p.gen_mass()[gen_dauIdx[1]]
                );
            bool matchPi0(false); bool matchPi1(false);
            std::pair<std::set<unsigned short>::iterator, bool> successPi0, successPi1;
            if (gen_chargePi0 == chargePi0) {
              matchPi0 = matchDau0Criterion.match(recoPi0, genPi0);
              matchPi1 = matchDau1Criterion.match(recoPi1, genPi1);
              if (matchPi0) successPi0 = matched_Pi_indices.insert(gen_dauIdx[0]);
              if (matchPi1) successPi1 = matched_Pi_indices.insert(gen_dauIdx[1]);
            } else {
              matchPi0 = matchDau0Criterion.match(recoPi0, genPi1);
              matchPi1 = matchDau1Criterion.match(recoPi1, genPi0);
              if (matchPi0) successPi0 = matched_Pi_indices.insert(gen_dauIdx[1]);
              if (matchPi1) successPi1 = matched_Pi_indices.insert(gen_dauIdx[0]);
            }
            //matchGEN = matchPi0 && matchPi1;
            PtEtaPhiM_t genKs (
                p.gen_pT()[igen],
                p.gen_eta()[igen],
                p.gen_phi()[igen],
                p.gen_mass()[igen]
                );
            matchGEN = matchCriterion.match(recoKs, genKs);
            std::pair<std::set<unsigned short>::iterator, bool> success;
            if (matchGEN) success = matched_Ks_indices.insert(igen);
            if (!success.second) continue;
            if (matchGEN) {
              hKsRecoAll->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
              if (gen_pdgIdPi0 == 211 && gen_pdgIdPi1 == 211) {
                hKsReco->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                hKsRecoMass->Fill(p.cand_mass()[ireco]);
                if (matchPi0 && matchPi1) {
                  if (successPi0.second && successPi1.second)
                    hKsReco2Pi->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                    hKsReco2PiMass->Fill(p.cand_mass()[ireco]);
                } else if ( matchPi0 || matchPi1) {
                  if ( (matchPi0 && successPi0.second) || (matchPi1 && successPi1.second))
                    hKsReco1Pi->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                } else {
                  hKsReco0Pi->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
                }
              }
              break;
            }
          }
        } // end Ks
      }

      for (size_t igen=0; igen<gensize; igen++) {
        auto gen_dauIdx = gen_dauIdxEvt.at(igen);
        // begin Ks
        if (abs(gen_pdgId[igen]) == 310) {
          //auto gen_chargeKs = gen_charge[gen_dauIdx[0]];
          //auto gen_chargeProton = gen_charge[gen_dauIdx[1]];
          auto gen_pdgIdPi0 = abs(gen_pdgId[gen_dauIdx[0]]);
          auto gen_pdgIdPi1 = abs(gen_pdgId[gen_dauIdx[1]]);
          //cout << gen_pdgId[igen] << endl;
          auto gen_momIdx = p.gen_momIdx()[igen][0];
          //cout << gen_momIdx << endl;
          //if (gen_pdgIdPi0 == 211 && gen_pdgIdPi1 == 211) {
            //hKsGen->Fill(p.gen_pT()[igen], p.gen_y()[igen]);
          //}
          hKsGen->Fill(p.gen_pT()[igen], p.gen_y()[igen]);
        } // end Ks
      }
    }
    ofile.cd();
    hKsGen->Write();
    hKsReco0Pi->Write();
    hKsReco1Pi->Write();
    hKsReco2Pi->Write();
    hKsReco->Write();
    hKsRecoAll->Write();
    hKsReco2PiMass->Write();
    hKsRecoMass->Write();
    return;
  }

  void genStableKsMultipleMatch(const TString& inputList)
  {
    using std::cout;
    using std::endl;
    using std::vector;
    using PtEtaPhiM_t = ROOT::Math::PtEtaPhiM4D<float>;

    MatchCriterion  matchCriterion(0.03, 0.5);

    TString basename(gSystem->BaseName(inputList));
    const auto firstPos = basename.Index(".list");
    basename.Replace(firstPos, 5, "_");

    TFile ofile(basename+"effHists.root", "recreate");
    std::unique_ptr<TH2D> hKsReco(new TH2D("hKsReco", ";pT (GeV);y;", 40, 0, 8, 10, -2.5, 2.5));
    std::unique_ptr<TH2D> hKsGen(new TH2D("hKsGen", ";pT (GeV);y;", 40, 0, 8, 10, -2.5, 2.5));

    TFileCollection tf("tf", "", inputList);
    TChain t("ksAna_mc/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC p(&t);

    auto nentries = p.GetEntries();
    //auto nentries = 1000L;

    cout << "Tree ksAna_mc/ParticleTree in " << inputList
      << " has " << nentries << " entries." << endl;;

    for (Long64_t ientry=0; ientry<nentries; ientry++) {
      p.GetEntry(ientry);
      auto gensize = p.gen_mass().size();
      auto recosize = p.cand_mass().size();
      //if(gensize == 0 || recosize == 0) continue;

      auto pdgId = p.cand_pdgId();
      auto charge = p.cand_charge();
      auto gen_pdgId = p.gen_pdgId();
      auto gen_charge = p.gen_charge();

      for (size_t ireco=0; ireco<recosize; ireco++) {
        // begin Ks
        if (pdgId[ireco] == 310) {
          // reco daughters
          bool matchGEN = false;
          PtEtaPhiM_t p4_recoKs (
              p.cand_pT()[ireco],
              p.cand_eta()[ireco],
              p.cand_phi()[ireco],
              p.cand_mass()[ireco]
          );

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {
            if (abs(gen_pdgId[igen]) != 310) continue;
            PtEtaPhiM_t p4_genKs (
              p.gen_pT()[igen],
              p.gen_eta()[igen],
              p.gen_phi()[igen],
              p.gen_mass()[igen]
            );

            matchGEN = matchCriterion.match(p4_recoKs, p4_genKs);
            if (matchGEN) {
              hKsReco->Fill(p.cand_pT()[ireco], p.cand_y()[ireco]);
              break;
            }
          }
        } // end Ks
      }

      for (size_t igen=0; igen<gensize; igen++) {
        // begin Ks
        if (abs(gen_pdgId[igen]) == 310) {
            hKsGen->Fill(p.gen_pT()[igen], p.gen_y()[igen]);
        } // end Ks
      }
    }
    ofile.cd();
    hKsGen->Write();
    hKsReco->Write();
    return;
  }
};
