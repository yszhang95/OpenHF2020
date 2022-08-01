
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;
void calSyst()
{
  const int nSources = 4;
  TFile* file[nSources];
  file[0] = TFile::Open("Cuts/syst.root");
  file[1] = TFile::Open("FitOrder/syst.root");
  file[2] = TFile::Open("PU/syst.root");
  file[3] = TFile::Open("NP/syst.root");
  TFile stat("../../Spectra/LcSpectra/spectra_dz1p0_LamC.root");
  TFile ofile("syst_LamC.root", "recreate");
  TFile ofile2("syst_LamC_total.root", "recreate");
  std::vector<double> errs;
  for (int ibin=0; ibin<nbins; ++ibin) {
    TGraphErrors* g[nSources];
    for (int i=0; i<nSources; ++i) {
      g[i] = (TGraphErrors*) file[i]->Get(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
      // g[i]->Print();
    }
    TH1D* h = (TH1D*)stat.Get(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    // TGraphErrors* gSyst = new TGraphErrors(h);
    // for (int ip=0; ip<gSyst->GetN(); ++ip) {
    //   if (ibin == nbins-1 && ip==0 ) continue;
    //   double err2 = 0;
    //   for (int i=0; i<nSources; ++i) {
    //     err2 += std::pow(g[i]->GetY()[ip], 2);
    //   }
    //   gSyst->SetPointError(ip, gSyst->GetEX()[ip], std::sqrt(err2) * gSyst->GetY()[ip]);
    // }
    TH1D* hSyst = (TH1D*)h->Clone();
    TH1D* hSyst2 = (TH1D*)h->Clone();
    for (int ip=0; ip<hSyst->GetNbinsX(); ++ip) {
      double err2 = 0;
      for (int i=0; i<nSources; ++i) {
        err2 += std::pow(g[i]->GetY()[ip], 2);
      }
      // std::cout << sqrt(err2) << std::endl;
      hSyst->SetBinError(ip+1, sqrt(err2) * hSyst->GetBinContent(ip+1));
      err2 += std::pow(0.023*3, 2);
      if (ibin == nbins-1 && ip==0) continue;
      errs.push_back(sqrt(err2));
      hSyst2->SetBinError(ip+1, sqrt(err2) * hSyst2->GetBinContent(ip+1));
    }
    ofile.cd();
    // gSyst->Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    hSyst->Write();
    ofile2.cd();
    hSyst2->Write();
  }
  std::cout << *std::max_element(errs.begin(), errs.end()) << "\n";
  std::cout << *std::min_element(errs.begin(), errs.end()) << "\n";
}
