void plotSpectra(int pdgId = 4122)
{
  const int ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const int nbins = sizeof(ntrks)/sizeof(int) - 1;
  Style_t markers[nbins] = {kFullSquare, kFullDiamond, kFullCrossX, kFullTriangleUp, kFullCross, kFullCircle};
  Color_t m_colors[nbins] = {kBlue, kCyan-3, kSpring-1, kOrange-6, kOrange, kRed};
  Color_t f_colors[nbins] = {kBlue-3, kCyan-3, kSpring-1, kOrange-6, kOrange, kRed-3};
  TGraphMultiErrors* spectra[nbins];

  std::string stat_name;
  std::string syst_name;
  std::string legHeader;
  std::string outName;
  if (pdgId == 4122) {
    stat_name = "../LcSpectra/spectra_dz1p0_LamC.root";
    syst_name = "../../Syst/LamC/syst_LamC_total.root";
    legHeader = "#Lambda_{c}^{+} + #Lambda_{c}^{-}";
    outName = "LamCSpectra";
  } else if (pdgId == 410) {
    stat_name = "../D0Spectra/spectra_dz1p0_D0.root";
    syst_name = "../../Syst/D0/syst_D0_total.root";
    legHeader = "D^{0} + #bar{D^{0}}";
    outName = "D0Spectra";
  }

  TFile fstat(stat_name.c_str());
  TFile fsyst(syst_name.c_str());

  TCanvas c("c", "", 600, 480);
  c.SetLeftMargin(0.15);
  c.SetTopMargin(0.08);
  c.SetBottomMargin(0.13);
  c.SetRightMargin(0.04);
  c.SetLogy();
  auto h = c.DrawFrame(2, 1E-6, 10, 1);
  h->SetTitle(";p_{T} (GeV);1/N_{evt}dN/dp_{T} (GeV^{-1})");
  h->GetXaxis()->CenterTitle();
  h->GetYaxis()->CenterTitle();
  h->GetXaxis()->SetTitleSize(0.04);
  h->GetYaxis()->SetTitleSize(0.04);

  TLegend leg(0.17, 0.15, 0.5, 0.45);
  leg.SetFillStyle(0);
  leg.SetBorderSize(0);
  leg.SetHeader(legHeader.c_str(), "c");

  for (int ibin=0; ibin<nbins; ++ibin) {
    auto hstat = fstat.Get<TH1D>(::Form("Ntrk%dto%d", ntrks[ibin], ntrks[ibin+1]));
    auto hsyst = fsyst.Get<TH1D>(::Form("Ntrk%dto%d", ntrks[ibin], ntrks[ibin+1]));
    spectra[ibin] = new TGraphMultiErrors(hstat);
    TGraphErrors g(hsyst);
    spectra[ibin]->AddYError(spectra[ibin]->GetN(), g.GetEY(), g.GetEY());
    spectra[ibin]->SetMarkerStyle(markers[ibin]);
    spectra[ibin]->SetMarkerColor(m_colors[ibin]);
    spectra[ibin]->SetLineColor(m_colors[ibin]);
    spectra[ibin]->GetAttLine(1)->SetLineColor(f_colors[ibin]);
    spectra[ibin]->GetAttFill(1)->SetFillColorAlpha(f_colors[ibin], 0.35);
    spectra[ibin]->Draw("PS ; Z ; 5 s=0.35");
    std::string label = ibin > 0? ::Form("%d #leq N_{trk}^{offline} < %d", ntrks[ibin], ntrks[ibin+1]) : ::Form("N_{trk}^{offline} < %d", ntrks[ibin+1]);
    leg.AddEntry(spectra[ibin] ,label.c_str(), "p");
  }
  leg.Draw();
  TLatex texCMS;
  texCMS.SetTextSize(0.055);
  texCMS.SetTextFont(42);
  texCMS.SetTextAlign(11);
  texCMS.DrawLatexNDC(0.15, 0.93, "CMS #it{#scale[0.95]{Preliminary}}");
  TLatex tex;
  tex.SetTextSize(0.05);
  tex.SetTextAlign(31);
  tex.SetTextFont(42);
  tex.DrawLatexNDC(0.96, 0.93, "pPb 8.16 TeV");
  gPad->SetTickx(1);
  gPad->SetTicky(1);

  c.Print(::Form("%s.pdf", outName.c_str()));
}
