double countEvents(std::string fileName);
double countEvents(std::string configName);

double countEvents(std::string fileName, const VarCuts& mycuts)
{
  TFile fin(fileName.c_str());
  std::string histName = "lambdacAna/hNtrkoffline";
  if (mycuts._usedz1p0) histName += "Dz1p0";
  if (mycuts._usegplus) histName += "Gplus";
  if (!mycuts._useWeight) histName += "Unweight";
  auto hNtrkoffline =  (TH1D*)fin.Get(histName.c_str());
  int NtrkBinMin = mycuts._NtrkofflineMin + 1;
  int NtrkBinMax = mycuts._NtrkofflineMax;
  double nEvents = hNtrkoffline->Integral(NtrkBinMin, NtrkBinMax);
  return nEvents;
}

double countEvents(std::string configName)
{
  FitParConfigs configs(configName.c_str());
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  double nEvents = 0;
  const auto files = inputConfigs.getPaths("Data");
  for (const auto & file : files) {
    const auto it = file.find(".list");
    if (it != std::string::npos) {
      cout << "Undefined behaviour of the files with extensions .list\n";
      return -1;
    } else {
      nEvents += countEvents(file, mycuts);
    }
  }
  return nEvents;
}

void getFit(const std::string configName, double& yields, double& err)
{
  FitParConfigs configs(configName.c_str());
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  const VarCuts mycuts(cutConfigs);

  const std::string mvaName = mycuts._mvaName;
  const std::string mvaStr = mycuts.getMva();
  const std::string kinStr = mycuts.getKinematics();
  const std::string eventStr = mycuts.getEventFilter();
  const std::string ntrkStr = mycuts.getNtrkoffline();

  const std::string label = kinStr + "_" + mvaStr + "_" + eventStr + "_" + ntrkStr;
  const std::string wsName = "ws_" + label;
  const std::string dsName = "ds_" + label;
  std::string fileName;
  try {
    fileName = outputConfigs.getPath("outdir");
  } catch (std::out_of_range& e) {
    fileName = "./";
  }
  if (*fileName.rend() != '/') fileName += "/";
  fileName += "ofile_" + wsName;
  fileName += mycuts._useWeight ? "_weighted_postfit.root" : "_postfit.root";

  auto f = TFile(fileName.c_str());
  auto myws = (RooWorkspace*)f.Get(wsName.c_str());
  auto nsig = (RooRealVar*) myws->var("nsig");
  yields = nsig->getVal();
  err = nsig->getErrorHi();
  return;
}

void getYields()
{
  TFile ofile("yields.root", "recreate");
  const unsigned int Ntrks[] = {0, 35, 60, 90, 120, 185};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {2., 3., 4., 5., 6., 8., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  // forget 2 -- 3
  for (int ipt=1; ipt<nPt; ++ipt) {
    for (int itrk=0; itrk<ntrks; ++itrk) {
      if (ipt == 0 && itrk == ntrks-1 ) continue;
      const char* useWeight = itrk == ntrks ? "weighted" : "unweighted";
      std::string configName = ::string_format("LamC/%s_%s/pT%.0fto%.0f_Ntrk%dto%d.conf", "dz1p0", useWeight, pts[ipt], pts[ipt+1], Ntrks[itrk], Ntrks[itrk+1]);
      cout << configName << endl;
      double yields = 0;
      double err = 0;
      getFit(configName, yields, err);
      double nEvents = countEvents(configName);
      yields /= nEvents;
      err /= nEvents;
    }
  }
}
