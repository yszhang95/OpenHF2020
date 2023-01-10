#include <stdexcept>
#include <iostream>
#include <sstream>

#include "TPRegexp.h"
#include "TObjArray.h"
#include "TString.h"
#include "TObjString.h"

#include "Utilities/FitUtils/ReadConfig.h"

#include "Utilities/Ana/Common.h"

#ifdef __READ_CONFIG__

using std::ifstream;
using std::istringstream;
using std::set;
using std::string;
using std::vector;
using std::cout;
using std::endl;

FitParConfigs::FitParConfigs(const char* name)
{
  ifstream filename(name);
  initialize(filename);
}

FitParConfigs::FitParConfigs(const TString& name)
{
  ifstream filename(name.Data());
  initialize(filename);
}

FitParConfigs::FitParConfigs(const string& name)
{
  ifstream filename(name);
  initialize(filename);
}

void FitParConfigs::initialize(ifstream& infile)
{
  string line;
  string category;
  while (std::getline(infile, line)) {
    TString text(line);
    text = text.Strip(TString::kBoth);
    // skip comments and empty lines
    if (text[0] == '#') continue;
    if (!text.Length()) continue;
    // if it matches BEG or END, continue
    // regular expression \[[Bb][Ee][Gg]\w*?\s*([(\w)]{1,})\s*\]
    // regular expression \[[Ee][Nn][Dd]\s*([(\w)]{1,})\s*\]
    TPRegexp regexpBeg(R"_(\[[Bb][Ee][Gg]\w*?\s*([(\w)]{1,})\s*\])_");
    TPRegexp regexpEnd(R"_(\[[Ee][Nn][Dd]\s*([(\w)]{1,})\s*\])_");

    // look for new category, otherwise keep the old one
    auto begarr = regexpBeg.MatchS(text);
    if (begarr->GetEntries()) {
      auto ptr = dynamic_cast<TObjString*>(begarr->At(1));
      TString temp = ptr->GetString();
      temp = temp.Strip(TString::kBoth);
      category = temp.Data();
    }

    // check if we reach the end of text block
    // check if the name in end and begin are different
    // finish is useful to determine when to clear category
    bool finish = false;
    auto endarr = regexpEnd.MatchS(text);
    if (endarr->GetEntries()) {
      auto ptr = dynamic_cast<TObjString*>(endarr->At(1));
      TString temp = ptr->GetString();
      temp = temp.Strip(TString::kBoth);
      TString begtemp = TString(category);
      if (temp != begtemp) {
        TString errorMessage = "Unmatched BEG-END pair for " + begtemp;
        throw std::logic_error(errorMessage.Data());
      }
      finish = true;
    }

    // if the key exists, fill in contents
    // category need to be non-empty
    // insertion return <it, false>, we are safe to push_back
    // insertion return <it, true>, we created a new entry
    if (!category.empty()
        && !_data.insert({category, vector<string>{} }).second
        && !finish) {
      _data.at(category).push_back(line);
    } else continue;

    // shrink to fit to save memory
    // clear category when finishing this beg-end block
    if (finish) {
      _data.at(category).shrink_to_fit();
      category.clear();
    }
  }

  _parConfigs.initialize(_data.at("ParConfigs"));
  _inputConfigs.initialize(_data.at("InputConfigs"));
  _outputConfigs.initialize(_data.at("OutputConfigs"));
  _cutConfigs.initialize(_data.at("CutConfigs"));
}

void FitParConfigs::dump()
{
  cout << "Dumping" << endl;
  cout << "Dumping _data\n" << endl;
  for (const auto& pr : _data) {
    cout << "\t" << pr.first << "\n";
    for (const auto& l : pr.second) {
      cout << "\t" << l << "\n";
    }
    cout << endl;
  }
}

void FitParConfigs::ParConfigs::initialize(const vector<string>& block)
{
  // std::string line;
  std::vector<std::string> _names;
  std::vector<bool>   _hasInit;
  std::vector<double> _init;
  std::vector<double> _min;
  std::vector<double> _max;

  for (auto line : block) {
    std::vector<TString> words;
    std::string w;
    istringstream words_stream(line);
    while (std::getline(words_stream, w, ',')) {
      TString word(w);
      word = word.Strip(TString::kBoth, ' ');
      word = word.Strip(TString::kBoth, '\t');
      if (word.BeginsWith("#")) continue;
      else words.push_back(word);
    }
    if (words.size() == 4) {
      const std::string name = words.at(0).Data();
      const double initVal = words.at(1).Atof();
      const double minVal  = words.at(2).Atof();
      const double maxVal  = words.at(3).Atof();
      const bool   hasInit = true;
      _names  .push_back(name);
      _init   .push_back(initVal);
      _min    .push_back(minVal );
      _max    .push_back(maxVal );
      _hasInit.push_back(hasInit);
    } else if (words.size() == 3) {
      const std::string name = words.at(0).Data();
      const double initVal = 0;
      const double minVal  = words.at(1).Atof();
      const double maxVal  = words.at(2).Atof();
      const bool   hasInit = false;
      _names  .push_back(name);
      _init   .push_back(initVal);
      _min    .push_back(minVal );
      _max    .push_back(maxVal );
      _hasInit.push_back(hasInit);
    }
  }
  using sz = std::vector<double>::size_type;
  for (sz it=0; it!=_names.size(); ++it) {
    _data[_names.at(it)] = std::make_tuple(_hasInit.at(it),
                                           _init.at(it),
                                           _min.at(it),
                                           _max.at(it) );
  }
}

void
FitParConfigs::ParConfigs::setInit(const std::string n, const double _init)
{
  if (std::get<0>(_data.at(n))) {
    std::get<1>(_data.at(n)) = _init;
  }
}
bool
FitParConfigs::ParConfigs::hasVariable(const std::string& n) const
{ return _data.find(n) != _data.end(); }

bool FitParConfigs::ParConfigs::hasInit(const std::string& n) const
{
  return std::get<0>(_data.at(n));
}
double FitParConfigs::ParConfigs::getInit(const std::string& n) const
{
  return std::get<1>(_data.at(n));
}
double FitParConfigs::ParConfigs::getMin(const std::string& n) const
{
  return std::get<2>(_data.at(n));
}
double FitParConfigs::ParConfigs::getMax(const std::string& n) const
{
  return std::get<3>(_data.at(n));
}
double FitParConfigs::ParConfigs::getInit(const std::string& n, const double val) const
{
  try{
    return std::get<1>(_data.at(n));
  } catch (std::out_of_range& e) {
    std::cout << "[Warning] Not found the initialized value for variable " << n
      << " in configurations. Taking "<< val << "as the return value.\n";
    return val;
  }
}
double FitParConfigs::ParConfigs::getMin(const std::string& n, const double val) const
{
  try{
    return std::get<2>(_data.at(n));
  } catch (std::out_of_range& e) {
    std::cout << "[Warning] Not found the minimum value for variable " << n
      << " in configurations. Taking "<< val << "as the return value.\n";
    return val;
  }
}
double FitParConfigs::ParConfigs::getMax(const std::string& n, const double val) const
{
  try{
    return std::get<3>(_data.at(n));
  } catch (std::out_of_range& e) {
    std::cout << "[Warning] Not found the minimum value for variable " << n
      << " in configurations. Taking "<< val << "as the return value.\n";
    return val;
  }
}

void FitParConfigs::InputConfigs::initialize(const vector<string>& block)
{
  for (auto line : block) {
    istringstream words(line);
    string word;
    vector<string> temp;
    while (std::getline(words, word, ',')) {
      TString word_nospace = word;
      word_nospace = word_nospace.Strip(TString::kBoth);
      word = word_nospace.Data();
      temp.push_back(word);
    }
    // push path to the end of _path
    string category = temp.at(0);
    string theType = temp.at(1);
    string path = temp.at(2);
    string theName = temp.at(3);
    auto it = _paths.insert({category, vector<string>{}});
    const bool firstTime = it.second;
    _paths.at(category).push_back(path);
    _types.insert({category, set<string>{}});
    _names.insert({category, set<string>{}});
    auto itType = _types.at(category).insert(theType);
    if (!firstTime && itType.second) {
      throw std::logic_error("The types in " + category +
                             " are inconsistent!");
    }
    auto itName = _names.at(category).insert(theName);
    if (!firstTime && itName.second) {
      throw std::logic_error("The names in " + category +
                             " are inconsistent!");
    }
  }
}

std::vector<std::string>
FitParConfigs::InputConfigs::getPaths(const std::string& category) const
{
  return _paths.at(category);
}
std::string
FitParConfigs::InputConfigs::getType(const std::string& category) const
{
  return *_types.at(category).begin();
}
std::string
FitParConfigs::InputConfigs::getName(const std::string& category) const
{
  return *_names.at(category).begin();
}

void FitParConfigs::OutputConfigs::initialize(const vector<string>& block)
{
  vector<string> paths;
  for (const auto& line : block) {
    vector<string> temp;
    string word;
    for (istringstream words(line);
         std::getline(words, word, ','); ) {
      TString word_nospace = word;
      word_nospace = word_nospace.Strip(TString::kBoth);
      word = word_nospace.Data();
      temp.push_back(word);
    }
    string category = temp.at(0);
    string theType = temp.at(1);
    string thePath = temp.at(2);
    auto it_types = _types.insert( {category, theType} );
    auto it_paths = _paths.insert( {category, thePath} );
    if (!it_types.second || !it_paths.second) {
      throw std::logic_error("There are duplicates in OutputConfigs.");
    }
  }
}

void FitParConfigs::CutConfigs::initialize(const vector<string>& block)
{
  for (auto line : block) {
    istringstream words(line);
    string word;
    vector<string> temp;
    while (std::getline(words, word, ',')) {
      TString word_nospace = word;
      word_nospace = word_nospace.Strip(TString::kBoth);
      word = word_nospace.Data();
      temp.push_back(word);
    }
    // push path to the end of _path
    string dataName = temp.at(0);
    string dataMin = temp.at(1);
    string dataMax = temp.at(2);
    // urgly but easy to write
    // temporarily use dataMin as boolean storage
    string dataType = temp.size()==4 ? temp.at(3) : temp.at(2);

    std::transform(dataType.begin(), dataType.end(), dataType.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // if it is a boolean
    if (temp.size() == 3) {
      std::transform(dataName.begin(), dataName.end(), dataName.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      std::transform(dataMin.begin(), dataMin.end(), dataMin.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      if (dataType == "bool") {
        if (dataMin == "0" || dataMin == "false" || dataMin == "null")
          _data_bool[dataName] = false;
        else
          _data_bool[dataName] = true;
      }
      else {
        throw std::logic_error("[FATAL] Only boolean variable can in three slots.");
      }
    }

    // dataType == MVA is a special case
    if (dataType == "mva") {
      _mvaName = dataName;
      dataName = "mva";
      dataType = "float";
    }

    std::transform(dataName.begin(), dataName.end(), dataName.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    // I should replace [] with insert to check duplicates in the future
    if (dataType == "float") {
      const float dataMinVal = std::stof(dataMin);
      const float dataMaxVal = std::stof(dataMax);
      _data_float_min[dataName] = dataMinVal;
      _data_float_max[dataName] = dataMaxVal;
    }
    if (dataType == "int") {
      std::transform(dataMin.begin(), dataMin.end(), dataMin.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      std::transform(dataMax.begin(), dataMax.end(), dataMax.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      auto pos_for_min = dataMin.find("inf");
      auto pos_for_max = dataMax.find("inf");
      if (pos_for_min == std::string::npos) {
        const int dataMinVal = std::stoi(dataMin);
        _data_int_min[dataName] = dataMinVal;
      }
      if (pos_for_max == std::string::npos) {
        const int dataMaxVal = std::stoi(dataMax);
        _data_int_max[dataName] = dataMaxVal;
      }
    }
  }
}

int FitParConfigs::CutConfigs::getIntMin(std::string s) const
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return _data_int_min.at(s);
}
int FitParConfigs::CutConfigs::getIntMax(std::string s) const
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return _data_int_max.at(s);
}
float FitParConfigs::CutConfigs::getFloatMin(std::string s) const
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return _data_float_min.at(s);
}
float FitParConfigs::CutConfigs::getFloatMax(std::string s) const
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return _data_float_max.at(s);
}
bool FitParConfigs::CutConfigs::getBool(std::string s) const
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return _data_bool.at(s);
}

VarCuts::VarCuts(const FitParConfigs::CutConfigs cutConfigs):
    _usedz1p0(0), _usegplus(0), _useWeight(0), _useMB(0), _useHM(0)
{
  _mvaName = cutConfigs.getMvaName();
  _mvaCut = cutConfigs.getFloatMin("mva");
  _pTMin = cutConfigs.getFloatMin("pT");
  _pTMax = cutConfigs.getFloatMax("pT");
  _yAbsMax = cutConfigs.getFloatMax("yAbs");
  try {
    int temp = cutConfigs.getIntMin("Ntrkoffline");
    if (temp<0) _NtrkofflineMin = 0;
    else _NtrkofflineMin = temp;
    if (temp<0) {
      std::cout << "[ERROR] The min of Ntrkoffline seems to be negative. "
        "Set it to be 0 instead\n";
    }
  } catch (std::out_of_range&) {
    _NtrkofflineMin = 0;
    std::cout << "[ERROR] The min of Ntrkoffline seems to be undefined. "
      "Set it to be 0 instead\n";
  }
  try {
    int temp = cutConfigs.getIntMax("Ntrkoffline");
    if (temp > UShort_t(-1)) _NtrkofflineMax = UShort_t(-1);
    else _NtrkofflineMax = temp;
    if (temp > UShort_t(-1))
      std::cout << "[ERROR] The max of Ntrkoffline is larger"
        " than maximum of unsigned short. "
        "Set it to be " << UShort_t(-1) << " instead\n";
  } catch (std::out_of_range&) {
    _NtrkofflineMax = UShort_t(-1);
    std::cout << "[ERROR] The max of Ntrkoffline seems to be undefined. "
      "Set it to be " << UShort_t(-1) << " instead\n";
  }

  try {
    _usedz1p0 = cutConfigs.getBool("usedz1p0");
  }
  catch (std::out_of_range& e) {
    std::cout << "[ERROR] Not found setup for useDz1p0. "
      "Set it to be false\n";
  }
  try {
    _usegplus = cutConfigs.getBool("usegplus");
  }
  catch (std::out_of_range& e) {
    std::cout << "[ERROR] Not found setup for useGplus. "
      "Set it to be false\n";
  }
  try {
    _useHM = cutConfigs.getBool("usehm");
  }
  catch (std::out_of_range& e) {
    std::cout << "[ERROR] Not found setup for useHM. "
      "Set it to be false\n";
  }
  try {
    _useMB = cutConfigs.getBool("usemb");
  }
  catch (std::out_of_range& e) {
    std::cout << "[ERROR] Not found setup for useMB. "
      "Set it to be false\n";
  }
  try {
    _useWeight = cutConfigs.getBool("useweight");
  }
  catch (std::out_of_range& e) {
    std::cout << "[ERROR] Not found setup for useWeight. "
      "Set it to be false\n";
  }

  // check boundaries
  if (_pTMin > _pTMax || std::abs(_pTMax-_pTMin)<1e-5) {
    throw std::logic_error("[FATAL] pTMin is larger than or equal to pTMax\n");
  }
  if (_yAbsMax<0) {
    throw std::logic_error("[FATAL] Maximum of |y| cannot be negative\n");
  }
  if (_NtrkofflineMin > _NtrkofflineMax) {
    throw std::logic_error("[FATAL] NrkofflineMin is larger than NtrkofflineMax\n");
  }
}

string VarCuts::getEventFilter() const
{
  string eventFilter;
  if (_usedz1p0) eventFilter += "dz1p0";
  if (_usegplus) {
    if (eventFilter.empty()) eventFilter += "gplus";
    else eventFilter += "_gplus";
  }
  if (_useMB) {
    if (eventFilter.empty()) eventFilter += "MB";
    else eventFilter += "_MB";
  }
  if (_useHM) {
    if (eventFilter.empty()) eventFilter += "HM";
    else eventFilter += "_HM";
  }
  return eventFilter;
}

string VarCuts::getNtrkoffline() const
{
  string ntrkofflineStr;
  if ( _NtrkofflineMin == 0 && _NtrkofflineMax == UShort_t(-1) ) {
    return "NtrkAll";
  }
  if (ntrkofflineStr.empty()) {
    ntrkofflineStr += "Ntrk" + std::to_string(_NtrkofflineMin) + "to";
    if (_NtrkofflineMax == UShort_t(-1)) {
      ntrkofflineStr += "Inf";
    }
    else {
      ntrkofflineStr += std::to_string(_NtrkofflineMax);
    }
  }
  return ntrkofflineStr;
}

string VarCuts::getKinematics() const
{
  auto kinematics = string_format("pT%gto%g_yAbs%g",
                                  _pTMin, _pTMax, _yAbsMax);
  std::replace( kinematics.begin(), kinematics.end(), '.', 'p');
  // alternative way to std::replace
  // for (auto pos = kinematics.find("."); pos!=string::npos; ) {
  //   kinematics.replace(pos, 1, "p");
  //   pos = kinematics.find(".");
  // }
  return kinematics;
}

string VarCuts::getMva() const
{
  auto mvaCutStr = string_format("mva%g", _mvaCut);
  std::replace( mvaCutStr.begin(), mvaCutStr.end(), '.', 'p');
  // alternative way to std::replace
  // auto pos = mvaCutStr.find(".");
  // mvaCutStr.replace(pos, 1, "p");
  auto posNeg = mvaCutStr.find("-");
  if (posNeg != string::npos)
    mvaCutStr.replace(posNeg, 1, "Neg");
  auto posInf = mvaCutStr.find("inf");
  if (posInf != string::npos)
    mvaCutStr.replace(posInf, 3, "Inf");
  return mvaCutStr;
}

std::map<std::string, std::string>
getNames(const FitParConfigs& configs, const VarCuts& mycuts)
{
  std::map<std::string, std::string> output;
  auto outputConfigs = configs.getOutputConfigs();
  const auto cutConfigs = configs.getCutConfigs();

  int effTypeMin = 0;
  int effTypeMax = 0;
  try {
    effTypeMin = cutConfigs.getIntMin("effType");
    effTypeMax = cutConfigs.getIntMax("effType");
  } catch (std::out_of_range& e) {
    if (mycuts._useWeight) {
      std::cout << "Cannot find effType in configuration. Set it to the central value\n";
      effTypeMin = 0;
      effTypeMax = 0;
    }
  }
  if (mycuts._useWeight) {
    if (effTypeMin != effTypeMax)
      throw std::logic_error("minimum and maximum for weight type must be the same.\n");
    const auto effType = EfficiencyTable<TGraph>::Value(effTypeMin);
    if (effType > EfficiencyTable<TGraph>::Value::effUp ||
        effType < EfficiencyTable<TGraph>::Value::effLow)
      throw std::logic_error("weight type must be within the range of EfficiencyTable<TGraph>::Value.\n");
    output["effType"] = std::to_string(effTypeMin);
  }
  const auto effType = EfficiencyTable<TGraph>::Value(effTypeMin);

  const std::string mvaName = mycuts._mvaName;
  const std::string mvaStr = mycuts.getMva();
  const std::string kinStr = mycuts.getKinematics();
  const std::string eventStr = mycuts.getEventFilter();
  const std::string ntrkStr = mycuts.getNtrkoffline();

  const std::string label = kinStr + "_" + mvaStr + "_" + eventStr + "_" + ntrkStr;
  output["wsName"] = "ws_" + label;
  output["dsName"] = "ds_" + label;
  std::string& fileName = output["fileName"];
  try {
    fileName = outputConfigs.getPath("outdir");
  } catch (std::out_of_range& e) {
    fileName = "./";
  }
  if (*fileName.rend() != '/') fileName += "/";
  fileName += "ofile_" + output.at("wsName");
  if (mycuts._useWeight) {
    switch (effType)  {
      case EfficiencyTable<TGraph>::Value::effCent :
        fileName += "_weighted.root"; output["weightstr"] = "weighted"; break;
      case EfficiencyTable<TGraph>::Value::effLow :
        fileName += "_weighted_up.root"; output["weightstr"] = "weighted_up"; break;
      case EfficiencyTable<TGraph>::Value::effUp :
        fileName += "_weighted_lo.root"; output["weightstr"] = "weighted_lo"; break;
      default:
        throw std::logic_error("The weight type is wrong.\n");
    }
  } else {
    fileName += ".root";
  }

  const auto MClabel = "_" + kinStr + "_" + mvaStr;
  output["dsMCSignalName"] = "dsMCSignal" + MClabel;
  output["dsMCSwapName"] = "dsMCSwap" + MClabel;
  output["dsMCAllName"] = "dsMCAll" + MClabel;
  output["dsMCKKName"] = "dsMCKK" + MClabel;
  output["dsMCPiPiName"] = "dsMCPiPi" + MClabel;
  return output;
}
#endif
