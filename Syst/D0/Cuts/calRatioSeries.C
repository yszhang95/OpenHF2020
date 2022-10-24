
#if defined(__CLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/FitUtils/fitUtils.h"

static double xMin = 0.555;
static double xMax = 0.565;

double fit_const(double *x, double *par)
{
  if (x[0] < xMax && x[0] > xMin) { TF1::RejectPoint(); return 0; }
  return par[0];
}

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

void calRatioSeries()
{
  const float optimalCut = 0.56;
  auto fyields = TFile("yieldsSeries.root");

  TGraphErrors* gSyst[nPt];

  for (int ibin=0; ibin<nbins; ++ibin) {
    // TGraphAsymmErrors gtight;
    // TGraphAsymmErrors gloose;
    gSyst[ibin] = new TGraphErrors(nPt);
    for (int ipt=0; ipt<nPt; ++ipt) {
      TH1D* h = fyields.Get<TH1D>(::Form("Ntrk%dto%d_pT%d", Ntrks[ibin], Ntrks[ibin+1], ipt));
      TGraphErrors g(h);
      TGraphErrors gAbs(h);
      const auto optimalBin = h->FindBin(optimalCut);
      const auto optimalYields = h->GetBinContent(optimalBin);
      const auto optimalError = h->GetBinError(optimalBin);
      for (int imva=0; imva<h->GetNbinsX(); ++imva) {
        const double pass = imva < optimalBin ? optimalYields : h->GetBinContent(imva+1);
        const double pass_err = imva < optimalBin ? optimalError : h->GetBinError(imva+1);
        const double total = imva >= optimalBin ? optimalYields : h->GetBinContent(imva+1);
        const double total_err = imva >= optimalBin ? optimalError : h->GetBinError(imva+1);

        float ratio(0), err(0);

        calRatio(pass, total, pass_err, total_err, ratio, err);
        g.SetPoint(imva, g.GetX()[imva], ratio);
        g.SetPointError(imva, 0, err);
        gAbs.SetPoint(imva, g.GetX()[imva], std::abs(ratio-1));
        gAbs.SetPointError(imva, 0, err);
      }

      TCanvas c("c", "", 600, 480);
      c.SetLeftMargin(0.14);
      auto hframe = gAbs.GetHistogram();
      // hframe->SetTitle(::Form("N_{trk}^{offline} %d -- %d p_{T} %.1f -- %.1f", Ntrks[ibin], Ntrks[ibin+1], pts[ipt], pts[ipt+1]));
      hframe->SetTitle("");
      hframe->GetXaxis()->SetTitle("BDT cut");
      hframe->GetYaxis()->SetTitle("| alternative/ default - 1 |");
      hframe->GetYaxis()->SetRangeUser(-0.01, 0.05);
      hframe->Draw();
      TF1 f("f", fit_const, gAbs.GetX()[0], gAbs.GetX()[gAbs.GetN()-1], 1);
      f.SetParameter(0, gAbs.GetY()[1]);
      gAbs.Fit(&f, "0");
      gAbs.SetMarkerStyle(20);
      gAbs.Draw("P");
      TF1 left("left", "[0]", gAbs.GetX()[0], 0.555);
      left.SetParameter(0, f.GetParameter(0));
      TF1 right("right", "[0]", 0.565, gAbs.GetX()[gAbs.GetN()-1]);
      right.SetParameter(0, f.GetParameter(0));
      left.Draw("SAME");
      right.Draw("SAME");
      TLatex tex;
      tex.SetTextFont(42);
      tex.SetTextAlign(11);
      tex.DrawLatexNDC(0.14, 0.92, "CMS Preliminary");
      tex.SetTextAlign(31);
      tex.DrawLatexNDC(0.9, 0.92, "pPb 8.16 TeV");
      tex.SetTextAlign(12);
      tex.DrawLatexNDC(0.2, 0.3, ::Form("%d #leq N_{trk}^{offline} < %d", Ntrks[ibin], Ntrks[ibin+1]));
      tex.DrawLatexNDC(0.2, 0.35, ::Form("%.1f #leq p_{T}^{D^{0}} < %.1f GeV", pts[ipt], pts[ipt+1]));
      c.Print(::Form("plots/Ntrk%dto%d_pT%d.pdf", Ntrks[ibin], Ntrks[ibin+1], ipt));
      
      double syst = f.GetParameter(0);
      const double x = (pts[ipt]+pts[ipt+1])/2.;
      const double x_err = (pts[ipt+1]-pts[ipt])/2.;
      gSyst[ibin]->SetPoint(ipt, x, syst);
      gSyst[ibin]->SetPointError(ipt, x_err, syst);
    }
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
