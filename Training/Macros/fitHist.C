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
  TF1 fMC("fMC", "[0]*([1] * TMath::Gaus(x, [2], [3]*(1+[5]), true) + (1-[1]) * TMath::Gaus(x, [2], [4]*(1+[5]), true))");
  fMC.SetParameter(0, 100);
  fMC.SetParameter(1, 0.5);
  fMC.SetParLimits(1, 0, 1);
  fMC.SetParameter(2, 2.2865);
  fMC.SetParameter(3, 0.005);
  fMC.SetParameter(4, 0.01);
  fMC.FixParameter(5, 0);
  //hMC->Fit(&fMC, "RN", "", 2.24, 2.32);
  //hMC->Fit(&fMC, "RN", "", 2.24, 2.32);
  hMC->Fit(&fMC, "RN", "", 2.2, 2.38);
  hMC->Fit(&fMC, "RN", "", 2.2, 2.38);
  //gROOT->ForceStyle();
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

   for (int i=0; i != 5; ++i) {
     f->SetParameter(i, fMC.GetParameter(i));
   }
   f->FixParameter(1, fMC.GetParameter(1));
   f->FixParameter(3, fMC.GetParameter(3));
   f->FixParameter(4, fMC.GetParameter(4));
   f->SetParameter(5, 0);
   f->SetParLimits(5, -1, 1);
   h->Fit(f.get(), "R", "", lw, up);
   h->Fit(f.get(), "R", "", lw, up);

   h->Fit(f.get(), "RM", "", lw, up);
   h->Fit(f.get(), "RME", "", lw, up);

   h->Draw("E");

   c->Draw();

   c->cd();
   f->Draw("SAME");
   fbkg->SetLineColor(kBlue);
   fbkg->SetLineStyle(2);
   fbkg->Draw("SAME");
   c->Modified();
   c->Update();
   c->Draw();
   TString outputName = c->GetName();
   c->Print(outputName+ "_cutbase.pdf");

   vector<double> output;
   std::copy(f->GetParameters(),
             f->GetParameters()+f->GetNpar(),
             std::back_inserter(output));
   std::copy(f->GetParErrors(),
             f->GetParErrors()+f->GetNpar(),
             std::back_inserter(output));
   return output;
}

std::vector<double> simpleFit(TH1* hData, TH1* hMC,
    const double pTMin, const double pTMax)
{
  gStyle->SetOptStat(0);
  //gStyle->SetPadTickX(1);
  //gStyle->SetPadTickY(1);

  TString output = Form("mass_pT%.1fto%.1f", pTMin, pTMax);
  for (auto pos = output.Index("."); pos>=0; ) {
    output.Replace(pos, 1, "p");
    pos = output.Index(".");
  }
  TCanvas c(output.Data(), "", 600, 450);
  c.SetTopMargin(0.07);
  c.SetLeftMargin(0.13);
  c.SetBottomMargin(0.14);
  c.SetRightMargin(0.03);

  TGaxis::SetMaxDigits(5);
  TGaxis::SetExponentOffset(-0.05);

  hData->SetTitle(Form(";M_{pK^{0}_{s}} (GeV);Events / %.1f MeV", 1000*hData->GetBinWidth(1)));
  hData->GetXaxis()->SetTitleSize(0.042);
  hData->GetYaxis()->SetTitleSize(0.042);
  hData->GetXaxis()->SetTitleOffset(1.2);
  hData->GetXaxis()->CenterTitle();
  hData->GetYaxis()->CenterTitle();
  hData->SetMarkerStyle(20);
  hData->SetMarkerSize(1.0);
  hData->SetLineColor(kBlack);
  hData->GetXaxis()->SetRangeUser(2.18, 2.4);
  //hData->SetMaximum(hData->GetMaximum() * 1.05);

  const double pars[] = {4000, -500, 200, 10};
  simpleFit(hMC, hData, pars, 2.18, 2.4, &c);

  auto f = (TF1*)hData->GetListOfFunctions()->FindObject("f")->Clone("fTotal");
  hData->GetListOfFunctions()->FindObject("f")->Delete();
  TF1 fbkg ("fbkg", "cheb3", 2.18, 2.4);
  fbkg.SetParameter(0, f->GetParameter(6));
  fbkg.SetParameter(1, f->GetParameter(7));
  fbkg.SetParameter(2, f->GetParameter(8));
  fbkg.SetParameter(3, f->GetParameter(9));

  const auto frac1 = f->GetParameter(1);
  const auto sigma1 = f->GetParameter(3);
  const auto sigma2 = f->GetParameter(4);
  const auto sigma = sqrt(frac1 * sigma1 * sigma1 + (1-frac1) * sigma2 * sigma2);
  const auto yieldsBkg = fbkg.Integral(2.2865-sigma*2.6, 2.2865+sigma*2.6)/ hData->GetBinWidth(1);
  const auto yieldsSignal = f->GetParameter(0)/ hData->GetBinWidth(1);
  std::cout << "After special cuts" << std::endl;
  std::cout << "Signal: " << yieldsSignal << std::endl;
  std::cout << "Background: " << yieldsBkg << std::endl;

  std::vector<double> yields;
  yields.push_back(yieldsSignal);
  yields.push_back(yieldsBkg);

  return yields;
}
