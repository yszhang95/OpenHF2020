#include "TChain.h"
#include "TFile.h"
#include "THashList.h"
#include "TH1D.h"
#include "TFileCollection.h"
//#include "../../Utilities/TreeReader/ParticleTree.hh"
#include "ParticleTree.hh"
//#include "RooExtCBShape.cxx"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooWorkspace.h"

#include "Math/PtEtaPhiM4D.h"
#include "Math/Vector4Dfwd.h"
#include "Math/LorentzVector.h"
#include "TMath.h"

#include <iostream>
#include <memory>
#include <cmath>

const float massLambda = 1.115683;
const float massPion = 0.13957;
const float massProton = 0.93827;
const float massElectron = 0.511e-3;

void ksRooDataSet(std::string inputFileList="")
{
  using namespace RooFit;

  typedef ROOT::Math::PtEtaPhiM4D<float> LorentzPolarVectorF;

  auto candMisMass = RooRealVar ( "Cand_MisMass" , "mislabel Candidate Mass" , 0.0,  100.0 , "GeV/c^{2}" );
  auto candKsMass = RooRealVar ( "Cand_KsMass" , "ks Candidate Mass" , 0.0,  100.0 , "GeV/c^{2}" );
  auto candPt   = RooRealVar ( "Cand_Pt"   , "Candidate Pt"   , 0.0,  100.0 , "GeV/c" );
  auto candY    = RooRealVar ( "Cand_Y"    , "Candidate Y"    , -4.0, 4.0   , ""          );
  auto cols = RooArgSet(candKsMass, candMisMass, candPt, candY);
  auto data = RooDataSet("ks", "k short", cols);

  //auto hmass = std::make_shared<TH1D>("hmass", "lambda c mass;Mass_{K_{s}^{0}p} (GeV);Events/ 5 MeV", 80, 2.0, 2.4);

  TChain lamc("lambdacAna_mc/ParticleTree");
  TFileCollection fc ("fc", "the list of input files", inputFileList.c_str());
  lamc.AddFileInfoList(fc.GetList()); 

  ParticleTree tree(&lamc);
  //std::cout << tree.GetEntriesFast() << std::endl;

  Long64_t nentries = tree.GetEntriesFast();

  Long64_t nbytes = 0, nb = 0;

  int ifile=0;
  for (Long64_t jentry=0; jentry<nentries; jentry++) {
    Long64_t ientry = tree.LoadTree(jentry);
    if (ientry < 0) break;
    nb = tree.GetEntry(jentry);   nbytes += nb;

    for(size_t i=0; i < (tree.cand_mass->size()); i++){
      // check if ks is like lambda
      //std::unique_ptr<LorentzPolarVectorF> dau1p4;
      //std::unique_ptr<LorentzPolarVectorF> dau2p4;
      std::unique_ptr<ROOT::Math::PtEtaPhiMVector> dau1p4;
      std::unique_ptr<ROOT::Math::PtEtaPhiMVector> dau2p4;
      size_t nDau = (*tree.cand_dauIdx).size();

      char ksDauCharge[2];

      if(abs((*tree.cand_pdgId)[i]) == 310 && (*tree.cand_matchGEN)[i]){
  
        auto ksDauIdx1 = (*tree.cand_dauIdx)[i][0];
        auto ksDauIdx2 = (*tree.cand_dauIdx)[i][1];
  
        const float pt1 = (*tree.cand_pT)[ksDauIdx1];
        const float eta1 = (*tree.cand_eta)[ksDauIdx1];
        const float phi1 = (*tree.cand_phi)[ksDauIdx1];
        ksDauCharge[0] = (*tree.cand_charge)[ksDauIdx1];
  
        const float pt2 = (*tree.cand_pT)[ksDauIdx2];
        const float eta2 = (*tree.cand_eta)[ksDauIdx2];
        const float phi2 = (*tree.cand_phi)[ksDauIdx2];
        ksDauCharge[1] = (*tree.cand_charge)[ksDauIdx2];
  
        //dau1p4 = std::unique_ptr<LorentzPolarVectorF>(new LorentzPolarVectorF(pt1, eta1, phi1, massPion));
        //dau2p4 = std::unique_ptr<LorentzPolarVectorF>(new LorentzPolarVectorF(pt2, eta2, phi2, massPion));
        dau1p4 = std::unique_ptr<ROOT::Math::PtEtaPhiMVector>(new ROOT::Math::PtEtaPhiMVector(pt1, eta1, phi1, massPion));
        dau2p4 = std::unique_ptr<ROOT::Math::PtEtaPhiMVector>(new ROOT::Math::PtEtaPhiMVector(pt2, eta2, phi2, massPion));

        dau1p4->SetM(massElectron);
        dau2p4->SetM(massElectron);
  
        //double newCandMOpt = std::sqrt( std::pow(dau1p4->E()+dau2p4->E(), 2) - 
            //std::pow(dau1p4->Px()+dau2p4->Px(), 2) - std::pow(dau1p4->Py()+dau2p4->Py(), 2) - std::pow(dau1p4->Py()+dau2p4->Py(), 2) );

        double newCandM = (*dau1p4 + *dau2p4).M();

        candMisMass.setVal( newCandM );
        candKsMass.setVal( (*tree.cand_mass)[i]);
        candPt.setVal((*tree.cand_pT)[i]);
        candY.setVal((*tree.cand_y)[i]);
        //hmass->Fill((*tree.cand_mass)[i]);
        data.add(cols);
      }
    }

    if(!tree.cand_mass) std::cout << "nullptr cand_mass" << std::endl;
    if(!tree.cand_matchGEN) std::cout << "nullptr cand_matchGEN" << std::endl;
    if(!tree.cand_isSwap) std::cout << "nullptr cand_isSwap" << std::endl;
    if(!tree.cand_pdgId) std::cout << "nullptr cand_pdgId" << std::endl;
    if(!tree.cand_status) std::cout << "nullptr cand_status" << std::endl;

    //if(ientry > 5) break;
    if( ifile == tree.fCurrent) {
      std::cout << "The first entry of " << ifile++ << "th file done" << std::endl;
    }
  }

  RooWorkspace workspace("ks", "ks");
  workspace.import(data);
  workspace.writeToFile("ksdataSet.root");

  //gDirectory->Print();
  gDirectory->Clear();
}
