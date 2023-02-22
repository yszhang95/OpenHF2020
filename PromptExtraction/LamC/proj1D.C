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

#include "optimizer.cc"

  using TH2DMap = std::map<std::string, ::TH2D*>;


TH1* proj1D(TH2D* h2, std::string name,
    const double value_low, const double value_high)
{
  TH1* h = nullptr;
  h2->Sumw2();
  auto bin_min = h2->GetYaxis()->FindBin(value_low);
  auto bin_max = h2->GetYaxis()->FindBin(value_high+0.1*h2->GetYaxis()->GetBinWidth(1)) - 1; // let us assum value_high falls on the edge
  h = h2->ProjectionX(name.c_str(), bin_min, bin_max);
  return h;
}

void proj1D(std::string configName)
{
  FitParConfigs configs(configName.c_str());
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  const auto output = ::getNames(configs, mycuts);

  std::string fileName = output.at("fileName");
  fileName = gSystem->BaseName(fileName.c_str());
  fileName = "output/" + fileName;

  auto fileNameMC = fileName;
  /*
  auto index = fileNameMC.find(".root");
  fileNameMC.replace(index, 5, "_mc.root");
  */

  TFile infile(fileName.c_str());
  TFile infileMC(fileNameMC.c_str());

  auto ofileName = fileName;
  ofileName.replace(fileName.size() - 5, 5, "_splot.root");

  std::map<std::string, TH2DMap> hists;
  for (const auto key : {"data", "PromptMC", "NonPromptMC"}) {
    hists["DCAVsMass"].insert({key, infile.Get<TH2D>(::Form("DCAVsMass%s", key))});
  }

  std::map<std::string, std::string> hists_names;
  std::map<std::string, std::string> hists_titles;
  std::map<std::string, std::vector<double>> hists_bins;

  hists_names["DCAVsMass"] = "DCA";
  hists_titles["DCAVsMass"] = "DCA";

  for (const auto p : hists_names) {
    hists_bins[p.first] = cutConfigs.getVDouble(p.second);
  }
  std::cout << "Got DCA\n";

  TFile ofile(ofileName.c_str(), "recreate");

  for (auto& e : hists) {

    std::cout << "Analyzing the variable " << e.first << "\n";

    auto hMC = proj1D(e.second.at("PromptMC"), hists_names.at(e.first) + "_mass", -1E4, 1E4);
    auto binning = hists_bins.at(e.first);
    const auto nbins = binning.size()-1;
    auto hdata = new TH1D(hists_names.at(e.first).c_str(), hists_titles.at(e.first).c_str(), binning.size()-1, binning.data());
    for (int ibin=0; ibin<nbins; ++ibin) {
      const double value_low = binning.at(ibin);
      const double value_up = binning.at(ibin+1);
      auto hproj = proj1D(e.second.at("data"), hists_names.at(e.first) + to_string(ibin), value_low, value_up);
      // do fit
      // fit(hMC, hproj)
      const std::string mvaStr = mycuts.getMva();
      TCanvas c(::Form("c_%s_pT%.0fto%.0f_%s", hproj->GetName(), mycuts._pTMin, mycuts._pTMax, mvaStr.c_str()), "", 600*2, 450);
      c.Divide(2, 1);
      const double pars[] = {20000, -5000, 100, 200};
      // not very ideal because of the configurations
      auto parameters = simpleFit(hMC, hproj, pars, 2.15, 2.42, &c);
      const auto npars = parameters.size();

      hdata->SetBinContent(ibin+1, parameters.at(0));
      hdata->SetBinError(ibin+1, parameters.at(npars/2));

      std::cout << "Done the bin " << ibin << "\n";
    }
    auto integral_data = hdata->Integral();
    // hdata->Scale(1./integral_data, "width");
    auto hPromptMC_var = e.second.at("PromptMC")->ProjectionY((hists_names.at(e.first)+"_PromptMC").c_str(), 0, 1000000);
    auto hPromptMC_rebin = hPromptMC_var->Rebin(binning.size()-1, ::Form("%s_rebin", hPromptMC_var->GetName()), binning.data());
    auto hNonPromptMC_var = e.second.at("NonPromptMC")->ProjectionY((hists_names.at(e.first)+"_NonPromptMC").c_str(), 0, 1000000);
    auto hNonPromptMC_rebin = hNonPromptMC_var->Rebin(binning.size()-1, ::Form("%s_rebin", hNonPromptMC_var->GetName()), binning.data());
    // auto integral_MC = hMC_var->Integral();
    // hMC_var->Scale(1./integral_MC, "width");
    // hMC_rebin->Scale(1./integral_MC, "width");
    ofile.cd();
    hdata->Write();
    hPromptMC_var->Write();
    hPromptMC_rebin->Write();
    hNonPromptMC_var->Write();
    hNonPromptMC_rebin->Write();
  }
}

void proj1D()
{
  // const float pts[] = {2, 3, 4, 5, 6, 8, 10};
  const float pts[] = {8, 10};
  const int nPt = sizeof(pts)/sizeof(float) - 1;

  // const int Ntrkofflines[] = {0, 35, 60, 90, 120, 185, 250};
  const int Ntrkofflines[] = {0, 185};
  const int nNtrkofflines = sizeof(Ntrkofflines)/sizeof(int) - 1;

  for (int ipt=0; ipt<nPt; ++ipt) {
  // for (int ipt=nPt-2; ipt<nPt-1; ++ipt) {
    for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
      std::cout << "Analyzing " << pts[ipt] << " " << pts[ipt+1] << " Ntrkoffline " << Ntrkofflines[itrk] << " " << Ntrkofflines[itrk+1] << "\n";
      if (std::abs(pts[ipt]-2)<1E-3 && Ntrkofflines[itrk+1] == 250) continue;
      std::string config = ::string_format("pT%.0fto%.0f", pts[ipt], pts[ipt+1]);
      if (Ntrkofflines[itrk+1] == 250)
        config = "configs/" + config + "_HM.conf";
      else
        config = "configs/" + config + "_MB.conf";
      config = gSystem->ExpandPathName(config.c_str());
      // std::cout << config << "\n";
      proj1D(config);
    }
  }
}

void proj1D(const float pTMin, const float pTMax)
{

  // const int Ntrkofflines[] = {0, 35, 60, 90, 120, 185, 250};
  const int Ntrkofflines[] = {0, 185};
  const int nNtrkofflines = sizeof(Ntrkofflines)/sizeof(int) - 1;

  for (int itrk=0; itrk<nNtrkofflines; ++itrk) {
    std::cout << "Analyzing pT " << pTMin << " " << pTMax << " Ntrkoffline " << Ntrkofflines[itrk] << " " << Ntrkofflines[itrk] << "\n";
    std::string config = ::string_format("pT%.0fto%.0f", pTMin, pTMax);
    if (Ntrkofflines[itrk+1] == 250)
      config = "configs/" + config + "_HM.conf";
    else
      config = "configs/" + config + "_MB.conf";
    config = gSystem->ExpandPathName(config.c_str());
    // std::cout << config << "\n";
    proj1D(config);
  }
}
