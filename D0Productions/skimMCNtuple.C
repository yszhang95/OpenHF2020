void skimMCNtuple(std::string inputList)
{
  TStopwatch ts;
  ts.Start();
  ROOT::EnableImplicitMT();
  std::ifstream infiletxt(inputList.c_str());
  std::string line;
  std::vector<std::string> fileNames;
  while (std::getline(infiletxt, line)) {
    if (line.at(0) == '#') continue;
    fileNames.push_back(line);
  }
  fileNames.shrink_to_fit();
  auto df = ROOT::RDataFrame("d0ana_mc_newreduced/VertexCompositeNtuple", fileNames,
      {"mass", "pT", "mva", "y", "pTD1", "pTD2", "EtaD1", "EtaD2", "pTerrD1", "pTerrD2", "3DPointingAngle", "2DPointingAngle"});
  auto dfskim = df
                  .Filter("matchGEN == 1")
                  .Define("angle3D", [](float x)->float { return std::abs(x); }, {"3DPointingAngle"}).Filter("angle3D<1")
                  .Define("angle2D", [](float x)->float { return std::abs(x); }, {"2DPointingAngle"}).Filter("angle2D<1")
                  .Define("pTerrRelD1", [](float x, float y)->float { return x/y; }, {"pTerrD1", "pTD1"}).Filter("pTerrRelD1<0.1")
                  .Define("pTerrRelD2", [](float x, float y)->float { return x/y; }, {"pTerrD2", "pTD2"}).Filter("pTerrRelD2<0.1")
                  .Filter("abs(EtaD1-EtaD2)<1.0").Filter("(pTD1+pTD2)>1.2")
                  .Filter("abs(y)<1");

  std::map<std::string, std::string> ptstrs;

  
  std::string inputBaseName = gSystem->BaseName(inputList.c_str());
  auto pos = inputBaseName.find(".list");
  inputBaseName.replace(pos, 5, "");
  std::string output = "D0_MC_Ntuple_" + inputBaseName + ".root";

  dfskim.Snapshot("D0NTuple", output);

  ts.Stop();
  ts.Print();
}
