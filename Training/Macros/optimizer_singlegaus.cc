#include <memory>
#include "Math/ChebyshevPol.h"

using std::shared_ptr;
using std::make_shared;

double bkgRejctPeak(double* x, double* pars)
{
  if (x[0] > 2.2865-0.016 && x[0] < 2.2865+0.016) {
    TF1::RejectPoint();
    return 0;
  }
  ROOT::Math::ChebyshevPol chebyshev(3);
  return chebyshev(x, pars);
}

std::vector<double>
simpleFit(TH1* hMC, TH1* h, const double* bkgvars,
               const double lw, const double up, TCanvas* c)
{
  c->cd(1);
  TF1 fMC("fMC", "gausn(0)", 2.24, 2.32);
  fMC.SetParameter(0, 100);
  fMC.SetParameter(1, 2.2865);
  fMC.SetParameter(2, 0.005);
  hMC->Fit(&fMC, "R", "", 2.24, 2.32);
  hMC->Fit(&fMC, "R", "", 2.24, 2.32);
  c->cd(2);
  gROOT->ForceStyle();
  gStyle->SetOptFit(1111);
  ROOT::Math::ChebyshevPol chebyshev(3);
  auto fbkgReject = make_shared<TF1>
    ("fbkgReject", bkgRejctPeak, lw, up, 4);
  auto fbkg = make_shared<TF1>("fbkg", &chebyshev, lw, up, 4);
  // auto f = make_shared<TF1>("f", "gausn(0)+cheb3(3)", lw, up);

  auto f = make_shared<TF1>("f", "gausn(0) + cheb3(3)", lw, up);

  fbkgReject->SetParameters(bkgvars);
  h->Fit(fbkgReject.get(), "R", "", lw, up);
  h->Fit(fbkgReject.get(), "MR", "", lw, up);

  for (int i=0; i<fbkgReject->GetNpar(); ++i) {
    fbkg->SetParameter(i, fbkgReject->GetParameter(i));
    f->FixParameter(i+3, fbkgReject->GetParameter(i));
  }
  f->SetParameter(0, 50);
  f->SetParameter(1, 2.2865);
  f->SetParameter(2, 0.005);
  f->SetParLimits(1, 2.2865-0.005, 2.2865+0.005);
  f->SetParLimits(2, 0, 0.015);

   for (int i=0; i != 3; ++i) {
     f->SetParameter(i, fMC.GetParameter(i));
   }
   h->Fit(f.get(), "R", "", lw, up);
   h->Fit(f.get(), "R", "", lw, up);

   h->Fit(f.get(), "RM", "", lw, up);
   h->Fit(f.get(), "RME", "", lw, up);

   c->Draw();

   c->cd();
   // h->Draw("E");
   f->Draw("SAME");
   fbkg->SetLineColor(kBlue);
   fbkg->SetLineStyle(2);
   fbkg->Draw("SAME");
   c->Modified();
   c->Update();
   c->Draw();
   c->Print(Form("plots/single%s.pdf", c->GetName()));

   vector<double> output;
   std::copy(f->GetParameters(),
             f->GetParameters()+f->GetNpar(),
             std::back_inserter(output));
   std::copy(f->GetParErrors(),
             f->GetParErrors()+f->GetNpar(),
             std::back_inserter(output));
   return output;
}

double getEffError(double total, double pass)
{
  double upper = TEfficiency::ClopperPearson(total, pass, 0.683, 1);
  double lower = TEfficiency::ClopperPearson(total, pass, 0.683, 0);
  return (upper-lower)/2.;
}

TH1* getEff(bool isSignal, TH3* h3, const char* name="",
            const double pTMin=3.0, const double pTMax=4.0,
            int nbins = -1)
{
  auto pTMinBin = h3->GetYaxis()->FindBin(pTMin);
  auto pTMaxBin = h3->GetYaxis()->FindBin(pTMax) - 1;
  if (isSignal) {
    auto hEff = h3->ProjectionZ(Form("SignalEff_%s", name));
    if (nbins > 0) hEff->Rebin(nbins);
    for (int i=0; i<hEff->GetNbinsX()+2; i++) {
      hEff->SetBinContent(i, 0);
      hEff->SetBinError(i, 0);
      int ibin = i;
      if (nbins > 0) ibin = h3->GetZaxis()->FindBin(hEff->GetBinCenter(i));
      auto h =  h3->ProjectionX("s", pTMinBin, pTMaxBin, ibin, 100000);
      hEff->SetBinContent(i, h->Integral());
      delete h;
    }
    const double total = hEff->GetBinContent(0);
    hEff->Scale(1./total);
    for (int i=0; i<hEff->GetNbinsX()+2; i++) {
      double error = getEffError(total, hEff->GetBinContent(i));
      hEff->SetBinError(i, error);
    }
    return hEff;
  } else {
    auto hEff = h3->ProjectionZ(Form("BackgroundEff_%s", name));
    if (nbins > 0) hEff->Rebin(nbins);
    const auto leftMinBin = h3->GetXaxis()->FindBin(2.2865-0.011);
    const auto leftMaxBin = h3->GetXaxis()->FindBin(2.2865-0.006);
    const auto rightMinBin = h3->GetXaxis()->FindBin(2.2865+0.006);
    const auto rightMaxBin = h3->GetXaxis()->FindBin(2.2865+0.011);

    for (int i=0; i<hEff->GetNbinsX()+2; i++) {
      hEff->SetBinContent(i, 0);
      hEff->SetBinError(i, 0);
      int ibin = i;
      if (nbins > 0) ibin = h3->GetZaxis()->FindBin(hEff->GetBinCenter(i));

      auto h =  h3->ProjectionX("s", pTMinBin, pTMaxBin, ibin, 100000);

      double bkg = h->Integral(leftMinBin, leftMaxBin);
      bkg += h->Integral(rightMinBin, rightMaxBin);
      hEff->SetBinContent(i, bkg);
      delete h;
    }
    const double total = hEff->GetBinContent(0);
    hEff->Scale(1./total);
    for (int i=0; i<hEff->GetNbinsX()+2; i++) {
      double error = getEffError(total, hEff->GetBinContent(i));
      hEff->SetBinError(i, error);
    }
    return hEff;
  }
  return nullptr;
}

TH1* proj1D(TH3* h3, const char* name,
            const double pTMin, const double pTMax,
            const double mvaMin, const double mvaMax)
{
  const auto pTBinMin = h3->GetYaxis()->FindBin(pTMin);
  const auto pTBinMax = h3->GetYaxis()->FindBin(pTMax) - 1;
  const auto mvaBinMin = h3->GetZaxis()->FindBin(mvaMin);
  const auto mvaBinMax = h3->GetZaxis()->FindBin(mvaMax) - 1;

  auto h = h3->ProjectionX(name,
                           pTBinMin, pTBinMax, mvaBinMin, mvaBinMax);
  return h;
}


void optimizer_singlegaus()
{
  TFile f("../test/TMVA_pPb_LambdaCKsP3to4App_dataHM1to6_pPbBoost_noDR.root");
  TH3D* hBDT;
  TH3D* hMLP;
  f.GetObject("lambdacAna/hMassPtMVA_BDTG_3D_noDR_y0;1", hBDT);
  f.GetObject("lambdacAna/hMassPtMVA_MLP3to4Np2N_noDR_y0;1", hMLP);
  if (!hBDT) { cout << "bad pointer hBDT" << endl; }
  if (!hMLP) { cout << "bad pointer hMLP" << endl; }

  TFile fMC("TMVA_MC_pPb_LambdaCKsP3to4App_noDR.root");
  TH3D* hBDTMC;
  TH3D* hMLPMC;
  fMC.GetObject("lambdacAna_mc/hMassPtMVA_BDTG_3D_noDR_y0;1", hBDTMC);
  fMC.GetObject("lambdacAna_mc/hMassPtMVA_MLP3to4Np2N_noDR_y0;1", hMLPMC);
  if (!hBDTMC) { cout << "bad pointer hBDTMC" << endl; }
  if (!hMLPMC) { cout << "bad pointer hMLPMC" << endl; }

  const double pTMin = 3.0;
  const double pTMax = 4.0;
  auto hEff = getEff(true, hMLPMC, "MLP", pTMin, pTMax);
  /*const double mvaCuts[] = {0.003, 0.0035, 0.004, 0.0045, 0.005, 0.0055, 0.006, 0.007};*/
  const int nMVA = 30;
  double mvaCuts[nMVA];
  for (size_t i=0; i != nMVA; ++i) {
    mvaCuts[i] = 0.003 + 0.0002 * i;
  }
  std::vector<TH1*> hMLP1Ds;
  std::vector<TH1*> hMCMLP1Ds;
  std::vector<double> dataSig;
  std::vector<double> yields_data;
  std::vector<double> yields_MC;
  std::vector<double> yieldsErr_MC;
  std::vector<double> histInt;
  std::vector<double> fitInt;
  std::vector<double> widths;
  for (const auto mvaCut : mvaCuts) {
    TString mvaCutStr = Form("mva%f", mvaCut);
    auto pos = mvaCutStr.Index(".");
    mvaCutStr.Replace(pos, 1, "p");
    auto h = proj1D(hMLP, mvaCutStr,
                    pTMin, pTMax, mvaCut, 1);
    // h->Rebin();
    auto hMC = proj1D(hMLPMC, mvaCutStr+"MC",
                      pTMin, pTMax, mvaCut, 1);
    h->SetTitle(Form("MLP>%f", mvaCut));
    hMC->SetTitle(Form("MC MLP>%f", mvaCut));
    h->GetXaxis()->SetTitle("M_{K_{s}^{0}p} (GeV)");
    hMC->GetXaxis()->SetTitle("M_{K_{s}^{0}p} (GeV)");
    h->GetYaxis()->SetTitle(Form("Events / %.3f GeV", h->GetBinWidth(1)));
    hMC->GetYaxis()->SetTitle(Form("Events / %.3f GeV", hMC->GetBinWidth(1)));
    hMLP1Ds.push_back(h);
    hMCMLP1Ds.push_back(hMC);
    TCanvas c("c"+mvaCutStr, "", 600*2, 450);
    c.Divide(2, 1);
    const double pars[] = {20000, -5000, 100, 200};
    auto parameters = simpleFit(hMC, h, pars,
              2.15, 2.45, &c);
    TF1 f("f", "gausn(0)+cheb3(3)", 2.15, 2.45);
    f.SetParameters(parameters.data());
    const double width = parameters.at(2);
    widths.push_back(width);
    const double sPlusBFit = f.Integral(2.2865-2.6*width, 2.2865+2.6*width) / h->GetBinWidth(1);
    fitInt.push_back(sPlusBFit);

    for (int i=3; i<7; ++i) {
      f.SetParameter(i, 0);
    }
    const double sDataFit = f.Integral(2.2865-2.6*width, 2.2865+2.6*width) / h->GetBinWidth(1);
    // const double sDataFit = f.GetParameter(0) / h->GetBinWidth(1);
    dataSig.push_back(sDataFit/std::sqrt(sPlusBFit));

    yields_data.push_back( parameters.at(0)/h->GetBinWidth(1) );

    const auto massLw = h->FindBin(2.2865-2.6*width);
    const auto massUp = h->FindBin(2.2865+2.6*width);
    const auto sPlusBHist = h->Integral(massLw, massUp);
    histInt.push_back(sPlusBHist);

    const auto mvaBin = hEff->FindBin(mvaCut);
    const auto eff = hEff->GetBinContent(mvaBin);
    yields_MC.push_back(yields_data.back()/eff);
    yieldsErr_MC.push_back( parameters.at( f.GetNpar() ) / h->GetBinWidth(1) /eff);
  }

  for (const auto e : dataSig) cout << e << endl;
  for (auto h : hMLP1Ds) h->Delete();
  for (auto h : hMCMLP1Ds) h->Delete();
  // for (const auto e : yields_data) cout << e/yields_data.at(1) << endl;

  for (size_t i=0; i != yields_MC.size(); ++i) {
    cout << mvaCuts[i] << ", "  << yields_MC.at(i)
         << " +/- " << yieldsErr_MC.at(i)
         << " " << yields_MC.at(i)/yieldsErr_MC.at(i) << endl;
  }

  TCanvas cYields("cYields", "", 600, 480);
  cYields.SetLeftMargin(0.15);
  TGraphErrors gYields(nMVA);
  for (int i=0; i != nMVA; ++i) {
    gYields.SetPoint(i, mvaCuts[i], yields_MC.at(i));
    gYields.SetPointError(i, 0, yieldsErr_MC.at(i));
  }
  gYields.SetMarkerStyle(20);
  TF1 fConstant("yields", "[0]", 0, 0.02);
  fConstant.SetParameter(0, yields_MC.front());
  gYields.Fit(&fConstant, "R", "", mvaCuts[0]-0.0005, mvaCuts[nMVA-1]+0.0005);
  auto hYieldsFrame = gYields.GetHistogram();
  hYieldsFrame->SetTitle("Expected yields w/o MVA cuts;MVA;s'");
  gYields.Draw("AP");
  cYields.Print("plots/yieldsMLP.pdf");

  const double total = fConstant.GetParameter(0);
  TCanvas c("sig", "", 600, 450);
  TGraphErrors gSig(nMVA);

  for (int i=0; i != nMVA; ++i) {
    const auto mvaCut = mvaCuts[i];
    const double pTMin = 3.0;
    const double pTMax = 4.0;
    TString mvaCutStr = Form("mva%f", mvaCut);
    auto pos = mvaCutStr.Index(".");
    mvaCutStr.Replace(pos, 1, "p");
    auto h = proj1D(hMLP, mvaCutStr,
                    pTMin, pTMax, mvaCut, 1);
    const double halfWindow = 2.6 * widths.at(i);
    // const double halfWindow = 2.6 * 0.01;
    const auto effbin = hEff->FindBin(mvaCut);
    cout << mvaCut << " effbin " << effbin << endl;
    const double yields = total * hEff->GetBinContent(effbin);
    const auto massLw = h->FindBin(2.2865-halfWindow);
    const auto massUp = h->FindBin(2.2865+halfWindow);
    // cout << widths.at(i) << endl;
    double sPlusB = h->Integral(massLw, massUp);
    sPlusB = sPlusB - yields;
    // cout << sPlusB << endl;
    const double sig = yields/sqrt(sPlusB);
    const double err = total/sqrt(sPlusB) * hEff->GetBinError(effbin);
    gSig.SetPoint(i, mvaCut, sig);
    cout << sig << endl;
    gSig.SetPointError(i, 0, err);
    h->Delete();
  }
  gSig.SetMarkerStyle(kFullCircle);
  auto hframe = gSig.GetHistogram();
  hframe->GetYaxis()->SetRangeUser(4., 8.);
  TF1 fitToSig("fitToSig", "pol3", 0.004, 0.0075);
  fitToSig.SetParameter(0, 5);
  fitToSig.SetParameter(1, 0.01);
  fitToSig.SetParameter(2, -0.01);
  gSig.Fit(&fitToSig, "R", "", 0.004, 0.008);
  cout << fitToSig.GetMaximumX() << endl;
  gSig.Draw("APE");
  TGraph sig(nMVA, mvaCuts, dataSig.data());
  sig.SetMarkerStyle(kOpenSquare);
  sig.Draw("PE");
  c.Print("plots/singlegaus_sigMLP.pdf");
}
