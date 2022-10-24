
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

void calRatio()
{
  auto f2 = TFile("yields_order2.root");
  auto f3 = TFile("yields_order3.root");
  auto f4 = TFile("yields_order4.root");
  auto fGausCB = TFile("yields_gausCB.root");

  TFile ofile("syst.root", "recreate");
  for (int ibin=0; ibin<nbins; ++ibin) {
    TH1D* h2 = f2.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    TH1D* h3 = f3.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    TH1D* h4 = f4.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    TH1D* hGausCB = fGausCB.Get<TH1D>(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    h2->Divide(h3);
    h4->Divide(h3);
    hGausCB->Divide(h3);
    // std::cout << "2nd / 3rd\n";
    // h2->Print("all");
    // std::cout << "\n";
    // std::cout << "4th / 3rd\n";
    // h4->Print("all");
    // std::cout << "\n";
    TCanvas c("c", "", 800, 600);
    c.SetLeftMargin(0.14);
    auto hframe = c.DrawFrame(0, 0.5, 12, 1.5);
    hframe->SetTitle(";#Lambda_{c} p_{T} (GeV);alternative / default");

    TGraphErrors* g2 = new TGraphErrors(h2);
    TGraphErrors* g4 = new TGraphErrors(h4);
    TGraphErrors* gGausCB = new TGraphErrors(hGausCB);

    TGraphErrors* gSyst = new TGraphErrors(h3);

    for (int ipt=0; ipt<g2->GetN(); ++ipt) {
      g2->SetPointError(ipt, g2->GetEX()[ipt], h3->GetBinError(ipt+1)/h3->GetBinContent(ipt+1));
      g4->SetPointError(ipt, g4->GetEX()[ipt], h3->GetBinError(ipt+1)/h3->GetBinContent(ipt+1));
      gGausCB->SetPointError(ipt, gGausCB->GetEX()[ipt], h3->GetBinError(ipt+1)/h3->GetBinContent(ipt+1));
    }

    if (ibin == 0) g4->RemovePoint(g4->GetN() - 1);

    g2->SetMarkerStyle(20);
    g2->SetMarkerColor(kRed);
    g2->SetMarkerSize(1.4);
    g2->SetLineColor(kRed);
    g2->Draw("PSAME");
    g4->SetMarkerStyle(kFullSquare);
    g4->SetMarkerColor(kBlue);
    g4->SetMarkerSize(1.4);
    g4->SetLineColor(kBlue);
    g4->Draw("PSAME");
    gGausCB->SetMarkerStyle(kFullDiamond);
    gGausCB->SetMarkerColor(kGreen+3);
    gGausCB->SetMarkerSize(1.4);
    gGausCB->SetLineColor(kGreen+3);
    gGausCB->Draw("PSAME");
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
    leg.AddEntry(g2, "2nd / 3rd", "P");
    leg.AddEntry(g4, "4th / 3rd", "P");
    leg.AddEntry(gGausCB, "Gaus+CB/ Gaus+Gaus", "P");
    leg.Draw();
    TLine line(hframe->GetXaxis()->GetXmin(), 1,
               hframe->GetXaxis()->GetXmax(), 1);
    line.SetLineStyle(2);
    line.Draw();
    c.Print(::Form("plots/Ntrk%dto%d.pdf", Ntrks[ibin], Ntrks[ibin+1]));
    c.Print(::Form("plots/Ntrk%dto%d.png", Ntrks[ibin], Ntrks[ibin+1]));

    const int np = gSyst->GetN();
    for (int ip=0; ip<np; ++ip) {
      const double x = gSyst->GetX()[ip];
      const double y2 = std::abs(g2->GetY()[ip] - 1);
      const double y4 = ibin == 0 && ip == np-1 ? 0 : std::abs(g4->GetY()[ip] - 1);
      const double yGausCB = std::abs(gGausCB->GetY()[ip] - 1);
      double y = y2 > y4 ? y2 : y4;
      //y = y > yGausCB ? y : yGausCB;
      y = std::sqrt( y*y + yGausCB*yGausCB );
      gSyst->SetPoint(ip, x, y);
      gSyst->SetPointError(ip, gSyst->GetEX()[ip], 0);
    }
    std::cout << Ntrks[ibin] << " -- " << Ntrks[ibin+1] << std::endl;
    if (ibin==nbins-1) {
      std::cout << *std::min_element(gSyst->GetY(), gSyst->GetY()+gSyst->GetN()) << std::endl;
      std::cout << *std::max_element(gSyst->GetY()+1, gSyst->GetY()+gSyst->GetN()) << std::endl;
    } else {
      std::cout << *std::min_element(gSyst->GetY(), gSyst->GetY()+gSyst->GetN()) << std::endl;
      std::cout << *std::max_element(gSyst->GetY(), gSyst->GetY()+gSyst->GetN()) << std::endl;
    }
    ofile.cd();
    gSyst->Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete gSyst;
    g2->Write(::Form("g2_Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete g2;
    g4->Write(::Form("g4_Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete g4;
    gGausCB->Write(::Form("gGausCB_Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    delete gGausCB;
  }
}
