void avg(std::string trig, const std::string type="Nonprompt")
{
  TFile ofile(::Form("avg_eff_%s_NP.root", trig.c_str()), "recreate");
  TGraphErrors* gHM;
  TGraphErrors* gMB;
  std::map<std::string, std::vector<double>> lumis;
  lumis["HM"] = {28917.856684357, 68876.167756928}; // ub
  lumis["MB"] = {1113.004725970, 3103.762734886}; // ub
  TFile fin("eff_NP.root");
  std::map<std::string, TGraphAsymmErrors*> graphs_pPb;
  std::map<std::string, TGraphAsymmErrors*> graphs_Pbp;
  graphs_pPb[trig] = fin.Get<TGraphAsymmErrors>(::Form("AccEff_pPb_%s", trig.c_str()));
  graphs_Pbp[trig] = fin.Get<TGraphAsymmErrors>(::Form("AccEff_Pbp_%s", trig.c_str()));
  

  TGraphAsymmErrors g(graphs_pPb.at(trig)->GetN());
  for (int ip=0; ip<g.GetN(); ++ip) {
      double x = graphs_pPb.at(trig)->GetX()[ip];
      // double xerr = graphs_pPb.at(trig)->GetEX()[ip];
      double y_pPb = graphs_pPb.at(trig)->GetY()[ip];
      double y_Pbp = graphs_Pbp.at(trig)->GetY()[ip];
      // missing
      // yerr
      const double y = (y_pPb*lumis.at(trig)[0]+y_Pbp*lumis.at(trig)[1])/(lumis.at(trig)[0]+lumis.at(trig)[1]);
      g.SetPoint(ip, x, y);

  }
  ofile.cd();
  g.Write(::Form("AvgAccEff"));
}

void avgEff()
{
  avg("HM");
  avg("MB");
}
