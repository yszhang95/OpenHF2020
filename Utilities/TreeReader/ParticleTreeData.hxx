//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Feb 18 17:08:18 2021 by ROOT version 6.06/01
// from TTree ParticleTree/ParticleTree
// found on file: /eos/cms/store/group/phys_heavyions/yousen/RiceHIN/OpenHF2020_LamCKsP/MC/LambdaC-KsPr_LCpT-0p9_pPb-EmbEPOS_8p16_Pythia8/PA8TeV_pPb_LamCKsP0p9_pT0p9to6p1_y1p1_MC_Training_20210217/Merged_PA8TeV_pPb_LamCKsP0p9_pT0p9to6p1_y1p1_MC_Training_20210217.root
//////////////////////////////////////////////////////////

#ifndef ParticleTreeData_hxx
#define ParticleTreeData_hxx

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>

#include "ParticleTree.hxx"

class ParticleTreeData : public ParticleTree {
public :
  ParticleTreeData(TTree *tree=0);
  virtual ~ParticleTreeData();

  bool            isMC() const override { return false; }

  Float_t         pileup() const override { return _pileup; }
  Float_t         rawInstLumi() const override { return _rawInstLumi; }
  Float_t         recordLumi() const override { return _recordLumi; }
  Float_t         totalLumi() const override { return _totalLumi; }

  std::vector<bool>&    passHLT() const override{ return *_passHLT; }
  std::vector<bool>&    passHLTPrescaler() const override{ return *_passHLTPrescaler; }
  std::vector<bool>&    passL1() const override{ return *_passL1; }
  std::vector<bool>&    passL1Prescaler() const override{ return *_passL1Prescaler; }
  std::vector<unsigned char>& hltPDs() const override{ return *_hltPDs; }
  std::vector<unsigned short>& hltPrescale() const override{ return *_hltPrescale; }
  std::vector<unsigned short>& l1Prescale() const override{ return *_l1Prescale; }
  std::vector<float>&   hltRecordLumi() const override{ return *_hltRecordLumi; }
  std::vector<float>&   hltTotalLumi() const override{ return *_hltTotalLumi; }

  Int_t           Ntrkgen() const override { throw std::runtime_error("Ntrkgen() can only be used for MC"); };
  Float_t         genWeight() const override { throw std::runtime_error("genWeight() can only be used for MC"); };
  Float_t         pTHat() const override { throw std::runtime_error("pTHat() can only be used for MC"); };
  std::vector<bool>&    cand_matchGEN() const override { throw std::runtime_error("cand_matchGEN() can only be used for MC"); };
  std::vector<bool>&    cand_momMatchGEN() const override { throw std::runtime_error("cand_momMatchGEN() can only be used for MC"); };
  std::vector<int>&     cand_genPdgId() const override { throw std::runtime_error("cand_genPdgId() can only be used for MC"); };
  std::vector<int>&     cand_isSwap() const override { throw std::runtime_error("cand_isSwap() can only be used for MC"); };
  std::vector<unsigned short>& cand_genIdx() const override { throw std::runtime_error("cand_genIdx() can only be used for MC"); };
  std::vector<unsigned int>& cand_momMatchIdx() const override { throw std::runtime_error("cand_momMatchIdx() can only be used for MC"); };
  std::vector<char>&    gen_charge() const override { throw std::runtime_error("gen_charge()can only be used for MC"); };
  std::vector<int>&     gen_pdgId() const override { throw std::runtime_error("gen_pdgId() can only be used for MC"); };
  std::vector<unsigned char>& gen_status() const override { throw std::runtime_error("gen_status() can only be used for MC"); };
  std::vector<unsigned short>& gen_statusBit() const override { throw std::runtime_error("gen_statusBit() can only be used for MC"); };
  std::vector<float>&   gen_angle2D() const override { throw std::runtime_error("gen_angle2D()can only be used for MC"); };
  std::vector<float>&   gen_angle3D() const override { throw std::runtime_error("gen_angle3D() can only be used for MC"); };
  std::vector<float>&   gen_decayLength2D() const override { throw std::runtime_error("gen_decayLength2() can only be used for MC"); };
  std::vector<float>&   gen_decayLength3D() const override { throw std::runtime_error("gen_decayLength3D() can only be used for MC"); };
  std::vector<float>&   gen_eta() const override { throw std::runtime_error("gen_eta() can only be used for MC"); };
  std::vector<float>&   gen_mass() const override { throw std::runtime_error("gen_mass() can only be used for MC"); };
  std::vector<float>&   gen_p() const override { throw std::runtime_error("gen_p() can only be used for MC"); };
  std::vector<float>&   gen_pT() const override { throw std::runtime_error("gen_pT() can only be used for MC"); };
  std::vector<float>&   gen_phi() const override { throw std::runtime_error("gen_phi() can only be used for MC"); };
  std::vector<float>&   gen_y() const override { throw std::runtime_error("gen_y() can only be used for MC"); };
  std::vector<std::vector<unsigned short>>& gen_dauIdx() const override { throw std::runtime_error("gen_dauIdx() can only be used for MC"); };
  std::vector<std::vector<unsigned short>>& gen_momIdx() const override { throw std::runtime_error("gen_momIdx() can only be used for MC"); };
  std::vector<std::vector<unsigned int>>& gen_candIdx() const override { throw std::runtime_error("gen_candIdx() can only be used for MC"); };


protected:
  void     Init (TTree *tree);

private:
  TTree          *fChain;   //!pointer to the analyzed TTree or TChain
  Int_t           fCurrent; //!current Tree number in a TChain

  // Fixed size dimensions of array or collections stored in the TTree if any.

  // Declaration of leaf types
  Float_t         _pileup;
  Float_t         _rawInstLumi;
  Float_t         _recordLumi;
  Float_t         _totalLumi;

  std::vector<bool>    *_passHLT;
  std::vector<bool>    *_passHLTPrescaler;
  std::vector<bool>    *_passL1;
  std::vector<bool>    *_passL1Prescaler;
  std::vector<unsigned char> *_hltPDs;
  std::vector<unsigned short> *_hltPrescale;
  std::vector<unsigned short> *_l1Prescale;
  std::vector<float>   *_hltRecordLumi;
  std::vector<float>   *_hltTotalLumi;


  // List of branches
  TBranch        *b_pileup;   //!
  TBranch        *b_rawInstLumi;   //!
  TBranch        *b_recordLumi;   //!
  TBranch        *b_totalLumi;   //!

  TBranch        *b_passHLT;   //!
  TBranch        *b_passHLTPrescaler;   //!
  TBranch        *b_passL1;   //!
  TBranch        *b_passL1Prescaler;   //!
  TBranch        *b_hltPDs;   //!
  TBranch        *b_hltPrescale;   //!
  TBranch        *b_l1Prescale;   //!
  TBranch        *b_hltRecordLumi;   //!
  TBranch        *b_hltTotalLumi;   //!
};

#endif
