void printQuality()
{
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;

  TFile f2("quality_order2.root");
  TFile f3("quality_order3.root");
  TFile f4("quality_order4.root");

  std::map<std::string, TH1D*> chi2;
  std::map<std::string, TH1D*> minNLL;
  for (int ibin=0; ibin<nbins; ++ibin) {
    chi2["order2"] = f2.Get<TH1D>(::Form("Chi2Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    minNLL["order2"] = f2.Get<TH1D>(::Form("MinNLLNtrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    chi2["order3"] = f3.Get<TH1D>(::Form("Chi2Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    minNLL["order3"] = f3.Get<TH1D>(::Form("MinNLLNtrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    chi2["order4"] = f4.Get<TH1D>(::Form("Chi2Ntrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    minNLL["order4"] = f4.Get<TH1D>(::Form("MinNLLNtrk%dto%d", Ntrks[ibin], Ntrks[ibin+1]));
    std::map<std::string, double> chi2Val;
    std::map<std::string, double> minNLLVal;
    std::map<std::string, double> LLR;
    std::cout << "Ntrk^offline: " << Ntrks[ibin] << " -- " << Ntrks[ibin+1] << "\n";
    std::cout << "p_T (GeV),chi2(2),chi2(3),chi2(4),2(minNLL(2)-minNLL(3)),2(minNLL(3)-minNLL(4)),Prob(2,3),Prob(3,4)\n";
    for (int ipt=0; ipt<nPt; ++ipt) {
      if (ibin == nbins-1 && ipt==0) continue;
      for (int i=2; i<=4; ++i) {
        chi2Val["order" + to_string(i)] = chi2["order" + to_string(i)]->GetBinContent(ipt+1);
        minNLLVal["order" + to_string(i)] = minNLL["order" + to_string(i)]->GetBinContent(ipt+1);
      }
      LLR["2-3"] = 2 * (minNLLVal["order2"] - minNLLVal["order3"]);
      LLR["3-4"] = 2 * (minNLLVal["order3"] - minNLLVal["order4"]);
      std::cout << pts[ipt] << "--" << pts[ipt+1] << ","
                << chi2Val["order2"] << ","
                << chi2Val["order3"] << ","
                << chi2Val["order4"] << ","
                << LLR["2-3"] << ","
                << LLR["3-4"] << ","
                << TMath::Prob(LLR["2-3"], 1) << ","
                << TMath::Prob(LLR["3-4"], 1) << "\n";

    }
  }

}
