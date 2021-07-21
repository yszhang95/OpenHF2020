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

  bool            isMC() const { return false; }

  Float_t         pileup() const { return _pileup; }
  Float_t         rawInstLumi() const { return _rawInstLumi; }
  Float_t         recordLumi() const { return _recordLumi; }
  Float_t         totalLumi() const { return _totalLumi; }

  std::vector<bool>&    passHLT() const { return *_passHLT; }
  std::vector<bool>&    passHLTPrescaler() const { return *_passHLTPrescaler; }
  std::vector<bool>&    passL1() const { return *_passL1; }
  std::vector<bool>&    passL1Prescaler() const { return *_passL1Prescaler; }
  std::vector<unsigned char>& hltPDs() const { return *_hltPDs; }
  std::vector<unsigned short>& hltPrescale() const { return *_hltPrescale; }
  std::vector<unsigned short>& l1Prescale() const { return *_l1Prescale; }
  std::vector<float>&   hltRecordLumi() const { return *_hltRecordLumi; }
  std::vector<float>&   hltTotalLumi() const { return *_hltTotalLumi; }


protected:
  virtual void     Init(TTree *tree);

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
