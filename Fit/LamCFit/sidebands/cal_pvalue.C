int lowest = 1;
// int lowest = 2;
int highest = 5;
// int highest = 4;

double cal_nll(std::string configName, int order)
{
  std::map<std::string, std::string> strs;

  std::cout << configName << "\n";

  FitParConfigs configs(configName);
  auto inputConfigs = configs.getInputConfigs();
  auto outputConfigs = configs.getOutputConfigs();
  auto par = configs.getParConfigs();
  const auto cutConfigs = configs.getCutConfigs();
  // modify it later
  VarCuts mycuts(cutConfigs);

  const auto NtrkMin = mycuts._NtrkofflineMin;
  const auto NtrkMax = mycuts._NtrkofflineMax;
  const float pTMin = mycuts._pTMin;
  const float pTMax = mycuts._pTMax;

  const std::string mvaName = mycuts._mvaName;
  const std::string mvaStr = mycuts.getMva();
  const std::string kinStr = mycuts.getKinematics();
  const std::string eventStr = mycuts.getEventFilter();
  const std::string ntrkStr = mycuts.getNtrkoffline();

  const auto names = ::getNames(configs, mycuts);

  const std::string wsName = names.at("wsName");
  const std::string dsName = names.at("dsName");
  strs["fileName"] = names.at("fileName");

  std::string fileName = strs.at("fileName");
  fileName = gSystem->BaseName(fileName.c_str());
  auto pos = fileName.find(".root");
  fileName.replace(pos, 5, ::Form("_order%d_postfit.root", order));
  fileName = "postfit/" + fileName;
  cout << fileName << "\n";
  TFile infile(fileName.c_str());
  auto myws = (RooWorkspace*) infile.Get(wsName.c_str());
  auto ret_ptr = (RooFitResult*) myws->obj(::Form("%s_result", dsName.c_str()));
  return ret_ptr->minNll();
}

void cal_pvalue()
{
  const float pts[] =   {2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 10.0};
  const unsigned int nPt = sizeof(pts)/sizeof(float) - 1;
  const int Ntrks[] = {0, 35, 60, 90, 120, 185, 250};
  const unsigned int nbins = sizeof(Ntrks)/sizeof(int) - 1;
  for (unsigned int ibin=0; ibin<nbins; ++ibin) {
    std::ofstream ofile(::string_format("Ntrk%dto%d.txt", Ntrks[ibin], Ntrks[ibin+1]));
    ofile << "pT";
    for (int i=lowest; i<highest+1; ++i) {
      ofile << "," << "order" << i;
    }
    for (int i=lowest; i<highest; ++i) {
      ofile << "," << "NLL(null" << i << "-alt" << i+1 << ")";
      ofile << "," << "Prob(null" << i << "-alt" << i+1 << ")";
    }
    ofile << std::endl;

    const auto default_precision {std::cout.precision()};
    constexpr auto max_precision {std::numeric_limits<long double>::digits10 + 1}; 
    // const auto max_precision {default_precision};

    ofile << std::setprecision(max_precision);

    for (unsigned int ipt=0; ipt<nPt; ++ipt) {
      if (Ntrks[ibin+1] == 250 && Ntrks[ibin] == 185 && std::abs(pts[ipt]-2)<1E-5) continue;
      std::string ptstr = ::string_format("pT%.0fto%.0f", pts[ipt], pts[ipt+1]);
      ofile << ptstr;
      const char* useWeight = Ntrks[ibin+1] == 250 && Ntrks[ibin] == 185 ? "weighted" : "unweighted";
      auto configName = ::string_format("${OPENHF2020TOP}/Spectra/ProduceWorkspace/LamC/dz1p0_%s/pT%.0fto%.0f_Ntrk%dto%d.conf", useWeight, pts[ipt], pts[ipt+1], Ntrks[ibin], Ntrks[ibin+1]);
      configName = gSystem->ExpandPathName(configName.c_str());
      std::map<int, double> nlls;
      for (int i=lowest; i<highest+1; ++i) {
        auto nll = cal_nll(configName, i);
        ofile << "," << nll;
        nlls[i] = nll;
      }
      for (int i=lowest; i<highest; ++i) {
        const auto diff = nlls[i] - nlls[i+1];
        const auto prob = TMath::Prob(diff, 1);
        ofile << "," << diff;
        ofile << "," << prob;
      }
      ofile << std::endl;
    }
    ofile << std::setprecision(default_precision);
  }

}
