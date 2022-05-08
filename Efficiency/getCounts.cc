void getCounts(const std::string boost)
{
  const std::string suffix = boost == "pPb" ? "_Gen" : "_Gen_etaFlipped";
  auto dfLowPt = ROOT::RDataFrame("lambdacAna_mc/genNTuple", "MC_" + boost + "_lambdacAna_mc_AllEntries" + suffix + ".root", {"gen_y", "gen_pT"});
  auto dfLowPtMidY = dfLowPt.Filter("abs(gen_y)<1");
  auto pT2to3 = dfLowPtMidY.Filter("gen_pT > 2 && gen_pT < 3").Count();
  auto pT3to4 = dfLowPtMidY.Filter("gen_pT > 3 && gen_pT < 4").Count();
  auto pT4to5 = dfLowPtMidY.Filter("gen_pT > 4 && gen_pT < 5").Count();
  auto pT5to6 = dfLowPtMidY.Filter("gen_pT > 5 && gen_pT < 6").Count();
  std::cout << "pT2to3," << pT2to3.GetValue() << "\n";
  std::cout << "pT3to4," << pT3to4.GetValue() << "\n";
  std::cout << "pT4to5," << pT4to5.GetValue() << "\n";
  std::cout << "pT5to6," << pT5to6.GetValue() << "\n";

  auto dfHighPt = ROOT::RDataFrame("lambdacAna_mc/genNTuple", "MC_" + boost + "_pT5p9_lambdacAna_mc_AllEntries" + suffix + ".root", {"gen_y", "gen_pT"});
  auto dfHighPtMidY = dfHighPt.Filter("abs(gen_y)<1");
  auto pT6to8 = dfHighPtMidY.Filter("gen_pT > 6 && gen_pT < 8").Count();
  auto pT8to10 = dfHighPtMidY.Filter("gen_pT > 8 && gen_pT < 10").Count();
  std::cout << "pT6to8," << pT6to8.GetValue() << "\n";
  std::cout << "pT8to10," << pT8to10.GetValue() << "\n";
}

void getCounts_pPb()
{
  std::cout << "pPb" << "\n";
  getCounts("pPb");
}

void getCounts_Pbp()
{
  std::cout << "Pbp" << "\n";
  getCounts("Pbp");
}
