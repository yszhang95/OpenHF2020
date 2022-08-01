double countEvents(std::string fileName);
double countEvents(std::string configName);

double countEvents(std::string fileName, const VarCuts& mycuts)
{
  TFile fin(fileName.c_str());
  std::string histName = "hNtrkoffline";
  if (mycuts._usedz1p0) histName += "Dz1p0";
  if (mycuts._usegplus) histName += "Gplus";
  // if (!mycuts._useWeight) histName += "Unweight";
  auto hNtrkoffline =  (TH1D*)fin.Get(histName.c_str());
  int NtrkBinMin = mycuts._NtrkofflineMin + 1;
  int NtrkBinMax = mycuts._NtrkofflineMax;
  double nEvents = hNtrkoffline->Integral(NtrkBinMin, NtrkBinMax);
  return nEvents;
}

double countEvents(const FitParConfigs::InputConfigs& inputConfigs, const VarCuts& mycuts)
{
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

void getFit(const std::map<std::string, std::string>& names,
            double& yields, double& err)
{

  std::string fileName = gSystem->BaseName(names.at("fileName").c_str());
  auto pos = fileName.find(".root");
  try {
    fileName.replace(pos, 5, names.at("suffix"));
  } catch (std::out_of_range& e) {
    fileName.replace(pos, 5, "_postfit.root");
  }
  std::string outdir = names.at("outdir");
  std::string wsName = names.at("wsName");

  fileName = outdir + fileName;

  auto f = TFile(fileName.c_str());
  auto myws = (RooWorkspace*)f.Get(wsName.c_str());
  auto nsig = (RooRealVar*) myws->var("nsig");
  yields = nsig->getVal();
  err = nsig->getErrorHi();
  return;
}
