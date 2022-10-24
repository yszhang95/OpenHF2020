void plotRatioVsPt();
void plotRatioVsNtrk();
void plotRatio()
{
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
  plotRatioVsPt();
  plotRatioVsNtrk();
}
void plotRatioVsPt()
{
  TFile infile("ratio_vs_pt_stat.root");
  TFile infileSyst("ratio_vs_pt_syst.root");
  auto hNtrk0to35 = infile.Get<TH1D>("Ntrk0to35");
  auto hNtrk185to250 = infile.Get<TH1D>("Ntrk185to250");

  auto hNtrk0to35Syst = infileSyst.Get<TH1D>("Ntrk0to35");
  auto hNtrk185to250SYst = infileSyst.Get<TH1D>("Ntrk185to250");

  TGraphErrors* Ntrk0to35 = new TGraphErrors(hNtrk0to35);
  TGraphErrors* Ntrk185to250 = new TGraphErrors(hNtrk185to250);
  Ntrk185to250->RemovePoint(0);

  TGraphErrors* Ntrk0to35Syst = new TGraphErrors(hNtrk0to35Syst);
  TGraphErrors* Ntrk185to250Syst = new TGraphErrors(hNtrk185to250SYst);
  Ntrk185to250Syst->RemovePoint(0);

  Ntrk185to250->SetMarkerStyle(kFullCircle);
  Ntrk185to250->SetMarkerColor(kRed);
  Ntrk185to250->SetLineColor(kRed);
  Ntrk0to35->SetMarkerStyle(kFullSquare);
  Ntrk0to35->SetMarkerColor(kBlue);
  Ntrk0to35->SetLineColor(kBlue);

  Ntrk0to35Syst->SetFillColorAlpha(kBlue-3, 0.3);
  Ntrk185to250Syst->SetFillColorAlpha(kRed-3, 0.3);
  Ntrk0to35Syst->SetLineWidth(0);
  Ntrk185to250Syst->SetLineWidth(0);
  Ntrk0to35Syst->SetMarkerSize(0);
  Ntrk185to250Syst->SetMarkerSize(0);

  TGraphErrors* Ntrk220to260_s_stat = new TGraphErrors("ratio_LamToKs_Ntrk220to260_stat.txt");
  TGraphErrors* Ntrk220to260_s_syst = new TGraphErrors("ratio_LamToKs_Ntrk220to260_syst.txt");
  TGraphErrors* Ntrk0to35_s_stat = new TGraphErrors("ratio_LamToKs_Ntrk0to35_stat.txt");
  TGraphErrors* Ntrk0to35_s_syst = new TGraphErrors("ratio_LamToKs_Ntrk0to35_syst.txt");

  Ntrk220to260_s_stat->SetMarkerStyle(kOpenCircle);
  // Ntrk220to260_s_stat->SetMarkerColor(kBlack);
  // Ntrk220to260_s_stat->SetLineColor(kBlack);
  Ntrk220to260_s_syst->SetMarkerSize(0);
  Ntrk220to260_s_syst->SetLineWidth(0);
  Ntrk220to260_s_syst->SetFillColorAlpha(14, 0.3);

  Ntrk0to35_s_stat->SetMarkerStyle(kOpenSquare);
  // Ntrk0to35_s_stat->SetMarkerColor(kBlack);
  // Ntrk0to35_s_stat->SetLineColor(kBlack);
  Ntrk0to35_s_syst->SetMarkerSize(0);
  Ntrk0to35_s_syst->SetLineWidth(0);
  Ntrk0to35_s_syst->SetFillColorAlpha(14, 0.3);

  TCanvas c("c_ratio_vs_pt", "", 600, 480);
  c.SetRightMargin(0.05);
  c.SetLeftMargin(0.13);
  c.SetTopMargin(0.08);
  c.SetBottomMargin(0.12);
  c.cd();
  auto hframe = c.DrawFrame(0, 0, 10, 1.4);
  hframe->SetTitle(";p_{T} (GeV);Baryon-to-meson ratio");
  hframe->GetXaxis()->SetTitleSize(0.042);
  hframe->GetYaxis()->SetTitleSize(0.042);
  hframe->GetXaxis()->CenterTitle();
  hframe->GetYaxis()->CenterTitle();

  Ntrk220to260_s_syst->Draw("E2");
  Ntrk0to35_s_syst->Draw("E2");
  Ntrk220to260_s_stat->Draw("P");
  Ntrk0to35_s_stat->Draw("P");

  Ntrk0to35Syst->Draw("E2");
  Ntrk185to250Syst->Draw("E2");
  Ntrk0to35->Draw("PSAME");
  Ntrk185to250->Draw("PSAME");

  TLegend leg(0.14, 0.75, 0.52,0.88);
  leg.SetHeader("#Lambda_{c}^{+} + #Lambda_{c}^{-}/ D^{0} + #bar{D^{0}}, #sqrt{s_{NN}}=8.16 TeV");
  leg.AddEntry(Ntrk0to35, "N_{trk}^{offline} < 35", "P");
  leg.AddEntry(Ntrk185to250, "185 < N_{trk}^{offline} < 250", "P");
  leg.SetBorderSize(0);
  leg.SetFillStyle(0);
  leg.Draw();

  TLegend leg2(0.56, 0.75, 0.89,0.88);
  leg2.SetHeader("(#Lambda + #bar{#Lambda})/2K_{S}^{0}, #sqrt{s_{NN}}=5.02 TeV");
  leg2.AddEntry(Ntrk0to35_s_stat, "N_{trk}^{offline} < 35", "P");
  leg2.AddEntry(Ntrk220to260_s_stat, "220 < N_{trk}^{offline} < 260", "P");
  leg2.SetBorderSize(0);
  leg2.SetFillStyle(0);
  leg2.Draw();

  TLatex texCMS;
  texCMS.SetTextAlign(11);
  texCMS.SetTextSize(0.05);
  texCMS.SetTextFont(42);
  texCMS.DrawLatexNDC(0.13, 0.935, "CMS #it{#scale[0.9]{preliminary}}");
  texCMS.SetTextAlign(31);
  texCMS.SetTextSize(0.045);
  // texCMS.DrawLatexNDC(0.95, 0.935, "pPb 8.16 TeV");
  texCMS.DrawLatexNDC(0.95, 0.935, "pPb 97.8 nb^{-1}");

  TLatex tex;
  tex.SetTextFont(42);
  tex.SetTextSize(0.042);
  tex.SetTextAlign(13);
  tex.DrawLatexNDC(0.18, 0.71, ::Form("-1.46 < y_{cm} < 0.54"));
  tex.DrawLatexNDC(0.67, 0.71, ::Form("|y_{cm}| < 1"));
  c.Print("ratio_vs_pt.pdf");
}
void plotRatioVsNtrk()
{
  const double pTMin = 3;
  const double pTMax = 5;
  TFile infile("ratio_vs_Ntrk_stat.root");
  TFile infileSyst("ratio_vs_Ntrk_syst.root");

  auto h_ratio_vs_Ntrk = infile.Get<TH1D>(::Form("pT%.0fto%.0f", pTMin, pTMax));
  auto h_ratio_vs_Ntrk_syst = infileSyst.Get<TH1D>(::Form("pT%.0fto%.0f", pTMin, pTMax));

  TGraphErrors* ratio_vs_Ntrk = new TGraphErrors(h_ratio_vs_Ntrk);
  TGraphErrors* ratio_vs_Ntrk_syst = new TGraphErrors(h_ratio_vs_Ntrk_syst);

  TGraphErrors* ratio_vs_Ntrk_s_stat = new TGraphErrors("ratio_LamToKs_pT3to5_stat.txt");
  TGraphErrors* ratio_vs_Ntrk_s_syst = new TGraphErrors("ratio_LamToKs_pT3to5_syst.txt");

  ratio_vs_Ntrk->SetMarkerStyle(20);
  ratio_vs_Ntrk->SetMarkerColor(kBlue);
  ratio_vs_Ntrk->SetMarkerSize(1.4);
  ratio_vs_Ntrk->SetLineColor(kBlue);
  // ratio_vs_Ntrk->SetFillColorAlpha(kBlue-3, 0.3);
  // ratio_vs_Ntrk->SetFillStyle(0);

  ratio_vs_Ntrk_syst->SetMarkerSize(0);
  ratio_vs_Ntrk_syst->SetLineWidth(0);
  ratio_vs_Ntrk_syst->SetFillColorAlpha(kBlue-3, 0.3);

  ratio_vs_Ntrk_s_stat->SetMarkerStyle(kOpenSquare);
  ratio_vs_Ntrk_s_stat->SetMarkerSize(1.4);
  // ratio_vs_Ntrk_s_stat->SetFillStyle(0);
  // ratio_vs_Ntrk_s_stat->SetFillColorAlpha(14, 0.3);
  ratio_vs_Ntrk_s_syst->SetFillColorAlpha(14, 0.3);
  ratio_vs_Ntrk_s_syst->SetMarkerSize(0);

  TCanvas c("c_ratio_vs_Ntrk", "", 600, 480);
  c.SetRightMargin(0.05);
  c.SetLeftMargin(0.13);
  c.SetTopMargin(0.08);
  c.SetBottomMargin(0.12);
  c.cd();
  auto hframe = c.DrawFrame(0, 0, 260, 1.4);
  hframe->SetTitle(";N_{trk}^{offline};Baryon-to-meson ratio");
  hframe->GetXaxis()->SetTitleSize(0.042);
  hframe->GetYaxis()->SetTitleSize(0.042);
  hframe->GetXaxis()->CenterTitle();
  hframe->GetYaxis()->CenterTitle();

  ratio_vs_Ntrk_s_syst->Draw("E2");
  ratio_vs_Ntrk_s_stat->Draw("PE");

  ratio_vs_Ntrk_syst->Draw("E2");
  ratio_vs_Ntrk->Draw("PE");

  TLegend leg(0.14, 0.67, 0.54,0.88);
  leg.AddEntry(ratio_vs_Ntrk, "#splitline{#Lambda_{c}^{+} + #Lambda_{c}^{-} / D^{0} + #bar{D}^{0}}{-1.46 < y_{cm} < 0.54, #sqrt{s_{NN}}=8.16 TeV}", "P");
  leg.AddEntry(ratio_vs_Ntrk_s_stat, "#splitline{#Lambda + #bar{#Lambda} / 2K_{S}^{0}}{|y_{cm}|<1, #sqrt{s_{NN}}=5.02 TeV}", "P");
  leg.SetBorderSize(0);
  leg.SetFillStyle(0);
  leg.Draw();

  TLatex tex;
  tex.SetTextFont(42);
  tex.SetTextSize(0.04);
  tex.SetTextAlign(13);
  tex.DrawLatexNDC(0.14, 0.65, ::Form("%.1f < p_{T} < %.1f GeV", pTMin, pTMax));

  TLatex texCMS;
  texCMS.SetTextAlign(11);
  texCMS.SetTextSize(0.05);
  texCMS.SetTextFont(42);
  texCMS.DrawLatexNDC(0.13, 0.935, "CMS #it{#scale[0.9]{preliminary}}");
  texCMS.SetTextAlign(31);
  texCMS.SetTextSize(0.045);
  // texCMS.DrawLatexNDC(0.95, 0.935, "pPb 8.16 TeV");
  texCMS.DrawLatexNDC(0.95, 0.935, "pPb 97.8 nb^{-1}");
  c.Print("ratio_vs_Ntrk.pdf");
}
