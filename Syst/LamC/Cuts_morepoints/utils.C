std::vector<double> getCuts(std::string trig, std::string ptbin)
{
  std::ifstream f("cuts.txt");
  std::string s;
  std::map<std::string, std::map<std::string, std::vector<double > > > input;
  std::string key, key2;
  while(std::getline(f, s)){
    std::replace(s.begin(), s.end(), ',', ' ');
    const bool isBegin = s.find("#BEGIN_") != std::string::npos;
    const bool isEnd = s.find("#END_") != std::string::npos;
    if (isBegin) { key = s.substr(7, s.size()-7); }
    if (isEnd) { key.clear(); }
    if (!key.empty()) input.insert({key, std::map<std::string, std::vector<double > > {}});
    if (key.empty()) continue;
    if (*s.begin() == '#') {
      if (!isBegin && !isEnd) key2 = s.substr(1, s.size()-1);
    } else {
      istringstream ss(s);
      double temp = 0;
      std::vector<double> cuts;
      while (ss >> temp) {
        cuts.push_back(temp);
      }
      input.at(key).insert({key2, cuts});
      key2.clear();
    }
  }
  return input[trig][ptbin];
}

std::map<std::string, std::string>
getOutNames(const VarCuts& mycuts)
{
  std::map<std::string, std::string> output;
  const std::string kinStr = mycuts.getKinematics();
  const std::string eventStr = mycuts.getEventFilter();
  output["outdir"] = "./output_workspace/";
  output["dsName"] = "ds_" + eventStr + "_" + kinStr;
  output["wsName"] = "ws_" + eventStr + "_" + kinStr;
  output["fileName"] = output.at("outdir") + "ofile_" + eventStr + "_" + kinStr + ".root";
  output["dsMCSignalName"] = "dsMC_" + eventStr + "_" + kinStr;
  return output;
}


struct VarSetup {
  int nbins_mva;
  float mva_min;
  float mva_max;
  int n_cuts;
  float cut_min;
  float cut_max;
  int step = 100000000;

  VarSetup(const std::string trig,
      const double ptmin, const double ptmax) {
    // 0 - nbins_mva
    // 1 - mva_min
    // 2 - mva_max
    // 3 - n_cuts: how many cuts I would use approximately
    // 4 - cut_min
    // 5 - cut_max

    if (trig == "MB") {
      // MB 2--3
      if (std::abs(ptmin-2)<1E-3 && std::abs(ptmax-3)<1E-3) {
        // optimal 0.0008
        nbins_mva = 140;
        mva_min = 2E-4;
        mva_max = 1.6E-3;
        // n_cuts = 35;
        n_cuts = 16;
        // cut_min = 2E-4;
        cut_min = 5.6E-4;
        cut_max = 1.2E-3;
      }
      // MB 3--4
      if (std::abs(ptmin-3)<1E-3 && std::abs(ptmax-4)<1E-3) {
        // optimal 0.0041
        nbins_mva = 200;
        mva_min = 0.001;
        mva_max = 0.005;
        n_cuts = 15;
        cut_min = 0.0025;
        cut_max = 0.0045;
      }
      // MB 4--6
      if (std::abs(ptmin-4)<1E-3 && std::abs(ptmax-5)<1E-3) {
        // optimal 0.0029
        nbins_mva = 160;
        mva_min = 0.0005;
        mva_max = 0.0045;
        n_cuts = 20;
        cut_min = 0.0005;
        cut_max = 0.0035;
      }
      if (std::abs(ptmin-5)<1E-3 && std::abs(ptmax-6)<1E-3) {
        // optimal 0.0029
        nbins_mva = 160;
        mva_min = 0.0005;
        mva_max = 0.0045;
        n_cuts = 20;
        cut_min = 0.0005;
        cut_max = 0.0035;
      }

      // MB 6--8
      if (std::abs(ptmin-6)<1E-3 && std::abs(ptmax-8)<1E-3) {
        // optimal 0.009
        nbins_mva = 180;
        mva_min = 0.0015;
        mva_max = 0.0195;
        n_cuts = 15;
        cut_min = 0.0055;
        cut_max = 0.0215;
      }
      // MB 8--10;
      if (std::abs(ptmin-8)<1E-3 && std::abs(ptmax-10)<1E-3) {
        nbins_mva = 100;
        mva_min = 0;
        mva_max = 0.020;
        n_cuts = 15;
        cut_min = 0;
        cut_max = 0.018;
      }
    }
    if (trig == "HM") {
      // HM 3--4
      if (std::abs(ptmin-3)<1E-3 && std::abs(ptmax-4)<1E-3) {
        // optimal 0.0064
        nbins_mva = 180;
        mva_min = 0.001;
        mva_max = 0.01;
        n_cuts = 14;
        cut_min = 0.001;
        cut_max = 0.008;
      }
      // HM 4--6
      if (std::abs(ptmin-4)<1E-3 && std::abs(ptmax-5)<1E-3) {
        // optimal 0.0022, 0.00185
        nbins_mva = 100;
        mva_min = 0.0005;
        mva_max = 0.003;
        n_cuts = 16;
        cut_min = 0.0005;
        cut_max = 0.003;
      }
      if (std::abs(ptmin-5)<1E-3 && std::abs(ptmax-6)<1E-3) {
        // optimal 0.0022, 0.00185
        nbins_mva = 100;
        mva_min = 0.0005;
        mva_max = 0.003;
        n_cuts = 16;
        cut_min = 0.0005;
        cut_max = 0.003;
      }
      // HM 6--8
      if (std::abs(ptmin-6)<1E-3 && std::abs(ptmax-8)<1E-3) {
        // optimal 0.00165
        nbins_mva = 140;
        mva_min = 0.0005;
        mva_max = 0.004;
        n_cuts = 16;
        cut_min = 0.0005;
        cut_max = 0.003;
      }
      // if (std::abs(ptmin-6)<1E-3 && std::abs(ptmax-8)<1E-3) return {100, 0, 0.02, 0, 0, 0};
      if (std::abs(ptmin-8)<1E-3 && std::abs(ptmax-10)<1E-3) {
        nbins_mva = 80;
        mva_min = 0;
        mva_max = 0.016;
        n_cuts = 14;
        cut_min = 0;
        cut_max = 0.014;
      }
    }
    double step_length = (cut_max - cut_min)/n_cuts/ ((mva_max - mva_min)/nbins_mva);
    step = static_cast<int>(std::round(step_length)+1E-3);
    if (step == 0) step = 1;

    std::cout << trig << " PT: " << ptmin << " -- " << ptmax << "\n";
    std::cout << "#bins for MVA is " << nbins_mva << "\n";
    std::cout << "Minimum for MVA is " << mva_min << "\n";
    std::cout << "Maximum for MVA is " << mva_max << "\n";
    std::cout << "#cuts for MVA is approximately " << n_cuts << "\n";
    std::cout << "Minimum cuts for MVA is " << cut_min << "\n";
    std::cout << "Maximum cuts for MVA is " << cut_max << "\n";
    std::cout << "#steps in terms of bins is " << step << "\n";
  }
};
