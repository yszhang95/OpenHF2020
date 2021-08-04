//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 18 17:08:18 2021 by ROOT version 6.06/01
// from TTree ParticleTree/ParticleTree
// found on file: /eos/cms/store/group/phys_heavyions/yousen/RiceHIN/OpenHF2020_LamCKsP/MC/LambdaC-KsPr_LCpT-0p9_pPb-EmbEPOS_8p16_Pythia8/PA8TeV_pPb_LamCKsP0p9_pT0p9to6p1_y1p1_MC_Training_20210217/Merged_PA8TeV_pPb_LamCKsP0p9_pT0p9to6p1_y1p1_MC_Training_20210217.root
//////////////////////////////////////////////////////////

#ifndef ParticleTreeMC_hxx
#define ParticleTreeMC_hxx

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>

#include "ParticleTree.hxx"

class ParticleTreeMC : public ParticleTree {
public :
  ParticleTreeMC(TTree *tree=0);
  virtual ~ParticleTreeMC();

  bool isMC() const override { return true; }

  Int_t           Ntrkgen() const override{ return _Ntrkgen; }
  Float_t         genWeight() const override{ return _genWeight; }
  Float_t         pTHat() const override{ return _pTHat;}
  std::vector<bool>&    cand_matchGEN() const override{ return *_cand_matchGEN; }
  std::vector<bool>&    cand_momMatchGEN() const override{ return *_cand_momMatchGEN; }
  std::vector<int>&     cand_genPdgId() const override{ return *_cand_genPdgId; }
  std::vector<int>&     cand_isSwap() const override{ return *_cand_isSwap; }
  std::vector<unsigned short>& cand_genIdx() const override{ return *_cand_genIdx; }
  std::vector<unsigned int>& cand_momMatchIdx() const override{ return *_cand_momMatchIdx; }
  std::vector<char>&    gen_charge() const override{ return *_gen_charge; }
  std::vector<int>&     gen_pdgId() const override{ return *_gen_pdgId; }
  std::vector<unsigned char>& gen_status() const override{ return *_gen_status; }
  std::vector<unsigned short>& gen_statusBit() const override{ return *_gen_statusBit; }
  std::vector<float>&   gen_angle2D() const override{ return *_gen_angle2D; }
  std::vector<float>&   gen_angle3D() const override{ return *_gen_angle3D; }
  std::vector<float>&   gen_decayLength2D() const override{ return *_gen_decayLength2D; }
  std::vector<float>&   gen_decayLength3D() const override{ return *_gen_decayLength3D; }
  std::vector<float>&   gen_eta() const override{ return *_gen_eta; }
  std::vector<float>&   gen_mass() const override{ return *_gen_mass; }
  std::vector<float>&   gen_p() const override{ return *_gen_p; }
  std::vector<float>&   gen_pT() const override{ return *_gen_pT; }
  std::vector<float>&   gen_phi() const override{ return *_gen_phi; }
  std::vector<float>&   gen_y() const override{ return *_gen_y; }
  std::vector<std::vector<unsigned short>>& gen_dauIdx() const override{ return *_gen_dauIdx; }
  std::vector<std::vector<unsigned short>>& gen_momIdx() const override{ return *_gen_momIdx; }
  std::vector<std::vector<unsigned int>>& gen_candIdx() const override{ return *_gen_candIdx; }

  Float_t         pileup() const override { throw std::runtime_error("This interface can only be used for data"); };
  Float_t         rawInstLumi() const override { throw std::runtime_error("This interface can only be used for data"); };
  Float_t         recordLumi() const override { throw std::runtime_error("This interface can only be used for data"); };
  Float_t         totalLumi() const override { throw std::runtime_error("This interface can only be used for data"); };

  std::vector<bool>&    passHLT() const override { throw std::runtime_error("This interface can only be used for data"); };
  std::vector<bool>&    passHLTPrescaler() const override { throw std::runtime_error("This interface can only be used for data"); };
  std::vector<bool>&    passL1() const override { throw std::runtime_error("This interface can only be used for data"); };
  std::vector<bool>&    passL1Prescaler() const override { throw std::runtime_error("This interface can only be used for data"); };
  std::vector<unsigned char>& hltPDs() const override { throw std::runtime_error("This interface can only be used for data"); };
  std::vector<unsigned short>& hltPrescale() const override { throw std::runtime_error("This interface can only be used for data"); };
  std::vector<unsigned short>& l1Prescale() const override { throw std::runtime_error("This interface can only be used for data"); };
  std::vector<float>&   hltRecordLumi() const override { throw std::runtime_error("This interface can only be used for data"); };
  std::vector<float>&   hltTotalLumi() const override { throw std::runtime_error("This interface can only be used for data"); };

protected:
  virtual void     Init(TTree *tree);

private:
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain

  // Fixed size dimensions of array or collections stored in the TTree if any.

  // Declaration of leaf types
  Int_t           _Ntrkgen;
  Float_t         _genWeight;
  Float_t         _pTHat;
  std::vector<bool>    *_cand_matchGEN;
  std::vector<bool>    *_cand_momMatchGEN;
  std::vector<int>     *_cand_genPdgId;
  std::vector<int>     *_cand_isSwap;
  std::vector<unsigned short> *_cand_genIdx;
  std::vector<unsigned int> *_cand_momMatchIdx;
  std::vector<char>    *_gen_charge;
  std::vector<int>     *_gen_pdgId;
  std::vector<unsigned char> *_gen_status;
  std::vector<unsigned short> *_gen_statusBit;
  std::vector<float>   *_gen_angle2D;
  std::vector<float>   *_gen_angle3D;
  std::vector<float>   *_gen_decayLength2D;
  std::vector<float>   *_gen_decayLength3D;
  std::vector<float>   *_gen_eta;
  std::vector<float>   *_gen_mass;
  std::vector<float>   *_gen_p;
  std::vector<float>   *_gen_pT;
  std::vector<float>   *_gen_phi;
  std::vector<float>   *_gen_y;
  std::vector<std::vector<unsigned short> > *_gen_dauIdx;
  std::vector<std::vector<unsigned short> > *_gen_momIdx;
  std::vector<std::vector<unsigned int> > *_gen_candIdx;

  // List of branches
  TBranch        *b_Ntrkgen;   //!
  TBranch        *b_genWeight;   //!
  TBranch        *b_pTHat;   //!
  TBranch        *b_cand_matchGEN;   //!
  TBranch        *b_cand_momMatchGEN;   //!
  TBranch        *b_cand_genPdgId;   //!
  TBranch        *b_cand_isSwap;   //!
  TBranch        *b_cand_genIdx;   //!
  TBranch        *b_cand_momMatchIdx;   //!
  TBranch        *b_gen_charge;   //!
  TBranch        *b_gen_pdgId;   //!
  TBranch        *b_gen_status;   //!
  TBranch        *b_gen_statusBit;   //!
  TBranch        *b_gen_angle2D;   //!
  TBranch        *b_gen_angle3D;   //!
  TBranch        *b_gen_decayLength2D;   //!
  TBranch        *b_gen_decayLength3D;   //!
  TBranch        *b_gen_eta;   //!
  TBranch        *b_gen_mass;   //!
  TBranch        *b_gen_p;   //!
  TBranch        *b_gen_pT;   //!
  TBranch        *b_gen_phi;   //!
  TBranch        *b_gen_y;   //!
  TBranch        *b_gen_dauIdx;   //!
  TBranch        *b_gen_momIdx;   //!
  TBranch        *b_gen_candIdx;   //!

};

#endif
