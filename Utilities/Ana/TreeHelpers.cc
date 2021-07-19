#ifndef TreeHelpers_H
#include "Ana/TreeHelpers.h"
#endif

#ifdef TreeHelpers_H
using std::vector;

bool checkDecayChain(Particle& par, unsigned short genIdx, const ParticleTreeMC& p,
                     const bool isFirstGen)
{
  // check pdg Id
  if (par.id() * p.gen_pdgId().at(genIdx) < 0 && isFirstGen) par.flipFlavor();
  if (par.id() != p.gen_pdgId().at(genIdx)) return false;

  // get daughters out
  const auto dauIdxs = p.gen_dauIdx().at(genIdx);

  // check number of daughters
  if (!par.longLived() && par.daughters().size() != dauIdxs.size()) return false;

  // if no daughters, both of these two have the same decay chain
  if (par.longLived() || par.daughters().size() == 0) {
      par.setTreeIdx(genIdx);
      return true;
  }

  // fill pdgIds and check them
  std::multiset<int> genDauPdgId; // check dau ID
  std::multiset<int> desiredPdgId; // check dau ID
  std::vector<int> vec_desiredPdgId; // for later permutation
  for (const auto idx : dauIdxs) {
    genDauPdgId.insert(p.gen_pdgId().at(idx));
  }
  for (const auto dau : par.daughters()) {
    desiredPdgId.insert(dau->id());
    vec_desiredPdgId.push_back(dau->id());
  }
  bool sameIds = desiredPdgId == genDauPdgId;
  if (!sameIds) return false; // they do not have the same set of decay products

  // if they have same daughter Ids
  // check their id order
  std::vector<unsigned int> idxs(vec_desiredPdgId.size());
  std::iota(std::begin(idxs), std::end(idxs), 0);
  do {
    bool sameOrder = true;
    for (size_t i=0;  i<idxs.size(); i++) {
      auto dIdx = dauIdxs[idxs[i]];
      sameOrder = sameOrder && (p.gen_pdgId().at(dIdx) == vec_desiredPdgId[i]);
    }
    if(sameOrder) {
      break;
    }
  } while(std::next_permutation(idxs.begin(), idxs.end()));

  // associated them if they are compatible
  bool sameChain = true;
  for (size_t i=0;  i<idxs.size(); i++) {
    sameChain = sameChain && checkDecayChain(*par.daughter(i), dauIdxs[idxs[i]], p, false);
    if (sameChain) par.daughter(i)->setTreeIdx( dauIdxs[idxs[i]] );
  }

  return sameChain;
}

PtEtaPhiM_t getRecoP4(size_t idx, const ParticleTreeMC& p)
{
  return PtEtaPhiM_t (
            p.cand_pT()[idx],
            p.cand_eta()[idx],
            p.cand_phi()[idx],
            p.cand_mass()[idx]
            );
}
PtEtaPhiM_t getRecoDauP4(size_t idx, size_t idau, const ParticleTreeMC& p)
{
  return PtEtaPhiM_t (
            p.cand_pTDau()[idx].at(idau),
            p.cand_etaDau()[idx].at(idau),
            p.cand_phiDau()[idx].at(idau),
            p.cand_massDau()[idx].at(idau)
            );
}

PtEtaPhiM_t getGenP4(size_t idx, const ParticleTreeMC& p)
{
  return PtEtaPhiM_t (
            p.gen_pT()[idx],
            p.gen_eta()[idx],
            p.gen_phi()[idx],
            p.gen_mass()[idx]
            );
}

void MyNTuple::setNDau(const unsigned short ndau,
                     const unsigned short ngdau,
                     const unsigned short* dauNGDau)
{
  nDau = ndau;
  nGDau = ngdau;
  for (unsigned short i=0; i<nDau; ++i) {
    dauHasNGDau[i] = dauNGDau[i];
  }
}

void MyNTuple::initMVABranches(const vector<TString>& methods)
{
  cand_nMVA = methods.size();
  for(size_t i=0; i<cand_nMVA; i++) {
    t->Branch(methods.at(i).Data(), &cand_MVA[i]);
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
  // particle level
  t->Branch("cand_charge", &cand_charge);
  t->Branch("cand_eta", &cand_eta);
  t->Branch("cand_y", &cand_y);
  t->Branch("cand_mass", &cand_mass);
  t->Branch("cand_pT", &cand_pT);
  t->Branch("cand_phi", &cand_phi);

  t->Branch("cand_matchGEN", &cand_matchGEN);
  t->Branch("cand_isSwap", &cand_isSwap);

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

  for (unsigned short iDau=0; iDau<nDau; ++iDau) {
    t->Branch(Form("cand_etaDau%d", iDau), &cand_etaDau[iDau]);
    t->Branch(Form("cand_massDau%d", iDau), &cand_massDau[iDau]);
    t->Branch(Form("cand_pTDau%d", iDau), &cand_pTDau[iDau]);
    t->Branch(Form("cand_phiDau%d", iDau), &cand_phiDau[iDau]);
  }

  // daughter level
  for (unsigned short iDau=0; iDau<nDau; ++iDau) {
    t->Branch(Form("cand_dau%d_matchGEN", iDau), &cand_dau_matchGEN[iDau]);
    t->Branch(Form("cand_dau%d_isSwap", iDau), &cand_dau_isSwap[iDau]);

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
      t->Branch(Form("trk_dau%d_dEdx_dedxHarmonic2", iDau), &trk_dau_dEdx_dedxHarmonic2[iDau]);
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
    t->Branch(Form("cand_gdau%d_matchGEN", iGDau), &cand_gdau_matchGEN[iGDau]);
    t->Branch(Form("cand_gdau%d_isSwap", iGDau), &cand_gdau_isSwap[iGDau]);

    t->Branch(Form("cand_gdau%d_charge", iGDau), &cand_gdau_charge[iGDau]);
    t->Branch(Form("cand_gdau%d_eta", iGDau), &cand_gdau_eta[iGDau]);
    t->Branch(Form("cand_gdau%d_mass", iGDau), &cand_gdau_mass[iGDau]);
    t->Branch(Form("cand_gdau%d_pT", iGDau), &cand_gdau_pT[iGDau]);
    // t->Branch(Form("cand_gdau%d_p", iGDau), &cand_gdau_p[iGDau]);
    t->Branch(Form("cand_gdau%d_phi", iGDau), &cand_gdau_phi[iGDau]);
    // t->Branch(Form("cand_gdau%d_y", iGDau), &cand_gdau_y[iGDau]);

    t->Branch(Form("trk_gdau%d_isHP", iGDau), &trk_gdau_isHP[iGDau]);
    t->Branch(Form("trk_gdau%d_nHit", iGDau), &trk_gdau_nHit[iGDau]);
    t->Branch(Form("trk_gdau%d_dEdx_dedxHarmonic2", iGDau), &trk_gdau_dEdx_dedxHarmonic2[iGDau]);
    t->Branch(Form("trk_gdau%d_dEdx_dedxPixelHarmonic2", iGDau), &trk_gdau_dEdx_dedxPixelHarmonic2[iGDau]);
    t->Branch(Form("trk_gdau%d_nChi2", iGDau), &trk_gdau_nChi2[iGDau]);
    t->Branch(Form("trk_gdau%d_pTErr", iGDau), &trk_gdau_pTErr[iGDau]);
    t->Branch(Form("trk_gdau%d_xyDCASignificance", iGDau), &trk_gdau_xyDCASignificance[iGDau]);
    t->Branch(Form("trk_gdau%d_zDCASignificance", iGDau), &trk_gdau_zDCASignificance[iGDau]);
  } // end granddaughter level
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
      this->trk_gdau_nChi2[gDauOffset] = p.trk_nChi2().at(trkIdx);
      this->trk_gdau_pTErr[gDauOffset] = p.trk_pTErr().at(trkIdx);
      this->trk_gdau_xyDCASignificance[gDauOffset] = p.trk_xyDCASignificance().at(trkIdx);
      this->trk_gdau_zDCASignificance[gDauOffset]  = p.trk_zDCASignificance().at(trkIdx);
      this->trk_gdau_dEdx_dedxHarmonic2[gDauOffset] = p.trk_dEdx_dedxHarmonic2().at(trkIdx);
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
      this->trk_dau_nChi2[iDau] = p.trk_nChi2().at(trkIdx);
      this->trk_dau_pTErr[iDau] = p.trk_pTErr().at(trkIdx);
      this->trk_dau_xyDCASignificance[iDau] = p.trk_xyDCASignificance().at(trkIdx);
      this->trk_dau_zDCASignificance[iDau] = p.trk_zDCASignificance().at(trkIdx);
      this->trk_dau_dEdx_dedxHarmonic2[iDau] = p.trk_dEdx_dedxHarmonic2().at(trkIdx);
      this->trk_dau_dEdx_dedxPixelHarmonic2[iDau] = p.trk_dEdx_dedxPixelHarmonic2().at(trkIdx);
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
  if (s == "cand_eta") return this->cand_eta;
  if (s == "cand_mass") return this->cand_mass;
  if (s == "cand_pT") return this->cand_pT;
  if (s == "cand_phi") return this->cand_phi;
  if (s == "cand_pseudoDecayLengthError2D") return this->cand_pseudoDecayLengthError2D;
  if (s == "cand_pseudoDecayLengthError3D") return this->cand_pseudoDecayLengthError3D;
  if (s == "cand_vtxChi2") return this->cand_vtxChi2;
  if (s == "cand_vtxProb") return this->cand_vtxProb;

  if (s.Contains("_dau")) {
    const auto index = s.Contains("cand") ? TString(s[8]).Atoi() : TString(s[7]).Atoi();

    if (s.Contains("Dau")) {
      auto gDauIndex = TString(s[s.Length()-1]).Atoi();
      if (s.Contains("etaDau")) return this->cand_dau_etaDau[index][gDauIndex];
      if (s.Contains("massDau")) return this->cand_dau_massDau[index][gDauIndex];
      if (s.Contains("pTDau")) return this->cand_dau_pTDau[index][gDauIndex];
      if (s.Contains("phiDau")) return this->cand_dau_phiDau[index][gDauIndex];
    } else {
      if (s.Contains("angle2D")) return this->cand_dau_angle2D[index];
      if (s.Contains("angle3D")) return this->cand_dau_angle3D[index];
      if (s.Contains("dca")) return this->cand_dau_dca[index];
      if (s.Contains("dcayLength2D")) return this->cand_dau_decayLength2D[index];
      if (s.Contains("dcayLength3D")) return this->cand_dau_decayLength3D[index];
      if (s.Contains("dcayLengthError2D")) return this->cand_dau_decayLengthError2D[index];
      if (s.Contains("dcayLengthError3D")) return this->cand_dau_decayLengthError3D[index];
      if (s.Contains("eta")) return this->cand_dau_eta[index];
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
    }
  } else if (s.Contains("_gdau")) {
    const auto index = s.Contains("cand") ? TString(s[9]).Atoi() : TString(s[8]).Atoi();
    if (s.Contains("eta")) return this->cand_gdau_eta[index];
    if (s.Contains("mass")) return this->cand_gdau_mass[index];
    if (s.Contains("pT")) return this->cand_gdau_pT[index];
    if (s.Contains("phi")) return this->cand_gdau_phi[index];
    if (s.Contains("nChi2")) return this->trk_gdau_nChi2[index];
    if (s.Contains("pTErr")) return this->trk_gdau_pTErr[index];
    if (s.Contains("xyDCASignificance")) return this->trk_gdau_xyDCASignificance[index];
    if (s.Contains("zDCASignificance")) return this->trk_gdau_zDCASignificance[index];
  } else if (s.Contains("Dau")) {
    auto index = TString(s[s.Length()-1]).Atoi();
    if (s.Contains("etaDau")) return this->cand_etaDau[index];
    if (s.Contains("massDau")) return this->cand_massDau[index];
    if (s.Contains("pTDau")) return this->cand_pTDau[index];
    if (s.Contains("phiDau")) return this->cand_phiDau[index];
  }
  std::cerr << "[ERROR] Not found variable " << s <<" in NTuple" << std::endl;
  return 0;
}
#endif
