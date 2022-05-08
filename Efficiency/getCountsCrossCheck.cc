void getCounts()
{
  auto dfLowPt = ROOT::RDataFrame("lambdacAna_mc/genNTuple", "MC_pPb_lambdacAna_mc_AllEntries_Gen.root", {"gen_y", "gen_pT"});
  auto dfLowPtMidY = dfLowPt.Filter("abs(gen_y)<1");
  auto pT3to3p5 = dfLowPtMidY.Filter("gen_pT > 3 && gen_pT < 3.5").Count();
  auto pT3p5to4 = dfLowPtMidY.Filter("gen_pT > 3.5 && gen_pT < 4").Count();
  std::cout << "pT3to3p5," << pT3to3p5.GetValue() << std::endl;
  std::cout << "pT3p5to4," << pT3p5to4.GetValue() << std::endl;
}
