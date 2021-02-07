#include "TH1D.h"
#include "TGraphAsymmErrors.h"

#include "ROOT/RMakeUnique.hxx"

#include <array>
#include <memory>

using namespace std;

array<unique_ptr<TGraphAsymmErrors>, 2>
calEff(const TH1D* hS, const TH1D* hB)
{
  auto n = hS->GetNbinsX();

  auto hIntegralS = std::make_unique<TH1D>(*hS);
  auto hIntegralB = std::make_unique<TH1D>(*hB);

  auto hIntegralSAll = std::make_unique<TH1D>(*hS);
  auto hIntegralBAll = std::make_unique<TH1D>(*hB);

  hIntegralS->Reset("ICEM");
  hIntegralB->Reset("ICEM");

  hIntegralSAll->Reset("ICEM");
  hIntegralBAll->Reset("ICEM");

  double sAllErr, bAllErr;
  double sAll = hS->IntegralAndError(0, n+1, sAllErr);
  double bAll = hB->IntegralAndError(0, n+1, bAllErr);

  for (int ibin=1; ibin<n+1; ibin++) {
    double s, serr;
    double b, berr;
    s = hS->IntegralAndError(ibin, n+1, serr);
    b = hB->IntegralAndError(ibin, n+1, berr);

    hIntegralS->SetBinContent(ibin, s);
    hIntegralS->SetBinError(ibin, serr);
    hIntegralB->SetBinContent(ibin, b);
    hIntegralB->SetBinError(ibin, berr);

    hIntegralSAll->SetBinContent(ibin, sAll);
    hIntegralSAll->SetBinError(ibin, sAllErr);
    hIntegralBAll->SetBinContent(ibin, bAll);
    hIntegralBAll->SetBinError(ibin, bAllErr);
  }
  array<unique_ptr<TGraphAsymmErrors>, 2> gEff;
  gEff[0] = std::make_unique<TGraphAsymmErrors>(n);
  gEff[1] = std::make_unique<TGraphAsymmErrors>(n);

  gEff[0]->Divide(hIntegralS.get(), hIntegralSAll.get());
  gEff[1]->Divide(hIntegralB.get(), hIntegralBAll.get());

  gEff[0]->SetName(hS->GetName());
  gEff[1]->SetName(hB->GetName());

  gEff[0]->SetTitle(hS->GetTitle());
  gEff[1]->SetTitle(hB->GetTitle());
  return gEff;
}
