#ifndef MyGenNTuple_H
#define MyGenNTuple_H
#include <iostream>
#include <vector>
#include "TTree.h"

#ifndef HelpClass_H
#include "Utilities/Ana/Common.h"
#endif

class TString;
class ParticleTree;
class ParticleTreeMC;

struct MyGenNTuple
{
  TTree* t;

  bool   flipEta;

  bool   dropDau;
  unsigned short  nDau;
  // daughter level
  unsigned short  nGDau;
  unsigned short  dauHasNGDau[100];

  // matched gen info
  bool            gen_isPrompt;
  Char_t          gen_charge;
  Int_t           gen_pdgId;
  Float_t         gen_angle3D;
  Float_t         gen_angle2D;
  Float_t         gen_decayLength3D;
  Float_t         gen_decayLength2D;
  Float_t         gen_mass;
  Float_t         gen_pT;
  Float_t         gen_eta;
  Float_t         gen_phi;
  Float_t         gen_y;
  Int_t           gen_momPdgId;
  // gen dau
  Int_t           gen_dau_pdgId[100];
  Float_t         gen_dau_angle3D[100];
  Float_t         gen_dau_angle2D[100];
  Float_t         gen_dau_decayLength3D[100];
  Float_t         gen_dau_decayLength2D[100];
  Float_t         gen_dau_pT[100];
  Float_t         gen_dau_eta[100];
  Float_t         gen_dau_phi[100];
  Float_t         gen_dau_mass[100];
  Float_t         gen_dau_y[100];
  // gen granddaughter
  Int_t           gen_gdau_pdgId[100];
  Float_t         gen_gdau_pT[100];
  Float_t         gen_gdau_eta[100];
  Float_t         gen_gdau_phi[100];
  Float_t         gen_gdau_mass[100];
  Float_t         gen_gdau_y[100];

  MyGenNTuple(TTree* t) : t(t), flipEta(0), dropDau(0),
                       nDau(0), nGDau(0)
  {
    if (!t) {
      std::cerr << "[ERROR] Nullptr of TTree is given to NTuple!"
                << std::endl;
    }
    for (auto& ngdau : dauHasNGDau) {
      ngdau = 0;
    }
  }
  ~MyGenNTuple()
  {
    std::cout << "Delete NTuple" << std::endl;
  }
  void  initGenBranches();
  void  setNDau(const unsigned short, const unsigned short,
                unsigned short const*);
  Int_t fillNTuple();
  bool  retrieveGenInfo(ParticleTreeMC&, Particle* ptr=nullptr);
  void  pruneNTuple(const std::vector<TString>&);
  float value(const TString&);
};

#endif
