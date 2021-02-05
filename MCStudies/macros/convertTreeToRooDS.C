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

#include "Math/LorentzVector.h"
#include "Math/PtEtaPhiM4D.h"
#include "Math/Vector4Dfwd.h"
#include "TMath.h"

#include <iostream>
#include <memory>
#include <cmath>

const float massLambda = 1.115683;
const float massPion = 0.13957;
const float massProton = 0.93827;

void convertTreeToRooDS(std::string inputFileList="")
{
  using namespace RooFit;

  auto candMass = RooRealVar ( "Cand_Mass" , "Candidate Mass" , 0.0,  100.0 , "GeV/c^{2}" );
  auto candPt   = RooRealVar ( "Cand_Pt"   , "Candidate Pt"   , 0.0,  100.0 , "GeV/c" );
  auto candY    = RooRealVar ( "Cand_Y"    , "Candidate Y"    , -4.0, 4.0   , ""          );
  auto cols = RooArgSet(candMass, candPt, candY);
  auto data = RooDataSet("lamc", "lambda C", cols);

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
      // check if the ks is likely to be a lambda
      std::unique_ptr<ROOT::Math::PtEtaPhiMVector> pi1p4;
      std::unique_ptr<ROOT::Math::PtEtaPhiMVector> pi2p4;
      size_t nDau = (*tree.cand_dauIdx).size();

      int ksIdx = -1;
      int pIdx = -1;
      char ksDauCharge[2];

      if(abs((*tree.cand_pdgId)[i]) == 4122 && static_cast<unsigned short>((*tree.cand_status)[i]) == 3){
        for(unsigned int idau=0; idau<nDau; ++idau){
          auto dauIdx = (*tree.cand_dauIdx)[i][idau];
          if(std::abs((*tree.cand_pdgId)[dauIdx]) == 310){
            ksIdx = dauIdx;
            pIdx = (dauIdx+1) % 2;
  
            auto grandDauIdx1 = (*tree.cand_dauIdx)[dauIdx][0];
            auto grandDauIdx2 = (*tree.cand_dauIdx)[dauIdx][1];
  
            const float pt1 = (*tree.cand_pT)[grandDauIdx1];
            const float eta1 = (*tree.cand_eta)[grandDauIdx1];
            const float phi1 = (*tree.cand_phi)[grandDauIdx1];
            ksDauCharge[0] = (*tree.cand_charge)[grandDauIdx1];
  
            const float pt2 = (*tree.cand_pT)[grandDauIdx2];
            const float eta2 = (*tree.cand_eta)[grandDauIdx2];
            const float phi2 = (*tree.cand_phi)[grandDauIdx2];
            ksDauCharge[1] = (*tree.cand_charge)[grandDauIdx2];
  
            pi1p4 = std::unique_ptr<ROOT::Math::PtEtaPhiMVector>(new ROOT::Math::PtEtaPhiMVector(pt1, eta1, phi1, massPion));
            pi2p4 = std::unique_ptr<ROOT::Math::PtEtaPhiMVector>(new ROOT::Math::PtEtaPhiMVector(pt2, eta2, phi2, massPion));
  
            break;
          }
  
        }
        //std::cout << (*pi1p4 + *pi2p4).M()<< "  " << (*tree.cand_mass)[ksIdx] << std::endl;

        if(pi1p4->P() > pi2p4->P()) {
          pi1p4->SetM(massProton);
        } else {
          pi2p4->SetM(massProton);
        }
  
        double newCandM = (*pi1p4 + *pi2p4).M();
        if( std::abs(newCandM-massLambda) < 0.02) continue;
      }

      if((*tree.cand_matchGEN)[i] && !((*tree.cand_isSwap)[i]) && abs((*tree.cand_pdgId)[i]) == 4122 && static_cast<unsigned short>((*tree.cand_status)[i]) == 3){
        /*
        std::cout << "mass " << (*tree.cand_mass)[i] << std::endl;
        std::cout << "matchGEN " << (*tree.cand_matchGEN)[i] << std::endl;
        std::cout << "isSwap " << (*tree.cand_isSwap)[i] << std::endl;
        std::cout << "pdgId " << (*tree.cand_pdgId)[i] << std::endl;
        std::cout << "status " << static_cast<unsigned short>((*tree.cand_status)[i]) << std::endl;
        */
        candMass.setVal((*tree.cand_mass)[i]);
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

  RooWorkspace workspace("lamc", "lambda C");
  workspace.import(data);
  workspace.writeToFile("dataSet.root");

  //gDirectory->Print();
  gDirectory->Clear();
}
