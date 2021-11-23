#include <fstream>
#include <iostream>
#include <string>

#include "TFile.h"
#include "TH1D.h"
#include "TString.h"
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "TMath.h"

using std::cout;
using std::endl;

struct Conf
{
  std::string _inFileList;
  std::string _wsName;
  std::string _wsTitle;
  std::string _dsName;
  std::string _dsTitle;
  std::string _outDir;
  std::string _ofile;
  bool _reweight;
};

void MergeWS(Conf conf) {

  auto inFileList = conf._inFileList;
  auto wsName = conf._wsName;
  auto wsTitle = conf._wsTitle;
  auto dsName = conf._dsName;
  auto dsTitle = conf._dsTitle;
  auto outDir = conf._outDir.size() ? conf._outDir : "./";
  auto ofile = conf._ofile;
  auto reweight = conf._reweight;

  if (reweight) cout << "\n[INFO] reweighting will be used\n" << endl;

  TFile* outputFile = TFile::Open((outDir+"/"+ofile).c_str(), "recreate");
  cout << "\n[INFO] Preparing output " << outputFile->GetName() << "\n" << endl;

  if (reweight) TH1::SetDefaultSumw2(kTRUE);
  TH1D hNtrkoffline("hNtrkoffline", "N_{trk}^{offline} for PV with highest N;N_{trk}^{offline};", 300, 0., 300.);
  TH1D hNtrkofflineDz1p0("hNtrkofflineDz1p0", "N_{trk}^{offline} for PV with highest N, dz1p0;N_{trk}^{offline};", 300, 0., 300.);
  TH1D hNtrkofflineGplus("hNtrkofflineGplus", "N_{trk}^{offline} for PV with highest N, Gplus;N_{trk}^{offline};", 300, 0., 300.);

  RooAbsData::setDefaultStorageType(RooAbsData::Tree);
  RooRealVar NtrkPV("Ntrkoffline", "Ntrkoffline w.r.t. PV with highest N", 0, 400, "");
  // LambdaC kinematic info
  RooRealVar cand_mass("cand_mass", "mass of #Lambda_{c}^{+} candidate", 2.15, 2.45, "GeV");
  RooRealVar cand_pT  ("cand_pT", "p_{T} of #Lambda_{c}^{+} candidate",  1.9, 100., "GeV");
  RooRealVar cand_eta("cand_eta", "#eta of #Lambda_{c}^{+} candidate", -2.4, 2.4, "");
  RooRealVar cand_y("cand_y", "rapidity of #Lambda_{c}^{+} candidate", -1.0, 1.0, "");
  const double piVal = TMath::Pi();
  RooRealVar cand_phi("cand_phi", "#phi of #Lambda_{c}^{+} candidate", -piVal, piVal, "");
  // LambdaC decay info
  RooRealVar cand_decayLength3D("cand_decayLength3D", "3D decay length of #Lambda_{c}^{+} candidate", 0, 20, "cm");
  RooRealVar cand_decayLengthSig3D("cand_decayLength3DSig", "3D decay length significance of #Lambda_{c}^{+} candidate", 0, 50);
  RooRealVar cand_cosAngle3D("cand_cosAngle3D", "Cosine of 3D pointing angle of #Lambda_{c}^{+} candidate", -1., 1.);
  // Ks kinematic info
  RooRealVar cand_dau0_mass("cand_dau0_mass", "mass of K_{S}^{0}", 0.46, 0.54, "GeV");
  RooRealVar cand_dau0_pT("cand_dau0_pT", "p_{T} of K_{S}^{0}", 0, 100, "GeV");
  RooRealVar cand_dau0_eta("cand_dau0_eta", "#eta of K_{S}^{0}", -2.4, 2.4, "");
  RooRealVar cand_dau0_phi("cand_dau0_phi", "#phi of K_{S}^0", -piVal, piVal, "");
  // proton info
  RooRealVar cand_dau1_pT("cand_dau1_pT", "p_{T} of proton", 0, 100, "GeV");
  RooRealVar cand_dau1_eta("cand_dau1_eta", "#eta of proton", -2.4, 2.4, "");
  RooRealVar cand_dau1_phi("cand_dau1_phi", "#phi of proton", -piVal, piVal, "");
  RooRealVar trk_dau1_dEdxRes("trk_dau1_dEdxRes", "dE/dx residual for proton", -1., 1., "");
  // trigger bits
  RooRealVar passHM("trigBit_2", "pass HM185", -1, 2, "");
  RooRealVar passMB("trigBit_4", "pass MB", -1, 2, "");
  RooRealVar passDz1p0("filterBit_4", "pass dz1p0", -1, 2, "");
  RooRealVar passGplus("filterBit_5", "pass Gplus", -1, 2, "");
  // MVA
  RooRealVar MVA("cand_mva", "first method in MVA collection", -1, 1, "");
  // at most 9 variables in old ROOT
  RooArgSet cols(cand_mass, cand_pT, cand_eta, cand_y, cand_phi,
                 cand_decayLength3D, cand_decayLengthSig3D, cand_cosAngle3D);
  cols.add(cand_dau0_mass); cols.add(cand_dau0_pT);
  cols.add(cand_dau0_eta); cols.add(cand_dau0_phi);
  cols.add(cand_dau1_pT); cols.add(cand_dau1_eta);
  cols.add(cand_dau1_phi); cols.add(trk_dau1_dEdxRes);
  cols.add(NtrkPV); cols.add(passHM); cols.add(passMB);
  cols.add(passDz1p0); cols.add(passGplus);
  cols.add(MVA);
  // weight
  RooRealVar wgtVar("weight", "event weight", 0, 100);
  if (reweight) cols.add(wgtVar);
  // workspace and dataset
  RooWorkspace ws(("merged_" + wsName).c_str(), wsTitle.c_str());
  const char* wgtVarName = reweight ? "weight" : 0;
  RooDataSet ds(("merged_" + dsName).c_str(), dsTitle.c_str(),
                  cols, wgtVarName);
  outputFile->Add(&ws);
  outputFile->Add(&ds);
  cout << "\n[INFO] Created output " << outputFile->GetName() << "\n" << endl;

  cout << "\n[INFO] Looping over files" << endl;
  std::ifstream infiles(inFileList.c_str());
  for (std::string str; std::getline(infiles, str, '\n'); ) {
    if (str.at(0) == '#') continue;
    cout << str << endl;

    TFile* infile = TFile::Open(str.c_str(), "READ");

    if (str.find("ws.root") == std::string::npos) {
      TH1D *hNtrk(0), *hNtrkDz1(0), *hNtrkGplus(0);
      infile->GetObject("lambdacAna/hNtrkoffline", hNtrk);
      infile->GetObject("lambdacAna/hNtrkofflineDz1p0", hNtrkDz1);
      infile->GetObject("lambdacAna/hNtrkofflineGplus", hNtrkGplus);
      hNtrkoffline.Add(hNtrk);
      hNtrkofflineDz1p0.Add(hNtrkDz1);
      hNtrkofflineGplus.Add(hNtrkGplus);
    } else {
      RooWorkspace* wstemp;
      infile->GetObject(wsName.c_str(), wstemp);
      auto dstemp = dynamic_cast<RooDataSet*>(wstemp->data(dsName.c_str()));
      ds.append(*dstemp);
    }
    infile->Close();
  }
  cout << "[INFO] Finished looping over files\n" << endl;
  cout << "[INFO] Writing data to " << outputFile->GetName() << endl;
  ws.import(ds);
  outputFile->cd();
  hNtrkoffline.Write();
  hNtrkofflineDz1p0.Write();
  hNtrkofflineGplus.Write();
  ws.Write();
  cout << "[INFO] Finished writing data to " << outputFile->GetName() << endl;
  outputFile->Close();
}
