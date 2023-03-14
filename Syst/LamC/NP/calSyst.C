TH1* compute_diff(TH1* hstd, TH1* halt, std::string name)
{
  TH1* h = (TH1*) hstd->Clone(name.c_str());
  for (int i=0; i<h->GetNbinsX()+2; ++i) {
    const double y1 = hstd->GetBinContent(i);
    const double y2 = halt->GetBinContent(i);
    h->SetBinContent(i, std::abs(y1-y2));
  }
  return h;
}

void calSyst()
{
  /*
  double std[] = {0.031, 0.026, 0.024, 0.035, 0.068, 0.058};
  double alt[] = {0.022, 0.026, 0.034, 0.06, 0.14, 0.1};
  */

  TFile f("syst.root", "recreate");

  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;
  for (const auto trig : {"HM", "MB"}) {
    std::cout << trig << "===============\n";
    // std
    TFile f_std(::Form("../../../PromptExtraction/FONLL/LamC/final_%s_bdecay_frac.root", trig));
    auto h_std = f_std.Get<TH1D>("hfrac");
    // pythia
    TFile f_pythia(::Form("../../../PromptExtraction/FONLL/LamC/final_%s_bdecay_frac_pythia.root", trig));
    auto h_pythia = f_pythia.Get<TH1D>("hfrac");
    // fonll
    TFile f_fonll(::Form("../../../PromptExtraction/FONLL/LamC/final_%s_bdecay_frac_modifyFONLL.root", trig));
    auto h_fonll = f_fonll.Get<TH1D>("hfrac");
    // FF
    TFile f_ff(::Form("../../../PromptExtraction/FONLL/LamC/final_%s_bdecay_frac_modifyFF.root", trig));
    auto h_ff = f_ff.Get<TH1D>("hfrac");
    // RpA
    TFile f_RpA(::Form("../../../PromptExtraction/FONLL/LamC/final_%s_bdecay_frac_ModifyRpA.root", trig));
    auto h_RpA= f_RpA.Get<TH1D>("hfrac");
    // compute
    // pythia
    auto h_diff_pythia = compute_diff(h_std, h_pythia, "h_diff_pythia");
    auto h_diff_fonll = compute_diff(h_std, h_fonll, "h_diff_fonll");
    auto h_diff_ff = compute_diff(h_std, h_ff, "h_diff_ff");
    auto h_diff_RpA = compute_diff(h_std, h_RpA, "h_diff_RpA");
    h_diff_pythia->Print("all");
    h_diff_fonll->Print("all");
    h_diff_ff->Print("all");
    h_diff_RpA->Print("all");

    TH1D h("h", "", nPt, pts);
    TGraphErrors* g = new TGraphErrors(&h);
    for (int ipt=0; ipt<nPt; ipt++) {
      double diff_pythia = h_diff_pythia->GetBinContent(ipt+1);
      double diff_fonll = h_diff_fonll->GetBinContent(ipt+1);
      double diff_ff = h_diff_ff->GetBinContent(ipt+1);
      double diff_RpA = h_diff_RpA->GetBinContent(ipt+1);

      double error = std::sqrt(
          std::pow(diff_pythia, 2)
          + std::pow(diff_fonll, 2)
          + std::pow(diff_ff, 2)
          + std::pow(diff_RpA, 2)
          + 0.18*0.18
          );

      g->SetPoint(ipt, g->GetX()[ipt], error);
    }

    if (!strcmp(trig, "HM")) {
      f.cd();
      int ibin = nbins-1;
      g->Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
      // delete g;
    } else {
      f.cd();
      for (int ibin=0; ibin<nbins-1; ++ibin) {
        g->Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
        // delete g;
      }
    }

  }
}
