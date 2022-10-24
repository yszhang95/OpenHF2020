void calRatio()
{
  const double pts[] = {3., 4., 5., 6., 8., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  TF1 f("f", "[0]", pts[0], pts[nPt]);
  f.SetParameter(0, 1);
  TFile infile("ratio.root", "READ");
  TH1D* h = infile.Get<TH1D>("hweighted");
  TH1D* hlo = infile.Get<TH1D>("hweighted_lo");
  TH1D* hup = infile.Get<TH1D>("hweighted_up");
  hlo->Divide(h);
  hup->Divide(h);
  hlo->Add(&f, -1);
  hup->Add(&f, -1);
  TCanvas c("c", "", 800, 550);
  c.SetLeftMargin(0.14);
  auto hframe = c.DrawFrame(0, -0.01, 10, 0.01);
  hframe->SetTitle("HighMultiplicity185;p_{T} (GeV);#Lambda_{c}^{+} alternative / default - 1");

  for (int ibin=0; ibin<hup->GetNbinsX(); ++ibin) {
    hup->SetBinError(ibin+1, 0);
  }
  for (int ibin=0; ibin<hlo->GetNbinsX(); ++ibin) {
    hlo->SetBinError(ibin+1, 0);
  }

  hup->SetMarkerStyle(20);
  hlo->SetMarkerStyle(kFullSquare);
  hup->SetLineColor(kRed);
  hup->SetMarkerColor(kRed);
  hlo->SetLineColor(kBlue);
  hlo->SetMarkerColor(kBlue);
  hup->Draw("PSAME");
  hlo->Draw("PSAME");
  TLine l(0., 0, 10, 0);
  l.SetLineStyle(2);
  l.Draw();
  TLegend leg(0.2, 0.75, 0.5, 0.89);
  leg.SetBorderSize(0);
  leg.SetFillStyle(0);
  leg.AddEntry(hup, "+1 #sigma", "p");
  leg.AddEntry(hlo, "-1 #sigma", "p");
  leg.Draw();

  c.Print("ratio_HMSyst.pdf");
}
