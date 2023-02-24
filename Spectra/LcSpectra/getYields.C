#include "../ProduceWorkspace/LamC/tools.C"
void getYields()
{
  TFile ofile(::Form("yields_dz1p0.root"), "recreate");
  const unsigned int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const double NtrksF[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {2., 3., 4., 5., 6., 8., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  TH1D* h[ntrks];
  for (int itrk=0; itrk<ntrks; ++itrk) {
    h[itrk] = new TH1D(::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]),
                       ::Form("N_{trk}^{offline} %d -- %d;p_{T};1/N_{evt} dN^{raw}/dp_{T}", Ntrks[itrk], Ntrks[itrk+1]),
                       nPt, pts);
    for (int ipt=0; ipt<nPt; ++ipt) {
      if (ipt == 0 && itrk == ntrks-1 ) continue;
      const char* useWeight = itrk == ntrks-1 ? "weighted" : "unweighted";
      std::string configName = ::string_format("../ProduceWorkspace/LamC/dz1p0_%s/pT%.0fto%.0f_Ntrk%dto%d.conf", useWeight, pts[ipt], pts[ipt+1], Ntrks[itrk], Ntrks[itrk+1]);
      cout << configName << endl;
      configName = gSystem->ExpandPathName(configName.c_str());
      FitParConfigs configs(configName);
      auto outputConfigs = configs.getOutputConfigs();
      auto cutConfigs = configs.getCutConfigs();
      VarCuts mycuts(cutConfigs);
      auto names = ::getNames(configs, mycuts);
      names["outdir"] = outputConfigs.getPath("outdir");
      // names["outdir"] = "/storage1/users/yz144/HIN-21-016/D0Trees/fit";
      if (*names.at("outdir").rbegin() != '/') names.at("outdir") += "/";
      std::cout << "getNames\n";
      // for (const auto key : names) {
      //  std::cout << key.first << ", " << key.second << "\n";
      // }
      // names["outdir"] = "./postfit/";
      double yields = 0;
      double err = 0;
      getFit(names, yields, err);
      std::cout << "getFit\n";
      double nEvents = countEvents(configName);
      std::cout << nEvents << "\n";
      yields /= nEvents;
      err /= nEvents;
      const double dPt = pts[ipt+1] - pts[ipt];
      yields /= dPt;
      err /= dPt;
      h[itrk]->SetBinContent(ipt+1, yields);
      h[itrk]->SetBinError(ipt+1, err);
    }
    ofile.cd();
    h[itrk]->Write();
    delete h[itrk];
  }
}
