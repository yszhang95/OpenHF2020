const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

const double pts[] = {2., 3., 4., 5., 6., 8., 10.};
const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;

void to_ntrk()
{
  TH1D* hNtrk[nbins];
  TH1D* hNtrkAbsDiff[nbins];

  auto f = TFile("ratio.root");
  TFile ofile("ratio_ntrk.root", "recreate");
  for (int itrk=0; itrk<nbins; ++itrk) {
    std::string histName =  ::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]);
    hNtrk[itrk] = (TH1D*)f.Get(histName.c_str());
    hNtrk[itrk]->GetYaxis()->SetTitle("Gplus / olv_dz1p0");
  }

  for (int ibin=0; ibin<nbins; ++ibin) {
    hNtrkAbsDiff[ibin] = new TH1D(::Form("hNtrkAbsDiff%dto%d", Ntrks[ibin], Ntrks[ibin+1]), ";p_{T}; | (olv_dz1p0 / Gplus) - 1 |", nPt, pts);
    // fix later
    hNtrk[ibin]->SetBinContent(1, 0);
    hNtrk[ibin]->SetBinError(1, 0);
    for (int ipt=1; ipt<nPt; ++ipt) {
      double content = hNtrk[ibin]->GetBinContent(ipt+1);
      hNtrkAbsDiff[ibin]->SetBinContent(ipt+1, std::abs(content-1));
      double error = hNtrk[ibin]->GetBinError(ipt+1);
      hNtrkAbsDiff[ibin]->SetBinError(ipt+1, error);
    }
    ofile.cd();
    hNtrk[ibin]->Write();
    hNtrkAbsDiff[ibin]->Write();
    delete hNtrkAbsDiff[ibin];
  }

}
void avg()
{
  to_ntrk();

  TFile infile("ratio_ntrk.root");
  TFile ofile("syst.root", "recreate");
  TH1D h ("h", "", nPt, pts);
  for (int ibin=0; ibin<nbins; ++ibin) {
    TCanvas c("c", "", 1200, 500);
    c.Divide(2, 1);

    std::string histName = ::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]);
    std::string histAbsDiffName = ::Form("hNtrkAbsDiff%dto%d", Ntrks[ibin], Ntrks[ibin+1]);
    auto hNtrk = (TH1D*)infile.Get(histName.c_str());
    auto hNtrkAbsDiff = (TH1D*) infile.Get(histAbsDiffName.c_str());
    auto pad1 = c.cd(1);
    pad1->SetLeftMargin(0.14);
    hNtrk->SetStats(0);
    hNtrk->GetYaxis()->SetRangeUser(0, 2);
    hNtrk->Draw("E");
    auto pad2 = c.cd(2);
    pad2->SetLeftMargin(0.14);
    hNtrkAbsDiff->SetStats(0);
    hNtrkAbsDiff->Draw("E");
    TF1 f("f", "[0]", 2, 8);
    hNtrkAbsDiff->Fit(&f, "R", "", 3, 10);
    pad1->cd();
    TLatex tex;
    tex.SetTextFont(42);
    tex.DrawLatexNDC(0.3, 0.3, ::Form("average: %.3f", f.GetParameter(0)));
    tex.DrawLatexNDC(0.3, 0.2, ::Form("%d #leq N_{trk}^{offline} < %d", Ntrks[ibin], Ntrks[ibin+1]));
    c.Print(::Form("output/Ntrk%dto%d.png", Ntrks[ibin], Ntrks[ibin+1]));
    c.Print(::Form("output/Ntrk%dto%d.pdf", Ntrks[ibin], Ntrks[ibin+1]));

    TGraphErrors g(&h);
    for (int ipt=0; ipt<nPt; ++ipt) {
      if (ibin == nbins-1 && ipt==0) continue;
      g.SetPoint(ipt, g.GetX()[ipt], f.GetParameter(0));
    }
    ofile.cd();
    g.Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
  }
}
