#include "../../../Spectra/ProduceWorkspace/D0/tools.C"

void getYields(std::string cut="std")
{
  TFile ofile(::Form("yields_%s.root", cut.c_str()), "recreate");
  const unsigned int Ntrks[] = {0, 185, 250};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {2., 3., 4., 5., 6., 7., 8., 9., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  TH1D* h[ntrks];
  for (int itrk=0; itrk<ntrks; ++itrk) {
    h[itrk] = new TH1D(::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]),
                       ::Form("N_{trk}^{offline} %d -- %d;p_{T};1/N_{evt} dN^{corr}/dp_{T}", Ntrks[itrk], Ntrks[itrk+1]),
                       nPt, pts);
    for (int ipt=0; ipt<nPt; ++ipt) {
      std::string configName = ::string_format("configs_%s/D0wKKPiPi_pT%.0fto%.0f_Ntrk%dto%d_%s.txt",
                                               cut.c_str(), pts[ipt], pts[ipt+1], Ntrks[itrk], Ntrks[itrk+1], cut.c_str());
      cout << configName << endl;
      FitParConfigs configs(configName);
      auto inputConfigs = configs.getInputConfigs();
      auto cutConfigs = configs.getCutConfigs();
      VarCuts mycuts(cutConfigs);
      mycuts._pTMin = pts[ipt];
      mycuts._pTMax = pts[ipt+1];
      mycuts._NtrkofflineMin = Ntrks[itrk];
      mycuts._NtrkofflineMax = Ntrks[itrk+1];
      std::cout << "usedz1p0 " << mycuts._usedz1p0 << "\n";
      auto names = ::getNames(configs, mycuts);
      names["outdir"] = ::Form("./output_%s/", cut.c_str());
      names["suffix"] = "_postfit.root";
      double yields = 0;
      double err = 0;
      getFit(names, yields, err);
      double nEvents = countEvents(inputConfigs, mycuts);
      yields /= nEvents;
      err /= nEvents;
      const double dPt = pts[ipt+1] - pts[ipt];
      yields /= dPt;
      err /= dPt;

      auto mvaStr = std::to_string(mycuts._mvaCut);
      mvaStr.replace(1, 1, "p");
      const auto effFileName = ::string_format("../../../Efficiency/D0/AccEff_PromptMC_mva%s.root", mvaStr.c_str());
      TFile effFile(effFileName.c_str());
      TGraphAsymmErrors *geff = (TGraphAsymmErrors*) effFile.Get("d0AccEff");

      yields /= geff->GetY()[ipt];
      err /= geff->GetY()[ipt];

      h[itrk]->SetBinContent(ipt+1, yields);
      h[itrk]->SetBinError(ipt+1, err);
    }
    ofile.cd();
    h[itrk]->Write();
    delete h[itrk];
  }
}
