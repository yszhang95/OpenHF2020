#ifndef __MyFunctions__
#define __MyFunctions__

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <array>
#include <string>
#include <memory>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <sstream>

#include "TString.h"
#include "TF1.h"

#include "TMVA/Types.h"

#include "Ana/TreeHelpers.h"

static int DEBUG = 0;

// trim functions copied from https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
}
class tmvaConfigs
{
public:
  tmvaConfigs(std::string, bool debug=false);
  tmvaConfigs(const tmvaConfigs&) = delete;
  ~tmvaConfigs() {};
  std::vector<std::map<std::string, TString>> getMethods() const;
  std::vector<std::map<std::string, TString>> getTrainVarsWoRange() const;
  std::vector<std::map<std::string, TString>> getSpecVarsWoRange() const;
  std::vector<std::map<std::string, TString>> getTrainVarsWRange() const;
  std::vector<std::map<std::string, TString>> getSpecVarsWRange() const;
  std::vector<std::map<std::string, TString>> getTrainVarExps() const;
  std::vector<std::map<std::string, TString>> getSpecVarExps() const;
  std::vector<std::map<std::string, TString>> getCutsVarExps() const;
  std::map<std::string, TString>              getTreeInfo() const;
  std::vector<std::string>                    getCommonCuts() const;
  std::vector<std::string>                    getMVACutMins() const;
  std::vector<std::string>                    getTrainXMLs() const;
  //  std::map<std::string, std::vector<std::string>> getConfigs();
  std::map<std::string, std::vector<std::string>> getConfigs() const;
  std::vector<TString>                        getOptions() const;
  std::string                                 getOutFileName() const;
  void                                        setOutFileName(const std::string&);
  std::string                                 getOutDirName() const;
  void                                        setOutDirName(const std::string&);
  std::string                                 getDataLoaderConfig() const;
  std::string                                 getFactoryConfig() const;
  std::string                                 signalFileList() const;
  std::string                                 backgroundFileList() const;
  double                                      signalWeight() const;
  double                                      backgroundWeight() const;
  bool                                        saveTree()  const;
  bool                                        saveDau()   const;
  bool                                        selectMVA() const;
  bool                                        useEventWiseWeight() const;
  bool                                        isMC() const;
  bool                                        saveMatchedOnly() const;
  bool                                        flipEta() const;

private:
  std::map<std::string, TString>              getMethod(const std::string&) const;
  std::vector<std::map<std::string, TString>> getVarsWoRange(const std::string&) const;
  std::map<std::string, TString>              getVarWoRange(const std::string&) const;
  std::vector<std::map<std::string, TString>> getVarsWRange(const std::string&) const;
  std::map<std::string, TString>              getVarWRange(const std::string&) const;
  std::vector<std::map<std::string, TString>> getVarExps(const std::string&) const;
  std::map<std::string, TString>              getVarExp(const std::string&) const;
  std::map<std::string, std::vector<std::string>> _configs;
  std::string _outfileName;
  std::string _outdirName;
  std::string _signalFileList;
  std::string _backgroundFileList;
  double _signalWeight;
  double _backgroundWeight;
  bool _saveTree;
  bool _saveDau;
  bool _selectMVA;
  bool _useEventWiseWeight;
  bool _isMC;
  bool _saveMatchedOnly;
  bool _flipEta;
  bool _debug;
};

std::map<std::string, TMVA::Types::EMVA> setupMethodCollection();

struct MVAHelper
{
  const static size_t NMAX = 100;
  size_t NVAR;
  size_t nVars;
  size_t nSpecs;
  size_t nCuts;
  std::array<TF1*, NMAX> fvar;
  std::array<TF1*, NMAX> fspec;
  std::array<TF1*, NMAX> fcuts;
  std::array<float, NMAX> vars;
  std::array<float, NMAX> specs;
  std::array<float, NMAX> cuts;
  explicit MVAHelper(std::vector<std::map<std::string, TString>>,
                     std::vector<std::map<std::string, TString>>,
                     std::vector<std::map<std::string, TString>>);
  MVAHelper(const MVAHelper&) = delete;
  ~MVAHelper();
  void GetValues( MyNTuple&, const std::vector<std::vector<TString>>&, const std::vector<std::vector<TString>>&, const std::vector<std::vector<TString>>&);
};

std::vector<TString> splitTString(const TString& in, const char* delimiter);
#endif
