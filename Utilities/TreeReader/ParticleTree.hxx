//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 18 17:08:18 2021 by ROOT version 6.06/01
// from TTree ParticleTree/ParticleTree
// found on file: /eos/cms/store/group/phys_heavyions/yousen/RiceHIN/OpenHF2020_LamCKsP/MC/LambdaC-KsPr_LCpT-0p9_pPb-EmbEPOS_8p16_Pythia8/PA8TeV_pPb_LamCKsP0p9_pT0p9to6p1_y1p1_MC_Training_20210217/Merged_PA8TeV_pPb_LamCKsP0p9_pT0p9to6p1_y1p1_MC_Training_20210217.root
//////////////////////////////////////////////////////////

#ifndef ParticleTree_hxx
#define ParticleTree_hxx

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>

class ParticleTree {
public:
  ParticleTree(TTree *tree=0);
  virtual ~ParticleTree();

  virtual Int_t    Cut(Long64_t entry);
  virtual Int_t    GetEntry(Long64_t entry);
  virtual Long64_t LoadTree(Long64_t entry);
  virtual Long64_t GetEntries();
  virtual Long64_t GetEntriesFast();
  virtual Bool_t   Notify();
  virtual void     Show(Long64_t entry = -1);

  virtual bool isMC() const = 0;

  UChar_t        nPV() const { return _nPV; }
  UShort_t       BXNb() const { return _BXNb; }
  UShort_t       Ntrkoffline() const { return _Ntrkoffline; }
  UInt_t         EventNb() const { return _EventNb; }
  UInt_t         LSNb() const { return _LSNb; }
  UInt_t         RunNb() const { return _RunNb; }
  Float_t        HFsumETMinus() const { return _HFsumETMinus; }
  Float_t        HFsumETPlus() const { return _HFsumETPlus; }
  Float_t        Npixel() const { return _Npixel; }
  Float_t        ZDCMinus() const { return _ZDCMinus; }
  Float_t        ZDCPlus() const { return _ZDCPlus; }
  Float_t        bestvtxX() const { return _bestvtxX; }
  Float_t        bestvtxY() const { return _bestvtxY; }
  Float_t        bestvtxZ() const { return _bestvtxZ; }
  std::vector<bool>&    evtSel() const { return *_evtSel; }
  std::vector<char>&    cand_charge() const { return *_cand_charge; }
  std::vector<int>&     cand_pdgId() const { return *_cand_pdgId; }
  std::vector<unsigned char>& cand_status() const { return *_cand_status; }
  std::vector<unsigned short>& cand_trkIdx() const { return *_cand_trkIdx; }
  std::vector<unsigned short>& cand_Ntrkoffline() const { return *_cand_Ntrkoffline; }
  std::vector<float>&   cand_angle2D() const { return *_cand_angle2D; }
  std::vector<float>&   cand_angle3D() const { return *_cand_angle3D; }
  std::vector<float>&   cand_dca() const { return *_cand_dca; }
  std::vector<float>&   cand_decayLength2D() const { return *_cand_decayLength2D; }
  std::vector<float>&   cand_decayLength3D() const { return *_cand_decayLength3D; }
  std::vector<float>&   cand_decayLengthError2D() const { return *_cand_decayLengthError2D; }
  std::vector<float>&   cand_decayLengthError3D() const { return *_cand_decayLengthError3D; }
  std::vector<float>&   cand_eta() const { return *_cand_eta; }
  std::vector<float>&   cand_mass() const { return *_cand_mass; }
  std::vector<float>&   cand_p() const { return *_cand_p; }
  std::vector<float>&   cand_pT() const { return *_cand_pT; }
  std::vector<float>&   cand_phi() const { return *_cand_phi; }
  std::vector<float>&   cand_pseudoDecayLengthError2D() const { return *_cand_pseudoDecayLengthError2D; }
  std::vector<float>&   cand_pseudoDecayLengthError3D() const { return *_cand_pseudoDecayLengthError3D; }
  std::vector<float>&   cand_vtxChi2() const { return *_cand_vtxChi2; }
  std::vector<float>&   cand_vtxProb() const { return *_cand_vtxProb; }
  std::vector<float>&   cand_y() const { return *_cand_y; }
  std::vector<std::vector<unsigned int>>& cand_dauIdx() const { return *_cand_dauIdx; }
  std::vector<std::vector<unsigned int>>& cand_momIdx() const { return *_cand_momIdx; }
  std::vector<std::vector<float>>& cand_etaDau() const { return *_cand_etaDau; }
  std::vector<std::vector<float>>& cand_massDau() const { return *_cand_massDau; }
  std::vector<std::vector<float>>& cand_pTDau() const { return *_cand_pTDau; }
  std::vector<std::vector<float>>& cand_phiDau() const {return *_cand_phiDau; }
  std::vector<bool>&    trk_isHP() const { return *_trk_isHP; }
  std::vector<unsigned short>& trk_nHit() const { return *_trk_nHit; }
  std::vector<unsigned short>& trk_nPixelHit() const { return *_trk_nPixelHit; }
  std::vector<unsigned short>& trk_nStripHit() const { return *_trk_nStripHit; }
  std::vector<float>&   trk_dEdx_dedxHarmonic2() const { return *_trk_dEdx_dedxHarmonic2; }
  std::vector<float>&   trk_dEdx_dedxPixelHarmonic2() const { return *_trk_dEdx_dedxPixelHarmonic2; }
  std::vector<float>&   trk_nChi2() const { return *_trk_nChi2; }
  std::vector<float>&   trk_pTErr() const { return *_trk_pTErr; }
  std::vector<float>&   trk_xyDCASignificance() const { return *_trk_xyDCASignificance; }
  std::vector<float>&   trk_zDCASignificance() const { return *_trk_zDCASignificance; }
  std::vector<std::vector<unsigned int>>& trk_candIdx() const { return *_trk_candIdx; }

  // MC specified
  virtual Int_t           Ntrkgen() const = 0;
  virtual Float_t         genWeight() const = 0;
  virtual Float_t         pTHat() const = 0;
  virtual std::vector<bool>&    cand_matchGEN() const = 0;
  virtual std::vector<bool>&    cand_momMatchGEN() const = 0;
  virtual std::vector<int>&     cand_genPdgId() const = 0;
  virtual std::vector<int>&     cand_isSwap() const = 0;
  virtual std::vector<unsigned short>& cand_genIdx() const = 0;
  virtual std::vector<unsigned int>& cand_momMatchIdx() const = 0;
  virtual std::vector<char>&    gen_charge() const  = 0;
  virtual std::vector<int>&     gen_pdgId() const  = 0;
  virtual std::vector<unsigned char>& gen_status() const  = 0;
  virtual std::vector<unsigned short>& gen_statusBit() const  = 0;
  virtual std::vector<float>&   gen_angle2D() const  = 0;
  virtual std::vector<float>&   gen_angle3D() const  = 0;
  virtual std::vector<float>&   gen_decayLength2D() const  = 0;
  virtual std::vector<float>&   gen_decayLength3D() const  = 0;
  virtual std::vector<float>&   gen_eta() const  = 0;
  virtual std::vector<float>&   gen_mass() const  = 0;
  virtual std::vector<float>&   gen_p() const  = 0;
  virtual std::vector<float>&   gen_pT() const  = 0;
  virtual std::vector<float>&   gen_phi() const  = 0;
  virtual std::vector<float>&   gen_y() const  = 0;
  virtual std::vector<std::vector<unsigned short>>& gen_dauIdx() const  = 0;
  virtual std::vector<std::vector<unsigned short>>& gen_momIdx() const  = 0;
  virtual std::vector<std::vector<unsigned int>>& gen_candIdx() const  = 0;

  // Data specified
  virtual  Float_t         pileup() const = 0;
  virtual  Float_t         rawInstLumi() const = 0;
  virtual  Float_t         recordLumi() const = 0;
  virtual  Float_t         totalLumi() const = 0;
  virtual  std::vector<bool>&    passHLT() const = 0;
  virtual  std::vector<bool>&    passHLTPrescaler() const = 0;
  virtual  std::vector<bool>&    passL1() const = 0;
  virtual  std::vector<bool>&    passL1Prescaler() const = 0;
  virtual  std::vector<unsigned char>& hltPDs() const = 0;
  virtual  std::vector<unsigned short>& hltPrescale() const = 0;
  virtual  std::vector<unsigned short>& l1Prescale() const = 0;
  virtual  std::vector<float>&   hltRecordLumi() const = 0;
  virtual  std::vector<float>&   hltTotalLumi() const = 0;

protected:
  void     Init(TTree *tree);

private:

  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain

  // Fixed size dimensions of array or collections stored in the TTree if any.

  // Declaration of leaf types

  UChar_t         _nPV;
  UShort_t        _BXNb;
  UShort_t        _Ntrkoffline;
  UInt_t          _EventNb;
  UInt_t          _LSNb;
  UInt_t          _RunNb;
  Float_t         _HFsumETMinus;
  Float_t         _HFsumETPlus;
  Float_t         _Npixel;
  Float_t         _ZDCMinus;
  Float_t         _ZDCPlus;
  Float_t         _bestvtxX;
  Float_t         _bestvtxY;
  Float_t         _bestvtxZ;
  std::vector<bool>    *_evtSel;
  std::vector<char>    *_cand_charge;
  std::vector<int>     *_cand_pdgId;
  std::vector<unsigned char> *_cand_status;
  std::vector<unsigned short> *_cand_trkIdx;
  std::vector<unsigned short> *_cand_Ntrkoffline;
  std::vector<float>   *_cand_angle2D;
  std::vector<float>   *_cand_angle3D;
  std::vector<float>   *_cand_dca;
  std::vector<float>   *_cand_decayLength2D;
  std::vector<float>   *_cand_decayLength3D;
  std::vector<float>   *_cand_decayLengthError2D;
  std::vector<float>   *_cand_decayLengthError3D;
  std::vector<float>   *_cand_eta;
  std::vector<float>   *_cand_mass;
  std::vector<float>   *_cand_p;
  std::vector<float>   *_cand_pT;
  std::vector<float>   *_cand_phi;
  std::vector<float>   *_cand_pseudoDecayLengthError2D;
  std::vector<float>   *_cand_pseudoDecayLengthError3D;
  std::vector<float>   *_cand_vtxChi2;
  std::vector<float>   *_cand_vtxProb;
  std::vector<float>   *_cand_y;
  std::vector<std::vector<unsigned int> > *_cand_dauIdx;
  std::vector<std::vector<unsigned int> > *_cand_momIdx;
  std::vector<std::vector<float> > *_cand_etaDau;
  std::vector<std::vector<float> > *_cand_massDau;
  std::vector<std::vector<float> > *_cand_pTDau;
  std::vector<std::vector<float> > *_cand_phiDau;
  std::vector<bool>    *_trk_isHP;
  std::vector<unsigned short> *_trk_nHit;
  std::vector<unsigned short> *_trk_nPixelHit;
  std::vector<unsigned short> *_trk_nStripHit;
  std::vector<float>   *_trk_dEdx_dedxHarmonic2;
  std::vector<float>   *_trk_dEdx_dedxPixelHarmonic2;
  std::vector<float>   *_trk_nChi2;
  std::vector<float>   *_trk_pTErr;
  std::vector<float>   *_trk_xyDCASignificance;
  std::vector<float>   *_trk_zDCASignificance;
  std::vector<std::vector<unsigned int> > *_trk_candIdx;

  // List of branches
  TBranch        *b_nPV;   //!
  TBranch        *b_BXNb;   //!
  TBranch        *b_Ntrkoffline;   //!
  TBranch        *b_EventNb;   //!
  TBranch        *b_LSNb;   //!
  TBranch        *b_RunNb;   //!
  TBranch        *b_Ntrkgen;   //!
  TBranch        *b_HFsumETMinus;   //!
  TBranch        *b_HFsumETPlus;   //!
  TBranch        *b_Npixel;   //!
  TBranch        *b_ZDCMinus;   //!
  TBranch        *b_ZDCPlus;   //!
  TBranch        *b_bestvtxX;   //!
  TBranch        *b_bestvtxY;   //!
  TBranch        *b_bestvtxZ;   //!
  TBranch        *b_evtSel;   //!
  TBranch        *b_cand_charge;   //!
  TBranch        *b_cand_pdgId;   //!
  TBranch        *b_cand_status;   //!
  TBranch        *b_cand_trkIdx;   //!
  TBranch        *b_cand_Ntrkoffline;   //!
  TBranch        *b_cand_angle2D;   //!
  TBranch        *b_cand_angle3D;   //!
  TBranch        *b_cand_dca;   //!
  TBranch        *b_cand_decayLength2D;   //!
  TBranch        *b_cand_decayLength3D;   //!
  TBranch        *b_cand_decayLengthError2D;   //!
  TBranch        *b_cand_decayLengthError3D;   //!
  TBranch        *b_cand_eta;   //!
  TBranch        *b_cand_mass;   //!
  TBranch        *b_cand_p;   //!
  TBranch        *b_cand_pT;   //!
  TBranch        *b_cand_phi;   //!
  TBranch        *b_cand_pseudoDecayLengthError2D;   //!
  TBranch        *b_cand_pseudoDecayLengthError3D;   //!
  TBranch        *b_cand_vtxChi2;   //!
  TBranch        *b_cand_vtxProb;   //!
  TBranch        *b_cand_y;   //!
  TBranch        *b_cand_dauIdx;   //!
  TBranch        *b_cand_momIdx;   //!
  TBranch        *b_cand_etaDau;   //!
  TBranch        *b_cand_massDau;   //!
  TBranch        *b_cand_pTDau;   //!
  TBranch        *b_cand_phiDau;   //!
  TBranch        *b_trk_isHP;   //!
  TBranch        *b_trk_nHit;   //!
  TBranch        *b_trk_nPixelHit;   //!
  TBranch        *b_trk_nStripHit;   //!
  TBranch        *b_trk_dEdx_dedxHarmonic2;   //!
  TBranch        *b_trk_dEdx_dedxPixelHarmonic2;   //!
  TBranch        *b_trk_nChi2;   //!
  TBranch        *b_trk_pTErr;   //!
  TBranch        *b_trk_xyDCASignificance;   //!
  TBranch        *b_trk_zDCASignificance;   //!
  TBranch        *b_trk_candIdx;   //!
};

#endif
