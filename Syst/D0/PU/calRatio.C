
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

void calRatio()
{
  auto fdz1p0 = TFile("yields_dz1p0.root");
  auto fgplus = TFile("yields_gplus.root");
    TFile ofile("syst.root", "recreate");

  for (int ibin=0; ibin<nbins; ++ibin) {
    TH1D* hdz1p0 = fdz1p0.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    TH1D* hgplus = fgplus.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    TGraphAsymmErrors g;
    hdz1p0->Sumw2();
    hgplus->Sumw2();
    hgplus->Divide(hdz1p0);
    // g.Divide(hgplus, hdz1p0);
    std::cout << "gplus / dz1p0\n";
    // hgplus->Print("all");
    std::cout << "\n";
    TCanvas c;
    c.SetLeftMargin(0.14);
    auto hframe = c.DrawFrame(0, 0.98, 12, 1.02);
    hframe->SetTitle(";D^{0} p_{T} (GeV);Gplus / olv_dz1p0");
    // hgplus->Draw("SAME");
    // g.SetMarkerStyle(20);
    // g.SetMarkerSize(1.4);
    // g.Draw("PSAME");
    hgplus->SetMarkerStyle(20);
    hgplus->SetMarkerSize(1.4);
    hgplus->Draw("PSAME");
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

    TGraphErrors* gDiff = new TGraphErrors(hgplus);
    for (int ip=0; ip<gDiff->GetN(); ip++) {
      gDiff->SetPoint(ip, gDiff->GetX()[ip], std::abs(gDiff->GetY()[ip]-1));
      gDiff->SetPointError(ip, gDiff->GetEX()[ip], gDiff->GetEY()[ip]);
    }
    TF1 f("f", "[0]", 2, 10);
    f.SetParameter(0, 0);
    gDiff->Fit(&f);
    TGraphErrors* gSyst = new TGraphErrors(*gDiff);
    for (int ip=0; ip<gDiff->GetN(); ip++) {
      gSyst->SetPoint(ip, gSyst->GetX()[ip], f.GetParameter(0));
    }
  
    ofile.cd();
    gSyst->Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete gDiff;
    delete gSyst;
  }
}
