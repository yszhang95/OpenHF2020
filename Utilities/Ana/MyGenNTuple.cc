#ifndef MyGenNTuple_H
#include "Utilities/Ana/MyGenNTuple.h"
#endif

#ifndef ParticleTreeMC_hxx
#include "Utilities/TreeReader/ParticleTreeMC.hxx"
#endif

#ifndef HelpClass_H
#include "Utilities/Ana/Common.h"
#endif

#ifndef TreeHelpers_H
#include "Utilities/Ana/TreeHelpers.h"
#endif

#ifdef MyGenNTuple_H
#include "TTree.h"
#include "TString.h"

using std::vector;

void MyGenNTuple::setNDau(const unsigned short ndau,
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
    std::cerr << "Total number of granddaughters does not match"
              << std::endl;
  }
}

void  MyGenNTuple::initGenBranches()
{
  // isPrompt
  t->Branch("gen_isPrompt", &gen_isPrompt);
  // particle level
  // t->Branch("gen_charge", &gen_charge);
  t->Branch("gen_pdgId", &gen_pdgId);
  t->Branch("gen_ancestorId", &gen_ancestorId);
  t->Branch("gen_ancestorIter", &gen_ancestorIter);
  t->Branch("gen_momPdgId", &gen_momPdgId);
  t->Branch("gen_eta", &gen_eta);
  t->Branch("gen_y", &gen_y);
  t->Branch("gen_mass", &gen_mass);
  t->Branch("gen_pT", &gen_pT);
  t->Branch("gen_phi", &gen_phi);

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

Int_t MyGenNTuple::fillNTuple()
{
  return t->Fill();
}

/**
// the following part has an issue to retrieve gen info
// because the treeidx may not be properly set
// I only test for the case taht Ks and proton are stable.
// Not sure what will happen if Ks is unstable, which
// means Ks.isLongLived() is False
*/

bool MyGenNTuple::retrieveGenInfo(ParticleTreeMC& p, Particle* ptr)
{
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
  gen_ancestorId = -99;
  gen_ancestorIter = -99;
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
  int iter = 0;
  for (auto momIdxs = p.gen_momIdx().at(it); !momIdxs.empty();
       momIdxs = p.gen_momIdx().at(ipar)) {
    if (momIdxs.size() > 1) {
      std::cerr << "Multiple mother particles" << std::endl;
    }
    iter++;
    auto momIdx = momIdxs.front();
    auto id = pdgId.at(momIdx);
    // must be absolute value
    std::string idstr = std::to_string(std::abs(id));
    if (idstr.at(0) == '5') {
      gen_isPrompt = false;
      gen_ancestorId = id;
      break;
    }
    gen_ancestorId = id;
    ipar = momIdx;
  }
  if (gen_isPrompt) {
    auto id = pdgId.at(ipar);
    if (abs(id) == 5) {
      gen_isPrompt = false;
      gen_ancestorId = id;
    }
    // if (abs(id) != 5) {
    //   auto dauIdxs = p.gen_dauIdx().at(ipar);
    //   for (const auto idx : dauIdxs) {
    //     std::cout << pdgId.at(idx) << "  ";
    //   }
    //   std::cout << std::endl;
    // }
  }
  this->gen_ancestorIter = iter;

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

float MyGenNTuple::value(const TString& s)
{
  if (s == "gen_angle2D") return this->gen_angle2D;
  if (s == "gen_angle3D") return this->gen_angle3D;
  if (s == "gen_decayLength2D") return this->gen_decayLength2D;
  if (s == "gen_decayLength3D") return this->gen_decayLength3D;
  if (s == "gen_eta") {
    if (flipEta)
      return -1*this->gen_eta;
    else
      return this->gen_eta;
  }
  if (s == "gen_mass") return this->gen_mass;
  if (s == "gen_pT") return this->gen_pT;
  if (s == "gen_y") {
    if (flipEta)
      return -1*this->gen_y;
    else
      return this->gen_y;
  }
  if (s == "gen_phi") return this->gen_phi;
  if (s.Contains("_dau")) {
    const auto index = TString(s[7]).Atoi();

    if (s.Contains("angle2D")) return this->gen_dau_angle2D[index];
    if (s.Contains("angle3D")) return this->gen_dau_angle3D[index];

    if (s.Contains("decayLength2D")) return this->gen_dau_decayLength2D[index];
    if (s.Contains("decayLength3D")) return this->gen_dau_decayLength3D[index];
    if (s.Contains("eta")) {
      if (flipEta)
        return -1*this->gen_dau_eta[index];
      else
        return this->gen_dau_eta[index];
    }
    if (s.Contains("mass")) return this->gen_dau_mass[index];
    if (s.Contains("pT")) return this->gen_dau_pT[index];
    if (s.Contains("phi")) return this->gen_dau_phi[index];
  } else if (s.Contains("_gdau")) {
    const auto index = TString(s[8]).Atoi();
    if (s.Contains("eta")) {
      if (flipEta)
        return -1*this->gen_gdau_eta[index];
      else
        return this->gen_gdau_eta[index];
    }
    if (s.Contains("mass")) return this->gen_gdau_mass[index];
    if (s.Contains("pT")) return this->gen_gdau_pT[index];
    if (s.Contains("phi")) return this->gen_gdau_phi[index];
  }

  std::cerr << "[ERROR] Not found variable "
            << s <<" in GenNTuple" << std::endl;
  return 0;
}

void MyGenNTuple::pruneNTuple(const std::vector<TString>& keptBranchesVec)
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
