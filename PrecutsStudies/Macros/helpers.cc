#include "TH1D.h"
#include "TGraphAsymmErrors.h"

#include "ROOT/RMakeUnique.hxx"

#include <array>
#include <memory>

using namespace std;

enum class Direction {less, larger};

array<unique_ptr<TGraphAsymmErrors>, 2>
calEff(const TH1D* hS, const TH1D* hB, const Direction dir=Direction::larger)
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

    auto start = dir == Direction::larger ?  ibin : 0;
    auto end = dir == Direction::larger ? (n+1) : ibin;

    double s, serr;
    double b, berr;
    s = hS->IntegralAndError(start, end, serr);
    b = hB->IntegralAndError(start, end, berr);

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

  gEff[0]->SetName(TString(hS->GetName()).Replace(0, 1, "g"));
  gEff[1]->SetName(TString(hB->GetName()).Replace(0, 1, "g"));

  gEff[0]->SetTitle(Form("%s;%s;",
        hS->GetTitle(),hS->GetXaxis()->GetTitle()));
  gEff[1]->SetTitle(Form("%s;%s;",
        hB->GetTitle(),hB->GetXaxis()->GetTitle()));

  return gEff;
}
