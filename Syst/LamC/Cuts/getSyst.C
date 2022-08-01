
#if defined(__CLING__)
R__ADD_INCLUDE_PATH($OPENHF2020TOP)
R__LOAD_LIBRARY($OPENHF2020TOP/Utilities/lib/libMyFitUtils.so)
#endif

#include "Utilities/FitUtils/ReadConfig.h"
#include "Utilities/FitUtils/fitUtils.h"

static double xMin;
static double xMax;

double fit_const(double *x, double *par)
{
  if (x[0] < xMax && x[0] > xMin) { TF1::RejectPoint(); return 0; }
  return par[0];
}

  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
void comput(const std::string trig, const double pTMin, const double pTMax)
{
  TFile infile(::Form("yields_%s.root", trig.c_str()));
  TFile ofile(::Form("yields_ratio_%s.root", trig.c_str()), "recreate");
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
}

double fit(const std::string trig, const double pTMin, const double pTMax)
{
  TFile infile(::Form("yields_ratio_%s.root", trig.c_str()));
    std::string ptbin = ::string_format("pT%.0fto%.0f", pTMin, pTMax);
    std::string configName = ::string_format("configs/pT%.0fto%.0f_%s.conf", pTMin, pTMax, trig.c_str());
    FitParConfigs configs(configName);
    const auto cutConfigs = configs.getCutConfigs();
    VarCuts mycuts(cutConfigs);
    const auto mvaCut = mycuts._mvaCut;
    auto g = infile.Get<TGraphErrors>((ptbin+"_ratio_absDiff").c_str());
    const int n = g->GetN();
    double* x = g->GetX();
    auto find_cut = [&mvaCut](double cut) { return std::abs(cut-mvaCut)/mvaCut < 1E-3; };
    auto pos = std::find_if(x, x+n, find_cut);
    auto ipos = pos-x;
    TF1 f("f", fit_const, x[0], x[n-1], 1);
    auto pos_l = x[ipos-1];
    auto pos_r = x[ipos+1];
    xMin = (pos_l+*pos)/2.;
    xMax = (pos_r+*pos)/2.;
    TCanvas c("c", "", 800, 600);
    c.SetLeftMargin(0.14);
    g->SetTitle(::Form("%s;MVA cut;| Alternative / default - 1 |", trig.c_str()));
    g->Fit(&f, "0");
    g->SetMarkerStyle(20);
    g->Draw("AP");
    TF1 left("left", "[0]", x[0], xMin);
    left.SetParameter(0, f.GetParameter(0));
    TF1 right("right", "[0]", xMax, x[n-1]);
    right.SetParameter(0, f.GetParameter(0));
    left.Draw("SAME");
    right.Draw("SAME");
    TLatex tex;
    tex.SetTextFont(42);
    tex.DrawLatexNDC(0.2, 0.85, ::Form("%.1f < p_{T} < %.1f GeV", pTMin, pTMax));
    tex.DrawLatexNDC(0.2, 0.79, ::Form("|y_{lab}|<1"));
    tex.DrawLatexNDC(0.2, 0.72, ::Form("sys. = %.3f", f.GetParameter(0)));
    c.Print(::Form("plots/pT_%.0fto%.0f_%s.png", pTMin, pTMax, trig.c_str()));
    c.Print(::Form("plots/pT_%.0fto%.0f_%s.pdf", pTMin, pTMax, trig.c_str()));

    return f.GetParameter(0);
}

void getSyst()
{
  TFile ofile("syst.root", "recreate");
  TGraphErrors gMB(nPt);
  TGraphErrors gHM(nPt);
  for (int ipt=0; ipt<nPt; ++ipt) {
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
  gMB.Write("gMB");
  gHM.Write("gHM");

  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

  for (int ibin=0; ibin<nbins-1; ++ibin) {
    gMB.Write(::Form("Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
  }
  gHM.Write("Ntrk185to250");
}
