#if defined(__CLING__) and !defined(__ROOTCLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <tuple>

#include <memory>

#include <algorithm>
#include <climits>
#include <numeric>

#include "TChain.h"
#include "TFileCollection.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "THashList.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TMath.h"
#include "TNtuple.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TSystem.h"

#include "Utilities/TreeReader/D0Event.h"
#include "Utilities/Ana/Common.h"

using std::cerr;
using std::cout;
using std::endl;

using std::vector;
using std::string;
using std::list;
using std::map;
using std::tuple;

using std::shared_ptr;
using std::unique_ptr;

// n is # of bins, instead of # of pts elements
int findPtBin(const int n, double const* pts, const double pt)
{
  for (int ipt=0; ipt<n; ++ipt)
  {
    if (pt < pts[ipt+1] && pt > pts[ipt]) return ipt;
  }
  return -1;
}

struct D0NTuple
{
  float  mva;
  float  y;
  float  mass;
  float  pT;
  float  eta;
  float  phi;
  float  dz1p0;
  float  gplus;
  float  weight;
  float  Ntrkoffline;
};

void skimReco(std::string inputList="test.list", std::string effFileName="")
{
  TStopwatch ts;
  ts.Start();

  const int nPt = 6;
  const double pTBins[nPt+1] = {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};

  TH1D* hMass[nPt];
  for (int ipt=0; ipt<nPt; ++ipt) {
    hMass[ipt] = new TH1D(Form("hMass_%d", ipt),
        Form("pT%.1f-%.1f;Mass (GeV);Events/5MeV", pTBins[ipt], pTBins[ipt+1]),
        60, 1.7, 2.0);
  }

  TChain tEvt("eventinfoana/EventInfoNtuple");
  TChain tD0("d0ana_newreduced/VertexCompositeNtuple");

  if (inputList.find(".list") != std::string::npos) {
    TFileCollection tf("tf", "", inputList.c_str());
    tEvt.AddFileInfoList(tf.GetList());
    tD0.AddFileInfoList(tf.GetList());
    std::cout << "Added files in " << inputList << std::endl;
  } else if (inputList.find(".root") != std::string::npos ) {
    std::istringstream istr(inputList);
    std::string filename;
    while (std::getline(istr, filename, ',')) {
      TString name(filename);
      name.Strip(TString::kBoth);
      tEvt.Add(name);
      tD0.Add(name);
      std::cout << "Added file " << name << std::endl;
    }
  } else {
    std::cerr << "No input files are given!!!" << std::endl;
    return;
  }

  D0Event devt(&tD0, &tEvt);
  devt.SetBranchStatus("*", 0);
  devt.SetBranchStatus("candSize", 1);
  devt.SetBranchStatus("pT", 1);
  devt.SetBranchStatus("y", 1);
  devt.SetBranchStatus("mass", 1);
  devt.SetBranchStatus("eta", 1);
  devt.SetBranchStatus("mva", 1);
  devt.SetBranchStatus("event.evtSel", 1);
  devt.SetBranchStatus("event.Ntrkoffline", 1);

  std::string inListBaseName;
  if (inputList.find(".list") != std::string::npos) {
    inListBaseName = gSystem->BaseName(inputList.c_str());
    auto pos = inListBaseName.find(".list");
    inListBaseName.replace(pos, 5, "");
  } else if (inputList.find(".root") != std::string::npos ) {
    std::istringstream istr(inputList);
    std::string filename;
    while (std::getline(istr, filename, ',')) {
      TString name(filename);
      name.Strip(TString::kBoth);
      string temp = gSystem->BaseName(name.Data());
      for (int i=0; i<5; ++i) {
        temp.pop_back();
      }
      inListBaseName += temp + "_";
    }
  }
  if (!inListBaseName.empty()) {
    if (*inListBaseName.rbegin() == '_')
      inListBaseName.pop_back();
  }

  TFile* ofile = TFile::Open(("d0output_" + inListBaseName + ".root").c_str(), "recreate");
  D0NTuple dtuple;
  TNtuple t("D0NTuple", "D0NTuple", "mva:y:mass:pT:eta:phi:dz1p0:gplus:weight:Ntrkoffline");

  TH1D hNtrkoffline("hNtrkoffline", "N_{trk}^{offline} for PV with highest N;N_{trk}^{offline};", 301, -0.5, 300.5);
  TH1D hNtrkofflineDz1p0("hNtrkofflineDz1p0", "N_{trk}^{offline} for PV with highest N, dz1p0;N_{trk}^{offline};", 301, -0.5, 300.5);
  TH1D hNtrkofflineGplus("hNtrkofflineGplus", "N_{trk}^{offline} for PV with highest N, Gplus;N_{trk}^{offline};", 301, -0.5, 300.5);
  hNtrkoffline.SetDirectory(ofile);

  TFile* effFile;
  TGraph* g(nullptr);
  if (effFileName.size()) {
    effFile = TFile::Open(effFileName.c_str());
    if (effFile->IsOpen()) {
        g = (TGraphAsymmErrors*) effFile->Get("Ntrk185");
    }
    effFile->Close();
  }
  EfficiencyTable<TGraph> effTab(g);
  const bool reweight = g;

  Long64_t nEntriesFast = devt.GetEntriesFast();
  for (Long64_t ientry=0; ientry<nEntriesFast; ++ientry) {
    auto bytes = devt.GetEntry(ientry);
    auto jentry = devt.LoadTree(ientry);
    if (jentry < 0) break;
    //cout << devt.EvtSel(4) << "\t" << devt.nTrkOffline() << endl;

    const auto ntrk = devt.nTrkOffline();
    double weight = 1.0;
    if (reweight) {
      if (ntrk < 400) weight = effTab.getWeight(ntrk);
    }
    // event level counting
    // wihtout any filters
    if (reweight) {
      hNtrkoffline.Fill(ntrk, weight);
    } else {
      hNtrkoffline.Fill(ntrk);
    }
    // with dz1p0
    if (devt.EvtSel(4)) {
      if (reweight) {
      hNtrkofflineDz1p0.Fill(ntrk, weight);
      } else {
        hNtrkofflineDz1p0.Fill(ntrk);
      }
    }
    // with gplus
    if (devt.EvtSel(5)) {
      if (reweight) {
        hNtrkofflineGplus.Fill(ntrk, weight);
      } else {
        hNtrkofflineGplus.Fill(ntrk);
      }
    }

    const auto candSize = devt.CandSize();
    if (candSize) {
      for (int id0=0; id0<candSize; ++id0) {
        //cout << devt.Mass(id0) << "\t"
        //  << devt.Pt(id0) << "\t"
        //  << devt.Y(id0) << "\t"
        //  << devt.Eta(id0) << "\t"
        //  << devt.Mva(id0) << endl;

        // make use of loose cuts
        //if (devt.Mva(id0) < 0.56) continue;
        if (devt.Mva(id0) < 0.5) continue;
        if (std::abs(devt.Y(id0)) > 1) continue;

        dtuple.mva = devt.Mva(id0);
        dtuple.mass = devt.Mass(id0);
        dtuple.y  = devt.Y(id0);
        dtuple.pT = devt.Pt(id0);
        dtuple.eta = devt.Eta(id0);
        dtuple.phi = devt.Phi(id0);
        dtuple.dz1p0 = devt.EvtSel(4); // dz1p0
        dtuple.gplus = devt.EvtSel(5); // gplus
        dtuple.weight = weight;
        dtuple.Ntrkoffline = ntrk;

        t.Fill(dtuple.mva,
               dtuple.y,
               dtuple.mass,
               dtuple.pT,
               dtuple.eta,
               dtuple.phi,
               dtuple.dz1p0,
               dtuple.gplus,
               dtuple.weight,
               dtuple.Ntrkoffline
               );

        auto ipt = findPtBin(nPt, pTBins, devt.Pt(id0));
        if (ipt<0) continue;

        if (reweight) {
          hMass[ipt]->Fill(devt.Mass(id0), weight);
        } else {
          hMass[ipt]->Fill(devt.Mass(id0));
        }
      }
    }
  }

  ofile->cd();
  t.Write();
  hNtrkoffline.Write();
  hNtrkofflineDz1p0.Write();
  hNtrkofflineGplus.Write();
  for (const auto h : hMass) h->Write();
  for (const auto h : hMass) h->Delete();
  // ofile->Close();
  // ofile->ls();
  delete ofile;
  ts.Stop();
  ts.Print();
}
