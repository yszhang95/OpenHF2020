#include "../../../Spectra/ProduceWorkspace/D0/tools.C"

void getYields(int order=3)
{
  TFile ofile(::Form("yields_order%d.root", order), "recreate");
  const unsigned int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const double NtrksF[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {2., 3., 4., 5., 6., 7., 8., 9., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  TH1D* h[ntrks];
  for (int itrk=0; itrk<ntrks; ++itrk) {
    h[itrk] = new TH1D(::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]),
                       ::Form("N_{trk}^{offline} %d -- %d;p_{T};1/N_{evt} dN^{raw}/dp_{T}", Ntrks[itrk], Ntrks[itrk+1]),
                       nPt, pts);
    for (int ipt=0; ipt<nPt; ++ipt) {
      const char* useWeight = itrk == ntrks-1 ? "HM" : "MB";
      std::string configName = ::string_format("D0wKKPiPi_%s_std.txt",
                                               useWeight);
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
      names["outdir"] = ::Form("./output_order%d/", order);
      names["suffix"] = ::string_format("_order%d_postfit.root", order);
      double yields = 0;
      double err = 0;
      getFit(names, yields, err);
      double nEvents = countEvents(inputConfigs, mycuts);
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
