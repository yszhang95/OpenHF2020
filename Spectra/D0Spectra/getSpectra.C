void getSpectra()
{
  const double BR = 0.0395;
  // (yields / dPt / N) /eff / BR * frac
  TFile fRawYields("yields_dz1p0.root");
  TFile fAccEff("../../Efficiency/D0/AccEff_PromptMC.root");
  // update March 9, 2023
  // TFile fFrac("../../D0Productions/DCAFit/frac.root");
  TFile fFrac("../../Syst/D0/NP/prompt_frac.root");
  TGraphAsymmErrors* d0AccEff = (TGraphAsymmErrors*) fAccEff.Get("d0AccEff");
  // TH1D* bestFrac = fFrac.Get<TH1D>("bestFrac");
  const int ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const int nbins = sizeof(ntrks)/sizeof(int) - 1;
  TFile ofile("spectra_dz1p0_D0.root", "recreate");
  for (int i=0; i<nbins; ++i) {
    auto h = fRawYields.Get<TH1D>(::Form("Ntrk%dto%d", ntrks[i], ntrks[i+1]));
    auto hD0 = (TH1D*)h->Clone();
    hD0->GetYaxis()->SetTitle("1/N_{evt} dN/dp_{T} (GeV^{-1})");
    TGraphErrors* gfrac = fFrac.Get<TGraphErrors>(::Form("Ntrk%dto%d", ntrks[i], ntrks[i+1]));
    const auto npt = h->GetNbinsX();
    for (int ipt=0; ipt<npt; ++ipt) {
      const auto raw = h->GetBinContent(ipt+1);
      const auto rawErr = h->GetBinError(ipt+1);
      const auto eff = d0AccEff->GetY()[ipt];
      // const auto frac = bestFrac->GetBinContent(ipt+1);
      const auto frac = gfrac->GetY()[ipt];
      const auto yields = raw / eff / BR * frac / 2.; // average over particle and anti-particle
      const auto yields_err = rawErr / eff / BR * frac / 2.; // average over particle and anti-particle
      std::cout << raw << " " << eff << "\n";
      hD0->SetBinContent(ipt+1, yields);
      hD0->SetBinError(ipt+1, yields_err);
    }
    ofile.cd();
    hD0->Write();
    delete hD0;
  }
}
