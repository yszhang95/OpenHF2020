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

