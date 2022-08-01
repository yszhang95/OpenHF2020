#if defined(__CLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/FitUtils/fitUtils.h"

const double binLow = 2.13;
const double leftEnd = 2.2465;
const double rightBegin = 2.3265;
const double binHigh = 2.48;
const double nBins = 100;

using namespace RooFit;
void getFitQuality(int order=3)
{
  TFile ofile(::Form("quality_order%d.root", order), "recreate");
  const unsigned int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const double NtrksF[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {2., 3., 4., 5., 6., 8., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  TH1D* hChi2[ntrks];
  TH1D* hMinNLL[ntrks];
  // forget 2 -- 3
  for (int itrk=0; itrk<ntrks; ++itrk) {
    hChi2[itrk] = new TH1D(::Form("Chi2Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]),
                       ::Form("N_{trk}^{offline} %d -- %d;p_{T};1/N_{evt} dN^{raw}/dp_{T}", Ntrks[itrk], Ntrks[itrk+1]),
                       nPt, pts);
    hMinNLL[itrk] = new TH1D(::Form("MinNLLNtrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]),
                       ::Form("N_{trk}^{offline} %d -- %d;p_{T};1/N_{evt} dN^{raw}/dp_{T}", Ntrks[itrk], Ntrks[itrk+1]),
                       nPt, pts);
    for (int ipt=0; ipt<nPt; ++ipt) {
      if (ipt == 0 && itrk == ntrks-1 ) continue;

      std::string dir_order;
      switch(order) {
        case 2: dir_order = "_2nd"; break;
        case 4: dir_order = "_4th"; break;
      }
      const char* useWeight = itrk == ntrks-1 ? "weighted" : "unweighted";
      auto configName = ::string_format("dz1p0_%s%s/pT%.0fto%.0f_Ntrk%dto%d.conf",
                                        useWeight, dir_order.c_str(), pts[ipt], pts[ipt+1], Ntrks[itrk], Ntrks[itrk+1]);
      configName = gSystem->ExpandPathName(configName.c_str());
      cout << configName << endl;
      FitParConfigs configs(configName);
      auto cutConfigs = configs.getCutConfigs();
      VarCuts mycuts(cutConfigs);
      auto names = ::getNames(configs, mycuts);
      names["outdir"] = "./sideband/";
      names["suffix"] = ::string_format("_order%d_sideband_postfit.root", order);
      names.at("fileName") = gSystem->BaseName(names.at("fileName").c_str());

      auto namesData = names;

      auto pos = names.at("fileName").find(".root");
      names.at("fileName").replace(pos, 5, names.at("suffix"));
      names.at("fileName") = names.at("outdir") + names.at("fileName");

      namesData.at("fileName") = "ws_hists/" + namesData.at("fileName");
      namesData.at("dsName").replace(1, 1, "h");

      TFile f(names.at("fileName").c_str());
      TFile fData(namesData.at("fileName").c_str());

      RooWorkspace* myws = (RooWorkspace*) f.Get(names.at("wsName").c_str());
      RooWorkspace* mywsData = (RooWorkspace*) fData.Get(names.at("wsName").c_str());
      auto dh = (RooDataHist*)mywsData->data(namesData.at("dsName").c_str());
      auto cand_mass = (RooRealVar*)mywsData->var("cand_mass");

      auto result = (RooFitResult*) myws->obj((names.at("dsName") + "_result").c_str());

      auto pars = result->floatParsFinal();
      RooArgSet coeffs("coeffs");
      for (int i=0; i<order; ++i) {
        auto coeff = RooRealVar(::Form("a%d", i+1), "", dynamic_cast<RooRealVar*>(pars.find(::Form("a%d", i+1)))->getVal());
        coeffs.addClone(coeff);
      }
      RooChebychev bkg("bkg", "", *cand_mass, coeffs);

      cand_mass->setRange("full", binLow, binHigh);
      cand_mass->setRange("left", binLow, leftEnd);
      cand_mass->setRange("right", rightBegin, binHigh);
      cand_mass->setMin(binLow);
      cand_mass->setMax(binHigh);
      auto chi2Var = bkg.createChi2(*dh, Range("left,right"), Extended(kFALSE));

      double chi2 = chi2Var->getVal();
      double minNLL = result->minNll();
      std::cout << chi2 << "\n";
      hChi2[itrk]->SetBinContent(ipt+1, chi2);
      hMinNLL[itrk]->SetBinContent(ipt+1, minNLL);
    }
    ofile.cd();
    hChi2[itrk]->Write();
    hMinNLL[itrk]->Write();
    delete hChi2[itrk];
    delete hMinNLL[itrk];
  }
}
