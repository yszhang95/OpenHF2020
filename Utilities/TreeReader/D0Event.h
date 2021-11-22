//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jun 17 16:23:04 2019 by ROOT version 6.06/01
// from TTree Event/Event
// found on file: /storage1/users/wl33/D0Trees/Data/pPb_Tree_D0_default_BDTCutNewAndTrack_b1_v2.root
//////////////////////////////////////////////////////////

#ifndef D0Event_h
#define D0Event_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TObject.h>

// Header file for the classes stored in the TTree if any.

class D0Event {

public:
  D0Event(TTree *d0Collection=0, TTree *evt=0);
  virtual ~D0Event();
  virtual Long64_t GetEntriesFast() { return fChain_D0->GetEntriesFast(); }
  virtual Long64_t GetEntries();
  virtual Int_t    GetEntry(Long64_t entry);
  virtual void     Init(TTree *d0Collection, TTree* evt);
  virtual void     SetBranchStatus(const char* bname, Bool_t status);
  virtual Bool_t   GetBranchStatus(const char* bname);
  virtual Bool_t   Notify();

  virtual Int_t     nTrkOffline()  const {return ntrkoffline;}
  virtual Int_t     nPixel()       const {return Npixel;}
  virtual Float_t   HFSumET()      const {return HFsumET;}
  virtual Float_t   BestVtxX()     const {return bestvtxX_D0;}
  virtual Float_t   BestVtxY()     const {return bestvtxY_D0;}
  virtual Float_t   BestVtxZ()     const {return bestvtxZ_D0;}
  virtual Int_t     CandSize()     const {return candSize;}
  virtual Float_t   Pt(const unsigned int icand)           const {return pT[icand];}
  virtual Float_t   Y(const unsigned int icand)            const {return y[icand];}
  virtual Float_t   Mass(const unsigned int icand)         const {return mass[icand];}
  virtual Float_t   Mva(const unsigned int icand)          const {return mva[icand];}
  virtual Float_t   Flavor(const unsigned int icand)       const {return flavor[icand];}
  virtual Float_t   Eta(const unsigned int icand)          const {return eta[icand];}
  virtual Float_t   Phi(const unsigned int icand)          const {return phi[icand];}
  virtual Float_t   vtxProb(const unsigned int icand)      const {return VtxProb[icand];}
  virtual Float_t   CosPointingAngle3D(const unsigned int icand)
    const {return m3DCosPointingAngle[icand];}
  virtual Float_t  PointingAngle3D(const unsigned int icand)
    const {return m3DPointingAngle[icand];}
  virtual Float_t  CosPointingAngle2D(const unsigned int icand) 
    const {return m2DCosPointingAngle[icand];}
  virtual Float_t  PointingAngle2D(const unsigned int icand) 
    const {return m2DPointingAngle[icand];}
  virtual Float_t  DecayLSig3D(const unsigned int icand)  const {return m3DDecayLengthSignificance[icand];}
  virtual Float_t  DecayL3D(const unsigned int icand)     const {return m3DDecayLength[icand];}
  virtual Float_t  DecayLSig2D(const unsigned int icand)  const {return m2DDecayLengthSignificance[icand];}
  virtual Float_t  DecayL2D(const unsigned int icand)     const {return m2DDecayLength[icand];}
  virtual Float_t  zDCASigD1(const unsigned int icand)    const {return zDCASignificanceDaugther1[icand];}
  virtual Float_t  xyDCASigD1(const unsigned int icand)   const {return xyDCASignificanceDaugther1[icand];}
  virtual Float_t  nHitD1(const unsigned int icand)       const {return NHitD1[icand];}
  virtual Bool_t   highPurityD1(const unsigned int icand) const {return HighPurityDaugther1[icand];}
  virtual Float_t  PtD1(const unsigned int icand)         const {return pTD1[icand];}
  virtual Float_t  PtErrD1(const unsigned int icand)      const {return pTerrD1[icand];}
  virtual Float_t  etaD1(const unsigned int icand)        const {return EtaD1[icand];}
  virtual Float_t  phiD1(const unsigned int icand)        const {return PhiD1[icand];}
  virtual Float_t  DedxD1(const unsigned int icand)       const {return dedxHarmonic2D1[icand];}
  virtual Float_t  zDCASigD2(const unsigned int icand)    const {return zDCASignificanceDaugther2[icand];}
  virtual Float_t  xyDCASigD2(const unsigned int icand)   const {return xyDCASignificanceDaugther2[icand];}
  virtual Float_t  nHitD2(const unsigned int icand)       const {return NHitD2[icand];}
  virtual Bool_t   highPurityD2(const unsigned int icand) const {return HighPurityDaugther2[icand];}
  virtual Float_t  PtD2(const unsigned int icand)         const {return pTD2[icand];}
  virtual Float_t  PtErrD2(const unsigned int icand)      const {return pTerrD2[icand];}
  virtual Float_t  phiD2(const unsigned int icand)        const {return PhiD2[icand];}
  virtual Float_t  etaD2(const unsigned int icand)        const {return EtaD2[icand];}
  virtual Float_t  DedxD2(const unsigned int icand)       const {return dedxHarmonic2D2[icand];}

  virtual Bool_t    EvtSel(const unsigned int itype)       const {return evtSel[itype];}

protected :
   TTree          *fChain_D0;   //!pointer to the analyzed TTree or TChain of D0 candidates
   TTree          *fChain_evt;   //!pointer to the analyzed TTree or TChain of evts

// Fixed size dimensions of array or collections stored in the TTree if any.

  // Declaration of leaf types
  Int_t           Ntrkoffline;
  Int_t           ntrkoffline;
  Int_t           Npixel;
  Float_t         HFsumET;
  Float_t         bestvtxX_D0;
  Float_t         bestvtxY_D0;
  Float_t         bestvtxZ_D0;
  Int_t           candSize;
  Float_t         pT[100];   //[candSize]
  Float_t         y[100];   //[candSize]
  Float_t         eta[100]; //[cadSize]
  Float_t         phi[100];   //[candSize]
  Float_t         mass[100];   //[candSize]
  Float_t         mva[100];   //[candSize]
  Float_t         flavor[100];   //[candSize]
  Float_t         VtxProb[100];   //[candSize]
  Float_t         m3DCosPointingAngle[100];   //[candSize]
  Float_t         m3DPointingAngle[100];   //[candSize]
  Float_t         m2DCosPointingAngle[100];   //[candSize]
  Float_t         m2DPointingAngle[100];   //[candSize]
  Float_t         m3DDecayLengthSignificance[100];   //[candSize]
  Float_t         m3DDecayLength[100];   //[candSize]
  Float_t         m2DDecayLengthSignificance[100];   //[candSize]
  Float_t         m2DDecayLength[100];   //[candSize]
  Float_t         zDCASignificanceDaugther1[100];   //[candSize]
  Float_t         xyDCASignificanceDaugther1[100];   //[candSize]
  Float_t         NHitD1[100];   //[candSize]
  Bool_t          HighPurityDaugther1[100];   //[candSize]
  Float_t         pTD1[100];   //[candSize]
  Float_t         pTerrD1[100];   //[candSize]
  Float_t         EtaD1[100];   //[candSize]
  Float_t         PhiD1[100];   //[candSize]
  Float_t         dedxHarmonic2D1[100];   //[candSize]
  Float_t         zDCASignificanceDaugther2[100];   //[candSize]
  Float_t         xyDCASignificanceDaugther2[100];   //[candSize]
  Float_t         NHitD2[100];   //[candSize]
  Bool_t          HighPurityDaugther2[100];   //[candSize]
  Float_t         pTD2[100];   //[candSize]
  Float_t         pTerrD2[100];   //[candSize]
  Float_t         EtaD2[100];   //[candSize]
  Float_t         PhiD2[100];   //[candSize]
  Float_t         dedxHarmonic2D2[100];   //[candSize]

  // event selection
  Bool_t          evtSel[6];

   // List of branches
  TBranch        *b_Ntrkoffline;   //!
  TBranch        *b_ntrkoffline;   //!
  TBranch        *b_Npixel;   //!
  TBranch        *b_HFsumET;   //!
  TBranch        *b_bestvtxX_D0;   //!
  TBranch        *b_bestvtxY_D0;   //!
  TBranch        *b_bestvtxZ_D0;   //!
  TBranch        *b_candSize;   //!
  TBranch        *b_pT;   //!
  TBranch        *b_y;   //!
  TBranch        *b_eta;   //!
  TBranch        *b_phi;   //!
  TBranch        *b_mass;   //!
  TBranch        *b_mva;   //!
  TBranch        *b_flavor;   //!
  TBranch        *b_VtxProb;   //!
  TBranch        *b_3DCosPointingAngle;   //!
  TBranch        *b_3DPointingAngle;   //!
  TBranch        *b_2DCosPointingAngle;   //!
  TBranch        *b_2DPointingAngle;   //!
  TBranch        *b_3DDecayLengthSignificance;   //!
  TBranch        *b_3DDecayLength;   //!
  TBranch        *b_2DDecayLengthSignificance;   //!
  TBranch        *b_2DDecayLength;   //!
  TBranch        *b_zDCASignificanceDaugther1;   //!
  TBranch        *b_xyDCASignificanceDaugther1;   //!
  TBranch        *b_NHitD1;   //!
  TBranch        *b_HighPurityDaugther1;   //!
  TBranch        *b_pTD1;   //!
  TBranch        *b_pTerrD1;   //!
  TBranch        *b_EtaD1;   //!
  TBranch        *b_PhiD1;   //!
  TBranch        *b_dedxHarmonic2D1;   //!
  TBranch        *b_zDCASignificanceDaugther2;   //!
  TBranch        *b_xyDCASignificanceDaugther2;   //!
  TBranch        *b_NHitD2;   //!
  TBranch        *b_HighPurityDaugther2;   //!
  TBranch        *b_pTD2;   //!
  TBranch        *b_pTerrD2;   //!
  TBranch        *b_EtaD2;   //!
  TBranch        *b_PhiD2;   //!
  TBranch        *b_dedxHarmonic2D2;   //!

  TBranch        *b_evtSel;   //!
};
#endif
