#ifndef MyNTuple_H
#include "Ana/MyNTuple.h"
#endif

#ifndef ParticleTreeMC_hxx
#include "TreeReader/ParticleTreeMC.hxx"
#endif

#ifndef ParticleTreeData_hxx
#include "TreeReader/ParticleTreeData.hxx"
#endif

#ifndef HelpClass_H
#include "Ana/Common.h"
#endif

#ifndef TreeHelpers_H
#include "Ana/TreeHelpers.h"
#endif

#ifdef MyNTuple_H
#include "TTree.h"
#include "TString.h"

using std::vector;

void MyNTuple::setNDau(const unsigned short ndau,
                     const unsigned short ngdau,
                     const unsigned short* dauNGDau)
{
  nDau = ndau;
  nGDau = 0;
  for (unsigned short i=0; i<nDau; ++i) {
    dauHasNGDau[i] = dauNGDau[i];
    nGDau += dauNGDau[i];
  }
  if (nGDau != ngdau) {
    std::cerr << "Total number of granddaughters does not match" << std::endl;
  }
}

void MyNTuple::initWeightBranch()
{
  eventWeight = 1.;
  t->Branch("eventWeight", &eventWeight);
}

void MyNTuple::setEventWeight(const float weight)
{
  eventWeight = weight;
}

void MyNTuple::initMVABranches(const vector<TString>& methods)
{
  cand_nMVA = methods.size();
  for(size_t i=0; i<cand_nMVA; i++) {
    t->Branch(Form("%s", methods.at(i).Data()), &cand_MVA[i]);
  }
}

void MyNTuple::initNTuple()
{
  if (!nDau) {
    std::cerr << "[ERROR] NTuple::nDau is zero."
      " Make sure you set number of daughters via NTuple::setNDau()!"
              << std::endl;
    return;
  }
  // mva
  // lumi
  if (!isMC) t->Branch("hltRecordLumi", &hltRecordLumi);
  if (!isMC) t->Branch("Ntrkoffline", &Ntrkoffline);
  if (!isMC) t->Branch("cand_Ntrkoffline", &cand_Ntrkoffline);
  if (!isMC) t->Branch("Ntrkoffline", &Ntrkoffline);
  if (!isMC) {
    for (UInt_t i=0; i<nTrigs; ++i) {
      t->Branch(Form("trigBit_%u", i), &trigBit[i]);
    }
    for (UInt_t i=0; i<nFilters; ++i) {
      t->Branch(Form("filterBit_%u", i), &filterBit[i]);
    }
  }
  // particle level
  t->Branch("cand_charge", &cand_charge);
  t->Branch("cand_eta", &cand_eta);
  t->Branch("cand_y", &cand_y);
  t->Branch("cand_mass", &cand_mass);
  t->Branch("cand_pT", &cand_pT);
  t->Branch("cand_phi", &cand_phi);

  if (isMC) {
    t->Branch("cand_matchGEN", &cand_matchGEN);
    t->Branch("cand_isSwap", &cand_isSwap);
  }

  if (!dropDau) {
    t->Branch("cand_vtxChi2", &cand_vtxChi2);
    t->Branch("cand_vtxProb", &cand_vtxProb);
    t->Branch("cand_angle2D", &cand_angle2D);
    t->Branch("cand_angle3D", &cand_angle3D);
    t->Branch("cand_dca", &cand_dca);
    t->Branch("cand_decayLength2D", &cand_decayLength2D);
    t->Branch("cand_decayLength3D", &cand_decayLength3D);
    t->Branch("cand_decayLengthError2D", &cand_decayLengthError2D);
    t->Branch("cand_decayLengthError3D", &cand_decayLengthError3D);
    t->Branch("cand_pseudoDecayLengthError2D", &cand_pseudoDecayLengthError2D);
    t->Branch("cand_pseudoDecayLengthError3D", &cand_pseudoDecayLengthError3D);

    if (nDau == 2) {
      t->Branch("cand_dauCosOpenAngle3D", &cand_dauCosOpenAngle3D);
      t->Branch("cand_dau_dR", &cand_dau_dR);
    }
  }

  if (!dropDau) {
    for (unsigned short iDau=0; iDau<nDau; ++iDau) {
      t->Branch(Form("cand_etaDau%d", iDau), &cand_etaDau[iDau]);
      t->Branch(Form("cand_massDau%d", iDau), &cand_massDau[iDau]);
      t->Branch(Form("cand_pTDau%d", iDau), &cand_pTDau[iDau]);
      t->Branch(Form("cand_phiDau%d", iDau), &cand_phiDau[iDau]);
    }

    // daughter level
    for (unsigned short iDau=0; iDau<nDau; ++iDau) {
      if (isMC) {
        t->Branch(Form("cand_dau%d_matchGEN", iDau), &cand_dau_matchGEN[iDau]);
        t->Branch(Form("cand_dau%d_isSwap", iDau), &cand_dau_isSwap[iDau]);
      }

      t->Branch(Form("cand_dau%d_charge", iDau), &cand_dau_charge[iDau]);
      t->Branch(Form("cand_dau%d_eta", iDau), &cand_dau_eta[iDau]);
      //    t->Branch(Form("cand_dau%d_y", iDau), &cand_dau_y[iDau]);
      //    t->Branch(Form("cand_dau%d_p", iDau), &cand_dau_p[iDau]);
      t->Branch(Form("cand_dau%d_mass", iDau), &cand_dau_mass[iDau]);
      t->Branch(Form("cand_dau%d_pT", iDau), &cand_dau_pT[iDau]);
      t->Branch(Form("cand_dau%d_phi", iDau), &cand_dau_phi[iDau]);

      unsigned short gDauOffset = 0;
      if (dauHasNGDau[iDau]) {
        t->Branch(Form("cand_dau%d_vtxChi2", iDau), &cand_dau_vtxChi2[iDau]);
        t->Branch(Form("cand_dau%d_vtxProb", iDau), &cand_dau_vtxProb[iDau]);
        t->Branch(Form("cand_dau%d_angle2D", iDau), &cand_dau_angle2D[iDau]);
        t->Branch(Form("cand_dau%d_angle3D", iDau), &cand_dau_angle3D[iDau]);
        t->Branch(Form("cand_dau%d_dca", iDau), &cand_dau_dca[iDau]);
        t->Branch(Form("cand_dau%d_decayLength2D", iDau), &cand_dau_decayLength2D[iDau]);
        t->Branch(Form("cand_dau%d_decayLength3D", iDau), &cand_dau_decayLength3D[iDau]);
        t->Branch(Form("cand_dau%d_decayLengthError2D", iDau), &cand_dau_decayLengthError2D[iDau]);
        t->Branch(Form("cand_dau%d_decayLengthError3D", iDau), &cand_dau_decayLengthError3D[iDau]);
        t->Branch(Form("cand_dau%d_pseudoDecayLengthError2D", iDau), &cand_dau_pseudoDecayLengthError2D[iDau]);
        t->Branch(Form("cand_dau%d_pseudoDecayLengthError3D", iDau), &cand_dau_pseudoDecayLengthError3D[iDau]);
        for (unsigned short iGDau=0; iGDau<dauHasNGDau[iDau]; ++iGDau) {
          t->Branch(Form("cand_dau%d_massDau%d", iDau, iGDau), &cand_dau_massDau[iDau][iGDau]);
          t->Branch(Form("cand_dau%d_pTDau%d", iDau, iGDau), &cand_dau_pTDau[iDau][iGDau]);
          t->Branch(Form("cand_dau%d_etaDau%d", iDau, iGDau), &cand_dau_etaDau[iDau][iGDau]);
          t->Branch(Form("cand_dau%d_phiDau%d", iDau, iGDau), &cand_dau_phiDau[iDau][iGDau]);
        }
      } else {
        t->Branch(Form("trk_dau%d_isHP", iDau), &trk_dau_isHP[iDau]);
        t->Branch(Form("trk_dau%d_nHit", iDau), &trk_dau_nHit[iDau]);
        t->Branch(Form("trk_dau%d_nPixelHit", iDau), &trk_dau_nPixelHit[iDau]);
        t->Branch(Form("trk_dau%d_nStripHit", iDau), &trk_dau_nStripHit[iDau]);
        t->Branch(Form("trk_dau%d_dEdx_dedxHarmonic2", iDau), &trk_dau_dEdx_dedxHarmonic2[iDau]);
        t->Branch(Form("trk_dau%d_dEdxRes", iDau), &trk_dau_dEdxRes[iDau]);
        t->Branch(Form("trk_dau%d_dEdx_dedxPixelHarmonic2", iDau), &trk_dau_dEdx_dedxPixelHarmonic2[iDau]);
        t->Branch(Form("trk_dau%d_nChi2", iDau), &trk_dau_nChi2[iDau]);
        t->Branch(Form("trk_dau%d_pTErr", iDau), &trk_dau_pTErr[iDau]);
        t->Branch(Form("trk_dau%d_xyDCASignificance", iDau), &trk_dau_xyDCASignificance[iDau]);
        t->Branch(Form("trk_dau%d_zDCASignificance", iDau), &trk_dau_zDCASignificance[iDau]);
      }
      gDauOffset += dauHasNGDau[iDau];
    } // end daughter level
    // granddaughter level
    for (unsigned short iGDau=0; iGDau<nGDau; ++iGDau) {
      if (isMC) {
        t->Branch(Form("cand_gdau%d_matchGEN", iGDau), &cand_gdau_matchGEN[iGDau]);
        t->Branch(Form("cand_gdau%d_isSwap", iGDau), &cand_gdau_isSwap[iGDau]);
      }

      t->Branch(Form("cand_gdau%d_charge", iGDau), &cand_gdau_charge[iGDau]);
      t->Branch(Form("cand_gdau%d_eta", iGDau), &cand_gdau_eta[iGDau]);
      t->Branch(Form("cand_gdau%d_mass", iGDau), &cand_gdau_mass[iGDau]);
      t->Branch(Form("cand_gdau%d_pT", iGDau), &cand_gdau_pT[iGDau]);
      // t->Branch(Form("cand_gdau%d_p", iGDau), &cand_gdau_p[iGDau]);
      t->Branch(Form("cand_gdau%d_phi", iGDau), &cand_gdau_phi[iGDau]);
      // t->Branch(Form("cand_gdau%d_y", iGDau), &cand_gdau_y[iGDau]);

      t->Branch(Form("trk_gdau%d_isHP", iGDau), &trk_gdau_isHP[iGDau]);
      t->Branch(Form("trk_gdau%d_nHit", iGDau), &trk_gdau_nHit[iGDau]);
      t->Branch(Form("trk_gdau%d_nPixelHit", iGDau), &trk_gdau_nPixelHit[iGDau]);
      t->Branch(Form("trk_gdau%d_nStripHit", iGDau), &trk_gdau_nStripHit[iGDau]);
      t->Branch(Form("trk_gdau%d_dEdx_dedxHarmonic2", iGDau), &trk_gdau_dEdx_dedxHarmonic2[iGDau]);
      t->Branch(Form("trk_gdau%d_dEdxRes", iGDau), &trk_gdau_dEdxRes[iGDau]);
      t->Branch(Form("trk_gdau%d_dEdx_dedxPixelHarmonic2", iGDau), &trk_gdau_dEdx_dedxPixelHarmonic2[iGDau]);
      t->Branch(Form("trk_gdau%d_nChi2", iGDau), &trk_gdau_nChi2[iGDau]);
      t->Branch(Form("trk_gdau%d_pTErr", iGDau), &trk_gdau_pTErr[iGDau]);
      t->Branch(Form("trk_gdau%d_xyDCASignificance", iGDau), &trk_gdau_xyDCASignificance[iGDau]);
      t->Branch(Form("trk_gdau%d_zDCASignificance", iGDau), &trk_gdau_zDCASignificance[iGDau]);
    } // end granddaughter level
  }
}

void  MyNTuple::initGenBranches()
{
  if (!isMC) return;

  // isPrompt
  t->Branch("gen_isPrompt", &gen_isPrompt);
  // particle level
  // t->Branch("gen_charge", &gen_charge);
  t->Branch("gen_pdgId", &gen_pdgId);
  t->Branch("gen_eta", &gen_eta);
  t->Branch("gen_y", &gen_y);
  t->Branch("gen_mass", &gen_mass);
  t->Branch("gen_pT", &gen_pT);
  t->Branch("gen_phi", &gen_phi);
  t->Branch("gen_momPdgId", &gen_momPdgId);

  if (!dropDau) {
    t->Branch("gen_angle2D", &gen_angle2D);
    t->Branch("gen_angle3D", &gen_angle3D);
    t->Branch("gen_decayLength2D", &gen_decayLength2D);
    t->Branch("gen_decayLength3D", &gen_decayLength3D);
  }

  if (!dropDau) {
    // daughter level
    for (unsigned short iDau=0; iDau<nDau; ++iDau) {
      t->Branch(Form("gen_dau%d_pdgId", iDau), &gen_dau_pdgId[iDau]);
      t->Branch(Form("gen_dau%d_eta", iDau), &gen_dau_eta[iDau]);
      t->Branch(Form("gen_dau%d_mass", iDau), &gen_dau_mass[iDau]);
      t->Branch(Form("gen_dau%d_pT", iDau), &gen_dau_pT[iDau]);
      t->Branch(Form("gen_dau%d_phi", iDau), &gen_dau_phi[iDau]);
      t->Branch(Form("gen_dau%d_y", iDau), &gen_dau_y[iDau]);

      unsigned short gDauOffset = 0;
      if (dauHasNGDau[iDau]) {
        t->Branch(Form("gen_dau%d_angle2D", iDau), &gen_dau_angle2D[iDau]);
        t->Branch(Form("gen_dau%d_angle3D", iDau), &gen_dau_angle3D[iDau]);
        t->Branch(Form("gen_dau%d_decayLength2D", iDau), &gen_dau_decayLength2D[iDau]);
        t->Branch(Form("gen_dau%d_decayLength3D", iDau), &gen_dau_decayLength3D[iDau]);
      }
      gDauOffset += dauHasNGDau[iDau];
    } // end daughter level
    // granddaughter level
    for (unsigned short iGDau=0; iGDau<nGDau; ++iGDau) {
      t->Branch(Form("gen_gdau%d_pdgId", iGDau), &gen_gdau_pdgId[iGDau]);
      t->Branch(Form("gen_gdau%d_eta", iGDau), &gen_gdau_eta[iGDau]);
      t->Branch(Form("gen_gdau%d_mass", iGDau), &gen_gdau_mass[iGDau]);
      t->Branch(Form("gen_gdau%d_pT", iGDau), &gen_gdau_pT[iGDau]);
      t->Branch(Form("gen_gdau%d_phi", iGDau), &gen_gdau_phi[iGDau]);
      t->Branch(Form("gen_gdau%d_y", iGDau), &gen_gdau_y[iGDau]);
    } // end granddaughter level
  }
}

Int_t MyNTuple::fillNTuple()
{
  return t->Fill();
}

bool MyNTuple::setMVAValues(const std::vector<float>& vals)
{
  for (size_t i=0; i<vals.size(); i++) {
    this->cand_MVA[i] = vals[i];
  }
  return true;
}

bool MyNTuple::retrieveTreeInfo(ParticleTree& p, Long64_t it)
{
  if (!isMC) {
    const auto passHLT = p.passHLT();
    //this->nTrigs = passHLT.size();
    for (UInt_t i=0; i< this->nTrigs; ++i) {
      this->trigBit[i] = passHLT.at(i);
    }
    const auto passFilter = p.evtSel();
    //this->nFilters = passFilter.size();
    for (UInt_t i=0; i<this->nFilters; ++i) {
      this->filterBit[i] = passFilter.at(i);
    }
  }

  this->Ntrkoffline = isMC ? 0 : p.Ntrkoffline();
  this->cand_Ntrkoffline = isMC ? 0 : p.cand_Ntrkoffline().at(it);
  this->cand_pT     = p.cand_pT().at(it);
  this->cand_eta    = p.cand_eta().at(it);
  this->cand_phi    = p.cand_phi().at(it);
  this->cand_mass   = p.cand_mass().at(it);
  this->cand_y      = p.cand_y().at(it);
  this->cand_charge = p.cand_charge().at(it);

  this->cand_dca = p.cand_dca().at(it);
  this->cand_angle2D = p.cand_angle2D().at(it);
  this->cand_angle3D = p.cand_angle3D().at(it);
  this->cand_vtxProb = p.cand_vtxProb().at(it);
  this->cand_vtxChi2 = p.cand_vtxChi2().at(it);
  this->cand_decayLength2D      = p.cand_decayLength2D().at(it);
  this->cand_decayLengthError2D = p.cand_decayLengthError2D().at(it);
  this->cand_decayLength3D      = p.cand_decayLength3D().at(it);
  this->cand_decayLengthError3D = p.cand_decayLengthError3D().at(it);
  this->cand_pseudoDecayLengthError2D = p.cand_pseudoDecayLengthError2D().at(it);
  this->cand_pseudoDecayLengthError3D = p.cand_pseudoDecayLengthError3D().at(it);

  for (unsigned short iDau=0; iDau<this->nDau; iDau++) {
    this->cand_pTDau[iDau] = p.cand_pTDau().at(it).at(iDau);
    this->cand_etaDau[iDau] = p.cand_etaDau().at(it).at(iDau);
    this->cand_phiDau[iDau] = p.cand_phiDau().at(it).at(iDau);
    this->cand_massDau[iDau] = p.cand_massDau().at(it).at(iDau);
  }

  // temporary for nDau == 2
  if (nDau==2) {
    PtEtaPhi_t dauP0 = getRecoDauP3(it, 0, p);
    PtEtaPhi_t dauP1 = getRecoDauP3(it, 1, p);
    const auto dauCosAngle = dauP0.Dot(dauP1)/dauP0.R()/dauP1.R();
    cand_dauCosOpenAngle3D = dauCosAngle;
    cand_dau_dR = ROOT::Math::VectorUtil::DeltaR(dauP0, dauP1);

    // const auto p0 = this->cand_pTDau[0] * std::cosh(this->cand_etaDau[0]);
    // const auto p1 = this->cand_pTDau[1] * std::cosh(this->cand_etaDau[1]);
    // const auto pz0 = this->cand_pTDau[0] * std::sinh(this->cand_etaDau[0]);
    // const auto pz1 = this->cand_pTDau[1] * std::sinh(this->cand_etaDau[1]);
    // const auto deltaPhi = ROOT::VecOps::DeltaPhi(this->cand_phiDau[0],
    //                                              this->cand_phiDau[1]);
    // const auto dot = this->cand_pTDau[0] * this->cand_pTDau[1] *
    //   std::cos( deltaPhi ) + pz0 * pz1;
    // const auto cosAngle = dot/(p0*p1);
    // cand_dauCosOpenAngle3D = cosAngle;

  }

  int stableIt = 0; // stable particle
  int interIt = 0; // intermediate states
  int gDauOffset = 0;
  const auto& dauIdxs = p.cand_dauIdx().at(it);
  if (dauIdxs.size() != this->nDau) {
    std::cerr << "[ERROR] retrieveTreeInfo in TreeHelpers.h"
              << " finds cand_dauIdx().(it).size() != this->nDau" << std::endl;
    return false;
  }
  for (size_t iDau=0; iDau<dauIdxs.size(); ++iDau) {
    const auto dIdx = dauIdxs.at(iDau);
    const auto& gDauIdxs = p.cand_dauIdx().at(dIdx);
    if (gDauIdxs.size() != this->dauHasNGDau[iDau]) {
      std::cerr << "[ERROR] retrieveTreeInfo in TreeHelpers.h"
                << " finds cand_dauIdx()[dIdx].size() != this->dauHasNGDau[iDau]"
                << std::endl;
      return false;
    }
    if (gDauIdxs.size()) {
      ++interIt;
      this->cand_dau_charge[iDau] = p.cand_charge().at(dIdx);
      this->cand_dau_pT[iDau]     = p.cand_pT().at(dIdx);
      this->cand_dau_eta[iDau]    = p.cand_eta().at(dIdx);
      this->cand_dau_phi[iDau]    = p.cand_phi().at(dIdx);
      this->cand_dau_mass[iDau]   = p.cand_mass().at(dIdx);
      //  this->cand_dau_y[iDau]    = p.cand_y().at(dIdx);

      this->cand_dau_dca[iDau]     = p.cand_dca().at(dIdx);
      this->cand_dau_vtxProb[iDau] = p.cand_vtxProb().at(dIdx);
      this->cand_dau_vtxChi2[iDau] = p.cand_vtxChi2().at(dIdx);
      this->cand_dau_angle3D[iDau] = p.cand_angle3D().at(dIdx);
      this->cand_dau_angle2D[iDau] = p.cand_angle2D().at(dIdx);
      this->cand_dau_decayLength3D[iDau] = p.cand_decayLength3D().at(dIdx);
      this->cand_dau_decayLength2D[iDau] = p.cand_decayLength2D().at(dIdx);
      this->cand_dau_decayLengthError3D[iDau] = p.cand_decayLengthError3D().at(dIdx);
      this->cand_dau_decayLengthError2D[iDau] = p.cand_decayLengthError2D().at(dIdx);
      this->cand_dau_pseudoDecayLengthError3D[iDau] = p.cand_pseudoDecayLengthError3D().at(dIdx);
      this->cand_dau_pseudoDecayLengthError2D[iDau] = p.cand_pseudoDecayLengthError2D().at(dIdx);
    }
    for (size_t iGDau=0; iGDau<gDauIdxs.size(); ++iGDau) {
      this->cand_dau_pTDau[iDau][iGDau]   = p.cand_pTDau().at(dIdx).at(iGDau);
      this->cand_dau_etaDau[iDau][iGDau]  = p.cand_etaDau().at(dIdx).at(iGDau);
      this->cand_dau_phiDau[iDau][iGDau]  = p.cand_phiDau().at(dIdx).at(iGDau);
      this->cand_dau_massDau[iDau][iGDau] = p.cand_massDau().at(dIdx).at(iGDau);

      const auto& gDIdx = gDauIdxs.at(iGDau);

      this->cand_gdau_charge[gDauOffset] = p.cand_charge().at(gDIdx);
      this->cand_gdau_mass[gDauOffset]   = p.cand_mass().at(gDIdx);
      this->cand_gdau_eta[gDauOffset]    = p.cand_eta().at(gDIdx);
      this->cand_gdau_phi[gDauOffset]    = p.cand_phi().at(gDIdx);
      this->cand_gdau_pT[gDauOffset]     = p.cand_pT().at(gDIdx);
      //      this->cand_gdau_y[gDauOffset]      = p.cand_y().at(gDIdx);
      const auto& trkIdx = p.cand_trkIdx().at(gDIdx);
      this->trk_gdau_isHP[gDauOffset] = p.trk_isHP().at(trkIdx);
      this->trk_gdau_nHit[gDauOffset] = p.trk_nHit().at(trkIdx);
      this->trk_gdau_nPixelHit[gDauOffset] = p.trk_nPixelHit().at(trkIdx);
      this->trk_gdau_nStripHit[gDauOffset] = p.trk_nStripHit().at(trkIdx);
      this->trk_gdau_nChi2[gDauOffset] = p.trk_nChi2().at(trkIdx);
      this->trk_gdau_pTErr[gDauOffset] = p.trk_pTErr().at(trkIdx);
      this->trk_gdau_xyDCASignificance[gDauOffset] = p.trk_xyDCASignificance().at(trkIdx);
      this->trk_gdau_zDCASignificance[gDauOffset]  = p.trk_zDCASignificance().at(trkIdx);
      this->trk_gdau_dEdx_dedxHarmonic2[gDauOffset] = p.trk_dEdx_dedxHarmonic2().at(trkIdx);
      // need to think about this
      const double dedxMean = dedxSel.getMean(this->cand_gdau_pT[gDauOffset] * std::cosh(this->cand_gdau_eta[gDauOffset]));
      this->trk_gdau_dEdxRes[gDauOffset] = (this->trk_gdau_dEdx_dedxHarmonic2[gDauOffset]-dedxMean)/dedxMean;
      this->trk_gdau_dEdx_dedxPixelHarmonic2[gDauOffset] = p.trk_dEdx_dedxPixelHarmonic2().at(trkIdx);

       gDauOffset++;
    }
    if (gDauIdxs.empty()) {
      ++stableIt;
      this->cand_dau_charge[iDau] = p.cand_charge().at(dIdx);
      this->cand_dau_pT[iDau]     = p.cand_pT().at(dIdx);
      this->cand_dau_eta[iDau]    = p.cand_eta().at(dIdx);
      this->cand_dau_phi[iDau]    = p.cand_phi().at(dIdx);
      this->cand_dau_mass[iDau]   = p.cand_mass().at(dIdx);
      //  this->cand_dau_y[iDau]    = p.cand_y().at(dIdx);
      const auto& trkIdx = p.cand_trkIdx().at(dIdx);
      this->trk_dau_isHP[iDau] = p.trk_isHP().at(trkIdx);
      this->trk_dau_nHit[iDau] = p.trk_nHit().at(trkIdx);
      this->trk_dau_nPixelHit[iDau] = p.trk_nPixelHit().at(trkIdx);
      this->trk_dau_nStripHit[iDau] = p.trk_nStripHit().at(trkIdx);
      this->trk_dau_nChi2[iDau] = p.trk_nChi2().at(trkIdx);
      this->trk_dau_pTErr[iDau] = p.trk_pTErr().at(trkIdx);
      this->trk_dau_xyDCASignificance[iDau] = p.trk_xyDCASignificance().at(trkIdx);
      this->trk_dau_zDCASignificance[iDau] = p.trk_zDCASignificance().at(trkIdx);
      this->trk_dau_dEdx_dedxHarmonic2[iDau] = p.trk_dEdx_dedxHarmonic2().at(trkIdx);
      const double dedxMean = dedxSel.getMean(this->cand_dau_pT[iDau] * std::cosh(this->cand_dau_eta[iDau]));
      this->trk_dau_dEdxRes[iDau] = (this->trk_dau_dEdx_dedxHarmonic2[iDau]-dedxMean)/dedxMean;
      this->trk_dau_dEdx_dedxPixelHarmonic2[iDau] = p.trk_dEdx_dedxPixelHarmonic2().at(trkIdx);
    }
  }

  return true;
}

/**
// the following part has an issue to retrieve gen info
// because the treeidx may not be properly set
// I only test for the case taht Ks and proton are stable.
// Not sure what will happen if Ks is unstable, which
// means Ks.isLongLived() is False
*/

bool MyNTuple::retrieveGenInfo(ParticleTreeMC& p, Particle* ptr)
{
  if (!isMC) return false;
  gen_isPrompt = true;
  gen_charge = -99;
  gen_pdgId = -99;
  gen_angle3D = -99;
  gen_angle2D = -99;
  gen_decayLength3D = -99;
  gen_decayLength2D = -99;
  gen_mass = -99;
  gen_pT = -99;
  gen_eta = -99;
  gen_phi = -99;
  gen_y = -99;
  gen_momPdgId = -99;
  for (size_t i=0; i<100; ++i) {
    gen_dau_pdgId[i] = -99;
    gen_dau_angle3D[i] = -99;
    gen_dau_angle2D[i] = -99;
    gen_dau_decayLength3D[i] = -99;
    gen_dau_decayLength2D[i] = -99;
    gen_dau_pT[i] = -99;
    gen_dau_eta[i] = -99;
    gen_dau_phi[i] = -99;
    gen_dau_mass[i] = -99;
    gen_dau_y[i] = -99;
  }

  for (size_t i=0; i<100; ++i) {
    gen_gdau_pdgId[i] = -99;
    gen_gdau_pT[i] = -99;
    gen_gdau_eta[i] = -99;
    gen_gdau_phi[i] = -99;
    gen_gdau_mass[i] = -99;
    gen_gdau_y[i] = -99;
  }
  if (!ptr) {
     return true;
  }

  const auto it = ptr->treeIdx();
  this->gen_momPdgId  = p.gen_pdgId().at(p.gen_momIdx().at(it).front());
  this->gen_pT     = p.gen_pT().at(it);
  this->gen_eta    = p.gen_eta().at(it);
  this->gen_phi    = p.gen_phi().at(it);
  this->gen_mass   = p.gen_mass().at(it);
  this->gen_y      = p.gen_y().at(it);
  this->gen_pdgId  = p.gen_pdgId().at(it);
  this->gen_charge = p.gen_charge().at(it);

  int ipar = it;
  auto pdgId = p.gen_pdgId();
  for (auto momIdxs = p.gen_momIdx().at(it); !momIdxs.empty();
       momIdxs = p.gen_momIdx().at(ipar)) {
    if (momIdxs.size() > 1) {
      std::cerr << "Multiple mother particles" << std::endl;
    }
    auto momIdx = momIdxs.front();
    auto id = pdgId.at(momIdx);
    std::string idstr = std::to_string(id);
    if (idstr.at(0) == '5') {
      gen_isPrompt = false;
      break;
    }
    ipar = momIdx;
  }
  if (gen_isPrompt) {
    auto id = pdgId.at(ipar);
    if (abs(id) == 5) {
      gen_isPrompt = false;
    }
    // if (abs(id) != 5) {
    //   auto dauIdxs = p.gen_dauIdx().at(ipar);
    //   for (const auto idx : dauIdxs) {
    //     std::cout << pdgId.at(idx) << "  ";
    //   }
    //   std::cout << std::endl;
    // }
  }

  this->gen_angle2D = p.gen_angle2D().at(it);
  this->gen_angle3D = p.gen_angle3D().at(it);

  this->gen_decayLength2D = p.gen_decayLength2D().at(it);
  this->gen_decayLength3D = p.gen_decayLength3D().at(it);

  int gDauOffset = 0;

  const auto daus = ptr->daughters();

  for (size_t iDau=0; iDau<daus.size(); ++iDau) {
    const auto dau = daus.at(iDau);
    const auto dIdx = dau->treeIdx();
    const auto gDaus = dau->daughters();

    this->gen_dau_pdgId[iDau] = p.gen_pdgId().at(dIdx);
    this->gen_dau_pT[iDau]    = p.gen_pT().at(dIdx);
    this->gen_dau_eta[iDau]   = p.gen_eta().at(dIdx);
    this->gen_dau_phi[iDau]   = p.gen_phi().at(dIdx);
    this->gen_dau_mass[iDau]  = p.gen_mass().at(dIdx);
    this->gen_dau_y[iDau]     = p.gen_y().at(dIdx);

    if (!gDaus.empty()) {
      this->gen_dau_angle3D[iDau] = p.gen_angle3D().at(dIdx);
      this->gen_dau_angle2D[iDau] = p.gen_angle2D().at(dIdx);

      this->gen_dau_decayLength3D[iDau] = p.gen_decayLength3D().at(dIdx);
      this->gen_dau_decayLength2D[iDau] = p.gen_decayLength2D().at(dIdx);
    }
    for (size_t iGDau=0; iGDau<gDaus.size(); ++iGDau) {
      const auto& gDIdx = gDaus.at(iGDau)->treeIdx();
      this->gen_gdau_pdgId[gDauOffset] = p.gen_pdgId().at(gDIdx);
      this->gen_gdau_mass[gDauOffset]  = p.gen_mass().at(gDIdx);
      this->gen_gdau_eta[gDauOffset]   = p.gen_eta().at(gDIdx);
      this->gen_gdau_phi[gDauOffset]   = p.gen_phi().at(gDIdx);
      this->gen_gdau_pT[gDauOffset]    = p.gen_pT().at(gDIdx);
      this->gen_gdau_y[gDauOffset]     = p.gen_y().at(gDIdx);
      gDauOffset++;
    }
  }

  return true;
}

float MyNTuple::value(const TString& s)
{
  if (s == "cand_angle2D") return this->cand_angle2D;
  if (s == "cand_angle3D") return this->cand_angle3D;
  if (s == "cand_dca") return this->cand_dca;
  if (s == "cand_decayLength2D") return this->cand_decayLength2D;
  if (s == "cand_decayLength3D") return this->cand_decayLength3D;
  if (s == "cand_decayLengthError2D") return this->cand_decayLengthError2D;
  if (s == "cand_decayLengthError3D") return this->cand_decayLengthError3D;
  if (s == "cand_eta") {
    if (flipEta)
      return -1*this->cand_eta;
    else
      return this->cand_eta;
  }
  if (s == "cand_mass") return this->cand_mass;
  if (s == "cand_pT") return this->cand_pT;
  if (s == "cand_y") {
    if (flipEta)
      return -1*this->cand_y;
    else
      return this->cand_y;
  }
  if (s == "cand_phi") return this->cand_phi;
  if (s == "cand_pseudoDecayLengthError2D") return this->cand_pseudoDecayLengthError2D;
  if (s == "cand_pseudoDecayLengthError3D") return this->cand_pseudoDecayLengthError3D;
  if (s == "cand_vtxChi2") return this->cand_vtxChi2;
  if (s == "cand_vtxProb") return this->cand_vtxProb;
  if (s == "cand_dauCosOpenAngle3D") return this->cand_dauCosOpenAngle3D;
  if (s == "cand_dau_dR") return this->cand_dau_dR;

  if (s.Contains("_dau")) {
    const auto index = s.Contains("cand") ? TString(s[8]).Atoi() : TString(s[7]).Atoi();

    if (s.Contains("Dau")) {
      auto gDauIndex = TString(s[s.Length()-1]).Atoi();
      if (s.Contains("etaDau")) {
        if (flipEta)
          return -1*this->cand_dau_etaDau[index][gDauIndex];
        else
          return this->cand_dau_etaDau[index][gDauIndex];
      }
      if (s.Contains("massDau")) return this->cand_dau_massDau[index][gDauIndex];
      if (s.Contains("pTDau")) return this->cand_dau_pTDau[index][gDauIndex];
      if (s.Contains("phiDau")) return this->cand_dau_phiDau[index][gDauIndex];
    } else {
      if (s.Contains("angle2D")) return this->cand_dau_angle2D[index];
      if (s.Contains("angle3D")) return this->cand_dau_angle3D[index];
      if (s.Contains("dca")) return this->cand_dau_dca[index];
      if (s.Contains("decayLength2D")) return this->cand_dau_decayLength2D[index];
      if (s.Contains("decayLength3D")) return this->cand_dau_decayLength3D[index];
      if (s.Contains("decayLengthError2D")) return this->cand_dau_decayLengthError2D[index];
      if (s.Contains("decayLengthError3D")) return this->cand_dau_decayLengthError3D[index];
      if (s.Contains("eta")) {
        if (flipEta)
          return -1*this->cand_dau_eta[index];
        else
          return this->cand_dau_eta[index];
      }
      if (s.Contains("mass")) return this->cand_dau_mass[index];
      if (s.Contains("pT")) return this->cand_dau_pT[index];
      if (s.Contains("phi")) return this->cand_dau_phi[index];
      if (s.Contains("pseudoDecayLengthError2D")) return this->cand_dau_pseudoDecayLengthError2D[index];
      if (s.Contains("pseudoDecayLengthError3D")) return this->cand_dau_pseudoDecayLengthError3D[index];
      if (s.Contains("vtxChi2")) return this->cand_dau_vtxChi2[index];
      if (s.Contains("vtxProb")) return this->cand_dau_vtxProb[index];
      if (s.Contains("nChi2")) return this->trk_dau_nChi2[index];
      if (s.Contains("pTErr")) return this->trk_dau_pTErr[index];
      if (s.Contains("xyDCASignificance")) return this->trk_dau_xyDCASignificance[index];
      if (s.Contains("zDCASignificance")) return this->trk_dau_zDCASignificance[index];
      if (s.Contains("dedxHarmonic2")) return this->trk_dau_dEdx_dedxHarmonic2[index];
      if (s.Contains("dEdxRes")) return this->trk_dau_dEdxRes[index];
      if (s.Contains("dedxPixelHarmonic2")) return this->trk_dau_dEdx_dedxPixelHarmonic2[index];
    }
  } else if (s.Contains("_gdau")) {
    const auto index = s.Contains("cand") ? TString(s[9]).Atoi() : TString(s[8]).Atoi();
    if (s.Contains("eta")) {
      if (flipEta)
        return -1*this->cand_gdau_eta[index];
      else
        return this->cand_gdau_eta[index];
    }
    if (s.Contains("mass")) return this->cand_gdau_mass[index];
    if (s.Contains("pT")) return this->cand_gdau_pT[index];
    if (s.Contains("phi")) return this->cand_gdau_phi[index];
    if (s.Contains("nChi2")) return this->trk_gdau_nChi2[index];
    if (s.Contains("pTErr")) return this->trk_gdau_pTErr[index];
    if (s.Contains("xyDCASignificance")) return this->trk_gdau_xyDCASignificance[index];
    if (s.Contains("zDCASignificance")) return this->trk_gdau_zDCASignificance[index];
    if (s.Contains("dedxHarmonic2")) return this->trk_gdau_dEdx_dedxHarmonic2[index];
    if (s.Contains("dEdxRes")) return this->trk_gdau_dEdxRes[index];
    if (s.Contains("dedxPixelHarmonic2")) return this->trk_gdau_dEdx_dedxPixelHarmonic2[index];
  } else if (s.Contains("Dau")) {
    auto index = TString(s[s.Length()-1]).Atoi();
    if (s.Contains("etaDau")) {
      if (flipEta)
        return -1*this->cand_etaDau[index];
      else
        return this->cand_etaDau[index];
    }
    if (s.Contains("massDau")) return this->cand_massDau[index];
    if (s.Contains("pTDau")) return this->cand_pTDau[index];
    if (s.Contains("phiDau")) return this->cand_phiDau[index];
  }

  if (s.Contains("mva")) {
    auto begPos = s.Index("[");
    ++begPos;
    auto endPos = s.Index("]");
    --endPos;
    TString num = s(begPos, endPos);
    int i = num.Atoi();
    return this->cand_MVA[i];
  }
  std::cerr << "[ERROR] Not found variable " << s <<" in NTuple" << std::endl;
  return 0;
}

void MyNTuple::pruneNTuple(const std::vector<TString>& keptBranchesVec)
{
#if 0
  // std::set_difference need sorted inputs
  std::set<TString> keptBranches(keptBranchesVec.begin(), keptBranchesVec.end());
  std::set<TString> branches;
  auto tbranches = t->GetListOfBranches();
  for (auto it = tbranches->begin(); it != tbranches->end(); ++it) {
    auto ins = branches.insert( (*it)->GetName() );
    if (ins.second) std::cout << "Branch " << *ins.first << " found" << std::endl;
  }

  std::set<TString> removedBranches;
  std::set_difference(branches.begin(), branches.end(),
                      keptBranches.begin(), keptBranches.end(),
                      std::inserter(removedBranches, removedBranches.end())
                      );
#endif
  t->SetBranchStatus("*", 0);
  for (const auto& b : keptBranchesVec) {
    std::cout << "Enabled branch " << b << std::endl;
    t->SetBranchStatus(b.Data(), 1);
  }
  TTree* oldtree = t->CloneTree(0);
  TTree* temp = t;
  t = oldtree;
}
#endif
