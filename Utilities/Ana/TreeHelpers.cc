#ifndef TreeHelpers_H
#include "Ana/TreeHelpers.h"
#endif

#ifdef TreeHelpers_H
// #include "ROOT/RVec.hxx"
using std::vector;

// bool checkDecayChain(Particle& par, unsigned short genIdx, const ParticleTreeMC& p,
//                      const bool isFirstGen)
bool checkDecayChain(Particle& par, unsigned short genIdx, const ParticleTree& p,
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

bool passEvent(const ParticleTree& p, const int pu, const int hlt)
{
  const bool passVtx = std::abs(p.bestvtxZ())<15.; // < 15cm
  const bool passPU = pu > 0 ? p.evtSel().at(pu) : true; // olv_dz_1 by default
  const bool passHLT = hlt > 0 ? p.passHLT().at(hlt) : true; // Multiplicity185_part* by default
  return passVtx && passPU && passHLT;
}


PtEtaPhiM_t getRecoP4(size_t idx, const ParticleTree& p)
{
  return PtEtaPhiM_t (
            p.cand_pT()[idx],
            p.cand_eta()[idx],
            p.cand_phi()[idx],
            p.cand_mass()[idx]
            );
}
PtEtaPhiM_t getRecoDauP4(size_t idx, size_t idau, const ParticleTree& p)
{
  return PtEtaPhiM_t (
            p.cand_pTDau()[idx].at(idau),
            p.cand_etaDau()[idx].at(idau),
            p.cand_phiDau()[idx].at(idau),
            p.cand_massDau()[idx].at(idau)
            );
}
PtEtaPhi_t getRecoDauP3(size_t idx, size_t idau, const ParticleTree& p)
{
  return PtEtaPhi_t (
            p.cand_pTDau()[idx].at(idau),
            p.cand_etaDau()[idx].at(idau),
            p.cand_phiDau()[idx].at(idau)
            );
}


// PtEtaPhiM_t getGenP4(size_t idx, const ParticleTreeMC& p)
PtEtaPhiM_t getGenP4(size_t idx, const ParticleTree& p)
{
  return PtEtaPhiM_t (
            p.gen_pT()[idx],
            p.gen_eta()[idx],
            p.gen_phi()[idx],
            p.gen_mass()[idx]
            );
}

#endif
