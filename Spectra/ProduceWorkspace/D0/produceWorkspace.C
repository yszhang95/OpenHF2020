using namespace RooFit;
void prepareWS(const FitParConfigs configs, const VarCuts& mycuts,
               const std::string treePt)
{
  const auto inputConfigs = configs.getInputConfigs();

  const std::string mvaName = mycuts._mvaName;
  const std::string mvaStr = mycuts.getMva();
  const std::string kinStr = mycuts.getKinematics();
  const std::string eventStr = mycuts.getEventFilter();
  const std::string ntrkStr = mycuts.getNtrkoffline();

  auto names = ::getNames(configs, mycuts);
  const auto wsName = names.at("wsName");
  const auto dsName = names.at("dsName");
  const std::string ofileName = names.at("fileName");

  std::string fileMC = inputConfigs.getPaths("PromptDMC").at(0);
  std::string fileD0toKK = inputConfigs.getPaths("KK").at(0);
  std::string fileD0toPiPi = inputConfigs.getPaths("PiPi").at(0);

  RooRealVar Ntrkoffline("Ntrkoffline", "Ntrkoffline", -1, 2000);
  RooRealVar mass("mass", "mass", 1.725, 2.0);
  RooRealVar pT("pT", "pT", 0, 100);
  RooRealVar y("y", "rapidity", -2.4, 2.4);
  RooRealVar matchGEN("matchGEN", "matchGEN", -1, 2);
  RooRealVar isSwap("isSwap", "isSwap", -1, 2);
  RooRealVar dz1p0("dz1p0", "dz1p0", -1, 2);
  RooRealVar gplus("gplus", "gplus", -1, 2);
  RooRealVar mva("mva", "mva", -1, 1);

  RooRealVar wgtVar("weight", "weight from HM trigger", -0.1, 1.5);

  auto argSetsMC = RooArgSet(mass, pT, y, matchGEN, isSwap, mva);

  mass.setRange("full", 1.725, 2.0);
  mass.setRange("signal", 1.725, 2.0);
  mass.setRange("KK", 1.725, 1.82);
  mass.setRange("PiPi", 1.88, 2.0);

  TFile ofile(ofileName.c_str(), "recreate");
  RooWorkspace myws(wsName.c_str(), "");
  RooAbsData::setDefaultStorageType(RooAbsData::Tree);

  // I take cuts as weights because
  // if pass cuts, weight = 1, otherwise weight = 0
  std::string cuts = string_format("pT > %.1f && pT < %.1f", mycuts._pTMin, mycuts._pTMax);

  if (mycuts._usedz1p0) cuts += " && dz1p0 > 0.5";
  else if (mycuts._usegplus) cuts += " && gplus > 0.5";

  cuts += ::string_format(" && abs(y)< %f", mycuts._yAbsMax);
  cuts += ::string_format(" && mva > %f", mycuts._mvaCut);
  cuts += ::string_format(" && Ntrkoffline > %d && Ntrkoffline < %d", mycuts._NtrkofflineMin, mycuts._NtrkofflineMax);

  // signal

  const auto common_cuts = ::string_format("pT>%.3f && pT<%.3f && abs(y)<%.3f",
                                           mycuts._pTMin, mycuts._pTMax, mycuts._yAbsMax);
  const auto cuts_true = ::string_format("%s && isSwap<0.5 && mva>%.3f",
                                         common_cuts.c_str(), mycuts._mvaCut);
  const auto MClabel = "_" + kinStr + "_" + mvaStr;
  const std::string dsMCSignalName = names.at("dsMCSignalName");
  RooDataSet dsMCSignal(dsMCSignalName.c_str(), "true D0 samples", argSetsMC,
                        ImportFromFile(fileMC.c_str(), treePt.c_str()),
                        Cut(cuts_true.c_str()));
  const auto cuts_swap = ::string_format("%s && isSwap>0.5 && mva>%.3f",
                                         common_cuts.c_str(), mycuts._mvaCut);
  const std::string dsMCSwapName = names.at("dsMCSwapName");
  RooDataSet dsMCSwap(dsMCSwapName.c_str(), "swap D0 samples", argSetsMC,
                      ImportFromFile(fileMC.c_str(), treePt.c_str()),
                      Cut(cuts_swap.c_str()));
  const auto cuts_inc = ::string_format("%s && mva>%.3f", common_cuts.c_str(), mycuts._mvaCut);
  const std::string dsMCAllName = names.at("dsMCAllName");
  RooDataSet dsMCAll(dsMCAllName.c_str(), "true+swap D0 samples", argSetsMC,
                     ImportFromFile(fileMC.c_str(), treePt.c_str()),
                     Cut(cuts_inc.c_str()));
  const std::string dsMCKKName = names.at("dsMCKKName");
  RooDataSet dsMCKK(dsMCKKName.c_str(), "MC D0->K+K0 samples", argSetsMC,
                    ImportFromFile(fileD0toKK.c_str(), treePt.c_str()),
                    Cut(cuts_inc.c_str()));
  const std::string dsMCPiPiName = names.at("dsMCPiPiName");
  RooDataSet dsMCPiPi(dsMCPiPiName.c_str(), "MC D0->K+K0 samples", argSetsMC,
                      ImportFromFile(fileD0toPiPi.c_str(), treePt.c_str()),
                      Cut(cuts_inc.c_str()));
  myws.import(dsMCSignal);
  myws.import(dsMCSwap);
  myws.import(dsMCAll);
  myws.import(dsMCKK);
  myws.import(dsMCPiPi);

  // data
  TChain chain(inputConfigs.getName("Data").c_str(), "D0NTuple");
  const auto files = inputConfigs.getPaths("Data");
  for (const auto & file : files) {
    const auto it = file.find(".list");
    if (it != std::string::npos) {
      TFileCollection tf("tf", "", file.c_str());
      chain.AddFileInfoList(tf.GetList());
    } else {
      chain.Add(file.c_str());
    }
  }
  // cuts as weight
  auto argSetsData = RooArgSet(mass, mva, pT, y, dz1p0, gplus, Ntrkoffline);
  // need to correct for this
  if (mycuts._useWeight) argSetsData.add(wgtVar);
  const char* wgtVarName = mycuts._useWeight ? "weight" : 0;
  RooDataSet dsData(dsName.c_str(), treePt.c_str(), &chain, argSetsData,
                    cuts.c_str(), wgtVarName);
  myws.import(dsData);
  ofile.cd();
  myws.Write();
}

void produceEachMult(int NtrkMin, int NtrkMax, std::string configName)
{
  FitParConfigs configs(configName);
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  auto par = configs.getParConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  // modify it later
  VarCuts mycuts(cutConfigs);

  // for loop
  const int nPt = 8;
  const float pts[nPt+1] =   {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
  const float treeMin[nPt] = {2.0, 3.0, 4.0, 5.0, 6.0, 6.0, 8.0, 8.0};
  const float treeMax[nPt] = {3.0, 4.0, 5.0, 6.0, 8.0, 8.0, 10., 10.};

  for (int ipt=0; ipt<nPt; ++ipt) {
    mycuts._pTMin = pts[ipt];
    mycuts._pTMax = pts[ipt+1];
    mycuts._NtrkofflineMin = NtrkMin;
    mycuts._NtrkofflineMax = NtrkMax;
    const auto treePt = ::Form("D0_pT%.0fto%.0f", treeMin[ipt], treeMax[ipt]);
    prepareWS(configs, mycuts, treePt);
  }
}

void produceWorkspace()
{
  produceEachMult(185, 250, "D0wKKPiPi_HM_std.txt");
  const int Ntrks[] = {0, 35, 60, 90, 120, 185};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;
  for (unsigned int ibin=0; ibin<nbins; ++ibin) {
    produceEachMult(Ntrks[ibin], Ntrks[ibin+1], "D0wKKPiPi_MB_std.txt");
  }
}
