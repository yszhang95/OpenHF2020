#include "TChain.h"
#include "TFile.h"
#include "THashList.h"
#include "TH1D.h"
#include "TFileCollection.h"
#include "../../Utilities/TreeReader/ParticleTree.hh"
#include "RooArgSet.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooWorkspace.h"

#include <iostream>
#include <memory>

void convertTreeToRooDS(std::string inputFileList="")
{
  using namespace RooFit;
  auto candMass      = RooRealVar ( "Cand_Mass"      , "Candidate Mass"           ,    -1.0 ,       100.0 , "GeV/c^{2}" );
  auto cols = RooArgSet(candMass);
  auto data = RooDataSet("lamc", "lambda C", cols);

  auto hmass = std::make_shared<TH1D>("hmass", "lambda c mass;Mass_{K_{s}^{0}p} (GeV);Events/ 5 MeV", 80, 2.0, 2.4);

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
      if((*tree.cand_matchGEN)[i] && !((*tree.cand_isSwap)[i]) && abs((*tree.cand_pdgId)[i]) == 4122 && static_cast<unsigned short>((*tree.cand_status)[i]) == 3){
        /*
        std::cout << "mass " << (*tree.cand_mass)[i] << std::endl;
        std::cout << "matchGEN " << (*tree.cand_matchGEN)[i] << std::endl;
        std::cout << "isSwap " << (*tree.cand_isSwap)[i] << std::endl;
        std::cout << "pdgId " << (*tree.cand_pdgId)[i] << std::endl;
        std::cout << "status " << static_cast<unsigned short>((*tree.cand_status)[i]) << std::endl;
        */
        candMass.setVal((*tree.cand_mass)[i]);
        hmass->Fill((*tree.cand_mass)[i]);
        data.add(cols);
      }
    }

    if(!tree.cand_mass) std::cout << "nullptr cand_mass" << std::endl;
    if(!tree.cand_matchGEN) std::cout << "nullptr cand_matchGEN" << std::endl;
    if(!tree.cand_isSwap) std::cout << "nullptr cand_isSwap" << std::endl;
    if(!tree.cand_pdgId) std::cout << "nullptr cand_pdgId" << std::endl;
    if(!tree.cand_status) std::cout << "nullptr cand_status" << std::endl;

    /*
    auto it_mass = tree.cand_mass->begin();
    auto it_matchGEN = tree.cand_matchGEN->begin();
    auto it_isSwap = tree.cand_isSwap->begin();
    auto it_pdgId = tree.cand_pdgId->begin();
    auto it_status = tree.cand_status->begin();
    for(; it_mass!=tree.cand_mass->end(); it_mass++){
      if(*it_matchGEN && !(*it_isSwap) && abs(*it_pdgId) == 4122 && *it_status == 3){
        std::cout << "mass " << *it_mass << std::endl;
        std::cout << "matchGEN " << *it_matchGEN << std::endl;
        std::cout << "isSwap " << *it_isSwap << std::endl;
        std::cout << "pdgId " << *it_pdgId << std::endl;
        std::cout << "status " << *it_status << std::endl;
      }
      it_matchGEN++;
      it_isSwap++;
      it_status++;
      it_pdgId++;
    }
    */

    //if(ientry > 5) break;
    if( ifile == tree.fCurrent) {
      std::cout << ifile++ << "th file done" << std::endl;
    }
  }

  RooWorkspace workspace("lamc", "lambda C");
  workspace.import(data);
  workspace.writeToFile("output.root");

  TFile ofile("hist.root", "recreate");
  hmass->Write();

  //gDirectory->Print();
  gDirectory->Clear();
}
