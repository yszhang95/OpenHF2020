void plot_MB(std::string particle="D0")
{
  gROOT->SetBatch(kTRUE);
  const int Ntrks[] = {0, 35, 60, 90, 120, 185};
  const int ntrk = sizeof(Ntrks)/sizeof(int) - 1;

  for (int itrk=0; itrk<5; ++itrk) {
    std::string fname = ::Form("%s_Ntrk%dto%d.root", particle.c_str(), Ntrks[itrk], Ntrks[itrk+1]);
    std::string oname = ::Form("%s_Ntrk%dto%d.pdf", particle.c_str(), Ntrks[itrk], Ntrks[itrk+1]);
    TFile fin(fname.c_str());
    auto h1 = fin.Get<TH1>("iter0/eff_iter0");
    auto h2 = fin.Get<TH1>("iter5/eff_iter5");
    TCanvas c("c", "", 800, 600);
    c.cd();
    gStyle->SetOptStat(0);
    h1->SetLineColor(kBlue);
    h2->SetLineColor(kRed);
    h1->SetMarkerColor(kBlue);
    h2->SetMarkerColor(kRed);
    h1->SetMarkerStyle(kOpenSquare);
    h2->SetMarkerStyle(kFullCircle);

    h1->SetTitle(::Form("%s N_{trk}^{offline} %d - %d", particle.c_str(), Ntrks[itrk], Ntrks[itrk+1]));

    h1->Draw("EP");
    h2->Draw("EPSAME");
    TLegend leg(0.15, 0.65, 0.5, 0.85);
    leg.SetFillStyle(0);
    leg.SetBorderSize(0);
    leg.AddEntry(h1, "w/o reweight", "p");
    leg.AddEntry(h2, "w/ reweight", "p");
    leg.Draw();
    c.Print(oname.c_str());
  }
}
