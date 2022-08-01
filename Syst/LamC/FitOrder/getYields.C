#include "../../../Spectra/ProduceWorkspace/LamC/tools.C"

void getYields(int order=3)
{
  TFile ofile(::Form("yields_order%d.root", order), "recreate");
  const unsigned int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const double NtrksF[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {2., 3., 4., 5., 6., 8., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  TH1D* h[ntrks];
  // forget 2 -- 3
  for (int itrk=0; itrk<ntrks; ++itrk) {
    h[itrk] = new TH1D(::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]),
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
      names["outdir"] = "./postfit/";
      names["suffix"] = ::string_format("_order%d_postfit.root", order);
      double yields = 0;
      double err = 0;
      getFit(names, yields, err);
      double nEvents = countEvents(configName);
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

void getYieldsGausCB()
{
  TFile ofile(::Form("yields_gausCB.root"), "recreate");
  const unsigned int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const double NtrksF[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {2., 3., 4., 5., 6., 8., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  TH1D* h[ntrks];
  // forget 2 -- 3
  for (int itrk=0; itrk<ntrks; ++itrk) {
    h[itrk] = new TH1D(::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]),
                       ::Form("N_{trk}^{offline} %d -- %d;p_{T};1/N_{evt} dN^{raw}/dp_{T}", Ntrks[itrk], Ntrks[itrk+1]),
                       nPt, pts);
    for (int ipt=0; ipt<nPt; ++ipt) {
      if (ipt == 0 && itrk == ntrks-1 ) continue;

      std::string dir_order;
      const char* useWeight = itrk == ntrks-1 ? "weighted" : "unweighted";
      auto configName = ::string_format("dz1p0_%s%s/pT%.0fto%.0f_Ntrk%dto%d.conf",
                                        useWeight, dir_order.c_str(), pts[ipt], pts[ipt+1], Ntrks[itrk], Ntrks[itrk+1]);
      configName = gSystem->ExpandPathName(configName.c_str());
      cout << configName << endl;
      FitParConfigs configs(configName);
      auto cutConfigs = configs.getCutConfigs();
      VarCuts mycuts(cutConfigs);
      auto names = ::getNames(configs, mycuts);
      names["outdir"] = "./postfit/";
      names["suffix"] = "_gausCB_postfit.root";
      double yields = 0;
      double err = 0;
      getFit(names, yields, err);
      double nEvents = countEvents(configName);
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
