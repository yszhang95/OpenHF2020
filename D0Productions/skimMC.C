void skimMC(std::string inputList)
{
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
  auto dfskim_std = dfskim.Filter("mva>0.56");
  auto dfskim_std_signal = dfskim_std.Filter("isSwap == 0");

  std::map<std::string, std::string> ptstrs;
  std::map<std::string, ROOT::RDF::RResultPtr<::TH1D > > ptrs;
  std::map<std::string, ROOT::RDF::RResultPtr<::TH1D > > ptrs_signal;
  ptstrs["pT2to3"] = "pT>2.0 && pT<3.0";
  ptstrs["pT3to4"] = "pT>3.0 && pT<4.0";
  ptstrs["pT4to5"] = "pT>4.0 && pT<5.0";
  ptstrs["pT5to6"] = "pT>5.0 && pT<6.0";
  ptstrs["pT6to8"] = "pT>6.0 && pT<8.0";
  ptstrs["pT8to10"] = "pT>8.0 && pT<10.0";

  ptrs_signal["pT2to3"] = dfskim_std_signal.Filter(ptstrs["pT2to3"]).Histo1D({("hMCMass_pT2to3"), "pT2to3;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs_signal["pT3to4"] = dfskim_std_signal.Filter(ptstrs["pT3to4"]).Histo1D({("hMCMass_pT3to4"), "pT3to4;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs_signal["pT4to5"] = dfskim_std_signal.Filter(ptstrs["pT4to5"]).Histo1D({("hMCMass_pT4to5"), "pT4to5;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs_signal["pT5to6"] = dfskim_std_signal.Filter(ptstrs["pT5to6"]).Histo1D({("hMCMass_pT5to6"), "pT5to6;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs_signal["pT6to8"] = dfskim_std_signal.Filter(ptstrs["pT6to8"]).Histo1D({("hMCMass_pT6to8"), "pT6to8;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs_signal["pT8to10"] = dfskim_std_signal.Filter(ptstrs["pT8to10"]).Histo1D({("hMCMass_pT8to10"), "pT8to10;Mass (GeV);", 60, 1.7, 2.0}, "mass");

  ptrs["pT2to3"] = dfskim_std.Filter(ptstrs["pT2to3"]).Histo1D({("hMCMassAll_pT2to3"), "pT2to3;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs["pT3to4"] = dfskim_std.Filter(ptstrs["pT3to4"]).Histo1D({("hMCMassAll_pT3to4"), "pT3to4;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs["pT4to5"] = dfskim_std.Filter(ptstrs["pT4to5"]).Histo1D({("hMCMassAll_pT4to5"), "pT4to5;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs["pT5to6"] = dfskim_std.Filter(ptstrs["pT5to6"]).Histo1D({("hMCMassAll_pT5to6"), "pT5to6;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs["pT6to8"] = dfskim_std.Filter(ptstrs["pT6to8"]).Histo1D({("hMCMassAll_pT6to8"), "pT6to8;Mass (GeV);", 60, 1.7, 2.0}, "mass");
  ptrs["pT8to10"] = dfskim_std.Filter(ptstrs["pT8to10"]).Histo1D({("hMCMassAll_pT8to10"), "pT8to10;Mass (GeV);", 60, 1.7, 2.0}, "mass");

  std::map<std::string, ROOT::RDF::RNode> out_tree_ptrs;
  out_tree_ptrs.insert( {"pT2to3",  ROOT::RDF::RNode(dfskim_std.Filter(ptstrs["pT2to3"]))} );
  out_tree_ptrs.insert( {"pT3to4", dfskim_std.Filter(ptstrs["pT3to4"])} );
  out_tree_ptrs.insert( {"pT4to5", dfskim_std.Filter(ptstrs["pT4to5"])} );
  out_tree_ptrs.insert( {"pT5to6", dfskim_std.Filter(ptstrs["pT5to6"])} );
  out_tree_ptrs.insert( {"pT6to8", dfskim_std.Filter(ptstrs["pT6to8"])} );
  out_tree_ptrs.insert( {"pT8to10", dfskim_std.Filter(ptstrs["pT8to10"])} );
  
  std::string inputBaseName = gSystem->BaseName(inputList.c_str());
  auto pos = inputBaseName.find(".list");
  inputBaseName.replace(pos, 5, "");
  TFile ofile(("histsMC_" + inputBaseName + ".root").c_str(), "recreate");
  ofile.cd();
  for (auto& e : ptrs_signal) e.second->Write();
  for (auto& e : ptrs) e.second->Write();
  ofile.Close();

  ROOT::RDF::RSnapshotOptions opts;
  opts.fMode = "update";
  for (auto p : out_tree_ptrs) p.second.Snapshot("D0_"+p.first, ofile.GetName(), ".{1,}", opts);
}
