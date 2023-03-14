using namespace ROOT;
void check_corr()
{
  EnableImplicitMT();
  std::ifstream input("MC_PromptD0_pPb.txt");
  std::vector<std::string > file_names;
  for (std::string l; std::getline(input, l); ) {
    file_names.push_back(l);
  }
  RDataFrame df("d0anaMCNoCut/VertexCompositeNtuple", file_names);
  // std::cout << *df.Count() << "\n";
  // std::cout << *df.Filter("matchGEN").Count() << "\n";
  auto h2pTD1 = df.Filter("abs(y)<1")
    .Histo2D({"hpTD1VsDpT", "gen-matched reconstructed D0;p_{T}^{D^{0}};p_{T}^{D1}", 200, 0, 10, 200, 0, 10}, "pT", "pTD1");
  std::cout << h2pTD1->GetCorrelationFactor() << "\n";
  auto h2EtaD1 = df.Filter("abs(y)<1")
    .Histo2D({"hEtaD1VsDEta", "gen-matched reconstructed D0;#{eta}^{D^{0}};#{eta}^{D1}", 200, 0, 10, 200, 0, 10}, "eta", "EtaD1");
  std::cout << h2EtaD1->GetCorrelationFactor() << "\n";
}
