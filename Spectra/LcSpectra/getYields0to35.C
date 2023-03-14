#include "../ProduceWorkspace/LamC/tools.C"

TH1D* hscale;

TH1D* getScales(TDirectory* dir)
{
  auto hscale = new TH1D("hNtrkoffline", "N_{trk}^{offline} for PV with highest N;N_{trk}^{offline};", 301, -0.5, 300.5);
  hscale->Sumw2();
  for (int i=0; i<(301+1); ++i) {
    hscale->SetBinContent(i, 1);
    hscale->SetBinError(i, 0);
  }
  TFile input("/storage1/users/yz144/LambdaC_KsProton/SkimTrees/effMB.root");
  TGraphAsymmErrors* g = input.Get<TGraphAsymmErrors>("Ntrk0to20");
  // cout << g->GetN() << std::endl;
  for (int i=0; i<20; ++i) {
    hscale->SetBinContent(i+1, g->GetY()[i]);
    hscale->SetBinError(i+1, g->GetErrorYhigh(i));
  }
  hscale->SetDirectory(0);
  return hscale;
}

double countEvents0to35(std::string fileName, const VarCuts& mycuts)
{
  TFile fin(fileName.c_str());
  std::string histName = "lambdacAna/hNtrkoffline";
  if (mycuts._usedz1p0) histName += "Dz1p0";
  if (mycuts._usegplus) histName += "Gplus";
  // if (!mycuts._useWeight) histName += "Unweight";
  auto hNtrkoffline =  (TH1D*)fin.Get(histName.c_str());

  double np1 = hNtrkoffline->Integral(2, 35);
  double np2 = hNtrkoffline->Integral(3, 35);
  double np3 = hNtrkoffline->Integral(4, 35);

  std::cout << "Before weighting:\t"
    << np1 << "\t"
    << np2 << "\t"
    << np3 << "\t"
    << np2/np1 << "\t"
    << np3/np1 << "\n";

  hNtrkoffline->Divide(hscale);
  // int NtrkBinMin = mycuts._NtrkofflineMin + 1;
  int NtrkBinMin = 2;
  int NtrkBinMax = mycuts._NtrkofflineMax;
  double nEvents = hNtrkoffline->Integral(NtrkBinMin, NtrkBinMax);
  double n1 = hNtrkoffline->Integral(2, 35);
  double n2 = hNtrkoffline->Integral(3, 35);
  double n3 = hNtrkoffline->Integral(4, 35);

  std::cout << "After weighting:\t"
    << n1 << "\t"
    << n2 << "\t"
    << n3 << "\t"
    << n2/n1 << "\t"
    << n3/n1 << "\n";

  std::cout << "Comparison: \t"
    << np1/n1 << "\t"
    << np2/n2 << "\t"
    << np3/n3 << "\n";
  return nEvents;
}

double countEvents0to35(std::string configName)
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
      nEvents += countEvents0to35(file, mycuts);
    }
  }
  return nEvents;
}

void getYields0to35()
{


  TFile ofile(::Form("yields_dz1p0_0to35.root"), "recreate");

  hscale = getScales(nullptr);

  const unsigned int Ntrks[] = {0, 35};
  const unsigned int ntrks = sizeof(Ntrks)/sizeof(unsigned int) - 1;
  const double pts[] = {2., 3., 4., 5., 6., 8., 10.};
  const unsigned int nPt = sizeof(pts)/sizeof(double) - 1;
  TH1D* h[nPt];
  for (int itrk=0; itrk<ntrks; ++itrk) {
    h[itrk] = new TH1D(::Form("Ntrk%dto%d", Ntrks[itrk], Ntrks[itrk+1]),
                       ::Form("N_{trk}^{offline} %d -- %d;p_{T};1/N_{evt} dN^{raw}/dp_{T}", Ntrks[itrk], Ntrks[itrk+1]),
                       nPt, pts);
    for (int ipt=0; ipt<nPt; ++ipt) {
      const char* useWeight = "weighted";
      std::string configName = ::string_format("../ProduceWorkspace/LamC/dz1p0_%s/pT%.0fto%.0f_Ntrk%dto%d.conf", useWeight, pts[ipt], pts[ipt+1], Ntrks[itrk], Ntrks[itrk+1]);
      cout << configName << endl;
      configName = gSystem->ExpandPathName(configName.c_str());
      FitParConfigs configs(configName);
      auto outputConfigs = configs.getOutputConfigs();
      auto cutConfigs = configs.getCutConfigs();
      VarCuts mycuts(cutConfigs);
      auto names = ::getNames(configs, mycuts);
      names["outdir"] = outputConfigs.getPath("outdir");
      // names["outdir"] = "/storage1/users/yz144/HIN-21-016/D0Trees/fit";
      if (*names.at("outdir").rbegin() != '/') names.at("outdir") += "/";
      std::cout << "getNames\n";
      // for (const auto key : names) {
      //  std::cout << key.first << ", " << key.second << "\n";
      // }
      // names["outdir"] = "./postfit/";
      double yields = 0;
      double err = 0;
      getFit(names, yields, err);
      std::cout << "getFit\n";
      // double nEvents = countEvents(configName);
      double nEvents = countEvents0to35(configName);
      std::cout << nEvents << "\n";
      yields /= nEvents;
      err /= nEvents;
      const double dPt = pts[ipt+1] - pts[ipt];
      yields /= dPt;
      err /= dPt;
      h[itrk]->SetBinContent(ipt+1, yields);
      h[itrk]->SetBinError(ipt+1, err);
    }
    ofile.cd();
    h[itrk]->Write();
    delete h[itrk];
  }
}
