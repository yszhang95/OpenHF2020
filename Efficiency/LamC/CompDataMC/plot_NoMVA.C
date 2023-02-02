
#if defined(__CLING__) and !defined(__ROOTCLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif
#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/Ana/Common.h"

#include "RooArgSet.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooWorkspace.h"

#include "TChain.h"
#include "TFile.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TTree.h"
#include "TSystem.h"


#include <memory>
#include <string>
#include <stdexcept>
void plot(std::string configName)
{
  FitParConfigs configs(configName.c_str());
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  const auto output = ::getNames(configs, mycuts);

  std::string fileName = output.at("fileName");
  fileName = gSystem->BaseName(fileName.c_str());

  auto ofileName = fileName;

  fileName = "output/" + fileName;
  fileName.replace(fileName.size()-5, 5, "_splot.root");
  cout << fileName << endl;

  TFile infile(fileName.c_str());

//  ofileName.replace(fileName.size() - 5, 5, "_canvases.root");

  // ofileName
  std::map<std::string, std::map<std::string, TH1* > > hists;
  hists["ProtonPtVsMass"].insert({"data", infile.Get<TH1D>("ProtonPt")});
  hists["ProtonPtVsMass"].insert({"MC", infile.Get<TH1D>("ProtonPt_MC")});
  hists["ProtonPtVsMass"].insert({"MCRebin", infile.Get<TH1D>("ProtonPt_MC_rebin")});

  hists["ProtonPVsMass"].insert({"data", infile.Get<TH1D>("ProtonP")});
  hists["ProtonPVsMass"].insert({"MC", infile.Get<TH1D>("ProtonP_MC")});
  hists["ProtonPVsMass"].insert({"MCRebin", infile.Get<TH1D>("ProtonP_MC_rebin")});

  hists["ProtonEtaVsMass"].insert({"data", infile.Get<TH1D>("ProtonEta")});
  hists["ProtonEtaVsMass"].insert({"MC", infile.Get<TH1D>("ProtonEta_MC")});
  hists["ProtonEtaVsMass"].insert({"MCRebin", infile.Get<TH1D>("ProtonEta_MC_rebin")});

  // hists["ProtonDeDxVsMass"].insert({"data", infile.Get<TH1D>("ProtonDeDxVsMass_data")});
  // hists["ProtonDeDxVsMass"].insert({"MC", infile.Get<TH1D>("ProtonDeDxVsMass_MC")});

  hists["CosAngleVsMass"].insert({"data", infile.Get<TH1D>("CosAngle")});
  hists["CosAngleVsMass"].insert({"MC", infile.Get<TH1D>("CosAngle_MC")});
  hists["CosAngleVsMass"].insert({"MCRebin", infile.Get<TH1D>("CosAngle_MC_rebin")});

  for (auto& e : hists) {
    std::cout << e.first << "\n";
    TCanvas c("c", "", 600, 480);
    c.SetRightMargin(0.02);
    e.second["MC"]->SetStats(0);
    e.second["MCRebin"]->SetStats(0);
    e.second["data"]->SetStats(0);

    e.second["data"]->SetMarkerStyle(20);
    e.second["data"]->SetMarkerSize(0.5);
    e.second["data"]->SetMarkerColor(kRed);

    e.second["MC"]->SetLineColor(kGreen);
    e.second["MCRebin"]->SetLineColor(kBlue);
    e.second["data"]->SetLineColor(kRed);

    e.second["MC"]->GetYaxis()->SetTitle(::Form("1/N dN/d%s", e.second.at("MC")->GetXaxis()->GetTitle()));
    e.second["MC"]->Draw();
    e.second["MCRebin"]->Draw("SAME");
    e.second["data"]->Draw("SAME");
    e.second["MC"]->SetTitle(::Form("N_{trk}^{offline} %d--%d #Lambda_{c}^{+} p_{T} %.1f--%.1f", mycuts._NtrkofflineMin, mycuts._NtrkofflineMax, mycuts._pTMin, mycuts._pTMax));
    std::string prefix(ofileName.begin(), ofileName.end()-5);
    TLegend leg(0.75, 0.73, 0.97, 0.89);
    leg.SetFillStyle(0);
    leg.SetBorderSize(0);
    leg.AddEntry(e.second["MC"], "MC", "l");
    leg.AddEntry(e.second["MCRebin"], "MC Rebin", "l");
    leg.AddEntry(e.second["data"], "Data", "l");
    leg.Draw();
    TLatex tex;
    tex.SetTextFont(42);
    tex.SetTextAlign(31);
    tex.DrawLatexNDC(0.98, 0.91, "pPb 8.16 TeV");
    c.Print(::Form("plots/%s_%s.png", prefix.c_str(), e.first.c_str()));
    c.Print(::Form("plots/%s_%s.pdf", prefix.c_str(), e.first.c_str()));
  }
}

void plot_NoMVA()
{
  const float pts[] = {2, 3, 4, 5, 6, 8, 10};
  const int nPt = sizeof(pts)/sizeof(float) - 1;

  // const int Ntrkofflines[] = {0, 35, 60, 90, 120, 185, 250};
  const int Ntrkofflines[] = {0, 185};
  const int nNtrkofflines = sizeof(Ntrkofflines)/sizeof(int) - 1;

  for (int ipt=0; ipt<nPt; ++ipt) {
  // for (int ipt=nPt-1; ipt<nPt; ++ipt) {
  // for (int ipt=nPt-2; ipt<nPt-1; ++ipt) {
    // for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
    for (int itrk=nNtrkofflines-1; itrk<nNtrkofflines; ++itrk) {
      if (ipt ==0 && Ntrkofflines[itrk+1] == 250) continue;
      std::string config = ::string_format("pT%.0fto%.0f", pts[ipt], pts[ipt+1]);
      if (Ntrkofflines[itrk+1] == 250)
        // config = "${OPENHF2020TOP}/Spectra/ProduceWorkspace/LamC/dz1p0_weighted/" + config;
        // config = "${OPENHF2020TOP}/Syst/LamC/Cuts_linear/configs/" + config + "_HM.conf";
        config = "configs/" + config + "_HM.conf";
      else
        // config = "${OPENHF2020TOP}/Spectra/ProduceWorkspace/LamC/dz1p0_unweighted/" + config;
        // config = "${OPENHF2020TOP}/Syst/LamC/Cuts_linear/configs/" + config + "_MB.conf";
        config = "configs/" + config + "_MB_NoMVA.conf";
      config = gSystem->ExpandPathName(config.c_str());
      std::cout << config << "\n";
      plot(config);
    }
  }
}

void plot_NoMVA(const float pTMin, const float pTMax)
{

  const int Ntrkofflines[] = {0, 185};
  const int nNtrkofflines = sizeof(Ntrkofflines)/sizeof(int) - 1;

  for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
    std::string config = ::string_format("pT%.0fto%.0f", pTMin, pTMax);
    if (Ntrkofflines[itrk+1] == 250)
      // config = "${OPENHF2020TOP}/Spectra/ProduceWorkspace/LamC/dz1p0_weighted/" + config;
      // config = "${OPENHF2020TOP}/Syst/LamC/Cuts_linear/configs/" + config + "_HM.conf";
      config = "configs/" + config + "_HM.conf";
    else
      // config = "${OPENHF2020TOP}/Spectra/ProduceWorkspace/LamC/dz1p0_unweighted/" + config;
      // config = "${OPENHF2020TOP}/Syst/LamC/Cuts_linear/configs/" + config + "_MB.conf";
      config = "configs/" + config + "_MB_NoMVA.conf";
    config = gSystem->ExpandPathName(config.c_str());
    std::cout << config << "\n";
    plot(config);
  }
}
