#ifndef ParticleTreeData_cxx
#define ParticleTreeData_cxx
#include <iostream>
#include "Utilities/TreeReader/ParticleTreeData.hxx"

using std::cout;
using std::endl;

ParticleTreeData::ParticleTreeData(TTree *tree) : fChain(tree)
{
  // if parameter tree is not specified (or zero), connect the file
  // used to generate this class and read the Tree.
  if (tree == 0) {
    fChain == nullptr;
    return;
  }
  ParticleTree::Init(tree);
  Init(tree);
}

ParticleTreeData::~ParticleTreeData()
{
}

void ParticleTreeData::Init(TTree *tree)
{
  // The Init() function is called when the selector needs to initialize
  // a new tree or chain. Typically here the branch addresses and branch
  // pointers of the tree will be set.
  // It is normally not necessary to make changes to the generated
  // code, but the routine can be extended by the user if needed.
  // Init() will be called many times when running on PROOF
  // (once per file to be processed).

  // Set object pointer
  _passHLT = 0;
  _passHLTPrescaler = 0;
  _passL1 = 0;
  _passL1Prescaler = 0;
  _hltPDs = 0;
  _hltPrescale = 0;
  _l1Prescale = 0;
  _hltRecordLumi = 0;
  _hltTotalLumi = 0;

  // Set branch addresses and branch pointers
  if (!tree) return;
  fChain = tree;
  fCurrent = -1;
  fChain->SetMakeClass(1);

  fChain->SetBranchAddress("pileup", &_pileup, &b_pileup);
  fChain->SetBranchAddress("rawInstLumi", &_rawInstLumi, &b_rawInstLumi);
  fChain->SetBranchAddress("recordLumi", &_recordLumi, &b_recordLumi);
  fChain->SetBranchAddress("totalLumi", &_totalLumi, &b_totalLumi);

  fChain->SetBranchAddress("passHLT", &_passHLT, &b_passHLT);
  fChain->SetBranchAddress("passHLTPrescaler", &_passHLTPrescaler, &b_passHLTPrescaler);
  fChain->SetBranchAddress("passL1", &_passL1, &b_passL1);
  fChain->SetBranchAddress("passL1Prescaler", &_passL1Prescaler, &b_passL1Prescaler);
  fChain->SetBranchAddress("hltPDs", &_hltPDs, &b_hltPDs);
  fChain->SetBranchAddress("hltPrescale", &_hltPrescale, &b_hltPrescale);
  fChain->SetBranchAddress("l1Prescale", &_l1Prescale, &b_l1Prescale);
  fChain->SetBranchAddress("hltRecordLumi", &_hltRecordLumi, &b_hltRecordLumi);
  fChain->SetBranchAddress("hltTotalLumi", &_hltTotalLumi, &b_hltTotalLumi);

}
#endif
