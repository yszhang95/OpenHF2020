//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Jul 21 15:27:49 2020 by ROOT version 6.06/01
// from TTree ParticleTree/ParticleTree
// found on file: /storage1/osg/stage_out/store/user/yousen/RiceHIN/pPb2016/LamCTree/VertexCompositeTree_lamc2ksppt5p9-Bst_lamCTest_20200720/LambdaC-KsPr_LCpT-5p9_PbP-EmbEPOS_8p16_Pythia8/VertexCompositeTree_lamc2ksppt5p9-Bst_lamCTest_20200720/200720_214651/0000/lambdacana_mc_10.root
//////////////////////////////////////////////////////////

#ifndef ParticleTree_h
#define ParticleTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class ParticleTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UChar_t         nPV;
   UShort_t        BXNb;
   UInt_t          EventNb;
   UInt_t          LSNb;
   UInt_t          RunNb;
   Int_t           Ntrkgen;
   Float_t         HFsumETMinus;
   Float_t         HFsumETPlus;
   Float_t         Npixel;
   Float_t         Ntrkoffline;
   Float_t         ZDCMinus;
   Float_t         ZDCPlus;
   Float_t         bestvtxX;
   Float_t         bestvtxY;
   Float_t         bestvtxZ;
   Float_t         genWeight;
   vector<bool>    *evtSel;
   vector<bool>    *passHLT;
   vector<bool>    *passL1;
   vector<bool>    *cand_matchGEN;
   vector<bool>    *cand_matchTRG0;
   vector<bool>    *cand_matchTRG1;
   vector<bool>    *cand_momMatchGEN;
   vector<char>    *cand_charge;
   vector<int>     *cand_genPdgId;
   vector<int>     *cand_isSwap;
   vector<int>     *cand_pdgId;
   vector<unsigned char> *cand_status;
   vector<unsigned short> *cand_genIdx;
   vector<unsigned short> *cand_momMatchIdx;
   vector<unsigned short> *cand_trkIdx;
   vector<float>   *cand_angle2D;
   vector<float>   *cand_angle3D;
   vector<float>   *cand_dca;
   vector<float>   *cand_decayLength2D;
   vector<float>   *cand_decayLength3D;
   vector<float>   *cand_decayLengthError2D;
   vector<float>   *cand_decayLengthError3D;
   vector<float>   *cand_eta;
   vector<float>   *cand_mass;
   vector<float>   *cand_p;
   vector<float>   *cand_pT;
   vector<float>   *cand_phi;
   vector<float>   *cand_pseudoDecayLengthError2D;
   vector<float>   *cand_pseudoDecayLengthError3D;
   vector<float>   *cand_vtxChi2;
   vector<float>   *cand_vtxProb;
   vector<float>   *cand_y;
   vector<vector<unsigned short> > *cand_dauIdx;
   vector<vector<unsigned short> > *cand_momIdx;
   vector<bool>    *trk_isHP;
   vector<unsigned short> *trk_nHit;
   vector<float>   *trk_dEdxHarmonic2;
   vector<float>   *trk_nChi2;
   vector<float>   *trk_pTErr;
   vector<float>   *trk_xyDCASignificance;
   vector<float>   *trk_zDCASignificance;
   vector<vector<unsigned short> > *trk_candIdx;
   vector<char>    *gen_charge;
   vector<int>     *gen_pdgId;
   vector<unsigned char> *gen_status;
   vector<unsigned short> *gen_statusBit;
   vector<float>   *gen_angle2D;
   vector<float>   *gen_angle3D;
   vector<float>   *gen_decayLength2D;
   vector<float>   *gen_decayLength3D;
   vector<float>   *gen_eta;
   vector<float>   *gen_mass;
   vector<float>   *gen_p;
   vector<float>   *gen_pT;
   vector<float>   *gen_phi;
   vector<float>   *gen_y;
   vector<vector<unsigned short> > *gen_candIdx;
   vector<vector<unsigned short> > *gen_dauIdx;
   vector<vector<unsigned short> > *gen_momIdx;

   // List of branches
   TBranch        *b_nPV;   //!
   TBranch        *b_BXNb;   //!
   TBranch        *b_EventNb;   //!
   TBranch        *b_LSNb;   //!
   TBranch        *b_RunNb;   //!
   TBranch        *b_Ntrkgen;   //!
   TBranch        *b_HFsumETMinus;   //!
   TBranch        *b_HFsumETPlus;   //!
   TBranch        *b_Npixel;   //!
   TBranch        *b_Ntrkoffline;   //!
   TBranch        *b_ZDCMinus;   //!
   TBranch        *b_ZDCPlus;   //!
   TBranch        *b_bestvtxX;   //!
   TBranch        *b_bestvtxY;   //!
   TBranch        *b_bestvtxZ;   //!
   TBranch        *b_genWeight;   //!
   TBranch        *b_evtSel;   //!
   TBranch        *b_passHLT;   //!
   TBranch        *b_passL1;   //!
   TBranch        *b_cand_matchGEN;   //!
   TBranch        *b_cand_matchTRG0;   //!
   TBranch        *b_cand_matchTRG1;   //!
   TBranch        *b_cand_momMatchGEN;   //!
   TBranch        *b_cand_charge;   //!
   TBranch        *b_cand_genPdgId;   //!
   TBranch        *b_cand_isSwap;   //!
   TBranch        *b_cand_pdgId;   //!
   TBranch        *b_cand_status;   //!
   TBranch        *b_cand_genIdx;   //!
   TBranch        *b_cand_momMatchIdx;   //!
   TBranch        *b_cand_trkIdx;   //!
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
   TBranch        *b_trk_isHP;   //!
   TBranch        *b_trk_nHit;   //!
   TBranch        *b_trk_dEdxHarmonic2;   //!
   TBranch        *b_trk_nChi2;   //!
   TBranch        *b_trk_pTErr;   //!
   TBranch        *b_trk_xyDCASignificance;   //!
   TBranch        *b_trk_zDCASignificance;   //!
   TBranch        *b_trk_candIdx;   //!
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
   TBranch        *b_gen_candIdx;   //!
   TBranch        *b_gen_dauIdx;   //!
   TBranch        *b_gen_momIdx;   //!

   ParticleTree(TTree *tree=0);
   virtual ~ParticleTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ParticleTree_cxx
ParticleTree::ParticleTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/storage1/osg/stage_out/store/user/yousen/RiceHIN/pPb2016/LamCTree/VertexCompositeTree_lamc2ksppt5p9-Bst_lamCTest_20200720/LambdaC-KsPr_LCpT-5p9_PbP-EmbEPOS_8p16_Pythia8/VertexCompositeTree_lamc2ksppt5p9-Bst_lamCTest_20200720/200720_214651/0000/lambdacana_mc_10.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/storage1/osg/stage_out/store/user/yousen/RiceHIN/pPb2016/LamCTree/VertexCompositeTree_lamc2ksppt5p9-Bst_lamCTest_20200720/LambdaC-KsPr_LCpT-5p9_PbP-EmbEPOS_8p16_Pythia8/VertexCompositeTree_lamc2ksppt5p9-Bst_lamCTest_20200720/200720_214651/0000/lambdacana_mc_10.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("/storage1/osg/stage_out/store/user/yousen/RiceHIN/pPb2016/LamCTree/VertexCompositeTree_lamc2ksppt5p9-Bst_lamCTest_20200720/LambdaC-KsPr_LCpT-5p9_PbP-EmbEPOS_8p16_Pythia8/VertexCompositeTree_lamc2ksppt5p9-Bst_lamCTest_20200720/200720_214651/0000/lambdacana_mc_10.root:/lambdacAna_mc");
      dir->GetObject("ParticleTree",tree);

   }
   Init(tree);
}

ParticleTree::~ParticleTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ParticleTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ParticleTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ParticleTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   evtSel = 0;
   passHLT = 0;
   passL1 = 0;
   cand_matchGEN = 0;
   cand_matchTRG0 = 0;
   cand_matchTRG1 = 0;
   cand_momMatchGEN = 0;
   cand_charge = 0;
   cand_genPdgId = 0;
   cand_isSwap = 0;
   cand_pdgId = 0;
   cand_status = 0;
   cand_genIdx = 0;
   cand_momMatchIdx = 0;
   cand_trkIdx = 0;
   cand_angle2D = 0;
   cand_angle3D = 0;
   cand_dca = 0;
   cand_decayLength2D = 0;
   cand_decayLength3D = 0;
   cand_decayLengthError2D = 0;
   cand_decayLengthError3D = 0;
   cand_eta = 0;
   cand_mass = 0;
   cand_p = 0;
   cand_pT = 0;
   cand_phi = 0;
   cand_pseudoDecayLengthError2D = 0;
   cand_pseudoDecayLengthError3D = 0;
   cand_vtxChi2 = 0;
   cand_vtxProb = 0;
   cand_y = 0;
   cand_dauIdx = 0;
   cand_momIdx = 0;
   trk_isHP = 0;
   trk_nHit = 0;
   trk_dEdxHarmonic2 = 0;
   trk_nChi2 = 0;
   trk_pTErr = 0;
   trk_xyDCASignificance = 0;
   trk_zDCASignificance = 0;
   trk_candIdx = 0;
   gen_charge = 0;
   gen_pdgId = 0;
   gen_status = 0;
   gen_statusBit = 0;
   gen_angle2D = 0;
   gen_angle3D = 0;
   gen_decayLength2D = 0;
   gen_decayLength3D = 0;
   gen_eta = 0;
   gen_mass = 0;
   gen_p = 0;
   gen_pT = 0;
   gen_phi = 0;
   gen_y = 0;
   gen_candIdx = 0;
   gen_dauIdx = 0;
   gen_momIdx = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("nPV", &nPV, &b_nPV);
   fChain->SetBranchAddress("BXNb", &BXNb, &b_BXNb);
   fChain->SetBranchAddress("EventNb", &EventNb, &b_EventNb);
   fChain->SetBranchAddress("LSNb", &LSNb, &b_LSNb);
   fChain->SetBranchAddress("RunNb", &RunNb, &b_RunNb);
   fChain->SetBranchAddress("Ntrkgen", &Ntrkgen, &b_Ntrkgen);
   fChain->SetBranchAddress("HFsumETMinus", &HFsumETMinus, &b_HFsumETMinus);
   fChain->SetBranchAddress("HFsumETPlus", &HFsumETPlus, &b_HFsumETPlus);
   fChain->SetBranchAddress("Npixel", &Npixel, &b_Npixel);
   fChain->SetBranchAddress("Ntrkoffline", &Ntrkoffline, &b_Ntrkoffline);
   fChain->SetBranchAddress("ZDCMinus", &ZDCMinus, &b_ZDCMinus);
   fChain->SetBranchAddress("ZDCPlus", &ZDCPlus, &b_ZDCPlus);
   fChain->SetBranchAddress("bestvtxX", &bestvtxX, &b_bestvtxX);
   fChain->SetBranchAddress("bestvtxY", &bestvtxY, &b_bestvtxY);
   fChain->SetBranchAddress("bestvtxZ", &bestvtxZ, &b_bestvtxZ);
   fChain->SetBranchAddress("genWeight", &genWeight, &b_genWeight);
   fChain->SetBranchAddress("evtSel", &evtSel, &b_evtSel);
   fChain->SetBranchAddress("passHLT", &passHLT, &b_passHLT);
   fChain->SetBranchAddress("passL1", &passL1, &b_passL1);
   fChain->SetBranchAddress("cand_matchGEN", &cand_matchGEN, &b_cand_matchGEN);
   fChain->SetBranchAddress("cand_matchTRG0", &cand_matchTRG0, &b_cand_matchTRG0);
   fChain->SetBranchAddress("cand_matchTRG1", &cand_matchTRG1, &b_cand_matchTRG1);
   fChain->SetBranchAddress("cand_momMatchGEN", &cand_momMatchGEN, &b_cand_momMatchGEN);
   fChain->SetBranchAddress("cand_charge", &cand_charge, &b_cand_charge);
   fChain->SetBranchAddress("cand_genPdgId", &cand_genPdgId, &b_cand_genPdgId);
   fChain->SetBranchAddress("cand_isSwap", &cand_isSwap, &b_cand_isSwap);
   fChain->SetBranchAddress("cand_pdgId", &cand_pdgId, &b_cand_pdgId);
   fChain->SetBranchAddress("cand_status", &cand_status, &b_cand_status);
   fChain->SetBranchAddress("cand_genIdx", &cand_genIdx, &b_cand_genIdx);
   fChain->SetBranchAddress("cand_momMatchIdx", &cand_momMatchIdx, &b_cand_momMatchIdx);
   fChain->SetBranchAddress("cand_trkIdx", &cand_trkIdx, &b_cand_trkIdx);
   fChain->SetBranchAddress("cand_angle2D", &cand_angle2D, &b_cand_angle2D);
   fChain->SetBranchAddress("cand_angle3D", &cand_angle3D, &b_cand_angle3D);
   fChain->SetBranchAddress("cand_dca", &cand_dca, &b_cand_dca);
   fChain->SetBranchAddress("cand_decayLength2D", &cand_decayLength2D, &b_cand_decayLength2D);
   fChain->SetBranchAddress("cand_decayLength3D", &cand_decayLength3D, &b_cand_decayLength3D);
   fChain->SetBranchAddress("cand_decayLengthError2D", &cand_decayLengthError2D, &b_cand_decayLengthError2D);
   fChain->SetBranchAddress("cand_decayLengthError3D", &cand_decayLengthError3D, &b_cand_decayLengthError3D);
   fChain->SetBranchAddress("cand_eta", &cand_eta, &b_cand_eta);
   fChain->SetBranchAddress("cand_mass", &cand_mass, &b_cand_mass);
   fChain->SetBranchAddress("cand_p", &cand_p, &b_cand_p);
   fChain->SetBranchAddress("cand_pT", &cand_pT, &b_cand_pT);
   fChain->SetBranchAddress("cand_phi", &cand_phi, &b_cand_phi);
   fChain->SetBranchAddress("cand_pseudoDecayLengthError2D", &cand_pseudoDecayLengthError2D, &b_cand_pseudoDecayLengthError2D);
   fChain->SetBranchAddress("cand_pseudoDecayLengthError3D", &cand_pseudoDecayLengthError3D, &b_cand_pseudoDecayLengthError3D);
   fChain->SetBranchAddress("cand_vtxChi2", &cand_vtxChi2, &b_cand_vtxChi2);
   fChain->SetBranchAddress("cand_vtxProb", &cand_vtxProb, &b_cand_vtxProb);
   fChain->SetBranchAddress("cand_y", &cand_y, &b_cand_y);
   fChain->SetBranchAddress("cand_dauIdx", &cand_dauIdx, &b_cand_dauIdx);
   fChain->SetBranchAddress("cand_momIdx", &cand_momIdx, &b_cand_momIdx);
   fChain->SetBranchAddress("trk_isHP", &trk_isHP, &b_trk_isHP);
   fChain->SetBranchAddress("trk_nHit", &trk_nHit, &b_trk_nHit);
   fChain->SetBranchAddress("trk_dEdxHarmonic2", &trk_dEdxHarmonic2, &b_trk_dEdxHarmonic2);
   fChain->SetBranchAddress("trk_nChi2", &trk_nChi2, &b_trk_nChi2);
   fChain->SetBranchAddress("trk_pTErr", &trk_pTErr, &b_trk_pTErr);
   fChain->SetBranchAddress("trk_xyDCASignificance", &trk_xyDCASignificance, &b_trk_xyDCASignificance);
   fChain->SetBranchAddress("trk_zDCASignificance", &trk_zDCASignificance, &b_trk_zDCASignificance);
   fChain->SetBranchAddress("trk_candIdx", &trk_candIdx, &b_trk_candIdx);
   fChain->SetBranchAddress("gen_charge", &gen_charge, &b_gen_charge);
   fChain->SetBranchAddress("gen_pdgId", &gen_pdgId, &b_gen_pdgId);
   fChain->SetBranchAddress("gen_status", &gen_status, &b_gen_status);
   fChain->SetBranchAddress("gen_statusBit", &gen_statusBit, &b_gen_statusBit);
   fChain->SetBranchAddress("gen_angle2D", &gen_angle2D, &b_gen_angle2D);
   fChain->SetBranchAddress("gen_angle3D", &gen_angle3D, &b_gen_angle3D);
   fChain->SetBranchAddress("gen_decayLength2D", &gen_decayLength2D, &b_gen_decayLength2D);
   fChain->SetBranchAddress("gen_decayLength3D", &gen_decayLength3D, &b_gen_decayLength3D);
   fChain->SetBranchAddress("gen_eta", &gen_eta, &b_gen_eta);
   fChain->SetBranchAddress("gen_mass", &gen_mass, &b_gen_mass);
   fChain->SetBranchAddress("gen_p", &gen_p, &b_gen_p);
   fChain->SetBranchAddress("gen_pT", &gen_pT, &b_gen_pT);
   fChain->SetBranchAddress("gen_phi", &gen_phi, &b_gen_phi);
   fChain->SetBranchAddress("gen_y", &gen_y, &b_gen_y);
   fChain->SetBranchAddress("gen_candIdx", &gen_candIdx, &b_gen_candIdx);
   fChain->SetBranchAddress("gen_dauIdx", &gen_dauIdx, &b_gen_dauIdx);
   fChain->SetBranchAddress("gen_momIdx", &gen_momIdx, &b_gen_momIdx);
   Notify();
}

Bool_t ParticleTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ParticleTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ParticleTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ParticleTree_cxx
