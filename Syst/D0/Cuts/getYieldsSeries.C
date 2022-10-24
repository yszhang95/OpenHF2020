#include "../../../Spectra/ProduceWorkspace/D0/tools.C"

void getYieldsSeries()
{
  TFile effFile("AccEff_PromptMC_Series.root");
  // mva cuts from 0.4 to 0.58 with a steap of 0.02
  // const auto mvaMin = 0.4;
  const auto mvaMin = 0.5;
  const auto mvaMax = 0.58;
  const auto step = 0.02;
  const auto nMVA = static_cast<int>(std::round((mvaMax - mvaMin)/step));
  vector<double> mvas(nMVA+1);
  for (int i=0; i != nMVA+1; ++i) {
    mvas[i] = mvaMin + step * i;
  }

  TFile ofile("yieldsSeries.root", "recreate");
  const unsigned int Ntrks[] = {0, 185, 250};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {2., 3., 4., 5., 6., 7., 8., 9., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  for (int itrk=0; itrk<ntrks; ++itrk) {
    TH1D* h[nPt];
    for (int ipt=0; ipt<nPt; ++ipt) {
    // for (int ipt=0; ipt<1; ++ipt) {
      h[ipt] = new TH1D(::Form("Ntrk%dto%d_pT%d", Ntrks[itrk], Ntrks[itrk+1], ipt),
                        ::Form("N_{trk}^{offline} %d -- %d p_{T} %.1f -- %.1f;BDT score;1/N_{evt} dN^{corr}/dp_{T}", Ntrks[itrk], Ntrks[itrk+1], pts[ipt], pts[ipt+1]),
                        nMVA+1, mvaMin - 0.5*step, mvaMax + 0.5*step);
      std::string configName = ::string_format("D0woKKPiPi_Ntrk%dto%d.txt",
                                               Ntrks[itrk], Ntrks[itrk+1]);
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
      for (int imva=0; imva<nMVA+1; ++imva) {
        mycuts._mvaCut = mvas[imva];
        auto names = ::getNames(configs, mycuts);
        names["outdir"] = "./output/files/";
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
        // const auto effFileName = ::string_format("../../../Efficiency/D0/AccEff_PromptMC_mva%s.root", mvaStr.c_str());
        auto geff = effFile.Get<TGraphAsymmErrors>(::Form("d0AccEff%s", mvaStr.c_str()));

        yields /= geff->GetY()[ipt];
        err /= geff->GetY()[ipt];

        h[ipt]->SetBinContent(imva+1, yields);
        h[ipt]->SetBinError(imva+1, err);

        delete geff;
      }
      ofile.cd();
      h[ipt]->Write();
      delete h[ipt];
    }
  }
}
