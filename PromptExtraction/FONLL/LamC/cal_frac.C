void cal_frac(std::string input="bdecay_frac.root")
{
  for (const auto trig : {"HM", "MB"}) {
    TFile eff_P(::Form("../../../Efficiency/LamC/avg_eff_%s.root", trig));
    TFile eff_NP(::Form("../../../Efficiency/LamC/avg_eff_%s_NP.root", trig));
    TGraphAsymmErrors* gp = eff_P.Get<TGraphAsymmErrors>("AvgAccEff");
    TGraphAsymmErrors* gnp = eff_NP.Get<TGraphAsymmErrors>("AvgAccEff");
    gp->Print();
    gnp->Print();
    TFile infile(input.c_str());
    TFile output(::Form("final_%s_%s", trig, input.c_str()), "RECREATE");
    auto hfrac = infile.Get<TH1>("hfrac");
    auto nbins = hfrac->GetNbinsX();
    for (int i=1; i<nbins+1; ++i) {
      if (i==1 && !strcmp(trig, "HM")) continue;

      int ip_p = i-1;
      int ip_np = i-1;
      if (gp->GetN() - gnp->GetN() == 1) {
        ip_np = ip_p-1;
      }
      
      auto y = hfrac->GetBinContent(i);
      std::cout << gp->GetX()[ip_p] << " " << gnp->GetX()[ip_np] << "\n";
      std::cout << y << " " << gnp->GetY()[ip_np]/gp->GetY()[ip_p] << "\n";
      y = y * gnp->GetY()[ip_np]/gp->GetY()[ip_p];
      hfrac->SetBinContent(i, y);
    }
    output.cd();
    hfrac->Write();
  }
}
