  const double bins[]  = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  const int nbins = sizeof(bins)/sizeof(double) - 1;

void getAccEff(const float mvaCut=0.56)
{
  const double ycut = 1;
  std::string path = "/storage1/users/yz144/HIN-21-016/D0Trees/PromptD_MC.root";
  auto df = ROOT::RDataFrame("d0ana_mc_newreduced/VertexCompositeNtuple", path,
      {"mass", "pT", "mva", "y", "pTD1", "pTD2", "EtaD1", "EtaD2", "pTerrD1", "pTerrD2", "3DPointingAngle", "2DPointingAngle"});
  auto dfskim = df
                  .Filter("matchGEN == 1")
                  .Define("angle3D", [](float x)->float { return std::abs(x); }, {"3DPointingAngle"}).Filter("angle3D<1")
                  .Define("angle2D", [](float x)->float { return std::abs(x); }, {"2DPointingAngle"}).Filter("angle2D<1")
                  .Define("pTerrRelD1", [](float x, float y)->float { return x/y; }, {"pTerrD1", "pTD1"}).Filter("pTerrRelD1<0.1")
                  .Define("pTerrRelD2", [](float x, float y)->float { return x/y; }, {"pTerrD2", "pTD2"}).Filter("pTerrRelD2<0.1")
                  .Filter("abs(EtaD1-EtaD2)<1.0").Filter("(pTD1+pTD2)>1.6")
                  .Filter(::Form("abs(y)<%f", ycut));
  auto dfskim_std = dfskim.Filter(::Form("mva > %f", mvaCut));
  auto dfskim_std_signal = dfskim_std.Filter("isSwap == 0");

  auto hreco = dfskim_std_signal.Histo1D({"reco_std", "reco_std;pT;", nbins, bins}, "pT");

  auto dfGen = ROOT::RDataFrame("d0ana_mc_newreduced/genCandidateNtuple", path);
  auto dfGenSkim = dfGen.Filter("(abs(DauID1_gen) == 321 &&  abs(DauID2_gen) == 211) || (abs(DauID2_gen) == 321 &&  abs(DauID1_gen) == 211)")
     .Filter(::Form("abs(y_gen)<%f", ycut));

  auto hgen = dfGenSkim.Histo1D({"gen", "gen;pT;", nbins, bins}, "pT_gen");

  TGraphAsymmErrors gEff(hreco.GetPtr());
  gEff.Divide(hreco.GetPtr(), hgen.GetPtr());

  std::string cutStr = std::to_string(mvaCut);
  cutStr.replace(1, 1, "p");
  // TFile ofile("AccEff_PromptMC.root", "recreate");
  TFile ofile(::Form("AccEff_PromptMC_mva%s.root", cutStr.c_str()), "recreate");
  ofile.cd();
  hreco->Write();
  hgen->Write();
  gEff.Write("d0AccEff");
}

/*
void getEff()
{
  const double ycut = 1;
  std::string path = "/storage1/users/yz144/HIN-21-016/D0Trees/PromptD_MC.root";
  auto df = ROOT::RDataFrame("d0ana_mc_newreduced/VertexCompositeNtuple", path,
      {"mass", "pT", "mva", "y", "pTD1", "pTD2", "EtaD1", "EtaD2", "pTerrD1", "pTerrD2", "3DPointingAngle", "2DPointingAngle"});
  auto dfskim = df
                  .Filter("matchGEN == 1")
                  .Define("angle3D", [](float x)->float { return std::abs(x); }, {"3DPointingAngle"}).Filter("angle3D<1")
                  .Define("angle2D", [](float x)->float { return std::abs(x); }, {"2DPointingAngle"}).Filter("angle2D<1")
                  .Define("pTerrRelD1", [](float x, float y)->float { return x/y; }, {"pTerrD1", "pTD1"}).Filter("pTerrRelD1<0.1")
                  .Define("pTerrRelD2", [](float x, float y)->float { return x/y; }, {"pTerrD2", "pTD2"}).Filter("pTerrRelD2<0.1")
                  .Filter("abs(EtaD1-EtaD2)<1.0").Filter("(pTD1+pTD2)>1.6")
                  .Filter(::Form("abs(y)<%f", ycut));
  auto dfskim_std = dfskim.Filter("mva > 0.56");
  auto dfskim_std_signal = dfskim_std.Filter("isSwap == 0");

  const double bins[]  = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0};
  const int nbins = sizeof(bins)/sizeof(double) - 1;

  auto hreco = dfskim_std_signal.Histo1D({"reco_std", "reco_std;pT;", nbins, bins}, "pT");

  auto dfGen = ROOT::RDataFrame("d0ana_mc_newreduced/genCandidateNtuple", fileNames);
  auto dfGenSkim = dfGen.Filter("(abs(DauID1_gen) == 321 &&  abs(DauID2_gen) == 211) || (abs(DauID2_gen) == 321 &&  abs(DauID1_gen) == 211)")
     .Filter(::Form("abs(y_gen)<%f", ycut));

  auto hgen = dfGenSkim.Histo1D({"gen", "gen;pT;", nbins, bins}, "pT_gen");

  TGraphAsymmErrors gEff(hreco.GetPtr());
  gEff.Divide(hreco.GetPtr(), hgen.GetPtr());

  TFile ofile("Eff_PromptMC.root", "recreate");
  ofile.cd();
  hreco->Write();
  hgen->Write();
  // gEff.Write("d0eff");
}
*/

/*
void getAcc()
{
  const double ycut = 1;
  std::string path = "/storage1/users/yz144/HIN-21-016/D0Trees/PromptD_MC.root";

  const double bins[]  = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 10.0};
  const int nbins = sizeof(bins)/sizeof(double) - 1;

  auto hreco = dfskim_std_signal.Histo1D({"reco_std", "reco_std;pT;", nbins, bins}, "pT");

  auto dfGen = ROOT::RDataFrame("d0ana_mc_newreduced/genCandidateNtuple", fileNames);
  auto dfGenSkim = dfGen.Filter("(abs(DauID1_gen) == 321 &&  abs(DauID2_gen) == 211) || (abs(DauID2_gen) == 321 &&  abs(DauID1_gen) == 211)")
     .Filter(::Form("abs(y_gen)<%f", ycut));

  auto dfGenSkimPass = dfGenSkim.Filter("abs()");

  auto hgen = dfGenSkim.Histo1D({"gen_all", "gen;pT;", nbins, bins}, "pT_gen");
  auto hpass = dfGenSkim.Histo1D({"gen_pass", "gen;pT;", nbins, bins}, "pT_gen");

  TGraphAsymmErrors gEff(hpass.GetPtr());
  gEff.Divide(hpass.GetPtr(), hgen.GetPtr());

  TFile ofile("Acc_PromptMC.root", "recreate");
  ofile.cd();
  hreco->Write();
  hgen->Write();
  gEff.Write("d0Acc");
}
*/
