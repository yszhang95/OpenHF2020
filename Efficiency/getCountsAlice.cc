void getCountsAlice()
{
  auto dfLowPt = ROOT::RDataFrame("lambdacAna_mc/genNTuple", "MC_pPb_lambdacAna_mc_AllEntries_Gen.root", {"gen_y", "gen_pT"});
  auto dfLowPtMidY = dfLowPt.Filter("abs(gen_y)<0.5");
  auto pT2to3 = dfLowPtMidY.Filter("gen_pT > 2 && gen_pT < 3").Count();
  auto pT3to4 = dfLowPtMidY.Filter("gen_pT > 3 && gen_pT < 4").Count();
  auto pT4to5 = dfLowPtMidY.Filter("gen_pT > 4 && gen_pT < 5").Count();
  auto pT5to6 = dfLowPtMidY.Filter("gen_pT > 5 && gen_pT < 6").Count();
  std::cout << "pT2to3," << pT2to3.GetValue() << std::endl;
  std::cout << "pT3to4," << pT3to4.GetValue() << std::endl;
  std::cout << "pT4to5," << pT4to5.GetValue() << std::endl;
  std::cout << "pT5to6," << pT5to6.GetValue() << std::endl;

  auto dfHighPt = ROOT::RDataFrame("lambdacAna_mc/genNTuple", "MC_pPb_pT5p9_lambdacAna_mc_AllEntries_Gen.root", {"gen_y", "gen_pT"});
  auto dfHighPtMidY = dfHighPt.Filter("abs(gen_y)<0.5");
  auto pT6to8 = dfHighPtMidY.Filter("gen_pT > 6 && gen_pT < 8").Count();
  auto pT8to10 = dfHighPtMidY.Filter("gen_pT > 8 && gen_pT < 10").Count();
  std::cout << "pT6to8," << pT6to8.GetValue() << std::endl;
  std::cout << "pT8to10," << pT8to10.GetValue() << std::endl;
}
