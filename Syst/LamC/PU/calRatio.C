void calRatio()
{
  const unsigned int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const double NtrksF[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  auto fdz1p0 = TFile("yields_dz1p0.root");
  auto fgplus = TFile("yields_gplus.root");
  TFile ofile("ratio.root", "recreate");
  for (int itrk=0; itrk<ntrks; ++itrk) {
    std::string histName =  ::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]);
    TH1D* hdz1p0 = (TH1D*) fdz1p0.Get(histName.c_str());
    TH1D* hgplus = (TH1D*) fgplus.Get(histName.c_str());
    for (int i=1; i<= hdz1p0->GetNbinsX(); ++i) {
      double y1 = hgplus->GetBinContent(i);
      double y2 = hdz1p0->GetBinContent(i);
      double e1 = hgplus->GetBinError(i);
      double e2 = hdz1p0->GetBinError(i);
      double e = y1/y2 * std::sqrt(std::abs(e1*e1/y1/y1 + e2*e2/y2/y2 - 2*e2*e2/y1/y2));
      hgplus->SetBinContent(i, y1/y2);
      hgplus->SetBinError(i, e);
    }
    ofile.cd();
    hgplus->Write();
  }
}
