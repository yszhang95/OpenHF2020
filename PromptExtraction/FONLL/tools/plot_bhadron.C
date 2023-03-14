void plot_b_xsec()
{
  TFile fin("pp_8p16TeV_bhadron_spectra.root");
  TGraphAsymmErrors* gB[6];
  TGraphAsymmErrors* gLb[6];
  TCanvas c("c", "", 800, 600);
  auto h = c.DrawFrame(0, 5E2, 30, 1E8);
  h->GetYaxis()->SetTitle("d#sigma/dp_{T} (pb/GeV)");
  h->GetXaxis()->SetTitle("p_{T} (GeV)");
  c.SetLogy();
  Color_t cs[] = {2, 46, 6, 7, 8, 9};
  for (int iy=0; iy<6; ++iy) {
    gB[iy] = fin.Get<TGraphAsymmErrors>(::Form("By%d", iy));
    gLb[iy] = fin.Get<TGraphAsymmErrors>(::Form("Lby%d", iy));
    gB[iy]->SetMarkerStyle(kOpenSquare);
    gB[iy]->SetMarkerColor(cs[iy]);
    gLb[iy]->SetMarkerStyle(kFullCircle);
    gLb[iy]->SetMarkerColor(cs[iy]);
    c.cd();
    gB[iy]->Draw("PZ");
    gLb[iy]->Draw("PZ");
  }

  TLegend legB(0.5, 0.63, 0.68, 0.9);
  legB.SetHeader("B^{+} + B^{0}");
  legB.SetBorderSize(0);
  legB.SetFillStyle(0);
  legB.SetTextSize(0.03);
  TLegend legLb(0.7, 0.63, 0.9, 0.9);
  legLb.SetHeader("#Lambda_{b}^{0}");
  legLb.SetTextSize(0.03);
  legLb.SetBorderSize(0);
  legLb.SetFillStyle(0);
  for (int iy=0; iy<6; ++iy) {
    legB.AddEntry(gB[iy], gB[iy]->GetTitle(), "p");
    legLb.AddEntry(gLb[iy], gLb[iy]->GetTitle(), "p");
  }
  legB.Draw();
  legLb.Draw();
  c.Print("bhadron_spectra.pdf");
}

void plot_btoLamC()
{
  TFile fin("pp_8p16TeV_b_to_Lambda_c.root");
  TH1D* hB = fin.Get<TH1D>("hBtoLc_pt");
  TH1D* hLb = fin.Get<TH1D>("hLbtoLc_pt");
  TH1D* hLc = fin.Get<TH1D>("hLc_pt");
  TCanvas c("c", "", 800, 600);
  auto hframe = c.DrawFrame(0, 5E2, 12, 2E8);
  hframe->SetTitle("pp #rightarrow b+x;p_{T};d#sigma/dp_{T} (pb/GeV)");
  c.SetLogy();
  hLc->SetMarkerStyle(kFullCircle);
  hLb->SetMarkerStyle(kOpenSquare);
  hLb->SetMarkerColor(kRed);
  hLb->SetLineColor(kRed);
  hB->SetMarkerStyle(kOpenDiamond);
  hB->SetMarkerColor(kBlue);
  hB->SetLineColor(kBlue);

  TLegend leg(0.65, 0.75, 0.85, 0.89);
  // legB.SetHeader("B^{+} + B^{0}");
  leg.SetBorderSize(0);
  leg.SetFillStyle(0);
  // leg.SetTextSize(0.03);
  leg.AddEntry(hLc, hLc->GetTitle(), "p");
  leg.AddEntry(hB, hB->GetTitle(), "p");
  leg.AddEntry(hLb, hLb->GetTitle(), "p");
  leg.Draw();
  // hLc->SetTitle("");
  hLc->Draw("SAMEPE");
  hLb->Draw("SAMEPE");
  hB->Draw("SAMEPE");
  c.Print("btoLc_spectra.pdf");
}

void plot_btoLc_pPb()
{
  TFile fin("bdecay_frac.root");
  TH1D* hLc = fin.Get<TH1D>("hbdecay_pPb");
  TCanvas c("c", "", 800, 600);
  auto hframe = c.DrawFrame(0, 1E-5, 12, 1);
  hframe->SetTitle("pPb #rightarrow b+X;p_{T};1/N_{evt} dN/dp_{T} (/GeV)");
  c.SetLogy();
  hLc->SetMarkerStyle(kFullCircle);

  // hLc->SetTitle("");
  hLc->Draw("SAMEPE");
  c.Print("btoLc_spectra_pPb.pdf");
}

void plot_bhadron()
{
  plot_b_xsec();
  plot_btoLamC();
  plot_btoLc_pPb();
}
