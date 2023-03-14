void getSpectra()
{
  // const double NP_fracs[] = {0.031, 0.026, 0.024, 0.035, 0.068, 0.058};
  const double BR = 0.0159 * 0.69;
  // (yields / dPt / N) /eff / BR * frac
  TFile fRawYields("yields_dz1p0.root");
  const int ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const int nbins = sizeof(ntrks)/sizeof(int) - 1;
  TFile ofile("spectra_dz1p0_LamC.root", "recreate");
  for (int i=0; i<nbins; ++i) {
    // NP frac
    std::string fracname =  i==nbins-1 ? "../../PromptExtraction/FONLL/LamC/final_HM_bdecay_frac.root"
      :  "../../PromptExtraction/FONLL/LamC/final_MB_bdecay_frac.root";
    TFile ffrac(fracname.c_str());
    auto hfrac = ffrac.Get<TH1>("hfrac");
    std::string effname =  i==nbins-1 ? "../../Efficiency/LamC/avg_eff_HM.root" : "../../Efficiency/LamC/avg_eff_MB.root";
    TFile fAccEff(effname.c_str());
    TGraphAsymmErrors* AccEff = (TGraphAsymmErrors*) fAccEff.Get("AvgAccEff");

    auto h = fRawYields.Get<TH1D>(::Form("Ntrk%dto%d", ntrks[i], ntrks[i+1]));

    auto hLamC = (TH1D*)h->Clone();

    hLamC->GetYaxis()->SetTitle("1/N_{evt} dN/dp_{T} (GeV^{-1})");
    const auto npt = h->GetNbinsX();
    for (int ipt=0; ipt<npt; ++ipt) {
      if (ipt==0 && i==nbins-1) continue;
      const auto raw = h->GetBinContent(ipt+1);
      const auto rawErr = h->GetBinError(ipt+1);
      const auto eff = AccEff->GetY()[ipt];
      const auto frac = 1- hfrac->GetBinContent(ipt+1);
      const auto yields = raw / eff / BR * frac / 2.; // average over particle and anti-particle
      const auto yields_err = rawErr / eff / BR * frac / 2.; // average over particle and anti-particle
      hLamC->SetBinContent(ipt+1, yields);
      hLamC->SetBinError(ipt+1, yields_err);
    }
    ofile.cd();
    hLamC->Write();
    delete hLamC;
  }
}
