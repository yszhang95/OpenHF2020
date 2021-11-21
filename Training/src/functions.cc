#include "functions.h"

#ifdef __MyFunctions__

#include <algorithm>

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

#include "TClonesArray.h"
#include "TObjString.h"
#include "TPRegexp.h"
#include "TObjString.h"
#include "TFileCollection.h"
#include "THashList.h"
#include "TXMLEngine.h"

#include "Ana/MyNTuple.h"

using std::map;
using std::pair;
using std::string;
using std::vector;
using std::ifstream;
using std::getline;
using std::cout;
using std::cerr;
using std::endl;
using std::istringstream;

using std::make_pair;

map<string, TMVA::Types::EMVA> setupMethodCollection()
{
  map<string, TMVA::Types::EMVA> MethodCollection;
  MethodCollection["Variable"       ] = TMVA::Types::kVariable       ;
  MethodCollection["Cuts"           ] = TMVA::Types::kCuts           ;
  MethodCollection["Likelihood"     ] = TMVA::Types::kLikelihood     ;
  MethodCollection["PDERS"          ] = TMVA::Types::kPDERS          ;
  MethodCollection["HMatrix"        ] = TMVA::Types::kHMatrix        ;
  MethodCollection["Fisher"         ] = TMVA::Types::kFisher         ;
  MethodCollection["KNN"            ] = TMVA::Types::kKNN            ;
  MethodCollection["CFMlpANN"       ] = TMVA::Types::kCFMlpANN       ;
  MethodCollection["TMlpANN"        ] = TMVA::Types::kTMlpANN        ;
  MethodCollection["BDT"            ] = TMVA::Types::kBDT            ;
  MethodCollection["DT"             ] = TMVA::Types::kDT             ;
  MethodCollection["RuleFit"        ] = TMVA::Types::kRuleFit        ;
  MethodCollection["SVM"            ] = TMVA::Types::kSVM            ;
  MethodCollection["MLP"            ] = TMVA::Types::kMLP            ;
  MethodCollection["BayesClassifier"]=  TMVA::Types::kBayesClassifier;
  MethodCollection["FDA"            ] = TMVA::Types::kFDA            ;
  MethodCollection["Boost"          ] = TMVA::Types::kBoost          ;
  MethodCollection["PDEFoam"        ] = TMVA::Types::kPDEFoam        ;
  MethodCollection["LD"             ] = TMVA::Types::kLD             ;
  MethodCollection["Plugins"        ] = TMVA::Types::kPlugins        ;
  MethodCollection["Category"       ] = TMVA::Types::kCategory       ;
#if ROOT_VERSION_CODE >= ROOT_VERSION(6,8,0)
  MethodCollection["DNN"            ] = TMVA::Types::kDNN            ;
  MethodCollection["DL"             ] = TMVA::Types::kDL             ;
  MethodCollection["PyKeras"        ] = TMVA::Types::kPyKeras        ;
  MethodCollection["CrossValidation"] = TMVA::Types::kCrossValidation;
#endif
  MethodCollection["PyRandomForest" ] = TMVA::Types::kPyRandomForest ;
  MethodCollection["PyAdaBoost"     ] = TMVA::Types::kPyAdaBoost     ;
  MethodCollection["PyGTB"          ] = TMVA::Types::kPyGTB          ;
  MethodCollection["C50"            ] = TMVA::Types::kC50            ;
  MethodCollection["RSNNS"          ] = TMVA::Types::kRSNNS          ;
  MethodCollection["RSVM"           ] = TMVA::Types::kRSVM           ;
  MethodCollection["RXGB"           ] = TMVA::Types::kRXGB           ;
  MethodCollection["MaxMethod"      ] = TMVA::Types::kMaxMethod      ;
  return MethodCollection;
}

tmvaConfigs::tmvaConfigs(string inputXML, bool debug):
  _outfileName(""), _outdirName(""),
  _signalFileList(""), _backgroundFileList(""),
  _signalWeight(1.), _backgroundWeight(1.),
  _triggerIndex(-1), _filterIndex(-1),
  _NtrkLow(0), _NtrkHigh(UShort_t(-1)),
  _saveTree(0), _saveDau(0), _selectMVA(0),
  _useEventWiseWeight(0), _isMC(0), _saveMatchedOnly(1),
  _flipEta(0), _selectDeDx(0), _wantAbort(0),  _debug(debug)
{
  if (_debug) { cout << "\nStart readConfigs" << endl; }

  TXMLEngine xml;
  XMLDocPointer_t xmldoc =  xml.ParseFile(inputXML.c_str());
  XMLNodePointer_t mainnode = xml.DocGetRootElement(xmldoc);
  XMLNodePointer_t child = xml.GetChild(mainnode);

  istringstream iStr;

  while(child != 0) {
    const char* childname = xml.GetNodeName(child);
    const char* content = xml.GetNodeContent(child);
    child = xml.GetNext(child);
    iStr.str(string(content));
    const auto& has = _configs.insert(
        map<string, vector<string>>::value_type(
          childname, vector<string>()));
    for(string temp; getline(iStr, temp); ){
      trim(temp);
      if(!temp.size()) continue;
      if(temp.at(0)=='#') continue;
      _configs.at(childname).push_back(temp);
      temp.clear();
    }
    iStr.clear();
  }

  if (_debug) {
    for (const auto& e : _configs) {
      cout << e.first << endl;
      for (const auto& ee : e.second) {
        cout << ee << endl;
      }
    }
  }
  xml.FreeDoc(xmldoc);
  if (_debug) { cout << "End readConfigs\n" << endl; }

  if(_configs.count("outfileName")
     && _configs.at("outfileName").size()) _outfileName = _configs.at("outfileName").front();
  if(_configs.count("outputDir")
     && _configs.at("outputDir").size()) _outdirName = _configs.at("outputDir").front();

  if (_debug) {
    cout << "outfileName: " << _outfileName << endl;
    cout << "outdirName: " << _outdirName << endl;
  }

  auto options = getOptions();
  for (auto& op : options) op.ToLower();
  auto it_SW = std::find_if(options.begin(), options.end(),
                            [](const TString& str)
                            { return str.Contains("siganlweight=");});
  auto it_BW = std::find_if(options.begin(), options.end(),
                            [](const TString& str)
                            { return str.Contains("backgroundweight=");});
  if (it_SW != options.end())
    _signalWeight = TString((*it_SW)(12, it_SW->Length()-1)).Atof();
  if (it_BW != options.end())
    _backgroundWeight = TString((*it_BW)(16, it_BW->Length()-1)).Atof();
  if (_debug) {
    cout << "SignalWeight=" << _signalWeight << endl;
    cout << "BackgroundWeight=" << _backgroundWeight << endl;
  }

  if (_configs.count("triggerIndex")
      && !_configs.at("triggerIndex").empty()) {
    const auto triggerIndex = _configs.at("triggerIndex").front();
    _triggerIndex = std::stoi(triggerIndex);
  }

  if (_configs.count("filterIndex")
      && !_configs.at("filterIndex").empty()) {
    const auto filterIndex =  _configs.at("filterIndex").front();
    _filterIndex = std::stoi(filterIndex);
  }
  if (_debug) {
    cout << "The trigger index is " << _triggerIndex << endl;
    cout << "The filter index is " << _filterIndex << endl;
  }

  if (_configs.count("NtrkRange")
      && !_configs.at("NtrkRange").empty()) {
    const TString NtrkRange = _configs.at("NtrkRange").front();
    vector<TString> Ntrks;
    if (NtrkRange.Index(",")>0) Ntrks = splitTString(NtrkRange, ",");
    else if (NtrkRange.Index(":")>0) Ntrks = splitTString(NtrkRange, ":");
    const auto NtrkLow = Ntrks.front();
    _NtrkLow = static_cast<UShort_t>(NtrkLow.Atoi());
    const auto NtrkHigh = Ntrks.back();
    _NtrkHigh = static_cast<UShort_t>(NtrkHigh.Atoi());
  }

  if (_configs.count("effTable")) {
    const auto temp = _configs.at("effTable");
    if (!temp.empty()) {
      const TString names = temp.front();
      if (names.Index(".root") > 0) {
        auto v = splitTString(names, ":");
        _effFileName = v.at(0).Data();
        _effGraphName = v.at(1).Data();
        TString effType = v.at(2);
        effType.ToLower();
        if (effType == "tgrapherrors") {
          _effGraphType = "TGraphErrors";
        } else if (effType == "tgraphasymmerrors") {
          _effGraphType = "TGraphAsymmErrors";
        }
        if (_debug) {
          std::cout << "_effFileName is " << _effFileName << endl;
          std::cout << "_effGraphName is " << _effGraphName << endl;
          std::cout << "_effGraphType is " << _effGraphType << endl;
        }
      }
    }
  }

  _saveTree  = std::find(options.begin(), options.end(), "savetree")  != options.end();
  _saveDau   = std::find(options.begin(), options.end(), "savedau")   != options.end();
  _selectMVA = std::find(options.begin(), options.end(), "selectmva") != options.end();
  _useEventWiseWeight = std::find(options.begin(), options.end(), "useeventwiseweight")
    != options.end();
  _isMC      = std::find(options.begin(), options.end(), "ismc")      != options.end();
  _saveMatchedOnly = std::find (options.begin(), options.end(), "!savematchedonly")
    == options.end();
  _flipEta   = std::find(options.begin(), options.end(), "flipeta") != options.end();
  _selectDeDx = std::find(options.begin(), options.end(), "selectdedx") != options.end();
  _wantAbort = std::find(options.begin(), options.end(), "wantabort") != options.end();

  if (_configs.count("signalFileList")
      && _configs.at("signalFileList").size())
    _signalFileList = _configs.at("signalFileList").front();
  if (_configs.count("backgroundFileList")
      && _configs.at("backgroundFileList").size())
    _backgroundFileList = _configs.at("backgroundFileList").front();
  if (_debug) {
    cout << "signalFileList: " << _signalFileList << endl;
    cout << "backgroundFileList: " << _backgroundFileList << endl;
  }

  if (_configs.count("KeptBranchNames")) {
    const auto& branches = _configs.at("KeptBranchNames");
    for (const auto& bs : branches) {
      TString temp = bs;
      auto bnames = splitTString(temp, ":");
      for (const auto& b : bnames) {
        if (b.Length() && !b.IsWhitespace()) {
          _keptBranchNames.push_back(b);
        }
      }
    }
    if (_debug) {
      cout << "Kept branch names are: \n";
      for (const auto& b : _keptBranchNames) {
        cout << "\t" << b << endl;
      }
    }
  }

  if (_configs.count("RooWorkspace")) {
    const auto& ws = _configs.at("RooWorkspace");
    if (!ws.empty()) {
      auto strs = ws.front();
      _workspaceStrs = splitTString(strs, ":");
    }
  }

  if (_configs.count("RooDataSet")) {
    const auto& ds = _configs.at("RooDataSet");
    if (!ds.empty()) {
      auto strs = ds.front();
      _dataSetStrs = splitTString(strs, ":");
    }
  }
}

/**
 * Return empty vector if there is no "methods" node in xml.
 * Otherwise return the vector of attributes of each method
 */
vector<map<string, TString>> tmvaConfigs::getMethods() const
{
  vector<map<string, TString>> output;
  if (!_configs.count("methods")
      || !_configs.at("methods").size()) return output;

  const auto& methods = _configs.at("methods");
  for (const auto& method : methods) {
    const auto temp = getMethod(method);
    output.push_back(temp);
    if (_debug) {
      cout << "In getMethods" << endl;
      for (const auto& kv : output.back()) {
        cout << kv.first << ":\t" << kv.second << endl;
      }
    }
  }
  return output;
}

/**
 * Helper function for getMethods
 */
map<string, TString> tmvaConfigs::getMethod(const string& instr) const
{
  TString in (instr.c_str());
  map<string, TString> output;
  TPRegexp r("(.*)\\? *(.*)\\? *(.*)");
  TObjArray* subStrL = r.MatchS(in);
  output["fullStr"] = dynamic_cast<TObjString*>(subStrL->At(0))->GetString();
  output["method"] = dynamic_cast<TObjString*>(subStrL->At(1))->GetString();
  output["name"] = dynamic_cast<TObjString*>(subStrL->At(2))->GetString();
  output["config"] = dynamic_cast<TObjString*>(subStrL->At(3))->GetString();
  for (auto& e : output) { e.second = e.second.Strip(TString::kBoth); }
  return output;
}

/**
 * Return empty vector if there is no "training_variables_worange" node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getTrainVarsWoRange() const
{
  return getVarsWoRange("training_variables_worange");
}

/**
 * Return empty vector if there is no "spectator_variables_worange" node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getSpecVarsWoRange() const
{
  return getVarsWoRange("spectator_variables_worange");
}

/**
 * Return empty vector if there is no "XXXX_worange" node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getVarsWoRange(const std::string& in) const
{
  vector<map<string, TString>> output;
  if (!_configs.count(in)
      || !_configs.size()) return output;
  const auto& vars = _configs.at(in);
  for (const auto& v : vars) {
    output.push_back(getVarWoRange(v));
    if(_debug){
      cout << "List all variables without ranges inside "  << in << endl;
      for (const auto& kv : output.back()) {
        cout << kv.first << ":\t" << kv.second << endl;
      }
    }
  }
  return output;
}

/**
 * Helper function to get the variables without range
 */
map<string, TString> tmvaConfigs::getVarWoRange(const string& instr) const
{
  map<string, TString> output;
  TString in(instr.c_str());
  TPRegexp r("(.*)\\? *(.*)\\? *(.*)\\? *(.)");
  TObjArray* subStrL = r.MatchS(in);
  output["fullStr"]     = dynamic_cast<TObjString*>(subStrL->At(0))->GetString();
  output["variable"]    = dynamic_cast<TObjString*>(subStrL->At(1))->GetString();
  output["description"] = dynamic_cast<TObjString*>(subStrL->At(2))->GetString();
  output["unit"]        = dynamic_cast<TObjString*>(subStrL->At(3))->GetString();
  output["type"]        = dynamic_cast<TObjString*>(subStrL->At(4))->GetString();
  for (auto& e : output) { e.second = e.second.Strip(TString::kBoth); }
  return output;
}

/**
 * Return empty vector if there is no "training_variables_wrange" node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getTrainVarsWRange() const
{
  return getVarsWRange("training_variables_wrange");
}

/**
 * Return empty vector if there is no "spectator_variables_wrange" node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getSpecVarsWRange() const
{
  return getVarsWRange("spectator_variables_wrange");
}

/**
 * Return empty vector if there is no "XXXX_wrange" node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getVarsWRange(const std::string& in) const
{
  vector<map<string, TString>> output;
  if (!_configs.count(in)
      || !_configs.at(in).size()) return output;
  const auto& vars = _configs.at(in);
  for (const auto& v : vars) {
    output.push_back(getVarWRange(v));
    if(_debug){
      cout << "List all variables with ranges inside "  << in << endl;
      for (const auto& kv : output.back()) {
        cout << kv.first << ":\t" << kv.second << endl;
      }
    }
  }
  return output;
}

/**
 * Helper function to get the variables with range
 */
map<string, TString> tmvaConfigs::getVarWRange(const string& instr) const
{
  map<string, TString> output;
  TString in(instr.c_str());
  TPRegexp r("(.*)\\? *(.*)\\? *(.*)\\? *(.*)\\? *(.*)\\? *(.*)");
  TObjArray* subStrL = r.MatchS(in);
  output["fullStr"    ] = dynamic_cast<TObjString*>(subStrL->At(0))->GetString();
  output["variable"   ] = dynamic_cast<TObjString*>(subStrL->At(1))->GetString();
  output["description"] = dynamic_cast<TObjString*>(subStrL->At(2))->GetString();
  output["unit"       ] = dynamic_cast<TObjString*>(subStrL->At(3))->GetString();
  output["type"       ] = dynamic_cast<TObjString*>(subStrL->At(4))->GetString();

  auto minStr = dynamic_cast<TObjString*>(subStrL->At(5))->GetString();
  auto maxStr = dynamic_cast<TObjString*>(subStrL->At(6))->GetString();

  TPRegexp num("\\s*(-?\\d+\\.?\\d*)\\s*");

  TObjArray* minNum = num.MatchS(minStr);
  if (minNum->GetLast()<0) {
    output["minStr"] = "";
    output["min"   ] = "";
  } else {
    output["minStr"] = dynamic_cast<TObjString*>(minNum->At(0))->GetString();
    output["min"   ] = dynamic_cast<TObjString*>(minNum->At(1))->GetString();
  }

  TObjArray* maxNum = num.MatchS(maxStr);
  if (maxNum->GetLast()<0) {
    output["maxStr"] = "";
    output["max"   ] = "";
  } else {
    output["maxStr"] = dynamic_cast<TObjString*>(maxNum->At(0))->GetString();
    output["max"   ] = dynamic_cast<TObjString*>(maxNum->At(1))->GetString();
  }
  for (auto& e : output) { e.second = e.second.Strip(TString::kBoth); }
  return output;
}

/**
 * Return empty vector if there is no "training_variables" node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getTrainVarExps() const
{
  return getVarExps("training_variables");
}

/**
 * Return empty vector if there is no "spectator_variables" node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getSpecVarExps() const
{
  return getVarExps("spectator_variables");
}

/**
 * Return empty vector if there is no "common_cuts" node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getCutsVarExps() const
{
  return getVarExps("common_cuts");
}

/**
 * Return empty vector if there is no "XXXX" (the string in) node in xml.
 * Otherwise return the vector of attributes of each variable
 */
vector<map<string, TString>> tmvaConfigs::getVarExps(const std::string& in) const
{
  vector<map<string, TString>> output;
  if (!_configs.count(in)
      || !_configs.at(in).size()) return output;
  const auto& vars = _configs.at(in);
  for (const auto& v : vars) {
    output.push_back(getVarExp(v));
    if(_debug){
      cout << "List all variables with expressions inside "  << in << endl;
      for (const auto& kv : output.back()) {
        cout << kv.first << ":\t" << kv.second << endl;
      }
    }
  }
  return output;
}

/**
 * A helper function to extract string formula for MVAHelper
 */
map<string, TString> tmvaConfigs::getVarExp(const std::string& instr) const
{
  map<string, TString> output;
  TString in(instr.c_str());
  TPRegexp r("(.*)\\?(.*)\\?(.*)");
  TObjArray* subStrL = r.MatchS(in);
  output["fullStr"] = dynamic_cast<TObjString*>(subStrL->At(0))->GetString();
  output["vars"] = dynamic_cast<TObjString*>(subStrL->At(1))->GetString();
  output["eqn"] = dynamic_cast<TObjString*>(subStrL->At(2))->GetString();
  output["eqn_vars"] = dynamic_cast<TObjString*>(subStrL->At(3))->GetString();
  for (auto& e : output) { e.second = e.second.Strip(TString::kBoth); }
  return output;
}

/**
 * Get out the tree information which is stored in the first line in the node "treeInfo".
 * If the node does not exist, an empty map will be returned
 */
map<string, TString> tmvaConfigs::getTreeInfo() const
{
  map<string, TString> output;
  if (!_configs.count("treeInfo")
      || !_configs.at("treeInfo").size()) return output;
  const TString& in = _configs.at("treeInfo").front();
  TPRegexp r("(.*)\\?(.*)\\?(.*)");
  TObjArray* subStrL = r.MatchS(in);
  output["fullStr"] = dynamic_cast<TObjString*>(subStrL->At(0))->GetString();
  output["treeDir"] = dynamic_cast<TObjString*>(subStrL->At(1))->GetString();
  output["pdgId"] = dynamic_cast<TObjString*>(subStrL->At(2))->GetString();
  output["nentries"] = dynamic_cast<TObjString*>(subStrL->At(3))->GetString();
  for (auto& e : output) { e.second = e.second.Strip(TString::kBoth); }
  return output;
}

/**
   Get the common cuts on signal and background trees.
   Can also be used as precuts.
 */

vector<string> tmvaConfigs::getCommonCuts() const
{
  vector<string> output;
  if (!_configs.count("common_cuts")
      && !_configs.at("common_cuts").size())
    return output;
  return _configs.at("common_cuts");
}

/**
   Get the minimum of MVA values. Each entry corresponds to one method in "methods" node.
   The candidated will be selected if it passes any one of the cut
 */
vector<string> tmvaConfigs::getMVACutMins() const
{
  vector<string> output;
  if (!_configs.count("mvaCutMin")
      || !_configs.at("mvaCutMin").size())
    return output;
  return _configs.at("mvaCutMin");
}

/**
   Get the trian XML "node".
   Each entry in this node corresponds to one train processes.
 */
vector<string> tmvaConfigs::getTrainXMLs() const
{
  vector<string> output;
  if (!_configs.count("trainXML")
      || !_configs.at("trainXML").size())
    return output;
  return _configs.at("trainXML");
}

/**
   Get configuration
 */
// map<string, vector<string>> tmvaConfigs::getConfigs()
// {
//   return _configs;
// }

/**
   Get configuration
 */
map<string, vector<string>> tmvaConfigs::getConfigs() const
{
  return _configs;
}

/**
   Get the name collection of input files
 */
vector<string> tmvaConfigs::getInputFileNames() const
{
  return _configs.at("inputFiles");
}

/**
   Get the name collection of signal files
 */
vector<string> tmvaConfigs::getSignalFileNames() const
{
  return _configs.at("signalFileList");
}

/**
   Get the name collection of background files
 */
vector<string> tmvaConfigs::getBackgroundFileNames() const
{
  return _configs.at("backgroundFileList");
}

/**
   Get the output file name
 */
string tmvaConfigs::getOutFileName() const
{
  return _outfileName;
}

/**
   Get the file name saving efficiency table
 */
string tmvaConfigs::getEffFileName() const
{
  return _effFileName;
}

/**
   Get the name of efficiency table
 */
string tmvaConfigs::getEffGraphName() const
{
  return _effGraphName;
}

/**
   Get the type name of efficiency table
 */
string tmvaConfigs::getEffGraphType() const
{
  if (_effGraphType.empty())
    throw std::runtime_error("Type of efficiency table is not given");
  return _effGraphType;
}

/**
   Set the output file name
 */
void  tmvaConfigs::setOutFileName(const string& in)
{
  _outfileName = in;
}

/**
   Get the output dir name
 */
string tmvaConfigs::getOutDirName() const
{
  return _outdirName;
}

/**
   Set the output dir name
 */
void  tmvaConfigs::setOutDirName(const string& in)
{
  _outdirName = in;
}

/**
   Get the congifuration string for dataloader
 */
string tmvaConfigs::getDataLoaderConfig() const
{
  string output;
  if (_configs.count("dataloader_config")
      && _configs.at("dataloader_config").size())
    output = _configs.at("dataloader_config").at(0);
    if (!output.size()) throw std::runtime_error("Missing dataloader configuration");
  return output;
}

/**
   Get the congifuration string for factory
 */
string tmvaConfigs::getFactoryConfig() const
{
  string output;
  if (_configs.count("factory_config")
      && _configs.at("factory_config").size())
    output = _configs.at("factory_config").at(0);
  if (!output.size()) throw std::runtime_error("Missing factory configuration");
  return output;
}

/**
   Get the file name of th list of signal inputs
 */
string tmvaConfigs::signalFileList() const
{
  return _signalFileList;
}

/**
   Get the file name of th list of background inputs
 */
string tmvaConfigs::backgroundFileList() const
{
  return _backgroundFileList;
}

/**
   Get the global signal weight
 */
double tmvaConfigs::signalWeight() const
{
  return _signalWeight;
}

/**
   Get the global background weight
 */
double tmvaConfigs::backgroundWeight() const
{
  return _backgroundWeight;
}

/**
   Get the index which trigger users want to use
 */

int tmvaConfigs::triggerIndex() const
{
  //if (_triggerIndex<0)
  //  throw std::runtime_error("trigger index is not properly initialized.");
  return _triggerIndex;
}

/**
   Get the index which filter users want to use
 */

int tmvaConfigs::filterIndex() const
{
  //if (_filterIndex<0)
  //  throw std::runtime_error("filter index is not properly initialized.");
  return _filterIndex;
}

/**
   Return the lower bound of Ntrkoffline (included).
 */
UShort_t tmvaConfigs::NtrkLow() const
{
  return _NtrkLow;
}

/**
   Return the lower bound of Ntrkoffline (excluded).
 */
UShort_t tmvaConfigs::NtrkHigh() const
{
  return _NtrkHigh;
}

/**
   Return if prune the ntuple
 */
bool tmvaConfigs::pruneNTuple() const
{
  return !_keptBranchNames.empty();
}

/**
   Return if the tree with MVA will be saved
 */
bool tmvaConfigs::saveTree() const
{
  return _saveTree;
}

/**
   Return if the information besides candidate four-momentum will be saved
 */
bool tmvaConfigs::saveDau() const
{
  return _saveTree;
}

/**
   Return if apply MVA selections.
   Should be used together with getMVACutMin()
 */
bool tmvaConfigs::selectMVA() const
{
  return _selectMVA;
}

/**
   Return if event-wise weights will be applied
   If true, a branch "weight" is needed.
 */
bool tmvaConfigs::useEventWiseWeight() const
{
  return _useEventWiseWeight;
}

/**
   Return if the input tree is from MC samples
 */
bool tmvaConfigs::isMC() const
{
  return _isMC;
}

/**
   Return if only the matched candidates will be saved
 */
bool tmvaConfigs::saveMatchedOnly() const
{
  return _saveMatchedOnly;
}

/**
   Return if flipping eta of candidate
 */
bool tmvaConfigs::flipEta() const
{
  return _flipEta;
}

/**
   Return if the dE/dx selection will be applied
 */
bool tmvaConfigs::selectDeDx() const
{
  return _selectDeDx;
}

/**
   Return if we want to abort when detecting kError
 */
bool tmvaConfigs::wantAbort() const
{
  return _wantAbort;
}

/**
   A helper function to split "options" node in XML file
 */
vector<TString> tmvaConfigs::getOptions() const
{
  vector<TString> output;
  if (_configs.count("options")
      && _configs.at("options").size()) {
    output = splitTString(_configs.at("options").front(), ":");
  }
  if (_debug) {
    cout << "Options are:" << endl;
    for (const auto& e : output) {
      cout << "\t" << e << endl;
    }
  }
  return output;
}

/**
   Return the collection of kept branch names
 */
vector<TString> tmvaConfigs::getKeptBranchNames() const
{
  return _keptBranchNames;
}

/**
   Return the name + title of RooWorkspace
 */
vector<TString> tmvaConfigs::getWorkspaceStrs() const
{
  return _workspaceStrs;
}

/**
   Return the name + title of RooDataSet
 */
vector<TString> tmvaConfigs::getDataSetStrs() const
{
  return _dataSetStrs;
}

/**
   Get the binning setup for each methods
 */
vector<vector<TString>> tmvaConfigs::getHistoBinning() const
{
  vector<vector<TString>> output;
  if (_configs.count("histoBinning")
      && _configs.at("histoBinning").size()) {
    const auto& bins = _configs.at("histoBinning");
    for (const auto& bin : bins) {
      const TString binning = bin;
      const auto foundQuote = binning.Index(":") > 0;
      const auto foundComma = binning.Index(",") > 0;
      if ( (foundQuote && foundComma) || (!foundQuote && !foundComma)) {
        cerr << "Cannot find delimiter ':' or ',',"
          " need one and only one type of delimiter!" << endl;
        throw std::runtime_error("No delimiter found for histogram binning");
      }
      if (foundQuote) output.push_back(splitTString(binning, ":"));
      if (foundComma) output.push_back(splitTString(binning, ","));
    }
    if (_debug) {
      cout << "Histograms binnings are:" << endl;
      size_t im=0;
      for (const auto& m : output){
        cout << "Method " << im++ << ":\t";
        for (const auto& binning : m) {
          cout << binning << "\t";
        }
        cout << endl;
      }
    }
  } else {
    throw std::runtime_error("cannot find histoBinning");
  }

  return output;
}

/**
   A helper class to convert tree content to values which are fed to TMVA reader
 */
MVAHelper::MVAHelper(const vector<map<string, TString>> trainingVars,
                     const vector<map<string, TString>> spectatorVars,
                     const vector<map<string, TString>> cutVars)
{
  fvar.fill(nullptr);
  fspec.fill(nullptr);
  fcuts.fill(nullptr);
  vars.fill(0);
  specs.fill(0);
  cuts.fill(0);
  nVars = trainingVars.size();
  nSpecs = spectatorVars.size();
  nCuts = cutVars.size();

  NVAR = std::max(trainingVars.size(), spectatorVars.size());
  NVAR = std::max(NVAR, cuts.size());

  if (NVAR > NMAX) cerr << "# of variables is larger than the maximum # allowed." << endl;
  for (size_t i=0; i<trainingVars.size(); i++) {
    fvar[i] = new TF1(Form("fvar%zu", i), trainingVars.at(i).at("eqn").Data());
  }
  for (size_t i=0; i<spectatorVars.size(); i++) {
    fspec[i] = new TF1(Form("fspec%zu", i), spectatorVars.at(i).at("eqn").Data());
  }
  for (size_t i=0; i<cutVars.size(); i++) {
    fcuts[i] = new TF1(Form("fcuts%zu", i), cutVars.at(i).at("eqn").Data());
  }
}
MVAHelper::~MVAHelper()
{
  std::cout << "Delete MVAHelper" << std::endl;
  for (auto& f : fvar) delete f;
  for (auto& f : fspec) delete f;
  for (auto& f : fcuts) delete f;
}

void MVAHelper::GetValues(MyNTuple& t,
                          const vector<vector<TString>>& trainingVars,
                          const vector<vector<TString>>& spectatorVars,
                          const vector<vector<TString>>& cutVars)
{
  for (size_t i=0; i<trainingVars.size(); i++) {
    if (trainingVars[i].size() != fvar[i]->GetNpar()) {
      cerr << "[ERROR] training variable size is different from parameter set size" << endl;
      cerr << fvar[i]->GetNpar() << endl;
      cerr << trainingVars[i].size() << endl;
    }

    for (size_t j=0; j<fvar[i]->GetNpar(); j++) {
      fvar[i]->SetParameter(j, t.value(trainingVars.at(i).at(j)));
    }
    vars[i] = fvar[i]->Eval(0);
  }
  for (size_t i=0; i<spectatorVars.size(); i++) {
    if (spectatorVars[i].size() != fspec[i]->GetNpar())
      cerr << "[ERROR] spectator variable size is different from parameter set size" << endl;
    for (size_t j=0; j<fspec[i]->GetNpar(); j++) {
      fspec[i]->SetParameter(j, t.value(spectatorVars.at(i).at(j)));
    }
    specs[i] = fspec[i]->Eval(0);
  }
  for (size_t i=0; i<cutVars.size(); i++) {
    if (cutVars[i].size() != fcuts[i]->GetNpar())
      cerr << "[ERROR] cut variable size is different from parameter set size" << endl;
    for (size_t j=0; j<fcuts[i]->GetNpar(); j++) {
      fcuts[i]->SetParameter(j, t.value(cutVars.at(i).at(j)));
    }
    cuts[i] = fcuts[i]->Eval(0);
  }
}

/**
   Add files to the TChain.
   File list need to be ended with ".list"

 */

void addFilesToChain(TChain* t, const vector<string>& fs)
{
  for (const auto& f : fs) {
    if (f.size() <=5 ) {
      cerr << "[EROOR] Cannot find correct file extension."
           << " Please checke the file name!" << endl;
      continue;
    }
    const auto firstPos = 0;
    if ( f.find(".list", firstPos) != std::string::npos ) {
      TFileCollection tf("tf", "", f.c_str());
      t->AddFileInfoList(tf.GetList());
    }
    else if ( f.find(".root", firstPos) != std::string::npos ) {
      t->Add(f.c_str());
    }
  }
  return;
}

vector<TString> splitTString(const TString& in, const char* delimiter)
{
  vector<TString> ss;

  int strStart = 0;
  int strEnd = in.Index(delimiter, 0);
  while (strEnd >= 0) {
    TString temp = in(strStart, strEnd-strStart);
    ss.push_back(temp.Strip(TString::kBoth));
    strStart = strEnd+1;
    strEnd = in.Index(delimiter, strStart);
  }
  TString out = in(strStart, in.Length()-strStart);
  out = out.Strip(TString::kBoth);
  ss.push_back(out);

  return ss;
}

varHists::varHists(const tmvaConfigs& config)
{
  if (config._configs.count("varHists")) {
    const auto& var_hists = config._configs.at("varHists");
    for (const auto& pars : var_hists) {
      // cout << pars << endl;
      const TString parList = pars;
      const auto foundQuote = parList.Index(":") > 0;
      const auto foundComma = parList.Index(",") > 0;
      if ( (foundQuote && foundComma) || (!foundQuote && !foundComma)) {
        cerr << "Cannot find delimiter ':' or ',',"
          " need one and only one type of delimiter!" << endl;
        throw std::runtime_error("No delimiter found for histogram binning");
      }
      vector<TString> parameters;
      if (foundQuote) parameters = splitTString(parList, ":");
      else if (foundComma) parameters = splitTString(parList, ",");

      // TH3
      if (parList.Index("TH3:", 0)>=0 || parList.Index("TH3,", 0)>=0) {
        // TH3([1], [2], [3], [4], [5], [6], [7], [8], [9], [10], [11])
        hist3Ds.push_back
          (std::make_unique<TH3D>
           (parameters.at(1).Data(), parameters.at(2).Data(),
            parameters.at(3).Atoi(), parameters.at(4).Atof(), parameters.at(5).Atof(),
            parameters.at(6).Atoi(), parameters.at(7).Atof(), parameters.at(8).Atof(),
            parameters.at(9).Atoi(), parameters.at(10).Atof(), parameters.at(11).Atof()
            ));
        var3Ds.push_back(parameters);
      }
      if (parList.Index("TH2:")>=0 || parList.Index("TH2,")>=0) {
        // TH2([1], [2], [3], [4], [5], [6], [7], [8])
        hist2Ds.push_back
          (std::make_unique<TH2D>
           (parameters.at(1).Data(), parameters.at(2).Data(),
            parameters.at(3).Atoi(), parameters.at(4).Atof(), parameters.at(5).Atof(),
            parameters.at(6).Atoi(), parameters.at(7).Atof(), parameters.at(8).Atof()
            ));
        var2Ds.push_back(parameters);
      }
      if (parList.Index("TH1:")>=0 || parList.Index("TH1,")>=0) {
        // TH2([1], [2], [3], [4], [5])
        hist1Ds.push_back
          (std::make_unique<TH1D>
           (parameters.at(1).Data(), parameters.at(2).Data(),
            parameters.at(3).Atoi(), parameters.at(4).Atof(), parameters.at(5).Atof()
            ));
        var1Ds.push_back(parameters);
      }
    }
  }
}

void varHists::fillHists(MyNTuple& t, const double weight)
{
  for (size_t i=0; i != hist3Ds.size(); ++i) {
    auto& h3D = hist3Ds.at(i);
    // std::cout << var3Ds.at(i).at(12).Length() << var3Ds.at(i).at(12) << std::endl;
    const double x = t.value(var3Ds.at(i).at(12));
    const double y = t.value(var3Ds.at(i).at(13));
    const double z = t.value(var3Ds.at(i).at(14));
    h3D->Fill(x, y, z, weight);
  }
  for (size_t i=0; i != hist2Ds.size(); ++i) {
    auto& h2D = hist2Ds.at(i);
    const double x = t.value(var2Ds.at(i).at(9));
    const double y = t.value(var2Ds.at(i).at(10));
    h2D->Fill(x, y, weight);
  }
  for (size_t i=0; i != hist1Ds.size(); ++i) {
    auto& h1D = hist1Ds.at(i);
    const double x= t.value(var1Ds.at(i).at(6));
    h1D->Fill(x, weight);
  }
}

void varHists::writeHists()
{
  for (auto& h : hist1Ds) h->Write();
  for (auto& h : hist2Ds) h->Write();
  for (auto& h : hist3Ds) h->Write();
}

#endif
