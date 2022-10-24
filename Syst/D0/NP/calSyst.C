
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.9, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

  TFile f("syst.root", "recreate");

void calSyst()
{
  TFile infile("../../../D0Productions/DCAFit/DCA_Chi2vsScale.root");
    TH1D* h = (TH1D*) infile.Get("bestScale");
    TH1D* h_lower = (TH1D*) infile.Get("bestScale_lower");
    TH1D* h_upper = (TH1D*) infile.Get("bestScale_upper");
  TFile ofile("syst.root", "recreate");
  for (int ibin=0; ibin<nbins; ++ibin) {
    TGraphErrors* g = new TGraphErrors(h);
    for (int ipt=0; ipt<nPt; ++ipt) {
      double upper = std::abs(h->GetBinContent(ipt+1) - h_upper->GetBinContent(ipt+1));
      double lower = std::abs(h->GetBinContent(ipt+1) - h_lower->GetBinContent(ipt+1));
      double err = std::max(upper, lower);
      g->SetPoint(ipt, g->GetX()[ipt], err);
      g->SetPointError(ipt, g->GetEX()[ipt], 0);
    }
    ofile.cd();
    g->Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete g;
  }
}
