#include "../../../Spectra/ProduceWorkspace/LamC/tools.C"

void getYields()
{
  std::string trig="dz1p0";
  const std::string weightTypes[] = {"weighted", "weighted_up", "weighted_lo"};
  const unsigned int Ntrks[] = {185, 250};
  const double NtrksF[] = {185, 250};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {3., 4., 5., 6., 8., 10.};
  // const double pts[] = {3., 4.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  // TH1D* h[nPt];
  // forget 2 -- 3
  // for (int ipt=0; ipt<nPt; ++ipt) {
  //   h[ipt] = new TH1D(::Form("pT%.0fto%.0f", pts[ipt], pts[ipt+1]), ::Form("p_{T} %.1f -- %.1f;p_{T};1/N_{evt} dN/dp_{T}", pts[ipt], pts[ipt+1]), ntrks, NtrksF);
  //   for (int itrk=0; itrk<ntrks; ++itrk) {
  //     std::string configName = ::string_format("../../../Spectra/ProduceWorkspace/LamC/%s_%s/pT%.0fto%.0f_Ntrk%dto%d.conf", trig.c_str(), useWeightStr, pts[ipt], pts[ipt+1], Ntrks[itrk], Ntrks[itrk+1]);
  //     cout << configName << endl;
  //     double yields = 0;
  //     double err = 0;
  //     getFit(configName, yields, err);
  //     double nEvents = countEvents(configName);
  //     yields /= nEvents;
  //     err /= nEvents;
  //     const double dPt = pts[ipt+1] - pts[ipt];
  //     yields /= dPt;
  //     err /= dPt;
  //     h[ipt]->SetBinContent(itrk+1, yields);
  //     h[ipt]->SetBinError(itrk+1, err);
  //     cout << nEvents << "\n";
  //   }
  //   ofile.cd();
  //   h[ipt]->Write();
  //   delete h[ipt];
  // }

  TFile output("yields_weighted.root", "recreate");
  TH1D* h[3];
  for (int i=0; i<3; ++i) {
    const auto weightType = weightTypes[i];
    TFile ofile(::Form("yields_%s.root", weightType.c_str()), "recreate");
    h[i] = new TH1D(::Form("h%s", weightType.c_str()), ::Form("Ntrk185to250_%s;p_{T} (GeV);1/N_{evt} dN^{raw}/dp_{T} (GeV^{-1})", weightType.c_str()), nPt, pts);
    for (int ipt=0; ipt<nPt; ++ipt) {
      const int itrk = 0;
      std::string configName = ::string_format("${OPENHF2020TOP}/Spectra/ProduceWorkspace/LamC/%s_%s/pT%.0fto%.0f_Ntrk%dto%d.conf", trig.c_str(), weightType.c_str(), pts[ipt], pts[ipt+1], Ntrks[itrk], Ntrks[itrk+1]);
      configName = gSystem->ExpandPathName(configName.c_str());
      cout << configName << endl;
      FitParConfigs configs(configName);
      auto cutConfigs = configs.getCutConfigs();
      VarCuts mycuts(cutConfigs);
      auto names = ::getNames(configs, mycuts);
      names["outdir"] = "./";
      double yields = 0;
      double err = 0;
      getFit(names, yields, err);
      double nEvents = countEvents(configName);
      yields /= nEvents;
      err /= nEvents;
      const double dPt = pts[ipt+1] - pts[ipt];
      yields /= dPt;
      err /= dPt;
      // cout << "yields " <<  yields << " +/- " << err << "\n";
      cout << "pT: " << pts[ipt] << " " << pts[ipt+1] << " GeV: " << yields << " +/- " << err << "\n";
      h[i]->SetBinContent(ipt+1, yields);
      h[i]->SetBinError(ipt+1, err);
    }
    output.cd();
    h[i]->Write();
  }
  output.Close();
  // for (auto hist : h) delete hist;
}
