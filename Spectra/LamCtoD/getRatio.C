void getRatioVsPt(const std::string type="stat")
{
  std::string LcName, D0Name;
  if (type == "stat") {
    LcName = "../LcSpectra/spectra_dz1p0_LamC.root";
    D0Name = "../D0Spectra/spectra_dz1p0_D0.root";
  }
  else if (type == "syst") {
    // 2.3 for tracking is missing
    LcName = "../../Syst/LamC/syst_LamC.root";
    D0Name = "../../Syst/D0/syst_D0.root";
  }
  TFile LcSpectra(LcName.c_str());
  TFile D0Spectra(D0Name.c_str());

  TFile fRatio(::Form("ratio_vs_pt_%s.root", type.c_str()), "recreate");

  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const int nbins = sizeof(Ntrks)/sizeof(int)-1;
  for (int ibin=0; ibin<nbins; ++ibin) {
    auto Lc = LcSpectra.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[1+ibin]));
    auto D0 = D0Spectra.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[1+ibin]));
    auto ratio = (TH1D*)Lc->Clone();
    const auto nPt = Lc->GetNbinsX();
    // hard code 
    for (int ipt=0; ipt<4; ++ipt) {
      if (ibin == nbins-1 && ipt==0) {
        ratio->SetBinContent(ipt+1, 0);
        ratio->SetBinError(ipt+1, 0);
      } else {
        const double yLc = Lc->GetBinContent(ipt+1);
        const double yerrLc = Lc->GetBinError(ipt+1);
        const double yD0 = D0->GetBinContent(ipt+1);
        const double yerrD0 = D0->GetBinError(ipt+1);
        const double y = yLc/yD0;
        const double yerr = y * sqrt(yerrLc*yerrLc/yLc/yLc +
            yerrD0*yerrD0/yD0/yD0);
        ratio->SetBinContent(ipt+1, y);
        ratio->SetBinError(ipt+1, yerr);
      }
    }
    // 6to8 
    {
      int ipt=4;
        const double yLc = Lc->GetBinContent(ipt+1) * Lc->GetBinWidth(ipt+1);
        const double yerrLc = Lc->GetBinError(ipt+1) * Lc->GetBinWidth(ipt+1);
        const int bin = D0->FindBin(6.5);
        double yD0 = D0->GetBinContent(bin) * D0->GetBinWidth(bin) + D0->GetBinContent(bin+1) * D0->GetBinWidth(bin+1);
        double yerrD0 = sqrt(pow(D0->GetBinError(bin) * D0->GetBinWidth(bin), 2)
            + pow(D0->GetBinError(bin+1) * D0->GetBinWidth(bin+1), 2));
        const double y = yLc/yD0;
        const double yerr = y * sqrt(yerrLc*yerrLc/yLc/yLc +
            yerrD0*yerrD0/yD0/yD0);
        ratio->SetBinContent(ipt+1, y);
        ratio->SetBinError(ipt+1, yerr);
    }
    // 8to10
    {
      int ipt=5;
        const double yLc = Lc->GetBinContent(ipt+1) * Lc->GetBinWidth(ipt+1);
        const double yerrLc = Lc->GetBinError(ipt+1) * Lc->GetBinWidth(ipt+1);
        const int bin = D0->FindBin(7.5);
        double yD0 = D0->GetBinContent(bin) * D0->GetBinWidth(bin) + D0->GetBinContent(bin+1) * D0->GetBinWidth(bin+1);
        double yerrD0 = sqrt(pow(D0->GetBinError(bin) * D0->GetBinWidth(bin), 2)
            + pow(D0->GetBinError(bin+1) * D0->GetBinWidth(bin+1), 2));
        const double y = yLc/yD0;
        const double yerr = y * sqrt(yerrLc*yerrLc/yLc/yLc +
            yerrD0*yerrD0/yD0/yD0);
        ratio->SetBinContent(ipt+1, y);
        ratio->SetBinError(ipt+1, yerr);
    }
    fRatio.cd();
    ratio->Write();
    delete ratio;
  }
}

void getRatioVsNtrk(std::string type="stat")
{
  const float pTMin = 3;
  const float pTMax = 5;
  std::string LcName, D0Name;
  if (type == "stat") {
    LcName = "../LcSpectra/spectra_dz1p0_LamC.root";
    D0Name = "../D0Spectra/spectra_dz1p0_D0.root";
  }
  else if (type == "syst") {
    // 2.3 for tracking is missing
    LcName = "../../Syst/LamC/syst_LamC.root";
    D0Name = "../../Syst/D0/syst_D0.root";
  }
  TFile LcSpectra(LcName.c_str());
  TFile D0Spectra(D0Name.c_str());

  TFile fRatio(::Form("ratio_vs_Ntrk_%s.root", type.c_str()), "recreate");
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const double NtrksF[] = {0, 35, 60, 90, 120, 185, 250};
  const int nbins = sizeof(Ntrks)/sizeof(int)-1;
  TH1D h(::Form("pT%.0fto%.0f", pTMin, pTMax), "", nbins, NtrksF);
  for (int ibin=0; ibin<nbins; ++ibin) {
    auto Lc = LcSpectra.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[1+ibin]));
    auto D0 = D0Spectra.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[1+ibin]));

    std::vector<double> yLc;
    std::vector<double> yerrLc;
    for (int ipt=0; ipt<Lc->GetNbinsX(); ++ipt) {
      const double x = Lc->GetBinCenter(ipt+1);
      if (x<pTMax && x>pTMin) {
        std::cout << x << "\n";
        const double temp = Lc->GetBinContent(ipt+1) * Lc->GetBinWidth(ipt+1);
        const double temp_err = Lc->GetBinError(ipt+1) * Lc->GetBinWidth(ipt+1);
        yLc.push_back(temp);
        yerrLc.push_back(temp_err);
      }
    }


    std::vector<double> yD0;
    std::vector<double> yerrD0;
    for (int ipt=0; ipt<D0->GetNbinsX(); ++ipt) {
      const double x = D0->GetBinCenter(ipt+1);
      if (x<pTMax && x>pTMin) {
        std::cout << x << "\n";
        const double temp = D0->GetBinContent(ipt+1) * D0->GetBinWidth(ipt+1);
        const double temp_err = D0->GetBinError(ipt+1) * D0->GetBinWidth(ipt+1);
        yD0.push_back(temp);
        yerrD0.push_back(temp_err);
      }
    }

    double yieldsLc = 0;
    double yieldsErr2Lc = 0;
    for (int i=0; i<yLc.size(); ++i) {
      yieldsLc += yLc[i];
      yieldsErr2Lc += yerrLc[i]*yerrLc[i];
    }
    double yieldsD0 = 0;
    double yieldsErr2D0 = 0;
    for (int i=0; i<yD0.size(); ++i) {
      yieldsD0 += yD0[i];
      yieldsErr2D0 += yerrD0[i]*yerrD0[i];
    }

    const double ratio = yieldsLc/ yieldsD0;
    const double err = ratio*sqrt(yieldsErr2Lc/yieldsLc/yieldsLc+
        yieldsErr2D0/yieldsD0/yieldsD0
        );
    h.SetBinContent(ibin+1, ratio);
    h.SetBinError(ibin+1, err);
  }
  fRatio.cd();
  h.Write();
}

void getRatio()
{
  getRatioVsPt();
  cout << "pT stat" << endl;
  getRatioVsNtrk();
  cout << "Ntrk stat" << endl;

  getRatioVsPt("syst");
  cout << "pT syst" << endl;
  getRatioVsNtrk("syst");
  cout << "Ntrk  syst" << endl;
}
