void getIncSpectraMB()
{
  const double BR = 0.0159 * 0.69;
  // (yields / dPt / N) /eff / BR * frac
  TFile fRawYields("yields_dz1p0_MB.root");
  TFile ofile("spectra_dz1p0_MB_LamC.root", "recreate");
  std::string effname =  "../../Efficiency/LamC/avg_eff_MB.root";
  TFile fAccEff(effname.c_str());
  TGraphAsymmErrors* AccEff = (TGraphAsymmErrors*) fAccEff.Get("AvgAccEff");

  auto h = fRawYields.Get<TH1D>("MB");

  auto hLamC = (TH1D*)h->Clone();

  hLamC->GetYaxis()->SetTitle("1/N_{evt} dN/dp_{T} (GeV^{-1})");
  const auto npt = h->GetNbinsX();
  for (int ipt=0; ipt<npt; ++ipt) {
    const auto raw = h->GetBinContent(ipt+1);
    const auto rawErr = h->GetBinError(ipt+1);
    const auto eff = AccEff->GetY()[ipt];
    const auto frac = 1;
    const auto yields = raw / eff / BR * frac;
    const auto yields_err = rawErr / eff / BR * frac;
    hLamC->SetBinContent(ipt+1, yields);
    hLamC->SetBinError(ipt+1, yields_err);
  }
  ofile.cd();
  hLamC->Write();
  delete hLamC;
}
