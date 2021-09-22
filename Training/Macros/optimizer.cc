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
  // TF1 fMC("fMC", "[0]*([1]*gausn(2) + (1-[1])*gausn(4))");
  // TF1 fMC("fMC", "[0]*([1] * TMath::Gaus(x, [2], [3], true) + (1-[1]) * TMath::Gaus(x, [2], [4], true))");
  TF1 fMC("fMC", "[0]*([1] * TMath::Gaus(x, [2], [3]*(1+[5]), true) + (1-[1]) * TMath::Gaus(x, [2], [4]*(1+[5]), true))");
  fMC.SetParameter(0, 100);
  fMC.SetParameter(1, 0.5);
  fMC.SetParLimits(1, 0, 1);
  fMC.SetParameter(2, 2.2865);
  fMC.SetParameter(3, 0.005);
  fMC.SetParameter(4, 0.01);
  fMC.FixParameter(5, 0);
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

  auto f = make_shared<TF1>("f", "fMC + cheb3(6)", lw, up);

  fbkgReject->SetParameters(bkgvars);
  h->Fit(fbkgReject.get(), "R", "", lw, up);
  h->Fit(fbkgReject.get(), "MR", "", lw, up);

  for (int i=0; i<fbkgReject->GetNpar(); ++i) {
    fbkg->SetParameter(i, fbkgReject->GetParameter(i));
    // f->FixParameter(i+3, fbkgReject->GetParameter(i));
    f->FixParameter(i+6, fbkgReject->GetParameter(i));
  }
   // f->SetParameter(0, 50);
   // f->SetParameter(1, 2.2865);
   // f->SetParameter(2, 0.005);
   // f->SetParLimits(1, 2.2865-0.005, 2.2865+0.005);
   // f->SetParLimits(2, 0, 0.015);

   for (int i=0; i != 5; ++i) {
     f->SetParameter(i, fMC.GetParameter(i));
   }
   // f->SetParLimits(1, 0, 1);
   f->FixParameter(1, fMC.GetParameter(1));
   f->FixParameter(3, fMC.GetParameter(3));
   f->FixParameter(4, fMC.GetParameter(4));
   f->SetParameter(5, 0);
   f->SetParLimits(5, -1, 1);
   h->Fit(f.get(), "R", "", lw, up);
   h->Fit(f.get(), "R", "", lw, up);
   for (int i=0; i != 5; ++i) {
     // f->ReleaseParameter(i);
   }

   h->Fit(f.get(), "RM", "", lw, up);
   h->Fit(f.get(), "RME", "", lw, up);

   // cout << "Done " << c->GetName()<< end
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
   c->Print(Form("plots/%s.pdf", c->GetName()));

   vector<double> output;
   std::copy(f->GetParameters(),
             f->GetParameters()+f->GetNpar(),
             std::back_inserter(output));
   std::copy(f->GetParErrors(),
             f->GetParErrors()+f->GetNpar(),
             std::back_inserter(output));
   return output;

  // return vector<double>();
}
/*
map<string, std::vector<pair<double, double>>> fit()
{
  gStyle->SetOptFit(11111);
  gStyle->SetOptStat(0);
  gROOT->ForceStyle();
  TFile* f = TFile::Open("hists.root");
  // TCanvas* cBDT = new TCanvas("cBDT", "BDT", 600, 500);
  auto cBDT = std::unique_ptr<TCanvas>
    (new TCanvas("cBDT", "", 600, 500));
  auto hBDT = f->Get<TH1>("mass_BDT");
  hBDT->SetTitle("BDT");
  hBDT->Sumw2();
  // TCanvas* cMLP = new TCanvas("cMLP", "MLP", 600, 500);
  auto cMLP = std::unique_ptr<TCanvas>
    (new TCanvas("cMLP", "", 600, 500));
  auto hMLP = f->Get<TH1>("mass_MLP");
  hMLP->SetTitle("MLP");
  hMLP->Sumw2();
  const double mlp [] = {6000, 20000, 5000, 5000};
  const double bdt [] = {2000, 40000, 500, 5000};
  // simpleFit(hBDT, bdt, 2.21, 2.42, cBDT);
  // simpleFit(hMLP, mlp, 2.2, 2.42, cMLP);
  map<string, vector<pair<double, double>>> output;
  output["BDT"] = simpleFit(hBDT, bdt, 2.21, 2.4, cBDT.get());
  output["MLP"] = simpleFit(hMLP, mlp, 2.2, 2.42, cMLP.get());
  return output;
}

TH1* getInt(TH3* h3, const char* name="", int nbins = -1)
{
  double pTMin = 3.0;
  double pTMax = 4.0;
  auto pTMinBin = h3->GetYaxis()->FindBin(pTMin);
  auto pTMaxBin = h3->GetYaxis()->FindBin(pTMax) - 1;
  auto massBinLw = h3->GetXaxis()->FindBin(2.2865-0.016);
  auto massBinUp = h3->GetXaxis()->FindBin(2.2865+0.016);

  auto hYields = h3->ProjectionZ(Form("peakYields_%s", name), 1, 100000, pTMinBin, pTMaxBin);
  if (nbins > 0) hYields->Rebin(nbins);
  for (int i=0; i<hYields->GetNbinsX()+2; i++) {
    hYields->SetBinContent(i, 0);
    hYields->SetBinError(i, 0);
    int ibin = i;
    if (nbins > 0) ibin = h3->GetZaxis()->FindBin(hYields->GetBinCenter(i));
    auto h =  h3->ProjectionX("yields", pTMinBin, pTMaxBin, ibin, 100000);
    double yields, error;
    yields = h->IntegralAndError(massBinLw, massBinUp, error);
    hYields->SetBinContent(i, yields);
    hYields->SetBinError(i, error);
    delete h;
  }
  return hYields;
}


TH1* calSignificance(double s, TH1* sEffOriginal, TH1* b, const char* name,
                     vector<tuple<TF1, double, double>>& fs)

{
  auto sEff = dynamic_cast<TH1*>(sEffOriginal->Clone(Form("%s_sEffcopy", name)));

  TH1* hSig = dynamic_cast<TH1*>( sEff->Clone(Form("sig_%s", name)) );
  for (int i=0; i<hSig->GetNbinsX(); ++i) {
    double sig = s * sEff->GetBinContent(i)/
      sqrt( b->GetBinContent(i) - s * sEff->GetBinContent(i) );
    double sigError = sig *
      sqrt(
           pow(sEff->GetBinError(i)/sEff->GetBinContent(i), 2) +
           pow(0.5 * b->GetBinError(i)/b->GetBinContent(i), 2)
           );
    // cout << "s: " << s * sEff->GetBinContent(i) << endl;
    // cout << "b: " << b->GetBinContent(i) << endl;
    if (isnan(sig)) sig=0;
    hSig->SetBinContent(i, sig);
    // hSig->SetBinError(i, sigError);
  }
  TCanvas c("sig", "", 600, 500);
  c.cd();
  for (auto& f : fs) {
    const double lw = std::get<1>(f);
    const double up = std::get<2>(f);
    auto& func = std::get<0>(f);
    hSig->Fit(&func, "R+", "", lw, up);
    hSig->Fit(&func, "R+", "", lw, up);
    cout << func.GetMaximumX() << ": " << func.GetMaximum() << endl;
  }
  hSig->SetStats(0);
  hSig->SetMinimum(0);
  hSig->SetTitle(name);
  hSig->GetYaxis()->SetTitle("S/#sqrt{S+B}");
  hSig->Draw("E");
  c.Modified();
  c.Update();
  hSig->GetYaxis()->SetRangeUser(0, c.GetUymax());
  double rightmax = 1.1* sEff->GetMaximum();
  double rightmin = sEff->GetMinimum();
  cout << c.GetUymax() << endl;
  cout << c.GetUymin() << endl;
  cout << rightmax << endl;
  cout << rightmin << endl;
  double scale = c.GetUymax()/rightmax;
  cout << "c uymax " << c.GetUymax() << endl;
  cout << "c uymin " << c.GetUymin() << endl;
  cout << "scale " << scale << endl;
  cout << "sEff " << sEff->GetMaximum() << endl;
  cout << "sEff " << sEff->GetMaximumBin() << endl;
  sEff->Scale(scale);
  cout << "sEff " << sEff->GetMaximum() << endl;
  cout << "sEff " << sEff->GetMaximumBin() << endl;
  // TF1 foffset("foffset", "[0]", -1 ,1 );
  // foffset.SetParameter(0, c.GetUymin());
  // sEff->Add(&foffset);
  sEff->SetLineColor(kRed);
  sEff->SetMarkerColor(kRed);
  sEff->Draw("ESAME");
  // sEff->Draw();
  TGaxis axis(c.GetUxmax(),c.GetUymin(),
                            c.GetUxmax(), c.GetUymax(),
              0, rightmax, 510, "+L");
  axis.SetLineColor(kRed);
  axis.SetLabelColor(kRed);
  axis.SetTitle("Signal efficiency");
  axis.Draw();
  TLine l;
  TLatex tex;
  tex.SetTextSize(0.03);
  tex.SetTextFont(42);
  l.SetLineStyle(2);
  for (const auto & f : fs) {
    const auto& func = std::get<0>(f);
    double xMax = func.GetMaximumX();
    l.DrawLine(xMax, c.GetUymin(),
               xMax, c.GetUymax());
    tex.DrawLatex(xMax, 1.0+c.GetUymin(), Form("MVA: %g", xMax));
    tex.DrawLatex(xMax, 0.5*(c.GetUymin()+c.GetUymin()), Form("Max: %g", func.GetMaximum()));
  }
  c.Print(Form("%s.pdf", hSig->GetName()));
  return hSig;
}

void calSig()
{
  TFile* fS = TFile::Open("TMVA_MC_pPb_LambdaCKsP3to4App.root");
  auto h3S_BDT = fS->Get<TH3>("lambdacAna_mc/hMassPtMVA_BDTG_3D_y0");
  auto h3S_MLP = fS->Get<TH3>("lambdacAna_mc/hMassPtMVA_MLP3to4Np2N_y0");
  auto fB = TFile::Open("TMVA_pPb_LambdaCKsP3to4App.root");
  auto h3B_BDT = fB->Get<TH3>("lambdacAna/hMassPtMVA_BDTG_3D_y0");
  auto h3B_MLP = fB->Get<TH3>("lambdacAna/hMassPtMVA_MLP3to4Np2N_y0");
  if (!h3B_BDT) { cout << "Empty" << endl; return; }
  auto sEffBDT = getEff(true, h3S_BDT, "BDT");
  auto sEffMLP = getEff(true, h3S_MLP, "MLP");
  auto sEffMLPRebin = getEff(true, h3S_MLP, "MLPRebin", 10);
  auto bEffBDT = getEff(false, h3B_BDT, "BDT");
  auto bEffMLP = getEff(false, h3B_MLP, "MLP");

  auto yields = fit();
  auto signalYieldsBDT = yields["BDT"].front().first/sEffBDT->GetBinContent(85);
  auto signalYieldsMLP = yields["MLP"].front().first/sEffMLP->GetBinContent(260);

  auto hBackgroundBDT = getInt(h3B_BDT, "BDT");
  auto hBackgroundMLP = getInt(h3B_MLP, "MLP");
  auto hBackgroundMLPRebin = getInt(h3B_MLP, "MLPRebin", 10);

  vector<tuple<TF1, double, double>>  bdt;
  TF1 fBDT("fBDT", "pol3", 0.4, 0.7);
  bdt.push_back(make_tuple(fBDT, 0.4, 0.7));
  auto hSigBDT = calSignificance(signalYieldsBDT, sEffBDT, hBackgroundBDT, "BDT", bdt);
  vector<tuple<TF1, double, double>>  mlp;
  TF1 fMLP1("MLP1", "pol3", 0.001, 0.0025);
  TF1 fMLP2("MLP2", "pol3", 0.0035, 0.008);
  mlp.push_back(make_tuple(fMLP1, 0.001, 0.0025));
  mlp.push_back(make_tuple(fMLP2, 0.0035, 0.008));
  auto hSigMLP = calSignificance(signalYieldsMLP, sEffMLP, hBackgroundMLP, "MLP", mlp);
  auto hSigMLPRebin =calSignificance(signalYieldsMLP, sEffMLPRebin, hBackgroundMLPRebin, "MLPRebin", mlp);
}

void calSigUp()
{
  TFile* fS = TFile::Open("TMVA_MC_pPb_LambdaCKsP3to4App.root");
  auto h3S_BDT = fS->Get<TH3>("lambdacAna_mc/hMassPtMVA_BDTG_3D_y0");
  auto h3S_MLP = fS->Get<TH3>("lambdacAna_mc/hMassPtMVA_MLP3to4Np2N_y0");
  auto fB = TFile::Open("TMVA_pPb_LambdaCKsP3to4App.root");
  auto h3B_BDT = fB->Get<TH3>("lambdacAna/hMassPtMVA_BDTG_3D_y0");
  auto h3B_MLP = fB->Get<TH3>("lambdacAna/hMassPtMVA_MLP3to4Np2N_y0");
  if (!h3B_BDT) { cout << "Empty" << endl; return; }
  auto sEffBDT = getEff(true, h3S_BDT, "BDT");
  auto sEffMLP = getEff(true, h3S_MLP, "MLP");
  auto sEffMLPRebin = getEff(true, h3S_MLP, "MLPRebin", 10);
  auto bEffBDT = getEff(false, h3B_BDT, "BDT");
  auto bEffMLP = getEff(false, h3B_MLP, "MLP");

  auto yields = fit();
  auto signalYieldsBDT = yields["BDT"].at(1).first/sEffBDT->GetBinContent(85);
  auto signalYieldsMLP = yields["MLP"].at(1).first/sEffMLP->GetBinContent(260);

  auto hBackgroundBDT = getInt(h3B_BDT, "BDT");
  auto hBackgroundMLP = getInt(h3B_MLP, "MLP");
  auto hBackgroundMLPRebin = getInt(h3B_MLP, "MLPRebin", 10);

  vector<tuple<TF1, double, double>>  bdt;
  TF1 fBDT("fBDT", "pol3", 0.4, 0.7);
  bdt.push_back(make_tuple(fBDT, 0.4, 0.7));
  auto hSigBDT = calSignificance(signalYieldsBDT, sEffBDT, hBackgroundBDT, "BDTUp", bdt);
  vector<tuple<TF1, double, double>>  mlp;
  TF1 fMLP1("MLP1", "pol3", 0.001, 0.0025);
  TF1 fMLP2("MLP2", "pol3", 0.0035, 0.008);
  mlp.push_back(make_tuple(fMLP1, 0.001, 0.0025));
  mlp.push_back(make_tuple(fMLP2, 0.0035, 0.008));
  auto hSigMLP = calSignificance(signalYieldsMLP, sEffMLP, hBackgroundMLP, "MLPUp", mlp);
  auto hSigMLPRebin =calSignificance(signalYieldsMLP, sEffMLPRebin, hBackgroundMLPRebin, "MLPRebinUp", mlp);
}

void calSigLw()
{
  TFile* fS = TFile::Open("TMVA_MC_pPb_LambdaCKsP3to4App.root");
  auto h3S_BDT = fS->Get<TH3>("lambdacAna_mc/hMassPtMVA_BDTG_3D_y0");
  auto h3S_MLP = fS->Get<TH3>("lambdacAna_mc/hMassPtMVA_MLP3to4Np2N_y0");
  auto fB = TFile::Open("TMVA_pPb_LambdaCKsP3to4App.root");
  auto h3B_BDT = fB->Get<TH3>("lambdacAna/hMassPtMVA_BDTG_3D_y0");
  auto h3B_MLP = fB->Get<TH3>("lambdacAna/hMassPtMVA_MLP3to4Np2N_y0");
  if (!h3B_BDT) { cout << "Empty" << endl; return; }
  auto sEffBDT = getEff(true, h3S_BDT, "BDT");
  auto sEffMLP = getEff(true, h3S_MLP, "MLP");
  auto sEffMLPRebin = getEff(true, h3S_MLP, "MLPRebin", 10);
  auto bEffBDT = getEff(false, h3B_BDT, "BDT");
  auto bEffMLP = getEff(false, h3B_MLP, "MLP");

  auto yields = fit();
  auto signalYieldsBDT = yields["BDT"].at(2).first/sEffBDT->GetBinContent(85);
  auto signalYieldsMLP = yields["MLP"].at(2).first/sEffMLP->GetBinContent(260);

  auto hBackgroundBDT = getInt(h3B_BDT, "BDT");
  auto hBackgroundMLP = getInt(h3B_MLP, "MLP");
  auto hBackgroundMLPRebin = getInt(h3B_MLP, "MLPRebin", 10);

  vector<tuple<TF1, double, double>>  bdt;
  TF1 fBDT("fBDT", "pol3", 0.4, 0.7);
  bdt.push_back(make_tuple(fBDT, 0.4, 0.7));
  auto hSigBDT = calSignificance(signalYieldsBDT, sEffBDT, hBackgroundBDT, "BDTLw", bdt);
  vector<tuple<TF1, double, double>>  mlp;
  TF1 fMLP1("MLP1", "pol3", 0.001, 0.0025);
  TF1 fMLP2("MLP2", "pol3", 0.0035, 0.008);
  mlp.push_back(make_tuple(fMLP1, 0.001, 0.0025));
  mlp.push_back(make_tuple(fMLP2, 0.0035, 0.008));
  auto hSigMLP = calSignificance(signalYieldsMLP, sEffMLP, hBackgroundMLP, "MLPLW", mlp);
  auto hSigMLPRebin =calSignificance(signalYieldsMLP, sEffMLPRebin, hBackgroundMLPRebin, "MLPRebinLw", mlp);
}
*/

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


void optimizer()
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
    // TF1 f("f", "gausn(0)+cheb3(3)", 2.15, 2.45);
    TF1 f("f", "[0]*([1] * TMath::Gaus(x, [2], [3]*(1+[5]), true) + (1-[1]) * TMath::Gaus(x, [2], [4]*(1+[5]), true)) + cheb3(6)", 2.15, 2.45);
    f.SetParameters(parameters.data());
    const double width = sqrt(pow(parameters.at(3), 2) * parameters.at(1)
                              + pow(parameters.at(4), 2) * (1-parameters.at(1)));
    widths.push_back(width);
    const double sPlusBFit = f.Integral(2.2865-2.6*width, 2.2865+2.6*width) / h->GetBinWidth(1);
    fitInt.push_back(sPlusBFit);

    for (int i=6; i<10; ++i) {
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
  gYields.Fit(&fConstant, "R", "", mvaCuts[0], mvaCuts[nMVA-1]);
  const double total = fConstant.GetParameter(0);
  gYields.Fit(&fConstant, "R", "", mvaCuts[0]-0.0005, mvaCuts[nMVA-1]+0.0005);
  auto hYieldsFrame = gYields.GetHistogram();
  hYieldsFrame->SetTitle("Expected yields w/o MVA cuts;MVA;s'");
  gYields.Draw("AP");
  cYields.Print("plots/yieldsMLP.pdf");

  TCanvas c("sig", "", 600, 450);
  // TH1D* hSig = (TH1D*)hEff->Clone("sigMLP");
  TGraphErrors gSig(nMVA);
  // const double total = yields_MC.at(5);

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
    const auto effbin = hEff->FindBin(mvaCut+1E-4);
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
  hframe->GetYaxis()->SetRangeUser(4., 7.);
  hframe->SetTitle("MLP;MVA;S/#sqrt{S+B}");
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
  TLine l;
  l.SetLineStyle(2);
  l.DrawLine(fitToSig.GetMaximumX(), 4., fitToSig.GetMaximumX(), 7.);
  TLegend leg(0.15, 0.7, 0.35, 0.85);
  leg.SetFillStyle(0);
  leg.SetBorderSize(0);
  leg.AddEntry( &gSig, "MC", "P");
  leg.AddEntry( &sig, "data", "P");
  leg.Draw();
  c.Print("plots/sigMLP.pdf");
}


void optimizerBDT()
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
  auto hEff = getEff(true, hBDTMC, "BDT", pTMin, pTMax);
  /*const double mvaCuts[] = {0.003, 0.0035, 0.004, 0.0045, 0.005, 0.0055, 0.006, 0.007};*/
  const int nMVA = 40;
  double mvaCuts[nMVA];
  for (size_t i=0; i != nMVA; ++i) {
    mvaCuts[i] = 0.4 + 0.01 * i;
  }
  std::vector<TH1*> hBDT1Ds;
  std::vector<TH1*> hMCBDT1Ds;
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
    auto h = proj1D(hBDT, mvaCutStr,
                    pTMin, pTMax, mvaCut, 1);
    // h->Rebin();
    auto hMC = proj1D(hBDTMC, mvaCutStr+"MC",
                      pTMin, pTMax, mvaCut, 1);
    h->SetTitle(Form("BDT>%f", mvaCut));
    hMC->SetTitle(Form("MC BDT>%f", mvaCut));
    hBDT1Ds.push_back(h);
    hMCBDT1Ds.push_back(hMC);
    TCanvas c("c"+mvaCutStr, "", 600*2, 450);
    c.Divide(2, 1);
    const double pars[] = {20000, -5000, 100, 200};
    auto parameters = simpleFit(hMC, h, pars,
              2.15, 2.45, &c);
    // TF1 f("f", "gausn(0)+cheb3(3)", 2.15, 2.45);
    TF1 f("f", "[0]*([1] * TMath::Gaus(x, [2], [3]*(1+[5]), true) + (1-[1]) * TMath::Gaus(x, [2], [4]*(1+[5]), true)) + cheb3(6)", 2.15, 2.45);
    f.SetParameters(parameters.data());
    const double width = sqrt(pow(parameters.at(3), 2) * parameters.at(1)
                              + pow(parameters.at(4), 2) * (1-parameters.at(1)));
    widths.push_back(width);
    const double sPlusBFit = f.Integral(2.2865-2.6*width, 2.2865+2.6*width) / h->GetBinWidth(1);
    fitInt.push_back(sPlusBFit);

    for (int i=6; i<10; ++i) {
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
  for (auto h : hBDT1Ds) h->Delete();
  for (auto h : hMCBDT1Ds) h->Delete();
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
  gYields.Fit(&fConstant);
  auto hYieldsFrame = gYields.GetHistogram();
  hYieldsFrame->SetTitle("Expected yields w/o MVA cuts;MVA;s'");
  gYields.Draw("AP");
  cYields.Print("plots/yieldsBDT.pdf");

  const double total = fConstant.GetParameter(0);
  TCanvas c("sig", "", 600, 450);
  // TH1D* hSig = (TH1D*)hEff->Clone("sigBDT");
  TGraphErrors gSig(nMVA);
  // const double total = yields_MC.at(5);

  for (int i=0; i != nMVA; ++i) {
    const auto mvaCut = mvaCuts[i];
    const double pTMin = 3.0;
    const double pTMax = 4.0;
    TString mvaCutStr = Form("mva%f", mvaCut);
    auto pos = mvaCutStr.Index(".");
    mvaCutStr.Replace(pos, 1, "p");
    auto h = proj1D(hBDT, mvaCutStr,
                    pTMin, pTMax, mvaCut, 1);
    const double halfWindow = 2.6 * widths.at(i);
    // const double halfWindow = 2.6 * 0.01;
    const auto effbin = hEff->FindBin(mvaCut+1E-4);
    cout << mvaCut << " effbin " << effbin << endl;
    const double yields = total * hEff->GetBinContent(effbin);
    const auto massLw = h->FindBin(2.2865-halfWindow);
    const auto massUp = h->FindBin(2.2865+halfWindow);
    // cout << widths.at(i) << endl;
    const double sPlusB = h->Integral(massLw, massUp);
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
  hframe->GetYaxis()->SetRangeUser(3., 8.);
  hframe->SetTitle("BDT;MVA;S/#sqrt{S+B}");
  TF1 fitToSig("fitToSig", "pol3", mvaCuts[0]-0.001, mvaCuts[nMVA-1]+0.001);
  fitToSig.SetParameter(0, 5);
  fitToSig.SetParameter(1, 0.01);
  fitToSig.SetParameter(2, -0.01);
  gSig.Fit(&fitToSig, "R", "", mvaCuts[0]-0.001, mvaCuts[nMVA-1]+0.001);
  cout << fitToSig.GetMaximumX() << endl;
  gSig.Draw("APE");
  TGraph sig(nMVA, mvaCuts, dataSig.data());
  sig.SetMarkerStyle(kOpenSquare);
  sig.Draw("PE");
  TLine l;
  l.SetLineStyle(2);
  l.DrawLine(fitToSig.GetMaximumX(), 3., fitToSig.GetMaximumX(), 8.);
  TLegend leg(0.15, 0.7, 0.35, 0.85);
  leg.SetFillStyle(0);
  leg.SetBorderSize(0);
  leg.AddEntry( &gSig, "MC", "P");
  leg.AddEntry( &sig, "data", "P");
  leg.Draw();
  c.Print("plots/sigBDT.pdf");
}
