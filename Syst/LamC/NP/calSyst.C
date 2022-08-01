void calSyst()
{
  double std[] = {0.031, 0.026, 0.024, 0.035, 0.068, 0.058};
  double alt[] = {0.022, 0.026, 0.034, 0.06, 0.14, 0.1};
  
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

  TFile f("syst.root", "recreate");
  for (int ibin=0; ibin<nbins; ++ibin) {
    TH1D h("h", "", nPt, pts);
    TGraphErrors* g = new TGraphErrors(&h);
    for (int ipt=0; ipt<nPt; ipt++) {
      g->SetPoint(ipt, g->GetX()[ipt], std::abs(alt[ipt] - std[ipt]));
    }
    f.cd();
    g->Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete g;
  }
}
