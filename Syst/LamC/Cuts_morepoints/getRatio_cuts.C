
#if defined(__CLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/FitUtils/fitUtils.h"

static double xMin;
static double xMax;

double fit_linear(double *x, double *par)
{
  if (x[0] < xMax && x[0] > xMin) { TF1::RejectPoint(); return 0; }
  return par[0] + par[1] * x[0];
}

  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
void comput(const std::string trig, const double pTMin, const double pTMax)
{
  TFile infile(::Form("yields_%s_cuts.root", trig.c_str()));
  TFile ofile(::Form("yields_ratio_%s_cuts.root", trig.c_str()), "recreate");
      std::string ptbin = ::string_format("pT%.0fto%.0f", pTMin, pTMax);

      std::string configName = ::string_format("configs/pT%.0fto%.0f_%s.conf", pTMin, pTMax, trig.c_str());
      FitParConfigs configs(configName);
      const auto cutConfigs = configs.getCutConfigs();
      VarCuts mycuts(cutConfigs);
      const double mvaCut = mycuts._mvaCut;

      TGraphErrors* g = infile.Get<TGraphErrors>(ptbin.c_str());
      const int n = g->GetN();
      double* x = g->GetX();
      auto find_cut = [&mvaCut](double cut) { return std::abs(cut-mvaCut)/mvaCut < 1E-3; };
      auto pos = std::find_if(x, x+n, find_cut);
      auto ipos = pos-x;

      const double std_yields = g->GetY()[ipos];
      const double std_err = g->GetEY()[ipos];

      std::string name = ptbin + "_ratio";
      TGraphErrors* gout = new TGraphErrors(n);
      TGraphErrors* gout_abs = new TGraphErrors(n);
      for (int ip=0; ip<n; ++ip) {
        const double y = g->GetY()[ip];
        const double yerr = g->GetEY()[ip];
        const double ratio = y/std_yields;
        const double corr_sqrt = ip < ipos ? yerr : std_err;
        const double corr = corr_sqrt * corr_sqrt;
        const double ratio_err = ratio*
                                 std::sqrt(std::abs(
                                                    yerr*yerr/y/y +
                                                    std_err*std_err/std_yields/std_yields
                                                    - 2 * corr/ std_yields/ y
                                                   ));
        gout->SetPoint(ip, x[ip], ratio);
        gout->SetPointError(ip, 0, ratio_err);
        gout_abs->SetPoint(ip, x[ip], std::abs(ratio-1));
        gout_abs->SetPointError(ip, 0, ratio_err);
      }
      ofile.cd();
      gout->Write(name.c_str());
      gout_abs->Write((name+"_absDiff").c_str());

      std::cout << "Computed pT " << pTMin << " -- " << pTMax << " for " << trig << "\n";
}

double fit(const std::string trig, const double pTMin, const double pTMax)
{
  // TFile infile(::Form("yields_ratio_%s_linear.root", trig.c_str()));
  TFile infile(::Form("yields_ratio_%s_cuts.root", trig.c_str()));
    std::string ptbin = ::string_format("pT%.0fto%.0f", pTMin, pTMax);
    std::string configName = ::string_format("configs/pT%.0fto%.0f_%s.conf", pTMin, pTMax, trig.c_str());
    FitParConfigs configs(configName);
    const auto cutConfigs = configs.getCutConfigs();
    VarCuts mycuts(cutConfigs);
    const auto mvaCut = mycuts._mvaCut;
    auto g = infile.Get<TGraphErrors>((ptbin+"_ratio").c_str());
    auto gAbs = infile.Get<TGraphErrors>((ptbin+"_ratio_absDiff").c_str());
    const int n = g->GetN();
    double* x = g->GetX();
    auto find_cut = [&mvaCut](double cut) { return std::abs(cut-mvaCut)/mvaCut < 1E-3; };
    auto pos = std::find_if(x, x+n, find_cut);
    auto ipos = pos-x;
    // TF1 f("f", fit_linear, x[0], x[n-1], 2);
    TF1 f("f", fit_linear, x[0], x[n-1], 1);
    auto pos_l = x[ipos-1];
    auto pos_r = x[ipos+1];
    xMin = (pos_l+*pos)/2.;
    xMax = (pos_r+*pos)/2.;
    TCanvas c("c", "", 800*2, 600);
    c.Divide(2, 1);
    auto pad1 = c.cd(1);
    pad1->SetLeftMargin(0.14);
    gAbs->SetTitle(::Form("%s;MVA cut;| Alternative / default - 1|", trig.c_str()));
    gAbs->Fit(&f, "0");
    gAbs->SetMarkerStyle(20);
    gAbs->Draw("AP");
    // TF1 left("left", "[0]+[1]*x", x[0], xMin);
    TF1 left("left", "[0]", x[0], xMin);
    left.SetParameter(0, f.GetParameter(0));
    // left.SetParameter(1, f.GetParameter(1));
    // TF1 right("right", "[0]+[1]*x", xMax, x[n-1]);
    TF1 right("right", "[0]", xMax, x[n-1]);
    right.SetParameter(0, f.GetParameter(0));
    // right.SetParameter(1, f.GetParameter(1));
    left.Draw("SAME");
    right.Draw("SAME");
    TLatex tex;
    tex.SetTextFont(42);
    tex.DrawLatexNDC(0.2, 0.85, ::Form("%.1f < p_{T} < %.1f GeV", pTMin, pTMax));
    tex.DrawLatexNDC(0.2, 0.79, ::Form("|y_{lab}|<1"));
    tex.DrawLatexNDC(0.2, 0.72, ::Form("sys. = %.3f", f.GetParameter(0)));
    auto pad2 = c.cd(2);
    g->SetTitle(::Form("%s;MVA cut;Alternative / default", trig.c_str()));
    g->SetMarkerStyle(20);
    auto h = g->GetHistogram();
    h->GetYaxis()->SetRangeUser(0.7, 1.3);
    g->Draw("AP");
    TLine l(g->GetX()[0], 1, g->GetX()[g->GetN()-1], 1);
    l.SetLineStyle(2);
    l.Draw();
    pad1->SetLeftMargin(0.14);
    c.Print(::Form("plots/pT_%.0fto%.0f_%s_const.png", pTMin, pTMax, trig.c_str()));
    c.Print(::Form("plots/pT_%.0fto%.0f_%s_const.pdf", pTMin, pTMax, trig.c_str()));

    return f.GetParameter(0);
}

void getRatio_cuts()
{
  TFile ofile("ratio_cuts.root", "recreate");
  TGraphErrors gMB(nPt);
  TGraphErrors gHM(nPt);
  for (int ipt=0; ipt<nPt; ++ipt) {
  // for (int ipt=nPt-1; ipt<nPt; ++ipt) {
    std::cout << ipt << "\n";
    const double x = (pts[ipt] + pts[ipt+1]) / 2.;
    const double xerr = (pts[ipt+1] - pts[ipt]) / 2.;
    if (ipt != 0) {
      comput("HM", pts[ipt], pts[ipt+1]);
      auto y = fit("HM", pts[ipt], pts[ipt+1]);
      // cout << hm << "\n";
      gHM.SetPoint(ipt, x, y);
      gHM.SetPointError(ipt, xerr, 0);
    }
    comput("MB", pts[ipt], pts[ipt+1]);
    auto y = fit("MB", pts[ipt], pts[ipt+1]);
    gMB.SetPoint(ipt, x, y);
    gMB.SetPointError(ipt, xerr, 0);
  }
      ofile.cd();
  // gMB.Write("gMB");
  // gHM.Write("gHM");

  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

  for (int ibin=0; ibin<nbins-1; ++ibin) {
    gMB.Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
  }
  gHM.Write("Ntrk185to250");
}
