#ifndef MyNTuple_H
#define MyNTuple_H
#include <iostream>
#include <vector>
#include "TTree.h"

#ifndef HelpClass_H
#include "Ana/Common.h"
#endif

class TTree;
// class DeDxSelection;
class TString;
class ParticleTree;
class ParticleTreeMC;
class ParticleTreeData;

struct MyNTuple
{
  TTree* t;

  bool   isMC;
  bool   flipEta;

  bool   dropDau;

  //  Event info
  //  UChar_t         nPV;
  //  UShort_t        BXNb;
  UShort_t        Ntrkoffline;
  UShort_t        cand_Ntrkoffline;
  UInt_t          EventNb;
  UInt_t          LSNb;
  UInt_t          RunNb;
  //  Float_t         HFsumETMinus;
  //  Float_t         HFsumETPlus;
  //  Float_t         Npixel;
  //  Float_t         ZDCMinus;
  //  Float_t         ZDCPlus;
  //  Float_t         bestvtxX;
  //  Float_t         bestvtxY;
  //  Float_t         bestvtxZ;
  Float_t         hltRecordLumi;

  UInt_t          nTrigs;
  Int_t           trigBit[100];
  UInt_t          nFilters;
  Int_t           filterBit[100];

  Float_t         eventWeight;

  UInt_t          cand_nMVA;
  Float_t         cand_MVA[100];

  Bool_t          cand_matchGEN;

  Char_t          cand_charge;
  Float_t         cand_eta;
  Float_t         cand_mass;
  //  Float_t         cand_p;
  Float_t         cand_pT;
  Float_t         cand_y;
  Float_t         cand_phi;

  Int_t           cand_isSwap;

  Float_t         cand_angle2D;
  Float_t         cand_angle3D;
  Float_t         cand_dca;
  Float_t         cand_decayLength2D;
  Float_t         cand_decayLength3D;
  Float_t         cand_decayLengthError2D;
  Float_t         cand_decayLengthError3D;
  Float_t         cand_pseudoDecayLengthError2D;
  Float_t         cand_pseudoDecayLengthError3D;
  Float_t         cand_vtxChi2;
  Float_t         cand_vtxProb;

  Float_t         cand_dauCosOpenAngle3D;
  Float_t         cand_dau_dR;

  unsigned short  nDau;
  Float_t         cand_etaDau[100];
  Float_t         cand_massDau[100];
  Float_t         cand_pTDau[100];
  Float_t         cand_phiDau[100];


  // daughter level
  unsigned short  nGDau;
  unsigned short  dauHasNGDau[100];

  Bool_t          cand_dau_matchGEN[100];
  Char_t          cand_dau_charge[100];
  Int_t           cand_dau_isSwap[100];

  // kinematics
  Float_t         cand_dau_eta[100];
  Float_t         cand_dau_mass[100];
  //  Float_t         cand_dau_p[100];
  Float_t         cand_dau_pT[100];
  Float_t         cand_dau_phi[100];
  //  Float_t         cand_dau_y[100];

  Float_t         cand_dau_etaDau[100][100];
  Float_t         cand_dau_pTDau[100][100];
  Float_t         cand_dau_massDau[100][100];
  Float_t         cand_dau_phiDau[100][100];

  Float_t         cand_dau_angle2D[100];
  Float_t         cand_dau_angle3D[100];
  Float_t         cand_dau_dca[100];
  Float_t         cand_dau_decayLength2D[100];
  Float_t         cand_dau_decayLength3D[100];
  Float_t         cand_dau_decayLengthError2D[100];
  Float_t         cand_dau_decayLengthError3D[100];
  Float_t         cand_dau_pseudoDecayLengthError2D[100];
  Float_t         cand_dau_pseudoDecayLengthError3D[100];
  Float_t         cand_dau_vtxChi2[100];
  Float_t         cand_dau_vtxProb[100];

  // track info
  Bool_t          trk_dau_isHP[100];
  UShort_t        trk_dau_nHit[100];
  UShort_t        trk_dau_nPixelHit[100];
  UShort_t        trk_dau_nStripHit[100];
  Float_t         trk_dau_dEdx_dedxHarmonic2[100];
  Float_t         trk_dau_dEdxRes[100];
  Float_t         trk_dau_dEdx_dedxPixelHarmonic2[100];
  Float_t         trk_dau_nChi2[100];
  Float_t         trk_dau_pTErr[100];
  Float_t         trk_dau_xyDCASignificance[100];
  Float_t         trk_dau_zDCASignificance[100];

  // granddaughter level
  Bool_t          cand_gdau_matchGEN[100];
  Char_t          cand_gdau_charge[100];
  Int_t           cand_gdau_isSwap[100];
  // kinematics
  Float_t         cand_gdau_eta[100];
  Float_t         cand_gdau_mass[100];
  // Float_t         cand_gdau_p[100];
  Float_t         cand_gdau_pT[100];
  Float_t         cand_gdau_phi[100];
  // Float_t         cand_gdau_y[100];

  // track info
  Bool_t          trk_gdau_isHP[100];
  UShort_t        trk_gdau_nHit[100];
  UShort_t        trk_gdau_nPixelHit[100];
  UShort_t        trk_gdau_nStripHit[100];
  Float_t         trk_gdau_dEdx_dedxHarmonic2[100];
  Float_t         trk_gdau_dEdxRes[100];
  Float_t         trk_gdau_dEdx_dedxPixelHarmonic2[100];
  Float_t         trk_gdau_nChi2[100];
  Float_t         trk_gdau_pTErr[100];
  Float_t         trk_gdau_xyDCASignificance[100];
  Float_t         trk_gdau_zDCASignificance[100];

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

  // correct for dEdx
  DeDxSelection dedxSel;

  // temporarily I know number of trigger bits is 5
  // and number of filter bits is 6
  MyNTuple(TTree* t) : t(t), flipEta(0), dropDau(0),
                       nTrigs(5), nFilters(6), cand_nMVA(0),
                       nDau(0), nGDau(0)
  {
    if (!t) {
      std::cerr << "[ERROR] Nullptr of TTree is given to NTuple!" << std::endl;
    }
    for (auto& ngdau : dauHasNGDau) {
      ngdau = 0;
    }
  }
  ~MyNTuple()
  {
    std::cout << "Delete NTuple" << std::endl;
  }
  void  initWeightBranch();
  void  setEventWeight(const float);
  void  initMVABranches(const std::vector<TString>&);
  void  initNTuple();
  void  initGenBranches();
  void  setNDau(const unsigned short, const unsigned short, unsigned short const*);
  Int_t fillNTuple();
  bool  setMVAValues(const std::vector<float>&);
  bool  retrieveTreeInfo(ParticleTree&, Long64_t);
  bool  retrieveGenInfo(ParticleTreeMC&, Particle* ptr=nullptr);
  float value(const TString& s);
  void  pruneNTuple(const std::vector<TString>&);
};

#endif
