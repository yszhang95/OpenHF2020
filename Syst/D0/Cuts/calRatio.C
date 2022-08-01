void calRatio(const float pass, const float total, const float pass_err, const float total_err,
              float& ratio, float& err, const bool reverse=false)
{
  if (reverse) ratio = total/pass;
  else ratio = pass/total;
  err = ratio * std::sqrt(std::abs(std::pow(pass_err/pass, 2)
                                   + std::pow(total_err/total, 2)
                                   - 2*total_err*total_err/pass/total));
}
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

void calRatio()
{
  auto fstd = TFile("yields_std.root");
  auto floose = TFile("yields_loose.root");
  auto ftight = TFile("yields_tight.root");

  TGraphErrors* gSyst[nPt];

  for (int ibin=0; ibin<nbins; ++ibin) {
    TH1D* hloose= floose.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    TH1D* htight = ftight.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    TH1D* hstd = fstd.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    // TGraphAsymmErrors gtight;
    // TGraphAsymmErrors gloose;
    TGraphErrors gtight(nPt);
    TGraphErrors gloose(nPt);
    gSyst[ibin] = new TGraphErrors(nPt);
    for (int ipt=0; ipt<nPt; ++ipt) {
      float ratio_loose(0); float err_loose(0);
      float ratio_tight(0); float err_tight(0);
      calRatio(hstd->GetBinContent(ipt+1), hloose->GetBinContent(ipt+1),
                hstd->GetBinError(ipt+1), hloose->GetBinError(ipt+1),
                ratio_loose, err_loose, true);
      calRatio(htight->GetBinContent(ipt+1), hstd->GetBinContent(ipt+1),
                htight->GetBinError(ipt+1), hstd->GetBinError(ipt+1),
                ratio_tight, err_tight);
      const double x_err = hstd->GetBinCenter(ipt+1) - hstd->GetBinLowEdge(ipt+1);
      const double x = hstd->GetBinCenter(ipt+1);
      gtight.SetPoint(ipt, x, ratio_tight);
      gtight.SetPointError(ipt, x_err, err_tight);
      gloose.SetPoint(ipt, x, ratio_loose);
      gloose.SetPointError(ipt, x_err, err_loose);
      double syst = 0;
      if (ibin == nbins-1) {
        syst = std::abs(ratio_loose-1);
      } else {
        syst = std::max(std::abs(ratio_loose-1), std::abs(ratio_tight-1));
      }
      gSyst[ibin]->SetPoint(ipt, x, syst);
      gSyst[ibin]->SetPointError(ipt, x_err, syst);
    }
    // gtight.Print();
    // gloose.Print();
    // std::cout << "\n";
    TCanvas c;
    c.SetLeftMargin(0.14);
    auto hframe = c.DrawFrame(0, 0.9, 12, 1.1);
    hframe->SetTitle(";D^{0} p_{T} (GeV);alternative/ default");
    gtight.SetMarkerStyle(20);
    gtight.SetMarkerSize(1.4);
    gtight.SetMarkerColor(kBlue);
    gtight.Draw("PSAME");
    gloose.SetMarkerStyle(kFullSquare);
    gloose.SetMarkerSize(1.4);
    gloose.SetMarkerColor(kRed);
    gloose.Draw("PSAME");
    TLegend leg(0.2, 0.75, 0.45, 0.89);
    leg.SetFillStyle(0);
    leg.SetBorderSize(0);
    leg.AddEntry(&gtight, "tight / default", "P");
    leg.AddEntry(&gloose, "loose / default", "P");
    leg.Draw();
    TLatex tex;
    tex.SetTextFont(42);
    tex.SetTextAlign(11);
    tex.DrawLatexNDC(0.14, 0.92, "CMS Preliminary");
    tex.SetTextAlign(31);
    tex.DrawLatexNDC(0.9, 0.92, "pPb 8.16 TeV");
    tex.SetTextAlign(12);
    tex.DrawLatexNDC(0.2, 0.3, ::Form("%d #leq N_{trk}^{offline} < %d", Ntrks[ibin], Ntrks[ibin+1]));
    c.Print(::Form("plots/Ntrk%dto%d.pdf", Ntrks[ibin], Ntrks[ibin+1]));
    c.Print(::Form("plots/Ntrk%dto%d.png", Ntrks[ibin], Ntrks[ibin+1]));

  }
  TFile ofile("syst.root", "recreate");
  ofile.cd();
  gSyst[0]->Write("Ntrk0to35");
  gSyst[0]->Write("Ntrk35to60");
  gSyst[0]->Write("Ntrk60to90");
  gSyst[0]->Write("Ntrk60to90");
  gSyst[0]->Write("Ntrk90to120");
  gSyst[0]->Write("Ntrk120to185");
  gSyst[1]->Write("Ntrk185to250");
  cout << "Finished writing to syst.root" << std::endl;
  // for (auto g : gSyst) delete g;
}
