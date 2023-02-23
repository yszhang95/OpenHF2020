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

void meanpt_vs_Nch(){

  const int num_bins = 20;
  int minNchCut[num_bins] = {0,5,10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100, 120, 150, 180, 220};
  int maxNchCut[num_bins] = {5,10, 15, 20, 25, 30, 35, 40, 45, 50, 60, 70, 80, 90, 100, 120, 150, 180, 220, 10000};


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
  int multBin = -1;

  Float_t trackPtErrOverPt[maxNch], trackDzOverDzSigma[maxNch]; 
  Float_t trackDxyOverDxySigma[maxNch], trackChiNdofOverNlayers[maxNch];
  Float_t trackNHits[maxNch];
  Float_t vz, vy, vx;
  Int_t centralityBin;

  Float_t ptsum, Mpt, meanpt;
  Float_t ptsumCorr, MptCorr, meanptCorr, NchCorr;
  Float_t Ma_2, Mb_2;

  TH1D::SetDefaultSumw2();
  TH1D *histMeanPT_2[num_bins];
  TH1D *histMeanPT_2Corr[num_bins];
  for(int i=0; i<num_bins; i++){
    histMeanPT_2[i] = new TH1D(Form("histMeanPT_2_%d",i), "", 330,-11,1000);
    histMeanPT_2Corr[i] = new TH1D(Form("histMeanPT_2Corr_%d",i), "", 330,-11,1000);
  }
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

      for(int imult=0;imult<num_bins;imult++){
        if((Nch>minNchCut[imult]&&Nch<=maxNchCut[imult])){
          multBin=imult;
        }
      }
      if(Nch==0) multBin=0;

      ptsum=0; Mpt=0;
      ptsumCorr=0; MptCorr=0;
      Ma_2=0; Mb_2=0;
      //particle loop
      for(int ip=0; ip<nParticle; ip++){
        if(pt[ip]>pt_min && pt[ip]<pt_max){
          if(fabs(eta[ip])<2.4){

            // track selections -> default
            if(trackPtErrOverPt[ip]>=0.1 || trackDzOverDzSigma[ip]>=3 || trackDxyOverDxySigma[ip]>=3) continue;

            if(eta[ip]>-0.5 && eta[ip]<0.5){
              ptsum += pt[ip];
              Mpt += 1.;
              weight = trkEff.getCorrection(pt[ip], eta[ip], centralityBin);
              ptsumCorr += pt[ip]*weight;
              MptCorr += weight;
              continue;
            }

            if(eta[ip]<-0.75){
              Ma_2+=1.;
            } else if(eta[ip]>0.75){
              Mb_2+=1.;
            }
          }
        }
      }

      if(Ma_2>1.9 && Mpt>1.9 && Mb_2>1.9){
        meanpt = ptsum/Mpt;
        histMeanPT_2[multBin]->Fill(meanpt);
        meanptCorr = ptsumCorr/MptCorr;
        histMeanPT_2Corr[multBin]->Fill(meanptCorr);
        histNchAllAnaCut->Fill(Nch);
        histNchCorrAllAnaCut->Fill(NchCorr);
      }

    }    
    delete infile;
  } 


  TFile *f = new TFile("output_meanpt.root","recreate");
  f->cd();

  for(int i=0; i<num_bins; i++){
    histMeanPT_2[i]->Write();
    histMeanPT_2Corr[i]->Write();
  }
  histNchAll->Write();
  histNchCorrAll->Write();
  histNchAllAnaCut->Write();
  histNchCorrAllAnaCut->Write();

  f->Write();
  f->Close();
  delete f;
}

