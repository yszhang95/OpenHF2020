#ifndef ParticleTreeMC_hxx
#include "TreeReader/ParticleTreeMC.hxx"
#endif

#ifndef HelpClass_H
#include "Ana/Common.h"
#endif

#ifndef TreeHelpers_H
#define TreeHelpers_H

#include <array>
#include <vector>
#include <tuple>
#include <set>

#include <algorithm>
#include <climits>
#include <numeric>

#include "Math/GenVector/LorentzVector.h"
#include "Math/GenVector/LorentzVectorfwd.h"
#include "Math/GenVector/PtEtaPhiM4D.h"
#include "Math/GenVector/VectorUtil.h"
#include "Math/Vector3D.h"

// typedefs
//using PtEtaPhiM_t = ROOT::Math::PtEtaPhiMVector;
typedef ROOT::Math::PtEtaPhiMVector PtEtaPhiM_t;
typedef ROOT::Math::RhoEtaPhiVector PtEtaPhi_t;

// functions
// bool checkDecayChain(Particle&, unsigned short,
//                      const ParticleTreeMC&, const bool isFirstGen=true);

bool checkDecayChain(Particle&, unsigned short,
                     const ParticleTree&, const bool isFirstGen=true);

PtEtaPhiM_t getRecoP4(size_t, const ParticleTree&);

PtEtaPhiM_t getRecoDauP4(size_t, size_t, const ParticleTree&);

PtEtaPhi_t getRecoDauP3(size_t, size_t, const ParticleTree&);

// PtEtaPhiM_t getGenP4(size_t, const ParticleTreeMC&);
PtEtaPhiM_t getGenP4(size_t, const ParticleTree&);

bool passEvent(const ParticleTree&, const int pu=4, const int hlt=2);

// classes and structs

class MatchCriterion {
  public:
    template <typename T>
      bool match (const T& reco, const T& gen)
      {
        const auto dR = ROOT::Math::VectorUtil::DeltaR(reco, gen);
        const auto dRelPt = TMath::Abs(gen.Pt() - reco.Pt())/gen.Pt();
        return dR < _deltaR && dRelPt < _deltaRelPt;
      }

    MatchCriterion (const float dR, const float dRelPt) :
      _deltaR(dR), _deltaRelPt(dRelPt) { };
    ~MatchCriterion() {}
    void SetDeltaRelPt(const float dRelPt) { _deltaRelPt = dRelPt; }
    void SetDeltaR(const float dR) { _deltaR = dR; }

  private:
    float _deltaR;
    float _deltaRelPt;
};

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
  Float_t         trk_gdau_dEdx_dedxPixelHarmonic2[100];
  Float_t         trk_gdau_nChi2[100];
  Float_t         trk_gdau_pTErr[100];
  Float_t         trk_gdau_xyDCASignificance[100];
  Float_t         trk_gdau_zDCASignificance[100];

  MyNTuple(TTree* t) : t(t),
                       nDau(0), nGDau(0),
                       cand_nMVA(0), dropDau(0),
                       flipEta(0)
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
  void  initMVABranches(const std::vector<TString>&);
  void  initNTuple();
  void  setNDau(const unsigned short, const unsigned short, unsigned short const*);
  Int_t fillNTuple();
  bool  setMVAValues(const std::vector<float>&);
  bool  retrieveTreeInfo(ParticleTree&, Long64_t);
  float value(const TString& s);
};


#endif
