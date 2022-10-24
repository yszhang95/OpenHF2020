  const double bins[]  = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  const int nbins = sizeof(bins)/sizeof(double) - 1;

void getAccEff(const float mvaCut, TFile& ofile)
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
  ofile.cd();
  // hreco->Write();
  // hgen->Write();
  gEff.Write(::Form("d0AccEff%s", cutStr.c_str()));
}

void getAccEffSeries()
{
  TFile ofile("AccEff_PromptMC_Series.root", "recreate");
  // mva cuts from 0.4 to 0.58 with a steap of 0.02
  const auto mvaMin = 0.4;
  const auto mvaMax = 0.58;
  const auto step = 0.02;
  const auto nMVA = static_cast<int>(std::round((mvaMax - mvaMin)/step));
  for (int iMVA=3; iMVA!=nMVA+1; ++iMVA) {
    const auto mvaCut = step * iMVA + mvaMin;
    getAccEff(mvaCut, ofile);
  }
}
