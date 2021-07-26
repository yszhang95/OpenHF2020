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

std::map<std::string, std::vector<std::string>> readConfigs(std::string);

std::map<std::string, TMVA::Types::EMVA> setupMethodCollection();

std::map<std::string, TString> getTrainPars(const TString&, const std::string&);

std::map<std::string, TString> getAppPars(const TString&, const std::string&);

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
