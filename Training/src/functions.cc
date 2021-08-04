#include "functions.h"

#ifdef __MyFunctions__

#include <algorithm>

#include "TClonesArray.h"
#include "TObjString.h"
#include "TPRegexp.h"
#include "TObjString.h"

#include "TXMLEngine.h"

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
  _saveTree(0), _saveDau(0), _selectMVA(0), _useEventWiseWeight(0),
  _debug(debug)
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

  const auto options = getOptions();
  auto it_SW = std::find_if(options.begin(), options.end(),
                            [](const TString& str)
                            { return str.Contains("SiganlWeight=");});
  auto it_BW = std::find_if(options.begin(), options.end(),
                            [](const TString& str)
                            { return str.Contains("BackgroundWeight=");});
  if (it_SW != options.end())
    _signalWeight = TString((*it_SW)(12, it_SW->Length()-1)).Atof();
  if (it_BW != options.end())
    _backgroundWeight = TString((*it_BW)(16, it_BW->Length()-1)).Atof();
  if (_debug) {
    cout << "SignalWeight=" << _signalWeight << endl;
    cout << "BackgroundWeight=" << _backgroundWeight << endl;
  }

  _saveTree  = std::find(options.begin(), options.end(), "saveTree")  != options.end();
  _saveDau   = std::find(options.begin(), options.end(), "saveDau")   != options.end();
  _selectMVA = std::find(options.begin(), options.end(), "selectMVA") != options.end();
  _useEventWiseWeight = std::find(options.begin(), options.end(), "useEventWiseWeight")
    != options.end();

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
  return getVarExps("commont_cuts");
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
  TPRegexp r("(.*)\\?(.*)\\?(.*)\\?(.*)");
  TObjArray* subStrL = r.MatchS(in);
  output["fullStr"] = dynamic_cast<TObjString*>(subStrL->At(0))->GetString();
  output["treeList"] = dynamic_cast<TObjString*>(subStrL->At(1))->GetString();
  output["treeDir"] = dynamic_cast<TObjString*>(subStrL->At(2))->GetString();
  output["pdgId"] = dynamic_cast<TObjString*>(subStrL->At(3))->GetString();
  output["nentries"] = dynamic_cast<TObjString*>(subStrL->At(4))->GetString();
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
   Get the output file name
 */
string tmvaConfigs::getOutFileName() const
{
  return _outfileName;
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

vector<TString> splitTString(const TString& in, const char* delimiter)
{
  vector<TString> ss;

  int strStart = 0;
  int strEnd = in.Index(delimiter, 0);
  while (strEnd >= 0) {
    ss.push_back(in(strStart, strEnd-strStart));
    strStart = strEnd+1;
    strEnd = in.Index(delimiter, strStart);
  }
  ss.push_back(in(strStart, in.Length()-strStart));

  return ss;
}
#endif
