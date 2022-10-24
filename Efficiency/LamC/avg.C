void avg(std::string trig)
{
  TFile ofile(::Form("avg_eff_%s.root", trig.c_str()), "recreate");
  TGraphErrors* gHM;
  TGraphErrors* gMB;
  std::map<std::string, std::vector<double>> lumis;
  lumis["HM"] = {28917.856684357, 68876.167756928}; // ub
  lumis["MB"] = {1113.004725970, 3103.762734886}; // ub
  TFile fgen_pPb("gen_pPb.root");
  TFile fgen_Pbp("gen_Pbp.root");
  TFile freco_pPb(::Form("reco_%s_pPb.root", trig.c_str()));
  TFile freco_Pbp(::Form("reco_%s_Pbp.root", trig.c_str()));
  TH1D* hAll_pPb = fgen_pPb.Get<TH1D>("hAll");
  TH1D* hAll_Pbp = fgen_Pbp.Get<TH1D>("hAll");
  TH1D* hpass_pPb = freco_pPb.Get<TH1D>("hpass");
  TH1D* hpass_Pbp = freco_Pbp.Get<TH1D>("hpass");
  
  hAll_pPb->Sumw2();
  hAll_pPb->Scale(lumis.at(trig)[0]);
  hAll_Pbp->Sumw2();
  hAll_Pbp->Scale(lumis.at(trig)[1]);

  hpass_pPb->Sumw2();
  hpass_pPb->Scale(lumis.at(trig)[0]);
  hpass_Pbp->Sumw2();
  hpass_Pbp->Scale(lumis.at(trig)[1]);

  hAll_pPb->Add(hAll_Pbp);
  hpass_pPb->Add(hpass_Pbp);

  TGraphAsymmErrors g;
  g.Divide(hpass_pPb, hAll_pPb);
  ofile.cd();
  g.Write(::Form("AvgAccEff"));
}

void avg()
{
  avg("HM");
  avg("MB");
}
