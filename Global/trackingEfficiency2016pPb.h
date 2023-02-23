#ifndef TRKEFF2016PPB
#define TRKEFF2016PPB

#include "TFile.h"
#include "TH2D.h"
#include "TH3F.h"
#include "TMath.h"
#include <iostream>
#include <string>

class TrkEff2016pPb{
public:

  TrkEff2016pPb( std::string collectionName = "general", std::string quality = "", bool isQuiet_ = false ,std::string filePath = "");
  ~TrkEff2016pPb();

  float getCorrection(float pt, float eta, int hiBin);
  float getEfficiency( float pt, float eta, int hiBin, bool passesCheck = false);

private:

  inline bool checkBounds(float pt, float eta, int hiBin);

  std::string mode;
  bool isQuiet;

  TFile * trkEff;

  TH2D * effPix[5];

  TH2D * effTable;

};

inline bool TrkEff2016pPb::checkBounds(float pt, float eta, int hiBin){
  if( TMath::Abs(eta) > 2.4 ){
    if( ! isQuiet) std::cout << "TrkEff2016pPb: track outside |eta|<2.4, please apply this cut!  I am returning a correction factor of 0 for this track for now." << std::endl;
    return false;
  }
  
  if( hiBin <0 || hiBin > 199){
    if( ! isQuiet) std::cout << "TrkEff2016pPb: hiBin is outside the range [0,199].  Please fix this issue!  I am returning a correction factor of 0 for this track for now." << std::endl;
    return false;
  }
  
  if( pt< 0 || pt > 500 ){
    if( ! isQuiet) std::cout << "TrkEff2016pPb: pT is outside the range [0,500].  I am returning a correction factor of 0 for this track for now." << std::endl;
    return false;
  }

  return true;
}

float TrkEff2016pPb::getCorrection(float pt, float eta, int hiBin){
  if( !checkBounds(pt, eta, hiBin) ) return 0;

  float efficiency = getEfficiency(pt, eta, hiBin, true);
  float fake = 0;  // eff is already eff/(1-fake) in the table

  //protect against dividing by 0
  if(efficiency > 0.001){
    return (1-fake)/efficiency;
  } else {
    if( ! isQuiet ) std::cout << "TrkEff2016pPb: Warning! Tracking efficiency is very low for this track (close to dividing by 0).  Returning correction factor of 0 for this track for now." << std::endl;
    return 0;
  }
}

float TrkEff2016pPb::getEfficiency( float pt, float eta, int hiBin, bool passesCheck){
  if( !passesCheck){
    if(  !checkBounds(pt, eta, hiBin) ) return 0;
  }

  if( mode.compare("general") == 0 || mode.compare("generalMB-") == 0 || mode.compare("generalMB+") == 0){
    //return eff->GetBinContent( eff->FindBin(eta, pt, hiBin) );
    return effTable->GetBinContent( effTable->FindBin(eta, pt) );
  }

  return 0;
}


TrkEff2016pPb::TrkEff2016pPb(std::string collectionName, std::string quality, bool isQuiet_, std::string filePath){
  isQuiet = isQuiet_;
  mode = collectionName;
  if( collectionName.compare("general") == 0 ){
    if(!isQuiet) std::cout << "TrkEff2016pPb class opening in general tracks mode!" << std::endl;
    
    if( quality.compare("Loose") == 0) trkEff = TFile::Open( (filePath + "Hijing_8TeV_MB_eff_v3_loose.root").c_str(),"open");
    else if( quality.compare("Tight") == 0) trkEff = TFile::Open( (filePath + "Hijing_8TeV_MB_eff_v3_tight.root").c_str(),"open");
    else if( quality.compare("Narrow") == 0) trkEff = TFile::Open( (filePath + "Hijing_8TeV_MB_eff_v4_narrow.root").c_str(),"open");
    else if( quality.compare("Wide") == 0) trkEff = TFile::Open( (filePath + "Hijing_8TeV_MB_eff_v4_wide.root").c_str(),"open");
    else trkEff = TFile::Open( (filePath + "Hijing_8TeV_dataBS.root").c_str(),"open");
    
    if(trkEff->IsZombie()){
      std::cout << "WARNING, COULD NOT FIND TRACK EFF FILE FOR GENERAL TRACKS!" << std::endl;
    } else {
      effTable = (TH2D*) trkEff->Get("rTotalEff3D_0");
    }
  
  }
}

TrkEff2016pPb::~TrkEff2016pPb(){
  trkEff->Close();
}

#endif
