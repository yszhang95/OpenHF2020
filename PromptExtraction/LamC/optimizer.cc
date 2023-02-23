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
  // TF1 fMC("fMC", "[0]*([1] * TMath::Gaus(x, [2], [3]*(1+[5]), true) + (1-[1]) * TMath::Gaus(x, [2], [4]*(1+[5]), true))");
  TF1 fMC("fMC", "[0]*([1] * TMath::Gaus(x, [2], [3]*[5], true) + (1-[1]) * TMath::Gaus(x, [2], [3]*[4]*[5], true))");
  fMC.SetParameter(0, 100);
  fMC.SetParameter(1, 0.5);
  fMC.SetParLimits(1, 0, 1);
  fMC.SetParameter(2, 2.2865);
  fMC.SetParameter(3, 0.005);
  // fMC.SetParameter(4, 0.01);
  fMC.SetParameter(4, 5);
  fMC.SetParLimits(4, 0, 20);
  fMC.SetParLimits(3, 0, 0.02);
  // fMC.FixParameter(5, 0);
  fMC.FixParameter(5, 1);
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

  double bkg_vars[4] = {bkgvars[0], bkgvars[1], bkgvars[2], bkgvars[3]};
  bkg_vars[0] = h->GetBinContent(1);
  bkg_vars[1] = (h->GetBinContent(h->GetNbinsX())-bkg_vars[0])/(h->GetBinLowEdge(h->GetNbinsX()-1)-h->GetBinLowEdge(1));

  fbkgReject->SetParameters(bkg_vars);
  h->Fit(fbkgReject.get(), "R", "", lw, up);
  auto result = h->Fit(fbkgReject.get(), "MRS", "", lw, up);
  // if (h->Integral() > 0 && result->Chi2()/result->Ndf() > 2) { throw std::logic_error("Bad fit"); }

  std::cout << "Done sideband fit\n";

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
   // f->SetParameter(5, 0);
   // f->SetParLimits(5, -1, 1);
   f->SetParameter(5, 1);
   f->SetParLimits(5, 0.01, 10);
   // h->Fit(f.get(), "R", "", lw, up);
   auto result_2 = h->Fit(f.get(), "SR", "", lw, up);
   if (result_2.Get()) {
     result_2->GetCorrelationMatrix().Print();
   }
   std::cout << "Done for the fit to ta fixing 1, 3, 4 and background\n";
   for (int i=0; i != 5; ++i) {
     // f->ReleaseParameter(i);
   }

   bool fixByMC = false;
   // if (std::abs(f->GetParameter(2)-fMC.GetParameter(2) > fMC.GetParameter(3)))
   //if (std::abs(f->GetParameter(2)-fMC.GetParameter(2)) > 0.003)
   if (std::abs(f->GetParameter(0)/f->GetParError(0)) < 2. || std::abs(f->GetParameter(2)-fMC.GetParameter(2)) > 0.003)
   {
     fixByMC = true;
     std::cout << "Fix mean to MC because S/B is too small\n";
     f->FixParameter(2, fMC.GetParameter(2));
     // f->FixParameter(5, 0);
     f->FixParameter(5, 1);
   }
   auto result_3 = h->Fit(f.get(), "SRM", "", lw, up);
   if (result_3.Get()) {
     result_3->GetCorrelationMatrix().Print();
   }

  for (int i=0; i<fbkgReject->GetNpar(); ++i) {
    // I do not release background because I find the HESSE is always not positive definite.
    // Fixing the background will improve the fit quality
    // fbkg->SetParameter(i, fbkgReject->GetParameter(i));
    // f->ReleaseParameter(i+6);
  }

  // always do so
  f->FixParameter(5, 1);
  // sigma is allowed to float, then parameter 5 is redundant.
  if (!fixByMC) {
    f->ReleaseParameter(3);
  } else {
    // f->FixParameter(3, fMC.GetParameter(3));
    f->SetParameter(0, std::abs(f->GetParameter(0)));
    f->SetParLimits(0, 0, 5*f->GetParameter(0));
    // f->ReleaseParameter(6);
    // f->ReleaseParameter(7);
    // f->ReleaseParameter(8);
    // f->ReleaseParameter(9);
  }

   auto result_all = h->Fit(f.get(), "RMS", "", lw, up);
   // std::cout << "Done the fit after releasing background\n";
   if (result_all.Get()) {
     result_all->GetCorrelationMatrix().Print();
   }

   // h->Fit(f.get(), "LRM", "", lw, up);
   //h->GetXaxis()->SetLimits(lw, up);
   h->GetXaxis()->SetRangeUser(lw, up);

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

   if (h->Integral() < 500) {
     f->SetParameter(0, 1E-7);
     f->SetParError(0, 1E-7);
   }
    if (h->Integral() > 0 && result->Chi2()/result->Ndf() > 2.1) { 
      f->SetParameter(0, 0);
      f->SetParError(0, 0);
    }
   vector<double> output;
   std::copy(f->GetParameters(),
             f->GetParameters()+f->GetNpar(),
             std::back_inserter(output));
   std::copy(f->GetParErrors(),
             f->GetParErrors()+f->GetNpar(),
             std::back_inserter(output));
   return output;
}
