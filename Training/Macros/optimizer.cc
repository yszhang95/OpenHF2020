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
  TF1 fMC("fMC", "[0]*([1] * TMath::Gaus(x, [2], [3]*(1+[5]), true) + (1-[1]) * TMath::Gaus(x, [2], [4]*(1+[5]), true))");
  fMC.SetParameter(0, 100);
  fMC.SetParameter(1, 0.5);
  fMC.SetParLimits(1, 0, 1);
  fMC.SetParameter(2, 2.2865);
  fMC.SetParameter(3, 0.005);
  fMC.SetParameter(4, 0.01);
  fMC.FixParameter(5, 0);
  hMC->Fit(&fMC, "R", "", 2.2, 2.38);
  hMC->Fit(&fMC, "R", "", 2.2, 2.38);
  c->cd(2);
  gROOT->ForceStyle();
  gStyle->SetOptFit(1111);
  ROOT::Math::ChebyshevPol chebyshev(3);
  auto fbkgReject = make_shared<TF1>
    ("fbkgReject", bkgRejctPeak, lw, up, 4);
  auto fbkg = make_shared<TF1>("fbkg", &chebyshev, lw, up, 4);

  auto f = make_shared<TF1>("f", "fMC + cheb3(6)", lw, up);

  fbkgReject->SetParameters(bkgvars);
  h->Fit(fbkgReject.get(), "R", "", lw, up);
  h->Fit(fbkgReject.get(), "MR", "", lw, up);

  for (int i=0; i<fbkgReject->GetNpar(); ++i) {
    fbkg->SetParameter(i, fbkgReject->GetParameter(i));
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

   c->Draw();

   c->cd();
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

/**
  Parameters we need:
  1. input data file
  2. input MC file
  3. input TH3D name in data
  4. input TH3D name in MC
  5. pTMin
  6. pTMax
  7. MVA name
  8. MVA range
 */
struct PARS
{
  std::string dataFileName;
  std::string dataHistName;

  std::string mcFileName;
  std::string mcHistName;

  std::string mvaName;

  std::vector<double> mvaRange;

  double pTMin;
  double pTMax;

  PARS(const double start, const double end, const double step)
    : pTMin(0), pTMax(0)
  {
    const double n_double = std::round((end - start)/step);
    const unsigned int n = static_cast<unsigned int> (n_double);
    for (int i=0; i<n; ++i) {
      mvaRange.push_back( start + i * step );
    }
    mvaRange.shrink_to_fit();
  }
};

void optimizer(PARS mypars)
{
  TFile f( mypars.dataFileName.c_str() );
  TH3D* hMVA;
  f.GetObject(mypars.dataHistName.c_str(), hMVA);
  if (!hMVA) { cout << "bad pointer for " << mypars.mvaName << endl; }

  TFile fMC(mypars.mcFileName.c_str());
  TH3D* hMVAMC;
  fMC.GetObject(mypars.mcHistName.c_str(), hMVAMC);
  if (!hMVAMC) { cout << "bad pointer for " << mypars.mcHistName << endl; }

  const double pTMin = mypars.pTMin;
  const double pTMax = mypars.pTMax;
  auto hEff = getEff(true, hMVAMC, mypars.mvaName.c_str(), pTMin, pTMax);

  const auto mvaCuts = mypars.mvaRange;
  const auto nMVA = mvaCuts.size();

  std::vector<TH1*> hMVA1Ds;
  std::vector<TH1*> hMVA1DMCs;

  // yields of data histogram within (-/+ 2.6 width)
  std::vector<double> histInt;

  // yields without MVA cuts, using yields data (within -/+2.6 width)
  // divided by MC efficiency (not account for mass range)
  std::vector<double> yields_MC;
  // error on yields_MC
  std::vector<double> yieldsErr_MC;

  // signal yields from fit
  std::vector<double> yields_data;
  // error on signal yields from fit
  std::vector<double> yieldsErr_data;
  // significance from yields_data/yieldsErr_data
  std::vector<double> sigFit;

  // significance calculated using S/sqrt(S+B), S and S+B are from data fit, integral over -/+ 2.6 wdith
  std::vector<double> dataSig;
  // S+B integral within -/+ 2.6 * width (see below), from data fit
  std::vector<double> fitInt;
  // width for signal,sqrt( c1*sigma1**2 + (1-c1)*sigma2**2 )
  std::vector<double> widths;

  for (const auto mvaCut : mvaCuts) {
    TString mvaCutStr = Form("%s_%f_pT%.1fto%.1f", mypars.mvaName.c_str(),
                             mvaCut, pTMin, pTMax);
    auto pos = mvaCutStr.Index(".");
    for ( ; pos>=0 ;) {
      mvaCutStr.Replace(pos, 1, "p");
      pos = mvaCutStr.Index(".");
    }

    auto h = proj1D(hMVA, mvaCutStr,
                    pTMin, pTMax, mvaCut, 1);
    // h->Rebin();
    auto hMC = proj1D(hMVAMC, mvaCutStr+"MC",
                      pTMin, pTMax, mvaCut, 1);
    h->SetTitle(Form("%s>%f", mypars.mvaName.c_str(), mvaCut));
    hMC->SetTitle(Form("MC %s>%f", mypars.mvaName.c_str(), mvaCut));
    h->GetXaxis()->SetTitle("M_{K_{s}^{0}p} (GeV)");
    hMC->GetXaxis()->SetTitle("M_{K_{s}^{0}p} (GeV)");
    h->GetYaxis()->SetTitle(Form("Events / %.3f GeV", h->GetBinWidth(1)));
    hMC->GetYaxis()->SetTitle(Form("Events / %.3f GeV", hMC->GetBinWidth(1)));
    hMVA1Ds.push_back(h);
    hMVA1DMCs.push_back(hMC);
    TCanvas c("c"+mvaCutStr, "", 600*2, 450);
    c.Divide(2, 1);
    const double pars[] = {20000, -5000, 100, 200};
    auto parameters = simpleFit(hMC, h, pars,
              2.15, 2.45, &c);
    TF1 f("f", "[0]*([1] * TMath::Gaus(x, [2], [3]*(1+[5]), true) + (1-[1]) * TMath::Gaus(x, [2], [4]*(1+[5]), true)) + cheb3(6)", 2.15, 2.45);
    f.SetParameters(parameters.data());
    const double width = sqrt(pow(parameters.at(3), 2) * parameters.at(1)
                              + pow(parameters.at(4), 2) * (1-parameters.at(1)));
    widths.push_back(width);

    const double sPlusBFit = f.Integral(2.2865-2.6*width, 2.2865+2.6*width) / h->GetBinWidth(1);
    fitInt.push_back(sPlusBFit);
    // const double par0 = f.GetParameter(0);
    // f.SetParameter(0, 0);
    // const double sPlusBFit = f.Integral(2.2865-2.6*width, 2.2865+2.6*width) / h->GetBinWidth(1);
    // fitInt.push_back(sPlusBFit);
    // f.SetParameter(0, par0);

    for (int i=6; i<10; ++i) {
      f.SetParameter(i, 0);
    }

    const double sDataFit = f.Integral(2.2865-2.6*width, 2.2865+2.6*width) / h->GetBinWidth(1);
    dataSig.push_back(sDataFit/std::sqrt(sPlusBFit));

    yields_data.push_back( parameters.at(0)/h->GetBinWidth(1) );
    yieldsErr_data.push_back( parameters.at(0+parameters.size()/2)/h->GetBinWidth(1) );
    sigFit.push_back(yields_data.back()/yieldsErr_data.back());

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
  TF1 fConstant("yields", "[0]", mvaCuts[0], mvaCuts[nMVA-1]);
  fConstant.SetParameter(0, yields_MC.front());
  gYields.Fit(&fConstant, "R", "", mvaCuts[0], mvaCuts[nMVA-1]);
  const double total = fConstant.GetParameter(0);
  gYields.Fit(&fConstant, "R", "", mvaCuts[0], mvaCuts[nMVA-1]);
  auto hYieldsFrame = gYields.GetHistogram();
  hYieldsFrame->SetTitle("Expected yields w/o MVA cuts;MVA;s'");
  gYields.Draw("AP");


  TCanvas c("sig", "", 600, 450);
  TGraphErrors gSig(nMVA);

  for (int i=0; i != nMVA; ++i) {
    const auto mvaCut = mvaCuts[i];
    TString mvaCutStr = Form("%s_%f_pT%.1fto%.1f", mypars.mvaName.c_str(),
                             mvaCut, pTMin, pTMax);
    auto pos = mvaCutStr.Index(".");
    for ( ; pos>=0 ;) {
      mvaCutStr.Replace(pos, 1, "p");
      pos = mvaCutStr.Index(".");
    }
    /*
    auto h = proj1D(hMVA, mvaCutStr,
                    pTMin, pTMax, mvaCut, 1);
    */
    auto h = hMVA1Ds.at(i);
    const double halfWindow = 2.6 * widths.at(i);
    const auto effbin = hEff->FindBin(mvaCut);
    // cout << mvaCut << " effbin " << effbin << endl;
    const double yields = total * hEff->GetBinContent(effbin);
    /*
    const auto massLw = h->FindBin(2.2865-halfWindow);
    const auto massUp = h->FindBin(2.2865+halfWindow);
    // cout << widths.at(i) << endl;
    double sPlusB = h->Integral(massLw, massUp);
    */
    // cout << sPlusB << endl;
    // sPlusB = sPlusB - yields;
    // cout << sPlusB << endl;
    //const double sig = yields/sqrt(sPlusB);
    const double sig = yields/sqrt(fitInt.at(i));
    // const double err = total/sqrt(sPlusB) * hEff->GetBinError(effbin);
    const double err = total/sqrt(fitInt.at(i)) * hEff->GetBinError(effbin);
    gSig.SetPoint(i, mvaCut, sig);
    // cout << sig << endl;
    gSig.SetPointError(i, 0, err);
    h->Delete();
  }
  gSig.SetMarkerStyle(kFullCircle);
  auto hframe = gSig.GetHistogram();
  const double sigMin = 2.0;
  const double sigMax = 12.0;
  hframe->GetYaxis()->SetRangeUser(sigMin, sigMax);
  hframe->SetTitle(Form("%s pT %.1f to %.1f;MVA;S/#sqrt{S+B}",
                        mypars.mvaName.c_str(), pTMin, pTMax));
  TF1 fitToSig("fitToSig", "pol3", mvaCuts[0], mvaCuts[nMVA-1]);
  fitToSig.SetParameter(0, 5);
  fitToSig.SetParameter(1, 0.01);
  fitToSig.SetParameter(2, -0.01);
  //gSig.Fit(&fitToSig, "R", "", 0.004, 0.008);
  gSig.Fit(&fitToSig, "R", "", mvaCuts[0], mvaCuts[nMVA-1]);
  // cout << fitToSig.GetMaximumX() << endl;
  gSig.Draw("APE");
  TGraph sig(nMVA, mvaCuts.data(), dataSig.data());
  sig.SetMarkerStyle(kOpenSquare);
  sig.Draw("PE");
  TGraph sigDataFit(nMVA, mvaCuts.data(), sigFit.data());
  sigDataFit.SetMarkerStyle(kFullSquare);
  sigDataFit.Draw("PE");
  TLine l;
  l.SetLineStyle(2);
  l.DrawLine(fitToSig.GetMaximumX(), sigMin, fitToSig.GetMaximumX(), sigMax);
  TLegend leg(0.15, 0.7, 0.35, 0.85);
  leg.SetFillStyle(0);
  leg.SetBorderSize(0);
  leg.AddEntry( &gSig, "MC", "P");
  leg.AddEntry( &sig, "data", "P");
  leg.Draw();
  TString outputPlotName = Form("%s_pT%.1f_%.1f", mypars.mvaName.c_str(), pTMin, pTMax);
  for (auto pos = outputPlotName.Index("."); pos>=0; ) {
    outputPlotName = outputPlotName.Replace(pos, 1, "p");
    pos = outputPlotName.Index(".");
  }

  c.Print("plots/sig_" + outputPlotName + ".pdf");

  cYields.Print(Form("plots/yields_%s.pdf", outputPlotName.Data()));

  // for (auto h : hMVA1Ds) h->Delete();
  // for (auto h : hMVA1DMCs) h->Delete();
}

void optimizer()
{
  // 2 to 3 MB
  /*
  PARS mypars(0.0002, 0.0016, 0.0001);
  mypars.dataFileName = "TMVA_MB_LamCKsP2to3App.root";
  mypars.mcFileName = "Merged_MC_LambdaCKsP2to3App.root";
  mypars.dataHistName = "hMassPtMVA_MLP2to3MBNp2N_noDR_y0_Total";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP2to3MBNp2N_noDR_y0";
  mypars.mvaName = "MLP2to3MBNp2N_noDR";
  mypars.pTMin = 2.0;
  mypars.pTMax = 3.0;
  optimizer(mypars);
  */

  PARS mypars(0.0002, 0.008, 0.0001);
  mypars.dataFileName = "dataMB_pPb_3to4.root";
  mypars.mcFileName = "TMVA_MC_pPb_LambdaCKsP3to4App_MB.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP3to4MBNp2N_noDR_y0";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP3to4MBNp2N_noDR_y0";
  mypars.mvaName = "MLP3to4MBNp2N_noDR";
  mypars.pTMin = 3.0;
  mypars.pTMax = 4.0;
  optimizer(mypars);

  /*
  PARS mypars(0.0002, 0.006, 0.0001);
  mypars.dataFileName = "dataMB_pPb_4to6.root";
  mypars.mcFileName = "TMVA_MC_pPb_LambdaCKsP4to6App_MB.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP4to6MBNp2N_noDR_y0";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP4to6MBNp2N_noDR_y0";
  mypars.mvaName = "MLP4to6MBNp2N_noDR";
  mypars.pTMin = 4.0;
  mypars.pTMax = 5.0;
  optimizer(mypars);
  */
  /*
  PARS mypars(0.0002, 0.006, 0.0001);
  mypars.dataFileName = "dataMB_pPb_4to6.root";
  mypars.mcFileName = "TMVA_MC_pPb_LambdaCKsP4to6App_MB.root";
  mypars.dataHistName = "lambdacAna/hMassPtMVA_MLP4to6MBNp2N_noDR_y0";
  mypars.mcHistName = "lambdacAna_mc/hMassPtMVA_MLP4to6MBNp2N_noDR_y0";
  mypars.mvaName = "MLP4to6MBNp2N_noDR";
  mypars.pTMin = 5.0;
  mypars.pTMax = 6.0;
  optimizer(mypars);
  */
}
