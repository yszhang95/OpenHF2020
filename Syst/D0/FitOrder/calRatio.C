
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

void calRatio()
{
  auto f2 = TFile("yields_order2.root");
  auto f3 = TFile("yields_order3.root");
  auto f4 = TFile("yields_order4.root");

  TFile ofile("syst.root", "recreate");
  for (int ibin=0; ibin<nbins; ++ibin) {
    TH1D* h2 = f2.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    TH1D* h3 = f3.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    TH1D* h4 = f4.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    h2->Divide(h3);
    h4->Divide(h3);
    /*
    std::cout << "2nd / 3rd\n";
    h2->Print("all");
    std::cout << "\n";
    std::cout << "4th / 3rd\n";
    h4->Print("all");
    std::cout << "\n";
    */
    TCanvas c;
    c.SetLeftMargin(0.14);
    auto hframe = c.DrawFrame(0, 0.98, 12, 1.02);
    hframe->SetTitle(";D^{0} p_{T} (GeV);4th (2nd)/ 3rd");
    h2->SetMarkerStyle(20);
    h2->SetMarkerColor(kRed);
    h2->SetMarkerSize(1.4);
    h2->SetLineWidth(0);
    h2->Draw("PSAME");
    h4->SetMarkerStyle(kFullSquare);
    h4->SetMarkerColor(kBlue);
    h4->SetMarkerSize(1.4);
    h4->SetLineWidth(0);
    h4->Draw("PSAME");
    TLatex tex;
    tex.SetTextFont(42);
    tex.SetTextAlign(11);
    tex.DrawLatexNDC(0.14, 0.92, "CMS Preliminary");
    tex.SetTextAlign(31);
    tex.DrawLatexNDC(0.9, 0.92, "pPb 8.16 TeV");
    tex.SetTextAlign(12);
    tex.DrawLatexNDC(0.2, 0.3, ::Form("%d #leq N_{trk}^{offline} < %d", Ntrks[ibin], Ntrks[ibin+1]));
    TLegend leg(0.7, 0.7, 0.89, 0.89);
    leg.SetFillStyle(0);
    leg.SetBorderSize(0);
    leg.AddEntry(h2, "2nd / 3rd", "P");
    leg.AddEntry(h4, "4th / 3rd", "P");
    leg.Draw();
    c.Print(::Form("plots/Ntrk%dto%d.pdf", Ntrks[ibin], Ntrks[ibin+1]));
    c.Print(::Form("plots/Ntrk%dto%d.png", Ntrks[ibin], Ntrks[ibin+1]));

    TGraphErrors* gSyst = new TGraphErrors(h3);
    TGraphErrors* g2 = new TGraphErrors(h2);
    TGraphErrors* g4 = new TGraphErrors(h4);

    const int np = gSyst->GetN();
    for (int ip=0; ip<np; ++ip) {
      const double x = gSyst->GetX()[ip];
      const double y2 = std::abs(g2->GetY()[ip] - 1);
      const double y4 = std::abs(g4->GetY()[ip] - 1);
      double y = y2 > y4 ? y2 : y4;
      gSyst->SetPoint(ip, x, y);
      gSyst->SetPointError(ip, gSyst->GetEX()[ip], 0);
    }
    ofile.cd();
    gSyst->Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete gSyst;
    g2->Write(::Form("g2_Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete g2;
    g4->Write(::Form("g4_Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete g4;
  }
}
