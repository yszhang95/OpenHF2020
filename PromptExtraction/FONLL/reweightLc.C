void reweightLc()
{
  auto evtgen = TFile::Open("EvtGen.root");
  auto fonll = TFile::Open("fonll.root");
  TGraphAsymmErrors* gfonll;
  TH2D* hLcVsLb;
  TH1D* hLbPt;
  fonll->GetObject("btoLamBtoLamC_dNOverdLamBPt;1", gfonll);
  evtgen->GetObject("hLcVsLb", hLcVsLb);
  evtgen->GetObject("hLbPt", hLbPt);

  hLcVsLb->Sumw2(kTRUE);
  hLbPt->Sumw2(kTRUE);


  std::vector<double> ratios;
  std::vector<double> ratiosUp;
  std::vector<double> ratiosLw;
  for (int ibin=0; ibin<hLbPt->GetNbinsX(); ++ibin) {
    // c2/c1
    auto c1 = hLbPt->GetBinContent(ibin+1);
    auto w = hLbPt->GetBinWidth(ibin+1);
    auto c2 = gfonll->GetY()[ibin];
    auto up = gfonll->GetEYhigh()[ibin];
    auto lw = gfonll->GetEYlow()[ibin];
    auto ratio = c2*w/c1;
    auto ratioUp = (c2+up) * w /c1;
    auto ratioLw = (c2-lw) * w /c1;
    //cout << c1 << "\t" << w << "\t" << c2  << "\t" << ratio << endl;
    ratios.push_back(ratio);
    ratiosUp.push_back(ratioUp);
    ratiosLw.push_back(ratioLw);
  }


  auto hscale2D = (TH2D*) hLcVsLb->Clone("hLcVsLbScaled");
  for (int xbin=0; xbin<hLcVsLb->GetNbinsX()+2; ++xbin) {
    hscale2D->SetBinContent(xbin, 0, 0); // underflow
    hscale2D->SetBinContent(xbin, hLcVsLb->GetNbinsY()+1, 0); // overflow 
    hscale2D->SetBinError(xbin, 0, 0); // underflow
    hscale2D->SetBinError(xbin, hLcVsLb->GetNbinsY()+1, 0); // overflow 
  }

  auto hscale2DUp = (TH2D*) hscale2D->Clone("hLcVsLbScaledUp");
  auto hscale2DLw = (TH2D*) hscale2D->Clone("hLcVsLbScaledLw");

  auto h = hscale2D->ProjectionY("hLcGen");
  auto hUp = hscale2DUp->ProjectionY("hLcGenUp");
  auto hLw = hscale2DLw->ProjectionY("hLcGenLw");

  for (int xbin=0; xbin<hLcVsLb->GetNbinsX(); ++xbin) {
    const double ratio = ratios.at(xbin);
    const double ratioUp = ratiosUp.at(xbin);
    const double ratioLw = ratiosLw.at(xbin);
    cout << "ratio " << ratio << endl;
    double mysum=0;
    double mysumErr=0;
    double mysumUp=0;
    double mysumErrUp=0;
    double mysumLw=0;
    double mysumErrLw=0;
    for (int ybin=0; ybin<hLcVsLb->GetNbinsY(); ++ybin) {
      //cout << hLcVsLb->GetBinContent(xbin+1, ybin+1) << endl;
      const auto mycontent = hLcVsLb->GetBinContent(xbin+1, ybin+1) * ratio;
      //cout << mycontent << endl;
      const auto mycontenterr = hLcVsLb->GetBinError(xbin+1, ybin+1) * ratio;
      hscale2D->SetBinContent(xbin+1, ybin+1, mycontent);
      hscale2D->SetBinError(xbin+1, ybin+1, mycontenterr);
      mysum += mycontent;

      const auto mycontentUp = hLcVsLb->GetBinContent(xbin+1, ybin+1) * ratioUp;
      const auto mycontenterrUp = hLcVsLb->GetBinError(xbin+1, ybin+1) * ratioUp;
      hscale2DUp->SetBinContent(xbin+1, ybin+1, mycontentUp);
      hscale2DUp->SetBinError(xbin+1, ybin+1, mycontenterrUp);

      const auto mycontentLw = hLcVsLb->GetBinContent(xbin+1, ybin+1) * ratioLw;
      const auto mycontenterrLw = hLcVsLb->GetBinError(xbin+1, ybin+1) * ratioLw;
      hscale2DLw->SetBinContent(xbin+1, ybin+1, mycontentLw);
      hscale2DLw->SetBinError(xbin+1, ybin+1, mycontenterrLw);
    }
    //cout << mysum << endl;
  }
  for (int ybin=0; ybin<hLcVsLb->GetNbinsY(); ++ybin) {
    double mysum=0;
    double mysumErr=0;
    double mysumUp=0;
    double mysumErrUp=0;
    double mysumLw=0;
    double mysumErrLw=0;
    for (int xbin=0; xbin<hLcVsLb->GetNbinsX(); ++xbin) {
      const auto mycontent = hscale2D->GetBinContent(xbin+1, ybin+1);
      const auto mycontenterr = hscale2D->GetBinError(xbin+1, ybin+1);
      mysum += mycontent;
      mysumErr += mycontenterr*mycontenterr;

      const auto mycontentUp = hscale2DUp->GetBinContent(xbin+1, ybin+1);
      const auto mycontenterrUp = hscale2DUp->GetBinError(xbin+1, ybin+1);
      mysumUp += mycontentUp;
      mysumErrUp += mycontenterrUp*mycontenterrUp;

      const auto mycontentLw = hscale2DLw->GetBinContent(xbin+1, ybin+1);
      const auto mycontenterrLw = hscale2DLw->GetBinError(xbin+1, ybin+1);
      mysumLw += mycontentLw;
      mysumErrLw += mycontenterrLw*mycontenterrLw;
    }
    mysumErr = sqrt(mysumErr);
    mysumErrUp = sqrt(mysumErrUp);
    mysumErrLw = sqrt(mysumErrLw);
    //cout << mysumErr << endl;
    //cout << mysum << endl;
    h->SetBinContent(ybin+1, mysum);
    h->SetBinError(ybin+1, mysumErr);
    hUp->SetBinContent(ybin+1, mysumUp);
    hUp->SetBinError(ybin+1, mysumErrUp);
    hLw->SetBinContent(ybin+1, mysumLw);
    hLw->SetBinError(ybin+1, mysumErrLw);
  }
  TFile ofile("scaledgen.root", "recreate");
  h->Write();
  hUp->Write();
  hLw->Write();
}

