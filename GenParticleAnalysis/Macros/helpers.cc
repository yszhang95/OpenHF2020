#include <algorithm>
#include <limits>
#include <memory>

#include <list>
#include <map>
#include <set>
#include <vector>

#include "Math/GenVector/PxPyPzE4D.h"
#include "Math/GenVector/LorentzVector.h"
#include "Math/GenVector/PtEtaPhiM4D.h"
//#include "Math/GenVector/PtEtaPhiE4D.h"
#include "Math/GenVector/VectorUtil.h"
#include "Math/SVector.h"
#include "Math/SMatrix.h"
#include "TMath.h"
#include "TChain.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TString.h"
#include "TSystem.h"

#include "TreeReader/ParticleTreeMC2.hxx"

namespace MCMatch{
  using std::cout;
  using std::endl;
  using std::vector;
  using PtEtaPhiM_t = ROOT::Math::PtEtaPhiM4D<double>;
  using Hist1DMaps = std::map<std::string, std::unique_ptr<TH1D>>;
  using Hist2DMaps = std::map<std::string, std::unique_ptr<TH2D>>;
  using Hist3DMaps = std::map<std::string, std::unique_ptr<TH3D>>;

  class MatchCriterion {
    public:
      template <typename T>
        bool match(const T& reco, const T& gen);

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
    bool MatchCriterion::match(const T& reco, const T& gen)
    {
      const auto dR = ROOT::Math::VectorUtil::DeltaR(reco, gen);
      const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/gen.Pt();
      //const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/reco.Pt();
      return dR < _deltaR && dRelPt < _deltaRelPt;
    }

  double deltaR (const float eta1, const float eta2,
      const float phi1, const float phi2)
  { return 0; }

  void genKsMatchInfo(const TString& inputList, TString type, const bool useRaw=false)
  {
    type.ToLower();
    bool sortByPt, sortByR;
    if (type == "dpt") sortByPt = true;
    else if (type == "dr") sortByR = true;
    else { cout << "Wrong input variable for sort type" << endl; return; }

    TString basename(gSystem->BaseName(inputList));
    const auto firstPos = basename.Index(".list");
    basename.Replace(firstPos, 5, "_");

    basename += sortByPt ? "sortBydPt" : "";
    basename += sortByR  ? "sortBydR"  : "";
    basename += useRaw   ? "_KsInfoPrefit.root"  : "_KsInfoPostfit.root";
    TFile ofile(basename, "recreate");
    cout << "Created " << ofile.GetName() << endl;

    Hist3DMaps h3RecoGenPeak, h3RecoGenSide;

    h3RecoGenPeak["pi+"] = std::move(std::unique_ptr<TH3D>( new TH3D(
            "hPeakChi2vsdPtvsdR_PiP", "peak #pi^{+};#DeltaR;(p_{T}^{reco}-p_{T}^{gen})/p_{T}^{gen};#chi^{2}",
            100, 0, 1.0, 10, 0., 0.5, 20, 0., 10)));
    h3RecoGenPeak["pi-"] = std::move(std::unique_ptr<TH3D>( new TH3D(
            "hPeakChi2vsdPtvsdR_PiM", "peak #pi^{-};#DeltaR;(p_{T}^{reco}-p_{T}^{gen})/p_{T}^{gen};#chi^{2}",
            100, 0, 1.0, 10, 0., 0.5, 20, 0., 10)));
    h3RecoGenSide["pi+"] = std::move(std::unique_ptr<TH3D>( new TH3D(
            "hSideChi2vsdPtvsdR_PiP", "side #pi^{+};#DeltaR;(p_{T}^{reco}-p_{T}^{gen})/p_{T}^{gen};#chi^{2}",
            100, 0, 1.0, 10, 0., 0.5, 20, 0., 10)));
    h3RecoGenSide["pi-"] = std::move(std::unique_ptr<TH3D>( new TH3D(
            "hSideChi2vsdPtvsdR_PiM", "side #pi^{-};#DeltaR;(p_{T}^{reco}-p_{T}^{gen})/p_{T}^{gen};#chi^{2}",
            100, 0, 1.0, 10, 0., 0.5, 20, 0., 10)));

    std::unique_ptr<TH1D> hmass(new TH1D("hKsMass", "K_{S}^{0};M_{#pi^{+}#pi^{-}} (GeV);Entries", 100, 0.4, 0.6));

    TFileCollection tf("tf", "", inputList);
    TChain t("lambdacAna_mc/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC2 p(&t);

    auto nentries = p.GetEntries();
    cout << "Tree lambdacAna_mc/ParticleTree in " << inputList
      << " has " << nentries << " entries." << endl;;

    const float width = 0.005;
    const float pdgMass = 0.49367;

    for (Long64_t ientry=0; ientry<nentries; ientry++) {
      if (ientry % 50000 == 0) cout << "pass " << ientry << endl;
      p.GetEntry(ientry);
      auto gensize = p.gen_mass().size();
      auto recosize = p.cand_mass().size();

      auto pdgId = p.cand_pdgId();
      auto charge = p.cand_charge();
      auto gen_pdgId = p.gen_pdgId();
      auto gen_charge = p.gen_charge();

      auto dauIdxEvt = p.cand_dauIdx();
      auto gen_dauIdxEvt = p.gen_dauIdx();

      std::set<unsigned int> piInPeak;
      std::set<unsigned int> piInWidePeak;
      std::set<unsigned int> piInSide;
      for (size_t ireco=0; ireco<recosize; ireco++) {
        // begin Ks
        if(pdgId.at(ireco) != 310) continue;
        bool passDzSig = true;
        bool passDxySig = true;
        for (const auto dIdx : dauIdxEvt.at(ireco)) {
          passDzSig = passDzSig && std::abs(p.trk_zDCASignificance()[p.cand_trkIdx().at(dIdx)]) > 1.;
          passDxySig = passDxySig && std::abs(p.trk_xyDCASignificance()[p.cand_trkIdx().at(dIdx)]) > 1.;
        }
        if (! (passDzSig && passDxySig)) continue;

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
        if ( std::abs(total.M() - 1.115683)< 0.01) continue;

        const auto mass = p.cand_mass()[ireco];
        const bool inPeak = std::abs(mass - pdgMass) < 1 * width;
        const bool inWidePeak = std::abs(mass - pdgMass) < 2 * width;
        const bool inSide = std::abs(mass - pdgMass) > 5 * width;
        //  collect daughters in peak and side regions at first
        //  find the intersection and remove the intersection
        for (const auto idx : dauIdxEvt.at(ireco)) {
          if (inPeak) piInPeak.insert(idx);
          if (inWidePeak) piInWidePeak.insert(idx);
          if (inSide) piInSide.insert(idx);
        }
        hmass->Fill(mass);
        // end Ks
      }
      // std::set is a sorted container
      // find the intersection
      std::vector<unsigned int> idx_intersection;
      std::set_intersection(piInWidePeak.begin(), piInWidePeak.end(),
          piInSide.begin(), piInSide.end(),
          std::back_inserter(idx_intersection)
          );
      // remove the intersection
      for (const auto e : idx_intersection) {
        //piInPeak.erase(e);
        piInSide.erase(e);
      }

      // find the closet gen information
      std::map<size_t, size_t> piInfoInPeak, piInfoInSide;
      //   begin peak
      for (const auto idx : piInPeak) {
        // reco should be done with fit values
        // need to find its mom
        PtEtaPhiM_t reco(
            p.cand_pT().at(idx),
            p.cand_eta().at(idx),
            p.cand_phi().at(idx),
            p.cand_mass().at(idx)
            );
        if (!useRaw) {
          std::vector<unsigned int> momIdxVec;
          std::copy_if(p.cand_momIdx().at(idx).begin(), p.cand_momIdx().at(idx).end(),
              std::back_inserter(momIdxVec),
              [&](unsigned int i) -> bool { return std::abs(p.cand_mass().at(i) - pdgMass) < 1 * width; }
              );
          const auto momIdx = *std::min_element(momIdxVec.begin(), momIdxVec.end(),
              [&](unsigned int i, unsigned int j)->bool{ return p.cand_vtxProb().at(i) < p.cand_vtxProb().at(j); }
              );
          if (!dauIdxEvt.at(momIdx).size()) { std::cerr << "dauIdxEvt.at(momIdx): no daughter found!" << endl; return; }
          size_t iDau=0;
          for ( ; iDau<dauIdxEvt.at(momIdx).size(); ++iDau) {
            if (dauIdxEvt.at(momIdx).at(iDau) == idx)
              break;
          }
          reco = PtEtaPhiM_t (
              p.cand_pTDau().at(momIdx).at(iDau),
              p.cand_etaDau().at(momIdx).at(iDau),
              p.cand_phiDau().at(momIdx).at(iDau),
              p.cand_massDau().at(momIdx).at(iDau)
              );
        }
        const auto recoCharge = charge.at(idx);

        float dRMin = std::numeric_limits<float>::max();
        float dRelPtMin = std::numeric_limits<float>::max();
        piInfoInPeak[idx] = size_t(-1);
        for (size_t genIdx=0; genIdx<gensize; ++genIdx) {
          // check stable or not
          if (p.gen_status().at(genIdx) != 1) continue;
          // check charge
          if (recoCharge != gen_charge.at(genIdx)) continue;
          // check mother ID
          const auto gen_momIdx = p.gen_momIdx().at(genIdx).at(0);
          if (std::abs(p.gen_pdgId().at(gen_momIdx)) != 310) continue;

          PtEtaPhiM_t gen(
              p.gen_pT()[genIdx],
              p.gen_eta()[genIdx],
              p.gen_phi()[genIdx],
              p.gen_mass()[genIdx]
              );
          const auto dR = ROOT::Math::VectorUtil::DeltaR(reco, gen);
          const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/gen.Pt();
          // closet particle by dR or dPt
          if (sortByR && dR < dRMin) { piInfoInPeak[idx] = genIdx; dRMin = dR; }
          if (sortByPt && dRelPt < dRelPtMin) { piInfoInPeak[idx] = genIdx; dRelPtMin = dRelPt; }
        }
      }
      //   end peak
      //   begin side
      for (const auto idx : piInSide) {
        // reco should be done with fit values
        // need to find its mother particle
        PtEtaPhiM_t reco(
            p.cand_pT().at(idx),
            p.cand_eta().at(idx),
            p.cand_phi().at(idx),
            p.cand_mass().at(idx)
            );
        if (!useRaw) {
          std::vector<unsigned int> momIdxVec;
          std::copy_if(p.cand_momIdx().at(idx).begin(), p.cand_momIdx().at(idx).end(),
              std::back_inserter(momIdxVec),
              [&](unsigned int i) -> bool { return std::abs(p.cand_mass().at(i) - pdgMass) > 5 * width; }
              );
          const auto momIdx = *std::min_element(momIdxVec.begin(), momIdxVec.end(),
              [&](unsigned int i, unsigned int j)->bool{ return p.cand_vtxProb().at(i) < p.cand_vtxProb().at(j); }
              );
          if (!dauIdxEvt.at(momIdx).size()) { std::cerr << "dauIdxEvt.at(momIdx): no daughter found!" << endl; return; }
          size_t iDau=0;
          for ( ; iDau<dauIdxEvt.at(momIdx).size(); ++iDau) {
            if (dauIdxEvt.at(momIdx).at(iDau) == idx)
              break;
          }
          reco = PtEtaPhiM_t (
              p.cand_pTDau().at(momIdx).at(iDau),
              p.cand_etaDau().at(momIdx).at(iDau),
              p.cand_phiDau().at(momIdx).at(iDau),
              p.cand_massDau().at(momIdx).at(iDau)
              );
        }
        const auto recoCharge = charge.at(idx);

        float dRMin = std::numeric_limits<float>::max();
        float dRelPtMin = std::numeric_limits<float>::max();
        piInfoInSide[idx] = size_t(-1);
        for (size_t genIdx=0; genIdx<gensize; ++genIdx) {
          // check stable or not
          if (p.gen_status().at(genIdx) != 1) continue;
          //cout << (int)p.gen_status().at(genIdx) << endl;
          // check charge
          if (recoCharge != gen_charge.at(genIdx)) continue;
          // check mother ID
          const auto gen_momIdx = p.gen_momIdx().at(genIdx).at(0);
          if (std::abs(p.gen_pdgId().at(gen_momIdx)) != 310) continue;

          PtEtaPhiM_t gen(
              p.gen_pT()[genIdx],
              p.gen_eta()[genIdx],
              p.gen_phi()[genIdx],
              p.gen_mass()[genIdx]
              );
          const auto dR = ROOT::Math::VectorUtil::DeltaR(reco, gen);
          const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/gen.Pt();
          // closet particle by dR or dPt
          if (sortByR && dR < dRMin) { piInfoInPeak[idx] = genIdx; dRMin = dR; }
          if (sortByPt && dRelPt < dRelPtMin) { piInfoInPeak[idx] = genIdx; dRelPtMin = dRelPt; }
        }
      }
      //   end side
      // compute dynamic info
      // begin peak
      for (const auto& e : piInfoInPeak) {
        const auto idx = e.first;
        const auto genIdx = e.second;
        if (genIdx == size_t(-1)) { /* std::cerr << "piInfoInPeak: genIdx == size_t(-1), no PiPi found" << endl;*/ continue; }
        PtEtaPhiM_t reco(
            p.cand_pT().at(idx),
            p.cand_eta().at(idx),
            p.cand_phi().at(idx),
            p.cand_mass().at(idx)
            );
        if (!useRaw) {
          std::vector<unsigned int> momIdxVec;
          std::copy_if(p.cand_momIdx().at(idx).begin(), p.cand_momIdx().at(idx).end(),
              std::back_inserter(momIdxVec),
              [&](unsigned int i) -> bool { return std::abs(p.cand_mass().at(i) - pdgMass) < 1 * width; }
              );
          const auto momIdx = *std::min_element(momIdxVec.begin(), momIdxVec.end(),
              [&](unsigned int i, unsigned int j)->bool{ return p.cand_vtxProb().at(i) < p.cand_vtxProb().at(j); }
              );
          if (!dauIdxEvt.at(momIdx).size()) { std::cerr << "dauIdxEvt.at(momIdx): no daughter found!" << endl; return; }
          size_t iDau=0;
          bool found = false;
          for ( ; iDau<dauIdxEvt.at(momIdx).size(); ++iDau) {
            if (dauIdxEvt.at(momIdx).at(iDau) == idx) {
              found = true;
              break;
            }
          }
          if (!found) continue;
          reco = PtEtaPhiM_t (
              p.cand_pTDau().at(momIdx).at(iDau),
              p.cand_etaDau().at(momIdx).at(iDau),
              p.cand_phiDau().at(momIdx).at(iDau),
              p.cand_massDau().at(momIdx).at(iDau)
              );
        }
        PtEtaPhiM_t gen(
            p.gen_pT()[genIdx],
            p.gen_eta()[genIdx],
            p.gen_phi()[genIdx],
            p.gen_mass()[genIdx]
            );
        const auto dR = ROOT::Math::VectorUtil::DeltaR(reco, gen);
        const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/gen.Pt();

        const float* trkPars = &p.trk_trackParameters().at(p.cand_trkIdx().at(idx)).at(0);
        const auto recoParameters = ROOT::Math::SVector<float, 5>(trkPars, 5);
        auto errorMatrix    = ROOT::Math::SMatrix<float, 5, 5, ROOT::Math::MatRepSym<float, 5>>
              (p.trk_trackCovariance().at(p.cand_trkIdx().at(idx)).cbegin(),
               p.trk_trackCovariance().at(p.cand_trkIdx().at(idx)).cend());
        errorMatrix.Invert();
        const float* genTrkPars = &p.gen_trackParameters().at(genIdx).at(0);
        const auto genParameters = ROOT::Math::SVector<float, 5>(genTrkPars, 5);
        // difference
        const auto diffParameters = genParameters - recoParameters;
        const auto chi2 = ROOT::Math::Dot(diffParameters * errorMatrix, diffParameters)/5.;
        if (charge.at(idx) ==  1) h3RecoGenPeak["pi+"]->Fill(dR, dRelPt, chi2);
        if (charge.at(idx) == -1) h3RecoGenPeak["pi-"]->Fill(dR, dRelPt, chi2);
      }
      // end peak
      // begin side
      for (const auto& e : piInfoInSide) {
        const auto idx = e.first;
        const auto genIdx = e.second;
        if (genIdx == size_t(-1)) { /*std::cerr << "piInfoInSide: genIdx == size_t(-1)" << endl;*/ continue; }
        PtEtaPhiM_t reco(
            p.cand_pT().at(idx),
            p.cand_eta().at(idx),
            p.cand_phi().at(idx),
            p.cand_mass().at(idx)
            );
        if (!useRaw) {
          std::vector<unsigned int> momIdxVec;
          std::copy_if(p.cand_momIdx().at(idx).begin(), p.cand_momIdx().at(idx).end(),
              std::back_inserter(momIdxVec),
              [&](unsigned int i) -> bool { return std::abs(p.cand_mass().at(i) - pdgMass) > 5 * width; }
              );
          const auto momIdx = *std::min_element(momIdxVec.begin(), momIdxVec.end(),
              [&](unsigned int i, unsigned int j)->bool{ return p.cand_vtxProb().at(i) < p.cand_vtxProb().at(j); }
              );
          if (!dauIdxEvt.at(momIdx).size()) { std::cerr << "dauIdxEvt.at(momIdx): no daughter found!" << endl; return; }
          size_t iDau=0;
          for ( ; iDau<dauIdxEvt.at(momIdx).size(); ++iDau) {
            if (dauIdxEvt.at(momIdx).at(iDau) == idx)
              break;
          }
          reco = PtEtaPhiM_t (
              p.cand_pTDau().at(momIdx).at(iDau),
              p.cand_etaDau().at(momIdx).at(iDau),
              p.cand_phiDau().at(momIdx).at(iDau),
              p.cand_massDau().at(momIdx).at(iDau)
              );
        }
        PtEtaPhiM_t gen(
            p.gen_pT()[genIdx],
            p.gen_eta()[genIdx],
            p.gen_phi()[genIdx],
            p.gen_mass()[genIdx]
            );
        const auto dR = ROOT::Math::VectorUtil::DeltaR(reco, gen);
        const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/gen.Pt();

        const float* trkPars = &p.trk_trackParameters().at(p.cand_trkIdx().at(idx)).at(0);
        const auto recoParameters = ROOT::Math::SVector<float, 5>(trkPars, 5);
        auto errorMatrix    = ROOT::Math::SMatrix<float, 5, 5, ROOT::Math::MatRepSym<float, 5>>
              (p.trk_trackCovariance().at(p.cand_trkIdx().at(idx)).cbegin(),
               p.trk_trackCovariance().at(p.cand_trkIdx().at(idx)).cend());
        errorMatrix.Invert();
        const float* genTrkPars = &p.gen_trackParameters().at(genIdx).at(0);
        const auto genParameters = ROOT::Math::SVector<float, 5>(genTrkPars, 5);
        // difference
        const auto diffParameters = genParameters - recoParameters;
        const auto chi2 = ROOT::Math::Dot(diffParameters * errorMatrix, diffParameters)/5.;
        if (charge.at(idx) ==  1) h3RecoGenSide["pi+"]->Fill(dR, dRelPt, chi2);
        if (charge.at(idx) == -1) h3RecoGenSide["pi-"]->Fill(dR, dRelPt, chi2);
      }
      // end side
    }
    ofile.cd();
    for (const auto& e : h3RecoGenPeak) e.second->Write();
    for (const auto& e : h3RecoGenSide) e.second->Write();
    hmass->Write();
  }

  void genLcMatchInfo(const TString& inputList)
  {

    MatchCriterion  matchCriterion(0.03, 0.5);
    TString basename(gSystem->BaseName(inputList));
    const auto firstPos = basename.Index(".list");
    basename.Replace(firstPos, 5, "_");
    cout << "Will create " << basename+"effHists.root" << endl;
    TFile ofile(basename+"effHists.root", "recreate");
    Hist2DMaps matchedLcChi2, matchedLcdPt, peakLcChi2, peakLcdPt, sideLcChi2, sideLcdPt;

    matchedLcChi2["proton"] = std::move(std::unique_ptr<TH2D> (new TH2D("hmatchLc_proton_chi2vsdR", "match Lc p3;dR;chi2;Entries",
            10, 0., 0.1, 32, 0., 8 )));
    matchedLcChi2["pi+"] = std::move(std::unique_ptr<TH2D> (new TH2D("hmatchLc_pip_chi2vsdR", "match Lc p3;dR;chi2;Entries",
            10, 0., 0.1, 32, 0., 8 )));
    matchedLcChi2["pi-"] = std::move(std::unique_ptr<TH2D> (new TH2D("hmatchLc_pim_chi2vsdR", "match Lc p3;dR;chi2;Entries",
            10, 0., 0.1, 32, 0., 8 )));
    matchedLcdPt["proton"] = std::move(std::unique_ptr<TH2D> (new TH2D("hmatchLc_proton_dPtvsdR", "match Lc p3;dR;dRelPt;Entries",
            10, 0., 0.1, 20, 0., 1.0 )));
    matchedLcdPt["pi+"] = std::move(std::unique_ptr<TH2D> (new TH2D("hmatchLc_pip_dPtvsdR", "match Lc p3;dR;dRelPt;Entries",
            10, 0., 0.1, 20, 0., 1.0 )));
    matchedLcdPt["pi-"] = std::move(std::unique_ptr<TH2D> (new TH2D("hmatchLc_pim_dPtvsdR", "match Lc p3;dR;dRelPt;Entries",
            10, 0., 0.1, 32, 0., 1.0 )));

    peakLcChi2["proton"] = std::move(std::unique_ptr<TH2D> (new TH2D("hpeakLc_proton_chi2vsdR", "match FS, Lc peak;dR;chi2;Entries",
            10, 0., 0.1, 32, 0., 8 )));
    peakLcChi2["pi+"] = std::move(std::unique_ptr<TH2D> (new TH2D("hpeakLc_pip_chi2vsdR", "match FS, Lc peak;dR;chi2;Entries",
            10, 0., 0.1, 32, 0., 8 )));
    peakLcChi2["pi-"] = std::move(std::unique_ptr<TH2D> (new TH2D("hpeakLc_pim_chi2vsdR", "match FS, Lc peak;dR;chi2;Entries",
            10, 0., 0.1, 32, 0., 8 )));
    peakLcdPt["proton"] = std::move(std::unique_ptr<TH2D> (new TH2D("hpeakLc_proton_dPtvsdR", "match FS, Lc peak;dR;dRelPt;Entries",
            10, 0., 0.1, 20, 0., 1.0 )));
    peakLcdPt["pi+"] = std::move(std::unique_ptr<TH2D> (new TH2D("hpeakLc_pip_dPtvsdR", "match FS, Lc peak;dR;dRelPt;Entries",
            10, 0., 0.1, 20, 0., 1.0 )));
    peakLcdPt["pi-"] = std::move(std::unique_ptr<TH2D> (new TH2D("hpeakLc_pim_dPtvsdR", "match FS, Lc peak;dR;dRelPt;Entries",
            10, 0., 0.1, 32, 0., 1.0 )));

    sideLcChi2["proton"] = std::move(std::unique_ptr<TH2D> (new TH2D("hsideLc_proton_chi2vsdR", "match FS, Lc side;dR;chi2;Entries",
            10, 0., 0.1, 32, 0., 8 )));
    sideLcChi2["pi+"] = std::move(std::unique_ptr<TH2D> (new TH2D("hsideLc_pip_chi2vsdR", "match FS, Lc side;dR;chi2;Entries",
            10, 0., 0.1, 32, 0., 8 )));
    sideLcChi2["pi-"] = std::move(std::unique_ptr<TH2D> (new TH2D("hsideLc_pim_chi2vsdR", "match FS, Lc side;dR;chi2;Entries",
            10, 0., 0.1, 32, 0., 8 )));
    sideLcdPt["proton"] = std::move(std::unique_ptr<TH2D> (new TH2D("hsideLc_proton_dPtvsdR", "match FS, Lc side;dR;dRelPt;Entries",
            10, 0., 0.1, 20, 0., 1.0 )));
    sideLcdPt["pi+"] = std::move(std::unique_ptr<TH2D> (new TH2D("hsideLc_pip_dPtvsdR", "match FS, Lc side;dR;dRelPt;Entries",
            10, 0., 0.1, 20, 0., 1.0 )));
    sideLcdPt["pi-"] = std::move(std::unique_ptr<TH2D> (new TH2D("hsideLc_pim_dPtvsdR", "match FS, Lc side;dR;dRelPt;Entries",
            10, 0., 0.1, 32, 0., 1.0 )));

    std::map<std::string, std::unique_ptr<TH1D>> matchedLcMass;
    matchedLcMass["matchLcP3"] = std::move( std::unique_ptr<TH1D>(new
          TH1D("hLcMassMatchLcP3", "Match Lc p3;Mass_{K_{S}p} (GeV);Entries", 100, 2.0, 2.5) ) );
    matchedLcMass["matchFSP3"] = std::move( std::unique_ptr<TH1D>(new
          TH1D("hLcMassMatchFSP3", "Match FS p3;Mass_{K_{S}p} (GeV);Entries", 100, 2.0, 2.5) ) );

    TFileCollection tf("tf", "", inputList);
    TChain t("lambdacAna_mc/ParticleTree");
    t.AddFileInfoList(tf.GetList());
    ParticleTreeMC2 p(&t);

    auto nentries = p.GetEntries();
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
          // reco indices and p4 
          bool matchGEN = false;
          std::map<std::string, unsigned int> recoIdx;
          std::map<std::string, PtEtaPhiM_t> recoP4;
          recoP4.insert(
              std::map<std::string, PtEtaPhiM_t>::value_type(
              "Lc", PtEtaPhiM_t(
              p.cand_pT()[ireco],
              p.cand_eta()[ireco],
              p.cand_phi()[ireco],
              p.cand_mass()[ireco]
              )));

          auto dauIdx = dauIdxEvt.at(ireco);
          recoP4.insert(
              std::map<std::string, PtEtaPhiM_t>::value_type(
              "Ks", PtEtaPhiM_t (
              p.cand_pT()[dauIdx[0]],
              p.cand_eta()[dauIdx[0]],
              p.cand_phi()[dauIdx[0]],
              p.cand_mass()[dauIdx[0]]
              )));
          recoP4.insert(
              std::map<std::string, PtEtaPhiM_t>::value_type(
              "proton", PtEtaPhiM_t (
              p.cand_pT()[dauIdx[1]],
              p.cand_eta()[dauIdx[1]],
              p.cand_phi()[dauIdx[1]],
              p.cand_mass()[dauIdx[1]]
              )));

          auto chargeKs = charge[dauIdx[0]];
          auto chargeProton = charge[dauIdx[1]];
          auto pdgIdKs = pdgId[dauIdx[0]];
          auto pdgIdProton = pdgId[dauIdx[1]];

          auto gdauIdx = dauIdxEvt.at(dauIdx[0]);

          recoP4.insert(
              std::map<std::string, PtEtaPhiM_t>::value_type(
              "pi-", PtEtaPhiM_t (
              p.cand_pT()[gdauIdx[0]],
              p.cand_eta()[gdauIdx[0]],
              p.cand_phi()[gdauIdx[0]],
              p.cand_mass()[gdauIdx[0]]
              )));
          recoP4.insert(
              std::map<std::string, PtEtaPhiM_t>::value_type(
              "pi+", PtEtaPhiM_t (
              p.cand_pT()[gdauIdx[1]],
              p.cand_eta()[gdauIdx[1]],
              p.cand_phi()[gdauIdx[1]],
              p.cand_mass()[gdauIdx[1]]
              )));

          auto chargePiM = charge[gdauIdx[0]];
          auto chargePiP = charge[gdauIdx[1]];
          auto pdgIdPiM = pdgId[gdauIdx[0]];
          auto pdgIdPiP = pdgId[gdauIdx[1]];

          recoIdx["Lc"] = ireco;
          recoIdx["Ks"] = dauIdx[0];
          recoIdx["proton"] = dauIdx[1];
          recoIdx["pi-"] = gdauIdx[0];
          recoIdx["pi+"] = gdauIdx[1];

          std::map<std::string, ROOT::Math::SVector<float, 5>> recoParameters;
          std::map<std::string, ROOT::Math::SMatrix<float, 5, 5, ROOT::Math::MatRepSym<float,5>>> errorMatrix;
          for (const auto& e : recoIdx) {
            const auto& handle = e.first;
            const auto  idx    = e.second;
            if (handle == "Ks" || handle == "Lc") continue;
            const float* trkPars = &p.trk_trackParameters().at(p.cand_trkIdx().at(idx)).at(0);
            recoParameters[handle] = ROOT::Math::SVector<float, 5>(trkPars, 5);
            errorMatrix[handle] = ROOT::Math::SMatrix<float, 5, 5, ROOT::Math::MatRepSym<float, 5>>
              (p.trk_trackCovariance().at(p.cand_trkIdx().at(idx)).cbegin(),
               p.trk_trackCovariance().at(p.cand_trkIdx().at(idx)).cend());
            errorMatrix[handle].Invert();
          }

          // reco-gen matching
          for (size_t igen=0; igen<gensize; igen++) {

            std::map<std::string, unsigned int> genIdx; // indices for Lc, Ks, proton, pi+ and pi-
            genIdx["Lc"] = igen;

            if (gen_pdgId[igen] != 4122 * charge[ireco]) continue; // not Lc, next 

            auto gen_dauIdx = gen_dauIdxEvt.at(igen); // a handle for daughter indices of this gen particle

            int gen_pdgIdKs(0), gen_pdgIdProton(0); // initialized with dummy values
            if (gen_dauIdx.size() != 2) continue; //
            for (const auto dIdx : gen_dauIdx) {
              if (abs(gen_pdgId[dIdx]) == 310) { gen_pdgIdKs = gen_pdgId[dIdx]; genIdx["Ks"] = dIdx; }
              if (abs(gen_pdgId[dIdx]) == 2212) { gen_pdgIdProton = gen_pdgId[dIdx]; genIdx["proton"]= dIdx; }
            }
            if (gen_pdgIdProton * gen_pdgIdKs == 0) continue; // !=0 if both of them are set properly

            if (gen_pdgIdProton != pdgIdProton * chargeProton // check proton pdgId and charge
                || gen_pdgIdKs != abs(pdgIdKs) || gen_charge[genIdx.at("Ks")]// check Ks pdgId, charge == 0
               ) continue;

            auto gen_gdauIdx = gen_dauIdxEvt.at(genIdx.at("Ks")); // get the dau indices of Ks

            int gen_pdgIdPiP(0), gen_pdgIdPiM(0); // initialized with dummy values
            for (const auto gdIdx : gen_gdauIdx) {
              if ( gen_pdgId[gdIdx] ==  211) { gen_pdgIdPiP =  211; genIdx["pi+"] = gdIdx; } // set pdgId and index for pi+
              if ( gen_pdgId[gdIdx] == -211) { gen_pdgIdPiM = -211; genIdx["pi-"] = gdIdx; } // set pdgId and index for pi-
            }
            if (gen_pdgIdPiP * gen_pdgIdPiM == 0) continue; // !=0 if both of them are set properly

            if (genIdx.size() != 5) continue; // I need Lc, Ks, proton, pi+, pi- in the vector, otherwise map::at will throw an exception

            // insert p4 for each particle
            std::map<std::string, PtEtaPhiM_t> genP4;

            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                "Lc", PtEtaPhiM_t (
                p.gen_pT()[igen],
                p.gen_eta()[igen],
                p.gen_phi()[igen],
                p.gen_mass()[igen]
              )));

            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                "Ks", PtEtaPhiM_t (
                p.gen_pT()[genIdx.at("Ks")],
                p.gen_eta()[genIdx.at("Ks")],
                p.gen_phi()[genIdx.at("Ks")],
                p.gen_mass()[genIdx.at("Ks")]
                )));
            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                "proton", PtEtaPhiM_t (
                p.gen_pT()[genIdx.at("proton")],
                p.gen_eta()[genIdx.at("proton")],
                p.gen_phi()[genIdx.at("proton")],
                p.gen_mass()[genIdx.at("proton")]
                )));

            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                "pi+", PtEtaPhiM_t (
                p.gen_pT()[genIdx.at("pi+")],
                p.gen_eta()[genIdx.at("pi+")],
                p.gen_phi()[genIdx.at("pi+")],
                p.gen_mass()[genIdx.at("pi+")]
                )));
            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                "pi-", PtEtaPhiM_t (
                p.gen_pT()[genIdx.at("pi-")],
                p.gen_eta()[genIdx.at("pi-")],
                p.gen_phi()[genIdx.at("pi-")],
                p.gen_mass()[genIdx.at("pi-")]
                )));

            matchGEN = matchCriterion.match(recoP4.at("Lc"), genP4.at("Lc"));

            // declaration of containers
            std::map<std::string, float> dR;
            std::map<std::string, float> dPt;
            std::map<std::string, float> chi2;

            std::map<std::string, ROOT::Math::SVector<float, 5>> genParameters;
            std::map<std::string, ROOT::Math::SVector<float, 5>> diffParameters;
            for (const auto& e : genIdx) {
              // key (particle) and value (index)
              auto& handle = e.first;
              auto  idx    = e.second;
              if (handle != "Ks" && handle != "Lc") {
                // gen tracking paramter
                const float* genTrkPars = &p.gen_trackParameters().at(idx).at(0);
                genParameters[handle] = ROOT::Math::SVector<float, 5>(genTrkPars, 5);
                // difference
                diffParameters[handle] = genParameters.at(handle) - recoParameters.at(handle);
                // adjust dPhi
                const auto dPhi = (diffParameters[handle])[2];
                if (dPhi > TMath::Pi()) (diffParameters[handle])[2] = dPhi - 2*TMath::Pi();
                else if (dPhi < -TMath::Pi()) (diffParameters[handle])[2] = dPhi + 2*TMath::Pi();
                // calculate chi2
                chi2[handle] = ROOT::Math::Dot(diffParameters.at(handle) * errorMatrix.at(handle), diffParameters.at(handle));
                chi2[handle] /= 5;
              }

              // calculated dR
              dR[handle] = ROOT::Math::VectorUtil::DeltaR(genP4.at(handle), recoP4.at(handle));
              dPt[handle] = abs(genP4.at(handle).Pt() - recoP4.at(handle).Pt())/genP4.at(handle).Pt();
            }

            if (matchGEN) {
              matchedLcMass["matchLcP3"]->Fill(p.cand_mass()[ireco]);
              for (const auto& e : matchedLcChi2) {
                e.second->Fill(dR.at(e.first), chi2.at(e.first));
              }
              for (const auto& e : matchedLcdPt) {
                e.second->Fill(dR.at(e.first), dPt.at(e.first));
              }
              break; // found one, not necessary to check later
            }
          }
          // another analysis, matchGEN, peak and side
          const float width = 0.024; // standard deviation by tree->Draw("cand_mass", "cand_matchGEN && cand_status == 3")
          const float mean  = 2.2865; // mean, PDG value
          if (pdgId[ireco] == 4122 && p.cand_matchGEN()[ireco] && !p.cand_isSwap()[ireco]) { // need to be Lc, match gen, and not a swap(mislabel) candidate
            matchedLcMass["matchFSP3"]->Fill(p.cand_mass()[ireco]);

            const bool inPeak = abs(p.cand_mass()[ireco] - mean) < 2 * width; // peak region is within 2 * width
            const bool inSide = abs(p.cand_mass()[ireco] - mean) > 3 * width; // side region is beyond 3 * width
            std::map<std::string, unsigned int> recoIdx; // indices for Lc, Ks, proton, pi+ and pi-
            std::map<std::string, PtEtaPhiM_t> recoP4; // reco p4 for Lc, Ks, proton, pi+ and pi-
            recoP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "Lc", PtEtaPhiM_t(
                    p.cand_pT()[ireco], // yes, ireco is Lc
                    p.cand_eta()[ireco],
                    p.cand_phi()[ireco],
                    p.cand_mass()[ireco]
                    )));

            auto dauIdx = dauIdxEvt.at(ireco);
            recoP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "Ks", PtEtaPhiM_t (
                    p.cand_pT()[dauIdx[0]], // 0 is always Ks (small mass)
                    p.cand_eta()[dauIdx[0]],
                    p.cand_phi()[dauIdx[0]],
                    p.cand_mass()[dauIdx[0]]
                    )));
            recoP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "proton", PtEtaPhiM_t (
                    p.cand_pT()[dauIdx[1]], // 1 is always proton (larger mass)
                    p.cand_eta()[dauIdx[1]],
                    p.cand_phi()[dauIdx[1]],
                    p.cand_mass()[dauIdx[1]]
                    )));

            auto chargeKs = charge[dauIdx[0]];
            auto chargeProton = charge[dauIdx[1]];
            auto pdgIdKs = pdgId[dauIdx[0]];
            auto pdgIdProton = pdgId[dauIdx[1]];

            auto gdauIdx = dauIdxEvt.at(dauIdx[0]); // indices for pi+pi-

            recoP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "pi-", PtEtaPhiM_t (
                    p.cand_pT()[gdauIdx[0]], // first is always pi-, with smaller charge
                    p.cand_eta()[gdauIdx[0]],
                    p.cand_phi()[gdauIdx[0]],
                    p.cand_mass()[gdauIdx[0]]
                    )));
            recoP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "pi+", PtEtaPhiM_t (
                    p.cand_pT()[gdauIdx[1]], // second is always pi+, with larger charge
                    p.cand_eta()[gdauIdx[1]],
                    p.cand_phi()[gdauIdx[1]],
                    p.cand_mass()[gdauIdx[1]]
                    )));

            auto chargePiM = charge[gdauIdx[0]];
            auto chargePiP = charge[gdauIdx[1]];
            auto pdgIdPiM = pdgId[gdauIdx[0]];
            auto pdgIdPiP = pdgId[gdauIdx[1]];

            recoIdx["Lc"] = ireco;
            recoIdx["Ks"] = dauIdx[0];
            recoIdx["proton"] = dauIdx[1];
            recoIdx["pi-"] = gdauIdx[0];
            recoIdx["pi+"] = gdauIdx[1];

            std::map<std::string, ROOT::Math::SVector<float, 5>> recoParameters;
            std::map<std::string, ROOT::Math::SMatrix<float, 5, 5, ROOT::Math::MatRepSym<float,5>>> errorMatrix;
            for (const auto& e : recoIdx) {
              const auto& handle = e.first;
              const auto  idx    = e.second;
              if (handle == "Ks" || handle == "Lc") continue;
              const float* trkPars = &p.trk_trackParameters().at(p.cand_trkIdx().at(idx)).at(0);
              recoParameters[handle] = ROOT::Math::SVector<float, 5>(trkPars, 5);
              errorMatrix[handle] = ROOT::Math::SMatrix<float, 5, 5, ROOT::Math::MatRepSym<float, 5>>
                (p.trk_trackCovariance().at(p.cand_trkIdx().at(idx)).cbegin(),
                 p.trk_trackCovariance().at(p.cand_trkIdx().at(idx)).cend());
              errorMatrix[handle].Invert();
            }

            std::map<std::string, unsigned int> genIdx;
            genIdx["Lc"] = p.cand_genIdx()[ireco]; // directly get the index
            genIdx["Ks"] = p.cand_genIdx().at(recoIdx.at("Ks")); // of course it matches
            genIdx["proton"] = p.cand_genIdx().at(recoIdx.at("proton")); // of course it matches
            genIdx["pi+"] = p.cand_genIdx().at(recoIdx.at("pi+")); // of course it matches
            genIdx["pi-"] = p.cand_genIdx().at(recoIdx.at("pi-")); // of course it matches

            std::map<std::string, PtEtaPhiM_t> genP4;

            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "Lc", PtEtaPhiM_t (
                    p.gen_pT()[genIdx.at("Lc")],
                    p.gen_eta()[genIdx.at("Lc")],
                    p.gen_phi()[genIdx.at("Lc")],
                    p.gen_mass()[genIdx.at("Lc")]
                    )));
            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "Ks", PtEtaPhiM_t (
                    p.gen_pT()[genIdx.at("Ks")],
                    p.gen_eta()[genIdx.at("Ks")],
                    p.gen_phi()[genIdx.at("Ks")],
                    p.gen_mass()[genIdx.at("Ks")]
                    )));
            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "proton", PtEtaPhiM_t (
                    p.gen_pT()[genIdx.at("proton")],
                    p.gen_eta()[genIdx.at("proton")],
                    p.gen_phi()[genIdx.at("proton")],
                    p.gen_mass()[genIdx.at("proton")]
                    )));
            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "pi+", PtEtaPhiM_t (
                    p.gen_pT()[genIdx.at("pi+")],
                    p.gen_eta()[genIdx.at("pi+")],
                    p.gen_phi()[genIdx.at("pi+")],
                    p.gen_mass()[genIdx.at("pi+")]
                    )));
            genP4.insert(
                std::map<std::string, PtEtaPhiM_t>::value_type(
                  "pi-", PtEtaPhiM_t (
                    p.gen_pT()[genIdx.at("pi-")],
                    p.gen_eta()[genIdx.at("pi-")],
                    p.gen_phi()[genIdx.at("pi-")],
                    p.gen_mass()[genIdx.at("pi-")]
                    )));

            std::map<std::string, float> dR;
            std::map<std::string, float> dPt;
            std::map<std::string, float> chi2;

            std::map<std::string, ROOT::Math::SVector<float, 5>> genParameters;
            std::map<std::string, ROOT::Math::SVector<float, 5>> diffParameters;
            for (const auto& e : genIdx) {
              // key (particle) and value (index)
              auto& handle = e.first;
              auto& idx = e.second;
              if (handle != "Ks" && handle != "Lc") {
                // gen tracking paramter
                const float* genTrkPars = &p.gen_trackParameters().at(idx).at(0);
                genParameters[handle] = ROOT::Math::SVector<float, 5>(genTrkPars, 5);
                // difference
                diffParameters[handle] = genParameters.at(handle) - recoParameters.at(handle);
                // adjust dPhi
                const auto dPhi = (diffParameters[handle])[2];
                if (dPhi > TMath::Pi()) (diffParameters[handle])[2] = dPhi - 2*TMath::Pi();
                else if (dPhi < -TMath::Pi()) (diffParameters[handle])[2] = dPhi + 2*TMath::Pi();
                // calculate chi2
                chi2[handle] = ROOT::Math::Dot(diffParameters.at(handle) * errorMatrix.at(handle), diffParameters.at(handle));
                chi2[handle] /= 5;
              }

              // calculated dR
              dR[handle] = ROOT::Math::VectorUtil::DeltaR(genP4.at(handle), recoP4.at(handle));
              dPt[handle] = abs(genP4.at(handle).Pt() - recoP4.at(handle).Pt())/genP4.at(handle).Pt();
            }

            if (inPeak) {
              for (auto& e : peakLcChi2) e.second->Fill(dR.at(e.first), chi2.at(e.first));
              for (auto& e : peakLcdPt) e.second->Fill(dR.at(e.first), dPt.at(e.first));
            }
            if (inSide) {
              for (auto& e : sideLcChi2) e.second->Fill(dR.at(e.first), chi2.at(e.first));
              for (auto& e : sideLcdPt) e.second->Fill(dR.at(e.first), dPt.at(e.first));
            }
          }
        } // end Lc
      } // end this event
    } // end loop all events

    // save outputs
    cout << "Ready to write output histograms" << endl;
    ofile.cd();
    for (const auto& e : matchedLcChi2) e.second->Write();
    for (const auto& e : matchedLcdPt) e.second->Write();
    for (const auto& e : peakLcChi2) e.second->Write();
    for (const auto& e : peakLcdPt) e.second->Write();
    for (const auto& e : sideLcChi2) e.second->Write();
    for (const auto& e : sideLcdPt) e.second->Write();
    for (const auto& e : matchedLcMass) e.second->Write();
    // close ofile will crash, histograms will be deleted twice
  }

  void genLcMultipleMatch(const TString& inputList)
  {
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
