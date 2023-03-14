// Run this macro first. It calculates the meanpt for each bin (which is required for the dynamic variance of meanpt variable in the next macro).
#include <iostream>
#include <fstream>
#include <vector>

#include <TFile.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TTree.h>
#include <TProfile.h>
#include <TComplex.h>

#include "trackingEfficiency2016pPb.h"

using namespace std;

void meanpt_vs_Nch(std::string inputTxt="NtrkCoarse.txt"){
  std::ifstream myrange(inputTxt);
  std::vector<int> edges;
  for (int i=0; myrange>>i; ) {
    edges.push_back(i);
  }


  // std::vector<int> minNchCut{2,  35, 60, 90,  120, 185};
  // std::vector<int> maxNchCut{35, 60, 90, 120, 185, 250};
  std::vector<int> minNchCut{};
  std::vector<int> maxNchCut{};
  for (size_t i=0; i<edges.size()-1; ++i) {
    minNchCut.push_back(edges.at(i));
    maxNchCut.push_back(edges.at(i+1));
  }

  std::vector<TH1*> hCorr(minNchCut.size());
  std::vector<TH1*> hOff(minNchCut.size());
  for (size_t itrk=0; itrk<minNchCut.size(); ++itrk) {
    std::string name_corr = ::Form("hNtrkCorr%dto%d", minNchCut.at(itrk), maxNchCut.at(itrk));
    std::string title_corr = ::Form("N_{trk}^{offline} %d--%d;N_{trk}^{corrected};Events",
        minNchCut.at(itrk), maxNchCut.at(itrk));
    hCorr.at(itrk) = new TH1D(name_corr.c_str(), title_corr.c_str(), 400, -0.5, 399.5);
    std::string name_off = ::Form("hNtrkOff%dto%d", minNchCut.at(itrk), maxNchCut.at(itrk));
    std::string title_off = ::Form("N_{trk}^{offline} %d--%d;N_{trk}^{offline};Events",
        minNchCut.at(itrk), maxNchCut.at(itrk));
    hOff.at(itrk) = new TH1D(name_off.c_str(), title_off.c_str(), 400, -0.5, 399.5);
  }

  ifstream flist;
  flist.open("listFiles_jb0.txt");

  char ifile[300];

  long  nEvt;
  const float pt_min = 0.5; //0.3
  const float pt_max = 5.; //2.0
  // const float pt_Nch_min = 0.5;
  const float pt_Nch_min = 0.4;
  // const float pt_Nch_max = 5.;
  const float pt_Nch_max = 1E5;

  const int maxNch=800;
  Float_t pt[maxNch], eta[maxNch], phi[maxNch], b;
  Int_t nParticle, Nch; 

  Float_t trackPtErrOverPt[maxNch], trackDzOverDzSigma[maxNch]; 
  Float_t trackDxyOverDxySigma[maxNch], trackChiNdofOverNlayers[maxNch];
  Float_t trackNHits[maxNch];
  Float_t vz, vy, vx;
  Int_t centralityBin;

  Float_t ptsum, Mpt, meanpt;
  Float_t ptsumCorr, MptCorr, meanptCorr, NchCorr;
  Float_t Ma_2, Mb_2;

  // TH1D::SetDefaultSumw2();
  TrkEff2016pPb trkEff =  TrkEff2016pPb("general", "", false, "root://eoscms.cern.ch//store/user/yousen/Ntrk_PPb/eff/");
  double weight=1;
  TH1D *histNchAll = new TH1D("histNchAll", "Nch", 500,0,500);
  TH1D *histNchCorrAll = new TH1D("histNchCorrAll", "Nch Corrected", 1000,0,1000);
  TH1D *histNchAllAnaCut = new TH1D("histNchAllAnaCut", "Nch with AnaCut", 500,0,500);
  TH1D *histNchCorrAllAnaCut = new TH1D("histNchCorrAllAnaCut", "Nch Corrected with AnaCut", 1000,0,1000);

  int nFile = 0;
  while( flist >> ifile ){
    nFile++;
    cout<<"nFile = "<< nFile << "; OPEN: " << ifile << endl;

    TFile *infile = TFile::Open(ifile,"read");

    TTree *t1 = (TTree*)infile->Get("demo/Tracks");
    t1->SetBranchAddress("nTrk", &nParticle);
    t1->SetBranchAddress("trackPhi", &phi);
    t1->SetBranchAddress("trackPt", &pt);
    t1->SetBranchAddress("trackEta", &eta);
    t1->SetBranchAddress("trackPtErrOverPt", &trackPtErrOverPt);
    t1->SetBranchAddress("trackDzOverDzSigma", &trackDzOverDzSigma);
    t1->SetBranchAddress("trackDxyOverDxySigma", &trackDxyOverDxySigma);
    //t1->SetBranchAddress("trackChiNdofOverNlayers", &trackChiNdofOverNlayers);
    //t1->SetBranchAddress("trackNHits", &trackNHits);
    t1->SetBranchAddress("vz", &vz);
    t1->SetBranchAddress("vy", &vy);
    t1->SetBranchAddress("vx", &vx);
    //t1->SetBranchAddress("centralityBin", &centralityBin);
    centralityBin = 0;

    nEvt = t1->GetEntries();
    //nEvt = 1000;

    for(int ne=0; ne<nEvt; ne++){
      t1->GetEntry(ne);


      if(ne%10000==0)  cout<<"Run "<<ne<<" of the total "<<nEvt<<" events; "<<endl;

      // event selections
      if(fabs(vz)>=15 || sqrt(vy*vy + vx*vx)>=0.2) continue;

      // find Nch
      Nch = 0;
      NchCorr = 0;
      for(int i=0; i<nParticle; i++){
        if(pt[i]>pt_Nch_min && pt[i]<pt_Nch_max){
          if(fabs(eta[i])>=2.4) continue;    

          // track selections -> default
          if(trackPtErrOverPt[i]>=0.1 || trackDzOverDzSigma[i]>=3 || trackDxyOverDxySigma[i]>=3) continue;

          Nch++; 

          weight = trkEff.getCorrection(pt[i], eta[i], centralityBin);
          NchCorr += weight; 
        }
      }
      if(Nch>10000) continue;

      histNchAll->Fill(Nch);
      histNchCorrAll->Fill(NchCorr);

      int multBin = -1;
      for(size_t imult=0;imult<minNchCut.size();imult++){
        if(Nch>=minNchCut[imult] && Nch<maxNchCut[imult]){
          multBin=imult;
          break;
        }
      }
      if (multBin<0) continue;
      hCorr.at(multBin)->Fill(NchCorr);
      hOff.at(multBin)->Fill(Nch);
    }    
    delete infile;
  } 

  for (size_t i=0; i<minNchCut.size(); ++i) {
    // std::cout << minNchCut.at(i) << " -- " << maxNchCut.at(i) << " " << hCorr.at(i)->GetMean() << "+/-" << hCorr.at(i)->GetMeanError() << " " << hOff.at(i)->GetMean() << "+/-" << hOff.at(i)->GetMeanError() << "\n";
    std::cout << minNchCut.at(i) << " -- " << maxNchCut.at(i) << "\t" << hOff.at(i)->GetMean() << "+/-" << hOff.at(i)->GetMeanError() << "\t" << hCorr.at(i)->GetMean() << "+/-" << hCorr.at(i)->GetMeanError() << "\n";
  }

  TFile *f = new TFile("output_Ntrk.root","recreate");
  f->cd();

  histNchAll->Write();
  histNchCorrAll->Write();
  for (auto h : hCorr) h->Write();
  for (auto h : hOff) h->Write();

  f->Close();
  delete f;
}

